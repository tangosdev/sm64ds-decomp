/* menu_online_profile.cpp -- the Adventure Online name and colour popup, logic and draw (run after093, lane PROFILE1). See menu_online_profile.h.

   KEYBOARD ONLY, BY POLLING. The name is typed by reading the keys' state once per logic tick (the same gates every other reader of the keyboard
   uses: the window must have focus, or a scripted SM64DS_HOST_KEY press stands in for the key), not through WM_CHAR: nothing in the window procedure
   changes, and a scripted run can type. A press is an edge of the key state; the popup opening with a key already down (the Space or Enter that
   picked the file) types nothing. Shift gives capitals (Caps Lock inverts, as on any keyboard). Backspace arrives as the host back key; the name field
   takes it as "delete", every other row as BACK.

   W A S D are the title's default D-pad keys, so while the name field has the focus a D-pad bit that comes from one of those letters alone (its
   arrow key not down) belongs to the letter and is dropped; the arrow keys, a pad and the stick still move the focus. */
#include "menu_online.h"
#include "menu_online_profile.h"
#include "host_settings.h"
#include "ui_text.h"
#include "menu_hostimg.h"
#include "vs_char_groups.h"
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" int port_host_key_script(int vk);       /* tests/walk_window.cpp: SM64DS_HOST_KEY, 0 when unset */
extern "C" void port_host_keys_clear(unsigned short mask);   /* hal/scene_boot.cpp */
extern "C" void settings_page_press_sound(void);   /* hal/settings_page_hooks.cpp: the title's press sound (INPUTMATRIX1) */
extern "C" int hal_window_focused(void);           /* hal/sub_screen.cpp */
extern "C" const char *port_fs_catalog_path(unsigned file_id);   /* hal/fs.cpp: a ROM file id -> a readable path, or null */

namespace {

struct Prof {
    int open;
    int pending;
    int focus;
    int which;                  /* the row being picked: 0 BODY, 1 SHOES */
    unsigned col[2];            /* 0xRRGGBB of the two picks */
    int cx, cy;                 /* the grid cell cursor */
    char name[20];
    int len;
    unsigned tick;
    unsigned char prev[256];
    int chr;                    /* BROCOLOR1: 0 Yoshi, 1 Mario, 2 Luigi, 3 Wario */
    unsigned bro[3][4];         /* the bros' picks, [bro][HAT OVERALLS SHOES UNDERSHIRT] (the stored order, hal/bro_colors.h) */
    unsigned char broset[3][4]; /* ...which are set (unset = the cartridge's colour; an unset undershirt wears the hat) */
    int bro_dirty;              /* a bro pick changed since the popup opened (only then are the twelve keys written) */
} P;
unsigned g_bumper_prev;         /* the shoulder keys last frame (edges only) */

/* user32 is loaded by name, like every other reader of the keyboard in the port (tools/sdl_import_guard.py refuses a static USER32 import) */
SHORT (WINAPI *p_GetAsyncKeyState)(int);
SHORT (WINAPI *p_GetKeyState)(int);
void load_user32()
{
    static int done;
    if (done) return;
    done = 1;
    if (HMODULE u = LoadLibraryA("user32.dll")) {
        p_GetAsyncKeyState = (SHORT (WINAPI *)(int))GetProcAddress(u, "GetAsyncKeyState");
        p_GetKeyState = (SHORT (WINAPI *)(int))GetProcAddress(u, "GetKeyState");
    }
}

int key_down(int vk)
{
    if (port_host_key_script(vk)) return 1;
    load_user32();
    return p_GetAsyncKeyState && hal_window_focused() && (p_GetAsyncKeyState(vk) & 0x8000) ? 1 : 0;
}

/* BROCOLOR1: the rows of the character on show. Yoshi: BODY, SHOES (P.col); a bro: HAT, SHIRT, OVERALLS, SHOES (P.bro, the cartridge's colour
   when unset). PALSPLIT1: k is the row as shown; profile_bro_row maps it to the stored row (the undershirt is stored last). */
const char *const kCharName[PROF_CHARS] = { "Yoshi", "Mario", "Luigi", "Wario" };
int nrows() { return P.chr ? PROF_BRO_ROWS : 2; }
const char *row_name(int k)
{
    static const char *const kY[2] = { "BODY", "SHOES" }, *const kB[PROF_BRO_ROWS] = { "HAT", "SHIRT", "OVERALLS", "SHOES" };
    return P.chr ? kB[k < 0 ? 0 : k > PROF_BRO_ROWS - 1 ? PROF_BRO_ROWS - 1 : k] : kY[k ? 1 : 0];
}
/* a bro's stored row r as he wears it: his pick, an unset undershirt the hat's, else the cartridge's */
uint32_t bro_wears(int b, int r)
{
    if (P.broset[b][r]) return P.bro[b][r];
    if (r == 3) return bro_wears(b, 0);
    return profile_bro_cart(b, r);
}
uint32_t row_colour(int k)
{
    if (!P.chr) return P.col[k ? 1 : 0];
    return bro_wears(P.chr - 1, profile_bro_row(k));
}
uint32_t cell_of(int k, int x, int y) { return P.chr ? profile_bro_cell(P.chr - 1, profile_bro_row(k), x, y) : profile_cell_colour(k, x, y); }
void set_row(int k, uint32_t c)
{
    if (!P.chr) { P.col[k ? 1 : 0] = c; return; }
    const int b = P.chr - 1, r = profile_bro_row(k);
    if (r == 0 && !P.broset[b][3]) {        /* PALSPLIT1: the hat moves alone; the undershirt keeps what it showed */
        P.bro[b][3] = bro_wears(b, 3);
        P.broset[b][3] = 1;
    }
    P.bro[b][r] = c;
    P.broset[b][r] = 1;
    P.bro_dirty = 1;
}

void log_name(const char *why)
{
    fprintf(stderr, "[profile] %s: name \"%s\" body %06X shoes %06X\n", why, P.name, P.col[0], P.col[1]);
}
void log_bros(const char *why)
{
    fprintf(stderr, "[profile] %s bros:", why);
    for (int b = 0; b < 3; ++b) {
        fprintf(stderr, " %s", kCharName[b + 1]);
        for (int k = 0; k < 4; ++k) {
            if (P.broset[b][k]) fprintf(stderr, " %06X", P.bro[b][k]);
            else fprintf(stderr, " -");
        }
    }
    fprintf(stderr, "\n");
}

/* the grid's cell colours. Hues every 30 degrees from red; the four shades of a hue are (saturation, value) = (50%, 100%), (100%, 100%), (100%, 70%),
   (100%, 40%), in integer arithmetic so every build agrees on every byte (red is FF0000, orange FF7F00, azure 007FFF). Column 1 is white, grey, dark grey, black. */
uint32_t hue_cell(int hue, int shade)
{
    static const int kS[4] = { 500, 1000, 1000, 1000 }, kV[4] = { 1000, 1000, 700, 400 };
    const int v = 255 * kV[shade] / 1000, s = kS[shade];
    const int sector = hue / 2, fm = (hue & 1) ? 500 : 0;
    const int p = v * (1000 - s) / 1000, q = v * (1000 - s * fm / 1000) / 1000, t = v * (1000 - s * (1000 - fm) / 1000) / 1000;
    int r, g, b;
    switch (sector) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    default: r = v; g = p; b = q; break;
    }
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

/* the cell under (cx, cy) holding exactly this colour, else -1 */
int find_cell(int which, uint32_t rgb, int *ox, int *oy)
{
    for (int x = 0; x < PROF_COLS; ++x)
        for (int y = 0; y < PROF_ROWS; ++y)
            if (cell_of(which, x, y) == rgb) { *ox = x; *oy = y; return 1; }
    return 0;
}

void goto_pick(int which)
{
    if (which < 0) which = 0;
    if (which > nrows() - 1) which = nrows() - 1;
    P.which = which;
    int x, y;
    if (find_cell(which, row_colour(which), &x, &y)) { P.cx = x; P.cy = y; }
}

/* BROCOLOR1: show another character (dir -1 / +1, wrapping); the rows follow it, the cursor goes to its first row's pick */
void step_char(int dir, const char *how)
{
    P.chr = (P.chr + dir + PROF_CHARS) % PROF_CHARS;
    goto_pick(0);
    fprintf(stderr, "[profile] character %s (%s)\n", kCharName[P.chr], how);
}

void type_char(char c)
{
    if (P.len >= 16) return;
    if (c == ' ' && (P.len == 0 || P.name[P.len - 1] == ' ')) return;      /* no leading or doubled spaces: the name is stored trimmed */
    P.name[P.len++] = c;
    P.name[P.len] = 0;
}

void finish(int ok)
{
    char clean[20];
    const int pending = P.pending;
    settings_page_press_sound();      /* a button press sounds like every other title button's */
    if (ok) {
        host_online_name_clean(P.name, clean, sizeof clean);
        if (!clean[0]) snprintf(clean, sizeof clean, "%s", "Player");
        host_setting_set_online_profile(clean, P.col[0], P.col[1]);
        snprintf(P.name, sizeof P.name, "%s", clean);
        log_name("saved");
        if (P.bro_dirty) {      /* BROCOLOR1: the bro keys, only when a bro pick changed */
            host_setting_set_online_bros(P.bro, P.broset);
            log_bros("saved");
        }
    } else {
        fprintf(stderr, "[profile] back: nothing saved\n");
    }
    P.open = 0;
    menu_online_profile_done(ok, pending);
}

}  // namespace

