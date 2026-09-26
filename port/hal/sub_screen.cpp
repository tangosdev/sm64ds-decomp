// The DS BOTTOM SCREEN as a corner panel, host side.
//
// Everything the bottom screen draws is matched game code -- HUD::Render,
// Minimap::Render, Message, and OAM::Render underneath all of them. What was
// missing was never the drawing; it was four seams:
//
//   1. DUAL OAM. OAM::Render's first decision is
//
//          if (!sub || data_0209e660 == 1) -> the MAIN shadow
//
//      and the HAL pinned that byte at 1, so every sub==true sprite in the
//      game landed in the top screen's shadow buffer. Clearing it is what
//      OAM::EnableSubOAM does on the ROM, and it is only safe once the three
//      main-shadow symbols are contiguous (hal/model_host.cpp), because
//      OAM::Reset's mode-0 path fills the buffer through them.
//
//   2. THE FRAME. The ROM's 2D frame is Reset at the top, Render calls in the
//      middle, Load at the bottom -- the shadow is rebuilt every frame and
//      uploaded once. Nothing in the port was calling any of the three.
//
//   3. POWCNT1 BIT 15. OAM::Load reads it to decide which shadow goes to
//      which engine's OAM. Zero swaps them, which would upload the top
//      screen's sprites to the bottom engine.
//
//   4. SCAN-OUT AND PLACEMENT, which is ntr/ppu_sub.cpp plus the blit here.
//
// The panel is composited at 1:1 DS pixels into the bottom-right corner of
// whatever tier the top screen is drawn at. TAB toggles it; with it off the
// top screen is left byte-for-byte untouched, which is the regression this
// stream has to keep passing.
//
// The touch bridge polls the cursor rather than taking window messages, so
// walk_window.cpp needs no wndproc changes: the panel is a rectangle on the
// screen and a click inside it is a stylus press at the same DS pixel.
//
// THE PRESENT RECTANGLE lives here for the same reason the touch does. Once
// the window became resizable, "client pixel divided by zoom" stopped being
// the framebuffer pixel -- the frame is now scaled to fit whatever the client
// area is and centred inside it, with black bars on whichever axis is over. A
// stylus press that did not undo that scaling would land somewhere else on
// the bottom screen at every window size but the default, so the two halves
// of the arithmetic have to stay together. walk_window.cpp publishes the
// rectangle it presented into; poll_touch below runs it backwards. Unset (a
// binary that opens no window, or one whose present path never ran) falls
// back to the plain zoom divide, which is what every caller did before.
//
// ---- THE STACKED LAYOUT (run link60, lane DSL1) -----------------------------
//
// THE PANEL IS THE WRONG SHAPE FOR A MINIGAME AND ALWAYS WAS. It is 128x96 at
// the default divisor, in a corner, and every minigame in this game is played
// with the stylus ON the bottom screen. You cannot aim at a preview. The
// directive is Tango's and it is one sentence: for minigames the two screens
// go one above the other, the same size, because it is a touchscreen game.
//
// So this file grows a MODE. Stacked: both screens full size, top above
// bottom, SCREEN_W x 2*SCREEN_H of client area (512x768 at the 2x tier). Inset:
// what is above, unchanged, and still the default for levels, where the panel
// is doing its job and the player's hands are on the keyboard.
//
// THE MODE LIVES HERE BECAUSE THE THREE THINGS IT CHANGES ALREADY DO. The
// panel, the present rectangle and the touch transform are all in this file,
// and they are in one file precisely so a change to the geometry cannot move
// the picture without moving the aim with it. A second presenter somewhere
// else would have been a second copy of that arithmetic.
//
// WHAT IT DOES NOT TOUCH, and this is structural rather than remembered: the
// framebuffer. ntr::Framebuffer stays SCREEN_W x SCREEN_H in both modes, the
// stacked image is a SEPARATE buffer composed after everything that writes
// the framebuffer has finished with it, and every ppu_write_bmp call site in
// the tree still dumps the same 512x384 frame it dumped before. The selftest
// battery's md5 rows depend on that geometry and this mode cannot reach them.
//
// WHO TURNS IT ON. hal/scene_boot.cpp asks IsMinigameActorID -- the ROM's own
// predicate, src/IsMinigameActorID.c, 0x169..0x186, already linked and already
// the gate on the ov006 overlay constructors -- and hands the answer to
// hal_sub_screen_set_stacked. A level never asks, so a level gets the inset.
// SM64DS_DUAL_SCREEN=1 forces it on and =0 forces it off, both ways, and the
// env is read HERE so there is one place that decides.
#include <cstdio>
#include <cstdlib>

#include <windows.h>

#include <cstring>       /* run mg16 lane MP3: the touch-ring store */

#include "ntr/ppu.h"
#include "hal/comms_seam.h"   /* run mg16 lane MP3: touch_ring_index/advance */

#include "hal/screen_gap.h"
#include "hal/host_settings.h"   /* the improved map's two keys */

/* The PNG reader, vendored public-domain and already the one this port uses
   for the replacement texture pack (ntr/hdtex.cpp, which is where the
   IMPLEMENTATION is compiled -- every ntr library variant carries it, and
   every target that links this file links one of them). Declarations only
   here. The relative include is the spelling hdtex.cpp uses, because
   port/CMakeLists.txt belongs to another lane. */
#include "../third_party/stb/stb_image.h"

namespace OAM {
void Reset();
}

/* Stage::LoadGraphics2D is a real C++ static member in its own TU, so it is
   reached by its MSVC name through a matching declaration rather than by the
   Itanium alias the .c callers use. */
class Stage {
public:
    static void LoadGraphics2D(bool b, int i);
};

extern "C" {
/* the present rectangle, defined at the bottom of this file */
void hal_present_set_rect(int x, int y, int w, int h, int src_w, int src_h);
/* THE FIT, the one copy of it; tests/walk_window.cpp's present() and the
   layout selftest below both call this rather than each spelling it. */
void hal_present_fit(int cw, int ch, int sw, int sh, int *x, int *y, int *w, int *h);
int hal_present_client_to_fb(int cx, int cy, int *fx, int *fy);
int hal_present_client_to_sub(int cx, int cy, int *dsx, int *dsy);
/* the layout mode, defined at the bottom of this file */
int hal_sub_screen_stacked(void);
void hal_touch_client_probe(void);
/* THE THREE ATTENTION ARROWS, re-anchored above the panel (engine A's raster,
   hal/message_compositor.cpp, which owns the sprites; this file owns where the
   panel is). Called with the panel rect this frame's compose just drew, so the
   arrows follow the size, the aspect and the render scale for free. Inert
   unless hal_minimap_arrow_reanchor_on below says so, and nothing to draw is
   the ordinary case: the ROM raises the cue only on a map event. */
void port_bounce_arrows_present(unsigned int *dst, int dst_w, int dst_h,
                                int px0, int py0, int pw, int ph);
/* THE LEVEL-CLEAR GLYPH OVERLAY (hal/message_compositor.cpp, which owns engine
   A's layers; this file owns the compose that reads it). While the composed
   level-clear picture is up, that unit sends the pixels of the layers carrying
   the menu's own lettering into these two framebuffer-shaped buffers instead
   of into the picture, so what the picture holds is the game's own frame with
   the lettering lifted off it -- no row moved and no region copied -- and this
   file draws the lifted pixels where the owner asked for them. `live` is that
   unit's answer for THIS frame; the buffers are indexed at a stride of
   ntr::SCREEN_W, the same as dst. See the banner over swap_present. */
int hal_lc_overlay_live(void);
const unsigned int *hal_lc_overlay_colour(void);
const unsigned char *hal_lc_overlay_mask(void);
/* the two engines' brightness blends (hal/fader_wipes.cpp). The SUB one is what
   the sub framebuffer is composed with; the main one is still read by the inset
   path, where the panel is inside engine A's framebuffer when walk_window's own
   fade composite runs over it. */
int port_fader_blend_state(int *evy, int *toWhite);
int port_fader_blend_state_sub(int *evy, int *toWhite);
void _ZN3OAM4LoadEv(void);
unsigned int _ZN3OAM12EnableSubOAMEv(void);
int hal_oam_layout_check(void);
/* the GX bank-state band guard (hal/cxx_aliases.cpp) */
int port_gxbank_layout_check(void);
/* the sprite-template guard (hal/oam_lists.cpp): every OamAttr* the HUD and
   the Minimap hand OAM::Render, checked for a missed pointer rebase */
int hal_oam_templates_check(void);
int hal_oam_walk_probe(void);
/* what hal/scene_boot.cpp's beat answered this frame: 1 = run func_02019144's
   tail, 0 = the current graphics block already did the display sync itself */
int port_graph_block_verdict(void);
extern unsigned char data_0209e660;
extern unsigned char data_0209caa0[];   /* the save block; byte 8 bit 7 = intro seen */
extern signed char data_0209f2f8;       /* current level */
/* The sub engine's LAYER ENABLE MASK, bits 0..4 = BG0..BG3, OBJ. Nine
   different ROM functions publish it into DISPCNT_B bits 8-12 with the very
   same line; Minimap::Behavior and Message::UpdateWindow are two of them. It
   is the game's own switchboard for what the bottom screen shows. */
extern unsigned char data_0209d454;
/* the engine A layer mask and the eight BG-offset shadows func_02019144
   publishes; all arm9 BSS, hosted by span in hal/scene_boot.cpp's blocks */
extern unsigned char data_0209d45c;
extern short data_0209d468, data_0209d46c, data_0209d4a4, data_0209d4a0;
extern short data_0209d49c, data_0209d478, data_0209d48c, data_0209d490;
extern short data_0209d494, data_0209d498, data_0209d484, data_0209d480;
extern short data_0209d47c, data_0209d470, data_0209d474, data_0209d488;
/* TouchInfo data_020a0de8[4]: {u8 touched, u8 edge, u8 x, u8 y} per slot, in
   DS bottom-screen pixels. Stage::CheckCameraInput and every TouchArea read
   it; nothing on the host was writing it.

   BYTE +1 IS AN EDGE AND NOT A HOLD. src/func_0203bb60.c, the ROM's own
   producer, writes `p[1] = touch ^ p[0]` -- set on the frame the stylus goes
   down and again on the frame it comes up. This file called it `held` for a
   long time and wrote a held flag into it; poll_touch's store carries the
   derivation and what the wrong shape cost.

   THE FOUR NAMES ARE ONE BLOCK. dsd named a symbol at each of slot 0's four
   bytes, and readers reach the fields through whichever name they were
   decompiled with -- Message::Update reads the edge as data_020a0de9[idx*4]
   and `y` as data_020a0deb[idx*4]. hal/auto_bss.cpp hosts all four over the
   one 16-byte run so a write here reaches every one of them; the touch probe
   below is what proves it. */
extern unsigned char data_020a0de8[];   /* +0 touched */
/* run mg16 lane MP3: the ROM's four-deep touch RING, hosted by
   hal/comms_conductor.cpp. This is link 3's input -- src/func_0203b9bc.c reads
   it and settles data_020a0dd8 -- and is a different thing from the TouchInfo
   block above, which is link 5's output. The store at the bottom of poll_touch
   is what fills it. */
extern unsigned char data_020a0df8[];
extern unsigned char data_020a0de9[];   /* +1 edge    */
extern unsigned char data_020a0dea[];   /* +2 x       */
extern unsigned char data_020a0deb[];   /* +3 y       */
/* Stage::CheckCameraInput's own inputs and outputs */
void _ZN5Stage16CheckCameraInputEv(void);
/* Stage::SetVramBanks, InitResources:262-263's own call. Ends in
   GX::SetBankForSubBGExtPltt(0x80); cxx_aliases.cpp:3281 already carries its
   /alternatename row, so it links with no new seat. */
void _ZN5Stage12SetVramBanksEv(void);
extern int data_0209f498[];      /* the Ctrl[4] block, stride 0x18 */
extern char data_0209f49c[];     /* split: held buttons  (DS: f498 + 4) */
extern char data_0209f49e[];     /* split: pressed       (DS: f498 + 6) */
extern void *data_0209f318;      /* the Camera actor */
/* THE LEVEL-CLEAR SCREEN'S OWN TWO WORDS, and the swap below reads nothing
   else. data_0209f20c is "the level-clear screen owns the game": the player's
   own star animation raises it (src/actors/Player.cpp:6522, :6718, :6742) and
   src/_ZN5Stage8BehaviorEv.cpp:206 runs Stage::LC_Update only while it is up.
   data_0209f2d4 is that function's own state word. */
extern unsigned char data_0209f20c;
extern unsigned char data_0209f2d4;
}

