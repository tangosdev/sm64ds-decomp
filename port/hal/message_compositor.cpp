// Engine-A 2D-over-3D compositor, host side.
//
// WHAT THE DS DOES AND THE PORT DID NOT. The top screen is engine A. Its 2D
// unit (four tile BGs, the OBJ sprite layer, the window unit) composites over
// the 3D engine's output in hardware every frame: 3D is effectively "BG0 in 3D
// mode", and the 2D BGs and sprites draw over it by priority. walk_window
// rendered ONLY the 3D frame to the top-screen framebuffer and never scanned
// engine A's 2D layers, so anything the game drew into engine A's 2D -- the
// dialogue box (BG3 + the cursor OBJ) above all -- was invisible.
//
// port/ntr/ppu.cpp already has the raster (ppu_scanout / ppu_scanout_obj), but
// ppu_scanout OVERWRITES the whole framebuffer with a backdrop-filled 2D image,
// which would erase the 3D scene. This file is the COMPOSITE form: it walks the
// same engine-A register state and writes ONLY the pixels a 2D layer actually
// covers, leaving the 3D frame showing everywhere the 2D is transparent. That
// is exactly what the DS's layer compositor produces.
//
// WHAT IT IMPLEMENTS (faithful to the dialogue box's needs):
//   - the engine-A text BGs, by priority, lower priority in front -- BG0
//     EXCLUDED whenever DISPCNT bit 3 makes it the 3D layer rather than a tile
//     background, which is the rule read_bg's own note derives and measures;
//   - the window unit (WIN0/WIN1, WININ/WINOUT), because the box uses WIN0 to
//     reveal BG3 as it animates open -- without it the box tilemap would show
//     full-screen the instant it is enabled;
//   - the engine-A OBJ layer (the message cursor arrows), composited last;
//   - the DS 256x192 space mapped to the host framebuffer by integer scale.
//
// WHAT IT IMPLEMENTS (blend), added run link60 lane BLND1:
//   - BLDCNT ALPHA (mode 1) and the always-on semi-transparent-OBJ alpha, which
//     is what dScMgCurling_c needs: its 0x0d40 puts no first-target BG bits up,
//     so the effect fires ONLY through the mode-1 blue shadow sprite (always a
//     1st target) alpha-blending over BG2 (a 2nd target) with BLDALPHA 0x1002.
//     A top BG that is a 1st target over a 2D layer that is a 2nd target blends
//     too. The window colour-effect bit (bit 5 of the window masks) gates it.
//     SM64DS_BLEND_OFF=1 restores the old opaque path for A/B and bisection.
//
// WHAT IT SKIPS (documented, none of it on the dialogue-box path):
//   - BLDY BRIGHTNESS (BLDCNT mode 2/3): owned by the fade path. This
//     compositor runs BEFORE walk_window's fade composite, which reads engine
//     A's BLDCNT/BLDY (port_fader_blend_state) and dims the whole framebuffer;
//     applying mode 2/3 here too would double the fade. Recognised and deferred.
//   - a 1st-target BG blending over the 3D layer (BG0 in 3D mode) rather than
//     over another 2D layer: the BG loop composites into a 2D hit buffer and
//     the 3D framebuffer is only read at OBJ time and at the final blit, so a
//     BG-over-3D alpha would need the framebuffer inside the BG loop. The
//     semi-transparent OBJ path DOES blend over the 3D layer. No curling frame
//     needs BG-over-3D (its first-target bits are empty); left as a follow-up.
//   - affine and bitmap BGs (engine A's box is text-mode);
//   - mosaic.
//
// The register/VRAM addresses are engine A's, cross-checked against ppu.cpp
// (same source of truth: the decomp's own G2::GetBG*Ptr shifts) and ppu_sub.cpp
// (the window unit, mirrored here for engine A's 0x04000000 register base).
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "ntr/ppu.h"