void profile_open(int pending_slot)
{
    memset(&P, 0, sizeof P);
    P.open = 1;
    P.pending = pending_slot;
    P.focus = PROF_F_NAME;
    const char *start = host_setting_online_name();
    if (!start[0]) start = host_setting_launcher_nick();
    if (!start[0]) start = "Player";
    snprintf(P.name, sizeof P.name, "%s", start);
    P.len = (int)strlen(P.name);
    unsigned body = 0, shoes = 0;
    if (host_setting_online_colours(&body, &shoes)) { P.col[0] = body; P.col[1] = shoes; }
    else { P.col[0] = 0x31A431; P.col[1] = 0xE66218; }            /* nothing picked yet: the ROM's green Yoshi */
    for (int b = 0; b < 3; ++b)                                     /* BROCOLOR1: the bros' saved picks */
        for (int k = 0; k < 4; ++k) {
            unsigned v = 0;
            P.broset[b][k] = (unsigned char)host_setting_online_bro(b, k, &v);
            P.bro[b][k] = P.broset[b][k] ? v : 0;
        }
    P.chr = 0;
    P.which = 0;
    goto_pick(0);
    /* keys already down when the popup opens type nothing */
    for (int vk = 0; vk < 256; ++vk) P.prev[vk] = (unsigned char)(key_down(vk) ? 1 : 0);
    fprintf(stderr, "[profile] popup open (file %d): name \"%s\" body %06X shoes %06X\n", pending_slot, P.name, P.col[0], P.col[1]);
    log_bros("open");
}

int profile_active(void) { return P.open; }

uint32_t profile_cell_colour(int which, int col, int row)
{
    static const uint32_t kClassic[2][4] = { { 0x31A431, 0xFF3139, 0x4A52B4, 0xFFCD00 }, { 0xE66218, 0x004AFF, 0xB410B4, 0x31BD08 } };
    static const uint32_t kNeutral[4] = { 0xFFFFFF, 0x808080, 0x202020, 0x000000 };
    if (row < 0) row = 0;
    if (row > 3) row = 3;
    if (col <= 0) return kClassic[which ? 1 : 0][row];
    if (col == 1) return kNeutral[row];
    return hue_cell((col - 2) % 12, row);
}

/* BROCOLOR1: a bro row's cells. Column 0 = the cartridge Mario, Luigi and Wario colour of the row and the cartridge green Yoshi's (body green for
   HAT and OVERALLS, boot orange for SHOES); the rest of the grid is the Yoshi grid's. */
uint32_t profile_bro_cart(int bro, int cat)
{
    static const int kCat[4] = { vscg::CAT_HAT, vscg::CAT_OVERALLS, vscg::CAT_SHOES, vscg::CAT_UNDERSHIRT };
    if (bro < 0 || bro > 2 || cat < 0 || cat > 3) return 0;
    return vscg::kBroCart[bro][kCat[cat]];
}
int profile_bro_row(int k)
{
    static const int kRow[PROF_BRO_ROWS] = { 0, 3, 1, 2 };     /* HAT, SHIRT, OVERALLS, SHOES as shown */
    return kRow[k < 0 ? 0 : k > PROF_BRO_ROWS - 1 ? PROF_BRO_ROWS - 1 : k];
}
uint32_t profile_bro_cell(int bro, int cat, int col, int row)
{
    (void)bro;
    if (row < 0) row = 0;
    if (row > 3) row = 3;
    if (col <= 0) return row < 3 ? profile_bro_cart(row, cat) : (cat == 2 ? 0xE66218u : 0x31A431u);
    return profile_cell_colour(0, col, row);
}