namespace {

const int kMargin = 8;

/* ---- THE CORNER PANEL'S GEOMETRY, COMPUTED ONCE ----------------------------
 *
 * THREE READERS, ONE ARITHMETIC. The compose draws the panel, this file
 * publishes its origin, and poll_touch's inset arm runs the whole thing
 * backwards to turn a client pixel into a DS pixel. Before the size became a
 * player option those three shared one integer divisor and agreed by
 * construction. A fraction cannot be trusted to agree by construction -- two
 * sites rounding 5/8 of 256 independently is exactly the shape of defect that
 * put every menu tap two thirds of the way up its button at 32:9 (lane WIDE4)
 * -- so the numbers are computed HERE, once per frame, and the other two read
 * these variables rather than recomputing anything.
 *
 * g_pan_num / g_pan_den is the size as a fraction of one DS screen:
 *
 *   option OFF   1 / g_div          exactly what this file has always drawn
 *   option ON    the MinimapScale table's ratio (index 0 is 1/2, which IS
 *                g_div 2, so scale 1 with the option on is the same picture)
 *
 * SM64DS_SUB_SCALE still rules with the option off and is IGNORED with it on:
 * the two knobs mean the same thing and the player-facing one wins when the
 * player has turned it on. Said here rather than discovered by a player whose
 * old environment variable stopped having an effect.
 */
int g_div = 2;             // panel downscale divisor (SM64DS_SUB_SCALE)
int g_pan_num = 1, g_pan_den = 2;   /* panel size as a fraction of a DS screen */
int g_pan_w, g_pan_h;               /* the drawn panel, in framebuffer pixels */
int g_pan_margin = kMargin;         /* what was left of the margin (see below) */

bool g_on = true;
bool g_ready;
/* Headless runs must not read the mouse or the keyboard. SM64DS_WINDOW_SELFTEST
   is CI's comparator and several of these binaries run on one machine at once,
   so a stray click or a TAB meant for another window would land in this one's
   frame and show up as a BMP diff nobody can reproduce. The selftest drives
   its own input; the panel adds none. */
bool g_headless;
/* SM64DS_INPUT_NOFOCUSGATE=1 puts the interactive keyboard back on the
   machine-global reads it used before the focus gate. Nothing in the tree
   sets it; it exists so a harness that really does want keys read with the
   window in the background has a documented way to say so. */
bool g_nofocusgate;
int g_x0, g_y0;            // panel origin in framebuffer pixels

/* The improved map's own gate, asked once per process. The keys are latched
   at boot like Aspect and RenderScale, so re-asking every frame would only
   re-walk the environment. */
int improved_map_on(void)
{
    static int v = -1;
    if (v < 0) v = host_setting_improved_minimap() ? 1 : 0;
    return v;
}

/* How far the decorated panel reaches past the map on each side, in
   framebuffer pixels, for the size currently in g_pan_*. Defined further down,
   beside the panel's own geometry, and declared here because the size has to
   be settled before anything is drawn. Zero on all four sides with the option
   off, where there is no panel. */
void panel_extents(int *l, int *t, int *r, int *b);

/* The inset selftest's size override, in g_pan_num's own units. Zero at every
   other moment, including in every player's run. */
int g_size_force;

/* THE SCREEN SWAP'S two per-frame answers, declared up here because
   handle_rect reads the first of them and sits above the block that defines
   the rest. Zero in every frame that is not a level-clear save menu, and
   zero for the whole of every run with the option off. See the banner over
   swap_geom_for. */
int g_swap;                      /* composed this frame? the compose's answer */
int g_menu_up;                   /* the ROM pair's answer for this frame */

/* THE LIFT/DRAW DRIFT DETECTOR, and it is one int. The engine-A compositor
   lifts the level-clear lettering and the coin sprites out of the picture on
   every frame hal_lc_compose_rows answers 1 for, and swap_present is the only
   thing that draws them again. This word is set where the lift is promised
   and cleared where the draw honours it, and hal_lc_compose_rows says so once
   per process if a promise was ever left unhonoured. It costs one store and
   one test a frame and it is the net under the whole mechanism: the 0.4.1
   gate's red row was exactly that drift, silent, with the pixels simply gone
   from the picture. */
int g_lc_lift_pending;

/* SM64DS_MINIMAP_TRACE=1: every rectangle the geometry above and the panel's
   own settled on, printed when one of them changes. It exists so that a
   capture can be MEASURED against the numbers the program actually drew,
   rather than against a restatement of this arithmetic in some other
   language -- which is the one thing a proof about a picture must not be.
   Silent without the variable. Defined beside the panel's geometry. */
void panel_trace(int w, int h);

/* THE ONE ARITHMETIC. See the banner over g_pan_num.
 *
 * THE PANEL IS WHAT IS ANCHORED, not the map, whenever there is a panel. The
 * owner asked for the whole thing -- plate, banner and all -- to stay in the
 * picture at every size, so the eight-pixel inset is measured from the
 * panel's outer edge and the map sits wherever that puts it. With the option
 * off there is no panel and this is the arithmetic it always was, to the
 * pixel.
 *
 * THE MARGIN GIVES WAY BEFORE THE SIZE DOES. At a large size there is no room
 * for the inset the smaller sizes sit in, so the margin shrinks to whatever
 * fits before the size is touched at all.
 *
 * AND THEN THE SIZE GIVES WAY. A size whose panel will not fit the picture
 * even flush to the corner steps down four pixels of map width at a time
 * until it does -- which is the ceiling the drag runs into, and the reason
 * the ceiling lives here rather than in the settings file: it depends on the
 * picture, and the picture depends on the window, the aspect and the render
 * scale. The old code answered "no room" by RETURNING WITHOUT DRAWING, which
 * for a player who picked the largest size was a map that silently did not
 * appear; a size one step smaller is a better answer than no map. */
void hal_sub_panel_geometry(int w, int h)
{
    int num = 1, den = g_div;
    if (improved_map_on()) {
        host_setting_minimap_scale_ratio(&num, &den);
        /* the inset selftest walks every size through this function without
           touching the player's settings; zero at every other moment */
        if (g_size_force > 0) num = g_size_force;
    }
    if (num < 1) num = 1;
    if (den < 1) den = 1;

    /* the smallest the improved map goes is the picture the port drew before
       it existed, which is half a DS screen: num/den = 1/2 */
    const int on = improved_map_on() ? 1 : 0;
    const int nmin = on ? (den / 2 < 1 ? 1 : den / 2) : num;
    int el = 0, et = 0, er = 0, eb = 0;
    for (;;) {
        g_pan_num = num;
        g_pan_den = den;
        g_pan_w = ntr::SUB_W * num / den;
        g_pan_h = ntr::SUB_H * num / den;
        if (!on) break;
        panel_extents(&el, &et, &er, &eb);
        if (num <= nmin) break;
        if (el + g_pan_w + er <= w && et + g_pan_h + eb <= h) break;
        num -= 4;
        if (num < nmin) num = nmin;
    }

    int m = kMargin;
    if (el + g_pan_w + er + m > w) m = w - (el + g_pan_w + er);
    if (et + g_pan_h + eb + m > h) m = h - (et + g_pan_h + eb);
    if (m < 0) m = 0;
    g_pan_margin = m;

    g_x0 = w - m - er - g_pan_w;
    g_y0 = h - m - eb - g_pan_h;
    if (g_x0 < 0) g_x0 = 0;
    if (g_y0 < 0) g_y0 = 0;

    if (on) panel_trace(w, h);
}

/* ---- THE DECORATIVE PANEL, COMPOSED FROM THE PLAYER'S OWN GAME DATA -------
 *
 * WHAT IT IS. A bordered panel behind the map, tiled with the game's own
 * wallpaper, with a plaque on its top edge reading MAP -- the design an artist
 * sent the owner. His two PNGs are DERIVED FROM THE CARTRIDGE (lane MINIMAP1
 * measured it: every colour in both sits on the DS's own BGR555 ladder, the
 * panel interior is an exact 32x32 repeat, and its fifteen wallpaper colours
 * are one 16-entry DS palette, in palette order, inside the ROM). The public
 * bundle is ROM-clean, so those files are never committed, copied, embedded or
 * read. The look comes from the player's own copy of the game instead, and
 * only the DESIGN'S GEOMETRY is taken from the artist's work, which is what
 * geometry is.
 *
 * WHERE THE WALLPAPER LIVES, MEASURED RATHER THAN GUESSED. The palette lane
 * MINIMAP1 found in the cartridge is ARCHIVE/en1.narc member 6, bank 2 -- the
 * ONLY bank in that whole archive whose sixteen entries carry those fifteen
 * colours, searched bank by bank. Drawing member 1's background map with
 * member 2's tiles through that bank paints exactly those fifteen colours and
 * repeats every 32 pixels horizontally with a 0.991 match, which is the same
 * 32x32 motif MINIMAP1 measured in the artist's panel. So the three members
 * are the wallpaper, and this reads them straight out of the archive:
 *
 *     0x8401  the background map   LZ-compressed, 32x32 entries
 *     0x8402  the tiles            LZ-compressed, 4bpp
 *     0x8406  the palettes         plain, nine banks of sixteen
 *
 * It is a MENU screen's background, so a course never loads it and there is no
 * copy in VRAM to borrow -- hence the archive read. It happens ONCE, the first
 * frame a course draws the panel, and only with the option on.
 *
 * THE GEOMETRY, off the artist's design, in DS pixels against its own 240x180
 * interior: six pixels of wallpaper outside a frame one pixel thick on the
 * left and top and two on the right and bottom, so the panel stands seven
 * pixels proud of the map on the left and top and eight on the right and
 * bottom. The plaque is 109 wide and 32 tall against a 255-wide panel, centred
 * on its top edge with ten of those rows above it. Every one of those numbers
 * is a size or a position.
 *
 * THE COLOURS ARE ROLES, taken from the player's palette and never from the
 * PNG: the wallpaper is the player's tiles as they are, the frame is the
 * darkest colour in the player's own wallpaper bank taken down to a shadow of
 * itself, and the plaque is the brightest one.
 *
 * WHAT IS STILL OWED. The word MAP is drawn in this file's own block letters,
 * not in the game's font: finding the cartridge's own MAP label, or its HUD
 * glyphs, is a hunt this lane did not have the clock for. Everything else on
 * the panel is the player's own data. */
extern "C" const unsigned char *port_fs_archive_member(unsigned fileID,
                                                       unsigned *len_out);
extern "C" void DecompressLZ16(void *src, void *dst);

const unsigned kWallScreen = 0x8401, kWallTiles = 0x8402, kWallPltt = 0x8406;
const int kWallBank = 2;          /* measured; see the banner above */

/* the motif, 32x32 ARGB, and the two colour roles taken from the same bank */
unsigned g_motif[32][32];
unsigned g_panel_dark, g_panel_bright;
int g_motif_state;                /* 0 not tried, 1 ready, -1 unavailable */

unsigned bgr555_to_argb(unsigned c)
{
    const unsigned r = c & 0x1F, g = (c >> 5) & 0x1F, b = (c >> 10) & 0x1F;
    return 0xFF000000u | ((r << 3 | r >> 2) << 16) | ((g << 3 | g >> 2) << 8)
           | (b << 3 | b >> 2);
}

/* The compressed member, expanded with the game's own decompressor. The size
   is the LZ header's own, which is what every caller of DecompressLZ16 in the
   ROM relies on; a member that is not LZ at all is refused rather than run. */
unsigned char *wall_expand(unsigned fileID, unsigned *out_len)
{
    unsigned n = 0;
    const unsigned char *raw = port_fs_archive_member(fileID, &n);
    if (!raw || n < 4 || raw[0] != 0x10) return 0;
    const unsigned size = (unsigned)raw[1] | (unsigned)raw[2] << 8
                        | (unsigned)raw[3] << 16;
    if (size < 32 || size > (1u << 20)) return 0;
    unsigned char *dst = (unsigned char *)std::malloc(size);
    if (!dst) return 0;
    /* DecompressLZ16 takes a writable source pointer the way the ROM's own
       callers hand it one; the bytes are not modified. */
    DecompressLZ16((void *)raw, dst);
    *out_len = size;
    return dst;
}

int wall_build(void)
{
    unsigned scr_n = 0, til_n = 0, pal_n = 0;
    unsigned char *scr = wall_expand(kWallScreen, &scr_n);
    unsigned char *til = scr ? wall_expand(kWallTiles, &til_n) : 0;
    const unsigned char *pal = port_fs_archive_member(kWallPltt, &pal_n);
    int ok = 0;
    if (scr && til && pal && scr_n >= 2048 && til_n >= 1024
        && pal_n >= (unsigned)(kWallBank + 1) * 32) {
        unsigned bank[16];
        int distinct = 0;
        for (int k = 0; k < 16; ++k) {
            const unsigned c = (unsigned)pal[kWallBank * 32 + k * 2]
                             | (unsigned)pal[kWallBank * 32 + k * 2 + 1] << 8;
            bank[k] = bgr555_to_argb(c);
            int seen = 0;
            for (int j = 0; j < k; ++j) if (bank[j] == bank[k]) seen = 1;
            distinct += !seen;
        }
        /* THE GUARD ON THE WHOLE DERIVATION. A wallpaper bank is sixteen
           distinct colours; a bank that is mostly one colour is not the one
           this was measured against, and the panel falls back rather than
           painting a flat rectangle and calling it the artist's design. */
        if (distinct >= 12) {
            /* The first 4x4 block of the background map whose sixteen entries
               all carry ONE non-zero palette, which is a block of wallpaper
               and not the header or the frame the menu draws around it. */
            int bx = -1, by = -1;
            for (int ty = 0; ty + 4 <= 32 && by < 0; ++ty)
                for (int tx = 0; tx + 4 <= 32; ++tx) {
                    int pl = -1, good = 1;
                    for (int j = 0; j < 4 && good; ++j)
                        for (int i = 0; i < 4; ++i) {
                            const unsigned e =
                                (unsigned)scr[((ty + j) * 32 + tx + i) * 2]
                                | (unsigned)scr[((ty + j) * 32 + tx + i) * 2 + 1] << 8;
                            const int p = (int)(e >> 12);
                            if (!p || (pl >= 0 && p != pl)) { good = 0; break; }
                            pl = p;
                        }
                    if (good) { bx = tx; by = ty; break; }
                }
            if (bx >= 0) {
                unsigned lo = 0xFFFFFFFFu, hi = 0;
                /* INDEX 0 IS NOT A COLOUR. On the DS a 4bpp background pixel
                   of index 0 is transparent and the layer under it shows
                   through, so painting it with the bank's entry 0 would put a
                   colour on the panel that the wallpaper never shows -- the
                   first build of this did, and the frame came out blue. The
                   motif paints those pixels with the pattern's own field
                   colour instead: the non-transparent index that covers most
                   of the block. */
                int hist[16] = {0};
                for (int j = 0; j < 4; ++j)
                    for (int i = 0; i < 4; ++i) {
                        const unsigned e =
                            (unsigned)scr[((by + j) * 32 + bx + i) * 2]
                            | (unsigned)scr[((by + j) * 32 + bx + i) * 2 + 1] << 8;
                        const unsigned t = e & 0x3FFu;
                        for (int k = 0; k < 32; ++k) {
                            const unsigned o = t * 32u + (unsigned)k;
                            if (o >= til_n) break;
                            ++hist[til[o] & 0xF];
                            ++hist[til[o] >> 4];
                        }
                    }
                int field = 1;
                for (int k = 2; k < 16; ++k)
                    if (hist[k] > hist[field]) field = k;
                bank[0] = bank[field];
                for (int j = 0; j < 4; ++j)
                    for (int i = 0; i < 4; ++i) {
                        const unsigned e =
                            (unsigned)scr[((by + j) * 32 + bx + i) * 2]
                            | (unsigned)scr[((by + j) * 32 + bx + i) * 2 + 1] << 8;
                        const unsigned t = e & 0x3FFu;
                        const int hf = (e >> 10) & 1, vf = (e >> 11) & 1;
                        for (int y = 0; y < 8; ++y)
                            for (int x = 0; x < 8; ++x) {
                                const int sx = hf ? 7 - x : x;
                                const int sy = vf ? 7 - y : y;
                                const unsigned o = t * 32u + (unsigned)sy * 4u
                                                 + (unsigned)(sx >> 1);
                                unsigned idx = 0;
                                if (o < til_n)
                                    idx = (sx & 1) ? (unsigned)(til[o] >> 4)
                                                   : (unsigned)(til[o] & 0xF);
                                const unsigned c = bank[idx];
                                g_motif[j * 8 + y][i * 8 + x] = c;
                                const unsigned l = ((c >> 16) & 0xFF)
                                                 + ((c >> 8) & 0xFF) + (c & 0xFF);
                                if (l < lo) { lo = l; g_panel_dark = c; }
                                if (l > hi) { hi = l; g_panel_bright = c; }
                            }
                    }
                /* THE FRAME'S COLOUR ROLE: the darkest colour the player's own
                   wallpaper actually uses, taken down to a third of itself so
                   it reads as a shadow line round the panel. A role, computed
                   from the player's palette; nothing is sampled from anyone's
                   artwork. */
                g_panel_dark = 0xFF000000u
                    | ((((g_panel_dark >> 16) & 0xFF) / 3) << 16)
                    | ((((g_panel_dark >> 8) & 0xFF) / 3) << 8)
                    | ((g_panel_dark & 0xFF) / 3);
                ok = 1;
            }
        }
    }
    std::free(scr);
    std::free(til);
    if (!ok && std::getenv("SM64DS_MINIMAP_TRACE"))
        std::fprintf(stderr, "[mmtrace] wallpaper not available "
                     "(screen %u tiles %u palette %u); plain panel\n",
                     scr_n, til_n, pal_n);
    else if (ok && std::getenv("SM64DS_MINIMAP_TRACE"))
        std::fprintf(stderr, "[mmtrace] wallpaper motif built from the "
                     "player's data; frame %06x plaque %06x\n",
                     g_panel_dark & 0xFFFFFF, g_panel_bright & 0xFFFFFF);
    return ok;
}

/* M, A and P in a 5x7 grid, one bit per column, this file's own lettering.
   Named for what it is: the game's own MAP label is still owed. */
const unsigned char kGlyphMAP[3][7] = {
    { 0x11, 0x1B, 0x15, 0x11, 0x11, 0x11, 0x11 },   /* M */
    { 0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11 },   /* A */
    { 0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10 },   /* P */
};

/* ---- THE ARTIST'S PANEL, WHEN THE PLAYER HAS THE PICTURES ------------------
 *
 * WHAT THIS IS. Someone drew the owner a panel to go round the map -- a
 * bordered plate with a MAP plaque on its top edge -- and sent him two PNGs.
 * When those two files are sitting in a folder beside the game data, THEY are
 * the panel: read at boot, drawn as they are. When they are not, the panel
 * composed from the player's own game data a few hundred lines above is drawn
 * instead, silently but for one line on stderr.
 *
 * THE PICTURES ARE NOT PART OF THIS PROGRAM, and that is the whole shape of
 * this code. They are not in the repository, not in the build, not embedded in
 * the executable and not copied into any tree: lane MINIMAP1 measured them to
 * be made from the cartridge's own art (every colour on the DS's own BGR555
 * ladder, an exact 32x32 repeat, and the panel's fifteen wallpaper colours
 * sitting in one 16-entry DS palette inside the ROM, in palette order). So
 * what this file does is read a FOLDER. Whether a folder with those two files
 * in it travels with a download is a packaging decision, and it is the
 * owner's, made outside this code.
 *
 * WHICH FOLDER: host_setting_minimap_dir() -- "<asset root>/minimap", the same
 * root textures_hd uses, with SM64DS_MINIMAP_DIR to point somewhere else.
 *
 * AT WHATEVER SIZE THEY COME. Nothing below counts on the two files being the
 * size the first pair happened to be. The well, the notch and the plaque's
 * place are all found as PROPORTIONS of the picture that carries them, so a
 * pair drawn at twice or four times the pixels lands on exactly the same
 * screen rectangles and simply has more detail to give when the map is small,
 * which is the whole point of sending a bigger one. The only thing the two
 * files owe each other is ONE PIXEL GRID: the plaque is drawn at the plate's
 * own scale, so both files have to be at the same multiple. The boot line
 * below prints both sizes so a mismatched pair is visible at a glance.
 * Filtering cost does not follow the file's size either: see ArtScaled.
 *
 * WHAT IS READ OUT OF THEM, AND IT IS MEASURED RATHER THAN ASSUMED. The panel
 * carries an inner outlined rectangle -- a dark line inset from its border --
 * and the map has to sit exactly inside it. Rather than trust a pixel count,
 * art_scan_inner below finds that rectangle by scanning the picture: the rows
 * and columns that are more than half dark are the frame's own lines, and the
 * interior is what they enclose. The same scan reports the transparent notch
 * at the top (where the plaque drops in) and the brightest and darkest colours
 * the artwork uses, which is where the resize handle takes its colours from.
 * Nothing about the artwork is hardcoded except the sanity bounds a scan has
 * to pass before it is believed.
 *
 * A picture that will not load, will not scan, or scans to something absurd is
 * simply not used. There is no half-way state. */
struct ArtImage {
    int w, h;
    unsigned *px;              /* ARGB8888, top row first */
};

ArtImage g_art_base, g_art_tab;
int g_art_state;               /* 0 not tried, 1 both here, -1 not available */
/* the inner outlined rectangle's INTERIOR, in base-image pixels, inclusive */
int g_art_ix0, g_art_iy0, g_art_ix1, g_art_iy1;
int g_art_notch;               /* fully transparent rows at the top */
unsigned g_art_bright, g_art_dark;   /* the artwork's own two extremes */
/* how far the map is inset inside the well when the well is not 4:3. Zero on
   the owner's own picture, whose well is 240 x 180 and so is exactly 4:3;
   published for the boot line and for the captures' measured rows. */
int g_art_gapx, g_art_gapy;

int art_load_one(const char *dir, const char *name, ArtImage *im)
{
    char path[1200];
    std::snprintf(path, sizeof path, "%s/%s", dir, name);
    int w = 0, h = 0, comp = 0;
    unsigned char *p = stbi_load(path, &w, &h, &comp, 4);
    if (!p) return 0;
    if (w < 8 || h < 8 || w > 4096 || h > 4096) { stbi_image_free(p); return 0; }
    unsigned *px = (unsigned *)std::malloc((size_t)w * (size_t)h * 4u);
    if (!px) { stbi_image_free(p); return 0; }
    for (int i = 0; i < w * h; ++i)
        px[i] = ((unsigned)p[i * 4 + 3] << 24) | ((unsigned)p[i * 4] << 16)
              | ((unsigned)p[i * 4 + 1] << 8) | (unsigned)p[i * 4 + 2];
    stbi_image_free(p);
    im->w = w;
    im->h = h;
    im->px = px;
    return 1;
}

void art_free(ArtImage *im)
{
    std::free(im->px);
    im->px = 0;
    im->w = im->h = 0;
}

inline unsigned art_lum(unsigned c)
{
    return (((c >> 16) & 0xFF) * 77 + ((c >> 8) & 0xFF) * 151
            + (c & 0xFF) * 28) >> 8;
}

/* THE INNER OUTLINED RECTANGLE, FOUND BY MEASUREMENT.
 *
 * The frame is a dark line on a light plate, so "dark" is defined against the
 * picture's own middle rather than against a constant: the median luminance of
 * its opaque pixels, halved. A row of the frame is then a row where more than
 * half the picture's width is dark, and a column likewise -- which no row of
 * wallpaper can be, and which a line of any thickness is. The first and last
 * such rows and columns are the four lines; walking in from each gives its
 * THICKNESS, and what the four thicknesses enclose is the interior.
 *
 * Then it has to pass: an interior at least 16 x 12, and all four frame lines
 * at least one pixel IN from the picture's own edge -- a dark line lying on
 * the outermost row is a picture with a dark border, not a panel with a map
 * well. That last test is the whole sanity check, and it is a test about
 * POSITION rather than about proportion on purpose: the artwork this was
 * written for is 255 x 205 with a 240 x 180 well, which is 94% of its width,
 * so any "the interior must be at most so many tenths of the picture" rule
 * large enough to be safe would have refused the very picture it exists to
 * accept. A picture that fails is refused whole, because a panel drawn round
 * a rectangle that is not the artist's is worse than the one this program can
 * compose for itself. */
int art_scan_inner(const ArtImage *im)
{
    long hist[256];
    for (int i = 0; i < 256; ++i) hist[i] = 0;
    long opaque = 0;
    for (int i = 0; i < im->w * im->h; ++i)
        if (im->px[i] >> 24) { ++hist[art_lum(im->px[i]) & 0xFF]; ++opaque; }
    if (opaque < 64) return 0;
    long half = opaque / 2, run = 0;
    unsigned med = 0;
    for (int i = 0; i < 256; ++i) {
        run += hist[i];
        if (run >= half) { med = (unsigned)i; break; }
    }
    const unsigned thr = med > 1 ? med / 2 : 1;

    /* the two profiles, and the extremes of the palette while the pixels are
       already in hand */
    unsigned lo = 0xFFFFFFFFu, hi = 0;
    g_art_bright = 0xFFFFFFFFu;
    g_art_dark = 0xFF000000u;
    int *rowd = (int *)std::calloc((size_t)im->h, sizeof(int));
    int *cold = (int *)std::calloc((size_t)im->w, sizeof(int));
    if (!rowd || !cold) { std::free(rowd); std::free(cold); return 0; }
    for (int y = 0; y < im->h; ++y)
        for (int x = 0; x < im->w; ++x) {
            const unsigned c = im->px[y * im->w + x];
            if (!(c >> 24)) continue;
            const unsigned l = art_lum(c);
            if (l < lo) { lo = l; g_art_dark = c | 0xFF000000u; }
            if (l >= hi) { hi = l; g_art_bright = c | 0xFF000000u; }
            if (l < thr) { ++rowd[y]; ++cold[x]; }
        }

    int t = -1, b = -1, l = -1, r = -1;
    for (int y = 0; y < im->h; ++y) if (rowd[y] * 2 > im->w) { if (t < 0) t = y; b = y; }
    for (int x = 0; x < im->w; ++x) if (cold[x] * 2 > im->h) { if (l < 0) l = x; r = x; }
    int ok = 0;
    if (t >= 0 && b > t && l >= 0 && r > l) {
        int t1 = t, b0 = b, l1 = l, r0 = r;
        while (t1 + 1 < im->h && rowd[t1 + 1] * 2 > im->w) ++t1;
        while (b0 - 1 >= 0 && rowd[b0 - 1] * 2 > im->w) --b0;
        while (l1 + 1 < im->w && cold[l1 + 1] * 2 > im->h) ++l1;
        while (r0 - 1 >= 0 && cold[r0 - 1] * 2 > im->h) --r0;
        const int ix0 = l1 + 1, ix1 = r0 - 1, iy0 = t1 + 1, iy1 = b0 - 1;
        const int iw = ix1 - ix0 + 1, ih = iy1 - iy0 + 1;
        if (t1 < b0 && l1 < r0 && iw >= 16 && ih >= 12 &&
            ix0 >= 2 && iy0 >= 2 && ix1 <= im->w - 3 && iy1 <= im->h - 3) {
            g_art_ix0 = ix0; g_art_ix1 = ix1;
            g_art_iy0 = iy0; g_art_iy1 = iy1;
            ok = 1;
        }
    }
    /* the notch: the fully transparent rows at the top, which is where the
       plaque drops in and by how much it stands proud of the plate */
    g_art_notch = 0;
    for (int y = 0; y < im->h; ++y) {
        int any = 0;
        for (int x = 0; x < im->w && !any; ++x) any = im->px[y * im->w + x] >> 24;
        if (any) break;
        ++g_art_notch;
    }
    std::free(rowd);
    std::free(cold);
    return ok;
}

/* Asked once. Both files or neither: a plate with no plaque, or a plaque with
   no plate, is not the design anyone drew. */
int art_build(void)
{
    const char *dir = host_setting_minimap_dir();
    int ok = art_load_one(dir, "SM64DSMapBase.png", &g_art_base)
          && art_load_one(dir, "SM64DSMapText.png", &g_art_tab)
          && art_scan_inner(&g_art_base)
          && g_art_tab.w <= g_art_base.w;
    if (!ok) {
        art_free(&g_art_base);
        art_free(&g_art_tab);
        std::fprintf(stderr, "[minimap] no panel artwork in %s -- the panel is "
                     "composed from the game's own data instead\n", dir);
    } else {
        const int iw = g_art_ix1 - g_art_ix0 + 1, ih = g_art_iy1 - g_art_iy0 + 1;
        std::fprintf(stderr, "[minimap] panel artwork from %s: plate %dx%d, "
                     "plaque %dx%d, map well %d,%d %dx%d, notch %d rows\n",
                     dir, g_art_base.w, g_art_base.h, g_art_tab.w, g_art_tab.h,
                     g_art_ix0, g_art_iy0, iw, ih, g_art_notch);
        /* THE MAP IS 4:3 AND SO, ON THE OWNER'S PICTURE, IS THE WELL. Said out
           loud either way, because a well that is not 4:3 means the map no
           longer touches all four of its sides and a reader is owed the number
           rather than a squint at a screenshot. */
        if (iw * 3 == ih * 4)
            std::fprintf(stderr, "[minimap] the well is exactly 4:3, so the map "
                         "fills it to all four sides at every size\n");
        else
            std::fprintf(stderr, "[minimap] the well is %dx%d, which is %d "
                         "pixels off 4:3 in width (4:3 at that height is %d): "
                         "the map keeps its own shape and is centred in the "
                         "well, so it touches two sides and not four\n",
                         iw, ih, iw - ih * 4 / 3, ih * 4 / 3);
    }
    return ok;
}

inline int art_on(void)
{
    if (!g_art_state) g_art_state = art_build() ? 1 : -1;
    return g_art_state == 1;
}

/* FLOOR division, because the picture extends OUTWARD from the rectangle the
   arithmetic below is anchored on and the coordinates going into it are
   therefore negative as often as not. C's own division truncates towards
   zero, which would fold the two pixels either side of the anchor onto the
   same destination row and put a seam at the anchor's own edge. */
inline int fdiv(int a, int b)
{
    return a >= 0 ? a / b : -((-a + b - 1) / b);
}

/* ONE PICTURE, PLACED BY AN ANCHOR RECTANGLE RATHER THAN BY ITS OWN.
 *
 * The caller names a rectangle INSIDE the source picture and the destination
 * rectangle it has to land on exactly; the rest of the picture follows from
 * the same linear map and spills outside as far as it goes. That is the shape
 * this needs because the thing being placed is the artist's map well: the well
 * must sit exactly on the map, and where the plate's outer edge then falls is
 * an answer, not an input. Placing the picture by its own rectangle and hoping
 * the well lands right is how a border ends up a pixel inside the map.
 *
 * The source's alpha is a stencil (0 or 255 is all this artwork has, and
 * anything between reads as present -- the plate is a stencil, not a blend).
 *
 * MAGNIFIED: nearest neighbour, one source pixel per destination block, for
 * the reason ntr's own compose gives for the map -- a filter would invent
 * pixels the artist did not draw. That is the answer at every ratio above
 * 1:1, exact multiple or not, and it is the answer this file already gave.
 *
 * REDUCED: THE AREA AVERAGE, each source pixel counted by HOW MUCH OF IT the
 * destination pixel actually covers. It used to count whole source pixels --
 * the block from one integer boundary to the next -- and that is what put the
 * owner's MAP lettering out of focus at the small sizes. At the default map
 * the plate is drawn at eight fifteenths of its own size, so a destination
 * pixel covers 1.875 source ones: a whole-pixel block is therefore one pixel
 * wide about an eighth of the time and two the rest, so a letter's stroke is
 * copied at full ink in the wrong place on some columns and mixed half and
 * half with the plate on others. The eye reads that unevenness as blur. With
 * fractional coverage every column gets the same, correct share of the stroke
 * and the lettering holds its shape all the way down.
 *
 * The source's alpha is a stencil (0 or 255 is all this artwork has, and
 * anything between reads as present -- the plate is a stencil, not a blend),
 * so a pixel the artist left empty contributes nothing to the average and a
 * destination pixel with no opaque source under it is not drawn at all.
 *
 * AND IT IS DONE ONCE PER SIZE, NOT ONCE PER FRAME. See ArtScaled below. */
struct ArtScaled {
    const ArtImage *src;                 /* what it was made from */
    int sx, sy, sw, sh, dw, dh;          /* and at what ratio */
    int ox, oy;                          /* its corner, relative to the anchor */
    int w, h;
    unsigned *px;                        /* ARGB; alpha 0 means draw nothing */
};

/* how much of source pixel u, whose own span is [u*den, u*den+den), falls
   inside the destination pixel's span [A, B) -- all three in units of den */
inline long long art_cov(long long A, long long B, long long u, long long den)
{
    const long long lo = u * den, hi = lo + den;
    const long long l = A > lo ? A : lo, r = B < hi ? B : hi;
    return r > l ? r - l : 0;
}

inline long long lfdiv(long long a, long long b)
{
    return a >= 0 ? a / b : -((-a + b - 1) / b);
}

/* THE ARTWORK AT THE SIZE IT IS BEING DRAWN AT, KEPT.
 *
 * This is what lets the artist hand over a file of ANY size. Filtering a
 * picture properly costs work in proportion to the SOURCE's pixels, so a plate
 * four times as wide is sixteen times the work -- about a million weighted
 * samples a frame at the default map size, every frame, for a picture that has
 * not changed. Scaling it once and keeping the result costs that only when the
 * map's size changes, which is when the player drags the corner, and the draw
 * itself is then a copy whose cost depends on the size on screen and not at
 * all on the size of the file. A 4x file and a 1x file draw at the same speed.
 *
 * Rebuilt when any of the seven numbers that define the scaling changes, and
 * not otherwise. */
void art_scaled_build(ArtScaled *o, const ArtImage *im,
                      int sx, int sy, int sw, int sh, int dw, int dh)
{
    if (o->src == im && o->sx == sx && o->sy == sy && o->sw == sw
        && o->sh == sh && o->dw == dw && o->dh == dh && o->px)
        return;
    std::free(o->px);
    o->px = 0;
    o->src = im;
    o->sx = sx; o->sy = sy; o->sw = sw; o->sh = sh; o->dw = dw; o->dh = dh;
    o->ox = fdiv(-sx * dw, sw);
    o->oy = fdiv(-sy * dh, sh);
    o->w = fdiv((im->w - sx) * dw, sw) - o->ox;
    o->h = fdiv((im->h - sy) * dh, sh) - o->oy;
    if (o->w < 1 || o->h < 1) { o->w = o->h = 0; return; }
    o->px = (unsigned *)std::calloc((size_t)o->w * (size_t)o->h, 4u);
    if (!o->px) { o->w = o->h = 0; return; }

    const int shrinkx = dw < sw, shrinky = dh < sh;
    for (int Y = 0; Y < o->h; ++Y) {
        /* this row's span in source rows, in units of dh */
        const long long Ay = (long long)(o->oy + Y) * sh + (long long)sy * dh;
        const long long By = Ay + sh;
        int v0 = (int)lfdiv(Ay, dh);
        int v1 = shrinky ? (int)lfdiv(By + dh - 1, dh) : v0 + 1;
        if (v0 < 0) v0 = 0;
        if (v1 > im->h) v1 = im->h;
        if (v0 >= im->h || v1 <= v0) continue;
        for (int X = 0; X < o->w; ++X) {
            const long long Ax = (long long)(o->ox + X) * sw + (long long)sx * dw;
            const long long Bx = Ax + sw;
            int u0 = (int)lfdiv(Ax, dw);
            int u1 = shrinkx ? (int)lfdiv(Bx + dw - 1, dw) : u0 + 1;
            if (u0 < 0) u0 = 0;
            if (u1 > im->w) u1 = im->w;
            if (u0 >= im->w || u1 <= u0) continue;
            unsigned long long r = 0, g = 0, b = 0, t = 0;
            for (int v = v0; v < v1; ++v) {
                const long long cy = shrinky ? art_cov(Ay, By, v, dh) : dh;
                if (cy <= 0) continue;
                const unsigned *srow = im->px + (size_t)v * im->w;
                for (int u = u0; u < u1; ++u) {
                    const unsigned c = srow[u];
                    if (!(c >> 24)) continue;
                    const long long cx = shrinkx ? art_cov(Ax, Bx, u, dw) : dw;
                    if (cx <= 0) continue;
                    const unsigned long long k = (unsigned long long)(cx * cy);
                    r += k * ((c >> 16) & 0xFF);
                    g += k * ((c >> 8) & 0xFF);
                    b += k * (c & 0xFF);
                    t += k;
                }
            }
            if (!t) continue;        /* nothing opaque under it: stays empty */
            o->px[(size_t)Y * o->w + X] =
                0xFF000000u | ((unsigned)((r + t / 2) / t) << 16)
                | ((unsigned)((g + t / 2) / t) << 8) | (unsigned)((b + t / 2) / t);
        }
    }
    if (std::getenv("SM64DS_MINIMAP_TRACE"))
        std::fprintf(stderr, "[mmtrace] artwork scaled: %dx%d -> %dx%d at "
                     "%d/%d, %s\n", im->w, im->h, o->w, o->h, dw, sw,
                     (shrinkx || shrinky) ? "area average" : "nearest");
}

/* The copy. The anchor rectangle still decides where it lands: the scaled
   picture's own corner is the anchor plus the offset the scaling worked out,
   so the well still sits exactly on the map and the rest of the plate falls
   where that puts it. */
void art_draw(const ArtScaled *o, unsigned *dst, int w, int h, int dx, int dy)
{
    if (!o->px) return;
    for (int Y = 0; Y < o->h; ++Y) {
        const int D = dy + o->oy + Y;
        if (D < 0 || D >= h) continue;
        const unsigned *srow = o->px + (size_t)Y * o->w;
        unsigned *drow = dst + (size_t)D * ntr::SCREEN_W;
        for (int X = 0; X < o->w; ++X) {
            const int E = dx + o->ox + X;
            if (E < 0 || E >= w) continue;
            const unsigned c = srow[X];
            if (c >> 24) drow[E] = c;
        }
    }
}

ArtScaled g_scaled_base, g_scaled_tab;

void art_blit(ArtScaled *o, const ArtImage *im, unsigned *dst, int w, int h,
              int sx, int sy, int sw, int sh,
              int dx, int dy, int dw, int dh)
{
    if (sw < 1 || sh < 1 || dw < 1 || dh < 1) return;
    art_scaled_build(o, im, sx, sy, sw, sh, dw, dh);
    art_draw(o, dst, w, h, dx, dy);
}

void px_put(unsigned *dst, int dw, int dh, int x, int y, unsigned c)
{
    if (x < 0 || y < 0 || x >= dw || y >= dh) return;
    dst[y * ntr::SCREEN_W + x] = c;
}

void px_fill(unsigned *dst, int dw, int dh, int x0, int y0, int w, int h,
             unsigned c)
{
    for (int y = y0; y < y0 + h; ++y)
        for (int x = x0; x < x0 + w; ++x)
            px_put(dst, dw, dh, x, y, c);
}

/* ---- THE PANEL'S RECTANGLES, COMPUTED ONCE FOR BOTH PASSES ----------------
 *
 * THE PANEL IS DRAWN IN TWO PASSES NOW, on either side of the map, because
 * the owner asked for exactly that: "the MAP banner and text should be above
 * the minimap layer and the background should be behind the minimap layer".
 * So the plate goes down first, the map is composed over it, and the plaque
 * goes on top of the map. That is the whole reason this arithmetic is here
 * rather than inside one drawing function: two passes reading one set of
 * numbers cannot disagree about where the plaque is, and the plaque now
 * overlaps something (the map) where before it only ever overlapped the
 * plate.
 *
 * WHERE THE PLAQUE GOES IS MEASURED OFF THE ARTIST'S OWN MOCK-UP rather than
 * chosen: in it the plate picture's top row and the plaque's top row are the
 * SAME row, and the plaque is centred across the plate's width -- (255 - 109)
 * / 2 = 73, and 73 is where it sits. The plate picture carries ten fully
 * transparent rows at the top for exactly that, the notch the plaque drops
 * into, so the plate is blitted whole, notch and all, and its transparent
 * rows draw nothing.
 *
 * Returns 1 when the artwork is the panel, 0 when it is the composed one. */
struct PanelGeom {
    int x0, y0, pw, ph;        /* the plate, or the composed panel */
    int tx, ty, tw, th;        /* the MAP plaque */
    int wx, wy, ww, wh;        /* the artist's well, placed (art only) */
    int vx, vy;                /* the panel's top-left VISIBLE corner */
};

int panel_geom(PanelGeom *g)
{
    /* The artist's margins, carried onto whatever size the map is drawn at.
       Seven DS pixels proud on the left and top, eight on the right and
       bottom, at the map's own scale, and never less than one pixel so the
       smallest map still has a panel round it. */
    const int sx7 = ntr::SUB_W * 7 * g_pan_num / (256 * g_pan_den);
    const int sx8 = ntr::SUB_W * 8 * g_pan_num / (256 * g_pan_den);
    const int sy7 = ntr::SUB_H * 7 * g_pan_num / (192 * g_pan_den);
    const int sy8 = ntr::SUB_H * 8 * g_pan_num / (192 * g_pan_den);
    /* A FLOOR OF SIX PIXELS, because the artist's margin is a PROPORTION of
       his own 240-wide map and the smallest map here is 128 wide: three pixels
       of margin is one pixel of wallpaper once the frame lines have taken
       their share, which is a smudge rather than a panel. Above the smallest
       couple of sizes the proportion is what rules. */
    const int ml = sx7 < 6 ? 6 : sx7, mt = sy7 < 6 ? 6 : sy7;
    const int mr = sx8 < 7 ? 7 : sx8, mb = sy8 < 7 ? 7 : sy8;

    const int x0 = g_x0 - ml, y0 = g_y0 - mt;
    const int pw = g_pan_w + ml + mr, ph = g_pan_h + mt + mb;

    /* ---- THE MAP GOES INSIDE THE PICTURE'S OWN SQUARE ---------------------
     *
     * The owner: "make it match up with the outline and be inside the square
     * on the image i sent". So the plate is not scaled to some rectangle of
     * this program's choosing and the map dropped roughly in the middle of it;
     * the WELL the artist drew is placed exactly onto the map, and the rest of
     * the picture follows from that. plate_to_fb_x / _y below are that one
     * linear map, and everything -- the plate's rectangle, the plaque's, the
     * resize handle's corner -- is read out of it, so nothing can drift.
     *
     * ITS SHAPE. The map is 4:3 at every size and so, on the owner's picture,
     * is the well: 240 x 180 at 7,17 inside a 255 x 205 plate. When a picture
     * turns up whose well is NOT 4:3 the map keeps its own shape and is
     * centred in the well instead, with the plate scaled by whichever axis
     * binds, so the map is inscribed rather than stretched. The boot line says
     * which of the two the picture got. */
    if (art_on()) {
        const int bw = g_art_base.w, bh = g_art_base.h;
        const int iw = g_art_ix1 - g_art_ix0 + 1;
        const int ih = g_art_iy1 - g_art_iy0 + 1;
        if (iw > 0 && ih > 0 && g_pan_w > 0 && g_pan_h > 0) {
            /* the well, grown to hold the map: whichever axis binds sets the
               scale, and the map is centred across the other one */
            int wellw = g_pan_w, wellh = g_pan_h;
            if ((long)ih * g_pan_w >= (long)iw * g_pan_h)
                wellh = (int)((long)ih * g_pan_w / iw);
            else
                wellw = (int)((long)iw * g_pan_h / ih);
            const int gapx = (wellw - g_pan_w) / 2, gapy = (wellh - g_pan_h) / 2;
            g_art_gapx = gapx;
            g_art_gapy = gapy;
            const int wx = g_x0 - gapx, wy = g_y0 - gapy;

            const int px0 = wx + fdiv(-g_art_ix0 * wellw, iw);
            const int px1 = wx + fdiv((bw - g_art_ix0) * wellw, iw);
            const int py0 = wy + fdiv(-g_art_iy0 * wellh, ih);
            const int py1 = wy + fdiv((bh - g_art_iy0) * wellh, ih);
            const int tu = (bw - g_art_tab.w) / 2;
            const int tx0 = wx + fdiv((tu - g_art_ix0) * wellw, iw);

            g->x0 = px0;
            g->y0 = py0;
            g->pw = px1 - px0;
            g->ph = py1 - py0;
            g->tx = tx0;
            g->ty = py0;
            g->tw = fdiv(g_art_tab.w * wellw, iw);
            g->th = fdiv(g_art_tab.h * wellh, ih);
            g->wx = wx;
            g->wy = wy;
            g->ww = wellw;
            g->wh = wellh;
            /* the plate's OPAQUE top-left, which is the corner a player sees
               and therefore the corner the resize handle goes on: the notch
               rows above it are the hole the plaque drops into and draw
               nothing at all */
            g->vx = px0;
            g->vy = py0 + fdiv(g_art_notch * wellh, ih);
            return 1;
        }
    }

    g->x0 = x0;
    g->y0 = y0;
    g->pw = pw;
    g->ph = ph;
    g->tw = pw * 109 / 255;
    g->th = ph * 32 / 195;
    if (g->tw < 12) g->tw = 12;
    if (g->th < 6) g->th = 6;
    g->tx = x0 + (pw - g->tw) / 2;
    g->ty = y0 - ph * 10 / 195;
    g->wx = g_x0;
    g->wy = g_y0;
    g->ww = g_pan_w;
    g->wh = g_pan_h;
    g->vx = x0;
    g->vy = y0;
    return 0;
}

/* How far the panel reaches past the map on each side. Declared near the top
   of the file: the size has to be settled before anything is drawn, and what
   settles it is whether this fits. The rectangles are worked out with the map
   at the origin so the answers are offsets rather than positions, and the two
   globals that stand in for it are put back afterwards because the caller has
   not decided them yet this frame. */
void panel_extents(int *l, int *t, int *r, int *b)
{
    const int sx = g_x0, sy = g_y0;
    g_x0 = 0;
    g_y0 = 0;
    PanelGeom g;
    panel_geom(&g);
    g_x0 = sx;
    g_y0 = sy;

    int x0 = g.x0 < g.tx ? g.x0 : g.tx;
    int y0 = g.y0 < g.ty ? g.y0 : g.ty;
    int x1 = g.x0 + g.pw, y1 = g.y0 + g.ph;
    if (g.tx + g.tw > x1) x1 = g.tx + g.tw;
    if (g.ty + g.th > y1) y1 = g.ty + g.th;
    *l = x0 < 0 ? -x0 : 0;
    *t = y0 < 0 ? -y0 : 0;
    *r = x1 > g_pan_w ? x1 - g_pan_w : 0;
    *b = y1 > g_pan_h ? y1 - g_pan_h : 0;
}

/* ---- THE RESIZE HANDLE ----------------------------------------------------
 *
 * The owner: "could we actually make it where you can grab the top left
 * corner and put a yellow/tan square there to grab and resize the minimap so
 * its easy to scale it with window size", and then, on a draft that showed it
 * only under the pointer: "make the yellow always be up. the square".
 *
 * SO IT IS ALWAYS THERE. No hover test, no fade, nothing to discover: while
 * the improved map is on and the map is on screen, the square is on its
 * top-left corner. There is exactly one condition that takes it away, and it
 * is not a preference about clutter -- MouseCapture. With that key on the
 * window hides the pointer and pins it to the middle of the picture to steer
 * the camera with, so there is no cursor in the player's hand to grab
 * anything with and a handle would be a control that cannot be operated. The
 * key reloads live, so turning MouseCapture off in the launcher brings the
 * square back without restarting the game.
 *
 * ITS COLOURS ARE THE PANEL'S OWN: the brightest and darkest colours in the
 * artist's picture when the picture is what is drawn, and the same two the
 * composed panel takes from the player's palette when it is not. Nothing here
 * picks a colour.
 *
 * ITS SIZE IS THE MAP'S, NOT THE PICTURE'S. The owner, on seeing it against a
 * small map: "the yellow square on the minimap doesnt scale with map size so
 * when the map is small its massive". It used to be ten pixels against a
 * 384-row PICTURE, a number the map had no say in, so on the smallest map the
 * square took a thirteenth of the map's width and on the largest it was a
 * speck in the corner. It is EIGHT PERCENT OF THE MAP'S OWN WIDTH now,
 * rounded to a whole pixel, so it reads the same against the thing it resizes
 * at every size: 10 pixels on a 128-wide map, 15 on 192, 20 on 256.
 *
 * WITH A FLOOR AND A CEILING, both in picture pixels, because a share of a
 * width is the wrong answer at the two ends: under eight pixels the square is
 * too small to put a pointer on, and over thirty-two it stops being a grip and
 * starts covering the artist's corner. The ceiling binds from a map 400 wide
 * up, so the biggest maps all carry the same 32-pixel square.
 *
 * The picture's height is still the parameter and is no longer read. It stays
 * so that the draw, the trace and the press go on asking this one function,
 * which is the whole reason the square cannot drift from the thing you grab. */
int handle_rect(int *hx, int *hy, int *hs, int h)
{
    (void)h;
    if (!improved_map_on() || hal_sub_screen_stacked()) return 0;
    /* AND NOT WHILE THE PICTURE IS COMPOSED: the map is not on the picture
       at all for those frames, so there is no map corner to grab. One test,
       and it covers the draw and the press both, because both come through
       here. */
    if (g_swap) return 0;
    if (host_setting_mouse_capture()) return 0;
    PanelGeom g;
    panel_geom(&g);
    int s = (g_pan_w * 8 + 50) / 100;
    if (s < 8) s = 8;
    if (s > 32) s = 32;
    if (hx) *hx = g.vx;
    if (hy) *hy = g.vy;
    if (hs) *hs = s;
    return 1;
}

/* THE DRAG'S STATE, host statics rather than anything in .dsstate, for the
   reason poll_touch's own latch gives: a save-state load must not restore a
   half-finished drag onto a hand that is not holding the button any more. */
int g_handle_own;                /* this press started on the handle */
int g_handle_num0;               /* g_pan_num when it started */
int g_handle_bx0, g_handle_by0;  /* where in the picture it started */
long g_handle_grabs, g_handle_frames;   /* for the proof's census line */

/* THE PRESS, IF IT IS THE HANDLE'S. Returns 1 when the handle owns it, and
 * then the press reaches nothing else at all: no stylus record, so no touch
 * for the ROM to read and no camera button for it to hit-test. A drag that
 * started here goes on owning the button after the cursor has left the
 * square, which is what dragging a corner means; only letting go ends it.
 *
 * WHICH WAY IT SCALES. The map is anchored at its bottom-right corner, so
 * pulling the handle up and to the left makes it bigger. Both axes count: the
 * map keeps its 4:3 shape, so a pixel of height is worth four thirds of a
 * pixel of width, and the two are averaged. A pull straight left, straight up
 * or along the diagonal all do the sensible thing.
 *
 * THE SIZE MOVES LIVE AND IS WRITTEN DOWN ONLY ON RELEASE. A settings file
 * rewritten sixty times a second is a file the launcher would be reading
 * half-written. */
int handle_press(int bx, int by, int on_picture)
{
    int hx, hy, hs;
    if (!handle_rect(&hx, &hy, &hs, ntr::active_h)) {
        g_handle_own = 0;
        return 0;
    }
    if (!g_handle_own) {
        if (!on_picture) return 0;
        if (bx < hx || by < hy || bx >= hx + hs || by >= hy + hs) return 0;
        g_handle_own = 1;
        g_handle_num0 = g_pan_num;
        g_handle_bx0 = bx;
        g_handle_by0 = by;
        ++g_handle_grabs;
        std::fprintf(stderr, "[minimap] handle grabbed at picture (%d,%d); the "
                     "map is %dx%d and this press reaches nothing else\n",
                     bx, by, g_pan_w, g_pan_h);
    }
    ++g_handle_frames;

    const int dx = g_handle_bx0 - bx, dy = g_handle_by0 - by;
    const int dw = (3 * dx + 4 * dy) / 6;      /* pixels of map width */
    int num = g_handle_num0 + dw * g_pan_den / ntr::SUB_W;
    const int nmin = g_pan_den / 2 < 1 ? 1 : g_pan_den / 2;
    if (num < nmin) num = nmin;
    /* the ceiling is the drawing layer's: hal_sub_panel_geometry steps a size
       down until the whole panel fits the picture, and what it settles on is
       what gets written down when the button comes up */
    host_setting_minimap_scale_set_live(2.0 * num / g_pan_den);
    return 1;
}

/* THE BUTTON CAME UP. The size the drawing layer actually settled on -- which
   is the one the player is looking at, after its own clamp -- goes to
   settings.json, so it survives a restart and the launcher's picker opens on
   it. */
void handle_release(void)
{
    if (!g_handle_own) return;
    g_handle_own = 0;
    const double s = 2.0 * g_pan_num / g_pan_den;
    host_setting_save_minimap_scale(s);
    std::fprintf(stderr, "[minimap] resized to %g (map %dx%d) and saved\n",
                 s, g_pan_w, g_pan_h);
}

/* See the declaration near the top of the file. One line per change, so a
   windowed run that presents every frame prints one line and not sixty a
   second. */
void panel_trace(int w, int h)
{
    static int trace = -1;
    static int last[8];
    if (trace < 0) trace = std::getenv("SM64DS_MINIMAP_TRACE") ? 1 : 0;
    if (!trace) return;
    const int now[8] = { w, h, g_pan_num, g_pan_den, g_x0, g_y0,
                         g_pan_w, g_pan_h };
    int moved = 0;
    for (int i = 0; i < 8; ++i)
        if (now[i] != last[i]) { moved = 1; last[i] = now[i]; }
    if (!moved) return;
    PanelGeom g;
    const int art = panel_geom(&g);
    int hx = 0, hy = 0, hs = 0;
    handle_rect(&hx, &hy, &hs, h);
    std::fprintf(stderr,
                 "[mmtrace] geom: picture %dx%d scale %.5f ratio %d/%d "
                 "map %d,%d %dx%d panel %d,%d %dx%d plaque %d,%d %dx%d "
                 "handle %d,%d %d well %d,%d %dx%d gap %d,%d art %d\n",
                 w, h, 2.0 * g_pan_num / g_pan_den, g_pan_num, g_pan_den,
                 g_x0, g_y0, g_pan_w, g_pan_h, g.x0, g.y0, g.pw, g.ph,
                 g.tx, g.ty, g.tw, g.th, hx, hy, hs,
                 g.wx, g.wy, g.ww, g.wh, g_art_gapx, g_art_gapy, art);
}

/* ---- THE INSET MAP'S OWN SELFTEST -----------------------------------------
 *
 * SM64DS_LAYOUT_SELFTEST's other half. The one further down this file asserts
 * the STACKED layout and runs from the stacked image's own builder, so it
 * never fires in a course -- and a course is the only place the inset map
 * exists. This is the same idea over the inset arm, run once per process the
 * first time a course composes.
 *
 * WHAT IT ASSERTS, at every size including three that are not on the
 * launcher's picker:
 *   G1 the four corners of the DRAWN map, run back through the exact
 *      expression poll_touch's inset arm uses, land within ONE DS pixel of DS
 *      (0,0), (255,0), (0,191) and (255,191). One pixel is the floor for a
 *      reduced size, where one drawn pixel covers two DS ones and the last
 *      drawn column can only name the first of the two it covers.
 *   G2 one pixel outside each of the four edges reads OUTSIDE.
 *   G3 the map is 4:3 exactly.
 *   G4 the whole decorated panel is inside the picture.
 * G4 is the one that can legitimately fail: it is the clamp's own promise,
 * and a size that cannot keep it is supposed to have been stepped down before
 * it was drawn. */
void inset_map_selftest(int w, int h)
{
    const char *env = std::getenv("SM64DS_LAYOUT_SELFTEST");
    if (!env || !*env) return;
    /* 128 is scale 1 and 512 is scale 4; 220, 300 and 460 are off the
       launcher's picker on purpose, because the size is a free number now.
       Every one is on the grid -- a multiple of four -- because that is the
       only thing a size is allowed to be, and a fixture that asks for a size
       the program cannot produce is testing nothing. */
    static const int forced[9] = { 128, 160, 192, 220, 256, 300, 384, 460, 512 };
    const int s_num = g_pan_num, s_den = g_pan_den, s_w = g_pan_w,
              s_h = g_pan_h, s_x = g_x0, s_y = g_y0, s_m = g_pan_margin;

    int passed = 0;
    for (int i = 0; i < 9; ++i) {
        g_size_force = forced[i];
        hal_sub_panel_geometry(w, h);
        char fails[8];
        int nf = 0;
        const int mx1 = g_x0 + g_pan_w - 1, my1 = g_y0 + g_pan_h - 1;
        int dx, dy, ok = 1;
#define INV(bx, by) (dx = fdiv(((bx) - g_x0) * g_pan_den, g_pan_num), \
                     dy = fdiv(((by) - g_y0) * g_pan_den, g_pan_num))
        INV(g_x0, g_y0);
        if (dx > 1 || dy > 1 || dx < 0 || dy < 0) ok = 0;
        INV(mx1, g_y0);
        if (dx < ntr::SUB_W - 2 || dx > ntr::SUB_W - 1 || dy > 1) ok = 0;
        INV(g_x0, my1);
        if (dy < ntr::SUB_H - 2 || dy > ntr::SUB_H - 1 || dx > 1) ok = 0;
        INV(mx1, my1);
        if (dx < ntr::SUB_W - 2 || dy < ntr::SUB_H - 2) ok = 0;
        if (!ok) fails[nf++] = '1';
        ok = 1;
        INV(g_x0 - 1, g_y0);
        if (dx >= 0) ok = 0;
        INV(g_x0, g_y0 - 1);
        if (dy >= 0) ok = 0;
        INV(mx1 + 1, g_y0);
        if (dx < ntr::SUB_W) ok = 0;
        INV(g_x0, my1 + 1);
        if (dy < ntr::SUB_H) ok = 0;
#undef INV
        if (!ok) fails[nf++] = '2';
        if (g_pan_w * 3 != g_pan_h * 4) fails[nf++] = '3';
        int el, et, er, eb;
        panel_extents(&el, &et, &er, &eb);
        if (g_x0 - el < 0 || g_y0 - et < 0 ||
            g_x0 + g_pan_w + er > w || g_y0 + g_pan_h + eb > h)
            fails[nf++] = '4';
        fails[nf] = 0;
        if (!nf) {
            ++passed;
            std::fprintf(stderr, "[insetst] size %d: map %d,%d %dx%d panel "
                         "%d,%d %dx%d G1..G4 PASS\n", forced[i], g_x0, g_y0,
                         g_pan_w, g_pan_h, g_x0 - el, g_y0 - et,
                         el + g_pan_w + er, et + g_pan_h + eb);
        } else {
            std::fprintf(stderr, "[insetst] size %d: map %d,%d %dx%d panel "
                         "%d,%d %dx%d FAIL G%s\n", forced[i], g_x0, g_y0,
                         g_pan_w, g_pan_h, g_x0 - el, g_y0 - et,
                         el + g_pan_w + er, et + g_pan_h + eb, fails);
        }
    }
    std::fprintf(stderr, "INSET MAP SELFTEST: %s %d/9\n",
                 passed == 9 ? "PASS" : "FAIL", passed);
    g_size_force = 0;
    g_pan_num = s_num;
    g_pan_den = s_den;
    g_pan_w = s_w;
    g_pan_h = s_h;
    g_x0 = s_x;
    g_y0 = s_y;
    g_pan_margin = s_m;
}


