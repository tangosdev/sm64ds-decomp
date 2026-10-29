/* menu_online.h -- the title's online menu (run after093, lane NAME1).

   The title's MP plank is ONLINE. Pressing it morphs ADVENTURE into ADVENTURE ONLINE and REC ROOM into VS (the planks turn like a file select
   tile); pressing it again, or Back, turns them back. ADVENTURE ONLINE opens the file select on the ONLINE saves (the signed online save of
   ntr/backup.cpp, never the solo files), picking a file asks the launcher for a server (hal/online_join.h) behind a "Finding a server..."
   screen, VS for now says the lobby is the launcher's. A ? button explains the mode.

   THE SPLIT. This file's state (g_on) lives on the LOGIC thread: the title recipe's menu_title_facts copies it into the facts blob (OnlFacts, at
   the fixed offset ONL_FACTS_OFF so the draw side finds it without knowing the recipe's own struct), and build() / the draw callbacks read only
   that copy. The press entry points below are called from the pad / touch seams (hal/settings_page_hooks.cpp, the recipe's pad map) and from
   the recipe's tick; all on the logic thread.

   WHAT DRAWS WHAT. With the HD UI mod's plank pictures (menu/online/*.png, menu_pause_art_ids.h ids 46..51) the planks are hostimg pieces;
   without them (no mod, or an older mod) the planks are drawn in code in the DS plank's colours with ui_text's font. */
#ifndef MENU_ONLINE_H
#define MENU_ONLINE_H
#include <stdint.h>
#include "menu_compose.h"
#include "menu_online_profile.h"

enum { ONL_SCR_NONE = 0, ONL_SCR_VS = 1, ONL_SCR_HELP = 2, ONL_SCR_WAIT = 3, ONL_SCR_FAIL = 4, ONL_SCR_PROFILE = 5 };   /* PROFILE1: the name and colour popup (hal/menu_online_profile.cpp) */
/* plank kinds (MenuPiece.wtw of a code drawn plank carries one) */
enum { ONL_K_ONLINE = 0, ONL_K_ADV_ONLINE = 1, ONL_K_VS = 2, ONL_K_HELP = 3, ONL_K_BACK = 4, ONL_K_CANCEL = 5, ONL_K_ADV = 6, ONL_K_REC = 7, ONL_K_SOLO = 8, ONL_K_OK = 9, ONL_K_NAME = 10 };

struct OnlFacts {
    int magic;          /* ONL_MAGIC when written by menu_online_facts */
    int on;             /* the online planks are the target (ADVENTURE ONLINE / VS) */
    int age;            /* ticks since the last toggle (0 .. 999; 999 = never toggled this visit) */
    int scr;            /* ONL_SCR_* modal screen */
    int scr_age;
    int flow;           /* the online file select is the page (ADVENTURE ONLINE was pressed) */
    int help_files;     /* the help screen was opened on the file select (its text differs) */
    char why[92];       /* the FAIL sentence (PADNAV1: 4 bytes shorter for padfocus below; the sentences are far shorter) */
    ProfFacts prof;     /* PROFILE1: the name and colour popup */
    int padfocus;       /* PADNAV1: the pad focus is on a host plank: ONL_PF_HELP (the ?) or ONL_PF_NAME, else 0 (the ROM cursor is the focus) */
};
enum { ONL_PF_HELP = 100, ONL_PF_NAME = 101 };
#define ONL_MAGIC 0x4f4e4c31
#define ONL_FACTS_OFF (1024 - 160)
static_assert(sizeof(OnlFacts) <= 160, "OnlFacts must fit the tail of MenuFacts.blob");

/* the DS boxes this menu owns (the hook and the pieces agree on them; a click maps proportionally into a piece's box) */
#define ONL_HELP_X 176
#define ONL_HELP_Y 4
#define ONL_HELP_W 32
#define ONL_HELP_H 32
#define ONL_BTN_X 88
#define ONL_BTN_Y 106
#define ONL_BTN_W 80
#define ONL_BTN_H 32

const OnlFacts *onl_facts_of(const MenuLayout *L);       /* null when the blob carries none (the layout selftest) */

