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
#include <cstdlib>
#include <cstring>

namespace ntr {

// ---- THE RUNTIME ACTIVE EXTENT (see ntr/ppu.h) -----------------------------
//
// Defaults are the tier's own screen. On NTR_WIDE_RT the framebuffer is the
// wide maximum but the DEFAULT aspect is 4:3 (the old 2x window's 512x384), so
// a run that never sets the toggle -- or a tool that links this without wiring
// the settings key -- is the 4:3 build exactly. configure_aspect() below is
// the only writer, called once at boot.
#ifdef NTR_WIDE_RT
int active_w = 512;
int active_h = 384;
#else
int active_w = SCREEN_W;
int active_h = SCREEN_H;
#endif
#ifdef NTR_WIDE169
// The fixed-compile 16:9 tier: the wide branches are ALWAYS on, so the runtime
// flag they now read is true from the start. (NTR_WIDE_RT starts 4:3 and flips
// this in configure_aspect; every 4:3 tier leaves it false.)
bool widescreen = true;
#else
bool widescreen = false;
#endif

void configure_aspect(double aspect)
{
#ifdef NTR_WIDE_RT
    if (!(aspect > 0.0)) {
        // 0, negative, or NaN: the native sentinel. The 4:3 window at the old
        // 2x size, rendered into the top-left of the wide buffer and presented
        // at that size -- byte-for-byte the shipped build.
        active_w = 512;
        active_h = 384;
    } else {
        // Clamp again at the point of use. host_setting_aspect already clamps,
        // but this is the divide that sizes a framebuffer and it does not get to
        // trust its caller.
        if (aspect < 1.0) aspect = 1.0;
        if (aspect > 3.0) aspect = 3.0;
        // The largest w:h rectangle that fits the wide-maximum buffer. Start
        // full width and derive the height; if that is taller than the buffer,
        // the ratio is narrower than the buffer's own and the height binds
        // instead. At 1.7777778 the first arm lands exactly on 1024x576, the
        // measured 16:9 tier, with no rounding at all.
        int w = SCREEN_W;
        int h = (int)(SCREEN_W / aspect + 0.5);
        if (h > SCREEN_H) {
            h = SCREEN_H;
            w = (int)(SCREEN_H * aspect + 0.5);
            if (w > SCREEN_W) w = SCREEN_W;
        }
        // Even extents: several centring and half-screen reads divide by two,
        // and an odd width there leaves a one-pixel seam for no gain. The
        // DERIVED side rounds in the direction that keeps the picture no WIDER
        // than the ratio asked for, so the delivered w:h stays inside the
        // clamp rather than sliding a hair past it -- 3.0 asked for on this
        // buffer is 1024/342 (2.994) and not 1024/340 (3.012). The PINNED side
        // is already even (both buffer dimensions are), so its round is a no-op.
        if (h != SCREEN_H) h = (h + 1) & ~1;   // width pinned: round height up
        else               w &= ~1;            // height pinned: round width down
        if (h > SCREEN_H) h = SCREEN_H;
        if (w < 2) w = 2;
        if (h < 2) h = 2;
        active_w = w;
        active_h = h;
    }
    // "Is this run's picture wider than the DS's 4:3", which is what every
    // runtime wide branch actually asks. Computed from the extent rather than
    // stored from the argument, so an Aspect of 1.3333 (a 4:3 ratio written out
    // longhand) correctly reads as NOT widescreen: there is no frustum to widen.
    widescreen = (active_w * 3 != active_h * 4);
#else
    // A fixed tier has one aspect; there is nothing to choose. Kept so callers
    // can invoke it unconditionally.
    (void)aspect;
#endif
}

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
    // FOUR bits of character base, not the GBA's two. The decomp's own
    // G2S::GetBG0CharPtr is (v & 0x3c) >> 2 << 14, and the game uses base 4 on
    // the sub screen -- with two bits every engine-B tile fetch lands at base 0.
    c.chars = em.vram_base + char_off + (((cnt & 0x3c) >> 2) << 14);

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
        for (int y = 0; y < active_h; ++y)
            for (int x = 0; x < active_w; ++x) fb.px[y][x] = 0xFFFFFFFFu;
        return;
    }

    BgConfig bgs[4];
    for (int i = 0; i < 4; ++i) bgs[i] = read_bg(em, i, dispcnt);

    const uint32_t backdrop = bgr555(rd16(em.pltt_base));

    for (int y = 0; y < active_h; ++y) {
        for (int x = 0; x < active_w; ++x) {
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

// --- OBJ layer --------------------------------------------------------------
// GBATEK "LCD OBJ": 128 OAM entries per engine; tiles in the engine's OBJ
// VRAM region, palette at pltt+0x200. Sizes come from the shape/size table.
// Affine params sit in the filler word of entries group*4+0..3 (pa,pb,pc,pd).
//
// OBJ MODE, attribute 0 bits 10-11. GBATEK: 0 Normal, 1 Semi-Transparent,
// 2 OBJ Window, 3 Prohibited (the DS reuses 3 for bitmap OBJs). The decomp's
// own OAM::Render pins the field position independently -- it composes the
// word as `(mode << 10) | (mosaic << 12)` in
// src/_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi.c, and the sibling TU's bitfield
// `yb:8, objMode:2, mode:2, mosaic:1, ...` puts the same two bits at 10-11
// (that TU's `objMode` name is bits 8-9, the affine/double-size pair, which is
// a naming collision between the two TUs and not a disagreement).
//
// THIS CODE USED TO SKIP MODE 3 AND CALL IT "the OBJ window". That is the wrong
// mode: mode 3 is the prohibited/bitmap encoding and mode 2 is the window. The
// effect of the mix-up is that an OBJ-window sprite -- which on hardware is
// INVISIBLE and contributes only a mask -- was drawn as an ordinary opaque
// sprite, painting the mask shape onto the screen. Nothing in the port's
// measured runs uses either mode (port/ppu_gap_audit.txt: zero mode-1, mode-2
// and mode-3 objects across 1200 sampled frames of level 1 and scene 4), so
// this is a latent contract error rather than a visible regression, and it is
// corrected here rather than left for whatever first uses the mode.
//
// This file has no window unit, so a mode-2 sprite cannot contribute its mask
// here. Not drawing it is still the closer answer of the two: on hardware it
// does not appear in the image either way.
/* ENGINE A's OAM SOURCE OVERRIDE, for the gapless seam. The port composites
   the top screen BEFORE this frame's OAM::Load and scans the bottom screen
   after it, so the two screens are one frame apart -- a deliberate trade
   everywhere else, but with the screens butted edge to edge a falling object
   loses one frame of travel at the seam and reads one pixel short while it
   straddles. When the mod is engaged, hal/screen_gap.cpp points this at the
   game's own working shadow (the block Load is ABOUT to upload), so the top
   screen draws the same frame the bottom will. Zero everywhere else, which
   is the hardware OAM and the behaviour every level has always had. */
static uint32_t g_oam_src_a;
void ppu_obj_oam_source_a(uint32_t addr) { g_oam_src_a = addr; }

void ppu_scanout_obj(Engine eng, Framebuffer &fb) {
    static const int kSizes[3][4][2] = {
        {{8, 8}, {16, 16}, {32, 32}, {64, 64}},    // square
        {{16, 8}, {32, 8}, {32, 16}, {64, 32}},    // wide
        {{8, 16}, {8, 32}, {16, 32}, {32, 64}},    // tall
    };
    const uint32_t oam_base =
        eng == ENGINE_A ? (g_oam_src_a ? g_oam_src_a : 0x07000000u)
                        : 0x07000400u;
    const uint32_t obj_vram = eng == ENGINE_A ? 0x06400000u : 0x06600000u;
    const uint32_t obj_pltt = kEngines[eng].pltt_base + 0x200u;
    // DISPCNT bit 4: OBJ 1D mapping; bits 20-21: tile-index boundary shift.
    const uint32_t dispcnt = rd32(kEngines[eng].reg_base);
    const uint32_t boundary = 32u << ((dispcnt >> 20) & 3);
    // BIT 4, 1 = one-dimensional (a sprite's tiles are consecutive), 0 = two-
    // dimensional (OBJ VRAM is a 32-tile-wide matrix and the next row of a
    // sprite is 32 slots on). THIS READER USED TO HARD-CODE 1D, and on the DS
    // that is the mode the game almost never leaves the engine in:
    // Scene::ResetHardwareRegisters ANDs 0xffcfffef into both engines' DISPCNT,
    // which clears bit 4 along with the boundary field, and nothing on the
    // scene paths sets it back (GX::SetGraphicsMode preserves bit 4 -- its mask
    // is 0xfff0fff0 -- and its third argument only ever reaches bit 3).
    // Measured on scene 374: DISPCNT_A 00001400 / 00007400 on all 300 samples,
    // bit 4 CLEAR. ntr/ppu_sub.cpp has read the bit since the sub screen's
    // bring-up; this is engine A catching up, and the two now compute the slot
    // the same way.
    const bool map1d = (dispcnt >> 4) & 1;

    for (int i = 127; i >= 0; --i) {           // low index on top: draw high first
        const uint16_t a0 = rd16(oam_base + i * 8u);
        const uint16_t a1 = rd16(oam_base + i * 8u + 2);
        const uint16_t a2 = rd16(oam_base + i * 8u + 4);
        /* SM64DS_OAMAGE_TRACE=1: one line per bob-omb entry, naming the table
           this raster read and the row it held. Temporary owner-pass probe. */
        {
            static int aget = -1;
            if (aget < 0) {
                const char *e = std::getenv("SM64DS_OAMAGE_TRACE");
                aget = e && *e && *e != '0';
            }
            static unsigned af;
            if (aget && i == 127) ++af;
            if (aget && a2 == 0x1010)
                std::fprintf(stderr, "[oamage] A f%u src=%08x slot%d y=%d\n",
                             af, (unsigned)oam_base, i, (int)(a0 & 0xff));
        }
        const bool affine = a0 & 0x100;
        if (!affine && (a0 & 0x200)) continue;             // disabled
        const unsigned objmode = (a0 >> 10) & 3;
        if (objmode == 2) continue;   // OBJ window: a mask, never drawn
        if (objmode == 3) continue;   // bitmap OBJ: not hosted
        const int shape = (a0 >> 14) & 3;
        if (shape == 3) continue;
        const int size = (a1 >> 14) & 3;
        const int w = kSizes[shape][size][0], h = kSizes[shape][size][1];
        const bool dbl = affine && (a0 & 0x200);
        const int bw = dbl ? w * 2 : w, bh = dbl ? h * 2 : h;
        int x = a1 & 0x1FF, y = a0 & 0xFF;
        if (x >= 256) x -= 512;
        if (y >= 192 && y >= 256 - bh) y -= 256;
        const bool c256 = a0 & 0x2000;
        const bool hflip = !affine && (a1 & 0x1000);
        const bool vflip = !affine && (a1 & 0x2000);
        const uint32_t tile = a2 & 0x3FF;
        const uint32_t pal = (a2 >> 12) & 0xF;

        int pa = 256, pb = 0, pc = 0, pd = 256;
        if (affine) {
            const int grp = (a1 >> 9) & 0x1F;
            pa = (int16_t)rd16(oam_base + (grp * 4 + 0) * 8u + 6);
            pb = (int16_t)rd16(oam_base + (grp * 4 + 1) * 8u + 6);
            pc = (int16_t)rd16(oam_base + (grp * 4 + 2) * 8u + 6);
            pd = (int16_t)rd16(oam_base + (grp * 4 + 3) * 8u + 6);
        }

        for (int sy = 0; sy < bh; ++sy) {
            const int py = y + sy;
            if (py < 0 || py >= active_h) continue;
            for (int sx = 0; sx < bw; ++sx) {
                const int px = x + sx;
                if (px < 0 || px >= active_w) continue;
                // texel coordinate: affine maps through the matrix around the
                // box center; plain is direct with optional flips.
                int tx, ty;
                if (affine) {
                    const int cx = sx - bw / 2, cy = sy - bh / 2;
                    tx = ((pa * cx + pb * cy) >> 8) + w / 2;
                    ty = ((pc * cx + pd * cy) >> 8) + h / 2;
                    if (tx < 0 || tx >= w || ty < 0 || ty >= h) continue;
                } else {
                    tx = hflip ? w - 1 - sx : sx;
                    ty = vflip ? h - 1 - sy : sy;
                }
                const int tcol = tx >> 3, trow = ty >> 3;
                const int fx = tx & 7, fy = ty & 7;
                // Where this 8x8 cell lives, in 32-byte tile slots from the
                // sprite's base. 1D: consecutive, and a 256-colour cell is two
                // slots. 2D: a 32-slot-wide matrix, and a 256-colour cell is
                // two slots wide within it.
                const uint32_t slot =
                    map1d ? (uint32_t)(trow * (w / 8) + tcol) * (c256 ? 2u : 1u)
                          : (uint32_t)(trow * 32 + (c256 ? tcol * 2 : tcol));
                const uint32_t cell = obj_vram + tile * boundary + slot * 32u;
                uint32_t index;
                if (c256) {
                    index = *reinterpret_cast<volatile uint8_t *>(cell + fy * 8u + fx);
                    if (!index) continue;
                    fb.px[py][px] = bgr555(rd16(obj_pltt + index * 2u));
                } else {
                    const uint8_t b =
                        *reinterpret_cast<volatile uint8_t *>(cell + fy * 4u + fx / 2);
                    index = (fx & 1) ? (b >> 4) : (b & 0xF);
                    if (!index) continue;
                    fb.px[py][px] = bgr555(rd16(obj_pltt + (pal * 16u + index) * 2u));
                }
            }
        }
    }
}

/* THE BMP WRITER IS PARAMETRIC ON SIZE, and ppu_write_bmp is it at the
   framebuffer's. It was written inline against SCREEN_W and SCREEN_H, which
   was right while the framebuffer was the only image the port ever dumped.
   The stacked presentation (ntr/ppu_sub.cpp) is a second image and it is
   twice as tall, so the size moved into arguments rather than being spelled a
   second time somewhere else. At w = SCREEN_W, h = SCREEN_H every byte this
   writes is the byte the old body wrote: same header fields, same bottom-up
   row order, same 4-byte row padding, same BGR triple. */
bool ppu_write_bmp_px(const char *path, const uint32_t *px, int w, int h) {
    if (!px || w <= 0 || h <= 0) return false;
    std::FILE *f = std::fopen(path, "wb");
    if (!f) return false;

    const uint32_t stride = (uint32_t)w * 3;
    const uint32_t pad = (4 - (stride & 3)) & 3;
    const uint32_t img = (stride + pad) * (uint32_t)h;
    const uint32_t off = 54;

    uint8_t hdr[54] = {};
    hdr[0] = 'B'; hdr[1] = 'M';
    const uint32_t total = off + img;
    std::memcpy(hdr + 2, &total, 4);
    std::memcpy(hdr + 10, &off, 4);
    const uint32_t dib = 40;
    std::memcpy(hdr + 14, &dib, 4);
    const int32_t bw = w, bh = h;
    std::memcpy(hdr + 18, &bw, 4);
    std::memcpy(hdr + 22, &bh, 4);
    const uint16_t planes = 1, bpp = 24;
    std::memcpy(hdr + 26, &planes, 2);
    std::memcpy(hdr + 28, &bpp, 2);
    std::memcpy(hdr + 34, &img, 4);
    std::fwrite(hdr, 1, sizeof hdr, f);

    const uint8_t zero[3] = {0, 0, 0};
    for (int y = h - 1; y >= 0; --y) {                 // BMP rows run bottom-up
        const uint32_t *row = px + (size_t)y * (size_t)w;
        for (int x = 0; x < w; ++x) {
            const uint32_t c = row[x];
            const uint8_t bgr[3] = {static_cast<uint8_t>(c), static_cast<uint8_t>(c >> 8),
                                    static_cast<uint8_t>(c >> 16)};
            std::fwrite(bgr, 1, 3, f);
        }
        if (pad) std::fwrite(zero, 1, pad, f);
    }
    std::fclose(f);
    return true;
}

bool ppu_write_bmp(const char *path, const Framebuffer &fb) {
    // The framebuffer's live image is active_w x active_h. On every fixed tier
    // (and on NTR_WIDE_RT with the toggle on) that equals the row stride, so the
    // buffer is already tight and this is the byte-identical old call.
    if (active_w == SCREEN_W)
        return ppu_write_bmp_px(path, &fb.px[0][0], active_w, active_h);
    // NTR_WIDE_RT, 4:3 toggle: the picture is narrower than the SCREEN_W stride,
    // so pack the active rows tight before the writer (which assumes stride ==
    // width) sees them. Static because this is a debug/probe dump, not per-frame.
    static uint32_t packed[SCREEN_W * SCREEN_H];
    for (int y = 0; y < active_h; ++y)
        std::memcpy(packed + (size_t)y * active_w, fb.px[y], (size_t)active_w * 4);
    return ppu_write_bmp_px(path, packed, active_w, active_h);
}

// ---- THE DISPLAY CAPTURE UNIT ----------------------------------------------
//
// See ntr/ppu.h for the register decode and for what is and is not modelled.
// Everything here is GBATEK's DISPCAPCNT and VRAM control tables.

namespace {

constexpr uint32_t kDispCapCnt = 0x04000064u;
constexpr uint32_t kVramCnt = 0x04000240u;   // A..G at +0..+6, H at +7, I at +8
constexpr uint32_t kLcdcBase = 0x06800000u;  // block N at +N*0x20000
constexpr uint32_t kBankSize = 0x20000u;     // 128 KB, banks A..D

// What the last capture put in each block, so ppu_vram_publish knows the one
// range worth carrying. `len` 0 means this block has never been captured into,
// which is every block in every scene that never programs the unit.
struct CapRegion {
    uint32_t off;    // bytes from the start of the bank
    uint32_t len;    // bytes written
};
CapRegion g_cap[4];

// The home each captured block was last seen at, so a change can be noticed.
// 0 is "not established yet" and the first publish pass only records.
uint32_t g_home[4];
int g_home_have[4];

// One-shot refusals. A picture that quietly comes out of an unmodelled corner
// is worse than one that does not come out at all, so each corner says its
// piece once and then stops.
int g_said_srcb, g_said_notlcdc;

/* THE BLOCK'S OWN STORAGE. The capture unit has its own port into VRAM and
   writes the BLOCK, not a mapping of it, so the destination is the LCDC-space
   address whatever the bank's VRAMCNT currently says. */
inline uint32_t lcdc_addr(unsigned block) {
    return kLcdcBase + (uint32_t)block * kBankSize;
}

/* WHERE A BANK IS VISIBLE RIGHT NOW, per GBATEK's VRAM control table, for the
   two banks the capture path uses and the two MSTs it uses them in. Returns 0
   for "nowhere a reader in this program looks", which covers a disabled bank,
   an ARM7 allocation and a texture slot alike -- all three are correct answers
   of the same kind: nothing here reads that. */
uint32_t bank_home(unsigned block, uint8_t cnt) {
    if (!(cnt & 0x80)) return 0;             // disabled
    const unsigned mst = cnt & 7;
    if (mst == 0) return lcdc_addr(block);   // LCDC, every bank
    if (block == 2 && mst == 4) return 0x06200000u;   // VRAM-C -> engine B BG
    if (block == 3 && mst == 4) return 0x06600000u;   // VRAM-D -> engine B OBJ
    /* Every other MST is a real answer of the same kind -- an ARM7 allocation,
       a texture slot, an engine A window -- and no reader on the capture path
       looks at any of them. Silent, because there is nothing wrong with it: a
       captured block sitting where nothing reads it is the ordinary state of
       three blocks out of four. */
    return 0;
}

}  // namespace

void ppu_vram_publish(void) {
    for (unsigned b = 0; b < 4; ++b) {
        if (!g_cap[b].len) continue;         // never captured into
        const uint8_t cnt = *reinterpret_cast<volatile uint8_t *>(kVramCnt + b);
        const uint32_t home = bank_home(b, cnt);
        if (!g_home_have[b]) {
            /* THE FIRST PASS ONLY RECORDS. The capture that made this block
               interesting has just landed at the LCDC address, which is where
               a reader would find it anyway while the bank is still there, so
               there is nothing to carry yet. */
            g_home_have[b] = 1;
            g_home[b] = home;
            continue;
        }
        if (home == g_home[b]) continue;
        g_home[b] = home;
        if (!home) continue;                 // nowhere this program reads
        if (home == lcdc_addr(b)) continue;  // the bytes are already there
        std::memcpy(reinterpret_cast<void *>(home + g_cap[b].off),
                    reinterpret_cast<const void *>(lcdc_addr(b) + g_cap[b].off),
                    g_cap[b].len);
    }
}

void ppu_display_capture(const uint32_t *src, int w, int h) {
    volatile uint32_t *reg = reinterpret_cast<volatile uint32_t *>(kDispCapCnt);
    const uint32_t cap = *reg;
    if (!(cap & 0x80000000u)) return;

    /* THE ENABLE BIT IS CLEARED HERE AND UNCONDITIONALLY, before any decision
       about whether the capture can be performed. The hardware clears it at the
       end of the captured frame whatever it captured, and a unit that only
       cleared it on the paths it understood would spin on the ones it did
       not. */
    *reg = cap & ~0x80000000u;

    const unsigned source = (cap >> 29) & 3;
    if (source != 0) {
        if (!g_said_srcb) {
            g_said_srcb = 1;
            std::fprintf(stderr,
                         "  [capture] DISPCAPCNT %08x selects capture source "
                         "%u (B, or A and B blended). Only source A -- engine "
                         "A's own output -- is modelled, because it is the only "
                         "one anything in this game programs. Nothing is "
                         "captured this frame.\n", cap, source);
            std::fflush(stderr);
        }
        return;
    }
    if (!src || w <= 0 || h <= 0) return;

    const unsigned block = (cap >> 16) & 3;

    /* ---- THE DESTINATION HAS TO BE A BLOCK ---------------------------------
     *
     * The capture unit writes a VRAM BLOCK, and a bank is only reachable as a
     * block while it is allocated to LCDC; once its VRAMCNT hands it to an
     * engine or to the texture unit it is that unit's memory. Every capture
     * this game asks for on purpose arranges exactly that first: both
     * dScMgD3DBase_c arms call func_02054430 -- which is Vram__Map, writing
     * VRAMCNT = 0x80 -- on the destination bank in the same breath as they arm
     * the register.
     *
     * THE ONE WRITE THAT DOES NOT is Scene::ResetHardwareRegisters', and it is
     * why this is a test rather than a note. It writes DISPCAPCNT = 0x80000000
     * -- the enable bit with every field zero, so a 128x128 capture into block
     * 0 at offset 0 -- and it runs at the top of every scene in the game. Block
     * 0 is VRAM-A, and this port maps TEXTURE SLOT 0 at 0x06800000
     * (ntr/gx.cpp's TEX_SLOT_BASE), so honouring it unconditionally writes
     * 32 KB of downsampled framebuffer over the first texture slot on the first
     * frame of every scene.
     *
     * HONEST SCOPING, because this is the one corner of the unit that is not
     * simply transcribed. The field layout, the auto-clear and the source
     * selection are GBATEK's and are reproduced as written. Whether the
     * hardware performs a capture into a bank that is NOT in LCDC is a question
     * GBATEK did not settle for this lane. The conservative reading is taken --
     * the destination must be a block -- for three reasons: it is what the
     * doc's own framing of a "VRAM write block" says; nothing in this ROM ever
     * asks for the other behaviour deliberately; and the two readings differ
     * observably in exactly one place, a 32 KB corruption of texture memory
     * that no ROM can have been written to rely on. Said once on stderr so the
     * choice is visible in any run that meets it. */
    const uint8_t dcnt = *reinterpret_cast<volatile uint8_t *>(kVramCnt + block);
    if ((dcnt & 0x87) != 0x80) {
        if (!g_said_notlcdc) {
            g_said_notlcdc = 1;
            std::fprintf(stderr,
                         "  [capture] DISPCAPCNT %08x names VRAM block %u, "
                         "whose VRAMCNT is %02x: it is not allocated to LCDC, "
                         "so it is not a block right now and nothing is "
                         "captured into it. (Scene::ResetHardwareRegisters "
                         "writes 0x80000000 at every scene boot; that names "
                         "block 0, where this port maps texture slot 0.)\n",
                         cap, block, dcnt);
            std::fflush(stderr);
        }
        return;
    }

    /* bits 20-21. Width is 256 except at size 0, which is the square one. */
    static const int kW[4] = {128, 256, 256, 256};
    static const int kH[4] = {128, 64, 128, 192};
    const unsigned size = (cap >> 20) & 3;
    const int cw = kW[size], ch = kH[size];

    const uint32_t off = ((cap >> 18) & 3) * 0x8000u;
    const uint32_t need = (uint32_t)cw * (uint32_t)ch * 2u;
    if (off + need > kBankSize) return;      // would run off the end of the bank

    /* NEAREST, not averaged; see the note in ntr/ppu.h. The ratio is a whole
       number at every tier the port builds. */
    const int rx = w / SUB_W, ry = h / SUB_H;
    uint16_t *dst = reinterpret_cast<uint16_t *>(lcdc_addr(block) + off);
    for (int y = 0; y < ch; ++y) {
        const int sy = ry > 0 ? y * ry : (y * h) / SUB_H;
        /* w/h are the LIVE image extent; the framebuffer's row stride is always
           SCREEN_W (the buffer max), which equals w on every fixed tier and is
           1024 on NTR_WIDE_RT with a 512-wide active image. Index by the stride,
           sample within the extent. */
        const uint32_t *row = src + (size_t)(sy < h ? sy : h - 1) * (size_t)SCREEN_W;
        for (int x = 0; x < cw; ++x) {
            const int sx = rx > 0 ? x * rx : (x * w) / SUB_W;
            const uint32_t p = row[sx < w ? sx : w - 1];
            /* ARGB8888 -> BGR555, and BIT 15 SET. On hardware the captured
               alpha bit says "this pixel was opaque"; engine A's finished
               framebuffer is opaque everywhere -- it starts from the scene's
               own clear colour -- so the bit is 1 for every captured pixel
               rather than derived from a channel the host framebuffer does not
               carry a meaning for. */
            dst[(size_t)y * cw + x] =
                (uint16_t)(0x8000u | (((p >> 3) & 0x1Fu) << 10) |
                           (((p >> 11) & 0x1Fu) << 5) | ((p >> 19) & 0x1Fu));
        }
    }
    g_cap[block].off = off;
    g_cap[block].len = need;
}

}  // namespace ntr
