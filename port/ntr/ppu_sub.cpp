// The DS BOTTOM SCREEN, rasterised whole.
//
// ppu.cpp scans one engine's text backgrounds and one engine's sprites into a
// Framebuffer, which is SCREEN_W x SCREEN_H -- the tier the top screen is
// rendered at (512x384 in the window, 1024x768 in the hires clone). The bottom
// screen cannot use that. It is not a 3D scene that can be rendered at any
// resolution: it is a tile raster whose every address is a DS pixel, so the
// only size it exists at is 256x192. Scaling it is the compositor's job, not
// the rasteriser's.
//
// So this file is a second, complete engine-B compositor at fixed 256x192.
// "Complete" as against ppu.cpp's two independent passes: the DS resolves
// sprites and backgrounds together by priority, and the bottom screen is where
// that matters -- the minimap sits on BG3 at priority 2 and the HUD's sprites
// have to land either side of it depending on their own priority word.
//
// What is here and what is not:
//
//   - text BGs, affine BGs, and EXTENDED AFFINE with 16-bit map entries (the
//     mode the minimap runs in: 8bpp tiles, per-tile palette select, rotated
//     and scaled by the BG3 matrix).
//   - BG EXTENDED PALETTES. Minimap::InitResources loads its colours through
//     GXS::LoadBGExtPltt and nothing else, so without this the minimap is a
//     black rectangle. The base address is the one that function writes to,
//     0x06898000, and the slot stride is the 0x2000 its own 0x6000 argument
//     implies for BG3.
//   - OBJ EXTENDED PALETTES, DISPCNT bit 31, 256-colour sprites only. The star
//     select's character portrait is loaded through GXS::LoadOBJExtPltt and
//     nothing else on that screen is, so without this the portrait draws with
//     the right shape in the wrong colours while the star and the text beside
//     it are correct. Base 0x068a0000, from that function's own arithmetic.
//   - the WINDOW unit, all three windows: WIN0/WIN1 by rectangle and the OBJ
//     window by mask, with WININ/WINOUT deciding what shows in each region.
//   - MASTER BRIGHTNESS, which is how the sub screen fades.
//
//   - BLDCNT ALPHA BLENDING (mode 1) and the always-on semi-transparent-OBJ
//     alpha. The second-target machinery is here now: the per-pixel resolver
//     below already walks the layers by priority, so it resolves the TOP pixel
//     and the one directly BELOW it, and when the top is a 1st target and the
//     below a 2nd target it outputs top*eva/16 + below*evb/16 (BLDALPHA),
//     clamped, in 5-bit like the hardware. A mode-1 OBJ (semi-transparent) is
//     always a 1st target and always alpha-blends regardless of BLDCNT, which
//     is what makes dScMgCurling_c's 0x0440 light-blue shadow render. The
//     window colour-effect bit (bit 5 of the window masks) gates it per region.
//     SM64DS_BLEND_OFF=1 restores the old opaque path for A/B and bisection.
//   - NOT the BLDY brightness modes (BLDCNT mode 2/3). Those are owned by the
//     fade path: hal/fader_wipes.cpp writes BLDCNT mode 2/3 + BLDY and the
//     sub-screen fade is applied downstream (ppu_compose_stacked's evy, and the
//     corner panel inside walk_window's fade composite), on top of the master
//     brightness this file applies. Applying mode 2/3 here too would double the
//     fade, so this unit recognises them and defers. The game only ever writes
//     mode 2/3 for fades and mode 1 (alpha) for effects, so the split is clean.
//   - EXTENDED AFFINE BITMAP BGs, both arms: 256-colour and DIRECT COLOUR.
//     BGxCNT bit 7 in an extended-affine slot means bitmap rather than 256
//     colours, and this file used to refuse the whole arm. It is how the
//     display capture unit's frame reaches the bottom screen in the
//     dScMgD3DBase_c family: BG2CNT_B 0x4284, direct colour, bitmap base
//     0x8000, which is exactly where DISPCAPCNT 0x80360010 put the frame.
//   - BITMAP OBJs, OBJ mode 3, direct colour, both mapping arms. They are
//     the other half of the dual-screen 3D path: on the frames engine B
//     drives the UPPER screen the captured frame comes back as twelve
//     64x64 direct-colour sprites rather than as a BG.
//   - no mosaic.
//
// The OBJ window used to be listed here as missing, and the line said "mode 3
// sprites are skipped". BOTH halves were wrong: the OBJ window is mode 2, mode
// 3 is the prohibited/bitmap encoding, and skipping 3 meant window sprites were
// DRAWN. tests/smoke_objwin.cpp pins the corrected contract.
//
// Address arithmetic is taken from the decomp's own byte-verified getters
// rather than from docs wherever one exists -- G2S::GetBG0CharPtr computes
//
//     0x6200000 + ((BG0CNT_SUB & 0x3c) >> 2 << 14)
//
// which is a FOUR-bit character base field, not the two bits a GBA-shaped
// reader would use. ppu.cpp had two, which put every engine-B tile fetch at
// character base 0.

#include "ntr/ppu.h"

#include "ntr/ppu_audit.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace ntr {
namespace {

constexpr uint32_t kRegBase = 0x04001000u;   // engine B
constexpr uint32_t kVramBase = 0x06200000u;  // engine B BG VRAM
constexpr uint32_t kPlttBase = 0x05000400u;  // engine B BG palette
constexpr uint32_t kObjVram = 0x06600000u;   // engine B OBJ VRAM
constexpr uint32_t kObjPltt = 0x05000600u;   // engine B OBJ palette
constexpr uint32_t kOamBase = 0x07000400u;   // engine B OAM
/* see the note over oam_b in the sub OBJ raster */
uint32_t g_oam_src_b;

/* ENGINE A's four, for the gap band's peek pass alone -- nothing else in this
   file reads engine A. Named with the engine in them so a later reader cannot
   mistake one for the sub engine's above. The live engine-A raster is
   hal/message_compositor.cpp and has its own copies of these; ntr/ppu.cpp's
   kEngines table has them too and is not reachable from here without dragging
   that file's whole EngineMap in for two words. */
constexpr uint32_t kRegBaseA = 0x04000000u;
constexpr uint32_t kOamBaseA = 0x07000000u;
constexpr uint32_t kObjVramA = 0x06400000u;
constexpr uint32_t kObjPlttA = 0x05000200u;
// GXS::LoadBGExtPltt's own destination base; slots are 0x2000 apart (its
// caller passes 0x6000 for BG3).
constexpr uint32_t kBgExtPltt = 0x06898000u;
// GXS::LoadOBJExtPltt's own destination base, out of the ROM rather than a
// doc: src/_ZN3GXS14LoadOBJExtPlttEPKvjj.c computes its destination as
// `destSlotAddr + 0x068a0000`, and src/_ZN2GX23SetBankForSubOBJExtPlttEt.c is
// what puts a bank there (VRAMCNT_I = 0x83) and sets DISPCNT_B bit 31 in the
// same two lines. One OBJ extended palette is sixteen slots of 256 colours,
// so the slot stride is 0x200 and the whole store is 0x2000.
constexpr uint32_t kObjExtPltt = 0x068A0000u;

inline uint16_t rd16(uint32_t a) { return *reinterpret_cast<volatile uint16_t *>(a); }
inline uint32_t rd32(uint32_t a) { return *reinterpret_cast<volatile uint32_t *>(a); }
inline uint8_t rd8(uint32_t a) { return *reinterpret_cast<volatile uint8_t *>(a); }

inline uint32_t bgr555(uint16_t c) {
    const uint32_t r = (c >> 0) & 0x1F, g = (c >> 5) & 0x1F, b = (c >> 10) & 0x1F;
    return 0xFF000000u | ((r << 3 | r >> 2) << 16) | ((g << 3 | g >> 2) << 8)
           | (b << 3 | b >> 2);
}

// ---- backgrounds ------------------------------------------------------------

/* BG_BITMAP_* are the two shapes an EXTENDED AFFINE BG takes when BGxCNT
   bit 7 is set. GBATEK: in an extended-affine slot bit 7 chooses between
   16-bit bgmap entries (bit 7 = 0, the minimap's mode) and a BITMAP
   (bit 7 = 1), and inside the bitmap arm bit 2 chooses 256-colour (8 bits
   per pixel through the standard palette) from DIRECT COLOUR (16 bits per
   pixel, BGR555, bit 15 = opaque). Both go through the same affine matrix
   the map-based extended BG does; the only thing that changes is how a
   sampled (px, py) turns into a colour. */
enum BgKind { BG_OFF, BG_TEXT, BG_AFFINE, BG_EXT_AFFINE, BG_BITMAP_256,
              BG_BITMAP_DIRECT };

struct BgLayer {
    BgKind kind;
    int prio;
    uint32_t screen;   // map address
    uint32_t chars;    // tile data address
    bool bpp8;
    int map_w, map_h;  // text: tiles; affine: pixels
    bool wrap;         // affine display-area overflow
    int hofs, vofs;    // text scroll
    int pa, pb, pc, pd;
    int refx, refy;    // 8.8 fixed
    uint32_t ext;      // extended palette slot base, 0 = not in use
};

// The DS BG-mode table, minus the 3D column engine B does not have. Each entry
// is the kind of BG0..BG3 in that mode. Mode 6 (large bitmap) is engine A only
// and is left off.
const unsigned char kModeKinds[6][4] = {
    /* 0 */ {BG_TEXT, BG_TEXT, BG_TEXT, BG_TEXT},
    /* 1 */ {BG_TEXT, BG_TEXT, BG_TEXT, BG_AFFINE},
    /* 2 */ {BG_TEXT, BG_TEXT, BG_AFFINE, BG_AFFINE},
    /* 3 */ {BG_TEXT, BG_TEXT, BG_TEXT, BG_EXT_AFFINE},
    /* 4 */ {BG_TEXT, BG_TEXT, BG_AFFINE, BG_EXT_AFFINE},
    /* 5 */ {BG_TEXT, BG_TEXT, BG_EXT_AFFINE, BG_EXT_AFFINE},
};

void read_bg(BgLayer &c, int bg, uint32_t dispcnt) {
    c.kind = BG_OFF;
    if (!((dispcnt >> (8 + bg)) & 1))
        return;
    const unsigned mode = dispcnt & 7;
    if (mode > 5)
        return;
    BgKind kind = (BgKind)kModeKinds[mode][bg];

    const uint16_t cnt = rd16(kRegBase + 0x08 + bg * 2);
    c.prio = cnt & 3;
    c.bpp8 = (cnt >> 7) & 1;

    // FOUR bits of character base, 16K units -- G2S::GetBG0CharPtr's own
    // (v & 0x3c) >> 2 << 14. Engine B has no DISPCNT base offset.
    c.chars = kVramBase + (((cnt & 0x3c) >> 2) << 14);
    c.screen = kVramBase + (((cnt & 0x1f00) >> 8) << 11);

    const int sz = (cnt >> 14) & 3;
    /* THE BITMAP ARM. `c.bpp8` is BGxCNT bit 7, which in an extended-affine
       slot does not mean "256 colours" at all -- it means BITMAP -- and this
       used to refuse the whole arm ("selects a bitmap, which is not hosted").
       It is hosted now, because it is how the display capture unit's frame
       reaches the screen: the dScMgD3DBase_c family points BG2-B at the block
       it captured into last frame, with BG2CNT_B 0x4284 (bit 7 bitmap, bit 2
       direct colour, screen base 2, size 1).

       THE BASE IS THE SCREEN BASE IN 16K UNITS, not the 2K units a map-based
       BG uses. The arithmetic is what makes the whole reading of this path
       check out: 2 x 0x4000 is 0x8000, and DISPCAPCNT 0x80360010's destination
       is bank C offset 1, which is 0x8000 bytes into bank C -- the same bytes,
       once bank C is mapped to engine B's BG. Two registers written by two
       different functions agreeing to the byte is not a coincidence a reader
       gets to ignore.

       The sizes are the bitmap set, not the map set: 128x128, 256x256, 512x256
       and 512x512 pixels. */
    if (kind == BG_EXT_AFFINE && c.bpp8) {
        static const short kBmpW[4] = {128, 256, 512, 512};
        static const short kBmpH[4] = {128, 256, 256, 512};
        c.kind = (cnt & 0x04) ? BG_BITMAP_DIRECT : BG_BITMAP_256;
        c.screen = kVramBase + (((cnt & 0x1f00) >> 8) << 14);
        c.map_w = kBmpW[sz];
        c.map_h = kBmpH[sz];
        c.wrap = (cnt >> 13) & 1;
        const uint32_t ab = kRegBase + 0x20 + (bg - 2) * 0x10;
        c.pa = (int16_t)rd16(ab + 0);
        c.pb = (int16_t)rd16(ab + 2);
        c.pc = (int16_t)rd16(ab + 4);
        c.pd = (int16_t)rd16(ab + 6);
        c.refx = (int)(rd32(ab + 8) << 4) >> 4;
        c.refy = (int)(rd32(ab + 12) << 4) >> 4;
        /* NO EXTENDED PALETTE. DISPCNT bit 30 arms them for 256-colour
           TILE BGs; a bitmap has no per-tile palette field to select a slot
           with, and the direct-colour arm has no palette at all. */
        c.ext = 0;
        return;
    }
    if (kind == BG_TEXT) {
        c.map_w = (sz & 1) ? 64 : 32;
        c.map_h = (sz & 2) ? 64 : 32;
        c.hofs = rd16(kRegBase + 0x10 + bg * 4) & 0x1FF;
        c.vofs = rd16(kRegBase + 0x12 + bg * 4) & 0x1FF;
    } else {
        c.map_w = c.map_h = 128 << sz;   // 128 / 256 / 512 / 1024 pixels
        c.wrap = (cnt >> 13) & 1;
        // Affine registers: BG2 at 0x20, BG3 at 0x30 (engine-relative).
        const uint32_t ab = kRegBase + 0x20 + (bg - 2) * 0x10;
        c.pa = (int16_t)rd16(ab + 0);
        c.pb = (int16_t)rd16(ab + 2);
        c.pc = (int16_t)rd16(ab + 4);
        c.pd = (int16_t)rd16(ab + 6);
        // BGxX/BGxY are 28-bit signed 20.8 values.
        c.refx = (int)(rd32(ab + 8) << 4) >> 4;
        c.refy = (int)(rd32(ab + 12) << 4) >> 4;
        // Extended affine with 16-bit entries uses 8bpp tiles whatever bit 7
        // said; bit 7 selected bitmap-vs-map, and we rejected bitmap above.
        if (kind == BG_EXT_AFFINE)
            c.bpp8 = true;
    }

    // Extended palettes. DISPCNT bit 30 arms them; they apply to 256-colour
    // BGs only. BG0/BG1 pick slot 0/1 or 2/3 through BGxCNT bit 13; BG2/BG3
    // are hardwired to slots 2/3.
    c.ext = 0;
    if (((dispcnt >> 30) & 1) && c.bpp8) {
        const int slot = bg < 2 ? (bg + (((cnt >> 13) & 1) ? 2 : 0)) : bg;
        c.ext = kBgExtPltt + (uint32_t)slot * 0x2000u;
    }
    c.kind = kind;
}

inline uint32_t map_entry_addr(const BgLayer &c, int tx, int ty) {
    const int bx = tx >> 5, by = ty >> 5;
    const int blocks_w = c.map_w >> 5;
    return c.screen + (by * blocks_w + bx) * 0x800 + (((ty & 31) * 32 + (tx & 31)) * 2);
}

// One BG pixel. Returns false where the layer is transparent here.
bool sample_bg(const BgLayer &c, int x, int y, uint32_t &out) {
    int px, py;
    uint16_t se;
    int tile, fx, fy;

    /* ---- THE BITMAP ARMS, and they leave before any tile arithmetic ------
     *
     * A bitmap BG has no tiles, no map and no flip bits: the affine transform
     * lands directly on a pixel. Same matrix, same 8.8 fixed point, same
     * display-area-overflow rule as the map-based affine BGs above, so the
     * coordinate half is spelled once here and only the fetch differs. */
    if (c.kind == BG_BITMAP_256 || c.kind == BG_BITMAP_DIRECT) {
        px = (c.refx + c.pa * x + c.pb * y) >> 8;
        py = (c.refy + c.pc * x + c.pd * y) >> 8;
        if (c.wrap) {
            px &= c.map_w - 1;
            py &= c.map_h - 1;
        } else if (px < 0 || px >= c.map_w || py < 0 || py >= c.map_h) {
            return false;
        }
        const uint32_t at = (uint32_t)py * (uint32_t)c.map_w + (uint32_t)px;
        if (c.kind == BG_BITMAP_DIRECT) {
            /* BIT 15 IS THE ALPHA and it is a HARD transparency, not a blend
               factor: a direct-colour bitmap pixel with bit 15 clear is not
               drawn at all and whatever is behind it shows. The display
               capture unit sets the bit on every pixel it writes, so a
               captured frame is opaque everywhere -- which is what makes a
               captured screen look like a screen and not like a stencil. */
            const uint16_t v = rd16(c.screen + at * 2);
            if (!(v & 0x8000)) return false;
            out = bgr555(v);
            return true;
        }
        /* 256-colour bitmap: index 0 is transparent, the same rule every other
           paletted layer in this file follows, and there is no per-tile
           palette to offset it by. */
        const uint8_t i8 = rd8(c.screen + at);
        if (!i8) return false;
        out = bgr555(rd16(kPlttBase + (uint32_t)i8 * 2));
        return true;
    }

    if (c.kind == BG_TEXT) {
        px = (x + c.hofs) & (c.map_w * 8 - 1);
        py = (y + c.vofs) & (c.map_h * 8 - 1);
        se = rd16(map_entry_addr(c, px >> 3, py >> 3));
        tile = se & 0x3FF;
        fx = px & 7;
        fy = py & 7;
        if (se & 0x400) fx = 7 - fx;
        if (se & 0x800) fy = 7 - fy;
    } else {
        // Affine: texture coordinate = ref + x*P + y*P, in 8.8.
        px = (c.refx + c.pa * x + c.pb * y) >> 8;
        py = (c.refy + c.pc * x + c.pd * y) >> 8;
        if (c.wrap) {
            px &= c.map_w - 1;
            py &= c.map_h - 1;
        } else if (px < 0 || px >= c.map_w || py < 0 || py >= c.map_h) {
            return false;
        }
        const int tw = c.map_w >> 3;
        if (c.kind == BG_AFFINE) {
            // 8-bit map entries: a bare tile number, no flip, no palette.
            tile = rd8(c.screen + (py >> 3) * tw + (px >> 3));
            se = 0;
            fx = px & 7;
            fy = py & 7;
        } else {
            // Extended affine, 16-bit entries: laid out as one flat tw*th
            // array rather than the text BG's 32x32 blocks.
            se = rd16(c.screen + ((py >> 3) * tw + (px >> 3)) * 2);
            tile = se & 0x3FF;
            fx = px & 7;
            fy = py & 7;
            if (se & 0x400) fx = 7 - fx;
            if (se & 0x800) fy = 7 - fy;
        }
    }

    uint32_t index;
    if (c.bpp8) {
        index = rd8(c.chars + (uint32_t)tile * 64 + fy * 8 + fx);
        if (index == 0) return false;
        if (c.ext) {
            out = bgr555(rd16(c.ext + (((se >> 12) & 0xF) * 256 + index) * 2));
            return true;
        }
    } else {
        const uint8_t pair = rd8(c.chars + (uint32_t)tile * 32 + fy * 4 + (fx >> 1));
        index = (fx & 1) ? (pair >> 4) : (pair & 0xF);
        if (index == 0) return false;
        index += ((se >> 12) & 0xF) * 16;
    }
    out = bgr555(rd16(kPlttBase + index * 2));
    return true;
}

// ---- sprites ----------------------------------------------------------------
//
// Rasterised into a whole-screen buffer first, because priority has to be
// resolved against the backgrounds pixel by pixel and a sprite's priority is
// its own, not its layer's.
//
// SPRITE-VS-SPRITE ORDER IS BY PRIORITY, NOT BY OAM INDEX. When two OBJ pixels
// overlap the DS keeps the one with the lowest priority NUMBER (attr2 bits
// 10-11, 0 = nearest); OAM index only breaks a tie, low index winning. This is
// melonDS's DrawSpritePixel exactly: it walks OAM 0->127 and overwrites iff the
// new pixel is opaque AND (the slot is empty OR new prio < old prio). This walk
// runs 127->0 so a lower-index sprite is processed LATER, so the equivalent
// overwrite test is `new prio <= old prio`: `<=` keeps the last (lowest-index)
// writer on a tie, and a strictly-better priority wins outright whatever the
// index. The header note used to say order was "by OAM index regardless of
// priority" and the write below was unconditional, which was pure lowest-index.
// That put a low-index low-priority sprite in front of a high-index
// high-priority one -- in curling, a prio-1 shell body and its prio-2 semi
// shadow (OAM 0,1) covered the prio-0 cursor bar and the prio-0 snow (OAM 10+),
// so the blue shadow tint drew over the bumper and the snow took the shell's
// texture where it crossed one. Both are the same defect, fixed at the write.

struct ObjPixel {
    uint32_t color;
    uint8_t prio;
    uint8_t hit;
    uint8_t semi;   // OBJ mode 1: always a 1st target, always alpha-blended
};

ObjPixel g_obj[192][256];

// The OBJ window's mask. A mode-2 sprite is not drawn: every one of its
// non-transparent texels instead marks that pixel as "inside the OBJ window",
// and WINOUT's upper half then says which layers show there. Kept separate
// from g_obj because a mask pixel contributes no colour and has no priority.
uint8_t g_objwin[192][256];

void raster_obj(uint32_t dispcnt) {
    static const int kSizes[3][4][2] = {
        {{8, 8}, {16, 16}, {32, 32}, {64, 64}},
        {{16, 8}, {32, 8}, {32, 16}, {64, 32}},
        {{8, 16}, {8, 32}, {16, 32}, {32, 64}},
    };
    std::memset(g_obj, 0, sizeof g_obj);
    std::memset(g_objwin, 0, sizeof g_objwin);
    if (!((dispcnt >> 12) & 1))
        return;
    const uint32_t boundary = 32u << ((dispcnt >> 20) & 3);
    // DISPCNT bit 4: 1 = one-dimensional mapping (a sprite's tiles are
    // consecutive), 0 = two-dimensional (OBJ VRAM is a 32-tile-wide matrix and
    // the next row of a sprite is 32 tiles on). Stage::InitResources clears it
    // for the sub engine -- `*p1 &= 0xFFCFFFEF` -- so the bottom screen is 2D
    // and a 1D-only reader smears every HUD sprite.
    const bool map1d = (dispcnt >> 4) & 1;
    // DISPCNT bit 31: OBJ EXTENDED PALETTES. They apply to 256-colour sprites
    // ONLY -- a 16-colour sprite keeps its 16-entry bank in the standard OBJ
    // palette whatever this bit says -- and when they are armed the attr2
    // palette field stops naming a 16-colour bank and names one of sixteen
    // 256-colour SLOTS instead.
    //
    // WHY THIS IS HERE. The star select's character portrait is the sprite
    // that needs it and nothing else on that screen does. dScStarSel_c::
    // InitResources loads the shared sub OBJ tiles plus a full 0x200-byte
    // standard palette through GXS::LoadOBJPltt, and then loads the SELECTED
    // CHARACTER separately: data/2D_cad/d_2d_player_select_mario_8bit_ncg.bin
    // decompressed to OBJ VRAM + 0x4000, and its _ncl companion pushed through
    // GXS::Begin/Load/EndLoadOBJExtPltt. Measured on scene 4: four 64x64
    // sprites in colour mode 256 at palette 0, tiles 512/516/576/580, and
    // 512 * 32 bytes IS that 0x4000 -- against nine 16-colour sprites for the
    // star and the text strip. Reading a 256-colour portrait through the
    // 16-colour sprites' standard palette gives every one of its pixels the
    // wrong colour while leaving its shape and its position exactly right.
    const uint32_t objext = ((dispcnt >> 31) & 1) ? kObjExtPltt : 0;

    /* THE SEAM'S FRAME ALIGNMENT, engine B's half. In the minigames' dual-OAM
       mode OAM::Load uploads ONLY the main bank and returns, so the sub OAM
       at 0x07000400 is filled on a different beat with a different age: the
       owner measured a straddling bob-omb's two halves stepping on ALTERNATE
       frames -- the top ticking while the bottom held and vice versa. When
       the mod is engaged, hal points this at engine B's own working shadow
       (data_0209ea74), the same cure ppu.cpp's engine A raster got, so both
       halves draw one frame. Zero everywhere else. */
    const uint32_t oam_b = g_oam_src_b ? g_oam_src_b : kOamBase;

    for (int i = 127; i >= 0; --i) {
        const uint16_t a0 = rd16(oam_b + i * 8u);
        const uint16_t a1 = rd16(oam_b + i * 8u + 2);
        const uint16_t a2 = rd16(oam_b + i * 8u + 4);
        /* the seam-snow overlay owns these while engaged; see seam_snow */
        if (ppu_seam_snow_owns(a2)) continue;
        /* SM64DS_OAMAGE_TRACE: engine B's half of the probe in ppu.cpp. */
        {
            static int bget = -1;
            if (bget < 0) {
                const char *e = std::getenv("SM64DS_OAMAGE_TRACE");
                bget = e && *e && *e != '0';
            }
            static unsigned bf;
            if (bget && i == 127) ++bf;
            if (bget && a2 == 0x1010)
                std::fprintf(stderr, "[oamage] B f%u src=%08x slot%d y=%d\n",
                             bf, (unsigned)oam_b, i, (int)(a0 & 0xff));
        }
        const bool affine = a0 & 0x100;
        if (!affine && (a0 & 0x200)) continue;          // disabled
        // OBJ MODE, attribute 0 bits 10-11: 0 normal, 1 semi-transparent,
        // 2 OBJ window, 3 prohibited (bitmap on the DS). This test used to be
        // `== 3` with the comment "OBJ window", which skipped the wrong mode
        // and drew a window sprite -- an invisible mask on hardware -- as an
        // ordinary opaque sprite. See the note over ppu.cpp's OBJ layer for
        // where the field position is pinned from the decomp's own OAM::Render.
        const unsigned objmode = (a0 >> 10) & 3;
        /* MODE 3 IS THE BITMAP SPRITE, and it used to be skipped as "not
           hosted". It is how the display capture unit's frame reaches the
           screen ENGINE B IS DRIVING ON THE OTHER HALF OF THE FLIP: the
           dScMgD3DBase_c family's sel==1 arm maps the bank it captured into
           last frame to engine B's OBJ VRAM, turns BG2 off and OBJ on, and
           the frame comes back as TWELVE 64x64 direct-colour sprites --
           4 x 3 of them, which is 256x192, which is 0x18000 bytes, which is
           the capture's own size to the byte. Measured on scene 384 with the
           block seated: 98299 of 131072 bytes nonzero in engine B's OBJ VRAM
           and exactly 12 placed entries in its OAM.
           Without this the upper screen is the white backdrop on every
           second frame, which is a 30 Hz flicker rather than a missing
           feature. */
        const bool is_bmp = objmode == 3;
        const bool is_win = objmode == 2;
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
        const uint8_t prio = (a2 >> 10) & 3;

        int pa = 256, pb = 0, pc = 0, pd = 256;
        if (affine) {
            const int grp = (a1 >> 9) & 0x1F;
            pa = (int16_t)rd16(oam_b + (grp * 4 + 0) * 8u + 6);
            pb = (int16_t)rd16(oam_b + (grp * 4 + 1) * 8u + 6);
            pc = (int16_t)rd16(oam_b + (grp * 4 + 2) * 8u + 6);
            pd = (int16_t)rd16(oam_b + (grp * 4 + 3) * 8u + 6);
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
                // Where this 8x8 cell lives, in 32-byte tile slots from the
                // sprite's base. 1D: consecutive. 2D: a 32-slot-wide matrix,
                // and a 256-colour cell is two slots wide.
                /* ---- THE BITMAP SPRITE, and it leaves before the tile
                   arithmetic below because it has no tiles at all -- the
                   sprite IS a rectangle of 16-bit pixels.

                   GBATEK. The rows are addressed by DISPCNT bit 5, the
                   BITMAP OBJ mapping bit, which is a different bit from the
                   tile mapping bit 4 this file reads into map1d:

                     1D (bit 5 set): the sprite's pixels are consecutive and
                        attr2's tile number steps in units of the boundary
                        DISPCNT bit 22 selects, 128 bytes or 256.
                     2D (bit 5 clear): OBJ VRAM is one wide bitmap and the
                        sprite is a window onto it. Bit 6 chooses the width,
                        128 dots or 256, and the tile number splits into a
                        column part and a row part accordingly.

                   Engine B reads DISPCNT_B 0x00011025 on the frames that
                   use this: bit 5 SET and bit 6 CLEAR, which is 2D mapping
                   at a 256-dot width. Both arms are implemented anyway -- a
                   reader that only does the arm the one scene uses is a
                   reader that lies the first time another scene does not --
                   but only the 2D arm is exercised and only it is proved.

                   ALPHA. attr2 bits 12-15 are the sprite's alpha, and ZERO
                   MEANS THE SPRITE IS NOT DISPLAYED -- the field is not a
                   palette bank here. Per pixel, bit 15 of the colour is the
                   opacity, the same rule the direct-colour bitmap BG uses.
                   The 1..15 blend levels are NOT applied: this file's alpha
                   path is BLDALPHA's EVA/EVB, a bitmap sprite's own alpha
                   is a third mechanism, and nothing in this game asks for a
                   value between -- the family submits its twelve at 15,
                   fully opaque. A sprite that asked for one would draw
                   opaque here, which is the honest failure: visible and in
                   the right place, rather than absent. */
                if (is_bmp) {
                    const unsigned alpha = (a2 >> 12) & 0xF;
                    if (!alpha) continue;
                    uint32_t at;
                    /* BIT 6 SELECTS THE MAPPING, BIT 5 THE 2D DIMENSION, and
                       an earlier version of this block had the two jobs the
                       other way round -- it chose the arm on bit 5 and took the
                       2D width from bit 6. GBATEK's NDS DISPCNT:

                         bit 5   Bitmap OBJ 2D-Dimension
                                 (0 = 128x512 dots, 1 = 256x256 dots)
                         bit 6   Bitmap OBJ Mapping  (0 = 2D, 1 = 1D)
                         bit 22  Bitmap OBJ 1D-Boundary (0 = 128, 1 = 256 bytes)

                       It is bit 4 that is the mapping bit for TILE OBJs, and
                       reading the bitmap pair as if it followed that layout is
                       how the two got swapped. */
                    if ((dispcnt >> 6) & 1) {
                        /* 1D: the sprite's pixels are consecutive and attr2's
                           number steps in boundary units. NOT the arm this game
                           uses -- see below -- and untested by anything in the
                           tree, so it is written from the doc and left labelled
                           as such rather than claimed. */
                        const uint32_t bnd = ((dispcnt >> 22) & 1) ? 256u : 128u;
                        at = tile * bnd + (uint32_t)(ty * w + tx) * 2u;
                    } else {
                        /* 2D: OBJ VRAM is ONE bitmap `wide` dots across and the
                           sprite is a window onto it, so attr2 is a (column,
                           row) pair rather than a linear offset -- low bits X in
                           8-dot units, upper bits Y -- and the within-sprite row
                           stride is the BITMAP's width, not the sprite's.

                           THE ROM SETTLES WHICH ARM THIS IS, in one statement.
                           src/func_ov006_020e7428.c, the family's own sprite
                           builder (matched, and in all four family slices),
                           opens with

                               *(u32 *)0x4001000 = (reg & 0xffbfff9f) | 0x20;

                           and 0xffbfff9f clears bits 5, 6 AND 22 before ORing
                           bit 5 alone back in: mapping 2D, dimension 256x256,
                           1D boundary deliberately zeroed because it is not in
                           use. The same function then writes

                               attr2 = (tx + (ty << 5)) | 0xf000

                           with tx stepping 0,8,16,24 across and ty 0,8,16 down,
                           which IS this encoding and is not a linear multiple of
                           any boundary. Under it the twelve 64x64 sprites tile
                           the 256x192 capture exactly: tile 8 -> byte 0x80 =
                           pixel (64,0), tile 256 -> 0x8000 = (0,64), tile 536 ->
                           0x10180 = (192,128). */
                        const uint32_t wide = ((dispcnt >> 5) & 1) ? 256u : 128u;
                        const uint32_t mask = (wide >> 3) - 1u;   /* 0x1F or 0x0F */
                        at = ((tile & mask) * 0x10u + (tile & ~mask) * 0x80u) +
                             ((uint32_t)ty * wide + (uint32_t)tx) * 2u;
                    }
                    const uint16_t v = rd16(kObjVram + at);
                    if (!(v & 0x8000)) continue;
                    if (g_obj[py][px].hit && prio > g_obj[py][px].prio)
                        continue;
                    g_obj[py][px].color = bgr555(v);
                    g_obj[py][px].prio = prio;
                    g_obj[py][px].hit = 1;
                    g_obj[py][px].semi = 0;
                    continue;
                }
                const uint32_t slot =
                    map1d ? (c256 ? (uint32_t)(trow * (w / 8) + tcol) * 2u
                                  : (uint32_t)(trow * (w / 8) + tcol))
                          : (uint32_t)(trow * 32 + (c256 ? tcol * 2 : tcol));
                const uint32_t cell = kObjVram + tile * boundary + slot * 32u;
                uint32_t color;
                if (c256) {
                    const uint32_t idx = rd8(cell + fy * 8u + fx);
                    if (!idx) continue;
                    // Extended: attr2's palette field picks a 256-colour slot
                    // in the OBJ extended store. Standard: one flat 256-entry
                    // palette and the field means nothing.
                    color = objext
                                ? bgr555(rd16(objext + (pal * 256u + idx) * 2u))
                                : bgr555(rd16(kObjPltt + idx * 2u));
                } else {
                    const uint8_t b = rd8(cell + fy * 4u + fx / 2);
                    const uint32_t idx = (fx & 1) ? (b >> 4) : (b & 0xF);
                    if (!idx) continue;
                    color = bgr555(rd16(kObjPltt + (pal * 16u + idx) * 2u));
                }
                // A window sprite contributes its SHAPE and nothing else: the
                // texel is opaque, so the pixel is inside the window, and the
                // colour it would have had is discarded.
                if (is_win) {
                    g_objwin[py][px] = 1;
                    continue;
                }
                // Priority resolves OBJ-vs-OBJ (see the header note): overwrite
                // only if this pixel is empty or this sprite's priority is at
                // least as good (lower or equal number). With the 127->0 walk
                // that keeps the lowest-index sprite on a tie and lets a
                // higher-priority sprite win regardless of its index.
                if (g_obj[py][px].hit && prio > g_obj[py][px].prio)
                    continue;
                g_obj[py][px].color = color;
                g_obj[py][px].prio = prio;
                g_obj[py][px].hit = 1;
                // OBJ mode 1 is semi-transparent: it alpha-blends with the
                // layer below it regardless of BLDCNT's first-target bits.
                g_obj[py][px].semi = (objmode == 1);
            }
        }
    }
}

