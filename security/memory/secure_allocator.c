#include "secure_allocator.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

#define ARIX_SECURE_PAGE_SIZE 4096
#define ARIX_SECURE_CANARY_SIZE 8
#define ARIX_SECURE_MAX_RECORDS 1024
#define ARIX_SECURE_OVERHEAD_SIZE 16
#define ARIX_SECURE_GUARD_MAGIC 0x414E58
#define ARIX_SECURE_QUARANTINE_SIZE 64
#define ARIX_SECURE_FREED_SET_SIZE 1024
#define ARIX_SECURE_FREELIST_CANARY ((uint64_t)0xDEADFEEE)

#define ARIX_SMALL_OBJECT_MAX 64
#define ARIX_SMALL_BINS 7
#define ARIX_SMALL_BIN_CAPACITY 256
#define ARIX_SMALL_BITMAP_WORDS 8

#define ARIX_SCRUB_PATTERN_ZERO 0
#define ARIX_SCRUB_PATTERN_ONE 1
#define ARIX_SCRUB_PATTERN_RANDOM 2

static int g_scrub_pattern = ARIX_SCRUB_PATTERN_ZERO;
static int g_quarantine_max = ARIX_SECURE_QUARANTINE_SIZE;

typedef struct {
    size_t actual_size;
    uint32_t magic;
    uint32_t reserved;
} ArixSecureAllocHeader;

typedef struct ArixSecureFreeNode {
    void* base;
    size_t total;
    uint64_t canary;
    struct ArixSecureFreeNode* next;
    struct ArixSecureFreeNode* prev;
} ArixSecureFreeNode;

typedef struct {
    void* base;
    size_t total_with_guards;
} ArixSecureQuarantineEntry;

typedef struct {
    void* blocks[ARIX_SMALL_BIN_CAPACITY];
    uint32_t bitmap[ARIX_SMALL_BITMAP_WORDS];
    int count;
    int bin_size;
} ArixSmallObjectBin;

static ArixSecureAllocRecord g_records[ARIX_SECURE_MAX_RECORDS];
static int g_record_count = 0;
static int g_initialized = 0;
static ArixSecureFreeNode* g_freelist_head = NULL;
static void* g_freed_set[ARIX_SECURE_FREED_SET_SIZE];
static ArixSecureQuarantineEntry g_quarantine[ARIX_SECURE_QUARANTINE_SIZE];
static int g_quarantine_count = 0;
static size_t g_stats_num_frees = 0;
static size_t g_stats_num_double_free = 0;
static size_t g_stats_num_canary_violations = 0;

static ArixSmallObjectBin g_small_bins[ARIX_SMALL_BINS];
static int g_small_bins_initialized = 0;

static int g_small_sizes[ARIX_SMALL_BINS] = {8, 16, 24, 32, 40, 48, 64};

static void secure_zero(void* ptr, size_t len) {
    volatile unsigned char* p = (volatile unsigned char*)ptr;
    for (size_t i = 0; i < len; i++) p[i] = 0;
}

static void secure_scrub(void* ptr, size_t len) {
    volatile unsigned char* p = (volatile unsigned char*)ptr;
    if (g_scrub_pattern == ARIX_SCRUB_PATTERN_ZERO) {
        for (size_t i = 0; i < len; i++) p[i] = 0;
    } else if (g_scrub_pattern == ARIX_SCRUB_PATTERN_ONE) {
        for (size_t i = 0; i < len; i++) p[i] = 0xFF;
    } else {
        for (size_t i = 0; i < len; i++) p[i] = (unsigned char)(rand() & 0xFF);
    }
}

static size_t round_up_page(size_t bytes) {
    return (bytes + ARIX_SECURE_PAGE_SIZE - 1) & ~(ARIX_SECURE_PAGE_SIZE - 1);
}

static uint32_t freed_set_hash(void* ptr) {
    return (uint32_t)(((uintptr_t)ptr >> 4) % ARIX_SECURE_FREED_SET_SIZE);
}

static int freed_set_check(void* ptr) {
    uint32_t idx = freed_set_hash(ptr);
    for (size_t i = 0; i < ARIX_SECURE_FREED_SET_SIZE; i++) {
        size_t j = (idx + (uint32_t)i) % ARIX_SECURE_FREED_SET_SIZE;
        if (g_freed_set[j] == ptr) return 1;
        if (g_freed_set[j] == NULL) return 0;
    }
    return 0;
}

