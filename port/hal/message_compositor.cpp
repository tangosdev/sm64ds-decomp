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
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "ntr/gx.h"
#include "ntr/ppu.h"
#include "ntr/rt.h"
#include "hal/gpu_raster.h"
#include "menu_compose.h"   /* run menu1 lane HOOK1: the lift plan (H1) */
#include "hd_meter.h"       /* run hunt8 lane HDMETER1: the HD power meter art out of the enabled mods */
#include <chrono>
#include <vector>
#include "hal/hitch_probe.h"   /* HITCH9C */
/* SAVEOVL1: may the menu layer be used at all (SM64DS_MENU_LAYER, default on)? The card's own readiness is asked per tick. */
static inline bool port_gpu_layer_ready_env()
{
    static int v = -1;
    if (v < 0) { const char *e = std::getenv("SM64DS_MENU_LAYER"); v = (e && *e == '0') ? 0 : 1; }
    return v != 0;
}
/* SAVEOVL1 PROOF: SM64DS_COMPA_ZONES=1 prints where the engine A compose spends a frame (mean ms per frame, split by whether a menu
   lift was live), once at exit. Off: one cached int test per mark. */
namespace {
struct CaZones {
    int on = -1;
    double acc[2][10] = {};
    unsigned long long n[2] = {};
    std::chrono::steady_clock::time_point t;
    int live = 0;
    static void report();
    void begin() {
        if (on < 0) {
            const char *e = std::getenv("SM64DS_COMPA_ZONES");
            on = (e && *e && *e != '0') ? 1 : 0;
            if (on) std::atexit(&CaZones::report);
        }
        if (on) t = std::chrono::steady_clock::now();
    }
    void mark(int z) {
        if (on <= 0) return;
        const auto now = std::chrono::steady_clock::now();
        acc[live][z] += std::chrono::duration<double, std::milli>(now - t).count();
        t = now;
    }
};
CaZones g_cz;
void CaZones::report() {
    static const char *nm[10] = {"h1_decide", "settle", "regs_to_default", "bg_loop_obj", "to_blit", "blit", "tail", "", "", ""};
    for (int l = 0; l < 2; ++l) {
        if (!g_cz.n[l]) continue;
        std::fprintf(stderr, "[compa] lift=%d frames %llu mean ms:", l, g_cz.n[l]);
        for (int z = 0; z < 7; ++z) std::fprintf(stderr, " %s %.3f", nm[z], g_cz.acc[l][z] / (double)g_cz.n[l]);
        std::fprintf(stderr, "\n");
    }
}
}

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
extern "C" int (*g_port_obj_a_skip)(int idx);   /* CLICKSTART1: ntr/ppu.cpp */
extern "C" {
/* THE IMPROVED MAP'S ATTENTION-ARROW RE-ANCHOR, asked once a frame. The banner
   is over the definition in hal/sub_screen.cpp, which owns both the option and
   the panel's rectangle; this file owns the sprites. Short-circuits on the
   option, so a run with the map option off never even asks about the layout.
   C linkage because that file's whole tail is one extern "C" block. */
int hal_minimap_arrow_reanchor_on(void);

/* H1 OF THE MENU COMPOSE (hal/sub_screen.cpp, hal/menu_compose.cpp): the frame's one predicate, asked at the head of
   the composite. Returns the lift plan, null on every frame no menu recipe lifts anything (every frame of an
   ordinary run), and then this file does exactly what it did before the layer existed. With a plan it fills the
   planes below: host-row bands (the level-clear lettering) and the DS-space top2d cells (recipes that rearrange
   the top screen's own layers). */
extern "C" const MenuLiftInfo *hal_menu_hook_h1(int w, int h);

/* Stage::RenderBouncingArrows' own sprite template, ov001 0x020abd88, mounted
   by name (port/ov001_syms.txt) and bound to the matched body's func_020abd88
   spelling in hal/sub_actors.cpp. The arrows are identified out of this table
   at run time rather than by a literal, the way the map's camera buttons are:
   a graphic that moved would take its tile number with it. */
extern const unsigned short _ZN3OAM14BOUNCING_ARROWE[];

/* Message::Update's four cursor templates (see msg_layer_px below), mounted by
   name through port/ov002_syms.txt: one OamAttr each, attr3 0xffff. */
extern unsigned char data_ov002_0210c390[];
extern unsigned char data_ov002_0210c398[];
extern unsigned char data_ov002_0210c3a0[];
extern unsigned char data_ov002_0210c3a8[];
}

/* MENUFOLD1: menu1's "a menu owns this tick's picture" switch (hal/menu_compose.cpp) is in the tree now, so TWODGPU1's weak default
   (twodgpu_menu_live_default and its /alternatename) is gone: a defined LHS makes the alias inert and the guard refuses it. */
extern "C" int hal_menu_live(void);

namespace {

/* WHICH HOST ROWS A DS ROW OCCUPIES. The 3D layer maps DS row y to host row
   y * active_h / 192 (ntr/gx.cpp's viewport), a true ratio. The 2D layers used
   the INTEGER active_h / 192, which is the same number at 4:3, 16:9 and every
   whole render scale, and 2 against 2.29 at 21:9 with no render scale
   (1024x440): the ending's letterbox bars are 3D rows, so the subtitle box,
   drawn at 2 rows per DS row, sat over the bottom of the picture instead of on
   the bar. ds_row_y0 is the first host row of DS row y and ds_row_of its
   inverse (host row -> DS row); at a whole multiple of 192 they are y * sy and
   hy / sy exactly. The height is present_h(): active_h, or the whole-multiple
   native rectangle of a scene presented natively, which the 3D viewport uses
   too. */
inline int ds_row_y0(int y) { return (y * ntr::present_h() + 191) / 192; }
inline int ds_row_of(int hy) { return hy * 192 / ntr::present_h(); }
/* THE SAME RATIO ACROSS: in a widescreen picture a DS pixel is square (the one
   uniform scale is active_h / 192), so DS column x starts at ds_col_x0(x) and
   ds_col_of(d) is the DS column under a host column d past the centring
   margin. At a whole multiple of 192 these are x * sy and d / sy exactly. */
inline int ds_col_x0(int x) { return (x * ntr::present_h() + 191) / 192; }
inline int ds_col_of(int d) { return d * 192 / ntr::present_h(); }

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
    /* MENUS093C (D23): a direct-colour BITMAP background (BG mode 3 / 4 / 5, BG3CNT bits 7 and 2): the held frame the title's
       drawing page hands over from. bmp_base is its first pixel, the matrix is the BG3 affine group (8 bit fractions). */
    bool bmp;
    uint32_t bmp_base;
    int bmp_w, bmp_h;
    int pa, pb, pc, pd;
    int rx, ry;
    bool wrap;
};

// Engine A's DISPCNT bit 3 is the BG0 2D/3D SELECTION, and it is not a setting
// that modifies a tile background -- it replaces one. With bit 3 set, BG0 is the
// 3D engine's output layer, BG0CNT's character and screen bases address nothing
// the game ever wrote, and the 2D unit never reads them.
//
// THE DECOMP'S OWN CODE PINS THE BIT. src/_ZN2GX15SetGraphicsModeEiii.cpp is
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

/* SM64DS_BGBITMAP_OFF=1: BG3 reads as a tile layer again (the 0.9.2 behaviour), for the A/B */
bool bitmap_bg_off() {
    static const bool v = std::getenv("SM64DS_BGBITMAP_OFF") != nullptr;
    return v;
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
    if (bg == 3 && (dispcnt & 7u) >= 3u && (dispcnt & 7u) <= 5u && (cnt & 0x84u) == 0x84u && !bitmap_bg_off()) {
        static const int wh[4][2] = { { 128, 128 }, { 256, 256 }, { 512, 256 }, { 512, 512 } };
        c.bmp = true;
        c.bmp_w = wh[(cnt >> 14) & 3][0];
        c.bmp_h = wh[(cnt >> 14) & 3][1];
        c.bmp_base = kVramBase + scr_off + (((uint32_t)(cnt >> 8) & 0x1Fu) << 14);
        c.pa = (int16_t)rd16(kRegBase + 0x30); c.pb = (int16_t)rd16(kRegBase + 0x32);
        c.pc = (int16_t)rd16(kRegBase + 0x34); c.pd = (int16_t)rd16(kRegBase + 0x36);
        c.rx = ((int32_t)((uint32_t)rd16(kRegBase + 0x38) | ((uint32_t)rd16(kRegBase + 0x3A) << 16)) << 4) >> 4;
        c.ry = ((int32_t)((uint32_t)rd16(kRegBase + 0x3C) | ((uint32_t)rd16(kRegBase + 0x3E) << 16)) << 4) >> 4;
        c.wrap = (cnt >> 13) & 1;
    }
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
    if (c.bmp) {
        int tx = (int)((c.rx + (long long)c.pa * x + (long long)c.pb * y) >> 8);
        int ty = (int)((c.ry + (long long)c.pc * x + (long long)c.pd * y) >> 8);
        if (c.wrap) { tx &= c.bmp_w - 1; ty &= c.bmp_h - 1; }
        else if (tx < 0 || ty < 0 || tx >= c.bmp_w || ty >= c.bmp_h) return false;
        const uint16_t v = rd16(c.bmp_base + ((uint32_t)ty * c.bmp_w + tx) * 2);
        if (!(v & 0x8000u)) return false;
        out = bgr555(v);
        return true;
    }
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

/* TITLEPERF1: the per-tick getenv calls of the composite, read once. */
inline bool msg_composite_debug() { static const bool v = std::getenv("SM64DS_MSG_COMPOSITE_DEBUG") != nullptr; return v; }

/* TITLEPERF1: SM64DS_COMPA_ROWFAST=0 puts the per-pixel sample_bg loop back on the same binary (the A/B for the row sampler below) */
inline bool row_fast_off() {
    static int off = -1;
    if (off < 0) {
#ifndef PORT_ROM_CLEAN
        const char *e = std::getenv("SM64DS_COMPA_ROWFAST");
        off = (e && e[0] == '0') ? 1 : 0;
#else
        off = 0;
#endif
    }
    return off != 0;
}

/* TITLEPERF1: sample_bg for the 256 pixels of one row, a tiled (non-bitmap) background only. The screen entry, the flips and the tile's
   address are read once per 8 pixels instead of once per pixel; every value is the one sample_bg would have returned (the same volatile
   reads at the same addresses, in the same arithmetic), so a row filled here equals 256 calls of sample_bg. hit[x] 0 = transparent. */
void sample_bg_row(const BgConfig &c, int y, uint32_t *col, uint8_t *hit) {
    const int py = (y + c.vofs) & (c.tiles_h * 8 - 1);
    const int wmask = c.tiles_w * 8 - 1;
    const int ty = py >> 3, fy0 = py & 7;
    int last_tx = -1;
    int fxflip = 0;
    uint32_t vrow = 0;      /* the tile's row of indices (8 bpp: 8 bytes, 4 bpp: 4 bytes) */
    uint32_t palbase = 0;
    for (int x = 0; x < 256; ++x) {
        const int px = (x + c.hofs) & wmask;
        const int tx = px >> 3;
        if (tx != last_tx) {
            last_tx = tx;
            const uint16_t se = rd16(screen_entry_addr(c, tx, ty));
            const int tile = se & 0x3FF;
            int fy = fy0;
            if (se & 0x800) fy = 7 - fy;
            fxflip = (se & 0x400) ? 1 : 0;
            vrow = c.bpp8 ? c.chars + tile * 64 + fy * 8 : c.chars + tile * 32 + fy * 4;
            palbase = c.bpp8 ? 0u : (uint32_t)((se >> 12) & 0xF) * 16u;
        }
        int fx = px & 7;
        if (fxflip) fx = 7 - fx;
        uint32_t index;
        if (c.bpp8) {
            index = *reinterpret_cast<volatile uint8_t *>(vrow + fx);
            if (index == 0) { hit[x] = 0; continue; }
        } else {
            const uint8_t pair = *reinterpret_cast<volatile uint8_t *>(vrow + (fx >> 1));
            index = (fx & 1) ? (pair >> 4) : (pair & 0xF);
            if (index == 0) { hit[x] = 0; continue; }
            index += palbase;
        }
        col[x] = bgr555(rd16(kPlttBase + index * 2));
        hit[x] = 1;
    }
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
    /* THE LINE-BY-LINE HORIZONTAL EDGES, on a frame whose mask-2 handler
       rewrote WIN0H / WIN1H per scanline (ntr::rt_window_rows): each line's
       X1, X2 and the in-window flag it inherits from the line above. */
    bool rows_on;
    uint8_t rx1[2][192], rx2[2][192], rs[2][192];
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
    /* A dWipe_c-style wipe programs the horizontal edges per scanline from
       its HBlank handler (func_0202f2c4); read above, the rectangle is only
       the last line's. Take every line's instead, and walk each the way the
       DS 2D unit does (melonDS GPU2D::CalculateWindowMask, the reference
       emulator this project's cartridge traces run on): along the line the
       in-window flag is CLEARED at X2 and otherwise SET at X1, and it carries
       into the next line. So X1 == X2 is an empty line, and X1 > X2 runs from
       X1 to the right edge and on into the next line up to its X2. The flag
       starts each frame clear. A frame with no handler keeps the rectangle
       above unchanged. */
    w.rows_on = false;
    const uint32_t *rows = nullptr;
    if (ntr::rt_window_rows(0, &rows)) {
        w.rows_on = true;
        for (int i = 0; i < 2; ++i) {
            uint8_t carry = 0;
            for (int y = 0; y < 192; ++y) {
                const uint16_t hh = (uint16_t)(i == 0 ? rows[y] : rows[y] >> 16);
                w.rx1[i][y] = (uint8_t)(hh >> 8);
                w.rx2[i][y] = (uint8_t)(hh & 0xFF);
                w.rs[i][y] = carry;
                carry = w.rx1[i][y] > w.rx2[i][y] ? 1 : 0;
            }
        }
        /* MGEXIT1: a one-screen recipe moves pieces of this screen, so a wipe cut into it would move with them (a star per piece); the
           finished picture is cut once instead (ntr::wipe_cut_picture, from hal/menu_compose.cpp) */
        if (ntr::wipe_suppressed()) { w.rows_on = false; w.any = false; }
    }
}

/* Is column x inside window i's horizontal extent on line y? */
inline bool win_h_inside(const Windows &w, int i, int x, int y) {
    if (!w.rows_on) return x >= w.x1[i] && x < w.x2[i];
    const int x1 = w.rx1[i][y], x2 = w.rx2[i][y];
    if (x1 < x2) return x < x1 ? w.rs[i][y] != 0 : x < x2;
    if (x1 > x2) return x < x2 ? w.rs[i][y] != 0 : x >= x1;
    return x < x2 ? w.rs[i][y] != 0 : false;
}

/* Hardware precedence is fixed: window 0 beats window 1, window 1 beats the OBJ
   window, and WINOUT covers what is in none of them. Same order as
   ntr/ppu_sub.cpp's. */
inline unsigned window_mask(const Windows &w, int x, int y) {
    if (!w.any) return 0x3F;
    for (int i = 0; i < 2; ++i)
        if (w.on[i] && win_h_inside(w, i, x, y) && y >= w.y1[i] && y < w.y2[i])
            return w.in[i];
    if (w.obj_on && g_objwin[y][x]) return w.obj_in;
    return w.out;
}

/* ---- THE 3D LAYER INSIDE THE WINDOW UNIT (run hunt5, lane WIPES1) -------------
 *
 * The minigames' star / circle wipes (dWipe_c, src/func_0202ee94.c) are not a
 * fade. The game programs engine A and engine B with the SAME two windows,
 * rewrites WIN0H / WIN1H on every scanline from its HBlank handler
 * (func_0202f2c4 -> ntr::rt_window_rows), and sets WINOUT to 0: outside the
 * shape no layer is drawn, so the screen shows the BACKDROP, palette entry 0,
 * which is white in every minigame (0x05000000 and 0x05000400 both read 7fff on
 * scene 390). Inside the shape every layer shows.
 *
 * BG0 IS ONE OF THOSE LAYERS WHEN DISPCNT BIT 3 MAKES IT THE 3D ENGINE'S
 * OUTPUT (bg0_is_3d above), and WINOUT bit 0 gates it like any other BG. The
 * port rendered the 3D frame into the framebuffer before this unit ran and then
 * masked only the 2D layers by window, so on engine A the 3D scene (and the
 * frame's clear fill, gx_clear_argb's navy where the clear alpha is 0) stayed
 * visible OUTSIDE the shape while engine B, which has no 3D, showed the white
 * backdrop. The cartridge shows the same colour outside the shape on both
 * screens.
 *
 * WHAT THIS DOES. For every host pixel whose DS cell the window unit gives a
 * mask with bit 0 clear it (1) removes the pixel from the 3D coverage mask,
 * which is exactly what a 3D layer that is not there looks like to the loops
 * below (a 2D layer behind the 3D layer then paints there, the way the BG loop
 * already does where 3D drew nothing), and (2) writes the backdrop into the
 * framebuffer, which is what shows where no 2D layer covers. The hit cells the
 * BG / OBJ loops produce afterwards overwrite it exactly as they overwrite the
 * 3D frame. Runs only on a frame whose window unit is armed AND whose BG0 is
 * the shown 3D layer; a window that lets BG0 through (every dialogue box) finds
 * no pixel to hide and costs one table scan.
 *
 * SM64DS_WIPE3D_OFF=1 puts the old picture back on this binary. */
inline bool hide3d_off_env() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_WIPE3D_OFF");
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v != 0;
}

inline bool winobj_off_env() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_WINOBJ_OFF");
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v != 0;
}

void hide_3d_outside_windows(uint32_t dispcnt, const Windows &w, ntr::Framebuffer &fb)
{
    if (!w.any || hide3d_off_env() || !bg0_3d_shown(dispcnt)) return;
    /* the DS columns / rows whose mask clears BG0, one verdict per DS pixel */
    static uint8_t hid[192][256];
    bool any_hidden = false;
    for (int y = 0; y < 192; ++y)
        for (int x = 0; x < 256; ++x) {
            const bool h = (window_mask(w, x, y) & 1u) == 0;
            hid[y][x] = h ? 1 : 0;
            any_hidden |= h;
        }
    const bool out_hidden = (w.out & 1u) == 0;
    if (!any_hidden && !out_hidden) return;
    uint8_t *const cover = const_cast<uint8_t *>(ntr::gx_coverage());
    uint32_t *const pre = ntr::gx_aa_preimage();
    const uint32_t bd = bgr555(rd16(kPlttBase));
    const int sx = ntr::active_w / 256;
    const int margin = ntr::active_w - ds_col_x0(256);
    for (int hy = 0; hy < ntr::active_h; ++hy) {
        const int y = ds_row_of(hy);
        for (int hx = 0; hx < ntr::active_w; ++hx) {
            int x;
            if (ntr::widescreen) {
                const int d = hx - margin / 2;
                x = d < 0 ? -1 : ds_col_of(d);
            } else {
                x = hx / sx;
            }
            const bool h = (x >= 0 && x < 256 && y < 192) ? hid[y][x] != 0 : out_hidden;
            if (!h) continue;
            fb.px[hy][hx] = bd;
            if (pre) pre[(size_t)hy * ntr::SCREEN_W + hx] = bd;
            if (cover) cover[(size_t)hy * ntr::SCREEN_W + hx] = 0;
        }
    }
}

/* ENDMENU1 (run hunt8, Tango 10-20: the THE END screen is pure black on the cartridge). DISPCNT bit 3 makes BG0 the 3D engine's
 * output and bit 8 is BG0's own enable. The ending fades the castle out, then clears bit 8 and fades the sprites back in: from that
 * frame the DS shows the backdrop (palette entry 0) behind the OBJ text, whatever the 3D engine still has in its buffer. The port
 * draws the 3D frame into the framebuffer without looking at bit 8 and this unit writes only the 2D pixels it owns, so the castle
 * stayed under the text. Same rule as hide_3d_outside_windows, for the whole screen: a 3D layer that is switched off is not in the
 * picture, so the present rectangle takes the backdrop, its coverage and the pre-smoothing copy go with it, and a frame held on the
 * graphics card is read back first so there is a picture to wipe. SM64DS_BG0OFF_KEEP=1 puts the old picture back on this binary. */
inline bool bg0off_keep_env() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_BG0OFF_KEEP");
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v != 0;
}