/* ---- THE LEVEL-CLEAR SAVE MENU, COMPOSED ONTO THE TOP SCREEN --------------
 *
 * WHAT IT IS, in the owner's own words, on being shown the first version of
 * this (the two screens trading places): "I wanted those to show up on the
 * top screen. move the course 2 all that text up to the top of the screen,
 * put the coins total very bottom, remove touch to select and slot the
 * buttons in where the space gets freed up in the middle. All of this on the
 * top screen."
 *
 * SO THE BIG PICTURE IS COMPOSED OUT OF BOTH SCREENS while the menu is up,
 * rather than swapped with one of them. Three things are drawn and there is
 * no fourth:
 *
 *   1. the TOP screen's course-clear text block at the TOP of the picture,
 *   2. the BOTTOM screen's three button plates in the MIDDLE, as large as the
 *      space between (1) and (3) allows with their proportions kept,
 *   3. the TOP screen's coin total at the VERY BOTTOM.
 *
 * AND THE PICTURE UNDER ALL THREE IS THE TOP SCREEN'S OWN FRAME, UNMOVED.
 * That sentence is the correction of 09-21 and it is the whole of this block's
 * third cut. The first two cuts moved RECTANGLES OF THE FINISHED PICTURE: the
 * text band copied picture rows 68..231 up to rows 0..163 and the coin band
 * copied rows 244..287 down to 340..383, and each of those carried the 3D
 * scene behind the glyphs along with them. The picture's own top 68 rows were
 * therefore never drawn and its rows 164..231 and 244..287 were drawn twice,
 * which is what the owner saw: "why is the background picture like stretched?
 * The screen height shouldnt change, just slot it in."
 *
 * So no row of the picture moves any more. hal/message_compositor.cpp lifts
 * the LAYERS that carry the menu's lettering out of the frame as it composites
 * engine A -- the glyphs alone, not the scene behind them -- and this file
 * draws those lifted pixels at the top and at the bottom over the picture's
 * own rows. The two bands below are now destinations for an overlay and no
 * longer copies of anything. The banner over LcOverlay in that file carries
 * the per-layer census that says which layers those are (BG3 the course text,
 * the sprites the coin total, BG2 the "TOUCH TO SELECT" band, which is hidden
 * because the owner asked for it gone).
 *
 * Everything else of both screens -- the course scenery, the bottom screen's
 * map, and the backdrop the plates sit on -- is not drawn at all, and the
 * middle of the picture is the game's own, so what the player sees is three
 * buttons floating over it. The lives counter in the top-left corner is the
 * top screen's own and STAYS WHERE IT IS: it is not re-slotted anywhere, the
 * picture's own top rows are in the picture now, and it does not touch the
 * lettering (the counter is x 12..95, the text x 96..393). The frame the menu
 * is answered the picture goes back to the ordinary one.
 * The ROM draws exactly what it drew before: THIS IS COMPOSITING OF THE TWO
 * FRAMES THE GAME ALREADY PRODUCED, and nothing else changes.
 *
 * THE TRIGGER IS THE ROM'S OWN STATE AND NEVER A TIMER, and it is a PAIR:
 *
 *      data_0209f20c != 0   AND   data_0209f2d4 between 1 and 3
 *
 * Read straight out of src/_ZN5Stage9LC_UpdateEv.cpp. Case 0 calls
 * Stage::UpdateMenuButtons(1) -- the call that recolours BG1's screen entries
 * into the three plates -- and sets data_0209f2d4 = 1 in the same statement
 * block, so the plates are on from the frame that word becomes 1. Every one
 * of the three answer arms in case 3 calls Stage::UpdateMenuButtons(0) and
 * moves the state to 4 or to 6 in the same tick, so the plates are off from
 * the frame the answer is taken. The word is 0 before case 0 runs and is put
 * back to 0 by case 6.
 *
 * WHY THE PAIR AND NOT THE STATE ALONE: data_0209f2d4 is only a state word
 * while the level-clear machinery is running at all, and what says it is
 * running is data_0209f20c -- Stage::Behavior's own gate. The pair is true
 * over exactly the frames the plates are on the screen and over no others.
 *
 * WHERE THE RECTANGLES COME FROM: this one place, once a frame, exactly the
 * way hal_sub_panel_geometry is the one place that decides where the map
 * panel is. The compose draws these rectangles and poll_touch's inverse reads
 * these same rectangles; neither recomputes the other's arithmetic. That is
 * the rule this file already lives by and the compose gets no exception.
 *
 * WHICH ROWS OF THE TOP SCREEN, AND THEY ARE MEASURED. A pixel census of the
 * top screen taken while the menu is up (the star row, the stacked layout so
 * the picture is the top screen alone at 512x384, frame 900):
 *
 *   the message glyphs are drawn in exactly one colour, (144,144,144), and in
 *   columns 110..400 they fall in four bands -- picture rows 72..93 the
 *   "COURSE N" line, 104..125 the course's name, 168..189 the star's name (or
 *   the silver-star line), 200..221 "CLEAR!". The blank line slot between
 *   them at 126..167 is the cartridge's own layout and is kept, because this
 *   is a composite of what the game drew and not a re-typesetting of it. Five
 *   line slots of 16 DS rows from DS row 36 -- so the block is DS rows
 *   36..115, and kTextRow0/kTextRow1 take one row of margin either side.
 *
 *   the coin total is drawn in orange, picture rows 251..283 (the coin sprite
 *   and the "x N" beside it): DS rows 125..141, and kCoinRow0/kCoinRow1 take
 *   the margin the same way.
 *
 *   AND "TOUCH TO SELECT" IS ON THE TOP SCREEN, not on the bottom one: it is
 *   an orange band low on the picture, and the per-layer census of 09-21 puts
 *   it exactly at picture rows 336..367, x 54..457, on engine A's BG2 and
 *   nothing else. With the picture no longer cropped it would be back in view,
 *   so the compose now HIDES that layer outright while it is up. That is the
 *   whole of "remove touch to select".
 *
 *   The same census re-measured the other two, and these are the numbers this
 *   block's row constants answer to: the course text is engine A's BG3, every
 *   one of its 5971 pixels the colour (144,144,144), picture rows 72..221,
 *   x 96..393; the coin total is a sprite, picture rows 256..287, x 196..271.
 *   Both lie inside the bands below, which is what makes the lift lossless.
 *
 * THE TWO BANDS ARE WHERE THE LIFTED GLYPHS GO, row for row, at the size the
 * top screen was already drawn at -- the same number of picture rows out as
 * in, the full picture width, no resampling of any kind. So the text is the
 * size the player is used to seeing it, at every render scale and every
 * aspect, because it is literally the same pixels moved up or down the
 * picture. What they are drawn OVER is the picture's own rows, untouched.
 *
 * THE PLATES TAKE WHAT IS LEFT. DS rows 0x28..0x97 of the bottom screen --
 * the three touch boxes at 0x28, 0x50 and 0x78, each 0x20 tall, and the two
 * eight-row gaps the cartridge leaves between them -- scaled by the largest
 * fraction that fits the band between the two text bands with 256:112 kept,
 * and centred in it. Nearest neighbour, because the plates are pixel art.
 * MEASURED AGAINST THE ALTERNATIVE rather than assumed: at the default
 * 512x384 picture the band is 176 rows, so the largest INTEGER multiple that
 * fits is 1 (a 256x112 rectangle, 32 per cent of the band) against the fitted
 * 11/7 (402x176, 90 per cent of it, and the full height). The integer fit
 * wastes two thirds of the space the owner asked the buttons to be slotted
 * into, so the fitted fraction is what is drawn.
 */
struct SwapGeom {
    /* the three plates: the only part of the bottom screen that is drawn, and
       the only part of the picture a stylus reaches */
    int px, py, pw, ph;
    int pnum, pden;              /* their size as a fraction of DS pixels */
    /* the two bands of the top screen's LETTERING. A band is a straight row
       move, so its height is the same number on both sides and only its
       origin changes; what travels is the lifted glyph pixels alone. */
    int ty_src, ty_dst, t_h;     /* the course-clear text block, at the top */
    int cy_src, cy_dst, c_h;     /* the coin total, at the very bottom */
};
SwapGeom g_sw;

/* The option, asked once per process and latched, like the improved map's. */
int save_menu_on_top(void)
{
    static int v = -1;
    if (v < 0) v = host_setting_save_menu_on_top() ? 1 : 0;
    return v;
}

/* THE PAIR. See the banner. */
int save_menu_is_up(void)
{
    return data_0209f20c != 0 && data_0209f2d4 >= 1 && data_0209f2d4 <= 3;
}

/* The bottom screen's plate span: the first plate's top row, and the row one
   past the third plate. The three offsets are LC_Update's own, out of case
   3's touch tests. */
const int kMenuFirstRow = 0x28;
const int kMenuLastRow = 0x78 + 0x20;
const int kMenuRows = kMenuLastRow - kMenuFirstRow;

/* AND THE PLATES INSIDE THAT SPAN, because the span is not all plate. The
   three touch boxes are 0x20 rows on a 0x28 pitch, so six rows of every
   forty belong to neither box -- and a census of the bottom screen's own
   raster while the menu is up says what is in them: THE LEVEL MAP. Engine B
   draws the course map behind the menu, and rows 0x4a..0x4f carry 169 to 172
   pixels of it each at x 45..216, rows 0x72..0x77 the same at x 41..213.
   They are not backdrop and no colour key can reach them.

   A PLATE IS TWO ROWS TALLER THAN ITS TOUCH BOX. Under each box's last body
   row the cartridge draws a solid black edge and a rounded shadow row under
   that -- a full-width run at 0x48 and a 238-wide one at 0x49, and the same
   pair at 0x70/0x71 and at 0x98/0x99 -- matching the rounded top edge at
   0x28. Both rows are plate and both are drawn, so a plate is 0x22 rows from
   its box's first row. The third plate's pair falls past kMenuLastRow and
   stays clipped away, which is what the span has always done with it. */
const int kPlatePitch = 0x28;
const int kPlateRows = 0x22;

/* Is this bottom-screen row one of the three plates' own rows? */
int menu_plate_row(int sy)
{
    const int o = sy - kMenuFirstRow;
    return o >= 0 && o < kMenuRows && o % kPlatePitch < kPlateRows;
}

/* THE COLOUR THE PLATES SIT ON, read off the machine rather than guessed.
   The PPU's backdrop is entry 0 of the engine's own BG palette -- engine B's
   is at 0x05000400 -- and it is the colour ppu_scanout_sub starts every
   pixel of the bottom screen from, so a pixel that is still it is a pixel
   engine B drew nothing on. Two things happen to it before it reaches the
   raster and both happen here: BGR555 is widened to eight bits a channel the
   way ntr's own bgr555 does it, and engine B's MASTER_BRIGHT (0x0400106C,
   mode in bits 14-15, factor in bits 0-4) is applied the way
   ppu_scanout_sub applies it to every pixel it writes -- without that a
   fading bottom screen would carry a backdrop this key no longer matched.
   Measured on the level-clear menu: palette entry 0x0FAB and the brightness
   unit off, so the raster's backdrop is (90,239,24), which the ROM's own -7
   on the finished picture then darkens to the (51,135,14) a capture shows.
   That pair is the fallback value if this read ever has to be replaced by a
   constant. */
unsigned menu_backdrop_px(void)
{
    const unsigned c = *(volatile unsigned short *)0x05000400u;
    const unsigned r5 = c & 0x1Fu, g5 = (c >> 5) & 0x1Fu,
                   b5 = (c >> 10) & 0x1Fu;
    int r = (int)(r5 << 3 | r5 >> 2), g = (int)(g5 << 3 | g5 >> 2),
        b = (int)(b5 << 3 | b5 >> 2);
    const unsigned mb = *(volatile unsigned short *)0x0400106Cu;
    const unsigned mode = (mb >> 14) & 3u;
    int f = (int)(mb & 0x1Fu);
    if (f > 16) f = 16;
    if (f && mode == 1) {
        r += (255 - r) * f / 16;
        g += (255 - g) * f / 16;
        b += (255 - b) * f / 16;
    } else if (f && mode == 2) {
        r -= r * f / 16;
        g -= g * f / 16;
        b -= b * f / 16;
    }
    return ((unsigned)r << 16) | ((unsigned)g << 8) | (unsigned)b;
}

/* The top screen's two bands, in DS rows. Both measured; see the banner. */
const int kTextRow0 = 0x22;      /* 34: one row above the COURSE N line */
const int kTextRow1 = 0x74;      /* 116: one row below CLEAR! */
const int kCoinRow0 = 0x7a;      /* 122 */
const int kCoinRow1 = 0x90;      /* 144 */

void swap_geom_for(int w, int h, SwapGeom *o)
{
    SwapGeom g;
    std::memset(&g, 0, sizeof g);
    if (w < 8 || h < 8) { *o = g; return; }
    /* THE TWO BANDS FIRST, because what is left over is the plates'.
       A DS ROW OF THE TOP SCREEN IS (h / SUB_H) PICTURE ROWS, FLOORED, and
       the floor is the correction of 09-21 rather than a rounding taste. That
       integer is the scale engine A's own compositor draws its 2D at
       (hal/message_compositor.cpp: sy = active_h / 192), so it is where the
       lettering this band carries actually IS. The old form multiplied first
       -- kTextRow0 * h / SUB_H -- which agrees at every picture whose height
       is a whole multiple of 192 (the 512x384 default, RenderScale 4, 16:9)
       and disagrees at the two that are not: at 21:9 the picture is 1024x440,
       the 2D is drawn at 2x, and the old text band began at picture row 77
       while the first glyph row is 72, so the top five rows of "COURSE N"
       fell outside it; at 32:9 the coin band began at row 183 and the coin
       total is drawn at 256..287, so it missed the coins entirely. Flooring
       first puts every band on the rows the glyphs are on, at every shape. */
    int vs = h / ntr::SUB_H;
    if (vs < 1) vs = 1;
    g.t_h = (kTextRow1 - kTextRow0) * vs;
    g.c_h = (kCoinRow1 - kCoinRow0) * vs;
    if (g.t_h < 1) g.t_h = 1;
    if (g.c_h < 1) g.c_h = 1;
    /* a picture too short to hold both bands and a plate between them gives
       each band a third of it rather than drawing them over each other */
    if (g.t_h + g.c_h > h - 4) {
        g.t_h = h / 3;
        g.c_h = h / 3;
    }
    g.ty_src = kTextRow0 * vs;
    if (g.ty_src + g.t_h > h) g.ty_src = h - g.t_h;
    if (g.ty_src < 0) g.ty_src = 0;
    g.ty_dst = 0;
    g.cy_src = kCoinRow0 * vs;
    if (g.cy_src + g.c_h > h) g.cy_src = h - g.c_h;
    if (g.cy_src < 0) g.cy_src = 0;
    g.cy_dst = h - g.c_h;
    /* THE MIDDLE, AND THE PLATES IN IT. The largest fraction that fits with
       256:112 kept: the width binds or the height does, and whichever binds
       is the fraction, reduced so the compose and the inverse are handed
       small numbers. */
    const int my0 = g.t_h, mh = g.cy_dst - my0;
    if (mh < 4) { *o = g; return; }
    int num, den;
    if ((long long)w * kMenuRows <= (long long)mh * ntr::SUB_W) {
        num = w;
        den = ntr::SUB_W;
    } else {
        num = mh;
        den = kMenuRows;
    }
    if (num < 1) num = 1;
    if (den < 1) den = 1;
    {
        int a = num, b = den;
        while (b) { const int t = a % b; a = b; b = t; }
        if (a > 1) { num /= a; den /= a; }
    }
    g.pnum = num;
    g.pden = den;
    g.pw = ntr::SUB_W * num / den;
    g.ph = kMenuRows * num / den;
    if (g.pw > w) g.pw = w;
    if (g.ph > mh) g.ph = mh;
    g.px = (w - g.pw) / 2;
    g.py = my0 + (mh - g.ph) / 2;
    if (g.px < 0) g.px = 0;
    if (g.py < my0) g.py = my0;
    *o = g;
}

/* SM64DS_SWAP_TRACE=1: every change of the ROM's two words and of the
   compose's own answer, with the rectangles this frame settled on. It exists
   so that a capture can be measured against the numbers the program actually
   drew, and so that the frame the compose begins can be joined to the frame
   the plates turn on, rather than to a restatement of this arithmetic
   somewhere else. The env name is the one the tools already pass through. */
void swap_trace(int w, int h, int frame)
{
    static int on = -1;
    static int last[3] = { -1, -1, -1 };
    if (on < 0) on = std::getenv("SM64DS_SWAP_TRACE") ? 1 : 0;
    if (!on) return;
    const int now[3] = { (int)data_0209f20c, (int)data_0209f2d4, g_swap };
    if (now[0] == last[0] && now[1] == last[1] && now[2] == last[2]) return;
    last[0] = now[0];
    last[1] = now[1];
    last[2] = now[2];
    std::fprintf(stderr, "[compose] f%d f20c=%d f2d4=%d composed=%d picture "
                 "%dx%d text 0,%d %dx%d from row %d  plates %d,%d %dx%d "
                 "(%d/%d)  coins 0,%d %dx%d from row %d\n", frame, now[0],
                 now[1], now[2], w, h, g_sw.ty_dst, w, g_sw.t_h,
                 g_sw.ty_src, g_sw.px, g_sw.py, g_sw.pw, g_sw.ph, g_sw.pnum,
                 g_sw.pden, g_sw.cy_dst, w, g_sw.c_h, g_sw.cy_src);
    std::fflush(stderr);
}

/* engine B's own raster target, defined with the rest of the raster state
   further down this file and named here because the composed present is above
   it. Same object, same internal linkage. */
extern ntr::SubFramebuffer g_sub;

/* One band of the LIFTED LETTERING, moved up or down the picture. Both the
   overlay and the framebuffer are indexed at a stride of SCREEN_W -- the
   overlay is written by hal/message_compositor.cpp's own blit, at the host
   pixels that blit would have written -- so a band is a row offset and
   nothing else, and only the pixels the overlay marks opaque are drawn. What
   is not marked is left alone, which is how the game's own picture comes
   through between the glyphs. */
void swap_glyph_band(unsigned *dst, int w, int h, int src_y, int dst_y,
                     int rows)
{
    if (rows < 1 || src_y < 0 || dst_y < 0) return;
    if (src_y + rows > h || dst_y + rows > h) return;
    if (!hal_lc_overlay_live()) return;
    const unsigned *col = hal_lc_overlay_colour();
    const unsigned char *msk = hal_lc_overlay_mask();
    if (!col || !msk) return;
    for (int y = 0; y < rows; ++y) {
        const size_t s = (size_t)(src_y + y) * ntr::SCREEN_W;
        const size_t d = (size_t)(dst_y + y) * ntr::SCREEN_W;
        for (int x = 0; x < w; ++x)
            if (msk[s + x]) dst[d + x] = col[s + x];
    }
}

/* THE COMPOSED PRESENT. The picture is already the game's own frame with the
   menu's lettering lifted out of it, so nothing here moves a row of it: the
   two bands below put the lifted glyphs at the top and at the bottom, and the
   three plates go over the middle. */
void swap_present(unsigned *dst, int w, int h)
{
    if (w <= 0 || h <= 0 || g_sw.pnum <= 0) return;
    /* the lift's promise, honoured: see g_lc_lift_pending's banner */
    g_lc_lift_pending = 0;
    /* WHAT IS UNDER ALL THREE IS THE GAME'S OWN PICTURE, EVERY ROW OF IT
       WHERE THE GAME PUT IT. The owner's two rulings are one ruling: "I want
       just the buttons not the whole background" and "the screen height
       shouldnt change, just slot it in". So nothing here blacks, clears,
       copies or scrolls the picture, and the only pixels written are plate
       and lifted glyph. */
    swap_glyph_band(dst, w, h, g_sw.ty_src, g_sw.ty_dst, g_sw.t_h);
    swap_glyph_band(dst, w, h, g_sw.cy_src, g_sw.cy_dst, g_sw.c_h);
    /* THE THREE PLATES, nearest neighbour out of engine B's own raster, and
       only the plates. TWO TESTS, both on the SOURCE pixel, before the ROM's
       own fade reaches the finished picture, and neither of them moves a
       rectangle: a source row between two plates is not drawn at all (it is
       the course map, see the banner over kPlateRows), and a source pixel
       still carrying the backdrop is not drawn either (the eight columns of
       margin either side of every plate, and the rounded corners). What
       lands on the picture is the plate, its outline and its lettering. */
    const unsigned key = menu_backdrop_px();
    for (int y = 0; y < g_sw.ph; ++y) {
        int sy = kMenuFirstRow + (int)(((long long)y * g_sw.pden) / g_sw.pnum);
        if (sy >= kMenuLastRow) sy = kMenuLastRow - 1;
        if (!menu_plate_row(sy)) continue;
        const unsigned *srow = g_sub.px[sy];
        for (int x = 0; x < g_sw.pw; ++x) {
            int sx = (int)(((long long)x * g_sw.pden) / g_sw.pnum);
            if (sx >= ntr::SUB_W) sx = ntr::SUB_W - 1;
            const unsigned c = srow[sx] & 0x00FFFFFFu;
            if (c == key) continue;
            px_put(dst, w, h, g_sw.px + x, g_sw.py + y, 0xFF000000u | c);
        }
    }
}

/* THE INVERSE, and it is the one arithmetic. A client point that has already
   been put into framebuffer pixels comes in; the DS pixel the ROM will see
   goes out, read off the very rectangle the compose just drew. A point off
   the plates is refused (inside = 0) rather than mapped, because nothing else
   on the composed picture is a touch surface: not the course text, not the
   coin row, and not the game's own picture either side of the plates. */
void swap_inverse(int bx, int by, int *dsx, int *dsy, int *inside)
{
    const SwapGeom &g = g_sw;
    int qx = bx, qy = by, on = 1;
    if (g.pnum < 1 || g.pw < 1 || g.ph < 1) {
        *dsx = -1;
        *dsy = -1;
        *inside = 0;
        return;
    }
    if (qx < g.px) { qx = g.px; on = 0; }
    if (qx >= g.px + g.pw) { qx = g.px + g.pw - 1; on = 0; }
    if (qy < g.py) { qy = g.py; on = 0; }
    if (qy >= g.py + g.ph) { qy = g.py + g.ph - 1; on = 0; }
    int x = fdiv((qx - g.px) * g.pden, g.pnum);
    int y = kMenuFirstRow + fdiv((qy - g.py) * g.pden, g.pnum);
    if (x < 0) x = 0;
    if (x >= ntr::SUB_W) x = ntr::SUB_W - 1;
    if (y < kMenuFirstRow) y = kMenuFirstRow;
    if (y >= kMenuLastRow) y = kMenuLastRow - 1;
    *dsx = x;
    *dsy = y;
    *inside = on;
}


/* ---- THE COMPOSED ARM OF SM64DS_LAYOUT_SELFTEST ---------------------------
 *
 * The third arm of the same selftest. The stacked arm asserts the stacked
 * layout and the inset arm asserts the map panel; this one asserts the
 * composed picture, over the shapes the option can be asked for: both
 * framebuffer tiers and the five aspects the launcher offers. It runs the
 * SAME two functions the compose and the stylus inverse run -- nothing here
 * restates that arithmetic -- and it needs no game state, because the
 * rectangles depend only on the picture.
 *
 *   S1 the four corners of the PLATE rectangle, run back through
 *      swap_inverse, land within ONE DS pixel of DS (0,0x28), (255,0x28),
 *      (0,0x97) and (255,0x97) and read INSIDE.
 *   S2 one pixel outside each of its four edges reads OUTSIDE.
 *   S3 the plate rectangle keeps 256:112 within a pixel, fits in the band
 *      between the two text bands, and the three bands neither overlap each
 *      other nor leave the picture.
 *   S4 THE THREE BUTTON RECTS: each of the menu's own touch boxes (DS rows
 *      0x28, 0x50 and 0x78, each 0x20 tall) lands as a whole rectangle inside
 *      the plate rectangle, and the centre of each maps back into that same
 *      DS box -- which is the click a player makes, asserted through the
 *      inverse rather than described.
 */
void swap_layout_selftest(void)
{
    const char *env = std::getenv("SM64DS_LAYOUT_SELFTEST");
    if (!env || !*env) return;
    static const double asp[5] = { 0.0, 1.7777778, 2.3333333, 3.5555556, 4.0 };
    static const int hs[2] = { 384, 768 };
    const SwapGeom saved = g_sw;
    int passed = 0, total = 0;
    for (int hi = 0; hi < 2; ++hi) {
        for (int ai = 0; ai < 5; ++ai) {
            const int h = hs[hi];
            int w = asp[ai] > 0.0 ? (int)(h * asp[ai] + 0.5) : h * 4 / 3;
            w &= ~1;
            swap_geom_for(w, h, &g_sw);
            const SwapGeom g = g_sw;
            char fails[8];
            int nf = 0, dx, dy, in, ok = 1;
            const int x1 = g.px + g.pw - 1, y1 = g.py + g.ph - 1;
            swap_inverse(g.px, g.py, &dx, &dy, &in);
            if (!in || dx > 1 || dy > kMenuFirstRow + 1) ok = 0;
            swap_inverse(x1, g.py, &dx, &dy, &in);
            if (!in || dx < ntr::SUB_W - 2 || dy > kMenuFirstRow + 1) ok = 0;
            swap_inverse(g.px, y1, &dx, &dy, &in);
            if (!in || dx > 1 || dy < kMenuLastRow - 2) ok = 0;
            swap_inverse(x1, y1, &dx, &dy, &in);
            if (!in || dx < ntr::SUB_W - 2 || dy < kMenuLastRow - 2) ok = 0;
            if (!ok) fails[nf++] = '1';
            ok = 1;
            swap_inverse(g.px - 1, g.py, &dx, &dy, &in);
            if (in) ok = 0;
            swap_inverse(g.px, g.py - 1, &dx, &dy, &in);
            if (in) ok = 0;
            swap_inverse(x1 + 1, g.py, &dx, &dy, &in);
            if (in) ok = 0;
            swap_inverse(g.px, y1 + 1, &dx, &dy, &in);
            if (in) ok = 0;
            if (!ok) fails[nf++] = '2';
            ok = 1;
            if (g.pw < 1 || g.ph < 1) ok = 0;
            else {
                const long long l = (long long)g.pw * kMenuRows;
                const long long r = (long long)g.ph * ntr::SUB_W;
                const long long tol = (long long)ntr::SUB_W + kMenuRows;
                if (l - r > tol || r - l > tol) ok = 0;
            }
            if (g.px < 0 || g.py < g.t_h || g.px + g.pw > w ||
                g.py + g.ph > g.cy_dst)
                ok = 0;
            if (g.t_h < 1 || g.c_h < 1 || g.ty_dst != 0 ||
                g.cy_dst + g.c_h != h || g.ty_src + g.t_h > h ||
                g.cy_src + g.c_h > h)
                ok = 0;
            if (!ok) fails[nf++] = '3';
            ok = 1;
            for (int b = 0; b < 3; ++b) {
                const int d0 = kMenuFirstRow + b * 0x28;
                const int d1 = d0 + 0x20 - 1;
                const int r0 = g.py + (d0 - kMenuFirstRow) * g.pnum / g.pden;
                const int r1 = g.py + (d1 + 1 - kMenuFirstRow) * g.pnum /
                                      g.pden - 1;
                if (r0 < g.py || r1 > g.py + g.ph - 1 || r1 < r0) ok = 0;
                swap_inverse(g.px + g.pw / 2, (r0 + r1) / 2, &dx, &dy, &in);
                if (!in || dy < d0 || dy > d1 || dx < 0 || dx >= ntr::SUB_W)
                    ok = 0;
            }
            if (!ok) fails[nf++] = '4';
            fails[nf] = 0;
            ++total;
            if (!nf) {
                ++passed;
                std::fprintf(stderr, "[composest] picture %dx%d: text 0,0 "
                             "%dx%d from row %d  plates %d,%d %dx%d (%d/%d)  "
                             "coins 0,%d %dx%d from row %d  S1..S4 PASS\n",
                             w, h, w, g.t_h, g.ty_src, g.px, g.py, g.pw, g.ph,
                             g.pnum, g.pden, g.cy_dst, w, g.c_h, g.cy_src);
            } else {
                std::fprintf(stderr, "[composest] picture %dx%d: text 0,0 "
                             "%dx%d from row %d  plates %d,%d %dx%d (%d/%d)  "
                             "coins 0,%d %dx%d from row %d  FAIL S%s\n",
                             w, h, w, g.t_h, g.ty_src, g.px, g.py, g.pw, g.ph,
                             g.pnum, g.pden, g.cy_dst, w, g.c_h, g.cy_src,
                             fails);
            }
        }
    }
    g_sw = saved;
    std::fprintf(stderr, "COMPOSED LAYOUT SELFTEST: %s %d/%d\n",
                 passed == total ? "PASS" : "FAIL", passed, total);
    std::fflush(stderr);
}

/* Is the picture COMPOSED right now, for a reader that is not the compose?
   The rectangles are recomputed every frame whatever the answer, so a reader
   asking this on the same frame the plates come on gets the rectangles that
   frame's compose is using.

   THIS IS THE COMPOSE'S ONE TRUTH, AND EVERY TERM THE DRAW DEPENDS ON HAS TO
   BE IN IT. The engine-A compositor asks this question -- through
   hal_lc_compose_rows, below -- to decide whether to LIFT the level-clear
   lettering and the coin sprites out of the picture, and swap_present is what
   puts them back. The two halves are one mechanism, so a condition that stops
   the draw without stopping the lift takes those pixels out of the picture
   and never puts them back.

   g_on -- the bottom-screen panel option, SM64DS_SUB_PANEL -- was exactly
   such a term. hal_sub_screen_present has one early return, `if (!g_on)
   return;`, and the compose's call to swap_present is below it, so with the
   panel off the lift ran and the draw did not: the course text, the coin
   total and "TOUCH TO SELECT" came out of the top screen and nothing was
   drawn back. That is the 0.4.1 gate's one red row -- the level-clear picture
   went course-independent, the same picture for Jolly Roger Bay and for
   Bob-omb Battlefield, and its whole difference from the game's own picture
   was BG3's 5971 glyph pixels plus the coin sprites' 1792 plus BG2's 8556,
   with nothing drawn in their place.

   Hoisting the draw above that early return is the other repair and it is the
   wrong one: with the panel off engine B is never scanned out at all (the
   `if (g_on) ppu_scanout_sub` a few lines above the return), so the plates
   would have no source to draw from. With the panel off the compose does not
   happen and the player keeps the game's own untouched top screen, text,
   coins and all.

   Anything added to hal_sub_screen_present that can skip swap_present belongs
   in this predicate too. The one-shot note in hal_lc_compose_rows below is
   the net for the day somebody forgets. */
int swap_now(void)
{
    return save_menu_on_top() && save_menu_is_up() &&
           !hal_sub_screen_stacked() && g_on && g_sw.pnum > 0;
}

