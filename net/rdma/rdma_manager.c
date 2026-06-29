/*
 * RDMA Manager Implementation — SKELETON
 * VERSION: v1.0
 */

#include "rdma_manager.h"
#include <stdlib.h>
#include <string.h>

int arix_rdma_open(ArixRDMAContext** ctx, int device_idx) {
    (void)device_idx;
    if (!ctx) return -1;
    *ctx = (ArixRDMAContext*)calloc(1, sizeof(ArixRDMAContext));
    return *ctx ? 0 : -1;
}

void arix_rdma_close(ArixRDMAContext* ctx) { free(ctx); }

int arix_rdma_register_memory(ArixRDMAContext* ctx, void* addr, size_t len, ArixRDMARegion** region) {
    (void)ctx; (void)addr; (void)len;
    if (!region) return -1;
    *region = (ArixRDMARegion*)calloc(1, sizeof(ArixRDMARegion));
    return *region ? 0 : -1;
}

int arix_rdma_deregister_memory(ArixRDMAContext* ctx, ArixRDMARegion* region) {
    (void)ctx; free(region); return 0;
}

int arix_rdma_create_qp(ArixRDMAContext* ctx, ArixRDMAQueuePair** qp) {
    (void)ctx;
    if (!qp) return -1;
    *qp = (ArixRDMAQueuePair*)calloc(1, sizeof(ArixRDMAQueuePair));
    return *qp ? 0 : -1;
}

int arix_rdma_connect_qp(ArixRDMAQueuePair* qp, int remote_qp_num, int remote_lid) {
    (void)qp; (void)remote_qp_num; (void)remote_lid; return 0;
}

int arix_rdma_disconnect_qp(ArixRDMAQueuePair* qp) { (void)qp; return 0; }

void arix_rdma_destroy_qp(ArixRDMAQueuePair* qp) { free(qp); }

int arix_rdma_read(ArixRDMAQueuePair* qp, void* local_addr, uint32_t lkey,
                   uint64_t remote_addr, uint32_t rkey, size_t len) {
    (void)qp; (void)local_addr; (void)lkey; (void)remote_addr; (void)rkey; (void)len;
    return 0;
}

int arix_rdma_write(ArixRDMAQueuePair* qp, void* local_addr, uint32_t lkey,
                    uint64_t remote_addr, uint32_t rkey, size_t len) {
    (void)qp; (void)local_addr; (void)lkey; (void)remote_addr; (void)rkey; (void)len;
    return 0;
}

int arix_rdma_poll_completion(ArixRDMAContext* ctx, int* num_completions) {
    (void)ctx; if (num_completions) *num_completions = 0; return 0;
}

int arix_rdma_send_tensor(ArixRDMAContext* ctx, const void* tensor, int dest_rank) {
    (void)ctx; (void)tensor; (void)dest_rank; return 0;
}

int arix_rdma_recv_tensor(ArixRDMAContext* ctx, void** tensor, int src_rank) {
    (void)ctx; (void)src_rank; if (tensor) *tensor = NULL; return 0;
}