void hide_3d_layer_off(uint32_t dispcnt, ntr::Framebuffer &fb)
{
    if (!bg0_is_3d(dispcnt) || ((dispcnt >> 8) & 1u) != 0 || bg0off_keep_env()) return;
    if (ntr::gx_card_overlay_active()) ntr::gx_card_settle_because("the 3D layer is switched off (BG0 enable clear)");
    uint8_t *const cover = const_cast<uint8_t *>(ntr::gx_coverage());
    uint32_t *const pre = ntr::gx_aa_preimage();
    const uint32_t bd = bgr555(rd16(kPlttBase));
    const int x0 = ntr::present_x(), y0 = ntr::present_y();
    const int x1 = x0 + ntr::present_w(), y1 = y0 + ntr::present_h();
    for (int hy = y0 < 0 ? 0 : y0; hy < y1 && hy < ntr::active_h; ++hy)
        for (int hx = x0 < 0 ? 0 : x0; hx < x1 && hx < ntr::active_w; ++hx) {
            fb.px[hy][hx] = bd;
            if (pre) pre[(size_t)hy * ntr::SCREEN_W + hx] = bd;
            if (cover) cover[(size_t)hy * ntr::SCREEN_W + hx] = 0;
        }
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
struct Cell { uint32_t color; bool hit; uint8_t owner; uint8_t prio;
              uint8_t msgcur; /* OBJ texels only: see msg_layer_px */ };
/* msgcur rides in what was the struct's tail padding, so the buffer did not
   grow for it. */
static_assert(sizeof(Cell) == 8, "Cell grew");
Cell g_a[192][256];

// Owner ids: 0-3 are BG0..BG3 and 4 is the sprite layer. The same numbering is
// the bit position in the layer switch below, so a mask and an attribution row
// read against each other without a translation table.
enum { kOwnerObj = 4, kOwnerN = 5 };
const char *const kOwnerName[kOwnerN] = {"BG0", "BG1", "BG2", "BG3", "OBJ"};

/* THE MESSAGE BOX'S OWN SPRITES, marked per pixel. Message::Update draws two
   cursors with OAM::Render (src/_ZN7Message6UpdateEv.cpp; the twin is
   src/func_020326ac.c): the choice pointer beside the answers of a Yes / No
   question (templates ov002 0x0210c3a0 / 0x0210c3a8), at the box origin
   data_0209d650 + 0xc, and the advance arrow (ov002 0x0210c390 / 0x0210c398)
   at the box's lower right. Both are positioned in the BOX's coordinates, so
   on the cartridge they sit inside the box by construction. The box is BG3,
   which the widescreen blit centres at the native scale; these texels are OBJ,
   which it anchored to the screen edges like the corner HUD, and that pulled
   the pointer out to the left of the box and the arrow out to the right (the
   0.5.2 forum report). Cell::msgcur is set on an OBJ texel one of those
   four templates put there, and the blit gives that texel BG3's placement.
   raster_obj writes the byte with every OBJ texel it stores, 1 or 0, so a
   later sprite that covers a cursor texel clears it; it is read only where
   the owner is OBJ, so the value a background pixel carries over from an
   older frame is never looked at. At 4:3 nothing reads it. */

/* The message layer, as the widescreen placement sees it: BG3's own texels,
   and the box's cursor sprites that the game positions inside BG3's box. */
inline bool msg_layer_px(int y, int x)
{
    return g_a[y][x].owner == 3
           || (g_a[y][x].owner == kOwnerObj && g_a[y][x].msgcur);
}

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

/* ---- THE LEVEL-CLEAR GLYPH OVERLAY ----------------------------------------
 *
 * WHAT IT IS FOR. hal/sub_screen.cpp composes the level-clear save menu onto
 * the top screen: the course-clear text at the top of the picture, the three
 * button plates in the middle, the coin total at the bottom. Its first two
 * cuts moved RECTANGLES OF THE FINISHED PICTURE -- the glyphs and the 3D scene
 * behind them together -- so the picture's own top rows were thrown away and
 * its middle rows appeared twice. The owner read that as a stretch: "why is
 * the background picture like stretched? The screen height shouldnt change,
 * just slot it in."
 *
 * THE ANSWER IS TO SEPARATE THE 2D LAYER FROM THE 3D PICTURE, which this file
 * is the only place that can do: it is the unit that resolves engine A's four
 * backgrounds and its sprites into one image and blits them over the 3D frame
 * the GX path already rendered. While the compose is up it sends the pixels of
 * the layers that carry the menu's own lettering into a separate buffer INSTEAD
 * of the framebuffer, keeping their colour and their host position and marking
 * which ones are opaque. What is left in the framebuffer is the game's own
 * frame with those layers lifted off it -- no row moved, no region copied --
 * and the compose then draws the lifted pixels where the owner asked for them.
 *
 * WHICH LAYERS, AND IT IS MEASURED AND NOT ASSUMED. Each of engine A's five
 * composited layers was masked off in turn with SM64DS_ENGINE_A_LAYERS on the
 * SAME frame of the same row (level 6, a star, the menu up, frame 900, the
 * plain top screen at 512x384), counting the pixels each one puts on the
 * picture that the no-2D picture does not have:
 *
 *   BG0     0 px  -- DISPCNT bit 3 is set, so BG0 IS the 3D layer and read_bg
 *                    never treats it as a background at all
 *   BG1     0 px
 *   BG2  8556 px  picture rows 336..367, x 54..457  -- "TOUCH TO SELECT"
 *   BG3  5971 px  picture rows  72..221, x 96..393  -- THE COURSE-CLEAR TEXT,
 *                    every one of the 5971 the single colour (144,144,144), in
 *                    the four line bands 72..93, 104..125, 166..189, 200..221
 *   OBJ  3646 px  two bands: 2..35 x 12..95 the LIVES COUNTER, and
 *                    256..287 x 196..271 THE COIN TOTAL
 *   all 18173 px  = 8556 + 5971 + 3646 exactly, so the three sets are disjoint
 *
 * SO THERE ARE THREE LAYERS AND EACH GETS ITS OWN RULE:
 *
 *   BG3, on the text band's rows -> LIFTED. It is the whole course-clear text
 *     block and its footprint (72..221) lies inside the band the compose lifts
 *     (68..231), so the lift loses none of it.
 *   OBJ, on the coin band's rows -> LIFTED. The coin total, 256..287, inside
 *     that band (244..287).
 *   BG2 -> HIDDEN, drawn nowhere. It is "TOUCH TO SELECT", which the owner
 *     asked to be removed and which the compose has never drawn.
 *   OBJ anywhere else -> LEFT EXACTLY WHERE IT IS. That is the lives counter
 *     in the top-left corner. It is part of the top screen's own picture, it
 *     is not re-slotted anywhere, and "the screen height shouldnt change" is a
 *     picture whose own top is still there. It cannot collide with the text
 *     either: the lifted text lands at x 96..393 and the counter is x 12..95.
 *
 * A COLOUR KEY WAS NOT AN OPTION and was never tried: the 3D picture behind
 * the text can hold any colour, including the glyphs' own grey, so keying on
 * colour would punch holes in the scenery. The layer is the only honest key,
 * and this unit already knows it per pixel (Cell::owner).
 *
 * THE COST WHEN THE COMPOSE IS OFF IS ONE INTEGER TEST PER FRAME.
 * hal_lc_compose_rows answers 0, lc_ov.lift is 0, the buffer is never
 * allocated, and every per-pixel test below short-circuits on a zero that was
 * read once for the whole frame. */
struct LcOverlay {
    uint32_t *col;               /* the lifted SRC_TOP_HOSTROW pixels, framebuffer-shaped ARGB, alpha 0 = nothing */
    int n;                       /* allocated pixels */
    MenuTopCell *top;            /* the lifted SRC_TOP_2D cells, DS space 256x192 */
    const MenuLiftInfo *info;    /* this frame's lift plan, null = lift nothing */
    int live;                    /* col holds THIS frame's pixels */
    int dirty_lo, dirty_hi;      /* host rows [lo, hi) that may be non-zero: col is zero everywhere else */
    int top_live;                /* top holds THIS frame's cells */
};
LcOverlay lc_ov;

/* SAVEOVL1: THE LIFT ON THE CARD. When the level-clear compose lifts glyphs out of the picture, it used to read the 3D
   back from the card first (a settle: 9 to 13 ms a tick) because the menu recipe then drew onto the real picture. With
   the menu layer (gpu_raster.h port_gpu_layer_*) the recipe draws into a layer of its own, so a lift is only a matter
   of WHICH PLANE a cell lands in, and the picture can stay on the card. g_lift_card says this tick's lift runs that
   way. SM64DS_MENU_LAYER=0 puts the old settle back. */
bool g_lift_card;

/* Asked once a frame, at the head of the composite (H1). Everything the lift does is behind the answer. */
void lc_overlay_begin(void)
{
    lc_ov.live = 0;
    lc_ov.top_live = 0;
    lc_ov.info = hal_menu_hook_h1(ntr::active_w, ntr::active_h);
    const MenuLiftInfo *li = lc_ov.info;
    if (!li) return;
    if (li->nbands) {
        const int n = ntr::SCREEN_W * ntr::SCREEN_H;
        if (lc_ov.n < n) {
            uint32_t *c = (uint32_t *)std::realloc(lc_ov.col, (size_t)n * sizeof *c);
            if (!c) { lc_ov.info = nullptr; return; }
            lc_ov.col = c;
            lc_ov.n = n;
            std::memset(lc_ov.col, 0, (size_t)lc_ov.n * sizeof *lc_ov.col);
            lc_ov.dirty_lo = lc_ov.dirty_hi = 0;
        }
        /* only the rows the last lifted frame could have written are cleared (a lift writes inside its bands only),
           so a menu frame no longer pays for a whole-buffer memset */
        if (lc_ov.dirty_hi > lc_ov.dirty_lo)
            std::memset(lc_ov.col + (size_t)lc_ov.dirty_lo * ntr::SCREEN_W, 0,
                        (size_t)(lc_ov.dirty_hi - lc_ov.dirty_lo) * ntr::SCREEN_W * sizeof *lc_ov.col);
        {
            int lo = ntr::SCREEN_H, hi = 0;
            for (int i = 0; i < li->nbands; ++i) {
                if (li->band[i].r0 < lo) lo = li->band[i].r0;
                if (li->band[i].r1 > hi) hi = li->band[i].r1;
            }
            if (lo < 0) lo = 0;
            if (hi > ntr::SCREEN_H) hi = ntr::SCREEN_H;
            if (hi < lo) hi = lo;
            lc_ov.dirty_lo = lo;
            lc_ov.dirty_hi = hi;
        }
        lc_ov.live = 1;
    }
    if (li->lift2d) {
        if (!lc_ov.top) lc_ov.top = (MenuTopCell *)std::malloc(256 * 192 * sizeof(MenuTopCell));
        if (lc_ov.top) {
            std::memset(lc_ov.top, 0, 256 * 192 * sizeof(MenuTopCell));
            lc_ov.top_live = 1;
        }
    }
}

/* What to do with a composited pixel's LAYER: 0 draw it, 1 lift it on its host-row band (when its row is inside the
   band), 2 hide it, 3 lift it into the DS-space top2d plane. Zero for everything while no recipe lifts, on one
   test of a pointer that was written once for the frame. */
inline int lc_overlay_verdict(unsigned owner)
{
    const MenuLiftInfo *li = lc_ov.info;
    if (!li) return 0;
    const unsigned b = 1u << owner;
    if (b & li->hide) return 2;
    if ((b & li->lift2d) && lc_ov.top_live) return 3;
    if (!lc_ov.live) return 0;
    for (int i = 0; i < li->nbands; ++i)
        if (li->band[i].layers & b) return 1;
    return 0;
}

/* AND THE HOST ROWS THAT LAYER IS LIFTED ON: the band is the layout's own rectangle, so the lift and the redraw cover
   EXACTLY the same pixels and a glyph can never be taken out of the picture and then fall outside the band that puts
   it back. A pixel of a lifted layer outside its band is left in the picture where the game put it (the lives
   counter, a sprite far above the coin band). */
inline bool lc_overlay_in_band(unsigned owner, int hy)
{
    const MenuLiftInfo *li = lc_ov.info;
    const unsigned b = 1u << owner;
    for (int i = 0; i < li->nbands; ++i)
        if ((li->band[i].layers & b) && hy >= li->band[i].r0 && hy < li->band[i].r1) return true;
    return false;
}

// ---- PER-ELEMENT HUD ANCHORING (widescreen) --------------------------------
//
// WHAT THIS REPLACES, AND WHY. The widescreen HUD used to be placed by a BAND
// SPLIT on source x: a DS column below 96 rode the left screen edge, one at 160
// or above rode the right edge, and the middle stayed centred. That is right
// for the sparse corner HUD and it is what makes a widescreen HUD read as one --
// the lives cluster out at the left edge, the coin and star counters out at the
// right. It has one failure, and status/WIDE169.md section 6 named it as OWED
// rather than fixed: an element that STRADDLES a band boundary is cut in half,
// with the whole spare width opened through the middle of it. The pixels below
// the split go one way and the pixels above it go the other, so the element is
// not merely misplaced, it is TORN, with a margin/2 void through it (128 px at
// 16:9, 288 px at 21:9 -- wider than the element itself).
//
// The elements that straddle are not exotic. The red-coin pip row crosses 96.
// The TIME label crosses 160. The pause banner's number sits ON 160. The VS
// "TIME UP" banner and a level-clear banner are centred rects wide enough to
// cross both. Every one of those is ordinary play.
//
// WHY IT COULD NOT BE FIXED AT THIS SEAM BEFORE. WIDE169 recorded the reason:
// by the time this code runs the compositor holds a flat 256x192 buffer of DS
// pixels and "has already lost per-element identity. There is no 'this pixel
// belongs to the coin counter' available at this point." That is true of the
// OWNER byte -- a whole layer is not an element -- but it is not true of the
// PIXELS. An on-screen element is a spatially COHERENT cluster of set pixels
// with empty space around it, and coherence is recoverable from the buffer
// itself by labelling it. So identity is not lost; it has to be recovered
// geometrically rather than read off a byte.
//
// WHAT THIS DOES. Once per frame, label the composited hit mask into connected
// ELEMENTS, then give each whole element ONE anchor, chosen from that element's
// own horizontal centre against the same two splits the band rule used. Every
// pixel of an element therefore moves by the same offset. An element cannot be
// torn by construction, because tearing requires two different offsets inside
// one element and an element now has exactly one.
//
// WHY IT IS A NO-OP ON EVERYTHING ALREADY MEASURED. For an element that lies
// wholly inside one band, its centre is in that band too, so it gets the band's
// offset -- the same offset every one of its pixels already had. WIDE169
// measured the three clusters in play: lives at source x 6-47 (centre 26, left),
// the health meter at 108-147 (centre 127, centre band) and the coin and star
// counters at 207-247 (centre 227, right). All three land byte-identically.
// The change is visible ONLY on an element that used to be torn, which is what
// makes the before/after a clean control rather than a whole-HUD reshuffle.
//
// GAP BRIDGING, AND WHY IT IS NOT OPTIONAL. Strict pixel connectivity is the
// wrong grain for this. A line of text is not one connected blob -- the letters
// do not touch -- so labelling it strictly would make every GLYPH its own
// element, each free to take a different anchor, which is the tear again at a
// finer grain. So the mask is DILATED before labelling: kBridgeX columns and
// kBridgeY rows. Two marks closer than the bridge are one element.
//
// The bridge is sized off the two facts that matter. It must be LARGER than the
// gaps inside one element: the DS font is on an 8 px tile grid and glyphs sit
// within a few pixels of each other, so 16 (two tiles) closes a text run and a
// digit pair. It must be SMALLER than the gaps between elements that are meant
// to anchor apart: the lives cluster ends at 47 and the meter starts at 108 (61
// clear), the meter ends at 147 and the counters start at 207 (59 clear). 16
// sits with a wide margin on both sides of that window.
//
// AND THE ERROR DIRECTION IS DELIBERATE. If the bridge over-merges, two
// elements share one anchor and both stay whole -- they sit where the merged
// centre puts them, which is a placement judgement. If it under-merges, one
// element splits and TEARS, which is a defect. Over-merging is a look and
// under-merging is a bug, so the bridge is sized to err large.
//
// COST. Four linear passes over the 192x256 mask (two dilation, two labelling)
// plus a sparse extent walk. It runs ONLY when the aspect is wide; at the
// native aspect the whole block is skipped and the placement is the byte-for-
// byte original x*sx, so the default build pays nothing at all.
/* THE N64 HUD ROW (HudLayout = 1; run menu1 lane HUD1). The DS stacks the coin counter under the star counter at the top
   right (digits y 20..36 against the star row's y 2..18). The N64 puts lives, coins and stars on ONE row, so this
   moves the coin counter, as one block (icon, x and digits, never a split glyph), up onto the star row and left into
   the middle of the screen: DS pixel (x, y) of the counter (x >= 188, y 20..36) goes to (x - dx, y - kHudCoinDy), dx = 52 normally, wider when a three digit star count needs the room. It is done in the
   DS 256x192 plane BEFORE the widescreen element pass, so the position is a canvas position in every aspect (4:3 plain
   x*sx, wide through the anchors below). Wide, the moved block is held out of the element labelling (it would bridge
   into the star counter) and takes the CENTRE anchor, the same one the power meter has, so the row spreads out
   (lives left edge, coins centre, stars right edge) instead of stretching. The red coin row and the silver star row
   already sit bottom left on the DS top screen (y 157..173, RenderRedCoins / RenderSilverStars), they stay.
   Absent or 0 = nothing here runs: one int read per frame. */
extern "C" int host_setting_hud_layout(void);
extern "C" int hal_menu_layer_ok(void);
static const int kHudZoneX0 = 188, kHudZoneY0 = 20, kHudZoneY1 = 36;   /* the coin counter's DS rectangle */
static const int kHudCoinDxMin = 24, kHudCoinDy = 18, kHudMeterGap = 7, kHudMeterGapMin = 2, kHudEdgeGap = 1, kHudStarGap = 11, kHudStarGapMin = 3;
/* HUDTWEAK1 (player ask, "N64 Accurate HUD Positioning"): the coin block sits 5 DS pixels further from the star counter (kHudStarGap
   6 -> 11, the squeeze floor stays 3), the power meter moves the same 5 pixels left so the two keep their gap, and the race timer,
   which the port draws one coin row (kHudCoinDy) under the coin count, moves up into the row the coin counter left. */
static const int kHudMeterLeft = 5;
static const int kHudMidX = 150;      /* HUDMENU2 (H10): the coin block's left edge in the plane when it sits on the centre anchor (the meter's art ends at DS x 142) */
static bool g_hud_mid;                 /* this frame's coin block is on the centre anchor */
extern "C" unsigned char data_ov002_02111178;
extern "C" int g_port_hud_dlx, g_port_hud_dsx;
static int g_slide_n, g_slide_ds[2][4];
static int g_slide_d[2];
struct HudSlideRect { int x0, y0, x1, y1; float d; };
struct HudSlideNote { int n; HudSlideRect r[2]; };
static HudSlideNote g_note;
/* HUDMENU2 (H11): the counters sliding in, as host rectangles and the DS pixels each moved this tick; read by walk_window.cpp's ip_build */
extern "C" const void *port_hud_slide_note(void) { return &g_note; }
/* the meter's DS rectangle (x 80..153), clear of the lives and the star counter. Its rows run to y 95, not to the 59 it rests within: the
   appear animation draws the meter at y 0x39 (src/_ZN3HUD17UpdateHealthMeterEv.cpp: mHealthMeterY = 0x39, held 0x5a ticks, then 4 a tick up to
   0x19), its lower half reaching about y 90, and a box that ends at 59 moved only the top half left (the meter showed sliced and offset). */
static const int kHudMeterX0 = 80, kHudMeterX1 = 153, kHudMeterY1 = 96;
static const int kHudTimerX0 = 154, kHudTimerX1 = 256, kHudTimerY0 = 38, kHudTimerY1 = 70;   /* the port's timer rows, x 0xa4..0xf8 and the glyph overhang, drawn at y 0x16 + 18 .. 0x1e + 18 + 16 */
extern "C" unsigned char data_ov002_02111184;   /* the ROM's "a timer runs on this course" byte */
/* THE HD POWER METER (run hunt8, lane HDMETER1). When an enabled mod carries hud_hd/power_meter art (hal/hd_meter.h) the meter is drawn
   from it instead of the DS sprites: same footprint (the sprite's 40 x 40 DS pixels at x kHdMeterX0, y mHealthMeterY + kHdMeterDy), same
   wedge count (the ROM's displayed count), same slide and blink (the ROM still runs HUD::RenderHealthMeter; its cells are what tell us the
   meter is on screen), same layer priority and brightness. sub_actors.cpp notes the ROM's y and wedge count each frame it draws the
   meter; the plane step below erases the DS cells of the footprint and the pass after the cell loops paints the art at host resolution.
   No mod, a smoke target, a non-English label, a level-clear lift or a natively presented scene: nothing here runs and the DS cells stay. */
extern "C" int g_port_hdmeter_live, g_port_hdmeter_y, g_port_hdmeter_health, g_port_meter_drawn;   /* hal/sub_actors.cpp */
extern "C" const HdMeterArt *(*port_hd_meter_art_hook)(int health, int k) = nullptr;   /* set by hal/hd_meter.cpp */
static int kHdMeterX0 = 108, kHdMeterDy = -24;   /* the DS sprite's top left (its 40 x 40 cells) against (0x80 - 20, mHealthMeterY): measured: cells x 108..147, rows y0 .. y0 + 39 with y0 = mHealthMeterY - 24 */
struct HdMeterJob { bool on; int x0, y0, off, k; const HdMeterArt *art; };
static HdMeterJob g_hdm;
static uint8_t g_hud_row[192][256];   /* 1 = a moved coin counter pixel (read only while g_hud_row_live) */
static Cell g_hud_cells[192][256];    /* the moved coin counter's cells, a plane of their own so they never replace a cell of g_a (the POWER label) */
static bool g_hud_row_live;
static bool g_hud_timer_live;   /* HUDTWEAK1: this frame's timer cells move up (the N64 row is on and a course timer runs) */
static bool g_hud_erase;   /* PROOF ARM (env SM64DS_HUD_ERASE_PROOF, off by default): the pre-fix behaviour, kept only to photograph the cause */

/* HITCH9C: SM64DS_MSG_FAST=0 puts the per-pixel originals (this file's bm_fill_default_ref and the uncached resolve) back on this same binary (the control half of the
   before / after); unset or anything else, the table form is used. Both leave the same bytes in g_bm. */
inline bool msg_fast() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_MSG_FAST");
        v = (e && *e == '0') ? 0 : 1;
    }
    return v != 0;
}

