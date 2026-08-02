// DS 2D engine, text-mode backgrounds.
//
// Register layout from GBATEK. The address arithmetic is cross-checked against
// the decomp's own G2::GetBG0ScrPtr, which computes
//
//     0x6000000 + ((DISPCNT & 0x38000000) >> 27 << 16) + ((BG0CNT & 0x1f00) >> 8 << 11)
//
// i.e. DISPCNT bits 27-29 select a 64K screen-base offset and BGxCNT bits 8-12 a
// 2K screen block. Those two shifts are the ones most likely to be wrong, so they
// are taken from code that is byte-verified against the ROM rather than from docs.

#include "ntr/ppu.h"

#include "ntr/mmio.h"

#include <cstdio>
#include <cstring>

namespace ntr {
namespace {

// Engine A registers start at 0x4000000, engine B at 0x4001000. Engine B has no
// DISPCNT char/screen base offset -- those bits are engine A only -- and its BG
// VRAM and palette live elsewhere.
struct EngineMap {
    uint32_t reg_base;
    uint32_t vram_base;
    uint32_t pltt_base;
    bool has_base_offset;
};

constexpr EngineMap kEngines[2] = {
    {0x04000000u, 0x06000000u, 0x05000000u, true},
    {0x04001000u, 0x06200000u, 0x05000400u, false},
};

inline uint16_t rd16(uint32_t a) { return *reinterpret_cast<volatile uint16_t *>(a); }
inline uint32_t rd32(uint32_t a) { return *reinterpret_cast<volatile uint32_t *>(a); }

// DS palettes are BGR555.
inline uint32_t bgr555(uint16_t c) {
    const uint32_t r = (c >> 0) & 0x1F, g = (c >> 5) & 0x1F, b = (c >> 10) & 0x1F;
    return 0xFF000000u | ((r << 3 | r >> 2) << 16) | ((g << 3 | g >> 2) << 8)
           | (b << 3 | b >> 2);
}

// Text-mode screen sizes, in tiles.
void text_size(unsigned sz, int &tw, int &th) {
    tw = (sz & 1) ? 64 : 32;
    th = (sz & 2) ? 64 : 32;
}

struct BgConfig {
    bool enabled;
    int priority;
    uint32_t screen;      // tilemap address
    uint32_t chars;       // tile data address
    bool bpp8;
    int tiles_w, tiles_h;
    int hofs, vofs;
};

BgConfig read_bg(const EngineMap &em, int bg, uint32_t dispcnt) {
    BgConfig c{};
    c.enabled = (dispcnt >> (8 + bg)) & 1;
    if (!c.enabled) return c;

    const uint16_t cnt = rd16(em.reg_base + 0x08 + bg * 2);
    c.priority = cnt & 3;
    c.bpp8 = (cnt >> 7) & 1;

    const uint32_t char_off = em.has_base_offset ? ((dispcnt >> 24) & 7) << 16 : 0;
    const uint32_t scr_off = em.has_base_offset ? ((dispcnt >> 27) & 7) << 16 : 0;

    c.screen = em.vram_base + scr_off + (((cnt >> 8) & 0x1F) << 11);
    c.chars = em.vram_base + char_off + (((cnt >> 2) & 3) << 14);

    text_size((cnt >> 14) & 3, c.tiles_w, c.tiles_h);

    // BGxHOFS/BGxVOFS are write-only 9-bit scroll registers, 4 bytes per BG.
    c.hofs = rd16(em.reg_base + 0x10 + bg * 4) & 0x1FF;
    c.vofs = rd16(em.reg_base + 0x12 + bg * 4) & 0x1FF;
    return c;
}

// One text-mode screen entry: tile 0-9, hflip 10, vflip 11, palette 12-15.
inline uint32_t screen_entry_addr(const BgConfig &c, int tx, int ty) {
    // 512-wide/tall maps are stored as consecutive 32x32 blocks of 2KB.
    const int bx = tx >> 5, by = ty >> 5;
    const int blocks_w = c.tiles_w >> 5;
    const int block = by * blocks_w + bx;
    return c.screen + block * 0x800 + (((ty & 31) * 32 + (tx & 31)) * 2);
}

// Returns false where the background is transparent at this pixel.
bool sample_bg(const EngineMap &em, const BgConfig &c, int x, int y, uint32_t &out) {
    const int px = (x + c.hofs) & (c.tiles_w * 8 - 1);
    const int py = (y + c.vofs) & (c.tiles_h * 8 - 1);

    const uint16_t se = rd16(screen_entry_addr(c, px >> 3, py >> 3));
    const int tile = se & 0x3FF;
    int fx = px & 7, fy = py & 7;
    if (se & 0x400) fx = 7 - fx;
    if (se & 0x800) fy = 7 - fy;

    uint32_t index;
    if (c.bpp8) {
        index = *reinterpret_cast<volatile uint8_t *>(c.chars + tile * 64 + fy * 8 + fx);
        if (index == 0) return false;
    } else {
        const uint8_t pair =
            *reinterpret_cast<volatile uint8_t *>(c.chars + tile * 32 + fy * 4 + (fx >> 1));
        index = (fx & 1) ? (pair >> 4) : (pair & 0xF);
        if (index == 0) return false;
        index += ((se >> 12) & 0xF) * 16;
    }
    out = bgr555(rd16(em.pltt_base + index * 2));
    return true;
}

}  // namespace

void ppu_scanout(Engine eng, Framebuffer &fb) {
    const EngineMap &em = kEngines[eng];
    const uint32_t dispcnt = rd32(em.reg_base);

    // Display mode 0 is "display off" -- the DS shows white, not black.
    const unsigned disp_mode = (dispcnt >> 16) & 3;
    const bool forced_blank = (dispcnt >> 7) & 1;
    if (disp_mode == 0 || forced_blank) {
        for (int y = 0; y < SCREEN_H; ++y)
            for (int x = 0; x < SCREEN_W; ++x) fb.px[y][x] = 0xFFFFFFFFu;
        return;
    }

    BgConfig bgs[4];
    for (int i = 0; i < 4; ++i) bgs[i] = read_bg(em, i, dispcnt);

    const uint32_t backdrop = bgr555(rd16(em.pltt_base));

    for (int y = 0; y < SCREEN_H; ++y) {
        for (int x = 0; x < SCREEN_W; ++x) {
            uint32_t c = backdrop;
            // Lower priority value wins; ties break toward the lower BG number,
            // so walk priorities 3..0 and let later writes overwrite.
            for (int prio = 3; prio >= 0; --prio) {
                for (int bg = 3; bg >= 0; --bg) {
                    if (!bgs[bg].enabled || bgs[bg].priority != prio) continue;
                    uint32_t s;
                    if (sample_bg(em, bgs[bg], x, y, s)) c = s;
                }
            }
            fb.px[y][x] = c;
        }
    }
}

bool ppu_write_bmp(const char *path, const Framebuffer &fb) {
    std::FILE *f = std::fopen(path, "wb");
    if (!f) return false;

    const uint32_t stride = SCREEN_W * 3;
    const uint32_t pad = (4 - (stride & 3)) & 3;
    const uint32_t img = (stride + pad) * SCREEN_H;
    const uint32_t off = 54;

    uint8_t hdr[54] = {};
    hdr[0] = 'B'; hdr[1] = 'M';
    const uint32_t total = off + img;
    std::memcpy(hdr + 2, &total, 4);
    std::memcpy(hdr + 10, &off, 4);
    const uint32_t dib = 40;
    std::memcpy(hdr + 14, &dib, 4);
    const int32_t w = SCREEN_W, h = SCREEN_H;
    std::memcpy(hdr + 18, &w, 4);
    std::memcpy(hdr + 22, &h, 4);
    const uint16_t planes = 1, bpp = 24;
    std::memcpy(hdr + 26, &planes, 2);
    std::memcpy(hdr + 28, &bpp, 2);
    std::memcpy(hdr + 34, &img, 4);
    std::fwrite(hdr, 1, sizeof hdr, f);

    const uint8_t zero[3] = {0, 0, 0};
    for (int y = SCREEN_H - 1; y >= 0; --y) {          // BMP rows run bottom-up
        for (int x = 0; x < SCREEN_W; ++x) {
            const uint32_t c = fb.px[y][x];
            const uint8_t bgr[3] = {static_cast<uint8_t>(c), static_cast<uint8_t>(c >> 8),
                                    static_cast<uint8_t>(c >> 16)};
            std::fwrite(bgr, 1, 3, f);
        }
        if (pad) std::fwrite(zero, 1, pad, f);
    }
    std::fclose(f);
    return true;
}

}  // namespace ntr