/* The square itself, drawn in the pass above the map so nothing covers it. */
void hal_sub_panel_handle(unsigned *dst, int w, int h)
{
    int hx, hy, hs;
    if (!handle_rect(&hx, &hy, &hs, h)) return;
    const int art = (g_art_state == 1);
    const unsigned bright = art ? g_art_bright
                                : ((g_motif_state == 1) ? g_panel_bright
                                                        : 0xFFE0C060u);
    const unsigned dark = art ? g_art_dark
                              : ((g_motif_state == 1) ? g_panel_dark
                                                      : 0xFF201408u);
    px_fill(dst, w, h, hx, hy, hs, hs, bright);
    px_fill(dst, w, h, hx, hy, hs, 1, dark);
    px_fill(dst, w, h, hx, hy + hs - 1, hs, 1, dark);
    px_fill(dst, w, h, hx, hy, 1, hs, dark);
    px_fill(dst, w, h, hx + hs - 1, hy, 1, hs, dark);
}

/* THE PANEL, DRAWN BEHIND THE MAP. Called from hal_sub_screen_present with the
   framebuffer, just before the map itself is composed over the middle of it.
   The plaque is NOT drawn here: it belongs above the map and is the second
   pass, hal_sub_panel_plaque below. */
void hal_sub_panel_decor(unsigned *dst, int w, int h)
{
    /* the archive read only matters when this program has to compose the
       panel itself, so a player with the artwork never pays for it */
    if (!art_on() && !g_motif_state) g_motif_state = wall_build() ? 1 : -1;

    PanelGeom g;
    if (panel_geom(&g)) {
        /* THE WELL ONTO THE MAP, and the rest of the picture wherever that
           puts it. See the banner over art_blit. */
        art_blit(&g_scaled_base, &g_art_base, dst, w, h,
                 g_art_ix0, g_art_iy0,
                 g_art_ix1 - g_art_ix0 + 1, g_art_iy1 - g_art_iy0 + 1,
                 g.wx, g.wy, g.ww, g.wh);
        return;
    }
    const int x0 = g.x0, y0 = g.y0, pw = g.pw, ph = g.ph;

    /* the two margins the frame lines are proportioned from, read back out of
       the rectangle rather than recomputed, so there is still one arithmetic */
    const int ml = g_x0 - x0, mr = x0 + pw - (g_x0 + g_pan_w);

    const unsigned flat = (g_motif_state == 1) ? g_motif[0][0] : 0xFF4A3B18u;
    const unsigned dark = (g_motif_state == 1) ? g_panel_dark : 0xFF201408u;

    /* the wallpaper, at the map's own pixel grid so the panel and the map read
       as one picture; a plain fill if the archive could not be read */
    /* The column's motif index, worked out once for the row loop rather than
       twice per pixel: the biggest panel is a whole framebuffer of them. */
    static unsigned char col[ntr::SCREEN_W];
    for (int x = x0 < 0 ? 0 : x0; x < x0 + pw && x < w; ++x)
        col[x] = (unsigned char)(((x - x0) * g_pan_den / g_pan_num) & 31);
    for (int y = y0; y < y0 + ph; ++y) {
        if (y < 0 || y >= h) continue;
        const unsigned *row = g_motif[((y - y0) * g_pan_den / g_pan_num) & 31];
        for (int x = x0 < 0 ? 0 : x0; x < x0 + pw && x < w; ++x)
            dst[y * ntr::SCREEN_W + x] =
                (g_motif_state == 1) ? row[col[x]] : flat;
    }

    /* THE FRAME, one artist pixel thick on the left and top and two on the
       right and bottom, drawn WHERE THE ARTIST HAS IT: immediately against the
       map, with nothing between. It used to be pushed one pixel further out to
       make room for the map's own black outline; the owner asked for that
       outline to go ("Remove the black outline that is usually around the
       minimap and make it match up with the outline"), it is gone with the
       option on, and this line moves into the space it left. */
    const int ft = ml / 7 < 1 ? 1 : ml / 7;
    const int fb_ = (mr * 2) / 8 < 1 ? 1 : (mr * 2) / 8;
    const int fx = g_x0, fy = g_y0;
    const int fw = g_pan_w, fh = g_pan_h;
    px_fill(dst, w, h, fx - ft, fy - ft, fw + ft * 2, ft, dark);
    px_fill(dst, w, h, fx - ft, fy + fh, fw + ft + fb_, fb_, dark);
    px_fill(dst, w, h, fx - ft, fy - ft, ft, fh + ft * 2, dark);
    px_fill(dst, w, h, fx + fw, fy - ft, fb_, fh + ft + fb_, dark);
}

/* THE PLAQUE, DRAWN ABOVE THE MAP. The second of the panel's two passes,
   called from hal_sub_screen_present AFTER ppu_compose_sub has put the map
   down -- the owner's own order, "the MAP banner and text should be above the
   minimap layer". It overlaps the top of the map now, which is what the
   artist's mock-up shows too: there the plaque covers the top fifteen rows of
   the 180-row map well, across its middle 109 columns.

   The attention arrows are drawn after this and stay above everything. */
void hal_sub_panel_plaque(unsigned *dst, int w, int h)
{
    PanelGeom g;
    const int art = panel_geom(&g);
    const int tx = g.tx, ty = g.ty, tw = g.tw, th = g.th;

    if (art) {
        /* the plaque rides the plate's scale exactly -- the same well-to-map
           ratio -- with its own top-left corner as the anchor */
        art_blit(&g_scaled_tab, &g_art_tab, dst, w, h,
                 0, 0,
                 g_art_ix1 - g_art_ix0 + 1, g_art_iy1 - g_art_iy0 + 1,
                 tx, ty, g.ww, g.wh);
        return;
    }

    const unsigned dark = (g_motif_state == 1) ? g_panel_dark : 0xFF201408u;
    const unsigned bright = (g_motif_state == 1) ? g_panel_bright : 0xFFE0C060u;

    /* THE PLAQUE. 109 wide and 32 tall against the artist's 255-wide panel,
       centred, with ten of its rows above the panel's top edge, and its
       corners cut at forty-five degrees over seven of its own pixels. */
    const int cut = tw * 7 / 109;
    for (int y = 0; y < th; ++y) {
        int in = 0;
        if (y < cut) in = cut - y;
        else if (y >= th - cut) in = cut - (th - 1 - y);
        if (in < 0) in = 0;
        px_fill(dst, w, h, tx + in, ty + y, tw - in * 2, 1, bright);
        px_put(dst, w, h, tx + in, ty + y, dark);
        px_put(dst, w, h, tx + tw - 1 - in, ty + y, dark);
    }
    px_fill(dst, w, h, tx + cut, ty, tw - cut * 2, 1, dark);
    px_fill(dst, w, h, tx + cut, ty + th - 1, tw - cut * 2, 1, dark);

    /* MAP, in this file's own letters, scaled to the plaque and centred. */
    int gs = th / 9;
    if (gs < 1) gs = 1;
    const int gw = (5 * 3 + 2 * 2) * gs;          /* three glyphs, two gaps */
    int gx = tx + (tw - gw) / 2;
    const int gy = ty + (th - 7 * gs) / 2;
    for (int gi = 0; gi < 3; ++gi) {
        for (int r = 0; r < 7; ++r)
            for (int c = 0; c < 5; ++c)
                if (kGlyphMAP[gi][r] & (0x10 >> c))
                    px_fill(dst, w, h, gx + c * gs, gy + r * gs, gs, gs, dark);
        gx += 7 * gs;
    }
}

/* ---- THE ROM'S OWN CAMERA-BUTTON ZONE, quoted -----------------------------
 *
 * src/_ZN5Stage16CheckCameraInputEv.cpp:52-58 decides whether a stylus press
 * is on one of the four map arrows, and these are its literal bounds:
 *
 *   camera mode 0 (the DS camera): y >= 0x8a AND (x <= 0x58 OR x >= 0xa7)
 *   camera mode 2 (the zoom):      y >= 0x9a AND  x >= 0xa7
 *
 * With the improved map on, a press inside that zone is NOT PUBLISHED, which
 * is how the arrows stop being clickable without anything in src/ changing
 * and without a hot zone left behind under an invisible button. The ROM's
 * own hit test then runs over a record that says nothing was touched there,
 * which is the same answer it gives today for a player who did not touch it.
 *
 * MODE 2's zone is a subset of mode 0's, so one test covers both and a
 * camera mode this port does not select cannot leak a hot zone either. */
int in_camera_button_zone(int dsx, int dsy)
{
    return dsy >= 0x8a && (dsx <= 0x58 || dsx >= 0xa7);
}

/* ---- AND THE OTHER HALF: THE ARROWS ARE NOT DRAWN --------------------------
 *
 * HOW THEY ARE IDENTIFIED, and it is not by position. A sprite's position is a
 * poor key here: the minimap's player icon and its star markers move over the
 * whole screen, and the arrows' own corners are exactly where the player icon
 * sits when the owner is at the bottom of a course, so a position test would
 * make Mario's head vanish at the bottom of the map. The key is the TILE
 * NUMBER the arrows' own templates carry. OAM::Render copies a template's
 * attr2 into the entry it writes and patches only the palette and priority
 * fields out of it, so the tile bits arrive unchanged; and the eight templates
 * are ROM data this binary links by name (ov002_data.c.obj, aliased in
 * hal/sub_actors.cpp). The set is therefore read from the cartridge's own
 * tables at run time. No literal, no guess, and a graphic that moved would
 * take its tile with it.
 *
 * Tile 0 is never added: it is the ROM's own "nothing here" and unrelated
 * templates carry it.
 *
 * The predicate is installed into ntr's raster (ppu_sub_set_obj_veto) because
 * the policy is this file's and the raster is a hardware model. */
extern "C" {
extern const unsigned short _ZN3OAM12CAM_BUTTON_LE[];
extern const unsigned short _ZN3OAM12CAM_BUTTON_RE[];
extern const unsigned short _ZN3OAM20CAM_BUTTON_L_PRESSEDE[];
extern const unsigned short _ZN3OAM20CAM_BUTTON_R_PRESSEDE[];
extern const unsigned short _ZN3OAM14S_CAM_BUTTON_LE[];
extern const unsigned short _ZN3OAM14S_CAM_BUTTON_RE[];
extern const unsigned short _ZN3OAM22S_CAM_BUTTON_L_PRESSEDE[];
extern const unsigned short _ZN3OAM22S_CAM_BUTTON_R_PRESSEDE[];
}

unsigned short g_cam_tiles[64];
int g_cam_tiles_n = -1;

void cam_tiles_add(const unsigned short *t)
{
    /* an OamAttr is four halfwords and attr3 == 0xffff ends the list
       (include/OamAttr.h). The sixteen is a guard, not a length: a list that
       never terminates is a corrupt table and must not be walked off. */
    for (int i = 0; i < 16; ++i) {
        const unsigned short a2 = t[i * 4 + 2];
        const unsigned short a3 = t[i * 4 + 3];
        const unsigned short tile = (unsigned short)(a2 & 0x03FFu);
        if (tile && g_cam_tiles_n <
                (int)(sizeof g_cam_tiles / sizeof g_cam_tiles[0])) {
            int seen = 0;
            for (int k = 0; k < g_cam_tiles_n; ++k)
                if (g_cam_tiles[k] == tile) { seen = 1; break; }
            if (!seen) g_cam_tiles[g_cam_tiles_n++] = tile;
        }
        if (a3 == 0xffff) return;
    }
}

int cam_button_veto(unsigned short a2)
{
    if (g_cam_tiles_n < 0) {
        g_cam_tiles_n = 0;
        cam_tiles_add(_ZN3OAM12CAM_BUTTON_LE);
        cam_tiles_add(_ZN3OAM12CAM_BUTTON_RE);
        cam_tiles_add(_ZN3OAM20CAM_BUTTON_L_PRESSEDE);
        cam_tiles_add(_ZN3OAM20CAM_BUTTON_R_PRESSEDE);
        cam_tiles_add(_ZN3OAM14S_CAM_BUTTON_LE);
        cam_tiles_add(_ZN3OAM14S_CAM_BUTTON_RE);
        cam_tiles_add(_ZN3OAM22S_CAM_BUTTON_L_PRESSEDE);
        cam_tiles_add(_ZN3OAM22S_CAM_BUTTON_R_PRESSEDE);
        if (std::getenv("SM64DS_MINIMAP_TRACE")) {
            std::fprintf(stderr, "[mmtrace] camera-button tiles (%d):",
                         g_cam_tiles_n);
            for (int k = 0; k < g_cam_tiles_n; ++k)
                std::fprintf(stderr, " %u", (unsigned)g_cam_tiles[k]);
            std::fprintf(stderr, "\n");
        }
    }
    const unsigned short tile = (unsigned short)(a2 & 0x03FFu);
    for (int k = 0; k < g_cam_tiles_n; ++k)
        if (g_cam_tiles[k] == tile)
            return improved_map_on() ? 3 : 1;
    return 0;
}
int g_zoom = 1;
HWND g_hwnd;
// The client rectangle the framebuffer was last presented into, in client
// pixels. Width zero means nobody has presented yet.
int g_pr_x, g_pr_y, g_pr_w, g_pr_h;
/* The SOURCE image that rectangle was filled from, in its own pixels. It is
   the framebuffer in inset mode and the stacked image in stacked mode, and the
   inverse transform needs it because the two are not the same shape. Zero
   means "nobody has presented yet", the same as g_pr_w. */
int g_pr_sw, g_pr_sh;

/* ---- the layout mode -------------------------------------------------------
   -1 while unresolved. The scene proposes (hal_sub_screen_set_stacked) and
   SM64DS_DUAL_SCREEN disposes; a run with no scene and no env is inset, which
   is every level. */
int g_stacked = -1;
int g_stacked_default;

int stacked_env(void)
{
    /* -1 = unset, 0 = forced off, 1 = forced on. Read once, like every other
       env in this file, so a run cannot change layout halfway through. */
    static int v = -2;
    if (v == -2) {
        const char *e = std::getenv("SM64DS_DUAL_SCREEN");
        v = e ? (std::atoi(e) ? 1 : 0) : -1;
    }
    return v;
}

BOOL(WINAPI *GetCursorPos_)(POINT *);
BOOL(WINAPI *ScreenToClient_)(HWND, POINT *);
SHORT(WINAPI *GetAsyncKeyState_)(int);
HWND(WINAPI *GetForegroundWindow_)(void);

ntr::SubFramebuffer g_sub;

int env_flag(const char *name, int dflt)
{
    const char *v = std::getenv(name);
    return v ? std::atoi(v) : dflt;
}

/* ---- SM64DS_TOUCH_PROBE: a scripted stylus, and what the four names read ---
 *
 * The stylus record is FOUR DS symbols over ONE 16-byte block -- data_020a0de8
 * .. data_020a0deb at +0/+1/+2/+3 of TouchInfo[4] (hal/auto_bss.cpp carries the
 * layout and its evidence). poll_touch writes the record through the FIRST
 * name; Message::Update reads `held` and `y` through the SECOND and FOURTH.
 * Whether those are the same bytes is a LINK-TIME property no compiler
 * diagnostic covers, and a headless run has no mouse, so without this hook the
 * question cannot be asked of a running binary at all.
 *
 *   SM64DS_TOUCH_PROBE="58-59:120:150,60,100-101:33:44,199-201"
 *
 * Comma-separated entries, the same grammar as SM64DS_PROBE_INPUT:
 *
 *   <frame>[-<frame>]:<x>:<y>   force a press at those DS pixels on those
 *                               frames, and log
 *   <frame>[-<frame>]           log only, no press
 *
 * One stderr line per listed frame:
 *
 *   [touch] f58 poke=yes(1,120,150) pre={de8=0 de9=0 dea=0 deb=0}
 *           post={de8=1 de9=1 dea=120 deb=150} d=(1,2,3) block=01017896 ...
 *
 * de9 reads 1 on the press frame and on the release frame and 0 in between,
 * because it is the ROM's touch-XOR-previous edge; see poll_touch's store.
 *
 * `pre` is what the names read on ENTRY to the poll -- which is what a
 * save-state restore left behind when the load frame is a logged frame --
 * and `post` is after the write. `d` is (&de9-&de8, &dea-&de8, &deb-&de8)
 * and MUST read (1,2,3): those three numbers ARE the bug. Hosted as separate
 * arrays they read (32,-,64) -- unrelated addresses, dea not hosted at all --
 * and de9/deb read zero on every frame forever.
 *
 * THE READS ARE VOLATILE ON PURPOSE. MSVC treats two named globals as
 * disjoint objects, so a load through data_020a0de9 in the same function as a
 * store through data_020a0de8 folds to the old value even across translation
 * units (measured: the plain read prints 0 where the volatile read prints 2).
 * The game path cannot hit that -- the one writer only stores and every
 * reader only loads -- but a probe that writes and reads in one function can,
 * and a probe that lies about the fix is worse than no probe. src/ already
 * carries the same shape: Stage::LC_Update declares data_020a0dea/deb
 * volatile.
 *
 * The probe's own state is host bookkeeping and deliberately NOT in .dsstate:
 * a save-state load must not rewind the script that is testing it. */
struct TouchProbeEnt {
    int f0, f1, x, y, poke;
};
/* 256, not 32 (run mg8, lane MMD). The parser silently STOPS at the cap, so a
   script that overruns it loses its tail without saying so. Thirty-two held
   three or four gestures; driving a minigame ROUND to its end is sixteen taps
   for one Memory Master board, eighty for the five clears that take its board
   from sixteen cards to eighteen, and a hundred and seventy for the ten that
   take it to twenty. The array is 5KB of host bookkeeping and no run reads
   past g_tp_n. */
TouchProbeEnt g_tp[256];
int g_tp_n = -1;            /* -1 = env not read yet, 0 = probe off */
int g_tp_frame;             /* polls since the first one */
int g_tp_cur = -1;          /* the frame being polled, for the camera log */

/* ---- THE FORCED TAP, run rel0215 lane boot-title -------------------------
 *
 * A press SM64DS_SKIP_MENU asks for, consumed by poll_touch below in the same
 * place the scripted probe and the comms injection are consumed. Two frames by
 * default, because the ROM's edge is `new XOR old` and a one-frame press is a
 * legal edge but leaves nothing held; two is what a tap looks like and it is
 * what the proven chains in this tree script.
 *
 * NOT IN .dsstate, for the touch probe's own reason: a save-state load must not
 * rewind the script that is driving the run. */
static int g_force_n;
static int g_force_x, g_force_y;

extern "C" void port_touch_force_press(int x, int y, int frames)
{
    g_force_x = x;
    g_force_y = y;
    g_force_n = frames;
}

/* 1 and fills *fx/*fy while a forced press is outstanding, consuming one
   frame of it. Declared locally at the call site rather than in a header
   because it has exactly one producer and one consumer. */
extern "C" int port_touch_forced(int *fx, int *fy)
{
    if (g_force_n <= 0)
        return 0;
    --g_force_n;
    *fx = g_force_x;
    *fy = g_force_y;
    return 1;
}

/* ---- THE IN-PROCESS STYLUS, run link100 lane TOUCH1 -----------------------
 *
 * SM64DS_CLICK_TEST's driver (tests/walk_window.cpp) used to press the stylus
 * by moving the real pointer with SetCursorPos and pushing a real button edge
 * through SendInput, which only works while the game's window is the
 * FOREGROUND window -- so the driver called SetForegroundWindow and the window
 * took Tango's screen. That is the one launcher path in the port that could
 * still do that, and it is banned.
 *
 * ARMING THIS IS THE BUTTON, and the point handed in is the cursor. Everything
 * below the query -- the two layout transforms, the drag latch, the clamp, the
 * change-edge store, the ring -- is the code a hand reaches, unchanged and not
 * copied: poll_touch takes its own live branch with these two values in place
 * of the OS's. So a click script still exercises the client-pixel transform
 * (which is the whole reason SM64DS_CLICK_TEST exists beside
 * SM64DS_TOUCH_PROBE, see that banner in tests/walk_window.cpp) without asking
 * the OS for anything and without a window in front of anybody.
 *
 * IT LATCHES, because the thing it replaces latched: the OS button state stays
 * down between the driver's call and the next poll, and the level loop's
 * driver call happens AFTER that loop's hal_sub_screen_frame_begin. A press
 * armed on one frame and consumed on the next is the behaviour the SendInput
 * path already had.
 *
 * Host bookkeeping, deliberately not in .dsstate, for the touch probe's own
 * reason: a save-state load must not rewind the script driving the run. */
static int g_inj_armed;
static int g_inj_cx, g_inj_cy;
static unsigned g_inj_frames;     /* polls that saw the latch armed */
static unsigned g_inj_presses;    /* arming edges */
static unsigned g_inj_on_surface; /* of those polls, how many published a DS pixel */

extern "C" void port_touch_inject_client(int cx, int cy)
{
    if (!g_inj_armed) ++g_inj_presses;
    g_inj_armed = 1;
    g_inj_cx = cx;
    g_inj_cy = cy;
}

extern "C" void port_touch_inject_release(void)
{
    g_inj_armed = 0;
}

/* frames, presses, surface hits. Used by the driver's own census line. */
extern "C" void port_touch_inject_census(unsigned *frames, unsigned *presses,
                                         unsigned *on_surface)
{
    if (frames)     *frames = g_inj_frames;
    if (presses)    *presses = g_inj_presses;
    if (on_surface) *on_surface = g_inj_on_surface;
}

/* ---- THE PROBE'S CENSUS, ON STDOUT, run link100 lane TOUCH1 ---------------
 *
 * WHY STDOUT AND WHY AT ALL. Every line this probe prints goes to stderr, and
 * on the scene path walk_window has already pointed stderr at playlog/. So a
 * harness that captures the child's stdout (which is where the [scene] report
 * every scene lane reads comes out) sees NOTHING from the probe, however well
 * it worked. port/touch_map.txt section 5 recorded that trap in 2026-08 after
 * losing four runs to it; prover CURLPROOF lost its whole measurement to the
 * same thing tonight and reported the probe dead on the minigame path when it
 * was writing the record correctly all along, 1492 poked frames deep.
 *
 * One line, on the stream the report is on, saying what the script did. Inert
 * unless SM64DS_TOUCH_PROBE is set: the atexit is registered from inside the
 * parser and only when the parse produced entries.
 *
 * PRESS EDGES ARE THE NUMBER THAT MATTERS and that is why they are counted
 * separately from poked frames. The ROM's own producer spells byte +1 as
 * `touch XOR previous` (src/func_0203bb60.c), so ONE held press is ONE edge,
 * and every gate in the game that reads "touched AND changed" -- curling's
 * func_ov006_020e1b54 is one -- can only fire on that single frame. A script
 * that holds one long press across a scene's warm-up timer therefore delivers
 * a press the game is structurally unable to see, and the old log gave a lane
 * no way to tell that apart from a broken bridge. */
static unsigned g_tp_polls, g_tp_poked, g_tp_edges, g_tp_releases;
static int g_tp_last_x = -1, g_tp_last_y = -1;

static void touch_probe_census(void)
{
    std::printf("[touch] probe census: %d script entr(ies), %u poll(s), %u "
                "poked frame(s), %u press edge(s), %u release(s), last DS "
                "(%d,%d)\n",
                g_tp_n, g_tp_polls, g_tp_poked, g_tp_edges, g_tp_releases,
                g_tp_last_x, g_tp_last_y);
    std::fflush(stdout);
}

void touch_probe_parse(void)
{
    g_tp_n = 0;
    const char *s = std::getenv("SM64DS_TOUCH_PROBE");
    if (!s) return;
    while (*s && g_tp_n < (int)(sizeof g_tp / sizeof *g_tp)) {
        TouchProbeEnt e = {0, 0, 0, 0, 0};
        e.f0 = std::atoi(s);
        while (*s && *s != '-' && *s != ':' && *s != ',') ++s;
        if (*s == '-') {
            ++s;
            e.f1 = std::atoi(s);
            while (*s && *s != ':' && *s != ',') ++s;
        } else {
            e.f1 = e.f0;
        }
        if (*s == ':') {
            ++s;
            e.x = std::atoi(s);
            while (*s && *s != ':' && *s != ',') ++s;
            if (*s == ':') {
                ++s;
                e.y = std::atoi(s);
                while (*s && *s != ',') ++s;
            }
            e.poke = 1;
        }
        g_tp[g_tp_n++] = e;
        while (*s && *s != ',') ++s;
        if (*s == ',') ++s;
    }
    if (g_tp_n > 0)
        std::atexit(touch_probe_census);
}

const TouchProbeEnt *touch_probe_at(int f)
{
    for (int i = 0; i < g_tp_n; ++i)
        if (f >= g_tp[i].f0 && f <= g_tp[i].f1) return &g_tp[i];
    return 0;
}

/* one byte, read the way a reader in another TU reads it -- see the note on
   volatile above */
unsigned char tp_rd(const unsigned char *p, int i)
{
    return *(const volatile unsigned char *)(p + i);
}

/* The stylus, from the mouse. Byte +0 is "down now" and byte +1 is THE CHANGE
   EDGE, new XOR old -- see the block above the two stores at the bottom of
   this function for the ROM derivation and for what the old spelling cost. */
/* ---- A DRAG OWNS THE STYLUS UNTIL THE BUTTON COMES UP ---------------------
 *
 * ON A DS THE STYLUS CANNOT LEAVE THE SCREEN MID-DRAG. There is no edge to
 * fall off: the panel is the whole input device, so every game on the machine
 * was written knowing that a release is something the player DID. This port
 * gave the player four edges the hardware has none of (the letterbox bars,
 * the hinge band, the top screen, and the desktop past the window) and
 * published a RELEASE the frame the cursor crossed any of them, because the
 * surface test was what gated `down` and nothing else held it.
 *
 * WHAT THAT COST, from a live session: "when i drag off screen it releases the
 * touch". On scene 368 that is the slingshot firing itself. The pull is
 * src/func_ov006_020fe394.cpp and its whole body is one branch on
 * `data_020a0de8[k * 4] != 0`: the true side keeps the ball at the captured
 * offset, and the FALSE side is the shot. It writes 2 into the slot's state
 * byte at +0x4f0d and either snaps the ball home or launches it. So a hand
 * that strayed past the window while the player was pulling back did not pause
 * the pull. It took the shot.
 *
 * SO THE LATCH. The first poll of a button-hold that resolves ON the surface
 * arms `drag_own`, and from there until the PHYSICAL button comes up the
 * stylus belongs to that drag: an off-surface point publishes the nearest
 * point on the bottom screen rather than publishing nothing. Both mappers
 * already compute exactly that point and already throw it away. Each one
 * clamps its output into the screen's own range and answers inside/outside
 * separately through the return value, so this consumes an answer that was
 * being discarded rather than adding a second opinion about where the screen
 * is. Display and touch keep deriving from the one StackLayout.
 *
 * WHAT IT DOES NOT CHANGE, and each of these is a test.
 *   * A press that STARTS off the surface publishes nothing, exactly as
 *     before. The latch can only be armed by a genuinely on-surface point, so
 *     a click in a bar is still a click in a bar and THE GAP BAND'S DEAD-CLICK
 *     RULE STILL HOLDS FOR PRESSES. What crossing the band cannot do any more
 *     is kill a drag that is already running.
 *   * A drag that never leaves the surface takes the same branch with the same
 *     arithmetic: on the surface the two clamps below are no-ops.
 *   * Byte +1 is untouched. It is still `down ^ was`, so the release edge is
 *     still exactly one frame, and it now lands on the real button-up wherever
 *     the cursor is, which is the whole of what the ROM means by a release.
 *   * No top-screen coordinate is ever published for a drag. The clamp is into
 *     the BOTTOM screen's range in both layouts; there is no branch here that
 *     can reach the other band.
 */
/* hal/tsc_arm7.cpp, run link100 lane R2D3: the ROM's own ring writer and the
   query that says whether it is live. MSVC refuses a linkage specification
   inside a function body (C2598), so these two declarations sit here, right
   above poll_touch's store, rather than inside it. */
extern "C" int  port_tsc_ring_armed(void);
extern "C" void port_tsc_arm7_frame_touch(void);
extern "C" unsigned long port_tsc_arm7_sample_count(void);
/* hal/comms_conductor.cpp's MP3_BSS row: the ROM's OWN reader's settled
   answer, {a,b,c,d} = {x,y,touched,valid} (struct Col, src/func_0203b9bc.c).
   Read-only here, for the watcher two paragraphs below. */
extern "C" unsigned short data_020a0dd8[4];

