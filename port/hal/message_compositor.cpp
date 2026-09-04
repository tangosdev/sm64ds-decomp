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
//
// WHAT IT IMPLEMENTS (the 3D layer's PRIORITY), added run mg5 lane Y3D:
//   - BG0 in 3D mode is a LAYER WITH A PRIORITY, not a backdrop. A 2D layer
//     whose priority number puts it behind BG0 may no longer paint over the
//     pixels the 3D engine actually drew. The 3D engine's own per-pixel
//     coverage comes from ntr::gx_coverage(), which the rasteriser fills
//     beside every store it makes into the framebuffer, so "the 3D engine drew
//     here" is a recorded fact rather than a colour comparison against the
//     clear. SM64DS_3D_PRIORITY_OFF=1 restores the unconditional overwrite for
//     A/B on one binary.
//
// WHAT IT IMPLEMENTS (OBJ-vs-OBJ PRIORITY), added run mg8 lane SGX:
//   - a sprite is ordered against other sprites by attribute 2 bits 10-11,
//     lowest number in front, and the OAM index only breaks a tie. raster_obj
//     wrote g_a unconditionally on its 127 -> 0 walk, which was pure
//     lowest-index order and is the defect run link60 lane BLND2 fixed on
//     engine B and left open here. Scene 375's playfield is on THIS engine, so
//     its snow (priority 0, OAM indices 13 and up) lost to every shell
//     (priority 1) and every semi-transparent reflection (priority 2) it
//     crossed. See the test in raster_obj for the rule and the one divergence
//     that remains. SM64DS_OBJPRIO_A_OFF=1 restores the unconditional write for
//     A/B on one binary.
//
// WHAT IT IMPLEMENTS (OBJ-vs-BG PRIORITY), added run mg10 lane RGX:
//   - a sprite is ordered against the BACKGROUNDS by the same attribute 2 bits
//     10-11, and the tie goes to the SPRITE. GBATEK's OAM attribute 2 calls the
//     field "Priority relative to BG (0-3; 0=Highest)" and states the tie
//     outright: when a sprite's priority equals a background layer's, the
//     sprite "becomes higher priority and is displayed on top of that BG
//     layer". GBATEK's BGxCNT says the same about the number itself ("BG
//     Priority (0-3, 0=Highest)") and settles BG-vs-BG ties by index ("BG0 is
//     having the highest, and BG3 the lowest priority"), which is the order the
//     BG loop below already walks. So a sprite loses to a background only when
//     the background's priority NUMBER is strictly smaller. That is the whole
//     rule, and mg8 lane SGX left it open on purpose: SGX's test is owner-gated
//     to OBJ so it could fix OBJ-vs-OBJ without moving every scene in the game.
//     This lane moves them, and names each one.
//   - THE 3D LAYER IS THE SAME RULE AND WAS ALREADY RIGHT. With DISPCNT bit 3
//     set, BG0 IS the 3D engine's output (see bg0_is_3d below, pinned on the
//     decomp's own GX::SetGraphicsMode), so "OBJ vs the 3D layer" is "OBJ vs
//     BG0" and the tie still goes to the sprite. read_bg disables BG0 exactly
//     then, so no g_a cell is ever owned by it and the test added here cannot
//     see it; the comparison happens instead at the final blit, in
//     layer_behind_3d, whose OBJ arm is `prio > p3d` -- strictly worse loses,
//     equal wins -- which is this same GBATEK sentence written for that layer.
//     Nothing about the 3D layer changes in this lane. It is stated here
//     because a reader who finds the new test and not that one would conclude
//     the 3D case is missing.
//   - SM64DS_OBJBG_OFF=1 restores the old "sprites beat every background"
//     write for A/B on one binary, the shape SM64DS_OBJPRIO_A_OFF and
//     SM64DS_3D_PRIORITY_OFF already have.
//   - SM64DS_OBJBG_PROBE=x,y prints every OAM entry that touches one DS pixel
//     and what each of the two OBJ tests decided about it. That is the probe
//     port/ppu_blend.txt section 4 took by hand-patching raster_obj for mg8;
//     keeping it means the next lane's per-pixel transcript needs no patch.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ntr/gx.h"
#include "ntr/ppu.h"

/* THE TOP ENGINE'S OBJ DISPLAY SHIFT, in DS rows, from hal/screen_gap.cpp.
 *
 * ZERO UNLESS SOMEBODY ASKS, AND NOTHING ASKS. It is the G the running
 * minigame's own InitResources wrote, and it is non-zero only while the
 * GaplessMinigames mod is engaged FOR THE SCENE NOW RUNNING and
 * SM64DS_GAPLESS_OBJ_SHIFT=1 has turned the shift on, which is one scene in the
 * program behind two switches that both default off. hal/screen_gap.cpp's
 * obj_shift_ds carries the measurement that made it opt-in: only the sprites
 * the framework's OAM router placed move with G, and this moves the layer. On
 * every other path this file is byte-for-byte the compositor it was.
 *
 * WHAT IT MEANS HERE. The mod stores zero into the framework word the ROM's OAM
 * router adds to every top-screen submission, so the game submits at world +
 * 0xc0 instead of world + 0xc0 + G and every sprite lands G rows higher up a
 * background that did not move with it. Adding it back at the raster is the
 * same picture the ROM composes: the two rasters below read each entry as
 * though it had been submitted at y + shift, so the window unit, the alpha
 * blend and the screen clip all answer for the row the texel is really shown
 * on. See THE OBJECT SHIFT in ntr/ppu.h; ntr/ppu_sub.cpp's hinge_paint draws
 * the rows this then clips off the bottom.
 *
 * Declared rather than included because hal/screen_gap.h pulls the whole layout
 * in and this file needs one int. C++ linkage, matching every other hal_ name
 * this file reaches and the definition in hal/screen_gap.cpp.
 *
 * AND THIS ONE IS THE LAYER TERM ALONE. The correction this file really applies
 * is per entry now: the five routers' own submissions are marked at the call
 * (hal/screen_gap.cpp) and carried to this raster on the OAM's own snapshot
 * path (ntr::ppu_obj_routed_live_resid), so the two rasters below add a term
 * that is different for entry 3 and entry 4 of the same frame. The layer term
 * is what the falsified arm adds to all 128 of them, it is zero unless
 * SM64DS_GAPLESS_OBJ_SHIFT=1 asks for it, and the two are never both non-zero
 * -- the per-entry correction stands down when the layer arm is on, so that an
 * A/B is one mechanism against the other and not one on top of the other. */
int hal_gapless_obj_raster_shift_ds(void);
/* the top screen's OAM source while the gapless mod is engaged; see the note
   over oam_a below and the definition in hal/screen_gap.cpp */
unsigned hal_gapless_oam_src_a(void);

