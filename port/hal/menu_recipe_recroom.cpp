/* menu_recipe_recroom.cpp -- the REC ROOM select as one-screen menu recipe "recroom" (run menu1, lane RECROOM1).
 *
 * Tango (10-09): "we literally only need the bottom screen". Scene 5 (dScMiniGm_c, ov005; hal/scene_mg_menu.cpp seats
 * it). The upper screen (Toad's room, the "TOUCH TO SELECT" band) is not shown at all; the lower screen (engine B, the
 * unswapped layout: DISPCNT_B 00011d00, BG0 cards, BG2 the tab column's strip, BG3 the star pattern, OBJ tabs, star
 * counts, NEW! stickers, the page arrow and the back arrow; BG1 is off) is the whole menu.
 *
 * LAYOUT (canvas = 192 units tall, 256 wide at 4:3; nothing is stretched, every piece is 1:1):
 *   - the background is the BG3 star pattern ALONE, as SRC_SUB pieces of 224 columns tiled across the canvas from
 *     its left edge (the pattern's period is 32 columns and divides 224, so a copy is seamless at every join); the last
 *     copy is one unit wide of the exact edge (a piece may overrun by a unit and is clamped), so no sliver of the
 *     hidden upper screen shows;
 *   - on top, ONE piece: BG0 + BG2 + OBJ of the whole lower screen, the engine-B backdrop keyed out, so the cards,
 *     tab column, stickers, arrows and star counts keep their own transparency and drag no background along. It
 *     is centred (the DS arrangement). On a wide picture that leaves the pattern continuing out to both sides.
 *     DECISION (RECROOM1): centred, not tabs-left / arrow-right. The page scroll slides the cards sideways under a
 *     clip at the screen's own edges (left: under the tab column, which is the ROM's own look; right: the 256th
 *     column). Split pieces would add a second invisible clip in the middle of the wide picture; one block keeps
 *     the ROM's motion exactly and the block edges are the DS screen's.
 *
 * INPUT. The Rec Room is touch only in the ROM. Hotspots are the ROM's own boxes, cited per line below; they follow the
 * live page: the card cells that the ROM would accept (MiniGm_IsUnlocked: the first two of a group are free, the rest
 * need an unlock bit), the page arrow only where the ROM draws and accepts it (not while a page flip is running). Driver
 * TOUCH: the pad / keys move a focus over the hotspots (automatic spatial navigation), A synthesises the touch at
 * the focused box's centre, B / Backspace presses the back arrow (MenuLayout.back). Mouse clicks map through the touch
 * piece to the ROM's boxes exactly (1:1). Focus highlight: the core's own (the focused item's pixels brightened and
 * outlined) plus the title's blue arrow over it (the game's own art).
 */
#include "menu_compose.h"
#include "menu_hostimg.h"

#include <string.h>
#include <stdio.h>

extern "C" {
void *port_scene_live_object(void);
int  port_scene_live_id(void);
int  MiniGm_IsUnlocked(void *self, int n);
int  MiniGm_HasSecondPage(void *self);
extern unsigned char data_020a0e40;     /* the local player's slot */
extern int data_020a0e58[];             /* PadData[i]: {u16 held, u16 pressed} */
extern unsigned char gMgMenuPage[];   /* the page mode: 0 = the 2 x 3 page, else the single row of three */
extern int gMgGroupBase;               /* the highlighted group's base index (0..3 = Yoshi / Mario / Luigi / Wario) */
}

namespace {

const int kScene = 5;
int g_foc_have, g_foc_x, g_foc_y;      /* the focused hotspot's box origin, read once a frame by menu_recroom_pad_ok (the core resets the focus while it solves the layout, so build cannot ask for it) */
const int kBackX = 0xd8, kBackY = 0x90, kBackW = 0x21, kBackH = 0x21;   /* src/MiniGm_UpdateExitButton.cpp:25-31 */

/* INT2 (the decide / draw split): what build() needs of the live scene, read once per tick by menu_recroom_facts on the logic thread
   (MenuLayout.facts.blob); build() reads this and no ROM address. */
struct RecFacts {
    int have;            /* the live scene object answered */
    int mode;            /* gMgMenuPage[0] */
    int cells;           /* bit i: the ROM accepts the cell (mode 0: row * 3 + col; else col) */
    int arrow;           /* the page arrow is drawn and accepted */
    int flip;            /* ANCHOR1: the cards are off their resting scroll, a page slide is running */
    int foc_have, foc_x, foc_y;
    int calls;           /* a per-tick counter for the arrow's bob */
};
static_assert(sizeof(RecFacts) <= sizeof(MenuFacts::blob), "RecFacts must fit MenuFacts.blob");

inline int ow(const void *o, int off) { return *(const int *)((const char *)o + off); }
inline int ob(const void *o, int off) { return *(const unsigned char *)((const char *)o + off); }

}  // namespace

