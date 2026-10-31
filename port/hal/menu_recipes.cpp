/* menu_recipes.cpp -- the ONE static recipe table (run menu1, lane P0CORE).
 *
 * Every planned recipe is listed here up front as a stub that is never up, so
 * a recipe lane fills its own row (and its own menu_recipe_<name>.cpp) without
 * touching this file's shape. No self-registration: static hal libraries drop
 * self-registering objects, and an explicit table cannot be dropped.
 *
 * Two rows are real in phase 0:
 *   fallback  the guarantee no one-screen menu is unreachable: the whole top
 *             screen as the backdrop and the whole bottom screen as one touch
 *             panel at HALF height (see DECISION below), centred low, with a
 *             3x3 grid of hotspots so a pad can poke it.
 *   test      only up for MENU_SCENE_TEST; the selftest's workhorse. It uses
 *             all four scales, three anchors in each axis, the narrow and the
 *             wide variants, and 12 hotspots.
 *
 * DECISION (P0CORE): the brief says the fallback bottom panel is "~55% height".
 * The allowed whole-step scales are 1, 3/4, 2/3 and 1/2, which give 100%, 75%,
 * 67% and 50% of the picture height; 1/2 is the nearest that keeps the pixel
 * art on whole steps, so the fallback uses 1/2 (96 of 192 canvas units).
 */
#include "menu_compose.h"

/* menu_recipe_pause.cpp (lane PAUSE1) */
int menu_pause_up(int scene_id);
void menu_pause_build(int variant, MenuLayout *L);
int menu_pause_cursor(int dir, int *owns);
int menu_pause_pad_ok(void);
void menu_pause_facts(MenuFacts *f);

namespace {

int up_never(int) { return 0; }
void build_none(int, MenuLayout *) {}

int up_test(int scene_id) { return scene_id == MENU_SCENE_TEST ? 1 : (scene_id == MENU_SCENE_TEST_CROP ? 2 : 0); }

/* TEST layout. Source rects are DS pixels of the sub screen; the canvas is
   192 units tall. NARROW (canvas_w < 300):
     p0 top band        (0,16,256,48) at 1     C,T  y   4..52
     p1 plates          (0,0,256,88)  at 3/4   C,T  y  56..122, x 32..224
     p2 left pad        (0,96,128,96) at 1/2   L,B  y 140..188, x   4..68
     p3 right pad       (128,96,128,96) at 2/3 R,B  y 124..188, x 167..252
   WIDE (canvas_w >= 300): p1 at 1 (y 52..140), p2 and p3 at 1/2, pushed to the
   corners, so the extra width is spent on margins, not on stretching. */
void build_test(int variant, MenuLayout *L)
{
    const int wide = menu_wide(L);
    MenuPiece p0 = menu_piece_make(SRC_TOP_2D, 0, 16, 256, 48, MENU_AX_C, MENU_AY_T, 0, 4, 1, 1);
    p0.layers = MENU_L_BG3 | MENU_L_OBJ;
    p0.autocrop = variant == 2;      /* variant 2 (MENU_SCENE_TEST_CROP) exercises the autocrop latch */
    menu_add_piece(L, p0);

    MenuPiece p1 = menu_piece_make(SRC_SUB, 0, 0, 256, 88, MENU_AX_C, MENU_AY_T, 0, wide ? 52 : 56,
                                   wide ? 1 : 3, wide ? 1 : 4);
    p1.touch = 1;
    p1.key_backdrop = 1;
    menu_add_piece(L, p1);

    MenuPiece p2 = menu_piece_make(SRC_SUB, 0, 96, 128, 96, MENU_AX_L, MENU_AY_B, wide ? 8 : 4, -4, 1, 2);
    p2.touch = 1;
    menu_add_piece(L, p2);

    MenuPiece p3 = menu_piece_make(SRC_SUB, 128, 96, 128, 96, MENU_AX_R, MENU_AY_B, wide ? -8 : -4, -4,
                                   wide ? 1 : 2, wide ? 2 : 3);
    p3.touch = 1;
    menu_add_piece(L, p3);

    for (int j = 0; j < 2; ++j)
        for (int i = 0; i < 4; ++i) menu_add_hotspot(L, 1, 8 + i * 60, 6 + j * 44, 56, 36, j * 4 + i);
    L->back = (int8_t)menu_add_hotspot(L, 2, 8, 104, 48, 40, -1);
    menu_add_hotspot(L, 2, 72, 104, 48, 40, -1);
    menu_add_hotspot(L, 3, 136, 104, 48, 40, -1);
    menu_add_hotspot(L, 3, 196, 104, 48, 40, -1);
    L->backdrop = MENU_BACKDROP_DIM_AUTO;
    L->stage = MENU_STAGE_POST_FADE;
}

/* FALLBACK: p0 the full top (a backdrop), p1 the whole bottom screen at 1/2,
   centred low, one touch piece; hotspots a 3x3 grid of 85x64 boxes. */
void build_fallback(int, MenuLayout *L)
{
    MenuPiece p0 = menu_piece_make(SRC_TOP_PIC, 0, 0, 256, 192, MENU_AX_C, MENU_AY_M, 0, 0, 1, 1);
    p0.bg = 1;
    menu_add_piece(L, p0);
    MenuPiece p1 = menu_piece_make(SRC_SUB, 0, 0, 256, 192, MENU_AX_C, MENU_AY_B, 0, -4, 1, 2);
    p1.touch = 1;
    menu_add_piece(L, p1);
    for (int j = 0; j < 3; ++j)
        for (int i = 0; i < 3; ++i) menu_add_hotspot(L, 1, i * 85, j * 64, 85, 64, -1);
    L->backdrop = MENU_BACKDROP_TOP_FULL;
    L->stage = MENU_STAGE_POST_FADE;
}

}  // namespace