int profile_name_needed(void) { host_setting_online_colours(0, 0); return host_setting_online_name()[0] == 0; }      /* the colours read also maps an old index key once */
const char *profile_saved_name(void) { return host_setting_online_name(); }

void profile_facts(ProfFacts *f)
{
    memset(f, 0, sizeof *f);
    f->focus = P.focus;
    f->chr = (unsigned char)P.chr;
    for (int k = 0; k < nrows(); ++k) {
        const uint32_t c = row_colour(k);
        f->row[k][0] = (unsigned char)(c >> 16); f->row[k][1] = (unsigned char)(c >> 8); f->row[k][2] = (unsigned char)c;
    }
    f->which = (unsigned char)P.which;
    f->cx = (unsigned char)P.cx;
    f->cy = (unsigned char)P.cy;
    f->caret = (unsigned char)((P.tick / 30) & 1);
    f->pending = (signed char)(P.open ? P.pending : -1);
    snprintf(f->name, sizeof f->name, "%s", P.name);
}

void profile_tick(void)
{
    if (!P.open) return;
    ++P.tick;
    /* Esc is BACK (pad B and Backspace on the other rows go through profile_back) */
    {
        const int esc = key_down(VK_ESCAPE);
        const int fresh = esc && !P.prev[VK_ESCAPE];
        P.prev[VK_ESCAPE] = (unsigned char)esc;
        if (fresh && P.tick > 8) { finish(0); return; }
    }
    if (P.focus != PROF_F_NAME) {
        for (int vk = 0; vk < 256; ++vk) P.prev[vk] = (unsigned char)(key_down(vk) ? 1 : 0);
        return;
    }
    const int shift = key_down(VK_SHIFT) ? 1 : 0;
    const int caps = (p_GetKeyState && (p_GetKeyState(VK_CAPITAL) & 1)) ? 1 : 0;
    const int upper = shift ^ caps;
    static const int kRanges[][2] = { { 'A', 'Z' }, { '0', '9' }, { VK_NUMPAD0, VK_NUMPAD9 }, { VK_SPACE, VK_SPACE } };
    int typed = 0;
    const int ctl = key_down(VK_CONTROL) || key_down(VK_MENU);      /* Ctrl or Alt held: a shortcut, not a letter (Ctrl+A must not type "a") */
    for (auto &r : kRanges)
        for (int vk = r[0]; vk <= r[1]; ++vk) {
            const int d = key_down(vk);
            const int fresh = d && !P.prev[vk];
            P.prev[vk] = (unsigned char)d;
            if (!fresh || ctl || P.tick <= 2) continue;
            char c;
            if (vk >= 'A' && vk <= 'Z') c = (char)(upper ? vk : vk + 32);
            else if (vk >= '0' && vk <= '9') c = (char)vk;
            else if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9) c = (char)('0' + vk - VK_NUMPAD0);
            else c = ' ';
            type_char(c);
            typed = 1;
        }
    if (typed) log_name("typed");
}

/* the pad word while the popup is up. dirs: 0x10 right, 0x20 left, 0x40 up, 0x80 down; confirm: 0x0002 (pad A, Space) and 0x0008 (Start, Enter). The
   bits it answers are the bits taken; they are taken even when nothing is done with them (the ROM's page is behind a modal). */
unsigned profile_pad(unsigned pressed, unsigned held)
{
    if (!P.open) return 0;
    const unsigned take = 0x00f0u | 0x0002u | 0x0008u;
    unsigned edge = pressed & take;
    if (P.focus == PROF_F_NAME) {
        /* a D-pad bit that is only a letter being typed (W A S D) is not a step */
        static const struct { unsigned bit; int arrow, letter; } kD[4] = { { 0x40, VK_UP, 'W' }, { 0x80, VK_DOWN, 'S' }, { 0x20, VK_LEFT, 'A' }, { 0x10, VK_RIGHT, 'D' } };
        for (auto &d : kD)
            if ((edge & d.bit) && key_down(d.letter) && !key_down(d.arrow)) edge &= ~d.bit;
        edge &= ~0x0002u;       /* Space is a typed space on the name row; pad A there does nothing */
    }
    if (!edge) return (pressed | held) & take;
    const int old_focus = P.focus, old_cx = P.cx, old_cy = P.cy, old_which = P.which, old_chr = P.chr;
    if (edge & 0x40) {
        if (P.focus == PROF_F_GRID) { if (P.cy > 0) --P.cy; else P.focus = PROF_F_ROW; }
        else if (P.focus == PROF_F_OK || P.focus == PROF_F_BACK) { P.focus = PROF_F_GRID; P.cy = PROF_ROWS - 1; }
        else if (P.focus > PROF_F_NAME) --P.focus;
    }
    if (edge & 0x80) {
        if (P.focus == PROF_F_GRID) { if (P.cy < PROF_ROWS - 1) ++P.cy; else P.focus = PROF_F_OK; }
        else if (P.focus < PROF_F_OK) ++P.focus;
    }
    if (edge & 0x20) {
        if (P.focus == PROF_F_CHAR) step_char(-1, "left");                   /* BROCOLOR1: left / right on the heads: the character */
        else if (P.focus == PROF_F_ROW) goto_pick(P.which - 1);               /* left on the row labels: the row before */
        else if (P.focus == PROF_F_GRID) { if (P.cx > 0) --P.cx; }
        else if (P.focus == PROF_F_BACK) P.focus = PROF_F_OK;
    }
    if (edge & 0x10) {
        if (P.focus == PROF_F_CHAR) step_char(1, "right");
        else if (P.focus == PROF_F_ROW) goto_pick(P.which + 1);               /* right on the row labels: the next row */
        else if (P.focus == PROF_F_GRID) { if (P.cx < PROF_COLS - 1) ++P.cx; }
        else if (P.focus == PROF_F_OK) P.focus = PROF_F_BACK;
    }
    if (old_focus != P.focus || old_cx != P.cx || old_cy != P.cy || old_which != P.which || old_chr != P.chr)
        fprintf(stderr, "[profile] focus %d row %s cell %d,%d\n", P.focus, row_name(P.which), P.cx, P.cy);
    if (edge & 0x0008) { if (P.tick > 8) { finish(P.focus == PROF_F_BACK ? 0 : 1); return take; } }   /* Enter / Start: the focused button; OK from every other row */
    if (edge & 0x0002) {
        if (P.tick > 8) {
            if (P.focus == PROF_F_OK) { finish(1); return take; }
            if (P.focus == PROF_F_BACK) { finish(0); return take; }
            if (P.focus == PROF_F_CHAR) step_char(1, "A");                                   /* A on the heads: the next character */
            if (P.focus == PROF_F_ROW) goto_pick((P.which + 1) % nrows());                  /* A on the row labels: the next row */
            if (P.focus == PROF_F_GRID) {                                                    /* A on a cell picks it for the row */
                set_row(P.which, cell_of(P.which, P.cx, P.cy));
                fprintf(stderr, "[profile] %s%s%s picked: cell %d,%d = %06X\n", P.chr ? kCharName[P.chr] : "", P.chr ? " " : "", row_name(P.which), P.cx, P.cy,
                        row_colour(P.which));
            }
        }
    }
    return (pressed | held) & take;
}