void poll_touch(void)
{
    unsigned char down = 0, sx = 0, sy = 0;
    /* what the LIVE mouse read this poll, kept for the recorder below. `seen`
       means the button was physically down and the cursor resolved to a client
       point; `on` means that point was on the stylus surface. The pair is what
       tells "no press" apart from "a press the transform refused", and only
       the second of those is a bug worth chasing. `clamped` is the third
       answer the latch adds: off the surface, and published anyway. */
    int live_cx = 0, live_cy = 0, live_seen = 0, live_on = 0, live_clamped = 0;
    /* and the fourth: the resize handle took the press. Without it the
       recorder below reports a grab as a press that landed on the top screen,
       which is the one line a player's log would show for a square they had
       just successfully grabbed. */
    int live_handle = 0;
    /* THE GESTURE'S OWN STATE, and a host static on purpose rather than
       anything in .dsstate: a save-state load must not restore a half-finished
       drag onto a hand that is not holding the button any more. Same argument
       the touch probe's bookkeeping makes a few lines above. */
    static int drag_own;
    /* THE PHYSICAL BUTTON, asked once, because it is now the thing that ends a
       drag and no longer merely the thing that starts one. */
    /* run link100 lane TOUCH1: an armed in-process injection IS the button and
       its client point IS the cursor. One branch, so everything below -- the
       two transforms, the latch, the clamp, the store -- is the same code the
       hand reaches. Unarmed this costs one load and nothing else. */
    const int inj = g_inj_armed;
    if (inj) ++g_inj_frames;
    const int btn = inj ? 1
                        : (!g_headless && g_on && GetAsyncKeyState_ &&
                           (GetAsyncKeyState_(VK_LBUTTON) & 0x8000) ? 1 : 0);
    if (!btn) {
        drag_own = 0;
        /* and a resize that was in progress is finished and written down */
        handle_release();
    }
    if (btn && (inj || (GetCursorPos_ && ScreenToClient_))) {
        POINT p;
        int got;
        if (inj) {
            p.x = g_inj_cx;
            p.y = g_inj_cy;
            got = 1;
        } else {
            got = (GetCursorPos_(&p) && ScreenToClient_(g_hwnd, &p)) ? 1 : 0;
        }
        if (got) {
            /* THE TRANSFORM HAS TWO SHAPES NOW, one per layout, and which one
               runs is the only thing the mode changes about the touch.

               STACKED: the bottom half of the picture IS the bottom screen, so
               the mapper hands back DS pixels directly and the whole half is
               live. A click anywhere in it lands on the corresponding DS pixel,
               which is the point of the mode.

               INSET: client pixels to framebuffer pixels (the present
               rectangle, run backwards), then panel pixels to DS pixels,
               because the panel is drawn at 1/g_div in a corner. Unchanged. */
            int fx = -1, fy = -1, on_picture = 0;
            int bx = -1, by = -1;
            if (hal_sub_screen_stacked()) {
                on_picture =
                    hal_present_client_to_sub((int)p.x, (int)p.y, &fx, &fy);
            } else {
                /* a click in a letterbox bar is not on the panel however close
                   the clamp puts it, so the inside answer gates the press */
                on_picture =
                    hal_present_client_to_fb((int)p.x, (int)p.y, &bx, &by);
                /* panel pixels back to DS pixels. The forward direction is
                   DS * num / den (hal_sub_panel_geometry), so the inverse is
                   * den / num, and with the option off num is 1 and den is
                   g_div -- byte for byte the multiply this replaces. */
                /* FLOOR, not truncate. A point one pixel LEFT of the map is
                   at -1 framebuffer pixels, and at a magnified size -1 times
                   den over num truncates to 0 -- so the pixel of panel just
                   outside the map's left edge read as DS column 0 and the
                   touch surface was one pixel wider than the picture on two
                   of its four sides. Harmless while the map was a bare inset
                   over the 3D view; not harmless with the artist's frame
                   drawn on exactly those pixels. */
                if (g_swap) {
                    /* THE PICTURE IS COMPOSED: the only touch surface on it
                       is the block of three plates in the middle, so the
                       inverse reads the very rectangle the compose just drew
                       -- the same numbers, the same floor divide, one
                       arithmetic -- and every other pixel of the picture
                       answers nothing.
                       WHAT ANSWERS NOTHING, said plainly, because all three
                       are things a player may have learned to click: the
                       course text at the top and the coin row at the bottom
                       are the TOP screen and no stylus has ever reached it;
                       the picture either side of the plates is the game's
                       own and not a screen at all; and the corner panel's
                       old three button points --
                       and the old full-picture swapped ones -- are not on the
                       plates any more, which is the honest answer, because
                       the buttons are not there any more. */
                    int in = 0;
                    swap_inverse(bx, by, &fx, &fy, &in);
                    if (!in) on_picture = 0;
                } else {
                    fx = fdiv((bx - g_x0) * g_pan_den, g_pan_num);
                    fy = fdiv((by - g_y0) * g_pan_den, g_pan_num);
                }
            }
            live_cx = (int)p.x;
            live_cy = (int)p.y;
            live_seen = 1;
            int on_surface = on_picture && fx >= 0 && fx < ntr::SUB_W &&
                             fy >= 0 && fy < ntr::SUB_H;
            /* THE ARROWS ARE NOT CLICKABLE WITH THE OPTION ON. Refused here,
               at the one seam that decides whether a host click becomes a
               stylus record at all, so the press never exists as far as the
               ROM is concerned -- no press, no camera step, and no invisible
               button to hit by accident. The bumpers, Q / E and the right
               stick reach the camera through tests/walk_window.cpp's own
               cam_turn and are untouched by this.
               IT ALSO ENDS A DRAG rather than clamping one: a press that
               wanders into the zone is dropped for as long as it is in there,
               which is what a player who cannot see a button expects. */
            /* NOT WHILE THE LEVEL-CLEAR MENU IS UP, whichever way round the
               screens are. The zone is DS y >= 0x8a with x outside the middle,
               and the menu's third plate runs to DS y 0x97 across the whole
               width -- so with the improved map on, the bottom twelve rows of
               that button's left and right thirds were a dead zone. The four
               camera arrows are not what the bottom screen is showing while
               the menu owns it, and the menu's own touch boxes are the only
               hit test running. g_menu_up is the ROM's own pair, taken once a
               frame at the presentation seam. */
            if (on_surface && improved_map_on() && !g_menu_up &&
                in_camera_button_zone(fx, fy)) {
                on_surface = 0;
                drag_own = 0;
            }
            /* THE RESIZE HANDLE TAKES THE PRESS WHOLE. Tested here, at the one
               seam that decides whether a host click becomes a stylus record
               at all, so a press that started on the square never exists as
               far as the ROM is concerned: no touch, no camera step, nothing
               published. It is tested AFTER the camera zone and before the
               arming edge, because the handle sits outside the map's own
               rectangle and the two can never both claim a press. */
            if (!hal_sub_screen_stacked() && handle_press(bx, by, on_picture)) {
                on_surface = 0;
                drag_own = 0;
                live_handle = 1;
            }
            /* the arming edge, and the only one there is */
            if (on_surface) drag_own = 1;
            if (inj && on_surface) ++g_inj_on_surface;
            if (on_surface || drag_own) {
                /* THE NEAREST POINT ON THE BOTTOM SCREEN. On the surface both
                   of these are no-ops, which is why a drag that stays on the
                   screen cannot tell this branch from the one it replaces. */
                const int qx = fx < 0 ? 0
                                      : (fx >= ntr::SUB_W ? ntr::SUB_W - 1 : fx);
                const int qy = fy < 0 ? 0
                                      : (fy >= ntr::SUB_H ? ntr::SUB_H - 1 : fy);
                down = 1;
                live_on = on_surface;
                live_clamped = !on_surface;
                sx = (unsigned char)qx;
                sy = (unsigned char)qy;
            }
        } else if (drag_own) {
            /* THE QUERY FAILED, NOT THE DRAG. ScreenToClient does not fail for
               a live window, but an owned drag must not end on something that
               is not the player's hand. Ending on anything else is the exact
               defect this latch exists to close. Hold the record where it is:
               `down` stays set, and feeding the published point back through
               the two stores below leaves the last one standing. */
            down = 1;
            sx = data_020a0de8[2];
            sy = data_020a0de8[3];
        }
    }
    if (g_tp_n < 0) touch_probe_parse();
    const int f = g_tp_frame++;
    g_tp_cur = f;
    const TouchProbeEnt *tp = g_tp_n > 0 ? touch_probe_at(f) : 0;
    if (g_tp_n > 0) ++g_tp_polls;
    if (tp && tp->poke) ++g_tp_poked;
    unsigned char pre8 = 0, pre9 = 0, prea = 0, preb = 0;
    if (tp) {
        /* what the four names read BEFORE this poll writes anything: slot 0,
           in Message::Update's own shape (data_020a0deX[idx * 4], idx = 0) */
        pre8 = tp_rd(data_020a0de8, 0);
        pre9 = tp_rd(data_020a0de9, 0);
        prea = tp_rd(data_020a0dea, 0);
        preb = tp_rd(data_020a0deb, 0);
        if (tp->poke) {
            down = 1;
            sx = (unsigned char)tp->x;
            sy = (unsigned char)tp->y;
        }
    }

    /* ---- SM64DS_SKIP_MENU's TAP, run rel0215 lane boot-title ---------------
     *
     * The third member of this family, in the same place and the same shape as
     * the two around it, and it exists for a reason the other two do not have.
     *
     * The title screen -- top-state 11, src/func_ov007_020b0a20.c -- WAITS FOR
     * A TOUCH AND NOTHING ELSE. Measured: a default boot with no input at all
     * sits in that state for 3000 frames and never leaves it
     * (out/boot-title/P0_attract_noinput.log, one state line at f1 and then
     * silence). That is correct, it is what a title screen does, and it means
     * there is no state the port can ASK for to get past it: the ROM's own way
     * out is an input edge, and its own gate then decides whether to take it.
     *
     * So SM64DS_SKIP_MENU supplies the tap the player would have made, and the
     * ROM's own conditions -- the down edge, its frame counter past 4, its own
     * reading of the save slots through func_ov007_020aebac, and its animation
     * being clear of its last 31 frames -- are all still the ones that decide.
     * If the ROM refuses, the title stays up. The port does not overrule it.
     *
     * LANDING IT HERE IS THE WHOLE POINT, and it is the neighbouring block's
     * point too: from this line on the tap is indistinguishable from a real
     * one and travels the ROM's own path -- the change-edge store below,
     * func_0203bb60, func_ov007_020c1db0's panel record, and the title's own
     * gate. Nothing downstream knows a hand was not on the panel.
     *
     * hal/title_entry.cpp is the only caller and it only ever asks while the
     * title is on screen. */
    {
        int fx = 0, fy = 0;
        if (port_touch_forced(&fx, &fy)) {
            down = 1;
            sx = (unsigned char)fx;
            sy = (unsigned char)fy;
        }
    }

    /* run mg16 lane MP3: SM64DS_COMMS_INJECT's stylus half, honoured in the
       same place and the same way as the touch probe above. It exists for the
       two-instance proofs, where there is no hand on the panel and the
       harness's own SM64DS_PAD_TEST / SM64DS_CLICK_TEST are read as unset
       under SM64DS_WINDOW_SELFTEST. Landing it HERE rather than in the comms
       record is the point: from this line on, the injected touch is
       indistinguishable from a real one and travels the ROM's own path --
       ring, func_0203b9bc, func_0203df40, the wire. */
    {
        int idown = 0, ix = 0, iy = 0;
        if (port::comms_inject_touch(&idown, &ix, &iy)) {
            down = (unsigned char)idown;
            sx = (unsigned char)ix;
            sy = (unsigned char)iy;
        }
    }

    /* ---- BYTE +1 IS THE ROM'S CHANGE EDGE, NOT "HELD FOR TWO FRAMES" ------
     *
     * This is link 5 of the chain port/touch_map.txt maps, and the ROM's own
     * link 5 is src/func_0203bb60.c. Its whole body, per controller slot:
     *
     *     p[1] = (u8)(r->field_4 ^ p[0]);   the NEW touch XOR the OLD one
     *     p[0] = (u8)r->field_4;            touched now
     *     p[2] = (u8)r->field_0;            x
     *     p[3] = (u8)r->field_2;            y
     *
     * So byte +1 is 1 on exactly two frames of a press: the frame it goes
     * down and the frame it comes back up. It is a TRANSITION flag. This
     * function used to write `down && was` there, which is the opposite
     * shape -- 0 on the press frame and 1 on every frame after it -- and the
     * whole game reads that byte.
     *
     * WHAT IT COST, measured on scene 368 (Bob-omb Squad) with
     * SM64DS_TOUCH_PROBE and the shot table's own state trace. The plunger is
     * src/func_ov006_020fe2e4.c (grab) and src/func_ov006_020fe394.cpp (pull and
     * release). The grab arms on `data_020a0de8[k*4] && data_020a0de9[k*4]`
     * and captures ball-minus-stylus into +0x4ee8/+0x4eec; the pull then holds
     * that offset, so WHERE the stylus is on the arming frame decides where
     * the ball sits for the rest of the drag and how far down it can be
     * pulled at all.
     *
     *   press for ONE frame              old: never armed. The ROM arms on
     *                                    that frame, because that frame IS
     *                                    the edge.
     *   press, then move on the NEXT     old: armed one frame late, at the
     *   frame (a fast mouse flick)       moved-to point, so the offset came
     *                                    out (0,-110); the ball then tracked
     *                                    110 px above the cursor, never left
     *                                    the anchor, and the release measured
     *                                    dist 8 -- under 0x10, which is
     *                                    func_ov006_020fe394's snap-back, so
     *                                    the shot never fired.
     *   press, hold still, then drag     old: worked. That is the whole of
     *                                    "sometimes I can grab it".
     *
     * TWO MORE CLASSES OF READER GET THEIR BEHAVIOUR BACK WITH THIS, and they
     * are why this is a record fix rather than a pachinko fix.
     * `de8 && de9` sites (the press edge: curling's func_ov006_020e1b54,
     * Coincentration's func_ov006_020dd0e0, and two dozen more) fired one
     * frame late and then EVERY frame of the hold instead of once.
     * `de8 == 0 && de9` sites (the release edge: src/func_ov006_020d1ba0.cpp:63,
     * _ZN17dScMgTrampoline_c16UpdateTouchInputEv.c:49, _ZN21cMgSmartball_spring_c12SaveSnapshotEv.c:82) were identically
     * false under the old spelling -- a byte that means "down and was down"
     * cannot be set while `down` is clear -- so release detection did not
     * exist on this port at all.
     *
     * X AND Y STAY LATCHED ON RELEASE, and that is not the same shortcut.
     * The ROM writes p[2]/p[3] every frame from link 3, and link 3
     * (src/func_0203b9bc.c) only publishes its idle 0xff/0xff when NO entry in
     * the four-deep ring carries a touch. On the release-edge frame the ring
     * is still draining, so neither of its accept branches and neither the
     * idle branch runs, and data_020a0dd8 keeps the last touched point --
     * which is exactly what the `if (down)` below leaves in place. The two
     * spellings differ only once the ring has drained, by which time byte +1
     * is 0 and no reader is looking. */
    const unsigned char was = data_020a0de8[0];
    data_020a0de8[0] = down;
    data_020a0de8[1] = (unsigned char)(down ^ was);
    if (down) {
        data_020a0de8[2] = sx;
        data_020a0de8[3] = sy;
    }

    /* ---- AND THE SAME SAMPLE INTO THE ROM'S OWN RING (run mg16, lane MP3) --
     *
     * Everything above writes link 5's OUTPUT directly. src/func_0203b9bc.c is
     * link 3, and it does not read any of it: it reads the four-deep touch RING
     * at data_020a0df8 and settles the answer into data_020a0dd8. Nothing in
     * this port had ever written that ring, so the moment src/func_0203df40.c
     * was linked -- it calls func_0203b9bc to fill the local comms record's
     * stylus fields -- the ROM published its IDLE QUAD every frame and the
     * stylus could not cross the wire. Measured: rung 3's parent saw
     * stylus={255,255,0} on all 300 frames while the injected KEY crossed
     * perfectly, because the key goes through a register this lane does write.
     *
     * So the port writes the ring the way the touch-panel driver does, and
     * func_0203b9bc reads it the way the ROM does. That is the same shape as
     * the key register: put the value where the hardware puts it and let the
     * ROM's own code do the rest, rather than teaching the game to skip a read.
     *
     * THE ENCODING IS THE ONE THIS FILE ALREADY DOCUMENTS twenty lines up, and
     * src/func_0203b9bc.c is where it was read off: {u16 x, u16 y, u16 touched,
     * u16 validity}, nine entries, and `d` is a VALIDITY MASK where any set bit
     * makes src/func_ov007_020c1db0.c drop the sample -- so a live touch
     * publishes d = 0 or the reader cancels it.
     *
     * THREE CONSECUTIVE SAMPLES ARE NEEDED before func_0203b9bc accepts one:
     * both of its accept branches test three of the four entries it looks at.
     * That is the ROM's own debounce and it is left alone. A one-frame tap
     * therefore does NOT cross the wire, which is correct DS behaviour rather
     * than a limitation of this store.
     *
     * The write index is data_020a80cc[6], which func_0205edc8 returns and
     * func_0203b9bc reads backwards from. It was advanced here because the
     * driver that would advance it is ARM7's -- and now hal/tsc_arm7.cpp IS
     * that driver (run link100, lane R2D3, continuing rung R2d).
     *
     * ---- RETIRED WHEN THE ROM'S OWN WRITER IS LIVE ------------------------
     *
     * hal/tsc_arm7.cpp section 4 has the full derivation; this is the summary
     * the store itself needs. The ring has exactly one writer: when
     * port_tsc_ring_armed() is true (SM64DS_TP_RING, default ON), the direct
     * memcpy below is skipped and port_tsc_arm7_frame_touch() runs instead --
     * once per frame, right here, which is this call's own cadence. That
     * function does not touch the ring itself; it gives the ARM7 the turn
     * that posts the auto-sample indication on channel 6, and the ROM's own
     * src/func_0205f300.c is what writes data_020a0df8 and advances the
     * cursor, off the shared sample words the ARM7 fills from TouchInfo.
     *
     * THE HOST'S INPUT IS NOT LOST -- it is what TouchInfo (data_020a0de8,
     * written four paragraphs above this one) IS. The mouse-to-stylus
     * mapping, the drag latch, SM64DS_SKIP_MENU's forced tap and
     * SM64DS_COMMS_INJECT's injected tap all already resolve into `down`,
     * `sx`, `sy` before this point and are stored into TouchInfo unchanged;
     * port_tsc_arm7_frame_touch reads that same slot to build the ARM7's
     * sample. So every one of those sources still reaches the ROM's writer,
     * through the sample rather than through this store.
     *
     * SM64DS_TP_RING=0 puts the direct write back exactly as it was, for a
     * run measuring the pre-rung shape. */
    {
        /* THE WATCHER (run link100, lane R2D3). SM64DS_TP_RING_WATCH=1 prints
           both writers' cumulative store counts every frame -- host_stores is
           this store's own counter, incremented ONLY on the branch below that
           still writes the ring directly, and port_tsc_arm7_sample_count()
           reads hal/tsc_arm7.cpp's count of ROM-side writes (see its banner:
           the send that count is kept beside is synchronous with
           src/func_0205f300.c's own store, so the count is not an estimate).
           Off by default: it is a proof aid, not a shipped line. */
        static unsigned long host_stores;
        if (port_tsc_ring_armed()) {
            port_tsc_arm7_frame_touch();
        } else {
            const int wi = port::touch_ring_index();
            unsigned char *e = data_020a0df8 + wi * 8;
            const unsigned short rx = down ? sx : 0;
            const unsigned short ry = down ? sy : 0;
            const unsigned short rc = down ? 1u : 0u;   /* +4: touched   */
            const unsigned short rd = 0;                /* +6: in range  */
            std::memcpy(e + 0, &rx, 2);
            std::memcpy(e + 2, &ry, 2);
            std::memcpy(e + 4, &rc, 2);
            std::memcpy(e + 6, &rd, 2);
            port::touch_ring_advance();
            ++host_stores;
        }
        {
            const char *watch = std::getenv("SM64DS_TP_RING_WATCH");
            if (watch && watch[0] != '0')
                std::fprintf(stderr, "[tp-ring-watch] f%d rom=%lu host=%lu "
                             "reader(a=%u b=%u c=%u d=%u)\n",
                             f, port_tsc_arm7_sample_count(), host_stores,
                             data_020a0dd8[0], data_020a0dd8[1],
                             data_020a0dd8[2], data_020a0dd8[3]);
        }
    }

    /* ---- THE STYLUS IN THE FLIGHT RECORDER (run link60, lane TCH2) ---------
     *
     * WHAT WAS MISSING WAS THE LINE, NOT THE TOUCH. Until now poll_touch wrote
     * the record and said nothing unless SM64DS_TOUCH_PROBE was set, and no
     * play session sets it. So a playlog from a real session carried no
     * evidence about the stylus AT ALL, and the only touch-shaped lines in it
     * came from walk_window's WM_LBUTTONDOWN handler -- a different path, with
     * a different question, that publishes into g_mouse_click_* which nothing
     * reads. A session where the stylus worked perfectly and a session where
     * it was never polled produced the SAME playlog, and the wrong one of the
     * two was the reading everybody took. That is what these lines close.
     *
     * THREE EDGES, not a line per frame: a press, a release, and a refusal.
     * The refusal is the one that earns its place -- the button is physically
     * down and the cursor IS inside the client area, and the transform still
     * declined it. In the stacked layout that is the ordinary, correct answer
     * for a click on the TOP half, so the line says which surface it landed on
     * rather than implying a fault. Said once per press, because a held button
     * over the top screen is one decision and not thirty a second.
     *
     * A drag logs its two ends. The path between them is the record itself and
     * SM64DS_TOUCH_PROBE prints that per frame when a lane wants it.
     *
     * QUIET WHERE IT HAS TO BE. g_headless kills the live branch above, so a
     * SM64DS_WINDOW_SELFTEST run or any windowless binary reaches this with
     * down == 0 and live_seen == 0 on every poll and prints nothing, ever. The
     * BMP battery cannot see these lines and neither can stdout: this is
     * stderr, which walk_window has already pointed at playlog/. */
    {
        static int down_was, held_from, refused_said, off_was;
        static unsigned char last_x, last_y;
        if (down && !down_was) {
            /* THE CLIENT POINT IS ONLY PRINTED WHEN THERE WAS ONE. A
               SM64DS_TOUCH_PROBE poke sets `down` with no mouse behind it, and
               printing client(0,0) for it would put a coordinate nobody
               measured into the flight recorder -- and would then be read back
               by anything checking the published DS pixel against the client
               point on its own line. Say which kind of press it was instead. */
            if (live_seen)
                std::fprintf(stderr, "[touch] f%d PRESS client(%d,%d) -> DS "
                             "(%u,%u) on the %s screen\n", f, live_cx, live_cy,
                             sx, sy,
                             hal_sub_screen_stacked() ? "bottom (stacked)"
                                                      : "bottom (inset panel)");
            else
                std::fprintf(stderr, "[touch] f%d PRESS (scripted probe, no "
                             "mouse) -> DS (%u,%u)\n", f, sx, sy);
            std::fflush(stderr);
            ++g_tp_edges;
            held_from = f;
            refused_said = 0;
            off_was = 0;
        } else if (!down && down_was) {
            ++g_tp_releases;
            std::fprintf(stderr, "[touch] f%d release after %d frame(s), last "
                         "DS (%u,%u)%s\n", f, f - held_from, last_x, last_y,
                         off_was ? ". The button came up OFF the stylus "
                                   "surface, which is where the release edge "
                                   "belongs." : "");
            std::fflush(stderr);
            refused_said = 0;
            off_was = 0;
        }
        /* BOTH ENDS OF AN EXCURSION, because the middle of one is now a thing
           that happens. A player who drags past the window edge and back gets
           two lines and keeps ONE press between them; the old build put a
           release here and a fresh press on the way back, and whatever the
           player was dragging read that release as an instruction. Two lines
           rather than one per frame, for the reason the refusal below is said
           once: a held button over the desktop is one decision, not thirty a
           second. */
        if (down && live_seen) {
            if (live_clamped && !off_was) {
                off_was = 1;
                std::fprintf(stderr, "[touch] f%d drag LEFT the stylus surface "
                             "at client(%d,%d), holding at DS (%u,%u) until "
                             "the button comes up\n",
                             f, live_cx, live_cy, sx, sy);
                std::fflush(stderr);
            } else if (!live_clamped && off_was) {
                off_was = 0;
                std::fprintf(stderr, "[touch] f%d drag back ON the stylus "
                             "surface at client(%d,%d) -> DS (%u,%u)\n",
                             f, live_cx, live_cy, sx, sy);
                std::fflush(stderr);
            }
        }
        /* THE LATCH CLEARS ON THE BUTTON, NOT ON THE TOUCH. `refused_said`
           stops a held press over the top screen printing thirty lines a
           second, and the release branch above only runs when a press was
           PUBLISHED -- so keying the reset off that alone made the first
           refused press the only one a run ever reported, and a two-point
           top-half fixture read as a one-point one. The physical button coming
           up is the edge that ends a refusal. */
        if (!live_seen)
            refused_said = 0;
        if (down) {
            last_x = sx;
            last_y = sy;
            g_tp_last_x = sx;
            g_tp_last_y = sy;
        } else if (live_seen && !live_on && !refused_said) {
            /* NOT "off-picture", and not a fault. Say where it landed. */
            int fx = 0, fy = 0;
            const int on_top = hal_present_client_to_fb(live_cx, live_cy,
                                                        &fx, &fy);
            refused_said = 1;
            std::fprintf(stderr, "[touch] f%d press client(%d,%d) is NOT on "
                         "the stylus surface: %s -- no touch published\n",
                         f, live_cx, live_cy,
                         live_handle
                             ? "the map's corner handle took it"
                             : (g_swap
                                    ? "the composed level-clear picture takes "
                                      "a click on the three buttons and "
                                      "nowhere else"
                                    : (on_top ? "it is on the TOP screen"
                                              : "it is in a letterbox bar")));
            std::fflush(stderr);
        }
        down_was = down;
    }

    if (tp) {
        const unsigned char *b = data_020a0de8;
        std::fprintf(stderr,
            "[touch] f%d poke=%s(%u,%u,%u) pre={de8=%u de9=%u dea=%u deb=%u} "
            "post={de8=%u de9=%u dea=%u deb=%u} d=(%d,%d,%d) "
            "block=%02x%02x%02x%02x "
            "%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x\n",
            f, tp->poke ? "yes" : "no", down, sx, sy,
            pre8, pre9, prea, preb,
            tp_rd(data_020a0de8, 0), tp_rd(data_020a0de9, 0),
            tp_rd(data_020a0dea, 0), tp_rd(data_020a0deb, 0),
            (int)(data_020a0de9 - data_020a0de8),
            (int)(data_020a0dea - data_020a0de8),
            (int)(data_020a0deb - data_020a0de8),
            tp_rd(b, 0), tp_rd(b, 1), tp_rd(b, 2), tp_rd(b, 3),
            tp_rd(b, 4), tp_rd(b, 5), tp_rd(b, 6), tp_rd(b, 7),
            tp_rd(b, 8), tp_rd(b, 9), tp_rd(b, 10), tp_rd(b, 11),
            tp_rd(b, 12), tp_rd(b, 13), tp_rd(b, 14), tp_rd(b, 15));
    }
}

}  // namespace