namespace {

// Engine A only.
constexpr uint32_t kRegBase  = 0x04000000u;
constexpr uint32_t kVramBase = 0x06000000u;
constexpr uint32_t kPlttBase = 0x05000000u;
constexpr uint32_t kOamBase  = 0x07000000u;

/* THE SEAM'S FRAME ALIGNMENT. This file's two OBJ rasters ARE the top
   screen's sprite drawing, and they run BEFORE the frame's OAM::Load while
   the sub screen scans out AFTER it, so the two halves of a straddling
   sprite are one frame of travel apart -- the owner measured a crossing
   bob-omb's halves ticking on alternate frames. With the gapless mod
   engaged, hal/screen_gap.cpp points this at the working shadow OAM::Load
   is about to upload for engine A, so both halves draw one frame. Zero
   everywhere else. */
inline uint32_t oam_a(void) { return kOamBase; }  /* see hal/sub_screen.cpp:
    the two screens are aligned by upload order now, not by a source swap --
    the shadow is empty at this raster's moment, measured 2026-08-20.
    [Run mg13 lane BNP: the upload moved AGAIN, to func_02019144's own line,
    which is a few statements above this raster. So the hardware OAM this reads
    is now THIS frame's block rather than the previous one, and it is still the
    same block engine B scans out. Reading 0x07000000 is what makes both of
    those true without a source swap, which is why the function is still this
    one line. THE OBJ/POWCNT1 PARITY in hal/sub_screen.cpp is the derivation.] */
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
// THE OTHER HALF OF THE SAME REGISTER, closed by run mg5 lane Y3D. Skipping
// BG0 stopped the port INVENTING a layer; it did not put the real 3D layer in
// its place, because the port renders 3D into the framebuffer before this
// compositor runs and the compositor only wrote over it. A 2D BG that on
// hardware sits BEHIND the 3D layer (a higher priority number than BG0's)
// covered it anyway. On scene 390 that was BG2 at priority 3 owning 48357 of
// the top screen's 49152 pixels while the 3D layer at priority 1 submitted 878
// polygons a frame: the minigame's flower and Yoshi were drawn and then buried
// whole. The mask the framebuffer did not carry is ntr::gx_coverage(); the
// priority arithmetic is layer_behind_3d() and the final blit below.
inline bool bg0_is_3d(uint32_t dispcnt) { return (dispcnt & 0x8u) != 0; }

// The 3D layer is in the picture only when bit 3 says BG0 IS the 3D engine's
// output AND BG0's enable bit is up. Bit 3 alone is not enough: a scene can
// select 3D mode and leave the layer disabled, and then the 2D unit owns every
// pixel exactly as it did before this change.
inline bool bg0_3d_shown(uint32_t dispcnt) {
    return bg0_is_3d(dispcnt) && ((dispcnt >> 8) & 1u) != 0;
}

// BG0CNT's priority field IS the 3D layer's priority. Same two bits, same
// register, whether BG0 is a tile background or the 3D engine's output.
inline int bg0_3d_priority() { return rd16(kRegBase + 0x08) & 3; }

// SM64DS_3D_PRIORITY_OFF=1 puts the unconditional overwrite back on this same
// binary, so a before/after is one build at one .dsstate base -- what
// notes/port-selftest-bmp-gate.md requires before two BMPs may be compared.
inline bool prio3d_off_env() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_3D_PRIORITY_OFF");
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v != 0;
}

// SM64DS_OBJPRIO_A_OFF=1 does the same for run mg8's OBJ-vs-OBJ priority test
// in raster_obj: it puts the old unconditional lowest-index write back, on this
// same binary, so every scene's before/after is one build at one .dsstate base.
inline bool objprio_off_env() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_OBJPRIO_A_OFF");
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v != 0;
}

// SM64DS_OBJBG_OFF=1 does the same for run mg10's OBJ-vs-BG test: it puts back
// the write that let every sprite cover every background whatever the two
// priorities said. Separate from SM64DS_OBJPRIO_A_OFF on purpose -- the two
// tests are two different hardware rules and a lane bisecting a changed frame
// has to be able to switch off one without the other.
inline bool objbg_off_env() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_OBJBG_OFF");
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v != 0;
}

/* SM64DS_OBJBG_PROBE=x,y -- the per-pixel transcript, at DS coordinates.
   Off unless set, and then it prints one line per OAM entry whose opaque texel
   lands on that pixel, with what g_a held when the entry arrived and what each
   OBJ test decided. Returns -1,-1 when unset. */
inline void objbg_probe_pixel(int &px, int &py) {
    static int x = -2, y = -2;
    if (x == -2) {
        x = y = -1;
        const char *e = std::getenv("SM64DS_OBJBG_PROBE");
        if (e && *e) {
            char *end = nullptr;
            const long vx = std::strtol(e, &end, 10);
            if (end && *end == ',') {
                const long vy = std::strtol(end + 1, nullptr, 10);
                if (vx >= 0 && vx < 256 && vy >= 0 && vy < 192) {
                    x = (int)vx; y = (int)vy;
                }
            }
        }
    }
    px = x; py = y;
}

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
    bool obj_on;
    unsigned in[2], obj_in, out;
};

/* THE OBJ WINDOW'S MASK, engine A. The third window has no rectangle: its
   region is the union of the opaque texels of every mode-2 sprite, and WINOUT's
   UPPER six bits say which layers show inside it.

   THIS FILE USED TO SAY IT COULD NOT CARRY THE MASK, and the reason given was
   structural: the BG loop runs before raster_obj, so the region would not exist
   yet when the BGs are masked. That is a statement about ORDER, not about
   possibility -- the mask needs only the sprite SHAPES, not their colours or
   their priority resolution, so it can be built in a cheap pre-pass before the
   BG loop and the full sprite raster can stay exactly where it is. Engine B
   (ntr/ppu_sub.cpp) resolves BGs and sprites together in one pass and so gets
   the mask for free; engine A pays one extra OAM walk for it, over mode-2
   entries only.

   The geometry below is raster_obj's own, deliberately: same size table, same
   DISPCNT bit-4 mapping, same affine group arithmetic, same "index 0 is
   transparent" test. A mask built with different geometry from the sprites it
   is meant to describe would cut the hole in the wrong place. */
uint8_t g_objwin[192][256];

/* SM64DS_OBJWIN_OFF=1 puts the old behaviour back on the SAME binary -- bit 15
   unread, so a scene with only the OBJ window on leaves the window unit disarmed
   and every layer draws everywhere. A before/after is then one build at one
   .dsstate base, which notes/port-selftest-bmp-gate.md requires before two BMPs
   may be compared at all. Same shape as SM64DS_3D_PRIORITY_OFF above. */
/* THE OBJECT SHIFT'S CENSUS SWITCH, and its frame counter. The counter steps
   once per engine A composite, which is once per frame on both frame loops, so
   a line's f number is the frame the top screen was drawn on. */
unsigned g_obj_frame;

inline bool objshift_trace_on() {
    static int on = -1;
    if (on < 0) {
        const char *e = std::getenv("SM64DS_OBJSHIFT_TRACE");
        on = e && *e && *e != '0';
    }
    return on != 0;
}

inline bool objwin_off_env() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_OBJWIN_OFF");
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v != 0;
}

