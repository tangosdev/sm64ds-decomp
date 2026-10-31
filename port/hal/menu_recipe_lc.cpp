/* menu_recipe_lc.cpp -- the LEVEL-CLEAR save menu as menu recipe #1 (run menu1, lane HOOK1).
 *
 * This is SaveMenuOnTop (the screen swap that lived in hal/sub_screen.cpp as swap_geom_for,
 * swap_present, swap_inverse, swap_now and hal_lc_menu_pad) moved into the generic compose
 * layer, with the same pixels, the same rectangles and the same stylus arithmetic. Nothing in
 * here is new behaviour; the migration is judged by a before/after picture compare.
 *
 *   TOP BANDS   two SRC_TOP_HOSTROW pieces: the course-clear lettering (engine-A BG3) moved
 *               to the very top of the picture and the coin total (OBJ) moved to the very
 *               bottom. They are host-row bands (hal/message_compositor.cpp lifts the pixels
 *               into a host-shaped plane, so the picture keeps what the 3D path drew under
 *               them) and only the rows move.
 *   PLATES      one SRC_SUB piece, the three button plates from a second engine-B scan-out of
 *               BG0 and BG1 alone (Stage::LC_Update case 2 turns exactly those on), plate rows
 *               only (menu_plate_row), the backdrop key dropped. Placement and sampling are
 *               the legacy rational (MenuPiece.lden), not one of the allowed steps, because
 *               byte identity with the 0.5.x picture is the gate.
 *   HOTSPOTS    the ROM's own three touch boxes, LC_Update case 3: DS rows 0x28, 0x50, 0x78,
 *               0x20 tall, any column (src/_ZN5Stage9LC_UpdateEv.cpp:175, :196, :217).
 *   STAGE       POST_FADE, and each piece takes engine A's fade itself (MenuPiece.fade_own, SAVEDIM1):
 *               the plates as the 3D under them does (the ROM's dim on the finished picture, as
 *               when they were drawn before the fade), the lifted lettering by the verdict each
 *               pixel carried out of the compositor. LC_Update case 2 leaves BG3 and OBJ out of
 *               the dim (src/game/stages/d_s_stage_methods.cpp:520), so the course name,
 *               CLEAR! / COMPLETE!, CONGRATULATIONS and the coin total stay bright at their new
 *               rows, as on the cartridge, while the 3D scene dims.
 *   VARIANT     1 + (new coin record) + 2 * (castle / secret-course star): the two ROM flags
 *               the band rows depend on, so build() is pure and the selftest can ask for any.
 *   DRIVER      ROM_CURSOR: up/down move the ROM's chosen plate (data_0209f2e0) through the
 *               ROM's own Stage::UpdateMenuButtons, A is handed to LC_Update as Start.
 */
#include "menu_compose.h"
#include "menu_hostimg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
extern unsigned char data_0209f20c;   /* "the level-clear screen owns the game" (Player.cpp:6522, :6718, :6742) */
extern unsigned char data_0209f2d4;   /* LC_Update's state word */
extern unsigned char data_0209f2b0;   /* NEW COIN RECORD flag: LC_Update case 1 sets it */
extern char data_02092124;            /* the course the screen is for */
extern int SublevelToLevel(int i);
extern unsigned char data_0209f2e0;   /* the chosen plate */
extern unsigned char data_0209f2b4;   /* how many buttons the menu has */
extern unsigned char data_0209f22c;   /* LC_Update's own wait */
extern unsigned char data_0209f244;   /* the chosen plate's blink */
extern int data_020a0db0[8];          /* the frame clock (hal/fader_wipes.cpp port_frame_clock_tick) */
void _ZN10dScStage_c17UpdateMenuButtonsEb(int b);
void func_02012790(int id);
int GetOwnerLanguage(void);
}