// ---- the window unit --------------------------------------------------------
//
// WIN0H/WIN1H hold X1<<8|X2 and WIN0V/WIN1V Y1<<8|Y2, all edges exclusive on
// the right/bottom. WININ's low six bits are what shows inside window 0 and
// its next six what shows inside window 1; WINOUT's low six are what shows
// outside every window and its NEXT six what shows inside the OBJ window.
// Bits 0..3 are BG0..BG3, bit 4 is OBJ, bit 5 the colour effect.
//
// THE OBJ WINDOW is the third window and it has no rectangle: its region is the
// union of the opaque texels of every mode-2 sprite, which raster_obj collects
// into g_objwin. DISPCNT bit 15 enables it, the same way bits 13 and 14 enable
// windows 0 and 1. Precedence is fixed in hardware: window 0 wins over window
// 1, window 1 over the OBJ window, and WINOUT covers what is in none of them.

struct Windows {
    bool any;
    int x1[2], x2[2], y1[2], y2[2];
    bool on[2];
    bool obj_on;
    unsigned in[2], obj_in, out;
};

void read_windows(uint32_t dispcnt, Windows &w) {
    w.on[0] = (dispcnt >> 13) & 1;
    w.on[1] = (dispcnt >> 14) & 1;
    w.obj_on = (dispcnt >> 15) & 1;
    // Any window being on changes what the OTHER regions show, so the OBJ
    // window arms the whole unit exactly as the two rectangles do.
    w.any = w.on[0] || w.on[1] || w.obj_on;
    if (!w.any) return;
    w.obj_in = (rd16(kRegBase + 0x4A) >> 8) & 0x3F;
    for (int i = 0; i < 2; ++i) {
        const uint16_t hh = rd16(kRegBase + 0x40 + i * 2);
        const uint16_t vv = rd16(kRegBase + 0x44 + i * 2);
        w.x1[i] = hh >> 8;
        w.x2[i] = hh & 0xFF;
        w.y1[i] = vv >> 8;
        w.y2[i] = vv & 0xFF;
        // A right/bottom edge at or before the left/top one means the window
        // runs to the end of the line, which is how the hardware behaves when
        // the register holds 0 for a full-width window.
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
    if (w.obj_on && g_objwin[y][x]) return w.obj_in;
    return w.out;
}

// ---- master brightness ------------------------------------------------------

struct Bright { int mode, factor; };

inline uint32_t apply_bright(uint32_t c, const Bright &b) {
    if (!b.factor || b.mode == 0 || b.mode == 3) return c;
    int r = (c >> 16) & 0xFF, g = (c >> 8) & 0xFF, bl = c & 0xFF;
    if (b.mode == 1) {
        r += (255 - r) * b.factor / 16;
        g += (255 - g) * b.factor / 16;
        bl += (255 - bl) * b.factor / 16;
    } else {
        r -= r * b.factor / 16;
        g -= g * b.factor / 16;
        bl -= bl * b.factor / 16;
    }
    return 0xFF000000u | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)bl;
}

// ---- the colour special-effects unit (BLDCNT) -------------------------------
//
// Only ALPHA (mode 1) and the always-on semi-transparent-OBJ alpha are applied
// here; the BLDY brightness modes 2/3 are the fade path's, see the header note.
// Register offsets are engine-relative: BLDCNT 0x50, BLDALPHA 0x52 on this
// engine's kRegBase. Layer ids match the window-mask bits: 0..3 BG0..BG3, 4
// OBJ, 5 the backdrop (BD).

// SM64DS_BLEND_OFF=1 keeps the old opaque path, so a before/after is one binary
// at one .dsstate base -- the only comparison notes/port-selftest-bmp-gate.md
// permits -- and a regression can be bisected against the pre-blend image.
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

// top*eva/16 + below*evb/16, per channel, in 5-bit and clamped to 31 -- the
// DS's own arithmetic. The framebuffer holds bgr555-expanded 8-bit channels and
// (v<<3|v>>2)>>3 recovers the original 5-bit value exactly, so the round trip
// is lossless and the blend is what the hardware produces.
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

// The effect for one pixel, given the top layer and the one directly below it.
// `below` (col[1]/id[1]) is always valid: a pixel with nothing under the top is
// resolved against the backdrop, id 5, which BLDCNT can name as a 2nd target.
inline uint32_t blend_apply(const Blend &bl, unsigned mask, uint32_t top,
                            int top_id, bool top_semi, uint32_t below,
                            int below_id) {
    if (bl.off) return top;
    // Window bit 5 disables colour special effects inside this region.
    if (!(mask & 0x20)) return top;
    const bool below_second = (bl.second & (1u << below_id)) != 0;
    if (top_semi) {
        // Always 1st target, always alpha, whatever the mode says. With no 2nd
        // target beneath it a semi-transparent OBJ draws opaque (hardware).
        if (below_second) return blend_alpha(top, below, bl.eva, bl.evb);
        return top;
    }
    if (bl.mode == 1 && (bl.first & (1u << top_id)) && below_second)
        return blend_alpha(top, below, bl.eva, bl.evb);
    // modes 2/3 (brightness) belong to the fade path; recognised and deferred.
    return top;
}

}  // namespace

void ppu_scanout_sub(SubFramebuffer &fb)
{
    // The audit's per-frame seam. This is the one scan-out the live harness
    // calls every frame, so it samples BOTH engines' register files rather than
    // just this one's -- engine A's live compositor is hal/message_compositor.cpp
    // and this lane does not own that file. Inert unless SM64DS_PPU_AUDIT is set.
    ppu_audit_sample("ppu_scanout_sub");

    const uint32_t dispcnt = rd32(kRegBase);
    const unsigned disp_mode = (dispcnt >> 16) & 3;
    const bool forced_blank = (dispcnt >> 7) & 1;

    Bright br;
    {
        const uint16_t mb = rd16(kRegBase + 0x6C);
        br.factor = mb & 0x1F;
        if (br.factor > 16) br.factor = 16;
        br.mode = (mb >> 14) & 3;
    }

    if (disp_mode == 0 || forced_blank) {
        // Display off is white on a DS panel, not black.
        for (int y = 0; y < SUB_H; ++y)
            for (int x = 0; x < SUB_W; ++x) fb.px[y][x] = 0xFFFFFFFFu;
        return;
    }

    BgLayer bgs[4];
    for (int i = 0; i < 4; ++i) read_bg(bgs[i], i, dispcnt);

    Windows win;
    read_windows(dispcnt, win);

    raster_obj(dispcnt);

    const uint32_t backdrop = bgr555(rd16(kPlttBase));
    const Blend bld = read_blend();

    for (int y = 0; y < SUB_H; ++y) {
        for (int x = 0; x < SUB_W; ++x) {
            const unsigned mask = window_mask(win, x, y);
            // Resolve the TOP visible layer and the one directly BELOW it, which
            // is all the colour-effect unit needs. Priority 0 is nearest; at
            // equal priority a sprite is above a background and among
            // backgrounds the lower number wins. Layer ids: 0..3 BG, 4 OBJ.
            uint32_t col[2] = {backdrop, backdrop};
            int id[2] = {5, 5};        // 5 = backdrop (BD), the implicit bottom
            bool semi = false;
            int found = 0;
            for (int prio = 0; prio < 4 && found < 2; ++prio) {
                const ObjPixel &o = g_obj[y][x];
                if (o.hit && o.prio == prio && (mask & 0x10)) {
                    if (!found) semi = o.semi;
                    col[found] = o.color; id[found] = 4; ++found;
                    if (found >= 2) break;
                }
                for (int bg = 0; bg < 4 && found < 2; ++bg) {
                    if (bgs[bg].kind == BG_OFF || bgs[bg].prio != prio) continue;
                    if (!(mask & (1u << bg))) continue;
                    uint32_t s;
                    if (sample_bg(bgs[bg], x, y, s)) {
                        col[found] = s; id[found] = bg; ++found;
                    }
                }
            }
            // Nothing drawn: the pixel is pure backdrop, no top layer to blend.
            const uint32_t c = found ? blend_apply(bld, mask, col[0], id[0], semi,
                                                   col[1], id[1])
                                     : backdrop;
            fb.px[y][x] = apply_bright(c, br);
        }
    }
}