void build_objwin(uint32_t dispcnt) {
    static const int kSizes[3][4][2] = {
        {{8, 8}, {16, 16}, {32, 32}, {64, 64}},
        {{16, 8}, {32, 8}, {32, 16}, {64, 32}},
        {{8, 16}, {8, 32}, {16, 32}, {32, 64}},
    };
    std::memset(g_objwin, 0, sizeof g_objwin);
    /* No sprites at all means no mask, and the window is then empty rather than
       absent: WINOUT still governs, which is what the hardware does. */
    if (!((dispcnt >> 12) & 1) || !((dispcnt >> 15) & 1) || objwin_off_env())
        return;
    const uint32_t boundary = 32u << ((dispcnt >> 20) & 3);
    const bool map1d = (dispcnt >> 4) & 1;
    /* THE MASK SHIFTS WITH THE SPRITES THAT MAKE IT. A window sprite's region
       is its own opaque texels, so if the OBJ layer is being read at y + shift
       then the region those sprites define is at y + shift too. Leaving it here
       would mask the backgrounds at rows no sprite is shown on any more, which
       is the one way this feature could put a hole in artwork rather than move
       an object. Zero shift, and this is the line it was.

       PER ENTRY, so the mask follows the sprites that make it entry by entry:
       a routed window sprite moves and a screen-space one does not, which is
       the same rule the OBJ raster below reads. */
    const int layer = hal_gapless_obj_raster_shift_ds();

    for (int i = 127; i >= 0; --i) {
        const uint16_t a0 = rd16(oam_a() + i * 8u);
        const uint16_t a1 = rd16(oam_a() + i * 8u + 2);
        const uint16_t a2 = rd16(oam_a() + i * 8u + 4);
        if (ntr::ppu_seam_snow_owns(a2)) continue;   /* seam-snow owns it */
        const bool affine = a0 & 0x100;
        if (!affine && (a0 & 0x200)) continue;          // disabled
        if (((a0 >> 10) & 3) != 2) continue;            // not an OBJ-window sprite
        const int shape = (a0 >> 14) & 3;
        if (shape == 3) continue;
        const int size = (a1 >> 14) & 3;
        const int w = kSizes[shape][size][0], h = kSizes[shape][size][1];
        const bool dbl = affine && (a0 & 0x200);
        const int bw = dbl ? w * 2 : w, bh = dbl ? h * 2 : h;
        int x = a1 & 0x1FF, y = a0 & 0xFF;
        if (x >= 256) x -= 512;
        if (y >= 192 && y >= 256 - bh) y -= 256;
        /* AFTER THE WRAP, NEVER BEFORE IT. The 8-bit y is the hardware's, and
           the sign it carries comes out of the sprite's own box height; adding
           to it first would turn a sprite parked at 224 into one at 256 and
           lose the negative row it was standing on. */
        const int shift = layer + ntr::ppu_obj_routed_live_resid(i);
        y += shift;
        const bool c256 = a0 & 0x2000;
        const bool hflip = !affine && (a1 & 0x1000);
        const bool vflip = !affine && (a1 & 0x2000);
        const uint32_t tile = a2 & 0x3FF;

        int pa = 256, pb = 0, pc = 0, pd = 256;
        if (affine) {
            const int grp = (a1 >> 9) & 0x1F;
            pa = (int16_t)rd16(oam_a() + (grp * 4 + 0) * 8u + 6);
            pb = (int16_t)rd16(oam_a() + (grp * 4 + 1) * 8u + 6);
            pc = (int16_t)rd16(oam_a() + (grp * 4 + 2) * 8u + 6);
            pd = (int16_t)rd16(oam_a() + (grp * 4 + 3) * 8u + 6);
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
                const uint32_t slot =
                    map1d ? (uint32_t)(trow * (w / 8) + tcol) * (c256 ? 2u : 1u)
                          : (uint32_t)(trow * 32 + (c256 ? tcol * 2 : tcol));
                const uint32_t cell = kObjVram + tile * boundary + slot * 32u;
                uint32_t index;
                if (c256) {
                    index = *reinterpret_cast<volatile uint8_t *>(cell + fy * 8u + fx);
                } else {
                    const uint8_t b =
                        *reinterpret_cast<volatile uint8_t *>(cell + fy * 4u + fx / 2);
                    index = (fx & 1) ? (b >> 4) : (b & 0xF);
                }
                /* An opaque texel puts the pixel inside the window; the colour
                   it would have had is discarded. A window sprite is never
                   drawn, which is why raster_obj skips mode 2 outright. */
                if (index) g_objwin[py][px] = 1;
            }
        }
    }
}

void read_windows(uint32_t dispcnt, Windows &w) {
    w.on[0] = (dispcnt >> 13) & 1;
    w.on[1] = (dispcnt >> 14) & 1;
    /* DISPCNT bit 15 is the OBJ WINDOW ENABLE, and it arms the window unit
       exactly as bits 13 and 14 do. It used to be unread here, so a scene that
       enabled ONLY the OBJ window left w.any false, window_mask returned 0x3F,
       and every layer was drawn everywhere -- WININ and WINOUT ignored. That is
       what buried scene 366's artwork: see the note above build_objwin. */
    w.obj_on = ((dispcnt >> 15) & 1) && !objwin_off_env();
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
        if (w.x2[i] <= w.x1[i]) w.x2[i] = 256;
        if (w.y2[i] <= w.y1[i]) w.y2[i] = 192;
    }
    const uint16_t winin = rd16(kRegBase + 0x48);
    w.in[0] = winin & 0x3F;
    w.in[1] = (winin >> 8) & 0x3F;
    w.out = rd16(kRegBase + 0x4A) & 0x3F;
}

/* Hardware precedence is fixed: window 0 beats window 1, window 1 beats the OBJ
   window, and WINOUT covers what is in none of them. Same order as
   ntr/ppu_sub.cpp's. */