namespace hudelem {

// The dilation radii, in DS pixels. See the bridge discussion above.
const int kBridgeX = 16, kBridgeY = 8;

// The anchor each label resolved to, as the host-pixel offset added to x*uni.
// Label 0 is "no element" and is never read for a hit pixel; it holds the
// centred offset so that a stray read is placed rather than thrown.
const int kMaxLabels = 4096;
int g_off[kMaxLabels];
int16_t g_lbl[192][256];

// Union-find over provisional labels, flattened by path halving. The label
// count is bounded by kMaxLabels; a mask fragmented past that (which no HUD
// produces -- it would need thousands of separate marks) stops allocating new
// labels and folds the excess into the last one, which keeps every pixel
// placed and whole rather than unlabelled.
int g_par[kMaxLabels];
int g_nlab;

int find_root(int a) {
    while (g_par[a] != a) { g_par[a] = g_par[g_par[a]]; a = g_par[a]; }
    return a;
}
void unite(int a, int b) {
    a = find_root(a); b = find_root(b);
    if (a != b) g_par[a > b ? a : b] = (a < b) ? a : b;
}

// Label the frame's elements and resolve each one's anchor offset.
//
// BG3 is excluded from the mask because the message layer has its own placement
// rule that predates this one and is not a band decision at all: dialogue is
// centred at the native scale, whatever else is on screen. Leaving its glyphs
// in the mask would also bridge a box of text into whatever HUD sits beside it
// and drag both to the box's centre. The box's own cursor sprites (msgcur)
// are held out with it, for the same two reasons: they take the box's placement,
// and they must not bridge into a HUD element beside the box.
//
// band_l / band_r are the same two splits the band rule used, so an element
// wholly inside a band keeps exactly that band's offset.
void resolve(int band_l, int band_r, int margin)
{
    // Pass 1: horizontal dilation. A running count of "columns since the last
    // set pixel" turns a left-to-right sweep into a one-sided dilation, and the
    // right-to-left sweep completes it, so the whole radius costs two adds per
    // pixel rather than a kBridgeX-wide window scan at each one.
    static uint8_t dil[192][256];
    /* HITCH9C: THE ELEMENT PASS IS A FUNCTION OF THE "ON" MASK AND THE THREE NUMBERS, NOTHING ELSE, and the mask is
       the same picture after picture for as long as a HUD or a message box stands still (the passes below were the
       biggest single cost of a dialogue frame: 3-4 ms of labelling an unchanged mask sixty times a second). The mask is
       built once here; if it and band_l / band_r / margin match the last call's, g_lbl and g_off are still what that
       call left (nothing else writes them) and the passes are skipped. SM64DS_MSG_FAST=0 never skips. */
    static uint8_t on_mask[192][256];
    static uint8_t last_mask[192][256];
    static int last_bl, last_br, last_mg;
    static bool last_ok;
    for (int y = 0; y < 192; ++y)
        for (int x = 0; x < 256; ++x)
            on_mask[y][x] = (g_a[y][x].hit && !msg_layer_px(y, x) && !(g_hud_erase && g_hud_row_live && g_hud_row[y][x])) ? 1 : 0;
    if (msg_fast() && last_ok && band_l == last_bl && band_r == last_br && margin == last_mg &&
        std::memcmp(on_mask, last_mask, sizeof on_mask) == 0)
        return;
    std::memcpy(last_mask, on_mask, sizeof on_mask);
    last_bl = band_l; last_br = band_r; last_mg = margin; last_ok = true;
    HITCH_SCOPE(HK_MSGRESOLVE);
    for (int y = 0; y < 192; ++y) {
        int run = kBridgeX + 1;
        for (int x = 0; x < 256; ++x) {
            const bool on = on_mask[y][x] != 0;
            run = on ? 0 : (run + 1);
            dil[y][x] = (run <= kBridgeX) ? 1 : 0;
        }
        run = kBridgeX + 1;
        for (int x = 255; x >= 0; --x) {
            const bool on = on_mask[y][x] != 0;
            run = on ? 0 : (run + 1);
            if (run <= kBridgeX) dil[y][x] = 1;
        }
    }
    // Pass 2: vertical dilation, the same two-sweep trick down the columns. It
    // reads the HORIZONTALLY dilated mask, so the result is the rectangular
    // dilation of the original -- which is what makes two marks that are
    // diagonally apart within the two radii one element.
    //
    // IT READS A SNAPSHOT, NOT THE BUFFER IT IS WRITING. Sweeping down and
    // then up over the SAME array lets the second sweep treat pixels the
    // first sweep invented as if they were real marks, which quietly grows
    // the vertical reach past kBridgeY. That would still be safe -- the
    // bridge errs large on purpose -- but it would no longer be the 16 x 8
    // rectangle this file documents, and port/tools/wide_sweep.py groups the
    // captured pixels by exactly that rectangle in order to measure the
    // elements the placement actually acted on. A checker grouping by a
    // different rule from the code is measuring its own opinion.
    static uint8_t vsrc[192][256];
    std::memcpy(vsrc, dil, sizeof(vsrc));
    for (int x = 0; x < 256; ++x) {
        int run = kBridgeY + 1;
        for (int y = 0; y < 192; ++y) {
            if (vsrc[y][x]) { run = 0; continue; }
            ++run;
            if (run <= kBridgeY) dil[y][x] = 1;
        }
        run = kBridgeY + 1;
        for (int y = 191; y >= 0; --y) {
            if (vsrc[y][x]) { run = 0; continue; }
            ++run;
            if (run <= kBridgeY) dil[y][x] = 1;
        }
    }

    // Pass 3: provisional labels with 8-connectivity over the dilated mask,
    // unioning as we go. 8 rather than 4 so a diagonal touch does not split an
    // element that the dilation only just closed.
    g_nlab = 1;
    g_par[0] = 0;
    for (int y = 0; y < 192; ++y) {
        for (int x = 0; x < 256; ++x) {
            if (!dil[y][x]) { g_lbl[y][x] = 0; continue; }
            int best = 0;
            for (int dy = -1; dy <= 0; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dy == 0 && dx >= 0) continue;
                    const int ny = y + dy, nx = x + dx;
                    if (ny < 0 || nx < 0 || nx >= 256) continue;
                    const int nl = g_lbl[ny][nx];
                    if (!nl) continue;
                    if (!best) best = nl; else unite(best, nl);
                }
            }
            if (!best) {
                if (g_nlab < kMaxLabels) { best = g_nlab; g_par[best] = best; ++g_nlab; }
                else best = kMaxLabels - 1;   // see the bound note above
            }
            g_lbl[y][x] = (int16_t)best;
        }
    }

    // Pass 4: flatten the union-find, then take each element's extent from the
    // ORIGINAL hit pixels rather than the dilated ones. That distinction is not
    // cosmetic: the anchor arithmetic has to see where the element really ends,
    // so that a right-band element keeps its native right margin exactly the way
    // the band rule did, instead of a margin shortened by the dilation radius.
    static int x0[kMaxLabels], x1[kMaxLabels];
    for (int i = 0; i < g_nlab; ++i) { x0[i] = 256; x1[i] = -1; }
    for (int y = 0; y < 192; ++y) {
        for (int x = 0; x < 256; ++x) {
            const int l = g_lbl[y][x];
            if (!l) continue;
            const int r = find_root(l);
            g_lbl[y][x] = (int16_t)r;
            if (!g_a[y][x].hit || msg_layer_px(y, x) || (g_hud_erase && g_hud_row_live && g_hud_row[y][x])) continue;
            if (x < x0[r]) x0[r] = x;
            if (x > x1[r]) x1[r] = x;
        }
    }

    // The anchor itself: one decision per element, from its own centre against
    // the two splits. A label with no original hit pixels (all dilation, no
    // substance) is never read for a hit pixel, but it is given the centred
    // offset rather than left undefined.
    g_off[0] = margin / 2;
    for (int i = 1; i < g_nlab; ++i) {
        if (x1[i] < 0) { g_off[i] = margin / 2; continue; }
        const int c = (x0[i] + x1[i]) / 2;
        g_off[i] = (c < band_l) ? 0 : (c >= band_r) ? margin : margin / 2;
    }
}

}  // namespace hudelem

// SM64DS_HUD_BANDSPLIT=1 -- put the OLD per-column band split back, on this same
// binary. It exists to be the control half of the per-element measurement: the
// torn frame and the whole one then come off ONE build at ONE state base, so a
// before/after image pair cannot be two different builds disagreeing about
// something else. Unset -- every ordinary run and every shipped build -- the
// per-element rule above is what places the HUD.
inline bool hud_bandsplit_env() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_HUD_BANDSPLIT");
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v != 0;
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
 * plotter (_ZN11dScMgBase_c11PaintSquareEiiii) adds G + 0xc0 for the top engine, so a scene
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
/* ---- THE THREE ATTENTION ARROWS, HELD BACK AND PUT SOMEWHERE ELSE ----------
 *
 * WHAT THE CUE IS. Stage::RenderBouncingArrows (src/, ROM 0x02023be0) draws
 * three arrows along the bottom edge of the TOP screen, bouncing between two
 * rows, with a sound: the DS telling the player to look at the other screen.
 * It is raised by the ROM's own condition word (HUD::Render calls it only when
 * data_0209f284 is set) on a map event. This port has no other screen -- the
 * bottom screen is a corner inset over the same picture -- so the arrows point
 * at bare floor a long way from the map.
 *
 * WHAT HAPPENS INSTEAD. With the improved map on, the arrow texels are kept
 * OUT of the top screen's own buffer here and handed to hal/sub_screen.cpp,
 * which draws the two outer ones, small, just above the map panel after it
 * has composed it: one over the panel's left quarter, one over its right
 * quarter (port_bounce_arrows_present). Nothing in src/ changes, no call is
 * skipped, the ROM still computes and submits three sprites, and the sound is
 * not touched at all.
 *
 * THE TRANSLATION IS CONSTANT, WHICH IS WHAT KEEPS THE BOUNCE. The arrows
 * bounce by alternating their row, so a placement that PINNED them to a fixed
 * row would hold them still. Vertically they are therefore moved by one offset
 * for the whole band -- the DS screen's bottom edge is mapped to just above the
 * panel's top edge -- and each arrow carries whatever row the ROM gave it. The
 * ROM rests them against the bottom edge, so at rest they sit on the panel and
 * bounce up off it, which is the motion the cartridge draws.
 *
 * THEY ARE SIZED TO THE PANEL. Drawn at the picture's own scale each arrow
 * was about a third of the inset panel's height and the three spanned wider
 * than it; each is now ARROW_PANEL_FRAC of the panel's width (one constant,
 * below), and the bounce is scaled with it. */
uint32_t g_arrow_px[192][256];
int g_arrow_n;                           /* texels captured this frame */
int g_arrow_x0, g_arrow_x1, g_arrow_y0, g_arrow_y1;   /* their bounding box */

/* The template's tile numbers, read out of the cartridge's own OamAttr records
   the way hal/sub_screen.cpp reads the camera buttons': an OamAttr is four
   halfwords and attr3 == 0xffff ends the list (include/OamAttr.h). Tile 0 is
   the ROM's "nothing here" and is never added. Sixteen is a guard against a
   list that never terminates, not a length. */
unsigned short g_arrow_tiles[16];
int g_arrow_tiles_n = -1;

void arrow_tiles_init(void)
{
    g_arrow_tiles_n = 0;
    const unsigned short *t = _ZN3OAM14BOUNCING_ARROWE;
    for (int i = 0; i < 16; ++i) {
        const unsigned short a2 = t[i * 4 + 2];
        const unsigned short a3 = t[i * 4 + 3];
        const unsigned short tile = (unsigned short)(a2 & 0x03FFu);
        if (tile && g_arrow_tiles_n <
                (int)(sizeof g_arrow_tiles / sizeof g_arrow_tiles[0])) {
            int seen = 0;
            for (int k = 0; k < g_arrow_tiles_n; ++k)
                if (g_arrow_tiles[k] == tile) { seen = 1; break; }
            if (!seen) g_arrow_tiles[g_arrow_tiles_n++] = tile;
        }
        if (a3 == 0xffff) break;
    }
    if ([] { static const bool v = std::getenv("SM64DS_MINIMAP_TRACE") != nullptr; return v; }()) {
        std::fprintf(stderr, "[mmtrace] bouncing-arrow tiles (%d):",
                     g_arrow_tiles_n);
        for (int k = 0; k < g_arrow_tiles_n; ++k)
            std::fprintf(stderr, " %u", (unsigned)g_arrow_tiles[k]);
        std::fprintf(stderr, "\n");
    }
}

/* A TILE NUMBER AND A BAND, both, for the same reason the camera-button veto
   wants both: engine A's OBJ VRAM is shared by every top-screen sprite and a
   tile number alone could collide with an unrelated graphic. The cue's own band
   is the bottom of the top screen -- RenderBouncingArrows passes y 0xae / 0xb0,
   174 and 176 of 192 -- so an entry whose box is centred above the screen's
   bottom third is not this cue whatever tile it carries. */
inline bool arrow_entry(unsigned short a2, int y, int bh)
{
    if (g_arrow_tiles_n < 0) arrow_tiles_init();
    if (y + bh / 2 < 128) return false;
    const unsigned short tile = (unsigned short)(a2 & 0x03FFu);
    for (int k = 0; k < g_arrow_tiles_n; ++k)
        if (g_arrow_tiles[k] == tile) return true;
    return false;
}

/* THE MESSAGE BOX'S CURSORS, read out of the cartridge's own four templates
   (the banner over msg_layer_px) rather than written down as numbers, the way the
   bouncing arrows are above. A drawn entry is one of them when its shape, its
   size, its tile and its palette are a template's: OAM::Render keeps all four
   from the template when Message::Update passes palette -1, and only the
   position and the priority come from the call. Four fields and not the tile
   alone because engine A's OBJ VRAM is shared by every top-screen sprite. */
struct MsgCur { unsigned short a0, a1, a2; };
MsgCur g_msgcur_tpl[4];

inline unsigned short tpl16(const unsigned char *p, int k)
{
    return (unsigned short)(p[k * 2] | (p[k * 2 + 1] << 8));
}

void msgcur_init(void)
{
    const unsigned char *const t[4] = {
        data_ov002_0210c390, data_ov002_0210c398,
        data_ov002_0210c3a0, data_ov002_0210c3a8};
    for (int k = 0; k < 4; ++k) {
        g_msgcur_tpl[k].a0 = (unsigned short)(tpl16(t[k], 0) & 0xC000u);
        g_msgcur_tpl[k].a1 = (unsigned short)(tpl16(t[k], 1) & 0xC000u);
        g_msgcur_tpl[k].a2 = (unsigned short)(tpl16(t[k], 2) & 0xF3FFu);
    }
}

inline bool msgcur_entry(unsigned short a0, unsigned short a1,
                         unsigned short a2)
{
    static bool init;
    if (!init) { msgcur_init(); init = true; }
    for (int k = 0; k < 4; ++k)
        if ((a0 & 0xC000u) == g_msgcur_tpl[k].a0
            && (a1 & 0xC000u) == g_msgcur_tpl[k].a1
            && (a2 & 0xF3FFu) == g_msgcur_tpl[k].a2)
            return true;
    return false;
}

void arrow_capture(int px, int py, uint32_t color)
{
    if (!g_arrow_n) {
        g_arrow_x0 = g_arrow_y0 = 1 << 30;
        g_arrow_x1 = g_arrow_y1 = -1;
    }
    if (!g_arrow_px[py][px]) ++g_arrow_n;
    g_arrow_px[py][px] = color | 0xFF000000u;
    if (px < g_arrow_x0) g_arrow_x0 = px;
    if (px > g_arrow_x1) g_arrow_x1 = px;
    if (py < g_arrow_y0) g_arrow_y0 = py;
    if (py > g_arrow_y1) g_arrow_y1 = py;
}

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
    /* THE ATTENTION ARROWS, asked once for the walk. Last frame's capture is
       dropped HERE rather than after it was drawn, so the layer is exactly
       what this frame submitted whether or not the panel got as far as
       drawing it. Only the rectangle that was written is touched. */
    if (g_arrow_n) {
        for (int ay = g_arrow_y0; ay <= g_arrow_y1; ++ay)
            std::memset(&g_arrow_px[ay][g_arrow_x0], 0,
                        (size_t)(g_arrow_x1 - g_arrow_x0 + 1) * sizeof(uint32_t));
        g_arrow_n = 0;
    }
    const bool arrows_moved = hal_minimap_arrow_reanchor_on() != 0;
    /* THE BOX'S CURSORS (msg_layer_px) are only looked for while BG3, the
       box's layer, is switched on in DISPCNT: Message::Update draws a cursor
       only over an open box. */
    const bool box_up = (dispcnt >> 11) & 1;
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
        if (g_port_obj_a_skip && g_port_obj_a_skip(i)) continue;   /* CLICKSTART1: drawn by the title recipe (ntr/ppu.cpp) */
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
        /* Is this one of the three bouncing arrows, and is this a run that
           moves them? If so its texels go to the capture layer instead of to
           g_a, so the whole cue leaves the top screen's own buffer in one
           piece -- it neither draws where the ROM put it nor takes part in the
           priority resolution of the sprites that stay. */
        const bool arrow = arrows_moved && arrow_entry(a2, y, bh);
        /* and is it one of the message box's own cursors (msg_layer_px)? */
        const bool msgcur = box_up && msgcur_entry(a0, a1, a2);
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
                /* THE CUE IS TAKEN OUT HERE, before any of the resolution
                   below, because it is not being drawn on this screen at all.
                   Taking it out after the priority tests would let it lose to
                   a sprite it is about to be moved away from. */
                if (arrow) { arrow_capture(px, py, color); continue; }
                /* THE WINDOW UNIT GATES THE SPRITES TOO (run hunt5, lane WIPES1).
                   WINOUT / WININ bit 4 is the OBJ layer, and the star / circle
                   wipes of the minigames program WINOUT = 0 on both engines
                   (dWipe_c, src/func_0202ee94.c): outside the shape not even a
                   sprite shows, only the backdrop. The BG loop and the 3D layer
                   (hide_3d_outside_windows) already honour that mask; the
                   sprites did not, so the lives counter, the score and the
                   playfield sprites of a minigame stayed on the screen outside
                   the star. Engine B's raster_obj has always tested this bit.
                   A window-less frame answers 0x3F, so only a frame with an
                   armed window can drop a pixel here. SM64DS_WINOBJ_OFF=1 puts
                   the old picture back on this binary. */
                if (!winobj_off_env() && !(window_mask(win, px, py) & 0x10))
                    continue;
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
                        /* THIS BLEND READS A 3D PIXEL, one per DS pixel, at the
                           block's top-left corner: a frame held on the graphics
                           card has none on the processor (run cardfull, CARD2).
                           Settle it first, before anything is written, so the
                           picture is the one the processor always made. */
                        ntr::gx_card_settle_because("a sprite blended over the 3D");
                        /* THE 3D PIXEL THIS SPRITE BLENDS AGAINST, read from
                           the pre-smoothing copy when there is one (ntr/gx.h
                           gx_aa_preimage). A semi-transparent sprite over a
                           smoothed edge would otherwise carry the smoothing
                           into the blend, and this blend is written into the
                           frame the display capture reads, so the game would
                           see the setting through it. Null -- the default --
                           is the framebuffer, unchanged. */
                        const uint32_t *pre3d = ntr::gx_aa_preimage();
                        const int by = py * yscale, bx = px * xscale;
                        color = blend_alpha(
                            color,
                            pre3d ? pre3d[(size_t)by * ntr::SCREEN_W + bx]
                                  : fb.px[by][bx],
                            bl.eva, bl.evb);
                    }
                }
                g_a[py][px].color = color;
                g_a[py][px].hit = true;
                g_a[py][px].owner = kOwnerObj;
                /* whose texel this is, for the widescreen placement */
                g_a[py][px].msgcur = msgcur ? 1 : 0;
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

// ---- ENGINE A'S BRIGHTNESS TARGETS (BLDCNT mode 2/3 with BLDY) ------------
//
// WHAT THE DS DOES. The colour special-effects unit's brightness modes lighten
// (mode 2) or darken (mode 3) a pixel only when two things hold AT THAT PIXEL:
// the layer on top there is one of BLDCNT's first targets (bits 0-5: BG0..BG3,
// OBJ, the backdrop), and the window the pixel falls in has its colour-effect
// bit (bit 5 of WININ / WINOUT) set. This is the rule engine B's scan-out
// already applies (ntr/ppu_sub.cpp, blend_apply).
//
// WHAT THE PORT DID. walk_window's engine-A fade composite read BLDCNT/BLDY
// and lightened or darkened EVERY pixel of the top screen. For a whole-screen
// fade (first targets 0x3f, no window) that is the same answer. For the
// dialogue box it is not: func_0201f32c (src/func_0201f32c.c:232-243) opens
// the box with the first targets data_0209d45c & ~0x38 (the 3D layer and the
// tile BGs under the text, never BG3, OBJ or the backdrop), WIN0 around the
// box with WININ's effect bit set and WINOUT = 0x17 (effect bit clear), then
// BLDY 7 up or 8 down. On the cartridge only the scene inside the box
// rectangle lightens or darkens and the text (BG3) and the HUD sprites stay as
// drawn; the port washed out the whole top screen, HUD, text and inset map.
//
// THE MASK. Built only on a frame that needs it (a brightness mode with a
// nonzero coefficient whose target set or window set leaves some pixel out);
// every other frame leaves it off and the fade composite keeps its old
// whole-panel loop, so a plain fade is untouched. One byte per HOST pixel,
// SCREEN_W stride like the framebuffer and gx_coverage: 1 = this pixel takes
// the brightness. The default pass gives every host pixel the layer the 2D
// unit sees where no 2D layer is on top -- the 3D layer where the 3D engine
// drew or the clear plane is opaque, else the backdrop -- and the window it
// falls in; the final blit then overwrites every pixel a 2D layer put on
// screen with that layer's own answer. Engine A's semi-transparent sprites
// take their alpha in raster_obj already; here they are judged by the OBJ
// target bit like any sprite.
//
// SM64DS_BRIGHT_MASK_OFF=1 keeps the old whole-panel loop on this binary, so a
// before/after is one exe.
unsigned char *g_bm;
bool g_bm_live;
/* THE MASK OF A FRAME HELD ON THE GRAPHICS CARD (run cardfull, lane CARD2) has
   no coverage to answer "is the 3D layer on top of this pixel", because the
   coverage is on the card. So the default pass leaves the answer to the
   composite and records both possible verdicts, with what the 2D wrote:
     bit 0  the fade takes this pixel if the 3D layer shows here
     bit 1  ... if the backdrop shows here
     bit 2  it takes the 2D pixel written here
     bit 3  a 2D pixel in front of the 3D layer was written here
     bit 4  a 2D pixel BEHIND the 3D layer was written here
   g_bm_card says the mask is in that encoding; bm_card_to_classic turns it into
   the one-byte verdict the processor's fade reads, which a frame that is
   settled before the fade needs. g_bm_shown3d / g_bm_clear_opaque are the two
   facts the answer needs besides coverage. */
bool g_bm_card;
bool g_bm_shown3d, g_bm_clear_opaque;

/* THE CORNER-INSET PANEL'S RECTANGLE FOR THIS FRAME, for port_engine_a_fade
   (the inset takes engine B's brightness, not engine A's: see there). Set by
   port_engine_a_bright_mask_rect, which hal_sub_screen_present calls on every
   frame that drew the panel; cleared where the brightness mask is, at the top
   of the compositor, so a frame that draws no panel never inherits one. */
bool g_inset_fade_set;
int g_inset_fade_rc[4];
/* SM64DS_CARD_SETTLE_DRILL=<bits> (a test, off unless set): settle a frame held
   on the graphics card at one more place, so the paths that only a rare frame
   reaches run on every frame of a row. bit 0 settles just before the final blit
   (a sprite blended over the 3D does this), bit 1 just before engine A's fade
   (the save menu's swapped picture does). The picture must come out the same. */
inline unsigned settle_drill() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_CARD_SETTLE_DRILL");
        v = e ? (int)std::strtoul(e, nullptr, 0) : 0;
    }
    return (unsigned)v;
}

inline bool bm_off_env() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_BRIGHT_MASK_OFF");
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v != 0;
}

// Does this frame's brightness leave any pixel out? A window region whose
// effect bit is clear, or a first-target set short of all six layers.
bool bm_needed(const Windows &w) {
    if (bm_off_env()) return false;
    const uint16_t cnt = rd16(kRegBase + 0x50);
    const int mode = (cnt >> 6) & 3;
    if (mode != 2 && mode != 3) return false;
    if ((rd16(kRegBase + 0x54) & 0x1F) == 0) return false;
    if ((cnt & 0x3F) != 0x3F) return true;
    if (!w.any) return false;
    for (int i = 0; i < 2; ++i)
        if (w.on[i] && !(w.in[i] & 0x20)) return true;
    if (w.obj_on && !(w.obj_in & 0x20)) return true;
    return !(w.out & 0x20);
}

inline unsigned char bm_verdict(unsigned layer, unsigned wmask) {
    const unsigned first = rd16(kRegBase + 0x50) & 0x3F;
    return ((wmask & 0x20) && (first & (1u << layer))) ? 1 : 0;
}