int profile_touch(int sx, int sy, int edge)
{
    if (!P.open) return 0;
    if (!edge) return 1;
    auto in = [&](int x, int y, int w, int h) { return sx >= x && sx < x + w && sy >= y && sy < y + h; };
    if (in(PROF_OK_X, PROF_OK_Y, PROF_OK_W, PROF_OK_H)) { if (P.tick > 8) finish(1); return 1; }
    if (in(PROF_BACK_X, PROF_BACK_Y, PROF_BACK_W, PROF_BACK_H)) { if (P.tick > 8) finish(0); return 1; }
    if (in(PROF_FIELD_X, PROF_FIELD_Y, PROF_FIELD_W, PROF_FIELD_H)) { P.focus = PROF_F_NAME; return 1; }
    for (int k = 0; k < PROF_CHARS; ++k)                    /* BROCOLOR1: a head shows its character */
        if (in(PROF_CHAR_X0 + k * PROF_CHAR_STEP, PROF_CHAR_Y, PROF_CHAR_S, PROF_CHAR_S)) {
            P.focus = PROF_F_CHAR;
            if (P.chr != k) { P.chr = k; goto_pick(0); fprintf(stderr, "[profile] character %s (click)\n", kCharName[P.chr]); }
            return 1;
        }
    if (in(PROF_LB_X, PROF_CHAR_Y + (PROF_CHAR_S - PROF_HINT_H) / 2, PROF_HINT_W, PROF_HINT_H)) { P.focus = PROF_F_CHAR; step_char(-1, "click LB"); return 1; }
    if (in(PROF_RB_X, PROF_CHAR_Y + (PROF_CHAR_S - PROF_HINT_H) / 2, PROF_HINT_W, PROF_HINT_H)) { P.focus = PROF_F_CHAR; step_char(1, "click RB"); return 1; }
    if (!P.chr) {
        if (in(PROF_TAB0_X, PROF_TAB_Y, PROF_TAB_W, PROF_TAB_H)) { P.focus = PROF_F_ROW; goto_pick(0); return 1; }
        if (in(PROF_TAB1_X, PROF_TAB_Y, PROF_TAB_W, PROF_TAB_H)) { P.focus = PROF_F_ROW; goto_pick(1); return 1; }
    } else {
        for (int k = 0; k < PROF_BRO_ROWS; ++k)
            if (in(PROF_TAB0_X + k * PROF_TAB4_STEP, PROF_TAB_Y, PROF_TAB4_W, PROF_TAB_H)) { P.focus = PROF_F_ROW; goto_pick(k); return 1; }
    }
    if (in(PROF_GRID_X, PROF_GRID_Y, PROF_GRID_W, PROF_GRID_H)) {
        const int x = (sx - PROF_GRID_X) / PROF_CELL_STEPX, y = (sy - PROF_GRID_Y) / PROF_CELL_STEPY;
        if (x >= 0 && x < PROF_COLS && y >= 0 && y < PROF_ROWS) {
            P.cx = x; P.cy = y; P.focus = PROF_F_GRID;
            set_row(P.which, cell_of(P.which, x, y));
            fprintf(stderr, "[profile] %s%s%s picked by click: cell %d,%d = %06X\n", P.chr ? kCharName[P.chr] : "", P.chr ? " " : "", row_name(P.which), x, y,
                    row_colour(P.which));
        }
        return 1;
    }
    return 1;
}

/* BROCOLOR1: LB / Q the character before, RB / E the next (Q and E are letters while the name field has the focus) */
extern "C" void menu_online_profile_bumpers(unsigned keys)
{
    const unsigned edge = keys & ~g_bumper_prev;
    g_bumper_prev = keys;
    if (!P.open || !edge || P.tick <= 8) return;
    const int name = P.focus == PROF_F_NAME;
    if ((edge & 1u) || (!name && (edge & 4u))) step_char(-1, (edge & 1u) ? "LB" : "Q");
    else if ((edge & 2u) || (!name && (edge & 8u))) step_char(1, (edge & 2u) ? "RB" : "E");
}

int profile_back(void)
{
    if (!P.open) return 0;
    if (P.tick > 2) finish(0);
    return 1;
}

int profile_backspace(void)
{
    if (!P.open) return 0;
    if (P.focus == PROF_F_NAME) {
        if (P.len > 0) { P.name[--P.len] = 0; log_name("backspace"); }
        return 1;
    }
    return profile_back();
}