static void freed_set_add(void* ptr) {
    uint32_t idx = freed_set_hash(ptr);
    for (size_t i = 0; i < ARIX_SECURE_FREED_SET_SIZE; i++) {
        size_t j = (idx + (uint32_t)i) % ARIX_SECURE_FREED_SET_SIZE;
        if (g_freed_set[j] == NULL || g_freed_set[j] == ptr) {
            g_freed_set[j] = ptr;
            return;
        }
    }
}

static void freed_set_remove(void* ptr) {
    uint32_t idx = freed_set_hash(ptr);
    for (size_t i = 0; i < ARIX_SECURE_FREED_SET_SIZE; i++) {
        size_t j = (idx + (uint32_t)i) % ARIX_SECURE_FREED_SET_SIZE;
        if (g_freed_set[j] == ptr) {
            g_freed_set[j] = NULL;
            return;
        }
        if (g_freed_set[j] == NULL) return;
    }
}

static void freed_set_clear(void) {
    memset(g_freed_set, 0, sizeof(g_freed_set));
}

static int add_record(void* addr, size_t size, size_t guard_f, size_t guard_b, uint64_t canary) {
    if (g_record_count >= ARIX_SECURE_MAX_RECORDS) return -1;
    ArixSecureAllocRecord* r = &g_records[g_record_count++];
    r->addr = addr; r->size = size; r->guard_front = guard_f;
    r->guard_back = guard_b; r->canary = canary; r->is_freed = 0;
    return 0;
}

