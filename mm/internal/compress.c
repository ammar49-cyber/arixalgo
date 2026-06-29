/*
 * Memory Compression Implementation — SKELETON
 * VERSION: v0.5
 */

#include "compress.h"
#include <stdlib.h>
#include <string.h>

int arix_compress_init(void) { return 0; }
int arix_compress_shutdown(void) { return 0; }
int arix_compress_register_codec(const ArixCompressionCodecImpl* codec) {
    (void)codec; return 0;
}
int arix_compress_unregister_codec(ArixCompressionCodec codec) { (void)codec; return 0; }
int arix_compress_apply(const void* src, size_t src_bytes, int dtype,
                        ArixCompressionCodec codec, ArixCompressedBuffer* dst) {
    (void)src; (void)src_bytes; (void)dtype; (void)codec;
    if (dst) memset(dst, 0, sizeof(*dst));
    return 0;
}
int arix_compress_decompress(const ArixCompressedBuffer* src, void* dst, size_t dst_bytes) {
    (void)src; (void)dst; (void)dst_bytes; return 0;
}
void arix_compress_buffer_destroy(ArixCompressedBuffer* buf) { free(buf->compressed_data); free(buf->metadata); }
int arix_compress_bfp_block_size(size_t element_bytes, size_t* block_size) {
    (void)element_bytes; if (block_size) *block_size = 128; return 0;
}
