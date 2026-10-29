/* menu_recipe_title.cpp -- the TITLE family as one-screen menu recipe "title" (run menu1, lane TITLE1).
 *
 * WHAT THE TITLE IS. Scene 1 (dScDSMT_c, ov007). On this family the two engines are swapped
 * (POWCNT1 bit 15 clear): ENGINE A is the physical LOWER screen (wallpaper, the 3D Mario head, the
 * buttons, the file plates, all touchable) and ENGINE B is the physical UPPER screen (logo, castle
 * photo, the prompt banner). In the compose layer's words the "top picture" (SRC_TOP_PIC) is therefore
 * the touch screen and SRC_SUB is the picture screen. Neither can be lifted by layer (P0ATLAS 0.3), so
 * every piece is cut from a FINISHED picture along rows and columns that are empty.
 *
 * THE LAYOUT, per page (canvas = 192 units tall, 256 wide at 4:3; pieces never stretch):
 *   attract (variant 1)   the lower screen as it is (star, TOUCH TO START) fills the picture; a click
 *                         anywhere is the touch the ROM wants.
 *   main menu (2)         the upper screen (logo on sky) is the BACKDROP, full size. The button bar of the
 *                         lower screen (VS / ADVENTURE / REC ROOM, rows 134..191, wallpaper between the
 *                         plates) sits on the bottom edge over the upper screen's "TOUCH TO START" banner,
 *                         which means nothing on this page; the wrench plate moves to the right edge above
 *                         the bar. The Mario head is the one thing not kept (it is a decoration and a
 *                         hidden doodle button; there is no room for it beside the logo).
 *   file pages (3..6)    the lower screen whole (the page IS the DS lower screen); see THE FILE PAGES below (run menu1, FILE1).
 * Wide canvases keep the same arrangement centred; nothing is stretched.
 *
 * DRIVER. NATIVE. The ROM reads the pad on every title page (src/func_ov007_020b3190.c: any non-B key
 * shows its own cursor, the D-pad moves it through func_ov007_020b3708's table, a confirm key presses
 * the focused element). Only the key MEANING differs: the port's confirm is DS bit 0x0002 (jump), which
 * is the ROM's B; the ROM's confirm is A, 0x0001. menu_title_pad_map swaps them. Mouse clicks reach the
 * ROM as the stylus through the touch pieces (core H3); hover shows the port's focus outline.
 *
 * HOTSPOTS. The ROM's own element boxes through port_title_elem_boxes (hal/title_entry.cpp, the shared
 * decoder, arithmetic of src/func_ov007_020b46b0.c), with the dumped boxes as the fallback when the
 * scene global is not built (the layout selftest). Which ids a page shows is read off the pictures
 * (out/TITLE1/shots), not guessed from positions.
 */
#include <chrono>
#include "menu_compose.h"
#include "settings_page.h"
#include "menu_hostimg.h"
#include "menu_online.h"
#include "egg_audio.h"
#include "ntr/gx.h"
#include "ntr/ppu.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mutex>

extern "C" {
int port_title_elem_boxes(int *slot, int *id, int *x0, int *y0, int *x1, int *y1, int max);
int port_title_elem_live(int slot);      /* MENUS093E: the ROM would take a touch on that element */
int port_title_page_state(int *top, int *elem, int *req);
int port_scene_live_id(void);
void func_ov007_020bdeb0(int idx);
void func_ov007_020bde70(int idx, int arg);      /* INTROCUT1: stops the sequence started under that id, fading over arg ticks */
int hal_menu_touch_queue_press(int dsx, int dsy);
void *func_ov007_020b3f20(int idx);      /* ROUND5: the title's resource table lookup */
extern char data_ov007_02104bc8[];       /* ROUND5: the star objects' second animation pointers (8 byte slots, one per object) */
}

namespace {

enum { V_ATTRACT = 1, V_MAIN = 2, V_FILES = 3, V_OPTIONS = 4, V_DOODLE = 5, V_TABLE = 6, V_INTRO = 7, V_TOY = 8 };   /* V_TOY: the face press (top 4, elem 0), FACE1 */

/* the 24 element boxes as dumped on the shipped ROM (SM64DS_TITLE_ELEMS, identical at f299, f419, f650,
   f719): id, x0, y0, x1, y1 in touch-screen DS pixels. The live decoder wins when it answers. */
struct Box { int id, x0, y0, x1, y1; };
const Box kBoxes[24] = {
    { 0, 72, 148, 184, 188 },  { 1, 2, 148, 66, 188 },    { 2, 190, 148, 254, 188 }, { 3, 212, 148, 252, 188 },
    { 4, 212, 148, 252, 188 }, { 5, 88, 172, 168, 188 },  { 6, 4, 148, 44, 188 },    { 7, 216, 0, 256, 40 },
    { 8, 96, 148, 160, 180 },  { 9, 112, 25, 232, 57 },   { 10, 86, 25, 118, 57 },   { 11, 224, 25, 256, 57 },
    { 12, 112, 73, 168, 105 }, { 13, 176, 73, 232, 105 }, { 14, 8, 44, 80, 92 },     { 15, 92, 68, 164, 116 },
    { 16, 176, 44, 248, 92 },  { 17, 14, 96, 86, 132 },   { 18, 92, 96, 164, 132 },  { 19, 170, 96, 242, 132 },
    { 20, 180, 140, 252, 176 }, { 21, 28, 140, 156, 176 }, { 22, 54, 108, 126, 148 }, { 23, 130, 108, 202, 148 },
};

enum { CH_NONE = 0, CH_INTRO = 1, CH_RETURN = 2 };
enum { CH_LOGO_POP = 22, CH_BOUNCE_LEN = 40, CH_FACE_FADE = 24, CH_LOGO_OUT = 24, CH_LOGO_IN = 30, CH_PLANK_DUR = 28, CH_PLANK_GAP = 6,
       CH_PLANK_START = 4, CH_FLY = 14, CH_VEIL = 10,
       /* INTRO2 (Tango 10-24): the logo's trip to the top left takes 2 s (120 ticks) after the head has flown in; the planks rise during it.
          LOGOSPEED1: the trip has its own clock now (CH_LOGO_TRIP); this stays the swap's length on the Mods OFF path (the ROM's own logo),
          and with the PC art the swap ends when the planks are up (CH_SWAP_INTRO_PC) */
       CH_LOGO_MOVE = 120, CH_PLANK_START_INTRO = 40,
       /* LOGOSPEED1 (Tango 10-29: "the logo pans a little too slow", then "can the logo move up much faster?"): the big logo's trip from
          the head's arrival to its landing as the small logo, in ticks (Chore::head_t's clock). 40 ticks = 0.67 s, 3x faster than the 120
          it was. The one number to tune the trip by: the ease, the glide between ticks and the exact-overlap swap at landing all follow it.
          The ROM has no trip to match: with Mods OFF its own logo stays where it is. The page settles (the skip window closes, the menu
          takes a press as a choice instead of a skip) at the later of swap_end - 4 and the landing (intro_logo_busy). With the PC art the
          intro's swap now ends with the planks (CH_SWAP_INTRO_PC = CH_PLANKS_END_INTRO, 80) instead of at 120, so with the head arriving at
          sw_t 36 the logo lands at sw_t 76 and the page settles at sw_t 76, with the planks finishing their last 4 ticks, instead of sitting
          still 40 ticks before it takes a choice (at 120 / 120 both were sw_t 156). Mods OFF keeps 120. */
       CH_LOGO_TRIP = 40,
       /* LOGOFADE1: the longest the logo waits for the head's arrival after the swap began before its hand-over starts anyway */
       CH_HEAD_WAIT = 90,
       /* LOGOFADE1: the head's own animation frame at which its entrance (the drop in and the bounce back up) is over and the head is at rest:
          measured on the picture under the logos (SM64DS_LF1_NOLOGO): the drop t323-333, the bounce t334-345, the settle to t357, frame 34 at t358 */
       CH_HEAD_REST_FRAME = 34,
       CH_PLANKS_END = CH_PLANK_START + 2 * CH_PLANK_GAP + CH_PLANK_DUR, CH_PLANKS_END_INTRO = CH_PLANK_START_INTRO + 2 * CH_PLANK_GAP + CH_PLANK_DUR,
       CH_SWAP_INTRO_PC = CH_PLANKS_END_INTRO };
struct Chore {
    int mode;            /* CH_* */
    int in_fam;          /* the main family (V_INTRO / V_MAIN) is the page */
    int prev_variant;
    int ct;              /* frames since the ROM's iris sprite first showed (-1 = not yet) */
    int fam_t;           /* frames since the family was entered */
    int sw_t;            /* frames since the swap began (-1 = not yet) */
    int face_a;          /* 0..255 */
    int skip;            /* a confirm press asked for the resting state */
    int iris_covered;    /* the iris covered the picture on the last drawn frame */
    int ff2_done;
    int attract_t;       /* the ROM's attract counter as last read */
    int fly_t;           /* ASSET6: frames since the press, while the star flies off the top (state 10 before the iris) */
    int released;        /* INTRO2: a tick without a touch / click went by since the family was entered (the press that started the intro is over) */
    int swap_a;          /* TITLEUI1: 255 - the ROM's brightness fade level (0..255) while the face swap's fade runs on the main page, -1 = none (rom_swap_level) */
    int swap_lv;         /* TITLEUI1: that level as the ROM holds it (0..0x1000), for the trace */
    int leave_w;         /* GLITCHFIX1: the ROM's white fade on the way out of the main page (top-state 6 to the file select / Rec Room), 0..255 whiteness */
    int head_t;          /* GLITCHFIX1: ticks since the intro's head arrived (its entrance lerp at rest), -1 = not yet */
} g_ch;
static int g_press_tick;      /* INTRO2: a touch / click went into the ROM this tick */
/* INTRO2: the swap (sw_t counts it) is the logo's 2 s trip in the intro and the planks' own rise on a return; the planks start at their own tick */
bool pc_assets_ready();
static inline int swap_end(const Chore &c) { return c.mode == CH_INTRO ? (pc_assets_ready() ? (int)CH_SWAP_INTRO_PC : (int)CH_LOGO_MOVE) : (int)CH_PLANKS_END; }
/* LOGOFADE1: the intro's logo hand-over (Chore::head_t's clock) can outlast the swap: the intro is not over (no skip window closed, no page at
   rest) until the logo has landed */
static inline bool intro_logo_busy(const Chore &c) { return c.mode == CH_INTRO && !c.skip && c.sw_t >= 0 && c.head_t < (int)CH_LOGO_TRIP && pc_assets_ready(); }
static inline int plank_start(const Chore &c) { return c.mode == CH_INTRO ? (int)CH_PLANK_START_INTRO : (int)CH_PLANK_START; }
static inline int planks_end(const Chore &c) { return c.mode == CH_INTRO ? (int)CH_PLANKS_END_INTRO : (int)CH_PLANKS_END; }

/* INT2 (the decide / draw split): the ROM values build() needs, read once per tick by menu_title_facts on the logic thread and carried in
   MenuLayout.facts.blob. build() and everything it calls read this and never a ROM address. */
struct TitleFacts {
    int nbox;                 /* the live element decoder's answer (0 = it said nothing: the dumped boxes stand in) */
    short bx[24][5];          /* id, x0, y0, x1, y1 */
    unsigned live;            /* MENUS093E: bit k = bx[k] is an element the ROM takes a touch on (a hidden button keeps its box) */
    int cur, prs;             /* rom_cursor(): the cursor element and whether it is being pressed */
    int frame;                /* the title Render counter (data_ov007_0210342c[0]) */
    int vis, s48;             /* the fade manager's iris sprite: shown, scale */
    Chore ch;                 /* the choreography clock as of this tick */
    int toy_e;                /* FACE1: ticks since the press page handed over to the drawing page (top 0 / 1, elem 1) while it still shows our picture */
    int dd_e;                 /* FACE2: ticks since the face press handed over to the drawing page (1 on the first hand-over tick), 0 = not reached from the press (no crossfade) */
    int dd_dead;              /* FACE2: 1 while the drawing page's own picture is not the page yet (the ROM's black hand-over frames): no strokes or buttons are cut from it */
    int out_e;                /* FACE2: ticks since the drawing page was left (1 on the first tick of the next page), 0 = not leaving it */
    int wz;                   /* MENUS093E: the zoom permille of this frame's wide file page (0 = none) */
    int wide3d;               /* FILE2: this tick's 3D frame covered the whole wide frame (the page background was zoomed in the 3D itself: no margin pieces) */
    int yh;                   /* FACE3 (review): the Yoshi hold (element machine 4) after the drawing page: our full-canvas iris covers it too */
    int e3o;                  /* FACE3: ticks since the ROM left the free-draw state for the face tracing (the hand button), 1 on the first tick, 0 = not leaving it */
    int e3;                   /* FACE3: ticks since the ROM entered the free-draw state (element machine 3), 1 on the first tick, 0 = not in it */
    int toy_v;                /* FACE1: the face press page's own frame counter (the page object's +0xc) PLUS ONE while top 4 / elem 0 is up, 0 = not the press page (a zeroed blob is not the press page) */
    int dd_live;              /* TITLE092 (3b): ticks since the drawing page's own picture became the page (1 on the first live tick), 0 = still the ROM's black hand-over frames */
    int sc_on;                /* TITLE092: this frame's 3D was drawn as the full-canvas scene (g_sc), and sc_T is its transform */
    float sc_K, sc_Tx, sc_Ty;
    int pg_top, pg_elem;      /* NAME1: the ROM's top-state and element machine of this tick (-1 = not built) */
};
static_assert(sizeof(TitleFacts) <= ONL_FACTS_OFF, "TitleFacts must leave the tail of MenuFacts.blob to the online facts");
thread_local const TitleFacts *t_f = 0;     /* set for the length of one build() */
int g_iris_cov_req;                         /* build() saw the picture covered: folded into g_ch.iris_covered by the next chore_step (a one-way flag) */
bool box_of(int id, Box *out);

bool box_of_live(int id, Box *out)
{
    int slot[24], ids[24], x0[24], y0[24], x1[24], y1[24];
    const int n = port_title_elem_boxes(slot, ids, x0, y0, x1, y1, 24);
    for (int k = 0; k < n; ++k)
        if (ids[k] == id) { out->id = id; out->x0 = x0[k]; out->y0 = y0[k]; out->x1 = x1[k]; out->y1 = y1[k]; return true; }
    for (int k = 0; k < 24; ++k)
        if (kBoxes[k].id == id) { *out = kBoxes[k]; return true; }
    return false;
}

bool box_of(int id, Box *out)      /* build()'s view: this tick's facts, else the dumped boxes */
{
    if (t_f)
        for (int k = 0; k < t_f->nbox; ++k)
            if (t_f->bx[k][0] == id) {
                out->id = id; out->x0 = t_f->bx[k][1]; out->y0 = t_f->bx[k][2]; out->x1 = t_f->bx[k][3]; out->y1 = t_f->bx[k][4];
                return true;
            }
    for (int k = 0; k < 24; ++k)
        if (kBoxes[k].id == id) { *out = kBoxes[k]; return true; }
    return false;
}

/* the page the ROM is on -> a variant. top-state and element machine are the ROM's own words
   (src/func_ov007_020b0548 dispatcher); the table below is what the traces showed. */
int g_last = V_ATTRACT;
int g_top = -1, g_elem = -1;

int variant_for(int top, int elem)
{
    /* THE TRACE (SM64DS_MENU_TRACE, windowed scene runs): attract 11 -> intro 10 -> arrival 1/0 -> main 0/0;
       a choice leaves through 6/0 (file select) or 4/0 (the doodle) and lands on 0/5 (file select) or 0/1
       (doodle, then 1/1, 2/1); the wrench is 8/0; file options 1/5; scores pick 3/5, copy 7/5, erase 9/5;
       the score table is 4/5 (loading) then 2/5. Transit states keep the last stable page. */
    if (top == 11) return elem == 0 ? V_ATTRACT : g_last;   /* ASSET5: top 11 with the file machine up (elem 5) is the return from a file page, not the first attract */
    if (top == 10) return V_INTRO;                /* the logo arrives while the buttons come in through the ROM's star iris */
    if (elem == 4 && g_last == V_DOODLE) return V_DOODLE;   /* FACE3: the Yoshi press on the drawing page: the ROM holds the page about 40 ticks before its star iris closes; the page stays ours (paper edge to edge) */
    if (elem == 3) return V_DOODLE;                 /* FACE3: the free-draw state after the pencil (reached only from the drawing page): same page family, its own layout in build_doodle */
    if (elem == 2 && g_last == V_DOODLE) return V_DOODLE;   /* FACE2: the ROM's drawing mode proper (elem 2) is the same page as the face tracing (elem 1) */
    if (elem == 1) return ((top == 0 || top == 1) && g_last == V_TOY) ? V_TOY : V_DOODLE;     /* FACE1: the hand-over frames keep our picture */
    /* TITLEUI1: element machine 6 is the face swap's change tick (0 / 6, between top-state 6 and 5, src/unnamed/ov007/func_ov007_020b166c.c reads it):
       still the main page. It used to fall through to the file pages for that one tick, which drew the ROM's whitened lower screen whole (a white frame)
       and made the main family start over as a return (a black veil and the planks rising again): the flash of the swap. */
    if (elem == 6 && (g_last == V_MAIN || g_last == V_INTRO)) return g_last;
    if (elem == 0) {
        if (top == 0) return V_MAIN;
        if (top == 8) return V_OPTIONS;
        if (top == 4) return V_TOY;               /* a press on the face: the head squashes and jumps, the plates leave (FACE1: drawn on the main menu's own wallpaper) */
        if ((top == 1 || top == 5) && (g_last == V_FILES || g_last == V_DOODLE)) return V_MAIN;   /* ASSET5: coming back from a file page: the ROM walks the head in, so
                                                                              this is the main family (face fade, no walk-in of the file page's own head) */
        return g_last;                            /* 1, 6: easing in or out */
    }
    if (top == 2) return V_TABLE;
    if (top == 4) return V_TABLE;                 /* the table loading (4/5): its list already types in, FILE1 */
    return V_FILES;                               /* 0, 1, 3, 7, 9 on the file machine */
}

/* piece + hotspots helper: a hotspot per id, boxes clamped into the piece's source rect */
int g_file_wz = 0;           /* MENUS093E: the zoom (permille) file_wide_cb last applied */
bool g_hot_live_only = false;          /* MENUS093E: the file select family: only elements the ROM takes a touch on */
void hot(MenuLayout *L, int piece, const int *ids, int n)
{
    const DsRect &s = L->p[piece].s;
    for (int k = 0; k < n; ++k) {
        Box b;
        if (!box_of(ids[k], &b)) continue;
        if (g_hot_live_only && t_f && t_f->nbox > 0) {       /* MENUS093E: a button the ROM has hidden (or not brought in yet) is not a hotspot: no hover sound, no click */
            bool live = true;
            for (int j = 0; j < t_f->nbox; ++j)
                if (t_f->bx[j][0] == ids[k]) { live = ((t_f->live >> j) & 1) != 0; break; }
            if (!live) continue;
        }
        int x0 = b.x0 < s.x ? s.x : b.x0, y0 = b.y0 < s.y ? s.y : b.y0;
        int x1 = b.x1 > s.x + s.w ? s.x + s.w : b.x1, y1 = b.y1 > s.y + s.h ? s.y + s.h : b.y1;
        if (x1 <= x0 || y1 <= y0) continue;
        menu_add_hotspot(L, piece, x0, y0, x1 - x0, y1 - y0, -1);
    }
}

int tune(int idx, int dflt)          /* SM64DS_TITLE_TUNE=a,b,c,d,... integer knobs for layout iteration */
{
    static int v[8], have = -1;
    if (have < 0) {
        have = 0;
        for (int i = 0; i < 8; ++i) v[i] = -9999;
        const char *e = getenv("SM64DS_TITLE_TUNE");
        for (int i = 0; e && *e && i < 8; ++i) {
            char *q;
            v[i] = (int)strtol(e, &q, 10);
            if (*q != ',') break;
            e = q + 1;
        }
    }
    return v[idx] == -9999 ? dflt : v[idx];
}

int ltune(int idx, int dflt)         /* SM64DS_MENU_LOGO_TUNE=top,height,dx in canvas units (the PC logo's place) */
{
    static int v[4], have = 0;
    if (!have) {
        have = 1;
        for (int i = 0; i < 4; ++i) v[i] = -9999;
        const char *e = getenv("SM64DS_MENU_LOGO_TUNE");
        for (int i = 0; e && *e && i < 4; ++i) {
            char *q;
            v[i] = (int)strtol(e, &q, 10);
            if (*q != ',') break;
            e = q + 1;
        }
    }
    return v[idx] == -9999 ? dflt : v[idx];
}

}  // namespace


/* ---- THE TITLE'S SPRITE-OBJECT FILTER (run menu1, lane ASSET1) ------------------------------------------
   The lower screen of the title is NOT 2D: engine A has no BG pixels and no sprites there (SM64DS_MSG_COMPOSITE_DEBUG:
   composited 0 px, OAM A 0 placed). Everything on it, the wallpaper, the plates, the wrench, the stars, is a 3D quad
   the ROM submits through func_ov007_020c421c (one call per object per frame). The port arm in that function asks
   this hook whether to draw. SM64DS_TITLE_DRAWTRACE=1 prints each call; SM64DS_TITLE_SKIPK=<hex> skips the calls whose
   per-frame index is a set bit (a probe for naming the objects). */