static ArixSecureFreeNode* freelist_node_alloc(void* base, size_t total) {
    ArixSecureFreeNode* node = (ArixSecureFreeNode*)malloc(sizeof(ArixSecureFreeNode));
    if (!node) return NULL;
    node->base = base;
    node->total = total;
    node->canary = ARIX_SECURE_FREELIST_CANARY;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

static void freelist_insert(ArixSecureFreeNode* node) {
    if (!g_freelist_head) {
        g_freelist_head = node;
        return;
    }
    ArixSecureFreeNode* cur = g_freelist_head;
    ArixSecureFreeNode* prv = NULL;
    while (cur && cur->base < node->base) {
        prv = cur;
        cur = cur->next;
    }
    if (prv) {
        char* prv_end = (char*)prv->base + prv->total;
        if (prv_end == node->base) {
            prv->total += node->total;
            prv->canary = ARIX_SECURE_FREELIST_CANARY;
            prv->next = node->next;
            if (node->next) node->next->prev = prv;
            free(node);
            node = prv;
        }
    }
    if (cur) {
        char* node_end = (char*)node->base + node->total;
        if (node_end == cur->base) {
            node->total += cur->total;
            node->canary = ARIX_SECURE_FREELIST_CANARY;
            node->next = cur->next;
            if (cur->next) cur->next->prev = node;
            if (g_freelist_head == cur) g_freelist_head = node;
            free(cur);
        }
    }
    if (node != prv && node->prev != prv) {
        node->next = cur;
        node->prev = prv;
        if (prv) prv->next = node;
        else g_freelist_head = node;
        if (cur) cur->prev = node;
    }
}

static void freelist_remove(ArixSecureFreeNode* node) {
    if (!node) return;
    if (node->prev) node->prev->next = node->next;
    else g_freelist_head = node->next;
    if (node->next) node->next->prev = node->prev;
    free(node);
}

static ArixSecureFreeNode* freelist_find_by_base(void* base) {
    ArixSecureFreeNode* cur = g_freelist_head;
    while (cur) {
        if (cur->base == base) return cur;
        if (cur->base > base) return NULL;
        cur = cur->next;
    }
    return NULL;
}

static void freelist_destroy(void) {
    ArixSecureFreeNode* cur = g_freelist_head;
    while (cur) {
        ArixSecureFreeNode* next = cur->next;
        free(cur);
        cur = next;
    }
    g_freelist_head = NULL;
}

static int find_small_bin(size_t size) {
    for (int i = 0; i < ARIX_SMALL_BINS; i++) {
        if (size <= (size_t)g_small_sizes[i]) return i;
    }
    return -1;
}

static void small_bin_mark_used(int bin_idx, int slot) {
    if (bin_idx < 0 || bin_idx >= ARIX_SMALL_BINS) return;
    int word = slot / 32;
    int bit = slot % 32;
    g_small_bins[bin_idx].bitmap[word] |= (1u << bit);
}

static void small_bin_mark_free(int bin_idx, int slot) {
    if (bin_idx < 0 || bin_idx >= ARIX_SMALL_BINS) return;
    int word = slot / 32;
    int bit = slot % 32;
    g_small_bins[bin_idx].bitmap[word] &= ~(1u << bit);
}

static int small_bin_find_free(int bin_idx) {
    if (bin_idx < 0 || bin_idx >= ARIX_SMALL_BINS) return -1;
    for (int w = 0; w < ARIX_SMALL_BITMAP_WORDS; w++) {
        uint32_t bits = g_small_bins[bin_idx].bitmap[w];
        if (bits != 0xFFFFFFFF) {
            for (int b = 0; b < 32; b++) {
                if (!(bits & (1u << b))) {
                    return w * 32 + b;
                }
            }
        }
    }
    return -1;
}

static int init_small_bins(void) {
    if (g_small_bins_initialized) return 0;
    for (int i = 0; i < ARIX_SMALL_BINS; i++) {
        memset(&g_small_bins[i], 0, sizeof(ArixSmallObjectBin));
        g_small_bins[i].bin_size = g_small_sizes[i];
        g_small_bins[i].count = 0;
    }
    g_small_bins_initialized = 1;
    return 0;
}

int arix_secure_allocator_init(ArixSecureAllocator* alloc) {
    if (!alloc) return -1;
    memset(alloc, 0, sizeof(*alloc));
    alloc->use_guard_pages = 1;
    alloc->use_canaries = 1;
    g_initialized = 1;
    g_record_count = 0;
    g_quarantine_count = 0;
    g_stats_num_frees = 0;
    g_stats_num_double_free = 0;
    g_stats_num_canary_violations = 0;
    g_scrub_pattern = ARIX_SCRUB_PATTERN_ZERO;
    g_quarantine_max = ARIX_SECURE_QUARANTINE_SIZE;
    freed_set_clear();
    freelist_destroy();
    init_small_bins();
    return 0;
}

void arix_secure_allocator_destroy(ArixSecureAllocator* alloc) {
    if (!alloc) return;
    for (int i = 0; i < g_record_count; i++) {
        if (!g_records[i].is_freed && g_records[i].addr) {
            arix_secure_free(alloc, g_records[i].addr);
        }
    }
    memset(g_records, 0, sizeof(g_records));
    g_record_count = 0;
    memset(alloc, 0, sizeof(*alloc));
    freed_set_clear();
    freelist_destroy();
    g_quarantine_count = 0;
    g_stats_num_frees = 0;
    g_stats_num_double_free = 0;
    g_stats_num_canary_violations = 0;
    memset(g_small_bins, 0, sizeof(g_small_bins));
    g_small_bins_initialized = 0;
}

void* arix_secure_alloc(ArixSecureAllocator* alloc, size_t bytes, size_t alignment) {
    (void)alignment;
    if (!alloc || bytes == 0) return NULL;

    if (bytes <= ARIX_SMALL_OBJECT_MAX) {
        int bin_idx = find_small_bin(bytes);
        if (bin_idx >= 0) {
            int slot = small_bin_find_free(bin_idx);
            if (slot >= 0) {
                small_bin_mark_used(bin_idx, slot);
                void* ptr = g_small_bins[bin_idx].blocks[slot];
                if (ptr) {
                    g_small_bins[bin_idx].count++;
                    add_record(ptr, g_small_bins[bin_idx].bin_size, 0, 0, 0);
                    alloc->total_allocated += g_small_bins[bin_idx].bin_size;
                    alloc->num_allocations++;
                    if (alloc->total_allocated > alloc->peak_allocated)
                        alloc->peak_allocated = alloc->total_allocated;
                    return ptr;
                }
            }
        }
    }

    size_t alloc_bytes = bytes + ARIX_SECURE_OVERHEAD_SIZE;
    size_t total = round_up_page(alloc_bytes + ARIX_SECURE_CANARY_SIZE);
    size_t total_with_guards = total + ARIX_SECURE_PAGE_SIZE * 2;

    void* base = NULL;
#ifdef _WIN32
    base = VirtualAlloc(NULL, total_with_guards, MEM_RESERVE, PAGE_NOACCESS);
    if (!base) return NULL;
    void* user_region = (char*)base + ARIX_SECURE_PAGE_SIZE;
    void* user_committed = VirtualAlloc(user_region, total, MEM_COMMIT, PAGE_READWRITE);
    if (!user_committed) { VirtualFree(base, 0, MEM_RELEASE); return NULL; }
#else
    base = mmap(NULL, total_with_guards, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (base == MAP_FAILED) return NULL;
    void* user_region = (char*)base + ARIX_SECURE_PAGE_SIZE;
    if (mprotect(user_region, total, PROT_READ | PROT_WRITE) != 0) {
        munmap(base, total_with_guards); return NULL;
    }
#endif

    ArixSecureAllocHeader* hdr = (ArixSecureAllocHeader*)user_region;
    hdr->magic = ARIX_SECURE_GUARD_MAGIC;
    hdr->actual_size = bytes;
    hdr->reserved = 0;

    void* ptr = (char*)user_region + ARIX_SECURE_OVERHEAD_SIZE;

    uint64_t canary_val = arix_secure_canary_generate();
    uint64_t* canary_ptr = (uint64_t*)((char*)ptr + total - ARIX_SECURE_OVERHEAD_SIZE - ARIX_SECURE_CANARY_SIZE);
    *canary_ptr = canary_val;

    if (alloc->use_guard_pages) {
        alloc->total_allocated += bytes;
        alloc->num_allocations++;
        if (alloc->total_allocated > alloc->peak_allocated)
            alloc->peak_allocated = alloc->total_allocated;
    }

    add_record(ptr, bytes, ARIX_SECURE_PAGE_SIZE, ARIX_SECURE_PAGE_SIZE, canary_val);
    return ptr;
}

void arix_secure_free(ArixSecureAllocator* alloc, void* ptr) {
    if (!alloc || !ptr) return;

    for (int i = 0; i < g_small_bins_initialized && i < ARIX_SMALL_BINS; i++) {
        for (int j = 0; j < ARIX_SMALL_BIN_CAPACITY; j++) {
            if (g_small_bins[i].blocks[j] == ptr) {
                small_bin_mark_free(i, j);
                g_small_bins[i].count--;
                alloc->total_allocated -= (alloc->total_allocated >= (size_t)g_small_bins[i].bin_size) ? (size_t)g_small_bins[i].bin_size : alloc->total_allocated;
                g_stats_num_frees++;
                return;
            }
        }
    }

    if (freed_set_check(ptr)) {
        g_stats_num_double_free++;
        if (alloc->on_overflow) {
            for (int i = 0; i < g_record_count; i++) {
                if (g_records[i].addr == ptr) {
                    alloc->on_overflow(&g_records[i]);
                    break;
                }
            }
        }
        return;
    }

    for (int i = 0; i < g_record_count; i++) {
        if (g_records[i].addr == ptr && !g_records[i].is_freed) {
            ArixSecureAllocRecord* r = &g_records[i];
            size_t alloc_bytes = r->size + ARIX_SECURE_OVERHEAD_SIZE;
            size_t total = round_up_page(alloc_bytes + ARIX_SECURE_CANARY_SIZE);
            size_t total_with_guards = total + r->guard_front + r->guard_back;
            void* base = (char*)ptr - r->guard_front - ARIX_SECURE_OVERHEAD_SIZE;
            void* user_region = (char*)base + r->guard_front;

            ArixSecureAllocHeader* hdr = (ArixSecureAllocHeader*)user_region;
            if (hdr->magic != ARIX_SECURE_GUARD_MAGIC) {
                g_stats_num_canary_violations++;
                if (alloc->on_overflow) alloc->on_overflow(r);
            }

            uint64_t* canary_ptr = (uint64_t*)((char*)ptr + total - ARIX_SECURE_OVERHEAD_SIZE - ARIX_SECURE_CANARY_SIZE);
            if (*canary_ptr != r->canary) {
                g_stats_num_canary_violations++;
                if (alloc->on_overflow) alloc->on_overflow(r);
            }

            secure_scrub(ptr, r->size);
            r->is_freed = 1;
            alloc->total_allocated -= (alloc->total_allocated >= r->size) ? r->size : alloc->total_allocated;
            g_stats_num_frees++;
            freed_set_add(ptr);

            ArixSecureFreeNode* node = freelist_node_alloc(base, total_with_guards);
            if (node) freelist_insert(node);

#ifdef _WIN32
            VirtualFree(base, 0, MEM_RELEASE);
#else
            munmap(base, total_with_guards);
#endif
            return;
        }
    }
}

void arix_secure_audit(ArixSecureAllocator* alloc) {
    if (!alloc) return;
    printf("=== ARIX Secure Allocator Audit ===\n");
    printf("Total allocated: %zu bytes\n", alloc->total_allocated);
    printf("Peak allocated: %zu bytes\n", alloc->peak_allocated);
    printf("Active allocations: %zu\n", alloc->num_allocations);
    int violations = 0;
    for (int i = 0; i < g_record_count; i++) {
        if (g_records[i].is_freed) continue;
        size_t alloc_bytes = g_records[i].size + ARIX_SECURE_OVERHEAD_SIZE;
        size_t total = round_up_page(alloc_bytes + ARIX_SECURE_CANARY_SIZE);
        uint64_t* canary_ptr = (uint64_t*)((char*)g_records[i].addr + total - ARIX_SECURE_OVERHEAD_SIZE - ARIX_SECURE_CANARY_SIZE);
        if (*canary_ptr != g_records[i].canary) {
            printf("VIOLATION: canary corrupted at %p\n", g_records[i].addr);
            violations++;
        }
        void* user_region = (char*)g_records[i].addr - ARIX_SECURE_OVERHEAD_SIZE;
        ArixSecureAllocHeader* hdr = (ArixSecureAllocHeader*)user_region;
        if (hdr->magic != ARIX_SECURE_GUARD_MAGIC) {
            printf("VIOLATION: header magic corrupted at %p\n", g_records[i].addr);
            violations++;
        }
    }
    printf("Canary violations: %d\n", violations);
    printf("Total frees: %zu\n", g_stats_num_frees);
    printf("Double-free detections: %zu\n", g_stats_num_double_free);
    printf("Canary violations (total): %zu\n", g_stats_num_canary_violations);
    printf("Freelist blocks: ");
    int fcount = 0;
    for (ArixSecureFreeNode* n = g_freelist_head; n; n = n->next) fcount++;
    printf("%d\n", fcount);
    printf("Quarantine entries: %d/%d\n", g_quarantine_count, g_quarantine_max);
    printf("Scrub pattern: %d\n", g_scrub_pattern);
    int small_total = 0;
    for (int i = 0; i < ARIX_SMALL_BINS; i++)
        small_total += g_small_bins[i].count;
    printf("Small object cache entries: %d\n", small_total);
    printf("================================\n");
}

uint64_t arix_secure_canary_generate(void) {
    static uint64_t counter = 0;
    counter = (counter + 1) * 0x9E3779B97F4A7C15ULL ^ 0xDEADBEEFCAFEBABEULL;
    return counter;
}

int arix_secure_canary_check(void* ptr, uint64_t canary) {
    if (!ptr) return -1;
    for (int i = 0; i < g_record_count; i++) {
        if (g_records[i].addr == ptr && !g_records[i].is_freed) {
            size_t alloc_bytes = g_records[i].size + ARIX_SECURE_OVERHEAD_SIZE;
            size_t total = round_up_page(alloc_bytes + ARIX_SECURE_CANARY_SIZE);
            uint64_t* canary_ptr = (uint64_t*)((char*)ptr + total - ARIX_SECURE_OVERHEAD_SIZE - ARIX_SECURE_CANARY_SIZE);
            return (*canary_ptr == canary && g_records[i].canary == canary) ? 0 : -1;
        }
    }
    return -1;
}

int arix_secure_freelist_check(ArixSecureAllocator* alloc) {
    (void)alloc;
    if (!g_initialized) return -1;
    int errors = 0;
    ArixSecureFreeNode* cur = g_freelist_head;
    while (cur) {
        if (cur->canary != ARIX_SECURE_FREELIST_CANARY) {
            fprintf(stderr, "FREELIST ERROR: canary corrupted at node %p (base %p)\n", (void*)cur, cur->base);
            errors++;
        }
        if (cur->prev && cur->prev->next != cur) {
            fprintf(stderr, "FREELIST ERROR: broken prev link at node base %p\n", cur->base);
            errors++;
        }
        if (cur->next && cur->next->prev != cur) {
            fprintf(stderr, "FREELIST ERROR: broken next link at node base %p\n", cur->base);
            errors++;
        }
        if (cur->next && (char*)cur->base + cur->total > (char*)cur->next->base) {
            fprintf(stderr, "FREELIST ERROR: overlapping or out-of-order blocks at base %p and %p\n", cur->base, cur->next->base);
            errors++;
        }
        if (cur->next && (char*)cur->base + cur->total == (char*)cur->next->base) {
            fprintf(stderr, "FREELIST ERROR: adjacent blocks should be merged at base %p\n", cur->base);
            errors++;
        }
        cur = cur->next;
    }
    return (errors == 0) ? 0 : -1;
}

int arix_secure_free_quarantine(ArixSecureAllocator* alloc, void* ptr) {
    if (!alloc || !ptr) return -1;
    if (g_quarantine_count >= g_quarantine_max) {
        arix_secure_free_flush_quarantine(alloc);
    }
    if (g_quarantine_count >= g_quarantine_max) return -1;

    if (freed_set_check(ptr)) {
        g_stats_num_double_free++;
        return -1;
    }

    for (int i = 0; i < g_record_count; i++) {
        if (g_records[i].addr == ptr && !g_records[i].is_freed) {
            ArixSecureAllocRecord* r = &g_records[i];
            size_t alloc_bytes = r->size + ARIX_SECURE_OVERHEAD_SIZE;
            size_t total = round_up_page(alloc_bytes + ARIX_SECURE_CANARY_SIZE);
            size_t total_with_guards = total + r->guard_front + r->guard_back;
            void* base = (char*)ptr - r->guard_front - ARIX_SECURE_OVERHEAD_SIZE;
            void* user_region = (char*)base + r->guard_front;

            ArixSecureAllocHeader* hdr = (ArixSecureAllocHeader*)user_region;
            if (hdr->magic != ARIX_SECURE_GUARD_MAGIC) {
                g_stats_num_canary_violations++;
                if (alloc->on_overflow) alloc->on_overflow(r);
            }

            uint64_t* canary_ptr = (uint64_t*)((char*)ptr + total - ARIX_SECURE_OVERHEAD_SIZE - ARIX_SECURE_CANARY_SIZE);
            if (*canary_ptr != r->canary) {
                g_stats_num_canary_violations++;
                if (alloc->on_overflow) alloc->on_overflow(r);
            }

            secure_scrub(ptr, r->size);
            r->is_freed = 1;
            alloc->total_allocated -= (alloc->total_allocated >= r->size) ? r->size : alloc->total_allocated;
            g_stats_num_frees++;
            freed_set_add(ptr);

            ArixSecureFreeNode* node = freelist_node_alloc(base, total_with_guards);
            if (node) freelist_insert(node);

            g_quarantine[g_quarantine_count].base = base;
            g_quarantine[g_quarantine_count].total_with_guards = total_with_guards;
            g_quarantine_count++;
            return 0;
        }
    }
    return -1;
}

void arix_secure_free_flush_quarantine(ArixSecureAllocator* alloc) {
    (void)alloc;
    for (int i = 0; i < g_quarantine_count; i++) {
        if (g_quarantine[i].base) {
#ifdef _WIN32
            VirtualFree(g_quarantine[i].base, 0, MEM_RELEASE);
#else
            munmap(g_quarantine[i].base, g_quarantine[i].total_with_guards);
#endif
        }
    }
    g_quarantine_count = 0;
    memset(g_quarantine, 0, sizeof(g_quarantine));
}

ArixSecureAllocStats arix_secure_allocator_get_stats(ArixSecureAllocator* alloc) {
    ArixSecureAllocStats stats;
    memset(&stats, 0, sizeof(stats));
    if (!alloc) return stats;
    stats.total_allocated = alloc->total_allocated;
    stats.peak_allocated = alloc->peak_allocated;
    stats.num_allocations = alloc->num_allocations;
    stats.num_frees = g_stats_num_frees;
    stats.num_double_free_detected = g_stats_num_double_free;
    stats.num_canary_violations = g_stats_num_canary_violations;
    return stats;
}

void arix_secure_allocator_set_scrub_pattern(int pattern_id) {
    if (pattern_id >= 0 && pattern_id <= 2)
        g_scrub_pattern = pattern_id;
}

double arix_secure_allocator_get_fragmentation(void) {
    if (!g_freelist_head) return 0.0;
    size_t total_free = 0;
    size_t largest_free = 0;
    int block_count = 0;
    ArixSecureFreeNode* cur = g_freelist_head;
    while (cur) {
        total_free += cur->total;
        if (cur->total > largest_free) largest_free = cur->total;
        block_count++;
        cur = cur->next;
    }
    if (total_free == 0 || block_count <= 1) return 0.0;
    return 1.0 - ((double)largest_free / (double)total_free);
}

int arix_secure_allocator_defragment(void) {
    if (!g_freelist_head) return 0;
    int merged = 0;
    ArixSecureFreeNode* cur = g_freelist_head;
    while (cur && cur->next) {
        char* cur_end = (char*)cur->base + cur->total;
        if (cur_end == cur->next->base) {
            cur->total += cur->next->total;
            ArixSecureFreeNode* victim = cur->next;
            cur->next = victim->next;
            if (victim->next) victim->next->prev = cur;
            free(victim);
            merged++;
        } else {
            cur = cur->next;
        }
    }
    return merged;
}

int arix_secure_allocator_walk(void (*walk_callback)(const ArixSecureAllocRecord* record)) {
    if (!walk_callback) return -1;
    int walked = 0;
    for (int i = 0; i < g_record_count; i++) {
        if (!g_records[i].is_freed && g_records[i].addr) {
            walk_callback(&g_records[i]);
            walked++;
        }
    }
    return walked;
}

void arix_secure_allocator_set_quarantine_max(int max_entries) {
    if (max_entries > 0 && max_entries <= 4096)
        g_quarantine_max = max_entries;
}

int arix_secure_allocator_get_quarantine_count(void) {
    return g_quarantine_count;
}

static int small_bin_populate(int bin_idx) {
    if (bin_idx < 0 || bin_idx >= ARIX_SMALL_BINS) return -1;
    ArixSmallObjectBin* bin = &g_small_bins[bin_idx];
    int size = bin->bin_size;
    int needed = 0;
    for (int w = 0; w < ARIX_SMALL_BITMAP_WORDS; w++) {
        uint32_t bits = bin->bitmap[w];
        for (int b = 0; b < 32; b++) {
            if (!(bits & (1u << b)) && bin->blocks[w * 32 + b] == NULL) {
                needed++;
            }
        }
    }
    if (needed == 0) return 0;
    size_t block_total = round_up_page((size_t)size * needed + ARIX_SECURE_OVERHEAD_SIZE + ARIX_SECURE_CANARY_SIZE);
    size_t total_with_guards = block_total + ARIX_SECURE_PAGE_SIZE * 2;
    void* base = NULL;
#ifdef _WIN32
    base = VirtualAlloc(NULL, total_with_guards, MEM_RESERVE, PAGE_NOACCESS);
    if (!base) return -1;
    void* user_region = (char*)base + ARIX_SECURE_PAGE_SIZE;
    void* user_committed = VirtualAlloc(user_region, block_total, MEM_COMMIT, PAGE_READWRITE);
    if (!user_committed) { VirtualFree(base, 0, MEM_RELEASE); return -1; }
#else
    base = mmap(NULL, total_with_guards, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (base == MAP_FAILED) return -1;
    void* user_region = (char*)base + ARIX_SECURE_PAGE_SIZE;
    if (mprotect(user_region, block_total, PROT_READ | PROT_WRITE) != 0) {
        munmap(base, total_with_guards); return -1;
    }
#endif
    int slot = 0;
    for (int w = 0; w < ARIX_SMALL_BITMAP_WORDS && slot < needed; w++) {
        uint32_t bits = bin->bitmap[w];
        for (int b = 0; b < 32 && slot < needed; b++) {
            int idx = w * 32 + b;
            if (!(bits & (1u << b)) && bin->blocks[idx] == NULL) {
                bin->blocks[idx] = (char*)user_region + (size_t)slot * size;
                ArixSecureAllocHeader* hdr = (ArixSecureAllocHeader*)bin->blocks[idx];
                hdr->magic = ARIX_SECURE_GUARD_MAGIC;
                hdr->actual_size = (size_t)size;
                hdr->reserved = 0;
                bin->blocks[idx] = (char*)bin->blocks[idx] + ARIX_SECURE_OVERHEAD_SIZE;
                slot++;
            }
        }
    }
    return slot;
}
static int freelist_split_block(ArixSecureFreeNode* node, size_t needed) {
    if (!node || node->total < needed) return -1;
    size_t remaining = node->total - needed;
    if (remaining < ARIX_SECURE_PAGE_SIZE) return -1;
    void* split_base = (char*)node->base + needed;
    ArixSecureFreeNode* split = freelist_node_alloc(split_base, remaining);
    if (!split) return -1;
    node->total = needed;
    split->next = node->next;
    split->prev = node;
    if (node->next) node->next->prev = split;
    node->next = split;
    return 0;
}

static int freelist_count_blocks(void) {
    int count = 0;
    ArixSecureFreeNode* cur = g_freelist_head;
    while (cur) { count++; cur = cur->next; }
    return count;
}

static size_t freelist_total_free(void) {
    size_t total = 0;
    ArixSecureFreeNode* cur = g_freelist_head;
    while (cur) { total += cur->total; cur = cur->next; }
    return total;
}

static void record_remove_by_addr(void* addr) {
    for (int i = 0; i < g_record_count; i++) {
        if (g_records[i].addr == addr) {
            g_records[i].is_freed = 1;
            g_records[i].addr = NULL;
            return;
        }
    }
}

static int record_find_by_addr(void* addr) {
    for (int i = 0; i < g_record_count; i++) {
        if (g_records[i].addr == addr && !g_records[i].is_freed)
            return i;
    }
    return -1;
}

static void small_bin_destroy(void) {
    for (int i = 0; i < ARIX_SMALL_BINS; i++) {
        memset(g_small_bins[i].blocks, 0, sizeof(g_small_bins[i].blocks));
        memset(g_small_bins[i].bitmap, 0, sizeof(g_small_bins[i].bitmap));
        g_small_bins[i].count = 0;
    }
    g_small_bins_initialized = 0;
}

uint64_t arix_secure_allocator_get_total_allocated(ArixSecureAllocator* alloc) {
    if (!alloc) return 0;
    return alloc->total_allocated;
}

uint64_t arix_secure_allocator_get_peak_allocated(ArixSecureAllocator* alloc) {
    if (!alloc) return 0;
    return alloc->peak_allocated;
}

size_t arix_secure_allocator_get_num_allocations(ArixSecureAllocator* alloc) {
    if (!alloc) return 0;
    return alloc->num_allocations;
}

int arix_secure_allocator_get_freelist_count(void) {
    return freelist_count_blocks();
}

size_t arix_secure_allocator_get_freelist_total(void) {
    return freelist_total_free();
}

void arix_secure_allocator_enable_guard_pages(ArixSecureAllocator* alloc, int enable) {
    if (alloc) alloc->use_guard_pages = enable;
}

void arix_secure_allocator_enable_canaries(ArixSecureAllocator* alloc, int enable) {
    if (alloc) alloc->use_canaries = enable;
}
static void freelist_validate_all(void) {
    ArixSecureFreeNode* cur = g_freelist_head;
    while (cur) {
        if (cur->total < ARIX_SECURE_PAGE_SIZE) {
            cur = cur->next;
            continue;
        }
        size_t half = cur->total / 2;
        if (half < ARIX_SECURE_PAGE_SIZE) { cur = cur->next; continue; }
        void* split_base = (char*)cur->base + half;
        ArixSecureFreeNode* split = freelist_node_alloc(split_base, cur->total - half);
        if (split) {
            cur->total = half;
            split->next = cur->next;
            split->prev = cur;
            if (cur->next) cur->next->prev = split;
            cur->next = split;
        }
        cur = cur->next;
    }
}

int arix_secure_allocator_trim_pool(ArixSecureAllocator* alloc) {
    (void)alloc;
    int removed = 0;
    ArixSecureFreeNode* cur = g_freelist_head;
    while (cur) {
        ArixSecureFreeNode* next = cur->next;
        if (cur->total >= ARIX_SECURE_PAGE_SIZE * 4) {
            size_t trim = cur->total - ARIX_SECURE_PAGE_SIZE * 2;
            if (freelist_split_block(cur, cur->total - trim) == 0) {
                ArixSecureFreeNode* to_free = cur->next;
                if (to_free) {
                    if (to_free->prev) to_free->prev->next = to_free->next;
                    if (to_free->next) to_free->next->prev = to_free->prev;
                    free(to_free);
                    removed++;
                }
            }
        }
        cur = next;
    }
    return removed;
}

size_t arix_secure_allocator_get_small_bin_count(int bin_idx) {
    if (bin_idx < 0 || bin_idx >= ARIX_SMALL_BINS) return 0;
    return (size_t)g_small_bins[bin_idx].count;
}

int arix_secure_allocator_get_small_bin_size(int bin_idx) {
    if (bin_idx < 0 || bin_idx >= ARIX_SMALL_BINS) return 0;
    return g_small_bins[bin_idx].bin_size;
}
int arix_secure_allocator_get_record_count(void) { return g_record_count; }
int arix_secure_allocator_get_initialized(void) { return g_initialized; }
void arix_secure_allocator_set_initialized(int v) { g_initialized = v; }
void arix_secure_allocator_reset_stats(void) { g_stats_num_frees = 0; g_stats_num_double_free = 0; g_stats_num_canary_violations = 0; }