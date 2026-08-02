// LZ77 decompression.
//
// Almost every asset in the ROM is compressed: 455 models, 493 animations, 241
// collision meshes. SM64DS prefixes the standard BIOS stream with its own "LZ77"
// magic, so files look like
//
//     "LZ77" | 0x10 | size:24 | <compressed>
//
// The BIOS entry points are ARM asm primitives in the decomp (asm-shims.txt), so
// they need host versions anyway; this is that, plus a convenience wrapper that
// understands the SM64DS magic.

#include "ntr/lz77.h"

#include <cstdint>
#include <cstring>

namespace ntr {

// Standard BIOS LZ77: flag byte, then 8 blocks. A set flag bit means a
// (length, displacement) back-reference into what has already been produced.
int32_t lz77_decompress(const uint8_t *src, int32_t src_len, uint8_t *dst,
                        int32_t dst_capacity) {
    if (src_len < 4) return -1;
    const int32_t size = static_cast<int32_t>(src[1] | (src[2] << 8) | (src[3] << 16));
    if ((src[0] & 0xF0) != 0x10) return -1;
    if (size > dst_capacity) return -1;

    int32_t in = 4, out = 0;
    while (out < size) {
        if (in >= src_len) return -1;
        const uint8_t flags = src[in++];
        for (int b = 0; b < 8 && out < size; ++b) {
            if (flags & (0x80 >> b)) {
                if (in + 1 >= src_len) return -1;
                const uint32_t v = (static_cast<uint32_t>(src[in]) << 8) | src[in + 1];
                in += 2;
                const int32_t n = static_cast<int32_t>((v >> 12) & 0xF) + 3;
                const int32_t disp = static_cast<int32_t>(v & 0xFFF) + 1;
                if (disp > out || out + n > size) return -1;
                for (int32_t i = 0; i < n; ++i, ++out) dst[out] = dst[out - disp];
            } else {
                if (in >= src_len) return -1;
                dst[out++] = src[in++];
            }
        }
    }
    return out;
}

int32_t lz77_size(const uint8_t *src, int32_t src_len) {
    const uint8_t *p = src;
    int32_t n = src_len;
    if (n >= 8 && std::memcmp(p, "LZ77", 4) == 0) { p += 4; n -= 4; }
    if (n < 4 || (p[0] & 0xF0) != 0x10) return -1;
    return static_cast<int32_t>(p[1] | (p[2] << 8) | (p[3] << 16));
}

int32_t lz77_unpack(const uint8_t *src, int32_t src_len, uint8_t *dst,
                    int32_t dst_capacity) {
    if (src_len >= 8 && std::memcmp(src, "LZ77", 4) == 0)
        return lz77_decompress(src + 4, src_len - 4, dst, dst_capacity);
    return lz77_decompress(src, src_len, dst, dst_capacity);
}

}  // namespace ntr

// --- decomp asm shims ------------------------------------------------------
// swi 0x11/0x12. Both are ARM asm in the decomp; these stand in for them.
// The BIOS takes only src/dst -- the length lives in the stream header.

void LZ77UnCompReadNormalWrite8bit(const void *src, void *dst) {
    const uint8_t *s = static_cast<const uint8_t *>(src);
    const int32_t size = ntr::lz77_size(s, 1 << 30);
    if (size > 0) ntr::lz77_unpack(s, 1 << 30, static_cast<uint8_t *>(dst), size);
}

void LZ77UnCompReadByCallbackWrite16bit(const void *src, void *dst) {
    LZ77UnCompReadNormalWrite8bit(src, dst);
}