/* ---- draw side ----------------------------------------------------------------------------------------------------- */
namespace {

inline int clampi(int v, int lo, int hi) { return v < lo ? lo : v > hi ? hi : v; }
inline uint32_t hex6(const char *s)
{
    uint32_t v = 0;
    for (int i = 0; i < 6; ++i) {
        const char c = s[i];
        v = v * 16 + (uint32_t)(c <= '9' ? c - '0' : (c | 32) - 'a' + 10);
    }
    return v;
}
void fill(uint32_t *dst, int stride, int w, int h, int x0, int y0, int x1, int y1, uint32_t rgb)
{
    x0 = clampi(x0, 0, w); x1 = clampi(x1, 0, w); y0 = clampi(y0, 0, h); y1 = clampi(y1, 0, h);
    for (int y = y0; y < y1; ++y)
        for (int x = x0; x < x1; ++x) dst[(size_t)y * stride + x] = 0xFF000000u | rgb;
}
void frame(uint32_t *dst, int stride, int w, int h, int x0, int y0, int x1, int y1, int t, uint32_t rgb)
{
    fill(dst, stride, w, h, x0, y0, x1, y0 + t, rgb);
    fill(dst, stride, w, h, x0, y1 - t, x1, y1, rgb);
    fill(dst, stride, w, h, x0, y0, x0 + t, y1, rgb);
    fill(dst, stride, w, h, x1 - t, y0, x1, y1, rgb);
}

/* BROCOLOR1: THE FOUR HEADS are the cartridge's own character select portraits (the StarSel busts): data/2D_cad/d_2d_player_select_<name>_8bit_ncg
   (file ids 150 Mario, 148 Luigi, 152 Wario, 154 Yoshi; the _ncl palette is the next id). Each _ncg is one LZ10 stream of 16384 bytes, a 128 x 128
   sheet of 8 x 8 8-bit tiles, 16 a row, index 0 transparent, the bust in the top-left 80 x 80 (the frame lattice lies beyond it): the same read
   hal/head_icons.cpp makes for the player list, kept here at full size. The HD UI mod carries no heads. Nothing is stored: the player's own files
   are read once, on the first draw; a file that cannot be read leaves that head's box empty. Area-averaged to the box with the coverage as alpha. */
const int kBust = 80;
uint32_t g_bust[PROF_CHARS][kBust * kBust];     /* 0xAARRGGBB, straight alpha (0 or 255) */
int g_bust_state[PROF_CHARS];                   /* 0 not tried, 1 ok, -1 failed */
struct BustScaled { int size; uint32_t *px; } g_bust_scaled[PROF_CHARS];

bool bust_read(const char *path, unsigned char **out, long *len)
{
    /* a catalog path is relative to <asset root>/extracted/dsd/files, where hal/fs.cpp reads (player_list_paint.h's plist_rom_path) */
    char full[600];
    const char *root = getenv("SM64DS_ASSET_ROOT");
    if (path && root && *root) {
        snprintf(full, sizeof full, "%s/extracted/dsd/files/%s", root, path);
        path = full;
    }
    FILE *f = path ? fopen(path, "rb") : 0;
    if (!f) return false;
    fseek(f, 0, SEEK_END);
    const long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (n < 8 || n > (1 << 20)) { fclose(f); return false; }
    unsigned char *b = (unsigned char *)malloc((size_t)n);
    const bool ok = b && fread(b, 1, (size_t)n, f) == (size_t)n;
    fclose(f);
    if (!ok) { free(b); return false; }
    *out = b;
    *len = n;
    return true;
}
bool bust_lz10(const unsigned char *s, long n, unsigned char *dst, long want)
{
    if (n < 4 || s[0] != 0x10) return false;
    const long size = s[1] | (s[2] << 8) | (s[3] << 16);
    if (size != want) return false;
    long o = 4, d = 0;
    while (d < size) {
        if (o >= n) return false;
        const unsigned flags = s[o++];
        for (int k = 0; k < 8 && d < size; ++k) {
            if (flags & (0x80u >> k)) {
                if (o + 2 > n) return false;
                const unsigned x = ((unsigned)s[o] << 8) | s[o + 1];
                o += 2;
                const long len = (long)(x >> 12) + 3, disp = (long)(x & 0xfff) + 1;
                if (disp > d) return false;
                for (long i = 0; i < len && d < size; ++i, ++d) dst[d] = dst[d - disp];
            } else {
                if (o >= n) return false;
                dst[d++] = s[o++];
            }
        }
    }
    return true;
}
const uint32_t *bust(int c)
{
    if (c < 0 || c >= PROF_CHARS) return 0;
    if (!g_bust_state[c]) {
        static const unsigned kNcg[PROF_CHARS] = { 154, 150, 148, 152 };     /* Yoshi Mario Luigi Wario */
        static unsigned char tiles[16384];
        unsigned char *g = 0, *l = 0;
        long gn = 0, ln = 0;
        g_bust_state[c] = -1;
        if (bust_read(port_fs_catalog_path(kNcg[c]), &g, &gn) && bust_read(port_fs_catalog_path(kNcg[c] + 1), &l, &ln) && ln >= 512 &&
            bust_lz10(g, gn, tiles, 16384)) {
            for (int y = 0; y < kBust; ++y)
                for (int x = 0; x < kBust; ++x) {
                    const unsigned idx = tiles[((y >> 3) * 16 + (x >> 3)) * 64 + (y & 7) * 8 + (x & 7)];
                    const unsigned v = l[idx * 2] | ((unsigned)l[idx * 2 + 1] << 8);
                    g_bust[c][y * kBust + x] = idx ? (0xFF000000u | (((v & 31) * 255 / 31) << 16) | ((((v >> 5) & 31) * 255 / 31) << 8) |
                                                      (((v >> 10) & 31) * 255 / 31)) : 0;
                }
            g_bust_state[c] = 1;
            fprintf(stderr, "[profile] head %d: the cartridge's portrait (file %u)\n", c, kNcg[c]);
        } else {
            fprintf(stderr, "[profile] head %d: portrait file %u not readable, box left empty\n", c, kNcg[c]);
        }
        free(g);
        free(l);
    }
    return g_bust_state[c] > 0 ? g_bust[c] : 0;
}
/* the bust area-averaged to size x size, premultiplied 0xAARRGGBB, cached per size */
const uint32_t *bust_scaled(int c, int size)
{
    const uint32_t *src = bust(c);
    if (!src || size < 4 || size > 1024) return 0;
    BustScaled &b = g_bust_scaled[c];
    if (b.px && b.size == size) return b.px;
    free(b.px);
    b.px = (uint32_t *)malloc((size_t)size * size * 4);
    b.size = size;
    if (!b.px) return 0;
    for (int oy = 0; oy < size; ++oy)
        for (int ox = 0; ox < size; ++ox) {
            const double x0 = (double)ox * kBust / size, x1 = (double)(ox + 1) * kBust / size;
            const double y0 = (double)oy * kBust / size, y1 = (double)(oy + 1) * kBust / size;
            double r = 0, g = 0, bl = 0, a = 0, area = 0;
            for (int sy = (int)y0; sy < kBust && sy < y1; ++sy)
                for (int sx = (int)x0; sx < kBust && sx < x1; ++sx) {
                    const double wx = (sx + 1 < x1 ? sx + 1 : x1) - (sx > x0 ? sx : x0);
                    const double wy = (sy + 1 < y1 ? sy + 1 : y1) - (sy > y0 ? sy : y0);
                    const double wt = wx * wy;
                    const uint32_t v = src[sy * kBust + sx];
                    area += wt;
                    if (!(v >> 24)) continue;
                    a += wt;
                    r += wt * ((v >> 16) & 255); g += wt * ((v >> 8) & 255); bl += wt * (v & 255);
                }
            if (area <= 0) area = 1;
            const unsigned A = (unsigned)(a / area * 255 + 0.5);
            b.px[oy * size + ox] = (A << 24) | ((unsigned)(r / area + 0.5) << 16) | ((unsigned)(g / area + 0.5) << 8) | (unsigned)(bl / area + 0.5);
        }
    return b.px;
}
/* src over dst, premultiplied; dim 0..255 fades the picture toward the plate colour underneath (256 = full) */
void blit_bust(uint32_t *dst, int stride, int w, int h, int x0, int y0, const uint32_t *px, int size, int keep)
{
    for (int y = 0; y < size; ++y) {
        const int dy = y0 + y;
        if (dy < 0 || dy >= h) continue;
        for (int x = 0; x < size; ++x) {
            const int dx = x0 + x;
            if (dx < 0 || dx >= w) continue;
            const uint32_t v = px[y * size + x];
            const unsigned a = ((v >> 24) * (unsigned)keep) >> 8;
            if (!a) continue;
            uint32_t &d = dst[(size_t)dy * stride + dx];
            const unsigned na = 255 - a;
            const unsigned r = (((v >> 16) & 255) * (unsigned)keep >> 8) + ((d >> 16) & 255) * na / 255;
            const unsigned g = (((v >> 8) & 255) * (unsigned)keep >> 8) + ((d >> 8) & 255) * na / 255;
            const unsigned b = ((v & 255) * (unsigned)keep >> 8) + (d & 255) * na / 255;
            d = 0xFF000000u | ((r > 255 ? 255 : r) << 16) | ((g > 255 ? 255 : g) << 8) | (b > 255 ? 255 : b);
        }
    }
}
inline uint32_t rgb_of(const unsigned char *c) { return ((uint32_t)c[0] << 16) | ((uint32_t)c[1] << 8) | c[2]; }

}  // namespace