bool ppu_write_bmp_sub(const char *path, const SubFramebuffer &fb)
{
    std::FILE *f = std::fopen(path, "wb");
    if (!f) return false;

    const uint32_t stride = SUB_W * 3;
    const uint32_t pad = (4 - (stride & 3)) & 3;
    const uint32_t img = (stride + pad) * SUB_H;
    const uint32_t off = 54;

    uint8_t hdr[54] = {};
    hdr[0] = 'B'; hdr[1] = 'M';
    const uint32_t total = off + img;
    std::memcpy(hdr + 2, &total, 4);
    std::memcpy(hdr + 10, &off, 4);
    const uint32_t dib = 40;
    std::memcpy(hdr + 14, &dib, 4);
    const int32_t w = SUB_W, h = SUB_H;
    std::memcpy(hdr + 18, &w, 4);
    std::memcpy(hdr + 22, &h, 4);
    const uint16_t planes = 1, bpp = 24;
    std::memcpy(hdr + 26, &planes, 2);
    std::memcpy(hdr + 28, &bpp, 2);
    std::memcpy(hdr + 34, &img, 4);
    std::fwrite(hdr, 1, sizeof hdr, f);

    const uint8_t zero[3] = {0, 0, 0};
    for (int y = SUB_H - 1; y >= 0; --y) {
        for (int x = 0; x < SUB_W; ++x) {
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

// ---- the corner panel -------------------------------------------------------
//
// Composited at 1:1 DS pixels whatever tier the top screen is drawn at, which
// is the whole reason this file exists. A one-pixel frame around it so the
// panel reads as a panel and not as a corruption of the 3D view.
void ppu_compose_sub(const SubFramebuffer &sub, uint32_t *dst, int dst_w,
                     int dst_h, int margin, int div)
{
    if (div < 1) div = 1;
    const int out_w = SUB_W / div, out_h = SUB_H / div;
    const int x0 = dst_w - out_w - margin;
    const int y0 = dst_h - out_h - margin;
    if (x0 < 1 || y0 < 1) return;      // no room; leave the frame alone

    for (int x = x0 - 1; x <= x0 + out_w; ++x) {
        dst[(y0 - 1) * dst_w + x] = 0xFF000000u;
        dst[(y0 + out_h) * dst_w + x] = 0xFF000000u;
    }
    for (int y = y0 - 1; y <= y0 + out_h; ++y) {
        dst[y * dst_w + (x0 - 1)] = 0xFF000000u;
        dst[y * dst_w + (x0 + out_w)] = 0xFF000000u;
    }
    if (div == 1) {
        for (int y = 0; y < SUB_H; ++y)
            std::memcpy(dst + (y0 + y) * dst_w + x0, sub.px[y], SUB_W * 4);
        return;
    }
    const int n = div * div;
    for (int y = 0; y < out_h; ++y)
        for (int x = 0; x < out_w; ++x) {
            unsigned r = 0, g = 0, b = 0;
            for (int sy = 0; sy < div; ++sy)
                for (int sx = 0; sx < div; ++sx) {
                    const uint32_t p = sub.px[y * div + sy][x * div + sx];
                    r += (p >> 16) & 0xFF;
                    g += (p >> 8) & 0xFF;
                    b += p & 0xFF;
                }
            dst[(y0 + y) * dst_w + (x0 + x)] =
                0xFF000000u | ((r / n) << 16) | ((g / n) << 8) | (b / n);
        }
}

// ---- THE GAP BAND -----------------------------------------------------------
//
// WHAT THE BAND IS. The DS's two panels are not edge to edge: there is a hinge
// between them, and the game KNOWS it. ov004's framework carries a word,
// data_ov004_020beb6c, that every minigame sets at InitResources to the number
// of DS rows of hinge to simulate -- 32 for most, 48 for two, 16 for the
// trampoline family, 80 for the snowball -- and RenderOamBothScreens submits
// the top engine's sprites at a2 + 0xc0 + that word. So the game's own world is
// 192 + G + 192 rows tall and the middle G of them are behind the plastic. The
// port stacked the two screens with nothing between them, which squeezed those
// G rows out of existence: a shell that rolls off the bottom screen reappears
// on the top screen one G-th of a screen too high, and the motion jumps.
//
// This band puts the missing rows back as PICTURE. It is not the hardware's
// behaviour and does not claim to be -- the hardware's behaviour is opaque
// plastic -- so what goes in it is the player's choice and nothing about the
// simulation changes either way.
//
// Two fills, and the ambient one is the point of the feature. See band_fill.
namespace {

// The ambient fill's column count. Twenty-four over 256 DS columns is a column
// every ten or so pixels, which is fine enough that a lamp on one side of the
// screen does not tint the other and coarse enough that the averaging is
// nothing: 24 columns x 2 edges x a two-column-wide window is under 2500 pixel
// reads a frame, against the 400 000 the compose already copies.
constexpr int kAmbCols = 24;

/* HOW WIDE EACH COLUMN SAMPLES. The design says "its own width plus HALF A
   COLUMN on each side", and then glosses that "1.5 column widths". Those two
   do not agree -- own width plus a half on each side is TWO column widths --
   and this takes the words rather than the gloss, so the halo is half a column
   and the window is two columns wide. It is one constant either way: numerator
   1 denominator 4 gives the 1.5-total reading. */
constexpr int kAmbHaloNum = 1, kAmbHaloDen = 2;

struct RGB { int r, g, b; };

/* Column c's own span [x0, x1) in the image's own pixels. Computed by the
   multiply-then-divide form rather than by accumulating a width, so the last
   column ends exactly at w whatever the remainder is. */
void amb_col_span(int w, int c, int &x0, int &x1)
{
    x0 = (c * w) / kAmbCols;
    x1 = ((c + 1) * w) / kAmbCols;
}

/* And the span it SAMPLES: its own plus the halo on each side, clamped at the
   image edges. The clamp is why the outermost columns are not lopsided in the
   other direction -- an unclamped window would read off the row. */
void amb_sample_span(int w, int c, int &s0, int &s1)
{
    int x0, x1;
    amb_col_span(w, c, x0, x1);
    const int halo = ((x1 - x0) * kAmbHaloNum) / kAmbHaloDen;
    s0 = x0 - halo;
    s1 = x1 + halo;
    if (s0 < 0) s0 = 0;
    if (s1 > w) s1 = w;
}

RGB amb_avg(const uint32_t *row, int s0, int s1)
{
    const int n = s1 - s0 > 0 ? s1 - s0 : 1;
    int r = 0, g = 0, b = 0;
    for (int x = s0; x < s1; ++x) {
        const uint32_t p = row[x];
        r += (int)((p >> 16) & 0xff);
        g += (int)((p >> 8) & 0xff);
        b += (int)(p & 0xff);
    }
    RGB o = {r / n, g / n, b / n};
    return o;
}

/* THE SAME AVERAGE WITH THE CROSSERS LEFT OUT. The wash is supposed to be the
   SCENE's glow, and the plain average reads whatever is on the edge row --
   including a bob-omb mid-crossing, whose black then pink turned whole columns
   black then pink for as long as it straddled the edge (the owner's 2026-08-20
   report), and including Mario, whom no OAM mask can see (the owner's
   2026-08-26 one). w256 is how much of a crosser stands in each DS column of
   this edge row, 0..255; see THE EDGE ROWS' SETTLED BACKGROUND below for what
   fills it and why it is a weight rather than a flag.
 *
 * A CROSSER COLUMN IS SUBSTITUTED RATHER THAN SKIPPED, and that is the change
 * the 2026-08-26 ruling wanted. Skipping renormalises over the columns that
 * are left, which is right until a span is covered END TO END -- and the old
 * fallback for that was the PLAIN average, i.e. the crosser's own colours,
 * which is precisely "the thing travelling through touched the gradient". The
 * settled background carries a colour for every column, so every span has a
 * background answer and no span can fall back to a contaminated mean.
 *
 * WITH EVERY WEIGHT AT ZERO THIS IS amb_avg EXACTLY, so a still frame with
 * nothing crossing either edge composes byte for byte what it did before. */
RGB amb_avg_bg(const uint32_t *row, int s0, int s1, const uint8_t *mask256,
               const uint8_t *w256, const uint32_t *bg256, int w)
{
    if (!mask256) return amb_avg(row, s0, s1);
    const int rx = w / 256 > 0 ? w / 256 : 1;
    int r = 0, g = 0, b = 0, n = 0;
    for (int x = s0; x < s1; ++x) {
        const int dsx = x / rx;
        /* A SPRITE COLUMN IS SKIPPED, exactly as it has been since the
           2026-08-20 report. The OAM mask is a fact about what the engine drew
           and "read the rest of the window instead" is the right answer for a
           column that has no background in it at all. Nothing about this arm
           moved in run mg15, which is why every gap-on capture in the port's
           existing picture set still hashes the same. */
        if (dsx < 256 && mask256[dsx]) continue;
        const uint32_t p = row[x];
        int pr = (int)((p >> 16) & 0xff);
        int pg = (int)((p >> 8) & 0xff);
        int pb = (int)(p & 0xff);
        /* AND EVERYTHING ELSE IS BLENDED TOWARD ITS OWN SETTLED VALUE by how
           much of a crosser stands in it. This is the arm the ruling added:
           the OAM mask cannot see a 3D crosser, and a weight computed off the
           column's own history can. At weight 0 -- every column of a still
           band, and every column of the seeded first frame -- this is the
           live pixel unchanged. */
        const int cw = (dsx < 256 && w256 && bg256) ? (int)w256[dsx] : 0;
        if (cw) {
            const uint32_t q = bg256[dsx];
            pr += ((int)((q >> 16) & 0xff) - pr) * cw / 255;
            pg += ((int)((q >> 8) & 0xff) - pg) * cw / 255;
            pb += ((int)(q & 0xff) - pb) * cw / 255;
        }
        r += pr;
        g += pg;
        b += pb;
        ++n;
    }
    /* A SPAN A SPRITE COVERS END TO END falls back to the SETTLED row over the
       same span rather than to the plain average of the live one. The plain
       average was the crosser's own colours, which is the thing the ruling
       forbids; the settled row is the background there by construction. On a
       frame where the memory was just seeded the two are the same pixels, so
       this changes no capture that composes once. */
    if (!n) {
        if (!bg256) return amb_avg(row, s0, s1);
        const int d0 = s0 / rx, d1 = (s1 + rx - 1) / rx;
        for (int dsx = d0; dsx < d1 && dsx < 256; ++dsx) {
            const uint32_t q = bg256[dsx];
            r += (int)((q >> 16) & 0xff);
            g += (int)((q >> 8) & 0xff);
            b += (int)(q & 0xff);
            ++n;
        }
        if (!n) return amb_avg(row, s0, s1);
    }
    RGB o = {r / n, g / n, b / n};
    return o;
}

/* ---- THE BAND'S PERSISTENCE ------------------------------------------------
 *
 * WHAT WAS WRONG WITH READING THE EDGE COLD. The fill re-read both edge rows
 * every frame and kept nothing, so the band's colour was a function of one
 * frame and of nothing before it. That is right for a still picture and wrong
 * for the thing the band exists to show: something crossing the seam. MEASURED
 * on scene 368, column 12, with SM64DS_GAP_AMB_TRACE over a scripted slingshot
 * launch -- the bottom edge sat at (184,200,254), dropped to (138,144,162) and
 * (106,115,142) as the ball passed over it, and was back at (184,200,254) the
 * next frame. Two frames, then a snap. The top edge was worse: one frame,
 * (178,196,247) -> (103,111,130) -> (178,196,247). A player sees a blink,
 * which is what the report said: "it renders for only like 2 frames".
 *
 * WHAT IT DOES NOW. Each column of each edge FOLLOWS its sample instead of
 * taking it: fast towards a disturbance, slowly back to rest. A crosser paints
 * a pulse that lands in a frame or two and fades over about ten, which is what
 * a strip of light behind the hinge would do.
 *
 * ATTACK AND DECAY ARE NOT UP AND DOWN, and that is the whole design decision
 * here rather than a detail. The obvious asymmetric follower attacks when the
 * sample RISES and decays when it falls -- a phosphor -- and the measurement
 * above is why that is the wrong shape for this band: the crossing object is
 * DARKER than the sky it crosses, so a phosphor would smear the ball away and
 * leave the picture the report already complains about. What "attack" has to
 * mean is the deviation GROWING, whichever way it points, and "decay" the
 * deviation shrinking. That needs a rest to measure deviation from, so there
 * are two followers per channel:
 *
 *   rest   the settled light, tracking the sample slowly and always. What the
 *          scene looks like when nothing is crossing.
 *   glow   what is drawn. Attacks while the sample is further from rest than
 *          glow is, decays otherwise.
 *
 * SO A SUSTAINED CHANGE IS FOLLOWED AND A BRIEF ONE LINGERS, and both fall out
 * of the same test rather than being special-cased. A fade, a scene's own
 * lighting change, a lamp that comes on and stays on: rest lags the sample, so
 * the deviation keeps growing, so glow attacks the whole way and the band does
 * not trail the picture. A ball that is gone in two frames: rest barely moved,
 * so the deviation collapses the moment it leaves and glow decays home.
 *
 * IT CONVERGES EXACTLY, which is a requirement and not a nicety: a still scene
 * has to end up at the same pixels the cold read gave, or every capture in the
 * feature's picture set changes meaning. Both followers step by CEILING of the
 * fraction of the remaining distance, never past the target, and NEVER BY LESS
 * THAN ONE WHOLE CHANNEL. All three clauses earn their place:
 *
 *   the ceiling      an integer follower that truncates stalls short of its
 *                    target and sits there forever;
 *   the clamp        num <= den, so the step cannot exceed the distance: no
 *                    overshoot, no oscillation, monotone approach;
 *   the floor        MEASURED, and it is the one that is not obvious. With the
 *                    step allowed to fall to a 256th of a channel the follower
 *                    is exponential all the way down, so the last WHOLE channel
 *                    of a decay takes about seventy frames -- the band sat one
 *                    unit off the direct read for over a second after a pulse
 *                    was long invisible. A floor of one channel closes that in
 *                    one frame per unit and bounds convergence at about sixty
 *                    frames from the worst deviation there is. It binds only
 *                    below fifteen channels of remaining distance, so the
 *                    curve a player actually sees is untouched.
 *
 * ALL INTEGER AND FRAME-INDEXED, for the reason the ramp below is: no wall
 * clock is read, so a headless run and a windowed run of the same frames give
 * the same band, and tools/gapproof.py can re-derive it from the sample series
 * with the same arithmetic. */

/* 1/256 of a channel step, so the tail of a decay is a curve rather than a
   staircase. The eight bits are the ONLY fixed point here: samples in, drawn
   colour out, both plain 0..255. */
constexpr int kAmbFixBits = 8;

/* ATTACK: seven eighths of the remaining distance per frame. 87.5% of a change
   is there in one frame and 98.4% in two, which is the "lands within a frame or
   two" the report asks for without making the leading edge a hard step. */
constexpr int kAmbAttackNum = 7, kAmbAttackDen = 8;

/* DECAY: one fifteenth per frame, a half-life of 10.0 frames (ln 2 / -ln(14/15)
   = 10.05), so a pulse is still half lit a sixth of a second later and gone by
   about a third. */
constexpr int kAmbDecayNum = 1, kAmbDecayDen = 15;

/* REST: one sixteenth per frame, half-life 10.7 frames. Slow enough that a
   two-frame crosser barely moves it -- which is what makes the crossing read as
   a deviation at all -- and quick enough that a real lighting change becomes
   the new rest within about a third of a second. */
constexpr int kAmbRestNum = 1, kAmbRestDen = 16;

/* One follower's move for one frame: the ceiling of num/den of the remaining
   distance, floored at a whole channel and clamped to the distance itself. See
   the three clauses in the note above. */
int amb_step(int d, int num, int den)
{
    if (d == 0) return 0;
    const int a = d > 0 ? d : -d;
    int s = (a * num + den - 1) / den;
    if (s < 1 << kAmbFixBits) s = 1 << kAmbFixBits;
    if (s > a) s = a;
    return d > 0 ? s : -s;
}

struct AmbMemory {
    int have;                       /* 0 until a scene's first ambient frame */
    int rest[2][kAmbCols][3];       /* [edge][column][channel], fixed point */
    int glow[2][kAmbCols][3];
};
/* Edge 0 is the top screen's bottom row, edge 1 the bottom screen's top row.
   One instance: the compose runs on one thread on one path and never re-enters,
   the same bookkeeping band_peek's scratch band makes. 1.1 KB of .bss. */
AmbMemory g_amb;

/* One channel of one column of one edge. Returns the colour to draw. */
int amb_follow(int e, int c, int i, int sample)
{
    const int t = sample << kAmbFixBits;
    int &rest = g_amb.rest[e][c][i];
    int &glow = g_amb.glow[e][c][i];
    rest += amb_step(t - rest, kAmbRestNum, kAmbRestDen);
    const int dt = t > rest ? t - rest : rest - t;
    const int dg = glow > rest ? glow - rest : rest - glow;
    if (dt >= dg)
        glow += amb_step(t - glow, kAmbAttackNum, kAmbAttackDen);
    else
        glow += amb_step(t - glow, kAmbDecayNum, kAmbDecayDen);
    return glow >> kAmbFixBits;
}

/* The scene's first ambient frame is the direct sample EXACTLY, not a ramp up
   from black: a minigame's band is lit correctly on the frame it appears. */
void amb_seed(int e, int c, const RGB &v)
{
    const int ch[3] = {v.r, v.g, v.b};
    for (int i = 0; i < 3; ++i)
        g_amb.rest[e][c][i] = g_amb.glow[e][c][i] = ch[i] << kAmbFixBits;
}

/* SM64DS_GAP_AMB_TRACE=1: one line per composed ambient frame carrying the
   twenty-four column colours of BOTH edges, as the fill read them and as the
   fill used them.
 *
 * WHY BOTH NUMBERS. A captured BMP is one frame, and one frame cannot show
 * what the band did over time -- which is exactly the question a player asks
 * about a fast crosser. This is the series, and it is also what lets the
 * checker re-derive a smoothed band at all: tools/gapproof.py reads t_raw as
 * the input history, runs its OWN follower over it, and must land on t_out.
 *
 * stderr, and off unless asked, so no capture and no play session carries it. */
int amb_trace_on(void)
{
    static int on = -1;
    if (on < 0) {
        const char *s = std::getenv("SM64DS_GAP_AMB_TRACE");
        on = s && *s && *s != '0';
    }
    return on;
}

unsigned g_amb_frame;

void amb_trace_list(const char *name, const RGB *v)
{
    std::fprintf(stderr, " %s=", name);
    for (int c = 0; c < kAmbCols; ++c)
        std::fprintf(stderr, "%s%02x%02x%02x", c ? "," : "", v[c].r & 0xff,
                     v[c].g & 0xff, v[c].b & 0xff);
}

/* THE AMBIENT FILL.
 *
 * The band is lit by the two pictures it sits between. Each of twenty-four
 * columns takes the average colour of the top screen's BOTTOM row and of the
 * bottom screen's TOP row over its own sample window, and is a vertical ramp
 * from the first to the second; the columns are interpolated between their
 * CENTRES across the width, so there is no seam where two columns meet.
 *
 * THE EDGE ROWS ARE RE-READ EVERY FRAME AND FOLLOWED RATHER THAN TAKEN, so the
 * band glows with the scene rather than with a colour picked once at load, and
 * something that crosses the seam in two frames leaves a pulse rather than a
 * blink. See THE BAND'S PERSISTENCE above for what the follower is and why its
 * asymmetry is not up-and-down.
 *
 * WHY THE RAMP IS SAMPLED AT ROW CENTRES. The band sits BETWEEN the two rows it
 * interpolates, so its first row is not the top edge and its last is not the
 * bottom edge; both are half a row in from them. (2k+1) / 2h is that, in
 * integers, and it is also what keeps a one-row band from being pinned to
 * either end.
 *
 * ALL INTEGER, and that is a testability decision as much as a speed one: a
 * float ramp is not reproducible from a second implementation, and the proof
 * this feature ships with is exactly a second implementation re-deriving the
 * band from the captured edge rows and diffing it. Every divide here is a
 * truncating integer divide and the checker does the same ones. */
void band_edge_obj_masks(uint8_t *mtop, uint8_t *mbot, int main_lower);
    /* defined after the band raster below; the masks say which DS columns of
       the two edge rows an OBJ covers, so the wash can sample the background
       alone. main_lower is POWCNT1 bit 15 clear: the two ENGINES exchange, the
       two edge rows do not, because mtop is the upper screen's last row and
       mbot the lower screen's first whichever engine is feeding each. */
int band_game_g(const StackLayout &lay);
    /* defined beside band_bias_a/b below, which is where the question belongs:
       how many of the band's rows the GAME actually has, which since the
       owner's uniform-hinge ruling is no longer the band's own height. Only
       the trace line above reads it from up here. */

/* ---- THE EDGE ROWS' SETTLED BACKGROUND, AND WHAT IS CROSSING THEM ----------
 *
 * THE OWNER'S RULING, 2026-08-26, on the uniform 32-row hinge: "you cant see
 * mario through the gradient. Any thing that travels through the screens
 * should not touch/effect the gradient they should just blur as they pass
 * through." Two sentences about two passes, and they want the SAME fact: which
 * pixels of the two edge rows are a crosser and which are the scene. The
 * gradient has to be built from the second set alone; the ghost has to be
 * built from the first.
 *
 * band_edge_obj_masks below answers that for SPRITES, off the two engines' own
 * OAM, and that mask is the whole of what the wash had. It cannot answer it on
 * the family the report is about. MEASURED, run mg15 lane BAND, scene 384
 * (Trampoline Time) and scene 372 (Bounce and Pounce), one headless run per
 * frame at 9 frames across a crossing: the band carried ZERO pixels of
 * anything but the ambient fill on every one of them, and
 * SM64DS_GAP_PEEK_TRACE=1 printed no census line at all -- no OAM entry's box
 * reaches the band on either scene. The crosser is MARIO, who on the
 * dScMgD3DBase_c family is 3D geometry on the live engine and a DISPLAY
 * CAPTURE BITMAP on the other panel. Neither is an OAM entry, so every band
 * pass in this file -- all of which read OAM -- has nothing to draw him from,
 * and every edge mask in this file has nothing to exclude him by. That is
 * exactly the two halves of the report: he is inside the wash's own average,
 * and he is absent from the band.
 *
 * WHAT IS AVAILABLE FOR ANY SOURCE is the row's own history. A scene's edge
 * row is nearly still; a crosser is a few frames of it being different. So
 * each edge keeps a SETTLED BACKGROUND -- one colour per DS column, following
 * the live row by the same slow step the ambient `rest` follower uses -- and a
 * column standing far enough off its own settled value is a crosser. NOTHING
 * IS INVENTED: the carried pixels are the game's own, read back out of the
 * composed image the two screen blits have just written.
 *
 * IT FOLLOWS THROUGH EVERYTHING, sprites included, and that is a decision
 * rather than an oversight. Holding the memory still under an OAM column was
 * built first, on the reading that a sprite must not teach the background its
 * own colour -- and it makes a PARKED sprite permanently deviant, because its
 * column's settled value stays frozen at whatever the row held before the
 * sprite arrived, so the band would carry a ghost of a HUD element for as long
 * as the minigame ran. A follower this slow already tells the two cases apart
 * on its own: a crosser is gone in three or four frames and moves a 1/16
 * follower by almost nothing, while something that parks is learned inside
 * about ten and stops reading as a crossing. One rule, both cases, and the OAM
 * mask keeps its own separate job in the wash.
 *
 * PER SCENE, dropped by ppu_band_ambient_reset beside the ambient memory and
 * for the same reason: a background learned in one minigame would light the
 * next one's first frames, and worse, would call its whole first second a
 * crossing. */

/* HOW FAR OFF ITS OWN SETTLED VALUE A COLUMN HAS TO STAND, in channels, before
   it reads as a crosser rather than as the scene. MEASURED on scene 384 over
   the 500..600 window: the still band's own column-to-column frame noise stays
   inside 6 channels (bandcensus.py's own threshold, and the band re-derives
   byte-exact there), while Mario against the night sky and against the blue
   platform moves whole columns by 60 and more. 24 sits well clear of the noise
   and well under the signal.
 *
 * AND IT IS A RAMP RATHER THAN A LINE, from kEdgeLo to kEdgeDev. A BINARY mask
 * was built and measured first, and the measurement is why this is a ramp: a
 * column's deviation crosses a single threshold back and forth on consecutive
 * frames at a crosser's own soft edge -- 3D geometry has no hard edge -- and
 * every flip swaps that column's whole contribution to the wash's average
 * between two different colours at once.
 *
 * THE NUMBER THE RAMP IS WORTH, run mg15 lane BAND, scene 384 windowed, the
 * crossing at frames 300..350, largest frame-to-frame move of the DRAWN
 * gradient endpoint over all 24 columns and both edges:
 *
 *     no crosser term at all (SM64DS_BAND_FIX=0)   83 upper, 81 lower
 *     binary mask at kEdgeDev                      20 upper, 14 lower
 *     this ramp                                     7 upper,  7 lower
 *
 * and over a QUIET window of the same run (frames 100..250, nothing crossing)
 * every arm reads 0 and 0, so the ramp costs a still band nothing. The
 * remaining 7 is the crosser's sub-kEdgeLo penumbra reaching the average
 * un-weighted; it is one column of twenty-four, inside a wash that is already
 * a blur, and lowering kEdgeLo to chase it would start treating a scene's own
 * animation as a crossing.
 *
 * WITH THE DEVIATION AT ZERO THE WEIGHT IS ZERO AND THE WASH READS THE LIVE
 * ROW EXACTLY, which is the whole zero-change guarantee: a still band with
 * nothing crossing either edge composes byte for byte what it always did. */
constexpr int kEdgeLo = 8, kEdgeDev = 24;

struct EdgeMem {
    int have;                  /* 0 until this scene's first band frame */
    int bg[2][256][3];         /* [edge][DS column][channel], fixed point */
};
/* Edge 0 is the UPPER screen's last row, edge 1 the LOWER screen's first --
   physical rows, which POWCNT1's swap does not move; see band_edge_obj_masks.
   One instance, the same bookkeeping the ambient memory makes and for the same
   reason: the compose runs on one thread on one path and never re-enters. */
EdgeMem g_edge;
/* HOW MUCH OF A CROSSER IS IN THIS COLUMN, 0..255. 0 is "this is the scene",
   255 is "this is entirely something passing through", and the two consumers
   read the same number: the wash blends the settled background in by it, and
   the carry fades the live pixel in by it. One number, so the gradient and the
   ghost can never disagree about what is crossing. */
uint8_t g_edge_w[2][256];
/* AND THE OAM MASK, kept separate and kept doing exactly its old job: the wash
   SKIPS a sprite column, which is what it has done since 2026-08-20 and what
   every gap-on capture in the port's picture set was taken under. The weight
   above is the new arm and it never touches a column this one claims. */
uint8_t g_edge_obj[2][256];
uint32_t g_edge_live[2][256];   /* this frame's live edge colour */
uint32_t g_edge_bg_px[2][256];  /* this frame's settled background colour */

/* SM64DS_BAND_FIX=0 gives back the band this lane found: the wash masked by
   OAM alone and no carry, on the same binary, which is what
   notes/port-selftest-bmp-gate.md requires before two BMPs may be compared at
   all. Nothing else turns any of this off. */
int band_fix_on(void)
{
    static int on = -1;
    if (on < 0) {
        const char *s = std::getenv("SM64DS_BAND_FIX");
        on = !(s && *s == '0');
    }
    return on;
}

/* SM64DS_BAND_TRACE=1: one line per composed band frame carrying the numbers
   this lane's claims are read off -- the game's own G against the band the
   layout drew, the two engines' row windows, and how many DS columns of each
   edge row read as a crosser. A capture with no such line is a capture this
   pass did not run on. stderr, off unless asked. */
int band_trace_on(void)
{
    static int on = -1;
    if (on < 0) {
        const char *s = std::getenv("SM64DS_BAND_TRACE");
        on = s && *s && *s != '0';
    }
    return on;
}

unsigned g_band_frame;
/* HOW MANY BAND CELLS THE CARRY WROTE THIS FRAME, reset at the head of the
   frame's edge reading and reported by band_ghost at the end of it. It is the
   direct per-frame answer to the only law this whole feature runs under --
   nothing may visibly vanish at the seam -- because a run of frames whose
   crosser weight is non-zero and whose carried count is non-zero on EVERY one
   of them is a crossing with no vanish frame in it, said by the program rather
   than inferred from a handful of captures. */
unsigned g_band_carried;

/* One DS column of one edge row, reduced from the host pixels that stand over
   it. The average rather than a sample, because the mask this feeds is about
   "is something here" and a thin crosser that falls between samples is exactly
   the thing that must not be missed. */
uint32_t edge_col(const uint32_t *row, int x, int rx)
{
    int r = 0, g = 0, b = 0;
    for (int o = 0; o < rx; ++o) {
        const uint32_t p = row[x * rx + o];
        r += (int)((p >> 16) & 0xff);
        g += (int)((p >> 8) & 0xff);
        b += (int)(p & 0xff);
    }
    return 0xFF000000u | ((uint32_t)(r / rx) << 16) |
           ((uint32_t)(g / rx) << 8) | (uint32_t)(b / rx);
}

/* THE FRAME'S EDGE READING, taken ONCE and read by both the fill and the
   ghost. It runs at the head of band_fill because the fill is the first band
   pass in the compose and the two screen blits are already down; the ghost
   runs later in the same frame and reads what this left. */
void band_edge_update(const uint32_t *dst, int dst_w, const StackLayout &lay)
{
    band_edge_obj_masks(g_edge_obj[0], g_edge_obj[1], lay.main_lower);
    const uint32_t *row[2] = {dst + (size_t)(lay.band_y - 1) * dst_w,
                              dst + (size_t)lay.bottom_y * dst_w};
    const int rx = dst_w / 256 > 0 ? dst_w / 256 : 1;
    for (int e = 0; e < 2; ++e)
        for (int x = 0; x < 256; ++x)
            g_edge_live[e][x] = edge_col(row[e], x, rx);

    /* SEEDED FROM THIS FRAME EXACTLY, once per scene, which is amb_seed's own
       rule and it is here for amb_seed's own reason: a minigame's band is lit
       correctly on the frame it appears rather than ramping up to it. It also
       makes the FIRST composed frame carry no crossing signal at all, by
       construction -- every deviation is zero -- and that is the honest answer,
       because one frame cannot say what is moving. Every headless capture in
       this port composes exactly once (hal/sub_screen.cpp's stacked image is
       built at the capture, not per tick), so every one of them is that frame,
       and the whole picture set the port already has is unmoved by this. */
    if (!g_edge.have) {
        g_edge.have = 1;
        for (int e = 0; e < 2; ++e)
            for (int x = 0; x < 256; ++x) {
                const uint32_t v = g_edge_live[e][x];
                g_edge.bg[e][x][0] = (int)((v >> 16) & 0xff) << kAmbFixBits;
                g_edge.bg[e][x][1] = (int)((v >> 8) & 0xff) << kAmbFixBits;
                g_edge.bg[e][x][2] = (int)(v & 0xff) << kAmbFixBits;
            }
    }

    int ncross[2] = {0, 0};
    for (int e = 0; e < 2; ++e)
        for (int x = 0; x < 256; ++x) {
            const uint32_t p = g_edge_live[e][x];
            const int ch[3] = {(int)((p >> 16) & 0xff), (int)((p >> 8) & 0xff),
                               (int)(p & 0xff)};
            for (int i = 0; i < 3; ++i)
                g_edge.bg[e][x][i] += amb_step(
                    (ch[i] << kAmbFixBits) - g_edge.bg[e][x][i],
                    kAmbRestNum, kAmbRestDen);
            const int bc[3] = {g_edge.bg[e][x][0] >> kAmbFixBits,
                               g_edge.bg[e][x][1] >> kAmbFixBits,
                               g_edge.bg[e][x][2] >> kAmbFixBits};
            g_edge_bg_px[e][x] = 0xFF000000u | ((uint32_t)bc[0] << 16) |
                                 ((uint32_t)bc[1] << 8) | (uint32_t)bc[2];
            int dv = 0;
            for (int i = 0; i < 3; ++i) {
                const int d = ch[i] > bc[i] ? ch[i] - bc[i] : bc[i] - ch[i];
                if (d > dv) dv = d;
            }
            const int w = !band_fix_on()  ? 0
                          : dv <= kEdgeLo ? 0
                          : dv >= kEdgeDev ? 255
                          : (dv - kEdgeLo) * 255 / (kEdgeDev - kEdgeLo);
            g_edge_w[e][x] = (uint8_t)w;
            if (w) ++ncross[e];
        }

    /* ---- A CROSSING IS LOCAL, AND A FADE IS NOT --------------------------
     *
     * The deviation test cannot tell "something is passing through this row"
     * from "this whole row is changing", and the second happens on every scene
     * transition: a fade moves every column at once, past every threshold, for
     * as long as the fade runs. Left alone, that has the band do exactly the
     * two things this lane exists to stop -- the wash blends toward a settled
     * value the fade has left behind, so the band TRAILS the picture, and the
     * carry paints the whole width of the band with the edge rows.
     *
     * So the width of the disturbance is the test, and it separates cleanly.
     * MEASURED, run mg15 lane BAND, scene 372 and scene 384 windowed over 600
     * frames each: every real crossing moved between 16 and 44 of the 256
     * columns of an edge (6% to 17%), while the scene's own opening fade moved
     * 126 to 256 of them (49% to 100%). A third of the row is a wide margin
     * either side of that gap.
     *
     * ABOVE IT THE WHOLE TERM STANDS DOWN FOR THAT EDGE, which puts the wash
     * back on the live row exactly -- the picture this file composed before
     * this lane, following the fade the way it always has -- and carries
     * nothing. An object genuinely wide enough to cover a third of the screen
     * while crossing would be refused too; nothing in these games is, and a
     * band that occasionally declines to draw a ghost is a far smaller thing
     * than a band that smears itself over every scene change. */
    for (int e = 0; e < 2; ++e)
        if (ncross[e] > 256 / 3) {
            std::memset(g_edge_w[e], 0, sizeof g_edge_w[e]);
            ncross[e] = -ncross[e];   /* the trace reports it as a refusal */
        }

    ++g_band_frame;
    g_band_carried = 0;
    if (band_trace_on()) {
        const int g = band_game_g(lay);
        /* A NEGATIVE COUNT IS THE WIDTH REFUSAL, not a smaller number: that
           edge saw the disturbance and declined to call it a crossing, and a
           reader has to be able to tell it from an edge that saw nothing. */
        std::fprintf(stderr, "[band] f%u scene G %d DS rows, layout draws %d "
                     "(%d host), upper engine writes band rows 0..%d, lower "
                     "%d..%d; crossers: upper edge %d/256 columns, lower edge "
                     "%d/256%s\n", g_band_frame, lay.game_g_ds, lay.gap_ds,
                     lay.band_h, g - 1, lay.gap_ds - g, lay.gap_ds - 1,
                     ncross[0], ncross[1],
                     band_fix_on() ? "" : "  [SM64DS_BAND_FIX=0]");
    }
}

void band_fill_ambient(uint32_t *dst, int dst_w, const StackLayout &lay)
{
    const uint32_t *top_edge = dst + (size_t)(lay.band_y - 1) * dst_w;
    const uint32_t *bot_edge = dst + (size_t)lay.bottom_y * dst_w;
    RGB tops[kAmbCols], bots[kAmbCols];
    RGB raw_t[kAmbCols], raw_b[kAmbCols];
    int centre[kAmbCols];
    for (int c = 0; c < kAmbCols; ++c) {
        int s0, s1, x0, x1;
        amb_sample_span(lay.w, c, s0, s1);
        amb_col_span(lay.w, c, x0, x1);
        raw_t[c] = amb_avg_bg(top_edge, s0, s1, g_edge_obj[0], g_edge_w[0],
                              g_edge_bg_px[0], lay.w);
        raw_b[c] = amb_avg_bg(bot_edge, s0, s1, g_edge_obj[1], g_edge_w[1],
                              g_edge_bg_px[1], lay.w);
        centre[c] = (x0 + x1) / 2;
    }

    /* SEEDED FROM THIS FRAME'S OWN SAMPLE, once per scene. See amb_seed. */
    if (!g_amb.have) {
        for (int c = 0; c < kAmbCols; ++c) {
            amb_seed(0, c, raw_t[c]);
            amb_seed(1, c, raw_b[c]);
        }
        g_amb.have = 1;
    }
    for (int c = 0; c < kAmbCols; ++c) {
        tops[c].r = amb_follow(0, c, 0, raw_t[c].r);
        tops[c].g = amb_follow(0, c, 1, raw_t[c].g);
        tops[c].b = amb_follow(0, c, 2, raw_t[c].b);
        bots[c].r = amb_follow(1, c, 0, raw_b[c].r);
        bots[c].g = amb_follow(1, c, 1, raw_b[c].g);
        bots[c].b = amb_follow(1, c, 2, raw_b[c].b);
    }

    ++g_amb_frame;
    if (amb_trace_on()) {
        std::fprintf(stderr, "[ambtrace] f%u", g_amb_frame);
        amb_trace_list("t_raw", raw_t);
        amb_trace_list("t_out", tops);
        amb_trace_list("b_raw", raw_b);
        amb_trace_list("b_out", bots);
        std::fprintf(stderr, "\n");
    }

    /* the two endpoint colours for every column of the image, resolved once
       rather than per row: the horizontal interpolation does not depend on y,
       and doing it inside the row loop would be band_h times the work for the
       same answer. Outside the first and last centres the end column's own
       value stands, which is the honest edge behaviour -- there is nothing
       beyond it to interpolate towards. */
    for (int x = 0; x < lay.w; ++x) {
        RGB t, b;
        if (x <= centre[0]) {
            t = tops[0];
            b = bots[0];
        } else if (x >= centre[kAmbCols - 1]) {
            t = tops[kAmbCols - 1];
            b = bots[kAmbCols - 1];
        } else {
            int c = 0;
            while (c + 1 < kAmbCols && centre[c + 1] <= x) ++c;
            const int span = centre[c + 1] - centre[c];
            const int u = x - centre[c], v = span - u;
            t.r = (tops[c].r * v + tops[c + 1].r * u) / span;
            t.g = (tops[c].g * v + tops[c + 1].g * u) / span;
            t.b = (tops[c].b * v + tops[c + 1].b * u) / span;
            b.r = (bots[c].r * v + bots[c + 1].r * u) / span;
            b.g = (bots[c].g * v + bots[c + 1].g * u) / span;
            b.b = (bots[c].b * v + bots[c + 1].b * u) / span;
        }
        const int den = 2 * lay.band_h;
        for (int k = 0; k < lay.band_h; ++k) {
            const int num = 2 * k + 1;
            const int r = (t.r * (den - num) + b.r * num) / den;
            const int g = (t.g * (den - num) + b.g * num) / den;
            const int bl = (t.b * (den - num) + b.b * num) / den;
            dst[(size_t)(lay.band_y + k) * dst_w + x] =
                0xFF000000u | ((uint32_t)r << 16) | ((uint32_t)g << 8) |
                (uint32_t)bl;
        }
    }
}

void band_flat(uint32_t *dst, int dst_w, const StackLayout &lay, uint32_t c)
{
    for (int k = 0; k < lay.band_h; ++k) {
        uint32_t *out = dst + (size_t)(lay.band_y + k) * dst_w;
        for (int x = 0; x < lay.w; ++x) out[x] = c;
    }
}

/* THE SCENE'S OWN BAND, PAINTED OVER WHATEVER IS BEHIND IT.
 *
 * lay.art is 256 x gap_ds DS pixels and the band is drawn at the same integer
 * scale the bottom half is, so one art pixel is one DS pixel and nothing is
 * interpolated -- the same decision the sub framebuffer's own blit makes, for
 * the same reason: every tier's ratio is a whole number and a filter would only
 * invent pixels nobody drew.
 *
 * ALPHA 0 IS THE MAGENTA KEY and it is a SKIP, not a blend. The artist's
 * transparent pixel means "leave this one to whatever is behind me", so the
 * backdrop this was called after -- black under peek, the ambient wash
 * otherwise -- is what stays there. There is no partial alpha: the key is one
 * exact colour in the file and this is one exact test. */
void band_art(uint32_t *dst, int dst_w, const StackLayout &lay)
{
    const int rx = lay.w / SUB_W, ry = lay.scale;
    for (int k = 0; k < lay.gap_ds; ++k)
        for (int x = 0; x < 256; ++x) {
            const uint32_t p = lay.art[(size_t)k * 256 + x];
            if (!(p >> 24)) continue;                 /* the key: leave it */
            for (int oy = 0; oy < ry; ++oy) {
                uint32_t *out =
                    dst + (size_t)(lay.band_y + k * ry + oy) * dst_w;
                for (int ox = 0; ox < rx; ++ox)
                    out[x * rx + ox] = p | 0xFF000000u;
            }
        }
}

/* WHAT GOES BEHIND THE BAND'S CONTENT, and PEEK IGNORES THE FILL MODE.
 *
 * With peek off the band is decoration and the player picks it: an ambient wash
 * of the two pictures around it, a flat colour, or the scene's own hand-drawn
 * art over the ambient wash.
 *
 * With peek on it is not decoration any more, it is a view of what is actually
 * there, and the true state of a band row with no sprite in it is EMPTY. A
 * chosen FILL behind that content would be this program drawing something over
 * the answer and calling the mixture the answer: an ambient wash blurs into the
 * sprites it sits behind, and a flat colour cannot be told apart from a sprite
 * of the same colour. So neither fill runs under peek.
 *
 * THE ART DOES, and that is a different question rather than an exception to
 * this one. A hand-drawn band is read as a picture -- a hinge, a machine, a
 * painted backing -- and nobody mistakes it for the game submitting a sprite,
 * which is exactly what an ambient wash or a flat colour invites. So peek's
 * backdrop is the scene's art when there is art and BLACK when there is not,
 * and every pixel over that backdrop is still only what the engines really
 * submitted plus what the continuity pass re-rendered from their own entries.
 *
 * So GapFillMode and GapColor apply only when GapPeek is false, the art applies
 * either way, and the settings note in hal/host_settings.cpp says so in those
 * words. */
void band_fill(uint32_t *dst, int dst_w, const StackLayout &lay)
{
    if (lay.band_h <= 0) return;
    /* THE FRAME'S EDGE READING FIRST, and unconditionally, because two later
       passes read it and only one of them is a fill mode. See THE EDGE ROWS'
       SETTLED BACKGROUND: the wash needs the crosser mask, and band_ghost --
       which runs after every fill mode, not only after the ambient one --
       needs the mask AND the live colours behind it. Taking it once here is
       also what keeps the two answers the same answer.

       IT COSTS THE TWO OAM ROW RASTERS the ambient fill already paid for, now
       paid on the solid and custom fills too. That is one 128-entry walk per
       engine over ONE row, which band_edge_obj_masks has always been, and it
       buys those modes a band that shows a crosser instead of swallowing
       one. */
    band_edge_update(dst, dst_w, lay);
    /* THE WORLD-BAND: the GaplessMinigames mode. These rows are playfield,
       not decoration. The engine's own backdrop colour was tried first and
       measured wrong by eye on scene 368 -- palette entry zero is WHITE
       there, a stark stripe between two skies. The background-only ambient
       wash is what actually reads as the scenery continuing through the
       hinge (it is built from the two adjacent background rows and nothing
       else, see amb_avg_bg), so it is the world-band's backdrop, and the
       crossing objects go over it CRISP via the peek raster in the compose
       -- not blurred: these rows are world, and the thing crossing them is
       really there. The chosen fill mode, the scene art and the flat colour
       all stand down: this band is not the player's decoration. */
    if (lay.world_band) {
        /* ART-CONTINUED BACKDROP, not the wash: the wash is the gap-mode
           glow and the owner reads it as a gap wherever it appears. These
           rows are playfield, so each column continues the scene's own art:
           the per-column MEDIAN of the 24 composed rows above the band,
           which follows vertical art structure (brick pillars, curtains)
           and throws away anything transient passing through the sample,
           so a falling coin cannot smear itself into the backdrop. */
        for (int x = 0; x < lay.w; ++x) {
            unsigned char rs[24], gs[24], bs[24];
            for (int k = 0; k < 24; ++k) {
                const uint32_t p =
                    dst[(size_t)(lay.band_y - 24 + k) * dst_w + x];
                rs[k] = (unsigned char)(p >> 16);
                gs[k] = (unsigned char)(p >> 8);
                bs[k] = (unsigned char)p;
            }
            for (int a = 0; a < 24; ++a)
                for (int b2 = a + 1; b2 < 24; ++b2) {
                    unsigned char t;
                    if (rs[b2] < rs[a]) { t = rs[a]; rs[a] = rs[b2]; rs[b2] = t; }
                    if (gs[b2] < gs[a]) { t = gs[a]; gs[a] = gs[b2]; gs[b2] = t; }
                    if (bs[b2] < bs[a]) { t = bs[a]; bs[a] = bs[b2]; bs[b2] = t; }
                }
            const uint32_t c = 0xFF000000u | ((uint32_t)rs[12] << 16) |
                               ((uint32_t)gs[12] << 8) | (uint32_t)bs[12];
            for (int k = 0; k < lay.band_h; ++k)
                dst[(size_t)(lay.band_y + k) * dst_w + x] = c;
        }
        return;
    }
    if (lay.peek) {
        band_flat(dst, dst_w, lay, 0xFF000000u);
        if (lay.art) band_art(dst, dst_w, lay);
        return;
    }
    if (lay.fill_mode == GAP_FILL_SOLID) {
        band_flat(dst, dst_w, lay, lay.fill_color | 0xFF000000u);
        return;
    }
    /* AMBIENT IS ALSO CUSTOM'S BACKDROP. A custom band with no art file is an
       ambient band -- that is the fallback -- and a custom band whose art is
       transparent in places shows the ambient wash through those places, which
       is what "the default backdrop" means in the file contract. */
    band_fill_ambient(dst, dst_w, lay);
    if (lay.fill_mode == GAP_FILL_CUSTOM && lay.art)
        band_art(dst, dst_w, lay);
}

/* ---- PEEK: the band's own hidden sprites -----------------------------------
 *
 * WHAT IS ACTUALLY IN THERE, and this is measured rather than assumed (run mg5
 * lane GAPPROBE, its OAM census over 900 frames of six minigames).
 *
 * Take world Y with its origin at the top row of the BOTTOM screen. The bottom
 * screen is rows 0..191, the band is rows -G..-1, and the top screen is
 * -(192+G)..-(G+1). The two engines' own row numbering then puts the SAME band
 * rows in two places:
 *
 *     main engine row 192 + k   ==   sub engine row -G + k   ==   world -G + k
 *
 * so a band row exists in both engines' coordinate systems and neither engine's
 * hardware draws it. What reaches OAM is decided by OAM::Render's cull, which
 * is `if (y + h < 0) return; if (y > 0xc0) return;` and then stores y & 0xff:
 *
 *   * on the MAIN engine a sprite that straddles the bottom edge of the top
 *     screen is submitted whole, so its rows past 191 are real OAM rows in
 *     192..191+G. Scene 376's two hinge sprites are exactly this: 128 rows tall
 *     at y 80 and 85, so 16 and 21 of their rows are band rows.
 *   * on the SUB engine a sprite that runs off the TOP of the bottom screen
 *     wraps into y >= 256 - h, which the DS reads as a negative row, so its
 *     rows are real OAM rows in -G..-1.
 *
 * and BETWEEN those two there is a hole. An object whose sprite ORIGIN sits at
 * world -G + k is submitted to the main engine as y = 192 + k, which the cull
 * throws away for every k >= 1, and to the sub engine as y = -G + k, which the
 * cull throws away until k >= G - h. So origins in k = 1 .. G-h-1 reach NEITHER
 * engine and the object is simply not drawn -- 23 rows of it for scene 374's
 * 8x8 dots at G = 32. THAT IS THE ROM'S OWN BEHAVIOUR, not a port defect, and
 * peek mode reproduces it by construction: this pass draws what the engines
 * were given and invents nothing, so the hole stays empty.
 *
 * A SECOND PASS RATHER THAN A TALLER RASTER. The two live rasterisers
 * (raster_obj above, engine B, and hal/message_compositor.cpp's, engine A) run
 * every frame on every path in the program, and growing their buffers by G rows
 * would put this off-by-default feature's cost and its risk on every level. This
 * walks OAM once more, only over the band's rows, only when peek is on.
 *
 * WHERE THE TWO ENGINES OVERLAP, ENGINE B WINS. Both engines address the same
 * world rows here, so where both have a pixel they are two submissions of the
 * same object and either is right; a rule is needed only so the answer does not
 * depend on OAM index luck. B is drawn second because the band is the bottom
 * screen's own coordinate space continued upward with no offset term -- sub row
 * -G + k IS band row k -- while the main engine's contribution needs the 192
 * subtraction, and when two answers are equally right the one with less
 * arithmetic between it and the pixel is the one to trust.
 */
struct BandPixel {
    uint32_t color;
    uint8_t prio;
    uint8_t hit;
    uint8_t eng;    /* which engine wrote it; see the overwrite test */
};

/* THE ENGINE BINDING: which 2D unit a band pass reads, and where that unit's
   registers, OAM, OBJ VRAM and palettes live. Three passes take one of these
   now -- the two peek rasters and the continuity re-render -- so "engine A,
   biased by -192" is spelled once at each call site rather than as six loose
   arguments at each of them. */
/* ---- WHAT THE TOP SCREEN IS SHOWING, and it is not what engine A's OAM says
 *
 * DECLARED HERE, ABOVE THE BAND MACHINERY, because two passes need it now. See
 * THE TWO SCREENS ARE ONE FRAME APART further down for the measurement: this
 * program composites engine A BEFORE the OAM upload and rasters engine B after
 * it, so the top screen is upload N-1 and the bottom screen is upload N.
 * ppu_seam_oam_mark copies engine A's OAM immediately before the upload, which
 * makes this copy the state the top screen was really drawn from.
 * [The sentence above describes SM64DS_OAM_LOAD_LATE=1. By default the upload
 * is at func_02019144's own line, ahead of both rasters, both screens are
 * upload N, and ppu_seam_oam_mark_uploaded takes the same copy from after the
 * upload. Either way this holds what the top half was really drawn from, which
 * is the only property the two passes below need. Run mg13 lane BNP.]
 *
 * The seam straddle pass reads it to decide what the top half has. hinge_paint
 * reads it because the band rows it draws are the top screen's own picture
 * continued past its bottom edge, and a continuation taken from the LIVE OAM
 * would be one frame ahead of the screen it continues. That is not the existing
 * top-versus-bottom lag, it would be a second one, inside a single engine's own
 * output, at the boundary a player is watching an object cross. */
uint8_t g_oam_a_shown[1024];
int g_oam_a_have;
/* engine B's copy of the same moment, for the seam pass's bottom-side
   lookup: since the upload moved below the bottom scanout, the LIVE sub OAM
   at seam time is one frame ahead of what either screen displays. */
uint8_t g_oam_b_shown[1024];
int g_oam_b_have;

uint16_t shown_rd16_b(int off)
{
    return (uint16_t)((unsigned)g_oam_b_shown[off] |
                      ((unsigned)g_oam_b_shown[off + 1] << 8));
}

uint16_t shown_rd16(int off)
{
    return (uint16_t)((unsigned)g_oam_a_shown[off] |
                      ((unsigned)g_oam_a_shown[off + 1] << 8));
}

/* ---- THE ROUTED SET, in the three states the OAM itself is in ---------------
 *
 * See ppu_obj_routed_record in ntr/ppu.h for what a mark IS and why it cannot
 * be derived at the raster. These are the storage, and they are HERE, four
 * lines under g_oam_a_shown, because that is the invariant: a mark is only
 * meaningful against the OAM block it was made for, and the three blocks are
 * the shadow being filled, the block hardware OAM holds, and the block the top
 * screen was drawn from. ppu_seam_oam_mark rotates the marks in the same
 * statement it takes the snapshot, so the two can never drift apart.
 *
 * The residual is 0 or the scene's G_rom, so a signed char is the type and not
 * a narrowing: G_rom is 32 on every gapless scene the port has proven and
 * GAP_DS_MAX bounds it well inside a byte either way. */
signed char g_obj_resid_shadow[128];
signed char g_obj_resid_live[128];
signed char g_obj_resid_shown[128];
uint8_t g_obj_routed_shadow[128];
uint8_t g_obj_routed_live[128];
uint8_t g_obj_routed_shown[128];
/* THE LAST SLOT A ROUTED SUBMISSION TOOK, and it is a fill detector rather than
   a statistic. OAM::Render hands out slots from a counter that OAM::Reset puts
   back to zero, so within one fill the slots a router takes only ever ascend; a
   slot that does not is the first routed submission of a NEW fill and the marks
   from the last one have to go before it lands. The rotation below clears the
   shadow every frame that uploads, which is every frame on the frame path, and
   this is what covers a frame that does not. */
int g_obj_routed_last = -1;

struct BandEngine {
    uint32_t reg, oam, vram, pltt, ext;
    int row_bias;        /* engine row -> band index: -192 (A), +gap_ds (B) */
    uint8_t id;
    const char *name;
    /* NON-NULL MEANS READ THE SNAPSHOT ABOVE INSTEAD OF THIS ENGINE'S LIVE OAM,
       attribute words and affine matrices alike. Only engine A ever has one,
       only hinge_paint sets one, and every other binding leaves it null and
       reads the hardware, which is what every pass here did before. */
    const uint8_t *shadow;
    /* THE PER-ENTRY TERMS, and both are null on every binding that predates the
       per-entry correction, which leaves band_row_of below exactly the
       `d.y + e.row_bias` it has always been.

       resid   per OAM slot, the DS rows this entry still has to move down by.
               Added to row_bias for THAT SLOT ONLY. See ppu_obj_routed_record.
       routed  per OAM slot, 1 if the framework's router placed this entry in
               world coordinates. A binding that carries one SKIPS every other
               entry, because a screen-space entry has no world row at all and a
               pass that reads one out of it is reading a number that does not
               exist. hinge_paint is the binding that needs this: without it the
               score row, which lives at engine y 168 in both arms, is drawn
               into the band -- measured, and it is the picture that falsified
               the layer shift. */
    const signed char *resid;
    const uint8_t *routed;
};

/* THE ONE PLACE AN ENTRY'S ROW IS TURNED INTO A BAND OR WORLD ROW. Every pass
   below asked `d.y + e.row_bias` for this; the per-entry correction adds one
   term that is per slot rather than per engine, and it is added here so that
   the census, the pairing test, the completion and the two band rasters cannot
   answer the question differently from each other. */
inline int band_row_of(const BandEngine &e, int slot, int y)
{
    return y + e.row_bias + (e.resid ? (int)e.resid[slot] : 0);
}

/* ---- WHICH SIDE OF THE BAND AN ENGINE IS ON ---------------------------------
 *
 * `row_bias` was never a property of an engine. It is a statement about which
 * of the two screens that engine is DRIVING, and every binding below spelled it
 * as a literal because until POWCNT1 was read the answer could not change.
 *
 * The engine on the UPPER screen has the band BELOW its own rows: its engine
 * row 192 is band row 0, so the bias is -192. The engine on the LOWER screen
 * has the band ABOVE its rows: its engine row -gap_ds is band row 0, so the
 * bias is +gap_ds. POWCNT1 bit 15 decides which engine is which and nothing
 * else about either binding moves -- the register base, the OAM, the VRAM and
 * the palettes all still belong to the engine that owns them.
 *
 * WITH THE BIT SET (lay.main_lower == 0) these return exactly the literals the
 * bindings carried before this existed, which is the whole of the zero-change
 * guarantee for every scene that never writes the register. */
inline int band_bias_a(const StackLayout &lay)
{
    return lay.main_lower ? lay.gap_ds : -192;
}

inline int band_bias_b(const StackLayout &lay)
{
    return lay.main_lower ? -192 : lay.gap_ds;
}

/* ---- THE GAME'S OWN G, AND WHY THE BAND'S HEIGHT IS NOT IT ANY MORE --------
 *
 * The owner's 2026-08-26 ruling draws EVERY minigame's hinge at 32 DS rows
 * (hal/screen_gap.cpp's ONE HINGE block), display only: the framework word is
 * not written, so a game whose own G is 16 -- the dScMgD3DBase_c four, 372,
 * 373, 384 and 385 -- still submits every sprite at a2 + 0xc0 + 16 while this
 * band is 32 rows tall. The ruling says so itself and asks for a report if it
 * is ever visible. This is that report's arithmetic, and the guard for it.
 *
 * band_bias_a says the upper screen's engine has band row 0 at its own engine
 * row 192, which is true. Its engine row 192 + k is WORLD row k - G_game, and
 * the band means its row k to be world k - gap_ds; the two agree only when
 * G_game IS the band's height. With G_game 16 in a 32-row band that engine's
 * rows 192+16 .. 192+31 are world 0 .. 15 -- rows THE LOWER SCREEN IS ALREADY
 * SHOWING -- so the raster would draw the bottom screen's own first sixteen
 * rows of sprites into the band's lower half a second time. Inventing picture,
 * not merely misplacing it.
 *
 * SO EACH ENGINE WRITES ONLY THE G_game ROWS NEAREST ITS OWN EDGE. The upper
 * screen's engine has world -G_game..-1 immediately below its last row: band
 * rows [0, G_game). The lower screen's has the same world rows immediately
 * above its first: band rows [gap_ds - G_game, gap_ds). Between them, when the
 * band is taller than G_game, are rows NO ENGINE HAS at all, and the ghost's
 * blur is what crosses them.
 *
 * LATENT RATHER THAN FILMED, and said plainly because the distinction is the
 * whole weight of this guard. MEASURED, run mg15 lane BAND: on scenes 372 and
 * 384, at every frame captured across a crossing, SM64DS_GAP_PEEK_TRACE=1
 * printed NO census line -- no OAM entry's box reaches the band on this family
 * at all -- and the band's pixels re-derived byte-exact from the ambient fill
 * alone. So the duplicate has not been seen; it is one routed 2D sprite away
 * from being seen, and it costs two comparisons to make impossible.
 *
 * WITH G_game == gap_ds -- every game that is not one of the four, and every
 * capture taken before the ruling -- both windows are the whole band and every
 * pass below is byte for byte what it was. A layout nobody told (game_g_ds 0,
 * which is every layout stack_layout builds and never hands to hal) is the
 * same. */
int band_game_g(const StackLayout &lay)
{
    if (lay.game_g_ds <= 0 || lay.game_g_ds >= lay.gap_ds) return lay.gap_ds;
    return lay.game_g_ds;
}

/* The band rows one binding may write, from which screen its engine is on.
   `upper` is what band_bias_a/b already decided; this is the same fact asked
   the other way, so the two can never disagree about a binding. */
inline void band_window(const StackLayout &lay, int upper, int &k0, int &k1)
{
    const int g = band_game_g(lay);
    if (upper) { k0 = 0; k1 = g; }
    else { k0 = lay.gap_ds - g; k1 = lay.gap_ds; }
}

/* ---- THE PASSES THAT ARE ENGINE A's BY CONSTRUCTION, UNDER A SWAP ----------
 *
 * Three of the passes below are not "the upper screen's engine" but ENGINE A's
 * specifically: the headroom strip, the hinge and the two seam passes all read
 * g_oam_a_shown, the per-entry residual marks, or engine A's own submission
 * geometry, and every one of those is a fact about the MAIN engine rather than
 * about a screen position. Re-pointing them at engine B would not be a swap, it
 * would be reading marks that describe a different engine's OAM.
 *
 * So under a swap they REFUSE rather than draw. That is a real behaviour
 * choice and it is made this way because drawing the wrong rows is worse than
 * drawing none: each of these passes exists to fill rows the engines could not
 * address, and a pass that fills them off the wrong engine's geometry puts
 * invented content into the picture.
 *
 * NO SCENE IN THE PROGRAM REACHES THIS TODAY. All four are gated on the
 * GaplessMinigames mod or its headroom (hal/screen_gap.cpp's kGaplessScenes:
 * 368, 374, 378, 366, 390), and none of those five writes POWCNT1 bit 15 --
 * the writers are the dScMgD3DBase_c family (372, 373, 384, 385) and 377.
 * The refusal is therefore unreachable, and it is here so that the day someone
 * adds a gapless row for a swapping scene the picture says so instead of
 * quietly gaining a wrong band.
 *
 * SAID ONCE PER PASS PER PROCESS, on stderr, because a silent refusal is the
 * thing this file's own notes keep calling a hazard. */
int swap_refuses(const StackLayout &lay, int which, const char *pass)
{
    static int said[4];
    if (!lay.main_lower) return 0;
    if (which >= 0 && which < 4 && !said[which]) {
        said[which] = 1;
        std::fprintf(stderr, "  [screens] %s STANDS DOWN: POWCNT1 bit 15 is "
                     "clear, so engine A is driving the LOWER screen, and this "
                     "pass is bound to engine A's own submission geometry "
                     "rather than to a screen position. It draws nothing this "
                     "frame rather than drawing engine A's rows in the wrong "
                     "half.\n", pass);
        std::fflush(stderr);
    }
    return 1;
}

/* THE PER ENTRY CORRECTION IN DS ROWS, spelled once. The band's world rows and
   the LAYER shift are two different questions about the same rows, and the
   difference between them is how much of the correction was made per entry:
   the whole of it in the default, none of it in the falsified layer arm, and
   zero either way in a layout with no band. Every binding below that needs to
   know which arm it is in asks this rather than testing an environment
   variable, because the layout is the one thing every pass already has. */
inline int per_entry_ds(const StackLayout &lay)
{
    return lay.obj_shift_ds - lay.obj_raster_ds;
}

/* One OAM halfword of a binding, from wherever that binding says its OAM is.
   The two readers below are the ONLY places a band pass touches OAM storage, so
   a binding that carries a snapshot carries it into the entries AND into the
   affine matrices, which are OAM words too and would otherwise be read live
   under a snapshot's attributes. */
uint16_t band_oam16(const BandEngine &e, unsigned off)
{
    return e.shadow ? (uint16_t)((unsigned)e.shadow[off] |
                                 ((unsigned)e.shadow[off + 1] << 8))
                    : rd16(e.oam + off);
}

/* What one OAM entry decodes to, before a texel is read. The BOX is what the
   hardware scans -- twice the sprite in each axis for a double-size affine
   entry -- and the sprite is what the tiles are. */
struct BandEntry {
    int x, y;            /* the BOX's left and top edge, in engine pixels */
    int w, h;            /* the SPRITE's size */
    int bw, bh;          /* the BOX's size */
    int affine, dbl;
};

/* Geometry and drawability, and nothing that reads VRAM. 0 means the entry
   draws nothing anywhere and every caller skips it. */
int band_decode(uint16_t a0, uint16_t a1, uint16_t a2, BandEntry &o)
{
    static const int kSizes[3][4][2] = {
        {{8, 8}, {16, 16}, {32, 32}, {64, 64}},
        {{16, 8}, {32, 8}, {32, 16}, {64, 32}},
        {{8, 16}, {8, 32}, {16, 32}, {32, 64}},
    };
    /* THE EMPTY SLOT, AND IT IS PARKED IN THE BAND. OAM::Reset (matched
       src/_ZN3OAM5ResetEv.cpp) clears all 128 entries to attribute word
       0x000000c0 and attribute 2 zero -- y = 0xc0, x = 0, tile 0, palette
       0, 8x8, mode normal, NOT the disable bit. That works on hardware
       because 0xc0 is 192, the first row BELOW the visible screen, so an
       unused slot is hidden by being parked exactly one row past the
       bottom edge. Which is exactly where this band starts.
       Measured, before this test was here: scene 376 drew 28 identical 8x8
       blobs of whatever happens to be in OBJ tile 0 across the top eight
       rows of the band, and scenes 368 and 374 the same. Every one of them
       was an empty slot.
       So the exact reset triple is skipped, and only that triple: a real
       sprite that genuinely sits at y 192 has an x, a tile or a palette,
       and keeps its pixels. */
    if (a0 == 0x00c0 && a1 == 0x0000 && a2 == 0x0000) return 0;
    const bool affine = (a0 & 0x100) != 0;
    if (!affine && (a0 & 0x200)) return 0;           // disabled
    const unsigned objmode = (a0 >> 10) & 3;
    if (objmode == 2 || objmode == 3) return 0;      // window mask; bitmap OBJ
    const int shape = (a0 >> 14) & 3;
    if (shape == 3) return 0;
    const int size = (a1 >> 14) & 3;
    o.w = kSizes[shape][size][0];
    o.h = kSizes[shape][size][1];
    o.affine = affine;
    o.dbl = affine && (a0 & 0x200) ? 1 : 0;
    o.bw = o.dbl ? o.w * 2 : o.w;
    o.bh = o.dbl ? o.h * 2 : o.h;
    o.x = a1 & 0x1FF;
    o.y = a0 & 0xFF;
    if (o.x >= 256) o.x -= 512;
    /* THE WRAP, and it is the same expression both live rasterisers use.
       This is what turns a sub-engine sprite parked at y 224..255 into the
       negative rows the band is made of, so getting it right here is the
       whole of whether the bottom half of the band has anything in it. */
    if (o.y >= 192 && o.y >= 256 - o.bh) o.y -= 256;
    return 1;
}

/* WHAT ONE ENTRY'S TEXELS ARE, decoded once instead of per pixel: the tile
 * addressing, the palette, the flips and the affine matrix. Two passes read
 * texels now -- the band's, below, and the seam straddle's at the bottom of
 * this file -- and the DS's OBJ tile addressing is the last thing in this file
 * that should exist in two transcriptions, so it exists in one and both read
 * it through band_texel. 0 means the entry draws nothing at all. */
struct BandTexels {
    uint32_t vram, pltt, objext;
    uint32_t boundary, tile, pal;
    int map1d, c256, hflip, vflip;
    int pa, pb, pc, pd;
    uint8_t prio;
};

int band_texels(const BandEngine &e, uint16_t a0, uint16_t a1, uint16_t a2,
                const BandEntry &d, BandTexels &o)
{
    const uint32_t dispcnt = rd32(e.reg);
    if (!((dispcnt >> 12) & 1)) return 0;            // OBJ layer off
    o.vram = e.vram;
    o.pltt = e.pltt;
    o.boundary = 32u << ((dispcnt >> 20) & 3);
    o.map1d = (dispcnt >> 4) & 1;
    o.objext = ((dispcnt >> 31) & 1) ? e.ext : 0;
    o.c256 = (a0 & 0x2000) != 0;
    o.hflip = !d.affine && (a1 & 0x1000);
    o.vflip = !d.affine && (a1 & 0x2000);
    o.tile = a2 & 0x3FF;
    o.pal = (a2 >> 12) & 0xF;
    o.prio = (a2 >> 10) & 3;

    o.pa = 256; o.pb = 0; o.pc = 0; o.pd = 256;
    if (d.affine) {
        const int grp = (a1 >> 9) & 0x1F;
        o.pa = (int16_t)band_oam16(e, (grp * 4 + 0) * 8u + 6);
        o.pb = (int16_t)band_oam16(e, (grp * 4 + 1) * 8u + 6);
        o.pc = (int16_t)band_oam16(e, (grp * 4 + 2) * 8u + 6);
        o.pd = (int16_t)band_oam16(e, (grp * 4 + 3) * 8u + 6);
    }
    return 1;
}

/* ONE TEXEL of a decoded entry. (sx, sy) walk the BOX, which for a double-size
   affine entry is twice the sprite in each axis. 0 means transparent, or a box
   pixel the matrix maps outside the sprite -- either way, nothing to draw. */
int band_texel(const BandTexels &t, const BandEntry &d, int sx, int sy,
               uint32_t &color)
{
    int tx, ty;
    if (d.affine) {
        const int cx = sx - d.bw / 2, cy = sy - d.bh / 2;
        tx = ((t.pa * cx + t.pb * cy) >> 8) + d.w / 2;
        ty = ((t.pc * cx + t.pd * cy) >> 8) + d.h / 2;
        if (tx < 0 || tx >= d.w || ty < 0 || ty >= d.h) return 0;
    } else {
        tx = t.hflip ? d.w - 1 - sx : sx;
        ty = t.vflip ? d.h - 1 - sy : sy;
    }
    const int tcol = tx >> 3, trow = ty >> 3;
    const int fx = tx & 7, fy = ty & 7;
    const uint32_t slot =
        t.map1d ? (uint32_t)(trow * (d.w / 8) + tcol) * (t.c256 ? 2u : 1u)
                : (uint32_t)(trow * 32 + (t.c256 ? tcol * 2 : tcol));
    const uint32_t cell = t.vram + t.tile * t.boundary + slot * 32u;
    if (t.c256) {
        const uint32_t idx = rd8(cell + fy * 8u + fx);
        if (!idx) return 0;
        color = t.objext ? bgr555(rd16(t.objext + (t.pal * 256u + idx) * 2u))
                         : bgr555(rd16(t.pltt + idx * 2u));
    } else {
        const uint8_t bb = rd8(cell + fy * 4u + fx / 2);
        const uint32_t idx = (fx & 1) ? (bb >> 4) : (bb & 0xF);
        if (!idx) return 0;
        color = bgr555(rd16(t.pltt + (t.pal * 16u + idx) * 2u));
    }
    return 1;
}

/* ONE ENTRY'S TEXELS INTO THE BAND, at a row the caller names.
 *
 * `ktop` is the band row the entry's BOX starts on, which for a live raster is
 * its own decoded y plus the engine's bias and for the continuity pass is where
 * the game says the object is right now. `only_empty` is the continuity pass's
 * rule: write nothing over a pixel an engine already drew.
 *
 * Every VRAM and palette read goes through the BandEngine, so a cached engine-A
 * entry re-rendered later still reads engine A's tiles through engine A's
 * palette, which is the whole of what makes the synthesized pixels the same
 * pixels the engine would have produced.
 *
 * `split` AND `n_below` ARE THE HEADROOM PASS'S CENSUS and nothing else reads
 * them, which is why they default to "no census". With split >= 0 the texels
 * this entry lands at band index >= split are counted into *n_below as well as
 * into the return, so ONE raster over ONE OAM walk reports "how much of this
 * sprite is in the strip" beside "how much of it is on the screen". Those two
 * numbers are the before and the after of the clipping the headroom fixes, and
 * taking them from the same frame of the same run is what makes them comparable
 * at all. Every other caller passes neither and is unchanged. */
/* `k0` / `k1` ARE THE BINDING'S OWN WINDOW, the band rows this engine may
   write, and every caller that does not have one passes the whole band. See
   THE GAME'S OWN G above: since the uniform-hinge ruling an engine's rows past
   its own G are the OTHER screen's picture, and a raster that wrote them would
   put a second copy of it in the band. */
int band_draw_entry(BandPixel *band, int gap_ds, const BandEngine &e,
                    uint16_t a0, uint16_t a1, uint16_t a2, const BandEntry &d,
                    int ktop, int x, int only_empty, int split = -1,
                    int *n_below = 0, int k0 = 0, int k1 = 1 << 24)
{
    BandTexels t;
    if (!band_texels(e, a0, a1, a2, d, t)) return 0;
    const uint8_t prio = t.prio;

    int drawn = 0;
    for (int sy = 0; sy < d.bh; ++sy) {
        const int k = ktop + sy;
        if (k < 0 || k >= gap_ds) continue;
        if (k < k0 || k >= k1) continue;
        for (int sx = 0; sx < d.bw; ++sx) {
            const int px = x + sx;
            if (px < 0 || px >= 256) continue;
            uint32_t color;
            if (!band_texel(t, d, sx, sy, color)) continue;
            BandPixel &bp = band[(size_t)k * 256 + px];
            /* THE CONTINUITY PASS NEVER OVERPAINTS AN ENGINE. Its whole claim
               is that it fills rows the engines did not draw, and a pass that
               could also change a pixel an engine DID draw would make that
               claim unverifiable from the picture. */
            if (only_empty && bp.hit) continue;
            ++drawn;
            if (split >= 0 && k >= split && n_below) ++*n_below;
            /* WITHIN one engine, OBJ priority resolves and the 127 -> 0
               walk breaks ties toward the lower index, exactly as
               raster_obj above. ACROSS the two engines the priority test
               holds too, and only the TIE rule differs: engine A runs
               first and engine B second, so on equal priority B takes the
               pixel -- B's upload is the fresher half of the seam, and for
               a routed object both engines' copies are the same pixels
               anyway. What B may NOT do any more is erase a better
               priority with a worse one. The routed minigames put both
               engines' entries on ONE priority scale, because both copies
               of every world object come from the same RenderOamBothScreens
               call with the same priority argument -- and OAM::Render's
               off-screen cull makes partial sets routine: measured on
               Shuffle Shell's two-sprite stack crossing at world row -20,
               the body (prio 1) was culled from engine B while the base
               (prio 2) wrapped in, and the unconditional cross-engine
               overwrite let that lone base paint over engine A's complete
               body -- the owner's "bottom layer of the shell goes in
               front of the top at the transition line". */
            if (bp.hit && prio > bp.prio) continue;
            bp.color = color;
            bp.prio = prio;
            bp.hit = 1;
            bp.eng = e.id;
        }
    }
    return drawn;
}

int band_trace_frames(void)
{
    /* SM64DS_GAP_PEEK_TRACE=1: one census line per peek frame, per engine, of
       every OAM entry whose BOX reaches the band and how many pixels of it
       actually landed there. The two numbers are not the same and the gap
       between them is the thing worth having: a double-size affine sprite has
       a box twice the size of anything it can draw, so an entry can be
       "submitted into the band" by every arithmetic you can do on OAM and
       still contribute nothing, because its texels are all in the middle of
       its own box. Scene 376's two 64x64 hinge sprites at double size are
       exactly that case, and without this line the only way to tell them apart
       from a broken raster is to read the matrix by hand.

       The continuity pass prints on the same switch, one line per tracked
       object per frame, because the question it answers -- was this object
       drawn by an engine, or synthesized, or neither -- is the same kind of
       question and is read off the same census. */
    static int on = -1;
    if (on < 0) {
        const char *s = std::getenv("SM64DS_GAP_PEEK_TRACE");
        on = s && *s && *s != '0';
    }
    return on;
}

/* One engine's sprites over the band, in DS pixels: band[k][x] for k in
 * [0, gap_ds).
 *
 * Priority resolves sprite against sprite exactly as raster_obj does above:
 * walk 127 -> 0 so a lower index is processed later, and overwrite when the
 * pixel is empty or this sprite's priority number is at least as good. */
void band_raster_engine(BandPixel *band, int gap_ds, const BandEngine &e,
                        int trace = -1, const char *tag = "gappeek",
                        int split = -1, int k0 = 0, int k1 = 1 << 24)
{
    if (!((rd32(e.reg) >> 12) & 1)) return;          // OBJ layer off
    if (trace < 0) trace = band_trace_frames();
    for (int i = 127; i >= 0; --i) {
        const uint16_t a0 = band_oam16(e, i * 8u);
        const uint16_t a1 = band_oam16(e, i * 8u + 2);
        const uint16_t a2 = band_oam16(e, i * 8u + 4);
        BandEntry d;
        if (!band_decode(a0, a1, a2, d)) continue;

        /* the BOX's reach into the band, before a single texel is read: the
           number every OAM-only analysis produces, kept beside the number this
           raster actually draws */
        /* A BINDING THAT NAMES THE ROUTED SET DRAWS NOTHING ELSE. See the
           routed field on BandEngine: a screen-space entry has no world row,
           and the band's rows are world rows. */
        if (e.routed && !e.routed[i]) continue;
        const int ktop = band_row_of(e, i, d.y);
        int box_rows = 0;
        for (int sy = 0; sy < d.bh; ++sy)
            if (ktop + sy >= 0 && ktop + sy < gap_ds) ++box_rows;

        int below = 0;
        const int drawn = band_draw_entry(band, gap_ds, e, a0, a1, a2, d, ktop,
                                          d.x, 0, split, &below, k0, k1);
        if (box_rows && trace)
            std::fprintf(stderr, "[%s] %s oam%3d a0=%04x a1=%04x a2=%04x "
                         "%dx%d%s at (%d,%d): box reaches %d row(s), %d "
                         "pixel(s) drawn%s\n", tag, e.name, i, a0, a1, a2,
                         d.w, d.h, d.dbl ? " dbl" : (d.affine ? " aff" : ""),
                         d.x, d.y, box_rows, drawn,
                         split < 0 ? "" : (below ? ", SOME OF IT ON THE SCREEN"
                                                 : ", ALL OF IT OFF-SCREEN"));
        /* THE SPLIT IS THE MEASUREMENT, printed on its own line so a log can be
           read with one grep: how much of this sprite the headroom strip shows
           and how much of it the top screen was already showing. Before the
           strip existed the first number was the number of pixels DROPPED. */
        if (split >= 0 && trace && (drawn || below))
            std::fprintf(stderr, "[%s] %s oam%3d split: %d px in the strip, "
                         "%d px on the screen\n", tag, e.name, i,
                         drawn - below, below);
    }
}

/* ---- CONTINUITY: the objects the game asks to be kept whole ----------------
 *
 * See the note over BandTrack in ntr/ppu.h for what this is and why it invents
 * nothing. The mechanism is two halves:
 *
 *   * WHILE AN ENGINE IS DRAWING THE OBJECT, its live OAM entry is found by
 *     matching the position and size the hook reports against the entries that
 *     engine really submitted, and the three attribute words are cached. The
 *     match is exact -- same box left edge, same band row for the box's top
 *     edge, same sprite size -- so an entry that merely happens to be nearby
 *     cannot be mistaken for the object.
 *   * WHILE NEITHER IS, the cached entry is re-rendered at the position the
 *     hook reports, into band pixels no engine wrote. That is the dead zone,
 *     and the pixels are the game's own tiles out of the same engine's VRAM
 *     through the same palette.
 *
 * A slot with no cached entry draws NOTHING. That is the honest answer for an
 * object that has not been submitted yet in this scene -- a ball still sitting
 * in the plunger has never been an OAM entry, and this pass has nothing to say
 * about what it would look like. */
struct BandCache {
    int have;
    uint16_t a0, a1, a2;
    BandEngine eng;
};

BandTrackFn g_track_fn;
BandCache g_track[BAND_TRACK_MAX];
/* PEEK FRAMES SINCE THE PROGRAM STARTED, and it is on the continuity lines
   rather than on the raster's because of what the two censuses are read for.
   The raster's is a list of what is in ONE frame and its reader is looking at a
   frame at a time. The continuity one is read as a SEQUENCE -- an object is
   drawn by an engine, then synthesized, then drawn again -- and "no frame in
   that sequence is missing" is not a claim a reader can make without a frame
   number to count. */
unsigned g_peek_frame;

int band_find_live(const BandEngine &e, const BandTrack &t, BandCache &out)
{
    for (int i = 127; i >= 0; --i) {
        const uint16_t a0 = rd16(e.oam + i * 8u);
        const uint16_t a1 = rd16(e.oam + i * 8u + 2);
        const uint16_t a2 = rd16(e.oam + i * 8u + 4);
        BandEntry d;
        if (!band_decode(a0, a1, a2, d)) continue;
        if (d.x != t.x || band_row_of(e, i, d.y) != t.y) continue;
        if (d.w != t.w || d.h != t.h) continue;
        out.have = 1;
        out.a0 = a0;
        out.a1 = a1;
        out.a2 = a2;
        out.eng = e;
        return 1;
    }
    return 0;
}

void band_continuity(BandPixel *band, int gap_ds, const BandEngine &ea,
                     const BandEngine &eb)
{
    if (!g_track_fn) return;
    BandTrack tr[BAND_TRACK_MAX];
    const int n = g_track_fn(tr, BAND_TRACK_MAX, gap_ds);
    for (int i = 0; i < n && i < BAND_TRACK_MAX; ++i) {
        const BandTrack &t = tr[i];
        if (t.slot < 0 || t.slot >= BAND_TRACK_MAX) continue;
        BandCache &c = g_track[t.slot];
        /* the box has to reach the band at all before anything else is asked.
           THE CENSUS SAYS SO ANYWAY, because "the object never came near the
           band" and "the object was in the band and nothing drew it" are the
           two answers a report about a vanishing object has to be told apart
           by, and without this line they look identical from outside. */
        if (t.y + t.h <= 0 || t.y >= gap_ds) {
            if (band_trace_frames())
                std::fprintf(stderr, "[gapcont] f%u slot %d %dx%d at (%d,%d): "
                             "outside the band\n", g_peek_frame, t.slot, t.w,
                             t.h, t.x, t.y);
            continue;
        }
        int live = band_find_live(ea, t, c);
        if (!live) live = band_find_live(eb, t, c);
        if (live) {
            if (band_trace_frames())
                std::fprintf(stderr, "[gapcont] f%u slot %d %dx%d at (%d,%d): "
                             "engine %s submitted it, attributes cached "
                             "a0=%04x a1=%04x a2=%04x\n", g_peek_frame,
                             t.slot, t.w, t.h, t.x, t.y, c.eng.name, c.a0,
                             c.a1, c.a2);
            continue;
        }
        if (!c.have) {
            if (band_trace_frames())
                std::fprintf(stderr, "[gapcont] f%u slot %d %dx%d at (%d,%d): "
                             "no engine has it and nothing is cached, drawing "
                             "nothing\n", g_peek_frame, t.slot, t.w, t.h,
                             t.x, t.y);
            continue;
        }
        BandEntry d;
        if (!band_decode(c.a0, c.a1, c.a2, d)) continue;
        const int drawn = band_draw_entry(band, gap_ds, c.eng, c.a0, c.a1,
                                          c.a2, d, t.y, t.x, 1);
        if (band_trace_frames())
            std::fprintf(stderr, "[gapcont] f%u slot %d %dx%d at (%d,%d): DEAD "
                         "ZONE, re-rendered engine %s a0=%04x a1=%04x a2=%04x, "
                         "%d pixel(s) synthesized\n", g_peek_frame, t.slot,
                         t.w, t.h, t.x, t.y, c.eng.name, c.a0, c.a1, c.a2,
                         drawn);
    }
}

/* THE EDGE ROWS' OBJ COVERAGE, one byte per DS column per row. Rastered off
   the two engines' own OAM exactly the way the band passes do -- engine A's
   screen row 191 is band index 0 under a bias of -191, engine B's row 0 under
   a bias of 0 -- so "an OBJ covers this column" is the same answer the screens
   themselves drew, not a guess from pixel colours.

   THE BIASES BELONG TO THE ROWS AND THE REGISTERS BELONG TO THE ENGINES, and
   POWCNT1's swap moves only the second half. mtop is the UPPER screen's last
   row whatever is feeding it, so it always wants bias -191; what changes is
   whether that row came out of engine A or engine B. */
void band_edge_obj_masks(uint8_t *mtop, uint8_t *mbot, int main_lower)
{
    static BandPixel rowbuf[256];
    const BandEngine a_upper = {kRegBaseA, kOamBaseA, kObjVramA, kObjPlttA, 0,
                                -191, 0, "A", 0};
    const BandEngine a_lower = {kRegBaseA, kOamBaseA, kObjVramA, kObjPlttA, 0,
                                0, 0, "A", 0};
    const BandEngine b_upper = {kRegBase, kOamBase, kObjVram, kObjPltt,
                                kObjExtPltt, -191, 1, "B", 0};
    const BandEngine b_lower = {kRegBase, kOamBase, kObjVram, kObjPltt,
                                kObjExtPltt, 0, 1, "B", 0};
    const BandEngine &up = main_lower ? b_upper : a_upper;
    const BandEngine &lo = main_lower ? a_lower : b_lower;
    std::memset(rowbuf, 0, sizeof rowbuf);
    band_raster_engine(rowbuf, 1, up);
    for (int x = 0; x < 256; ++x) mtop[x] = rowbuf[x].hit;
    std::memset(rowbuf, 0, sizeof rowbuf);
    band_raster_engine(rowbuf, 1, lo);
    for (int x = 0; x < 256; ++x) mbot[x] = rowbuf[x].hit;
}

/* ---- THE GHOST: what is crossing the gap, blurred behind the glow ----------
 *
 * The owner's spec, 2026-08-20: the wash is the BACKGROUND's glow (the masks
 * above), and a thing travelling through the gap should read as a blurred
 * shape passing behind it -- not as its colours flooding whole columns of the
 * wash, and not as a crisp sprite floating on decoration (the retired band
 * regime's look). So this rasters exactly what band_peek would show --
 * both engines' band rows plus the continuity pass's dead-zone entries, the
 * game's own tiles out of the game's own VRAM -- then box-blurs it and lays
 * it over the fill at three-quarter strength. Peek itself is untouched: peek
 * is the honest view and stays crisp.
 *
 * Radius 2 DS pixels, two separable passes, integers only. The grid is at
 * most GAP_DS_MAX x 256, so the cost is a band-sized blur once per composed
 * frame, only while a real band is on screen. */
void band_ghost(uint32_t *dst, int dst_w, const StackLayout &lay)
{
    if (lay.band_h <= 0 || lay.gap_ds <= 0) return;
    static BandPixel *band;
    static int *acc;    /* r,g,b,a quads, blur scratch, two planes */
    if (!band) {
        band = (BandPixel *)std::calloc((size_t)GAP_DS_MAX * 256,
                                        sizeof *band);
        acc = (int *)std::calloc((size_t)GAP_DS_MAX * 256 * 8, sizeof *acc);
        if (!band || !acc) return;
    }
    std::memset(band, 0, sizeof(BandPixel) * (size_t)lay.gap_ds * 256);
    /* the biases come from band_bias_a/b so the swap moves them together with
       every other band pass; with bit 15 set they are -192 and +gap_ds, which
       is the pair this shipped with */
    const BandEngine ea = {kRegBaseA, kOamBaseA, kObjVramA, kObjPlttA, 0,
                           band_bias_a(lay), 0, "A", 0};
    const BandEngine eb = {kRegBase, kOamBase, kObjVram, kObjPltt, kObjExtPltt,
                           band_bias_b(lay), 1, "B", 0};
    /* AND EACH ENGINE'S OWN WINDOW, because since the uniform-hinge ruling the
       band can be taller than the game's G and an engine's rows past its own G
       are the other screen's picture. See THE GAME'S OWN G above; with the two
       equal these are the whole band and the two calls are what they were. */
    int ak0, ak1, bk0, bk1;
    band_window(lay, !lay.main_lower, ak0, ak1);
    band_window(lay, lay.main_lower, bk0, bk1);
    band_raster_engine(band, lay.gap_ds, ea, -1, "gappeek", -1, ak0, ak1);
    band_raster_engine(band, lay.gap_ds, eb, -1, "gappeek", -1, bk0, bk1);
    band_continuity(band, lay.gap_ds, ea, eb);

    const int H = lay.gap_ds, W = 256, R = 2;
    int *p0 = acc, *p1 = acc + (size_t)GAP_DS_MAX * 256 * 4;
    int any = 0;
    for (int k = 0; k < H; ++k)
        for (int x = 0; x < W; ++x) {
            int *o = p0 + ((size_t)k * W + x) * 4;
            const BandPixel &bp = band[(size_t)k * W + x];
            if (bp.hit) {
                o[0] = (int)((bp.color >> 16) & 0xff);
                o[1] = (int)((bp.color >> 8) & 0xff);
                o[2] = (int)(bp.color & 0xff);
                o[3] = 255;
                any = 1;
            } else {
                o[0] = o[1] = o[2] = o[3] = 0;
            }
        }
    /* ---- THE CROSSER CARRY -------------------------------------------------
     *
     * THE OWNER'S RULING, 2026-08-26: a thing travelling through the screens
     * "should just blur as they pass through". The rasters above answer that
     * for anything the game submitted as an OAM entry, out of its own tiles,
     * at its own world row. They cannot answer it for the crosser the report
     * is actually about: on the dScMgD3DBase_c family Mario is 3D geometry on
     * the live engine and a display-capture bitmap on the other panel, and
     * neither is an OAM entry. MEASURED, and it is the whole finding of run
     * mg15 lane BAND -- scenes 372 and 384, every frame captured across a
     * crossing, ZERO band pixels of anything but the ambient fill and NO
     * gappeek census line at all. He is not misplaced in the band. He is not
     * in it.
     *
     * AND THERE IS NOTHING CRISP TO DRAW HIM WITH. The band's rows are world
     * rows below the 3D engine's own 192-row viewport; no geometry was
     * rasterised there, on hardware or here, so any crisp answer would be
     * invented. What IS real is the pixels he occupies in the two rows either
     * side of the band, which the two screen blits have already written into
     * this very image. Those get carried into the band and faded, and the box
     * blur below is what makes them a blur rather than a stretch.
     *
     * FROM ITS OWN EDGE, FADING TO THE FAR ONE, both edges independently. An
     * object leaving the upper screen fades DOWN the band while its arrival at
     * the lower screen fades UP, and the two cross over in the middle, so
     * there is no band row at which it is drawn by neither -- which is the law
     * this whole feature exists under: nothing may visibly vanish at the seam.
     *
     * ONLY WHERE THE RASTER LEFT THE GRID EMPTY, and the test is the stronger
     * alpha rather than a first-writer-wins: a real OAM entry above carries
     * 255, the carry's own maximum is 255 at its own edge, so `a <= o[3]`
     * refuses every carry over a rastered pixel by arithmetic instead of by
     * ordering, and the two edges' carries resolve against each other by which
     * one is nearer. */
    if (band_fix_on())
        for (int e = 0; e < 2; ++e)
            for (int x = 0; x < W; ++x) {
                const int cw = g_edge_w[e][x];
                if (!cw) continue;
                const uint32_t p = g_edge_live[e][x];
                const int cr = (int)((p >> 16) & 0xff);
                const int cg = (int)((p >> 8) & 0xff);
                const int cb = (int)(p & 0xff);
                for (int k = 0; k < H; ++k) {
                    const int d = e == 0 ? k : H - 1 - k;
                    /* THE FALLOFF TIMES THE COLUMN'S OWN WEIGHT, so a column
                       the wash still mostly trusts contributes a faint ghost
                       and one it has written off entirely contributes a full
                       one. Same number both passes read, so the band never
                       shows a ghost the gradient has not already stepped
                       aside for. */
                    const int a = 255 * (H - d) / H * cw / 255;
                    int *o = p0 + ((size_t)k * W + x) * 4;
                    if (a <= o[3]) continue;
                    o[0] = cr; o[1] = cg; o[2] = cb; o[3] = a;
                    any = 1;
                    ++g_band_carried;
                }
            }
    if (band_trace_on())
        std::fprintf(stderr, "[band] f%u ghost: %u band cell(s) carried from "
                     "the edge rows\n", g_band_frame, g_band_carried);
    if (!any) return;
    /* horizontal then vertical box, premultiplied by coverage so a blurred
       edge fades instead of smearing black */
    for (int k = 0; k < H; ++k)
        for (int x = 0; x < W; ++x) {
            int s[4] = {0, 0, 0, 0};
            int n = 0;
            for (int d = -R; d <= R; ++d) {
                const int u = x + d;
                if (u < 0 || u >= W) continue;
                const int *o = p0 + ((size_t)k * W + u) * 4;
                s[0] += o[0] * o[3]; s[1] += o[1] * o[3];
                s[2] += o[2] * o[3]; s[3] += o[3];
                ++n;
            }
            int *q = p1 + ((size_t)k * W + x) * 4;
            if (s[3]) {
                q[0] = s[0] / s[3]; q[1] = s[1] / s[3]; q[2] = s[2] / s[3];
                q[3] = s[3] / (n ? n : 1);
            } else {
                q[0] = q[1] = q[2] = q[3] = 0;
            }
        }
    for (int k = 0; k < H; ++k)
        for (int x = 0; x < W; ++x) {
            int s[4] = {0, 0, 0, 0};
            int n = 0;
            for (int d = -R; d <= R; ++d) {
                const int u = k + d;
                if (u < 0 || u >= H) continue;
                const int *o = p1 + ((size_t)u * W + x) * 4;
                s[0] += o[0] * o[3]; s[1] += o[1] * o[3];
                s[2] += o[2] * o[3]; s[3] += o[3];
                ++n;
            }
            int *q = p0 + ((size_t)k * W + x) * 4;
            if (s[3]) {
                q[0] = s[0] / s[3]; q[1] = s[1] / s[3]; q[2] = s[2] / s[3];
                q[3] = (s[3] / (n ? n : 1)) * 3 / 4;   /* behind the glow */
            } else {
                q[0] = q[1] = q[2] = q[3] = 0;
            }
        }
    const int rx = lay.w / SUB_W, ry = lay.scale;
    if (rx <= 0 || ry <= 0) return;
    for (int k = 0; k < H; ++k)
        for (int x = 0; x < W; ++x) {
            const int *q = p0 + ((size_t)k * W + x) * 4;
            const int a = q[3];
            if (!a) continue;
            for (int oy = 0; oy < ry; ++oy) {
                uint32_t *out = dst + (size_t)(lay.band_y + k * ry + oy) * dst_w;
                for (int ox = 0; ox < rx; ++ox) {
                    uint32_t *px = &out[x * rx + ox];
                    const int wr = (int)((*px >> 16) & 0xff);
                    const int wg = (int)((*px >> 8) & 0xff);
                    const int wb = (int)(*px & 0xff);
                    const int r = wr + (q[0] - wr) * a / 255;
                    const int g = wg + (q[1] - wg) * a / 255;
                    const int b = wb + (q[2] - wb) * a / 255;
                    *px = 0xFF000000u | ((uint32_t)r << 16) |
                          ((uint32_t)g << 8) | (uint32_t)b;
                }
            }
        }
}

void band_peek(uint32_t *dst, int dst_w, const StackLayout &lay)
{
    if (lay.band_h <= 0 || lay.gap_ds <= 0) return;
    /* GAP_DS_MAX rows of 256 DS pixels, allocated on the first peek frame and
       never freed -- the same trade hal/sub_screen.cpp makes for the stacked
       image itself, for the same reason. As a file-scope array it would be
       192 KB of host .bss in EVERY binary that links this layer, including
       every smoke and every level run, for a feature that is off by default;
       as a lazy allocation it is one null pointer until somebody switches peek
       on. Not the compose's stack for the same size reason. The compose runs
       on one thread on one path and never re-enters, so a static pointer is
       the whole of the bookkeeping. */
    static BandPixel *band;
    if (!band) {
        band = (BandPixel *)std::calloc((size_t)GAP_DS_MAX * 256,
                                        sizeof *band);
        /* no peek this run. band_fill has already painted the backdrop, which
           is an empty band rather than a wrong one: peek's own answer for a
           row with nothing in it. */
        if (!band) return;
    }
    std::memset(band, 0, sizeof(BandPixel) * (size_t)lay.gap_ds * 256);
    ++g_peek_frame;

    /* ENGINE A FIRST. On the upper screen its band rows are engine rows
       192..191+G, so the bias that turns an engine row into a band index is
       -192; under POWCNT1's swap it is the lower screen's engine and the bias
       is +G. band_bias_a carries that and nothing else about this binding
       moves. Engine A's OBJ extended palette store is not modelled anywhere in
       this program, so it passes 0 and a 256-colour engine-A sprite reads the
       standard palette -- the same answer ntr/ppu.cpp's own engine-A raster
       gives. */
    const BandEngine ea = {kRegBaseA, kOamBaseA, kObjVramA, kObjPlttA, 0,
                           band_bias_a(lay), 0, "A", 0};
    /* ENGINE B SECOND, so it wins where both drew; see the header note. Its
       band rows are engine rows -G..-1 on the lower screen, so the bias is +G,
       and -192 when the swap puts it on the upper one.

       THE DRAW ORDER IS NOT SWAPPED WITH THE BIASES, deliberately. "B second"
       is about the sub engine's rows being the ones a player is looking at
       through the stylus, which is a statement about the physical bottom screen
       and about the OWNER's judgment of the picture, not about which engine
       feeds it. No scene that swaps also has a band with both engines drawing
       into it today, so no picture in the program depends on this either way;
       when one does, it is the owner's call and not a derivation. */
    const BandEngine eb = {kRegBase, kOamBase, kObjVram, kObjPltt, kObjExtPltt,
                           band_bias_b(lay), 1, "B", 0};
    /* AND EACH ONE'S OWN WINDOW, for the reason band_ghost's two calls carry
       one: since the uniform-hinge ruling an engine's band rows past its own G
       are the OTHER screen's picture, and peek's whole claim is that it draws
       what the engines were given -- which does not include drawing the bottom
       screen's own rows a second time. With G equal to the band's height these
       are the whole band and both calls are what they were. */
    int ak0, ak1, bk0, bk1;
    band_window(lay, !lay.main_lower, ak0, ak1);
    band_window(lay, lay.main_lower, bk0, bk1);
    band_raster_engine(band, lay.gap_ds, ea, -1, "gappeek", -1, ak0, ak1);
    band_raster_engine(band, lay.gap_ds, eb, -1, "gappeek", -1, bk0, bk1);
    /* AND THE DEAD ZONE LAST, so "did an engine draw this pixel" is asked of
       the finished merge rather than of half of it. */
    band_continuity(band, lay.gap_ds, ea, eb);

    /* into the image, at the same integer scale the bottom half is drawn at */
    const int rx = lay.w / SUB_W, ry = lay.scale;
    for (int k = 0; k < lay.gap_ds; ++k)
        for (int x = 0; x < 256; ++x) {
            const BandPixel &bp = band[(size_t)k * 256 + x];
            if (!bp.hit) continue;
            for (int oy = 0; oy < ry; ++oy) {
                uint32_t *out = dst + (size_t)(lay.band_y + k * ry + oy) * dst_w;
                for (int ox = 0; ox < rx; ++ox) out[x * rx + ox] = bp.color;
            }
        }
}

/* ---- THE HINGE ROWS: the band, when the band is the world's own rows -------
 *
 * WHAT IT IS FOR. See THE OBJECT SHIFT in ntr/ppu.h. With the GaplessMinigames
 * mod engaged and the shift on, the top engine's OBJ layer is displayed
 * obj_shift_ds rows lower than the engine puts it, which is where the ROM puts
 * it, and the image carries obj_shift_ds extra rows BELOW the top screen so the
 * rows a shifted sprite runs off the bottom edge into are still in the picture.
 * Those rows are world -obj_shift_ds..-1. This draws them.
 *
 * IT IS THE SAME BAND, WITH SOMETHING REAL IN IT. band_fill has already laid
 * the backdrop down by the player's own fill setting, exactly as it does for a
 * gap-on minigame, so a run with the shift on and a run with the ROM's gap look
 * alike where nothing is crossing. What is different is that these rows are
 * addressed: the top engine submits into them, so this puts the engine's own
 * texels over the fill rather than leaving decoration in front of a hole.
 *
 * THE BIAS IS obj_shift_ds - 192 AND THAT IS THE WHOLE ARITHMETIC. Engine A's
 * entry at OAM row y is world row y - 192 (the mod has zeroed G, so the game
 * submits at world + 0xc0 exactly). Band index k is world row + obj_shift_ds,
 * because band row 0 IS world row -obj_shift_ds. So k = y - 192 + obj_shift_ds,
 * which is band_raster_engine's row_bias and nothing else. Band index 0 is
 * engine row 192 - obj_shift_ds, the first row the shifted top screen clipped,
 * so the strip begins exactly where the screen ends and the join is continuous
 * by construction rather than by a fudge.
 *
 * READ OUT OF THE SNAPSHOT, NOT THE LIVE OAM, and that is not a detail. These
 * rows are the top screen's picture continued, and the top screen is upload
 * N-1 (see THE TWO SCREENS ARE ONE FRAME APART below, and its bracketed
 * correction: by default the upload is at func_02019144's own line now, so
 * both screens are upload N and the snapshot IS the live OAM. The binding
 * below is right either way and is what keeps it right under
 * SM64DS_OAM_LOAD_LATE=1). Reading the live OAM
 * would put a continuation one frame ahead of the thing it continues, so an
 * object leaving the top screen would jump forward as it crossed the join. The
 * shadow binding is what BandEngine::shadow is for, and it carries into the
 * affine matrices as well as the attribute words. No snapshot yet, on the
 * program's first composed frame or on a path that never uploads, means this
 * pass declines rather than guesses, which leaves the band as band_fill left
 * it.
 *
 * A TARGETED PASS RATHER THAN A TALLER LIVE RASTER, band_peek's own trade for
 * band_peek's own reason: growing engine A's buffers by G_rom rows would put
 * this mode's cost on every level in the program. This walks engine A's OAM
 * once more, over the band's rows alone, only when a shifted layout is live.
 *
 * PEEK AND THE BAND'S CONTINUITY PASS ARE NOT RUN IN THIS MODE, and that is
 * decided in ppu_compose_stacked rather than here. Both were built to answer
 * "what is hidden in the rows neither engine can address", and under the shift
 * the top engine addresses them. Peek's engine-B binding would also be wrong:
 * its bias is +gap_ds, which reads the band as a hinge below the bottom
 * screen's own frame, and here the band sits ABOVE world row 0 where engine B
 * has nothing to say at all.
 */
/* THE SAME SWITCH THE ENGINE-A RASTER USES, so one variable lights both halves
   of the shift: hal/message_compositor.cpp names what the top screen kept and
   this names what the band drew, off one run, with one grep. */
int hinge_trace_on(void)
{
    static int on = -1;
    if (on < 0) {
        const char *s = std::getenv("SM64DS_OBJSHIFT_TRACE");
        on = s && *s && *s != '0';
    }
    return on;
}

unsigned g_hinge_frame;

void hinge_paint(uint32_t *dst, int dst_w, const StackLayout &lay)
{
    if (lay.obj_shift_ds <= 0 || lay.band_h <= 0) return;
    if (swap_refuses(lay, 0, "hinge_paint")) return;
    if (!g_oam_a_have) return;
    /* GAP_DS_MAX rows of 256 DS pixels, allocated on the first hinge frame and
       never freed: band_peek's trade, for band_peek's reason. */
    static BandPixel *hinge;
    if (!hinge) {
        hinge = (BandPixel *)std::calloc((size_t)GAP_DS_MAX * 256,
                                         sizeof *hinge);
        if (!hinge) return;
    }
    const int rows = lay.obj_shift_ds < lay.gap_ds ? lay.obj_shift_ds
                                                   : lay.gap_ds;
    if (rows <= 0 || rows > GAP_DS_MAX) return;
    std::memset(hinge, 0, sizeof(BandPixel) * (size_t)rows * 256);
    const int trace = hinge_trace_on();
    ++g_hinge_frame;
    /* THE BAND INDEX OF AN ENTRY, in both arms and in one expression. Band row
       0 is world row -obj_shift_ds and the world row of an engine A entry is
       its OAM y plus whatever of the correction has not been made yet, so
       k = y + obj_raster_ds + resid - 192. In the layer arm obj_raster_ds
       carries all of it and there are no residuals, which is the
       `obj_shift_ds - 192` this shipped with; in the per-entry default it
       carries none and the residuals carry the rest.

       AND THE ROUTED FILTER, which is the falsification's own fix. The layer
       arm drew every entry engine A had into the band, so the score row at
       engine y 168 was pushed into it along with the crossing ball. Only a
       routed entry has a world row at all, so only a routed entry belongs in
       rows that ARE world rows. */
    const int pe = per_entry_ds(lay);
    const BandEngine ea = {kRegBaseA, kOamBaseA, kObjVramA, kObjPlttA, 0,
                           lay.obj_raster_ds - 192, 0, "A", g_oam_a_shown,
                           pe ? g_obj_resid_shown : 0,
                           pe ? g_obj_routed_shown : 0};
    band_raster_engine(hinge, rows, ea, trace, "hinge");

    const int rx = lay.w / SUB_W, ry = lay.scale;
    if (rx <= 0 || ry <= 0) return;
    int px_drawn = 0;
    for (int k = 0; k < rows; ++k)
        for (int x = 0; x < SUB_W; ++x) {
            const BandPixel &bp = hinge[(size_t)k * 256 + x];
            if (!bp.hit) continue;
            ++px_drawn;
            for (int oy = 0; oy < ry; ++oy) {
                uint32_t *out = dst + (size_t)(lay.band_y + k * ry + oy) * dst_w;
                for (int ox = 0; ox < rx; ++ox) out[x * rx + ox] = bp.color;
            }
        }
    if (trace)
        std::fprintf(stderr, "[hinge] f%u band %d DS row(s) (world -%d..-1), "
                     "%s: %d DS pixel(s) drawn over the fill\n",
                     g_hinge_frame, rows, rows,
                     pe ? "routed entries only, corrected per entry"
                        : "every engine A entry, layer shift", px_drawn);
}

/* ---- HEADROOM: the rows above the top screen, and what goes in them --------
 *
 * WHAT IT IS FOR. See the StackLayout note in ntr/ppu.h. With the
 * GaplessMinigames mod engaged the game's G is zero, the world a minigame
 * simulates is still 192 + G_rom + 192 rows tall because its actors are placed
 * by fixed constants, and the top G_rom rows of that world -- Bob-omb Squad's
 * -224..-193 -- land ABOVE the top screen's first row. The layout gives the
 * image G_rom extra rows there. This fills them.
 *
 * WHAT THE RASTERISERS DO WITH A WRAPPED y, because that decides how much of an
 * object is left to draw at all. OAM::Render culls at submission -- `if
 * (y + h < 0) return; if (y > 0xc0) return;` -- and stores y & 0xff, so an
 * object ENTIRELY above the top screen never reaches OAM and cannot be drawn by
 * anything, here or on hardware, while one that straddles the top edge is
 * submitted with a wrapped 8-bit y. Both live rasters -- engine A's in
 * hal/message_compositor.cpp and engine B's raster_obj above -- undo the wrap
 * with the same expression band_decode uses, `if (y >= 192 && y >= 256 - bh)
 * y -= 256`, and then clip every row with `if (py < 0 || py >= 192) continue`.
 * So the port ALREADY reads a wrapped y as negative and ALREADY drops the
 * negative rows, and the strip below is exactly the rows it drops.
 *
 * WHAT IS UP THERE ON SCENE 368, measured over 401 frames by this pass's own
 * census (SM64DS_HEADROOM_TRACE, and port/tools/headroom.py reads it):
 *
 *   LAKITU, a 32x64 double-size affine entry that cruises across near the old
 *   top edge, has the top row of its sprite at engine rows -7..0 -- jammed flat
 *   against the first row of the picture, where the ROM puts it 32 rows down --
 *   and loses 1 to 7 of its rows on 174 of those frames. That is the report.
 *
 *   THE PARACHUTING BOB-OMBS, 32x32 double-size, spawn at world -256 and enter
 *   wholly above the top screen. Without the strip the first of them is not
 *   drawn AT ALL for 45 to 55 frames after it is submitted, and one of them
 *   never reached the screen at all in a 400-frame run. They are most of what
 *   the strip draws: 79039 of 79401 strip pixels over that census.
 *
 * A TARGETED STRIP PASS RATHER THAN A TALLER LIVE RASTER, which is the trade
 * band_peek's own note argues and for the same reason: growing engine A's
 * buffers by G_rom rows would put this mode's cost and its risk on every level
 * in the program. This walks engine A's OAM once more, over the strip's rows
 * alone, only when a headroom layout is live.
 *
 * ONE EXTRA ROW IS RASTERED AND NEVER DRAWN. The strip is head_ds rows and the
 * raster is head_ds + 1, so index head_ds is engine row 0 -- the top screen's
 * own first row -- and it exists purely as a COVERAGE MASK for the backdrop.
 * It is not composited.
 *
 * THE BACKDROP IS THE PICTURE'S OWN TOP ROW, NOT THE BACKGROUND LAYER'S, and
 * that is measured rather than assumed. The strip's BG rows were never meant to
 * render and reading them cold gives garbage rather than nothing: engine A's
 * only enabled background on scene 368 is BG3, a 32x32 text map at vofs 0, so
 * strip rows -32..-1 are map rows 24..31 -- and that map carries a 32x24 blit.
 * SM64DS_MSG_COMPOSITE_DEBUG's own char census reports 768 of its 1024 tiles
 * with data, which is exactly the 24 rows the screen shows, so the eight rows
 * above index tiles that carry none, or index tile 0 over and over. Neither is
 * sky.
 *
 * So the strip is FILLED, per column, from the top screen's own first rendered
 * row. The seam is continuous BY CONSTRUCTION -- the row above the top screen's
 * first row is the top screen's first row -- and it stays continuous a whole
 * strip up because the picture is vertically flat where it meets the seam: on
 * scene 368 the top of the image is a horizontal sky gradient whose DS row 0
 * and DS row 1 differ by a mean of 2.6 of 255 per channel over all 512 columns.
 * The ambient band's twenty-four-column wash was the other candidate and is not
 * used: it would average that gradient into bands and put a horizontal step
 * where there is none.
 *
 * AND THE COLUMNS A SPRITE COVERS ARE RECONSTRUCTED, which is what the extra
 * rastered row is for. Sampling the composed image blind would sample LAKITU
 * wherever Lakitu is pinned against the top edge and smear its surviving rows
 * up the entire strip, in exactly the columns the strip is about to draw the
 * rest of Lakitu into. So a column whose engine row 0 carries an engine-A
 * sprite takes its backdrop from a linear ramp between the nearest uncovered
 * columns either side of it -- the sky behind the sprite, rebuilt from the sky
 * beside it -- and only a frame in which EVERY column is covered leaves the
 * composed row standing as it is.
 *
 * THE FILL FADES AND THE SPRITES DO NOT, which is the peek band's behaviour and
 * is deliberate: the fill is a copy of framebuffer rows walk_window has already
 * run its fade composite over, so it fades with them for free, and the strip's
 * sprites are the engine's own texels exactly as band_peek draws them.
 */
int head_trace_on(void)
{
    static int on = -1;
    if (on < 0) {
        const char *s = std::getenv("SM64DS_HEADROOM_TRACE");
        on = s && *s && *s != '0';
    }
    return on;
}

/* ---- THE SEAM STRADDLE: the same hole with the band taken away -------------
 *
 * See the note over ppu_band_continuity in ntr/ppu.h for what this is. In one
 * paragraph: with GaplessMinigames on the game's own G is zero, so the ROM's
 * OAM router (src/func_ov004_020aff38.cpp) submits a sprite to EXACTLY ONE
 * engine -- world y in [-256, -1] to the top, [-64, 191] to the bottom, first
 * match returns -- and that engine's raster then clips it at its own screen
 * edge. A 16x16 sprite whose box crosses world row 0 loses the rows hanging
 * over the seam entirely and pops to the other screen when its centre crosses.
 * This pass draws those rows, out of the entry the game itself submitted.
 *
 * WORLD ROW -> COMPOSED IMAGE ROW, and the whole pass rests on the image and
 * the world running at one rate through the seam. The bottom screen's world row
 * r sits at lay.bottom_y + r * scale. The top screen's world row r (negative)
 * is engine row r + 192, at (r + 192) * scale = SCREEN_H + r * scale, and with
 * no band lay.bottom_y IS SCREEN_H, so both halves are `bottom_y + r * scale`.
 * THAT IDENTITY IS WHAT GAPLESS MEANS.
 *
 * IT SURVIVES THE OBJECT SHIFT, AND NOTHING ELSE. With obj_shift_ds set the
 * image has a band again, but its rows ARE world -obj_shift_ds..-1 and the top
 * engine draws into them at its own shifted submission, so bottom_y grew by
 * exactly the rows the picture gained and the one expression is still the world
 * row's place everywhere in the image. With an ORDINARY band it does not
 * survive: those rows are a hinge no engine addresses, the halves are that many
 * rows apart in the picture and not in the world, and a sprite drawn across the
 * seam would be drawn across the hinge. seam_straddle's guard is that
 * distinction, and it is not a test for a band.
 *
 * THE RULE IS ABOUT THE PICTURE, NOT ABOUT THE SIMULATION, and that is not a
 * stylistic choice -- see THE TWO SCREENS ARE ONE FRAME APART below. For a
 * tracked object, ask which half of the composed image is actually showing it:
 *
 *   BOTH halves have it     draw nothing. The both-engine router
 *                           (func_ov004_020b023c) puts its objects in both OAM
 *                           buffers and each engine clips its own half, so
 *                           they already arrive whole; the parachuting
 *                           bob-ombs are that case. This is also the answer on
 *                           the one frame where the two halves are showing two
 *                           different states of the same object, where drawing
 *                           anything would be adding a second copy of it.
 *   ONE half has it         and its box crosses the seam: draw the rows on the
 *                           other side of the seam, from THAT half's entry, at
 *                           THAT entry's own position.
 *   NEITHER half has it     draw nothing. That is the honest answer for an
 *                           object that has not been submitted yet, and for
 *                           the one frame the display lag leaves empty.
 *
 * WHY IT WRITES OVER THE COMPOSED PIXEL, where the band's continuity pass
 * refuses to. The band fills rows NO engine addressed, so "was this pixel
 * drawn" is a question with an answer and the pass can decline. These rows are
 * ordinary screen rows: fully painted, by an engine, every frame. Declining
 * would mean drawing nothing, which is the defect. So the rule is the one the
 * hardware would apply if the router had submitted to both engines: the
 * sprite's own opaque texels land over what is there. Bob-omb Squad's ball is
 * OBJ priority 0, the highest a sprite can have, so on a continuous display
 * there is nothing under it that would win -- and only an object the scene
 * NAMES reaches this at all, which is the containment. What this pass does NOT
 * do is invent: the texels are the submitted entry's own, out of the same
 * engine's VRAM through the same palette, and the half that already has the
 * object is never written to.
 */

int seam_trace_frames(void)
{
    static int on = -1;
    if (on < 0) {
        const char *s = std::getenv("SM64DS_GAP_STRADDLE_TRACE");
        on = s && *s && *s != '0';
    }
    return on;
}

unsigned g_head_frame;

/* One column of backdrop between two known ones. Integer and truncating, for
   the reason band_fill_ambient's note gives: a ramp a second implementation
   cannot reproduce exactly is a ramp nobody can check. */
uint32_t head_lerp(uint32_t c0, uint32_t c1, int u, int span)
{
    const int v = span - u;
    const int r = ((int)((c0 >> 16) & 0xff) * v + (int)((c1 >> 16) & 0xff) * u) / span;
    const int g = ((int)((c0 >> 8) & 0xff) * v + (int)((c1 >> 8) & 0xff) * u) / span;
    const int b = ((int)(c0 & 0xff) * v + (int)(c1 & 0xff) * u) / span;
    return 0xFF000000u | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

void head_paint(uint32_t *dst, int dst_w, const StackLayout &lay)
{
    if (lay.head_h <= 0 || lay.head_ds <= 0) return;
    if (swap_refuses(lay, 1, "head_paint")) return;
    /* GAP_DS_MAX + SUB_H rows of 256 DS pixels, allocated on the first headroom
       frame and never freed: band_peek's trade, for band_peek's reason. As a
       file-scope array it would be host .bss in EVERY binary that links this
       layer -- every smoke, every level run -- for a mode that is off by
       default and reachable on one scene. */
    static BandPixel *strip;
    if (!strip) {
        strip = (BandPixel *)std::calloc((size_t)(GAP_DS_MAX + SUB_H) * 256,
                                         sizeof *strip);
        if (!strip) return;
    }
    const int trace = head_trace_on();
    /* head_ds rows of strip, then the coverage row -- and under the trace the
       whole top screen as well, so the census can say which frame an object
       first has texels ON the screen. That is the frame it would have become
       visible WITHOUT this feature, and having both numbers off one frame of
       one run is what makes the before and the after comparable. */
    const int rows = lay.head_ds + (trace ? SUB_H : 1);
    std::memset(strip, 0, sizeof(BandPixel) * (size_t)rows * 256);
    ++g_head_frame;
    /* ENGINE A ONLY: the headroom is above the TOP screen and engine B has
       nothing to say about it. The bias that turns an engine row into a strip
       index is +head_ds, so engine row -head_ds is index 0 and engine row 0 is
       index head_ds. Engine A's OBJ extended palette store is not modelled
       anywhere in this program, so it passes 0 -- the same answer band_peek's
       engine-A pass gives, for the same reason. */
    const BandEngine ea = {kRegBaseA, kOamBaseA, kObjVramA, kObjPlttA, 0,
                           lay.head_ds, 0, "A", 0};
    band_raster_engine(strip, rows, ea, trace, "headroom", lay.head_ds);

    /* ---- the backdrop ---- */
    const uint32_t *edge = dst + (size_t)lay.top_y * dst_w;
    const int rx = lay.w / SUB_W, ry = lay.scale;
    uint32_t fill[STACK_W];
    int covered[STACK_W];
    int n_free = 0;
    for (int x = 0; x < lay.w; ++x) {
        const int dsx = rx > 0 ? x / rx : 0;
        covered[x] = strip[(size_t)lay.head_ds * 256 +
                           (dsx < SUB_W ? dsx : SUB_W - 1)].hit ? 1 : 0;
        fill[x] = edge[x];
        if (!covered[x]) ++n_free;
    }
    if (n_free && n_free < lay.w) {
        int x = 0;
        while (x < lay.w) {
            if (!covered[x]) { ++x; continue; }
            const int a = x;
            while (x < lay.w && covered[x]) ++x;
            const int b = x - 1;                 /* the covered run is [a, b] */
            /* the nearest uncovered column on each side, and that side's own
               value where the run touches an edge of the image. Both reads are
               of columns this loop never writes, so they are the sampled row's
               own colours and never a ramp of a ramp. */
            const uint32_t left = a > 0 ? fill[a - 1] : fill[x];
            const uint32_t right = x < lay.w ? fill[x] : fill[a - 1];
            const int span = b - a + 2;
            for (int k = a; k <= b; ++k)
                fill[k] = head_lerp(left, right, k - a + 1, span);
        }
    }
    for (int y = 0; y < lay.head_h; ++y) {
        uint32_t *out = dst + (size_t)y * dst_w;
        for (int x = 0; x < lay.w; ++x) out[x] = fill[x];
    }

    /* ---- and the strip's own sprites over it, at the bottom half's scale ---
       Strip index k is engine row k - head_ds, which is image row
       top_y + (k - head_ds) * scale == k * scale, so the strip starts at image
       row 0 and ends exactly where the top screen begins. */
    int px_drawn = 0;
    for (int k = 0; k < lay.head_ds; ++k)
        for (int x = 0; x < SUB_W; ++x) {
            const BandPixel &bp = strip[(size_t)k * 256 + x];
            if (!bp.hit) continue;
            ++px_drawn;
            for (int oy = 0; oy < ry; ++oy) {
                uint32_t *out = dst + (size_t)(k * ry + oy) * dst_w;
                for (int ox = 0; ox < rx; ++ox) out[x * rx + ox] = bp.color;
            }
        }
    if (trace)
        std::fprintf(stderr, "[headroom] f%u strip %d DS row(s) (engine rows "
                     "-%d..-1): %d DS pixel(s) drawn, %d of %d column(s) had "
                     "a sprite over the top screen's first row and were "
                     "reconstructed\n", g_head_frame, lay.head_ds, lay.head_ds,
                     px_drawn, lay.w - n_free, lay.w);
}

/* COMPOSE FRAMES SINCE THE PROGRAM STARTED, for the band continuity counter's
   reason: the claim this pass makes is about a SEQUENCE -- an object drawn on
   one half, then on the other -- and "no frame in it is missing" is not
   readable off a census with no frame number in it. */
unsigned g_seam_frame;

/* ---- THE TWO SCREENS ARE ONE FRAME APART IN THIS PORT ----------------------
 *
 * MEASURED HERE, and this pass cannot be written correctly without it. The
 * ROM's display sync (src/func_02019144.c) is `OAM::Flush(); OAM::Load();` at
 * VBlank, so on hardware both engines scan out the same uploaded OAM. The port
 * reproduces that tail inside hal/sub_screen.cpp's hal_sub_screen_present, and
 * BOTH frame loops -- port/tests/walk_window.cpp's and hal/scene_boot.cpp's
 * port_scene_run -- call port_message_composite_engine_a immediately BEFORE
 * it. So engine A's raster reads 0x07000000 before this frame's upload and
 * engine B's reads it after:
 *
 *     top screen    = upload N-1
 *     bottom screen = upload N
 *
 * The numbers, scene 368 gapless, scripted launch, headless captures one frame
 * apart: the ball's engine A entry is y = 178 at frame 231, and the composed
 * image that carries it at y = 178 is frame 232's. Frame 233 carries frame
 * 232's y = 169 and frame 234 carries 160. One frame, every frame, both loops.
 *
 * THAT IS NOT THIS PASS'S DEFECT AND NOT ITS FIX. Moving the upload ahead of
 * the engine A composite changes the timing of every sprite on the top screen
 * in every level in the program; it is another lane's change over another
 * lane's files and it would move every capture in the tree. What this pass
 * owes is to be RIGHT IN ITS PRESENCE. Completing an object out of the entry
 * engine A holds NOW would put rows on the bottom screen for a ball whose top
 * half is not in the picture yet -- a new artifact, not a fix -- so the entry
 * engine A's half of the picture was really drawn from is kept here.
 *
 * [THAT CHANGE HAPPENED, run mg13 lane BNP, and the block above is kept as
 * written because it is still exactly true under SM64DS_OAM_LOAD_LATE=1 and
 * because it is the derivation the move was made from. WHAT IS TRUE BY DEFAULT
 * NOW: hal/sub_screen.cpp uploads at func_02019144's own line, between the
 * block's slot 2 and the layer publishes, which is ahead of BOTH rasters, so
 *
 *     top screen = bottom screen = upload N
 *
 * and the one frame between the halves is gone. It had to go: on the
 * dScMgD3DBase_c family the engine A arm alternates every frame, so a top
 * screen a frame behind its own arm put every routed sprite on the wrong
 * physical screen -- Tango's "the blue arrows are in the wrong spot" on scene
 * 372. See THE OBJ/POWCNT1 PARITY in hal/sub_screen.cpp for the ROM
 * derivation and the measurement.
 *
 * THIS PASS IS UNCHANGED AND STILL CORRECT. The snapshot it reads is still the
 * OAM the top half was drawn from -- ppu_seam_oam_mark_uploaded takes it from
 * the other side of the upload, which is where that block now is -- and
 * kSeamLag below is an upper bound on how far apart the two halves may be, so
 * a lag of zero sits well inside it. What retires is the NEED for the
 * allowance, not the allowance.]
 *
 * TAKEN AT THE UPLOAD, NOT AT THE COMPOSE, and that is the difference between
 * a snapshot that is right and one that is usually right. hal/sub_screen.cpp
 * calls ppu_seam_oam_mark immediately before OAM::Load, so the copy is the OAM
 * engine A's compositor rasterised from a few lines earlier -- once per frame,
 * on the one path that uploads. Keeping it at the end of this pass instead
 * assumed one compose per frame, and the capture path composes a SECOND time
 * over the same framebuffer to write its BMP: that second compose read a
 * snapshot one upload ahead of the picture it was looking at, so the pass
 * declined on exactly the frame a before/after capture was taken for. Measured
 * -- SM64DS_SCENE_BMP_STACKED at frame 232 came out byte-identical between the
 * two builds while the per-frame trace showed the completion happening.
 *
 * Engine B needs no such thing: its raster runs after the upload, so its own
 * OAM is what its half of the picture holds.
 *
 * g_oam_a_shown and shown_rd16 are declared at the top of the band machinery,
 * because hinge_paint reads them too; the note there says why. */


/* WHERE THE TOP SCREEN REALLY HAS IT. Matched on the sprite's SIZE and on
   NEITHER coordinate, because both are what a frame of lag changes: an object
   moving diagonally is at a different x and a different y in the two halves.
   Of the entries that match the size, the one nearest the position the game
   reports is the object, and an entry further away than a whole frame of
   travel is somebody else's. kSeamLag is that bound: Bob-omb Squad's ball
   moves nine rows in a frame at the speed the plunger gives it, and 32 is
   comfortably clear of that while still being narrower than the chase it would
   take to confuse two balls.

   IT RETURNS THE ENTRY'S OWN x AND y, and every consumer uses those and not
   the game's. The whole point of reading the snapshot is that the picture is a
   frame behind the simulation; drawing the completion at the simulation's
   position would put it beside the half it is completing. */
const int kSeamLag = 32;

/* The bottom half's lookup, against ITS shown snapshot and with the same
   tolerant match seam_shown_a uses: the tracker's position is this tick's
   and the snapshot is the displayed upload, so an exact-row equality drops
   every fast mover -- the cannonball's half-drawn crossing frames. Engine
   B's rows ARE world rows (bias 0), so the returned row needs no bias. */
int seam_shown_b(const BandEngine &eb, const BandTrack &t, BandCache &out,
                 int &row, int &col)
{
    if (!g_oam_b_have) return 0;
    int found = 0, best = 0, bestx = 0, best_d = 0;
    uint16_t b0 = 0, b1 = 0, b2 = 0;
    for (int i = 127; i >= 0; --i) {
        const uint16_t a0 = shown_rd16_b(i * 8);
        const uint16_t a1 = shown_rd16_b(i * 8 + 2);
        const uint16_t a2 = shown_rd16_b(i * 8 + 4);
        BandEntry d;
        if (!band_decode(a0, a1, a2, d)) continue;
        if (d.w != t.w || d.h != t.h) continue;
        const int r = band_row_of(eb, i, d.y);
        int dy = r - t.y, dx = d.x - t.x;
        if (dy < 0) dy = -dy;
        if (dx < 0) dx = -dx;
        if (dy > kSeamLag || dx > kSeamLag) continue;
        const int dist = dy + dx;
        if (!found || dist < best_d) {
            found = 1;
            best = r;
            bestx = d.x;
            best_d = dist;
            b0 = a0;
            b1 = a1;
            b2 = a2;
        }
    }
    if (!found) return 0;
    out.have = 1;
    out.a0 = b0;
    out.a1 = b1;
    out.a2 = b2;
    out.eng = eb;
    row = best;
    col = bestx;
    return 1;
}

int seam_shown_a(const BandEngine &ea, const BandTrack &t, BandCache &out,
                 int &row, int &col)
{
    if (!g_oam_a_have) return 0;
    /* FOUND IS ITS OWN FLAG and the row is not a sentinel. Every row this can
       return is NEGATIVE -- an entry that reaches the top screen at all sits
       above world row 0 -- so "-1 means nothing found" is a value in range and
       reading it as one throws away every match. It did, on the first cut of
       this: the completion reported "top half: no" on the exact frames the
       census reported the entry with 192 of 192 texels standing in the
       picture. */
    int found = 0, best = 0, bestx = 0, best_d = 0;
    uint16_t b0 = 0, b1 = 0, b2 = 0;
    for (int i = 127; i >= 0; --i) {
        const uint16_t a0 = shown_rd16(i * 8);
        const uint16_t a1 = shown_rd16(i * 8 + 2);
        const uint16_t a2 = shown_rd16(i * 8 + 4);
        BandEntry d;
        if (!band_decode(a0, a1, a2, d)) continue;
        if (d.w != t.w || d.h != t.h) continue;
        const int r = band_row_of(ea, i, d.y);
        int dy = r - t.y, dx = d.x - t.x;
        if (dy < 0) dy = -dy;
        if (dx < 0) dx = -dx;
        if (dy > kSeamLag || dx > kSeamLag) continue;
        const int dist = dy + dx;
        if (!found || dist < best_d) {
            found = 1;
            best = r;
            bestx = d.x;
            best_d = dist;
            b0 = a0;
            b1 = a1;
            b2 = a2;
        }
    }
    if (!found) return 0;
    out.have = 1;
    out.a0 = b0;
    out.a1 = b1;
    out.a2 = b2;
    out.eng = ea;
    row = best;
    col = bestx;
    return 1;
}

/* The bottom half's own fade, spelled here because a pixel written into the
   composed image after the compose has to carry the fade the compose applied.
   Both halves get it: the top half arrives already faded (walk_window's own
   composite ran over the framebuffer) and the bottom half is faded by the loop
   in ppu_compose_stacked, so a synthesized pixel is faded whichever half it
   lands in. Same expression, one place. */
uint32_t seam_fade(uint32_t p, int evy, int to_white)
{
    if (!evy) return p;
    int r = (p >> 16) & 0xff, g = (p >> 8) & 0xff, b = p & 0xff;
    if (to_white) {
        r += ((255 - r) * evy) >> 4;
        g += ((255 - g) * evy) >> 4;
        b += ((255 - b) * evy) >> 4;
    } else {
        r -= (r * evy) >> 4;
        g -= (g * evy) >> 4;
        b -= (b * evy) >> 4;
    }
    return 0xFF000000u | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

/* One entry's texels into the composed image, for world rows in [rlo, rhi).
   `rtop` is the world row the BOX starts on and `x` its left DS column. */
int seam_draw_entry(uint32_t *dst, int dst_w, const StackLayout &lay,
                    const BandCache &c, const BandEntry &d, int rtop, int x,
                    int rlo, int rhi, int evy, int to_white)
{
    BandTexels t;
    if (!band_texels(c.eng, c.a0, c.a1, c.a2, d, t)) return 0;
    const int rx = lay.w / SUB_W, ry = lay.scale;
    if (rx <= 0 || ry <= 0) return 0;
    int drawn = 0;
    for (int sy = 0; sy < d.bh; ++sy) {
        const int r = rtop + sy;
        if (r < rlo || r >= rhi) continue;
        const int hy = lay.bottom_y + r * ry;
        if (hy < 0 || hy + ry > lay.h) continue;
        for (int sx = 0; sx < d.bw; ++sx) {
            const int px = x + sx;
            if (px < 0 || px >= SUB_W) continue;
            uint32_t color;
            if (!band_texel(t, d, sx, sy, color)) continue;
            color = seam_fade(color, evy, to_white);
            ++drawn;
            for (int oy = 0; oy < ry; ++oy) {
                uint32_t *out = dst + (size_t)(hy + oy) * dst_w;
                for (int ox = 0; ox < rx; ++ox) out[px * rx + ox] = color;
            }
        }
    }
    return drawn;
}

/* THE MEASUREMENT, and it is read off the composed image rather than off this
   pass's own bookkeeping. For every opaque texel of the entry: which half of
   the image its world row falls in, and whether the pixel standing there IS
   that texel. `op_*` is how many the sprite has on that side and `hit_*` how
   many of them the picture actually shows, so hit == op on both sides is the
   whole claim -- the object is whole across the seam, in the game's own texels
   -- and the two numbers read separately are what tells "the rows are missing"
   apart from "the rows are there and wrong". */
struct SeamCount {
    int op_top, op_bot, hit_top, hit_bot;
};

void seam_measure(const uint32_t *dst, int dst_w, const StackLayout &lay,
                  const BandCache &c, const BandEntry &d, int rtop, int x,
                  int evy, int to_white, SeamCount &o)
{
    o.op_top = o.op_bot = o.hit_top = o.hit_bot = 0;
    BandTexels t;
    if (!band_texels(c.eng, c.a0, c.a1, c.a2, d, t)) return;
    const int rx = lay.w / SUB_W, ry = lay.scale;
    if (rx <= 0 || ry <= 0) return;
    for (int sy = 0; sy < d.bh; ++sy) {
        const int r = rtop + sy;
        const int hy = lay.bottom_y + r * ry;
        if (hy < 0 || hy + ry > lay.h) continue;
        for (int sx = 0; sx < d.bw; ++sx) {
            const int px = x + sx;
            if (px < 0 || px >= SUB_W) continue;
            uint32_t color;
            if (!band_texel(t, d, sx, sy, color)) continue;
            color = seam_fade(color, evy, to_white);
            /* the block's top-left host pixel. The blit is nearest neighbour
               at a whole-number ratio, so every pixel of the block carries the
               same colour and one of them is the block. */
            const uint32_t got = dst[(size_t)hy * dst_w + px * rx];
            const int hit = (got & 0xFFFFFFu) == (color & 0xFFFFFFu);
            if (r < 0) {
                ++o.op_top;
                o.hit_top += hit;
            } else {
                ++o.op_bot;
                o.hit_bot += hit;
            }
        }
    }
}

/* THE CENSUS: every entry, in the state its own half of the picture was drawn
   from, whose box crosses the seam -- engine A out of the shown snapshot,
   engine B out of its live OAM -- and whether the other half carries the same
   object. It is how the both-engine router's objects are told apart from the
   one-engine router's WITHOUT a table naming which is which, so the bob-omb
   statement is read straight off it, and it is the line a report about a
   wrong-looking crossing gets answered from. It runs BEFORE the completion
   below, so its numbers are always the picture as the two engines left it. */
int seam_census_paired(const BandEngine &other, int from_shadow,
                       const BandEntry &d, int rtop, int x, uint16_t a2,
                       int &idx, int &row)
{
    for (int i = 127; i >= 0; --i) {
        uint16_t b0, b1, b2;
        if (from_shadow) {
            b0 = shown_rd16(i * 8);
            b1 = shown_rd16(i * 8 + 2);
            b2 = shown_rd16(i * 8 + 4);
        } else {
            b0 = rd16(other.oam + i * 8u);
            b1 = rd16(other.oam + i * 8u + 2);
            b2 = rd16(other.oam + i * 8u + 4);
        }
        BandEntry e;
        if (!band_decode(b0, b1, b2, e)) continue;
        if (e.w != d.w || e.h != d.h || b2 != a2) continue;
        /* THE TOLERANCE IS ON BOTH AXES, and it has to be. The two halves are
           an upload apart, so a moving object is at a different x AND a
           different y in them; an exact-x pairing test reported "ONE HALF
           ONLY" for a bob-omb that both halves plainly had, on exactly the
           frames it was moving sideways. Same tile, same size, within a frame
           of travel in both axes is the object. */
        const int r = band_row_of(other, i, e.y);
        int dy = r - rtop, dx = e.x - x;
        if (dy < 0) dy = -dy;
        if (dx < 0) dx = -dx;
        if (dy > kSeamLag || dx > kSeamLag) continue;
        idx = i;
        row = r;
        return 1;
    }
    return 0;
}

void seam_census(const uint32_t *dst, int dst_w, const StackLayout &lay,
                 const BandEngine &ea, const BandEngine &eb, int evy,
                 int to_white)
{
    for (int s = 0; s < 2; ++s) {
        const int from_shadow = (s == 0);
        const BandEngine &e = from_shadow ? ea : eb;
        const BandEngine &other = from_shadow ? eb : ea;
        if (from_shadow && !g_oam_a_have) continue;
        for (int i = 127; i >= 0; --i) {
            uint16_t a0, a1, a2;
            if (from_shadow) {
                a0 = shown_rd16(i * 8);
                a1 = shown_rd16(i * 8 + 2);
                a2 = shown_rd16(i * 8 + 4);
            } else {
                a0 = rd16(e.oam + i * 8u);
                a1 = rd16(e.oam + i * 8u + 2);
                a2 = rd16(e.oam + i * 8u + 4);
            }
            BandEntry d;
            if (!band_decode(a0, a1, a2, d)) continue;
            const int rtop = band_row_of(e, i, d.y);
            if (rtop >= 0 || rtop + d.bh <= 0) continue;   // does not cross
            BandCache c;
            c.have = 1;
            c.a0 = a0;
            c.a1 = a1;
            c.a2 = a2;
            c.eng = e;
            SeamCount n;
            seam_measure(dst, dst_w, lay, c, d, rtop, d.x, evy, to_white, n);
            int pi = -1, pr = 0;
            /* THE OTHER HALF, IN ITS OWN STATE: engine B's live OAM when
               this entry came out of the snapshot, and the snapshot when it
               came out of engine B. */
            const int pair = seam_census_paired(other, !from_shadow, d, rtop,
                                                d.x, a2, pi, pr);
            std::fprintf(stderr, "[gapseam] f%u %s%s oam%3d a0=%04x a1=%04x "
                         "a2=%04x %dx%d%s at (%d,%d world): image top %d/%d "
                         "bot %d/%d, %s\n", g_seam_frame, e.name,
                         from_shadow ? " shown" : "", i, a0, a1, a2, d.w, d.h,
                         d.dbl ? " dbl" : (d.affine ? " aff" : ""), d.x, rtop,
                         n.hit_top, n.op_top, n.hit_bot, n.op_bot,
                         pair ? "BOTH HALVES" : "ONE HALF ONLY");
            if (pair)
                std::fprintf(stderr, "[gapseam] f%u     the other half has it "
                             "as %s oam%d at world %d, so both sides of the "
                             "seam are the engines' own\n", g_seam_frame,
                             other.name, pi, pr);
        }
    }
}

/* The completion itself, over the objects the scene NAMES. Read with gap_ds 0,
   a BandTrack's y is the world row of the sprite box's top edge -- the band
   frame's row 0 is world row -G and G is zero -- so the same per-scene reader
   the band uses answers this pass with no second contract. */
void seam_complete(uint32_t *dst, int dst_w, const StackLayout &lay,
                   const BandEngine &ea, const BandEngine &eb, int evy,
                   int to_white)
{
    const int trace = seam_trace_frames();
    BandTrack tr[BAND_TRACK_MAX];
    const int n = g_track_fn(tr, BAND_TRACK_MAX, 0);
    for (int i = 0; i < n && i < BAND_TRACK_MAX; ++i) {
        const BandTrack &t = tr[i];
        if (t.slot < 0 || t.slot >= BAND_TRACK_MAX) continue;
        /* NEAR THE SEAM AT ALL, which is a wider window than the one the
           completion acts on. The trace wants the frames either side of a
           crossing as much as the crossing itself -- "the object arrived on
           the adjacent row and no frame in between is missing" is a claim
           about a sequence -- so the census window is the box plus a frame of
           travel and the completion's window is the box crossing row 0. */
        if (t.y + t.h <= -kSeamLag || t.y >= kSeamLag) continue;

        /* WHICH HALF OF THE PICTURE HAS IT. The bottom half is engine B's own
           OAM, which its raster read after the upload, so the match is exact.
           The top half is the snapshot, and there the match is on the sprite's
           size with BOTH coordinates left free, because a frame of lag moves
           an object in both; see kSeamLag. */
        BandCache cb, ca;
        cb.have = ca.have = 0;
        int ra = 0, xa = 0, rb = 0, xb = 0;
        const int has_bot = seam_shown_b(eb, t, cb, rb, xb);
        const int has_top = seam_shown_a(ea, t, ca, ra, xa);

        /* BOTH DIRECTIONS ARE ONE TEST. A box that starts above the seam and
           ends below it straddles, and it does not matter whether the object
           is rising or falling: the rows that are missing are the ones on the
           side its own half cannot address, and which side that is follows
           from which half has it. */
        BandEntry db, da, dk;
        const int ok_b = has_bot && band_decode(cb.a0, cb.a1, cb.a2, db);
        const int ok_a = has_top && band_decode(ca.a0, ca.a1, ca.a2, da);
        const int str_b = ok_b && rb < 0 && rb + db.bh > 0;
        const int str_a = ok_a && ra < 0 && ra + da.bh > 0;

        /* REMEMBER THE ENTRY WHILE AN ENGINE HAS IT. g_track is the same
           per-slot store the peek band's continuity pass keeps, refreshed
           here too because in the bandless mod nothing else runs to fill it.
           The fold rule holds by the readers' own contract: every object a
           reader folds onto one slot shares one attribute template, so the
           cached triple is the vanished object's own. */
        if (ok_b) g_track[t.slot] = cb;
        else if (ok_a) g_track[t.slot] = ca;

        const BandCache *use = 0;
        const BandEntry *ud = 0;
        int rtop = 0, xtop = 0, rlo = 0, rhi = 0;
        const char *why = "";
        if (has_top && has_bot) {
            why = "both halves have it, nothing to complete";
        } else if (str_b) {
            use = &cb;
            ud = &db;
            /* THE SNAPSHOT'S OWN POSITION, exactly as the top case below
               uses ra/xa: the drawn halves are the shown upload, and drawing
               the completion at the tracker's newer position split a fast
               ball across the seam for the frames of its crossing. */
            rtop = rb;
            xtop = xb;
            rlo = rb;
            rhi = 0;            /* the rows the bottom screen cannot address */
            why = "bottom half only";
        } else if (str_a) {
            use = &ca;
            ud = &da;
            rtop = ra;
            xtop = xa;          /* the snapshot's own column, not the game's */
            rlo = 0;
            rhi = ra + da.bh;   /* the rows the top screen cannot address */
            why = "top half only";
        } else if (!has_top && !has_bot) {
            /* THE OWNER'S BUG 2 ON SCENE 368: the slingshot ball's router
               gives it to ONE engine and the cull refuses both near the
               seam, so for a few world rows of the crossing NO OAM anywhere
               holds the ball and the live lookups above come back empty.
               The tracker still knows exactly where it is, and the store
               above still holds the entry an engine last submitted for this
               slot, so the whole box is drawn at the tracker's position --
               rows either side of the seam alike, since neither half can
               contribute anything to double it. */
            BandCache &kc = g_track[t.slot];
            if (kc.have && band_decode(kc.a0, kc.a1, kc.a2, dk)) {
                use = &kc;
                ud = &dk;
                rtop = t.y;
                xtop = t.x;
                rlo = t.y;
                rhi = t.y + dk.bh;
                why = "neither half has it, drawn whole from the slot's "
                      "last entry";
            } else {
                why = "neither half has it, drawing nothing";
            }
        } else {
            why = "in one half and not across the seam";
        }

        if (!use) {
            if (trace)
                std::fprintf(stderr, "[gapstraddle] f%u slot %d %dx%d at "
                             "(%d,%d world): top %s, bottom %s -- %s\n",
                             g_seam_frame, t.slot, t.w, t.h, t.x, t.y,
                             has_top ? "HAS IT" : "no", has_bot ? "HAS IT"
                                                                : "no", why);
            continue;
        }
        SeamCount before, after;
        if (trace)
            seam_measure(dst, dst_w, lay, *use, *ud, rtop, xtop, evy, to_white,
                         before);
        const int drawn = seam_draw_entry(dst, dst_w, lay, *use, *ud, rtop,
                                          xtop, rlo, rhi, evy, to_white);
        if (!trace) continue;
        seam_measure(dst, dst_w, lay, *use, *ud, rtop, xtop, evy, to_white,
                     after);
        std::fprintf(stderr, "[gapstraddle] f%u slot %d %dx%d at (%d,%d world) "
                     "%s, drawn from %s a0=%04x a1=%04x a2=%04x at (%d,%d "
                     "world): before top %d/%d bot %d/%d, after top %d/%d "
                     "bot %d/%d, %d synthesized, total %d/%d\n",
                     g_seam_frame, t.slot,
                     t.w, t.h, t.x, t.y, why, use->eng.name, use->a0, use->a1,
                     use->a2, xtop, rtop, before.hit_top, before.op_top,
                     before.hit_bot, before.op_bot, after.hit_top, after.op_top,
                     after.hit_bot, after.op_bot, drawn,
                     after.hit_top + after.hit_bot, after.op_top + after.op_bot);
    }
}

/* ---- SEAM SNOW: the mod's own weather, one field for both screens ----------
 *
 * Owner's spec, third and FINAL architecture for Shuffle Shell's snow, in
 * his words: new code that uses the top snow as an overlay and makes it go
 * down the whole length -- no spawning at the gap, and the normal game
 * untouched. The two tracking designs before this both died of the same
 * disease: they inferred game state from OAM diffing, and a save-state
 * capture (F5) churns that table into fake deaths and fake motion -- the
 * spammed-F5 report was the churn minting extra flakes at the seam. This
 * pass infers NOTHING from the game. While the mod is engaged for a scene
 * with a registered attribute list, the OBJ rasters hide the game's own
 * flake sprites (ppu_seam_snow_owns; visuals only, the simulation runs
 * untouched) and this draws a synthetic field of the same flakes -- the
 * game's own attr words, tiles and palette out of its own VRAM -- spawned
 * only above the top screen, falling the full joined height, paced by the
 * game's registered tick counter so any pause freezes it with the game.
 * Deterministic LCG, so captures reproduce. An empty list stands both the
 * pass and the suppression down, which is every scene and every mode
 * except an engaged 374. */
struct SnowFlake {
    int active;
    int x, wy;          /* x in DS cols <<4; wy in world rows <<4, -192 top */
    int vx, vy;         /* per game tick, <<4 */
    int kind;
};
enum { SNOW_MAX = 128 };
SnowFlake g_snow[SNOW_MAX];
GhostAttrFn g_ghost_attr_fn;
GhostTickFn g_ghost_tick_fn;
unsigned g_snow_last_ticks;
int g_snow_warm;      /* the field has been pre-seeded for this engagement */
unsigned g_snow_lcg = 0x12345u;

unsigned snow_rand(void)
{
    g_snow_lcg = g_snow_lcg * 1664525u + 1013904223u;
    return g_snow_lcg >> 16;
}

void seam_snow(uint32_t *dst, int dst_w, const StackLayout &lay, int evy,
               int to_white)
{
    if (!lay.seam) return;
    if (swap_refuses(lay, 2, "seam_snow")) return;
    int n_attr = 0;
    const unsigned short *attrs = g_ghost_attr_fn ? g_ghost_attr_fn(&n_attr) : 0;
    if (!attrs || n_attr <= 0) {
        for (int i = 0; i < SNOW_MAX; ++i) g_snow[i].active = 0;
        g_snow_warm = 0;
        return;
    }
    /* PRE-SEEDED, owner's spec: the sky is already mid-snowfall when the
       player first sees it. An empty field filling at one flake per tick
       reads as a pile forming at the top and marching down; seeding the
       same population at random heights over the whole span is the steady
       state the spawner would reach anyway, delivered before frame one. */
    if (!g_snow_warm) {
        g_snow_warm = 1;
        for (int i = 0; i < 90 && i < SNOW_MAX; ++i) {
            SnowFlake &f = g_snow[i];
            f.active = 1;
            f.x = (int)(snow_rand() % 249) << 4;
            f.wy = (-224 + (int)(snow_rand() % 416)) << 4;
            f.vx = (int)(snow_rand() % 9) - 4;
            f.vy = 5 + (int)(snow_rand() % 6);
            f.kind = n_attr > 0 ? (int)(snow_rand() % (unsigned)n_attr) : 0;
        }
    }
    unsigned steps = 0;
    if (g_ghost_tick_fn) {
        const unsigned now = g_ghost_tick_fn();
        steps = now - g_snow_last_ticks;
        if (steps > 4) steps = 4;   /* a resume is a resume, not a catch-up */
        g_snow_last_ticks = now;
    }
    int live = 0;
    for (int i = 0; i < SNOW_MAX; ++i)
        if (g_snow[i].active) ++live;
    for (unsigned st = 0; st < steps; ++st) {
        /* ~90 flakes over the 384-row field reads like the game's own
           density doubled for the doubled height; one spawn per tick fills
           gradually rather than as a curtain, and every spawn is ABOVE the
           top screen -- nothing ever pops in at the seam. */
        if (live < 90) {
            for (int i = 0; i < SNOW_MAX; ++i)
                if (!g_snow[i].active) {
                    SnowFlake &f = g_snow[i];
                    f.active = 1;
                    f.x = (int)(snow_rand() % 249) << 4;
                    f.wy = (-192 - 8 - (int)(snow_rand() % 24)) << 4;
                    f.vx = (int)(snow_rand() % 9) - 4;
                    f.vy = 5 + (int)(snow_rand() % 6);
                    f.kind = (int)(snow_rand() % (unsigned)n_attr);
                    ++live;
                    break;
                }
        }
        for (int i = 0; i < SNOW_MAX; ++i) {
            SnowFlake &f = g_snow[i];
            if (!f.active) continue;
            f.x += f.vx;
            f.wy += f.vy;
            if (f.wy > (192 << 4) || f.x < 0 || f.x >= (249 << 4))
                f.active = 0;
        }
    }
    const BandEngine ea = {kRegBaseA, kOamBaseA, kObjVramA, kObjPlttA, 0,
                           0, 0, "A", 0};
    for (int i = 0; i < SNOW_MAX; ++i) {
        SnowFlake &f = g_snow[i];
        if (!f.active) continue;
        BandCache c;
        c.have = 1;
        c.a0 = 0x0000;
        c.a1 = 0x0000;
        c.a2 = attrs[f.kind];
        c.eng = ea;
        BandEntry d;
        if (!band_decode(c.a0, c.a1, c.a2, d)) continue;
        const int wy = f.wy >> 4;
        seam_draw_entry(dst, dst_w, lay, c, d, wy, f.x >> 4, wy, wy + d.bh,
                        evy, to_white);
    }
}

void seam_straddle(uint32_t *dst, int dst_w, const StackLayout &lay, int evy,
                   int to_white)
{
    /* THE TWO CONDITIONS ARE READ TOGETHER and neither implies the other.
       lay.seam is the mod: the GAME's own G is zero, so the two screens are
       one continuous world. The second condition is the PICTURE, and it is
       about the ruler rather than about the band: this pass writes at
       `bottom_y + r * scale` for a world row r, and that expression is only the
       right place when the image's rows and the world's rows run at the same
       rate through the seam.

       WITH NO BAND they do, which is the case this shipped with: bottom_y IS
       SCREEN_H, so the top screen's world row r is at (r + 192) * scale and the
       bottom screen's is at bottom_y + r * scale, and those are one expression.

       WITH AN ORDINARY BAND they do not. Every level in the game has gap_ds 0
       with a real G behind it, and MinigameGap off gives gap_ds 0 with a real G
       behind it too; a gap-on minigame has a band whose rows the engines cannot
       address at all. In each of those a sprite that leaves one screen has
       genuinely gone behind the hinge, and joining it across the seam would be
       inventing a continuity the game does not have.

       WITH A SHIFTED BAND they do again, and that is why obj_shift_ds is named
       here rather than the pass being switched off. Those band rows ARE world
       rows -obj_shift_ds..-1, drawn by the top engine at its own shifted
       submission, so the ruler runs unbroken from image row 0 to the last row
       and `bottom_y + r * scale` is the world row's place everywhere in it.
       hinge_paint draws what engine A HAS; this draws the rows the router gave
       to one engine and that engine's own screen edge then clipped, which is a
       different hole and is still open. The two never write the same pixel: a
       completion is only ever drawn on the side its own half cannot address. */
    if (!lay.seam || (lay.gap_ds && !lay.obj_shift_ds)) return;
    if (swap_refuses(lay, 3, "seam_straddle")) return;
    ++g_seam_frame;
    /* the same two bindings the band's passes use, with engine B's row bias at
       the gapless G: engine row -> world row is -192 for A and 0 for B */
    /* ENGINE ROW -> WORLD ROW, and the per-entry correction moves it. An
       engine A entry sits at world + 0xc0 in the arm this pass shipped for, so
       -192 was the whole answer; with the correction made at the router's call
       a routed entry sits at world + 0xc0 + G_rom instead, and what is left of
       the correction is the entry's own residual. So the bias is -(192 + pe)
       and band_row_of adds the residual back per slot -- which comes to -192
       again for an entry the band is carrying, and to -224 for one the top
       screen is. Engine B is world row for world row in every arm. */
    const int pe = per_entry_ds(lay);
    const BandEngine ea = {kRegBaseA, kOamBaseA, kObjVramA, kObjPlttA, 0,
                           -192 - pe, 0, "A", 0,
                           pe ? g_obj_resid_shown : 0,
                           pe ? g_obj_routed_shown : 0};
    const BandEngine eb = {kRegBase, kOamBase, kObjVram, kObjPltt, kObjExtPltt,
                           0, 1, "B", 0, 0, 0};
    if (seam_trace_frames())
        seam_census(dst, dst_w, lay, ea, eb, evy, to_white);
    if (g_track_fn) seam_complete(dst, dst_w, lay, ea, eb, evy, to_white);
}

}  // namespace

// ---- the stacked presentation -----------------------------------------------
//
// The layout decides everything; see StackLayout in ntr/ppu.h. This writes
// every pixel of dst, so the caller does not have to clear it.

StackLayout stack_layout(int gap_ds, int head_ds, int obj_shift_ds,
                         int fill_mode, uint32_t fill_color, int peek,
                         const uint32_t *art)
{
    StackLayout l;
    if (gap_ds < 0) gap_ds = 0;
    if (gap_ds > GAP_DS_MAX) gap_ds = GAP_DS_MAX;
    /* THE SHIFT IS CLAMPED TO THE BAND IT NEEDS, not just to the ceiling. The
       rows a shifted object is pushed into are the band's rows; a shift with no
       band, or a shift taller than the band, would push texels into image rows
       that do not exist, so the number that survives here is the one the image
       can actually carry. A caller that means the mode passes the same value
       twice. */
    if (obj_shift_ds < 0) obj_shift_ds = 0;
    if (obj_shift_ds > GAP_DS_MAX) obj_shift_ds = GAP_DS_MAX;
    if (obj_shift_ds > gap_ds) obj_shift_ds = gap_ds;
    /* THE HEADROOM IS CLAMPED BY THE SAME CEILING AND FOR THE SAME REASON: it
       comes from the same framework word, one scene's InitResources having
       written it before the gapless mod zeroed it, and a wild read must not be
       able to ask for a buffer the size of the desktop. */
    if (head_ds < 0) head_ds = 0;
    if (head_ds > GAP_DS_MAX) head_ds = GAP_DS_MAX;
    l.gap_ds = gap_ds;
    l.obj_shift_ds = obj_shift_ds;
    l.head_ds = head_ds;
    l.scale = SCREEN_H / SUB_H;
    l.w = STACK_W;
    l.head_h = head_ds * l.scale;
    /* EVERY BAND SHIFTS TOGETHER, which is the whole reason the headroom is a
       field of this struct rather than a second arithmetic somewhere: the
       compose, the DIB header, the window size, the BMP writer and both stylus
       mappers read top_y / band_y / bottom_y and none of them recomputes them.
       With head_h zero all four are exactly what they were. */
    l.top_y = l.head_h;
    l.band_y = l.head_h + SCREEN_H;
    l.band_h = gap_ds * l.scale;
    l.bottom_y = l.band_y + l.band_h;
    l.h = l.head_h + SCREEN_H * 2 + l.band_h;
    l.fill_mode = fill_mode == GAP_FILL_SOLID   ? GAP_FILL_SOLID
                  : fill_mode == GAP_FILL_CUSTOM ? GAP_FILL_CUSTOM
                                                 : GAP_FILL_AMBIENT;
    l.fill_color = fill_color | 0xFF000000u;
    l.peek = peek ? 1 : 0;
    /* NOT AN INPUT HERE, and deliberately so. The seam flag is the GAME's own G
       being zero, which this function is not told about -- it is handed the
       LAYOUT's G, and the two are the same number only when the mod is on.
       hal/screen_gap.cpp sets it on the struct every frame; see the field's
       note in ntr/ppu.h. A layout built and never touched again is a layout
       with the pass off, which is the behaviour this shipped with. */
    l.seam = 0;
    /* world_band is hal's too, set beside the seam flag, and zero here for
       the same reason -- an uninitialized field in a struct built on the
       stack is garbage, and this one gates band passes. */
    l.world_band = 0;
    /* NOT AN INPUT HERE EITHER, for the reason the seam flag is not: this is
       told the BAND's height and the layer shift is a separate question about
       the same rows. It is zero unless the falsified layer arm is asked for,
       and hal/screen_gap.cpp sets it on the struct at the same latch it sets
       the seam flag, clamped to the band this function just decided. */
    l.obj_raster_ds = 0;
    /* NO BAND, NO ART. The art is exactly gap_ds rows tall, so a layout with no
       band cannot carry one, and dropping it here means no consumer has to ask
       the question twice. */
    l.art = gap_ds ? art : 0;
    /* NOT AN INPUT HERE, for the reason the seam flag is not: POWCNT1's display
       swap is a per-FRAME fact and this is a per-scene latch. Zero is "engine A
       on the upper screen", which is what every layout in the program carried
       before the bit was read at all, so a layout nobody tells composes exactly
       the way it always did. hal/screen_gap.cpp sets it every frame. */
    l.main_lower = 0;
    /* NOT AN INPUT HERE, for the reason the seam flag and the swap are not:
       this function is handed the LAYOUT's band and the game's own G is a
       different question about the same rows, read live every frame.
       hal/screen_gap.cpp sets it beside those two. Zero means "the whole band
       is the game's", which is what every band pass assumed before this
       existed and is still right for every game whose G is the band's
       height. */
    l.game_g_ds = 0;
    return l;
}

/* Installing the hook CLEARS THE ATTRIBUTE CACHE, and that is the whole of the
   cache's lifetime management. hal/screen_gap.cpp installs at every latch --
   which is every time the scene or its G changes -- so a cached entry can never
   outlive the scene whose OAM it was read from, and a stale tile number cannot
   be re-rendered into the next minigame's band. */
void ppu_band_continuity(BandTrackFn fn)
{
    g_track_fn = fn;
    for (int i = 0; i < BAND_TRACK_MAX; ++i) g_track[i].have = 0;
}

/* Registered beside the continuity reader, cleared the same way: a scene
   change must not let one game's ghosts snow into the next. */
void ppu_seam_ghost_attrs(GhostAttrFn fn)
{
    g_ghost_attr_fn = fn;
    for (int i = 0; i < SNOW_MAX; ++i) g_snow[i].active = 0;
    g_snow_warm = 0;
}

/* The rasters' question: does the overlay own this sprite identity right
   now? Engine-side suppression of the game's own flakes, gated on the same
   registration everything else here rides. */
int ppu_seam_snow_owns(uint16_t a2)
{
    if (!g_ghost_attr_fn) return 0;
    int n = 0;
    const unsigned short *attrs = g_ghost_attr_fn(&n);
    for (int k = 0; k < n; ++k)
        if (attrs[k] == a2) return 1;
    return 0;
}

void ppu_seam_ghost_ticks(GhostTickFn fn) { g_ghost_tick_fn = fn; }

/* Engine B's OAM source override; ppu.h documents it beside engine A's. */
void ppu_obj_oam_source_b(uint32_t addr) { g_oam_src_b = addr; }

/* The engine A OAM copy the seam straddle pass reads as "what the top screen is
   showing". See the note over g_oam_a_shown for why it exists and why the
   caller is the one place in the program that uploads OAM. */
void ppu_seam_oam_mark(void)
{
    for (int i = 0; i < 1024; ++i)
        g_oam_a_shown[i] = rd8(kOamBaseA + (unsigned)i);
    g_oam_a_have = 1;
    for (int i = 0; i < 1024; ++i)
        g_oam_b_shown[i] = rd8(kOamBase + (unsigned)i);
    g_oam_b_have = 1;
    /* AND THE ROUTED MARKS ROTATE IN THE SAME BREATH, which is the only reason
       they are stored in this file. The copy above is upload N-1, the block the
       engine A compositor rasterised from a few lines earlier; the marks that
       describe it are the ones in LIVE, because the shadow has been filling
       with frame N's submissions ever since. So SHOWN takes what LIVE holds,
       LIVE takes the shadow that OAM::Load is about to send to the hardware,
       and the shadow starts clean for frame N+1. Three assignments in the one
       place that knows the upload happened; anywhere else and a mark would sit
       one frame away from the entry it describes. */
    for (int i = 0; i < 128; ++i) {
        g_obj_resid_shown[i] = g_obj_resid_live[i];
        g_obj_routed_shown[i] = g_obj_routed_live[i];
        g_obj_resid_live[i] = g_obj_resid_shadow[i];
        g_obj_routed_live[i] = g_obj_routed_shadow[i];
        g_obj_resid_shadow[i] = 0;
        g_obj_routed_shadow[i] = 0;
    }
    g_obj_routed_last = -1;
}

/* THE SAME SNAPSHOT, TAKEN AFTER A ROM-ORDERED UPLOAD. See ntr/ppu.h.
   The copy above is upload N-1 because it is taken BEFORE the upload; this one
   is taken after it, so kOamBaseA already holds upload N -- which is the block
   both OBJ rasters read when func_02019144's upload runs at the head of the
   display path instead of at its foot. The marks that describe upload N are the
   SHADOW's, because the shadow IS upload N now; there is no intermediate frame
   for LIVE to hold, so both stages take the shadow's marks in one statement and
   the shadow starts clean. Everything else -- the two 1 KB copies, the have
   flags, the fill detector -- is ppu_seam_oam_mark's, unchanged. */
void ppu_seam_oam_mark_uploaded(void)
{
    for (int i = 0; i < 1024; ++i)
        g_oam_a_shown[i] = rd8(kOamBaseA + (unsigned)i);
    g_oam_a_have = 1;
    for (int i = 0; i < 1024; ++i)
        g_oam_b_shown[i] = rd8(kOamBase + (unsigned)i);
    g_oam_b_have = 1;
    for (int i = 0; i < 128; ++i) {
        g_obj_resid_live[i] = g_obj_resid_shadow[i];
        g_obj_routed_live[i] = g_obj_routed_shadow[i];
        g_obj_resid_shown[i] = g_obj_resid_shadow[i];
        g_obj_routed_shown[i] = g_obj_routed_shadow[i];
        g_obj_resid_shadow[i] = 0;
        g_obj_routed_shadow[i] = 0;
    }
    g_obj_routed_last = -1;
}

void ppu_obj_routed_shadow_reset(void)
{
    for (int i = 0; i < 128; ++i) {
        g_obj_resid_shadow[i] = 0;
        g_obj_routed_shadow[i] = 0;
    }
    g_obj_routed_last = -1;
}

void ppu_obj_routed_record(int slot, int resid)
{
    if (slot < 0 || slot >= 128) return;
    /* THE FILL DETECTOR, and it is not an optimisation. See g_obj_routed_last:
       slots only ascend inside one fill, so a slot that does not ascend is the
       first routed submission of a new one and last fill's marks would
       otherwise sit under this fill's entries. */
    if (slot <= g_obj_routed_last) ppu_obj_routed_shadow_reset();
    g_obj_routed_last = slot;
    g_obj_routed_shadow[slot] = 1;
    g_obj_resid_shadow[slot] = (signed char)resid;
}

int ppu_obj_routed_live_resid(int slot)
{
    if (slot < 0 || slot >= 128) return 0;
    return (int)g_obj_resid_live[slot];
}

int ppu_obj_routed_live_is(int slot)
{
    if (slot < 0 || slot >= 128) return 0;
    return g_obj_routed_live[slot] ? 1 : 0;
}

/* DROPPING THE MEMORY IS THE WHOLE OF ITS LIFETIME MANAGEMENT, and it is the
   same rule the attribute cache above follows: hal/screen_gap.cpp calls this at
   every latch, which is every time the scene or its G changes, so one
   minigame's glow can never be what the next one's band starts from. Clearing
   `have` rather than zeroing the arrays is deliberate -- the next ambient frame
   seeds them from its own sample, so the first frame of a scene is the direct
   read and not a ramp up out of black. */
void ppu_band_ambient_reset(void)
{
    g_amb.have = 0;
    /* AND THE EDGE ROWS' SETTLED BACKGROUND with it, for the reason the
       ambient memory is dropped here: it is per-scene state whose lifetime is
       the scene. A background learned in one minigame would light the next
       one's band off the last one's picture -- and worse than the wash's
       version of that, it would call the whole of the next scene's first
       second a crossing, because every column would stand far off a settled
       value that belongs to a different game. */
    g_edge.have = 0;
    std::memset(g_edge_w, 0, sizeof g_edge_w);
    std::memset(g_edge_obj, 0, sizeof g_edge_obj);
}

void ppu_compose_stacked(const uint32_t *top, const SubFramebuffer &sub,
                         uint32_t *dst, int dst_w, int dst_h, int evy,
                         int to_white, const StackLayout &lay)
{
    if (!top || !dst || dst_w != lay.w || dst_h != lay.h) return;
    if (evy < 0) evy = 0;
    if (evy > 16) evy = 16;

    /* ---- WHICH ENGINE GOES IN WHICH HALF ---------------------------------
     *
     * POWCNT1 (0x04000304) bit 15 is the DS's display swap: set sends engine A
     * to the upper LCD, clear sends it to the lower one. It changes NOTHING
     * about either engine's rendering -- both scan out exactly what they
     * already scanned out -- so the whole of it lives here, in the two
     * destinations below.
     *
     * lay.main_lower is that bit, inverted, read live by hal/screen_gap.cpp
     * every frame. With it 0 these two are lay.top_y and lay.bottom_y, which
     * is the unconditional pair this function shipped with, so every scene
     * that leaves the bit alone composes byte for byte the way it did.
     *
     * THE IMAGE ROWS DO NOT MOVE, and that is the point of naming them this
     * way round: top_y is the UPPER PHYSICAL SCREEN and bottom_y the LOWER one,
     * in the window and in the BMP and under the stylus mapper, whichever
     * engine happens to be feeding each. The DS's touchscreen is the lower
     * panel and stays the lower panel across a swap, so hal's touch mapping
     * needs no term here -- which is also the answer to "dragging on the bottom
     * half draws at the top": the drag was landing on the right panel all
     * along and the port was showing that panel's engine in the wrong half. */
    const int a_y = lay.main_lower ? lay.bottom_y : lay.top_y;
    const int b_y = lay.main_lower ? lay.top_y : lay.bottom_y;

    // ENGINE A, verbatim: it is already faded and already carries the F3
    // overlay, because it is the framebuffer the caller finished with.
    // AT a_y RATHER THAN AT ROW ZERO, which is the one word the headroom
    // changes here: with a headroom the upper screen starts head_h rows down
    // and with none top_y is 0 and this is the memcpy it always was.
    std::memcpy(dst + (size_t)a_y * dst_w, top,
                (size_t)SCREEN_W * SCREEN_H * 4);

    /* ENGINE B. The ratio is a whole number at every tier the port
       builds (1, 2 and 4), and a SHIFT rather than a divide would be wrong the
       day a tier is not a power of two, so it stays a divide. */
    const int rx = SCREEN_W / SUB_W, ry = SCREEN_H / SUB_H;
    for (int y = 0; y < SCREEN_H; ++y) {
        const int sy = ry > 0 ? y / ry : (y * SUB_H) / SCREEN_H;
        const uint32_t *src = sub.px[sy < SUB_H ? sy : SUB_H - 1];
        uint32_t *out = dst + (size_t)(b_y + y) * dst_w;
        for (int x = 0; x < SCREEN_W; ++x) {
            const int sx = rx > 0 ? x / rx : (x * SUB_W) / SCREEN_W;
            uint32_t p = src[sx < SUB_W ? sx : SUB_W - 1];
            if (evy) {
                /* the same expression walk_window's fade composite runs over
                   the framebuffer, so the two halves fade together.

                   AND THE QUESTION UNDER IT IS OPEN. evy comes from
                   port_fader_blend_state, which reads the MAIN engine's
                   BLDCNT/BLDY at 0x4000050 and 0x4000054. This copy exists
                   because the corner panel gets that fade today, and it gets
                   it by accident: the panel is inside the framebuffer when
                   walk_window's fade loop runs over it. Whether engine A's
                   fade belongs on the SUB screen AT ALL on hardware is a
                   question nobody has opened. The sub engine has its own
                   master brightness at 0x0400106C and ppu_scanout_sub above
                   already applies it, so this may well be a second fade on
                   top of the right one. Reproducing today's behaviour is
                   deliberate, so that switching layout changes the layout and
                   nothing else; it is not a claim that today's behaviour is
                   correct. No run has yet exercised a fade in the stacked
                   layout. */
                int r = (p >> 16) & 0xff, g = (p >> 8) & 0xff, b = p & 0xff;
                if (to_white) {
                    r += ((255 - r) * evy) >> 4;
                    g += ((255 - g) * evy) >> 4;
                    b += ((255 - b) * evy) >> 4;
                } else {
                    r -= (r * evy) >> 4;
                    g -= (g * evy) >> 4;
                    b -= (b * evy) >> 4;
                }
                p = 0xFF000000u | ((uint32_t)r << 16) | ((uint32_t)g << 8) |
                    (uint32_t)b;
            }
            out[x] = p;
        }
    }

    /* THE HEADROOM, here because it reads the first row of the UPPER screen and
       nothing else in the image, and under a swap that row is written by the
       second of the two blits above rather than the first. It used to sit
       between them, which was the same thing while engine A was always the
       upper screen; the move is byte-for-byte neutral with the bit set, because
       with no swap the two blits write disjoint rows and head_paint reads only
       the ones the first of them wrote. With no headroom head_h is 0 and this
       returns immediately, so a layout without one is byte-for-byte what it was
       before this existed. */
    head_paint(dst, dst_w, lay);

    /* THE BAND LAST, because the ambient fill reads the two rows the loops
       above just wrote -- the top screen's bottom row and the bottom screen's
       top row -- and reading them before they are written would light the band
       off the previous frame. With no gap band_h is 0 and both calls return
       immediately, so a gapless image is byte-for-byte what it was before this
       existed.

       THE FADE IS NOT APPLIED TO THE BAND, and it does not need to be: the
       ambient fill is a function of two rows that have ALREADY been faded, so
       it fades with them for free. A solid fill is the player's own colour and
       fading it would be this program deciding that a preference is part of the
       picture. */
    band_fill(dst, dst_w, lay);
    /* PEEK IS THE HINGE'S PASS AND THE HINGE'S ALONE. With the object shift on
       the band is not a hinge: the top engine submits into those rows and
       hinge_paint below draws what it submitted. Running peek there as well
       would put a second rendering of the same objects into the same rows off a
       binding whose engine-B bias assumes a gap that is not there. The layout
       already carries peek 0 in this mode (hal/screen_gap.cpp forces it, and
       says so), so this test is the belt beside that brace. */
    if ((lay.peek || lay.world_band) && !lay.obj_shift_ds)
        band_peek(dst, dst_w, lay);
    /* and the ghost when peek is OFF: whatever is really crossing the gap
       shows as a blurred shape behind the glow instead of vanishing or
       flooding the wash. Peek keeps the crisp honest view; the world-band
       just ran the crisp raster above; the hinge mode draws its own rows. */
    if (!lay.peek && !lay.world_band && !lay.obj_shift_ds)
        band_ghost(dst, dst_w, lay);
    /* AND THE BAND'S OWN CONTENT, when the band has any: engine A's texels for
       the world rows the shift moved into it, over the fill that was just laid
       down. Returns immediately with no shift, so a gap-on band is byte-for-byte
       what it was before this existed. */
    hinge_paint(dst, dst_w, lay);

    /* AND THE SEAM LAST, after everything that could have written the rows
       either side of it. It is the band's mirror image: the band draws rows no
       engine can address, this draws rows an engine addressed and then clipped
       at its own screen edge, and the two never both run -- one needs a band
       and the other needs none. With the mod off it returns on its first test,
       so a gapless image is byte-for-byte what it was before this existed. */
    seam_straddle(dst, dst_w, lay, evy, to_white);
    /* and the mod's own snow field; see seam_snow */
    seam_snow(dst, dst_w, lay, evy, to_white);
}

}  // namespace ntr