inline unsigned window_mask(const Windows &w, int x, int y) {
    if (!w.any) return 0x3F;
    for (int i = 0; i < 2; ++i)
        if (w.on[i] && x >= w.x1[i] && x < w.x2[i] && y >= w.y1[i] && y < w.y2[i])
            return w.in[i];
    if (w.obj_on && g_objwin[y][x]) return w.obj_in;
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
//
// THE PRIORITY BYTE IS THE SECOND MEASUREMENT HALF, and it is here for the 3D
// layer. The owner byte says WHICH layer the viewer sees; it does not say
// whether that layer is allowed to be in front of the 3D engine's output,
// which is a question about the layer's PRIORITY and not its identity. Two
// pixels owned by BG2 on the same frame can answer differently if the game
// moves BG2CNT's priority field between them.
struct Cell { uint32_t color; bool hit; uint8_t owner; uint8_t prio; };
Cell g_a[192][256];

// Owner ids: 0-3 are BG0..BG3 and 4 is the sprite layer. The same numbering is
// the bit position in the layer switch below, so a mask and an attribution row
// read against each other without a translation table.
enum { kOwnerObj = 4, kOwnerN = 5 };
const char *const kOwnerName[kOwnerN] = {"BG0", "BG1", "BG2", "BG3", "OBJ"};

// Is the layer that owns a composited pixel BEHIND the 3D layer?
//
// The DS orders the four BGs by (priority, BG index), lower first, so among
// equal priorities BG0 -- which in 3D mode IS the 3D layer -- is the frontmost
// of them. A BG1..BG3 pixel at the SAME priority number as BG0 therefore loses
// to the 3D layer, which is why the BG test is >= and not >.
//
// Sprites are the exception the hardware makes: OBJ at priority p draws in
// FRONT of a BG at the same priority p, so a sprite only loses to the 3D layer
// when its own priority number is strictly greater. On scene 390 that is what
// keeps the four HUD sprites (priority 0) on top of the 3D layer (priority 1)
// while BG2 (priority 3) goes behind it.
//
// Owner 0 cannot reach here while the 3D layer is in the picture: read_bg
// disables BG0 exactly then, so no pixel is ever owned by it.
inline bool layer_behind_3d(unsigned owner, int prio, int p3d) {
    return (owner == (unsigned)kOwnerObj) ? (prio > p3d) : (prio >= p3d);
}

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

/* THE 3D LAYER'S OWN ROW, in HOST framebuffer pixels rather than DS ones,
   because that is the resolution the decision is made at: the blit tests
   coverage per host pixel so a 3D pixel keeps its full detail instead of being
   flattened to one sample per DS cell.
   g_kept3d is where the 3D engine drew AND no 2D layer in front of it covered;
   g_buried3d is where the 3D engine drew and a 2D layer in front DID cover.
   The pair is the whole before/after of this file's priority half: with the
   old unconditional overwrite every covered pixel is buried and kept is 0. */
uint32_t g_kept3d, g_buried3d;

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
    std::fprintf(stderr, "[msgcomp]   3D  %6u px kept, %u px buried under a 2D "
                 "layer in front of it (host %dx%d px)%s\n", g_kept3d,
                 g_buried3d, ntr::SCREEN_W, ntr::SCREEN_H,
                 prio3d_off_env() ? "  [SM64DS_3D_PRIORITY_OFF=1]" : "");
    /* WREND probe: what COLOUR each layer resolved to on that same last frame.
       Owning a pixel and painting a visible one are different facts, and a
       layer that composites full-screen in black is invisible while every
       attribution row above still reads healthy. */
    for (int o = 0; o < kOwnerN; ++o) {
        if (!g_attrib[o].px) continue;
        uint32_t black = 0, n = 0;
        uint32_t top[4] = {0}; uint32_t cnt[4] = {0};
        for (int y = 0; y < 192; ++y)
            for (int x = 0; x < 256; ++x) {
                if (!g_a[y][x].hit || g_a[y][x].owner != o) continue;
                ++n;
                const uint32_t c = g_a[y][x].color & 0xFFFFFFu;
                if (((c >> 16) & 0xFF) < 16 && ((c >> 8) & 0xFF) < 16 &&
                    (c & 0xFF) < 16) ++black;
                for (int k = 0; k < 4; ++k) {
                    if (cnt[k] && top[k] != c) continue;
                    top[k] = c; ++cnt[k]; break;
                }
            }
        std::fprintf(stderr, "[msgcomp]   %-3s colour: %u px, %u near-black "
                     "(%.1f%%), first colours %06x x%u %06x x%u\n",
                     kOwnerName[o], n, black, n ? 100.0 * black / n : 0.0,
                     top[0], cnt[0], top[1], cnt[1]);
    }
    /* And the engine A BG palette AS IT STANDS AT EXIT, next to the frame-0
       dump above: a palette that is full at InitResources and empty at the end
       is a different defect from one that never loaded. */
    {
        int nz = 0;
        for (int e = 0; e < 256; ++e)
            if (rd16(kPlttBase + e * 2)) ++nz;
        std::fprintf(stderr, "[msgcomp]   engA BG palette AT EXIT: %d/256 "
                     "entries nonzero\n", nz);
        uint32_t inwin = 0;
        for (int y = 0; y < 192; ++y)
            for (int x = 0; x < 256; ++x) if (g_objwin[y][x]) ++inwin;
        const uint32_t dc = rd32(kRegBase);
        std::fprintf(stderr, "[msgcomp]   OBJ WINDOW: DISPCNT bit 15 = %u, "
                     "WINOUT %04x (out %02x, objwin-in %02x), mask covers "
                     "%u/49152 px on the last frame\n", (dc >> 15) & 1u,
                     rd16(kRegBase + 0x4A), rd16(kRegBase + 0x4A) & 0x3F,
                     (rd16(kRegBase + 0x4A) >> 8) & 0x3F, inwin);
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

/* ---- SM64DS_ENGA_LAYERS: engine A's layers, per frame, in whatever arm ----
 *
 * WHY IT EXISTS AND WHAT IT REPLACES. The one-shot dump above fires on the
 * FIRST frame that finds any 2D enabled and again at the peak-coverage frame,
 * and it reads its per-BG lines through read_bg, which returns an empty config
 * for a layer DISPCNT has not enabled. Both of those cost a lane a wrong
 * conclusion: a layer that is configured and switched on later reads as absent,
 * and a scroll register that moves during the scene is never sampled twice.
 * Scene 368 is exactly that shape -- BG2CNT is a 512x256 layout that the layer
 * mask does not carry on frame one -- and the headroom lane concluded "there is
 * nothing above the screen to draw" off a single frame of a single arm.
 *
 * So this reads the RAW registers for all four backgrounds whether or not they
 * are enabled, every frame, and prints on the first frame, on every change of
 * the whole register signature, and once every 60 frames so a still scene still
 * leaves a trail. The pixel census under it is the finished hit buffer, which
 * is the only place "which layer did the viewer actually see" is answerable.
 *
 * IT IS THE ARM-COMPARISON TOOL, and that is its point. GaplessMinigames writes
 * zero into the game's own G, and whether a scene's BACKGROUND scroll follows G
 * is a per-scene fact nobody can argue from source: the framework's own BG
 * plotter (func_ov004_020ae3b4) adds G + 0xc0 for the top engine, so a scene
 * whose layers are placed through it moves when G moves and a scene that writes
 * BGnVOFS itself may not. Run this in both arms and diff the two logs.
 */
int enga_layers_on(void)
{
    static int on = -1;
    if (on < 0) {
        const char *s = std::getenv("SM64DS_ENGA_LAYERS");
        on = s && *s && *s != '0';
    }
    return on;
}

unsigned g_enga_frame;

/* `mask` is data_0209d45c, the software layer mask, passed IN rather than
   declared here: this block is inside the anonymous namespace, where an extern
   declaration takes internal linkage and names a variable nothing defines. The
   one caller is inside the extern "C" function that already declares it. */
void enga_layer_regs(uint32_t dispcnt, unsigned mask)
{
    if (!enga_layers_on()) return;
    ++g_enga_frame;
    /* THE SIGNATURE IS EVERY REGISTER THIS PROBE PRINTS, so "no line since f12"
       means "nothing this probe can see has changed since f12" and not "nobody
       looked". Scroll registers included: a scroll that moves once mid-scene is
       the whole question here. */
    uint32_t sig = dispcnt ^ (mask << 3);
    for (int b = 0; b < 4; ++b)
        sig = sig * 131u + rd16(kRegBase + 0x08 + b * 2)
              + (uint32_t)rd16(kRegBase + 0x10 + b * 4) * 7u
              + (uint32_t)rd16(kRegBase + 0x12 + b * 4) * 13u;
    static uint32_t last_sig;
    static int have;
    const int changed = !have || sig != last_sig;
    if (!changed && g_enga_frame % 60) return;
    have = 1;
    last_sig = sig;
    std::fprintf(stderr, "[engA] f%u DISPCNT=%08x d45c=%02x enables=%02x "
                 "bg0_3d=%d%s\n", g_enga_frame, dispcnt, mask,
                 (dispcnt >> 8) & 0x1F,
                 bg0_is_3d(dispcnt) ? 1 : 0, changed ? " CHANGED" : "");
    for (int b = 0; b < 4; ++b) {
        const uint16_t cnt = rd16(kRegBase + 0x08 + b * 2);
        const unsigned sz = (cnt >> 14) & 3;
        const int tw = (sz & 1) ? 64 : 32, th = (sz & 2) ? 64 : 32;
        const uint32_t char_off = ((dispcnt >> 24) & 7) << 16;
        const uint32_t scr_off = ((dispcnt >> 27) & 7) << 16;
        /* READ WHETHER OR NOT IT IS ENABLED. A disabled layer's registers are
           still the game's own statement about where that layer would be, and
           this probe's whole job is to be readable before the layer is on. */
        std::fprintf(stderr, "[engA] f%u   BG%d %-8s cnt=%04x prio=%d bpp%d "
                     "%dx%d screen=%08x chars=%08x ofs(%d,%d)\n",
                     g_enga_frame, b,
                     ((dispcnt >> (8 + b)) & 1)
                         ? (b == 0 && bg0_is_3d(dispcnt) ? "3D" : "ON")
                         : "off",
                     cnt, cnt & 3, ((cnt >> 7) & 1) ? 8 : 4, tw, th,
                     kVramBase + scr_off + (((cnt >> 8) & 0x1F) << 11),
                     kVramBase + char_off + (((cnt & 0x3c) >> 2) << 14),
                     rd16(kRegBase + 0x10 + b * 4) & 0x1FF,
                     rd16(kRegBase + 0x12 + b * 4) & 0x1FF);
    }
}

/* THE SAME FRAME'S PIXELS, after the whole 2D raster has run. Printed on the
   frames the register half printed, so one grep gives a layer's configuration
   and what it actually put on the screen side by side. `top32` is the count in
   engine rows 0..31 alone, which is the band the gapless headroom has to
   reproduce: it is the world's rows -224..-193 in the ROM's own gapped frame,
   and a layer with nothing there has nothing for the strip to draw. */
void enga_layer_px(void)
{
    if (!enga_layers_on()) return;
    static unsigned last_printed;
    if (last_printed == g_enga_frame) return;
    last_printed = g_enga_frame;
    unsigned px[kOwnerN] = {0}, top32[kOwnerN] = {0};
    int y0[kOwnerN], y1[kOwnerN];
    for (int o = 0; o < kOwnerN; ++o) { y0[o] = 192; y1[o] = -1; }
    for (int y = 0; y < 192; ++y)
        for (int x = 0; x < 256; ++x) {
            if (!g_a[y][x].hit) continue;
            const unsigned o = g_a[y][x].owner;
            ++px[o];
            if (y < 32) ++top32[o];
            if (y < y0[o]) y0[o] = y;
            if (y > y1[o]) y1[o] = y;
        }
    for (int o = 0; o < kOwnerN; ++o)
        if (px[o])
            std::fprintf(stderr, "[engA] f%u   %-3s %6u px, rows %d..%d, "
                         "%u of them in engine rows 0..31\n", g_enga_frame,
                         kOwnerName[o], px[o], y0[o], y1[o], top32[o]);
}

/* ---- WHAT EACH BACKGROUND HOLDS ABOVE THE SCREEN --------------------------
 *
 * THE MEASUREMENT THE HEADROOM LANE OWED AND DID NOT TAKE. Engine A's 2D
 * backgrounds are maps that WRAP: sample_bg masks (y + vofs) with the map's own
 * height, so engine row -1 is a real map row and not an error. Which map row it
 * is depends on the layer, and the two layers scene 368 runs disagree:
 *
 *   BG3  32x32 tiles, 256 px tall, vofs 0    engine -32..-1 -> map rows 224..255
 *   BG2  64x32 tiles, 256 px tall, vofs 64   engine -32..-1 -> map rows  32..63
 *
 * so "the rows above the screen" is a different question per layer and cannot
 * be answered for the engine as a whole. This prints, per enabled background,
 * how many of the 32x256 texels above the screen are opaque, how many of the 32
 * rows AT the top of the screen are, and whether the two bands are the same
 * picture -- because if they are, drawing the upper band into a strip above the
 * lower one paints the same artwork twice.
 */
void enga_strip_probe(uint32_t dispcnt)
{
    if (!enga_layers_on()) return;
    static unsigned last;
    if (last == g_enga_frame) return;
    last = g_enga_frame;
    Windows win;
    read_windows(dispcnt, win);
    for (int b = 0; b < 4; ++b) {
        const BgConfig c = read_bg(b, dispcnt);
        if (!c.enabled) continue;
        unsigned above = 0, top = 0, same = 0;
        for (int k = 0; k < 32; ++k)
            for (int x = 0; x < 256; ++x) {
                uint32_t a = 0, t = 0;
                const bool ha = sample_bg(c, x, k - 32, a);
                const bool ht = sample_bg(c, x, k, t);
                if (ha) ++above;
                if (ht) ++top;
                if (ha == ht && (!ha || a == t)) ++same;
            }
        const int vh = c.tiles_h * 8;
        std::fprintf(stderr, "[engA] f%u   BG%d strip: engine rows -32..-1 are "
                     "map rows %d..%d, %u/8192 opaque; engine rows 0..31 are "
                     "map rows %d..%d, %u/8192 opaque; the two bands agree on "
                     "%u/8192 pixels\n", g_enga_frame, b,
                     ((-32 + c.vofs) % vh + vh) % vh,
                     ((-1 + c.vofs) % vh + vh) % vh, above,
                     c.vofs % vh, (31 + c.vofs) % vh, top, same);
    }
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
    /* THE DISPLAY SHIFT, read once for the walk. See the note over
       hal_gapless_obj_shift_ds at the top of this file: it is zero on every
       path in the program but one, and where it is not zero it puts this
       engine's sprites back on the rows the ROM submits them at. It is applied
       to the ENTRY's y and not to the pixel, so the screen clip below, the
       window test and the alpha blend are all this engine's own answers for the
       row the texel is really shown on -- the picture engine A composes when
       the framework word carries its own value, which is the picture gap-on
       mode composes. The rows this then clips off the bottom edge are drawn by
       ntr/ppu_sub.cpp's hinge_paint, out of the same entries. */
    const int layer = hal_gapless_obj_raster_shift_ds();
    /* SM64DS_OBJSHIFT_TRACE=1 prints the per-entry census this is proven with,
       and it prints in BOTH ARMS. With the shift on it names the DISPLAY row
       each entry lands on; with it off it names the row the engine put it on,
       which on a gap-on run is the row the ROM itself submits at. Those two
       numbers are the whole claim, and port/tools/objshift.py reads exactly
       these lines and compares them frame for frame. Off, this loop is the loop
       it was. */
    const int trace = objshift_trace_on();
    /* Read once for the walk: the A/B arm for the OBJ-vs-OBJ priority test. */
    const bool objprio_off = objprio_off_env();
    /* and for the OBJ-vs-BG one, plus the DS pixel the transcript follows */
    const bool objbg_off = objbg_off_env();
    int probe_x, probe_y;
    objbg_probe_pixel(probe_x, probe_y);
    ++g_obj_frame;
    /* the transcript's first line: what the BACKGROUNDS left at the probed
       pixel, before a single sprite is read. Without it a reader cannot tell a
       sprite that lost from a sprite that never covered the pixel. */
    if (probe_x >= 0) {
        const Cell &c0 = g_a[probe_y][probe_x];
        std::fprintf(stderr, "[objbg] f%u (%d,%d) BG loop left %s prio %u "
                     "colour %06x  [objprio_off=%d objbg_off=%d]\n",
                     g_obj_frame, probe_x, probe_y,
                     c0.hit ? kOwnerName[c0.owner] : "(nothing)",
                     c0.hit ? c0.prio : 0u, c0.hit ? (c0.color & 0xFFFFFFu) : 0u,
                     objprio_off ? 1 : 0, objbg_off ? 1 : 0);
    }

    for (int i = 127; i >= 0; --i) {
        if (ntr::ppu_seam_snow_owns(rd16(oam_a() + i * 8u + 4)))
            continue;   /* the seam-snow overlay owns these while engaged */
        /* SM64DS_OAMAGE_TRACE: the top screen's half of the age probe. */
        {
            static int tget = -1;
            if (tget < 0) {
                const char *e = std::getenv("SM64DS_OAMAGE_TRACE");
                tget = e && *e && *e != '0';
            }
            if (tget && rd16(oam_a() + i * 8u + 4) == 0x1010)
                std::fprintf(stderr, "[oamage] T f%u src=%08x slot%d y=%d\n",
                             g_obj_frame, (unsigned)oam_a(), i,
                             (int)(rd16(oam_a() + i * 8u) & 0xff));
        }
        const uint16_t a0 = rd16(oam_a() + i * 8u);
        const uint16_t a1 = rd16(oam_a() + i * 8u + 2);
        const uint16_t a2 = rd16(oam_a() + i * 8u + 4);
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
        // THE MASK IS MODELLED NOW, and not here: build_objwin above walks OAM
        // for mode-2 entries alone and lays the region down before the BG loop
        // runs, so the BGs can be masked with it. This raster still SKIPS mode
        // 2, because a window sprite contributes a shape and never a colour.
        // The line this replaced called the two-pass order a structural limit;
        // the order was real and the limit was not, because the mask needs the
        // sprite SHAPES only and those are available before any colour is.
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
        /* AFTER THE WRAP, NEVER BEFORE IT. The 8-bit y is the hardware's, and
           the sign it carries comes out of the sprite's own box height; adding
           to it first would turn a sprite parked at 224 into one at 256 and
           lose the negative row it was standing on. */
        const int shift = layer + ntr::ppu_obj_routed_live_resid(i);
        y += shift;
        const bool c256 = a0 & 0x2000;
        const bool hflip = !affine && (a1 & 0x1000);
        const bool vflip = !affine && (a1 & 0x2000);
        const uint32_t tile = a2 & 0x3FF;
        const uint32_t pal = (a2 >> 12) & 0xF;
        /* attribute 2 bits 10-11, this sprite's own priority. Read here rather
           than at the store because the OBJ-vs-OBJ test below needs it before
           a texel is written. */
        const uint8_t prio = (uint8_t)((a2 >> 10) & 3);

        /* ONE LINE PER DRAWABLE ENTRY, before a texel is read, so it is the
           SUBMISSION and the clip rather than a count of what survived. `at`
           is the engine row the game put the box on, WITHOUT the shift, which
           is the number the two arms differ in; `display` is where it is drawn.
           `top` is how many of the box's rows the top screen keeps and `band`
           how many fall past its bottom edge into the rows hinge_paint draws;
           `above` is what is lost off the top, which is what the ROM loses too
           when its own submission runs off that edge. */
        if (trace) {
            const int d0 = y, d1 = y + bh - 1;
            int above = 0, on = 0, band = 0;
            for (int k = d0; k <= d1; ++k) {
                if (k < 0) ++above;
                else if (k < 192) ++on;
                else ++band;
            }
            std::fprintf(stderr, "[objshift] f%u oam%3d a0=%04x a1=%04x "
                         "a2=%04x %dx%d%s at (%d,%d) shift %d display %d..%d: "
                         "%d above, %d top, %d band\n", g_obj_frame, i, a0, a1,
                         a2, w, h, dbl ? " dbl" : (affine ? " aff" : ""), x,
                         y - shift, shift, d0, d1, above, on, band);
        }

        int pa = 256, pb = 0, pc = 0, pd = 256;
        if (affine) {
            const int grp = (a1 >> 9) & 0x1F;
            pa = (int16_t)rd16(oam_a() + (grp * 4 + 0) * 8u + 6);
            pb = (int16_t)rd16(oam_a() + (grp * 4 + 1) * 8u + 6);
            pc = (int16_t)rd16(oam_a() + (grp * 4 + 2) * 8u + 6);
            pd = (int16_t)rd16(oam_a() + (grp * 4 + 3) * 8u + 6);
        }

        /* SM64DS_STAR_AFF: the engine-A OBJ affine trace. This raster is the
           ONLY live consumer of engine A's sprites (ntr/ppu.cpp's
           ppu_scanout_obj has no caller outside tests/smoke_oam.cpp), so a
           probe placed there measures nothing. */
        {
            static int on = -1;
            if (on < 0) on = std::getenv("SM64DS_STAR_AFF") ? 1 : 0;
            if (on && affine)
                std::fprintf(stderr, "[staraff] f%u oam%d grp=%d %dx%d dbl=%d "
                             "at(%d,%d) tile=%u pa=%d pb=%d pc=%d pd=%d\n",
                             g_obj_frame, i, (int)((a1 >> 9) & 0x1F), w, h,
                             dbl ? 1 : 0, x, y, (unsigned)tile, pa, pb, pc, pd);
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
                /* OBJ-vs-OBJ IS RESOLVED BY PRIORITY, NOT BY OAM INDEX, and
                   this test is what makes that true on engine A. GBATEK's OAM
                   notes: attribute 2 bits 10-11 order a sprite against the
                   other sprites, and the OAM index only breaks a tie between
                   equal priorities. The DS shows ONE OBJ pixel per screen
                   pixel, so a sprite that loses here contributes nothing at
                   all -- not a colour, not a semi-transparent flag.

                   The walk above is 127 -> 0, so the overwrite test is
                   "empty, or a different layer, or new prio <= old prio":
                   `<=` keeps the LAST writer on a tie, which on this walk is
                   the lowest OAM index, and a strictly better priority number
                   wins whatever the index. This is ntr/ppu_sub.cpp's
                   raster_obj line for line -- run link60 lane BLND2 landed it
                   on engine B and the comment that stood here said engine A's
                   half was "a separate gap and not this lane's". Run mg8 lane
                   SGX is that lane: scene 375 (dScMgCurling2_c, Shell Smash)
                   puts its whole playfield on ENGINE A -- six shells at
                   priority 1, their six semi-transparent reflections at
                   priority 2, and the falling snow at priority 0, measured
                   with SM64DS_PPU_AUDIT -- and the snow carries the HIGHEST
                   OAM indices, so index order drew every shell and every
                   reflection over it.

                   THE OWNER TEST IS PART OF THE RULE AND NOT AN OPTIMISATION.
                   g_a is a shared buffer: the BG loop has already written
                   background pixels into it with their own priorities, and a
                   sprite must not lose to a BG under THIS test, whose tie rule
                   is the sprite-vs-sprite one. OBJ-vs-BG is a different
                   hardware rule with a different tie, and it is the test
                   immediately below. Run mg8 lane SGX left that one out
                   deliberately because folding the two together -- by dropping
                   this owner check -- would have applied the WRONG tie to
                   backgrounds and moved every scene in the game inside a lane
                   scoped to sprite ordering. The two are separate tests here
                   for exactly that reason, and they have separate kill
                   switches. */
                if (!objprio_off && g_a[py][px].hit
                    && g_a[py][px].owner == kOwnerObj
                    && prio > g_a[py][px].prio) {
                    if (px == probe_x && py == probe_y)
                        std::fprintf(stderr, "[objbg] f%u (%d,%d) oam%3d "
                                     "mode%u prio%u texel %06x: SKIP, OBJ prio "
                                     "%u is in front\n", g_obj_frame, px, py, i,
                                     objmode, prio, color & 0xFFFFFFu,
                                     g_a[py][px].prio);
                    continue;
                }
                /* OBJ-vs-BG, run mg10 lane RGX. GBATEK, OAM attribute 2 bits
                   10-11: the field is the sprite's "Priority relative to BG
                   (0-3; 0=Highest)", and where it equals a background layer's
                   priority the sprite "becomes higher priority and is
                   displayed on top of that BG layer". So the sprite loses ONLY
                   to a background whose priority NUMBER is strictly smaller,
                   and the test is `<` rather than `<=`.

                   g_a's owner is the FRONTMOST background at this pixel, not
                   an arbitrary one: the BG loop walks prio 3 -> 0 and, within a
                   priority, BG3 -> BG0, so the last background to write is the
                   one GBATEK's BGxCNT note puts in front ("In case that some or
                   all BGs are set to same priority then BG0 is having the
                   highest, and BG3 the lowest priority"). Comparing against
                   that one cell answers the whole layer: if the sprite loses to
                   the frontmost background it is hidden, and if it wins it is
                   in front of every background at this pixel.

                   WHAT THIS ALSO FIXES, WITHOUT BEING ABOUT BLENDING. The
                   semi-transparent path below blends against g_a's colour, and
                   before this test that colour could be a background IN FRONT
                   of the sprite -- a blend against a layer the hardware would
                   never have put underneath it. Those pixels are now skipped
                   outright, which is the same answer the hardware gives.

                   THE 3D LAYER IS NOT IN THIS TEST AND DOES NOT NEED TO BE:
                   read_bg disables BG0 whenever it is the 3D layer, so no g_a
                   cell is owned by it, and OBJ-vs-3D is resolved at the final
                   blit by layer_behind_3d -- `prio > p3d` for a sprite, which
                   is this same GBATEK tie written for that layer. */
                if (!objbg_off && g_a[py][px].hit
                    && g_a[py][px].owner != kOwnerObj
                    && g_a[py][px].prio < prio) {
                    if (px == probe_x && py == probe_y)
                        std::fprintf(stderr, "[objbg] f%u (%d,%d) oam%3d "
                                     "mode%u prio%u texel %06x: SKIP, %s prio "
                                     "%u is in front\n", g_obj_frame, px, py, i,
                                     objmode, prio, color & 0xFFFFFFu,
                                     kOwnerName[g_a[py][px].owner],
                                     g_a[py][px].prio);
                    continue;
                }
                if (px == probe_x && py == probe_y)
                    std::fprintf(stderr, "[objbg] f%u (%d,%d) oam%3d mode%u "
                                 "prio%u texel %06x: WRITE over %s%u\n",
                                 g_obj_frame, px, py, i, objmode, prio,
                                 color & 0xFFFFFFu,
                                 g_a[py][px].hit
                                     ? kOwnerName[g_a[py][px].owner] : "(none)",
                                 g_a[py][px].hit ? g_a[py][px].prio : 0);
                // OBJ mode 1 (semi-transparent) alpha-blends with the layer
                // directly below it -- the BG already composited into g_a here,
                // or the 3D framebuffer (BG0 in 3D mode) where no 2D covers the
                // pixel -- whatever BLDCNT's first-target bits say. Window bit 5
                // gates the effect. No 2nd target below: drawn opaque (hardware).
                //
                // ONE DIVERGENCE REMAINS AND IS NAMED RATHER THAN HIDDEN: when
                // a semi sprite WINS this test over an already-written sprite
                // pixel, it blends against that loser's colour instead of
                // against the 2nd-target layer below the whole OBJ layer, which
                // this file no longer holds.
                //
                // TWO SHAPES REACH IT, not the one this comment used to name
                // (run mg8 reviewer, and ppu_blend.txt section 3 carries the
                // long form):
                //   * a semi OBJ whose priority is at least as good as a
                //     NORMAL OBJ it overlaps -- neither shell game has one,
                //     reflections are priority 2, the worst number any sprite
                //     in either scene carries;
                //   * TWO SEMI OBJs AT EQUAL PRIORITY overlapping each other.
                //     The lower-index one wins the tie, finds an OBJ pixel
                //     underneath, and since BLDCNT_A names no OBJ second
                //     target it writes opaque. The reflections ARE this shape.
                // Both are PRE-EXISTING -- the old unconditional path did the
                // same thing -- so this fix makes neither worse nor more
                // frequent. Closing it means resolving OBJ into its own
                // buffer the way ntr/ppu_sub.cpp does.
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
                /* The priority the OBJ-vs-OBJ test above, the OBJ-vs-BG test
                   at the top of this loop, and the 3D layer's test in the
                   final blit all read. */
                g_a[py][px].prio = prio;
            }
        }
    }
    /* and the transcript's last line: what the whole 2D unit resolved to. The
       final blit still gets a say (layer_behind_3d), so this is the 2D answer
       and not necessarily the pixel on screen -- said here rather than left for
       a reader to assume. */
    if (probe_x >= 0) {
        const Cell &c1 = g_a[probe_y][probe_x];
        std::fprintf(stderr, "[objbg] f%u (%d,%d) 2D result: %s prio %u colour "
                     "%06x (the 3D layer's priority is resolved at the blit)\n",
                     g_obj_frame, probe_x, probe_y,
                     c1.hit ? kOwnerName[c1.owner] : "(nothing)",
                     c1.hit ? c1.prio : 0u, c1.hit ? (c1.color & 0xFFFFFFu) : 0u);
    }
}

}  // namespace