namespace {

// Engine A only.
constexpr uint32_t kRegBase  = 0x04000000u;
constexpr uint32_t kVramBase = 0x06000000u;
constexpr uint32_t kPlttBase = 0x05000000u;
constexpr uint32_t kOamBase  = 0x07000000u;
constexpr uint32_t kObjVram  = 0x06400000u;

inline uint16_t rd16(uint32_t a) { return *reinterpret_cast<volatile uint16_t *>(a); }
inline uint32_t rd32(uint32_t a) { return *reinterpret_cast<volatile uint32_t *>(a); }

inline uint32_t bgr555(uint16_t c) {
    const uint32_t r = (c >> 0) & 0x1F, g = (c >> 5) & 0x1F, b = (c >> 10) & 0x1F;
    return 0xFF000000u | ((r << 3 | r >> 2) << 16) | ((g << 3 | g >> 2) << 8)
           | (b << 3 | b >> 2);
}

struct BgConfig {
    bool enabled;
    int priority;
    uint32_t screen;   // tilemap address
    uint32_t chars;    // tile data address
    bool bpp8;
    int tiles_w, tiles_h;
    int hofs, vofs;
};

// Engine A's DISPCNT bit 3 is the BG0 2D/3D SELECTION, and it is not a setting
// that modifies a tile background -- it replaces one. With bit 3 set, BG0 is the
// 3D engine's output layer, BG0CNT's character and screen bases address nothing
// the game ever wrote, and the 2D unit never reads them.
//
// THE DECOMP'S OWN CODE PINS THE BIT. src/_ZN2GX15SetGraphicsModeEiii.c is
// GX::SetGraphicsMode(dispMode, bgMode, bg0_3d) and its whole use of the third
// argument is `reg = ((unsigned)c << 3) | reg`. ntr/ppu.cpp's OBJ note already
// says the same thing from the other side ("its third argument only ever
// reaches bit 3"), and ppu_audit.cpp's 3D section already prints the rule as
// prose: "DISPCNT bit 8 (BG0 enable) plus bit 3 (BG0 in 3D mode) is what puts
// this layer in the picture at all." Nothing implemented it.
//
// WHAT NOT IMPLEMENTING IT COST, measured on scene 390 (dScMgFlower_c, the
// "Loves Me...?" minigame), run mg5 lane YTILE. The scene runs with
// DISPCNT = 0x00001508 -- bit 3 SET, BG0/BG2/OBJ enabled -- and BG0CNT = 0x0001,
// so this reader took BG0 for a 4bpp text background with BOTH its tilemap and
// its tiles at 0x06000000, which this scene never writes, at priority 1: in
// FRONT of the BG2 artwork at priority 3. It decoded whatever bytes an earlier
// load had left at the bottom of main BG VRAM into 8x8 blocks and painted 3634
// of the top screen's 49152 pixels with them.
//
// The damage is measurable against the cartridge rather than against taste. A
// reference image built straight from the ROM's own files -- the LZ77 decode of
// /MG/d_2d_mg_bg_flower_up_ncg.bin and _nsc.bin through the palette
// _ncl.bin loads at byte offset 0x60 -- matches a BG2-only capture on
// 49152 of 49152 pixels, and matches the full frame on 44736 (91.0%), with 55
// tiles of 768 wrong in all 64 of their pixels. Compositing BG2 with each other
// layer in turn (SM64DS_ENGINE_A_LAYERS) attributes every one of those 55 tiles
// to BG0 and none to BG1 or BG3.
//
// SCENE 390 IS NOT THE ONLY ONE. Sampled with SM64DS_PPU_AUDIT: scene 1 (the
// title) runs DISPCNT 0x00011b08 and scene 4 runs 0x08011d08, both with bit 3
// set and BG0 enabled, so both were painting the same fabricated layer. Scenes
// 366, 368, 374 and 378 leave BG0 disabled, and every mounted LEVEL runs
// DISPCNT 0x40011803 -- bit 3 clear, BG0 disabled -- so no level frame changes.
//
// WHAT THIS DOES NOT FIX, said plainly so nobody reads more into it. Skipping
// BG0 stops the port INVENTING a layer; it does not put the real 3D layer in
// its place. The port renders 3D into the framebuffer before this compositor
// runs and the compositor only writes over it, so a 2D BG that on hardware sits
// BEHIND the 3D layer (a higher priority number than BG0's) still covers it
// here. On scene 390 that is BG2 at priority 3 under the 3D layer at priority 1.
// Fixing that needs a 3D coverage mask the framebuffer does not carry, it is the
// same gap the header's "1st-target BG blending over the 3D layer" note already
// records, and it is not this change.
inline bool bg0_is_3d(uint32_t dispcnt) { return (dispcnt & 0x8u) != 0; }

BgConfig read_bg(int bg, uint32_t dispcnt) {
    BgConfig c{};
    c.enabled = (dispcnt >> (8 + bg)) & 1;
    if (bg == 0 && bg0_is_3d(dispcnt)) c.enabled = false;
    if (!c.enabled) return c;
    const uint16_t cnt = rd16(kRegBase + 0x08 + bg * 2);
    c.priority = cnt & 3;
    c.bpp8 = (cnt >> 7) & 1;
    // Engine A has the DISPCNT char/screen base offsets (bits 24-26 / 27-29).
    const uint32_t char_off = ((dispcnt >> 24) & 7) << 16;
    const uint32_t scr_off = ((dispcnt >> 27) & 7) << 16;
    c.screen = kVramBase + scr_off + (((cnt >> 8) & 0x1F) << 11);
    c.chars = kVramBase + char_off + (((cnt & 0x3c) >> 2) << 14);
    const unsigned sz = (cnt >> 14) & 3;
    c.tiles_w = (sz & 1) ? 64 : 32;
    c.tiles_h = (sz & 2) ? 64 : 32;
    c.hofs = rd16(kRegBase + 0x10 + bg * 4) & 0x1FF;
    c.vofs = rd16(kRegBase + 0x12 + bg * 4) & 0x1FF;
    return c;
}

inline uint32_t screen_entry_addr(const BgConfig &c, int tx, int ty) {
    const int bx = tx >> 5, by = ty >> 5;
    const int blocks_w = c.tiles_w >> 5;
    const int block = by * blocks_w + bx;
    return c.screen + block * 0x800 + (((ty & 31) * 32 + (tx & 31)) * 2);
}

// false where the BG is transparent at this pixel.
bool sample_bg(const BgConfig &c, int x, int y, uint32_t &out) {
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
    out = bgr555(rd16(kPlttBase + index * 2));
    return true;
}

// ---- the window unit (engine A, register base 0x04000000) ------------------
// Mirror of ppu_sub.cpp's read_windows/window_mask. Bits 0..3 BG0..BG3, bit 4
// OBJ, bit 5 colour effect. Edges are exclusive; a right/bottom at or before
// the left/top means "to end of line" (a zero register = full width).
struct Windows {
    bool any;
    int x1[2], x2[2], y1[2], y2[2];
    bool on[2];
    unsigned in[2], out;
};

void read_windows(uint32_t dispcnt, Windows &w) {
    w.on[0] = (dispcnt >> 13) & 1;
    w.on[1] = (dispcnt >> 14) & 1;
    w.any = w.on[0] || w.on[1];
    if (!w.any) return;
    for (int i = 0; i < 2; ++i) {
        const uint16_t hh = rd16(kRegBase + 0x40 + i * 2);
        const uint16_t vv = rd16(kRegBase + 0x44 + i * 2);
        w.x1[i] = hh >> 8;
        w.x2[i] = hh & 0xFF;
        w.y1[i] = vv >> 8;
        w.y2[i] = vv & 0xFF;
        if (w.x2[i] <= w.x1[i]) w.x2[i] = 256;
        if (w.y2[i] <= w.y1[i]) w.y2[i] = 192;
    }
    const uint16_t winin = rd16(kRegBase + 0x48);
    w.in[0] = winin & 0x3F;
    w.in[1] = (winin >> 8) & 0x3F;
    w.out = rd16(kRegBase + 0x4A) & 0x3F;
}

inline unsigned window_mask(const Windows &w, int x, int y) {
    if (!w.any) return 0x3F;
    for (int i = 0; i < 2; ++i)
        if (w.on[i] && x >= w.x1[i] && x < w.x2[i] && y >= w.y1[i] && y < w.y2[i])
            return w.in[i];
    return w.out;
}

// ---- the colour special-effects unit (BLDCNT, engine A) --------------------
// Alpha only (mode 1) plus the always-on semi-transparent-OBJ alpha; brightness
// modes 2/3 are the fade path's (see the header note). Mirror of ppu_sub.cpp's
// unit for engine A's 0x04000050 blend registers. Layer ids match the window
// bits: 0..3 BG, 4 OBJ (kOwnerObj below).

// SM64DS_BLEND_OFF=1 keeps the old opaque path, so a before/after is one binary
// at one .dsstate base (notes/port-selftest-bmp-gate.md) and a regression can
// be bisected against the pre-blend image.
inline bool blend_off_env() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_BLEND_OFF");
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v != 0;
}

