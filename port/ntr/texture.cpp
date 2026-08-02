// DS texture decoding to RGBA.
//
// Formats from GBATEK. Format 5 (4x4-compressed) matters most: 84% of this
// game's 1,408 textures use it, so skipping it would mean texturing almost
// nothing.
//
// Its layout inside a BMD is confirmed by three consecutive texture records --
// each one's main data begins exactly `size + size/2` after the previous, so the
// per-block palette-index table sits immediately after the main block and is
// half its length.

#include "ntr/texture.h"

#include <cstring>

namespace ntr {
namespace {

inline uint32_t bgr555(uint16_t c, bool opaque = true) {
    const uint32_t r = c & 0x1F, g = (c >> 5) & 0x1F, b = (c >> 10) & 0x1F;
    return (opaque ? 0xFF000000u : 0u) | ((r << 3 | r >> 2) << 16)
           | ((g << 3 | g >> 2) << 8) | (b << 3 | b >> 2);
}

inline uint32_t with_alpha(uint32_t argb, uint32_t a) {
    return (argb & 0x00FFFFFFu) | (a << 24);
}

inline uint16_t pal_at(const uint8_t *pal, int32_t pal_len, uint32_t idx) {
    const uint32_t off = idx * 2u;
    if (off + 1 >= static_cast<uint32_t>(pal_len)) return 0;
    return static_cast<uint16_t>(pal[off] | (pal[off + 1] << 8));
}

// Blend two BGR555 colours in 1/8ths, staying in 5-bit space like the hardware.
uint16_t blend555(uint16_t a, uint16_t b, int wa, int wb) {
    const int ar = a & 0x1F, ag = (a >> 5) & 0x1F, ab = (a >> 10) & 0x1F;
    const int br = b & 0x1F, bg = (b >> 5) & 0x1F, bb = (b >> 10) & 0x1F;
    const int r = (ar * wa + br * wb) / 8;
    const int g = (ag * wa + bg * wb) / 8;
    const int bl = (ab * wa + bb * wb) / 8;
    return static_cast<uint16_t>((r & 0x1F) | ((g & 0x1F) << 5) | ((bl & 0x1F) << 10));
}

}  // namespace

bool texture_decode(const TextureDesc &d, std::vector<uint32_t> &out) {
    if (d.width <= 0 || d.height <= 0) return false;
    const size_t n = static_cast<size_t>(d.width) * d.height;
    out.assign(n, 0);

    switch (d.format) {
        case 1: {  // A3I5: 5-bit index, 3-bit alpha
            if (d.data_len < static_cast<int32_t>(n)) return false;
            for (size_t i = 0; i < n; ++i) {
                const uint8_t b = d.data[i];
                const uint32_t a = (b >> 5) & 7;
                out[i] = with_alpha(bgr555(pal_at(d.pal, d.pal_len, b & 0x1F)),
                                    (a * 255) / 7);
            }
            return true;
        }
        case 2: {  // 4-colour palette, 2bpp
            for (size_t i = 0; i < n; ++i) {
                const size_t byte = i >> 2;
                if (byte >= static_cast<size_t>(d.data_len)) break;
                const uint32_t idx = (d.data[byte] >> ((i & 3) * 2)) & 3;
                out[i] = (idx == 0 && d.color0_transparent)
                             ? 0u : bgr555(pal_at(d.pal, d.pal_len, idx));
            }
            return true;
        }
        case 3: {  // 16-colour, 4bpp
            for (size_t i = 0; i < n; ++i) {
                const size_t byte = i >> 1;
                if (byte >= static_cast<size_t>(d.data_len)) break;
                const uint32_t idx = (i & 1) ? (d.data[byte] >> 4) : (d.data[byte] & 0xF);
                out[i] = (idx == 0 && d.color0_transparent)
                             ? 0u : bgr555(pal_at(d.pal, d.pal_len, idx));
            }
            return true;
        }
        case 4: {  // 256-colour, 8bpp
            for (size_t i = 0; i < n; ++i) {
                if (i >= static_cast<size_t>(d.data_len)) break;
                const uint32_t idx = d.data[i];
                out[i] = (idx == 0 && d.color0_transparent)
                             ? 0u : bgr555(pal_at(d.pal, d.pal_len, idx));
            }
            return true;
        }
        case 5: {  // 4x4-compressed
            const int bw = d.width / 4, bh = d.height / 4;
            const int32_t need = static_cast<int32_t>(bw) * bh * 4;
            if (d.data_len < need || !d.index || d.index_len < need / 2) return false;

            for (int by = 0; by < bh; ++by) {
                for (int bx = 0; bx < bw; ++bx) {
                    const int block = by * bw + bx;
                    const uint32_t texels =
                        static_cast<uint32_t>(d.data[block * 4])
                        | (static_cast<uint32_t>(d.data[block * 4 + 1]) << 8)
                        | (static_cast<uint32_t>(d.data[block * 4 + 2]) << 16)
                        | (static_cast<uint32_t>(d.data[block * 4 + 3]) << 24);
                    const uint16_t info = static_cast<uint16_t>(
                        d.index[block * 2] | (d.index[block * 2 + 1] << 8));

                    // Palette offset is in 4-byte units, i.e. two colours.
                    const uint32_t base = (info & 0x3FFF) * 2u;
                    const uint32_t mode = (info >> 14) & 3;

                    const uint16_t c0 = pal_at(d.pal, d.pal_len, base);
                    const uint16_t c1 = pal_at(d.pal, d.pal_len, base + 1);
                    uint16_t c2 = 0, c3 = 0;
                    bool a2 = true, a3 = true;
                    switch (mode) {
                        case 0: c2 = pal_at(d.pal, d.pal_len, base + 2); c3 = 0; a3 = false; break;
                        case 1: c2 = blend555(c0, c1, 4, 4); c3 = 0; a3 = false; break;
                        case 2: c2 = pal_at(d.pal, d.pal_len, base + 2);
                                c3 = pal_at(d.pal, d.pal_len, base + 3); break;
                        case 3: c2 = blend555(c0, c1, 5, 3);
                                c3 = blend555(c0, c1, 3, 5); break;
                    }
                    const uint16_t cols[4] = {c0, c1, c2, c3};
                    const bool op[4] = {true, true, a2, a3};

                    for (int ty = 0; ty < 4; ++ty) {
                        for (int tx = 0; tx < 4; ++tx) {
                            const int bit = (ty * 4 + tx) * 2;
                            const uint32_t sel = (texels >> bit) & 3;
                            const int px = bx * 4 + tx, py = by * 4 + ty;
                            if (px >= d.width || py >= d.height) continue;
                            out[static_cast<size_t>(py) * d.width + px] =
                                op[sel] ? bgr555(cols[sel]) : 0u;
                        }
                    }
                }
            }
            return true;
        }
        case 6: {  // A5I3: 3-bit index, 5-bit alpha
            if (d.data_len < static_cast<int32_t>(n)) return false;
            for (size_t i = 0; i < n; ++i) {
                const uint8_t b = d.data[i];
                const uint32_t a = (b >> 3) & 0x1F;
                out[i] = with_alpha(bgr555(pal_at(d.pal, d.pal_len, b & 7)),
                                    (a * 255) / 31);
            }
            return true;
        }
        case 7: {  // direct 16-bit colour; bit 15 is the alpha flag
            for (size_t i = 0; i < n; ++i) {
                if ((i * 2 + 1) >= static_cast<size_t>(d.data_len)) break;
                const uint16_t c = static_cast<uint16_t>(d.data[i * 2] | (d.data[i * 2 + 1] << 8));
                out[i] = (c & 0x8000) ? bgr555(c) : 0u;
            }
            return true;
        }
        default:
            return false;
    }
}

}  // namespace ntr