// The default pass: every host pixel, as if no 2D layer covered it. The host
// column maps to its DS column the way the message layer is placed (x * scale
// at 4:3; centred at the uniform scale in 16:9), so the box's window lines up
// with the box's text; a host column outside the DS panel is outside every
// window and takes WINOUT.
void bm_fill_default_ref(uint32_t dispcnt, const Windows &w) {
    if (!g_bm) {
        g_bm = static_cast<unsigned char *>(
            std::malloc((size_t)ntr::SCREEN_W * ntr::SCREEN_H));
        if (!g_bm) return;
    }
    const int sx = ntr::active_w / 256;
    const int margin = ntr::active_w - ds_col_x0(256);
    const bool shown3d = bg0_3d_shown(dispcnt);
    /* A FRAME HELD ON THE GRAPHICS CARD has no coverage plane here: both
       verdicts are recorded and the composite picks (see g_bm_card above) */
    const bool card = ntr::gx_card_overlay_active() != 0;
    const uint8_t *cover = (shown3d && !card) ? ntr::gx_coverage() : nullptr;
    /* CLEAR_COLOR bits 16-20: an opaque clear plane is still the 3D layer */
    const bool clear_opaque =
        shown3d && ((rd32(0x04000350) >> 16) & 0x1F) != 0;
    g_bm_card = card;
    g_bm_shown3d = shown3d;
    g_bm_clear_opaque = clear_opaque;
    const unsigned out_mask = w.any ? w.out : 0x3F;
    for (int hy = 0; hy < ntr::active_h; ++hy) {
        const int y = ds_row_of(hy);
        unsigned char *row = g_bm + (size_t)hy * ntr::SCREEN_W;
        for (int hx = 0; hx < ntr::active_w; ++hx) {
            int x;
            if (ntr::widescreen) {
                const int d = hx - margin / 2;
                x = d < 0 ? -1 : ds_col_of(d);
            } else {
                x = hx / sx;
            }
            const unsigned wm = (x >= 0 && x < 256 && y < 192)
                                    ? window_mask(w, x, y) : out_mask;
            if (card) {
                row[hx] = (unsigned char)(bm_verdict(0u, wm) |
                                          (bm_verdict(5u, wm) << 1));
                continue;
            }
            const bool is3d = shown3d
                && (clear_opaque || cover[(size_t)hy * ntr::SCREEN_W + hx]);
            row[hx] = bm_verdict(is3d ? 0u : 5u, wm);
        }
    }
    g_bm_live = true;
}

/* HITCH9C: the same pass with its per-pixel arithmetic hoisted. The original ran, for every host pixel (1.3 M of them at
   a 4x scale in 16:9), a function call and two integer divisions to find the DS column, a window_mask() call, and a
   register read inside bm_verdict. None of that depends on anything but the column, the DS row and the frame's
   registers, so: the column map is built once per call, the window mask of a DS row once per DS row (256 calls, not
   one per host pixel), and the register once per call. A message box that darkens the 3D picture used to spend about
   4 ms of every picture here (measured, hal/hitch_probe.h's msgbm cell). Byte-identical to bm_fill_default_ref. */
void bm_fill_default(uint32_t dispcnt, const Windows &w) {
    if (!msg_fast()) { bm_fill_default_ref(dispcnt, w); return; }
    HITCH_SCOPE(HK_MSGBM);
    if (!g_bm) {
        g_bm = static_cast<unsigned char *>(
            std::malloc((size_t)ntr::SCREEN_W * ntr::SCREEN_H));
        if (!g_bm) return;
    }
    const int sx = ntr::active_w / 256;
    const int margin = ntr::active_w - ds_col_x0(256);
    const bool shown3d = bg0_3d_shown(dispcnt);
    const bool card = ntr::gx_card_overlay_active() != 0;
    const uint8_t *cover = (shown3d && !card) ? ntr::gx_coverage() : nullptr;
    const bool clear_opaque =
        shown3d && ((rd32(0x04000350) >> 16) & 0x1F) != 0;
    g_bm_card = card;
    g_bm_shown3d = shown3d;
    g_bm_clear_opaque = clear_opaque;
    const unsigned out_mask = w.any ? w.out : 0x3F;
    const unsigned first = rd16(kRegBase + 0x50) & 0x3F;        // bm_verdict's register, read once
    const int aw = ntr::active_w, ah = ntr::active_h;
    // host column -> DS column, -1 left of the DS panel (the original's x)
    static std::vector<int> xmap;
    xmap.resize((size_t)aw);
    const int ph = ntr::present_h();
    for (int hx = 0; hx < aw; ++hx) {
        if (ntr::widescreen) {
            const int d = hx - margin / 2;
            xmap[(size_t)hx] = d < 0 ? -1 : d * 192 / ph;       // ds_col_of(d)
        } else {
            xmap[(size_t)hx] = hx / sx;
        }
    }
    // verdict bits for a window mask: bm_verdict(layer, wm) = ((wm & 0x20) && (first & (1 << layer))) ? 1 : 0
    const unsigned char v0 = ((first & 1u) != 0) ? 1 : 0;       // layer 0 (the 3D layer) when the mask bit is set
    const unsigned char v5 = ((first & (1u << 5)) != 0) ? 1 : 0; // layer 5 (the backdrop)
    unsigned char wmrow[256];
    int wm_y = -1;
    for (int hy = 0; hy < ah; ++hy) {
        const int y = hy * 192 / ph;                            // ds_row_of(hy)
        unsigned char *row = g_bm + (size_t)hy * ntr::SCREEN_W;
        if (y != wm_y) {
            wm_y = y;
            if (y < 192)
                for (int x = 0; x < 256; ++x) wmrow[x] = (unsigned char)(window_mask(w, x, y) & 0xFF);
        }
        const uint8_t *crow = cover ? cover + (size_t)hy * ntr::SCREEN_W : nullptr;
        for (int hx = 0; hx < aw; ++hx) {
            const int x = xmap[(size_t)hx];
            const unsigned wm = (x >= 0 && x < 256 && y < 192) ? (unsigned)wmrow[x] : out_mask;
            const unsigned char hit = (wm & 0x20) ? 1 : 0;      // bm_verdict's window half
            if (card) {
                row[hx] = (unsigned char)((hit & v0) | ((hit & v5) << 1));
                continue;
            }
            const bool is3d = shown3d && (clear_opaque || crow[hx]);
            row[hx] = is3d ? (unsigned char)(hit & v0) : (unsigned char)(hit & v5);
        }
    }
    g_bm_live = true;
}

/* The mask of a frame that was held on the card and has been settled since:
   one verdict a pixel, the way the processor's fade reads it. Needs the
   coverage plane, which the settle has just filled. */
void bm_card_to_classic() {
    if (!g_bm_card) return;
    g_bm_card = false;
    if (!g_bm || !g_bm_live) return;
    const uint8_t *cover = g_bm_shown3d ? ntr::gx_coverage() : nullptr;
    for (int hy = 0; hy < ntr::active_h; ++hy) {
        unsigned char *row = g_bm + (size_t)hy * ntr::SCREEN_W;
        for (int hx = 0; hx < ntr::active_w; ++hx) {
            const unsigned code = row[hx];
            const bool cov = cover && cover[(size_t)hy * ntr::SCREEN_W + hx];
            const bool is3d = g_bm_clear_opaque || (g_bm_shown3d && cov);
            const bool def = (code & (is3d ? 1u : 2u)) != 0;
            const bool bmv = ((code >> 2) & 1u) != 0;
            const bool v = (code & 8u) ? bmv
                         : (code & 16u) ? (cov ? def : bmv) : def;
            row[hx] = v ? 1 : 0;
        }
    }
}

/* ---- TWODGPU1 (run hunt8): THE HOST-RESOLUTION HALF OF THIS UNIT ON THE CARD ----
 *
 * On a tick the graphics card holds (ntr/gx.h, GX_OV_*) the three loops that run
 * over the whole picture -- the 3D layer's window hide, the brightness mask's
 * default pass and the placement of every 2D cell -- are done by the card
 * (hal/gpu_raster.cpp, the section of the same name) into the overlay and mask
 * targets the composite reads, with the same integer rules. What stays here is
 * the DS-space half: the cell plane, the window masks, the per-cell verdicts.
 *
 * THE CPU PATH STAYS: a tick composes on the processor when any of these holds
 * (the switch is twod_wanted): Renderer 0 or a frame not held on the card, a
 * menu1 menu owns the picture (hal_menu_live), the level-clear overlay is live,
 * a test drill is armed, SM64DS_TWOD_CARD=0. A frame the card gave up on
 * between the begin and the blit, or that something settled in between (a
 * sprite blended over the 3D), goes down the processor's loops too.
 * SM64DS_TWOD_CARD_CHECK=1 runs the processor's loops beside the card's and
 * compares them (hal/gpu_raster.cpp, port_gpu_twod_check). */
unsigned char *g_tw_wm;       /* window masks of the 256x192 DS cells (heap: no new host global of size) */
uint32_t *g_tw_inst;          /* one rect per hit cell, four words each */
int g_tw_n;

inline bool twod_check_env() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_TWOD_CARD_CHECK");
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v != 0;
}

/* the switch: may this tick's host-resolution work go to the card? */
bool twod_wanted() {
    if (!port_gpu_twod_ready()) return false;
    if (!ntr::gx_card_overlay_active()) return false;
    if ((lc_ov.live || lc_ov.top_live) && !g_lift_card) return false;
    if (settle_drill()) return false;
    if (hal_menu_live() && !g_lift_card) return false;
    return true;
}

/* Start the tick's overlay on the card: it takes the 3D layer's window hide and
   the brightness mask's default pass (bm_on says the mask is live, with the
   state bm_fill_default would have left). False when the card has nothing for
   this tick; nothing has been touched then. */
bool twod_begin(uint32_t dispcnt, const Windows &w, ntr::Framebuffer &fb, bool &bm_on)
{
    bm_on = false;
    if (!g_tw_wm) g_tw_wm = static_cast<unsigned char *>(std::malloc(256 * 192));
    if (!g_tw_inst) g_tw_inst = static_cast<uint32_t *>(std::malloc(256 * 192 * 16));
    if (!g_tw_wm || !g_tw_inst) return false;
    const bool hide_cond = w.any && !hide3d_off_env() && bg0_3d_shown(dispcnt);
    const bool bm_cond = bm_needed(w);
    PortTwodFrame p;
    std::memset(&p, 0, sizeof p);
    p.active_w = ntr::active_w;
    p.active_h = ntr::active_h;
    p.present_h = ntr::present_h();
    p.widescreen = ntr::widescreen ? 1 : 0;
    p.sx = ntr::active_w / 256;
    p.half_margin = (ntr::active_w - ds_col_x0(256)) / 2;
    p.backdrop = bgr555(rd16(kPlttBase));
    p.out_mask = w.any ? (int)w.out : 0x3F;
    p.first = (int)(rd16(kRegBase + 0x50) & 0x3F);
    bool any_hidden = false;
    if (w.any && (hide_cond || bm_cond)) {
        for (int y = 0; y < 192; ++y)
            for (int x = 0; x < 256; ++x) {
                const unsigned m = window_mask(w, x, y);
                g_tw_wm[y * 256 + x] = (unsigned char)m;
                if (!(m & 1u)) any_hidden = true;
            }
        p.wm = g_tw_wm;
    }
    const bool out_hidden = w.any && (w.out & 1u) == 0;
    p.hide = (hide_cond && (any_hidden || out_hidden)) ? 1 : 0;
    p.bm_on = bm_cond ? 1 : 0;
    if (bm_cond && !g_bm) {
        g_bm = static_cast<unsigned char *>(
            std::malloc((size_t)ntr::SCREEN_W * ntr::SCREEN_H));
        if (!g_bm) return false;
    }
    if (!port_gpu_twod_begin(&p, bm_cond ? g_bm : nullptr, ntr::SCREEN_W,
                             &fb.px[0][0]))
        return false;
    g_tw_n = 0;
    if (bm_cond) {
        const bool shown3d = bg0_3d_shown(dispcnt);
        g_bm_card = true;
        g_bm_shown3d = shown3d;
        g_bm_clear_opaque = shown3d && ((rd32(0x04000350) >> 16) & 0x1F) != 0;
        g_bm_live = true;
        bm_on = true;
    }
    return true;
}

/* ---- lane TWOD: engine A's BG loop, the old one kept as the reference -------
 *
 * bg_loop_ref is the BG loop of port_message_composite_engine_a as it shipped
 * (0.5.1), word for word, drawing into whatever hit buffer it is handed. The live
 * loop now walks a per-frame list of the enabled BGs in paint order instead of
 * testing all sixteen (priority, BG) pairs at every pixel. SM64DS_TWOD_VERIFY=1
 * copies the hit buffer as it stands before the loop, runs this reference over the
 * copy, and compares every cell's colour, hit, owner and priority with the live
 * result: one line per differing frame (the first 20) and a SUMMARY line at exit.
 * SM64DS_TWOD_OLD=1 draws through the reference alone (timing A/B on one binary). */
void bg_loop_ref(Cell (*a)[256], const BgConfig *bgs, const Windows &win,
                 unsigned lmask, const Blend &bl)
{
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
                            && (bl.first & (1u << bg)) && a[y][x].hit
                            && (bl.second & (1u << a[y][x].owner)))
                            s = blend_alpha(s, a[y][x].color, bl.eva, bl.evb);
                        a[y][x].color = s;
                        a[y][x].hit = true;
                        a[y][x].owner = (uint8_t)bg;
                        a[y][x].prio = (uint8_t)prio;
                    }
                }
            }
        }
    }
}

Cell (*g_a_ref)[256];
long g_a_frames, g_a_bad, g_a_badcells;

int twod_env_on(const char *name, int &v)
{
    if (v < 0) {
        const char *e = std::getenv(name);
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v;
}

int twod_verify_a_on() { static int v = -1; return twod_env_on("SM64DS_TWOD_VERIFY", v); }
int twod_old_a_on() { static int v = -1; return twod_env_on("SM64DS_TWOD_OLD", v); }

void twod_verify_a_summary()
{
    std::fprintf(stderr, "[twod] SUMMARY engA verify: %ld composites compared, %ld differ "
                 "(%ld cells)\n", g_a_frames, g_a_bad, g_a_badcells);
}

/* Before the loop: keep the hit buffer as the loop will find it. */
bool twod_verify_a_begin()
{
    if (!twod_verify_a_on()) return false;
    if (!g_a_ref) {
        g_a_ref = static_cast<Cell (*)[256]>(std::calloc(192, sizeof(Cell) * 256));
        if (!g_a_ref) return false;
        std::atexit(twod_verify_a_summary);
    }
    std::memcpy(g_a_ref, g_a, sizeof(Cell) * 192 * 256);
    return true;
}

/* After the loop: the reference over the copy, then every cell compared. */
void twod_verify_a_end(const BgConfig *bgs, const Windows &win, unsigned lmask,
                       const Blend &bl, uint32_t dispcnt)
{
    bg_loop_ref(g_a_ref, bgs, win, lmask, bl);
    ++g_a_frames;
    long n = 0;
    int fx = -1, fy = -1;
    for (int y = 0; y < 192; ++y)
        for (int x = 0; x < 256; ++x) {
            const Cell &p = g_a[y][x], &q = g_a_ref[y][x];
            if (p.color != q.color || p.hit != q.hit || p.owner != q.owner || p.prio != q.prio) {
                if (!n) { fx = x; fy = y; }
                ++n;
            }
        }
    if (!n) return;
    ++g_a_bad;
    g_a_badcells += n;
    if (g_a_bad <= 20)
        std::fprintf(stderr, "[twod] engA MISMATCH composite %ld: %ld cells, first (%d,%d) "
                     "new %08x/%d/%u ref %08x/%d/%u, DISPCNT %08x\n", g_a_frames, n, fx, fy,
                     (unsigned)g_a[fy][fx].color, (int)g_a[fy][fx].hit, (unsigned)g_a[fy][fx].owner,
                     (unsigned)g_a_ref[fy][fx].color, (int)g_a_ref[fy][fx].hit,
                     (unsigned)g_a_ref[fy][fx].owner, (unsigned)dispcnt);
}

}  // namespace

/* The fade composite's question (tests/walk_window.cpp): null = lighten or
   darken the whole panel (no brightness this frame, or one that reaches every
   pixel); otherwise one byte per host pixel, SCREEN_W stride, 1 = apply. */
extern "C" const unsigned char *port_engine_a_bright_mask(void)
{
    return g_bm_live ? g_bm : nullptr;
}

/* ONE VERDICT FOR A HOST RECTANGLE (hal/sub_screen.cpp, the corner-inset
   panel). The inset is engine B's picture pasted into engine A's framebuffer,
   a host layout and not a DS layer, so the per-pixel rule above has nothing to
   say about it: judged by the engine-A layer that happens to lie under it, a
   partial-target blend (the course-info and pause screens' BLDCNT 00e1)
   punched that layer's shape into the map. The panel gets one verdict too,
   and it is NOT TAKEN (run hunt8, lane FIX092B): the map is engine B's picture
   and a partial-target dim, a message box or the pause, is engine A's and never
   reaches it (it used to darken the map under every message box). A mask-free
   fade, a plain fade to black, still covers the whole panel. Inert when no mask was built (then the whole panel is blended
   anyway). Clipped to the live picture. */
extern "C" void port_engine_a_bright_mask_rect(int x0, int y0, int x1, int y1)
{
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > ntr::active_w) x1 = ntr::active_w;
    if (y1 > ntr::active_h) y1 = ntr::active_h;
    /* the rectangle itself, for port_engine_a_fade, on every frame that drew
       the panel (the mask below exists only on frames that built one) */
    g_inset_fade_set = x1 > x0 && y1 > y0;
    g_inset_fade_rc[0] = x0; g_inset_fade_rc[1] = y0;
    g_inset_fade_rc[2] = x1; g_inset_fade_rc[3] = y1;
    /* TWODGPU1: a mask that lives on the card gets the panel's verdict there */
    if (g_bm_live && port_gpu_twod_live()) {
        port_gpu_twod_bm_rect(x0, y0, x1, y1);
        if (!twod_check_env()) return;
    }
    /* FIX092B: the verdict is "not taken". The map is engine B's picture, and
       a partial-target dim (a message box, the pause) is engine A's. */
    if (!g_bm_live || !g_bm) return;
    for (int y = y0; y < y1; ++y)
        for (int x = x0; x < x1; ++x) {
            unsigned char &m = g_bm[(size_t)y * ntr::SCREEN_W + x];
            m = 0;
        }
    if (port_gpu_twod_live() && twod_check_env())
        port_gpu_twod_check(reinterpret_cast<const unsigned *>(g_bm), ntr::SCREEN_W, g_bm,
                            ntr::SCREEN_W, 1);
}

/* THE FADE COMPOSITE, ENGINE A'S, for both frame loops.

   The DS colour-special-effects unit's brightness modes (BLDCNT mode 2 or 3
   plus BLDY) darken or brighten engine A's picture after the scene is drawn.
   0x4000050 / 0x4000054 are engine A's and only engine A's; engine B's blend
   is applied per pixel inside ntr/ppu_sub.cpp's scan-out. `px` is engine A's
   framebuffer (SCREEN_W stride, w x h live), so this is engine A's blend and
   nothing else, over the pixels the mask above says the DS blends (all of
   them on an ordinary fade).

   THIS USED TO LIVE IN tests/walk_window.cpp, WHICH ONLY THE LEVEL LOOP RUNS.
   The scene loop (hal/scene_boot.cpp: the title, the file select, the
   minigames) composited engine A and presented it without ever reading engine
   A's BLDCNT / BLDY, so every engine-A fade on those screens was missing: the
   title's opening fades engine A in from EVY 16 over its first frames and the
   port showed it at full brightness from frame 0. Both loops call
   hal_sub_screen_present right after this compositor, so it runs from the
   tail of that function (hal/sub_screen.cpp) and both get it, at the point
   the level loop always applied it: after the corner-inset panel went into
   the framebuffer (the inset still takes engine A's blend there, unchanged),
   before the host debug overlay and before the stacked compose reads `px`.

   EVY is the 0..16 coefficient: fade-to-black is rgb*(1 - evy/16),
   fade-to-white is rgb + (255-rgb)*evy/16, both per channel. */
extern "C" int port_fader_blend_state(int *evy, int *toWhite);
extern "C" int port_fader_blend_state_sub(int *evy, int *toWhite);
extern "C" int port_fader_entrance_wipe_armed(void);   /* hal/fader_wipes.cpp */
extern "C" void *data_0209f5bc;                  /* the installed fader */

/* THE CORNER-INSET PANEL DURING A WIPE (run hunt8, lane FADEFLASH1).

   The panel is engine B's picture, the DS's bottom screen, pasted into engine
   A's framebuffer, and it has always taken ENGINE A'S blend here. Every plain
   fade the fader drives writes both engines the same BLDCNT / BLDY, so for
   those the two answers are one answer. A level wipe is the exception:
   FaderWipe::AdvanceFade (src/engine/fader/_ZN9FaderWipe11AdvanceFadeEv.cpp)
   switches engine A's effects OFF while the wipe mesh moves (the mesh darkens
   the top screen itself) and ramps only engine B's BLDY with the interpolator
   (G2x::SetBlendBrightness on 0x4001050), so on the cartridge the bottom
   screen fades out WITH the closing wipe and back in with the opening one.
   Measured on the real ROM (headless melonDS, the level 34 exit, one row per
   frame): bottom screen mean luma 146.8 -> 0.0 across the closing, 0.0 through
   the load, then 8.9 and rising as the opening starts. The port's panel stayed
   at its full 145 -> 138 until engine A's cover frame took the whole picture
   to black, and came back at full brightness on the first opening frame: a
   bright box blinking out and in around the two black frames of every exit.

   So while a level wipe is the installed fader, with engine A on the upper
   screen and no partial-target mask in play (a message box or a pause dim
   keeps its old whole-panel behaviour), the panel takes engine B's own blend
   and engine A's blend covers everything else. Any frame where the two
   engines hold the same values is untouched. SM64DS_INSET_B_FADE_OFF=1 keeps
   the old behaviour on the same binary for a before / after. */
static int inset_b_fade_off(void)
{
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_INSET_B_FADE_OFF");
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v;
}

/* brightness mode 2 / 3 at coefficient `evy` over px, inside (inside != 0) or
   outside (inside == 0) the half-open host rectangle rc; `bm` is the per-pixel
   mask or null */
static void fade_pass(unsigned int *px, int w, int h, int evy, int toWhite,
                      const unsigned char *bm, const int *rc, int inside)
{
    for (int y = 0; y < h; ++y) {
        uint32_t *row = px + (size_t)y * ntr::SCREEN_W;
        const unsigned char *brow =
            bm ? bm + (size_t)y * ntr::SCREEN_W : nullptr;
        const int yin = rc && y >= rc[1] && y < rc[3];
        for (int x = 0; x < w; ++x) {
            if (rc) {
                const int in = yin && x >= rc[0] && x < rc[2];
                if (in != inside) continue;
            }
            if (brow && !brow[x]) continue;
            uint32_t p = row[x];
            int r = (p >> 16) & 0xff, g = (p >> 8) & 0xff, b = p & 0xff;
            if (toWhite) {
                r += ((255 - r) * evy) >> 4;
                g += ((255 - g) * evy) >> 4;
                b += ((255 - b) * evy) >> 4;
            } else {
                r -= (r * evy) >> 4;
                g -= (g * evy) >> 4;
                b -= (b * evy) >> 4;
            }
            row[x] = 0xFF000000u | ((uint32_t)r << 16) |
                     ((uint32_t)g << 8) | (uint32_t)b;
        }
    }
}

/* the overlay words the processor wrote itself (alpha FF) inside the host
   rectangle rc, faded by engine B's coefficient; every other word is left alone
   (zero alpha means "the card's own pixel here", which a fade must not turn into
   an opaque black one) */
