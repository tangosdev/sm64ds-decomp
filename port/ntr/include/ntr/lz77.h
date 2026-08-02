#ifndef NTR_LZ77_H
#define NTR_LZ77_H

#include <stdint.h>

namespace ntr {

// Decompressed size, or -1. Accepts the SM64DS "LZ77" magic or a bare stream.
int32_t lz77_size(const uint8_t *src, int32_t src_len);

// Unpack, skipping the SM64DS magic if present. Returns bytes written, or -1.
int32_t lz77_unpack(const uint8_t *src, int32_t src_len, uint8_t *dst, int32_t dst_capacity);

// Bare BIOS stream (no magic): 0x10, size:24, data.
int32_t lz77_decompress(const uint8_t *src, int32_t src_len, uint8_t *dst, int32_t dst_capacity);

}  // namespace ntr

#endif