extern "C" {

/* THE FOCUS GATE'S ONE QUESTION: is this program's window the one the player
   is actually typing into?

   It has to be asked because GetAsyncKeyState reports PHYSICAL key state for
   the whole session, foreground or not. Without this, a player who alt-tabbed
   into a chat window and typed kept walking Mario around behind them, and a
   held direction stayed held forever.

   Everything INTERACTIVE routes its keyboard reads through here. Nothing
   scripted does: SM64DS_WINDOW_SELFTEST kills the live reads a layer up,
   SM64DS_PROBE_INPUT ORs its bits straight into the pad mirror, and the
   SM64DS_SELFTEST_* probes write the stick and the buttons by hand. A hidden,
   unfocused, automated run therefore sees exactly what it saw before.

   Two deliberate fail-open cases, so this can never be what silently locks a
   run out of its own input: no window yet (g_hwnd unset, which is every
   binary that does not open one) and no GetForegroundWindow (never on
   Windows, but the whole user32 surface here is loaded by name). */
int hal_window_focused(void)
{
    if (g_nofocusgate) return 1;
    if (!g_hwnd || !GetForegroundWindow_) return 1;
    return GetForegroundWindow_() == g_hwnd ? 1 : 0;
}

/* THE HALF EVERY MODE NEEDS, split out of hal_sub_screen_init so the scene
   path can have it without the half that belongs to a level.
 *
 * THE SEAM IS THE ROM'S, not a convenience. Three things live above it:
 *
 *   1. the pure-host wiring (hwnd, zoom, the env knobs, the user32 imports).
 *      No ROM equivalent; both modes need it.
 *   2. func_02053c40's display reset -- POWCNT1 bit 15 and the BG affine
 *      identity words. That function is the FOURTH call in func_0201a054,
 *      which main() calls exactly once, and nothing else in the ROM calls it.
 *      It is ONCE-PER-BOOT, before Scene::PrepareToSpawnBoot, so it precedes
 *      every scene and every level alike.
 *   3. OAM::EnableSubOAM, for a reason that is the port's and not the DS's --
 *      see the block below.
 *
 * Everything below the seam is Stage::InitResources' -- Stage::LoadGraphics2D,
 * data_0209d454 = 0x18, GXS::SetGraphicsMode(3) -- and a non-level scene must
 * not get it. dScStarSel_c::InitResources (src/_ZN12dScStarSel_c13InitResourcesEv.cpp) does its
 * own sub bring-up and reaches a DIFFERENT answer at every point:
 *
 *              Stage::InitResources      dScStarSel_c::InitResources
 *   BG mode    GXS::SetGraphicsMode(3)   GXS::SetGraphicsMode(0)   line 124
 *   layers     data_0209d454 = 0x18      data_0209d454 = 0x10      line 419
 *              (BG3 minimap + OBJ)       (OBJ only; no minimap)
 *   sub assets Stage::LoadGraphics2D     its own LoadFile chain to
 *                                        0x06600000 + GXS::LoadOBJPltt
 *                                        + GXS::*LoadOBJExtPltt  lines 230-248
 *
 * Running the Stage half on the scene path would force the minimap's extended
 * affine mode onto a screen with no minimap, switch on a BG3 the scene never
 * loaded, and drop the level's 2D asset set over the sub OBJ tiles the scene
 * had just loaded for itself. Measured, before any of this changed: a scene-4
 * run already reaches DISPCNT_B 00011000 with mask 10 -- BG mode 0, display
 * mode 1, OBJ only -- which is dScStarSel_c's own line 419 and 421, arrived at
 * with no help from this file at all. The sub DISPLAY was never the thing that
 * was missing.
 *
 * Idempotent, and it has to be: the level path reaches it through
 * hal_sub_screen_init and a future caller could reach it twice. */
void hal_sub_screen_init_hw(void *hwnd, int zoom)
{
    static int armed;

    g_hwnd = (HWND)hwnd;
    g_zoom = zoom > 0 ? zoom : 1;
    g_on = env_flag("SM64DS_SUB_PANEL", 1) != 0;
    /* NO WINDOW, NO INTERACTIVE KEYBOARD -- and this clause is here because
       splitting the function created the case that needs it.

       Two behaviours that are each correct compose into a wrong one the first
       time this runs on a windowless path. The focus gate FAILS OPEN on a null
       hwnd on purpose (hal_window_focused returns 1, so a binary that opens no
       window is never locked out of its own input), and the TAB latch in
       hal_sub_screen_frame_begin is dead only while GetAsyncKeyState_ is null.
       A scene run passes hwnd == nullptr and now loads user32 anyway, so
       without this the pointer goes live, the gate says yes, and a TAB held
       anywhere on the machine toggles the panel in the middle of an automated
       run and changes the BMP it writes.

       That is the exact non-determinism g_headless was added for, so a null
       hwnd carries it for the same reason SM64DS_WINDOW_SELFTEST does. The
       level path passes a real hwnd and is unaffected. */
    g_headless = std::getenv("SM64DS_WINDOW_SELFTEST") != 0 || hwnd == nullptr;
    g_nofocusgate = std::getenv("SM64DS_INPUT_NOFOCUSGATE") != 0;

    if (HMODULE u = LoadLibraryA("user32.dll")) {
        GetCursorPos_ = (decltype(GetCursorPos_))GetProcAddress(u, "GetCursorPos");
        ScreenToClient_ = (decltype(ScreenToClient_))GetProcAddress(u, "ScreenToClient");
        GetAsyncKeyState_ =
            (decltype(GetAsyncKeyState_))GetProcAddress(u, "GetAsyncKeyState");
        GetForegroundWindow_ =
            (decltype(GetForegroundWindow_))GetProcAddress(u, "GetForegroundWindow");
    }

    /* POWCNT1 bit 15: main engine drives the top screen. OAM::Load reads this
       bit to pick which shadow goes to which engine's OAM, and Scene::Reset-
       HardwareRegisters -- the ROM function that would set it -- is not in the
       port's boot. */
    *(volatile unsigned short *)0x04000304 |= 0x8000;

    /* THE AFFINE IDENTITY, and the reason the minimap drew nothing at all.
       BG3-sub in BG mode 3 is an EXTENDED AFFINE layer, so every one of its
       pixels goes through BG3PA..BG3PD before it picks a map entry. The ROM
       seeds those to 0x100 (1.0 in 8.8) in func_02053c40 -- the hardware reset
       that runs once from func_0201a054, the full boot the port does not have
       -- and nothing afterwards writes them for a minimap that is not being
       rotated. Left at zero the matrix is degenerate: every screen pixel maps
       to map pixel (0, 0), so the layer samples one blank tile and the whole
       panel comes out backdrop white. Minimap::InitResources configures BG3CNT
       and turns the layer on and is right to; it never touches the matrix.

       THIS IS FOUR OF THE EIGHT WORDS func_02053c40 WRITES, and the comment
       used to claim both engines. It does not: every address below is
       0x040010xx, the SUB engine. The ROM also seeds the main engine's four
       at 0x04000020 / 26 / 30 / 36, and the port does not. Sub BG2 is covered
       here so a later affine BG2 on THIS engine does not land on the same
       puzzle; main BG2 and BG3 are still unseeded, which is a real gap and
       deliberately not closed from this line -- writing main-engine display
       registers during bring-up can move the level frame, and that is a
       measurement somebody has to make rather than a word somebody changes. */
    *(volatile unsigned short *)0x04001020 = 0x100;   /* BG2PA sub */
    *(volatile unsigned short *)0x04001026 = 0x100;   /* BG2PD sub */
    *(volatile unsigned short *)0x04001030 = 0x100;   /* BG3PA sub */
    *(volatile unsigned short *)0x04001036 = 0x100;   /* BG3PD sub */

    /* DUAL OAM, and this is the one line the bottom screen actually turns on.
     *
     * WHAT data_0209e660 IS. config/arm9/symbols.txt has it kind:bss, so the
     * DS boots it to ZERO, and zero is the two-engine value:
     *
     *     OAM::Load    e660 != 0 -> func_020566dc(e674) and RETURN.  That is
     *                               0x07000000, engine A, alone.
     *                  e660 == 0 -> func_020566dc + func_02056674, which is
     *                               0x07000000 AND 0x07000400 -- both engines,
     *                               with POWCNT1 bit 15 choosing which shadow
     *                               lands on which.
     *     OAM::Render  `!sub || data_0209e660 == 1` sends every sub==true
     *                  sprite into the MAIN shadow instead.
     *
     * So on the DS the bottom screen is live from boot and only ov006's
     * minigames ever set the flag (_ZN14dScMgD3DBase_c25OnAimedAtWithEggReturnVecEv, _ZN14dScMgD3DBase_c9Virtual84Ev),
     * clearing it again on the way out.
     *
     * WHY THE PORT NEEDS A CALL AT ALL. hal/model_host.cpp:91 hosts the global
     * as `unsigned char data_0209e660 = 1;` -- deliberately, and NOT the DS's
     * boot value. That pin is a port-side default, and while it stands every
     * sub sprite is redirected into the main shadow and engine B's OAM is
     * never uploaded. The level path has always covered it by calling
     * EnableSubOAM here; a scene run reached neither, which is the whole
     * defect. OAM::EnableSubOAM is the ROM's own name for `e660 = 0`
     * (src/_ZN3OAM12EnableSubOAMEv.cpp is that one store and nothing else), so
     * calling it is how the port spells "restore the DS's boot state" without
     * inventing a write.
     *
     * THE PIN ITSELF IS NOT THIS LANE'S TO PULL. Clearing e660's initialiser
     * in model_host.cpp would be the ROM-faithful fix and it is that file's
     * call: port/tests/smoke_oam.cpp:42 asserts `data_0209e660 == 1` as the
     * HAL's documented direct-fill mode, and the layout guard below exists
     * because the alternative mode needs the three shadow symbols adjacent.
     * ROUTED, not fixed here.
     *
     * Guarded on the layout check for the original reason: in mode 0 the Reset
     * path fills the shadow through data_0209e67c/data_0209e694, and if those
     * are separate host arrays it would leave 127 of 128 entries as garbage
     * and draw the heap onto the bottom screen. */
    /* The OTHER grouped-section band, checked in the same breath and for the
       same reason. hal/cxx_aliases.cpp lays the GX bank state out in ROM order
       so the SetBankFor* family's member writes land on the words
       func_020540f0 and its siblings read by name; if that stops holding,
       GXS::EndLoadOBJExtPltt restores bank 0 and takes DISPCNT_B bit 31 with
       it, and the sub screen's 256-colour sprites quietly lose their extended
       palette. Checked on BOTH paths because this is the shared half.

       THE RETURN IS LOAD-BEARING, the way the sibling's below is, and it was
       not on review. A bare call put the failure into one line of a run's
       stdout and then carried straight on into the silent-zero behaviour the
       band exists to remove: no fault, no red, and a bottom screen that is
       merely wrong. Nothing here can repair a layout at run time, so there is
       no fallback to take -- what the branch owes is to say what is now untrue
       and to stop a proof run from reporting green over it.
       SM64DS_FAULTS_FATAL is the battery's own switch (port/tools/battery.py
       sets it on every level and scene step), so a regression lands as a
       failed step instead of as scrollback. */
    if (!port_gxbank_layout_check()) {
        std::printf("[gx] the SetBankFor* family is writing past its own "
                    "state: DISPCNT_B bits 30 and 31 will not survive a "
                    "Begin/EndLoad*ExtPltt pair, so sub extended palettes are "
                    "off and neighbouring hosted globals are being clobbered\n");
        if (std::getenv("SM64DS_FAULTS_FATAL")) {
            /* FLUSH STDOUT BEFORE THE ABORT. std::abort does not flush stdio,
               and walk_window's stdout is a pipe in every automated run, so
               the two lines above exist only in a buffer at this point. The
               first cut of this branch aborted with them still in it: nonzero
               exit, empty stdout, a quieter failure than the one it replaced.
               Measured with the band deliberately mis-aligned -- zero bytes of
               stdout before this flush, both lines after.
               The stderr line was never at risk and is not why the flush is
               here: walk_window reopens stderr onto playlog/play_*.log, so it
               lands there either way, which is also where to look for it. */
            std::fprintf(stderr, "  (SM64DS_FAULTS_FATAL: GX bank band not "
                                 "contiguous -> hard abort)\n");
            std::fflush(stdout);
            std::fflush(stderr);
            std::abort();
        }
    }

    if (hal_oam_layout_check()) {
        _ZN3OAM12EnableSubOAMEv();
        std::printf("[sub] dual OAM armed (data_0209e660 = %u)\n",
                    data_0209e660);
    } else {
        std::printf("[sub] dual OAM NOT armed: sub sprites stay on the main "
                    "shadow\n");
    }
    OAM::Reset();

    std::printf("[sub] panel %s (TAB toggles, SM64DS_SUB_PANEL=0 to start "
                "off)\n", g_on ? "on" : "off");
    if (armed) {
        std::printf("[sub] hw bring-up was already armed; re-armed idempotently"
                    "\n");
    }
    armed = 1;
}

/* The LEVEL path's bring-up. Only the shared, process-lifetime half lives
   here now: POWCNT1, the BG3 affine identity, OAM::EnableSubOAM, OAM::Reset
   and port_gxbank_layout_check, all inside hal_sub_screen_init_hw. The Stage
   half -- Stage::SetVramBanks, the sub DISPCNT block, the layer mask and
   Stage::LoadGraphics2D -- moved to hal_sub_screen_level_init below, because
   Stage::InitResources runs that half on EVERY level entry and this function
   used to run it once a process. That means the first boot now reaches the
   Stage half BEFORE the hardware half runs here, rather than after: the call
   site is walk_window.cpp:9316, which still calls hal_sub_screen_init once at
   boot and hal_sub_screen_level_init on every level entry afterward. Nothing
   in the hardware half reads anything the Stage half writes, so the order
   swap on that first boot changes nothing it depends on. */
void hal_sub_screen_init(void *hwnd, int zoom)
{
    hal_sub_screen_init_hw(hwnd, zoom);
}

/* Stage::InitResources' own sub-screen configuration (InitResources:262-351),
   run from port_stage_boot_body on every level entry instead of once at
   process start. Order matches InitResources': SetVramBanks first, then the
   DISPCNT block, then the layer mask, then LoadGraphics2D -- so LoadGraphics2D
   runs before Stage::LoadClsnAndObjects spawns the Minimap, the ROM's own
   order, and the minimap's GXS::LoadBGPltt(f2, 0, 2) writes palette entry 0
   last instead of having it overwritten afterward. */
extern "C" void hal_sub_screen_level_init(void)
{
    /* Stage::SetVramBanks, InitResources:262-263. Ends in
       GX::SetBankForSubBGExtPltt(0x80), which sets DISPCNT_B bit 30, writes
       VRAMCNT_H = 0x82 and RECORDS 0x80 in data_020a609e. That record is what
       Minimap::InitResources' own GXS::BeginLoadBGExtPltt / EndLoadBGExtPltt
       round trip reads back to restore the bit after clearing it -- without
       this call nothing is recorded, the restore passes 0, and the bit stays
       off after the second and every later level entry. */
    _ZN5Stage12SetVramBanksEv();

    /* ENGINE A'S BG SCREEN BASE OFFSET, Stage::InitResources:272:
     *
     *     *p0 = (*p0 & ~0x38000000) | 0x08000000;      p0 = DISPCNT_A
     *
     * DISPCNT bits 27-29 are engine A's BG SCREEN base offset in 64K pages and
     * the ROM sets it to ONE. It is the statement that keeps the main engine's
     * four tilemaps out of its own character data, and the port was not making
     * it: DISPCNT_A read back 0x00001d08 at the level-clear menu, bits 27-29
     * zero, so every engine-A tilemap sat 0x10000 BELOW where the cartridge
     * puts it.
     *
     * WHAT THAT COST, measured on a star return with the save menu up. The
     * fonts live at func_02054d88() (src/func_02054d88.c), which on this path
     * is 0x06000000: the small 8x16 message font at +0, and the big 16x16 menu
     * font that LoadFont(0) decompresses at +0x8000 (src/LoadFont.cpp:39).
     * With the offset missing BG3_A's tilemap lands at 0x06008800 and BG1_A's
     * at 0x0600f800, both INSIDE the big font. So the moment the level-clear
     * banner drew its own text, func_0201d850's MultiStore16(0x2ff, ...) wrote
     * 0x800 bytes of blank-tile entries straight through the menu font, and
     * the three save-menu buttons came up as combed stripes instead of
     * letters. The 0x02ff halfwords and the runs of 0x0200, 0x0201, 0x0202
     * were readable in a dump of the font, which is how this was found.
     *
     * Nothing else has to change for it. The ROM's own pointer helpers read
     * the same bits (src/_ZN2G212GetBG3ScrPtrEv.cpp computes `sbase` from
     * DISPCNT & 0x38000000), and port/ntr/ppu.cpp:300-301 already honours the
     * offset on engine A, so the write and the scan-out move together.
     *
     * It sits here because this function is the port's copy of
     * Stage::InitResources:262-351, and :272 is inside that span, one
     * statement above the sub engine's own DISPCNT block below. */
    {
        volatile unsigned *p0 = (volatile unsigned *)0x04000000;
        *p0 = (*p0 & ~0x38000000u) | 0x08000000u;
    }

    /* THE SUB ENGINE'S OWN DISPCNT, which nothing in the port was setting --
       it read back 0, meaning "display off, no layers, no sprites".
       Every value here is Stage::InitResources', the ROM function that puts
       the bottom screen into gameplay shape and that the port does not run:

           *p1 &= 0xFFCFFFEF        OBJ mapping 2D, tile boundary 32
           GXS::SetGraphicsMode(3)  BG mode 3: BG0/1/2 text, BG3 EXTENDED --
                                    which is the mode the minimap needs, and
                                    the reason its 16-bit map entries carry a
                                    palette field at all
           *p1 |= 0x10000           display mode 1, the graphics display
           data_0209d454 = 0x18     BG3 + OBJ: the minimap and the sprites

       Bit 30 (BG extended palettes) is GX::SetBankForSubBGExtPltt's, reached
       from GXS::EndLoadBGExtPltt through the VRAM bank allocator the port
       does not host. Set here so the minimap's palettes are readable. Kept
       as belt-and-braces behind SetVramBanks above; removing it is a
       separate question this change does not answer. */
    {
        volatile unsigned *p1 = (volatile unsigned *)0x04001000;
        *p1 &= 0xFFCFFFEFu;
        *p1 = (*p1 & ~7u) | (unsigned)env_flag("SM64DS_SUB_BGMODE", 3);
        *p1 |= 0x10000u;            /* display mode 1 */
        *p1 |= 0x40000000u;         /* BG extended palettes */
        if (!data_0209d454)
            data_0209d454 = 0x18;   /* Stage::InitResources' own value */
    }

    /* THE LAYER MASK, and it is Stage::InitResources' own value:
     *
     *     data_0209d454 = 0x18;
     *
     * bit 3 BG3 -- the minimap -- and bit 4 OBJ -- every sprite the HUD and the
     * minimap draw. That one line is the gameplay bottom screen, and it lives
     * in the same ROM function this block already copies the DISPCNT words out
     * of, which the port does not run.
     *
     * Nothing else was going to set the OBJ bit. Minimap::Behavior maintains
     * bit 3 every frame -- ORs it in when it has a map id, clears it when it
     * does not -- but no hosted path touches bit 4, so with the in-game asset
     * set the mask came out 0x08 and the sprites were composited out of a
     * screen they had already been drawn into.
     *
     * SM64DS_SUB_LAYERS still overrides, and now it is a debugging knob rather
     * than the only way to see anything. */
    data_0209d454 = 0x18;

    /* THE BOTTOM SCREEN'S OWN VRAM. Stage::LoadGraphics2D is the ROM's 2D
       asset load and it fills both screens: the sub BG character data, the
       three sub tilemaps, the sub BG palette, and the BGxCNT_SUB words that
       say where all of it lives. Without it engine B scans out an empty bank
       and the panel is one flat backdrop colour.
       The bit at data_0209caa0[8] & 0x80 -- "the intro has played" -- picks
       between two entirely different asset sets:
         clear: files 0x239..0x23e, plain FAT files. The PRE-INTRO CLOUDS --
                BG0 characters, the BG0 and BG1 tilemaps, the sub BG palette,
                and the sub OBJ tiles with their palette.
         set:   the IN-GAME set, whose character data and half its tilemaps
                are language files (handles 0xa00d / 0xa009 / 0xa00b) living
                in ARCHIVE/cee.narc.

       THE IN-GAME SET IS THE DEFAULT NOW. It was not, for one reason: the old
       extraction was a US dump that did not carry cee.narc, so the in-game
       tilemaps loaded and every tile they named came out blank, and the clouds
       were the branch whose bytes existed. The EU dump has the archive and all
       three handles resolve, so the port takes the branch the game takes while
       a level is being played.

       It also matters for what the sprites can be SEEN over. The cloud set
       loads its three BGs at priority 0 and every cloud pixel is opaque, so the
       whole HUD lost the priority compare against a backdrop that should not
       have been on screen at all. SM64DS_GFX2D_PREINTRO=1 puts the clouds
       back. */
    if (!std::getenv("SM64DS_NO_GFX2D")) {
        const unsigned char saved = data_0209caa0[8];
        if (std::getenv("SM64DS_GFX2D_PREINTRO"))
            data_0209caa0[8] &= ~0x80;
        else
            data_0209caa0[8] |= 0x80;
        Stage::LoadGraphics2D(false, data_0209f2f8);
        data_0209caa0[8] = saved;
        std::printf("[sub] Stage::LoadGraphics2D(0, %d) done, layer mask "
                    "data_0209d454 = %02x\n", (int)data_0209f2f8,
                    data_0209d454);
    }
}

/* Top of the 2D frame: both shadows back to "every sprite disabled" and both
   entry counters to zero, so this frame's Render calls fill from the start. */
void hal_sub_screen_frame_begin(void)
{
    /* Once, on the first frame, and deliberately here rather than in init:
       by now every pointer pass has run -- ovdata's per-overlay one from the
       harness, the cross-overlay fixups from hal_fill_hud_vtable and
       hal_fill_minimap_vtable -- and the HUD and the Minimap are about to walk
       what those passes produced. hal/oam_lists.cpp says what a missed entry
       costs and why the fault it causes lands nowhere near it. */
    hal_oam_templates_check();
    /* SM64DS_OAM_WALK_PROBE=1: show the fault itself, on a guarded page, so
       the mechanism is reproducible rather than one run in a couple of
       hundred. Once, and only when asked for. */
    {
        static int probed;
        if (!probed && std::getenv("SM64DS_OAM_WALK_PROBE")) {
            probed = 1;
            hal_oam_walk_probe();
        }
    }

    /* TAB is interactive keyboard, so it is gated on focus like the rest. The
       latch is ARMED rather than cleared while the window is in the
       background: a TAB pressed elsewhere and still down on the way back
       reads as already-held, so it cannot toggle the panel on arrival.

       IT IS INERT IN THE STACKED LAYOUT. TAB hides the corner panel because
       the panel is furniture over the game; the stacked bottom half IS the
       game, and a key that blanked half the window and took the stylus with
       it (poll_touch is gated on g_on) would be a way to lose a minigame by
       leaning on the keyboard. The window size is fixed at creation for the
       same reason, so there is nothing for a toggle to fall back to. */
    static int tab_was;
    if (GetAsyncKeyState_ && !g_headless && !hal_sub_screen_stacked()) {
        if (!hal_window_focused()) {
            tab_was = 1;
        } else {
            const int tab = (GetAsyncKeyState_(VK_TAB) & 0x8000) != 0;
            if (tab && !tab_was) g_on = !g_on;
            tab_was = tab;
        }
    }
    OAM::Reset();
    poll_touch();
    /* Inert unless SM64DS_TOUCH_CLIENT_PROBE is set, and here rather than
       inside poll_touch because it wants a present rectangle the frame loop
       has actually published. */
    hal_touch_client_probe();
}

/* ---- THE DISPLAY-ROUTING PROBE, default off --------------------------------
 *
 * SM64DS_SCREENS_PROBE=1 prints the registers that decide WHICH ENGINE'S OUTPUT
 * LANDS ON WHICH PHYSICAL SCREEN, and it prints them the only way a
 * per-frame-toggling register can honestly be reported: once at the first
 * frame, again on every CHANGE to the tuple, and a census at the end of the
 * run. A sample taken at one frame of a scene whose slot 24 flips the bit every
 * frame is not a measurement of anything.
 *
 * The four registers, all GBATEK:
 *   POWCNT1   0x04000304  bit 15 = display swap (1 = engine A to the UPPER
 *                         screen, 0 = to the lower one)
 *   DISPCAPCNT 0x04000064 the display capture unit: bit 31 enable (the
 *                         hardware clears it itself at the end of the captured
 *                         frame), bits 16-17 destination VRAM block, 18-19
 *                         destination offset in 0x8000-byte units, 20-21 size
 *                         and 29-30 source
 *   DISPCNT_B 0x04001000  engine B's mode and layer enables, which is what says
 *                         whether the captured frame has a layer to show
 *                         through
 *   BG2CNT_B  0x0400100c  the bitmap BG's own word when it has one
 *
 * NOTHING HERE CHANGES A PIXEL. It reads four registers and writes stderr.
 */
void hal_screens_probe(void)
{
    static int on = -1;
    if (on < 0) on = env_flag("SM64DS_SCREENS_PROBE", 0);
    if (!on) return;
    static unsigned long frame;
    static unsigned long swaps;      /* frames whose bit 15 differed from the last */
    static unsigned long lower;      /* frames with engine A on the lower screen */
    static unsigned long cap_armed;  /* frames that entered with capture enabled */
    static unsigned last_key = ~0u;
    static int last_bit = -1;

    const unsigned pow1 = *(volatile unsigned short *)0x04000304;
    const unsigned cap = *(volatile unsigned *)0x04000064;
    const unsigned dispb = *(volatile unsigned *)0x04001000;
    const unsigned bg2b = *(volatile unsigned short *)0x0400100c;
    const int bit = (pow1 >> 15) & 1;

    ++frame;
    if (!bit) ++lower;
    if (cap & 0x80000000u) ++cap_armed;
    if (last_bit >= 0 && bit != last_bit) ++swaps;
    last_bit = bit;

    /* the two banks the capture path moves, C and D, and their VRAMCNT bytes:
       bit 7 enable, bits 0-2 MST, bits 3-4 OFS. 0x84 is the value both arms
       write -- enabled, MST 4, which is engine B BG for C and engine B OBJ for
       D -- and 0x80 is Vram__Map's "back to LCDC". */
    const unsigned vc = *(volatile unsigned char *)0x04000242;
    const unsigned vd = *(volatile unsigned char *)0x04000243;

    const unsigned key = (unsigned)bit | (cap & 0xF0FF0000u) |
                         ((dispb & 0x1F07u) << 1) | (vc << 20) | (vd << 12);
    if (key != last_key) {
        last_key = key;
        std::fprintf(stderr,
                     "  [screens] f%-6lu POWCNT1 %04x (engine A -> %s) "
                     "DISPCAPCNT %08x (%s block %u ofs %u size %u src %u) "
                     "DISPCNT_B %08x mode %u BG %c%c%c%c OBJ %c BG2CNT_B %04x "
                     "VRAMCNT_C %02x VRAMCNT_D %02x\n",
                     frame - 1, pow1, bit ? "UPPER" : "LOWER", cap,
                     (cap & 0x80000000u) ? "ARMED" : "idle",
                     (cap >> 16) & 3, (cap >> 18) & 3, (cap >> 20) & 3,
                     (cap >> 29) & 3, dispb, dispb & 7,
                     (dispb & 0x0100) ? '0' : '-', (dispb & 0x0200) ? '1' : '-',
                     (dispb & 0x0400) ? '2' : '-', (dispb & 0x0800) ? '3' : '-',
                     (dispb & 0x1000) ? 'y' : '-', bg2b, vc, vd);
        std::fflush(stderr);
    }
    /* the census, every 300 frames: the numbers a per-frame flip is actually
       read off, which no single sample can carry */
    if (frame % 300 == 0) {
        /* AND THE FOUR BG-OFFSET SHADOWS WITH IT. func_02019144's tail is the
           only thing that publishes them, and its FIRST beat is the graphics
           block's slot 2 -- so with the block unseated the tail republished
           frozen boot values every frame while the scene's scroll word moved.
           Sampling them beside the display registers is what makes "the
           offsets follow the game" a reading rather than an inference: on a
           scrolling scene these move between censuses and on a still one they
           do not. */
        std::fprintf(stderr,
                     "  [screens] census f%lu: %lu frame(s) with engine A on "
                     "the LOWER screen, %lu bit-15 change(s), %lu frame(s) "
                     "entered with DISPCAPCNT armed; BG2 offsets A %u,%u "
                     "B %u,%u\n",
                     frame, lower, swaps, cap_armed,
                     *(volatile unsigned short *)0x04000018,
                     *(volatile unsigned short *)0x0400001a,
                     *(volatile unsigned short *)0x04001018,
                     *(volatile unsigned short *)0x0400101a);
        std::fflush(stderr);
    }
}

/* ---- THE OBJ/POWCNT1 PARITY PROBE, default off -----------------------------
 *
 * SM64DS_OBJ_PARITY=1 prints, once a frame, the three facts a claim about WHICH
 * SCREEN A SPRITE LANDED ON has to be made of, all sampled at the one moment in
 * the frame where all three are simultaneously true:
 *
 *   POWCNT1 bit 15   the arm the engine-A OBJ raster ran under THIS frame. The
 *                    raster is in hal/message_compositor.cpp, it ran a few
 *                    statements before this function was entered, and
 *                    func_02019144's slot-24 beat -- which writes this register
 *                    -- ran immediately before it. So this is the arm that
 *                    raster used, not a later one.
 *   HW OAM           0x07000000, which is what that raster read: the shadow
 *                    OAM::Load uploaded at the END of the PREVIOUS frame.
 *   THE SHADOW       data_0209e674 / data_0209e664, which is what the game
 *                    wrote during THIS frame's Render and what OAM::Load is
 *                    about to upload at the foot of this function.
 *
 * AND THE FRAMEWORK'S sel WORD, data_ov004_020beb68 + 0x4664. The slot-24 beat
 * has already flipped it by this point, so the value printed is the one the
 * raster's arm was derived from, and the shadow beside it was written under the
 * OTHER one. That is the whole measurement: on the dScMgD3DBase_c family the
 * ROM's own routers pick a sprite's screen mapping off sel, so a shadow written
 * under sel=s belongs to the arm the NEXT beat sets, and printing both says
 * whether this program pairs them the way the DS does.
 *
 * NOTHING HERE CHANGES A PIXEL. It reads two register words and two tables. */
extern "C" {
extern int data_0209e664;                    /* the main shadow's counter */
extern unsigned short data_0209e674[];       /* the main shadow itself */
extern unsigned char data_ov004_020beb68[4]; /* the framework's scene pointer */
}
/* which frame order this binary is running; defined with the upload below */
static int oam_load_late(void);
void hal_obj_parity_probe(void)
{
    static int on = -1;
    if (on < 0) on = env_flag("SM64DS_OBJ_PARITY", 0);
    if (!on) return;
    static int upto = -1;
    if (upto < 0) upto = env_flag("SM64DS_OBJ_PARITY_FRAMES", 40);
    static unsigned long frame;
    const unsigned long f = frame++;
    if ((long)f >= (long)upto) return;

    const int bit = (*(volatile unsigned short *)0x04000304 >> 15) & 1;
    unsigned g = (unsigned)data_ov004_020beb68[0]
               | ((unsigned)data_ov004_020beb68[1] << 8)
               | ((unsigned)data_ov004_020beb68[2] << 16)
               | ((unsigned)data_ov004_020beb68[3] << 24);
    int sel = -1;
    if (g) sel = (int)*(unsigned short *)(g + 0x4664);

    /* PARKED ENTRIES ARE NOT SPRITES. OAM::Reset leaves the unused slots at
       y = 0xc0 with tile 0, which is one row past the bottom of a 192-row
       screen and draws nothing; counting them would bury the handful of real
       entries under 116 blanks. Reported separately rather than dropped. */
    char hw[512];
    int hn = 0, hc = 0, hpark = 0;
    hw[0] = 0;
    for (int i = 0; i < 128; ++i) {
        const unsigned short a0 = *(volatile unsigned short *)(0x07000000u + i * 8u);
        const unsigned short a1 = *(volatile unsigned short *)(0x07000000u + i * 8u + 2);
        const unsigned short a2 = *(volatile unsigned short *)(0x07000000u + i * 8u + 4);
        if (!(a0 & 0x100) && (a0 & 0x200)) continue;   /* disabled */
        if (!a0 && !a1 && !a2) continue;
        if ((a0 & 0xFF) == 0xC0 && !a1 && !a2) { ++hpark; continue; }
        ++hc;
        if ((size_t)hn < sizeof hw - 32)
            hn += std::snprintf(hw + hn, sizeof hw - (size_t)hn,
                                " %d:(%d,%d,t%u)", i, (int)(a1 & 0x1FF),
                                (int)(a0 & 0xFF), (unsigned)(a2 & 0x3FF));
    }

    char sh[512];
    int sn = 0;
    sh[0] = 0;
    const int scount = data_0209e664;
    for (int i = 0; i < scount && i < 128; ++i) {
        const unsigned short a0 = data_0209e674[i * 4 + 0];
        const unsigned short a1 = data_0209e674[i * 4 + 1];
        const unsigned short a2 = data_0209e674[i * 4 + 2];
        if ((size_t)sn < sizeof sh - 32)
            sn += std::snprintf(sh + sn, sizeof sh - (size_t)sn,
                                " %d:(%d,%d,t%u)", i, (int)(a1 & 0x1FF),
                                (int)(a0 & 0xFF), (unsigned)(a2 & 0x3FF));
    }

    /* WHICH sel THE DISPLAYED BLOCK WAS SUBMITTED UNDER, and it depends on
       where the upload is. With the ROM-ordered upload the hardware OAM IS the
       shadow this frame's Render just wrote, and that Render ran before the
       beat, so it saw the PREVIOUS sel. With the late upload the hardware OAM
       is the block written one frame earlier still, which on a period-2
       alternation is the sel the beat has just set -- the defect. */
    const int shadow_sel = sel < 0 ? -1 : 1 - sel;
    const int shown_sel = oam_load_late() ? sel : shadow_sel;
    std::fprintf(stderr,
                 "[objparity] f%-4lu arm=%s sel=%d upload=%s | DISPLAYED "
                 "(hw oam, %d live + %d parked, submitted under sel=%d):%s | "
                 "SHADOW (written this frame under sel=%d, %d entr(y|ies)):%s\n",
                 f, bit ? "UPPER" : "LOWER", sel,
                 oam_load_late() ? "LATE" : "ROM", hc, hpark, shown_sel,
                 hw[0] ? hw : " -", shadow_sel, scount,
                 sh[0] ? sh : " -");
    std::fflush(stderr);
}

/* ---- THE OBJ/POWCNT1 PARITY: func_02019144's OAM UPLOAD, AT THE ROM'S PLACE --
 *
 * WHAT WAS WRONG, and it is a frame-order fact about this program rather than
 * anything in the game's code.
 *
 * src/func_02019144.c is the DS's once-per-frame display sync and its order is
 * fixed: dispatch the current graphics block's slot 2 (which for every minigame
 * reaches the scene's slot 24, and for the dScMgD3DBase_c family that slot is
 * what writes POWCNT1's display-swap bit), then OAM::Flush, then OAM::Load,
 * then the two DISPCNT layer-mask publishes, then the eight BG offsets. It runs
 * in VBLANK, so the frame it configures is scanned out AFTER it: the sprites on
 * screen during a frame are the ones the upload at the head of that frame put
 * in the hardware OAM, and the game wrote them during the frame BEFORE.
 *
 * This program ran the beat and the engine-A publish at the head of the display
 * path (hal/message_compositor.cpp) and then rasterised engine A's OBJ -- but
 * the upload was at the FOOT of this function, below both rasters. So both
 * screens drew from upload N-1 while the arm they were placed under came from
 * beat N, and the game code that wrote upload N-1 had run one beat earlier
 * still. Two beats between the submission and the arm; on a family whose arm
 * alternates EVERY frame, two beats is the same parity as none, and every
 * sprite landed on the opposite screen from the one the ROM puts it on.
 *
 * THE ROM SAYS WHICH SCREEN, three independent ways, all in matched src:
 *
 *   src/Hud_RenderSprite.cpp        its dScMgD3DBase_c arm draws ONLY while
 *                                   sel (scene + 0x4664) is 0, and its ordinary
 *                                   arm is OAM::RenderSub -- the SUB engine,
 *                                   which is the bottom screen.
 *   src/RenderOamMainScreen.cpp     its D3D arm draws ONLY while sel is 1, and
 *                                   its ordinary arm is OAM::Render(draw=0) --
 *                                   the MAIN engine, the top screen.
 *   src/RenderOamBothScreens.cpp    its D3D arm submits at y + 0xc0 + G while
 *                                   sel is 1 and at y while sel is 0, and 0xc0
 *                                   + G is exactly the offset its ordinary arm
 *                                   uses for the TOP screen's copy.
 *
 * All three agree: a submission made while sel == 1 belongs on the top screen
 * and one made while sel == 0 belongs on the bottom. src/actors/dScMgD3DBase_c.cpp
 * -- slot 24 -- sets POWCNT1 the other way round on the same frame (sel == 1
 * clears bit 15, which sends engine A to the LOWER screen), so on the DS a
 * submission is displayed under the arm the NEXT beat sets. That is not a
 * coincidence to be worked around; it is the double buffering, and putting the
 * upload back where func_02019144 has it reproduces it exactly.
 *
 * MEASURED BOTH WAYS with SM64DS_OBJ_PARITY=1 above, scene 372.
 *
 * SM64DS_OAM_LOAD_LATE=1 puts the old foot-of-the-frame upload back on this
 * same binary, so a before/after is one build and one asset base -- which is
 * what notes/port-selftest-bmp-gate.md requires before two captures may be
 * compared at all.
 *
 * WHAT DOES NOT CHANGE. Both engines still read ONE upload, which is the whole
 * of the split-sprite cure the late position was reached for: the two halves of
 * a sprite straddling the gapless seam still draw from the same block, it is
 * simply the block the ROM would have them draw from. OAM::Flush is NOT added
 * here -- it is CP15 cache maintenance (src/_ZN3OAM5FlushEv.cpp) with nothing to
 * do on a host, and this program has never called it. */
static int oam_load_late(void)
{
    static int v = -1;
    if (v < 0) v = env_flag("SM64DS_OAM_LOAD_LATE", 0);
    return v;
}

/* Called from hal/message_compositor.cpp between func_02019144's slot-2 beat
   and its engine-A layer publish, which is the line the ROM has it on, and
   therefore ahead of both OBJ rasters. The caller has already asked the beat
   for its verdict and only calls this when the beat said to run the tail. */
extern "C" void port_frame_oam_upload(void)
{
    if (oam_load_late()) return;
    _ZN3OAM4LoadEv();
    ntr::ppu_seam_oam_mark_uploaded();
}

/* ---- IS THE ATTENTION CUE RE-ANCHORED THIS FRAME? -------------------------
 *
 * Asked by engine A's raster (hal/message_compositor.cpp) once per frame,
 * before it decides whether to hold the three bouncing arrows back out of the
 * top screen's own buffer.
 *
 * The cue is the ROM's "look at the bottom screen": Stage::RenderBouncingArrows
 * puts three arrows along the bottom edge of the TOP screen and plays a sound,
 * because on a DS the thing to look at is the other panel. On this port the
 * bottom screen is a corner inset over the same picture, so the arrows point at
 * empty floor. With the improved map on they are moved to sit just above the
 * inset instead -- which is the whole request.
 *
 * SAME GATE AS THE REST OF THE OPTION: the improved map on, and the corner
 * inset layout, which is a course. Menus and minigames take the stacked
 * layout, where the bottom screen is a real second panel and the ROM's own
 * placement is already right. */
int hal_minimap_arrow_reanchor_on(void)
{
    return improved_map_on() && !hal_sub_screen_stacked();
}

/* THE LEVEL-CLEAR SAVE MENU'S OWN ANSWER, for the one reader outside this
   file. The window's mouse capture pins the pointer to the middle of the
   client area and hides it, and fences it inside that area -- so with the
   capture on a player cannot aim at the save menu at all: not at the corner
   panel, which the fence keeps him off, and not at the big picture either,
   because the pointer is not his to move. The capture's own rule is that it
   stays off wherever the pointer is really a pen, and a menu with three touch
   boxes is exactly that. Same predicate the swap reads, so the two cannot
   disagree about when the menu is up. */
extern "C" int hal_save_menu_up(void)
{
    return save_menu_is_up() ? 1 : 0;
}

/* THE COMPOSE'S TWO SOURCE BANDS, for hal/message_compositor.cpp, asked once
   at the head of engine A's composite. It is the ONE question that file asks
   this one, and it is the whole of the gate: off, it answers 0 and that file
   allocates nothing, clears nothing and changes no pixel.
   The rows are HOST rows of the picture, out of the very rectangles this
   frame's compose will draw into, so the pixels engine A lifts out are the
   pixels the compose puts back, and the two cannot drift apart.
   ASKED BEFORE hal_sub_screen_present HAS RUN THIS FRAME, which is why the
   rectangles read here are last frame's. They depend only on the picture's
   size, which does not change inside a frame, and swap_geom_for runs every
   frame whatever the answer -- so the numbers are right from the first
   composed frame. swap_now's own g_sw.pnum > 0 term is what keeps this quiet
   before the first frame's geometry exists. */
extern "C" int hal_lc_compose_rows(int *text_r0, int *text_r1,
                                   int *coin_r0, int *coin_r1)
{
    /* THE PROMISE FROM LAST FRAME, CHECKED. If the lift was armed and
       swap_present never ran, the lettering and the coin total were taken out
       of that frame's picture and nothing put them back -- the defect this
       file's swap_now banner describes. It cannot happen while every term the
       draw depends on is in swap_now, which is the point of saying so here
       rather than trusting the two to stay in step. Once per process, to
       stderr, never fatal: a sweep row that trips it stays readable. */
    if (g_lc_lift_pending) {
        static int said;
        if (!said) {
            said = 1;
            std::fprintf(stderr, "[compose] LIFT WITHOUT DRAW: the level-clear "
                         "lettering was lifted out of a frame swap_present "
                         "never reached; swap_now is missing a term the draw "
                         "depends on\n");
            std::fflush(stderr);
        }
        g_lc_lift_pending = 0;
    }
    if (!swap_now()) return 0;
    g_lc_lift_pending = 1;
    *text_r0 = g_sw.ty_src;
    *text_r1 = g_sw.ty_src + g_sw.t_h;
    *coin_r0 = g_sw.cy_src;
    *coin_r1 = g_sw.cy_src + g_sw.c_h;
    return 1;
}

/* Bottom of the frame: upload the shadows the game filled, rasterise engine B,
   drop it into the corner. With the panel off nothing here writes a pixel. */
void hal_sub_screen_present(unsigned int *dst, int w, int h)
{
    hal_screens_probe();
    hal_obj_parity_probe();
    /* SM64DS_SUB_SCALE is a divisor: 1 = full DS size (a quarter of the 2x
       window, Tango's "super in the way"), 2 = half size (1/16 of the
       window, the default), up to 4. */
    {
        static int init;
        if (!init) {
            init = 1;
            const int v = env_flag("SM64DS_SUB_SCALE", 2);
            g_div = v < 1 ? 1 : (v > 4 ? 4 : v);
            /* the improved map's sprite veto, installed once. It is installed
               whatever the option says, because it also LABELS the four
               entries for the census; it only declines them when the option
               is on (cam_button_veto's own return). */
            ntr::ppu_sub_set_obj_veto(cam_button_veto);
        }
    }
    hal_sub_panel_geometry(w, h);
    /* THE COMPOSE'S OWN ANSWER AND ITS RECTANGLES, once a frame, beside the
       panel's and by the same rule: one place decides, everybody else reads.
       They are computed whatever the answer is, so poll_touch's inverse has
       this frame's numbers on the very frame the plates come on. */
    {
        static int swap_frame;
        swap_geom_for(w, h, &g_sw);
        g_menu_up = save_menu_is_up();
        g_swap = swap_now();
        swap_trace(w, h, swap_frame++);
    }
    /* the inset arm of SM64DS_LAYOUT_SELFTEST, once per process and only in a
       course, which is the only place this map exists */
    {
        static int done;
        if (!done && !hal_sub_screen_stacked()) {
            done = 1;
            inset_map_selftest(w, h);
            swap_layout_selftest();
        }
    }
    /* THE TOUCH TARGET, not presented with the option on. Measured on this
       build: the marker is BG2 of the sub engine, which the game enables only
       while the bottom screen registers a touch (DISPCNT_B 0x40011803 ->
       0x40011c03, bit 10), and the port draws it in the map's TOP-LEFT CORNER
       whatever point was actually touched -- a click at the middle of the map
       moves 1260 pixels and every one of them is inside a quarter disc of
       radius 40 at DS (0,0). A marker that does not follow the touch is worse
       than no marker, and a player clicking with a mouse needs no marker at
       all, so with the improved map on this layer is not presented.
       ONLY IN THE CORNER-INSET LAYOUT, which is a course: menus and minigames
       take the stacked layout, where the bottom screen is the thing being
       played on and BG2 is nobody's marker. Zero otherwise, which is the
       raster's own old behaviour. */
    ntr::ppu_sub_set_bg_suppress(
        (improved_map_on() && !hal_sub_screen_stacked()) ? (1u << 10) : 0u);
    /* THE MAP'S BLACK OUTLINE, and where it comes from. It is not the
       cartridge's: the DS bottom screen's own outermost rows and columns are
       whatever the game drew there. It is this port's, one pixel wide, drawn
       by ntr::ppu_compose_sub immediately outside the map so a bare inset
       reads as a panel rather than as a hole in the 3D picture. With a
       decorated panel round the map that line sits on top of the artist's own
       border, which is precisely what the owner asked to be rid of, so with
       the improved map on it is not drawn and the panel's own frame takes its
       place. Off, and in the stacked layout, it is exactly what it was. */
    ntr::ppu_sub_set_compose_border(
        (improved_map_on() && !hal_sub_screen_stacked()) ? 0 : 1);
    /* Publish the layer mask, the way nine ROM functions do with this exact
       line. Minimap::Behavior and Message::UpdateWindow both write
       data_0209d454 and then push it themselves; doing it once more here is
       what covers the frames where neither of them ran.
       SM64DS_SUB_LAYERS forces the mask, which is how a layer the game has
       switched off gets looked at without pretending it is on. */
    /* THE TAIL IS CONDITIONAL NOW, and the condition is the ROM's own.
       func_02019144 asks the current graphics block's slot 2 first and RETURNS
       before any of this when it answers 0. hal/message_compositor.cpp runs
       that beat at the head of the engine A display path; this reads the
       answer rather than asking again, because the block's slot 2 is a
       once-per-frame callback and calling it twice would run a scene's whole
       display sync twice. Both frame loops (hal/scene_boot.cpp's
       port_scene_run and walk_window's own) call the engine A compositor
       immediately before this function, so the answer is this frame's.
       Measured by run link60 Stage 5 lane TS1: with the tail unconditional the
       port published a zero mask over the title screen's own layer enables
       every frame and OAM::Load re-parked the sprites the scene had just
       uploaded. port/ov007_seat.txt 5h. */
    const int run_tail = port_graph_block_verdict();
    if (run_tail) {
        static int forced = -2;
        if (forced == -2) forced = env_flag("SM64DS_SUB_LAYERS", -1);
        const unsigned mask =
            forced >= 0 ? (unsigned)forced : (unsigned)data_0209d454;
        *(volatile unsigned *)0x04001000 =
            (*(volatile unsigned *)0x04001000 & ~0x1f00u) | (mask << 8);
    }
    /* func_02019144's FIRST beat, the current block's slot 2, is
       hal/scene_boot.cpp's port_graph_block_beat for every block now, the
       Stage's included: its table is seated and registered
       (hal/arm9_tables_link100.cpp, run linkfull lane SEATS3), so
       Stage::GraphCallback2 -- the minimap's BG3 affine -- runs there, once a
       frame, and the hand copy of it this branch used to call is retired. The
       rest of func_02019144 is the layer-mask publish above and the OAM upload
       below. */
    if (run_tail) {
        /* IMMEDIATELY BEFORE THE UPLOAD, and that placement is the whole point.
           port_message_composite_engine_a ran a few lines earlier in both frame
           loops and rasterised engine A's sprites out of 0x07000000 as it
           stands RIGHT NOW -- the previous frame's upload. The line below
           replaces that with this frame's, so from here on the top screen in
           the framebuffer and engine A's OAM are one frame apart. The gapless
           seam pass has to know which of the two the picture was drawn from
           before it may complete an object across the seam, so it is handed the
           answer here rather than left to infer it from a compose count. See
           ntr/ppu.h's ppu_seam_oam_mark and the note in ntr/ppu_sub.cpp.
           INSIDE the run_tail branch because that is what makes it true: with
           no upload the OAM does not change, and a mark taken anyway would
           advance a snapshot the picture had not moved past. */
        /* THE UPLOAD MOVED BELOW THE SCANOUT, and that one reorder is the
           whole cure for the split-sprite stutter the owner filmed: the top
           screen's rasters run before this function and read the OAM the
           PREVIOUS Load uploaded, while the bottom screen used to scan out
           after THIS frame's Load -- one frame apart, so a sprite straddling
           the gapless seam ticked on alternate halves. Both banks upload in
           one Load, so scanning the bottom first puts both screens on the
           same upload, every scene, with no source-swapping and no special
           case. The seam mark stays glued to the Load it describes. */

        /* func_02019144's EIGHT BG-OFFSET PUBLISHES, the last of its beats the
           port was still skipping. SetBgNOffset (src) writes SHADOW words, and
           on the DS this once-per-frame sync is the only thing that carries
           them into the hardware registers. hal/message_pump.cpp copies the
           engine A BG3 line while a message box is up; nothing copied any of
           them the rest of the time, so a scene that scrolls through the ROM's
           own setters scrolled a register that never moved. Scene 368 is the
           measured case: dScMgPachinko_c's pan calls SetBg2Offset every frame,
           the hardware BG2 offset stayed at the one raw init write forever,
           and the Bob-omb Squad airship hull (map rows 0..3, loaded and
           stamped correctly) sat above a window that never scrolled up to it
           -- reported as "the screen is just not showing the top".
           Register order and the BG0-A 3D branch are func_02019144's own,
           lines 46..61, and it runs under the same run_tail verdict the ROM
           gates it with. */
        *(volatile unsigned *)0x04000000 =
            (*(volatile unsigned *)0x04000000 & ~0x1f00u)
            | ((unsigned)data_0209d45c << 8);
        if (!(*(volatile unsigned *)0x04000000 & 8)) {
            *(volatile unsigned *)0x04000010 =
                (data_0209d468 & 0x1ff) | (0x1ff0000u & (data_0209d46c << 16));
        }
        /* The else branch is func_02055454(data_0209d468): with BG0 as the 3D
           plane the ROM routes the X shadow to the 3D scroll word instead of
           the register. That TU is not in this link and the port's 3D path
           has never read the word it writes, so the branch is left unseated
           rather than half-seated; a level frame behaves exactly as before
           this publish existed. */
        *(volatile unsigned *)0x04000014 =
            (data_0209d4a4 & 0x1ff) | (0x1ff0000u & (data_0209d4a0 << 16));
        *(volatile unsigned *)0x04000018 =
            (data_0209d49c & 0x1ff) | (0x1ff0000u & (data_0209d478 << 16));
        *(volatile unsigned *)0x0400001c =
            (data_0209d48c & 0x1ff) | (0x1ff0000u & (data_0209d490 << 16));
        *(volatile unsigned *)0x04001010 =
            (data_0209d494 & 0x1ff) | (0x1ff0000u & (data_0209d498 << 16));
        *(volatile unsigned *)0x04001014 =
            (data_0209d484 & 0x1ff) | (0x1ff0000u & (data_0209d480 << 16));
        *(volatile unsigned *)0x04001018 =
            (data_0209d47c & 0x1ff) | (0x1ff0000u & (data_0209d470 << 16));
        *(volatile unsigned *)0x0400101c =
            (data_0209d474 & 0x1ff) | (0x1ff0000u & (data_0209d488 << 16));
    }
    if (run_tail) {
        /* nothing scanned when the panel is off, but the upload and its mark
           still have to happen -- they are the frame's, not the panel's */
    }
    /* ---- THE DISPLAY CAPTURE UNIT, in the hardware's own order --------------
     *
     * PUBLISH FIRST, then capture, then rasterise engine B. That is the order
     * the DS runs them in and each step depends on the one before it: the VRAM
     * bank mapping is programmed in VBlank (which for this program is
     * func_02019144's slot-24 beat, several statements ago), the capture happens
     * during the frame that mapping applies to, and the scan-out below reads
     * whatever the mapping made visible.
     *
     * `dst` is engine A's FINISHED framebuffer -- gx_render and
     * port_message_composite_engine_a both ran on it before this function was
     * called, in both frame loops -- and it is the composed graphics screen
     * DISPCAPCNT's source A names. Read, never written: the capture writes VRAM.
     *
     * BOTH ARE INERT UNTIL A GAME PROGRAMS THE UNIT. ppu_display_capture returns
     * on its first test when DISPCAPCNT's enable bit is clear, and
     * ppu_vram_publish skips every block that has never been captured into,
     * which is all four in every scene that never writes 0x04000064. */
    ntr::ppu_vram_publish();
    ntr::ppu_display_capture(dst, w, h);

    if (g_on) ntr::ppu_scanout_sub(g_sub);
    /* THE LATE UPLOAD, and it is now the OPT-IN arm. port_frame_oam_upload
       above already ran this frame's OAM::Load at func_02019144's own line,
       ahead of both rasters; this block is what SM64DS_OAM_LOAD_LATE=1 gets
       back, unchanged, so the two frame orders are one binary apart. */
    if (run_tail && oam_load_late()) {
        ntr::ppu_seam_oam_mark();
        _ZN3OAM4LoadEv();
    }
    if (!g_on) return;
    /* THE STACKED LAYOUT WRITES NOTHING INTO dst HERE, and that is the whole
       trick. dst is the framebuffer, the scan-out above is everything the
       bottom screen needs, and the stacked image is built downstream by
       hal_sub_screen_compose_stacked once the caller has finished with the
       framebuffer -- after its fade composite and after its debug overlay. So
       the framebuffer this function is handed comes out of a stacked frame
       byte-for-byte identical to a panel-off frame, which is what keeps every
       ppu_write_bmp site in the tree at 512x384 and unmoved. */
    if (!hal_sub_screen_stacked() && g_swap) {
        /* THE PICTURE IS COMPOSED OUT OF BOTH SCREENS THIS FRAME. Nothing of
           the map panel is drawn: the picture is two bands of the top screen,
           the three plates, and the game's own picture under them, and the
           plate, the banner, the handle and the attention arrows are none of
           those. */
        swap_present(dst, w, h);
    } else if (!hal_sub_screen_stacked()) {
        /* THE DECORATIVE PANEL FIRST, because it goes BEHIND the map: the
           compose below writes the map over the middle of it and its own one
           pixel frame lands on the panel's inner edge. Only with the option
           on; with it off not a pixel of this runs. */
        if (improved_map_on()) hal_sub_panel_decor(dst, w, h);
        ntr::ppu_compose_sub(g_sub, dst, w, h, g_x0, g_y0, g_pan_num, g_pan_den);
        /* AND THE PANEL'S SECOND PASS, the MAP plaque, which the owner asked
           to sit ABOVE the map rather than behind it. Three layers in his own
           order: the plate, the map, the banner. */
        if (improved_map_on()) hal_sub_panel_plaque(dst, w, h);
        /* AND THE RESIZE HANDLE, above the map and the banner both, because it
           is a control and a control that something can be drawn over is a
           control a player cannot find. */
        if (improved_map_on()) hal_sub_panel_handle(dst, w, h);
        /* AFTER THE PANEL, DELIBERATELY. The arrows sit just above the panel's
           top edge, so at every size but the largest they do not touch it; at
           the largest size on a 4:3 picture the panel IS the picture and the
           band has nowhere above to go, and drawing them here means the player
           still sees the cue instead of losing it under the map. Engine A's
           raster ran earlier in the frame (port_message_composite_engine_a),
           which is why the sprites are handed over rather than drawn there. */
        port_bounce_arrows_present(dst, w, h, g_x0, g_y0, g_pan_w, g_pan_h);
    }
    g_ready = true;

    /* SM64DS_BG2_TRACE=1: engine B's BG2 control and affine registers, once a
       frame and only when one of them has changed. The touch marker the owner
       calls "the target" is that layer (lane MINIMAP1 measured it: the sub
       engine's 128 sprite entries are byte-identical touched and untouched,
       and DISPCNT_B's layer mask goes 0x18 to 0x1c on touch), and it is drawn
       in the map's top-left corner wherever the touch actually was. An affine
       layer is PLACED by its reference point, so this prints what the game
       writes there against where the touch is: if the reference point moves
       with the stylus the port is misreading it, and if it does not the
       cartridge places the marker some other way. */
    {
        static int on = -1;
        static unsigned last[6];
        if (on < 0) on = std::getenv("SM64DS_BG2_TRACE") ? 1 : 0;
        if (on) {
            const unsigned cnt = *(volatile unsigned short *)0x0400100c;
            /* the map's own occupancy, because a text background can also be
               placed by REDRAWING it: the box of non-empty entries says which
               it is. Screen base is BGxCNT bits 8..12 in 2K units. */
            const unsigned screen = 0x06200000u + (((cnt & 0x1f00u) >> 8) << 11);
            unsigned bx0 = 64, bx1 = 0, by0 = 64, by1 = 0, nz = 0;
            for (unsigned ty = 0; ty < 32; ++ty)
                for (unsigned tx = 0; tx < 64; ++tx) {
                    const unsigned short e = *(volatile unsigned short *)
                        (screen + (ty * 64u + tx) * 2u);
                    if (!(e & 0x3FFu)) continue;
                    ++nz;
                    if (tx < bx0) bx0 = tx;
                    if (tx > bx1) bx1 = tx;
                    if (ty < by0) by0 = ty;
                    if (ty > by1) by1 = ty;
                }
            const unsigned now[6] = {
                *(volatile unsigned *)0x04001000,
                cnt,
                (unsigned)(*(volatile unsigned short *)0x04001018)
                    | (unsigned)(*(volatile unsigned short *)0x0400101a) << 16,
                *(volatile unsigned *)0x04001028,
                *(volatile unsigned *)0x0400102c,
                (bx0 << 24) | (bx1 << 16) | (by0 << 8) | by1,
            };
            if (std::memcmp(now, last, sizeof now) != 0) {
                std::memcpy(last, now, sizeof now);
                std::fprintf(stderr, "[bg2b] DISPCNT %08x BG2CNT %04x "
                             "HOFS %u VOFS %u BG2X %08x BG2Y %08x  map@%08x "
                             "%u entries, tiles x %u..%u y %u..%u\n",
                             now[0], now[1], now[2] & 0xFFFFu, now[2] >> 16,
                             now[3], now[4], screen, nz, bx0, bx1, by0, by1);
            }
        }
    }

    /* SM64DS_SUB_DUMP=N: the bottom screen alone, at 256x192, on frame N. */
    {
        static int at = -2, frame;
        if (at == -2) at = env_flag("SM64DS_SUB_DUMP", -1);
        if (frame++ == at) {
            ntr::ppu_write_bmp_sub("sub_screen.bmp", g_sub);
            std::printf("[sub] wrote sub_screen.bmp at frame %d "
                        "(DISPCNT_B %08x, mask %02x, BG3CNT %04x)\n", at,
                        *(volatile unsigned *)0x04001000,
                        (unsigned)data_0209d454,
                        *(volatile unsigned short *)0x0400100e);
        }
    }
}

/* ---- the stacked layout ----------------------------------------------------
 *
 * THE MODE IS A PROPOSAL AND AN OVERRIDE. hal/scene_boot.cpp proposes, out of
 * the ROM's own IsMinigameActorID; SM64DS_DUAL_SCREEN overrides in either
 * direction. Nothing proposes on a level path, so a level is inset unless the
 * env says otherwise, which is Tango's directive read literally: the corner
 * panel is doing its job during a level and the complaint was about minigames.
 *
 * IT LATCHES ON FIRST READ. hal_sub_screen_stacked is called from the frame
 * loop, from the touch poll and from walk_window's window sizing, and a mode
 * that could change after the window has been created would leave a window
 * the wrong shape for the picture in it. The setter refuses once the answer
 * has been handed out, loudly, because a late set means the call order is
 * wrong somewhere and silently keeping the old answer would hide that.
 */
static int g_stacked_latched;

int hal_sub_screen_stacked(void)
{
    if (g_stacked < 0) {
        const int e = stacked_env();
        g_stacked = e >= 0 ? e : (g_stacked_default ? 1 : 0);
        /* Say it once, on the run that decided it, and say WHERE the answer
           came from. "The panel is in the corner" and "the panel is in the
           corner because nothing proposed otherwise" are different findings
           when a minigame comes up in the wrong layout. */
        std::printf("[sub] layout: %s (default %s, %s)\n",
                    g_stacked ? "STACKED, both DS screens full size"
                              : "corner inset panel",
                    g_stacked_default ? "stacked" : "inset",
                    e < 0 ? "SM64DS_DUAL_SCREEN unset"
                          : (e ? "SM64DS_DUAL_SCREEN=1 forces it on"
                               : "SM64DS_DUAL_SCREEN=0 forces it off"));
        std::fflush(stdout);
    }
    g_stacked_latched = 1;
    return g_stacked;
}

void hal_sub_screen_set_stacked(int on)
{
    if (g_stacked_latched) {
        std::fprintf(stderr, "  [sub] stacked layout already latched at %d; "
                     "the request for %d is IGNORED (it arrived after the "
                     "first reader)\n", g_stacked, on ? 1 : 0);
        return;
    }
    g_stacked_default = on ? 1 : 0;
}

/* ---- THE SCENE-BOUNDARY RE-LATCH (run vsdec, lane LAY) ---------------------
 *
 * THE LATCH ABOVE IS RIGHT AND IT IS NOT ENOUGH ANY MORE, and the reason is a
 * change in the boot rather than a defect in it.
 *
 * When the latch was written every crossing from one kind of scene to another
 * was a RELAUNCH -- tests/walk_window.cpp's port_menu_relaunch and its VS
 * sibling start a fresh process with a fresh environment -- so "the process
 * shows one kind of scene" was true by construction and a mode that could
 * change under a live window was a bug and nothing else. The boot-to-title
 * ruling added the first crossing that does NOT relaunch: hal/title_entry.cpp
 * carries the title into the adventure IN THIS PROCESS, deliberately, because a
 * child process would lose the save the player just picked. The title is a
 * two-screen scene and the adventure is not, and with only the setter above the
 * adventure inherited the title's stacked window and kept it for the whole
 * session. That is the report this exists to answer.
 *
 * SO THE MODE FOLLOWS THE LIVE SCENE, AND ONLY AT A SCENE BOUNDARY. This is
 * not a relaxation of the latch: hal_sub_screen_set_stacked still refuses a
 * late write, because a late write is still a call-order mistake. What this is
 * is a SECOND, NAMED DOOR, taken by exactly the code that knows the scene under
 * the window has just been replaced, and the caller is required to re-shape the
 * window to the answer -- which is why it returns whether the answer moved.
 *
 * THE ENV STILL DISPOSES. SM64DS_DUAL_SCREEN is read once for the run, above,
 * and a forced value survives every boundary: a player or a harness that pinned
 * the layout pinned it for the process. `on` is a PROPOSAL here for the same
 * reason it is one in the setter.
 *
 * NOTHING HAS BEEN HANDED OUT YET is not a change. Before the first reader the
 * mode is still open and this is just a proposal, so it returns 0 and there is
 * nothing for a caller to re-shape.
 */
int hal_sub_screen_relatch(int on)
{
    g_stacked_default = on ? 1 : 0;
    if (g_stacked < 0)
        return 0;                 /* still open; the first reader will decide */
    const int e = stacked_env();
    const int want = e >= 0 ? e : (on ? 1 : 0);
    if (want == g_stacked) {
        std::printf("[sub] layout: the scene changed and the answer did not "
                    "(%s, %s)\n",
                    g_stacked ? "STACKED" : "corner inset panel",
                    e < 0 ? "proposed" : "SM64DS_DUAL_SCREEN forces it");
        std::fflush(stdout);
        return 0;
    }
    std::printf("[sub] layout: the scene changed and so does the presentation: "
                "%s -> %s (proposal %s)\n",
                g_stacked ? "STACKED, both DS screens full size"
                          : "corner inset panel",
                want ? "STACKED, both DS screens full size"
                     : "corner inset panel",
                on ? "stacked" : "inset");
    std::fflush(stdout);
    g_stacked = want;
    return 1;
}

/* Forward declarations: layout_log and layout_selftest are called from
   hal_sub_screen_stacked_image just below, but their bodies are defined next
   to hal_present_set_rect / hal_present_fit further down, where
   client_to_src and the hal_present_* mappers they drive are already in
   scope. client_to_src itself is defined further down still. THESE MUST SIT
   HERE, past the file's one anonymous namespace (which closed above): a
   forward declaration inside that namespace is a DIFFERENT entity from the
   file-static definition below, and calling the file-static one where both
   are visible is an ambiguous overload, not a redeclaration. */
static void client_to_src(int cx, int cy, int *x, int *y, int *sw, int *sh);
static void layout_log(const char *why);
static void layout_selftest(void);

/* Build the stacked image and hand back a pointer to it. `top` is the FINISHED
 * framebuffer -- faded, overlaid, everything -- and the return is
 * ntr::STACK_W x ntr::STACK_H, or null when the mode is off or the bottom
 * screen has never been scanned out. This reads the framebuffer and does not
 * write it, which is what lets it run after the frame is final and leave it
 * final.
 *
 * THE BUFFER IS HEAP AND IT IS THIS FILE'S, allocated on the first stacked
 * frame and never freed, and both of those are deliberate.
 *
 * Heap rather than a static array because a static would be 1.5 MB of host
 * .bss at the 2x tier, in EVERY build, including every inset run and every
 * selftest. port/tools/battery.py's note on the selftest BMP is explicit that
 * host-global layout outside .dsstate perturbs the rendered frame even at an
 * equal section base, so a layout feature that nobody has switched on has no
 * business moving the host's globals around. Allocated lazily, an inset run
 * carries one null pointer.
 *
 * This file's rather than the caller's because there are two callers (the
 * window's frame loop and the scene runner) and a buffer each would be the
 * same 1.5 MB twice.
 *
 * THE RETURN IS NOT CONST, and it used to be. The buffer handed back is this
 * file's own and every frame's compose rewrites the whole of it, so the const
 * was never protecting anything from anybody; what it did do was stop the one
 * caller that legitimately has to WRITE on the finished image -- the host's own
 * debug overlays in tests/walk_window.cpp, which must land on a fixed PHYSICAL
 * half and therefore cannot be painted into a framebuffer whose half moves.
 * See hal_sub_screen_stacked_top_y below. Nothing in the GAME writes here.
 */
unsigned int *hal_sub_screen_stacked_image(const unsigned int *top)
{
    static unsigned int *px;
    static size_t cap;
    static int refused;
    static unsigned last_layout_gen = (unsigned)-1;
    if (!hal_sub_screen_stacked()) return 0;
    /* THE LAYOUT LOG AND SELFTEST fire once per scene, as soon as the mode is
       known to be stacked -- BEFORE the g_ready/top gate just below, which is
       about whether THIS FRAME'S picture is ready to compose, not about
       whether the LAYOUT (a property of the scene's G and the aspect, latched
       well before any frame renders) is known. hal_screen_layout_generation()
       is what actually changes when a minigame's InitResources latches a new
       G -- not every frame recomputes a different layout, so logging/
       asserting on every call would be a line per frame for no new
       information. */
    {
        const unsigned layout_gen = hal_screen_layout_generation();
        if (layout_gen != last_layout_gen) {
            last_layout_gen = layout_gen;
            layout_log("layout");
            layout_selftest();
        }
    }
    if (!g_ready || !top) return 0;
    const ntr::StackLayout &lay = *hal_screen_layout();
    /* SIZED FOR THE GAP THE LAYOUT ASKS FOR, and grown rather than sized once
       for the worst case. A minigame's G latches at its InitResources and does
       not move again, so this reallocates at most once per scene; sizing for
       GAP_DS_MAX up front would carry 400 KB nobody uses through every gapless
       run, and the note above is explicit that this allocation is lazy on
       purpose. Never shrinks: a scene with a smaller gap reuses the block. */
    const size_t need = (size_t)lay.w * (size_t)lay.h * sizeof *px;
    if (need > cap && !refused) {
        unsigned int *bigger = (unsigned int *)std::realloc(px, need);
        if (bigger) {
            px = bigger;
            cap = need;
        } else {
            refused = 1;
            std::fprintf(stderr, "  [sub] stacked layout: could not allocate "
                         "the %dx%d image; presenting the top screen alone\n",
                         lay.w, lay.h);
        }
    }
    if (!px || cap < need) return 0;
    /* THE SUB ENGINE'S OWN BRIGHTNESS, NOT THE MAIN ENGINE'S.
     *
     * `top` arrives having already been through walk_window's fade composite,
     * which reads engine A's BLDCNT/BLDY -- correct, because `top` IS engine
     * A's framebuffer. g_sub is engine B's, and the DS's colour-special-effects
     * unit is per engine, so engine B's half has to be composed with engine B's
     * registers. This line used to read engine A's for both.
     *
     * IT WAS INVISIBLE UNTIL THE TITLE. Every fade hal/fader_wipes.cpp drives
     * writes both engines the same values, so on every screen the port had
     * filmed the two answers were identical. The opening is the first screen
     * where they are not: ov007 puts BOTH engines at brightness-decrease EVY 16
     * and then fades only engine A back in, which is how the cartridge's upper
     * LCD is fully black while its lower one is lit. Reading engine A here left
     * the sub half unblackened, and since that same screen also clears POWCNT1
     * bit 15 (engine B drives the UPPER LCD there), the unblackened half was
     * the one the player is looking at. */
    int evy = 0, to_white = 0;
    if (!port_fader_blend_state_sub(&evy, &to_white)) evy = 0;
    ntr::ppu_compose_stacked(top, g_sub, px, lay.w, lay.h, evy, to_white, lay);
    return px;
}

/* The image's live size, for the consumers that need the number and not the
   pixels: walk_window's DIB header and window sizing, and the stacked BMP
   capture. Reads the same layout the compose above does, which is the whole
   point of there being one. */
void hal_sub_screen_stacked_size(int *w, int *h)
{
    const ntr::StackLayout &lay = *hal_screen_layout();
    if (w) *w = lay.w;
    if (h) *h = lay.h;
}

/* The image's HEADROOM in host rows, for the one consumer that has to name the
   parts of the image rather than just its size: the scene capture's own line in
   hal/scene_boot.cpp, which used to derive "the gap" by subtracting two screen
   heights from the image height and would now be reporting headroom plus gap
   under the gap's name. Reads the same layout everything else does. */
int hal_sub_screen_stacked_headroom(void)
{
    return hal_screen_layout()->head_h;
}

/* THE FIRST IMAGE ROW OF THE UPPER PHYSICAL SCREEN, for the host's overlays.
 *
 * ppu_compose_stacked names its two destinations top_y and bottom_y for the
 * PHYSICAL screens and picks which ENGINE goes in each off POWCNT1 bit 15, so
 * top_y is the upper LCD's first row whichever engine is feeding it that frame.
 * That is exactly the guarantee an overlay needs: the F3 stats, the F5 menu and
 * the save-state toast are HOST UI, not ROM content, and they belong on one
 * screen and one screen only. Painted into engine A's framebuffer instead they
 * inherit the ENGINE's affinity and follow it across the swap -- which is the
 * defect this exists to close (372/373/384/385 toggle the bit every frame, 377
 * clears it and leaves it clear).
 *
 * It is head_h today and it is read off the layout rather than spelled as one,
 * because a headroom is not the only thing that could ever sit above the upper
 * screen and a second reader of the same number is how the two fall out of
 * step. Pure: it reads the layout and nothing else. */
int hal_sub_screen_stacked_top_y(void)
{
    return hal_screen_layout()->top_y;
}

/* And whether the rows BETWEEN the halves are a hinge or the world's own rows,
   in DS rows, for the same one consumer and for the same reason. With the
   object shift on the band is world -obj_shift_ds..-1 with the top engine
   drawing into it, and a capture line that called that a gap would put a gap-on
   word on the one picture that has none.

   FORWARDED THROUGH THIS FILE rather than read from hal/screen_gap.cpp
   directly, which is the shape hal_sub_screen_stacked_generation already has
   and for a harder reason than tidiness: not every binary that links
   hal/scene_boot.cpp links the layout owner, and a direct call from there is an
   unresolved symbol in smoke_player. */
int hal_sub_screen_stacked_obj_shift(void)
{
    return hal_screen_layout()->obj_shift_ds;
}

/* A counter that steps whenever that size changes -- which is once, when a
   minigame's InitResources latches its G. walk_window watches it to know when
   to re-shape the DIB header and re-size the window; a consumer that reads the
   size every frame anyway can ignore it.
   Forwarded from hal/screen_gap.cpp through this file so that walk_window's
   declaration of it sits beside the other hal_sub_screen_* names and cannot
   pick up a different linkage from them. */
unsigned hal_sub_screen_stacked_generation(void)
{
    return hal_screen_layout_generation();
}

/* The bottom screen's camera buttons, through the game's own hit test.
 *
 * Stage::CheckCameraInput reads the stylus record, decides which of the two
 * on-screen arrows it is inside, and ORs the rotate bits into the Ctrl block
 * at data_0209f498 + 4 (held) and + 6 (pressed).
 *
 * THE SPLIT-SYMBOL BRIDGE. On the DS those two halfwords ARE data_0209f49c
 * and data_0209f49e -- 0x0209f498 + 4 and + 6 -- and the readers the port
 * uses (func_02009e70's `held & 0x4300`) name the split symbols. On the host
 * they are separate storage, which is why walk_window already copies fields
 * between them after Stage::CheckInput. So the camera buttons' contribution
 * is merged the same way, by OR: the pad word is already written, and a
 * stylus press adds to it rather than replacing it.
 *
 * The gate in front of all of it is the CAMERA's own +0x154 bit 0x1000. With
 * that clear the ROM draws no buttons and reads no touches, so if the panel
 * shows arrows and nothing rotates, this is the word to look at. */
void hal_sub_camera_input(void)
{
    const char *ctrl = (const char *)data_0209f498;
    _ZN5Stage16CheckCameraInputEv();
    *(unsigned short *)data_0209f49c |= *(const unsigned short *)(ctrl + 4);
    *(unsigned short *)data_0209f49e |= *(const unsigned short *)(ctrl + 6);

    /* SM64DS_TOUCH_PROBE: what a REAL reader made of the record this frame.
       Stage::CheckCameraInput is matched ROM code from src/, it reads the
       stylus through data_020a0de8[i].{touched,held,x,y}, and these two
       halfwords are its entire output -- so a rotate bit here is the whole
       write-to-read path proven end to end, not a probe reading its own
       write back. It runs after poll_touch in the same frame. */
    if (g_tp_n > 0 && touch_probe_at(g_tp_cur))
        std::fprintf(stderr, "[touch] f%d cam: Ctrl+4 held=%04x Ctrl+6 "
                     "pressed=%04x  f49c=%04x f49e=%04x  gate(+0x154)=%08x\n",
                     g_tp_cur, *(const unsigned short *)(ctrl + 4),
                     *(const unsigned short *)(ctrl + 6),
                     *(const unsigned short *)data_0209f49c,
                     *(const unsigned short *)data_0209f49e,
                     data_0209f318
                         ? *(const unsigned *)((const char *)data_0209f318 +
                                               0x154)
                         : 0u);

    static int said;
    if (!said++) {
        const char *cam = (const char *)data_0209f318;
        std::printf("[sub] camera buttons: Camera %p, +0x154 = %08x, live %s\n",
                    (const void *)cam,
                    cam ? *(const unsigned *)(cam + 0x154) : 0u,
                    cam && (*(const unsigned *)(cam + 0x154) & 0x1000) ? "yes"
                                                                      : "NO");
    }
}

int hal_sub_screen_on(void) { return g_on ? 1 : 0; }

/* THE PRESENT RECTANGLE, both directions.
 *
 * walk_window.cpp's present() computes where the framebuffer landed inside the
 * client area and calls the setter with it; every consumer that has to turn a
 * mouse position into a game position calls the mapper. Keeping the two in one
 * place is the point: the forward arithmetic changes if the fit ever does, and
 * a second copy of the inverse somewhere else is how a resize feature quietly
 * breaks stylus aim.
 *
 * The mapper RETURNS whether the point was inside the picture, and fills the
 * framebuffer point either way -- clamped to the nearest edge pixel when it
 * was not. Callers that care about the difference (a click in a letterbox bar
 * is not a stylus press) test the return; callers that only ever want a valid
 * framebuffer coordinate can ignore it and get the old clamped behaviour.
 *
 * THE SETTER TAKES THE SOURCE SIZE NOW, and it has to. In the stacked layout
 * the rectangle was filled from an image twice as tall as the framebuffer, so
 * "how many source pixels tall is that rectangle" stopped being a constant the
 * inverse could assume. Passing it is the honest fix; deriving it from the
 * mode inside here would put a second copy of the mode's geometry in the one
 * place the note above says must not have a second copy.
 *
 * The two mappers are the same arithmetic over different bands of the same
 * rectangle, which is why they share client_to_src below rather than being
 * spelled twice.
 */
void hal_present_set_rect(int x, int y, int w, int h, int src_w, int src_h)
{
    const int changed = x != g_pr_x || y != g_pr_y || w != g_pr_w || h != g_pr_h ||
                         src_w != g_pr_sw || src_h != g_pr_sh;
    g_pr_x = x;
    g_pr_y = y;
    g_pr_w = w;
    g_pr_h = h;
    g_pr_sw = src_w;
    g_pr_sh = src_h;
    /* THE LAYOUT LOG's why="present" call: only on an actual change, so a
       windowed run that presents every frame does not print one line per
       frame forever -- the six numbers here only move on a resize or a
       layout change, and hal_sub_screen_stacked_image's own why="layout"
       call already covers the layout-only case with no present() at all
       (the headless scene runner). */
    if (changed) layout_log("present");
}

/* THE FIT, the one copy of it. present() in tests/walk_window.cpp
   called this arithmetic inline; the layout selftest has to drive the
   SAME code or it is checking a restatement. The largest sw:sh
   rectangle inside cw x ch, compared as a cross product so the
   choice is exact rather than a rounded ratio, centred. */
void hal_present_fit(int cw, int ch, int sw, int sh,
                     int *x, int *y, int *w, int *h)
{
    int dw, dh;
    if (sw <= 0 || sh <= 0 || cw <= 0 || ch <= 0) {
        if (x) *x = 0; if (y) *y = 0; if (w) *w = 0; if (h) *h = 0;
        return;
    }
    if ((long long)cw * sh <= (long long)ch * sw) {
        dw = cw;
        dh = (int)(((long long)cw * sh) / sw);
    } else {
        dh = ch;
        dw = (int)(((long long)ch * sw) / sh);
    }
    if (dw < 1) dw = 1;
    if (dh < 1) dh = 1;
    if (w) *w = dw;
    if (h) *h = dh;
    if (x) *x = (cw - dw) / 2;
    if (y) *y = (ch - dh) / 2;
}

/* THE LAYOUT LOG: does nothing unless SM64DS_LAYOUT_LOG is set. One line per
   call otherwise, read by hal_screen_layout(), the present rectangle
   (g_pr_*) and the same touch-mapping arithmetic hal_present_client_to_sub
   below uses (through client_to_src, so the printed numbers cannot drift
   from what a real touch would resolve to). Called from hal_present_set_rect
   when the published rectangle changes (why="present") and from
   hal_sub_screen_stacked_image when the layout generation changes
   (why="layout"), so a headless scene run and a windowed run both produce
   it. */
static void layout_log(const char *why)
{
    const char *env = std::getenv("SM64DS_LAYOUT_LOG");
    if (!env || !*env) return;
    const ntr::StackLayout &lay = *hal_screen_layout();
    const int stacked = hal_sub_screen_stacked();
    int dummy_x, dummy_y, sw, sh;
    client_to_src(0, 0, &dummy_x, &dummy_y, &sw, &sh);
    const int px0 = (int)((long long)sw * lay.pan_x0 / (lay.w > 0 ? lay.w : 1));
    const int pw  = (int)((long long)sw * lay.pan_w  / (lay.w > 0 ? lay.w : 1));
    const int b_y0   = stacked ? (int)(((long long)sh * lay.bottom_y) /
                                       (lay.h > 0 ? lay.h : 1)) : 0;
    const int band_h = stacked ? (int)(((long long)sh * lay.pan_h) /
                                       (lay.h > 0 ? lay.h : 1)) : 0;
    std::fprintf(stderr,
        "[layout] %s scene=%s aspect_active=%dx%d scale=%d "
        "image=%dx%d head=%d topregion=(0,%d,%d,%d) "
        "toppic=(%d,%d,%d,%d) band=(%d,%d) "
        "botregion=(0,%d,%d,%d) "
        "botpic=(%d,%d,%d,%d) "
        "present=(%d,%d,%d,%d) src=%dx%d "
        "touch=(%d,%d,%d,%d)\n",
        why, stacked ? "stacked" : "inset", ntr::active_w, ntr::active_h, lay.scale,
        lay.w, lay.h, lay.head_h, lay.top_y, lay.w, lay.pan_h,
        lay.pan_x0, lay.top_y, lay.pan_w, lay.pan_h, lay.band_y, lay.band_h,
        lay.bottom_y, lay.w, lay.pan_h,
        lay.pan_x0, lay.bottom_y, lay.pan_w, lay.pan_h,
        g_pr_x, g_pr_y, g_pr_w, g_pr_h, g_pr_sw, g_pr_sh,
        px0, b_y0, pw, band_h);
}

/* THE LAYOUT SELFTEST: does nothing unless SM64DS_LAYOUT_SELFTEST is set (to
   "1" for the default list of client sizes plus the image's own, or to a
   comma-separated cw x ch list). Runs from the same place layout_log's
   why="layout" call runs, so it fires once per scene at whatever scene and
   aspect the run is at. Asserts invariants A..F for each client size and
   prints one line per size plus a final summary. Drives hal_present_fit and
   the two hal_present_client_to_* mappers -- the SAME code a real present()
   and a real click drive -- rather than restating their arithmetic, so a bug
   in either mapper is caught here too. RESTORES the present rectangle it
   found on entry before returning, so a windowed run is not disturbed. */
static void layout_selftest(void)
{
    const char *env = std::getenv("SM64DS_LAYOUT_SELFTEST");
    if (!env || !*env) return;
    const ntr::StackLayout &lay = *hal_screen_layout();

    struct WH { int w, h; };
    WH sizes[8];
    int n = 0;
    if (std::strcmp(env, "1") == 0) {
        static const WH defaults[3] = { {1027, 578}, {1920, 1080}, {3440, 1440} };
        for (int i = 0; i < 3 && n < 7; ++i) sizes[n++] = defaults[i];
    } else {
        const char *p = env;
        while (*p && n < 7) {
            int w = 0, h = 0;
            while (*p >= '0' && *p <= '9') { w = w * 10 + (*p - '0'); ++p; }
            if (*p == 'x' || *p == 'X') ++p;
            while (*p >= '0' && *p <= '9') { h = h * 10 + (*p - '0'); ++p; }
            if (w > 0 && h > 0) sizes[n++] = WH{ w, h };
            if (*p == ',') ++p; else break;
        }
    }
    if (n < 8) { sizes[n].w = lay.w; sizes[n].h = lay.h; ++n; }

    /* save the six published numbers so this run does not disturb a real
       present(). */
    const int save_x = g_pr_x, save_y = g_pr_y, save_w = g_pr_w, save_h = g_pr_h,
              save_sw = g_pr_sw, save_sh = g_pr_sh;

    int passed = 0;
    for (int i = 0; i < n; ++i) {
        const int cw = sizes[i].w, ch = sizes[i].h;
        char fails[8] = {0};
        int nfail = 0;

        /* A: no region beyond the pictures */
        if (lay.h != lay.head_h + lay.pan_h * 2 + lay.band_h)
            fails[nfail++] = 'A';
        /* B: the pictures are adjacent */
        if (lay.band_y != lay.top_y + lay.pan_h ||
            lay.bottom_y != lay.band_y + lay.band_h)
            fails[nfail++] = 'B';
        /* C: both pictures are 4:3 */
        if (lay.pan_w * ntr::SUB_H != lay.pan_h * ntr::SUB_W)
            fails[nfail++] = 'C';
        /* D: centred, inside the image */
        if (lay.pan_x0 < 0 || lay.pan_x0 + lay.pan_w > lay.w)
            fails[nfail++] = 'D';

        int rx, ry, rw, rh;
        hal_present_fit(cw, ch, lay.w, lay.h, &rx, &ry, &rw, &rh);
        hal_present_set_rect(rx, ry, rw, rh, lay.w, lay.h);

        /* the client-space corners of the drawn bottom picture. xout/yout is
           a step guaranteed to cross at least one source pixel outward (the
           scale plus slop), for the outside checks below. */
        const long long bx0 = rx + (long long)lay.pan_x0 * rw / lay.w;
        const long long bx1 = rx + (long long)(lay.pan_x0 + lay.pan_w) * rw / lay.w - 1;
        const long long by0 = ry + (long long)lay.bottom_y * rh / lay.h;
        const long long by1 = ry + (long long)(lay.bottom_y + lay.pan_h) * rh / lay.h - 1;
        const long long xout = rw / lay.w + 2;
        const long long yout = rh / lay.h + 2;

        /* E: the four corners of the drawn bottom picture land within ONE DS
           pixel of DS (0,0), (SUB_W-1,0), (0,SUB_H-1) and (SUB_W-1,SUB_H-1);
           one pixel outside each of the four edges is OUTSIDE.

           WIDE4V, run link100 (status/WIDE4V.md): the check used to step
           picw/3 client pixels in from each corner and allow SUB_W/3 (85) DS
           pixels of slack -- the SAME fraction on both sides of the divide,
           so it carried NO margin at all. Reproduced 10/10 byte-identical at
           1027x578 against every WIDE aspect (native passed): the
           near-corner step landed at DS x 169 where the check required 170,
           a miss of exactly one DS pixel from a tolerance that was never
           wider than the rounding it was meant to absorb. It was never a
           race, an uninitialised value or a stale published rect --
           SM64DS_LAYOUT_SELFTEST is deterministic given a fixed aspect and
           client size, and ten repeats and a five-aspect sweep both proved
           it (same FAIL, same numbers, every time).

           THIS TESTS THE LITERAL CORNER instead, at the ONE DS pixel bound
           the brief accepts for a non-integer present scale, and does not
           require hal_present_client_to_sub's own "inside" answer AT THAT
           EXACT PIXEL: a floor-based forward fit and a floor-based inverse
           do not always agree on which side of a boundary sample falls, so
           the literal corner pixel of a picture can come back "outside" on
           the low (0) edge of either axis even though the DS pixel it
           clamps to is exactly right. Measured at 1027x578/32:9: the top-
           left corner's y reads "outside" and clamps to DS y 0 anyway; the
           very next client row already reads "inside" y 0. That is the
           fit's own unavoidable one-pixel seam -- present before this card
           wherever a window was not an integer multiple of the image, since
           the horizontal arm (already correct) has the same floor divide --
           not a defect for this test to fail on. The outside checks below
           stay pixel-tight: a point stepped out by at least two source
           pixels' worth of client slop has no such seam to explain a false
           negative. */
        {
            int dsx, dsy, ok = 1;
            hal_present_client_to_sub((int)bx0, (int)by0, &dsx, &dsy);
            if (dsx > 1 || dsy > 1) ok = 0;
            hal_present_client_to_sub((int)bx1, (int)by0, &dsx, &dsy);
            if (dsx < ntr::SUB_W - 2 || dsy > 1) ok = 0;
            hal_present_client_to_sub((int)bx0, (int)by1, &dsx, &dsy);
            if (dsx > 1 || dsy < ntr::SUB_H - 2) ok = 0;
            hal_present_client_to_sub((int)bx1, (int)by1, &dsx, &dsy);
            if (dsx < ntr::SUB_W - 2 || dsy < ntr::SUB_H - 2) ok = 0;
            if (hal_present_client_to_sub((int)(bx0 - xout), (int)((by0 + by1) / 2), &dsx, &dsy)) ok = 0;
            if (hal_present_client_to_sub((int)(bx1 + xout), (int)((by0 + by1) / 2), &dsx, &dsy)) ok = 0;
            if (hal_present_client_to_sub((int)((bx0 + bx1) / 2), (int)(by0 - yout), &dsx, &dsy)) ok = 0;
            if (hal_present_client_to_sub((int)((bx0 + bx1) / 2), (int)(by1 + yout), &dsx, &dsy)) ok = 0;
            if (!ok) fails[nfail++] = 'E';
        }
        /* F: the same four corners of the drawn TOP picture map to INSIDE
           through hal_present_client_to_fb, and one pixel below the top
           picture's last row maps OUTSIDE. */
        {
            const long long ty0 = ry + (long long)lay.top_y * rh / lay.h;
            const long long ty1 = ry + (long long)(lay.top_y + lay.pan_h) * rh / lay.h - 1;
            int fx, fy, ok = 1;
            if (!hal_present_client_to_fb((int)bx0, (int)ty0, &fx, &fy)) ok = 0;
            if (!hal_present_client_to_fb((int)bx1, (int)ty0, &fx, &fy)) ok = 0;
            if (!hal_present_client_to_fb((int)bx0, (int)ty1, &fx, &fy)) ok = 0;
            if (!hal_present_client_to_fb((int)bx1, (int)ty1, &fx, &fy)) ok = 0;
            if (hal_present_client_to_fb((int)bx0, (int)(ty1 + yout), &fx, &fy)) ok = 0;
            if (!ok) fails[nfail++] = 'F';
        }

        if (nfail == 0) {
            std::fprintf(stderr, "[layoutst] %dx%d A..F PASS\n", cw, ch);
            ++passed;
        } else {
            fails[nfail] = 0;
            std::fprintf(stderr, "[layoutst] %dx%d FAIL %s\n", cw, ch, fails);
        }
    }
    std::fprintf(stderr, "LAYOUT SELFTEST: %s %d/%d\n",
                 passed == n ? "PASS" : "FAIL", passed, n);

    g_pr_x = save_x; g_pr_y = save_y; g_pr_w = save_w; g_pr_h = save_h;
    g_pr_sw = save_sw; g_pr_sh = save_sh;
}

/* Client pixels to SOURCE-IMAGE pixels: the inverse of present()'s fit. Does
   no clamping and no range test -- both mappers below want the raw answer
   first, and they disagree about what "inside" means. Returns the source size
   it worked against in *sw / *sh so the caller can split it into bands. */
static void client_to_src(int cx, int cy, int *x, int *y, int *sw, int *sh)
{
    /* An old caller of the four-argument setter, or a build whose present path
       has not run, leaves the source size at zero. The fallback is the size of
       the image this run would present, which is the framebuffer's in the
       inset layout and the LAYOUT's in the stacked one.

       IT USED TO BE THE FRAMEBUFFER'S IN BOTH, which was right while the
       stacked image did not exist and became a headless run reporting a source
       one screen tall for an image two screens tall. Nothing on a windowed
       path reads this branch -- present() publishes a real rectangle on the
       first frame -- so the only reader is the touch probe on a headless run,
       and a probe that cannot be believed on the path it was written for is a
       probe with no purpose. The fallback maps client to source by the zoom
       divide below either way; only the size the bands are cut out of
       changes. */
    const ntr::StackLayout &fb_lay = *hal_screen_layout();
    const int s_w = g_pr_sw > 0 ? g_pr_sw
                                : (hal_sub_screen_stacked() ? fb_lay.w
                                                            : ntr::SCREEN_W);
    const int s_h = g_pr_sh > 0 ? g_pr_sh
                                : (hal_sub_screen_stacked() ? fb_lay.h
                                                            : ntr::SCREEN_H);
    if (sw) *sw = s_w;
    if (sh) *sh = s_h;
    if (g_pr_w > 0 && g_pr_h > 0) {
        /* the inverse of the fit: shift by the letterbox origin, then scale
           the picture's client size back to the source image's */
        *x = (int)(((long long)(cx - g_pr_x) * s_w) / g_pr_w);
        *y = (int)(((long long)(cy - g_pr_y) * s_h) / g_pr_h);
        /* a negative client offset truncates toward zero, which would fold
           the first row of bar pixels onto row 0 of the picture. Push those
           back out so an inside/outside answer is exact at the seam. */
        if (cx < g_pr_x) *x = -1;
        if (cy < g_pr_y) *y = -1;
    } else if (hal_sub_screen_stacked()) {
        /* NOTHING HAS PRESENTED AND THE STACKED IMAGE CARRIES ITS OWN SCALE,
           so the divide below would halve a point that is already in the
           image's own pixels. Measured (run link100 lane TOUCH1): a scene 374
           window quiet-spawned minimized never presents at all -- the probe
           prints "NO PRESENT RECTANGLE was ever published ... src 0x0, layout
           stacked, image 512x832 ... bottom_y 448" after fifteen hundred
           frames -- and the halved point lands 400 rows above the bottom
           screen's band, so every click in that window reads as the TOP screen
           and nothing is ever published. Minimized-never-activated is the ONLY
           window shape allowed while Tango is present, so that is not a corner
           case, it is the shape every click-driven proof has to run in.

           1:1 IS THE IMAGE'S OWN MAPPING, not a guess: the stacked image is
           built at 512x832 and the window's client area is sized to follow it
           (hal/gap's "the stacked image grew 768 -> 832 rows; the window
           client area follows"), so with no fit to invert, client pixel IS
           image pixel. The zoom divide stays for the inset layout, where the
           source is one framebuffer and the zoom is the window's.

           IT CANNOT MOVE A RUN THAT PRESENTED. present() publishes a real
           rectangle on its first frame and that takes the branch above; this
           one is only reached before the first present or when there is none
           at all. */
        *x = cx;
        *y = cy;
    } else {
        /* nothing has presented yet: the fixed-zoom divide this was before */
        *x = cx / (g_zoom > 0 ? g_zoom : 1);
        *y = cy / (g_zoom > 0 ? g_zoom : 1);
    }
}

int hal_present_client_to_fb(int cx, int cy, int *fx, int *fy)
{
    int x, y, sw, sh;
    client_to_src(cx, cy, &x, &y, &sw, &sh);
    /* THIS FUNCTION STILL MEANS THE TOP SCREEN, in both layouts, and every one
       of its existing callers still gets exactly what it got before. In the
       inset layout the source IS the framebuffer and the test below is the
       test that was here. In the stacked layout the source is taller and the
       top screen is its upper band, so a click in the gap or in the lower half
       answers "not on the picture" -- which is the truth for a caller asking
       about the top screen. hal_present_client_to_sub is the lower band.

       THE TOP SCREEN'S BAND IS THE SAME ROWS WITH OR WITHOUT A GAP, because
       the gap is inserted BELOW it: top_y is 0 and the screen is SCREEN_H
       rows. So this test does not need the layout to be right, and it reads
       it anyway -- if a later change ever moves the top screen, the one place
       that decides where it is has to be the one place both mappers read. */
    /* WIDESCREEN (16:9) INPUT. When the run is wide this inverse maps a client
       point to a FRAMEBUFFER pixel, and the framebuffer is the 3D layer the
       widened Hor+ field fills edge to edge (ntr/gx.cpp); present()'s fit already
       preserves the active aspect, so a click on the world still lands on the
       world. The top HUD reanchor (hal/message_compositor.cpp) moves only the 2D
       overlay composited ON TOP of this framebuffer, and no caller of this
       inverse converts a framebuffer x back into DS-256 HUD space -- the stylus
       surface is the BOTTOM screen (unchanged 256x192), the top screen is
       display-only in a level. So the anchor has no input consumer to mirror
       here and this mapping is already correct (and the extent below is active_w/
       active_h, so a 4:3 run clamps to 512x384 exactly as before). If a future
       top-screen touch UI needs to hit a reanchored HUD element, the inverse of
       the message_compositor band split belongs here; flagged for Tango. */
    const ntr::StackLayout &lay = *hal_screen_layout();
    const int inside = x >= 0 && y >= lay.top_y && x < ntr::active_w &&
                       y < lay.top_y + (hal_sub_screen_stacked() ? lay.pan_h
                                                                 : ntr::active_h);
    /* AND THE FRAMEBUFFER ROW IS THE SOURCE ROW MINUS top_y, which was a no-op
       for as long as top_y was zero and is not one any more: the gapless
       headroom puts head_h rows of image ABOVE the top screen, so a click on
       the top screen's first row arrives here as source row head_h and the
       framebuffer row it means is 0. The paragraph above already said the one
       place that decides where the top screen is has to be the one place both
       mappers read; this is that promise being kept. With no headroom top_y is
       0 and the subtraction changes nothing.
       A CLICK IN THE HEADROOM MAPS TO NOTHING, the gap band's own rule and for
       the same reason: those rows are picture with no panel under them, the
       hardware has no touch there, and a clamp would publish a press on the top
       screen's first row for a click that was never on it. `y` goes negative,
       `inside` is already false, and the clamp below only ever fires for a
       caller that ignores the return. */
    y -= lay.top_y;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= ntr::active_w) x = ntr::active_w - 1;
    if (y >= ntr::active_h) y = ntr::active_h - 1;
    if (fx) *fx = x;
    if (fy) *fy = y;
    return inside;
}