void menu_online_profile_draw(const MenuLayout *L, const OnlFacts *of, uint32_t *dst, int w, int h, int stride)
{
    const ProfFacts &f = of->prof;
    const int un = L->unum, ud = L->uden;
    auto X = [&](int dsx) { return w / 2 + (dsx - 128) * un / ud; };
    auto Y = [&](int dsy) { return dsy * un / ud; };
    auto S = [&](int u) { return u * un / ud; };
    /* the panel: the parchment of the other modal screens, a little taller */
    const int px = X(PROF_PANEL_X), py = Y(PROF_PANEL_Y), pw = S(PROF_PANEL_W), ph = S(PROF_PANEL_H);
    const int bw = clampi(S(3), 2, 10);
    fill(dst, stride, w, h, px, py, px + pw, py + ph, 0x8A5A2A);
    for (int y = py + bw; y < py + ph - bw; ++y) {
        const int k = (y - py) * 255 / ph;
        const uint32_t r = (0xF4 * (255 - k) + 0xE6 * k) / 255, g = (0xE4 * (255 - k) + 0xCE * k) / 255, b = (0xB8 * (255 - k) + 0x96 * k) / 255;
        fill(dst, stride, w, h, px + bw, y, px + pw - bw, y + 1, (r << 16) | (g << 8) | b);
    }
    if (!ui_text_ready()) return;
    const UiSurface sf = { dst, stride, w, h };
    const float em = 10.5f * (float)un / (float)ud;
    ui_text_draw(&sf, (float)X(PROF_FIELD_X), (float)Y(14), "Your name", em, 0x6B3A12, 255, 0);
    /* the field */
    const int fx0 = X(PROF_FIELD_X), fy0 = Y(PROF_FIELD_Y), fx1 = X(PROF_FIELD_X + PROF_FIELD_W), fy1 = Y(PROF_FIELD_Y + PROF_FIELD_H);
    fill(dst, stride, w, h, fx0, fy0, fx1, fy1, 0xFFF8E4);
    frame(dst, stride, w, h, fx0, fy0, fx1, fy1, clampi(S(f.focus == PROF_F_NAME ? 2 : 1), 1, 6), f.focus == PROF_F_NAME ? 0xD2641E : 0x8A5A2A);
    const float fem = 12.f * (float)un / (float)ud;
    const float fbase = (float)(fy0 + fy1) * 0.5f + ui_text_cap_height(fem) * 0.5f;
    const int clip[4] = { fx0 + 2, fy0 + 2, fx1 - 2, fy1 - 2 };
    const float tx = (float)(fx0 + S(6));
    ui_text_draw(&sf, tx, fbase, f.name, fem, 0x4A2208, 255, clip);
    if (f.focus == PROF_F_NAME && f.caret) {
        const int cx = (int)(tx + ui_text_width(f.name, fem)) + S(1);
        fill(dst, stride, w, h, cx, fy0 + S(4), cx + clampi(S(1), 1, 3), fy1 - S(4), 0x4A2208);
    }
    {   /* BROCOLOR1: the hint moved up beside the label, right-aligned, to make room for the character row */
        const char *hint = "keyboard to change";
        const float hem = 7.5f * (float)un / (float)ud;
        ui_text_draw(&sf, (float)X(PROF_FIELD_X + PROF_FIELD_W) - ui_text_width(hint, hem), (float)Y(14), hint, hem, 0x9A7A4A, 255, 0);
    }
    /* BROCOLOR1: the character row. LB and RB hints in the plank look either side; the heads on plates, the one on show raised and framed
       (orange while the row has the focus), the others faded */
    {
        const int hy0 = Y(PROF_CHAR_Y + (PROF_CHAR_S - PROF_HINT_H) / 2), hy1 = Y(PROF_CHAR_Y + (PROF_CHAR_S - PROF_HINT_H) / 2 + PROF_HINT_H);
        const float kem = 7.5f * (float)un / (float)ud;
        for (int side = 0; side < 2; ++side) {
            const int hx0 = X(side ? PROF_RB_X : PROF_LB_X), hx1 = X((side ? PROF_RB_X : PROF_LB_X) + PROF_HINT_W);
            fill(dst, stride, w, h, hx0, hy0, hx1, hy1, 0xC8955A);
            frame(dst, stride, w, h, hx0, hy0, hx1, hy1, clampi(S(1), 1, 4), 0x6B3A12);
            const char *t = side ? "RB" : "LB";
            ui_text_draw(&sf, (float)(hx0 + hx1) * 0.5f - ui_text_width(t, kem) * 0.5f, (float)(hy0 + hy1) * 0.5f + ui_text_cap_height(kem) * 0.5f, t, kem,
                         0xFFF4D8, 255, 0);
        }
        for (int k = 0; k < PROF_CHARS; ++k) {
            const int bx0 = X(PROF_CHAR_X0 + k * PROF_CHAR_STEP), by0 = Y(PROF_CHAR_Y);
            const int bx1 = X(PROF_CHAR_X0 + k * PROF_CHAR_STEP + PROF_CHAR_S), by1 = Y(PROF_CHAR_Y + PROF_CHAR_S);
            const int sel = f.chr == k;
            fill(dst, stride, w, h, bx0, by0, bx1, by1, sel ? 0xFFF1CC : 0xE2CB97);
            const int size = (bx1 - bx0) - 2 * clampi(S(1), 1, 4);
            if (const uint32_t *px = bust_scaled(k, size))
                blit_bust(dst, stride, w, h, bx0 + (bx1 - bx0 - size) / 2, by0 + (by1 - by0 - size) / 2, px, size, sel ? 256 : 150);
            frame(dst, stride, w, h, bx0, by0, bx1, by1, clampi(S(sel ? 2 : 1), 1, 6), sel ? (f.focus == PROF_F_CHAR ? 0xD2641E : 0x4A2208) : 0x8A5A2A);
        }
    }
    /* the row labels, each with its live swatch: two for Yoshi, four for a bro (PALSPLIT1: HAT, SHIRT, OVERALLS, SHOES); the picked row has the
       frame (orange while the labels have the focus) */
    const int nrow = f.chr ? PROF_BRO_ROWS : 2;
    static const char *const kLabY[2] = { "BODY", "SHOES" }, *const kLabB[PROF_BRO_ROWS] = { "HAT", "SHIRT", "OVERALLS", "SHOES" };
    for (int k = 0; k < nrow; ++k) {
        const int tx0 = f.chr ? X(PROF_TAB0_X + k * PROF_TAB4_STEP) : X(k ? PROF_TAB1_X : PROF_TAB0_X);
        const int ty0 = Y(PROF_TAB_Y), tx1 = tx0 + S(f.chr ? PROF_TAB4_W : PROF_TAB_W), ty1 = Y(PROF_TAB_Y + PROF_TAB_H);
        const int sel = f.which == k;
        fill(dst, stride, w, h, tx0, ty0, tx1, ty1, sel ? 0xFFF1CC : 0xEBD7A6);
        frame(dst, stride, w, h, tx0, ty0, tx1, ty1, clampi(S(sel ? 2 : 1), 1, 6), sel ? (f.focus == PROF_F_ROW ? 0xD2641E : 0x4A2208) : 0x8A5A2A);
        const char *lab = f.chr ? kLabB[k] : kLabY[k];
        float lem = (f.chr ? 8.f : 10.f) * (float)un / (float)ud;
        {   /* the label always ends before its swatch, whatever the font measures */
            const float room = (float)(S(f.chr ? PROF_TAB4_W - 3 - 2 - PROF_TAB4_SW - 2 : PROF_TAB_W - 6 - 5 - 34 - 4));
            const float tw = ui_text_width(lab, lem);
            if (tw > room && tw > 0.f) lem *= room / tw;
        }
        ui_text_draw(&sf, (float)(tx0 + S(f.chr ? 3 : 6)), (float)(ty0 + ty1) * 0.5f + ui_text_cap_height(lem) * 0.5f, lab, lem, sel ? 0x4A2208 : 0x8A6A3A, 255, 0);
        const int sx1 = tx1 - S(f.chr ? 2 : 5), sx0 = sx1 - S(f.chr ? PROF_TAB4_SW : 34);
        fill(dst, stride, w, h, sx0, ty0 + S(3), sx1, ty1 - S(3), rgb_of(f.row[k]));
        frame(dst, stride, w, h, sx0, ty0 + S(3), sx1, ty1 - S(3), 1, 0x4A2208);
    }
    /* the grid: column 0 the ROM's classic colours for the row, column 1 white / greys / black, then twelve hues in four shades */
    for (int x = 0; x < PROF_COLS; ++x)
        for (int y = 0; y < PROF_ROWS; ++y) {
            const int x0 = X(PROF_GRID_X + x * PROF_CELL_STEPX), y0 = Y(PROF_GRID_Y + y * PROF_CELL_STEPY);
            const int x1 = X(PROF_GRID_X + x * PROF_CELL_STEPX + PROF_CELL_W), y1 = Y(PROF_GRID_Y + y * PROF_CELL_STEPY + PROF_CELL_H);
            const uint32_t c = f.chr ? profile_bro_cell(f.chr - 1, profile_bro_row(f.which), x, y) : profile_cell_colour(f.which, x, y);
            fill(dst, stride, w, h, x0, y0, x1, y1, c);
            frame(dst, stride, w, h, x0, y0, x1, y1, 1, 0x4A2208);
            if (c == rgb_of(f.row[f.which < PROF_BRO_ROWS ? f.which : 0])) {     /* the cell that is the row's current pick: a light inner mark with a dark edge */
                frame(dst, stride, w, h, x0 + S(2), y0 + S(2), x1 - S(2), y1 - S(2), clampi(S(1), 1, 3), 0xFFFFFF);
                frame(dst, stride, w, h, x0 + S(1), y0 + S(1), x1 - S(1), y1 - S(1), 1, 0x000000);
            }
        }
    if (f.focus == PROF_F_GRID) {                           /* the cell cursor */
        const int x0 = X(PROF_GRID_X + f.cx * PROF_CELL_STEPX), y0 = Y(PROF_GRID_Y + f.cy * PROF_CELL_STEPY);
        const int x1 = X(PROF_GRID_X + f.cx * PROF_CELL_STEPX + PROF_CELL_W), y1 = Y(PROF_GRID_Y + f.cy * PROF_CELL_STEPY + PROF_CELL_H);
        frame(dst, stride, w, h, x0 - S(2), y0 - S(2), x1 + S(2), y1 + S(2), clampi(S(2), 1, 5), 0xD2641E);
    }
    {
        char hexs[80];
        if (f.chr) snprintf(hexs, sizeof hexs, "HAT %06X   SHIRT %06X   OVERALLS %06X   SHOES %06X", rgb_of(f.row[0]), rgb_of(f.row[1]),
                            rgb_of(f.row[2]), rgb_of(f.row[3]));
        else snprintf(hexs, sizeof hexs, "BODY %06X   SHOES %06X", rgb_of(f.row[0]), rgb_of(f.row[1]));
        float cem = (f.chr ? 7.5f : 8.5f) * (float)un / (float)ud;
        {
            const float room = (float)S(PROF_PANEL_W - 16), tw = ui_text_width(hexs, cem);
            if (tw > room && tw > 0.f) cem *= room / tw;
        }
        ui_text_draw(&sf, (float)X(128) - ui_text_width(hexs, cem) * 0.5f, (float)Y(146), hexs, cem, 0x6B3A12, 255, 0);
    }
    if (f.focus == PROF_F_OK || f.focus == PROF_F_BACK) {
        const int bx = f.focus == PROF_F_OK ? PROF_OK_X : PROF_BACK_X, by = f.focus == PROF_F_OK ? PROF_OK_Y : PROF_BACK_Y;
        const int bw2 = f.focus == PROF_F_OK ? PROF_OK_W : PROF_BACK_W, bh2 = f.focus == PROF_F_OK ? PROF_OK_H : PROF_BACK_H;
        const int dh = S(bh2), dw = menu_online_plank_w(f.focus == PROF_F_OK ? ONL_K_OK : ONL_K_BACK, dh);
        const int cx = X(bx + bw2 / 2);
        frame(dst, stride, w, h, cx - dw / 2 - S(3), Y(by) - S(2), cx + dw / 2 + S(3), Y(by) + dh + S(2), clampi(S(2), 1, 5), 0xD2641E);
    }
}