struct Blend {
    bool off;
    int mode;          // BLDCNT bits 6-7: 0 off, 1 alpha, 2 bright-up, 3 down
    unsigned first;    // bits 0-5: 1st-target layers
    unsigned second;   // bits 8-13: 2nd-target layers
    int eva, evb;      // BLDALPHA: 1st/2nd coefficients, 0..16 in 1/16 steps
};

inline Blend read_blend() {
    Blend b;
    b.off = blend_off_env();
    const uint16_t cnt = rd16(kRegBase + 0x50);
    const uint16_t alpha = rd16(kRegBase + 0x52);
    b.mode = (cnt >> 6) & 3;
    b.first = cnt & 0x3F;
    b.second = (cnt >> 8) & 0x3F;
    b.eva = alpha & 0x1F; if (b.eva > 16) b.eva = 16;
    b.evb = (alpha >> 8) & 0x1F; if (b.evb > 16) b.evb = 16;
    return b;
}

// top*eva/16 + below*evb/16 per channel, in 5-bit clamped to 31 -- the DS's own
// arithmetic. bgr555 expansion (v<<3|v>>2) is losslessly reversible by >>3, so
// the round trip reproduces the hardware blend exactly.
inline uint32_t blend_alpha(uint32_t top, uint32_t below, int eva, int evb) {
    const int r1 = ((top >> 16) & 0xFF) >> 3, g1 = ((top >> 8) & 0xFF) >> 3,
              b1 = (top & 0xFF) >> 3;
    const int r2 = ((below >> 16) & 0xFF) >> 3, g2 = ((below >> 8) & 0xFF) >> 3,
              b2 = (below & 0xFF) >> 3;
    int r = (r1 * eva + r2 * evb) >> 4; if (r > 31) r = 31;
    int g = (g1 * eva + g2 * evb) >> 4; if (g > 31) g = 31;
    int b = (b1 * eva + b2 * evb) >> 4; if (b > 31) b = 31;
    return 0xFF000000u | ((uint32_t)(r << 3 | r >> 2) << 16)
                       | ((uint32_t)(g << 3 | g >> 2) << 8)
                       | (uint32_t)(b << 3 | b >> 2);
}

// The DS 256x192 image the 2D unit produces, per pixel: colour, "did any 2D
// layer write here", and WHICH layer wrote it last.
//
// THE OWNER BYTE IS THE MEASUREMENT HALF, and it exists because a top-screen
// frame can be wrong in two unrelated ways -- the backgrounds decode wrong, or
// the sprites do -- and one BMP cannot tell them apart. Both layers land in the
// same pixels and the picture is their sum, so "the frame is scrambled" names
// no surface at all. Recording the last writer costs one byte per pixel and
// turns that report into a question about a named layer.
struct Cell { uint32_t color; bool hit; uint8_t owner; };
Cell g_a[192][256];

// Owner ids: 0-3 are BG0..BG3 and 4 is the sprite layer. The same numbering is
// the bit position in the layer switch below, so a mask and an attribution row
// read against each other without a translation table.
enum { kOwnerObj = 4, kOwnerN = 5 };
const char *const kOwnerName[kOwnerN] = {"BG0", "BG1", "BG2", "BG3", "OBJ"};

// SM64DS_ENGINE_A_LAYERS=<hex>: composite only these layers. 0x04 is BG2 alone,
// 0x10 the sprites alone, and unset is 0x1f -- every layer, which is what every
// gate step and every ordinary run does. A capture taken with one bit set is
// that layer's own image off the SAME binary at the SAME .dsstate base as the
// full frame, which is what notes/port-selftest-bmp-gate.md requires before two
// BMPs may be compared at all. It is a diagnostic filter and not a state
// change: the register reads, the early return and the window unit all still
// see what the game actually programmed.
unsigned layer_mask_env() {
    static int m = -1;
    if (m < 0) {
        const char *e = std::getenv("SM64DS_ENGINE_A_LAYERS");
        m = (e && *e) ? (int)(std::strtoul(e, nullptr, 16) & 0x1F) : 0x1F;
    }
    return (unsigned)m;
}