/* THE TOUCH TRANSFORM FOR THE STACKED LAYOUT: a client point to a DS pixel on
 * the bottom screen. Returns 1 only when the point is genuinely on the bottom
 * half of the picture -- a letterbox bar is not, the top screen is not, AND
 * THE GAP BAND IS NOT.
 *
 * Three steps: undo the fit into source pixels, subtract the layout's
 * bottom_y to get into the bottom screen's own band, then scale that band down
 * to the DS's own 256x192. The scale is a ratio rather than a shift because
 * SCREEN_W/SCREEN_H is the tier's, not necessarily two.
 *
 * bottom_y RATHER THAN SCREEN_H, and that one word is the whole of what the
 * gap changes here. With a 48-row gap the bottom screen starts 96 host rows
 * lower than it used to, and a mapper that kept subtracting one screen height
 * would put every press 96 client pixels above where the player aimed -- worse
 * than an offset, because it would still return "inside" and nothing would say
 * so. The layout is read rather than recomputed for exactly that reason.
 *
 * A CLICK IN THE GAP MAPS TO NOTHING. It is not clamped to the nearest screen:
 * there is no DS pixel under the hinge, the hardware has no touch there, and a
 * clamp would publish a stylus press at the top row of the bottom screen for a
 * click that was never on it. The band is between bottom_y and the top
 * screen's last row, `by` is negative there, and the test below refuses it --
 * the same answer a letterbox bar gets, which is the honest one.
 *
 * WHY IT IS EXACT AT THE 2x TIER: the bottom band is 512x384 and the DS screen
 * is 256x192, so x maps by (x * 256) / 512 = x / 2 and y the same. Client
 * point -> band pixel -> DS pixel with two integer divides and no rounding
 * term that can drift a row at the seam. The clamp below only ever fires for a
 * caller that ignores the return.
 */