namespace {
void profile_panel_cb(const MenuLayout *L, int piece, uint32_t *dst, int w, int h, int stride)
{
    (void)piece;
    const OnlFacts *f = onl_facts_of(L);
    if (!f || f->scr != ONL_SCR_PROFILE || w < 64 || h < 64) return;
    /* the veil, as the other modal screens */
    for (int y = 0; y < h; ++y) {
        uint32_t *p = dst + (size_t)y * stride;
        for (int x = 0; x < w; ++x) {
            const uint32_t c = p[x];
            p[x] = 0xFF000000u | ((((c >> 16) & 255) * 96 / 255) << 16) | ((((c >> 8) & 255) * 96 / 255) << 8) | ((c & 255) * 96 / 255);
        }
    }
    menu_online_profile_draw(L, f, dst, w, h, stride);
}

/* a transparent touch piece over a DS box (the click maps proportionally into the box), with a hotspot */
void touch_box(MenuLayout *L, int bx, int by, int bw, int bh)
{
    const int un = L->unum, ud = L->uden;
    const int fx = L->w / 2 + (bx - 128) * un / ud, fy = by * un / ud, fw = bw * un / ud, fh = bh * un / ud;
    MenuPiece pl = menu_piece_make(SRC_HOSTIMG, bx, by, bw, bh, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    pl.lden = 1; pl.lnum = 1; pl.fw = (short)fw; pl.fh = (short)fh; pl.fx = (short)fx; pl.fy = (short)fy;
    pl.touch = 1;
    pl.nodraw = 1;
    const int ip = menu_add_piece(L, pl);
    if (ip >= 0) menu_add_hotspot(L, ip, bx, by, bw, bh, -1);
}
}  // namespace

void menu_online_profile_modal(MenuLayout *L, const OnlFacts *f)
{
    MenuPiece p = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    p.bg = 1; p.fixdest = 1; p.fx = 0; p.fy = 0; p.fw = (short)L->w; p.fh = (short)L->h;
    p.custom = profile_panel_cb;
    menu_add_piece(L, p);
    touch_box(L, PROF_FIELD_X, PROF_FIELD_Y, PROF_FIELD_W, PROF_FIELD_H);
    touch_box(L, PROF_LB_X, PROF_CHAR_Y, PROF_RB_X + PROF_HINT_W - PROF_LB_X, PROF_CHAR_S);     /* BROCOLOR1: LB, the heads, RB */
    touch_box(L, PROF_TAB0_X, PROF_TAB_Y, PROF_TAB1_X + PROF_TAB_W - PROF_TAB0_X, PROF_TAB_H);  /* the row labels (two or four across the same span) */
    touch_box(L, PROF_GRID_X, PROF_GRID_Y, PROF_GRID_W, PROF_GRID_H);
    const int un = L->unum, ud = L->uden;
    const int kinds[2] = { ONL_K_OK, ONL_K_BACK };
    const int bxs[2][4] = { { PROF_OK_X, PROF_OK_Y, PROF_OK_W, PROF_OK_H }, { PROF_BACK_X, PROF_BACK_Y, PROF_BACK_W, PROF_BACK_H } };
    for (int k = 0; k < 2; ++k) {
        const int dh = bxs[k][3] * un / ud, dw = menu_online_plank_w(kinds[k], dh);
        const int cx = L->w / 2 + (bxs[k][0] + bxs[k][2] / 2 - 128) * un / ud;
        const int lit = 1;      /* both planks always bright (as NAME1's modal buttons); the focus is the ring the panel draws around the box */
        const int ip = menu_online_add_plank(L, kinds[k], cx - dw / 2, bxs[k][1] * un / ud, dw, dh, bxs[k][0], bxs[k][1], bxs[k][2], bxs[k][3], lit, 0, 0, 1, 0);
        if (ip >= 0) menu_add_hotspot(L, ip, bxs[k][0], bxs[k][1], bxs[k][2], bxs[k][3], -1);
    }
}

void menu_online_add_name_plank(MenuLayout *L, int right_edge_x, int top_y, int h_px)
{
    const int dw = menu_online_plank_w(ONL_K_NAME, h_px);
    const OnlFacts *pf = onl_facts_of(L);      /* PADNAV1: lit while the pad focus is on it */
    const int ip = menu_online_add_plank(L, ONL_K_NAME, right_edge_x - dw, top_y, dw, h_px, ONL_NAME_X, ONL_NAME_Y, ONL_NAME_W, ONL_NAME_H, pf && pf->padfocus == ONL_PF_NAME ? 1 : 0, 0, 0, 1, 0);
    if (ip >= 0) menu_add_hotspot(L, ip, ONL_NAME_X, ONL_NAME_Y, ONL_NAME_W, ONL_NAME_H, -1);
}