// The LAST composited frame's attribution, kept whole rather than accumulated.
// The BMP a run writes is the last frame, so the numbers a capture is read
// against have to be that frame's own and not a 300-frame sum.
struct Attrib { uint32_t px; int x0, x1, y0, y1; };
Attrib g_attrib[kOwnerN];
uint32_t g_attrib_frames;
bool g_attrib_registered;

void attrib_dump() {
    if (!g_attrib_frames) return;
    std::fprintf(stderr, "[msgcomp] LAST COMPOSITED FRAME, per-layer "
                 "attribution over %u composited frame(s), layer mask %02x\n",
                 g_attrib_frames, layer_mask_env());
    for (int o = 0; o < kOwnerN; ++o) {
        const Attrib &a = g_attrib[o];
        if (!a.px) {
            std::fprintf(stderr, "[msgcomp]   %-3s       0 px\n", kOwnerName[o]);
            continue;
        }
        std::fprintf(stderr, "[msgcomp]   %-3s  %6u px  x %d..%d  y %d..%d\n",
                     kOwnerName[o], a.px, a.x0, a.x1, a.y0, a.y1);
    }
}

// Counts what each layer OWNS in the frame just composited. A pixel belongs to
// the layer that wrote it last, which is the layer the viewer sees, so the
// rows sum to the composited-pixel count rather than to the coverage of each
// layer taken alone. Run one layer at a time (the switch above) for that.
void attrib_take() {
    if (!g_attrib_registered) {
        g_attrib_registered = true;
        std::atexit(attrib_dump);
    }
    for (int o = 0; o < kOwnerN; ++o) {
        g_attrib[o].px = 0;
        g_attrib[o].x0 = 256; g_attrib[o].x1 = -1;
        g_attrib[o].y0 = 192; g_attrib[o].y1 = -1;
    }
    for (int y = 0; y < 192; ++y) {
        for (int x = 0; x < 256; ++x) {
            if (!g_a[y][x].hit) continue;
            Attrib &a = g_attrib[g_a[y][x].owner];
            ++a.px;
            if (x < a.x0) a.x0 = x;
            if (x > a.x1) a.x1 = x;
            if (y < a.y0) a.y0 = y;
            if (y > a.y1) a.y1 = y;
        }
    }
    ++g_attrib_frames;
}