/* menu_recipe_starsel.cpp (lane STARGO1): the star select and game over rows */
int  menu_starsel_up(int scene_id);
void menu_starsel_build(int variant, MenuLayout *L);
int  menu_gameover_up(int scene_id);
void menu_gameover_build(int variant, MenuLayout *L);
int  menu_gameover_rom_cursor(int dir, int *sel);
int  menu_gameover_pad_ok(void);
void menu_starsel_facts(MenuFacts *f);
void menu_starsel_pad_map(uint16_t *pressed, int host_back);
void menu_gameover_facts(MenuFacts *f);

/* menu_recipe_recroom.cpp (lane RECROOM1): the Rec Room select */
int  menu_recroom_up(int scene_id);
void menu_recroom_build(int variant, MenuLayout *L);
int  menu_recroom_pad_ok(void);
void menu_recroom_facts(MenuFacts *f);

/* name, up, build, rom_cursor, enable_key, driver, flags, nvariants, pad_ok, facts */
/* menu_recipe_lc.cpp (lane SAVE2): the save screen in N64 order, behind OneScreenMenus; listed BEFORE lc so it wins when both are on */
int  menu_lc2_up(int scene_id);
void menu_lc2_build(int variant, MenuLayout *L);
int  menu_lc2_rom_cursor(int dir, int *sel);
void menu_lc2_facts(MenuFacts *f);