static void fade_pass_ov(unsigned int *px, int w, int h, int evy, int toWhite,
                         const int *rc)
{
    const int x0 = rc[0] < 0 ? 0 : rc[0], x1 = rc[2] > w ? w : rc[2];
    const int y0 = rc[1] < 0 ? 0 : rc[1], y1 = rc[3] > h ? h : rc[3];
    for (int y = y0; y < y1; ++y) {
        uint32_t *row = px + (size_t)y * ntr::SCREEN_W;
        for (int x = x0; x < x1; ++x) {
            uint32_t p = row[x];
            if ((p >> 24) != 0xFFu) continue;
            int r = (p >> 16) & 0xff, g = (p >> 8) & 0xff, b = p & 0xff;
            if (toWhite) {
                r += ((255 - r) * evy) >> 4;
                g += ((255 - g) * evy) >> 4;
                b += ((255 - b) * evy) >> 4;
            } else {
                r -= (r * evy) >> 4;
                g -= (g * evy) >> 4;
                b -= (b * evy) >> 4;
            }
            row[x] = 0xFF000000u | ((uint32_t)r << 16) |
                     ((uint32_t)g << 8) | (uint32_t)b;
        }
    }
}

/* SM64DS_PANEL_WIPE_SETTLE=1 puts the old settle back (test only, for the before / after) */
static int bootstall_panel_on(void)
{
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_PANEL_WIPE_SETTLE");
        v = (e && *e && *e != '0') ? 0 : 1;
    }
    return v;
}

extern "C" void port_engine_a_fade(unsigned int *px, int w, int h)
{
    int evy = 0, toWhite = 0;
    const int a_on = px && port_fader_blend_state(&evy, &toWhite);
    if (a_on && evy > 16) evy = 16;
    /* the inset on engine B's blend (see above) */
    int b_evy = 0, b_white = 0;
    const int inset_b =
        px && g_inset_fade_set && !g_bm_live && !inset_b_fade_off() &&
        (*(volatile unsigned short *)0x4000304 & 0x8000u) &&   /* A upper */
        port_fader_entrance_wipe_armed() &&
        /* a wipe that is moving or closed; a parked open one (interp 0, which
           is where every wipe sits through an ordinary level) leaves the
           panel on the old rule */
        data_0209f5bc && ((const int *)data_0209f5bc)[1] != 0;
    if (inset_b && !port_fader_blend_state_sub(&b_evy, &b_white))
        b_evy = 0;
    if (settle_drill() & 2u) ntr::gx_card_settle_because("a test drill, before the fade");
    /* A FRAME HELD ON THE GRAPHICS CARD (run cardfull, lane CARD2): the picture
       is not on the processor to fade, and no pixel of it needs the brightness
       mask (a frame that did was settled before it got here). The composite
       applies this same fade, in the same integers, to the whole picture. The
       one fade the composite cannot say is the inset panel on engine B's own
       blend (a second coefficient inside one rectangle): that frame is made an
       ordinary one first and takes the processor's path below. */
    /* BOTH ENGINES ON THE SAME COEFFICIENT (run hunt8, lane BOOTSTALL1): the
       two fades above (engine A outside the panel, engine B inside it) are
       one fade over the whole picture, and no mask is live (inset_b needs
       !g_bm_live), so the frame is an ordinary one for the card. */
    const bool same_fade = inset_b && a_on && b_evy &&
                           (b_evy > 16 ? 16 : b_evy) == evy &&
                           (b_white ? 1 : 0) == (toWhite ? 1 : 0) &&
                           bootstall_panel_on();
    if (ntr::gx_card_overlay_active()) {
        if (!(inset_b && b_evy) || same_fade) {
            if (a_on) {
                port_gpu_card_set_fade(evy, toWhite);
                port_gpu_card_set_mask(g_bm_live ? g_bm : nullptr,
                                       ntr::SCREEN_W, g_bm_shown3d,
                                       g_bm_clear_opaque);
            }
            return;
        }
        /* THE LEVEL-ENTRY WIPE (run hunt8, lane BOOTSTALL1). While the wipe
           mesh moves the DS has engine A's effects off and fades only engine
           B, so the one thing to fade is the corner panel, and the panel is
           words the processor wrote itself into this overlay (alpha FF, a 2D
           pixel in front). Fading those words here is the same rule fade_pass
           runs on the settled picture, applied to the pixels that are the
           panel; nothing under it needs reading back. Settling the whole
           picture for it cost the first second of every course 5 ms of
           readback and fade on the tick plus a settled replay for each
           in-between (about 29 ticks, a 14 to 48 ms hole in the pictures at
           144 Hz). A tick where engine A fades too keeps the settle. */
        if (!a_on && px && bootstall_panel_on()) {
            if (b_evy > 16) b_evy = 16;
            fade_pass_ov(px, w, h, b_evy, b_white, g_inset_fade_rc);
            return;
        }
        ntr::gx_card_settle_because("the inset panel on engine B's fade");
    }
    if (g_bm_card) bm_card_to_classic();
    if (!px || (!a_on && !(inset_b && b_evy)))
        return;
    /* ONLY THE PIXELS THE DS BLENDS: the layer on top there is one of
       BLDCNT's first targets and its window's effect bit is set. Null when
       every pixel qualifies (a plain fade). */
    const unsigned char *bm = port_engine_a_bright_mask();
    if (!inset_b) {
        fade_pass(px, w, h, evy, toWhite, bm, nullptr, 0);
        return;
    }
    if (a_on)
        fade_pass(px, w, h, evy, toWhite, bm, g_inset_fade_rc, 0);
    if (b_evy) {
        if (b_evy > 16) b_evy = 16;
        fade_pass(px, w, h, b_evy, b_white, nullptr, g_inset_fade_rc, 1);
    }
}

/* ---- AND WHERE THE CUE GOES: TWO SMALL ARROWS ON THE MAP'S TOP CORNERS ----
 *
 * Called by hal/sub_screen.cpp with the rectangle the map was just drawn into,
 * after the panel. Nothing captured means nothing drawn, which is every frame
 * but the ones the ROM raises the cue on: the cue starts, bounces and ends
 * exactly when Stage::RenderBouncingArrows draws it, and its sound is the
 * ROM's own and is not touched here.
 *
 * THE PANEL IS WHAT THEY ARE SIZED TO, not the picture. The DS sprites drawn
 * at the picture's scale were each about a third of the inset panel's height
 * and the three of them spanned wider than the panel (course 1, the Bob-omb
 * Buddy's "look on the Touch Screen map" line, 4:3). So each arrow is drawn
 * ARROW_PANEL_FRAC of the panel's width wide (never wider than the sprite at
 * the picture's own scale, for the biggest maps), keeping the sprite's aspect,
 * and only the two OUTER arrows the ROM submitted are drawn: the leftmost
 * centred over the panel's left quarter, the rightmost over its right quarter.
 * The middle one of the ROM's three (x 0x80) is not drawn. The panel here is
 * the decorated one the player sees (hal_sub_inset_rect, plate included), so
 * the arrows sit over its outer corners, a gap of ARROW_GAP_FRAC of its height
 * above its top edge. They are the ROM's own sprite texels (engine A's OBJ
 * layer), not mod art, so a mod pack changes nothing here.
 *
 * GROUPS ARE FOUND BY THEIR COLUMNS. The capture layer holds nothing but this
 * cue, so a run of columns with any texel in it is one arrow and a gap ends it.
 * That reads the arrows the ROM actually submitted rather than assuming three.
 *
 * THE BOUNCE IS THE ROM'S, SCALED. The arrows bounce by alternating their row
 * (y 0xae / 0xb0), so the band is moved by one LATCHED offset for the whole
 * cue: the lowest row the cue has reached is its resting row, and that row's
 * bottom edge lands the gap above the panel; every other row is carried
 * relative to it at the same scale as the width, so the two-row bounce shows
 * as the same motion, scaled. The latch drops the moment a frame captures
 * nothing, so the next cue measures itself afresh.
 *
 * If the band would run off the top of the picture (the biggest map on a 4:3
 * window is the whole window) it is pushed back down until it fits, and the
 * player sees the cue over the map instead of not at all. */
namespace {
/* The size may want nudging: this is the one number. */
const double ARROW_PANEL_FRAC = 0.20;   /* one arrow's width / the panel's width */
const double ARROW_GAP_FRAC = 0.10;     /* gap under the resting arrows / the panel's height */

/* The template's own box (DS pixels), for the room the cue needs above the panel before any arrow is captured
   (hal/sub_screen.cpp's pause-page fit asks at the head of the frame). Read out of the cartridge's OamAttr records:
   shape in attr0 bits 14-15, size in attr1 bits 14-15, signed offsets in attr0 bits 0-7 and attr1 bits 0-8. */
void arrow_template_box(int *bw, int *bh)
{
    static int w = -1, h = -1;
    if (w < 0) {
        static const unsigned char dims[3][4][2] = {
            { {8, 8}, {16, 16}, {32, 32}, {64, 64} },
            { {16, 8}, {32, 8}, {32, 16}, {64, 32} },
            { {8, 16}, {8, 32}, {16, 32}, {32, 64} } };
        const unsigned short *t = _ZN3OAM14BOUNCING_ARROWE;
        int x0 = 1 << 30, y0 = 1 << 30, x1 = -(1 << 30), y1 = -(1 << 30);
        for (int i = 0; i < 16; ++i) {
            const unsigned a0 = t[i * 4], a1 = t[i * 4 + 1];
            const unsigned shape = (a0 >> 14) & 3u, size = (a1 >> 14) & 3u;
            if (shape < 3) {
                const int ox = (int)(a1 & 0x1FFu) - ((a1 & 0x100u) ? 0x200 : 0);
                const int oy = (int)(signed char)(a0 & 0xFFu);
                if (ox < x0) x0 = ox;
                if (oy < y0) y0 = oy;
                if (ox + dims[shape][size][0] > x1) x1 = ox + dims[shape][size][0];
                if (oy + dims[shape][size][1] > y1) y1 = oy + dims[shape][size][1];
            }
            if (t[i * 4 + 3] == 0xffff) break;
        }
        w = x1 > x0 ? x1 - x0 : 24;
        h = y1 > y0 ? y1 - y0 : 24;
    }
    *bw = w;
    *bh = h;
}
}   /* namespace */

extern "C" int hal_sub_inset_rect(int *x, int *y, int *w, int *h);

/* THE ROOM THE CUE TAKES ABOVE A PANEL pw x ph (decorated, host pixels) on a picture pic_h rows tall: the gap, the
   arrow's height at its drawn scale and the ROM's two-row bounce. hal/sub_screen.cpp's pause-page fit keeps the page
   out of it. Sideways the arrows stay inside the panel's own width, so no room is needed at its sides. */
extern "C" int port_bounce_arrows_room(int pw, int ph, int pic_h)
{
    int bw, bh;
    arrow_template_box(&bw, &bh);
    double f = ARROW_PANEL_FRAC * pw / bw;
    if (pic_h > 0 && f > pic_h / 192.0) f = pic_h / 192.0;   /* never bigger than the sprite at the picture's own scale */
    return (int)(ARROW_GAP_FRAC * ph + (bh + 2) * f + 0.999);
}

extern "C" void port_bounce_arrows_present(unsigned int *dst, int dst_w,
                                           int dst_h, int px0, int py0,
                                           int pw, int ph)
{
    /* The latch this cue's vertical offset is measured into, and the frame
       that drops it: a frame with nothing captured is a frame with no cue. */
    static int rest_held, rest_y1;
    if (!g_arrow_n || !dst || dst_w < 1 || dst_h < 1) { rest_held = 0; return; }
    if (!rest_held || g_arrow_y1 > rest_y1) { rest_y1 = g_arrow_y1; rest_held = 1; }

    /* Which columns this cue occupies, and therefore how many arrows it is. */
    int gx0[8], gx1[8], n = 0;
    {
        int run = -1;
        for (int x = g_arrow_x0; x <= g_arrow_x1 + 1; ++x) {
            bool used = false;
            if (x <= g_arrow_x1)
                for (int y = g_arrow_y0; y <= g_arrow_y1 && !used; ++y)
                    if (g_arrow_px[y][x]) used = true;
            if (used) {
                if (run < 0) run = x;
            } else if (run >= 0) {
                if (n < (int)(sizeof gx0 / sizeof gx0[0])) {
                    gx0[n] = run;
                    gx1[n] = x - 1;
                    ++n;
                }
                run = -1;
            }
        }
    }
    if (n < 1) return;

    /* The panel as the player sees it: the decorated rectangle the present
       just drew, or the bare map rectangle if there is none. */
    int qx = px0, qy = py0, qw = pw, qh = ph;
    {
        int x, y, w, h;
        if (hal_sub_inset_rect(&x, &y, &w, &h)) { qx = x; qy = y; qw = w - x; qh = h - y; }
    }
    if (qw < 1 || qh < 1) return;

    /* The two outer arrows (or the one, if the ROM drew one). */
    const int m = n >= 2 ? 2 : 1;
    const int pick[2] = { 0, n - 1 };
    int aw = 1;
    for (int j = 0; j < m; ++j)
        if (gx1[pick[j]] - gx0[pick[j]] + 1 > aw) aw = gx1[pick[j]] - gx0[pick[j]] + 1;
    double f = ARROW_PANEL_FRAC * qw / aw;   /* host pixels per DS pixel, both ways */
    /* A big map (MinimapScale 3 and up) would make that larger than the sprite at the picture's own scale, the size the
       request found too big: never draw them bigger than that. */
    if (f > dst_h / 192.0) f = dst_h / 192.0;
    if (f <= 0) return;

    /* `base` is the host row the resting row's bottom edge lands on. */
    int base = qy - (int)(ARROW_GAP_FRAC * qh + 0.5);
    const int top = base - (int)((rest_y1 + 1 - g_arrow_y0) * f + 0.5);
    if (top < 0) base -= top;

    const int stride = ntr::SCREEN_W;
    static const int trace = std::getenv("SM64DS_MINIMAP_TRACE") ? 1 : 0;
    for (int j = 0; j < m; ++j) {
        const int k = pick[j];
        const int wds = gx1[k] - gx0[k] + 1;
        const double cx = (m == 1) ? qx + qw * 0.5 : qx + qw * (j == 0 ? 0.125 : 0.875);
        const int dw = (int)(wds * f + 0.5);
        int left = (int)(cx - dw * 0.5 + 0.5);
        /* the same clamp sideways: a whole arrow over the corner, never half of one */
        if (left + dw > dst_w) left = dst_w - dw;
        if (left < 0) left = 0;
        const int hy_a = base - (int)((rest_y1 + 1 - g_arrow_y0) * f + 0.5);
        const int hy_b = base - (int)((rest_y1 - g_arrow_y1) * f + 0.5);
        if (trace)
            std::fprintf(stderr, "[mmtrace] arrow %d of %d (drawn %d of %d) ds x %d..%d rows %d..%d -> host x %d..%d "
                         "rows %d..%d scale %.3f panel %d,%d %dx%d arrow/panel %.3f\n",
                         k + 1, n, j + 1, m, gx0[k], gx1[k], g_arrow_y0, g_arrow_y1, left, left + dw - 1,
                         hy_a, hy_b - 1, f, qx, qy, qw, qh, (double)dw / qw);
        for (int hy = hy_a; hy < hy_b; ++hy) {
            if (hy < 0 || hy >= dst_h) continue;
            const int sy = rest_y1 + 1 + (int)std::floor((hy + 0.5 - base) / f);
            if (sy < g_arrow_y0 || sy > g_arrow_y1) continue;
            for (int hx = left; hx < left + dw; ++hx) {
                if (hx >= dst_w) break;
                const int sx = gx0[k] + (int)std::floor((hx + 0.5 - left) / f);
                if (sx < gx0[k] || sx > gx1[k]) continue;
                const uint32_t c = g_arrow_px[sy][sx];
                if (c) dst[hy * stride + hx] = c;
            }
        }
    }
}

/* func_02019144's FIRST beat, hal/scene_boot.cpp. 1 = run that function's
   tail, 0 = the current graphics block did the display sync itself. */
extern "C" int port_graph_block_beat(void);
/* func_02019144's OAM upload, at the ROM's line in the frame; the body and the
   whole derivation are in hal/sub_screen.cpp, which owns OAM::Load's wiring. */
extern "C" void port_frame_oam_upload(void);

// Rasterise engine A's 2D layers into the DS 256x192 hit buffer, then composite
// the covered pixels over the host 3D framebuffer (scaled by integer factor).
// Called from walk_window right after gx_render and before the fade composite.
/* ---- POLISH093 P3: THE DIALOGUE BOX OPENING, BETWEEN TWO TICKS ----------------------------------------------------------------
   The box is BG3 shown through WIN0, and the ROM grows WIN0 a step every tick, so with SmoothMotion the pictures between two ticks showed
   the box at tick N's size from the first. The tick keeps the window it composed with and the one before it; while the box is GROWING (the
   last rectangle inside this one) a picture at `alpha` between them shows the box only inside the rectangle that far along. Both
   rectangles come back in host pixels (the compose's own mapping of DS cells: a DS pixel is present_h / 192 host pixels, centred in a wide
   picture), outer = this tick's, inner = the one at alpha. Not for a closing box (the picture of the larger rectangle does not exist) and
   not for a wipe (per-line windows) or any other window use: the signature is BG3 revealed through WIN0 alone. 0 when there is nothing
   to do. SM64DS_TWEEN_BOX=0 puts the old picture back. */
namespace { struct TwWin { int valid, x1, x2, y1, y2; }; TwWin g_tw_cur, g_tw_prev; int g_tw_closing; }
/* INTERP093 I1: 1 when the last port_tween_win answer was a CLOSING box (the rectangle shrinking): outer is then the rectangle that far along
   (between the last tick's and this one's), inner this tick's; the ring between them is the box's panel with no text. */
extern "C" int port_tween_closing(void) { return g_tw_closing; }
/* TEXT2D1: the dialogue box's line scroll (the ROM moves the text rows up 4 DS pixels a tick while its state is 3). A picture between two ticks
   shows the tick's text pixels moved back down by (1 - alpha) of that step, inside the box's rectangle (host pixels, like port_tween_win's).
   Returns 1 and fills rect (x0,y0,x1,y1) and *step_host (the full step in host pixels) when this tick scrolled; 0 otherwise.
   SM64DS_TEXT_GLIDE=0 is the kill switch (the tick's own picture). */
static int g_ts_live, g_ts_pend;
extern "C" int port_text_scroll_take(void);
extern "C" int port_text_scroll_note(int rect[4], double *step_host)
{
    static int off = -1;
    if (off < 0) { const char *e = std::getenv("SM64DS_TEXT_GLIDE"); off = (e && e[0] == '0') ? 1 : 0; }
    if (off || !g_ts_live || !g_tw_cur.valid) return 0;
    const int ph = ntr::present_h();
    const int half = (ntr::active_w - ds_col_x0(256)) / 2;
    rect[0] = half + (int)(g_tw_cur.x1 * ph / 192.0 + 0.5); rect[2] = half + (int)(g_tw_cur.x2 * ph / 192.0 + 0.5);
    rect[1] = (int)(g_tw_cur.y1 * ph / 192.0 + 0.5);        rect[3] = (int)(g_tw_cur.y2 * ph / 192.0 + 0.5);
    *step_host = 4.0 * ph / 192.0;
    return 1;
}
extern "C" int port_tween_win(double alpha, int outer[4], int inner[4])
{
    g_tw_closing = 0;
    static int off = -1;
    if (off < 0) { const char *e = std::getenv("SM64DS_TWEEN_BOX"); off = (e && e[0] == '0') ? 1 : 0; }
    if (off || !g_tw_cur.valid || !g_tw_prev.valid) return 0;
    const TwWin &c = g_tw_cur, &p = g_tw_prev;
    if (c.x1 == p.x1 && c.x2 == p.x2 && c.y1 == p.y1 && c.y2 == p.y2) return 0;
    if (c.x2 - c.x1 > 256 || c.y2 - c.y1 > 192 || p.x2 <= p.x1 || p.y2 <= p.y1) return 0;
    const bool growing = !(p.x1 < c.x1 || p.x2 > c.x2 || p.y1 < c.y1 || p.y2 > c.y2);
    const bool shrinking = !(c.x1 < p.x1 || c.x2 > p.x2 || c.y1 < p.y1 || c.y2 > p.y2);
    static int cl_off = -1;
    if (cl_off < 0) { const char *e = std::getenv("SM64DS_TWEEN_CLOSE"); cl_off = (e && e[0] == '0') ? 1 : 0; }
    if (!growing && !(shrinking && !cl_off)) return 0;
    if (alpha < 0) alpha = 0;
    if (alpha > 1) alpha = 1;
    const int ph = ntr::present_h();
    const int half = (ntr::active_w - ds_col_x0(256)) / 2;
    auto hx = [&](double x) { return half + (int)(x * ph / 192.0 + 0.5); };
    auto hy = [&](double y) { return (int)(y * ph / 192.0 + 0.5); };
    auto lerp = [&](int a, int b) { return a + (b - a) * alpha; };
    if (growing) {
        outer[0] = hx(c.x1); outer[1] = hy(c.y1); outer[2] = hx(c.x2); outer[3] = hy(c.y2);
        inner[0] = hx(lerp(p.x1, c.x1)); inner[1] = hy(lerp(p.y1, c.y1)); inner[2] = hx(lerp(p.x2, c.x2)); inner[3] = hy(lerp(p.y2, c.y2));
        return 1;
    }
    outer[0] = hx(lerp(p.x1, c.x1)); outer[1] = hy(lerp(p.y1, c.y1)); outer[2] = hx(lerp(p.x2, c.x2)); outer[3] = hy(lerp(p.y2, c.y2));
    inner[0] = hx(c.x1); inner[1] = hy(c.y1); inner[2] = hx(c.x2); inner[3] = hy(c.y2);
    g_tw_closing = 1;
    return 1;
}

/* RABBIT1: the glowing rabbits' rank by catch order (setting FixOriginalBugs, English only).
   The ROM gives every glowing rabbit but the last the same line, messages 0x142 (Mario) / 0x146 (Yoshi): "I'm the second fastest of my
   eight brothers" (src/game/actors/d_a_mip.cpp, StateReleased: SaveData::NumGlowingRabbitsFound() == 7 picks 0x143 / 0x147 "fastest", anything
   else 0x142 / 0x146). With the setting on, the word "second" in those two messages becomes the ordinal for 8 - (rabbits found before this
   catch): 0 eighth, 1 seventh, 2 sixth, 3 fifth, 4 fourth, 5 third, 6 second (the ROM's own text). Found 7 keeps 0x143 / 0x147 untouched.
   HOW: the bank in memory is never edited. The two messages' entries (data_0209d708 + id * 8, {s32 textOffset, u16 width, u8 lines}) get a
   textOffset that points at a host copy of the text with the six glyph bytes of "second" swapped; setting off (or a rank of "second") puts the
   ROM's textOffset back, so the bank is then byte for byte the cartridge's. Done once a frame here (the count cannot change between the talk
   being chosen and the box reading the entry: the rabbit is counted found only after its line). The widths: every ordinal line measures
   <= 113 px on the DS font (data_0208f074) against the 117 px box, so no line is re-broken. */