/* ---- logic thread --------------------------------------------------------------------------------------------- */
void menu_online_facts(OnlFacts *f);                     /* called once per tick by menu_title_facts */
void menu_online_tick(int top, int elem, int req);       /* called once per tick by menu_title_up: waits, the medium follows the page */
void menu_online_scene_left(void);                       /* the title scene is not the live scene */
int  menu_online_enabled(void);                          /* the one-screen menus are on: the online UI is shown at all */
int  menu_online_on(void);
int  menu_online_modal(void);                            /* a modal screen is up */
/* a confirm on the main page's ROM cursor element `elem` (edge = this tick is the press's first). 1 = the ROM must not see it. */
int  menu_online_press_main(int elem, int edge);
/* a confirm on the file select's cursor element (14, 15, 16 = file A, B, C) while the online file select is the page. 1 = the ROM must not see it. */
int  menu_online_press_file(int elem, int edge);
/* a stylus at (sx, sy) in DS pixels on the title; kind 1 main page, 2 file select page. 1 = swallowed. */
int  menu_online_touch(int kind, int down, int sx, int sy);
void menu_online_pad_frame(int kind, int cursor);          /* PADNAV1: once a frame on the title main page / file select: forget a host focus the mouse or a page change left behind */
int  menu_online_dir(int kind, int cursor, unsigned dirs);   /* a D-pad step the ROM cursor has nowhere to go with: wrench + left, or the file row + up, opens the ? text */
int  menu_online_help_pad(int kind);                     /* X / Y on a page that has the ? button: 1 = taken */
int  menu_online_padfocus(void);                         /* TITLEUI1: the host plank the pad focus is on (ONL_PF_*), 0 = the ROM cursor is the focus */
/* TITLEUI1: a mouse hover on a page of `kind` (1 main page, 2 file select): pf = ONL_PF_HELP / ONL_PF_NAME lands the pad focus on that host plank (the arrow
   moves there, as a D-pad step would put it), 0 hands the focus back to the ROM cursor (a hover on the wrench or a ROM plank). cursor = the ROM cursor element. */
void menu_online_hover_focus(int kind, int pf, int cursor);
int  menu_online_modal_up(void);                         /* a ? text, the VS note or the wait screen is up (INPUTMATRIX1: the pad is modal for it) */
int  menu_online_back(int kind);                         /* the back key: 1 = taken (closes a screen or turns the planks back) */

/* ---- recipe (build / draw side) -------------------------------------------------------------------------------- */
/* the state of plank k (0 = the MP slot, 1 = ADVENTURE, 2 = REC ROOM) at age: kind = ONL_K_*, w1000 = width in permille of the full plank (the turn) */
void menu_online_plank_state(const OnlFacts *f, int k, int *kind, int *w1000);
/* FLIPSMOOTH1: 1 while plank k's turn is on its ticks 1 .. 2 * half (a = ticks into it, from / to = the faces): the pictures between ticks follow it */
int  menu_online_plank_phase(const OnlFacts *f, int k, int *a, int *half, int *from, int *to);
/* adds the piece of an online-family plank (kind): the HD picture if the mod has it, else a code drawn plank. The dest rect is in host pixels; the
   ROM touch box (bx, by, bw, bh) is what a click maps to. lit / pressed: the selection look. alpha 0 = opaque. Returns the piece index or -1. */
int  menu_online_add_plank(MenuLayout *L, int kind, int fx, int fy, int fw, int fh, int bx, int by, int bw, int bh, int lit, int pressed, int alpha,
                           int touch, int frame);
int  menu_online_plank_w(int kind, int fh);              /* the full width of a plank of this kind at height fh (the picture's shape, else a fixed one) */
int  menu_online_plank_art_ready(int kind);              /* 1 when the mod's picture for this kind decoded */
/* the modal screen over the page (veil, panel, text, its button) and the ? button; both add pieces to the layout. */
/* the green of the online mode: a custom piece that turns the BLUE parts of what is already drawn (the sky and the logo pattern of the title, the water of the
   file select; wood, faces and lettering are not blue and stay) toward a light green. The title ramps it over the morph (34 ticks, the length of the face press page's
   wallpaper-to-paper fade, which is the in-port tint with a measured timing) and back; the online file select (flow) holds it at full. Add it right after the
   background so only the background is in the picture when it runs. No piece when the strength is 0. */
void menu_online_add_tint(MenuLayout *L, const OnlFacts *f);
int  menu_online_tint_permille(const OnlFacts *f);
void menu_online_add_modal(MenuLayout *L, const OnlFacts *f);
void menu_online_add_arrow(MenuLayout *L, int up, int fx, int fy, int fw, int fh);   /* NAME2: the title's selection arrow from the code's own art (no mod picture) */
void menu_online_add_help(MenuLayout *L, int right_edge_x, int top_y, int size_px);   /* the ? button at that host place; the box is ONL_HELP_* */
#endif