const MenuRecipe g_menu_recipes[] = {
    { "lc2",      menu_lc2_up, menu_lc2_build, menu_lc2_rom_cursor, MENU_KEY_ONE_SCREEN_MENUS, MENU_DRV_ROM_CURSOR, 0, 8,
      menu_lc_pad_ok, menu_lc2_facts },
    { "lc",       menu_lc_up, menu_lc_build, menu_lc_rom_cursor, MENU_KEY_SAVE_MENU_ON_TOP, MENU_DRV_ROM_CURSOR, 0, 8,
      menu_lc_pad_ok },
    { "pause",    menu_pause_up, menu_pause_build, menu_pause_cursor, MENU_KEY_ONE_SCREEN_MENUS, MENU_DRV_MIXED, MENU_RF_PAD_B_BACK, 8,
      menu_pause_pad_ok, menu_pause_facts },
    { "title",    menu_title_up, menu_title_build, 0, MENU_KEY_ONE_SCREEN_MENUS, MENU_DRV_NATIVE, 0, MENU_TITLE_NVARIANTS,
      0, menu_title_facts, menu_title_pad_map },
    { "starsel",  menu_starsel_up, menu_starsel_build, 0, MENU_KEY_ONE_SCREEN_MENUS, MENU_DRV_NATIVE, 0, 2, 0,
      menu_starsel_facts, menu_starsel_pad_map },
    { "gameover", menu_gameover_up, menu_gameover_build, menu_gameover_rom_cursor, MENU_KEY_ONE_SCREEN_MENUS,
      MENU_DRV_ROM_CURSOR, 0, 1, menu_gameover_pad_ok,
      menu_gameover_facts },
    { "recroom",  menu_recroom_up, menu_recroom_build, 0, MENU_KEY_ONE_SCREEN_MENUS, MENU_DRV_TOUCH, MENU_RF_START_CONFIRM, 1,
      menu_recroom_pad_ok, menu_recroom_facts },
    { "vs",       up_never, build_none, 0, MENU_KEY_ONE_SCREEN_MENUS,  MENU_DRV_TOUCH,      0, 0 },
    { "fallback", up_never, build_fallback, 0, MENU_KEY_ONE_SCREEN_MENUS, MENU_DRV_NATIVE,  MENU_RF_FALLBACK, 1 },
    { "test",     up_test,  build_test, 0, MENU_KEY_NONE,              MENU_DRV_TOUCH,      0, 2 },
};
const int g_menu_nrecipes = (int)(sizeof g_menu_recipes / sizeof g_menu_recipes[0]);

/* PADMENUS1: the ROM's own game over and level-clear (save) screens with OneScreenMenus off and no recipe live. The ROM has no pad cursor on either, so
   the recipe's own ROM cursor driver (the same function hal_menu_pad runs for its MENU_DRV_ROM_CURSOR rows) is run from the pad seam, and the confirm
   becomes the Start the ROM confirms with. */
static void rom_cursor_pad(uint16_t *pressed, int (*pad_ok)(void), int (*cursor)(int, int *))
{
    if (!pad_ok()) return;
    uint16_t k = *pressed;
    int dir = -1;
    if (k & MENU_PAD_UP) dir = MENU_DIR_UP;
    else if (k & MENU_PAD_DOWN) dir = MENU_DIR_DOWN;
    else if (k & MENU_PAD_LEFT) dir = MENU_DIR_LEFT;
    else if (k & MENU_PAD_RIGHT) dir = MENU_DIR_RIGHT;
    if (dir >= 0) { int sel = 0; cursor(dir, &sel); }
    k = (uint16_t)(k & ~0xf0u);
    if (k & MENU_PAD_CONFIRM) k = (uint16_t)((k & ~(unsigned)MENU_PAD_CONFIRM) | MENU_PAD_START);
    *pressed = k;
}
extern "C" void menu_gameover_pad_rom(uint16_t *pressed) { rom_cursor_pad(pressed, menu_gameover_pad_ok, menu_gameover_rom_cursor); }
extern "C" void menu_lc_pad_rom(uint16_t *pressed) { rom_cursor_pad(pressed, menu_lc_pad_ok, menu_lc_rom_cursor); }

/* PADMENUS1: the Rec Room with OneScreenMenus off. The scene reads no pad at all; the recipe's own pad step (menu_recroom_pad_ok) is where the pad's B
   becomes a touch on the back arrow, and it asks the ROM first whether the scene is reading the touch screen (not flipping a page, exiting or fading).
   Run from the pad seam before the key word is read. */
extern "C" void menu_recroom_pad_rom(void) { (void)menu_recroom_pad_ok(); }