extern "C" int host_setting_fix_original_bugs(void);
extern "C" int GetOwnerLanguage(void);
extern "C" int _ZN8SaveData22NumGlowingRabbitsFoundEv(void);
extern "C" int data_0209d6fc[];   /* gMessageBankBase */
extern "C" int data_0209d708[];   /* gMessageEntries */
extern "C" int data_0209d70c[];   /* gMessageInfoSection */
static void rabbit_rank_refresh(void)
{
    static const int kIds[2] = { 0x142, 0x146 };
    static const char *const kOrd[7] = { "eighth", "seventh", "sixth", "fifth", "fourth", "third", "second" };
    static unsigned char s_text[2][7][320];   /* [message][rank][bytes] */
    static int s_orig[2];                     /* the ROM's textOffset values */
    static int s_base, s_ready;               /* bank base the copies were made for */
    static int s_cur = -1;                    /* -1 = ROM offsets in place, 0..6 = the rank in place */
    const int base_i = data_0209d6fc[0];
    if (!base_i || !data_0209d708[0] || !data_0209d70c[0]) return;
    const int lang = GetOwnerLanguage();
    const bool english = !(lang >= 2 && lang <= 5);
    int want = -1;
    if (english && host_setting_fix_original_bugs()) {
        const int found = _ZN8SaveData22NumGlowingRabbitsFoundEv();
        if (found >= 0 && found <= 6) want = found;   /* found 7 keeps 0x143 / 0x147, the ROM's "fastest" */
    }
    if (!s_ready && want < 0) return;   /* nothing ever changed: nothing to restore */
    unsigned char *entries = reinterpret_cast<unsigned char *>((size_t)data_0209d708[0]);
    if (s_ready && s_base != base_i) { s_ready = 0; s_cur = -1; }
    if (!s_ready) {
        unsigned char *info = reinterpret_cast<unsigned char *>((size_t)data_0209d70c[0]);
        const unsigned char *tbase = reinterpret_cast<unsigned char *>((size_t)base_i) + 0x28 + *reinterpret_cast<int *>(info + 4);
        static const unsigned char kSecond[6] = { 0x3f, 0x31, 0x2f, 0x3b, 0x3a, 0x30 };   /* s e c o n d in the font's glyph order */
        for (int m = 0; m < 2; ++m) {
            int off = *reinterpret_cast<int *>(entries + kIds[m] * 8);
            const unsigned char *t = tbase + off;
            int n = 0; while (n < 300 && t[n] != 0xff) ++n;
            int at = -1;
            for (int i = 0; i + 6 <= n; ++i) if (!memcmp(t + i, kSecond, 6)) { at = i; break; }
            if (at < 0 || n >= 300) { static int warned; if (!warned++) fprintf(stderr, "[rabbit] message %x is not the ROM text; left alone\n", kIds[m]); s_ready = 0; return; }
            for (int r = 0; r < 7; ++r) {
                const char *w = kOrd[r]; const int wl = (int)strlen(w);
                unsigned char *d = s_text[m][r]; int o = 0;
                memcpy(d, t, at); o = at;
                for (int k = 0; k < wl; ++k) d[o++] = (unsigned char)(0x2d + (w[k] - 'a'));   /* a..z = 0x2d.. */
                memcpy(d + o, t + at + 6, n - at - 6 + 1); /* with the 0xff */
            }
            s_orig[m] = off;
        }
        s_base = base_i; s_ready = 1; s_cur = -1;
    }
    if (want == s_cur) return;
    const unsigned char *tbase = reinterpret_cast<unsigned char *>((size_t)base_i) + 0x28
        + *reinterpret_cast<int *>((size_t)data_0209d70c[0] + 4);
    for (int m = 0; m < 2; ++m) {
        int off = (want < 0 || want == 6) ? s_orig[m] : (int)(s_text[m][want] - tbase);
        *reinterpret_cast<int *>(entries + kIds[m] * 8) = off;
    }
    fprintf(stderr, "[rabbit] rank line: %s (found %d)\n", want < 0 ? "ROM text" : kOrd[want], want);
    s_cur = want;
}

/* run after093, lane MGINTERP1: 1 while port_message_engine_a_replay draws a picture between two minigame ticks. The frame's
   own work at the head of the compositor (the graph block's beat, the VRAM publish, the OAM upload, the layer-mask publish, the
   text-scroll take, the tween window record, the level-clear question) is the TICK's and is not run again; only the raster. */