extern "C" {
extern unsigned char data_020a0e40;   /* the local player's slot */
extern int data_020a0e58[];           /* PadData[i]: {u16 held, u16 pressed} */
extern char *data_ov007_02103360;     /* the title's element-cursor struct (src/func_ov007_020b3708.c) */
}
static inline int *data_020a0e58_ref() { return data_020a0e58; }
static inline int data_020a0e40_ref() { return data_020a0e40; }
extern "C" {
extern int (*g_port_title_draw_skip)(char *obj);
extern int *data_ov007_0210342c;
extern char *data_ov007_02104bd8;     /* the title's screen-fade manager (src/func_ov007_020bf690.c) */
}
namespace {
/* LOGOFADE1: the frame of the title head's animation (the head object at the title global + 0xf4, its animation at + 0x18, the frame at + 0x10
   and the frame count at + 0x12 as src/unnamed/ov007/func_ov007_020b1718.c reads them); -1 when there is none */
static inline int head_anim_frame()
{
    if (!data_ov007_0210342c) return -1;
    const char *F = *(const char *const *)((const char *)data_ov007_0210342c + 0xf4);
    const char *A = F ? *(const char *const *)(F + 0x18) : 0;
    return A ? (int)*(const unsigned short *)(A + 0x10) : -1;
}
int g_tk_frame = -1, g_tk_idx = 0;
int g_up_stamp = -1;          /* the title Render counter at which menu_title_up last answered (the composite of that frame) */

/* The wallpaper is dropped from the submit only while the one-screen main menu is the page, decided from the ROM's own
   state at draw time. g_up_stamp proves the recipe ran for the PREVIOUS frame of this same page family (it is not called
   when the setting is off or the layout is stacked), and the page is read live so the first frame of a page change is
   already right. */
bool pc_assets_ready()
{
    return menu_hostimg_ready(MENU_IMG_LOGO) && menu_hostimg_ready(MENU_IMG_BTN_MP) && menu_hostimg_ready(MENU_IMG_BTN_ADV) &&
           menu_hostimg_ready(MENU_IMG_BTN_REC) && menu_hostimg_ready(MENU_IMG_ARROW_UP) && menu_hostimg_ready(MENU_IMG_ARROW_DOWN) &&
           menu_hostimg_ready(MENU_IMG_PATTERN) && menu_hostimg_ready(MENU_IMG_STAR);   /* UIMOD1: the flying star too: all or the ROM's own title */
}

/* FACE2: the drawing page's two paper sprite objects (ids 0x18 and 0x17) are not submitted on the one-screen drawing page: our own paper is
   drawn, and the 3D layer then holds only the strokes and the two buttons, so each is cut by coverage and placed on its own */
bool doodle_skip_active()
{
    if (!data_ov007_0210342c || g_up_stamp != data_ov007_0210342c[0]) return false;
    int top, elem, req;
    if (!port_title_page_state(&top, &elem, &req)) return false;
    return variant_for(top, elem) == V_DOODLE;
}

bool wall_skip_active()
{
    if (!data_ov007_0210342c || g_up_stamp != data_ov007_0210342c[0]) return false;
    int top, elem, req;
    if (!port_title_page_state(&top, &elem, &req)) return false;
    const int v = variant_for(top, elem);
    if (v == V_TOY || v == V_OPTIONS) return pc_assets_ready();   /* FILE2: the sound / backlight page draws the same moving PC pattern */
    return v == V_MAIN || v == V_INTRO;
}

/* NAME2: the main family only (the title's planks page and its intro), without the sound page and the face press page that wall_skip_active also takes */
bool main_page_skip_active()
{
    if (!data_ov007_0210342c || g_up_stamp != data_ov007_0210342c[0]) return false;
    int top, elem, req;
    if (!port_title_page_state(&top, &elem, &req)) return false;
    const int v = variant_for(top, elem);
    return v == V_MAIN || v == V_INTRO;
}

/* POLISH1A: the file pages' orange cursor arrow. The ROM's cursor object (id 0x21) draws the file_menu_U orange down arrow on the file
   machine's pages (elem 5: select, options, scores pick, copy, erase). While the page is the one-screen file page and the HD arrow
   is ready the sprite is not submitted; its place (DS pixels, the object's own x / y: the arrow's top left is (x - 11, y - 8), the tip
   is the bottom centre) and the title frame it was seen on are kept, and the file page draws the HD arrow there. */
int g_oa_frame = -1, g_oa_x = 0, g_oa_y = 0;
bool file_arrow_up()
{
    if (!data_ov007_0210342c || g_up_stamp != data_ov007_0210342c[0]) return false;
    int top, elem, req;
    if (!port_title_page_state(&top, &elem, &req)) return false;
    return elem != 0 && variant_for(top, elem) == V_FILES && menu_hostimg_ready(MENU_IMG_ARROW_ORANGE);
}

int title_skip(char *o)
{
    if (*(unsigned char *)(o + 0x4e) == 0x21 && file_arrow_up()) {
        g_oa_frame = data_ov007_0210342c[0];
        g_oa_x = ((const int *)o)[1] >> 12;
        g_oa_y = ((const int *)o)[2] >> 12;
        return 1;
    }
    if (data_ov007_0210342c) {
        const char *g = (const char *)data_ov007_0210342c;
        for (int i = 0; i < 3; ++i)
            if (o == *(char *const *)(g + 0x108 + 4 * i)) return wall_skip_active() ? 1 : 0;
    }
    if ((*(unsigned char *)(o + 0x4e) == 0x18 || *(unsigned char *)(o + 0x4e) == 0x17) && doodle_skip_active()) return 1;
    /* ASSET3: the ROM's own cursor arrow (object id 0x21, drawn only while the cursor is shown) is replaced by the host's own pixel
       arrow (the ROM art, cut from this very object), so it is not in the picture on the one-screen main menu */
    if (*(unsigned char *)(o + 0x4e) == 0x21 && wall_skip_active() && menu_hostimg_ready(MENU_IMG_ARROW_UP) && menu_hostimg_ready(MENU_IMG_ARROW_DOWN) &&
        menu_hostimg_ready(MENU_IMG_BTN_MP) && menu_hostimg_ready(MENU_IMG_BTN_ADV) && menu_hostimg_ready(MENU_IMG_BTN_REC)) return 1;   /* only when our arrows will be drawn */
    /* ASSET4: the three plate sprites (object id 0x1b at y >= 100; the wrench is the same id at y 20) are also left out of the
       submit when the host planks are drawn, so the 3D layer holds only the face, star, sparkles and wrench and the face piece can be
       the whole layer (the ROM's plate art no longer shows under the host planks, nothing below the chin is ever in it) */
    if (*(unsigned char *)(o + 0x4e) == 0x1b && (((const int *)o)[2] >> 12) >= 100 && wall_skip_active() && menu_hostimg_ready(MENU_IMG_BTN_MP) &&
        menu_hostimg_ready(MENU_IMG_BTN_ADV) && menu_hostimg_ready(MENU_IMG_BTN_REC)) return 1;
    /* NAME2: with the DS's own planks the cursor arrow is drawn from the code's art (build_main), so the ROM's own, which sits over the plate rows
       of the layer, is left out of the submit and the layer can stop above the plates; the main page only */
    if (*(unsigned char *)(o + 0x4e) == 0x21 && main_page_skip_active() && !(menu_hostimg_ready(MENU_IMG_ARROW_UP) && menu_hostimg_ready(MENU_IMG_ARROW_DOWN) &&
        menu_hostimg_ready(MENU_IMG_BTN_MP) && menu_hostimg_ready(MENU_IMG_BTN_ADV) && menu_hostimg_ready(MENU_IMG_BTN_REC))) return 1;
    static int trace = -1;
    static unsigned skipk;
    if (trace < 0) {
        const char *e = getenv("SM64DS_TITLE_DRAWTRACE");
        trace = (e && *e && *e != '0') ? 1 : 0;
#ifndef PORT_ROM_CLEAN      /* the object-skip probe is a proof arm: not in the shipped build */
        e = getenv("SM64DS_TITLE_SKIPK");
        skipk = e ? (unsigned)strtoul(e, 0, 16) : 0;
#endif
    }
    if (!trace && !skipk) return 0;
    const int fr = data_ov007_0210342c ? data_ov007_0210342c[0] : 0;
    if (fr != g_tk_frame) { g_tk_frame = fr; g_tk_idx = 0; }
    const int k = g_tk_idx++;
    static int trlo = -2, trhi = -2;
    if (trlo == -2) {
        trlo = trhi = -1;
        const char *e = getenv("SM64DS_TITLE_TRACEFR");      /* FACE1: a-b = trace every object call of those title frames */
        if (e) { char *q; trlo = (int)strtol(e, &q, 10); trhi = *q == '-' ? (int)strtol(q + 1, 0, 10) : trlo; }
    }
    if (trace && (fr == 300 || fr == 480 || fr == 600 || (fr >= trlo && fr <= trhi))) {
        const int *w = (const int *)o;
        fprintf(stderr, "[tdraw] f%d k%d obj=%p flags=%08x p4=%d p8=%d tex=%04x id=%02x a=%d\n", fr, k, (void *)o, w[0], w[1], w[2],
                (unsigned)*(unsigned short *)(o + 0x44), (unsigned)*(unsigned char *)(o + 0x4e), (w[0x50 / 4] >> 12) & 0xff);
    }
    return k < 32 && ((skipk >> k) & 1);
}
struct TitleHookInit { TitleHookInit() { g_port_title_draw_skip = title_skip; } } g_title_hook_init;
/* The ROM's own cursor on the main menu (src/func_ov007_020b3708.c: the cursor element is the short at the head of the
   struct data_ov007_02103360 points to; 0 = ADVENTURE, 1 = VS, 2 = REC ROOM, 7 = the wrench, 0xff = the cursor is not shown yet: the ROM
   shows it on the first non-B key, src/func_ov007_020b3190.c), and whether it is being pressed: the DS A key (0x0001, which
   menu_title_pad_map writes into the held word) is down, or the page has already taken a choice (top-state request). */
/* GLITCHFIX1: THE PRESS IS HELD UNTIL THE PAGE LEAVES. Read tick by tick, "pressed" was the A key held, or the request word set while the page is at
   rest; between the key's release and the request, and again once the request (one tick long) had turned into the page's way out (top-state 6, the
   white fade), neither held, so the chosen plank blinked up and down on a confirm. The press is now held from the tick it is seen: across the bridge
   from the release to the ROM's request (at most kPressBridge ticks; a press the ROM never takes lets go), and from the request on for as long as the
   main family is the page (the leave ends it: the file select, the Rec Room, the drawing page and the wrench's page are other pages). The plank it
   was taken on stays the lit one while it is held. Called once per tick (menu_title_facts). */
enum { kPressBridge = 12 };
void rom_cursor(int *cursor, int *pressed)
{
    static int lat = -1, lat_wait = 0;
    *cursor = -1; *pressed = 0;
    if (!(g_last == V_MAIN || g_last == V_INTRO)) lat = -1;
    int top = -1, elem = -1, req = -1;
    const bool st = port_title_page_state(&top, &elem, &req) != 0;
    const short *p0 = data_ov007_02103360 ? *(const short **)data_ov007_02103360 : 0;
    /* the idle face swap asks for top-state 6 through the same request word as a choice; its own request (the title global + 0x10) is 8
       (TITLEART1 (J)), and no plank was pressed for it: it neither shows nor holds a press */
    const bool face_swap = data_ov007_0210342c && data_ov007_0210342c[4] == 8;
    if (p0 && (p0[0] == 7 || (p0[0] >= 0 && p0[0] <= 2))) {      /* 7 = the wrench (ASSET3) */
        *cursor = p0[0];
        if (st && top == 0 && req != -1 && !face_swap) *pressed = 1;
        const unsigned short *held = (const unsigned short *)((char *)data_020a0e58_ref() + data_020a0e40_ref() * 4);
        if (*held & 0x0001) *pressed = 1;
    }
    if (*pressed && *cursor >= 0 && st && top == 0) { lat = *cursor; lat_wait = 0; }      /* a press on the page at rest (not the intro's skip) */
    else if (lat >= 0) {
        const bool taken = st && (req != -1 || top != 0);      /* the ROM took the choice: its request, or the page on its way out (6, 4, 8) */
        if (taken) lat_wait = 0;
        else if (++lat_wait > kPressBridge || (*cursor >= 0 && *cursor != lat)) lat = -1;
    }
#ifndef PORT_ROM_CLEAN
    { static int tr = -1; if (tr < 0) tr = getenv("SM64DS_GF1_TRACE") ? 1 : 0;
      if (tr) fprintf(stderr, "[gf1-cur] f%d top=%d elem=%d req=%d raw_cur=%d raw_prs=%d lat=%d wait=%d\n", data_ov007_0210342c ? data_ov007_0210342c[0] : -1,
                      top, elem, req, *cursor, *pressed, lat, lat_wait); }
#endif
    if (lat >= 0) { *cursor = lat; *pressed = 1; }
}

/* FACE1: THE FACE'S SCALE ON THE MAIN MENU. The head (the whole 3D layer minus the wrench corner) is drawn at kn/kd of the DS layer, its chin
   (source y 134.5, measured on the ROM's own render) kept at canvas y 135.7 and its centre (source x 127.8) on the canvas centre, so a bigger
   head grows upward between the small logo and the wrench and never reaches the planks. SM64DS_FACE_K=n,d picks the step (1,1 / 5,6 / 3,4 / 2,3). */
struct FaceK { int n, d; };
FaceK face_k()
{
    static FaceK k = { 0, 0 };
    if (!k.n) {
        k.n = 5; k.d = 6;
        const char *e = getenv("SM64DS_FACE_K");
        int a = 0, b = 0;
        if (e && sscanf(e, "%d,%d", &a, &b) == 2 && a > 0 && b > 0 && a <= 255 && b <= 255) { k.n = a; k.d = b; }
    }
    return k;
}
inline int face_dx(const FaceK &) { return 0; }
inline int face_dy(const FaceK &k) { return (int)(135.7 - 134.5 * k.n / k.d + 0.5); }     /* canvas units of the layer's source y = 0 */

/* THE MAIN MENU, built from the title's own art objects (run menu1, lane ASSET1; Tango, 10-09: "take the top screen
   and replace the bottom screen's background with it", and the stars stay).
   The lower screen is 3D (see the filter note above). With the wallpaper objects skipped at submit, the picture
   holds only the head, the star and sparkles, the three plates and the wrench, and the 3D coverage plane says
   which pixels those are, so each piece below is cut keyed by coverage: transparent wherever the 3D engine drew
   nothing, so no wallpaper tile, no clear colour and no box comes along. The upper screen (engine B: the logo on
   its sky) is the backdrop, with its "TOUCH TO START" sprites left out (the plates stand where they were), and on
   a wide canvas its sky is carried out to the edges by xfill columns, nothing stretched.
   Source rects are in lower-screen DS pixels and tile the screen: head+star+sparkles (216 x 140, the whole upper
   part; a drag on the face stays inside it), sparkles right of the wrench, the wrench, and the three plates. They
   are placed at the lower screen's own positions on the 4:3 core; tune knobs (SM64DS_TITLE_TUNE) move the head. */
/* NAME1: with the ? button flush against the window's right edge (30 units wide) the wrench steps left by that width, so the pair stays side by side with the ? outermost */
int wrench_dx(const MenuLayout *L)
{
    const OnlFacts *o = onl_facts_of(L);
    return (o && o->on) ? -35 : 0;      /* NAME2: the ? (30 units) and its gap from the window edge (5 units, the same as its gap from the top) */
}

/* ARROWSLOW1 (Tango 10-28: "not jump so rapidly ... interpolate it and make it .25 speed"): THE MAIN MENU'S BLUE CURSOR ARROW BOBS AT A THIRD OF
   THE ROM'S PACE, SMOOTHLY (Tango, same evening: "a little bit faster" than a quarter). The ROM's own arrow steps through 0,1,2,3,4,3,2,1 half units
   on an 8-tick cycle; this one takes 24 ticks over the same
   0 to 4 half units on an eased curve (menu_bob_curve), and with Smooth Motion each picture between ticks is drawn at its own place on that curve
   (hal_menu_present, the piece's bob fields). Without Smooth Motion it steps once a tick. Only the bob changes: the arrow's place when the cursor
   moves to another plank snaps as before. SM64DS_ARROW_BOB=classic draws the ROM's 8-tick cadence (for A/B).
   base2 = the arrow's top at bob 0, in half units; sub = host pixels taken off after the scale (the arrow's height when it hangs above a plank). */
static bool arrow_bob_classic()
{
    static int v = -1;
    if (v < 0) {
        const char *e = getenv("SM64DS_ARROW_BOB");
        v = (e && strcmp(e, "classic") == 0) ? 1 : 0;
    }
    return v != 0;
}
static void arrow_bob_place(MenuPiece &ar, int base2, int sub, int un, int ud, int fr)
{
    if (arrow_bob_classic()) {
        static const int kBob[8] = { 0, 1, 2, 3, 4, 3, 2, 1 };       /* the plank pulse's cadence, in half units */
        ar.fy = (short)(((base2 + kBob[fr & 7]) * un) / (2 * ud) - sub);
        return;
    }
    ar.bob = 1;
    ar.bob_y0 = (short)((int)floor((double)base2 * un / (2.0 * ud) + 0.5) - sub);
    ar.bob_amp = (float)(2.0 * un / ud);                              /* 4 half units in host pixels */
    ar.bob_t = fr;
    ar.fy = (short)(ar.bob_y0 + (int)floor(ar.bob_amp * menu_bob_curve((double)fr) / 4.0 + 0.5));
}

/* NAME1 (run after093): THE ONLINE PLANKS. Plank k of the main page (0 = the MP slot, now ONLINE; 1 = ADVENTURE; 2 = REC ROOM) is in its solo or online
   state, or turning between them (menu_online_plank_state). An online-family plank (ONLINE, ADVENTURE ONLINE, VS) is added here, from the mod's
   picture or drawn in code (hal/menu_online_draw.cpp), over the ROM's own box; the function answers 1 when it did. A solo plank is the caller's own
   (its picture or its ROM cut), drawn at *w1000 permille of its width while it turns. */
bool online_plank(MenuLayout *L, int k, const Box &bx, int rise_px, int alpha, bool lit, bool pressed, bool touch, int *w1000)
{
    const OnlFacts *of = onl_facts_of(L);
    int kind = ONL_K_ONLINE;
    menu_online_plank_state(of, k, &kind, w1000);
    if (kind != ONL_K_ONLINE && kind != ONL_K_SOLO && kind != ONL_K_ADV_ONLINE && kind != ONL_K_VS) return false;
    const int un = L->unum, ud = L->uden;
    const int bh = bx.y1 - bx.y0, dh = bh * un / ud;
    int dw = menu_online_plank_w(kind, dh) * *w1000 / 1000;
    if (dw < 1) dw = 1;
    const int cxu = (bx.x0 + bx.x1) / 2 - 128, cyu2 = bx.y0 + bx.y1;
    const int fx = L->w / 2 + cxu * un / ud - dw / 2, fy = cyu2 * un / (2 * ud) - dh / 2 + rise_px;
    const int ip = menu_online_add_plank(L, kind, fx, fy, dw, dh, bx.x0, bx.y0, bx.x1 - bx.x0, bx.y1 - bx.y0, lit ? 1 : 0, pressed ? 1 : 0, alpha, touch ? 1 : 0,
                                         t_f ? t_f->frame : 0);
    if (touch && ip >= 0) menu_add_hotspot(L, ip, bx.x0, bx.y0, bx.x1 - bx.x0, bx.y1 - bx.y0, -1);
    return true;
}

/* the ? button of the online screens and the modal screen over the page (both after the page's own pieces, so they are topmost) */
void online_overlays(MenuLayout *L, bool main_page, bool show_help)
{
    const OnlFacts *of = onl_facts_of(L);
    if (!of) return;
    const int un = L->unum, ud = L->uden;
    if (show_help && of->scr == ONL_SCR_NONE) {
        (void)main_page;      /* 5 units in from the WINDOW's right edge on every page; on the title the wrench stands beside it (wrench_dx) */
        menu_online_add_help(L, L->w - 5 * un / ud, 5 * un / ud, 30 * un / ud);      /* NAME2: 5 units in from the window's right edge, as it is from the top */
        if (!main_page) menu_online_add_name_plank(L, L->w - 40 * un / ud, 5 * un / ud, 30 * un / ud);      /* PROFILE1: the NAME plank beside the ? on the online file select (5 units gap) */
        if (of->padfocus) {      /* PADNAV1: the pad focus is on the ? or NAME plank: the cursor arrow points up at it from below (the plank is lit too) */
            const int hpx = 30 * un / ud;
            const int right = of->padfocus == ONL_PF_HELP ? L->w - 5 * un / ud : L->w - 40 * un / ud;
            const int pw = of->padfocus == ONL_PF_HELP ? hpx : menu_online_plank_w(ONL_K_NAME, hpx);
            if (!main_page) {
                /* TITLEART1 (Tango 10-26: "it should be orange arrow"): on the file select the cue is the file pages' own ORANGE arrow, at its size (22 x 15
                   units, as the HD one under the file plates) and bobbing on the title's 8-tick cadence, drawn smooth at the window's scale; the same with
                   or without the HD UI mod (the mod draws no arrow of its own here) */
                static const int kBob[8] = { 0, 1, 2, 3, 4, 3, 2, 1 };
                const int bob2 = kBob[(t_f ? t_f->frame : 0) & 7];
                const int aw = 22 * un / ud, ah = 15 * un / ud;
                menu_online_add_arrow(L, 3, right - pw / 2 - aw / 2, 5 * un / ud + hpx + ((2 + bob2) * un) / (2 * ud), aw, ah);
            } else {
            const int aw = 18 * un / ud, ah = aw * 15 / 22;
            menu_online_add_arrow(L, 1, right - pw / 2 - aw / 2, 5 * un / ud + hpx + un / ud, aw, ah);
            }
        }
    }
    menu_online_add_modal(L, of);
}

/* TITLEART1: THE SWAP FADE WITHOUT THE HD UI MOD. Here the face, the wrench and the ROM's own plates are cut from the ROM's lower screen, which the
   ROM itself whitens through its brightness fade (rom_swap_level, the level TITLEUI1 reads for the HD page); only the pieces the port draws in code (the
   online planks, the arrow, the ?) stayed at full strength. They take alpha (255 - whiteness) over a picture that is going white under them, so they go
   white with it. Pieces cut from the ROM's picture are left alone (fading them too would fade them twice). */
Box rest_box(int id, const Box &live)      /* TITLEART1: the element's box on the settled main page (the dumped boxes), else the live one */
{
    for (int k = 0; k < 24; ++k)
        if (kBoxes[k].id == id) return kBoxes[k];
    return live;
}

void swap_fade_host(MenuLayout *L, int from, int a)
{
    if (a >= 255) return;
    for (int i = from; i < L->np; ++i) {
        MenuPiece &p = L->p[i];
        if (p.nodraw || p.src == SRC_TOP_PIC || p.src == SRC_SUB) continue;
        /* TITLEART2: a code drawn piece (custom: the planks, the arrow and the ? without their pictures) is drawn by menu_piece_draw before it looks at
           nodraw, so at the swap's peak (whiteness 0x1000, alpha 0: the change tick 0 / 6 and the first top-state 5 tick)
           they were hidden in name only and drawn at full strength over the white page for two ticks. A custom piece has no touch of its own, so it
           loses its draw call here as well. */
        if (a <= 0) { p.nodraw = 1; p.custom = nullptr; continue; }
        const int own = p.alpha ? p.alpha : 255;
        const int na = own * a / 255;
        p.alpha = (uint8_t)(na < 1 ? 1 : na);
    }
}

/* EGG1 (Tango 10-27): THE SMALL LOGO'S SECRET. A MOUSE click on the logo at the top left of the main page plays one short clip
   (hal/egg_audio.cpp) through the game's own sound, at the game's volume, once; a click while it plays does nothing. Mouse only: the logo
   is no hotspot (the pad / keyboard walk never reaches it, hovering it does nothing and makes no sound) and no touch piece (the click never
   reaches the ROM as the stylus: hal/sub_screen.cpp publishes a recipe click only on a touch piece, so a point off every touch piece is no
   DS touch at all). Where the logo overlaps a touch piece (the head, the wrench, a plank) that piece keeps the click and the ROM has it as
   before; the clip plays only off them. The target: with the PC art, the small logo piece, once it is fully in (not during the intro's trip);
   without it there is no small logo, and the ROM's own logo on the upper-screen backdrop is the target: its letters' box (DS px 11,21 to
   247,141 of that screen) mapped through the backdrop piece. Nothing on the intro, the face press, the file pages or under an online popup.
   The builds (the logic thread's decide, or a draw-side re-solve) write the spot, tagged with the picture size; the pad map (the logic
   thread, once a frame) reads it and only trusts a spot made at the size of this tick's decide. */
struct EggSpot { int on, w, h, x, y, cw, ch, nt; int t[12][4]; };
static std::mutex g_egg_mu;
static EggSpot g_egg;

static void egg_note(MenuLayout *L, int piece, bool ok, const int *dsbox)
{
    EggSpot e;
    memset(&e, 0, sizeof e);
    e.w = L->w; e.h = L->h;
    if (ok && piece >= 0 && piece < L->np) {
        menu_layout_dest(L);      /* pure: the dest rects from the pieces (menu_layout_solve runs it again after build) */
        const int *d = L->dest[piece];
        if (dsbox) {              /* a DS box (x0, y0, x1, y1) of the piece's 256 x 192 source */
            e.x = d[0] + dsbox[0] * d[2] / 256; e.y = d[1] + dsbox[1] * d[3] / 192;
            e.cw = d[0] + dsbox[2] * d[2] / 256 - e.x; e.ch = d[1] + dsbox[3] * d[3] / 192 - e.y;
        } else {
            e.x = d[0]; e.y = d[1]; e.cw = d[2]; e.ch = d[3];
        }
        for (int i = 0; i < L->np && e.nt < 12; ++i) {
            if (!L->p[i].touch) continue;
            memcpy(e.t[e.nt++], L->dest[i], sizeof e.t[0]);
        }
        e.on = e.cw > 0 && e.ch > 0;
    }
    std::lock_guard<std::mutex> lk(g_egg_mu);
    if (hal_menu_trace_on() && (e.on != g_egg.on || e.x != g_egg.x || e.y != g_egg.y || e.cw != g_egg.cw || e.ch != g_egg.ch || e.w != g_egg.w)) {
        fprintf(stderr, "[egg] spot %s: picture %dx%d logo (%d,%d,%d,%d), %d touch pieces", e.on ? "on" : "off", e.w, e.h, e.x, e.y, e.cw, e.ch, e.nt);
        for (int i = 0; i < e.nt; ++i) fprintf(stderr, " (%d,%d,%d,%d)", e.t[i][0], e.t[i][1], e.t[i][2], e.t[i][3]);
        fprintf(stderr, "\n");
    }
    g_egg = e;
}

/* the page is at rest: the intro's choreography is over, no face press, no online popup up */
static bool egg_page_ok(const MenuLayout *L, bool intro)
{
    if (intro || !t_f || t_f->toy_v > 0) return false;
    const Chore &c = t_f->ch;
    if (!(c.sw_t >= swap_end(c) - 4 || c.skip || c.mode == CH_NONE) || intro_logo_busy(c)) return false;
    const OnlFacts *of = onl_facts_of(L);
    return !(of && of->scr != ONL_SCR_NONE);
}

void build_main(MenuLayout *L, bool intro)
{
    int egg_piece = -1, egg_ds = 0;      /* EGG1: the logo piece of this page (egg_ds: the DS backdrop, its logo box) */
    const int cw = L->w * L->uden / L->unum;                 /* exact canvas width in units, floored */
    const int side = cw > 256 ? (cw - 256 + 1) / 2 + 2 : 0;  /* the wide margin on each side, with an overlap */
    L->backdrop = MENU_BACKDROP_TOP_AS_IS;
    L->stage = MENU_STAGE_POST_FADE;
    L->no_focus_paint = 1;      /* the ROM cursor, the lit plank and the arrow are the selection (ASSET3) */
    if (menu_hostimg_ready(MENU_IMG_LOGO)) {
        /* ASSET2: the sky is a smooth gradient at the window's own resolution, coloured from the DS logo screen's own sky
           (the mean of its leftmost 16 columns, per row; those columns hold no logo), and the "Super Mario 64 PC" logo is a
           host picture scaled to the window: the DS logo layer is not drawn at all. Both fill / centre on any aspect. */
        MenuPiece gr = menu_piece_make(SRC_SUB, 0, 0, 16, 192, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        gr.bg = 1; gr.hostgrad = 1; gr.layers = 0x0f;
        gr.lden = 1; gr.lnum = 1; gr.fx = 0; gr.fy = 0; gr.fw = (short)L->w; gr.fh = (short)L->h;
        menu_add_piece(L, gr);
        int iw = 1, ih = 1;
        menu_hostimg_size(MENU_IMG_LOGO, &iw, &ih);
        const int lt = ltune(0, 26), lh = ltune(1, 112), ldx = ltune(2, 0);
        const int dh = lh * L->unum / L->uden, dw = (int)((long long)dh * iw / ih);
        MenuPiece lg = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        lg.bg = 1; lg.hostimg = MENU_IMG_LOGO;
        lg.lden = 1; lg.lnum = 1; lg.fw = (short)dw; lg.fh = (short)dh;
        lg.fx = (short)((L->w - dw) / 2 + ldx * L->unum / L->uden); lg.fy = (short)(lt * L->unum / L->uden);
        egg_piece = menu_add_piece(L, lg);
    } else {
    if (side > 0) {
        MenuPiece lf = menu_piece_make(SRC_SUB, 0, 0, 1, 192, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        lf.bg = 1; lf.xfill = 1; lf.fill_w = (short)side; lf.layers = (uint8_t)tune(5, 0x0f);
        menu_add_piece(L, lf);
        MenuPiece rt = menu_piece_make(SRC_SUB, 255, 0, 1, 192, MENU_AX_R, MENU_AY_T, 0, 0, 1, 1);
        rt.bg = 1; rt.xfill = 1; rt.fill_w = (short)side; rt.layers = (uint8_t)tune(5, 0x0f);
        menu_add_piece(L, rt);
    }
    MenuPiece bg = menu_piece_make(SRC_SUB, 0, 0, 256, 192, MENU_AX_C, MENU_AY_M, 0, 0, 1, 1);
    bg.bg = 1;
    bg.layers = (uint8_t)tune(5, 0x0f);
    egg_piece = menu_add_piece(L, bg);
    egg_ds = 1;
    }
    /* THE INTRO (top-state 10): the ROM opens the lower screen through a black star iris, drawn as 3D polygons with a thin
       dark edge, and the plates and the head arrive inside it. On one screen the logo simply fades in alone and the
       lower screen's art appears with the page itself, so nothing of the iris is drawn. */
    menu_online_add_tint(L, onl_facts_of(L));      /* NAME1: the green over the sky (and, with the DS logo, its blue letters) */
    if (intro) { egg_note(L, -1, false, nullptr); return; }
    if (tune(6, -1) >= 0 && tune(7, -1) >= tune(6, -1)) {
        /* DEBUG (SM64DS_TITLE_TUNE index 6,7 = first,last engine-B OAM entry): the upper screen's OAM entries alone, keyed on
           their own transparency, drawn at the top of the picture: the proof of the OAM-range plane on a live screen */
        MenuPiece op = menu_piece_make(SRC_SUB, 0, 150, 256, 40, MENU_AX_C, MENU_AY_T, 0, 4, 1, 1);
        op.layers = MENU_L_OBJ; op.oam_on = 1; op.oam_lo = (uint8_t)tune(6, 0); op.oam_hi = (uint8_t)tune(7, 0); op.key_backdrop = 1;
        menu_add_piece(L, op);
    }
    /* the stars and sparkles stay where the ROM puts them on the 4:3 core (a piece anchored C with dx = srcx - 128 + w / 2
       lands at its own lower-screen x); the head is cut with room around it for a stretch, scaled 2/3 and set low over the
       logo's DS like a mascot; the wrench sits in the top right corner of the canvas */
    /* ASSET4: THE FACE IS NEVER CUT. The title's lower 3D layer now holds only the head, the star, the sparkles and the wrench
       (wallpaper, plates and the ROM's cursor arrow are skipped at submit), so the face piece is the WHOLE 256 x 192 layer minus
       the wrench's own 40 x 40 corner (excluded by rect, so the two never overlap): whatever pose the idle animation or the
       press squash takes, the head, cap, ears, chin, star and sparkles are inside the source. It keeps the round 3 scale and
       place (a source x maps to centre - 83.33 + 2x/3 units, y to 46 + 2y/3, exactly the old 156 x 140 head piece's transform),
       and stars and sparkles now share that transform so they stay where the ROM put them around the face. A touch-only piece
       holds the old head rect (the ROM's face toy reads a touch on the head). */
    const int hk = tune(0, 2), hd = tune(1, 3);
    /* NAME2 (Tango, 4:3 plank overlap): with the DS's own plates (no host planks) the layer still holds the three plates, and this piece draws
       the whole layer at 2/3 scale, so they showed in small behind and above the code drawn SOLO / ADVENTURE ONLINE / VS planks, and behind the
       ROM's own planks (the cut pieces below draw the real plates at full size). The plates start at source row 140 (the cut pieces' own y),
       the head, star, sparkles and cursor are above it: the piece stops there. With the host planks the plate sprites are never submitted. */
    const bool plates_in_layer = !(menu_hostimg_ready(MENU_IMG_BTN_MP) && menu_hostimg_ready(MENU_IMG_BTN_ADV) && menu_hostimg_ready(MENU_IMG_BTN_REC));
    const int np_swap = L->np;      /* TITLEART1: from here the lower screen's pieces; the face swap fades the code-drawn ones (swap_fade_host) */
    MenuPiece face = menu_piece_make(SRC_TOP_PIC, 0, 0, 256, plates_in_layer ? 140 : 192, MENU_AX_C, MENU_AY_T, tune(2, -18) + 20, tune(3, 46), hk, hd);
    face.keycover = 1; face.bg = 1; face.has_ex = 1; face.ex.x = 216; face.ex.y = 0; face.ex.w = 40; face.ex.h = 40;
    /* INTRO2: the ROM's head entrance fades in over its first 9 ticks on the DS-art title too (the intro only, never a later page) */
    int ent_a = 255;
    if (t_f && t_f->ch.mode == CH_INTRO && t_f->ch.sw_t >= 0 && !t_f->ch.skip && t_f->ch.sw_t < 9) ent_a = (t_f->ch.sw_t + 1) * 255 / 9;
    if (t_f && t_f->ch.mode == CH_INTRO && t_f->ch.sw_t < 0 && !t_f->ch.skip && t_f->ch.ct >= CH_LOGO_POP + CH_BOUNCE_LEN) ent_a = 1;      /* before the swap the head is still off the picture: nothing of the layer shows */
    if (ent_a < 255) face.alpha = (uint8_t)ent_a;
    menu_add_piece(L, face);
    MenuPiece head = menu_piece_make(SRC_TOP_PIC, 20, 0, 156, 140, MENU_AX_C, MENU_AY_T, tune(2, -18), tune(3, 46), hk, hd);
    head.touch = 1; head.nodraw = 1;
    menu_add_piece(L, head);
    MenuPiece wr = menu_piece_make(SRC_TOP_PIC, 216, 0, 40, 40, MENU_AX_R, MENU_AY_T, wrench_dx(L), 0, 1, 1);
    wr.keycover = 1; wr.touch = 1;
    if (ent_a < 255) wr.alpha = (uint8_t)ent_a;
    if (settings_page_press_look() == 2) { wr.dy = 1; wr.dx = (short)(wrench_dx(L) - 1); }   /* HUDMENU2 (H4): the wrench sinks while the press hold of a pad / key confirm runs */
    const int iwr = menu_add_piece(L, wr);
    static const int kPlateId[3] = { 1, 0, 2 };                      /* the ROM's element ids: VS, ADVENTURE, REC ROOM */
    static const int kPlankImg[3] = { MENU_IMG_BTN_MP, MENU_IMG_BTN_ADV, MENU_IMG_BTN_REC };
    const int cur = t_f ? t_f->cur : -1, prs = t_f ? t_f->prs : 0;
    const bool planks = menu_hostimg_ready(MENU_IMG_BTN_MP) && menu_hostimg_ready(MENU_IMG_BTN_ADV) && menu_hostimg_ready(MENU_IMG_BTN_REC);
    for (int k = 0; k < 3; ++k) {
        Box bx;
        if (!box_of(kPlateId[k], &bx)) continue;
        int w1000 = 1000;
        /* TITLEART1: a code plank stands on its element's REST box (the dumped one). After the face swap the ROM brings its plates back in from off the
           screen (top-state 1) and the live boxes travel with them, wrapping past the screen's edges; a code plank that followed them vanished, then slid
           in crossed with its neighbour and cut by the window's edge (VS over SOLO). The ROM's own plate art is not in this picture (it is under the
           code plank), so nothing of the ROM's arrival is lost; a touch during it is not taken by the ROM anyway. */
        const Box rb = rest_box(kPlateId[k], bx);
        if (online_plank(L, k, rb, 0, 0, cur == kPlateId[k], cur == kPlateId[k] && prs, true, &w1000)) continue;   /* NAME1: ONLINE / ADVENTURE ONLINE / VS */
        if (planks) {
            /* ASSET2: a plank drawn at the window's resolution over the ROM's own box (the 3D plate is not drawn: its
               wallpaper object is skipped at submit and its sprite is not in any piece). 40 units tall (the box height),
               centred on the box, the three never overlap; clicks map proportionally into the box, so they reach the ROM exactly. */
            int iw = 1, ih = 1;
            menu_hostimg_size(kPlankImg[k], &iw, &ih);
            const int bh = bx.y1 - bx.y0;
            const int dh = bh * L->unum / L->uden, dw0 = (int)((long long)dh * iw / ih);
            const int dw = dw0 * w1000 / 1000 < 1 ? 1 : dw0 * w1000 / 1000;     /* NAME1: the turn */
            const int cxu = (bx.x0 + bx.x1) / 2 - 128, cyu2 = bx.y0 + bx.y1;   /* centre x from 128, centre y * 2 */
            MenuPiece pl = menu_piece_make(SRC_HOSTIMG, bx.x0, bx.y0, bx.x1 - bx.x0, bx.y1 - bx.y0, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
            pl.hostimg = (uint8_t)kPlankImg[k]; pl.touch = 1;
            pl.lden = 1; pl.lnum = 1; pl.fw = (short)dw; pl.fh = (short)dh;
            pl.fx = (short)(L->w / 2 + cxu * L->unum / L->uden - dw / 2);
            pl.fy = (short)(cyu2 * L->unum / (2 * L->uden) - dh / 2);
            if (cur == kPlateId[k]) pl.hl = (uint8_t)(1 | (prs ? 2 : 0));
            const int ip = menu_add_piece(L, pl);
            menu_add_hotspot(L, ip, bx.x0, bx.y0, bx.x1 - bx.x0, bx.y1 - bx.y0, -1);
            continue;
        }
        static const int kPlateX[3] = { 0, 70, 188 }, kPlateW[3] = { 70, 118, 68 };
        MenuPiece pl = menu_piece_make(SRC_TOP_PIC, kPlateX[k], 140, kPlateW[k], 52, MENU_AX_C, MENU_AY_B, kPlateX[k] - 128 + kPlateW[k] / 2, 0, 1, 1);
        pl.keycover = 1; pl.touch = 1;
        if (w1000 < 1000) {                                                    /* NAME1: the turn of the ROM's own plate: its cut, narrowed about its centre */
            const int un = L->unum, ud = L->uden;
            int dwf = kPlateW[k] * un / ud * w1000 / 1000;
            if (dwf < 1) dwf = 1;
            const int dhf = 52 * un / ud, cx = L->w / 2 + (kPlateX[k] - 128 + kPlateW[k] / 2) * un / ud;
            pl.fixdest = 1; pl.fx = (short)(cx - dwf / 2); pl.fy = (short)(L->h - dhf); pl.fw = (short)dwf; pl.fh = (short)dhf;
        }
        const int ip = menu_add_piece(L, pl);
        hot(L, ip, &kPlateId[k], 1);
    }
    static const int wr_ids[1] = { 7 };
    hot(L, iwr, wr_ids, 1);
    /* ASSET3: THE SELECTION ARROW. The ROM's own cursor arrow (a small blue triangle, drawn as a 3D object and skipped at submit
       above) is drawn again from its own art at the window's resolution: pointing down at the selected plank from above, or up at the
       wrench from below, bobbing on the ROM's 8-frame cadence. Pieces here are backdrop-class (no touch): a click goes through to
       the plank under it. */
    /* NAME2: without the mod's pictures the arrow is the code's own copy of the same art (the ROM's own is skipped at submit, title_skip) */
    const bool art_arrows = planks && menu_hostimg_ready(MENU_IMG_ARROW_UP) && menu_hostimg_ready(MENU_IMG_ARROW_DOWN);
    const OnlFacts *pfm = onl_facts_of(L);     /* PADNAV1: no arrow over the wrench while the pad focus is on the ? */
    if ((cur == 7 || (cur >= 0 && cur <= 2)) && !(pfm && pfm->padfocus)) {
        const int fr = t_f ? t_f->frame : 0;                           /* ARROWSLOW1: the bob is arrow_bob_place's */
        const int aimg = cur == 7 ? MENU_IMG_ARROW_UP : MENU_IMG_ARROW_DOWN;
        int iw = 22, ih = 15;
        if (art_arrows) menu_hostimg_size(aimg, &iw, &ih);
        const int au = ltune(3, 18);                                  /* the arrow's width in canvas units (the DS art is 22 x 15 pixels) */
        const int dw = au * L->unum / L->uden, dh = (int)((long long)dw * ih / iw);
        MenuPiece ar = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        ar.bg = 1; ar.hostimg = (uint8_t)aimg; ar.lden = 1; ar.lnum = 1; ar.fw = (short)dw; ar.fh = (short)dh;
        if (cur == 7) {
            Box wb;
            if (!box_of(7, &wb)) wb = kBoxes[7];
            ar.fx = (short)(L->w - (256 - (wb.x0 + wb.x1) / 2 - wrench_dx(L)) * L->unum / L->uden - dw / 2);
            arrow_bob_place(ar, wb.y1 * 2 + 2, 0, L->unum, L->uden, fr);
        } else {
            /* ASSET4 (Tango): ONE arrow, centred on the plank, pointing down. ARROWFIX1: raised 2 units so the flat tip no longer sinks into the
               plank's dark top border and reads as cut off (it bobs from 1 unit above the border to 1 unit into it) */
            Box bx;
            if (box_of(cur, &bx)) {
                if (!art_arrows) bx = rest_box(cur, bx);      /* TITLEART1: the code arrow names the code plank, which stands on its rest box */
                arrow_bob_place(ar, bx.y0 * 2 - 2, dh, L->unum, L->uden, fr);
                ar.fx = (short)(L->w / 2 + ((bx.x0 + bx.x1) / 2 - 128) * L->unum / L->uden - dw / 2);
            } else {
                ar.fx = -1000;
            }
        }
        if (ar.fx > -1000) {
            if (art_arrows) menu_add_piece(L, ar);
            else {
                const int np0 = L->np;
                menu_online_add_arrow(L, cur == 7 ? 1 : 0, ar.fx, ar.fy, dw, dh);
                if (L->np == np0 + 1) {      /* ARROWSLOW1: the code arrow bobs the same way */
                    MenuPiece &cp = L->p[np0];
                    cp.bob = ar.bob; cp.bob_y0 = ar.bob_y0; cp.bob_amp = ar.bob_amp; cp.bob_t = ar.bob_t;
                }
            }
        }
    }
    const Chore cs = t_f ? t_f->ch : Chore();
    if (cs.swap_a >= 0) swap_fade_host(L, np_swap, cs.swap_a);      /* TITLEART1: the code planks and the arrow take the ROM's swap fade */
    const int np_ov = L->np;
    { const OnlFacts *of = onl_facts_of(L); online_overlays(L, true, of && of->on); }     /* NAME1: the ? button and the modal screen */
    { const OnlFacts *of = onl_facts_of(L); if (cs.swap_a >= 0 && !(of && of->scr != ONL_SCR_NONE)) swap_fade_host(L, np_ov, cs.swap_a); }     /* TITLEART1: the ? too */
    if (cs.swap_a >= 0 && hal_menu_trace_on()) fprintf(stderr, "[swapfade-host] f%d level %d alpha %d\n", t_f ? t_f->frame : -1, cs.swap_lv, cs.swap_a);      /* TITLEART2 */
    {
        static const int kDsLogo[4] = { 11, 21, 247, 141 };      /* EGG1: the ROM logo's letters on its upper screen (measured on the 4:3 picture, out/EGG1) */
        egg_note(L, egg_piece, egg_page_ok(L, intro), egg_ds ? kDsLogo : nullptr);
    }
}


/* ====================================================================================================================
   ASSET5: THE TITLE CHOREOGRAPHY (one-screen mode only). Tango, 10-09: the attract shows the bottom screen as the DS does, the star flies off the
   top, the star iris opens on our menu, the PC logo drops and bounces, the face fades in, the logo fades out and back in small at the top
   left while MP / ADVENTURE / REC ROOM bounce in.

   WHAT THE ROM DOES (measured with SM64DS_TITLE_TL, every frame of the trace is a line; 60 frames a second):
     top-state 11 (attract): the star flies up the lower screen and off its top edge (about frame 52 of a run with no input), the sparkle trail
        follows; the ROM goes on to show the logo on the UPPER screen, which one screen does not show.
     top-state 10: counter t from 0. The lower screen is black (the ROM faded it, RequestScreenFade(3, 10)); sound events at t = 30 and t = 128;
        at t = 60 the fade manager (data_ov007_02104bd8) raises its star sprite (the iris): sprite scale s grows by 1889.5 a frame, the star's
        outer radius is 0.004382 * s DS pixels, it covers the whole lower screen after about 35 frames, and the ROM lets it run 68 frames
        (progress at +60 a frame to 4096), then calls func_ov007_020b16f0 which asks for top-state 1.
     top-state 1 (arrival): 30 frames, the head and plates come in; top-state 0 (the settled main menu) follows.
   The DS logo (upper screen) pops in with a vertical damped bounce: centre y 78 +- 20 DS pixels, period 8 frames, amplitude falling 0.78 every 4
   frames (rigid translation, scale 1, x fixed; measured by template matching 20 frames of the engine-B picture).

   WHAT THIS DOES WITH IT. All timing below is in 60 Hz frames.
     - the attract is the lower screen as it is (V_ATTRACT); in one-screen mode, once the star and its first sparkles are off the top of the
       lower screen (attract counter 56), the port gives the ROM the touch-to-start edge it would have taken from the player (the same path the
       ROM's own skip takes: the upper-screen logo reveal that follows is not on this picture anyway). SM64DS_TITLE_FF=0 turns this and the
       next two off.
     - state 10: the picture is black until the ROM's iris sprite appears; from the t = 30 voice on the counter runs the ROM's own way (INTROVOICE2:
       the dead 27 frames used to be skipped, which put the t = 60 iris whoosh 3 frames after "It's-a me" and the t = 128 "Mario!" 41 frames
       after it, over its tail; the DS has 30 and 98 frames).
     - the iris: this file reads the ROM's own sprite scale every frame, so the pacing is the ROM's, and draws the star clean over the wallpaper
       (menu_compose.cpp draw_iris). Once it covers the picture the ROM's own iris is told it is finished (progress set to its end) so the arrival
       follows at once instead of 33 frames later.
     - the PC logo pops in 22 frames after the iris appears (about two thirds open) with the DS logo's own bounce.
     - the swap starts when the logo has bounced and the ROM is at rest (top-state 0, the face is where it is on the 4:3 screen): the face fades in over
       24 frames (it is never drawn before: no walk in from off screen), the big logo fades out over 24, the small logo fades in at the top left over 30 (B2 (d), TRANS093: slower than the 10 and 14 it had),
       and the planks come up from the bottom edge with an overshoot, 6 frames apart, 28 frames each. Input is live as soon as the ROM is at rest.
       A confirm press during the swap finishes it at once and still goes to the ROM (it shows the cursor on ADVENTURE).
     - a return from a file page (the ROM walks the head in again) skips the iris and the big logo: the small logo is there, the page fades in from
       black over 10 frames, and the same swap runs for the face and the planks.
   ==================================================================================================================== */

int ff_on()
{
    static int v = -1;
    if (v < 0) { const char *e = getenv("SM64DS_TITLE_FF"); v = (e && *e == '0') ? 0 : 1; }
    return v;
}

int ctune(int idx, int dflt)         /* SM64DS_CHORE_TUNE=a,b,c,... integer knobs for timing / look iteration */
{
    static int v[12], have = 0;
    if (!have) {
        have = 1;
        for (int i = 0; i < 12; ++i) v[i] = -9999;
        const char *e = getenv("SM64DS_CHORE_TUNE");
        for (int i = 0; e && *e && i < 12; ++i) {
            char *q;
            v[i] = (int)strtol(e, &q, 10);
            if (*q != ',') break;
            e = q + 1;
        }
    }
    return v[idx] == -9999 ? dflt : v[idx];
}

/* the ROM's iris sprite: visible flag and scale (the fade manager's sprite, offsets +0x94 and +0x48) and its progress object */
bool rom_iris(int *vis, int *s48)
{
    *vis = 0; *s48 = 0;
    const char *F = data_ov007_02104bd8;
    if (!F) return false;
    const char *sub = *(const char *const *)(F + 4);
    if (!sub) return false;
    *vis = *(const int *)(sub + 0x94) != 0;
    *s48 = *(const int *)(sub + 0x48);
    return true;
}

/* TITLEUI1: THE FACE SWAP'S FADE. Idle on the main page, the ROM changes the head (Mario <-> Yoshi): top-state 6 (src/unnamed/ov007/func_ov007_020b155c.c)
   asks the screen-fade manager for RequestScreenFade(1, 0, ..) and, after the change, top-state 5 (func_ov007_020b166c.c, element machine 6 before it)
   for RequestScreenFade(1, 2, ..). Type 1 (src/unnamed/ov007/func_ov007_020bf790.c) is a brightness fade of the whole lower screen (object mode 4, layers
   0x3f, brightness +0x10: toward white, step 0x3c a tick: 68 ticks), flags 0 / 2 = no iris sprite, the second one run backwards. The manager's object
   (data_ov007_02104bd8 -> obj6) holds the level: progress +0x14 (0..0x1000) through curve +0x10, turned round when +0x1c is 1
   (src/unnamed/ov007/func_ov007_020c8498.c), handed to func_ov007_020c8b34.c, which writes the lower screen's blend brightness from it: +0x2c (+16, white)
   at 0, +0x30 (0, the picture as it is) at 0x1000. Returns the whiteness, 0x1000 minus that level (0 = untouched, 0x1000 = all white), while such a fade
   (a white brightness fade without the iris) is up or held at its end, else -1. */
int rom_swap_level()
{
    const char *F = data_ov007_02104bd8;
    if (!F) return -1;
    const char *o6 = *(const char *const *)F;
    if (!o6) return -1;
    const int mode = *(const int *)(o6 + 0), st = *(const int *)(o6 + 8), curve = *(const int *)(o6 + 0x10), prog = *(const int *)(o6 + 0x14);
    const int dir = *(const int *)(o6 + 0x1c), bright = *(const int *)(o6 + 0x2c), flags = *(const int *)(F + 8);
    if (mode != 4 || st == 0 || bright <= 0 || (flags & 1)) return -1;
    int r = prog;
    if (curve == 1) { r = prog << 1; if (r > 0x1000) r = 0x2000 - r; }
    else if (curve == 2) r = (int)(((long long)prog * prog) >> 12);
    if (dir == 1) r = 0x1000 - r;
    r = 0x1000 - r;
    return r < 0 ? 0 : r > 0x1000 ? 0x1000 : r;
}

float ease_out_back(float t)         /* 0..1 -> 0..1 with a 9 percent overshoot that settles at 1 */
{
    if (t <= 0.f) return 0.f;
    if (t >= 1.f) return 1.f;
    const float c1 = 1.70158f * 0.8f, c3 = c1 + 1.f, u = t - 1.f;
    return 1.f + c3 * u * u * u + c1 * u * u;
}

static int g_face_swap;      /* TITLEART1 (J): see chore_step */
/* one frame of the choreography clock, from menu_title_up (once per DS frame, after the ROM ran its tick) */
void chore_step(int variant, int top, int elem, int req)
{
    Chore &c = g_ch;
    const bool fam = variant == V_INTRO || variant == V_MAIN;
    if (!fam) {
        g_face_swap = 0;
        c.in_fam = 0; c.mode = CH_NONE; c.prev_variant = variant; c.leave_w = 0;
        if (variant == V_ATTRACT && data_ov007_0210342c) {
            const char *sp = *(const char *const *)((const char *)data_ov007_0210342c + 8);
            c.attract_t = sp ? *(const int *)(sp + 0xc) : 0;
        }
        return;
    }
    if (!c.in_fam) {              /* entering the main family */
        c.in_fam = 1;
        c.mode = (c.prev_variant == V_ATTRACT) ? CH_INTRO : CH_RETURN;
        c.ct = -1; c.fam_t = 0; c.sw_t = -1; c.face_a = 0; c.skip = 0; c.iris_covered = 0; c.ff2_done = 0; c.fly_t = 0; c.released = 0; c.head_t = -1;
        if (hal_menu_trace_on()) fprintf(stderr, "[chore] enter family mode=%s top=%d\n", c.mode == CH_INTRO ? "intro" : "return", top);
    }
    ++c.fam_t;
    if (!g_press_tick) c.released = 1;
    g_press_tick = 0;
    if (g_iris_cov_req) { g_iris_cov_req = 0; c.iris_covered = 1; }      /* build() saw the picture covered */
    const bool rest = top == 0 && elem == 0 && req == -1;
    int vis = 0, s48 = 0;
    rom_iris(&vis, &s48);
    if (c.mode == CH_INTRO) {
        if (c.skip) c.iris_covered = 1;      /* INTRO2: a press skips the wipe too */
        if (c.ct >= 0 && !vis && !c.iris_covered) c.iris_covered = 1;      /* the ROM's iris is gone: nothing left to draw */
        if (c.ct < 0) {
            if (top == 10 && !vis) ++c.fly_t;
            if (vis && top == 10) { c.ct = 0; if (hal_menu_trace_on()) fprintf(stderr, "[chore] iris starts (s48 %d)\n", s48); }
            else if (top == 1 || top == 0) c.ct = ctune(0, CH_LOGO_POP) + ctune(1, CH_BOUNCE_LEN);   /* no iris came: the intro is over */
        } else {
            ++c.ct;
        }
    }
    const int bounce_end = ctune(0, CH_LOGO_POP) + ctune(1, CH_BOUNCE_LEN);
    const bool bounced = c.mode == CH_RETURN || c.skip || (c.ct >= bounce_end);
    const bool arriving = c.mode == CH_RETURN && top == 1 && elem == 0;      /* ASSET6: the ROM's head is on its way in */
    if (c.sw_t < 0) {
        if ((rest || arriving) && bounced) {
            c.sw_t = 0;
            if (hal_menu_trace_on()) fprintf(stderr, "[chore] swap starts (ct %d, fam_t %d)\n", c.ct, c.fam_t);
        }
    } else {
        ++c.sw_t;
    }
    if (c.skip && c.sw_t >= 0 && c.sw_t < swap_end(c)) c.sw_t = swap_end(c);
    /* INTRO2: in the intro the head is not faded in: the ROM's own entrance (top-state 1, src/func_ov007_020b1b74.c: the head object plays
       its fly-in while the scale ramps for 30 ticks) is shown from its first tick, whole, and the swap (the logo's trip) starts when it is at rest */
    const bool head_in = c.mode == CH_INTRO && bounced && (top == 1 || rest);
    /* the face: faded in once the ROM is at rest and the swap began; faded out quickly when the ROM leaves rest (it walks out) */
    /* the flight is the ROM's, at its own speed (top-state 0's first anim: the head is not on the picture before the swap starts); only the pop is
       smoothed: transparent to full over the first 9 ticks of the swap (Tango 10-24 late) */
    if (c.sw_t < 0) c.face_a = (head_in && c.skip) ? 255 : 0;
    else if (head_in || rest || arriving) {
        const int step = c.skip || c.sw_t >= swap_end(c) ? 255 : (c.mode == CH_INTRO ? 255 / 9 + 1 : 255 / CH_FACE_FADE + 1);
        c.face_a += step; if (c.face_a > 255) c.face_a = 255;
    } else {
        c.face_a -= 40; if (c.face_a < 0) c.face_a = 0;
    }
    /* GLITCHFIX1: THE HEAD HAS ARRIVED when the ROM's entrance is over (top-state 0, its scale word g+0xff8 settled at 0x1000: src/func_ov007_020b1b74.c
       ramps it over the 30 ticks of top-state 1 and leaves it there) and the head is drawn whole (face_a at 255, the end of the pop's smoothing).
       The big logo stays solid until then (build_main_pc): its fade used to start on the swap's first tick, so the head arrived behind a logo already
       4 to 15 percent see-through. */
    /* LOGOFADE1: the scale word is at g+0xf8 (the ROM's stores: str [r0, #0xf8] at 020b1c2c / 020b1cac / 020b1cdc; the shadow struct's member is
       named ff8 for its offset, 0xf8). GLITCHFIX1 read g+0xff8, a word that is never 0x1000 there, so the check never fired. */
    if (c.mode == CH_INTRO && c.sw_t >= 0) {
        const int ff8 = data_ov007_0210342c ? *(const int *)((const char *)data_ov007_0210342c + 0xf8) : 0x1000;
        if (c.skip && c.head_t < CH_LOGO_TRIP) c.head_t = CH_LOGO_TRIP;
        else if (c.head_t >= 0) ++c.head_t;
        else if (top == 0 && ff8 == 0x1000 && c.face_a >= 255 && head_anim_frame() >= CH_HEAD_REST_FRAME) {
            c.head_t = 0;
            if (hal_menu_trace_on()) fprintf(stderr, "[chore] the head has arrived (sw_t %d): the logo's hand-over starts\n", c.sw_t);
        } else if (c.sw_t >= CH_HEAD_WAIT) {
            c.head_t = 0;      /* never hold the intro on a solid logo */
            if (hal_menu_trace_on()) fprintf(stderr, "[chore] the head has not arrived by sw_t %d: the logo's hand-over starts anyway (top %d ff8 %d face_a %d)\n",
                                             c.sw_t, top, ff8, c.face_a);
        }
    }
    /* TITLEUI1: the face swap (top-state 6, the change tick 0 / 6, top-state 5): the head, the wrench and every port plank take the ROM's own fade
       level (rom_swap_level) tick for tick, so the face is not faded a second time by the port's own clock */
    /* TITLEART1 (J): ONLY THE FACE SWAP. Top-state 6 also leaves the main page for the file select and the Rec Room with the same white fade;
       the face swap is the one whose request word (the title global + 0x10, src/unnamed/ov007/func_ov007_020b155c.c) is 8, the case whose fade
       callback (func_ov007_020b1604.c) sets element machine 6 and so the fade back of top-state 5. A leave keeps the main page's own way out
       (the face drops in about 6 ticks, the planks stay until the cut). g_face_swap: 0 none, 1 fading out (top 6, 0 / 6), 2 coming back. */
    {
        const int g10 = data_ov007_0210342c ? data_ov007_0210342c[4] : -1;
        if (top == 6 && elem == 0 && g10 == 8 && g_face_swap == 0) g_face_swap = 1;
        if (top == 0 && elem == 6 && g_face_swap == 0) g_face_swap = 1;
        if (g_face_swap && rest && c.sw_t >= swap_end(c)) g_face_swap = 0;
    }
    c.swap_a = -1; c.swap_lv = -1;
    if (g_face_swap && ((top == 6 && elem == 0) || (top == 0 && elem == 6) || (top == 5 && elem == 0))) {
        const int lv = rom_swap_level();
        if (lv >= 0) { c.swap_lv = lv; c.swap_a = 255 - (lv * 255 + 0x800) / 0x1000; if (top != 5) c.face_a = 255; }
    }
    /* TITLEART1 (J): THE WAY BACK. In top-state 5 the ROM fades the page back in from white with no head on it, and its top-state 1 then brings the
       new head and the plates in. The page enters ONCE, as a return from a file page does (b2's look): from the first tick of top-state 5 the head,
       the wrench, the planks, the ? and the arrow are off, and the ROM's arrival starts the swap clock again, so the head and the wrench fade in
       over CH_FACE_FADE from the tick the ROM begins to bring the head (no pop at full strength) and the planks rise once from the bottom edge. */
    if (g_face_swap == 1 && top == 5) {
        g_face_swap = 2;
        c.mode = CH_RETURN; c.sw_t = -1; c.fam_t = CH_VEIL + 4; c.face_a = 0; c.skip = 0;      /* fam_t past the return veil: the page is not black */
        /* with the HD UI mod the ROM's white fade back is not in the picture (the page is ours), so the ROM is told its fade is over at once, as a
           return from a file page does (chore_rom_writes, b2's pace); without the mod the page IS the ROM's picture, so its own fade runs whole */
        c.ff2_done = pc_assets_ready() ? 0 : 1;
        if (hal_menu_trace_on()) fprintf(stderr, "[chore] face swap: the way back enters once (top 5)\n");
    }
    if (g_face_swap == 2 && c.sw_t < 0) c.face_a = 0;
    /* GLITCHFIX1: THE WAY OUT GOES WHITE WITH THE ROM. Leaving for the file select or the Rec Room, top-state 6 runs the same white brightness fade as the
       face swap (RequestScreenFade(1, 0, ..), rom_swap_level), but the main page with the mod's art is the port's own picture, drawn over the faded one,
       so it stayed in full colour until the cut to the ROM's white page (one picture from mean 159 to 248). The page now takes the ROM's level, tick for tick. */
    c.leave_w = 0;
    if (!g_face_swap && top == 6 && elem == 0) {
        const int lv = rom_swap_level();
        if (lv > 0) c.leave_w = (lv * 255 + 0x800) / 0x1000;
    }
#ifndef PORT_ROM_CLEAN
    { static int tr = -1; if (tr < 0) tr = getenv("SM64DS_GF1_TRACE") ? 1 : 0;     /* GLITCHFIX1 proof instrument */
      if (tr) fprintf(stderr, "[gf1-chore] f%d top=%d elem=%d req=%d sw_t=%d face_a=%d head_t=%d leave_w=%d lv=%d\n", data_ov007_0210342c ? data_ov007_0210342c[0] : -1,
                      top, elem, req, c.sw_t, c.face_a, c.head_t, c.leave_w, rom_swap_level()); }
    { static int tr = -1; if (tr < 0) tr = getenv("SM64DS_LF1_TRACE") ? 1 : 0;     /* LOGOFADE1 proof instrument: the head's entrance state */
      if (tr && data_ov007_0210342c) {
          const char *g = (const char *)data_ov007_0210342c;
          const char *F = *(const char *const *)(g + 0xf4);
          const char *A = F ? *(const char *const *)(F + 0x18) : 0;
          fprintf(stderr, "[lf1] f%d top=%d sw_t=%d head_t=%d g1c=%d g20=%d gec=%d gf8=%d F18=%s c=%d fr=%u n=%u w0=%d\n", data_ov007_0210342c[0], top, c.sw_t,
                  c.head_t, *(const int *)(g + 0x1c), *(const int *)(g + 0x20), *(const int *)(g + 0xec), *(const int *)(g + 0xf8), A ? "y" : "n",
                  A ? *(const short *)(A + 0xc) : 0, A ? *(const unsigned short *)(A + 0x10) : 0u, A ? *(const unsigned short *)(A + 0x12) : 0u, A ? *(const int *)A : 0);
      } }
#endif
    c.prev_variant = variant;
}

/* ROM state the choreography writes (SM64DS_TITLE_FF=0 turns all of it off). Called from menu_title_up. */
/* ASSET6: THE ATTRACT WAITS FOR THE PLAYER. The ROM (src/func_ov007_020b0a20.c) plays the star's flight and leaves the attract by itself
   when a save exists (func_ov007_020aebac == 0): the animation ends and it asks for state 10. With no save it picks the other animation
   (the star idling over TOUCH TO START) and waits. Tango asked for the waiting picture whatever the save, so while the attract is up the
   port shows the ROM the empty-save words (func_ov007_020aebac reads bytes +0xb..+0xd and the word +4 of the record at g+0x28), and puts the
   real words back on the frame the player presses (pad key, click or touch) so the ROM's own gate takes that press. Restored too whenever the
   attract is left. Nothing else reads those words on the attract page. */
static unsigned char g_sv_b[3];
static int g_sv_i;
static int g_sv_faked;
static char *g_sv_q;       /* INT2: the record the words were taken from; the words only go back into that same record (a torn-down title scene never gets a stale write) */
static char *sv_rec()
{
    if (!data_ov007_0210342c) return 0;
    char **pp = *(char ***)((char *)data_ov007_0210342c + 0x28);
    return pp ? *pp : 0;
}
static void sv_fake()
{
    char *q = sv_rec();
    if (!q || g_sv_faked) return;
    memcpy(g_sv_b, q + 0xb, 3);
    g_sv_i = *(int *)(q + 4);
    g_sv_q = q;
    if (!g_sv_b[0] && !g_sv_b[1] && !g_sv_b[2] && !g_sv_i) return;     /* already empty: the ROM waits by itself */
    memset(q + 0xb, 0, 3);
    *(int *)(q + 4) = 0;
    g_sv_faked = 1;
    if (hal_menu_trace_on()) fprintf(stderr, "[chore] attract holds: save words hidden from the ROM until a press\n");
}
static int g_sv_lie;       /* FIELD093K: the record was EMPTY and the words were made non-empty for the press (see sv_press) */
static char *g_sv_lq;
/* FIELD093K: a press on the attract with an EMPTY save (a new player). The ROM's skip of the star flight (src/func_ov007_020b0a20.c: the key / touch edge
   branch) only runs when func_ov007_020aebac() == 0, i.e. when a save exists, so a new player waited the whole 260 tick flight (6 s from the press to the
   wipe) and a pad / key press did nothing at all. The port shows the ROM a non-empty record for the press, exactly what it already does for a played save
   (sv_restore): the ROM then takes its own skip and asks for state 10. The words go back to empty once the attract is left. */
static void sv_press()
{
    char *q = sv_rec();
    if (!q || g_sv_lie) return;
    if (q[0xb] || q[0xc] || q[0xd] || *(int *)(q + 4)) return;     /* a played save: sv_restore gave the real words */
    q[0xb] = 1;
    g_sv_lie = 1;
    g_sv_lq = q;
    if (hal_menu_trace_on()) fprintf(stderr, "[chore] press on an empty save: the ROM sees a save for this press (its flight skip)\n");
}
static void sv_unlie()
{
    if (!g_sv_lie) return;
    if (g_sv_lq && g_sv_lq == sv_rec()) g_sv_lq[0xb] = 0;
    g_sv_lie = 0;
    g_sv_lq = 0;
}
static void sv_restore()
{
    if (!g_sv_faked) return;
    char *q = sv_rec();
    if (q && q == g_sv_q) { memcpy(q + 0xb, g_sv_b, 3); *(int *)(q + 4) = g_sv_i; }
    g_sv_faked = 0;
    g_sv_q = 0;
    if (hal_menu_trace_on()) fprintf(stderr, "[chore] press: save words given back\n");
}
/* INTROCUT1 (Tango, round 18): the press no longer cuts the star's flight at once. The ROM's own flight runs (a touch on the star starts it: src/func_ov007_020b0da0.c, box
   x 0x60..0xa0, y 0x40..0x80; it stops the idle loop sound and starts the flight sound by itself) and CH_CUT ticks after the press, when the star is off the top of the picture,
   the skip is applied: the ROM is shown a save (the real words on a played save, a marker word on an empty one) and a key edge, which is the ROM's own skip of the rest of the flight
   (src/func_ov007_020b0a20.c flag block) and the beat and the wipe follow. The same on a played and an empty save. SM64DS_INTRO_CUT=<ticks> tries another length (dev builds). */
/* One cut length for both saves: a played save is made to look and fly exactly like the empty one (the star's scale is put to the no-save value right after the ROM's
   tick 0, see chore_rom_writes), so the star body leaves at press+88 on either (measured on the native flight, blob under 100 pixels). Tango picked the cut at press+134 on the numbered
   native sheets, after the sparkle trail, 46 ticks after the star body. */
enum { CH_CUT = 134 };
static int g_cut_t = -1;        /* ticks since the press on the attract page (0 on the press tick), -1 = no press */
static int g_cut_fire;          /* the cut is due: the next pre-tick pad hook applies the skip */
static int g_cut_len;           /* the cut length chosen at the press */
static int cut_len_for()
{
    static int len = -1;
    if (len < 0) {
        len = CH_CUT;
#ifndef PORT_ROM_CLEAN
        const char *e = getenv("SM64DS_INTRO_CUT");     /* dev only: another cut length in ticks */
        if (e && *e) len = atoi(e);
#endif
        if (len < 1) len = 1;
    }
    return len;
}
static bool cut_ready() { return g_cut_t < 0 && g_last == V_ATTRACT && g_elem == 0 && g_ch.attract_t >= 6 && ff_on(); }
static void cut_begin()
{
    if (!cut_ready()) return;
    g_cut_t = 0;
    g_cut_fire = 0;
    g_cut_len = cut_len_for();
    if (hal_menu_trace_on()) fprintf(stderr, "[chore] press on the attract (%s save): the ROM's flight runs, the skip comes in %d ticks\n", g_sv_faked ? "a" : "no", g_cut_len);
}
/* a touch / click going into the ROM on the attract page that has not started its flight yet is aimed at the star (the ROM only starts the flight on a touch inside the star's box) */
extern "C" void port_title_press_aim(unsigned char *x, unsigned char *y)
{
    if (!cut_ready()) return;
    *x = 128; *y = 96;
}
extern "C" void port_title_press_seen(void)
{
    cut_begin();
    g_press_tick = 1;
    if (g_ch.in_fam && g_ch.mode == CH_INTRO && g_ch.released && (g_ch.sw_t < swap_end(g_ch) || intro_logo_busy(g_ch))) g_ch.skip = 1;      /* INTRO2: any new click skips the intro too */
}   /* the ROM's gate reads input from its frame 4 on */     /* sub_screen.cpp: a touch / click is going into the ROM this frame */

void chore_rom_writes(int top, int req)
{
    if (!data_ov007_0210342c) return;
    if (top == 11 && g_ch.attract_t >= 0 && !g_sv_faked && g_ch.attract_t < 8 && g_last == V_ATTRACT) {
        sv_fake();
        /* ROUND5: the ROM reads the save record at its attract tick 0 (src/func_ov007_020b0a20.c, mode 0: the star's scale is 0xDAC with a save, 0x1770 with none) and the port can hide the
           words only after that tick has run, so a played save showed the small star and, at the press, its other (popping, twice edge-on) flight. The words are hidden from tick 1 on, so the
           scale (element +0x40 / +0x44 / +0x48 of the attract object, the three words tick 0 sets together) is put to the no-save value by hand here, once, right after tick 0. */
        if (g_sv_faked && data_ov007_0210342c) {
            char *gg = (char *)data_ov007_0210342c;
            char *s100 = *(char **)(gg + 0x100);
            char *el = s100 ? *(char **)(s100 + 4) : 0;
            if (el) {
                *(int *)(el + 0x40) = 0x1770; *(int *)(el + 0x44) = 0x1770; *(int *)(el + 0x48) = 0x1770;
                /* and the animation: beaa0 (src/func_ov007_020beaa0.c) picked resource 0xe with no save and 0xf with one for the two star objects (p0 = 0, 1), the "other flight" */
                for (int p0 = 0; p0 < 2; ++p0) *(void **)(data_ov007_02104bc8 + 8 * p0) = func_ov007_020b3f20(0xe);
                if (hal_menu_trace_on()) fprintf(stderr, "[chore] played save: the star's scale put to the no-save value (0x1770)\n");
            }
        }
    }
    else if (top != 11) { sv_restore(); sv_unlie(); }
    if (top == 11 && g_cut_t >= 0) { if (++g_cut_t >= g_cut_len) g_cut_fire = 1; }
    else if (top != 11) { g_cut_t = -1; g_cut_fire = 0; }
    if (!ff_on()) return;
    char *g = (char *)data_ov007_0210342c;
    char *sp = *(char **)(g + 8);
    if (!sp) return;
    int *t = (int *)(sp + 0xc);
    /* ASSET6: a return from a file page runs the ROM's star iris backwards (top-state 5, 69 frames) over a page we show as a short veil: the
       iris is told it is finished once the veil is up, so the head's arrival (top-state 1) follows at once */
    if (top == 5 && g_ch.mode == CH_RETURN && g_ch.fam_t >= CH_VEIL && !g_ch.ff2_done && data_ov007_02104bd8) {
        char *o6 = *(char **)data_ov007_02104bd8;
        if (o6 && *(int *)(o6 + 8) == 1) {
            const int step = *(int *)(o6 + 0x18);
            const int want = 0x1000 - step + 1;
            if (*(int *)(o6 + 0x14) < want) *(int *)(o6 + 0x14) = want;
            g_ch.ff2_done = 1;
            if (hal_menu_trace_on()) fprintf(stderr, "[chore] return: ROM iris progress finished early\n");
        }
    }
    /* INTRO2: the skip also ends the head's 30 tick entrance (state 1, counter 0..30: src/func_ov007_020b1b74.c hands over to the resting menu past 30) */
    if (top == 1 && g_ch.mode == CH_INTRO && g_ch.skip) { if (*t >= 1 && *t < 9) *t = 9; else if (*t >= 11 && *t < 29) *t = 29; }
    /* ASSET6: leaving a file page the ROM idles 30 frames (state 11, element machine 5) before its star iris closes over the page: skipped once its own 6 frame fade-in is over (an iris asked for while that fade runs is dropped) */
    if (top == 11 && g_elem == 5 && *t >= 9 && *t < 29) { *t = 29; if (hal_menu_trace_on()) fprintf(stderr, "[chore] file page exit: idle counter skipped\n"); }
    if (top == 10 && g_ch.mode == CH_INTRO) {
        /* the ROM's iris starts at t = 60; before it the lower screen is black and nothing is keyed on t after the t = 30 sound */
        /* ASSET6: the star has flown off the top (CH_FLY frames after the press); the counter skips to 29 (the t = 30 sound still plays) and
           runs the ROM's own way after it (INTROVOICE2), so the iris comes at t = 60 */
        if (*t >= ctune(9, CH_FLY) && *t < 29) { *t = 29; if (hal_menu_trace_on()) fprintf(stderr, "[chore] ROM counter t jumped to 29\n"); }
        /* INTRO2: a press skips to the menu: the counter runs on to the tick before the wipe is asked for (t = 60), and the finish below
           then needs no voice wait */
        if (g_ch.skip && *t < 59) *t = 59;
        /* once our iris covers the picture the ROM's iris is finished: progress to its last step so the callback asks for the arrival.
           INTROVOICE2: src/func_ov007_020b10dc.c plays the title voice in two calls of the top-state 10 handler: id 0x2b ("It's-a me") at
           t = 30 and id 0x2d ("Mario!") at t = 128 (Luigi 0x2c at 30 and 0x2e at 122), with the iris whoosh (0x30) at t = 60 between them.
           Our iris covers the picture well before t = 128, so the finish waits until the ROM's own handler has made the second call (t past
           it): the DS order and gaps 0x2b, 0x30, 0x2d, never overlapping. A confirm press (skip) does not wait. */
        if (g_ch.iris_covered && !g_ch.ff2_done && data_ov007_02104bd8) {
            char *o6 = *(char **)data_ov007_02104bd8;
            const int voice_t = (*(int *)(g + 0xec) == 1) ? 0x7a : 0x80;
            int ivis = 0, is48 = 0;
            rom_iris(&ivis, &is48);      /* INTRO2: a skip finishes the iris object only once the wipe itself is up (before it, object 6 is the screen fade) */
            if (o6 && *(int *)(o6 + 8) == 1 && (*t > voice_t || (g_ch.skip && ivis))) {
                const int step = *(int *)(o6 + 0x18);
                const int want = 0x1000 - step + 1;
                if (*(int *)(o6 + 0x14) < want) *(int *)(o6 + 0x14) = want;
                g_ch.ff2_done = 1;
                if (hal_menu_trace_on()) fprintf(stderr, "[chore] ROM iris progress finished early (t %d)\n", *t);
            }
        }
    }
}

static inline float smooth01(float x) { if (x <= 0.f) return 0.f; if (x >= 1.f) return 1.f; return x * x * (3.f - 2.f * x); }

/* the DS logo's bounce, measured (above): offset in canvas units after the pop, tb frames since it (down is positive) */
float logo_bounce(int tb)
{
    if (tb < 0) return 0.f;
    const float amp = 21.f * expf(-(tb - 2.f) / 16.1f);
    const float a = (amp > 21.f ? 21.f : amp) * sinf(6.2831853f * tb / 8.f);
    return -a;
}

/* TITLEUI1: pieces [from, L->np) at alpha a (0..255) on top of their own: a = 255 leaves them, 0 hides them (their touch maps stay) */
void swap_fade_span(MenuLayout *L, int from, int to, int a)      /* TITLEART2: pieces [from, to) only */
{
    if (a >= 255) return;
    for (int i = from; i < to; ++i) {
        MenuPiece &p = L->p[i];
        if (p.nodraw) continue;
        if (a <= 0) { p.nodraw = 1; continue; }
        const int own = p.alpha ? p.alpha : 255;
        const int na = own * a / 255;
        p.alpha = (uint8_t)(na < 1 ? 1 : na);
    }
}
void swap_fade(MenuLayout *L, int from, int a) { swap_fade_span(L, from, L->np, a); }
/* TITLEUI1: one line a tick across the face swap (SM64DS_MENU_TRACE): the ROM's level and the alpha each kind of piece is drawn at (255 = whole, 0 = hidden,
   -2 = no such piece this tick) */
void swap_trace(const MenuLayout *L, const Chore &c, int from, int to)
{
    int wr = -2, help = -2, arr = -2, plank = -2, face = -2;
    for (int i = from; i < L->np; ++i) {
        const MenuPiece &p = L->p[i];
        const int a = p.nodraw ? 0 : p.alpha ? p.alpha : 255;
        if (p.src == SRC_TOP_PIC && p.s.x == 216 && p.s.w == 40 && p.touch) wr = p.nodraw && !p.keycover ? -1 : a;      /* -1: drawn inside the scene layer (face) */
        else if (p.src == SRC_TOP_PIC && p.s.w == 256) face = a;
        else if (p.hostimg == MENU_IMG_ARROW_UP || p.hostimg == MENU_IMG_ARROW_DOWN) arr = a;
        else if (i >= to && p.s.x == ONL_HELP_X && p.s.y == ONL_HELP_Y && p.s.w == ONL_HELP_W && !(p.nodraw && p.touch && a == 0 && help != -2)) help = a;
        else if (i >= to && p.custom) arr = a;
        else if (i < to && p.s.y >= 100 && p.touch && plank == -2) plank = a;
    }
    if (wr == -1) wr = face;
    fprintf(stderr, "[swapfade] f%d level %d swap_a %d face %d wrench %d plank %d help %d arrow %d\n", t_f ? t_f->frame : -1, c.swap_lv, c.swap_a, face, wr, plank, help, arr);
}

/* THE MAIN MENU WITH OUR ART (logo, planks, arrows and wallpaper all loaded). intro = the ROM is not at rest yet (top-state 10 / 1). */
/* FACE1: THE FACE PRESS PAGE (top 4, elem 0). The ROM plays its press animation on the head (it squashes, jumps off the top and lands back)
   while growing it to 1.345 times the main menu's size (the red cap measured 74 DS px wide on the first frame, 99.5 from frame 60 on, centre
   x 123.2 throughout, cap top at layer y 27.8 and 24.2; shots in out/FACE1/shots). Drawing the layer at the main menu's own transform would
   make the head pop up 1.345 times; instead the layer is drawn at the main menu's step divided by that growth (a smoothstep over 56 frames
   fitted to the cap widths) and anchored on the cap, so the head keeps the main menu's size and place for the whole animation and the toy's
   rest pose. Only the layer's own transform changes, so the ROM's squash and jump are still its own. */
float toy_grow(int v)
{
    const float t = v <= 0 ? 0.f : v >= 56 ? 1.f : (float)v / 56.f;
    return t * t * (3.f - 2.f * t);
}

/* the dest rect (host pixels) of the whole 3D layer when its head is where the main menu draws it, at growth (1 + 0.345 * growth_t) of the
   ROM's own press animation (see toy_grow); growth_t 1 = the toy's rest pose, which is also the size the drawing page's strokes were
   traced at (measured: the outline of the drawing page laid over the press page's rest pose follows the head's face, hat to chin) */
void toy_dest(const MenuLayout *L, float growth_t, int *ox, int *oy, int *ow, int *oh)
{
    const FaceK fk = face_k();
    const float k0 = (float)fk.n / (float)fk.d, gr = 1.f + 0.345f * growth_t, k = k0 / gr;
    const float ax = 123.3f, ay = 27.8f + (24.2f - 27.8f) * growth_t;                            /* the layer point that stays on the cap */
    const float cxu = (256.f - 256.f * k0) / 2.f + 123.3f * k0 + (float)face_dx(fk);             /* where the main menu puts it (canvas units) */
    const float cyu = (float)face_dy(fk) + 27.8f * k0;
    const float upx = (float)L->unum / (float)L->uden;
    const float hostx0 = (float)L->w / 2.f + (cxu - ax * k - 128.f) * upx, hosty0 = (cyu - ay * k) * upx;
    const int x0 = (int)(hostx0 + 0.5f), y0 = (int)(hosty0 + 0.5f);
    *ox = x0; *oy = y0;
    *ow = (int)(hostx0 + 256.f * k * upx + 0.5f) - x0; *oh = (int)(hosty0 + 192.f * k * upx + 0.5f) - y0;
}

/* ====================================================================================================================
   TITLE092: THE TITLE'S LOWER SCREEN AS ONE FULL-CANVAS 3D SCENE (Tango, 10-14: "instead of actually moving the UI and things, you just
   cut out that zone and moved it which is lazy"). The lower screen is a 3D scene; the old recipes drew it into the DS 256 x 192 box and cut
   a rect of the finished picture, so whatever the ROM put outside the box (a star above the top, a stroke past the right edge, a spark at
   the side edge of a wide window) was clipped, and a scaled cut was upscaled pixels. Now the scene callback (ntr/gx.h
   gx_set_menu_scene_cb) makes the 3D cover the whole frame and places the scene by a real transform, drawn sharp at the window's own
   resolution: the head, stars and sparkles and the page's models by the scene transform, each sprite object by its own (the wrench pinned
   to the corner, the drawing page's buttons to the edges). The recipe then lays that one layer over the host wallpaper (one piece: the
   whole 3D layer as a unit, keyed by coverage) and keeps only click-mapping twins for the touch areas.
   T = {K, Tx, Ty}: the DS box centre (128, 96) lands at the canvas centre + (Tx, Ty) canvas units, scaled K about it
   (canvas unit = window height / 192; Ty down). {1, 0, 0} is the DS layout. SM64DS_TITLE_WIDE=0 turns the whole thing off (the old cut
   pieces draw); SM64DS_TITLE_SCENE=Kpermille,Tx,Ty tunes the main menu's. */
struct SceneT { float K, Tx, Ty; };
struct SceneNow {              /* what this frame's callback answered; build reads it through TitleFacts */
    int on, variant, frame;
    SceneT T;
    int e3;                    /* the callback's own free-draw tick counter (same rule as the facts' e3) */
} g_sc;
int scene_on_env()
{
    static int v = -1;
    if (v < 0) { const char *e = getenv("SM64DS_TITLE_WIDE"); v = (e && *e == '0') ? 0 : 1; }
    return v;
}
int stune(int idx, int dflt)         /* SM64DS_TITLE_SCENE=Kpermille,Tx,Ty (canvas units, Ty down) */
{
    static int v[3], have = 0;
    if (!have) {
        have = 1;
        for (int i = 0; i < 3; ++i) v[i] = -9999;
        const char *e = getenv("SM64DS_TITLE_SCENE");
        for (int i = 0; e && *e && i < 3; ++i) {
            char *q;
            v[i] = (int)strtol(e, &q, 10);
            if (*q != ',') break;
            e = q + 1;
        }
    }
    return v[idx] == -9999 ? dflt : v[idx];
}
/* the main menu's place: the head 1.05 times the DS size (the old recipe drew it at 5/6, so 26 percent bigger), its cap top at canvas y
   about 12 and its chin about 124 (the old chin was 136): bigger and higher between the small logo and the planks, and the DS screen's top
   edge lands above the canvas top, so the star that falls in from the top of the DS screen enters at the canvas edge */
SceneT scene_main()
{
    SceneT t;
    t.K = (float)stune(0, 1050) / 1000.f;
    t.Tx = (float)stune(1, 0);
    t.Ty = (float)stune(2, -12);
    return t;
}
/* the press page (top 4): the ROM grows the head to 1.345 times; the scene is drawn at the main menu's step divided by that growth and
   anchored on the cap, so the head keeps its size and place (FACE1's rule, as an affine) */
SceneT scene_toy(float growth_t)
{
    const SceneT m = scene_main();
    const float gr = 1.f + 0.345f * growth_t, k = m.K / gr;
    const float ay = 27.8f + (24.2f - 27.8f) * growth_t;
    SceneT t;
    t.K = k;
    t.Tx = m.Tx + (123.3f - 128.f) * (m.K - k);
    t.Ty = m.Ty + (27.8f - 96.f) * m.K - (ay - 96.f) * k;
    return t;
}
/* the drawing page: the strokes were traced at the toy's rest pose; the free-draw state grows the canvas to the DS canvas's own size,
   centred (e3 ticks since it began) */
SceneT scene_doodle(int e3)
{
    SceneT t = scene_toy(1.f);
    if (e3 > 0) {
        const float u = (float)(e3 - 8) / 26.f, g = u <= 0.f ? 0.f : u >= 1.f ? 1.f : u * u * (3.f - 2.f * u);
        t.K = t.K + (1.f - t.K) * g; t.Tx *= (1.f - g); t.Ty *= (1.f - g);
    }
    return t;
}
/* the canvas in units: width, height (window / unit) */
void scene_canvas_units(float *cw, float *ch)
{
    const float ph = (float)(ntr::present_h() > 0 ? ntr::present_h() : 192);
    *cw = (float)ntr::active_w * 192.f / ph;
    *ch = (float)ntr::active_h * 192.f / ph;
}
void scene_xf(const SceneT &t, float cw, float ch, float xf[4])
{
    xf[0] = t.K; xf[1] = 2.f * t.Tx / cw; xf[2] = t.K; xf[3] = -2.f * t.Ty / ch;
}
/* DS box rect -> host pixels under T for a layout (the touch twins of the scene's parts) */
void scene_rect_host(const MenuLayout *L, const SceneT &t, int sx, int sy, int sw, int sh, short *ox, short *oy, short *ow, short *oh)
{
    const float u = (float)L->unum / (float)L->uden;
    const float x0 = (float)L->w * 0.5f + (t.Tx + ((float)sx - 128.f) * t.K) * u, y0 = (float)L->h * 0.5f + (t.Ty + ((float)sy - 96.f) * t.K) * u;
    const float x1 = x0 + (float)sw * t.K * u, y1 = y0 + (float)sh * t.K * u;
    *ox = (short)(x0 + 0.5f); *oy = (short)(y0 + 0.5f);
    *ow = (short)((int)(x1 + 0.5f) - (int)(x0 + 0.5f)); *oh = (short)((int)(y1 + 0.5f) - (int)(y0 + 0.5f));
}
/* the free-draw tick counter, advanced once per ROM frame by the callback (the facts' e3c rule) */
void scene_e3_step(int top, int elem, int frame)
{
    static int last_frame = -1;
    if (frame == last_frame) return;
    last_frame = frame;
    g_sc.e3 = (elem == 3 || (elem == 4 && top == 0 && g_sc.e3 > 0)) ? g_sc.e3 + 1 : 0;
}

/* TITLE092: this tick's 3D is the full-canvas scene (facts), and its transform */
bool scene_now() { return t_f && t_f->wide3d && t_f->sc_on; }
SceneT scene_of_facts() { SceneT t; t.K = t_f->sc_K; t.Tx = t_f->sc_Tx; t.Ty = t_f->sc_Ty; return t; }
/* the whole 3D layer as ONE element (the layer is a unit, the scene the ROM drew, not a rect of a finished screen): the window-sized
   copy of the frame's 3D picture keyed by what the 3D engine wrote */
MenuPiece scene_layer(const MenuLayout *L)
{
    MenuPiece p = menu_piece_make(SRC_TOP_PIC, 0, 0, 256, 192, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    p.fixdest = 1; p.fx = 0; p.fy = 0; p.fw = (short)L->w; p.fh = (short)L->h;
    p.keycover = 1; p.bg = 1;
    return p;
}
/* a click-mapping twin of a DS rect of the scene, placed where the scene puts it */
MenuPiece scene_twin(const MenuLayout *L, const SceneT &t, int sx, int sy, int sw, int sh)
{
    MenuPiece p = menu_piece_make(SRC_TOP_PIC, sx, sy, sw, sh, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    p.fixdest = 1;
    scene_rect_host(L, t, sx, sy, sw, sh, &p.fx, &p.fy, &p.fw, &p.fh);
    p.touch = 1; p.nodraw = 1;
    return p;
}

/* EGG1: THE HEAD'S TOUCH PIECE STEPS OUT FROM UNDER THE SMALL LOGO. The head's touch-only piece is a rectangle (the ROM takes a touch
   anywhere on it) and its left part lies under the small logo, which is drawn above the head (TITLEART2): measured on b3, a click on the
   logo's letters at 4:3 published a DS touch and the ROM went to its face press (top 0 elem 2); at 16:9 the logo's right third is over the
   head's rectangle too. While the secret is live the piece is cut along the logo's own edges into the head's rectangle minus the logo:
   the part right of the logo (full height: the face), and the strips above and under the logo. Each keeps its share of the source
   rectangle (the cut's source column / row is the nearest to the host edge), so a click anywhere they cover still reaches the ROM within
   a DS pixel of where the whole piece sent it (the largest difference is printed under SM64DS_MENU_TRACE). Only when the logo hangs over
   the head's left side; any other overlap is left alone (the click then stays the head's and the clip does not play: egg_click_poll). */
static int egg_inv1(int s0, int sw, int d0, int dw, int b) { const long long n = (long long)(b - d0) * sw; return s0 + (int)(n >= 0 ? n / dw : -((-n + dw - 1) / dw)); }
static void egg_err(int *err, int e) { if (e < 0) e = -e; if (e > *err) *err = e; }
static void egg_trim_head(MenuLayout *L, int hi, int lx, int ly, int lw, int lh)
{
    if (hi < 0 || hi >= L->np) return;
    menu_layout_dest(L);
    const int *d = L->dest[hi];
    const int d0 = d[0], d1 = d[1], d2 = d[2], d3 = d[3];
    const int lx1 = lx + lw, ly1 = ly + lh;
    if (d2 <= 0 || d3 <= 0 || lx1 <= d0 || ly1 <= d1 || lx >= d0 + d2 || ly >= d1 + d3) return;     /* no overlap */
    if (lx > d0 || lx1 >= d0 + d2) {
        if (hal_menu_trace_on()) fprintf(stderr, "[egg] the logo overlaps the head piece (%d,%d,%d,%d) other than over its left side: left as it is\n", d0, d1, d2, d3);
        return;
    }
    const MenuPiece P = L->p[hi];
    const int sw = P.s.w, sh = P.s.h;
    const int kx = (int)(((long long)(lx1 - d0) * sw * 2 + d2) / (2LL * d2));                  /* the source column nearest the logo's right edge */
    if (kx <= 0 || kx >= sw) return;
    const int kyt = ly > d1 ? (int)(((long long)(ly - d1) * sh * 2 + d3) / (2LL * d3)) : 0;
    const int kyb = ly1 < d1 + d3 ? (int)(((long long)(ly1 - d1) * sh * 2 + d3) / (2LL * d3)) : sh;
    MenuPiece r = P;
    r.fixdest = 1; r.fx = (short)lx1; r.fy = (short)d1; r.fw = (short)(d0 + d2 - lx1); r.fh = (short)d3;
    r.s.x = (short)(P.s.x + kx); r.s.w = (short)(sw - kx);
    L->p[hi] = r;
    int err = 0;
    for (int bx = lx1; bx < d0 + d2; ++bx) egg_err(&err, egg_inv1(r.s.x, r.s.w, lx1, r.fw, bx) - egg_inv1(P.s.x, sw, d0, d2, bx));
    for (int bx = d0; bx < lx1; ++bx) egg_err(&err, egg_inv1(P.s.x, kx, d0, lx1 - d0, bx) - egg_inv1(P.s.x, sw, d0, d2, bx));
    if (kyt > 0 && ly > d1) {
        MenuPiece t = P;
        t.fixdest = 1; t.fx = (short)d0; t.fy = (short)d1; t.fw = (short)(lx1 - d0); t.fh = (short)(ly - d1);
        t.s.w = (short)kx; t.s.h = (short)kyt;
        menu_add_piece(L, t);
        for (int by = d1; by < ly; ++by) egg_err(&err, egg_inv1(t.s.y, t.s.h, d1, t.fh, by) - egg_inv1(P.s.y, sh, d1, d3, by));
    }
    if (kyb < sh && ly1 < d1 + d3) {
        MenuPiece b = P;
        b.fixdest = 1; b.fx = (short)d0; b.fy = (short)ly1; b.fw = (short)(lx1 - d0); b.fh = (short)(d1 + d3 - ly1);
        b.s.w = (short)kx; b.s.y = (short)(P.s.y + kyb); b.s.h = (short)(sh - kyb);
        menu_add_piece(L, b);
        for (int by = ly1; by < d1 + d3; ++by) egg_err(&err, egg_inv1(b.s.y, b.s.h, ly1, b.fh, by) - egg_inv1(P.s.y, sh, d1, d3, by));
    }
    if (hal_menu_trace_on()) {
        static int last[6];
        const int now[6] = { d0, d1, d2, d3, lx1, ly1 };
        if (memcmp(last, now, sizeof now)) {
            memcpy(last, now, sizeof now);
            fprintf(stderr, "[egg] head piece (%d,%d,%d,%d) cut around the logo (%d,%d,%d,%d): src column %d, rows %d / %d, largest DS difference %d px\n",
                    d0, d1, d2, d3, lx, ly, lw, lh, P.s.x + kx, P.s.y + kyt, P.s.y + kyb, err);
        }
    }
}

/* GLITCHFIX1: the leave's white fade over the finished page (Chore::leave_w): every pixel moves toward white by the piece's alpha, as the DS's
   brightness-up fade moves each colour toward white by its level */
void white_veil(const MenuLayout *L, int piece, uint32_t *dst, int w, int h, int stride)
{
    const unsigned a = L->p[piece].alpha;
    if (!a) return;
    unsigned char lut[256];
    for (unsigned v = 0; v < 256; ++v) lut[v] = (unsigned char)(v + ((255u - v) * a + 127u) / 255u);
    for (int y = 0; y < h; ++y) {
        uint32_t *row = dst + (size_t)y * stride;
        for (int x = 0; x < w; ++x) {
            const uint32_t o = row[x];
            row[x] = (o & 0xff000000u) | ((uint32_t)lut[(o >> 16) & 255] << 16) | ((uint32_t)lut[(o >> 8) & 255] << 8) | lut[o & 255];
        }
    }
}

void build_main_pc(MenuLayout *L, bool intro)
{
    Chore c = t_f ? t_f->ch : Chore();      /* a copy: build() never writes the clock */
    const int w = L->w, h = L->h, un = L->unum, ud = L->uden;
    L->backdrop = MENU_BACKDROP_TOP_AS_IS;
    L->stage = MENU_STAGE_POST_FADE;
    L->no_focus_paint = 1;
    auto U = [&](int units) { return units * un / ud; };
    auto Uf = [&](float units) { return (int)(units * (float)un / (float)ud + (units < 0 ? -0.5f : 0.5f)); };
    /* the wallpaper: gradient plus the PC pattern at the OG's scale (64 units a tile wide) */
    {
        MenuPiece wl = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        wl.bg = 1; wl.wall = 1; wl.lden = 1; wl.lnum = 1; wl.fx = 0; wl.fy = 0; wl.fw = (short)w; wl.fh = (short)h;
        const int tu = ctune(5, 64);
        wl.wtw = (short)(tu * un / ud);
        wl.wth = (short)((int)((long long)wl.wtw * 730 / 410));
        wl.wox = (short)((ctune(6, -6) * un) / ud);
        wl.woy = (short)((ctune(7, -11) * un) / ud);
        wl.wink = (uint8_t)ctune(8, 107);
        menu_add_piece(L, wl);
    }
    menu_online_add_tint(L, onl_facts_of(L));      /* NAME1: the online mode's green over the sky and the pattern, before anything else is drawn */
    {
        /* MENUS093C (D23): on the face press page the DS lower screen's wallpaper fades to the drawing paper's cream while the head jumps
           (measured on the cartridge: still blue 8 ticks after the touch, lavender at 20, beige at 32, cream by 44). Ours stayed blue
           until the hand-over and then crossfaded in 18 ticks. SM64DS_PRESS_PAPER=0 keeps the blue page. */
        static const int pp_on = [] { const char *e = getenv("SM64DS_PRESS_PAPER"); return !(e && *e == '0'); }();
        const int tvp = t_f ? t_f->toy_v - 1 : -1;
        if (pp_on && tvp >= 8 && !intro) {
            int a = tvp >= 42 ? 255 : 255 * (tvp - 8) / 34;
            MenuPiece pp = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
            pp.bg = 1; pp.paper = 1; pp.lden = 1; pp.lnum = 1; pp.fx = 0; pp.fy = 0; pp.fw = (short)w; pp.fh = (short)h;
            pp.alpha = (uint8_t)(a >= 255 ? 0 : (a < 1 ? 1 : a));
            menu_add_piece(L, pp);
        }
    }
    int iw = 1, ih = 1;
    menu_hostimg_size(MENU_IMG_LOGO, &iw, &ih);
    /* the logos: big (centred, as the main menu used to have it) and small (top left) */
    int big_a = 0, small_a = 0;
    float dy = 0.f, mv = 0.f;
    if (c.mode == CH_INTRO) {
        const int tb = c.ct - ctune(0, CH_LOGO_POP);
        if (c.ct >= 0 && tb >= 0) {
            big_a = tb >= 3 ? 255 : 255 * (tb + 1) / 4;
            dy = logo_bounce(tb);
        }
        if (c.sw_t >= 0) {
            /* INTRO2 (Tango 10-24): the big logo travels to the top left and shrinks to the small one's size and place over CH_LOGO_TRIP ticks (smoothstep; 2 s until LOGOSPEED1):
               one logo that becomes the small one.
               LOGOFADE1: the whole hand-over runs on ONE clock, the head's arrival (Chore::head_t): the big logo stays solid and in place until the
               head has arrived, then travels. It stays solid on the way (a see-through logo over the head is the clipping Tango reported), and on
               the tick it lands it IS the small logo, same size and place (add_logos: mv 1 gives the small one's U(ctune(2..4)) box), so the
               small logo takes over on that tick: two logos are never on the picture at once. */
            const float u = c.head_t < 0 ? 0.f : c.head_t >= CH_LOGO_TRIP ? 1.f : (float)c.head_t / (float)CH_LOGO_TRIP;
            mv = smooth01(u);
            if (u >= 1.f) { big_a = 0; small_a = 255; }
        }
    } else {
        small_a = 255;
    }
    if (c.skip) { big_a = 0; small_a = 255; }
    /* TITLEART2: the PC logo is always above the head. On the intro the head fades in (from the swap tick, about t341 of the real route) while the
       big logo still hovers centred; with the logo pieces added before the face piece any face alpha above 0 drew the head over the logo, the hat
       first. The logos are now added after the face, the head's touch map and the wrench, and before the planks, and the face swap's fade skips them
       as it did before (they were outside its span). */
    int egg_small = -1;      /* EGG1: the small logo piece */
#ifndef PORT_ROM_CLEAN
    { static const int nl = getenv("SM64DS_LF1_NOLOGO") ? 1 : 0; if (nl) { big_a = 0; small_a = 0; } }     /* LOGOFADE1 proof instrument: the picture under the logos */
#endif
    auto add_logos = [&]() {
        if (big_a > 0) {
            const int lt = ltune(0, 26), lh = ltune(1, 112), ldx = ltune(2, 0);
            const int dhb = U(lh), dwb = (int)((long long)dhb * iw / ih);
            const int dhs = U(ctune(2, 40)), dws = (int)((long long)dhs * iw / ih);
            const int dh = dhb + (int)((float)(dhs - dhb) * mv), dw = (int)((long long)dh * iw / ih);
            const int fx0 = (w - dwb) / 2 + U(ldx), fx1 = U(ctune(3, 6)), fy0 = U(lt) + Uf(dy), fy1 = U(ctune(4, 6));
            (void)dws;
            MenuPiece lg = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
            lg.bg = 1; lg.hostimg = MENU_IMG_LOGO; lg.nofade = 1; lg.alpha = (uint8_t)(big_a >= 255 ? 0 : big_a);
            lg.lden = 1; lg.lnum = 1; lg.fw = (short)dw; lg.fh = (short)dh;
            lg.fx = (short)(fx0 + (int)((float)(fx1 - fx0) * mv)); lg.fy = (short)(fy0 + (int)((float)(fy1 - fy0) * mv));
            const int ibig = menu_add_piece(L, lg);
            /* LOGOFADE1: on its trip the logo glides on every picture: the present draws a picture between ticks part of the way from its rect on
               the tick before (the same smoothstep one head_t earlier) to this tick's (MenuLayout glide, ARROWSLOW1's in-between fraction) */
            if (c.mode == CH_INTRO && !c.skip && c.head_t >= 1 && c.head_t < CH_LOGO_TRIP && ibig >= 0) {
                const float mp = smooth01((float)(c.head_t - 1) / (float)CH_LOGO_TRIP);
                const int dhp = dhb + (int)((float)(dhs - dhb) * mp), dwp = (int)((long long)dhp * iw / ih);
                L->glide_on = 1; L->glide_piece = (uint8_t)ibig;
                L->glide_from[0] = (short)(fx0 + (int)((float)(fx1 - fx0) * mp)); L->glide_from[1] = (short)(fy0 + (int)((float)(fy1 - fy0) * mp));
                L->glide_from[2] = (short)dwp; L->glide_from[3] = (short)dhp;
            }
        }
        if (small_a > 0) {
            const int lh = ctune(2, 40), mx = ctune(3, 6), my = ctune(4, 6);
            const int dh = U(lh), dw = (int)((long long)dh * iw / ih);
            MenuPiece lg = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
            lg.bg = 1; lg.hostimg = MENU_IMG_LOGO; lg.nofade = 1; lg.alpha = (uint8_t)(small_a >= 255 ? 0 : small_a);
            lg.lden = 1; lg.lnum = 1; lg.fw = (short)dw; lg.fh = (short)dh;
            lg.fx = (short)U(mx); lg.fy = (short)U(my);
            egg_small = menu_add_piece(L, lg);
        }
    };
    /* EGG1: the head's touch piece steps out from under the small logo (egg_trim_head) once the logo is fully in, off the face press */
    auto egg_cut_head = [&](int ihd) {
        if (small_a < 255 || (t_f && t_f->toy_v > 0) || !egg_page_ok(L, intro)) return;
        const int edh = U(ctune(2, 40)), edw = (int)((long long)edh * iw / ih);      /* the small logo's place, as add_logos puts it */
        egg_trim_head(L, ihd, U(ctune(3, 6)), U(ctune(4, 6)), edw, edh);
    };
    if (intro) add_logos();
    const int np_swap = L->np;      /* TITLEUI1: the pieces from here to the overlays are the lower screen's: the face swap fades them (swap_fade) */
    if (!intro) {
        /* the face (the whole lower 3D layer minus the wrench corner, round 4's piece) at the swap's alpha */
        const FaceK fk = face_k();
        const int hk = fk.n, hd = fk.d;
        const int tv = t_f ? t_f->toy_v - 1 : -1;      /* FACE1: >= 0 on the face press page */
        const bool toy = tv >= 0;
        const int fa = toy ? 255 : c.face_a;
        if (scene_now()) {
            /* TITLE092: the head, stars, sparkles and wrench are one 3D layer the scene already placed (bigger, higher, unclipped); the
               pieces below are only click maps */
            const SceneT T = scene_of_facts();
            if (toy && tv > 93) {
                /* 3b: from counter 94 the ROM dims the lower screen for the hand-over; its dim lands on the 3D pixels but not on our wallpaper, so
                   the head went dark against a bright page (the flash). The bright frame held at counter 93 stays instead. */
                MenuPiece sn = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
                sn.bg = 1; sn.snap = 1; sn.lden = 1; sn.lnum = 1; sn.fx = 0; sn.fy = 0; sn.fw = (short)L->w; sn.fh = (short)L->h;
                menu_add_piece(L, sn);
            } else if (fa > 0) {
                MenuPiece sc = scene_layer(L);
                sc.alpha = (uint8_t)(fa >= 255 ? 0 : fa);
                menu_add_piece(L, sc);
            }
            if (toy) {
                if (tv <= 93) L->snap_save = 1;      /* FACE2: the held frame of the crossfade into the drawing page */
                menu_add_piece(L, scene_twin(L, T, 0, 0, 256, 192));        /* the page reads touches over the whole layer */
            } else {
                egg_cut_head(menu_add_piece(L, scene_twin(L, T, 20, 0, 156, 134)));      /* EGG1: the live head (the scene's twin) */
                MenuPiece wr = menu_piece_make(SRC_TOP_PIC, 216, 0, 40, 40, MENU_AX_R, MENU_AY_T, wrench_dx(L), 0, 1, 1);
                wr.touch = 1; wr.nodraw = 1;
                const int iwr = menu_add_piece(L, wr);
                static const int wr_ids[1] = { 7 };
                hot(L, iwr, wr_ids, 1);
            }
        } else
        if (toy) {
            MenuPiece face = menu_piece_make(SRC_TOP_PIC, 0, 0, 256, 192, MENU_AX_C, MENU_AY_T, 0, 0, 1, 1);
            face.fixdest = 1; face.keycover = 1; face.bg = 1; face.has_ex = 1; face.ex.x = 216; face.ex.y = 0; face.ex.w = 40; face.ex.h = 40;
            int x0, y0, fw, fh;
            toy_dest(L, toy_grow(tv), &x0, &y0, &fw, &fh);
            face.fx = (short)x0; face.fy = (short)y0; face.fw = (short)fw; face.fh = (short)fh;
            menu_add_piece(L, face);
            if (tv <= 93) L->snap_save = 1;      /* FACE2: the held frame of the crossfade into the drawing page (the ROM's own fade-out dims the picture from counter 94) */
            MenuPiece head = face;                      /* the page reads touches over the whole layer; the same transform inverts them */
            head.keycover = 0; head.has_ex = 0; head.bg = 0; head.touch = 1; head.nodraw = 1;
            menu_add_piece(L, head);
            {
                /* the wrench leaves with the planks */
                const int wa = tv >= 10 ? 0 : 255 - 255 * tv / 10;
                if (wa > 0) {
                    MenuPiece wr = menu_piece_make(SRC_TOP_PIC, 216, 0, 40, 40, MENU_AX_R, MENU_AY_T, wrench_dx(L), 0, 1, 1);
                    wr.keycover = 1; wr.alpha = (uint8_t)(wa >= 255 ? 0 : wa);
                    menu_add_piece(L, wr);
                }
            }
        } else {
        if (fa > 0) {
            MenuPiece face = menu_piece_make(SRC_TOP_PIC, 0, 0, 256, 192, MENU_AX_C, MENU_AY_T, face_dx(fk), face_dy(fk), hk, hd);
            face.keycover = 1; face.bg = 1; face.has_ex = 1; face.ex.x = 216; face.ex.y = 0; face.ex.w = 40; face.ex.h = 40;
            face.alpha = (uint8_t)(fa >= 255 ? 0 : fa);
            menu_add_piece(L, face);
        }
        MenuPiece head = menu_piece_make(SRC_TOP_PIC, 20, 0, 156, 134, MENU_AX_C, MENU_AY_T, face_dx(fk) - (30 * hk + hd / 2) / hd, face_dy(fk), hk, hd);
        head.touch = 1; head.nodraw = 1;
        egg_cut_head(menu_add_piece(L, head));      /* EGG1 */
        MenuPiece wr = menu_piece_make(SRC_TOP_PIC, 216, 0, 40, 40, MENU_AX_R, MENU_AY_T, wrench_dx(L), 0, 1, 1);
        wr.keycover = 1; wr.touch = 1; wr.alpha = (uint8_t)(fa >= 255 ? 0 : fa);
        wr.nodraw = fa <= 0;
        {
            const int iwr = menu_add_piece(L, wr);
            static const int wr_ids[1] = { 7 };
            hot(L, iwr, wr_ids, 1);
        }
        }
        const int np_logo = L->np;
        add_logos();
        const int np_logo_end = L->np;
        /* the planks, each rising from the bottom edge with an overshoot, staggered */
        static const int kPlateId[3] = { 1, 0, 2 };
        static const int kPlankImg[3] = { MENU_IMG_BTN_MP, MENU_IMG_BTN_ADV, MENU_IMG_BTN_REC };
        const int cur = t_f ? t_f->cur : -1, prs = t_f ? t_f->prs : 0;
        const bool landed = c.sw_t >= planks_end(c) - 4 || c.skip;
        for (int k = 0; k < 3; ++k) {
            Box bx;
            if (!box_of(kPlateId[k], &bx)) continue;
            const int pt = c.sw_t - (plank_start(c) + CH_PLANK_GAP * k);
            if (!toy && (c.sw_t < 0 || (pt < 0 && !c.skip))) continue;
            float rise = 0.f;                                       /* canvas units below the resting place */
            int plank_a = 255;
            if (toy) {                                              /* FACE1: the planks leave downward, fading, as the head starts to squash */
                const int u = tv - 2 * k;
                if (u >= 14) continue;
                if (u > 0) { const float e = (float)u / 14.f; rise = e * e * 72.f; plank_a = 255 - 255 * u / 14; }
            } else if (!c.skip && pt < CH_PLANK_DUR) rise = (1.f - ease_out_back((float)pt / CH_PLANK_DUR)) * 72.f;
            int w1000 = 1000;
            if (online_plank(L, k, bx, Uf(rise), plank_a, landed && cur == kPlateId[k], landed && cur == kPlateId[k] && prs, !toy, &w1000)) continue;   /* NAME1 */
            int pw = 1, ph = 1;
            menu_hostimg_size(kPlankImg[k], &pw, &ph);
            const int bh = bx.y1 - bx.y0;
            const int dh = bh * un / ud, dw0 = (int)((long long)dh * pw / ph);
            const int dw = dw0 * w1000 / 1000 < 1 ? 1 : dw0 * w1000 / 1000;     /* NAME1: the turn */
            const int cxu = (bx.x0 + bx.x1) / 2 - 128, cyu2 = bx.y0 + bx.y1;
            MenuPiece pl = menu_piece_make(SRC_HOSTIMG, bx.x0, bx.y0, bx.x1 - bx.x0, bx.y1 - bx.y0, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
            pl.hostimg = (uint8_t)kPlankImg[k]; pl.touch = toy ? 0 : 1;
            pl.lden = 1; pl.lnum = 1; pl.fw = (short)dw; pl.fh = (short)dh;
            pl.fx = (short)(w / 2 + cxu * un / ud - dw / 2);
            pl.fy = (short)(cyu2 * un / (2 * ud) - dh / 2 + Uf(rise));
            if (plank_a < 255) pl.alpha = (uint8_t)(plank_a < 1 ? 1 : plank_a);
            if (landed && cur == kPlateId[k]) pl.hl = (uint8_t)(1 | (prs ? 2 : 0));
            const int ip = menu_add_piece(L, pl);
            if (!toy) menu_add_hotspot(L, ip, bx.x0, bx.y0, bx.x1 - bx.x0, bx.y1 - bx.y0, -1);
        }
        if (c.swap_a >= 0) { swap_fade_span(L, np_swap, np_logo, c.swap_a); swap_fade(L, np_logo_end, c.swap_a); }      /* TITLEUI1: the face, the wrench and the planks take the ROM's fade level (TITLEART2: not the logos) */
        const int np_arrow = L->np;
        /* the selection arrow, once the planks have landed */
        const OnlFacts *pfl = onl_facts_of(L);      /* PADNAV1: no cursor arrow here while the pad focus is on the ? */
        if (landed && !(pfl && pfl->padfocus) && menu_hostimg_ready(MENU_IMG_ARROW_UP) && menu_hostimg_ready(MENU_IMG_ARROW_DOWN) && (cur == 7 || (cur >= 0 && cur <= 2))) {
            const int fr = t_f ? t_f->frame : 0;                       /* ARROWSLOW1: the bob is arrow_bob_place's */
            const int aimg = cur == 7 ? MENU_IMG_ARROW_UP : MENU_IMG_ARROW_DOWN;
            int aw = 1, ah = 1;
            menu_hostimg_size(aimg, &aw, &ah);
            const int au = ltune(3, 18);
            const int dw = au * un / ud, dh = (int)((long long)dw * ah / aw);
            MenuPiece ar = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
            ar.bg = 1; ar.hostimg = (uint8_t)aimg; ar.lden = 1; ar.lnum = 1; ar.fw = (short)dw; ar.fh = (short)dh;
            if (cur == 7) {
                Box wb;
                if (!box_of(7, &wb)) wb = kBoxes[7];
                ar.fx = (short)(w - (256 - (wb.x0 + wb.x1) / 2 - wrench_dx(L)) * un / ud - dw / 2);
                arrow_bob_place(ar, wb.y1 * 2 + 2, 0, un, ud, fr);
                menu_add_piece(L, ar);
            } else {
                Box bx;
                if (box_of(cur, &bx)) {
                    arrow_bob_place(ar, bx.y0 * 2 - 2, dh, un, ud, fr);
                    ar.fx = (short)(w / 2 + ((bx.x0 + bx.x1) / 2 - 128) * un / ud - dw / 2);
                    menu_add_piece(L, ar);
                }
            }
        }
        if (c.swap_a >= 0) swap_fade(L, np_arrow, c.swap_a);      /* TITLEUI1: and the arrow */
    }
    egg_note(L, egg_small, small_a >= 255 && !(t_f && t_f->toy_v > 0) && egg_page_ok(L, intro), nullptr);      /* EGG1: once the small logo is fully in */
    if (!intro) {                                              /* NAME1: the ? button (online planks, settled) and the modal screen */
        const OnlFacts *of = onl_facts_of(L);
        const bool settled = (c.sw_t >= swap_end(c) - 4 || c.skip || c.mode == CH_NONE) && !intro_logo_busy(c);
        const int np_ov = L->np;
        online_overlays(L, true, of && of->on && settled && !(t_f && t_f->toy_v > 0));
        if (c.swap_a >= 0 && !(of && of->scr != ONL_SCR_NONE)) swap_fade(L, np_ov, c.swap_a);      /* TITLEUI1: the ? and its arrow too */
        if (c.swap_a >= 0 && hal_menu_trace_on()) swap_trace(L, c, np_swap, np_ov);
    }
    /* the iris: black outside the star while the ROM's iris is up (and all black before it shows), a veil on a returning page */
    if (c.mode == CH_INTRO && !c.iris_covered && c.ct <= ctune(0, CH_LOGO_POP) + 60) {
        const int vis = t_f ? t_f->vis : 0, s48 = t_f ? t_f->s48 : 0;
        MenuPiece ir = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        ir.bg = 1; ir.iris = 1; ir.lden = 1; ir.lnum = 1; ir.fx = 0; ir.fy = 0; ir.fw = (short)w; ir.fh = (short)h;
        float Rds = 0.f;
        if (vis && c.ct >= 0) Rds = 0.004382f * (float)s48;
        if (c.ct >= 0 && !vis) c.iris_covered = 1;
        /* INTRO2 (Tango 10-24): the wipe is the ROM's own (the fade manager's star sprite, raised at ROM counter t = 60 by src/func_ov007_020b10dc.c
           with the whoosh), about half a second after the star has left the top: black until the ROM's sprite shows, then its radius. The early
           synthetic star of TITLE092 / INTROCARD092 (3a) is gone. */
        const float Rpx = Rds * (float)h / 192.f;
        ir.icx = w / 2;
        ir.icy = (int)((96.f + 0.0864f * Rds) * (float)h / 192.f + 0.5f);
        ir.ir16 = (int)(Rpx * 16.f);
        if (c.iris_covered || (Rpx > 0.f && menu_iris_covers(w, h, (float)ir.icx, (float)ir.icy, Rpx))) { c.iris_covered = 1; g_iris_cov_req = 1; }
        else menu_add_piece(L, ir);
    } else if (c.mode == CH_RETURN && c.fam_t < 14) {
        MenuPiece ir = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        ir.bg = 1; ir.iris = 1; ir.lden = 1; ir.lnum = 1; ir.fx = 0; ir.fy = 0; ir.fw = (short)w; ir.fh = (short)h;
        ir.ir16 = 0;
        ir.alpha = (uint8_t)(255 * (14 - c.fam_t) / 14);      /* TITLE092 (3b): the page fades IN from the black of the ROM's closing iris (the veil used to grow darker and drop away: the dark flash) */
        if (ir.alpha == 0) ir.alpha = 1;
        menu_add_piece(L, ir);
    }
    if (c.leave_w > 0) {      /* GLITCHFIX1: the way out to the file select / Rec Room whitens with the ROM's own fade, over everything */
        MenuPiece wv = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        wv.bg = 1; wv.fixdest = 1; wv.fx = 0; wv.fy = 0; wv.fw = (short)w; wv.fh = (short)h;
        wv.custom = white_veil;
        wv.alpha = (uint8_t)(c.leave_w > 255 ? 255 : c.leave_w);
        menu_add_piece(L, wv);
    }
}

void build_main_sel(MenuLayout *L, bool intro)
{
    if (menu_hostimg_ready(MENU_IMG_LOGO) && menu_hostimg_ready(MENU_IMG_BTN_MP) && menu_hostimg_ready(MENU_IMG_BTN_ADV) &&
        menu_hostimg_ready(MENU_IMG_BTN_REC) && menu_hostimg_ready(MENU_IMG_ARROW_UP) && menu_hostimg_ready(MENU_IMG_ARROW_DOWN) &&
        menu_hostimg_ready(MENU_IMG_PATTERN) && menu_hostimg_ready(MENU_IMG_STAR))
        build_main_pc(L, intro);
    else
        build_main(L, intro);
}

/* FACE2: THE DRAWING PAGE (top 0..2 / elem 1, then elem 2: Mario traces his face, then the player draws). Built from pieces:
     the held main menu frame, crossfaded out while the paper fades in (the first DD_XF ticks after the press page hands over),
     the paper (the ROM's own tile, edge to edge, no side bars),
     the strokes: the 3D layer's pixels, keyed by coverage, at the SAME size and place as the head on the main menu (toy_dest at the
       toy's rest pose; the strokes were traced over that pose), so the outline lands on the head it replaces; a drag on it maps through
       the same transform to the ROM's canvas,
     the Yoshi button and the pencil, cut from the same layer by their own boxes and anchored to the left and right edges at their DS rows
       (4:3: exactly where the DS puts them; wide: the same offsets from the edges), each its own touch piece in the ROM's own box. */
enum { DD_XF = 18 };       /* the crossfade, in DS ticks: 0.3 s */
enum { DD_HF = 120 };      /* MENUS093C (D23): the held face frame fades out into the paper over this many ticks while the strokes are drawn over it (the DS fades the 3D head
                              out over about 2 s: measured on the cartridge, the head is still half there 100 ticks after the drawing page is live) */

/* FACE3: the free-draw page's paper: 0 = the drawing page's cream paper stays, 1 = the page's own green paper fades in (SM64DS_PAPER3=0/1) */
int paper3_mode()
{
    static int m = -1;
    if (m < 0) { const char *e = getenv("SM64DS_PAPER3"); m = (e && *e == '0') ? 0 : 1; }
    return m;
}

void build_doodle(MenuLayout *L)
{
    const int w = L->w, h = L->h;
    L->backdrop = MENU_BACKDROP_TOP_AS_IS;
    L->stage = MENU_STAGE_POST_FADE;
    L->no_focus_paint = 1;
    const bool live = !(t_f && t_f->dd_dead);
    /* TITLE092 (3b): the crossfade from the held face frame to the drawing page runs from the moment the page IS live (the ROM's own
       hand-over frames are black and its brightness fade dims the picture): until then the held bright frame stays, whole; then the paper
       and the page's pieces fade in over it. Before, the fade ran over the dead frames, so the paper was bare for a while and the strokes
       and buttons then appeared at once. */
    const int e = (t_f && t_f->dd_e > 0) ? (live ? (t_f->dd_live > 0 ? t_f->dd_live : 1) : 0) : 1000;
    const bool closing = t_f && t_f->vis && (t_f->e3 > 0 || t_f->yh);    /* FACE3: the Yoshi press: the ROM closes its star iris (polygons in the 3D layer, drawn only in the 4:3 area) */
    if (live && e >= DD_HF && !closing && !(t_f && t_f->yh)) L->snap_save = 1;      /* the held frame of the way out (and of the iris: the last frame before the ROM's iris shows) */
    if (e < DD_HF) {
        MenuPiece sn = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        sn.bg = 1; sn.snap = 1; sn.lden = 1; sn.lnum = 1; sn.fx = 0; sn.fy = 0; sn.fw = (short)w; sn.fh = (short)h;
        menu_add_piece(L, sn);
    }
    if (e > 0) {
        MenuPiece pp = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        pp.bg = 1; pp.paper = 1; pp.lden = 1; pp.lnum = 1; pp.fx = 0; pp.fy = 0; pp.fw = (short)w; pp.fh = (short)h;
        const int a = e >= DD_HF ? 255 : 255 * e / DD_HF;
        pp.alpha = (uint8_t)(a >= 255 ? 0 : (a < 1 ? 1 : a));
        menu_add_piece(L, pp);
    }
    const int e3 = t_f ? t_f->e3 : 0;      /* FACE3: the free-draw state (the ROM's element machine 3, after the pencil) */
    if (e3 > 0 && paper3_mode() == 1) {    /* the page's own green paper fades in over the cream one (0.3 s) */
        MenuPiece pg = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        pg.bg = 1; pg.paper = 2; pg.lden = 1; pg.lnum = 1; pg.fx = 0; pg.fy = 0; pg.fw = (short)w; pg.fh = (short)h;
        const int a = e3 >= DD_XF ? 255 : 255 * e3 / DD_XF;
        pg.alpha = (uint8_t)(a >= 255 ? 0 : (a < 1 ? 1 : a));
        menu_add_piece(L, pg);
    }
    const int e3o = t_f ? t_f->e3o : 0;
    if (e3 == 0 && e3o > 0 && e3o < DD_XF && paper3_mode() == 1) {    /* the hand button: the green paper fades back to the drawing page's cream (0.3 s) */
        MenuPiece pg = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        pg.bg = 1; pg.paper = 2; pg.lden = 1; pg.lnum = 1; pg.fx = 0; pg.fy = 0; pg.fw = (short)w; pg.fh = (short)h;
        const int a = 255 - 255 * e3o / DD_XF;
        pg.alpha = (uint8_t)(a < 1 ? 1 : a);
        menu_add_piece(L, pg);
    }
    if (closing && live) {      /* the page is frozen under our own iris: the held frame, then the star over the whole canvas (same star, same radius as the ROM's) */
        MenuPiece sn = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        sn.bg = 1; sn.snap = 1; sn.lden = 1; sn.lnum = 1; sn.fx = 0; sn.fy = 0; sn.fw = (short)w; sn.fh = (short)h;
        menu_add_piece(L, sn);
        MenuPiece ir = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        ir.bg = 1; ir.iris = 1; ir.lden = 1; ir.lnum = 1; ir.fx = 0; ir.fy = 0; ir.fw = (short)w; ir.fh = (short)h;
        const float Rds = 0.004382f * (float)t_f->s48, Rpx = Rds * (float)h / 192.f;
        ir.icx = w / 2;
        ir.icy = (int)((96.f + 0.0864f * Rds) * (float)h / 192.f + 0.5f);
        ir.ir16 = (int)(Rpx * 16.f);
        if (!(Rpx > 0.f && menu_iris_covers(w, h, (float)ir.icx, (float)ir.icy, Rpx))) menu_add_piece(L, ir);
        return;
    }
    if (!live) return;
    static const DsRect kYo = { 0, 132, 60, 60 }, kPen = { 196, 132, 60, 60 }, kBar = { 84, 168, 88, 24 };
    if (scene_now()) {
        /* TITLE092: the strokes, the Yoshi button, the pencil and the ink bar are one 3D layer the scene placed (the strokes at the head's
           size and place, the buttons pinned to the canvas edges at their DS rows, nothing clipped at the DS box); only click maps here */
        const SceneT T = scene_of_facts();
        MenuPiece sc = scene_layer(L);
        if (e < DD_XF) { const int a = 255 * e / DD_XF; sc.alpha = (uint8_t)(a < 1 ? 1 : a); }      /* 3b: the page's own parts fade in with the paper, they do not pop */
        menu_add_piece(L, sc);
        const int th = 192;           /* INTROCARD092: the whole DS stylus surface (the buttons are added after it and win the hit); it ended at row 130 / 146, so a drag stopped at about half the window */
        menu_add_piece(L, scene_twin(L, T, 0, 0, 256, th));
        MenuPiece yo = menu_piece_make(SRC_TOP_PIC, kYo.x, kYo.y, kYo.w, kYo.h, MENU_AX_L, MENU_AY_B, 0, 0, 1, 1);
        yo.touch = 1; yo.nodraw = 1;
        menu_add_piece(L, yo);
        MenuPiece pe = menu_piece_make(SRC_TOP_PIC, kPen.x, kPen.y, kPen.w, kPen.h, MENU_AX_R, MENU_AY_B, 0, 0, 1, 1);
        pe.touch = 1; pe.nodraw = 1;
        menu_add_piece(L, pe);
        if (e3 > 0) {
            MenuPiece br = menu_piece_make(SRC_TOP_PIC, kBar.x, kBar.y, kBar.w, kBar.h, MENU_AX_C, MENU_AY_B, 0, 0, 1, 1);
            br.touch = 1; br.nodraw = 1;
            menu_add_piece(L, br);
        }
        return;
    }     /* the buttons' rows of the lower screen: boxes x 4..44 / 212..252, y 148..188, with the slide-in path below */
    int x0, y0, fw, fh;
    toy_dest(L, 1.f, &x0, &y0, &fw, &fh);
    if (e3 > 0) {      /* FACE3: the canvas starts at the head's size and place (the cut from the drawing page is seamless), then grows to the DS canvas's own size, centred, for drawing */
        const float t = (float)(e3 - 8) / 26.f, g = t <= 0.f ? 0.f : t >= 1.f ? 1.f : t * t * (3.f - 2.f * t);
        const float upx = (float)L->unum / (float)L->uden, ffw = 256.f * upx, ffh = 192.f * upx;
        const float fx0 = (float)w * 0.5f - ffw * 0.5f, fy0 = ((float)h - ffh) * 0.5f;
        x0 = (int)((float)x0 + (fx0 - (float)x0) * g + 0.5f); y0 = (int)((float)y0 + (fy0 - (float)y0) * g + 0.5f);
        fw = (int)((float)fw + (ffw - (float)fw) * g + 0.5f); fh = (int)((float)fh + (ffh - (float)fh) * g + 0.5f);
    }
    MenuPiece st = menu_piece_make(SRC_TOP_PIC, 0, 0, 256, 192, MENU_AX_C, MENU_AY_T, 0, 0, 1, 1);
    st.fixdest = 1; st.keycover = 1; st.bg = 1; st.has_ex = 1; st.ex = kYo; st.has_ex2 = 1; st.ex2 = kPen;
    if (e3 > 0) { st.has_ex3 = 1; st.ex3 = kBar; }
    if (e3 > 0 && e3 <= 12) { st.ex2.y = 100; st.ex2.h = 92; }      /* the pencil's tick sprite while it slides out */
    st.fx = (short)x0; st.fy = (short)y0; st.fw = (short)fw; st.fh = (short)fh;
    menu_add_piece(L, st);
    MenuPiece tc = st;                           /* the touch twin: the same transform inverts a click on the strokes' area to the ROM's canvas */
    tc.keycover = 0; tc.has_ex = 0; tc.has_ex2 = 0; tc.bg = 0; tc.touch = 1; tc.nodraw = 1;
    tc.has_ex3 = 0;
    const int th = 192;           /* INTROCARD092: the whole stylus surface; the buttons' pieces come after and win the hit (the canvas ended at row 130 / 146: a drag stopped at about half the window) */
    tc.s.h = (short)th; tc.fh = (short)((fh * th + 96) / 192);     /* ends above the button rows so it never meets the buttons' own touch boxes */
    menu_add_piece(L, tc);
    MenuPiece yo = menu_piece_make(SRC_TOP_PIC, kYo.x, kYo.y, kYo.w, kYo.h, MENU_AX_L, MENU_AY_B, 0, 0, 1, 1);
    yo.keycover = 1; yo.touch = 1;
    menu_add_piece(L, yo);
    MenuPiece pe = menu_piece_make(SRC_TOP_PIC, kPen.x, kPen.y, kPen.w, kPen.h, MENU_AX_R, MENU_AY_B, 0, 0, 1, 1);
    pe.keycover = 1; pe.touch = 1;
    menu_add_piece(L, pe);
    if (e3 > 0) {      /* FACE3: the ink bar, its own piece, centred on the bottom edge (the ROM's box 88..168 x 172..188) */
        MenuPiece br = menu_piece_make(SRC_TOP_PIC, kBar.x, kBar.y, kBar.w, kBar.h, MENU_AX_C, MENU_AY_B, 0, 0, 1, 1);
        br.keycover = 1; br.touch = 1;
        menu_add_piece(L, br);
    }
}

}  // namespace

extern "C" char *data_ov007_02104bd8;     /* the title's screen-fade manager (src/func_ov007_020bf690.c): {obj6, iris sprite, flags, _, request} */

/* ASSET5 measurement (SM64DS_TITLE_TL=1): one line per DS frame with the page, the page's own counter and the fade manager's
   fields (the star iris is the manager's sprite: scale at +0x40/+0x48, drawn while +0x94 is set) */
static void title_timeline(int top, int elem, int req)
{
    static int on = -1;
    if (on < 0) { const char *e = getenv("SM64DS_TITLE_TL"); on = (e && *e && *e != '0') ? 1 : 0; }
    if (!on || !data_ov007_0210342c) return;
    const char *g = (const char *)data_ov007_0210342c;
    const char *sp = *(const char *const *)(g + 8);
    const int t = sp ? *(const int *)(sp + 0xc) : -1;
    int f10 = -1, o8 = -1, o10 = -1, o14 = -1, o18 = -1, o1c = -1, fl = -1, vis = -1, s40 = 0, s48 = 0, rot = 0;
    int px = 0, py = 0, pz = 0;
    const char *F = data_ov007_02104bd8;
    if (F) {
        const char *o6 = *(const char *const *)F, *sub = *(const char *const *)(F + 4);
        f10 = *(const int *)(F + 0x10); fl = *(const int *)(F + 8);
        if (o6) { o8 = *(const int *)(o6 + 8); o10 = *(const int *)(o6 + 0x10); o14 = *(const int *)(o6 + 0x14); o18 = *(const int *)(o6 + 0x18); o1c = *(const int *)(o6 + 0x1c); }
        if (sub) { vis = *(const int *)(sub + 0x94); s40 = *(const int *)(sub + 0x40); s48 = *(const int *)(sub + 0x48); rot = *(const short *)(sub + 0x4c);
                   px = *(const int *)(sub + 0x34); py = *(const int *)(sub + 0x38); pz = *(const int *)(sub + 0x3c); }
    }
    fprintf(stderr, "[tl] f%d top=%d elem=%d req=%d t=%d | fade f10=%d fl8=%d o6{8=%d 10=%d 14=%d 18=%d 1c=%d} iris vis=%d s40=%d s48=%d rot=%d pos=%d,%d,%d\n",
            data_ov007_0210342c[0], top, elem, req, t, f10, fl, o8, o10, o14, o18, o1c, vis, s40, s48, rot, px, py, pz);
}

int menu_title_up(int scene_id)
{
    if (scene_id != 1) { g_up_stamp = -1; menu_online_scene_left(); return 0; }   /* FILEFIX1: out of the title scene no stale stamp can match the title counter (file_wide_cb runs on every frame's first 3D command) */
    int top, elem, req;
    {   /* INTROCARD092 (SM64DS_TITLE_TL=2): the wall-clock gap between title ticks, printed when it is over 45 ms, with the page and the ROM's own counter */
        static int on = -1;
        if (on < 0) { const char *e = getenv("SM64DS_TITLE_TL"); on = (e && *e == '2') ? 1 : 0; }
        if (on) {
            static std::chrono::steady_clock::time_point last;
            static int have = 0, n = 0;
            const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            const double ms = have ? std::chrono::duration<double, std::milli>(now - last).count() : 0.0;
            last = now; have = 1;
            const int fr = data_ov007_0210342c ? data_ov007_0210342c[0] : -1;
            if (ms > 45.0 || (n++ % 30) == 0) fprintf(stderr, "[tl2] f%d top=%d elem=%d dt=%.0fms%s\n", fr, top, elem, ms, ms > 45.0 ? " HITCH" : ""); fflush(stderr);
        }
    if (!port_title_page_state(&top, &elem, &req)) return g_last;   /* not built yet: the attract */
    title_timeline(top, elem, req);
    menu_online_tick(top, elem, req);       /* NAME1: the online menu's waits and the save medium follow this page */
    }
    if (top != g_top || elem != g_elem) {
        g_top = top; g_elem = elem;
        if (hal_menu_trace_on()) fprintf(stderr, "[menu] title page top=%d elem=%d req=%d\n", top, elem, req);
    }
    g_last = variant_for(top, elem);
    if (hal_menu_trace_on() && getenv("SM64DS_HEAD_TRACE") && data_ov007_0210342c) {   /* FACE1 probe: the head object's three lerped fields */
        const char *g = (const char *)data_ov007_0210342c;
        const char *m = *(const char *const *)(g + 0x30);
        if (m) fprintf(stderr, "[head] f%d top=%d elem=%d x=%d y=%d z=%d s=%d  f20=%d v=%d\n", data_ov007_0210342c[0], top, elem, *(const int *)(m + 0x14),
                       *(const int *)(m + 0x18), *(const int *)(m + 0x1c), *(const int *)(m + 0x2c), *(const int *)(g + 0x20),
                       *(const int *)(*(const char *const *)(g + 8) + 0xc));
    }
    if (data_ov007_0210342c) g_up_stamp = data_ov007_0210342c[0];
    chore_step(g_last, top, elem, req);
    chore_rom_writes(top, req);
    return g_last;
}

/* INT2: THE TITLE'S ROM FACTS for build(), once per tick on the logic thread after menu_title_up (so the choreography clock is this tick's) */
void menu_title_facts(MenuFacts *f)
{
    TitleFacts tf;
    memset(&tf, 0, sizeof tf);
    int slot[24], ids[24], x0[24], y0[24], x1[24], y1[24];
    const int n = port_title_elem_boxes(slot, ids, x0, y0, x1, y1, 24);
    tf.nbox = n > 0 ? (n > 24 ? 24 : n) : 0;
    for (int k = 0; k < tf.nbox; ++k) {
        tf.bx[k][0] = (short)ids[k]; tf.bx[k][1] = (short)x0[k]; tf.bx[k][2] = (short)y0[k]; tf.bx[k][3] = (short)x1[k]; tf.bx[k][4] = (short)y1[k];
        if (port_title_elem_live(slot[k])) tf.live |= 1u << k;
    }
    if (hal_menu_trace_on()) {
        static unsigned last_live = 0xffffffffu;
        if (tf.live != last_live) {
            last_live = tf.live;
            char buf[160]; int bl = 0;
            for (int k = 0; k < tf.nbox && bl < 150; ++k) if ((tf.live >> k) & 1) bl += snprintf(buf + bl, sizeof buf - bl, " %d", (int)tf.bx[k][0]);
            fprintf(stderr, "[filelive] f%d elements the ROM takes a touch on:%s\n", data_ov007_0210342c ? data_ov007_0210342c[0] : 0, buf);
        }
    }
    rom_cursor(&tf.cur, &tf.prs);
    tf.frame = data_ov007_0210342c ? data_ov007_0210342c[0] : 0;
    rom_iris(&tf.vis, &tf.s48);
    tf.ch = g_ch;
    tf.wide3d = ntr::gx_menu_wide_active() ? 1 : 0;
    tf.wz = tf.wide3d ? g_file_wz : 0;
    if (hal_menu_trace_on()) { static int lw = -1; if (tf.wz != lw) { lw = tf.wz; fprintf(stderr, "[filewide] zoom permille %d (active %dx%d present %dx%d)\n", tf.wz, ntr::active_w, ntr::active_h, ntr::present_w(), ntr::present_h()); } }
    tf.sc_on = (tf.wide3d && g_sc.on && data_ov007_0210342c && g_sc.frame + 1 >= data_ov007_0210342c[0]) ? 1 : 0;   /* the callback ran in this tick's Render (the counter moved on once since) */
    tf.sc_K = g_sc.T.K; tf.sc_Tx = g_sc.T.Tx; tf.sc_Ty = g_sc.T.Ty;
    tf.toy_v = 0;
    {
        static int last_v = -1, last_e = 0, dd_chain = 0, dd_e = 0, was_dd = 0, out_e = 0, dd_live_c = 0;
        int top = -1, elem = -1, req = -1;
        const bool st = port_title_page_state(&top, &elem, &req) != 0;
        static int e3c = 0, e3oc = 0;
        const int e3p = e3c;
        e3c = (st && (elem == 3 || (elem == 4 && top == 0 && e3c > 0))) ? e3c + 1 : 0;
        tf.e3 = e3c;
        if (st && e3p > 0 && e3c == 0 && elem == 2) e3oc = 1;
        else if (e3oc > 0 && st && elem == 2) e3oc = e3oc > DD_XF + 1 ? 0 : e3oc + 1;
        else e3oc = 0;
        tf.e3o = e3oc;
        tf.yh = (st && elem == 4 && g_last == V_DOODLE) ? 1 : 0;
        static int yoshi_out = 0;
        yoshi_out = (st && elem == 4) ? 1 : (st && elem == 2) ? 0 : yoshi_out;      /* the Yoshi press: the ROM's own star iris covers the hand-over, no held frame fades over it */
        if (st && top == 4 && elem == 0 && data_ov007_0210342c) {
            const char *sp = *(const char *const *)((const char *)data_ov007_0210342c + 8);
            tf.toy_v = (sp ? *(const int *)(sp + 0xc) : 0);
            if (tf.toy_v < 0) tf.toy_v = 0;
            tf.toy_v += 1;
            last_v = tf.toy_v; last_e = 0;
            dd_chain = 1; dd_e = 0; was_dd = 0; out_e = 0; dd_live_c = 0;
        } else if (last_v >= 0 && elem == 1 && (top == 0 || top == 1)) {   /* the ROM's hand-over: its own fade, the drawing page not up yet */
            tf.toy_v = 1001; tf.toy_e = ++last_e;
        } else {
            last_v = -1; last_e = 0;
        }
        /* FACE2: the drawing page chain (g_last is this tick's page) */
        if (g_last == V_DOODLE || (g_last == V_TOY && tf.toy_v >= 1000)) {
            if (dd_chain) { dd_e = tf.toy_v >= 1000 ? tf.toy_e : dd_e + 1; tf.dd_e = dd_e; }
            tf.dd_dead = (tf.toy_v >= 1000 || (elem == 1 && top != 2)) ? 1 : 0;
            dd_live_c = (dd_chain && !tf.dd_dead) ? dd_live_c + 1 : 0;
            tf.dd_live = dd_live_c;
            was_dd = 1; out_e = 0;
        } else if (was_dd && yoshi_out) {
            was_dd = 0; out_e = 0; dd_chain = 0; dd_live_c = 0;
        } else if (was_dd) {
            tf.out_e = ++out_e;
            if (out_e >= DD_XF + 2) { was_dd = 0; out_e = 0; dd_chain = 0; dd_live_c = 0; }
        }
    }
    {
        int t2 = -1, e2 = -1, r2 = -1;
        if (port_title_page_state(&t2, &e2, &r2)) { tf.pg_top = t2; tf.pg_elem = e2; } else { tf.pg_top = -1; tf.pg_elem = -1; }
    }
    memcpy(f->blob, &tf, sizeof tf);
    menu_online_facts((OnlFacts *)(f->blob + ONL_FACTS_OFF));      /* NAME1 */
}

/* ---- THE FILE PAGES (run menu1, lane FILE1) ----------------------------------------------------------------
   File select, file options, the scores pick, copy, erase, the sound / backlight page and the doodle page ARE the DS lower
   screen (Tango, 10-09: "nothing really needs to change at all, just make the bottom screen fit 4:3 and widescreen"). The
   lower screen is engine A's picture (the title's engines are swapped), so each page is that picture whole:
     4:3     one piece, the whole 256 x 192 lower screen at 1/1, which is the whole window. No castle photo, no banner,
             no panel.
     wide    the same piece, centred at its own size (the DS arrangement of plates, back arrow and rows is untouched,
             nothing is stretched), and the margins on both sides carry the page's own background out: the picture's
             leftmost / rightmost column repeated across the margin (xfill), which is the page's colour per row
             (the reflection of the castle, or the SUPER MARIO DS wallpaper, fades into itself at the edge).
   WHAT THE BACKGROUND IS (measured, SM64DS_TITLE_DRAWTRACE + a probe that skipped every submitted sprite object): on the
   file select family the lower screen is a 3D reflection of the castle (two 256 x 256 textures, drawn by the page's own
   model, NOT by func_ov007_020c421c) with the plates and glyphs on top as sprite objects; the wallpaper objects k0..k2
   of the main menu are not drawn there at all. The sound / backlight and doodle pages show the SUPER MARIO DS pattern
   wallpaper objects. Both stay in the picture. TODO(ASSET5): once the moving "Super Mario 64 PC" pattern background of
   the main menu is built, the options and doodle pages should draw it in place of the wallpaper objects, and the
   file pages' margins could take its colour.
   Hotspots are the ROM's own element boxes in lower-screen DS pixels (piece 1:1), so a click lands in the box the ROM
   hit-tests (src/func_ov007_020b46b0.c arithmetic through port_title_elem_boxes).
   THE SCORE TABLE is two screens of text in the ROM: courses 1..9 and the header on the upper screen (engine B), courses
   10..15 and the castle's secret stars on the lower (engine A, a 2D BG layer that types in), over the file plates,
   which the ROM never removes, so on the DS they show through the list. On one page:
     background  the lower screen's own picture (reflection tinted by the page, the back arrow where the ROM puts it),
                 with the three file plates and their star / number glyphs not submitted (the sprite-object filter
                 below) and engine A's text layers lifted out of the picture (lift_owners) so they are drawn as text
                 alone;
     the list    two text pieces stacked at 2/3, the upper screen's text layers alone (keyed on the backdrop colour,
                 own transparency) and the lower screen's lifted text cells (own alpha), one list top to bottom like the
                 N64 table, in the left of the 4:3 area so the back arrow keeps its own place at the lower right.
   Nothing is cut from a finished picture except the background itself. */
extern "C" {
extern int (*g_port_title_draw_skip)(char *obj);
}
namespace {
int ftune(int idx, int dflt)          /* SM64DS_FILE_TUNE=a,b,c,... integer knobs for layout iteration (FILE1) */
{
    static int v[10], have = 0;
    if (!have) {
        have = 1;
        for (int i = 0; i < 10; ++i) v[i] = -9999;
        const char *e = getenv("SM64DS_FILE_TUNE");
        for (int i = 0; e && *e && i < 10; ++i) {
            char *q;
            v[i] = (int)strtol(e, &q, 10);
            if (*q != ',') break;
            e = q + 1;
        }
    }
    return v[idx] == -9999 ? dflt : v[idx];
}

/* the score table's page: its plates (and their glyphs) are not submitted. They sit above DS row 130; the back arrow
   (y 158) and every other object stays. Chained in front of the main menu's filter. */
int (*g_file_prev_skip)(char *obj);
bool table_skip_active()
{
    if (!data_ov007_0210342c || g_up_stamp != data_ov007_0210342c[0]) return false;
    int top, elem, req;
    if (!port_title_page_state(&top, &elem, &req)) return false;
    return variant_for(top, elem) == V_TABLE;
}
int file_skip_inner(char *o);
void scene_sprite_xf(char *o);
/* ANCHOR1: the back arrow (tile and glyph, DS box x 180..252, y 140..176) of the wide file pages (select, options, scores pick, copy,
   erase, the score table) is a corner control: it keeps its DS inset (4 units from the right edge) from the WINDOW's right edge, the
   same translation the drawing page's pencil gets in scene_sprite_xf. The plates and the page stay centred; at 4:3 the translation
   is zero and nothing is set. file_back_shift_units() is the one number the sprite, the cursor arrow and the touch piece share. */
static bool file_back_obj(int x, int y) { return x >= 180 && y >= 136; }
/* ANCHOR1 (design, Tango's choice): SM64DS_FILEOPT_EDGE=1 mirrors the FILE OPTIONS plank to the window's LEFT edge with the back arrow's inset (4 units),
   so the bottom row balances the moved back arrow. Default off: the plank stays centred with the plates. */
static bool fileopt_edge_on()
{
    static int v = -1;
    if (v < 0) { const char *e = getenv("SM64DS_FILEOPT_EDGE"); v = (e && *e && *e != '0') ? 1 : 0; }
    return v != 0;
}
static bool fileopt_obj(int x, int y) { return x >= 28 && x <= 156 && y >= 136; }       /* the plank's object (id 1c) and its cursor sit inside its DS box */
static bool file_wide_page_now()
{
    if (!ntr::widescreen || ntr::active_h <= 0 || !hal_menu_live()) return false;
    if (!data_ov007_0210342c || g_up_stamp != data_ov007_0210342c[0]) return false;
    int top, elem, req;
    if (!port_title_page_state(&top, &elem, &req)) return false;
    const int v = variant_for(top, elem);
    return (v == V_FILES || v == V_TABLE) && ntr::gx_menu_wide_active();
}
static void file_back_xf(char *o)
{
    if (!file_wide_page_now()) return;
    float cw, ch;
    scene_canvas_units(&cw, &ch);
    if (cw <= 256.5f) return;
    const int x = ((const int *)o)[1] >> 12, y = ((const int *)o)[2] >> 12;
    if (file_back_obj(x, y)) ntr::gx_menu_sprite_xf(1.f, (cw - 256.f) / cw, 1.f, 0.f);       /* 2 * ((cw - 256) / 2) / cw */
    else if (fileopt_edge_on() && fileopt_obj(x, y)) ntr::gx_menu_sprite_xf(1.f, -((cw - 256.f) + 48.f) / cw, 1.f, 0.f);   /* its box's left edge (DS x 28) to 4 units from the window's left edge */
}
int file_skip(char *o)
{
    const int r = file_skip_inner(o);
    if (!r) {
        ntr::gx_menu_sprite(1);      /* FILE2: this object is drawn: its triangles are sprite objects, not the page model */
        scene_sprite_xf(o);          /* TITLE092: and in the title's full-canvas scene it has its own place */
        file_back_xf(o);             /* ANCHOR1: the wide file pages' back arrow rides the window's right edge */
    }
    return r;
}
void file_draw_end() { ntr::gx_menu_sprite(0); }
int file_skip_inner(char *o)
{
    if (table_skip_active()) {
        const int y = *(const int *)(o + 8) >> 12;
        static int tr = -1;
        if (tr < 0) { const char *e = getenv("SM64DS_FILE_TRACE"); tr = (e && *e && *e != '0') ? 1 : 0; }
        if (tr && (data_ov007_0210342c[0] % 120) == 0)
            fprintf(stderr, "[file] table obj id=%02x x=%d y=%d %s\n", (unsigned)*(unsigned char *)(o + 0x4e), *(const int *)(o + 4) >> 12, y,
                    y < 130 ? "skipped" : "kept");
        if (y < 130) return 1;
    }
    return g_file_prev_skip ? g_file_prev_skip(o) : 0;
}
}  // namespace
/* ANCHOR1 (F4): the water ripple of a cursor that LEAVES an element. The ROM spawns it at the element's object position (func_ov007_020beeb0,
   the branch with no stylus down, strength 0x7d0, through func_ov007_020bed60), which for the back arrow is its DS place; the arrow is now drawn
   at the window's right edge, so the ripple came up where the arrow used to be. This maps that DS point to the water point under the arrow's
   WINDOW place: the page behind it is zoomed about the canvas centre (g_file_wz), so the point is the centre plus the arrow's offset from the
   canvas centre over the zoom, the same arithmetic menu_inverse uses for a touch on the water. Anything else is left alone, and so is a stylus
   touch (the host already hands the ROM the water point for one). */
extern "C" void port_title_ripple_xf(int *x, int *y)
{
    const bool bk = file_back_obj(*x, *y), op = !bk && fileopt_edge_on() && fileopt_obj(*x, *y);
    if ((!bk && !op) || !file_wide_page_now()) return;
    float cw, ch;
    scene_canvas_units(&cw, &ch);
    if (cw <= 256.5f || g_file_wz <= 1000) return;
    const float zoom = (float)g_file_wz / 1000.0f;
    const float shift = bk ? (cw - 256.0f) * 0.5f : -((cw - 256.0f) * 0.5f + 24.0f);
    int nx = 128 + (int)(((float)(*x - 128) + shift) / zoom);
    int ny = 96 + (int)((float)(*y - 96) / zoom);
    if (nx < 0) nx = 0;
    if (nx > 255) nx = 255;
    if (ny < 0) ny = 0;
    if (ny > 191) ny = 191;
    if (hal_menu_trace_on()) fprintf(stderr, "[filecur] ripple of the cursor leaving a moved plank: DS (%d,%d) -> water (%d,%d)\n", *x, *y, nx, ny);
    *x = nx; *y = ny;
}
extern "C" {
extern void (*g_port_title_draw_end)(void);
}
namespace {
/* FILE2: THE WIDE BACKGROUND. On the pages whose lower screen is the castle reflection (file select, file options, scores pick,
   copy, erase, the score table) the 3D layer of a wide window covers the whole frame: the page model (the reflection) is zoomed
   in the 3D about the frame's centre so it fills the width (the top and bottom crop), while the plates, glyphs and the back arrow
   (sprite objects) are drawn by the same pass at their DS places in the centred 4:3 area, at their DS size. Asked once per frame
   by ntr/gx.cpp at the first 3D command, in the ROM's Render, so the page is the ROM's live page. */
bool file_wide_cb(float *zoom, float *dy)
{
    static int off = -1;
    if (off < 0) { const char *e = getenv("SM64DS_FILE_WIDE"); off = (e && *e == '0') ? 1 : 0; }
    if (off || !ntr::widescreen || ntr::active_h <= 0) return false;
    if (!hal_menu_live()) return false;      /* FILEFIX1: no recipe owns the picture (setting off, gameplay): never walk the title's pointers */
    if (!data_ov007_0210342c || g_up_stamp != data_ov007_0210342c[0]) return false;
    int top, elem, req;
    if (!port_title_page_state(&top, &elem, &req)) return false;
    const int v = variant_for(top, elem);
    if (v != V_FILES && v != V_TABLE) return false;
    /* the canvas width in DS units over the width of real page content: the reflection plane is about 292 DS units wide (measured,
       its edge is a dark zigzag outline beyond that), so the page is zoomed exactly as much as keeps the plane's content on both
       edges (288 = a little inside it) and never more; 256 would be the fit-the-width zoom, 9999 no zoom at all (a true wide field,
       which shows the empty plane edge) */
    static int wc = 288, dp = 0, tuned = 0;       /* SM64DS_FILE_WIDE_TUNE=content width in DS units, vertical shift permille of the half height; read once */
    if (!tuned) {
        tuned = 1;
        if (const char *e = getenv("SM64DS_FILE_WIDE_TUNE")) sscanf(e, "%d,%d", &wc, &dp);
    }
    const float cwu = (float)ntr::active_w * 192.0f / (float)(ntr::present_h() > 0 ? ntr::present_h() : 192);   /* the canvas width in DS units (192 rows = one DS screen) */
    float z = cwu / (float)(wc > 64 ? wc : 64);
    if (z < 1.0f) z = 1.0f;
    *zoom = z;
    *dy = (float)dp / 1000.0f;
    g_file_wz = (int)(z * 1000.0f + 0.5f);       /* MENUS093E: the touch inverse needs it */
    return true;
}
/* TITLE092: THE SCENE CALLBACK, asked once per frame at the first 3D command (in the ROM's Render, so the page is live). The file pages
   keep FILE2's zoom (file_wide_cb); the attract, the intro, the main menu, the face press page and the drawing page answer with the
   scene transform of their page (see the block above build_main_pc). Every one of those pages is built from this answer in build():
   a page this callback does not answer for is built by its old cut pieces, so the two conditions below mirror build_main_sel's. */
int title_page_counter()
{
    if (!data_ov007_0210342c) return 0;
    const char *sp = *(const char *const *)((const char *)data_ov007_0210342c + 8);
    return sp ? *(const int *)(sp + 0xc) : 0;
}
bool title_scene_cb(float xf[4])
{
    g_sc.on = 0;
    float z = 1.f, dy = 0.f;
    if (file_wide_cb(&z, &dy)) { xf[0] = z; xf[1] = 0.f; xf[2] = z; xf[3] = dy; return true; }
    if (!scene_on_env() || ntr::active_h <= 0 || ntr::active_w <= 0) return false;
    if (!hal_menu_live()) return false;
    if (!data_ov007_0210342c || g_up_stamp != data_ov007_0210342c[0]) return false;
    int top, elem, req;
    if (!port_title_page_state(&top, &elem, &req)) return false;
    const int v = variant_for(top, elem);
    const int fr = data_ov007_0210342c[0];
    scene_e3_step(top, elem, fr);
    SceneT T = { 1.f, 0.f, 0.f };
    const bool pc = pc_assets_ready();
    switch (v) {
    case V_ATTRACT:
        break;
    case V_INTRO:
        if (g_ch.mode == CH_INTRO && g_ch.ct < 0) break;           /* the first frames of the intro: the DS layout, the host star flies over it */
        if (!pc) return false;
        T = scene_main();
        break;
    case V_MAIN:
        if (!pc) return false;
        T = scene_main();
        break;
    case V_TOY:
        if (!pc) {
            /* TITLEART1: without the HD UI mod the press page is the ROM's lower screen at the DS layout (file_pages_build); the hand-over frames
               that follow it keep that layout (see V_DOODLE) */
            if (top == 4 && elem == 0) return false;
            break;
        }
        if (top == 4 && elem == 0) T = scene_toy(toy_grow(title_page_counter()));
        else T = scene_doodle(0);                                   /* the hand-over frames: the drawing page's rest pose */
        break;
    case V_DOODLE:
        /* TITLEART1: without the HD UI mod the head you press is drawn at the DS layout (the press page above), so the drawing page is drawn at that
           same layout {1, 0, 0}: the ROM's line art then lies on the head it replaces, as on the DS (FACE1 measured the strokes over the press page's
           rest pose), and the crossfade from the press page's held frame no longer jumps from a big head to a small outline. The touch twins and the
           sprite placement read this same transform, so a stroke lands under the pointer. The HD page keeps its own (the head the main menu drew). */
        T = pc ? scene_doodle(g_sc.e3) : T;
        break;
    default:
        return false;
    }
    float cw, ch;
    scene_canvas_units(&cw, &ch);
    scene_xf(T, cw, ch, xf);
    g_sc.on = 1; g_sc.variant = v; g_sc.frame = fr; g_sc.T = T;
    {
        static int last_v = -1;
        if (hal_menu_trace_on() && (v != last_v || getenv("SM64DS_TITLE_SCENETRACE")))
            fprintf(stderr, "[scene92] f%d variant %d top %d elem %d T K %.3f Tx %.2f Ty %.2f canvas %.1f x %.1f xf %.3f %.3f %.3f %.3f\n", fr, v, top, elem,
                    T.K, T.Tx, T.Ty, cw, ch, xf[0], xf[1], xf[2], xf[3]);
        last_v = v;
    }
    return true;
}

/* TITLE092: a sprite object (the ROM's 3D quads through func_ov007_020c421c) in the scene frame. Its screen place is its own x / y (DS
   pixels, o + 4 / o + 8); the scene transform does not touch sprite objects, so each is placed here: the wrench pinned to the top right
   corner of the canvas (and lifted out of the top as the face is pressed), the drawing page's buttons pinned to the left / right /
   bottom edge at their DS rows, anything else on the drawing page (the canvas the strokes are on) by the scene transform. */
void scene_sprite_xf(char *o)
{
    if (!g_sc.on || !data_ov007_0210342c || g_sc.frame + 1 < data_ov007_0210342c[0]) return;      /* the callback ran in this tick's Render, the counter moved on once since (as build's sc_on) */
    const int v = g_sc.variant;
    if (v == V_ATTRACT || (v == V_INTRO && g_ch.ct < 0)) return;
    float cw, ch;
    scene_canvas_units(&cw, &ch);
    const float sx = (cw - 256.f) * 0.5f, sy = (ch - 192.f) * 0.5f;       /* the canvas margins beside / above the DS box */
    const int id = *(unsigned char *)(o + 0x4e), x = ((const int *)o)[1] >> 12, y = ((const int *)o)[2] >> 12;
    if (hal_menu_trace_on() && getenv("SM64DS_TITLE_SCENETRACE") && (data_ov007_0210342c[0] % 30) == 0)
        fprintf(stderr, "[scene92] f%d sprite id=%02x x=%d y=%d variant %d\n", data_ov007_0210342c[0], id, x, y, v);
    if (v == V_MAIN || v == V_INTRO || v == V_TOY) {
        if (id == 0x1b && y < 100) {
            float up = 0.f;
            if (v == V_TOY) { const int tv = title_page_counter(); const float e = tv >= 10 ? 1.f : (float)tv / 10.f; up = e * e * 60.f; }
            ntr::gx_menu_sprite_xf(1.f, 2.f * (sx - (menu_online_on() ? 35.f : 0.f)) / cw, 1.f, 2.f * (sy + up) / ch);      /* NAME1: beside the ? button in online mode; NAME2: the ? (30) and its gap from the window edge (5) */
        }
        return;
    }
    if (v == V_DOODLE) {
        if (x < 64) ntr::gx_menu_sprite_xf(1.f, -2.f * sx / cw, 1.f, -2.f * sy / ch);               /* the Yoshi button: left edge, bottom */
        else if (x > 192) ntr::gx_menu_sprite_xf(1.f, 2.f * sx / cw, 1.f, -2.f * sy / ch);         /* the pencil: right edge, bottom */
        else if (y >= 160) ntr::gx_menu_sprite_xf(1.f, 0.f, 1.f, -2.f * sy / ch);                  /* the ink bar: bottom centre */
        else { float xf[4]; scene_xf(g_sc.T, cw, ch, xf); ntr::gx_menu_sprite_xf(xf[0], xf[1], xf[2], xf[3]); }
    }
}

/* FIELDUI1 (U2): a mouse hover on the title or the file pages plays the ROM's own D-pad move sound (func_ov007_020bdeb0 index 4, or 5 on
   the page the ROM's own step picks it for, src/unnamed/ov007/func_ov007_020b3708.c), not the generic menu cursor sound. Only while the
   title's page state answers (the title family is the live page). */
int title_cursor_snd()
{
    int top, elem, req;
    if (!data_ov007_02103360 || !data_ov007_0210342c || !port_title_page_state(&top, &elem, &req)) return 0;
    if (g_up_stamp < 0 || data_ov007_0210342c[0] - g_up_stamp > 2 || g_up_stamp - data_ov007_0210342c[0] > 2) return 0;
    /* SNDCAM1 (S1): a hover that moves nothing makes no sound. A file is already chosen (the file machine's leaving states 11 / 12), or the
       main page has taken a choice (req != -1): the screen is on its way out and nothing under the mouse is a new selection. And when the
       ROM's own cursor (the orange arrow on the file pages, the lit plank on the main page) already is on the element the mouse landed on,
       there is nothing to announce: the same test title_cursor_assist uses before it writes the hover into the cursor. */
    if (elem == 5 && (top == 11 || top == 12)) return 1;
    if (req != -1) return 1;
    int q0dbg0 = -2, q0dbg1 = -2;
    {
        short *q0 = *(short **)data_ov007_02103360;
        int hx, hy;
        unsigned st;
        if (q0) { q0dbg0 = q0[0]; q0dbg1 = q0[1]; }
        if (q0 && hal_menu_hover_box(&hx, &hy, &st)) {
            /* TITLEUI1: the online planks the pad focus can sit on (the ?, NAME): the arrow is already under the one the mouse landed on, nothing to announce.
               A hover back on the ROM element the cursor holds while the focus is on such a plank does move the arrow (title_cursor_assist hands the focus back). */
            const int pf = menu_online_padfocus();
            const int hpf = (hx == ONL_HELP_X && hy == ONL_HELP_Y) ? ONL_PF_HELP : (hx == ONL_NAME_X && hy == ONL_NAME_Y) ? ONL_PF_NAME : 0;
            if (hpf && hpf == pf) {
                if (hal_menu_trace_on()) fprintf(stderr, "[menu] hover on plank %d: the pad focus is already there, no sound\n", hpf);
                return 1;
            }
            int slot[24], ids[24], x0[24], y0[24], x1[24], y1[24];
            const int n = hpf ? 0 : port_title_elem_boxes(slot, ids, x0, y0, x1, y1, 24);
            for (int k = 0; k < n; ++k) {
                if (x0[k] != hx || y0[k] != hy || !port_title_elem_live(slot[k])) continue;
                if ((q0[0] == ids[k] || q0[1] == ids[k]) && !pf) {
                    if (hal_menu_trace_on()) fprintf(stderr, "[menu] hover on element %d: the ROM cursor is already there, no sound\n", ids[k]);
                    return 1;
                }
                break;
            }
        }
    }
    if (hal_menu_trace_on()) fprintf(stderr, "[menu] hover sound: page %d/%d, cursor %d -> %d\n", top, elem, q0dbg0, q0dbg1);
    func_ov007_020bdeb0((*(int *)(data_ov007_02103360 + 8) != 2) ? 4 : 5);
    return 1;
}

struct FileHookInit {
    FileHookInit()
    {
        g_menu_cursor_snd_hook = title_cursor_snd;
        g_file_prev_skip = g_port_title_draw_skip; g_port_title_draw_skip = file_skip;
        g_port_title_draw_end = file_draw_end;
        ntr::gx_set_menu_wide_cb(file_wide_cb);
        ntr::gx_set_menu_scene_cb(title_scene_cb);
    }
} g_file_hook_init;

/* the margins of a wide canvas: the picture's outermost column carried across, one piece a side, under the page */
void file_fills(MenuLayout *L)
{
    const int cw = L->w * L->uden / L->unum;
    const int side = cw > 256 ? (cw - 256 + 1) / 2 + 2 : 0;       /* each margin, with an overlap under the page */
    if (side <= 0) return;
    /* source width 0: the draw maps every margin pixel to ONE host column of the edge DS column (width 1 would spread the margin
       over that DS column's four host columns in vertical steps) */
    MenuPiece lf = menu_piece_make(SRC_TOP_PIC, 0, 0, 0, 192, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    lf.bg = 1; lf.xfill = 1; lf.fill_w = (short)side;
    menu_add_piece(L, lf);
    MenuPiece rt = menu_piece_make(SRC_TOP_PIC, 255, 0, 0, 192, MENU_AX_R, MENU_AY_T, 0, 0, 1, 1);
    rt.bg = 1; rt.xfill = 1; rt.fill_w = (short)side;
    menu_add_piece(L, rt);
}

/* FILE2: the moving PC pattern wallpaper of the main menu (gradient + the "Super Mario 64 PC" pattern, the OG's scale and drift), full canvas */
void add_pc_wall(MenuLayout *L)
{
    const int un = L->unum, ud = L->uden;
    MenuPiece wl = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    wl.bg = 1; wl.wall = 1; wl.lden = 1; wl.lnum = 1; wl.fx = 0; wl.fy = 0; wl.fw = (short)L->w; wl.fh = (short)L->h;
    const int tu = ctune(5, 64);
    wl.wtw = (short)(tu * un / ud);
    wl.wth = (short)((int)((long long)wl.wtw * 730 / 410));
    wl.wox = (short)((ctune(6, -6) * un) / ud);
    wl.woy = (short)((ctune(7, -11) * un) / ud);
    wl.wink = (uint8_t)ctune(8, 107);
    menu_add_piece(L, wl);
}

void file_pages_build(int variant, MenuLayout *L)
{
    L->backdrop = MENU_BACKDROP_TOP_AS_IS;
    L->stage = MENU_STAGE_POST_FADE;
    /* FILE2: the sound / backlight page: the ROM's wallpaper objects are not submitted (wall_skip_active), the moving PC pattern is
       drawn full width under the page's own 3D objects (the head, the sliders, the plates), cut by the 3D coverage so nothing
       but those objects comes along. The reflection pages with a wide 3D frame already carry their background to the edges. */
    const bool pat = variant == V_OPTIONS && pc_assets_ready();
    if (pat) add_pc_wall(L);
    else if (!(t_f && t_f->wide3d)) file_fills(L);
    /* the page: the whole lower screen, 1/1, centred (4:3: the whole window) */
    MenuPiece pg = menu_piece_make(SRC_TOP_PIC, 0, 0, 256, 192, MENU_AX_C, MENU_AY_M, 0, 0, 1, 1);
    pg.bg = 1; pg.touch = 1;
    if (pat) pg.keycover = 1;
    if (t_f && t_f->wide3d) pg.nodraw = 1;      /* FILE2: the wide 3D frame already is the picture, in place: this piece only maps clicks */
    /* ANCHOR1: on the wide 3D file pages the back arrow rides the window's right edge (file_back_xf), so its touch box rides with it: the
       click-mapping piece is cut in three, the page above row 133 and the lower left (both centred, as before) and the arrow's own corner
       (DS x 180..256, y 133..192, right edge on the canvas edge). A click where the arrow used to be maps to nothing. */
    const bool split = t_f && t_f->wide3d && !pat && L->w * L->uden / L->unum > 256;
    int ip_lo = -1, ip_bk = -1;
    if (split) {
        pg.s.h = 133;
        pg.ay = MENU_AY_T;
    }
    const int ip = menu_add_piece(L, pg);
    if (variant == V_FILES) menu_online_add_tint(L, onl_facts_of(L));      /* NAME1: the online file select keeps the green over its water */
    int ip_fo = -1;
    if (split && fileopt_edge_on()) {
        /* the knob: the lower left is cut into the part left of the plank, the part between the plank and the arrow's corner (both centred, so the
           old plank place maps to nothing), and the plank's own piece on the window's left edge (DS x 24 .. 160, its left edge 4 units in) */
        MenuPiece a = pg; a.s.x = 0; a.s.y = 133; a.s.w = 24; a.s.h = 59; a.ax = MENU_AX_C; a.ay = MENU_AY_T; a.dx = (short)(0 + 12 - 128); a.dy = 133;
        ip_lo = menu_add_piece(L, a);
        MenuPiece b = pg; b.s.x = 160; b.s.y = 133; b.s.w = 20; b.s.h = 59; b.ax = MENU_AX_C; b.ay = MENU_AY_T; b.dx = (short)(160 + 10 - 128); b.dy = 133;
        menu_add_piece(L, b);
        MenuPiece f = pg; f.s.x = 24; f.s.y = 133; f.s.w = 136; f.s.h = 59; f.ax = MENU_AX_L; f.ay = MENU_AY_T; f.dx = 0; f.dy = 133;
        ip_fo = menu_add_piece(L, f);
    }
    if (split && ip_fo < 0) {
        MenuPiece lo = pg; lo.s.x = 0; lo.s.y = 133; lo.s.w = 180; lo.s.h = 59; lo.ax = MENU_AX_C; lo.ay = MENU_AY_T; lo.dx = (short)(0 + 90 - 128); lo.dy = 133;
        ip_lo = menu_add_piece(L, lo);
    }
    if (split) {
        MenuPiece bk = pg; bk.s.x = 180; bk.s.y = 133; bk.s.w = 76; bk.s.h = 59; bk.ax = MENU_AX_R; bk.ay = MENU_AY_T; bk.dx = 0; bk.dy = 133;
        ip_bk = menu_add_piece(L, bk);
    }
    if (t_f && t_f->wide3d && !pat) L->wz = t_f->wz;       /* MENUS093E: the reflection is zoomed in the 3D: a touch on the water is the zoomed point */
    if (pat) {
        /* the page's own 2D layer (the SOUND and BACKLIGHT labels, the star) is lifted out of the picture and drawn over the 3D objects,
           in place, with its own transparency */
        L->lift_owners = MENU_L_BG1 | MENU_L_BG2 | MENU_L_BG3 | MENU_L_OBJ;
        MenuPiece tx = menu_piece_make(SRC_TOP_2D, 0, 0, 256, 192, MENU_AX_C, MENU_AY_M, 0, 0, 1, 1);
        tx.layers = L->lift_owners;
        menu_add_piece(L, tx);
    }
    if (variant == V_TABLE) {
        L->lift_owners = (uint8_t)ftune(0, MENU_L_BG1 | MENU_L_BG2 | MENU_L_BG3 | MENU_L_OBJ);
        const int kn = ftune(5, 2), kd = ftune(6, 3);
        const int us = ftune(1, 10), uh = ftune(2, 176), ls = ftune(3, 12), lh = ftune(4, 108);
        const int dx = ftune(7, -39);                         /* the piece's centre from the canvas centre: the text sits at the left of the 4:3 area */
        const int dy0 = ftune(8, 0);
        MenuPiece up = menu_piece_make(SRC_SUB, 0, us, 256, uh, MENU_AX_C, MENU_AY_T, dx, dy0, kn, kd);
        up.layers = (uint8_t)ftune(9, MENU_L_BG1); up.key_backdrop = 1;   /* BG1 is the text; engine B's OBJ layer holds a few stray sprite pixels beside two rows (measured) */
        menu_add_piece(L, up);
        MenuPiece lo = menu_piece_make(SRC_TOP_2D, 0, ls, 256, lh, MENU_AX_C, MENU_AY_T, dx, dy0 + (uh * kn + kd - 1) / kd, kn, kd);   /* rounded up so the two never overlap */
        lo.layers = L->lift_owners;
        menu_add_piece(L, lo);
        static const int bk_ids[1] = { 20 };
        hot(L, ip_bk >= 0 ? ip_bk : ip, bk_ids, 1);
        return;
    }
    if (variant == V_DOODLE || variant == V_TOY) return;
    if (variant == V_OPTIONS) {
        static const int ids[6] = { 10, 9, 11, 12, 13, 8 };
        hot(L, ip, ids, 6);
        return;
    }
    /* POLISH1A: the HD orange cursor arrow where the ROM's sprite was (kept this frame or the one before: the page and the sprite are
       submitted by the same Render), same 22 x 15 DS footprint, tip anchored at the bottom centre; drawn over the plates */
    const OnlFacts *pfo = onl_facts_of(L);     /* PADNAV1: with the pad focus on the ? or NAME plank the cursor arrow is not drawn (the plank is lit instead) */
    if (variant == V_FILES && t_f && g_oa_frame >= 0 && t_f->frame - g_oa_frame <= 1 && !(pfo && pfo->padfocus) &&
        menu_hostimg_ready(MENU_IMG_ARROW_ORANGE)) {
        const int un = L->unum, ud = L->uden;
        const int dw = 22 * un / ud, dh = 15 * un / ud;
        MenuPiece ar = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        ar.bg = 1; ar.hostimg = MENU_IMG_ARROW_ORANGE; ar.lden = 1; ar.lnum = 1; ar.fw = (short)dw; ar.fh = (short)dh;
        ar.fx = (short)(L->w / 2 + (g_oa_x - 11 - 128) * un / ud);
        if (ip_bk >= 0 && file_back_obj(g_oa_x, g_oa_y + 8)) ar.fx = (short)(ar.fx + (L->w - 256 * un / ud) / 2);      /* ANCHOR1: the cursor arrow points at the back arrow where it is now */
        else if (ip_bk >= 0 && fileopt_edge_on() && fileopt_obj(g_oa_x, g_oa_y + 8)) ar.fx = (short)(ar.fx - ((L->w - 256 * un / ud) / 2 + 24 * un / ud));      /* ... and at the FILE OPTIONS plank where the knob put it */
        ar.fy = (short)((g_oa_y - 8) * un / ud + (L->h - 192 * un / ud) / 2);
        menu_add_piece(L, ar);
    }
    static const int fids[8] = { 14, 15, 16, 17, 18, 19, 21, 20 };
    g_hot_live_only = true;
    if (split) {
        hot(L, ip, fids, 6);
        hot(L, ip_fo >= 0 ? ip_fo : ip_lo, fids + 6, 1);
        hot(L, ip_bk, fids + 7, 1);
    } else {
        hot(L, ip, fids, 8);
    }
    g_hot_live_only = false;
    if (variant == V_FILES) {                                  /* NAME1: the online file select: the ? button (the select page itself) and the modal screen */
        const OnlFacts *of = onl_facts_of(L);
        if (of && of->flow) online_overlays(L, false, t_f && t_f->pg_top == 0 && t_f->pg_elem == 5);
    }
}
}  // namespace

namespace {
struct TitleFactsScope { TitleFactsScope(const TitleFacts *f) { t_f = f; } ~TitleFactsScope() { t_f = 0; } };
}

static void menu_title_build_impl(int variant, MenuLayout *L);
/* INT2: the attract and the first frames of the intro are pure black edge to edge: the side areas of a wide canvas would otherwise show the
   ROM's upper-screen backdrop (a lighter blue-grey) around the black 4:3 lower screen. A full-canvas black veil goes under the picture. */
static void add_black_under(MenuLayout *L)
{
    MenuPiece bk = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    bk.bg = 1; bk.iris = 1; bk.lden = 1; bk.lnum = 1; bk.fx = 0; bk.fy = 0; bk.fw = (short)L->w; bk.fh = (short)L->h;
    bk.ir16 = 0;
    menu_add_piece(L, bk);
}

void menu_title_build(int variant, MenuLayout *L)
{
    TitleFactsScope scope((const TitleFacts *)L->facts.blob);
    menu_title_build_impl(variant, L);
    if (variant != V_MAIN) egg_note(L, -1, false, nullptr);      /* EGG1: only the main page has the secret */
    if (t_f && t_f->out_e > 0 && t_f->out_e < DD_XF && variant != V_DOODLE && (variant == V_MAIN || variant == V_TOY)) {     /* FACE2: the drawing page's last frame fades out over the page that follows */
        MenuPiece sn = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        sn.bg = 1; sn.snap = 1; sn.lden = 1; sn.lnum = 1; sn.fx = 0; sn.fy = 0; sn.fw = (short)L->w; sn.fh = (short)L->h;
        const int a = 255 - 255 * t_f->out_e / DD_XF;
        sn.alpha = (uint8_t)(a >= 255 ? 0 : (a < 1 ? 1 : a));
        menu_add_piece(L, sn);
    }
}

static void menu_title_build_impl(int variant, MenuLayout *L)
{
    if (variant == V_ATTRACT) {
        add_black_under(L);
        MenuPiece p = menu_piece_make(SRC_TOP_PIC, 0, 0, 256, 192, MENU_AX_C, MENU_AY_M, 0, 0, 1, 1);
        p.touch = 1; p.key_backdrop = 1;
        if (scene_now()) {      /* TITLE092: the scene covers the canvas (nothing clipped at the DS box's edge); the 4:3 piece only maps the touch */
            MenuPiece sc = scene_layer(L);
            sc.key_backdrop = 1;
            menu_add_piece(L, sc);
            p.nodraw = 1;
        }
        menu_add_piece(L, p);
        L->backdrop = MENU_BACKDROP_TOP_AS_IS;
        L->stage = MENU_STAGE_POST_FADE;
        return;
    }
    /* TITLE092 (3a): the black first frames are only while the host star is on its way up (fly_t < 14: its centre is above the top edge at
       14); from then the iris takes over at once, see build_main_pc, instead of a black beat and the ROM's iris starting from a point */
    if (variant == V_INTRO && t_f && t_f->ch.mode == CH_INTRO && t_f->ch.ct < 0 && !t_f->ch.skip && (t_f->ch.fly_t < 14 || getenv("SM64DS_CHORE_BLACK"))) {
        add_black_under(L);
        /* ASSET6: the first frames of the ROM's state 10 are the lower screen going out (the star leaves, the screen fades to black): shown
           as the ROM draws them until its iris sprite appears */
        MenuPiece p = menu_piece_make(SRC_TOP_PIC, 0, 0, 256, 192, MENU_AX_C, MENU_AY_M, 0, 0, 1, 1);
        p.touch = 1; p.key_backdrop = 1;
        if (scene_now()) {      /* TITLE092: as the attract */
            MenuPiece sc = scene_layer(L);
            sc.key_backdrop = 1;
            menu_add_piece(L, sc);
            p.nodraw = 1;
        }
        menu_add_piece(L, p);
        L->backdrop = MENU_BACKDROP_TOP_AS_IS;
        L->stage = MENU_STAGE_POST_FADE;
        /* INTROCUT1: no host star here any more. The press no longer cuts the ROM's flight, so its own star has already left the top of the picture when the skip comes
           (the host copy used to fly up from the middle of the screen, which showed as the star jumping back). */
        return;
    }
    if (variant == V_DOODLE || (variant == V_TOY && t_f && t_f->toy_v >= 1000)) {   /* FACE2: the hand-over frames are the crossfade, not a main menu with a dip */
        build_doodle(L);
        return;
    }
    if (variant == V_MAIN || variant == V_INTRO || (variant == V_TOY && t_f && t_f->toy_v > 0 && pc_assets_ready())) {
        build_main_sel(L, variant == V_INTRO);
        return;
    }
    file_pages_build(variant, L);
}

/* THE KEY MEANING. pressed is one PadData pressed word (DS key bits: A 0x0001, B 0x0002, Start 0x0008,
   right/left/up/down 0x10/0x20/0x40/0x80). The port's confirm (pad A, Space, Enter) arrives as 0x0002;
   the title ROM confirms on A (0x0001) and treats B as back, so confirm moves to 0x0001 and the host
   back key (Backspace / the bound B-equivalent) becomes the ROM's B. */
extern "C" {
extern unsigned char data_020a0e40;   /* the local player's slot */
extern int data_020a0e58[];           /* PadData[i]: {u16 held, u16 pressed} */
}

/* ASSET3: THE ROM CURSOR, STARTED AND FOLLOWING THE MOUSE.
   The ROM shows its cursor on the first non-B key and starts it on ADVENTURE (src/func_ov007_020b3190.c: it writes the target
   element 0 into the cursor struct's second short; the cursor machinery of func_ov007_020b2e64 does the rest). On the one-screen main
   menu the port does that same write once the arrival has settled, so ADVENTURE is selected without a first press. A mouse hover
   (hal_menu_hover_box) writes the hovered element the same way, so the ROM's cursor, the lit plank and the arrow always name the
   element a confirm will press. 1 = VS (MP), 0 = ADVENTURE, 2 = REC ROOM, 7 = the wrench. Only on the settled main page with no
   choice taken. */
static void title_cursor_assist()
{
    static int stable;
    static unsigned hov_seen;
    int top, elem, req;
    if (data_ov007_02103360 && port_title_page_state(&top, &elem, &req) && elem == 5 && req == -1 && (top == 0 || top == 1 || top == 3 || top == 7 || top == 9)) {
        /* MENUS093E: THE FILE PAGES' CURSOR FOLLOWS THE MOUSE. The same hover write as the main menu's, for the file select family: the cursor values
           14..21 are the element ids (files A B C, score / copy / erase, the wrench-side button and the back arrow); a hovered element the ROM takes
           a touch on becomes the cursor's target, so the orange arrow (the ROM's own object, drawn by the host as the HD arrow) goes where the
           mouse is, and A then presses what the arrow is on. */
        static unsigned hov_seen_f;
        short *q0 = *(short **)data_ov007_02103360;
        int hx, hy;
        unsigned st;
        if (q0 && hal_menu_hover_box(&hx, &hy, &st) && st != hov_seen_f) {
            hov_seen_f = st;
            /* TITLEUI1: a hover on the online file select's ? or NAME plank lands the pad focus there (the arrow follows it, as the D-pad's step would) */
            if (hx == ONL_HELP_X && hy == ONL_HELP_Y) { menu_online_hover_focus(2, ONL_PF_HELP, q0[0]); return; }
            if (hx == ONL_NAME_X && hy == ONL_NAME_Y) { menu_online_hover_focus(2, ONL_PF_NAME, q0[0]); return; }
            int slot[24], ids[24], x0[24], y0[24], x1[24], y1[24];
            const int n = port_title_elem_boxes(slot, ids, x0, y0, x1, y1, 24);
            for (int k = 0; k < n; ++k) {
                if (ids[k] < 14 || ids[k] > 21 || x0[k] != hx || y0[k] != hy || !port_title_elem_live(slot[k])) continue;
                if (menu_online_padfocus()) menu_online_hover_focus(2, 0, q0[0]);      /* TITLEUI1: the arrow leaves the ? / NAME plank for the hovered element */
                if (q0[0] != ids[k] && q0[1] != ids[k]) {
                    if (hal_menu_trace_on()) fprintf(stderr, "[filecur] hover element %d: cursor %d -> %d (page %d/%d)\n", ids[k], (int)q0[0], ids[k], top, elem);
                    q0[1] = (short)ids[k];
                }
                break;
            }
        }
        return;
    }
    if (!data_ov007_02103360 || !port_title_page_state(&top, &elem, &req) || top != 0 || elem != 0 || req != -1) { stable = 0; return; }
    short *p0 = *(short **)data_ov007_02103360;
    if (!p0) return;
    if (stable < 20) { ++stable; return; }
    int hx, hy;
    unsigned st;
    if (hal_menu_hover_box(&hx, &hy, &st) && st != hov_seen) {
        hov_seen = st;
        /* TITLEUI1: the ? of the online title is a host plank the ROM's cursor cannot hold: a hover on it lands the pad focus there, so the arrow goes under
           it exactly as the D-pad's step from the wrench puts it (hal/menu_online.cpp menu_online_dir) */
        if (hx == ONL_HELP_X && hy == ONL_HELP_Y) { menu_online_hover_focus(1, ONL_PF_HELP, p0[0]); return; }
        static const int ids[4] = { 1, 0, 2, 7 };
        for (int k = 0; k < 4; ++k) {
            Box b;
            if (box_of_live(ids[k], &b) && b.x0 == hx && b.y0 == hy) {
                if (menu_online_padfocus()) menu_online_hover_focus(1, 0, p0[0]);      /* TITLEUI1: the wrench or a plank takes the arrow back from the ? */
                if (p0[0] != ids[k]) p0[1] = (short)ids[k];
                return;
            }
        }
    }
    if (p0[0] == 0xff) p0[1] = 0;
}

extern "C" void port_host_keys_clear(unsigned short mask);   /* hal/scene_boot.cpp */
static void title_pad_keys(uint16_t *pressed, int host_back, int inject);

/* EGG1: THE CLICK. Read once a frame on the logic thread from the pad map, beside the stylus seam rather than through it: the button is the
   mouse's own (GetAsyncKeyState, the same rule hal/sub_screen.cpp's poll_touch keeps: only while this window is the foreground one and only a
   press that began while it already was, so the click that brings the window to the front is no click), or the scripted stylus while one is
   latched (port_touch_inject_peek: the dev click scripts; nothing arms it in a kit). Only the press EDGE counts. The client point goes through
   the present rectangle to a picture pixel, and the spot the build wrote decides. The pad pointer (a controller-driven stylus) is not the
   mouse button and never gets here; neither do keys. */
extern "C" int port_touch_inject_peek(int *cx, int *cy);              /* hal/sub_screen.cpp */
extern "C" int hal_window_focused(void);                             /* hal/sub_screen.cpp */
extern "C" int hal_window_headless(void);                            /* hal/sub_screen.cpp (SEAMCLEAN1) */
extern "C" int hal_present_client_to_fb(int cx, int cy, int *fx, int *fy);
static void egg_click_poll()
{
    static int raw_was, focus_was, hold_ok, was_down;
    int down = 0, cx = 0, cy = 0, scripted = 0;
    if (port_touch_inject_peek(&cx, &cy)) {
        down = 1;
        scripted = 1;
    } else {
        int px = 0, py = 0;
        /* SEAMCLEAN1: no window, no mouse (hal_window_focused fails open there, so a real click in whatever window was in front
           would have been read by a windowless run) */
        if (hal_window_headless()) { was_down = 0; return; }
        const int raw = egg_mouse_read(&px, &py);
        const int foc = hal_window_focused();
        if (!raw)          hold_ok = 0;
        else if (!raw_was) hold_ok = foc && focus_was;
        else if (!foc)     hold_ok = 0;
        raw_was = raw;
        focus_was = foc;
        down = raw && foc && hold_ok;
        if (raw < 0) down = 0;      /* the button is down but the point could not be read */
        cx = px; cy = py;
    }
    const int edge = down && !was_down;
    was_down = down;
    if (!edge) return;
    int fx = -1, fy = -1;
    if (!hal_present_client_to_fb(cx, cy, &fx, &fy)) return;
    EggSpot e;
    { std::lock_guard<std::mutex> lk(g_egg_mu); e = g_egg; }
    int lw = 0, lh = 0;
    if (!e.on || !hal_menu_last_size(&lw, &lh) || lw != e.w || lh != e.h) return;
    if (fx < e.x || fx >= e.x + e.cw || fy < e.y || fy >= e.y + e.ch) return;
    for (int i = 0; i < e.nt; ++i) {
        const int *t = e.t[i];
        if (fx >= t[0] && fx < t[0] + t[2] && fy >= t[1] && fy < t[1] + t[3]) {
            fprintf(stderr, "[egg] click at picture (%d,%d) on the logo over touch piece (%d,%d,%d,%d): that piece has it\n", fx, fy, t[0], t[1], t[2], t[3]);
            return;
        }
    }
    if (settings_page_open()) return;
    const int r = egg_audio_play();
    fprintf(stderr, "[egg] %s click at picture (%d,%d) on the small logo: %s\n", scripted ? "scripted" : "mouse", fx, fy,
            r > 0 ? "clip started" : r == 0 ? "already playing, nothing" : "no clip");
}

void menu_title_pad_map(uint16_t *pressed, int host_back)
{
    egg_click_poll();      /* EGG1 */
    title_cursor_assist();
    /* ASSET5: THE ATTRACT'S TOUCH-TO-START, given by the port once the star is off the top of the lower screen (counter 56; SM64DS_TITLE_FF=0
       turns it off), for one frame as the DS A key edge the ROM's own skip reads; and a confirm during the swap finishes the choreography. */
    int inject = 0;
    {
        int top, elem, req;
        const bool got = port_title_page_state(&top, &elem, &req) != 0;
        if (got && top == 11 && cut_ready() && (*pressed & (MENU_PAD_CONFIRM | 0x0001 | 0x0008))) {
            cut_begin();
            hal_menu_touch_queue_press(128, 96);      /* a key / pad press starts the flight the way a touch on the star does */
        }
        if (got && top == 11 && g_cut_fire) {
            sv_restore();
            sv_press();
            func_ov007_020bde70(0x34, 0x1e);      /* the flight's own sound ends with the flight (the skip block stops 0x32 / 0x33, which are not the ones playing) */
            func_ov007_020bde70(0x32, 0x1e);
            func_ov007_020bde70(0x33, 0x1e);
            inject = 1;      /* the held word gets the A edge: the ROM's gate reads an edge */
            if (hal_menu_trace_on()) fprintf(stderr, "[chore] the skip is applied (flight tick %d)\n", g_cut_t);
        }
        if (g_ch.in_fam && (g_ch.sw_t < swap_end(g_ch) || intro_logo_busy(g_ch)) && ((*pressed & MENU_PAD_CONFIRM) || g_ch.skip)) {
            if (!g_ch.skip && hal_menu_trace_on()) fprintf(stderr, "[chore] confirm during the choreography: finishing it\n");
            g_ch.skip = 1;
        }
    }
    {
        /* NAME1: the online menu's keys. Back closes a screen of it or turns the online planks back; pad B on the file select is the back arrow, so
           it closes a screen first. The ? text opens from the mouse or the D-pad (settings_page_hooks.cpp). Everything else is the ROM's. */
        const int kind = g_last == V_MAIN ? 1 : g_last == V_FILES ? 2 : 0;
        if (host_back && hal_menu_trace_on()) fprintf(stderr, "[online] back key: page kind %d (variant %d) top %d elem %d\n", kind, g_last, g_top, g_elem);
        if (kind) {
            unsigned short *hh = (unsigned short *)((char *)data_020a0e58 + data_020a0e40 * 4);
            const int punch = kind == 2 && (*pressed & 0x0001);
            if (host_back && profile_backspace()) host_back = 0;      /* PROFILE1: Backspace edits the name field first (and is BACK on the popup's other rows) */
            if ((host_back || punch) && menu_online_back(kind)) {
                host_back = 0;
                if (punch) { *pressed = (uint16_t)(*pressed & ~0x0001u); *hh = (unsigned short)(*hh & ~0x0001u); port_host_keys_clear(0x0001); }
            }
        }
    }
    title_pad_keys(pressed, host_back, inject);
}

/* PADMENUS1: the title's key meaning, shared by the one-screen recipe (menu_title_pad_map above) and the ROM's own two-screen title (OneScreenMenus
   off, menu_title_pad_map_rom below): pad A is the ROM's confirm, pad B the back arrow on every page that has one. */
static void title_pad_keys(uint16_t *pressed, int host_back, int inject)
{
    /* The ROM's key record (src/func_ov007_020b3190.c: `*(u16*)p & ~*(u16*)(p+2)`, and the repeat counters of
       func_ov007_020b2e64) is built from the HELD word, so the pressed word alone is not enough: the held
       word moves 0x0002 -> 0x0001 too, and a host back is a one-frame B in it. */
    unsigned short *held = (unsigned short *)((char *)data_020a0e58 + data_020a0e40 * 4);
    unsigned short h = *held;
    /* ASSET6: PAD B (the port's punch binding, DS A bit 0x0001 natively; pad A arrives as 0x0002) is the back arrow on every page that has one
       (file select, file options, scores, copy / erase, doodle): the ROM has only a touch back, so it is the same host back Backspace gives.
       On the main page and the attract 0x0001 stays a confirm. */
    if ((g_last == V_FILES || g_last == V_OPTIONS || g_last == V_TABLE || g_last == V_DOODLE || g_last == V_TOY) && ((*pressed | h) & 0x0001)) {
        if (*pressed & 0x0001) host_back = 1;
        *pressed = (uint16_t)(*pressed & ~0x0001u);
        h = (unsigned short)(h & ~0x0001u);
        port_host_keys_clear(0x0001);     /* INPUTMATRIX1: the title also reads the host key word, which still held the B as the DS A key: B picked the file under the cursor */
    }
    /* INPUTMATRIX1: pad B (the DS A bit) is no confirm on the main page either: it is taken out of the held, pressed and host key words before pad A is converted
       below. (The one-screen page's online back is the hook's, hal/settings_page_hooks.cpp.) */
    if (g_last == V_MAIN && ((*pressed | h) & 0x0001)) {
        *pressed = (uint16_t)(*pressed & ~0x0001u);
        h = (unsigned short)(h & ~0x0001u);
        port_host_keys_clear(0x0001);
    }
    /* FACE3: the free-draw page has no pad handling in the ROM (touch only): the back key (Backspace, pad B) taps its Yoshi button, the ROM's own box 6, which leaves for the main menu */
    if (host_back && g_last == V_DOODLE && g_elem == 3) {
        Box b;
        if (box_of_live(6, &b)) hal_menu_touch_queue_press((b.x0 + b.x1) / 2, (b.y0 + b.y1) / 2);
    }
    if (h & MENU_PAD_CONFIRM) h = (unsigned short)((h & ~(unsigned)MENU_PAD_CONFIRM) | 0x0001);
    if (inject) h = (unsigned short)(h | 0x0001);
    if (host_back) h = (unsigned short)(h | 0x0002);
    *held = h;
    uint16_t k = *pressed;
    if (k & MENU_PAD_CONFIRM) k = (uint16_t)((k & ~(unsigned)MENU_PAD_CONFIRM) | 0x0001);
    if (host_back) k = (uint16_t)(k | 0x0002);
    *pressed = k;
}

/* PADMENUS1: the same pad meaning for the ROM's own two-screen title and file select (OneScreenMenus off), where no recipe is live: pad A confirms,
   pad B goes back on the pages that have a back arrow. The page is read off the ROM's own state (the recipe's per-tick answer is not running). The
   attract and choreography parts of the recipe's version are the recipe's own and stay out. */
extern "C" void menu_title_pad_map_rom(uint16_t *pressed, int host_back)
{
    int top, elem, req;
    if (!port_title_page_state(&top, &elem, &req)) return;
    title_cursor_assist();      /* the ROM cursor starts on ADVENTURE, as under the recipe: a first D-pad press moves it instead of waking it */
    if (top != g_top || elem != g_elem) {
        g_top = top; g_elem = elem;
        if (hal_menu_trace_on()) fprintf(stderr, "[menu] title page top=%d elem=%d req=%d (rom two-screen)\n", top, elem, req);
    }
    g_last = variant_for(top, elem);
    title_pad_keys(pressed, host_back, 0);
}

/* PADMENUS1: the file machine's sub pages (scores 3/5 and the table 2/5, copy 7/5, erase 9/5) read their back arrow by touch only, and the ROM takes the DS A key
   (the pad's B) there as a confirm on the file under the cursor. So the pad's B must never reach the game on those pages: the window loop asks this first, taps the back
   arrow itself (which sits where the file select's own back arrow does, hotspot box (180,140,72,36), the lower right) and takes the button out of the pad before the
   game's key words are built. Only while the title scene is up. */
extern "C" int menu_title_file_subpage(void)
{
    if (port_scene_live_id() != 1) return 0;
    return g_elem == 5 && (g_top == 3 || g_top == 2 || g_top == 7 || g_top == 9);
}
extern "C" void menu_title_file_subpage_back(void)
{
    const int q = hal_menu_touch_queue_press(180 + 36, 140 + 18);
    if (hal_menu_trace_on()) fprintf(stderr, "[menu] file sub page %d/%d: pad back taps the back arrow (queued %d)\n", g_top, g_elem, q);
}