// ---- OBJ (sprites): the cursor arrows -------------------------------------
// Engine-A OAM at 0x07000000, OBJ VRAM at 0x06400000, OBJ palette pltt+0x200.
// Composites into g_a (only non-transparent texels), on top of the BGs, same
// as the DS at equal-or-lower priority. Plain and affine, 16/256-colour, and
// BOTH tile mappings -- the same subset ppu_scanout_obj implements.
//
// IT SAID "1D MAPPING" HERE AND MEANT IT: the slot arithmetic was
// trow * (w/8) + tcol with no reference to DISPCNT bit 4, and this is engine
// A's LIVE path, so every multi-row sprite on every scene that reaches this
// compositor read its second row of cells from the wrong place. ppu_audit.cpp's
// own DISPCNT note recorded the gap against ntr/ppu.cpp, which is the file with
// the same arithmetic and NO live caller; the note was right about the
// arithmetic and pointed at the copy that cannot reach a screen. Both are
// fixed, in ntr/ppu_sub.cpp's shape.
void raster_obj(uint32_t dispcnt, const Blend &bl, const Windows &win,
                ntr::Framebuffer &fb) {
    static const int kSizes[3][4][2] = {
        {{8, 8}, {16, 16}, {32, 32}, {64, 64}},
        {{16, 8}, {32, 8}, {32, 16}, {64, 32}},
        {{8, 16}, {8, 32}, {16, 32}, {32, 64}},
    };
    const uint32_t obj_pltt = kPlttBase + 0x200u;
    const uint32_t boundary = 32u << ((dispcnt >> 20) & 3);
    // Integer scale DS 256x192 -> host, for reading the 3D pixel under a
    // semi-transparent sprite where no 2D layer covers it. Named distinctly
    // from the sx/sy sprite-walk counters below, which would otherwise shadow.
    const int xscale = ntr::SCREEN_W / 256;
    const int yscale = ntr::SCREEN_H / 192;
    // DISPCNT bit 4: 1 = one-dimensional tile mapping, 0 = two-dimensional.
    // Scene::ResetHardwareRegisters clears it on both engines (it ANDs
    // 0xffcfffef, which takes bit 4 and the boundary field together) and
    // nothing on a scene path sets it again, so 2D is the mode this compositor
    // meets. Measured on scene 374, DISPCNT_A 00001400 / 00007400 on all 300
    // samples. Read rather than assumed, because the ONE test that drives this
    // raster -- tests/smoke_oam.cpp -- writes DISPCNT = 1 << 4 and lays its
    // tiles out consecutively, so its expectations are 1D expectations and a
    // flipped default would have broken them instead of honouring the bit.
    const bool map1d = (dispcnt >> 4) & 1;

    for (int i = 127; i >= 0; --i) {
        const uint16_t a0 = rd16(kOamBase + i * 8u);
        const uint16_t a1 = rd16(kOamBase + i * 8u + 2);
        const uint16_t a2 = rd16(kOamBase + i * 8u + 4);
        const bool affine = a0 & 0x100;
        if (!affine && (a0 & 0x200)) continue;
        // OBJ MODE (attribute 0 bits 10-11): 0 normal, 1 semi-transparent,
        // 2 OBJ window, 3 prohibited/bitmap. This test used to be `== 3` under
        // the name "OBJ window", which skipped the wrong mode and painted a
        // window sprite -- invisible on hardware, it contributes only a mask --
        // as an ordinary opaque sprite. Corrected in step with ntr/ppu.cpp and
        // ntr/ppu_sub.cpp; the field position is pinned there from the decomp's
        // own OAM::Render rather than from docs alone.
        //
        // THE MASK ITSELF IS NOT MODELLED HERE, and that is a structural limit
        // rather than an oversight: this compositor runs its BG loop BEFORE
        // raster_obj, so an OBJ-window region would not exist yet when the BGs
        // are masked. ppu_sub.cpp composites in one pass and does model it.
        // Skipping the sprite is still strictly closer to hardware than drawing
        // it, because on hardware it never appears in the image.
        const unsigned objmode = (a0 >> 10) & 3;
        if (objmode == 2 || objmode == 3) continue;
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
            pa = (int16_t)rd16(kOamBase + (grp * 4 + 0) * 8u + 6);
            pb = (int16_t)rd16(kOamBase + (grp * 4 + 1) * 8u + 6);
            pc = (int16_t)rd16(kOamBase + (grp * 4 + 2) * 8u + 6);
            pd = (int16_t)rd16(kOamBase + (grp * 4 + 3) * 8u + 6);
        }

        for (int sy = 0; sy < bh; ++sy) {
            const int py = y + sy;
            if (py < 0 || py >= 192) continue;
            for (int sx = 0; sx < bw; ++sx) {
                const int px = x + sx;
                if (px < 0 || px >= 256) continue;
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
                // The cell's slot from the sprite's base, in 32-byte units.
                // 1D: consecutive, a 256-colour cell taking two. 2D: a
                // 32-slot-wide matrix, a 256-colour cell two slots wide.
                const uint32_t slot =
                    map1d ? (uint32_t)(trow * (w / 8) + tcol) * (c256 ? 2u : 1u)
                          : (uint32_t)(trow * 32 + (c256 ? tcol * 2 : tcol));
                const uint32_t cell = kObjVram + tile * boundary + slot * 32u;
                uint32_t index, color;
                if (c256) {
                    index = *reinterpret_cast<volatile uint8_t *>(cell + fy * 8u + fx);
                    if (!index) continue;
                    color = bgr555(rd16(obj_pltt + index * 2u));
                } else {
                    const uint8_t b =
                        *reinterpret_cast<volatile uint8_t *>(cell + fy * 4u + fx / 2);
                    index = (fx & 1) ? (b >> 4) : (b & 0xF);
                    if (!index) continue;
                    color = bgr555(rd16(obj_pltt + (pal * 16u + index) * 2u));
                }
                // OBJ mode 1 (semi-transparent) alpha-blends with the layer
                // directly below it -- the BG already composited into g_a here,
                // or the 3D framebuffer (BG0 in 3D mode) where no 2D covers the
                // pixel -- whatever BLDCNT's first-target bits say. Window bit 5
                // gates the effect. No 2nd target below: drawn opaque (hardware).
                if (!bl.off && objmode == 1
                    && (window_mask(win, px, py) & 0x20)) {
                    if (g_a[py][px].hit) {
                        if (bl.second & (1u << g_a[py][px].owner))
                            color = blend_alpha(color, g_a[py][px].color,
                                                bl.eva, bl.evb);
                    } else if (bl.second & 1u) {   // BG0 (3D) is a 2nd target
                        color = blend_alpha(color, fb.px[py * yscale][px * xscale],
                                            bl.eva, bl.evb);
                    }
                }
                g_a[py][px].color = color;
                g_a[py][px].hit = true;
                g_a[py][px].owner = kOwnerObj;
            }
        }
    }
}

}  // namespace

/* func_02019144's FIRST beat, hal/scene_boot.cpp. 1 = run that function's
   tail, 0 = the current graphics block did the display sync itself. */
extern "C" int port_graph_block_beat(void);