static int g_mc_replay;
static const uint32_t *g_mc_dirty_final;   /* MGINTERP1: the tick's finished picture, for a replay that redraws only what moved */
static uint32_t *g_mc_pre_blit;
static Cell g_a_tick[192][256];
static unsigned char g_mc_regs_drawn[0x40], g_mc_oam_drawn[0x400];
extern "C" const unsigned char *port_message_engine_a_regs_drawn(void) { return g_mc_regs_drawn; }
extern "C" const unsigned char *port_message_engine_a_oam_drawn(void) { return g_mc_oam_drawn; }
extern "C" void port_message_composite_engine_a(void *fbp)
{
    ntr::Framebuffer &fb = *reinterpret_cast<ntr::Framebuffer *>(fbp);
    if (!g_mc_replay) {
    rabbit_rank_refresh();   /* RABBIT1 */
    /* POLISH093 P3 (the text box opening between two ticks, see port_tween_win below): this tick's window record starts empty and the
       last tick's becomes the previous one. Set where the box's window is read, further down. */
    g_tw_prev = g_tw_cur;
    g_tw_cur.valid = 0;
    g_note.n = 0; g_slide_n = 0;   /* HUDMENU2 (H11) */
    g_ts_pend = port_text_scroll_take(); g_ts_live = 0;   /* TEXT2D1 */

    /* THE LEVEL-CLEAR GLYPH OVERLAY'S ONE QUESTION FOR THE FRAME, asked here
       -- above every early return -- so that a frame this unit declines is a
       frame the overlay reports as empty rather than as last frame's. Off, it
       is one call that answers 0 and nothing else in this file changes. */
    g_cz.begin(); g_cz.live = 0;
    lc_overlay_begin();
    g_cz.live = (lc_ov.live || lc_ov.top_live) ? 1 : 0;
    g_cz.mark(0);
    ++g_cz.n[g_cz.live];
    }   /* !g_mc_replay */
    /* and the brightness mask's: off until this frame builds one */
    g_bm_live = false;
    g_inset_fade_set = false;
    g_bm_card = false;
    /* TWODGPU1: the previous tick's overlay on the card is not this tick's */
    port_gpu_twod_reset();
    /* A FRAME HELD ON THE GRAPHICS CARD (run cardfull, lane CARD2) carries only
       this unit's 2D in the framebuffer, as an overlay (ntr/gx.h, GX_OV_*). The
       level-clear compose lifts pixels out of the picture it drew under them,
       so it needs the real 3D pixels: make the frame an ordinary one first. */
    /* menu1's lift into the DS-space plane (top_live) reads the 3D coverage too, so it settles the same way */
    g_lift_card = false;
    if ((lc_ov.live || lc_ov.top_live) && ntr::gx_card_overlay_active() && port_gpu_twod_ready() &&
        port_gpu_layer_ready_env() && hal_menu_layer_ok())
        g_lift_card = true;
    if ((lc_ov.live || lc_ov.top_live) && !g_lift_card) ntr::gx_card_settle_because("the level-clear compose");
    g_cz.mark(1);
    /* TWODGPU1: may this tick's host-resolution half go to the card? (after
       menu1's decision, which lc_overlay_begin above asks for). MGINTERP1: a replay draws on the processor. */
    const bool tw_try = g_mc_replay ? false : twod_wanted();

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
    const bool run_tail = g_mc_replay ? false : port_graph_block_beat() != 0;   /* MGINTERP1: the tick's beat already ran */
    /* DRAWFLASH1: A VRAM BANK'S NEW MAPPING IS THIS FRAME'S, so the captured block is in its window before anything below reads it.
       The beat above is the ROM's VBlank work: it programs VRAMCNT together with DISPCNT / BGxCNT, and on the DS the bank shows through its
       new mapping from the frame's first scanline, as BG3's direct-colour bitmap does. The move of the captured block into that window
       lived only in hal_sub_screen_present, after this compose, so for one picture BG3's held-face bitmap was read out of a window still
       holding the previous capture (the title's drawing page entered a second time: the earlier face, half overwritten, over the page).
       The same publish, now in the DS's order; the later call finds nothing left to do. */
    if (!g_mc_replay) ntr::ppu_vram_publish();
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
    /* MGINTERP1: the register file this raster reads, kept for the pictures between ticks. The BG offsets the frame's sync
       copies in later (hal/sub_screen.cpp) reach this raster one frame after engine B's, so a replay must start from these. */
    if (!g_mc_replay) {
        std::memcpy(g_mc_regs_drawn, reinterpret_cast<const void *>(kRegBase), sizeof g_mc_regs_drawn);
        std::memcpy(g_mc_oam_drawn, reinterpret_cast<const void *>(kOamBase), sizeof g_mc_oam_drawn);
    }

    const uint32_t dispcnt = rd32(kRegBase);
    const unsigned disp_mode = (dispcnt >> 16) & 3;
    /* the per-frame layer census, BEFORE the forced-blank exit below, because
       a frame the compositor declines is still a frame whose registers are the
       game's own statement about its layers. */
    if (!g_mc_replay) {
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
    if (msg_composite_debug()) {
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
    hide_3d_layer_off(dispcnt, fb);

    BgConfig bgs[4];
    bool any_bg = false;
    for (int i = 0; i < 4; ++i) { bgs[i] = read_bg(i, dispcnt); any_bg |= bgs[i].enabled; }
    const bool obj_on = (dispcnt >> 12) & 1;
    if (!any_bg && !obj_on) {
        /* nothing 2D enabled: the box is not up, leave the 3D frame. The
           brightness still decides per pixel between the 3D layer and the
           backdrop, and inside or outside WIN0 / WIN1 (no OBJ layer, so no
           OBJ window). */
        Windows w0;
        read_windows(dispcnt, w0);
        w0.obj_on = false;
        w0.any = w0.on[0] || w0.on[1];
        bool tw_bm0 = false;
        const bool tw_took0 = tw_try && twod_begin(dispcnt, w0, fb, tw_bm0);
        if (!tw_took0 || twod_check_env()) {
            hide_3d_outside_windows(dispcnt, w0, fb);
            if (bm_needed(w0)) bm_fill_default(dispcnt, w0);
        }
        if (tw_took0 && twod_check_env())
            port_gpu_twod_check(&fb.px[0][0], ntr::SCREEN_W,
                                g_bm_live ? g_bm : nullptr, ntr::SCREEN_W, 0);
        return;
    }

    /* THE OBJ-WINDOW MASK IS BUILT BEFORE THE BG LOOP, which is the whole point
       of it being a separate pass: the BGs below are masked per pixel and the
       mask has to exist by then. raster_obj still runs after them and still
       skips mode-2 sprites, so no sprite is drawn twice. */
    build_objwin(dispcnt);

    Windows win;
    read_windows(dispcnt, win);
    /* POLISH093 P3: the dialogue box's signature: BG3 on, revealed only through WIN0 (WININ0 lets BG3 in, WINOUT keeps it out), no
       per-line window rows (a wipe). Its rectangle, in DS pixels, is what the box opens by. */
    if (win.on[0] && !win.rows_on && bgs[3].enabled && (win.in[0] & 8u) && !(win.out & 8u)) {
        g_ts_live = g_ts_pend;   /* TEXT2D1 */
        g_tw_cur.valid = 1;
        g_tw_cur.x1 = win.x1[0]; g_tw_cur.x2 = win.x2[0];
        g_tw_cur.y1 = win.y1[0]; g_tw_cur.y2 = win.y2[0];
        /* HUDMENU2 (H7, Tango 10-20): the ending's subtitle box. DRAWDIST2 centred Peach's picture (rows 24..167) between equal bars, and the
           box the ROM puts on rows 160..191 then hung out of the picture into the lower bar. The box's window and its BG3 rows are moved
           up in this compose's own copies (never the registers) until the box ends at the picture's last row, so box and text sit inside
           the picture. Wide pictures only (the same condition that moved the 3D rows); 4:3 keeps the ROM's rows. SM64DS_SUBTITLE_UP=0 for the before picture. */
        static int up_off = -1;
        if (up_off < 0) { const char *e = std::getenv("SM64DS_SUBTITLE_UP"); up_off = (e && e[0] == '0') ? 1 : 0; }
        if (!up_off && ntr::gx_ending_viewport_shifted() && win.y2[0] > 168) {
            const int S = win.y2[0] - 168 + 4;   /* 4 rows of air under the last line */
            win.y1[0] -= S; win.y2[0] -= S - 2;   /* two rows of window under the last line: the glyphs' lowest row sits on the ROM window's last row */
            bgs[3].vofs = (bgs[3].vofs + S) & 0x1FF;
            g_tw_cur.y1 = win.y1[0]; g_tw_cur.y2 = win.y2[0];
        }
    }
    /* the 3D layer is a layer like the others: WINOUT / WININ bit 0 gate it
       (see hide_3d_outside_windows) */
    /* TWODGPU1: on a tick the card holds, the card does this and the default
       pass below (twod_begin); SM64DS_TWOD_CARD_CHECK also runs the loops */
    bool tw_bm = false;
    const bool tw_took = tw_try && twod_begin(dispcnt, win, fb, tw_bm);
    const bool tw_chk = tw_took && twod_check_env();
    bool bm_on = false;
    if (!tw_took || tw_chk)
        hide_3d_outside_windows(dispcnt, win, fb);
    /* the brightness targets' default pass (see ENGINE A'S BRIGHTNESS
       TARGETS); the final blit below fills in every 2D pixel */
    if (!tw_took || tw_chk) {
        if (bm_needed(win)) {
            bm_fill_default(dispcnt, win);
            bm_on = g_bm_live;
        }
    } else {
        bm_on = tw_bm;
    }

    g_cz.mark(2);
    // Clear the hit buffer, then BGs by priority (lower priority drawn last so
    // it wins), honouring the per-pixel window mask.
    for (int y = 0; y < 192; ++y)
        for (int x = 0; x < 256; ++x) g_a[y][x].hit = false;

    const unsigned lmask = layer_mask_env();
    const Blend bl = read_blend();

    /* THE ENABLED BGs IN PAINT ORDER, once a frame (lane TWOD). The loop below
       used to test all sixteen (priority, BG) pairs at every pixel, far to near,
       and skip the ones not enabled, not at that priority or not in the layer
       filter; this list is those same tests answered once, in the same order,
       so each pixel visits only the layers that can draw. A frame with none
       (every level's HUD is sprites) skips the loop. The window test and
       everything after it are per pixel as before. bg_loop_ref is the old loop
       (SM64DS_TWOD_VERIFY=1 compares the two, SM64DS_TWOD_OLD=1 runs it alone). */
    int bgorder[4], nbg = 0;
    for (int prio = 3; prio >= 0; --prio)
        for (int bg = 3; bg >= 0; --bg)
            if (bgs[bg].enabled && bgs[bg].priority == prio && (lmask & (1u << bg)))
                bgorder[nbg++] = bg;
    const bool verify_a = twod_verify_a_begin();
    if (twod_old_a_on()) {
        bg_loop_ref(g_a, bgs, win, lmask, bl);
    } else if (nbg) {
        static uint32_t rcol[4][256];
        static uint8_t rhit[4][256];
        const bool rowfast = !row_fast_off();
        for (int y = 0; y < 192; ++y) {
            bool rf[4] = {false, false, false, false};
            if (rowfast)
                for (int k = 0; k < nbg; ++k) {
                    const int bg = bgorder[k];
                    if (!bgs[bg].bmp) { sample_bg_row(bgs[bg], y, rcol[bg], rhit[bg]); rf[bg] = true; }
                }
            for (int x = 0; x < 256; ++x) {
                const unsigned mask = window_mask(win, x, y);
                for (int k = 0; k < nbg; ++k) {
                    const int bg = bgorder[k];
                    if (!(mask & (1u << bg))) continue;
                    uint32_t s;
                    if (rf[bg] ? (rhit[bg][x] ? (s = rcol[bg][x], true) : false) : sample_bg(bgs[bg], x, y, s)) {
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
                        g_a[y][x].prio = (uint8_t)bgs[bg].priority;
                    }
                }
            }
        }
    }
    if (verify_a) twod_verify_a_end(bgs, win, lmask, bl, dispcnt);

    if (obj_on && (lmask & (1u << kOwnerObj)))
        raster_obj(dispcnt, bl, win, fb);

    g_cz.mark(3);
    enga_layer_px();
    enga_strip_probe(dispcnt);

    if (msg_composite_debug())
        attrib_take();

    // One-shot diagnostic (SM64DS_MSG_COMPOSITE_DEBUG): the engine-A 2D state
    // the frame the compositor first finds any 2D enabled, so a headless run
    // can confirm the box BG is on and how many pixels it covers.
    if (msg_composite_debug()) {
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
    const int margin = ntr::active_w - ds_col_x0(256);  /* spare width (256; 0 at 4:3) */
    /* THE SPLITS. 96 and 160 are the shipped values, chosen for where this
       game's HUD clusters actually sit, and SM64DS_HUD_BANDS=<l>,<r> moves
       them for one run without touching the build.

       IT EXISTS TO MAKE THE TEAR REPRODUCIBLE ON DEMAND, which is a
       measurement problem and not a feature. The per-element rule prevents a
       straddling element from being cut; proving it prevents anything needs a
       straddling element to exist. Measured on the states this port can
       actually reach, none does: the VS HUD's own clusters land at DS x
       107-149 and 192-256, each wholly inside one band, so the band split and
       the element rule place them identically and a before/after shows
       nothing. That is a real and welcome result -- today's content does not
       exhibit the defect -- but it leaves the checker unproven, and a check
       that has never failed is not evidence.

       Moving a split ONTO a cluster that is on screen produces a genuine
       straddle out of real content, on the shipped binary, at the same state
       base. The band-split control must then tear it and the element rule
       must not, which is the falsifiable pair the sweep needs.

       Unset, this is exactly the two constants it replaces. */
    int band_l = 96, band_r = 160;                 /* source-x band splits */
    static const char *const bands_env = std::getenv("SM64DS_HUD_BANDS");
    if (const char *bands = bands_env) {
        int l = 0, r = 0;
        if (std::sscanf(bands, "%d,%d", &l, &r) == 2
            && l >= 0 && r > l && r <= 256) { band_l = l; band_r = r; }
    }
    /* THE ELEMENT PASS. Labelling is the whole per-element rule (see the
       hudelem banner above): it runs ONCE for the frame, here, after every
       layer has been resolved into g_a and before a single pixel is placed,
       so every pixel of one element reads the same anchor. It is skipped
       entirely at the native aspect -- where margin is 0 and there is nothing
       to anchor -- and skipped when the band-split control is armed, which is
       what lets that control be the OLD code path and not a re-derivation of
       it. */
    /* THE N64 ROW, once per frame: only in a 3D scene shown at its own layout, never under a level-clear lift. */
    g_hud_row_live = false;
    g_hud_timer_live = false;
    /* HDMETER1: take this frame's meter note (the ROM drew the meter through the adventure HUD arm) and clear it */
    const bool hdm_note = g_port_hdmeter_live != 0;
    const int hdm_y = g_port_hdmeter_y, hdm_health = g_port_hdmeter_health;
    g_port_hdmeter_live = 0;
    /* SLICE1: the N64 row's meter shift below moves every OBJ cell of the meter's DS rectangle (x 80..153, y < 96). Where the ROM draws no
       meter (the cannon scope, a cutscene) that rectangle holds other sprites, the cannon's aim reticle among them, and the shift cut the
       reticle in two and slid its upper half 5 pixels left. Only a frame whose HUD arm drew the meter is shifted, and only the rows the meter (at its own y) can reach. */
    const bool meter_drawn = g_port_meter_drawn != 0;
    g_port_meter_drawn = 0;
    bool hdm_n64 = false;
    g_hdm.on = false;
#ifndef PORT_ROM_CLEAN      /* env-only proof arm: not in the shipped build (g_hud_erase stays false) */
    { static const bool e = std::getenv("SM64DS_HUD_ERASE_PROOF") != nullptr; g_hud_erase = e; }
#endif
    /* HUDMENU2 (H11): the life counter's and the star counter's DS boxes while the ROM slides them in (state 3), taken before anything moves
       them; the note after the element pass turns them into host rectangles for the pictures between two ticks. */
    g_slide_n = 0;
    if (data_ov002_02111178 == 3 && bg0_3d_shown(dispcnt) && !ntr::present_native() && !lc_ov.live && !lc_ov.top_live) {
        for (int c = 0; c < 2; ++c) {
            const int xa = c ? 180 : 0, xb = c ? 255 : 110;
            int bx0 = 999, bx1 = -1, by0 = 999, by1 = -1;
            for (int y = 0; y < 19; ++y)
                for (int x = xa; x <= xb; ++x) {
                    const Cell &g = g_a[y][x];
                    if (!g.hit || g.owner != kOwnerObj || g.msgcur) continue;
                    if (x < bx0) bx0 = x; if (x > bx1) bx1 = x; if (y < by0) by0 = y; if (y > by1) by1 = y;
                }
            const int d = c ? g_port_hud_dsx : g_port_hud_dlx;
            if (bx1 >= 0 && d != 0) {
                g_slide_ds[g_slide_n][0] = bx0; g_slide_ds[g_slide_n][1] = by0; g_slide_ds[g_slide_n][2] = bx1; g_slide_ds[g_slide_n][3] = by1;
                g_slide_d[g_slide_n] = d;
                ++g_slide_n;
            }
        }
    }
#ifndef PORT_ROM_CLEAN
    {   /* HUDMENU2 proof arm: SM64DS_HUD_OAMDUMP=<n> lists engine A's placed OAM entries for the first n frames (every frame: the last n of them) */
        static int n = -1, cnt = 0;
        if (n < 0) { const char *e = std::getenv("SM64DS_HUD_OAMDUMP"); n = e ? std::atoi(e) : 0; }
        if (n > 0 && ++cnt % n == 0) {
            std::fprintf(stderr, "[oamdump] frame %d\n", cnt);
            for (int i = 0; i < 128; ++i) {
                const uint16_t a0 = rd16(kOamBase + i * 8), a1 = rd16(kOamBase + i * 8 + 2), a2 = rd16(kOamBase + i * 8 + 4);
                if ((a0 & 0x300) == 0x200) continue;   /* disabled */
                const int y = a0 & 0xff, x = a1 & 0x1ff;
                if (y >= 192 && y < 240) continue;
                std::fprintf(stderr, "[oamdump]  #%d x=%d y=%d shape=%d size=%d mode=%d tile=%d pal=%d prio=%d\n", i, x >= 256 ? x - 512 : x, y >= 128 ? y - 256 : y,
                             (a0 >> 14) & 3, (a1 >> 14) & 3, (a0 >> 10) & 3, a2 & 0x3ff, (a2 >> 12) & 15, (a2 >> 10) & 3);
            }
        }
    }
#endif
    if (host_setting_hud_layout() == 1 && bg0_3d_shown(dispcnt) && !ntr::present_native()
        && !lc_ov.live && !lc_ov.top_live) {
        g_hud_timer_live = data_ov002_02111184 != 0;
        hdm_n64 = true;
        /* HUDTWEAK1: the power meter's cells (OBJ, x 80..153, y 0..59) go kHudMeterLeft pixels left, in the plane, before anything
           measures them, so the coin block's gap to the meter is the one it had. */
        const int meter_y1 = hdm_y + 38 < kHudMeterY1 ? hdm_y + 38 : kHudMeterY1;   /* SLICE1: the meter and its label end 38 rows under the ROM's meter y (the hidden meter sits at y -24) */
        for (int y = 0; meter_drawn && y < meter_y1; ++y)
            for (int x = kHudMeterX0; x <= kHudMeterX1; ++x) {
                Cell &c = g_a[y][x];
                if (c.hit && c.owner == kOwnerObj && !c.msgcur) {
                    g_a[y][x - kHudMeterLeft] = c;
                    c.hit = false;
                }
            }
        /* THE RACE TIMER (src/_ZN3HUD15RenderTimeTimerEv.cpp: x 0xa4..0xf0, y 0x16 / 0x1e) shares these rows, and its
           pixels are not the coin counter's. The counter itself never reaches left of x 191 (icon of a three digit
           count), so any sprite pixel at x 160..187 in the zone rows is the timer: leave the DS placement alone then. */
        bool timer_up = false;
        for (int y = kHudZoneY0; y <= kHudZoneY1 && !timer_up; ++y)
            for (int x = 160; x < kHudZoneX0; ++x)
                if (g_a[y][x].hit && g_a[y][x].owner == kOwnerObj) { timer_up = true; break; }
        if (!timer_up) {
        /* THE STAR COUNTER'S LEFT EDGE. The ROM blinks the counter on a star collect (src/_ZN3HUD15RenderStarCountEv.cpp,
           data_020a0db0 & 0x18) so a frame can show it blank or shifted: keep the smallest left edge seen (a count only grows
           within a level) and let a larger one replace it after 120 frames of agreeing, so the coin block never hops. */
        /* Proof arm (SM64DS_HUD_BLINK_SIM=1): blank the star counter for 8 frames in 16, the way the ROM's collect blink does,
           so a strip can show the coin block does not hop. Never set in play. */
#ifndef PORT_ROM_CLEAN
        static const bool blink_sim = std::getenv("SM64DS_HUD_BLINK_SIM") != nullptr;
#else
        static const bool blink_sim = false;
#endif
        static unsigned blink_n = 0;
        if (blink_sim && ((blink_n++ >> 3) & 1))
            for (int y = 0; y < kHudZoneY0 - 1; ++y)
                for (int x = 180; x < 256; ++x)
                    if (g_a[y][x].hit && g_a[y][x].owner == kOwnerObj) g_a[y][x].hit = false;
        int star_left = 256;
        /* HUDMENU2 (H1 / H9): the star counter is ONE piece (icon, x, digits, gaps of a few pixels), so its left edge is walked from its right
           end leftwards and stops at the first run of kStarBridge empty columns. Any other sprite pixel of rows 0..18 further left (a stray
           glyph; the tester's picture shows one beside the coin counter) is not the star counter and must not pull the coin block left
           of where the stars really start. SM64DS_STAR_BRIDGE=0 puts the old first-pixel scan back. */
        {
            static int bridge = -1;
            if (bridge < 0) { const char *e = std::getenv("SM64DS_STAR_BRIDGE"); bridge = e ? std::atoi(e) : 14; }
            int right = -1;
            for (int y = 0; y < kHudZoneY0 - 1; ++y)
                for (int x = 255; x > right && x >= 180; --x)
                    if (g_a[y][x].hit && g_a[y][x].owner == kOwnerObj) { right = x; break; }
            if (right >= 0 && bridge > 0) {
                int x = right, gap = 0;
                for (; x >= 180 && gap <= bridge; --x) {
                    bool any = false;
                    for (int y = 0; y < kHudZoneY0 - 1 && !any; ++y) any = g_a[y][x].hit && g_a[y][x].owner == kOwnerObj;
                    if (any) { star_left = x; gap = 0; } else ++gap;
                }
            } else {
                for (int y = 0; y < kHudZoneY0 - 1; ++y)
                    for (int x = 180; x < star_left; ++x)
                        if (g_a[y][x].hit && g_a[y][x].owner == kOwnerObj) { star_left = x; break; }
            }
        }
        const int star_raw = star_left;
        static int s_star_left = 256, s_star_larger = 0, s_star_right = -1;
        if (star_left < 256) {
            if (star_left <= s_star_left) { s_star_left = star_left; s_star_larger = 0; }
            else if (++s_star_larger >= 120) { s_star_left = star_left; s_star_larger = 0; s_star_right = -1; }
        }
        star_left = s_star_left;
        /* THE POWER METER's rightmost OBJ pixel in the rows the coin block lands on (x 80..187), and the coin block's own
           left edge before the move. The block keeps kHudMeterGap clear pixels from the meter when the star counter allows. */
        int meter_right = -1, blk_left = 256;
        for (int y = kHudZoneY0 - kHudCoinDy; y <= kHudZoneY1 - kHudCoinDy; ++y)
            for (int x = 187; x >= 80; --x)
                if (g_a[y][x].hit && g_a[y][x].owner == kOwnerObj) { if (x > meter_right) meter_right = x; break; }
        for (int y = kHudZoneY0; y <= kHudZoneY1; ++y)
            for (int x = kHudZoneX0; x < blk_left; ++x)
                if (g_a[y][x].hit && g_a[y][x].owner == kOwnerObj && !g_a[y][x].msgcur) { blk_left = x; break; }
        int blk_right = -1;
        for (int y = kHudZoneY0; y <= kHudZoneY1; ++y)
            for (int x = 255; x > blk_right; --x)
                if (g_a[y][x].hit && g_a[y][x].owner == kOwnerObj && !g_a[y][x].msgcur) { blk_right = x; break; }
        /* THE STAR COUNTER'S RIGHT EDGE (the largest seen, reset when the left edge is replaced). */
        int star_right = -1;
        for (int y = 0; y < kHudZoneY0 - 1; ++y)
            for (int x = 255; x > star_right; --x)
                if (g_a[y][x].hit && g_a[y][x].owner == kOwnerObj) { star_right = x; break; }
        if (star_right > s_star_right) s_star_right = star_right;
        star_right = s_star_right;
        /* The block goes as far right as the DS plane lets it: its right end star_gap pixels short of the star counter.
           It never goes to a second line. At 4:3 the DS plane is the screen, so if the meter is closer than the wanted gap, make room in the smallest steps:
             1. squeeze the star gap from kHudStarGap toward kHudStarGapMin;
             2. shrink the meter gap from kHudMeterGap toward kHudMeterGapMin (at 4:3 only; wide, the meter is on the
                centre anchor and the block on the right, so the screen gap is larger than the plane gap);
             3. at 4:3, slide the star counter (and the block with it) right, up to the plane edge less kHudEdgeGap.
           Wide, the meter sits on the centre anchor and the block on the right one, so the screen gap is always larger than the
           plane gap and the meter test is skipped. If even that leaves the meter touched, nothing moves and the numbers are printed (SM64DS_HUD_DBG). */
        int dx_move = 0, star_gap = kHudStarGap, meter_gap = kHudMeterGap, star_shift = 0;
        const bool wide = ntr::widescreen;
        bool fits = blk_right >= 0;
        g_hud_mid = false;
        if (fits && wide) {
            /* HUDMENU2 (H10, "Coins in the middle"): in a wide picture the block takes the CENTRE anchor, the power meter's own, with its left edge
               kHudMidX in the plane: right of the meter, left of the stars, the SM64 row (lives left, coins middle, stars right) whatever
               the star count. At 4:3 there is no spare width, so the squeeze below (next to the stars) stays. */
            int d = blk_left - kHudMidX;
            if (d < kHudCoinDxMin) d = kHudCoinDxMin;
            dx_move = d; g_hud_mid = true;
        } else
        if (fits) {
            const int star_room = (wide || star_right < 0 || star_left >= 256) ? 0 : (255 - kHudEdgeGap - star_right);
            bool ok = false;
            for (int sh = 0; sh <= (star_room > 0 ? star_room : 0) && !ok; ++sh)
                for (int mg = kHudMeterGap; mg >= (wide ? 1 : kHudMeterGapMin) && !ok; --mg) {
                    if (sh > 0 && mg != kHudMeterGapMin) continue;
                    for (int sg = kHudStarGap; sg >= kHudStarGapMin && !ok; --sg) {
                        if (sh > 0 && sg != kHudStarGapMin) continue;
                        int d = blk_right - (star_left + sh - sg);
                        if (d < kHudCoinDxMin) d = kHudCoinDxMin;
                        if (wide || meter_right < 0 || blk_left - d >= meter_right + 1 + mg) {
                            ok = true; dx_move = d; star_gap = sg; meter_gap = mg; star_shift = sh;
                        }
                    }
                }
            fits = ok;
        }
#ifndef PORT_ROM_CLEAN
        static const bool dbg = std::getenv("SM64DS_HUD_DBG") != nullptr;
#else
        static const bool dbg = false;
#endif
        if (dbg) std::fprintf(stderr, "[hudrow] star_raw=%d star_left=%d star_right=%d meter_right=%d blk=%d..%d dx=%d star_gap=%d meter_gap=%d star_shift=%d fits=%d\n",
                              star_raw, star_left, star_right, meter_right, blk_left, blk_right, dx_move, star_gap, meter_gap, star_shift, (int)fits);
        if (fits && star_shift > 0) {
            /* slide the star counter right by star_shift DS pixels (same cells, still on the right anchor) */
            for (int y = 0; y < kHudZoneY0 - 1; ++y)
                for (int x = 255 - star_shift; x >= 180; --x) {
                    Cell &c = g_a[y][x];
                    if (c.hit && c.owner == kOwnerObj) { g_a[y][x + star_shift] = c; c.hit = false; }
                }
        }
        if (fits) {
        static Cell tmp[kHudZoneY1 - kHudZoneY0 + 1][256 - kHudZoneX0];
        for (int y = kHudZoneY0; y <= kHudZoneY1; ++y)
            for (int x = kHudZoneX0; x < 256; ++x) {
                Cell &c = g_a[y][x];
                const bool mv = c.hit && c.owner == kOwnerObj && !c.msgcur;
                tmp[y - kHudZoneY0][x - kHudZoneX0] = c;
                tmp[y - kHudZoneY0][x - kHudZoneX0].hit = mv;
                if (mv) c.hit = false;
            }
        std::memset(g_hud_row, 0, sizeof(g_hud_row));
        int hud_erased = 0;
        static uint8_t mask_buf[192 * 256];
        uint8_t *const mask_out = (dbg && std::getenv("SM64DS_HUD_MASK_OUT")) ? (std::memset(mask_buf, 0, sizeof(mask_buf)), mask_buf) : nullptr;
        for (int y = kHudZoneY0; y <= kHudZoneY1; ++y)
            for (int x = kHudZoneX0; x < 256; ++x) {
                const Cell &c = tmp[y - kHudZoneY0][x - kHudZoneX0];
                if (!c.hit) continue;
                g_hud_cells[y - kHudCoinDy][x - dx_move] = c;   /* never g_a: the cell there may be a letter of POWER */
                const bool under = g_a[y - kHudCoinDy][x - dx_move].hit;   /* a live DS plane cell (a letter of POWER) sits where this moved cell lands */
                if (under) ++hud_erased;
                if (mask_out) mask_out[(y - kHudCoinDy) * 256 + (x - dx_move)] = under ? 2 : 1;
                if (g_hud_erase) g_a[y - kHudCoinDy][x - dx_move] = c;   /* PROOF ARM (env only): the pre-fix behaviour, the moved cell REPLACES the plane cell under it */
                g_hud_row[y - kHudCoinDy][x - dx_move] = 1;
                g_hud_row_live = true;
            }
        if (mask_out) {   /* debug: the moved block's plane mask, 1 = coin cell, 2 = coin cell landing on a live plane cell; P5, 256x192 */
            if (FILE *mf = std::fopen(std::getenv("SM64DS_HUD_MASK_OUT"), "wb")) {
                std::fputs("P5 256 192 255 ", mf); std::fwrite(mask_buf, 1, sizeof(mask_buf), mf); std::fclose(mf);
            }
        }
        if (g_hud_erase || dbg) std::fprintf(stderr, "[hudrow] moved cells landing on a live DS plane cell: %d (proof arm %d)\n", hud_erased, (int)g_hud_erase);
        }
        }
    }
    if (g_hud_timer_live) {
        /* HUDTWEAK1: the timer's cells go up kHudCoinDy rows, into the row the coin counter left, after the coin step (above) so
           they are not taken for coin pixels. */
        for (int y = kHudTimerY0; y < kHudTimerY1; ++y)
            for (int x = kHudTimerX0; x < kHudTimerX1; ++x) {
                Cell &c = g_a[y][x];
                if (c.hit && c.owner == kOwnerObj && !c.msgcur) {
                    g_a[y - kHudCoinDy][x] = c;
                    c.hit = false;
                }
            }
    }
    const bool bandsplit = hud_bandsplit_env();
    if (ntr::widescreen && !bandsplit) hudelem::resolve(band_l, band_r, margin);
    /* HUDMENU2 (H11): the slide note's host rectangles, with the anchor each counter really sits on in this picture */
    g_note.n = 0;
    if (g_slide_n && !bandsplit) {
        const int sxs = ntr::active_w / 256;
        for (int i = 0; i < g_slide_n && i < 2; ++i) {
            const int x0 = g_slide_ds[i][0], y0 = g_slide_ds[i][1], x1 = g_slide_ds[i][2], y1 = g_slide_ds[i][3];
            int off = 0;
            if (ntr::widescreen) off = hudelem::g_off[hudelem::g_lbl[y0][x0] > 0 ? hudelem::g_lbl[y0][x0] : 0];
            HudSlideRect &r = g_note.r[g_note.n++];
            r.x0 = ntr::widescreen ? ds_col_x0(x0) + off : x0 * sxs;
            r.x1 = ntr::widescreen ? ds_col_x0(x1 + 1) + off : (x1 + 1) * sxs;
            r.y0 = ds_row_y0(y0); r.y1 = ds_row_y0(y1 + 1);
            r.d = (float)g_slide_d[i];
        }
    }
    /* TWO QUESTIONS, KEPT APART SO THE A/B ARM STILL MEASURES. `shown3d` is
       whether the 3D layer is in the picture at all, which decides whether the
       coverage mask means anything; `honour3d` is whether this run obeys its
       priority. The kill switch only clears the second, so the disabled arm
       still counts every 3D pixel it paints over instead of reporting a row of
       zeroes that could be read as "there was no 3D layer". */
    const bool shown3d = bg0_3d_shown(dispcnt);
    const bool honour3d = shown3d && !prio3d_off_env();
    const int p3d = bg0_3d_priority();
    /* A FRAME HELD ON THE GRAPHICS CARD has no coverage plane here: the composite
       asks the card instead, with the verdict written into the pixel's alpha
       (ntr/gx.h, GX_OV_*). Everything below that reads `cover` is then the
       processor-only branch, and the pre-smoothing copy is null as well (the
       capture builds its picture from the card). */
    if (settle_drill() & 1u) ntr::gx_card_settle_because("a test drill, before the blit");
    /* SAVEOVL1: a lifted layer BEHIND the 3D needs the 3D's coverage to know which of its cells show: that tick reads
       the picture back after all, and the old classic loops below take it */
    if (g_lift_card && ntr::gx_card_overlay_active() && honour3d) {
        bool behind_lift = false;
        for (int y = 0; y < 192 && !behind_lift; ++y)
            for (int x = 0; x < 256; ++x) {
                const Cell &gc = g_a[y][x];
                if (!gc.hit) continue;
                const int v = lc_overlay_verdict(gc.owner);
                if ((v == 1 || v == 3) && layer_behind_3d(gc.owner, gc.prio, p3d)) { behind_lift = true; break; }
            }
        if (behind_lift) {
            g_lift_card = false;
            ntr::gx_card_settle_because("a lifted layer sits behind the 3D");
        }
    }
    const bool ovl3d = ntr::gx_card_overlay_active() != 0;
    /* a frame settled after its mask was made (a sprite blended over the 3D)
       is an ordinary one from here: the mask has to be one too */
    if (g_bm_card && !ovl3d) bm_card_to_classic();
    const uint8_t *cover = (shown3d && !ovl3d) ? ntr::gx_coverage() : nullptr;
    /* TWODGPU1: the card places the cells (a settle since the begin, a card
       that stopped or a frame that is no longer held send them down the loops) */
    const bool gpu_blit = tw_took && ovl3d && port_gpu_twod_live();
    /* THE PRE-SMOOTHING COPY OF THIS FRAME, or null when the AntiAliasing
       setting is off -- which is the default and is every run that has not
       asked for it. See ntr/gx.h's gx_aa_preimage: the game reads its own top
       screen back through the DS display capture unit, and that copy is the
       picture the setting promises it. Every host pixel written below goes
       there too, so what the capture reads is a FINISHED frame -- this unit's
       2D layers over the 3D -- that the smoothing never touched. One extra
       store per composited pixel, and only when the setting is on. */
    uint32_t *const pre = ntr::gx_aa_preimage();
    uint32_t kept = 0, buried = 0;
    /* HDMETER1, step 1: erase the DS meter's cells and work out where the art goes (after the element pass, so the anchor is known) */
    if (hdm_note && port_hd_meter_art_hook && shown3d && !ntr::present_native() && !lc_ov.live && !lc_ov.top_live && hdm_health >= 0 && hdm_health <= 8) {
#ifndef PORT_ROM_CLEAN
        static const bool dbg = std::getenv("SM64DS_HD_METER_DBG") != nullptr;
        { static bool read; if (!read) { read = true; if (const char *e = std::getenv("SM64DS_HD_METER_DX")) kHdMeterX0 = std::atoi(e); if (const char *e = std::getenv("SM64DS_HD_METER_DY")) kHdMeterDy = std::atoi(e); } }
#else
        static const bool dbg = false;
#endif
        const int X0 = kHdMeterX0 - (hdm_n64 ? kHudMeterLeft : 0), Y0 = hdm_y + kHdMeterDy;
        const int k = (ntr::present_h() / 192 >= 2) ? 4 : 1;
        const HdMeterArt *art = port_hd_meter_art_hook(hdm_health, k);
        if (art) {
            int n = 0, outside = 0, prio = -1, fx = -1, fy = -1;
            int bx0 = 999, bx1 = -1, by0 = 999, by1 = -1;
            for (int y = 0; y < 192 && y < Y0 + 48; ++y)
                for (int x = (X0 - 6 < 0 ? 0 : X0 - 6); x < X0 + 46 && x < 256; ++x) {
                    const Cell &c = g_a[y][x];
                    if (!c.hit || c.owner != kOwnerObj || c.msgcur) continue;
                    if (x >= X0 && x < X0 + 40 && y >= Y0 && y < Y0 + 40) {
                        if (n++ == 0) { prio = c.prio; fx = x; fy = y; }
                        if (x < bx0) bx0 = x; if (x > bx1) bx1 = x; if (y < by0) by0 = y; if (y > by1) by1 = y;
                    } else ++outside;
                }
            if (dbg) std::fprintf(stderr, "[hdmeter] y=%d health=%d n64=%d X0=%d Y0=%d cells in footprint %d (bbox %d..%d x %d..%d) outside %d k=%d\n",
                                  hdm_y, hdm_health, (int)hdm_n64, X0, Y0, n, bx0, bx1, by0, by1, outside, art->k);
            if (n > 0 && (!honour3d || !layer_behind_3d(kOwnerObj, prio, p3d))) {
                for (int y = Y0 < 0 ? 0 : Y0; y < Y0 + 40 && y < 192; ++y)
                    for (int x = X0; x < X0 + 40; ++x) {
                        Cell &c = g_a[y][x];
                        if (c.hit && c.owner == kOwnerObj && !c.msgcur) c.hit = false;
                    }
                g_hdm.on = true; g_hdm.x0 = X0; g_hdm.y0 = Y0; g_hdm.k = art->k; g_hdm.art = art;
                g_hdm.off = ntr::widescreen ? (bandsplit ? margin / 2 : hudelem::g_off[hudelem::g_lbl[fy][fx]]) : 0;
            }
        }
    }
    g_cz.mark(4);
    /* MGINTERP1: a replay with the tick's finished picture to start from (port_message_engine_a_replay_dirty) redraws only the
       DS cells whose resolved pixel differs from the tick's: the picture as it stands before this blit is kept aside (a cell
       that is no longer drawn takes it back), the tick's finished picture is put in, and the cells the tick drew identically
       are left as they are. Native presentation only, where a cell's place does not depend on what is in it. */
    const bool mc_dirty = g_mc_replay && g_mc_dirty_final && ntr::present_native();
    if (!g_mc_replay) std::memcpy(g_a_tick, g_a, sizeof g_a_tick);
    if (mc_dirty) {
        if (!g_mc_pre_blit) g_mc_pre_blit = static_cast<uint32_t *>(std::malloc(sizeof(uint32_t) * (size_t)ntr::SCREEN_W * ntr::SCREEN_H));
        for (int y = 0; g_mc_pre_blit && y < ntr::active_h; ++y) {
            std::memcpy(g_mc_pre_blit + (size_t)y * ntr::SCREEN_W, fb.px[y], (size_t)ntr::active_w * 4);
            std::memcpy(fb.px[y], g_mc_dirty_final + (size_t)y * ntr::SCREEN_W, (size_t)ntr::active_w * 4);
        }
    }
    /* MGINTERP1: the host column and row of every DS column and row, once a frame, instead of two cross-file calls and a
       divide for each of them per cell (the same numbers: ds_col_x0 / ds_row_y0) */
    int mc_col_x0[257], mc_row_y0[193];
    const bool mc_native = ntr::present_native();
    for (int i = 0; i <= 256; ++i) mc_col_x0[i] = ds_col_x0(i);
    for (int i = 0; i <= 192; ++i) mc_row_y0[i] = ds_row_y0(i);
    for (int pass = 0; pass < 2; ++pass) {
    if (pass && !g_hud_row_live) break;
    /* pass 0 = the DS plane, pass 1 = the moved coin block's own plane (drawn after, so it covers what is under it) */
    for (int y = 0; y < 192; ++y) {
        for (int x = 0; x < 256; ++x) {
            const Cell &gc = pass ? g_hud_cells[y][x] : g_a[y][x];
            if (mc_dirty && !pass) {
                const Cell &tc = g_a_tick[y][x];
                if (!gc.hit && !tc.hit) continue;
                if (gc.hit && tc.hit && std::memcmp(&gc, &tc, sizeof gc) == 0) continue;
                if (!gc.hit) {
                    /* drawn by the tick, not now: the picture as it was before this blit */
                    const int hx0 = ntr::widescreen ? mc_col_x0[x] + margin / 2 : x * sx;
                    const int bw = ntr::widescreen ? mc_col_x0[x + 1] - mc_col_x0[x] : sx;
                    for (int hy = mc_row_y0[y]; g_mc_pre_blit && hy < mc_row_y0[y + 1]; ++hy)
                        std::memcpy(&fb.px[hy][hx0], g_mc_pre_blit + (size_t)hy * ntr::SCREEN_W + hx0, (size_t)bw * 4);
                    continue;
                }
            }
            if (!gc.hit) continue;
            if (pass && !g_hud_row[y][x]) continue;   /* g_hud_cells is not cleared per frame: only this frame's moved cells */
            if (!pass && g_hud_erase && g_hud_row_live && g_hud_row[y][x]) continue;
            const uint32_t c = gc.color;
            /* horizontal placement of this DS column's block. In 4:3 (toggle
               off) ntr::widescreen is false and this is the plain x*sx block; in
               16:9 it reanchors. The wide arm also self-normalises -- margin is 0
               and uni == sx at 4:3 -- but the runtime branch keeps the off path
               byte-for-byte the old code. */
            int bw, hx0;
            if (ntr::widescreen) {
            const int ux0 = mc_col_x0[x];
            bw = mc_col_x0[x + 1] - ux0;
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
            /* THE PLACEMENT, in precedence order.
               BG3 first: the message layer is centred at the native scale
               whatever else is on screen, which is the rule that predates the
               element pass and the reason BG3 is held out of its mask. The
               box's cursor sprites go with it (msg_layer_px): the game puts them
               in the box's own coordinates, so they move as the box moves.
               Then a full-2D top screen, which is pillarboxed whole.
               Otherwise the pixel takes ITS OWN ELEMENT's anchor, so an
               element that straddles a split moves in one piece instead of
               being cut at the split. With SM64DS_HUD_BANDSPLIT armed the
               last arm is the original per-column band split instead, which
               is the control the before/after images are taken against. */
            hx0 = msg_layer_px(y, x)
                          ? ux0 + margin / 2
                          /* A SCENE PRESENTED NATIVELY TAKES THE PILLARBOX ARM
                             whether or not a 3D layer is behind it. Tango's
                             ruling of 2026-09-17: a minigame presents exactly
                             as it does at 4:3, so its HUD keeps its native
                             positions, centred, at the uniform scale -- the
                             arm the full-2D minigame boards already took by
                             way of !shown3d. margin / 2 is ntr::present_x(),
                             and it is the bottom panel's pan_x0 too, so all
                             three layers line up. */
                          : (mc_native || !shown3d)
                          ? ux0 + margin / 2
                          : pass
                          ? (g_hud_mid ? ux0 + margin / 2 : ux0 + margin)   /* the moved coin block rides the right edge with the star counter; HUDMENU2: in a wide picture the centre anchor, beside the meter */
                          : !bandsplit
                          ? ux0 + hudelem::g_off[hudelem::g_lbl[y][x]]
                          : (x < band_l) ? ux0
                          : (x >= band_r) ? ux0 + margin
                                          : ux0 + margin / 2;
            } else {
            bw = sx;
            hx0 = x * sx;
            }
            /* THE LEVEL-CLEAR COMPOSE'S LAYER SPLIT, and it is one read of a
               zero on every frame of every ordinary run (see the LcOverlay
               banner). Verdict 1 sends this pixel to the overlay INSTEAD of
               the framebuffer wherever it falls inside its layer's own band,
               so the picture keeps what engine A's 3D path drew under it;
               verdict 2 drops it. The 3D priority, the placement and
               the pre-smoothing copy are all still this loop's own answers --
               nothing here re-decides where a pixel goes, only which buffer it
               lands in, so the lifted glyphs are exactly the pixels the player
               would have seen at these rows. The capture copy keeps them at
               their own rows either way: the display capture unit is the ROM
               reading its own screen back and it is not part of this mod. */
            /* THE HOST ROWS OF THIS DS ROW: the same rows the 3D layer's
               viewport and the window test use (ds_row_y0), so a DS row is
               one screen row in every layer whether or not active_h is a
               whole multiple of 192. */
            const int hy0 = mc_row_y0[y], hy1 = mc_row_y0[y + 1];
            const unsigned lcow = gc.owner;
            const int lcv = lc_overlay_verdict(lcow);
            if (lcv == 2) continue;
            /* this 2D layer's brightness verdict, for every host pixel it
               puts on screen (ENGINE A'S BRIGHTNESS TARGETS) */
            const unsigned char bmv =
                bm_on ? bm_verdict(lcow, window_mask(win, x, y)) : 0;
            /* SAVEOVL1: THE LIFT ON A CARD TICK. The lifted layers are in front of the 3D (checked above), so a lifted cell
               is never an overlay word: it goes to its plane exactly as the classic loop below puts it there. A cell of
               a banded layer is split by host row, the rows inside the band to the plane, the others drawn as ever. */
            if (ovl3d && lcv == 3) {
                MenuTopCell &tc = lc_ov.top[y * 256 + x];
                tc.argb = c | 0xff000000u;
                tc.bm = bm_on ? bmv : 1;
                tc.owner = (uint8_t)lcow;
                continue;
            }
            if (ovl3d && lcv == 1) {
                int run0 = -1;
                for (int hy = hy0; hy <= hy1; ++hy) {
                    const bool inb = hy < hy1 && lc_overlay_in_band(lcow, hy);
                    if (hy < hy1 && inb)
                        for (int dx = 0; dx < bw; ++dx) lc_ov.col[(size_t)hy * ntr::SCREEN_W + hx0 + dx] = c | 0xff000000u;
                    if (hy < hy1 && !inb) { if (run0 < 0) run0 = hy; continue; }
                    if (run0 >= 0) {
                        /* rows run0..hy-1 of this cell stay in the picture */
                        if (gpu_blit && g_tw_n < 256 * 192 && hx0 >= 0 && bw > 0) {
                            uint32_t *r = g_tw_inst + (size_t)g_tw_n * 4;
                            r[0] = (uint32_t)hx0 | ((uint32_t)run0 << 16);
                            r[1] = (uint32_t)bw | ((uint32_t)(hy - run0) << 16);
                            r[2] = (c & 0x00FFFFFFu) | (GX_OV_FRONT << 24);
                            r[3] = bmv ? 1u : 0u;
                            ++g_tw_n;
                        }
                        if (!gpu_blit || tw_chk) {
                            for (int ry = run0; ry < hy; ++ry)
                                for (int dx = 0; dx < bw; ++dx) {
                                    const int hx = hx0 + dx;
                                    fb.px[ry][hx] = (c & 0x00FFFFFFu) | (GX_OV_FRONT << 24);
                                    if (bm_on) {
                                        unsigned char &m = g_bm[(size_t)ry * ntr::SCREEN_W + hx];
                                        m = (unsigned char)((m & 3u) | (bmv ? 4u : 0u) | 8u);
                                    }
                                }
                        }
                        run0 = -1;
                    }
                }
                continue;
            }
            if (gpu_blit && g_tw_n < 256 * 192) {
                /* the cell as one rect for the card: this loop's own placement
                   (hx0, bw, the host rows of the DS row), the verdict the
                   branches below would write, the mask bit */
                const bool front = !honour3d
                    || !layer_behind_3d(gc.owner, gc.prio, p3d);
                if (!front) ntr::gx_card_note_behind();
                if (hx0 >= 0 && bw > 0 && hy1 > hy0) {
                    uint32_t *r = g_tw_inst + (size_t)g_tw_n * 4;
                    r[0] = (uint32_t)hx0 | ((uint32_t)hy0 << 16);
                    r[1] = (uint32_t)bw | ((uint32_t)(hy1 - hy0) << 16);
                    r[2] = (c & 0x00FFFFFFu) | ((front ? GX_OV_FRONT : GX_OV_BEHIND) << 24);
                    r[3] = bmv ? 1u : 0u;
                    ++g_tw_n;
                }
                if (!tw_chk) continue;
            }
            if (lcv == 3) {
                /* LIFTED INTO THE DS-SPACE PLANE: not written to the picture at all. A cell the 3D layer covers (when
                   the layer is behind it) is not lifted. The pre-smoothing copy still gets it. */
                const bool behind = honour3d && layer_behind_3d(gc.owner, gc.prio, p3d);
                if (!(behind && cover[hy0 * ntr::SCREEN_W + hx0])) {
                    MenuTopCell &tc = lc_ov.top[y * 256 + x];
                    tc.argb = c | 0xff000000u;
                    tc.bm = bm_on ? bmv : 1;      /* 1 = the ROM's fade/brightness applies to this pixel */
                    tc.owner = (uint8_t)lcow;
                }
                if (pre)
                    for (int hy = hy0; hy < hy1; ++hy)
                        for (int dx = 0; dx < bw; ++dx)
                            pre[(size_t)hy * ntr::SCREEN_W + hx0 + dx] = c;
                continue;
            }
            if (!honour3d
                || !layer_behind_3d(gc.owner, gc.prio, p3d)) {
                /* the owning 2D layer is in FRONT of the 3D layer (or there is
                   no 3D layer in the picture): the old unconditional write */
                if (!ovl3d && !lcv && !bm_on) {
                    /* MGINTERP1: the plain case (no card overlay, no level-clear lift, no brightness mask) as row fills; the
                       same writes and the same count as the loop below, without its per-pixel tests */
                    for (int hy = hy0; hy < hy1; ++hy) {
                        uint32_t *row = &fb.px[hy][hx0];
                        for (int dx = 0; dx < bw; ++dx) row[dx] = c;
                        if (cover) {
                            const uint8_t *cv = cover + (size_t)hy * ntr::SCREEN_W + hx0;
                            for (int dx = 0; dx < bw; ++dx) buried += cv[dx] ? 1 : 0;
                        }
                        if (pre) {
                            uint32_t *pr = pre + (size_t)hy * ntr::SCREEN_W + hx0;
                            for (int dx = 0; dx < bw; ++dx) pr[dx] = c;
                        }
                    }
                    continue;
                }
                for (int hy = hy0; hy < hy1; ++hy)
                    for (int dx = 0; dx < bw; ++dx) {
                        const int hx = hx0 + dx;
                        if (ovl3d) {
                            fb.px[hy][hx] = (c & 0x00FFFFFFu) | (GX_OV_FRONT << 24);
                            if (bm_on) {
                                unsigned char &m = g_bm[(size_t)hy * ntr::SCREEN_W + hx];
                                m = (unsigned char)((m & 3u) | (bmv ? 4u : 0u) | 8u);
                            }
                            continue;
                        }
                        if (cover && cover[hy * ntr::SCREEN_W + hx]) ++buried;
                        if (lcv && lc_overlay_in_band(lcow, hy)) {
                            lc_ov.col[(size_t)hy * ntr::SCREEN_W + hx] = c | 0xff000000u;
                        } else {
                            fb.px[hy][hx] = c;
                            if (bm_on)
                                g_bm[(size_t)hy * ntr::SCREEN_W + hx] = bmv;
                        }
                        if (pre) pre[(size_t)hy * ntr::SCREEN_W + hx] = c;
                    }
                continue;
            }
            /* BEHIND the 3D layer: it may only fill where 3D drew nothing */
            if (ovl3d) ntr::gx_card_note_behind();
            for (int hy = hy0; hy < hy1; ++hy)
                for (int dx = 0; dx < bw; ++dx) {
                    const int hx = hx0 + dx;
                    if (ovl3d) {
                        fb.px[hy][hx] = (c & 0x00FFFFFFu) | (GX_OV_BEHIND << 24);
                        if (bm_on) {
                            unsigned char &m = g_bm[(size_t)hy * ntr::SCREEN_W + hx];
                            m = (unsigned char)((m & 3u) | (bmv ? 4u : 0u) | 16u);
                        }
                        continue;
                    }
                    if (cover[hy * ntr::SCREEN_W + hx]) { ++kept; continue; }
                    if (lcv && lc_overlay_in_band(lcow, hy)) {
                        lc_ov.col[(size_t)hy * ntr::SCREEN_W + hx] = c | 0xff000000u;
                    } else {
                        fb.px[hy][hx] = c;
                        if (bm_on)
                            g_bm[(size_t)hy * ntr::SCREEN_W + hx] = bmv;
                    }
                    if (pre) pre[(size_t)hy * ntr::SCREEN_W + hx] = c;
                }
        }
    }
    }   /* the HUD pass loop (menu1 HUD1) */
    /* HDMETER1, step 2: the art, at host resolution, over the footprint's DS cells that no other layer holds. Each host pixel
       averages the art pixels it covers (alpha weighted) and is drawn when at least half of them are opaque, so the 1x art lands
       pixel for pixel on the DS grid and the 4x art is box filtered down to whatever the window is. It goes down through the same
       three routes the cell loop uses (the card's rects, the card overlay words, the plain frame). */
    if (g_hdm.on) {
        const HdMeterArt &A = *g_hdm.art;
        const int k = g_hdm.k;
        struct HdPx { uint32_t rgb; uint8_t on, bm; };
        static std::vector<HdPx> buf;
        int ya = g_hdm.y0 < 0 ? 0 : g_hdm.y0, yb = g_hdm.y0 + 40 > 192 ? 192 : g_hdm.y0 + 40;
        int hxa = 1 << 30, hxb = -1;
        for (int xx = 0; xx < 40; ++xx) {
            const int x = g_hdm.x0 + xx;
            if (x < 0 || x > 255) continue;
            const int a = (ntr::widescreen ? ds_col_x0(x) + g_hdm.off : x * sx), b = (ntr::widescreen ? ds_col_x0(x + 1) + g_hdm.off : (x + 1) * sx);
            if (a < hxa) hxa = a;
            if (b > hxb) hxb = b;
        }
        const int hya = ds_row_y0(ya), hyb = ds_row_y0(yb);
        const int W = hxb - hxa, H = hyb - hya;
        if (W > 0 && H > 0 && W <= 4096 && H <= 4096) {
            buf.assign((size_t)W * H, HdPx{0, 0, 0});
            for (int y = ya; y < yb; ++y) {
                const int yy = y - g_hdm.y0;
                const int hy0 = ds_row_y0(y), hy1 = ds_row_y0(y + 1), bh = hy1 - hy0;
                for (int xx = 0; xx < 40; ++xx) {
                    const int x = g_hdm.x0 + xx;
                    if (x < 0 || x > 255 || g_a[y][x].hit) continue;
                    const int hx0 = ntr::widescreen ? ds_col_x0(x) + g_hdm.off : x * sx;
                    const int bw = ntr::widescreen ? ds_col_x0(x + 1) - ds_col_x0(x) : sx;
                    const unsigned char bmv = bm_on ? bm_verdict((unsigned)kOwnerObj, window_mask(win, x, y)) : 0;
                    for (int j = 0; j < bh; ++j) {
                        const int ay0 = yy * k + j * k / bh;
                        int ay1 = yy * k + ((j + 1) * k + bh - 1) / bh;
                        if (ay1 <= ay0) ay1 = ay0 + 1;
                        for (int dx = 0; dx < bw; ++dx) {
                            const int ax0 = xx * k + dx * k / bw;
                            int ax1 = xx * k + ((dx + 1) * k + bw - 1) / bw;
                            if (ax1 <= ax0) ax1 = ax0 + 1;
                            unsigned sa = 0, sr = 0, sg = 0, sb = 0, cnt = 0;
                            for (int ay = ay0; ay < ay1 && ay < A.h; ++ay)
                                for (int ax = ax0; ax < ax1 && ax < A.w; ++ax) {
                                    const unsigned char *q = A.rgba + ((size_t)ay * A.w + ax) * 4;
                                    sa += q[3]; sr += q[0] * q[3]; sg += q[1] * q[3]; sb += q[2] * q[3]; ++cnt;
                                }
                            if (!cnt || sa * 2 < cnt * 255u || !sa) continue;
                            HdPx &px = buf[(size_t)(hy0 + j - hya) * W + (hx0 + dx - hxa)];
                            px.rgb = ((sr / sa) << 16) | ((sg / sa) << 8) | (sb / sa);
                            px.on = 1; px.bm = bmv ? 1 : 0;
                        }
                    }
                }
            }
            for (int ry = 0; ry < H; ++ry) {
                const int hy = hya + ry;
                for (int rx = 0; rx < W;) {
                    const HdPx &p0 = buf[(size_t)ry * W + rx];
                    if (!p0.on) { ++rx; continue; }
                    int rn = 1;
                    while (rx + rn < W) {
                        const HdPx &pn = buf[(size_t)ry * W + rx + rn];
                        if (!pn.on || pn.rgb != p0.rgb || pn.bm != p0.bm) break;
                        ++rn;
                    }
                    const int hx = hxa + rx;
                    if (gpu_blit && g_tw_n < 256 * 192) {
                        uint32_t *r = g_tw_inst + (size_t)g_tw_n * 4;
                        r[0] = (uint32_t)hx | ((uint32_t)hy << 16);
                        r[1] = (uint32_t)rn | (1u << 16);
                        r[2] = (p0.rgb & 0x00FFFFFFu) | (GX_OV_FRONT << 24);
                        r[3] = p0.bm ? 1u : 0u;
                        ++g_tw_n;
                    }
                    if (!gpu_blit || tw_chk) {
                        for (int d = 0; d < rn; ++d) {
                            const int x2 = hx + d;
                            if (ovl3d) {
                                fb.px[hy][x2] = (p0.rgb & 0x00FFFFFFu) | (GX_OV_FRONT << 24);
                                if (bm_on) {
                                    unsigned char &m = g_bm[(size_t)hy * ntr::SCREEN_W + x2];
                                    m = (unsigned char)((m & 3u) | (p0.bm ? 4u : 0u) | 8u);
                                }
                                continue;
                            }
                            if (cover && cover[hy * ntr::SCREEN_W + x2]) ++buried;
                            fb.px[hy][x2] = p0.rgb;
                            if (bm_on) g_bm[(size_t)hy * ntr::SCREEN_W + x2] = p0.bm;
                            if (pre) pre[(size_t)hy * ntr::SCREEN_W + x2] = p0.rgb;
                        }
                    }
                    rx += rn;
                }
            }
        }
        g_hdm.on = false;
    }
    g_cz.mark(5);
    if (gpu_blit) {
        port_gpu_twod_rects(g_tw_inst, g_tw_n);
        if (tw_chk)
            port_gpu_twod_check(&fb.px[0][0], ntr::SCREEN_W, g_bm_live ? g_bm : nullptr,
                                ntr::SCREEN_W, 0);
    }
    /* Pixels the 3D engine drew that NO 2D layer covered at all were never in
       the loop above; they are kept too, and counting them is what makes the
       row comparable with the host framebuffer's own size. A full-screen sweep
       is instrumentation and nothing reads it unless the probe is on, so it is
       gated: an ordinary play frame does not pay for a number nobody prints. */
    if (honour3d && cover && msg_composite_debug()) {
        for (int hy = 0; hy < ntr::active_h; ++hy)
            for (int hx = 0; hx < ntr::active_w; ++hx)
                if (cover[hy * ntr::SCREEN_W + hx]
                    && (ds_row_of(hy) >= 192 || !g_a[ds_row_of(hy)][hx / sx].hit))
                    ++kept;
    }
    g_kept3d = kept;
    g_buried3d = buried;
    g_cz.mark(6);
}

/* ---- WHAT THE MENU COMPOSE READS BACK ---------------------------------------
 *
 * The planes are framebuffer-shaped (host plane: one ARGB entry per host pixel at a stride of ntr::SCREEN_W) or DS
 * shaped (top2d). `live` is this frame's own answer and not a latch: the composite above clears it on every frame no
 * recipe lifts and sets it on every frame one does, above its own early returns, so a draw that asks on a frame this
 * unit declined is told the plane is empty instead of being handed the last frame's glyphs. */
extern "C" const unsigned int *port_menu_hostrow_plane(void)
{
    return lc_ov.live && lc_ov.col ? lc_ov.col : nullptr;
}

extern "C" const MenuTopCell *port_menu_top2d_plane(void)
{
    return lc_ov.top_live && lc_ov.top ? lc_ov.top : nullptr;
}

/* ENGINE A'S FADE FOR ONE LIFTED PIXEL, for a recipe drawn AFTER port_engine_a_fade: the same arithmetic as fade_pass,
   with the verdict the pixel carried out of the compositor (bm 1 = the ROM's brightness applies to it). Prep reads
   the blend state once for the frame. */
static int g_fpx_evy, g_fpx_white;
extern "C" void port_engine_a_fade_px_prep(void)
{
    int evy = 0, white = 0;
    const int on = port_fader_blend_state(&evy, &white);
    if (on && evy > 16) evy = 16;
    g_fpx_evy = on ? evy : 0;
    g_fpx_white = white;
}

extern "C" unsigned int port_engine_a_fade_px(unsigned int p, unsigned char bm)
{
    if (!g_fpx_evy || !bm) return p;
    int r = (p >> 16) & 0xff, g = (p >> 8) & 0xff, b = p & 0xff;
    if (g_fpx_white) {
        r += ((255 - r) * g_fpx_evy) >> 4;
        g += ((255 - g) * g_fpx_evy) >> 4;
        b += ((255 - b) * g_fpx_evy) >> 4;
    } else {
        r -= (r * g_fpx_evy) >> 4;
        g -= (g * g_fpx_evy) >> 4;
        b -= (b * g_fpx_evy) >> 4;
    }
    return 0xFF000000u | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

/* run after093, lane MGINTERP1: engine A's 2D drawn again into fbp (a picture between two minigame ticks, whose 3D is already
   in it) from the registers and the OAM as they stand now, which the caller has set to the blended state. Only the raster runs:
   see g_mc_replay. The records the raster leaves for the pictures between ticks of a TEXT box (the tween window, the text
   scroll, the HUD slide note) are the tick's and are put back afterwards. Returns 1. */
extern "C" int port_message_engine_a_replay(void *fbp);
/* the same, starting from the tick's finished picture (SCREEN_W stride): only the cells that differ from the tick's are drawn */
extern "C" int port_message_engine_a_replay_dirty(void *fbp, const unsigned int *tick_final)
{
    g_mc_dirty_final = reinterpret_cast<const uint32_t *>(tick_final);
    const int r = port_message_engine_a_replay(fbp);
    g_mc_dirty_final = nullptr;
    return r;
}
extern "C" int port_message_engine_a_replay(void *fbp)
{
    const TwWin tw_cur = g_tw_cur, tw_prev = g_tw_prev;
    const int tw_closing = g_tw_closing, ts_live = g_ts_live, ts_pend = g_ts_pend;
    const HudSlideNote note = g_note;
    int slide_n = g_slide_n, slide_ds[2][4], slide_d[2];
    std::memcpy(slide_ds, g_slide_ds, sizeof slide_ds);
    std::memcpy(slide_d, g_slide_d, sizeof slide_d);
    g_mc_replay = 1;
    port_message_composite_engine_a(fbp);
    g_mc_replay = 0;
    g_tw_cur = tw_cur; g_tw_prev = tw_prev; g_tw_closing = tw_closing;
    g_ts_live = ts_live; g_ts_pend = ts_pend;
    g_note = note;
    g_slide_n = slide_n;
    std::memcpy(g_slide_ds, slide_ds, sizeof slide_ds);
    std::memcpy(g_slide_d, slide_d, sizeof slide_d);
    return 1;
}