/* func_02019144's FIRST beat, hal/scene_boot.cpp. 1 = run that function's
   tail, 0 = the current graphics block did the display sync itself. */
extern "C" int port_graph_block_beat(void);
/* func_02019144's OAM upload, at the ROM's line in the frame; the body and the
   whole derivation are in hal/sub_screen.cpp, which owns OAM::Load's wiring. */
extern "C" void port_frame_oam_upload(void);

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
    /* func_02019144 LINES 44-45, THE OAM UPLOAD, and it belongs HERE.
       The ROM's order is slot-2 beat, OAM::Flush, OAM::Load, and only then the
       two layer publishes -- all of it in VBlank, so the frame it configures is
       scanned out afterwards and the sprites on screen are the ones this upload
       just placed. This program used to upload at the FOOT of
       hal_sub_screen_present, below both OBJ rasters, which left every sprite
       two beats behind the POWCNT1 arm it was submitted for and put the whole
       dScMgD3DBase_c family's 2D on the wrong screen. See THE OBJ/POWCNT1
       PARITY in hal/sub_screen.cpp for the ROM derivation, the measurement and
       the SM64DS_OAM_LOAD_LATE kill switch. Under the beat's own verdict for
       func_02019144's own reason: with the block answering 0 the ROM returns
       before reaching this line. */
    if (run_tail)
        port_frame_oam_upload();
    {
        extern unsigned char data_0209d45c;
        if (!publish_off && run_tail)
            *reinterpret_cast<volatile uint32_t *>(kRegBase) =
                (rd32(kRegBase) & ~0x1F00u) | ((uint32_t)data_0209d45c << 8);
    }

    const uint32_t dispcnt = rd32(kRegBase);
    const unsigned disp_mode = (dispcnt >> 16) & 3;
    /* the per-frame layer census, BEFORE the forced-blank exit below, because
       a frame the compositor declines is still a frame whose registers are the
       game's own statement about its layers. */
    {
        extern unsigned char data_0209d45c;
        enga_layer_regs(dispcnt, (unsigned)data_0209d45c);
    }
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

    /* THE OBJ-WINDOW MASK IS BUILT BEFORE THE BG LOOP, which is the whole point
       of it being a separate pass: the BGs below are masked per pixel and the
       mask has to exist by then. raster_obj still runs after them and still
       skips mode-2 sprites, so no sprite is drawn twice. */
    build_objwin(dispcnt);

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
                        g_a[y][x].prio = (uint8_t)prio;
                    }
                }
            }
        }
    }

    if (obj_on && (lmask & (1u << kOwnerObj)))
        raster_obj(dispcnt, bl, win, fb);

    enga_layer_px();
    enga_strip_probe(dispcnt);

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
            /* WREND probe: WHICH palette entries the enabled BGs actually
               index, and what those entries hold. A BG that composites
               full-screen and reads black is either indexing a bank nobody
               loaded or reading the wrong palette base, and the two are told
               apart by printing the bank histogram beside the palette. */
            for (int b = 0; b < 4; ++b) {
                if (!bgs[b].enabled) continue;
                unsigned bank[16] = {0};
                for (int ty = 0; ty < 24; ++ty)
                    for (int tx = 0; tx < 32; ++tx)
                        ++bank[(rd16(screen_entry_addr(bgs[b], tx, ty)) >> 12) & 0xF];
                std::fprintf(stderr, "[msgcomp]   BG%d screen-entry palette banks:", b);
                for (int k = 0; k < 16; ++k)
                    if (bank[k]) std::fprintf(stderr, " bank%d x%u", k, bank[k]);
                std::fprintf(stderr, "\n");
                /* the 4bpp index histogram this BG actually samples, and how
                   much of its char region carries data at all */
                unsigned idx[16] = {0};
                for (int y = 0; y < 192; ++y)
                    for (int x = 0; x < 256; ++x) {
                        const uint16_t se = rd16(screen_entry_addr(bgs[b], x >> 3, y >> 3));
                        const uint8_t pr = *reinterpret_cast<volatile uint8_t *>(
                            bgs[b].chars + (se & 0x3FF) * 32 + (y & 7) * 4 + ((x & 7) >> 1));
                        ++idx[(x & 1) ? (pr >> 4) : (pr & 0xF)];
                    }
                int filled = 0;
                for (int t = 0; t < 1024; ++t)
                    for (int k = 0; k < 32; ++k)
                        if (*reinterpret_cast<volatile uint8_t *>(bgs[b].chars + t * 32 + k)) {
                            ++filled; break;
                        }
                std::fprintf(stderr, "[msgcomp]   BG%d chars %08x %d/1024 tiles "
                             "with data; 4bpp index histogram:", b, bgs[b].chars, filled);
                for (int k = 0; k < 16; ++k)
                    if (idx[k]) std::fprintf(stderr, " i%d x%u", k, idx[k]);
                std::fprintf(stderr, "\n");
            }
            {
                int nz = 0;
                for (int e = 0; e < 256; ++e)
                    if (rd16(kPlttBase + e * 2)) ++nz;
                std::fprintf(stderr, "[msgcomp]   engA BG palette %08x: %d/256 "
                             "entries nonzero\n", kPlttBase, nz);
                for (int bnk = 0; bnk < 16; ++bnk) {
                    int bnz = 0;
                    for (int e = 0; e < 16; ++e)
                        if (rd16(kPlttBase + (bnk * 16 + e) * 2)) ++bnz;
                    if (bnz) std::fprintf(stderr, "[msgcomp]     bank%-2d %2d/16 "
                                          "nonzero, e0..e3 %04x %04x %04x %04x\n",
                                          bnk, bnz,
                                          rd16(kPlttBase + (bnk * 16 + 0) * 2),
                                          rd16(kPlttBase + (bnk * 16 + 1) * 2),
                                          rd16(kPlttBase + (bnk * 16 + 2) * 2),
                                          rd16(kPlttBase + (bnk * 16 + 3) * 2));
                }
            }
        }
    }

    // Composite the covered 2D pixels over the 3D framebuffer. The window's OBJ
    // and colour-effect bits are already resolved above for BGs; the sprite
    // raster is drawn wherever OAM places it (the cursor is inside the box
    // window in practice). Integer scale from DS 256x192 to SCREEN_W x SCREEN_H.
    //
    // THE 3D LAYER'S PRIORITY IS RESOLVED HERE AND NOT IN THE BG LOOP, and the
    // reason is resolution. The 2D unit is modelled at the DS's own 256x192;
    // the 3D frame is already in the framebuffer at the host tier, 512x384 at
    // the window's 2x. Sampling one 3D pixel per DS cell and re-writing the
    // whole 2x2 block with it would throw away three quarters of the 3D
    // image's detail to answer a question that can be asked per HOST pixel
    // instead. So a DS cell whose owner is BEHIND the 3D layer writes only the
    // host pixels the 3D engine did NOT draw, and leaves the rest untouched --
    // which is also why there is no colour blit for the 3D layer at all: its
    // pixels are already where they belong.
    const int sx = ntr::active_w / 256;
    const int sy = ntr::active_h / 192;
    /* WIDESCREEN HUD REANCHOR (16:9). The DS composes the top HUD in its own
       256x192 space. On the 4:3 tiers sx == sy, so one integer scale fills the
       screen with no distortion. At 1024x576 sx (4) != sy (3): the old x*sx
       mapping would stretch the HUD 33% wider than tall AND smear it across the
       whole 1024, hugging the left. Instead scale UNIFORMLY by sy (no stretch,
       square blocks) and ANCHOR each source-x band horizontally -- sides out,
       middle stays:
         - left band  (source x <  band_l): rides the LEFT edge   (hx = x*uni)
         - right band (source x >= band_r): rides the RIGHT edge   (+ margin)
         - centre band                     : stays centred          (+ margin/2)
       `margin` is the spare width the un-stretched HUD leaves (1024-768 = 256).
       The band splits are by SOURCE X because the composited buffer has lost
       per-element identity; in SM64DS the top-screen HUD (power meter, coin and
       star counts, lives) clusters top-left, so it rides left at native size,
       and the radar/minimap lives on the bottom (engine B) screen and is not
       touched here at all. See the report for the bands Tango may want to
       adjust per element. The 4:3 tiers keep the exact x*sx block. */
    const int uni = sy;                            /* uniform native scale (3) */
    const int margin = ntr::active_w - 256 * uni;  /* spare width (256; 0 at 4:3) */
    const int band_l = 96, band_r = 160;           /* source-x band splits */
    /* TWO QUESTIONS, KEPT APART SO THE A/B ARM STILL MEASURES. `shown3d` is
       whether the 3D layer is in the picture at all, which decides whether the
       coverage mask means anything; `honour3d` is whether this run obeys its
       priority. The kill switch only clears the second, so the disabled arm
       still counts every 3D pixel it paints over instead of reporting a row of
       zeroes that could be read as "there was no 3D layer". */
    const bool shown3d = bg0_3d_shown(dispcnt);
    const bool honour3d = shown3d && !prio3d_off_env();
    const int p3d = bg0_3d_priority();
    const uint8_t *cover = shown3d ? ntr::gx_coverage() : nullptr;
    uint32_t kept = 0, buried = 0;
    for (int y = 0; y < 192; ++y) {
        for (int x = 0; x < 256; ++x) {
            if (!g_a[y][x].hit) continue;
            const uint32_t c = g_a[y][x].color;
            /* horizontal placement of this DS column's block. In 4:3 (toggle
               off) ntr::widescreen is false and this is the plain x*sx block; in
               16:9 it reanchors. The wide arm also self-normalises -- margin is 0
               and uni == sx at 4:3 -- but the runtime branch keeps the off path
               byte-for-byte the old code. */
            int bw, hx0;
            if (ntr::widescreen) {
            bw = uni;
            /* THE REANCHOR IS FOR A SPARSE HUD OVER A WIDE 3D VIEW, and only
               there. With a 3D layer in the picture (shown3d) the bands it opens
               between the HUD clusters are filled by the 3D field, so pushing the
               power meter left and the star count right reads as a widescreen
               HUD. But a MINIGAME's top screen is a FULL 2D raster with no 3D
               behind it (shown3d is false), and reanchoring that tears it into
               vertical strips with black gaps where the bands separate -- the two
               dark bars a pachinko/smartball board showed. So a full 2D screen is
               PILLARBOXED instead: centred at the uniform scale, black margins
               either side, margin/2 == the bottom panel's pan_x0 (128) so the two
               DS screens line up exactly (ntr/ppu_sub.cpp's stacked wide branch). */
            /* THE MESSAGE LAYER IS NEVER BAND-SPLIT. BG3 (owner 3,
               kOwnerName[3]) is engine A's dialogue/message layer -- its glyph
               tiles are the box text (the box rect itself is blank, so only the
               glyphs are `hit` here). The band-split is right for the sparse
               corner HUD, which is meant to ride out to the edges, but it TEARS
               a line of text: the two margin gaps open between the source-x
               bands, so the run reads ~1.78x wide with 128px voids where the
               splits fall. So BG3 takes the SAME pillarbox placement a full 2D
               screen gets (x*uni + margin/2, centred at the uniform native
               scale) whether or not a 3D layer is behind it -- native size,
               centred, no tear -- while every other BG keeps the approved
               band-split. One condition, no new state, and inert at 4:3 because
               the whole arm is behind `if (ntr::widescreen)`. */
            hx0 = (g_a[y][x].owner == 3)
                          ? x * uni + margin / 2
                          : !shown3d
                          ? x * uni + margin / 2
                          : (x < band_l) ? x * uni
                          : (x >= band_r) ? x * uni + margin
                                          : x * uni + margin / 2;
            } else {
            bw = sx;
            hx0 = x * sx;
            }
            if (!honour3d
                || !layer_behind_3d(g_a[y][x].owner, g_a[y][x].prio, p3d)) {
                /* the owning 2D layer is in FRONT of the 3D layer (or there is
                   no 3D layer in the picture): the old unconditional write */
                for (int dy = 0; dy < sy; ++dy)
                    for (int dx = 0; dx < bw; ++dx) {
                        const int hy = y * sy + dy, hx = hx0 + dx;
                        if (cover && cover[hy * ntr::SCREEN_W + hx]) ++buried;
                        fb.px[hy][hx] = c;
                    }
                continue;
            }
            /* BEHIND the 3D layer: it may only fill where 3D drew nothing */
            for (int dy = 0; dy < sy; ++dy)
                for (int dx = 0; dx < bw; ++dx) {
                    const int hy = y * sy + dy, hx = hx0 + dx;
                    if (cover[hy * ntr::SCREEN_W + hx]) { ++kept; continue; }
                    fb.px[hy][hx] = c;
                }
        }
    }
    /* Pixels the 3D engine drew that NO 2D layer covered at all were never in
       the loop above; they are kept too, and counting them is what makes the
       row comparable with the host framebuffer's own size. A full-screen sweep
       is instrumentation and nothing reads it unless the probe is on, so it is
       gated: an ordinary play frame does not pay for a number nobody prints. */
    if (honour3d && std::getenv("SM64DS_MSG_COMPOSITE_DEBUG")) {
        for (int hy = 0; hy < ntr::active_h; ++hy)
            for (int hx = 0; hx < ntr::active_w; ++hx)
                if (cover[hy * ntr::SCREEN_W + hx]
                    && !g_a[hy / sy][hx / sx].hit)
                    ++kept;
    }
    g_kept3d = kept;
    g_buried3d = buried;
}