extern "C++" {

int menu_recroom_up(int scene_id)
{
    return scene_id == kScene ? 1 : 0;
}

void menu_recroom_facts(MenuFacts *f)
{
    static int calls;
    RecFacts r;
    memset(&r, 0, sizeof r);
    const void *o = port_scene_live_id() == kScene ? port_scene_live_object() : 0;
    r.have = o != 0;
    r.mode = o ? gMgMenuPage[0] : 0;
    const int base = o ? gMgGroupBase : 0;
    if (o) {
        if (r.mode == 0) {
            for (int row = 0; row < 2; ++row)
                for (int col = 0; col < 3; ++col)
                    if (MiniGm_IsUnlocked((void *)o, base + row * 0xc + col * 4)) r.cells |= 1 << (row * 3 + col);
        } else {
            for (int col = 0; col < 3; ++col)
                if (MiniGm_IsUnlocked((void *)o, base + 0x18 + col * 4)) r.cells |= 1 << col;
        }
        r.arrow = ob(o, 0x54) == 0 && (r.mode != 0 || MiniGm_HasSecondPage((void *)o) != 0);
        r.flip = ow(o, 0x50) != (r.mode != 0 ? 0xb0 : 0);        /* mSubBgScrollX (src/minigames/d_s_mini_gm.cpp MiniGm_UpdatePageScroll): off the page's resting scroll (0 page one, 0xb0 page two) = the cards are sliding */
    }
    r.foc_have = g_foc_have; r.foc_x = g_foc_x; r.foc_y = g_foc_y;
    r.calls = ++calls;
    if (hal_menu_trace_on()) { static int lf = -1; if (r.flip != lf) { lf = r.flip; fprintf(stderr, "[recroom] f%d slide flag %d\n", calls, r.flip); } }
    if (hal_menu_trace_on()) {
        static int last = -1;
        const int bd = *(volatile unsigned short *)0x05000400u;      /* the engine-B backdrop the key_backdrop piece drops */
        if (bd != last) { last = bd; fprintf(stderr, "[recroom] f%d engine B backdrop (palette 0) = %04x\n", calls, bd); }
    }
    memcpy(f->blob, &r, sizeof r);
}

void menu_recroom_build(int, MenuLayout *L)
{
    const RecFacts &R = *(const RecFacts *)L->facts.blob;
    const bool o = R.have != 0;
    const int cw = L->w * L->uden / L->unum;               /* exact canvas width in units, floored */

    const bool one = cw <= 256;     /* the DS screen's own width (4:3): the screen itself is the whole picture */
    if (!one) {
    /* the star pattern, BG3 alone, tiled from the left edge in 224 column copies of source columns 32..255: the pattern
       repeats every 32 columns and 224 = 7 x 32, so a copy joins the next seamlessly; the leftmost 8 source columns are
       not part of the repeat (measured, they differ from columns 32..39) and are never used */
    for (int x = 0; x < cw; x += 224) {
        const int sw = cw - x < 224 ? cw - x : 224;        /* whole units: the last copy ends on a whole unit <= the edge */
        MenuPiece t = menu_piece_make(SRC_SUB, 32, 0, sw, 192, MENU_AX_L, MENU_AY_T, x, 0, 1, 1);
        t.layers = MENU_L_BG3;
        t.bg = 1;
        menu_add_piece(L, t);
    }
    /* the picture is rarely a whole number of units wide; the sliver past the last whole unit (under one unit) is
       covered by a 2 unit piece on the right edge, so no strip of the hidden upper screen shows. Its source columns
       are the pattern's phase at that place modulo its 32 column period (the join is within a fraction of a unit). */
    {
        MenuPiece e = menu_piece_make(SRC_SUB, 32 + (cw - 2) % 32, 0, 2, 192, MENU_AX_R, MENU_AY_T, 0, 0, 1, 1);
        e.layers = MENU_L_BG3;
        e.bg = 1;
        menu_add_piece(L, e);
    }
    }

    /* the menu itself: tab column, cards, stickers, star counts, arrows, centred. Wide: BG0 + BG2 + OBJ with the
       engine-B backdrop keyed out, over the pattern. 4:3: every layer the ROM has on (BG0, BG2, BG3, OBJ = the live
       enables, so no extra scan-out), opaque, the DS screen as it is. */
    MenuPiece m = menu_piece_make(SRC_SUB, 0, 0, 256, 192, MENU_AX_C, MENU_AY_M, 0, 0, 1, 1);
    m.layers = one ? (MENU_L_BG0 | MENU_L_BG2 | MENU_L_BG3 | MENU_L_OBJ) : (MENU_L_BG0 | MENU_L_BG2 | MENU_L_OBJ);
    m.key_backdrop = one ? 0 : 1;
    L->key_sentinel = one ? 0 : 1;      /* MENUS093E: key the exact undrawn pixels, not the colour of palette entry 0 (black after a minigame took the cards' outlines with it) */
    m.touch = 1;
    m.bg = 1;                                              /* it overlaps the pattern pieces by design */
    const int mode = R.mode;
    /* ANCHOR1: on a wide picture the corner controls ride the WINDOW's edges (Tango 10-24, the corner-control rule), the cards stay
       centred. The one block is cut into pieces of the same layers: the tab column at the left edge (DS x 0 .. kLw; on page two the
       page arrow is beside it), the page arrow and the back arrow at the right edge (DS x 0xd0 .. 256, right edge on the picture's
       edge), the cards between, centred. 4:3 (one) is the single block, untouched. The cards themselves are one layer under all of it,
       out to the window edges (RECROOMSLIDE1, below), so a page slide passes under the tab column and the arrows as on the DS. */
    int pm = -1, p_tab = -1, p_right = -1, p_back = -1, p_cards = -1;
    if (one) {
        pm = menu_add_piece(L, m);
        p_tab = p_right = p_back = p_cards = pm;
    } else {
        const int lw = mode == 0 ? 0x3c : 0x5c;          /* the tab column's strip ends at DS x 0x37; the cards start at 0x40 (page two: 0x60) */
        auto cut = [&](int sx, int sy, int sw, int sh, int ax, uint8_t layers) {
            MenuPiece q = m;
            q.s.x = (short)sx; q.s.y = (short)sy; q.s.w = (short)sw; q.s.h = (short)sh;
            q.ay = MENU_AY_T; q.dy = (short)sy; q.ax = (uint8_t)ax; q.layers = layers;
            q.dx = (short)(ax == MENU_AX_C ? sx + sw / 2 - 128 : 0);       /* centred pieces keep their DS x from the DS box's centre */
            return menu_add_piece(L, q);
        };
        /* RECROOMSLIDE1 (Tango 10-29, ANCHOR1's plan): THE CARDS RUN OUT INTO THE MARGINS. BG0 (the cards' art and nothing else) is a 512 column
           map the ROM scrolls by mSubBgScrollX (0 page one, 0xb0 page two; src/minigames/d_s_mini_gm.cpp MiniGm_UpdatePageScroll): page one
           sits at map x 0x40..0xd0, page two at 0x110..0x1a0, so at rest the page not showing is parked 16 columns past the screen's right
           edge (page one) or under the strip (page two). A wide picture is that same map seen through a wider window: the DS box draws BG0 at
           its own scroll (one centred piece, DS x 0..255), and the margins draw BG0 scanned out with its scroll moved by 256 columns
           (MENU_L_BG0_NEXT: source x c is DS x 256 + c, and on a 512 column map also DS x c - 256), out to both window edges (at most 128
           columns a side, the map's other half). It is drawn at rest too: the margins show the
           neighbouring page where the map keeps it, so nothing pops in on the first slide frame. Over it, every piece at a window edge or
           in the DS box draws the strip and the objects only (BG2 + OBJ), so a card is never drawn twice: the cards pass under the tab
           column at the left window edge and under the page arrow and back arrow at the right one. Four masks a frame (stars, BG0, BG0 moved,
           strip + objects), the cap of menu_sub_for. 4:3 is untouched. */
        {
            MenuPiece u = m;
            u.s.x = 0; u.s.y = 0; u.s.w = 256; u.s.h = 192;
            u.layers = MENU_L_BG0; u.touch = 0; u.ax = MENU_AX_C; u.ay = MENU_AY_T; u.dx = 0; u.dy = 0;
            menu_add_piece(L, u);
            /* The margins are placed in host pixels on the DS box's own grid (a centred piece's offset is whole units, and the margin is
               rarely a whole number of units): DS column p starts at host x hx(p), the floor of the exact place, the same floor the core
               gives the DS box piece's own edges (menu_layout_dest), and the fixed placement samples one source column per unit from its
               left edge, so every margin column sits within a host pixel of where a wider DS screen would put it. */
            const long long UN = L->unum, UD = L->uden;
            auto hx = [&](int p) { return (int)(((long long)L->w * UD + 2LL * (p - 128) * UN) / (2 * UD)); };      /* p >= -128 keeps it >= 0 here */
            auto fixed = [&](int sx, int sw, int x0, int x1) {
                if (x1 <= x0 || sw < 1) return;
                MenuPiece q = u;
                q.layers = MENU_L_BG0_NEXT | MENU_L_BG0;
                q.s.x = (short)sx; q.s.w = (short)sw;
                q.lnum = (int)UN; q.lden = (int)UD;
                q.fx = (short)x0; q.fy = 0; q.fw = (short)(x1 - x0); q.fh = (short)L->h;
                menu_add_piece(L, q);
            };
            /* K = whole DS columns of the margin (at most 128, the map's other half); on the right the piece runs from DS x 256 to the window
               edge (its last column cut by the edge); on the left from DS x -K to the DS box, and the sliver left of that (under one unit) is
               the one column DS x -K - 1, cut by the edge */
            const long long num = (long long)L->w * UD - 256LL * UN;
            int K = num > 0 ? (int)(num / (2 * UN)) : 0;
            const bool capped = K >= 128;
            if (capped) K = 128;
            if (num > 0) {
                fixed(0, capped ? 128 : K + 1, hx(256), capped ? hx(384) : L->w);
                fixed(256 - K, K, hx(-K), hx(0));
                if (!capped) fixed(255 - K, 1, 0, hx(-K));
            }
        }
        const uint8_t arrows = (uint8_t)(MENU_L_BG2 | MENU_L_OBJ);
        p_tab = cut(0, 0, lw, 192, MENU_AX_L, arrows);
        if (mode == 0) {
            p_cards = cut(lw, 0, 0xd0 - lw, 192, MENU_AX_C, arrows);
            p_right = cut(0xd0, 0, 48, 0x88, MENU_AX_R, arrows);
        } else {
            p_cards = cut(lw, 0, 256 - lw, 0x88, MENU_AX_C, arrows);
            cut(lw, 0x88, 0xd0 - lw, 56, MENU_AX_C, arrows);
            p_right = p_tab;                              /* page two: the page arrow is beside the tab column */
        }
        p_back = cut(0xd0, 0x88, 48, 56, MENU_AX_R, arrows);
        pm = p_cards;
    }

    /* the ROM's boxes (inclusive byte compares in the cited source, so a box is one wider than the span) */
    /* category tabs: x <= 0x30, tab i y 2 + 0x30 i .. 0x2e + 0x30 i (src/MiniGm_UpdateGroupTabs.cpp:60-71) */
    for (int i = 0; i < 4; ++i) menu_add_hotspot(L, p_tab, 0, 2 + 0x30 * i, 0x31, 0x2d, -1);
    /* back arrow: x 0xd8..0xf8, y 0x90..0xb0 (src/MiniGm_UpdateExitButton.cpp:25-31) */
    L->back = (int8_t)menu_add_hotspot(L, p_back, kBackX, kBackY, kBackW, kBackH, -1);

    /* no live object (the layout selftest): the 2 x 3 page with every card and the next-page arrow */
    /* game cells (src/UpdateGameSelect.cpp:77-90): the ROM accepts a cell whose game is unlocked (c00b4) */
    if (mode == 0) {
        for (int row = 0; row < 2; ++row)
            for (int col = 0; col < 3; ++col) {
                if (o && !((R.cells >> (row * 3 + col)) & 1)) continue;
                menu_add_hotspot(L, p_cards, 0x40 + 0x30 * col, 0x18 + 0x48 * row, 0x30, 0x48, -1);
            }
    } else {
        for (int col = 0; col < 3; ++col) {
            if (o && !((R.cells >> col) & 1)) continue;
            menu_add_hotspot(L, p_cards, 0x60 + 0x30 * col, 0x38, 0x30, 0x48, -1);
        }
    }
    /* page arrow: drawn only while no flip is running and, on the first page, only if some page two exists
       (src/_ZN11dScMiniGm_c6RenderEv.cpp:84-110); boxes src/MiniGm_UpdatePageArrows.c:30-45 */
    const bool arrow = !o || R.arrow;
    if (arrow) {
        if (mode == 0) menu_add_hotspot(L, p_right, 0xd8, 0x40, 0x21, 0x41, -1);
        else menu_add_hotspot(L, p_right, 0x38, 0x40, 0x21, 0x41, -1);
    }
    /* THE FOCUS ARROW (a pad or key focus, or the mouse hovering): the title's own blue triangle (menu_hostimg.h, the art
       cut from the ROM's cursor object) points down at the focused item from above, its tip a little way into the item's
       top edge, bobbing on the 8-frame cadence; pulled down into the picture where the item is at the top edge. The core
       also brightens and outlines the item itself. Backdrop class, so a click goes through to the item below it. */
    const int fx = R.foc_x, fy = R.foc_y, calls = R.calls;
    if (!one && R.foc_have) menu_layout_dest(L);     /* ANCHOR1: menu_hotspot_centre reads the pieces' dest rects, which the solve makes only after build(): without them every wide hotspot centred at host x 0 (the arrow half off the window edge, or off it entirely over a card) */
    if (R.foc_have && menu_hostimg_ready(MENU_IMG_ARROW_DOWN)) {
        for (int j = 0; j < L->nh; ++j) {
            const MenuHotspot &H = L->hs[j];
            if (H.box.x != fx || H.box.y != fy) continue;
            static const int kBob[8] = { 0, 1, 2, 3, 4, 3, 2, 1 };       /* half units */
            int iw = 1, ih = 1;
            menu_hostimg_size(MENU_IMG_ARROW_DOWN, &iw, &ih);
            const int dw = 14 * L->unum / L->uden, dh = (int)((long long)dw * ih / iw);
            MenuPiece ar = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
            ar.bg = 1; ar.hostimg = MENU_IMG_ARROW_DOWN; ar.lden = 1; ar.lnum = 1; ar.fw = (short)dw; ar.fh = (short)dh;
            const int cxu2 = 2 * H.box.x + H.box.w - 256;                  /* centre x from 128, times 2 */
            ar.fx = (short)(L->w / 2 + cxu2 * L->unum / (2 * L->uden) - dw / 2);
            if (!one) { int hx, hy; menu_hotspot_centre(L, j, &hx, &hy); ar.fx = (short)(hx - dw / 2); }      /* ANCHOR1: the hotspot's own place, its piece may ride an edge (the dest rects were made just above: build runs before the solve makes them) */
            int top = ((2 * H.box.y + 6 + kBob[calls & 7]) * L->unum) / (2 * L->uden) - dh;
            if (top < 0) top = 0;
            if (top + dh > L->h) top = L->h - dh;
            ar.fy = (short)top;
            menu_add_piece(L, ar);
            break;
        }
    }
    L->backdrop = MENU_BACKDROP_TOP_AS_IS;
    L->stage = MENU_STAGE_POST_FADE;
}

/* The ROM reads the touch only while it is not flipping a page, exiting or fading out its choice
   (the guards at the head of MiniGm_UpdateExitButton / 0378 / 06cc / 0878); the pad is not read at all.
   Called once a frame by hal_menu_pad while this recipe is live. It is also where the pad's B goes: the port's
   B button is the DS A key (0x0001, "the punch"), which this scene never reads, so a press of it is taken
   here as the back arrow (a synthesised touch at the back box's centre) and stripped. Backspace takes the
   core's own host_back path. */
int menu_recroom_pad_ok(void)
{
    const void *o = port_scene_live_object();
    if (!o || port_scene_live_id() != kScene) { g_foc_have = 0; return 0; }
    {
        unsigned st;
        g_foc_have = hal_menu_hover_box(&g_foc_x, &g_foc_y, &st);
    }
    if (ob(o, 0xac) != 0 || ob(o, 0x54) == 1) return 0;
    if (ow(o, 0x90) > 0 || ow(o, 0x94) > 0 || ow(o, 0x98) > 0) return 0;
    unsigned short *pressed = (unsigned short *)((char *)data_020a0e58 + data_020a0e40 * 4 + 2);
    if (*pressed & 0x0001) {
        hal_menu_touch_queue_press(kBackX + kBackW / 2, kBackY + kBackH / 2);
        *pressed = (unsigned short)(*pressed & ~0x0001u);
    }
    return 1;
}

}  // extern "C++"
