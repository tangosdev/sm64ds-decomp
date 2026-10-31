/* menu_online_profile.h -- the Adventure Online name and colour popup (run after093, lane PROFILE1).

   Tango (10-25): "We need a name and colour popup before people can join adventure online." The popup is one of the online menu's modal screens
   (ONL_SCR_PROFILE in menu_online.h). It shows when an online file is picked while no name has been saved yet, and from a NAME plank on the online
   file select at any time. The name is typed on the keyboard only (letters, digits, space, Backspace; Enter = OK; 16 at most); a pad player keeps
   the name the field starts with (the saved name, else the launcher's nickname, else "Player") and can still pick the colours and press OK. The
   colours are the player's online Yoshi BODY and SHOES, each freely chosen from a hue by shade grid (a picker in the game's plank look); the join
   exports them as SM64DS_PLAYER_COLOR / SM64DS_PLAYER_SHOES (hal/online_join.cpp), which the comms identity ('SYNI') already carries to every other
   console.

   Same split as the rest of the online menu: the logic below runs on the logic thread and is copied into the facts blob (ProfFacts inside OnlFacts);
   the draw side reads only the facts. Settings: host_settings.h OnlineName / OnlineBodyColor / OnlineShoeColor. */
#ifndef MENU_ONLINE_PROFILE_H
#define MENU_ONLINE_PROFILE_H
#include <stdint.h>
#include "menu_compose.h"

struct OnlFacts;

/* the first four pairs are the ROM's own four Yoshi rows (kVsRomRows in hal/vs_palette_gen.h; each pair is the row's DRAWN body and boot shade,
   index 2 and index 8, where the VS arithmetic puts a pick, so a classic cell gives that cartridge Yoshi word for word): the picker's "classic" cells. The last four were the old fixed list; they are kept only so a settings.json
   with the old OnlineYoshiColor index maps to its pair (host_settings.cpp carries the same eight). */
struct OnlineYoshiColor { const char *name; const char *body; const char *shoes; };
static const OnlineYoshiColor kOnlineYoshiColors[8] = {
    { "Green",  "31A431", "E66218" },
    { "Red",    "FF3139", "004AFF" },
    { "Blue",   "4A52B4", "B410B4" },
    { "Yellow", "FFCD00", "31BD08" },
    { "Pink",   "FF7FBF", "8A1F5C" },
    { "Cyan",   "20D8E0", "0E5E7A" },
    { "Purple", "8A4FD8", "3A1A7A" },
    { "Orange", "FF8A1E", "7A3A10" },
};

/* focus rows of the popup */
enum { PROF_F_NAME = 0, PROF_F_ROW = 1, PROF_F_GRID = 2, PROF_F_OK = 3, PROF_F_BACK = 4 };
/* the picker grid: PROF_COLS columns x PROF_ROWS rows. Column 0 = the ROM's four classic colours (body or shoe set, by the row picked), column 1 = white,
   grey, dark grey, black, columns 2..13 = twelve hues (red at 0 degrees, every 30) each in four shades (light, pure, dark, darker). */
#define PROF_COLS 14
#define PROF_ROWS 4
uint32_t profile_cell_colour(int which, int col, int row);      /* 0xRRGGBB of a grid cell; which 0 = BODY, 1 = SHOES */

/* what the draw side needs (copied into OnlFacts by menu_online_facts) */
struct ProfFacts {
    uint32_t body, shoes;         /* 0xRRGGBB */
    unsigned char focus;          /* PROF_F_* */
    unsigned char which;          /* the row being picked: 0 BODY, 1 SHOES */
    unsigned char cx, cy;         /* the grid cell cursor */
    unsigned char caret;          /* blink phase, 0 / 1 */
    signed char pending;          /* the file waiting for the OK (0..2) or -1 */
    char name[20];
};

/* the DS boxes of the popup (the hook and the pieces agree on them; a click maps proportionally into a piece's box) */
#define PROF_PANEL_X 16
#define PROF_PANEL_Y 4
#define PROF_PANEL_W 224
#define PROF_PANEL_H 178
#define PROF_FIELD_X 28
#define PROF_FIELD_Y 18
#define PROF_FIELD_W 200
#define PROF_FIELD_H 18
#define PROF_TAB_Y 49          /* the BODY and SHOES row labels, each with its live swatch */
#define PROF_TAB_H 18
#define PROF_TAB0_X 28
#define PROF_TAB1_X 130
#define PROF_TAB_W 98
#define PROF_GRID_X 24
#define PROF_GRID_Y 74
#define PROF_CELL_W 14
#define PROF_CELL_H 12
#define PROF_CELL_STEPX 15
#define PROF_CELL_STEPY 13
#define PROF_GRID_W (PROF_COLS * PROF_CELL_STEPX - 1)
#define PROF_GRID_H (PROF_ROWS * PROF_CELL_STEPY - 1)
#define PROF_OK_X 40
#define PROF_OK_Y 148
#define PROF_OK_W 80
#define PROF_OK_H 28
#define PROF_BACK_X 136
#define PROF_BACK_Y 148
#define PROF_BACK_W 80
#define PROF_BACK_H 28
/* the NAME plank on the online file select */
#define ONL_NAME_X 108
#define ONL_NAME_Y 5
#define ONL_NAME_W 60
#define ONL_NAME_H 30

/* ---- logic thread ---------------------------------------------------------------------------------------------- */
void profile_open(int pending_slot);                    /* pending_slot -1 = opened from the NAME plank (no join after OK) */
int  profile_active(void);
int  profile_name_needed(void);                         /* 1 while no name has been saved yet (the popup shows before a join) */
const char *profile_saved_name(void);                   /* the saved name ("" if none) */
void profile_facts(ProfFacts *f);
void profile_tick(void);                                /* once per tick while open: the typed keys, the caret */
/* a pad word's bits while the popup is up: the bits the popup takes (the hook strips them from the ROM's key words while down) */
unsigned profile_pad(unsigned pressed, unsigned held);
int  profile_touch(int sx, int sy, int edge);           /* a stylus / click press; always 1 (modal) */
int  profile_back(void);                                /* the back key / pad B: BACK */
int  profile_backspace(void);                           /* Backspace: 1 = the popup took it (deleted a character, or went back from another row) */
/* the popup finished: the owner (menu_online.cpp) closes the screen and, on ok with a pending file, starts the join */
extern void menu_online_profile_done(int ok, int pending_slot);

/* ---- build / draw side ------------------------------------------------------------------------------------------ */
void menu_online_profile_modal(MenuLayout *L, const OnlFacts *f);       /* the popup's pieces */
void menu_online_profile_draw(const MenuLayout *L, const OnlFacts *f, uint32_t *dst, int w, int h, int stride);   /* the panel, called from the modal piece */
void menu_online_add_name_plank(MenuLayout *L, int right_edge_x, int top_y, int h_px);     /* the NAME plank of the online file select */
#endif