namespace {

const int SUB_W = 256, SUB_H = 192;

/* The bottom screen's plate span: the first plate's top row, and the row one past the third
   plate. The three offsets are LC_Update's own, out of case 3's touch tests. */
const int kMenuFirstRow = 0x28;
const int kMenuLastRow = 0x78 + 0x20;
const int kMenuRows = kMenuLastRow - kMenuFirstRow;

/* THE PLATES INSIDE THAT SPAN: the three touch boxes are 0x20 rows on a 0x28 pitch, and a plate
   is two rows taller than its box (a black edge and a rounded shadow row). The rows between
   plates carry the course map and are not drawn. (Measured, see git history of hal/sub_screen.cpp
   kPlateRows.) */
const int kPlatePitch = 0x28;
const int kPlateRows = 0x22;

int menu_plate_row(int sy)
{
    const int o = sy - kMenuFirstRow;
    return o >= 0 && o < kMenuRows && o % kPlatePitch < kPlateRows;
}

/* The top screen's two bands, in DS rows. Both measured. */
const int kTextRow0 = 0x22;      /* 34: one row above the COURSE N line */
const int kTextRow1 = 0x74;      /* 116: one row below CLEAR! */
const int kCoinRow0 = 0x7a;      /* 122 */
const int kCoinRow1 = 0x90;      /* 144 */
/* THE COIN BAND WHEN THE COURSE HAS A NEW COIN RECORD: LC_Render draws the "HIGH SCORE" banner as an
   OBJ at DS y 0x80 and Stage::Render moves the coin total down under it (rows 119..160). */
const int kCoinRow0Rec = 0x77;
const int kCoinRow1Rec = 0xa0;
/* A castle star or secret course (SublevelToLevel >= 0xf) places the total 0x10 rows higher
   (src/_ZN5Stage6RenderEv.cpp:146). */
const int kCoinHighShift = 0x10;
/* THE BOSS COURSES (levels 0xf..0x14: the Bowser arenas and the three key courses, Goomboss Battle among them): LC_Render also
   draws the CONGRATULATIONS lettering as an OBJ at DS y 0x38, glyph rows 50..62 (src/game/stages/d_s_stage_methods.cpp:388), and
   the course name and CLEAR! sit under it on BG3 (rows 67..78 and 84..94). The text block of such a course is lifted from the row
   above the lettering with both layers, so the three lines keep the cartridge's order and spacing, and it lands just under the
   HUD's top row of counters (DS rows 0..17, a few rows clear of them) instead of at the very top. */
const int kBossTextRow0 = 47;
const int kBossTextDst = 20;

/* the geometry, exactly swap_geom_for's */
struct LcGeom {
    int px, py, pw, ph;          /* the three plates' rectangle */
    int pnum, pden;              /* their size as a fraction of DS pixels */
    int ty_src, ty_dst, t_h;     /* the course-clear text block, at the top */
    int cy_src, cy_dst, c_h;     /* the coin total, at the very bottom */
    int t_lift_h;                /* the rows of the text band that are lifted and drawn */
};

LcGeom g_geom;                   /* the last build's, for the trace and the selftest */

void lc_geom_for(int w, int h, LcGeom *o, int record, int high, int boss)
{
    LcGeom g;
    memset(&g, 0, sizeof g);
    if (w < 8 || h < 8) { *o = g; return; }
    /* the bands follow the rows the glyphs are on: DS row r is the first host row at or after r * h / 192 */
    auto hrow = [h](int r) { return (r * h + SUB_H - 1) / SUB_H; };
    const int tr0 = boss ? kBossTextRow0 : kTextRow0;
    g.t_h = hrow(kTextRow1) - hrow(tr0);
    const int cr0 = (record ? kCoinRow0Rec : kCoinRow0) - (high ? kCoinHighShift : 0);
    const int cr1 = (record ? kCoinRow1Rec : kCoinRow1) - (high ? kCoinHighShift : 0);
    g.c_h = hrow(cr1) - hrow(cr0);
    if (g.t_h < 1) g.t_h = 1;
    if (g.c_h < 1) g.c_h = 1;
    /* a picture too short to hold both bands and a plate between them gives each band a third of it */
    if (g.t_h + g.c_h > h - 4) {
        g.t_h = h / 3;
        g.c_h = h / 3;
    }
    g.ty_src = hrow(tr0);
    if (g.ty_src + g.t_h > h) g.ty_src = h - g.t_h;
    if (g.ty_src < 0) g.ty_src = 0;
    g.ty_dst = boss ? hrow(kBossTextDst) : 0;
    g.cy_src = hrow(cr0);
    if (g.cy_src + g.c_h > h) g.cy_src = h - g.c_h;
    if (g.cy_src < 0) g.cy_src = 0;
    g.cy_dst = h - g.c_h;
    /* one overlay serves both bands: where the coin band begins above the text band's last row (the
       castle shift), the text band stops at the coin band's first row */
    g.t_lift_h = g.t_h;
    if (g.cy_src > g.ty_src && g.cy_src < g.ty_src + g.t_h)
        g.t_lift_h = g.cy_src - g.ty_src;
    /* THE MIDDLE, AND THE PLATES IN IT: the largest fraction that fits with 256:112 kept */
    const int my0 = g.ty_dst + g.t_h, mh = g.cy_dst - my0;
    if (mh < 4) { *o = g; return; }
    int num, den;
    if ((long long)w * kMenuRows <= (long long)mh * SUB_W) {
        num = w;
        den = SUB_W;
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
    g.pw = SUB_W * num / den;
    g.ph = kMenuRows * num / den;
    if (g.pw > w) g.pw = w;
    if (g.ph > mh) g.ph = mh;
    g.px = (w - g.pw) / 2;
    g.py = my0 + (mh - g.ph) / 2;
    if (g.px < 0) g.px = 0;
    if (g.py < my0) g.py = my0;
    *o = g;
}

/* THE PAIR: the level-clear screen owns the game and LC_Update is in one of the states that draws
   the menu (src/_ZN5Stage8BehaviorEv.cpp:206 runs LC_Update only while data_0209f20c is up). */
int lc_pair(void)
{
    return data_0209f20c != 0 && data_0209f2d4 >= 1 && data_0209f2d4 <= 3;
}


/* SHELF1 (S4): THE PLATES WAIT FOR THE DIM. LC_Update case 2 writes the dim (BLDY 7) and moves to state 3 in one tick, so on the
   cartridge the plates and the dim land together. The port eases that dim over kLcDimTicks of the frame clock
   (hal/fader_wipes.cpp lc_dim_ease, MENUS093B), and the plates used to stand at full strength while the picture was still bright.
   dim_progress() is 0 until state 3 is first seen, then climbs to 255 over the same number of ticks; up_lc / up_lc2 (called once a
   frame, before the layout is solved) keep it. 255 whenever the screen is past state 3 or was first seen there. */
const int kLcDimTicks = 12;                 /* the same number as lc_dim_ease's kLcEaseTicks */
int g_dim_clock = -1;                       /* the frame clock when state 3 was first seen; -1 = not seen on this screen */
int g_dim_state;                            /* the state word last time up_* ran */

int dim_progress(void);
void dim_track(void)
{
    const int st = (data_0209f20c != 0) ? (int)data_0209f2d4 : 0;
    if (st == 0 || st > 6) { g_dim_clock = -1; g_dim_state = 0; return; }
    if (st < 3) g_dim_clock = -1;
    else if (st == 3 && g_dim_state >= 1 && g_dim_state < 3) g_dim_clock = data_020a0db0[0];   /* the tick LC_Update case 2 wrote the dim */
    g_dim_state = st;
}

int dim_progress(void)
{
    if (data_0209f20c == 0) return 255;     /* no screen (the layout selftest, a warm-up solve): the plates at rest */
    const int st = (int)data_0209f2d4;
    if (st > 3 || st == 0) return 255;      /* pressed, saving or closing: the dim is long over */
    if (st < 3) return 0;                   /* the coin count and the 60 tick wait: the plates do not exist yet */
    if (g_dim_clock < 0) return 255;        /* first seen late (a loaded state): never hold the plates back */
    const int dt = data_020a0db0[0] - g_dim_clock;
    if (dt < 0 || dt >= kLcDimTicks) return 255;
    return dt * 255 / kLcDimTicks;
}

int g_cursor_shown;              /* the pad cursor: the first press only shows it */

int up_lc(int scene_id)
{
    dim_track();
    if (scene_id == MENU_SCENE_GATED || !lc_pair()) {
        g_cursor_shown = 0;      /* the menu is gone: the next one shows its cursor first */
        return 0;
    }
    const int record = data_0209f2b0 != 0;
    const int lvl = SublevelToLevel(data_02092124);
    const int high = lvl >= 0xf;
    return 1 + record + 2 * high + 4 * (lvl >= 0xf && lvl < 0x15);
}

void build_lc(int variant, MenuLayout *L)
{
    const int record = (variant - 1) & 1, high = ((variant - 1) >> 1) & 1, boss = ((variant - 1) >> 2) & 1;
    LcGeom g;
    lc_geom_for(L->w, L->h, &g, record, high, boss);
    g_geom = g;
    if (g.pnum <= 0) return;               /* no room: nothing is composed (np stays 0, so the recipe is not live) */

    /* the course-clear lettering, BG3, from its source rows to the top of the picture */
    MenuPiece t = menu_piece_make(SRC_TOP_HOSTROW, 0, g.ty_src, L->w, g.t_lift_h, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    t.layers = MENU_L_BG3;
    t.fade_own = 1;
    if (boss) {
        /* the CONGRATULATIONS OBJ travels with the name and CLEAR! (it stayed behind before and CLEAR! landed on it) */
        t.layers = MENU_L_BG3 | MENU_L_OBJ;
        t.lden = 1; t.lnum = 1;
        t.fx = 0; t.fy = (short)g.ty_dst; t.fw = (short)L->w; t.fh = (short)g.t_lift_h;
    }
    menu_add_piece(L, t);
    /* the coin total, OBJ, from its source rows to the very bottom */
    MenuPiece c = menu_piece_make(SRC_TOP_HOSTROW, 0, g.cy_src, L->w, g.c_h, MENU_AX_L, MENU_AY_B, 0, 0, 1, 1);
    c.layers = MENU_L_OBJ;
    c.fade_own = 1;
    menu_add_piece(L, c);
    /* SHELF1 (S4): the plates wait for the end of the port's dim ease (the cartridge dims and shows them in one tick) */
    if (dim_progress() < 255) {
        L->hide_owners = MENU_L_BG2;
        L->backdrop = MENU_BACKDROP_TOP_AS_IS;
        L->stage = MENU_STAGE_POST_FADE;
        return;
    }
    /* the three plates, BG0 | BG1 of engine B, placed and sampled by the legacy rational */
    MenuPiece p = menu_piece_make(SRC_SUB, 0, kMenuFirstRow, SUB_W, kMenuRows, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    p.layers = MENU_L_BG0 | MENU_L_BG1;
    p.key_backdrop = 1;
    p.row_ok = menu_plate_row;
    p.touch = 1;
    p.fade_own = 1;
    p.fx = (short)g.px; p.fy = (short)g.py; p.fw = (short)g.pw; p.fh = (short)g.ph;
    p.lnum = g.pnum; p.lden = g.pden;
    const int pi = menu_add_piece(L, p);
    for (int b = 0; b < 3; ++b) menu_add_hotspot(L, pi, 0, kMenuFirstRow + b * 0x28, SUB_W, 0x20, b);
    L->hide_owners = MENU_L_BG2;           /* "TOUCH TO SELECT", which the compose has never drawn */
    L->backdrop = MENU_BACKDROP_TOP_AS_IS;
    L->stage = MENU_STAGE_POST_FADE;
}

/* The ROM reads the menu with a pad only in LC_Update's case 3 and only when no wait is running: the
   level-clear flag up, the state word 3, data_0209f22c and the plate's blink both 0. Anything else
   leaves the pressed word alone. */
int pad_ok_lc(void)
{
    if (data_0209f20c == 0 || data_0209f2d4 != 3) {
        g_cursor_shown = 0;
        return 0;
    }
    return data_0209f22c == 0 && data_0209f244 == 0;
}

/* hal_lc_menu_pad's body, unchanged: the first press shows the cursor, later presses move the chosen
   plate one step and stop at either end, through the ROM's own UpdateMenuButtons and sound 0x12e
   (the idiom of the star select, src/_ZN12dScStarSel_c8BehaviorEv.cpp). */
int rom_cursor_lc(int dir, int *sel_out)
{
    if (dir != MENU_DIR_UP && dir != MENU_DIR_DOWN) return 0;
    const int last = data_0209f2b4 ? data_0209f2b4 - 1 : 0;
    int sel = data_0209f2e0;
    int moved = 0;
    if (!g_cursor_shown) {
        g_cursor_shown = 1;
        moved = 1;
    } else if (dir == MENU_DIR_UP && sel > 0) {
        --sel;
        moved = 1;
    } else if (dir == MENU_DIR_DOWN && sel < last) {
        ++sel;
        moved = 1;
    }
    if (moved) {
        data_0209f2e0 = (unsigned char)sel;
        _ZN10dScStage_c17UpdateMenuButtonsEb(0);
        host_menu_cursor_sound();
    }
    if (sel_out) *sel_out = sel;
    return moved;
}

}  // namespace

/* ====================================================================================================================
   RECIPE lc2: THE SAVE SCREEN IN N64 ORDER (run menu1, lane SAVE2), behind OneScreenMenus. SaveMenuOnTop alone is recipe lc
   above and is not touched.

   Tango: COURSE n, the star line, the coin total centred, the ROM's HIGH SCORE banner above when the coin record is beaten,
   then the three choices as high-res planks (the DS font, our art) underneath, smaller so the game shows through.

   WHAT IS MOVED, per layer, as host-row bands (the same mechanism as lc: hal/message_compositor.cpp lifts a band's pixels out
   of the picture and the compose puts them back where this recipe says; only rows move, the glyphs are the ROM's own):
     BG3 [35,64)   COURSE n and the course name        (glyph rows 36..46 and 52..62, measured)
     BG3 [82,95)   the star line with its star icon    (glyph rows 83..93)
     BG3 [97,114)  CLEAR!                              lifted and not drawn: the N64 order has no such line
                   (a castle star or secret course: no COURSE lines; the star line is [66,80) and CLEAR! [82,98))
     OBJ banner    the ROM's own HIGH SCORE art (LC_Render, the OAM at DS y 0x80; its flashing is the ROM's own palette
                   rotation, LC_Render's LoadOBJPltt on data_0209f2a8), only when data_0209f2b0 says the record was beaten
     OBJ coins     the coin total (Stage::Render: COIN, TIMES and the number, DS y 0x80, +0x10 under the banner)
     OBJ [162,192) the two bouncing arrows that point at the touch screen: lifted and not drawn (there is no touch screen)
   A castle star or secret course (SublevelToLevel >= 0xf) puts the total 0x10 rows higher (src/_ZN5Stage6RenderEv.cpp:146).
   (Rows measured on the 4x picture of the base exe, lvl6 star, with and without a record.)

   THE PLANKS replace the ROM's BG1 plates (not drawn). English only: the lettering is the DS font pre-rendered by
   tools/prepare_lc_art.py; any other language keeps the ROM's own plates (the localised lettering), scaled to 1/2, in the
   same place. Clicks map proportionally into the ROM's own three boxes (LC_Update case 3: x 8..247, rows 0x28, 0x50, 0x78,
   0x20 tall), so they reach the ROM exactly as the stylus would. The pad moves the ROM's own chosen plate
   (data_0209f2e0 through Stage::UpdateMenuButtons); the mouse does the same by hover (facts() below, logic thread). The
   game's small arrow stands over the chosen plank, centred.
   ==================================================================================================================== */
namespace {

int g_lc2_frame;
unsigned g_lc2_hov_seen;

int g_lc2_last;     /* the variant of the last live call; 0 when the screen is not up */
int g_lc2_hold;     /* 1 on the one frame the layout is held after the ROM's screen flag fell */

int up_lc2(int scene_id)
{
    /* up through the save itself (LC_Update states 4 and 5) and the closing state 6: the old two-screen top (CLEAR!, the touch
       arrows) must not come back for any of them */
    g_lc2_hold = 0;
    dim_track();
    if (scene_id != MENU_SCENE_GATED && data_0209f20c != 0 && data_0209f2d4 >= 1 && data_0209f2d4 <= 6) {
        const int lvl = SublevelToLevel(data_02092124);
        /* levels 0xf..0x14 (the Bowser arenas and the three key courses, Goomboss Battle among them): LC_Render also draws the
           CONGRATULATIONS lettering as an OBJ at DS y 0x38 (src/game/stages/d_s_stage_methods.cpp:388), which no band lifted */
        g_lc2_last = 1 + (data_0209f2b0 != 0) + 2 * (lvl >= 0xf) + 4 * (lvl >= 0xf && lvl < 0x15);
        return g_lc2_last;
    }
    /* the screen flag falls one frame before the picture has changed back: hold the layout for exactly that frame, once */
    if (g_lc2_last && scene_id != MENU_SCENE_GATED && data_0209f20c == 0) {
        const int v = g_lc2_last;
        g_lc2_last = 0;
        g_lc2_hold = 1;
        return v;
    }
    g_lc2_last = 0;
    return 0;
}

/* the ROM reads the pad in LC_Update case 3 only (see pad_ok_lc); the same gate for the hover cursor */
int lc2_cursor_live(void)
{
    return data_0209f20c != 0 && data_0209f2d4 == 3 && data_0209f22c == 0 && data_0209f244 == 0;
}

void set_chosen(int sel)
{
    const int last = data_0209f2b4 ? data_0209f2b4 - 1 : 0;
    if (sel < 0 || sel > last || sel == (int)data_0209f2e0) return;
    data_0209f2e0 = (unsigned char)sel;
    _ZN10dScStage_c17UpdateMenuButtonsEb(0);
}

/* the cursor starts on Save and Continue (the ROM's own: LC_Update case 0 sets data_0209f2e0 = 0), so the first press already moves */
int rom_cursor_lc2(int dir, int *sel_out)
{
    if (dir != MENU_DIR_UP && dir != MENU_DIR_DOWN) return 0;
    const int last = data_0209f2b4 ? data_0209f2b4 - 1 : 0;
    int sel = data_0209f2e0;
    int moved = 0;
    if (dir == MENU_DIR_UP && sel > 0) { --sel; moved = 1; }
    else if (dir == MENU_DIR_DOWN && sel < last) { ++sel; moved = 1; }
    if (moved) {
        data_0209f2e0 = (unsigned char)sel;
        _ZN10dScStage_c17UpdateMenuButtonsEb(0);
        host_menu_cursor_sound();
    }
    if (sel_out) *sel_out = sel;
    return moved;
}

void lc2_facts(MenuFacts *f)
{
    f->language = (int16_t)GetOwnerLanguage();
    /* MOUSE HOVER: a pointer that moved onto a hotspot makes its plate the ROM's chosen one (silent), so the lit plank, the
       arrow and a later Enter all name the same plate */
    int hx, hy;
    unsigned st;
    if (hal_menu_hover_box(&hx, &hy, &st) && st != g_lc2_hov_seen) {
        g_lc2_hov_seen = st;
        if (lc2_cursor_live() && hy >= kMenuFirstRow && hy < kMenuLastRow) set_chosen((hy - kMenuFirstRow) / kPlatePitch);
    }
    f->spare[0] = data_0209f2e0;
    f->spare[1] = ++g_lc2_frame;
    f->spare[3] = g_lc2_hold;          /* the held last frame after the screen flag fell: text and total only */
    f->spare[2] = data_0209f2d4;       /* the ROM's state word: the plates exist from state 2 on (case 1 is the coin count) */
    f->spare[4] = data_0209f22c;       /* SHELF1 (S1): LC_Update's own wait; after a press (states 4 and 6) it is the plate's pressed hold */
    f->spare[5] = dim_progress();      /* SHELF1 (S4): 0..255, the port's dim ease; the plates fade in with it */
}

void build_lc2(int variant, MenuLayout *L)
{
    const int record = (variant - 1) & 1, high = ((variant - 1) >> 1) & 1, boss = ((variant - 1) >> 2) & 1;
    const int W = L->w, H = L->h;
    if (W < 96 || H < 96) return;
    auto U = [L](int u) { return (int)((long long)u * L->unum / L->uden); };
    auto hrow = [H](int r) { return (r * H + SUB_H - 1) / SUB_H; };
    const int sh = high ? kCoinHighShift : 0;
    const bool art = L->facts.language <= 1 && menu_hostimg_ready(MENU_IMG_BTN_SAVE_CONT) &&
                     menu_hostimg_ready(MENU_IMG_BTN_SAVE_QUIT) && menu_hostimg_ready(MENU_IMG_BTN_CONT);   /* our planks, else the ROM's plates */
    const bool arrow = menu_hostimg_ready(MENU_IMG_ARROW_DOWN);
    const int sel = (L->facts.spare[0] >= 0 && L->facts.spare[0] <= 2) ? L->facts.spare[0] : 0;
    const int state = L->facts.spare[2];         /* the ROM's LC_Update state; 0 = unknown, as in the selftest */
    const bool saving = state == 5;              /* the save runs: the ROM's status box instead of the plates (state 4, 9 frames after the press, still has the plates) */
    /* state 1 counts the coins and 6 closes: text and total only, no plates. SHELF1 (S1): state 6 right after a press on Continue still
       holds the plate for the ROM's 8 tick wait (data_0209f22c), pressed; the save that ends in 6 has no wait left, so no plates then.
       SHELF1 (S4): nothing before the dim has started (spare[5] 0 is state 2, whose plates the ROM has not enabled yet) */
    const int wait = L->facts.spare[4];
    const int dimp = L->facts.spare[5];
    const bool show = state != 1 && (state != 6 || wait != 0) && !saving && !L->facts.spare[3] && dimp > 0;
    const bool pressed = wait != 0 && (state == 4 || state == 6);
    const bool planks = show && art;

    /* the source bands, DS rows */
    /* a castle star or secret course has no COURSE / name lines: just the star line (glyph rows 67..78) and CLEAR! (84..94) */
    const int a0 = 35, a1 = 64;
    const int b0 = high ? 66 : 82, b1 = high ? 80 : 95, c0 = high ? 82 : 97, c1 = high ? 98 : 114;
    const int ban0 = 0x77 - sh, ban1 = 0x8d - sh;                       /* HIGH SCORE (record only) */
    const int coin0 = (record ? 0x8d : 0x7a) - sh, coin1 = (record ? 0xa1 : 0x90) - sh;
    const int arr0 = 162, arr1 = 192;
    const int hA = high ? 0 : hrow(a1) - hrow(a0), hB = hrow(b1) - hrow(b0);
    /* CONGRATULATIONS (boss courses): glyph rows 50..62, the band stops where the star line's band starts */
    const int cg0 = 47, cg1 = b0;
    const int hCg = boss ? hrow(cg1) - hrow(cg0) : 0;
    const int hBan = record ? hrow(ban1) - hrow(ban0) : 0, hCoin = hrow(coin1) - hrow(coin0);
    /* the plates: one size, a stack */
    const int pw = U(104), ph = pw * 2 / 15, pgap = U(5);
    int plates_h;
    int fbw = 0, fbh = 0;                                               /* the ROM plates' dest size (non-English) */
    if (art) {
        plates_h = 3 * ph + 2 * pgap;
    } else {
        fbw = U(256) / 2;
        fbh = U(kMenuRows) / 2;
        plates_h = fbh;
    }
    /* the ROM's own status box ("Saving file..." / "File saved.") that takes the plates' place while the save runs: drawn at a whole
       number of host pixels per DS pixel, the nearest to 2/3 of a DS pixel's size and never under one (the DS font is one pixel thin,
       and a box drawn under 1:1 dropped whole columns of it: at RenderScale 1 "File saved." lost the F's stem). The same size as
       before at RenderScale 3. Where the box is taller than the plates and a gap (RenderScale 1), the column keeps room for it the
       whole time, so nothing moves when the save starts. */
    const int boxk = ((4 * L->unum + 3 * L->uden) / (6 * L->uden)) > 1 ? (4 * L->unum + 3 * L->uden) / (6 * L->uden) : 1;
    if (80 * boxk - U(9) > plates_h) plates_h = 80 * boxk - U(9);         /* U(9) = g4, the gap over the plates */
    const int g1 = U(3), g2 = high ? 0 : U(3), g3 = U(5), g4 = U(9);
    int T = hA + g2 + hB + g3 + hCoin + g4 + plates_h;
    if (record) T += hBan + g1;
    if (boss) T += hCg + g1;
    int y = (H - T) / 2;
    if (y < U(4)) y = U(4);

    auto band = [&](uint8_t layers, int r0, int r1, int dest_y, bool draw) {
        MenuPiece b = menu_piece_make(SRC_TOP_HOSTROW, 0, hrow(r0), W, hrow(r1) - hrow(r0), MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        b.layers = layers;
        b.bg = draw ? 0 : 1;           /* the hidden bands sit on the others' rows and are never drawn, so they alone are exempt from the overlap rule */
        b.nodraw = draw ? 0 : 1;
        b.lden = 1; b.lnum = 1;
        b.fx = 0; b.fy = (short)dest_y; b.fw = (short)W; b.fh = (short)b.s.h;
        menu_add_piece(L, b);
    };
    band(MENU_L_BG3, c0, c1, 0, false);                                 /* CLEAR! */
    band(MENU_L_OBJ, arr0, arr1, 0, false);                             /* the touch-screen arrows */
    int yy = y;
    if (record) { band(MENU_L_OBJ, ban0, ban1, yy, true); yy += hBan + g1; }
    if (boss) { band(MENU_L_OBJ, cg0, cg1, yy, true); yy += hCg + g1; }     /* CONGRATULATIONS above the star line, the DS order */
    if (!high) { band(MENU_L_BG3, a0, a1, yy, true); yy += hA + g2; }
    band(MENU_L_BG3, b0, b1, yy, true); yy += hB + g3;
    band(MENU_L_OBJ, coin0, coin1, yy, true); yy += hCoin + g4;
    const int py0 = yy;

    int hs_top[3] = { 0, 0, 0 };
    const int hs_cx = W / 2;
    if (!show) {
        /* the coin count: no plates and no arrow yet */
    } else if (planks) {
        static const int kImg[3] = { MENU_IMG_BTN_SAVE_CONT, MENU_IMG_BTN_SAVE_QUIT, MENU_IMG_BTN_CONT };
        for (int k = 0; k < 3; ++k) {
            MenuPiece pl = menu_piece_make(SRC_HOSTIMG, 8, kMenuFirstRow + k * kPlatePitch, 240, 0x20, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
            pl.hostimg = (uint8_t)kImg[k];
            pl.touch = 1;
            pl.lden = 1; pl.lnum = 1;
            pl.fw = (short)pw; pl.fh = (short)ph;
            pl.fx = (short)((W - pw) / 2);
            pl.fy = (short)(py0 + k * (ph + pgap));
            hs_top[k] = pl.fy;
            if (k == sel) pl.hl = (uint8_t)(1 | (pressed ? 2 : 0));      /* S1: sunk and darker while the ROM's press wait runs, like the title plates */
            if (dimp < 255) pl.alpha = (uint8_t)(dimp < 1 ? 1 : dimp);   /* S4: fades in with the dim */
            const int ip = menu_add_piece(L, pl);
            menu_add_hotspot(L, ip, 8, kMenuFirstRow + k * kPlatePitch, 240, 0x20, k);
        }
    } else if (dimp >= 255) {
        /* the ROM's own plates at 1/2, host-fixed like lc's (S4: no fade for these, they wait for the end of the dim): dest pixel -> source = floor(pixel * 2 uden / unum) */
        MenuPiece p = menu_piece_make(SRC_SUB, 0, kMenuFirstRow, SUB_W, kMenuRows, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        p.layers = MENU_L_BG0 | MENU_L_BG1;
        p.key_backdrop = 1;
        p.row_ok = menu_plate_row;
        p.touch = 1;
        p.fx = (short)((W - fbw) / 2); p.fy = (short)py0; p.fw = (short)fbw; p.fh = (short)fbh;
        p.lnum = L->unum; p.lden = 2 * L->uden;
        const int pi = menu_add_piece(L, p);
        for (int b = 0; b < 3; ++b) {
            menu_add_hotspot(L, pi, 0, kMenuFirstRow + b * kPlatePitch, SUB_W, 0x20, b);
            hs_top[b] = py0 + (b * kPlatePitch) * fbh / kMenuRows;
        }
    }
    if (arrow && show && (planks || dimp >= 255)) {
        static const int kBob[8] = { 0, 1, 2, 3, 4, 3, 2, 1 };
        const int bob2 = kBob[L->facts.spare[1] & 7];
        int iw = 1, ih = 1;
        menu_hostimg_size(MENU_IMG_ARROW_DOWN, &iw, &ih);
        const int aw = U(10), ah = (int)((long long)aw * ih / iw);
        MenuPiece ar = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        ar.bg = 1; ar.hostimg = MENU_IMG_ARROW_DOWN; ar.lden = 1; ar.lnum = 1; ar.fw = (short)aw; ar.fh = (short)ah;
        if (dimp < 255) ar.alpha = (uint8_t)(dimp < 1 ? 1 : dimp);
        ar.fx = (short)(hs_cx - aw / 2);
        ar.fy = (short)(hs_top[sel] + (2 + bob2) * L->unum / (2 * L->uden) - ah);
        menu_add_piece(L, ar);
    }
    if (saving) {
        /* the ROM's own "Saving file..." box, bottom screen (56,40,144,80, the same box the pause menu's save shows), where the plates were */
        MenuPiece sp = menu_piece_make(SRC_SUB, 56, 40, 144, 80, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        sp.layers = MENU_L_BG0 | MENU_L_BG1 | MENU_L_OBJ;   /* the box and its text; never the minimap behind it */
        sp.key_backdrop = 1;
        const int sw = 144 * boxk, sh2 = 80 * boxk;                      /* boxk host px per DS px (above) */
        sp.fx = (short)((W - sw) / 2); sp.fy = (short)(py0 + (plates_h - sh2) / 2); sp.fw = (short)sw; sp.fh = (short)sh2;
        sp.lnum = boxk; sp.lden = 1;
        menu_add_piece(L, sp);
    }
    L->no_focus_paint = 1;                 /* the lit plank and the arrow are the selection */
    L->hide_owners = MENU_L_BG2;           /* "TOUCH TO SELECT" */
    L->backdrop = MENU_BACKDROP_TOP_AS_IS;
    L->stage = MENU_STAGE_POST_FADE;
}

}  // namespace

int  menu_lc2_up(int scene_id) { return up_lc2(scene_id); }
void menu_lc2_build(int variant, MenuLayout *L) { build_lc2(variant, L); }
int  menu_lc2_rom_cursor(int dir, int *sel) { return rom_cursor_lc2(dir, sel); }
void menu_lc2_facts(MenuFacts *f) { lc2_facts(f); }

/* the recipe's four functions, named in the table by menu_recipes.cpp (function pointers, so the table stays a
   constant initialiser and nothing here depends on static-initialisation order) */
int  menu_lc_up(int scene_id) { return up_lc(scene_id); }
void menu_lc_build(int variant, MenuLayout *L) { build_lc(variant, L); }
int  menu_lc_rom_cursor(int dir, int *sel) { return rom_cursor_lc(dir, sel); }
int  menu_lc_pad_ok(void) { return pad_ok_lc(); }

/* SM64DS_SWAP_TRACE / SM64DS_MENU_TRACE: every change of the ROM's words and of the compose's own answer, with
   the rectangles this frame settled on, in the line format the capture tools already read. `composed` is
   hal_menu_live() for this frame. */
extern "C" void hal_lc_trace(int w, int h, int frame, int composed)
{
    if (!hal_menu_trace_on()) return;
    static int last[4] = { -1, -1, -1, -1 };
    const int now[4] = { (int)data_0209f20c, (int)data_0209f2d4, composed, (int)data_0209f2e0 };
    if (now[0] == last[0] && now[1] == last[1] && now[2] == last[2] && now[3] == last[3]) return;
    memcpy(last, now, sizeof last);
    /* the rectangles are computed fresh for this line, from this frame's ROM flags, exactly as the old swap trace
       did every frame (g_geom is only the last BUILD's, and a frame that is not composed builds nothing) */
    LcGeom g;
    const int lvl = data_0209f20c != 0 ? SublevelToLevel(data_02092124) : 0;
    lc_geom_for(w, h, &g, data_0209f2b0 != 0, lvl >= 0xf, lvl >= 0xf && lvl < 0x15);
    fprintf(stderr, "[compose] f%d f20c=%d f2d4=%d f2e0=%d composed=%d picture "
            "%dx%d text 0,%d %dx%d from row %d  plates %d,%d %dx%d "
            "(%d/%d)  coins 0,%d %dx%d from row %d\n", frame, now[0],
            now[1], now[3], now[2], w, h, g.ty_dst, w, g.t_h,
            g.ty_src, g.px, g.py, g.pw, g.ph, g.pnum, g.pden, g.cy_dst, w, g.c_h, g.cy_src);
    fflush(stderr);
}

/* ---- THE COMPOSED ARM OF SM64DS_LAYOUT_SELFTEST (the lc row of the selftest) -----------------------------------
 *
 * Asserts the composed picture over the shapes the option can be asked for: both framebuffer tiers and the five
 * aspects the launcher offers. It solves the REAL recipe and inverts through the REAL menu_inverse, the two
 * functions the draw and the stylus run, and needs no game state (variant 1).
 *   S1 the four corners of the PLATE rectangle invert to within ONE DS pixel of DS (0,0x28), (255,0x28), (0,0x97),
 *      (255,0x97) and read INSIDE.
 *   S2 one pixel outside each of its four edges reads OUTSIDE.
 *   S3 the plate rectangle keeps 256:112 within a pixel, fits between the two text bands, and the bands neither
 *      overlap each other nor leave the picture.
 *   S4 each of the menu's own touch boxes lands inside the plate rectangle and its centre inverts back into that
 *      same DS box. */
extern "C" int hal_menu_lc_selftest(void)
{
    static const double asp[5] = { 0.0, 1.7777778, 2.3333333, 3.5555556, 4.0 };
    static const int hs[2] = { 384, 768 };
    int passed = 0, total = 0;
    static MenuLayout L;
    const MenuRecipe *lc = 0;
    for (int i = 0; i < g_menu_nrecipes; ++i)
        if (strcmp(g_menu_recipes[i].name, "lc") == 0) lc = &g_menu_recipes[i];
    if (!lc) return 1;
    for (int hi = 0; hi < 2; ++hi) {
        for (int ai = 0; ai < 5; ++ai) {
            const int h = hs[hi];
            int w = asp[ai] > 0.0 ? (int)(h * asp[ai] + 0.5) : h * 4 / 3;
            w &= ~1;
            menu_layout_solve(&L, lc, 1, w, h);
            const LcGeom g = g_geom;
            int pidx = -1;
            for (int i = 0; i < L.np; ++i) if (L.p[i].src == SRC_SUB) pidx = i;
            char fails[8];
            int nf = 0, dx = 0, dy = 0, in = 0, ok = 1;
            auto inv = [&](int bx, int by) { int drag = -1; menu_inverse(&L, bx, by, &drag, &dx, &dy, &in); };
            const int x1 = g.px + g.pw - 1, y1 = g.py + g.ph - 1;
            if (pidx < 0 || g.pnum < 1) { ok = 0; }
            else {
                inv(g.px, g.py);
                if (!in || dx > 1 || dy > kMenuFirstRow + 1) ok = 0;
                inv(x1, g.py);
                if (!in || dx < SUB_W - 2 || dy > kMenuFirstRow + 1) ok = 0;
                inv(g.px, y1);
                if (!in || dx > 1 || dy < kMenuLastRow - 2) ok = 0;
                inv(x1, y1);
                if (!in || dx < SUB_W - 2 || dy < kMenuLastRow - 2) ok = 0;
            }
            if (!ok) fails[nf++] = '1';
            ok = 1;
            if (pidx < 0) ok = 0;
            else {
                inv(g.px - 1, g.py);
                if (in) ok = 0;
                inv(g.px, g.py - 1);
                if (in) ok = 0;
                inv(x1 + 1, g.py);
                if (in) ok = 0;
                inv(g.px, y1 + 1);
                if (in) ok = 0;
            }
            if (!ok) fails[nf++] = '2';
            ok = 1;
            if (g.pw < 1 || g.ph < 1) ok = 0;
            else {
                const long long l = (long long)g.pw * kMenuRows;
                const long long r = (long long)g.ph * SUB_W;
                const long long tol = (long long)SUB_W + kMenuRows;
                if (l - r > tol || r - l > tol) ok = 0;
            }
            if (g.px < 0 || g.py < g.t_h || g.px + g.pw > w || g.py + g.ph > g.cy_dst) ok = 0;
            if (g.t_h < 1 || g.c_h < 1 || g.ty_dst != 0 || g.cy_dst + g.c_h != h || g.ty_src + g.t_h > h ||
                g.cy_src + g.c_h > h)
                ok = 0;
            if (!ok) fails[nf++] = '3';
            ok = 1;
            for (int b = 0; b < 3 && pidx >= 0; ++b) {
                const int d0 = kMenuFirstRow + b * 0x28;
                const int d1 = d0 + 0x20 - 1;
                const int r0 = g.py + (d0 - kMenuFirstRow) * g.pnum / g.pden;
                const int r1 = g.py + (d1 + 1 - kMenuFirstRow) * g.pnum / g.pden - 1;
                if (r0 < g.py || r1 > g.py + g.ph - 1 || r1 < r0) ok = 0;
                inv(g.px + g.pw / 2, (r0 + r1) / 2);
                if (!in || dy < d0 || dy > d1 || dx < 0 || dx >= SUB_W) ok = 0;
            }
            if (!ok) fails[nf++] = '4';
            fails[nf] = 0;
            ++total;
            if (!nf) {
                ++passed;
                fprintf(stderr, "[composest] picture %dx%d: text 0,0 "
                        "%dx%d from row %d  plates %d,%d %dx%d (%d/%d)  "
                        "coins 0,%d %dx%d from row %d  S1..S4 PASS\n",
                        w, h, w, g.t_h, g.ty_src, g.px, g.py, g.pw, g.ph,
                        g.pnum, g.pden, g.cy_dst, w, g.c_h, g.cy_src);
            } else {
                fprintf(stderr, "[composest] picture %dx%d: text 0,0 "
                        "%dx%d from row %d  plates %d,%d %dx%d (%d/%d)  "
                        "coins 0,%d %dx%d from row %d  FAIL S%s\n",
                        w, h, w, g.t_h, g.ty_src, g.px, g.py, g.pw, g.ph,
                        g.pnum, g.pden, g.cy_dst, w, g.c_h, g.cy_src,
                        fails);
            }
        }
    }
    fprintf(stderr, "COMPOSED LAYOUT SELFTEST: %s %d/%d\n", passed == total ? "PASS" : "FAIL", passed, total);
    fflush(stderr);
    return passed == total ? 0 : 1;
}