// Rasterise engine A's 2D layers into the DS 256x192 hit buffer, then composite
// the covered pixels over the host 3D framebuffer (scaled by integer factor).
// Called from walk_window right after gx_render and before the fade composite.
extern "C" void port_message_composite_engine_a(void *fbp)
{
    ntr::Framebuffer &fb = *reinterpret_cast<ntr::Framebuffer *>(fbp);

    /* ---- func_02019144 LINE 46, THE ENGINE-A LAYER-MASK PUBLISH -----------
     *
     * On the DS the BG and OBJ enables are not written by the code that turns
     * a layer on. That code sets a SOFTWARE mask, data_0209d45c, and
     * func_02019144 copies it into DISPCNT bits 8-12 once per frame:
     *
     *     *(vu32 *)0x4000000 = (*(vu32 *)0x4000000 & ~0x1f00) | (d45c << 8);
     *
     * Other functions write that same expression -- Message's two, ov003's,
     * ov004's minigame enter/leave pair, ov007's -- but each of those is one
     * scene's or one subsystem's own moment. NO COUNT IS QUOTED HERE ON
     * PURPOSE: a first sweep for the literal `data_0209d45c << 8` answered
     * nine files and a statement-level sweep answered a different eight, so
     * the number is an artifact of the pattern and not a fact about the image.
     * What matters is not how many there are but that func_02019144 is the one
     * that runs EVERY FRAME FOR EVERYBODY, and the port does not run it.
     *
     * On this path that is measured rather than argued: the run's own slot
     * census shows ov004's framework slots 29 and 30, the two that publish on
     * the way into and out of a minigame, were never entered, and the mask
     * never reached the register on any of 300 frames.
     *
     * What the port had instead was hal/message_pump.cpp's
     * port_message_flush_engine_a_regs, which is this same line and returns
     * early unless a dialogue box is up -- so the enables reached the register
     * only on the one path that has a box.
     *
     * MEASURED, scene 374 (dScMgCurling_c), 300 frames: data_0209d45c = 0x14
     * (BG2 and OBJ) with DISPCNT bits 8-12 = 0x00 on every sample, while
     * BG2CNT held 0x1222 -- a full-screen 32x32 text background whose tiles,
     * tilemap and palette dScMgCurling_c::InitResources had loaded -- and
     * engine A's OAM held 53 placed sprites. The exit below is
     * `!any_bg && !obj_on`, so the compositor returned on frame 0 and on every
     * frame after it, and the top screen kept nothing but the host's clear
     * colour. Not a viewport fault, and the sharper form of that: the game
     * issues NO viewport command at all on this path (VIEWPORT_SETS 0 on all
     * 300 samples) and submits zero polygons, which for a 2D minigame is the
     * ROM's intent. An earlier draft of this comment said the engine "latched
     * a full-screen viewport on all 300 samples"; it latched gx_reset's
     * full-screen DEFAULT, because nothing overwrote it, and the two are not
     * the same claim.
     *
     * HERE rather than in a frame hook, for the same reason engine B's publish
     * sits where it does. hal_sub_screen_present (hal/sub_screen.cpp)
     * publishes data_0209d454 into DISPCNT_B and then calls ppu_scanout_sub
     * further down the SAME function, so engine B rasterises the mask it just
     * published. Named by function and not by line: the first cut of this
     * comment cited :706 and "twenty-five lines later" for a publish that is
     * at 717-718 and a call at 730, and a line number in a file another lane
     * owns rots on their next edit even when it starts out right. Putting the
     * engine-A publish at the head of the engine-A display path gives engine A
     * the same property; anywhere earlier in the frame and the composite below
     * would read a register one tick stale.
     *
     * SM64DS_ENGINE_A_NO_PUBLISH=1 puts the old behaviour back on this same
     * binary, so a before/after is one build and one .dsstate base -- which is
     * what notes/port-selftest-bmp-gate.md requires before two BMPs may be
     * compared at all.
     *
     * port_message_flush_engine_a_regs still writes the same expression while a
     * box is up. It is now redundant in its DISPCNT half and NOT removed here:
     * it is another lane's file, the write is idempotent, and its BG3-offset
     * half is still the only thing flushing that register. Recorded rather than
     * cleaned up. */
    static int publish_off = -1;
    if (publish_off < 0)
        publish_off = std::getenv("SM64DS_ENGINE_A_NO_PUBLISH") ? 1 : 0;
    /* func_02019144's FIRST beat, which gates everything after it. The line
       below is that function's line 46 and this is that function's line 39:
       the ROM asks the current graphics block whether it has already done the
       display sync itself, and RETURNS if it says yes. Run link60 Stage 5 lane
       TS1 measured scene 1 saying yes on every frame while the port published
       over the top of it. hal/scene_boot.cpp owns the dispatch and the rule
       about which blocks may be dispatched at all; port/ov007_seat.txt 5h is
       the derivation and the measurement.
       HERE, at the head of the engine A display path, for the reason the
       publish below is here: the rasteriser must read what the beat just
       wrote, and the beat writes DISPCNT, BGxCNT and OAM. The engine B half
       of the tail reads the same verdict inside hal_sub_screen_present. */
    const bool run_tail = port_graph_block_beat() != 0;
    {
        extern unsigned char data_0209d45c;
        if (!publish_off && run_tail)
            *reinterpret_cast<volatile uint32_t *>(kRegBase) =
                (rd32(kRegBase) & ~0x1F00u) | ((uint32_t)data_0209d45c << 8);
    }

    const uint32_t dispcnt = rd32(kRegBase);
    const unsigned disp_mode = (dispcnt >> 16) & 3;
    const bool forced_blank = (dispcnt >> 7) & 1;

    /* raw engine-A register dump before any early-return, so a headless run
       sees the real DISPCNT/BGxCNT whatever produced them even if no BG reads
       as enabled.
       IT USED TO FIRE ONLY WHILE A BOX WAS UP (`if (data_0209d660 && ...)`),
       and that gate made it blind to the case it is most needed for. The
       compositor's own exit below is `!any_bg && !obj_on`, which is DISPCNT
       bits 8-12 -- and the one thing that would explain a screen with nothing
       on it is those bits reading zero while data_0209d45c, the software layer
       mask the game maintains and func_02019144 copies into them once a frame,
       reads nonzero. A probe that cannot run without a dialogue box can never
       report that pair, because a minigame has no dialogue box.
       So it fires on the first frame either way and PRINTS BOTH SIDES: the
       mask the game computed and the register bits that were supposed to
       receive it. `boxed` keeps the old reading available rather than
       dropping it -- a line taken with no box up and a line taken with one are
       different measurements and the reader has to be able to tell. */
    if (std::getenv("SM64DS_MSG_COMPOSITE_DEBUG")) {
        extern unsigned char data_0209d660, data_0209d45c;
        static int dumped;
        if (!dumped) {
            dumped = 1;
            const unsigned mask = (unsigned)data_0209d45c;
            const unsigned live = (dispcnt >> 8) & 0x1F;
            std::fprintf(stderr, "[msgcomp] raw engA DISPCNT=%08x mode=%u fblank=%d "
                         "BG0CNT=%04x BG1CNT=%04x BG2CNT=%04x BG3CNT=%04x "
                         "d45c=%02x boxed=%u\n", dispcnt, disp_mode, forced_blank,
                         rd16(kRegBase + 0x08), rd16(kRegBase + 0x0a),
                         rd16(kRegBase + 0x0c), rd16(kRegBase + 0x0e),
                         mask, (unsigned)data_0209d660);
            /* The bit-3 verdict, printed rather than inferred from BG0's
               absence in the per-BG dump below. A layer that is skipped and a
               layer that was never enabled look identical there, and the two
               are different facts about what the game asked for. */
            std::fprintf(stderr, "[msgcomp] DISPCNT bit 3 = %u: BG0 is the %s. "
                         "BG0 enable bit = %u\n", bg0_is_3d(dispcnt) ? 1u : 0u,
                         bg0_is_3d(dispcnt) ? "3D LAYER, not composited here "
                                              "(the 3D frame is already in the "
                                              "framebuffer)"
                                            : "an ordinary 2D text background",
                         (dispcnt >> 8) & 1u);
            /* THE VERDICT WORD IS NOT `mask == live`, and the first cut of
               this probe made exactly that mistake. Equal-and-zero is the
               common case on a level, where the mask is 0x00 and the register
               bits are 0x00 -- and a probe that answers "published" there
               asserts a copy that may never have executed. It said so on
               every level run of the NO_PUBLISH arm, which is the arm where
               the copy is switched OFF. The comparison cannot carry the claim
               because zero equals zero whoever wrote it, or nobody.
               So the publish path reports ITSELF (publish_off, the same latch
               the write above reads), and the comparison is only allowed to
               speak where it has content: equal AND nonzero.

               THE `NOT PUBLISHED` ARM IS NOW UNREACHABLE BY CONSTRUCTION, and
               it is kept anyway. With the publish live it runs immediately
               above the DISPCNT read below, so the two cannot disagree; with
               it disabled the first arm answers. It is a standing assertion
               rather than a live report: the day an edit puts anything between
               the publish and the read, or moves the publish out of this
               function, that arm starts printing and names what broke. The
               four arms were each exercised on a real run before this was
               written -- disabled and enabled, on scene 374 with a nonzero
               mask and on level 1 with a zero one. */
            const char *verdict =
                !run_tail
                    ? "publish SKIPPED: the current graphics block's slot 2 "
                      "answered 0, so func_02019144 would have returned before "
                      "its line 46 and the enables in the register are the "
                      "scene's own"
                : publish_off
                    ? "publish DISABLED by SM64DS_ENGINE_A_NO_PUBLISH; whatever "
                      "is in the register was put there by something else"
                    : (mask == 0 && live == 0)
                          ? "nothing to publish, mask is zero -- the equality "
                            "here proves nothing either way"
                          : (mask == live)
                                ? "published"
                                : "NOT PUBLISHED (func_02019144 line 46 is the "
                                  "ROM's per-frame copy)";
            std::fprintf(stderr, "[msgcomp] layer mask d45c=%02x vs DISPCNT "
                         "bits 8-12=%02x -- %s\n", mask, live, verdict);
        }
    }

    /* NOTE: we do NOT gate on DISPCNT display mode here. On the DS, engine A's
       display-mode field (bits 16-17) selects display off / graphics / VRAM /
       main-memory. The PORT renders 3D to engine A WITHOUT driving engine A's
       DISPCNT (the 3D path writes the framebuffer directly), so this field
       stays 0 ("display off") for the whole run even while the box setup
       (func_0201f32c) enables BG3 + WIN0 in the SAME register. Gating on mode
       here would drop every real box. Forced blank (bit 7) is honoured because
       the game does set it deliberately. The per-BG enable bits below are the
       real signal for what 2D to composite. */
    if (forced_blank)
        return;

    BgConfig bgs[4];
    bool any_bg = false;
    for (int i = 0; i < 4; ++i) { bgs[i] = read_bg(i, dispcnt); any_bg |= bgs[i].enabled; }
    const bool obj_on = (dispcnt >> 12) & 1;
    if (!any_bg && !obj_on)
        return;   // nothing 2D enabled: the box is not up, leave the 3D frame

    Windows win;
    read_windows(dispcnt, win);

    // Clear the hit buffer, then BGs by priority (lower priority drawn last so
    // it wins), honouring the per-pixel window mask.
    for (int y = 0; y < 192; ++y)
        for (int x = 0; x < 256; ++x) g_a[y][x].hit = false;

    const unsigned lmask = layer_mask_env();
    const Blend bl = read_blend();

    for (int y = 0; y < 192; ++y) {
        for (int x = 0; x < 256; ++x) {
            const unsigned mask = window_mask(win, x, y);
            for (int prio = 3; prio >= 0; --prio) {
                for (int bg = 3; bg >= 0; --bg) {
                    if (!bgs[bg].enabled || bgs[bg].priority != prio) continue;
                    if (!(mask & (1u << bg))) continue;
                    if (!(lmask & (1u << bg))) continue;
                    uint32_t s;
                    if (sample_bg(bgs[bg], x, y, s)) {
                        // BGs draw far->near (prio 3..0, then bg 3..0), so when
                        // a 1st-target BG writes, g_a already holds the layer
                        // directly below it. Mode-1 alpha over a 2nd-target 2D
                        // layer, gated by window bit 5.
                        if (!bl.off && bl.mode == 1 && (mask & 0x20)
                            && (bl.first & (1u << bg)) && g_a[y][x].hit
                            && (bl.second & (1u << g_a[y][x].owner)))
                            s = blend_alpha(s, g_a[y][x].color, bl.eva, bl.evb);
                        g_a[y][x].color = s;
                        g_a[y][x].hit = true;
                        g_a[y][x].owner = (uint8_t)bg;
                    }
                }
            }
        }
    }

    if (obj_on && (lmask & (1u << kOwnerObj)))
        raster_obj(dispcnt, bl, win, fb);

    if (std::getenv("SM64DS_MSG_COMPOSITE_DEBUG"))
        attrib_take();

    // One-shot diagnostic (SM64DS_MSG_COMPOSITE_DEBUG): the engine-A 2D state
    // the frame the compositor first finds any 2D enabled, so a headless run
    // can confirm the box BG is on and how many pixels it covers.
    if (std::getenv("SM64DS_MSG_COMPOSITE_DEBUG")) {
        int hits = 0;
        for (int y = 0; y < 192; ++y)
            for (int x = 0; x < 256; ++x) if (g_a[y][x].hit) ++hits;
        extern unsigned char data_0209d6bc;   /* Message::Update state */
        static int maxhits = -1;
        /* Print each time the composited-pixel count reaches a new maximum OR
           the box reaches its settled prompt/idle state (d6bc >= 4), so a
           headless run sees the box grow, its peak coverage, and the tilemap at
           the settled frame -- plus BG config and the window rect. */
        static int dumped_settled;
        const bool settled = (data_0209d6bc >= 4 && !dumped_settled);
        if (settled) dumped_settled = 1;
        if ((any_bg || obj_on) && (hits > maxhits || settled)) {
            if (hits > maxhits) maxhits = hits;
            std::fprintf(stderr, "[msgcomp] engine A: DISPCNT=%08x bg_en=%d%d%d%d "
                         "obj=%d win=%d(x0 %d..%d y0 %d..%d in0=%02x out=%02x), "
                         "composited %d/%d px\n", dispcnt,
                         bgs[0].enabled, bgs[1].enabled, bgs[2].enabled,
                         bgs[3].enabled, obj_on, win.any,
                         win.any ? win.x1[0] : 0, win.any ? win.x2[0] : 0,
                         win.any ? win.y1[0] : 0, win.any ? win.y2[0] : 0,
                         win.any ? win.in[0] : 0, win.any ? win.out : 0,
                         hits, 256 * 192);
            for (int b = 0; b < 4; ++b)
                if (bgs[b].enabled)
                    std::fprintf(stderr, "[msgcomp]   BG%d prio %d chars %08x "
                                 "screen %08x bpp%d %dx%d ofs(%d,%d)\n", b,
                                 bgs[b].priority, bgs[b].chars, bgs[b].screen,
                                 bgs[b].bpp8 ? 8 : 4, bgs[b].tiles_w, bgs[b].tiles_h,
                                 bgs[b].hofs, bgs[b].vofs);
            /* dump a slice of BG3's tilemap (nonzero screen entries) and whether
               its char data has any nonzero bytes, to see if the box content was
               uploaded and where the nonzero tiles sit on screen */
            if (bgs[3].enabled) {
                int nz = 0; uint32_t first = 0;
                for (int ty = 0; ty < 24 && nz < 8; ++ty)
                    for (int tx = 0; tx < 32; ++tx) {
                        uint16_t se = rd16(bgs[3].screen + (ty * 32 + tx) * 2);
                        if ((se & 0x3FF) != 0) {
                            if (!nz) std::fprintf(stderr, "[msgcomp]   BG3 nonzero tiles:");
                            if (nz < 8) std::fprintf(stderr, " (%d,%d)=t%u", tx, ty, se & 0x3FF);
                            ++nz; first = se & 0x3FF;
                        }
                    }
                std::fprintf(stderr, "\n[msgcomp]   BG3 tilemap nonzero entries (first 24 rows): %d\n", nz);
                /* is tile `first`'s char data nonzero? */
                int cnz = 0;
                for (int k = 0; k < 32; ++k)
                    if (*reinterpret_cast<volatile uint8_t *>(bgs[3].chars + first * 32 + k)) ++cnz;
                std::fprintf(stderr, "[msgcomp]   BG3 tile %u char nonzero bytes: %d/32\n", first, cnz);
                /* scan which tiles across the char region actually have data,
                   to locate where the uploaded glyphs/frame really live */
                int filled = 0, firstfilled = -1;
                for (int t = 0; t < 1024; ++t) {
                    int any = 0;
                    for (int k = 0; k < 32; ++k)
                        if (*reinterpret_cast<volatile uint8_t *>(bgs[3].chars + t * 32 + k)) { any = 1; break; }
                    if (any) { ++filled; if (firstfilled < 0) firstfilled = t; }
                }
                std::fprintf(stderr, "[msgcomp]   BG3 char region: %d/1024 tiles have data, first at t%d\n",
                             filled, firstfilled);
                /* what do the tilemap entries look like across the top rows
                   (where func_0201f32c writes the box frame starting at +0x42 =
                   row1 col1) -- is it all 895 or a mix incl. glyph tiles 0x200+? */
                for (int ry = 0; ry < 4; ++ry) {
                    std::fprintf(stderr, "[msgcomp]   BG3 row%d:", ry);
                    for (int tx = 0; tx < 20; ++tx)
                        std::fprintf(stderr, " %u", rd16(bgs[3].screen + (ry * 32 + tx) * 2) & 0x3FF);
                    std::fprintf(stderr, "\n");
                }
            }
        }
    }

    // Composite the covered 2D pixels over the 3D framebuffer. The window's OBJ
    // and colour-effect bits are already resolved above for BGs; the sprite
    // raster is drawn wherever OAM places it (the cursor is inside the box
    // window in practice). Integer scale from DS 256x192 to SCREEN_W x SCREEN_H.
    const int sx = ntr::SCREEN_W / 256;
    const int sy = ntr::SCREEN_H / 192;
    for (int y = 0; y < 192; ++y) {
        for (int x = 0; x < 256; ++x) {
            if (!g_a[y][x].hit) continue;
            const uint32_t c = g_a[y][x].color;
            for (int dy = 0; dy < sy; ++dy)
                for (int dx = 0; dx < sx; ++dx)
                    fb.px[y * sy + dy][x * sx + dx] = c;
        }
    }
}