int hal_present_client_to_sub(int cx, int cy, int *dsx, int *dsy)
{
    int x, y, sw, sh;
    client_to_src(cx, cy, &x, &y, &sw, &sh);
    /* the bottom screen's band of the source image. In the inset layout the
       source is one screen tall, sh - bottom_y is zero, and this correctly
       answers "outside" for every point -- in that layout the panel is the
       bottom screen and poll_touch takes the other branch. */
    const ntr::StackLayout &lay = *hal_screen_layout();
    /* THE STYLUS SURFACE IS THE DRAWN PANEL, to the pixel, on both axes.
       It used to be the REGION on the vertical axis -- sh - bottom_y --
       which is the panel only when active_h is a whole multiple of 192.
       At 32:9 the region is 288 rows and the panel 192, so DS 0..191 was
       spread over 288 client rows and every press read two thirds of the
       way up the button it landed on. pan_h is scaled into this mapper's
       own source height exactly the way pan_x0 / pan_w are scaled into its
       source width two lines below, so a present() that reports a
       different source size (the headless probe's fallback) still lands.
       THE INSET LAYOUT ANSWERS "OUTSIDE" FOR EVERY POINT, which is what
       sh - bottom_y used to say by arithmetic and what this now says by
       name: there is no stacked band there, the panel is the bottom
       screen and poll_touch takes the other branch. */
    const int stacked = hal_sub_screen_stacked();
    const int b_y0 = stacked ? (int)(((long long)sh * lay.bottom_y) /
                                     (lay.h > 0 ? lay.h : 1)) : 0;
    const int band_h = stacked ? (int)(((long long)sh * lay.pan_h) /
                                       (lay.h > 0 ? lay.h : 1)) : 0;
    const int by = y - b_y0;
    /* The bottom panel is PILLARBOXED when the run is 16:9 and full-width when it
       is 4:3, and this ONE mapping covers both: it reads lay.pan_x0/pan_w, which
       the layout sets to (0, full width) on a 4:3 run and to the centred band on
       a wide one. So with the toggle off px0 is 0, pw is the full width, and this
       is byte-for-byte the old square mapping; no #ifdef, it self-normalises. A
       click in a wide run's black margin is off the panel, exactly as a letterbox
       bar or the gap is. pan_x0/pan_w are in the layout's own width; scale them
       into this mapper's source width so a present() that reports a different
       source size (the headless probe's fallback) still lands. */
    const int px0 = (int)((long long)sw * lay.pan_x0 / (lay.w > 0 ? lay.w : 1));
    const int pw  = (int)((long long)sw * lay.pan_w  / (lay.w > 0 ? lay.w : 1));
    const int bx = x - px0;
    const int inside = band_h > 0 && bx >= 0 && bx < pw && by >= 0 && by < band_h;
    int dx = 0, dy = 0;
    if (band_h > 0) {
        dx = (int)(((long long)bx * ntr::SUB_W) / (pw > 0 ? pw : 1));
        dy = (int)(((long long)by * ntr::SUB_H) / band_h);
    }
    if (dx < 0) dx = 0;
    if (dy < 0) dy = 0;
    if (dx >= ntr::SUB_W) dx = ntr::SUB_W - 1;
    if (dy >= ntr::SUB_H) dy = ntr::SUB_H - 1;
    if (dsx) *dsx = dx;
    if (dsy) *dsy = dy;
    return inside;
}

/* SM64DS_TOUCH_CLIENT_PROBE="cx,cy[;cx,cy...]": run the client-to-DS transform
 * over the listed CLIENT points, once, on the first frame that polls, and print
 * what each one maps to. It exists because the stacked layout's transform
 * cannot otherwise be exercised deterministically: the scene runner is
 * headless by construction (walk_window hands over to port_scene_run before
 * the window is created), so there is no mouse on the only path a minigame
 * runs on today, and a transform nobody can run is a transform nobody can
 * check. This drives the SAME function poll_touch drives, against the SAME
 * present rectangle, so it is the real arithmetic and not a restatement of it.
 *
 * IT PRINTS TO STDERR, AND ON A SCENE RUN STDERR IS NOT ON YOUR TERMINAL.
 * walk_window's flight recorder takes stderr into playlog/play_*.log on every
 * path that is not a window selftest, which is exactly the headless path this
 * probe exists for, so a reproducer that greps its own console counts zero
 * lines and reads that as the probe not firing. Either add
 * SM64DS_NO_PLAYLOG=1, or read playlog/. A windowed SM64DS_WINDOW_SELFTEST run
 * is the one case where the recorder is off and stderr arrives directly.
 *
 * It reads no mouse and writes no stylus. It is a printer.
 */
void hal_touch_client_probe(void)
{
    static int done, waited;
    const char *s = std::getenv("SM64DS_TOUCH_CLIENT_PROBE");
    if (done || !s) return;
    /* WAIT FOR A PRESENT RECTANGLE THE FRAME LOOP PUBLISHED, and this cost the
       lane one wrong reading before it was written down. This poll runs at the
       TOP of a frame, and by the time the first one runs the window procedure
       has already answered a WM_PAINT from CreateWindow -- so a rectangle
       exists, and it is one the framebuffer filled, before the frame loop has
       ever built a stacked image. The probe fired on it and printed
       "layout stacked" beside "src 512x384", which is two true halves reading
       as one false whole.

       So: skip the first two polls, which puts this past the frame loop's own
       first present, and then require a rectangle. Past thirty polls with no
       rectangle at all the run genuinely has no window (the scene runner is
       that case) and the fixed-zoom fallback is the honest answer, said in
       those words rather than passed off as the windowed one. */
    if (++waited <= 2) return;
    if (g_pr_w <= 0 && waited < 30) return;
    done = 1;
    if (g_pr_w <= 0)
        std::fprintf(stderr, "[touchmap] NO PRESENT RECTANGLE was ever "
                     "published (headless run, no window). What follows is the "
                     "fixed-zoom fallback, not the windowed transform.\n");
    {
        const ntr::StackLayout &lay = *hal_screen_layout();
        std::fprintf(stderr, "[touchmap] rect x%d y%d w%d h%d src %dx%d, "
                     "layout %s, image %dx%d top_y %d band %d+%d bottom_y %d\n",
                     g_pr_x, g_pr_y, g_pr_w, g_pr_h, g_pr_sw, g_pr_sh,
                     hal_sub_screen_stacked() ? "stacked" : "inset",
                     lay.w, lay.h, lay.top_y, lay.band_y, lay.band_h,
                     lay.bottom_y);
    }
    while (*s) {
        const int cx = std::atoi(s);
        while (*s && *s != ',' && *s != ';') ++s;
        if (*s == ',') ++s;
        const int cy = std::atoi(s);
        while (*s && *s != ';') ++s;
        if (*s == ';') ++s;
        int fx = 0, fy = 0, dx = 0, dy = 0;
        const int on_top = hal_present_client_to_fb(cx, cy, &fx, &fy);
        const int on_sub = hal_present_client_to_sub(cx, cy, &dx, &dy);
        /* WHICH BAND OF THE IMAGE IT LANDED IN, said in the source's own rows.
           "out of both" is three different things -- a letterbox bar, the gap,
           and a point off the window entirely -- and only the middle one is a
           feature. Naming it is what makes a gap click PROVABLY nothing rather
           than merely unclaimed. */
        const ntr::StackLayout &lay = *hal_screen_layout();
        int sx = 0, sy = 0, ssw = 0, ssh = 0;
        client_to_src(cx, cy, &sx, &sy, &ssw, &ssh);
        const char *where = "bar";
        if (sx >= 0 && sx < ssw) {
            if (lay.head_h > 0 && sy >= 0 && sy < lay.top_y)
                where = "HEADROOM";
            else if (sy >= lay.top_y && sy < lay.top_y + lay.pan_h)
                where = "top screen";
            else if (lay.band_h > 0 && sy >= lay.band_y && sy < lay.bottom_y)
                where = "GAP";
            else if (sy >= lay.bottom_y && sy < ssh)
                where = "bottom screen";
        }
        std::fprintf(stderr, "[touchmap] client (%d,%d) -> src(%d,%d) %s | "
                     "top %s fb(%d,%d) | sub %s ds(%d,%d)\n", cx, cy, sx, sy,
                     where, on_top ? "IN " : "out", fx, fy,
                     on_sub ? "IN " : "out", dx, dy);
    }
    std::fflush(stderr);
}

// ---- the three leaves LoadGraphics2D names but never reaches ---------------
//
// Each of these sits on a branch the port does not take, and each drags a
// subsystem with no host seam behind it. They are stubbed by name rather than
// sliced in, and each says so if it is ever actually called -- which would
// mean the branch analysis is wrong, not that the stub is.

/* LoadFont3D is NOT faced here any more (VS wiring lane, run vs1). The
   "only from LoadGraphics2D(b != 0)" premise stopped holding when scene 6
   mounted: dScEntry_c::InitResources calls LoadFont3D DIRECTLY
   (src/_ZN10dScEntry_c13InitResourcesEv.cpp:101), and the face swallowed
   Message::LoadTextVS with the font upload, so the VS text plot read the
   null data_0209fcf8 -- measured as the c0000005 at func_020341a8+0x23 on
   the first scene-6 boot. Every dependency is hosted now (LoadFile,
   DecompressLZ16, MultiCopy_Int, func_02054d88, and the five per-language
   VS text banks through port/tools/romdata.py NAMED), so the matched
   src/LoadFont3D.c is the body, compiled by slice_vs. Leaving the stub here
   would be a duplicate definition -- the wave-C precedent above. */

/* Top-screen furniture: it rasterises the controller-mode caption into
   G2::GetBG2CharPtr through func_0201d590, the main engine's text path. The
   bottom screen never reads any of it.
   PORT_HOST_ABI: src writes the top-screen BG2 text layer, a subsystem the port
   does not host. */
void LoadControllerModeText(int a)
{
    static int said;
    if (!said++)
        std::printf("  [sub] LoadControllerModeText(%d): top-screen text is "
                    "not hosted\n", a);
}

/* The third leaf, func_ov004_020adc4c, is NOT stubbed here any more (run linkw
   wave C, lane cat-2d). It was tagged `src reads data_ov004_020beb60, and
   ov004 is not mounted`, and that one BSS word is now hosted in
   hal/oam2d_ov004_bss.cpp, so the matched src/func_ov004_020adc4c.c is the
   body LoadFont reaches. Leaving the stub here would be a duplicate
   definition. The branch that calls it is still not taken: it is LoadFont's
   data_0209d698 == 2 arm, the minigame framework's font, and the window loads
   font 0. ov004 is the minigame scene framework, not the VS mode this comment
   used to say; see port/ov004_ov007_2d_map.txt. */

/* Debug: the bottom screen on its own, at its own size. */
void hal_sub_screen_dump(const char *path)
{
    ntr::ppu_scanout_sub(g_sub);
    ntr::ppu_write_bmp_sub(path, g_sub);
}

/* THE SAME PICTURE WITHOUT RE-SCANNING, and the difference matters. The dump
   above rasterises engine B again before writing, which is right for a debug
   hook asked for out of nowhere and WRONG for a capture taken beside a stacked
   image: a second scan-out reads the registers as they stand now, and on a
   scene whose slot 24 rewrites DISPCNT_B every frame that is a different frame's
   configuration. This writes the buffer the frame actually presented.
   Returns 0 when no frame has been scanned out yet. */
int hal_sub_screen_write_bmp(const char *path)
{
    if (!g_ready) return 0;
    return ntr::ppu_write_bmp_sub(path, g_sub) ? 1 : 0;
}

/* What engine B is actually configured to do, printed once. The bottom screen
   failing silently reads identically whether the registers are wrong or the
   VRAM is empty, and this is the line that tells them apart. */
void hal_sub_screen_probe(void)
{
    const unsigned dispcnt = *(volatile unsigned *)0x04001000;
    std::printf("[sub] DISPCNT_B %08x: mode %u, dispmode %u, BG %c%c%c%c OBJ %c, "
                "extpal %c, win %c%c\n",
                dispcnt, dispcnt & 7, (dispcnt >> 16) & 3,
                (dispcnt >> 8) & 1 ? '0' : '-', (dispcnt >> 9) & 1 ? '1' : '-',
                (dispcnt >> 10) & 1 ? '2' : '-', (dispcnt >> 11) & 1 ? '3' : '-',
                (dispcnt >> 12) & 1 ? 'y' : '-', (dispcnt >> 30) & 1 ? 'y' : '-',
                (dispcnt >> 13) & 1 ? '0' : '-', (dispcnt >> 14) & 1 ? '1' : '-');
    for (int i = 0; i < 4; ++i) {
        const unsigned short cnt = *(volatile unsigned short *)(0x04001008 + i * 2);
        std::printf("[sub] BG%dCNT %04x: prio %u, char %08x, screen %08x, "
                    "%s, size %u\n", i, cnt, cnt & 3,
                    0x06200000u + (((cnt & 0x3c) >> 2) << 14),
                    0x06200000u + (((cnt & 0x1f00) >> 8) << 11),
                    (cnt >> 7) & 1 ? "8bpp" : "4bpp", (cnt >> 14) & 3);
    }
    /* How much of the bottom screen's VRAM is not zero -- the difference
       between "the loaders ran" and "the loaders ran and wrote nothing". */
    static const struct { const char *n; unsigned a, len; } regions[] = {
        {"BG VRAM  ", 0x06200000u, 0x20000u},
        {"OBJ VRAM ", 0x06600000u, 0x20000u},
        {"BG pltt  ", 0x05000400u, 0x200u},
        {"OBJ pltt ", 0x05000600u, 0x200u},
        {"BG extpal", 0x06898000u, 0x8000u},
        {"sub OAM  ", 0x07000400u, 0x400u},
    };
    for (unsigned r = 0; r < sizeof regions / sizeof *regions; ++r) {
        unsigned nz = 0;
        const volatile unsigned char *p =
            (const volatile unsigned char *)regions[r].a;
        for (unsigned i = 0; i < regions[r].len; ++i)
            if (p[i]) ++nz;
        std::printf("[sub] %s %08x: %u/%u bytes nonzero\n", regions[r].n,
                    regions[r].a, nz, regions[r].len);
    }

    /* THE OAM ENTRY CENSUS, in the same three buckets the Stage-3 audit used,
       so its numbers and these are the same measurement.
         all-zero  the engine was never uploaded to -- boot VRAM, untouched
         y == 192  OAM::Reset's parked value (0xc0), an entry deliberately off
         other y   a sprite the game actually placed
       An engine reading 128/0/0 has had nothing written to it AT ALL, which is
       a different fault from an engine that was uploaded a screen of parked
       entries, and only this split tells them apart. Both engines, because
       "the sub screen is empty" and "the sub sprites went to the main screen"
       produce the same sub picture and opposite main censuses. */
    for (int e = 0; e < 2; ++e) {
        const unsigned base = e ? 0x07000400u : 0x07000000u;
        unsigned zero = 0, parked = 0, real = 0;
        for (int i = 0; i < 128; ++i) {
            const volatile unsigned short *a =
                (const volatile unsigned short *)(base + i * 8);
            const unsigned short a0 = a[0], a1 = a[1], a2 = a[2];
            if (!a0 && !a1 && !a2) { ++zero; continue; }
            if ((a0 & 0xff) == 192) ++parked; else ++real;
        }
        std::printf("[sub] OAM %s %08x: %u all-zero, %u parked (y=192), "
                    "%u placed\n", e ? "engine B" : "engine A", base,
                    zero, parked, real);
    }
    /* The backdrop is engine B's BG palette entry 0 and nothing else -- see
       ntr/ppu_sub.cpp's `bgr555(rd16(kPlttBase))`. Printed because "the panel
       is one flat colour" and "the panel is one flat colour AND that is the
       colour the game asked for" are different findings. */
    {
        const unsigned short bd = *(volatile unsigned short *)0x05000400;
        std::printf("[sub] backdrop = sub BG pltt[0] = %04x (r%u g%u b%u)\n",
                    bd, bd & 0x1f, (bd >> 5) & 0x1f, (bd >> 10) & 0x1f);
    }
}

}  // extern "C"
