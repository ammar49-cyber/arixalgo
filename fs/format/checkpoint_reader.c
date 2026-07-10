#include "checkpoint_reader.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    FILE* fp;
    SNEPPXCheckpointHeader header;
    SNEPPXTensorRecord* records;
    uint32_t num_records;
} SNEPPXCkptHandle;

int SNEPPX_ckpt_write_open(const char* path, SNEPPXCheckpointHeader* header, void** handle) {
    if (!path || !header || !handle) return -1;
    FILE* fp = fopen(path, "wb");
    if (!fp) return -1;
    SNEPPXCkptHandle* h = (SNEPPXCkptHandle*)calloc(1, sizeof(SNEPPXCkptHandle));
    if (!h) { fclose(fp); return -1; }
    h->fp = fp;
    memcpy(&h->header, header, sizeof(SNEPPXCheckpointHeader));
    h->header.magic_lo = SNEPPX_CKPT_MAGIC;
    h->header.magic_hi = SNEPPX_CKPT_MAGIC_HI;
    h->header.version = SNEPPX_CKPT_VERSION;
    fwrite(&h->header, sizeof(SNEPPXCheckpointHeader), 1, fp);
    h->records = (SNEPPXTensorRecord*)calloc(header->num_tensors, sizeof(SNEPPXTensorRecord));
    h->num_records = header->num_tensors;
    *handle = h;
    return 0;
}

int SNEPPX_ckpt_write_tensor(void* handle, const void* tensor_data, const SNEPPXTensorRecord* record) {
    if (!handle || !tensor_data || !record) return -1;
    SNEPPXCkptHandle* h = (SNEPPXCkptHandle*)handle;
    if (h->num_records == 0) return -1;
    uint32_t idx = h->header.num_tensors - h->num_records;
    memcpy(&h->records[idx], record, sizeof(SNEPPXTensorRecord));
    long pos = ftell(h->fp);
    h->records[idx].data_offset = (uint64_t)pos;
    fwrite(tensor_data, 1, record->data_size, h->fp);
    h->num_records--;
    return 0;
}

int SNEPPX_ckpt_write_metadata(void* handle, const char* metadata_json, size_t json_len) {
    if (!handle || !metadata_json) return -1;
    SNEPPXCkptHandle* h = (SNEPPXCkptHandle*)handle;
    long meta_pos = ftell(h->fp);
    h->header.metadata_offset = (uint64_t)meta_pos;
    h->header.metadata_size = (uint64_t)json_len;
    fwrite(metadata_json, 1, json_len, h->fp);
    long end_pos = ftell(h->fp);
    h->header.total_size = (uint64_t)end_pos;
    fseek(h->fp, 0, SEEK_SET);
    fwrite(&h->header, sizeof(SNEPPXCheckpointHeader), 1, h->fp);
    uint64_t records_offset = sizeof(SNEPPXCheckpointHeader);
    fseek(h->fp, (long)records_offset, SEEK_SET);
    fwrite(h->records, sizeof(SNEPPXTensorRecord), h->header.num_tensors, h->fp);
    return 0;
}

int SNEPPX_ckpt_write_close(void* handle) {
    if (!handle) return -1;
    SNEPPXCkptHandle* h = (SNEPPXCkptHandle*)handle;
    fclose(h->fp);
    free(h->records);
    free(h);
    return 0;
}

int SNEPPX_ckpt_read_open(const char* path, SNEPPXCheckpointHeader* header, void** handle) {
    if (!path || !header || !handle) return -1;
    FILE* fp = fopen(path, "rb");
    if (!fp) return -1;
    SNEPPXCkptHandle* h = (SNEPPXCkptHandle*)calloc(1, sizeof(SNEPPXCkptHandle));
    if (!h) { fclose(fp); return -1; }
    h->fp = fp;
    if (fread(&h->header, sizeof(SNEPPXCheckpointHeader), 1, fp) != 1) {
        fclose(fp); free(h); return -1;
    }
    if (h->header.magic_lo != SNEPPX_CKPT_MAGIC || h->header.magic_hi != SNEPPX_CKPT_MAGIC_HI) {
        fclose(fp); free(h); return -1;
    }
    memcpy(header, &h->header, sizeof(SNEPPXCheckpointHeader));
    if (h->header.num_tensors > 0) {
        h->records = (SNEPPXTensorRecord*)calloc(h->header.num_tensors, sizeof(SNEPPXTensorRecord));
        if (!h->records) { fclose(fp); free(h); return -1; }
        size_t nread = fread(h->records, sizeof(SNEPPXTensorRecord), h->header.num_tensors, fp);
        if (nread != h->header.num_tensors) {
            fclose(fp); free(h->records); free(h); return -1;
        }
    }
    *handle = h;
    return 0;
}

int SNEPPX_ckpt_read_tensor(void* handle, size_t tensor_idx, void* tensor_data, SNEPPXTensorRecord* record) {
    if (!handle || !tensor_data) return -1;
    SNEPPXCkptHandle* h = (SNEPPXCkptHandle*)handle;
    if (tensor_idx >= h->header.num_tensors) return -1;
    if (record) memcpy(record, &h->records[tensor_idx], sizeof(SNEPPXTensorRecord));
    fseeko64(h->fp, (off64_t)h->records[tensor_idx].data_offset, SEEK_SET);
    size_t nread = fread(tensor_data, 1, h->records[tensor_idx].data_size, h->fp);
    if (nread != h->records[tensor_idx].data_size) return -1;
    return 0;
}

int SNEPPX_ckpt_read_metadata(void* handle, char** metadata_json, size_t* json_len) {
    if (!handle || !metadata_json || !json_len) return -1;
    SNEPPXCkptHandle* h = (SNEPPXCkptHandle*)handle;
    if (h->header.metadata_size == 0) { *metadata_json = NULL; *json_len = 0; return 0; }
    *metadata_json = (char*)malloc(h->header.metadata_size + 1);
    if (!*metadata_json) return -1;
    fseeko64(h->fp, (off64_t)h->header.metadata_offset, SEEK_SET);
    size_t nread = fread(*metadata_json, 1, h->header.metadata_size, h->fp);
    if (nread != h->header.metadata_size) { free(*metadata_json); *metadata_json = NULL; return -1; }
    (*metadata_json)[h->header.metadata_size] = '\0';
    *json_len = h->header.metadata_size;
    return 0;
}

int SNEPPX_ckpt_read_close(void* handle) {
    if (!handle) return -1;
    SNEPPXCkptHandle* h = (SNEPPXCkptHandle*)handle;
    fclose(h->fp);
    free(h->records);
    free(h);
    return 0;
}

int SNEPPX_ckpt_validate(const char* path) {
    if (!path) return -1;
    FILE* fp = fopen(path, "rb");
    if (!fp) return -1;
    SNEPPXCheckpointHeader header;
    if (fread(&header, sizeof(SNEPPXCheckpointHeader), 1, fp) != 1) {
        fclose(fp); return -1;
    }
    fclose(fp);
    if (header.magic_lo != SNEPPX_CKPT_MAGIC || header.magic_hi != SNEPPX_CKPT_MAGIC_HI) return -1;
    if (header.version > SNEPPX_CKPT_VERSION) return -1;
    return 0;
}

int SNEPPX_ckpt_supports_version(uint32_t version) {
    return version <= SNEPPX_CKPT_VERSION ? 1 : 0;
}
