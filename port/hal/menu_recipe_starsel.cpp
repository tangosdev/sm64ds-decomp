/* menu_recipe_starsel.cpp -- the STAR SELECT and GAME OVER menus as one-screen recipes (run menu1, lane STARGO1).
 *
 * Both scenes run in the level process (the star select as the interlude, game over through the
 * crossing) and both keep the ROM's own code untouched: the recipes only re-place what the ROM draws
 * and map the host's pad / mouse onto the input the ROM already reads.
 *
 * STAR SELECT (scene 4, dScStarSel_c, src/_ZN12dScStarSel_c8BehaviorEv.cpp)
 *   The upper screen stays the picture (course medal, 3D stars, names). The bottom screen's star
 *   icons are engine-B OBJ sprites in the top 0x28 rows (the ROM's own touch box: ty < 0x28, :187).
 *   One SRC_SUB piece (OBJ only) lifts the icon rows and sets them under the course name, in the free
 *   band between the lives and star counters. Driver NATIVE: the ROM reads D-pad L/R (0x20 / 0x10,
 *   :205-243) and A itself; a click on the placed piece is inverted to the DS touch the ROM's own
 *   box test (:187) reads. One hotspot per icon, from the live object (+0x114 count, +0x11a.. icon x
 *   bytes, +0x131 availability mask), for hover.
 *   VARIANT 1 the stars row; VARIANT 2 the multiplayer character row (+0x133 == 1), where the whole
 *   bottom screen is the choice: drawn as a half-size panel (the fallback's shape).
 *
 * GAME OVER (scene 8, dScGameOver_c, src/game/stages/d_s_game_over.cpp)
 *   The upper screen stays the picture (GAME OVER lettering, gradient); the yes/no plates (engine-B BG0
 *   labels + BG1 plates) are set under the lettering, one piece per plate. The ROM's touch boxes are
 *   x 8..0xf7 with y 0x38..0x57 (Continue) and 0x68..0x87 (Quit) (:342, :364).
 *   Driver ROM_CURSOR: the ROM keeps a cursor byte (+0x91, 0 Continue, 1 Quit) that the plates'
 *   highlight (func_ov003_020b0730) paints, and a confirm (IsButtonInputValid) commits the CURRENT
 *   one. Only a touch ever moved it. Here the pad moves the byte and repaints through that same
 *   function, so pad and keys reach Quit too; A (rewritten to Start) commits it.
 */
#include "menu_compose.h"

#include "menu_hostimg.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "char_strip.h"
#include "menu_starsel_plate_art.h"

extern "C" {
int  SublevelToLevel(int);
extern signed char data_02092110;
void *port_scene_live_object(void);
int  port_scene_live_id(void);
void _ZN13dScGameOver_c19func_ov003_020b0730Ei(void *self, int arg);
void func_02012790(int id);
int GetOwnerLanguage(void);
extern short data_ov003_020b1774[];
extern short data_ov003_020b1764[];
extern int data_0208ee44;
extern unsigned char data_0209caa0[];
extern unsigned char data_ov003_020b169c[];
}

/* Message::DisplayStarNameForStarSelect, the static the ROM's Behavior calls when the selection changes (declared the way
   hal/message_pump.cpp declares its Message statics) */
struct Message { static void DisplayStarNameForStarSelect(unsigned int star); };

namespace {

inline unsigned char ob(const void *o, int off) { return ((const unsigned char *)o)[off]; }

/* ---------------------------------------------------------------- star select */

const int kStarScene = 4;

int up_starsel(int scene_id)
{
    if (scene_id != kStarScene) return 0;
    const void *o = port_scene_live_object();
    if (o && ob(o, 0x133) == 1) return 2;      /* the multiplayer character row */
    return 1;
}

/* THE UPPER SCREEN ONLY (Tango 10-10: the "TOUCH ME!" head and the row of small star icons on the lower screen are
   not drawn; they were redundant with the big stars and the head is a touch control). The ROM still runs them: they
   are engine-B sprites nobody sees, and a click is turned into the touch the ROM's own boxes read.
   THE BIG STARS are 3D models (src/_ZN12dScStarSel_c6RenderEv.cpp:90-101)80-95): centred on x = 0, 0x18000 apart, at one
   depth, so their screen x is linear in the index. Measured on the 4:3 picture: centres at DS x 128 + (i - (n-1)/2) * 34.8
   (kStarDx10 tenths), centre row y 32, about 34 units across. A click on big star i is mapped to the ROM's icon box i
   (x_i - 8 .. x_i + 7, y < 0x28: :187) and on the star that is already selected to the head's box (func_ov003_020ae358.c:
   x 0x58..0xa7, y within 0x28 of the byte at +0x12b), so a second click (or a double click) starts the act, as the head
   did. The pieces are touch-only host rectangles (nodraw) mapped proportionally into those boxes. */
const int kStarDx10 = 348, kStarCy = 32, kStarHalfW = 17, kStarY0 = 8, kStarY1 = 55;

/* STARSEL3: YOSHI'S CAPS ROW AS SPRITES. The ROM draws each cap as two engine-B sprites, the speech bubble then the cap
   (Render :565-585, OAM::RenderSub at CHAR_X / CHAR_Y), and the "TOUCH ME" head (case 3, OAM::Render at 0x80, PANEL_Y) before them.
   STARSEL2 lifted a screen rectangle around each bubble, so whatever else engine B drew there came along: the head, which grows to
   1.5x when a star is picked (Render :517-530), showed inside the boxes (Thunder). Here each cap is lifted as its own OAM entry
   (MenuPiece.oam_on), found in engine B's OAM each tick, and drawn on Tango's plate; nothing else on the lower screen can reach it.
   Kept in MenuFacts.blob from kCapsAt on. */
struct SsCaps {
    uint8_t cap_oam[3];      /* per character (0 Mario, 1 Luigi, 2 Wario): its cap's OAM index + 1, 0 = not found */
    int16_t cap_box[3][4];   /* that entry's box on the lower screen, DS pixels (x, y, w, h) */
    uint8_t cur_oam;         /* the ROM's caps-row cursor (data_ov001_020abd80, Render :508-510) + 1, 0 = not drawn */
    int16_t cur_box[4];
    uint8_t cursor_mode, char_cursor, cursor_on, strip_mode;   /* +0x133, +0x134, +0x135, +0x130 */
    uint16_t bright_b;       /* engine B's MASTER_BRIGHT (0x0400106C): the plates fade with the sprites they carry */
};
const int kCapsAt = 64;

/* one engine-B OAM entry's box (attr0 y / shape / affine, attr1 x / size), false when it is hidden */
bool sub_obj_box(int i, int16_t box[4], int *area)
{
    const volatile uint16_t *e = (const volatile uint16_t *)(uintptr_t)(0x07000400u + (unsigned)i * 8u);
    const unsigned a0 = e[0], a1 = e[1];
    const bool affine = (a0 & 0x100u) != 0;
    if (!affine && (a0 & 0x200u)) return false;
    static const uint8_t kW[3][4] = { { 8, 16, 32, 64 }, { 16, 32, 32, 64 }, { 8, 8, 16, 32 } };
    static const uint8_t kH[3][4] = { { 8, 16, 32, 64 }, { 8, 8, 16, 32 }, { 16, 32, 32, 64 } };
    const unsigned shape = a0 >> 14, size = a1 >> 14;
    if (shape > 2) return false;
    int w = kW[shape][size], h = kH[shape][size];
    if (affine && (a0 & 0x200u)) { w *= 2; h *= 2; }
    int y = (int)(a0 & 0xffu), x = (int)(a1 & 0x1ffu);
    if (y >= 192) y -= 256;
    if (x >= 256) x -= 512;
    box[0] = (int16_t)x; box[1] = (int16_t)y; box[2] = (int16_t)w; box[3] = (int16_t)h;
    *area = w * h;
    return true;
}

/* the smallest shown entry whose box holds (px, py), among indices [lo, hi), no larger than 32 x 32 (the caps and the cursor are 16 x 16; the
   head's quarters are 64 x 64 and the bubbles 32 x 32 or 16 x 32, larger than the cap they hold); -1 none */
int sub_obj_at(int px, int py, int lo, int hi, int16_t box[4])
{
    int best = -1, best_area = 32 * 32 + 1;
    for (int i = lo; i < hi && i < 128; ++i) {
        int16_t b[4];
        int a;
        if (!sub_obj_box(i, b, &a)) continue;
        if (px < b[0] || px >= b[0] + b[2] || py < b[1] || py >= b[1] + b[3]) continue;
        if (a < best_area) { best = i; best_area = a; memcpy(box, b, sizeof b); }
    }
    return best;
}

/* the strip slot of character c: dScStarSel_c::func_ov003_020adec0 (src/game/stages/d_s_star_sel_methods.cpp:147), the slot the ROM's own
   touch on that cap writes to CHAR_CURSOR (func_ov003_020ae358 :376) */
int rom_slot_of(const unsigned char *o, int c)
{
    const int mode = o[0x130];
    if (mode == 4) {
        for (int i = 0; i < 3; ++i) if (data_ov003_020b169c[i] == c) return i;
    } else if (mode >= 2) {
        const unsigned unl = *(const unsigned *)(data_0209caa0 + 8);
        int slot = 0;
        for (int other = 0; other < 3; ++other)
            if ((unl >> other) & 1) { if (other == c) return slot; ++slot; }
    }
    return 0;
}

/* the cap the caps row's cursor is on: dScStarSel_c::func_ov003_020adf50 (:178) */
int rom_char_at_cursor(const unsigned char *o)
{
    const int mode = o[0x130];
    if (mode == 4) return data_ov003_020b169c[o[0x134]];
    if (mode >= 2) {
        const unsigned unl = *(const unsigned *)(data_0209caa0 + 8);
        int slot = 0;
        for (int c = 0; c < 3; ++c)
            if ((unl >> c) & 1) { if (slot == o[0x134]) return c; ++slot; }
    }
    return 0;
}

void caps_facts(const unsigned char *o, MenuFacts *f)
{
    SsCaps K;
    memset(&K, 0, sizeof K);
    K.cursor_mode = o[0x133]; K.char_cursor = o[0x134]; K.cursor_on = o[0x135]; K.strip_mode = o[0x130];
    K.bright_b = *(volatile unsigned short *)0x0400106Cu;
    int hi = 128;
    for (int c = 0; c < 3; ++c) {
        if (!((f->blob[7] >> c) & 1) || f->blob[c] == 0) continue;
        /* the cap sits in the bubble's upper part (measured: centre CHAR_X, CHAR_Y - 8); the bubble holds the same point and is larger */
        const int i = sub_obj_at(f->blob[c], f->blob[3 + c] - 8, 0, 128, K.cap_box[c]);
        if (i >= 0) { K.cap_oam[c] = (uint8_t)(i + 1); if (i < hi) hi = i; }
    }
    if (K.cursor_on && K.cursor_mode == 1) {
        const int c = rom_char_at_cursor(o);
        if (c >= 0 && c < 3 && f->blob[c]) {
            /* RenderSub(data_ov001_020abd80, CHAR_X - 0x24, CHAR_Y - 8): drawn before the head and the caps */
            const int i = sub_obj_at(f->blob[c] - 0x24, f->blob[3 + c] - 8, 0, hi, K.cur_box);
            if (i >= 0) K.cur_oam = (uint8_t)(i + 1);
        }
    }
    memcpy(f->blob + kCapsAt, &K, sizeof K);
#ifndef PORT_ROM_CLEAN
    {
        static int dump = -1;
        if (dump < 0) dump = getenv("SM64DS_SS3_OAM") ? 1 : 0;
        static unsigned last = 0;
        unsigned sig = 0;
        for (int c = 0; c < 3; ++c) sig = sig * 131u + K.cap_oam[c] * 7u + (unsigned)K.cap_box[c][0] * 3u + (unsigned)K.cap_box[c][1];
        sig = sig * 131u + K.cur_oam + K.cursor_mode * 1000u + K.char_cursor * 100000u + (unsigned)K.cur_box[0] * 17u;
        if (dump && sig != last) {
            last = sig;
            fprintf(stderr, "[ss3] caps oam: mario %d (%d,%d %dx%d) luigi %d (%d,%d %dx%d) wario %d (%d,%d %dx%d) cursor %d (%d,%d %dx%d) mode %d slot %d on %d bright %04x\n",
                    K.cap_oam[0] - 1, K.cap_box[0][0], K.cap_box[0][1], K.cap_box[0][2], K.cap_box[0][3],
                    K.cap_oam[1] - 1, K.cap_box[1][0], K.cap_box[1][1], K.cap_box[1][2], K.cap_box[1][3],
                    K.cap_oam[2] - 1, K.cap_box[2][0], K.cap_box[2][1], K.cap_box[2][2], K.cap_box[2][3],
                    K.cur_oam - 1, K.cur_box[0], K.cur_box[1], K.cur_box[2], K.cur_box[3], K.cursor_mode, K.char_cursor, K.cursor_on, K.bright_b);
            for (int i = 0; i < 128; ++i) {
                int16_t b[4];
                int a;
                if (sub_obj_box(i, b, &a))
                    fprintf(stderr, "[ss3]   oam %3d box (%d,%d %dx%d) attr %04x %04x %04x\n", i, b[0], b[1], b[2], b[3],
                            ((const volatile uint16_t *)(uintptr_t)(0x07000400u + i * 8u))[0], ((const volatile uint16_t *)(uintptr_t)(0x07000400u + i * 8u))[1],
                            ((const volatile uint16_t *)(uintptr_t)(0x07000400u + i * 8u))[2]);
            }
        }
    }
#endif
}

/* MENUSPLIT1: everything build_starsel needs from the live scene object, read once per tick by the decide step */
void starsel_facts(MenuFacts *f)
{
    const void *o = port_scene_live_object();
    if (!o || port_scene_live_id() != kStarScene) return;
    /* Bowser and secret courses: InitResources gates the star count, mask and selection on the course (level <= 0xe,
       src/_ZN12dScStarSel_c13InitResourcesEv.cpp:280), so those bytes are not set there; the ROM draws no star row. */
    if (SublevelToLevel(data_02092110) > 0xe) return;
    f->starsel_valid = 1;
    f->starsel_n = ob(o, 0x114);
    f->starsel_mask = ob(o, 0x131);
    for (int i = 0; i < 8; ++i) f->starsel_x[i] = ob(o, 0x11a + i);
    f->starsel_head_y = ob(o, 0x12b);
    f->spare[0] = 1 + ob(o, 0x115);       /* the selected act index + 1 (0 = nothing read) */
    { static int calls; f->spare[2] = ++calls; }     /* a per-tick counter for the pointer's bob */
    f->spare[1] = ob(o, 0x135);           /* the input phase: 0 intro, 1 waiting for the pad to come up, 2 live */
    f->spare[4] = port_char_strip_up();   /* STARSEL2: the character panel is part of this screen */
    f->spare[5] = port_char_strip_focus();   /* STARSEL2: the panel has the cursor (the star pointer steps aside) */
    /* STARSEL2: YOSHI'S CAPS ROW (the cartridge's own character choice on the lower screen, Render :555-585): with Yoshi and more than one
       character (STRIP_MODE +0x130 > 1) the ROM draws a speech bubble with a cap for each unlocked character i < 3 at CHAR_X / CHAR_Y
       (+0x124 + i / +0x128 + i), and its yellow cursor left of the one under CHAR_CURSOR when CURSOR_MODE (+0x133) is 1. It is shown when the
       panel is not (the CharacterStrip setting off), the panel being the other way to make the same choice. */
    for (int i = 0; i < 3; ++i) { f->blob[i] = ob(o, 0x124 + i); f->blob[3 + i] = ob(o, 0x128 + i); }
    f->blob[6] = (data_0209caa0[0x41] == 3 && ob(o, 0x130) > 1 && !f->spare[4]) ? 1 : 0;
    f->blob[7] = (unsigned char)(*(int *)(data_0209caa0 + 8) & 7);
    if (f->blob[6]) caps_facts((const unsigned char *)o, f);
#ifndef PORT_ROM_CLEAN
    {   /* STARSEL3 proof rows: the pick the ROM made (Behavior :80-98 / func_ov003_020ae358: ANIM_MODE +0x139, PICKED_CHAR +0x132) */
        static int last = -1;
        const int now = ob(o, 0x139) | ob(o, 0x132) << 8;
        if (now != last && getenv("SM64DS_SS3_OAM")) {
            last = now;
            fprintf(stderr, "[ss3] pick state: anim mode %d, picked character %d, cursor mode %d, slot %d, star %d\n", ob(o, 0x139), ob(o, 0x132),
                    ob(o, 0x133), ob(o, 0x134), ob(o, 0x115) + 1);
        }
    }
#endif
#ifndef PORT_ROM_CLEAN
    {
        static unsigned last = 0xffffffffu;
        const unsigned now = (unsigned)f->blob[0] | (unsigned)f->blob[1] << 6 | (unsigned)f->blob[2] << 12 | (unsigned)f->blob[3] << 18 | (unsigned)f->blob[6] << 31;
        if (now != last && f->blob[6]) {
            last = now;
            fprintf(stderr, "[ss2] caps row: x %d %d %d y %d %d %d, unlocked %x, strip mode %d, cursor mode %d\n", f->blob[0], f->blob[1], f->blob[2],
                    f->blob[3], f->blob[4], f->blob[5], f->blob[7], ob(o, 0x130), ob(o, 0x133));
        }
    }
#endif
}

/* STARSEL093 (D7): the keys of the star select while the one-screen menu is up. The lower screen is not drawn, so the ROM's states
   that live only there must not be reachable:
   - Down (the DS moves the cursor onto the Mario head on the lower screen, 0x133 = 2, where Left / Right do nothing: the "cursor
     stops" report) is dropped, except where it opens the character row (0x130 > 1 and the character is 3: variant 2, drawn).
   - the first press only woke the pad (0x135 0 -> 2) and did nothing else: the press that wakes it is let through as the live
     phase, so one press of A starts the act and the first Left / Right moves.
   - the mouse over a star selects it (the ROM's own selection change, without the touch edge that would start the act).
   Added to the pressed word only; the ROM reads the keys itself (native driver). */
extern "C" void port_host_keys_clear(unsigned short mask);   /* hal/scene_boot.cpp */
void starsel_hover(unsigned char *o);
extern "C" unsigned char data_020a0e40;
extern "C" int data_020a0e58[];
void starsel_keys(uint16_t *pressed, int)
{
    unsigned char *o = (unsigned char *)port_scene_live_object();
    if (!o || port_scene_live_id() != kStarScene) return;
    if (SublevelToLevel(data_02092110) > 0xe) return;              /* Bowser / secret courses: the ROM's own star-less screen */
    {   /* INPUTMATRIX1 (Tango: the pad's south button confirms and east backs in every menu): pad B arrives as the DS A bit, which the ROM takes as
           a confirm, so east started the act. It is taken out of the pressed, held and host key words here; pad A (the DS B bit, which the ROM
           also takes as a confirm on this screen) is the confirm, and east does nothing. */
        unsigned short *hh = (unsigned short *)((char *)data_020a0e58 + data_020a0e40 * 4);
        *pressed = (uint16_t)(*pressed & ~0x0001u);
        *hh = (unsigned short)(*hh & ~0x0001u);
        port_host_keys_clear(0x0001);
    }
    starsel_hover(o);
    if (o[0x133] == 1) return;                                      /* the character row: the ROM's own keys */
    const bool charrow = o[0x130] > 1 && data_0209caa0[0x41] == 3;
    uint16_t k = *pressed;
    if (o[0x139] == 0) {
        if (o[0x133] == 2) o[0x133] = 0;                           /* nothing on the lower screen to be on */
        if (!charrow) k = (uint16_t)(k & ~(unsigned)0x0080u);       /* DS Down */
        if (o[0x135] == 0 && (k & (0x0001u | 0x0002u | 0x0008u | 0x00f0u))) o[0x135] = 2;
        /* PLAY093: with the character row (Yoshi, another character unlocked) the ROM's first confirm on the stars only moves its cursor to the
           lower-screen head (0x133 = 2, BEHAVIOR :87-90) and the second starts the act. The reset above takes that cursor away again each frame,
           so the second press never came: a confirm here is the second press, one press starts the act (as it did before the strip). */
        static const bool yfix = !getenv("SM64DS_STARSEL_YOSHI_FIX") || getenv("SM64DS_STARSEL_YOSHI_FIX")[0] != '0';   /* 0 = the old behaviour, for the before row */
        if (yfix && charrow && o[0x135] == 2 && o[0x133] == 0 && (k & 0x000fu)) o[0x133] = 2;
    }
    *pressed = k;
}

/* THE MOUSE OVER A STAR OR A CAP puts the ROM's cursor on it, the way the pad would (no pick: a click or the confirm does that).
   A star: the ROM's own selection change (+0x115, the star name, CURSOR_MODE 0), without the touch edge that would start the act.
   STARSEL3: a cap of Yoshi's caps row (Thunder: hovering the caps did not move the cursor): CURSOR_MODE 1 and CHAR_CURSOR = that cap's strip
   slot, which is where Down then Left / Right put it (Behavior :205-243, func_ov003_020ae358 sect2) and what the ROM's own touch on the cap
   writes (func_ov003_020ae358 :376); a confirm then picks it (Behavior :87-98). A pointer that wakes the screen wakes it the way the first
   press does (+0x135 = 2, see starsel_keys).
   THE SOUND: hal_menu_hover plays the cursor sound once when the pointer arrives on another hotspot; nothing here plays it again (the
   recipe's own second request used to land on the next tick: two sounds for one hover onto a star). */
void starsel_hover(unsigned char *o)
{
    static unsigned seen;
    int hx, hy;
    unsigned st;
    if (o[0x139] != 0 || !hal_menu_hover_box(&hx, &hy, &st) || st == seen) return;
    seen = st;
    if (data_0209caa0[0x41] == 3 && o[0x130] > 1 && !port_char_strip_up()) {     /* the caps row is drawn (starsel_facts blob[6]) */
        const unsigned unl = *(const unsigned *)(data_0209caa0 + 8);
        for (int c = 0; c < 3; ++c) {
            if (!((unl >> c) & 1) || o[0x124 + c] == 0) continue;
            if (hx != o[0x124 + c] - 24 || hy != o[0x128 + c] - 23) continue;
            const int slot = rom_slot_of(o, c);
            if (o[0x133] != 1 || o[0x134] != slot) {
                o[0x133] = 1;
                o[0x134] = (unsigned char)slot;
                if (o[0x135] == 0) o[0x135] = 2;
#ifndef PORT_ROM_CLEAN
                fprintf(stderr, "[ss3] hover: cap of character %d -> cursor mode 1, slot %d\n", c, slot);
#endif
            }
            return;
        }
    }
    const int n = o[0x114], cur = o[0x115];
    for (int i = 0; i < n && i < 8; ++i) {
        int sx = (int)o[0x11a + i] - 8;
        if (sx < 0) sx = 0;
        if (sx > 240) sx = 240;
        if (((o[0x131] >> i) & 1) && sx == hx && hy == 0) {
            if (i != cur || o[0x133] != 0) {
                o[0x115] = (unsigned char)i;
                o[0x133] = 0;
                o[0x117] = (unsigned char)(data_0208ee44 * 3);
                if (i != cur) Message::DisplayStarNameForStarSelect((unsigned int)i);
#ifndef PORT_ROM_CLEAN
                fprintf(stderr, "[ss3] hover: star %d -> selected, cursor mode 0\n", i);
#endif
            }
            return;
        }
    }
}

/* STARHOOK1: the big stars' rects for hal/menu_starsel_api.cpp (run menu1, for mp1's players-in-stars heads). The upper screen is
   the same picture whether OneScreenMenus is on or off (the recipe keeps it as the ROM draws it), so the rects are in
   TICK-FRAMEBUFFER pixels (w x h = the picture size, unit = h / 192) and do not depend on the layout. Reads no ROM: only the facts.
   WHERE THE STARS ARE. The ROM places star i in the world at x = (i - (n-1)/2) * 0x18000, y 0x28000 (src/_ZN12dScStarSel_c6RenderEv.cpp:90-101) and InitResources writes the matching icon column bytes starsel_x[i] = 0x92 (n even) or 0x80 (n odd) - (n/2) * 0x23 + i * 0x23
   (src/_ZN12dScStarSel_c13InitResourcesEv.cpp:324-328), 35 DS pixels apart, centred on 128. Measured on the 4:3 picture (STARSEL2 shots,
   n = 1..6): the big star's centre column is exactly starsel_x[i] and its centre row 34.5 (0x28000 through the fixed camera; the 16:9 and
   21:9 pictures give the same numbers in units of h / 192, within 0.6 percent of a unit per star). The idle star is 30 x 28.5 DS pixels
   across; the selected one is drawn 1.25x (Render :93-96, scale 0x1400 / 0x1000) and turns about its vertical axis, so its width
   varies and the rect is the widest it gets. Half extents in tenths of a DS pixel. */
const int kBigHalfW10 = 150, kBigHalfH10 = 145, kBigSelHalfW10 = 188, kBigSelHalfH10 = 181, kBigCy10 = 345;

/* STARSEL2 (Tango 10-28): THE STAR SELECT LAYOUT. Tango's picture: the numbers and the big stars on top, a band under them for the caps row,
   then the star's name, the coin score and the course medal lower down, and one bottom row with the lives counter against the left edge of the
   window, the course name in the middle and the star total against the right edge, at every aspect. The 2D of the upper screen is engine A's
   (src/game/stages/d_s_star_sel_methods.cpp Render :455-480 and the counters :203-300): BG2 is the course medal, BG3 the text (the star name,
   SCORE, the course name), OBJ the star numbers, the coin record, the medal's number, the lives and the star total. Those three layers are
   lifted out of the picture and put back band by band; the big stars are the 3D layer and stay where the ROM draws them, so every star rect,
   the pointer and the clicks are unchanged. Bands in DS rows, measured on the 4:3 picture (STARSEL2 shots): numbers 4..11, star name
   56..66, SCORE and the coins 76..91, the medal 96..155, the course name 160..170, the counters 171..187. One unit is a DS pixel at h / 192,
   scale 1 everywhere (the DS pixels stay square and whole). */
struct SsBand { short y0, y1, x0, x1; short dy; uint8_t ax; };
const SsBand kSsBands[] = {
    {   0,  54,   0, 256,   0, MENU_AX_C },    /* the star numbers: where the ROM has them, over the 3D stars */
    {  54,  72,   0, 256,  24, MENU_AX_C },    /* the star's name: under the caps row's band (rows 80..90) */
    {  72,  94,   0, 256,  19, MENU_AX_C },    /* SCORE, the coin and the record (rows 95..110) */
    {  94, 158,  40, 216,  16, MENU_AX_C },    /* the course medal and its number (rows 112..171) */
    { 158, 171,  40, 216,  14, MENU_AX_C },    /* the course name: the middle of the bottom row (rows 172..185) */
    { 171, 192,   0,  96,   0, MENU_AX_L },    /* the lives counter, against the left edge: from DS column 0, so its ink keeps the ROM's 2-column margin, the star total's on the right (STARSEL3, Tango) */
    { 171, 192, 160, 254,   0, MENU_AX_R },    /* the star total, against the right edge */
};

void starsel2_layout(MenuLayout *L)
{
    for (unsigned i = 0; i < sizeof kSsBands / sizeof kSsBands[0]; ++i) {
        const SsBand &b = kSsBands[i];
        MenuPiece p = menu_piece_make(SRC_TOP_2D, b.x0, b.y0, b.x1 - b.x0, b.y1 - b.y0, b.ax, MENU_AY_T, 0, b.y0 + b.dy, 1, 1);
        p.layers = MENU_L_BG2 | MENU_L_BG3 | MENU_L_OBJ;
        p.bg = 1;
        menu_add_piece(L, p);
    }
    L->lift_owners = MENU_L_BG2 | MENU_L_BG3 | MENU_L_OBJ;
}

/* STARSEL3 (Tango 10-31): YOSHI'S CAPS ROW ON PLATES. The ROM's caps row is a speech bubble per cap on the lower screen (Render :565-585). STARSEL2
   lifted each bubble at 2/3 size into the band under the stars; the bubbles are tall (their tails), so the row touched the stars' points above and
   the star's name below. Now each cap sits on Tango's plate (hal/menu_starsel_plate_art.h: the bubble's body without the tail), drawn at a
   whole-number scale so its pixels stay square, and the cap is its own sprite lifted at the same scale and centred on the plate. Luigi's plate
   under star 2, Mario's under star 4, Wario's under star 6, as before. The plate is centred on DS row kPlateCy (the band between the selected
   star's lowest point, row 52.6, and the star name's first row, 80) and is about 18 DS rows tall (kPlateRows), so a clear gap stays above and
   below at every window size.
   CHAR_X / CHAR_Y are per character (i = 0 Mario, 1 Luigi, 2 Wario). A cap the ROM left at x 0 is one InitResources never placed (a file with
   three characters places two caps) and is left out.
   TOUCH: one touch-only rect per plate, mapped proportionally into the bubble's body on the lower screen (CHAR_X - 24 .. + 24, CHAR_Y - 23 .. + 7),
   inside the ROM's own touch box for that cap (func_ov003_020ae358 :376: x within CHAR_X - 0x18 .. + 0x18, y within CHAR_Y - 0x18 .. + 0x13), so
   a click is the ROM's own touch on the cap. Its hotspot box is that body rect, which is how starsel_hover recognises it.
   THE CURSOR: when the ROM's cursor is on the caps row (CURSOR_MODE 1) its own sprite (Render :508-510, left of the bubble) is lifted too and
   placed left of the plate at the same distance, scaled like the plate. */
const int kCapBodyDx = 24, kCapBodyDy = 23, kPlateCy10 = 665, kPlateRows10 = 180;
const int kCapSlotX[3] = { 58, 128, 198 };          /* the 7-star row's stars 2 / 4 / 6 */
const int kCapOrder[3] = { 1, 0, 2 };               /* Luigi, Mario, Wario, left to right */

struct CapGeom { int n; int c[3]; int x[3], y[3]; int s; };   /* per shown cap: character, plate corner in host pixels; s = host pixels per art pixel */

void caps_geom(const MenuLayout *L, CapGeom *g)
{
    memset(g, 0, sizeof *g);
    const MenuFacts &F = L->facts;
    if (!F.starsel_valid || !F.blob[6] || L->uden <= 0) return;
    /* the whole-number scale nearest to kPlateRows DS rows: s = round(rows * unit / art height), at least 1 */
    const long long UN = L->unum, UD = L->uden;
    int s = (int)((2LL * kPlateRows10 * UN + (long long)kCapPlateH * 10 * UD) / (2LL * kCapPlateH * 10 * UD));
    if (s < 1) s = 1;
    g->s = s;
    const int pw = kCapPlateW * s, ph = kCapPlateH * s;
    const int cy = (int)((long long)kPlateCy10 * UN / (10 * UD));
    for (int k = 0; k < 3; ++k) {
        const int c = kCapOrder[k];
        if (!((F.blob[7] >> c) & 1) || F.blob[c] == 0) continue;
        int sx = kCapSlotX[k];
        if (F.starsel_n == 7) sx = F.starsel_x[2 * k + 1];          /* the row's own star column (the same numbers) */
        const int cx = L->w / 2 + (int)((long long)(sx - 128) * UN / UD);
        g->c[g->n] = c; g->x[g->n] = cx - pw / 2; g->y[g->n] = cy - ph / 2;
        ++g->n;
    }
}

/* the plates: the art pixel by pixel at s x s host pixels, faded the way engine B's MASTER_BRIGHT fades the sprites on them (the scan-out's
   law, hal/sub_screen.cpp menu_backdrop_px_of) */
void starsel3_plates(const MenuLayout *L, int, uint32_t *dst, int w, int h, int stride)
{
    CapGeom g;
    caps_geom(L, &g);
    if (!g.n) return;
    SsCaps K;
    memcpy(&K, L->facts.blob + kCapsAt, sizeof K);
    const unsigned mode = (K.bright_b >> 14) & 3u;
    int f = (int)(K.bright_b & 0x1fu);
    if (f > 16) f = 16;
    if (mode != 1 && mode != 2) f = 0;
    uint32_t pal[8], src[8];
    int np = 0;
    const int s = g.s;
    for (int j = 0; j < g.n; ++j) {
        for (int ay = 0; ay < kCapPlateH; ++ay) {
            for (int ax = 0; ax < kCapPlateW; ++ax) {
                const uint32_t a = kCapPlateArt[ay * kCapPlateW + ax];
                if (!(a >> 24)) continue;
                uint32_t c = a;
                if (f) {
                    int q = -1;
                    for (int t = 0; t < np; ++t) if (src[t] == a) { q = t; break; }
                    if (q < 0) {
                        int r = (int)((a >> 16) & 0xff), gg = (int)((a >> 8) & 0xff), b = (int)(a & 0xff);
                        if (mode == 1) { r += (255 - r) * f / 16; gg += (255 - gg) * f / 16; b += (255 - b) * f / 16; }
                        else { r -= r * f / 16; gg -= gg * f / 16; b -= b * f / 16; }
                        q = np < 8 ? np++ : 7;
                        src[q] = a; pal[q] = 0xff000000u | (unsigned)r << 16 | (unsigned)gg << 8 | (unsigned)b;
                    }
                    c = pal[q];
                }
                const int x0 = g.x[j] + ax * s, y0 = g.y[j] + ay * s;
                for (int yy = y0; yy < y0 + s; ++yy) {
                    if (yy < 0 || yy >= h) continue;
                    uint32_t *row = dst + (size_t)yy * stride;
                    for (int xx = x0; xx < x0 + s; ++xx) if (xx >= 0 && xx < w) row[xx] = c;
                }
            }
        }
    }
}

/* the character the caps row's cursor slot stands for (func_ov003_020adf50 on the facts) */
int caps_char_of_slot(const MenuFacts &F, int slot, int mode)
{
    if (mode == 4) return data_ov003_020b169c[slot & 3];
    int n = 0;
    for (int c = 0; c < 3; ++c)
        if ((F.blob[7] >> c) & 1) { if (n == slot) return c; ++n; }
    return 0;
}

void starsel2_caps(MenuLayout *L)
{
    CapGeom g;
    caps_geom(L, &g);
    if (!g.n) return;
    const MenuFacts &F = L->facts;
    SsCaps K;
    memcpy(&K, F.blob + kCapsAt, sizeof K);
    const int s = g.s, pw = kCapPlateW * s, ph = kCapPlateH * s;
    {
        MenuPiece pl = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        pl.bg = 1; pl.lden = 1; pl.lnum = 1; pl.fx = 0; pl.fy = 0; pl.fw = (short)L->w; pl.fh = (short)L->h;
        pl.custom = starsel3_plates;
        menu_add_piece(L, pl);
    }
    const int cur_char = (K.cur_oam && K.cursor_mode == 1) ? caps_char_of_slot(F, K.char_cursor, K.strip_mode) : -1;
    for (int j = 0; j < g.n; ++j) {
        const int c = g.c[j];
        /* the touch rect: the plate, mapped into the bubble's body on the lower screen */
        const int bx = F.blob[c] - kCapBodyDx, by = F.blob[3 + c] - kCapBodyDy;
        MenuPiece t = menu_piece_make(SRC_HOSTIMG, bx, by, 2 * kCapBodyDx, kCapPlateH, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        t.nodraw = 1; t.touch = 1; t.lden = 1; t.lnum = 1;
        t.fx = (short)g.x[j]; t.fy = (short)g.y[j]; t.fw = (short)pw; t.fh = (short)ph;
        const int ti = menu_add_piece(L, t);
        if (ti >= 0) menu_add_hotspot(L, ti, bx, by, 2 * kCapBodyDx, kCapPlateH, -1);
        /* the cap: its own sprite, s host pixels per DS pixel, centred on the plate */
        if (K.cap_oam[c]) {
            const int16_t *b = K.cap_box[c];
            int sx = b[0], sy = b[1], sw = b[2], sh = b[3];
            if (sx < 0) { sw += sx; sx = 0; }
            if (sy < 0) { sh += sy; sy = 0; }
            if (sx + sw > 256) sw = 256 - sx;
            if (sy + sh > 192) sh = 192 - sy;
            if (sw > 0 && sh > 0) {
                MenuPiece p = menu_piece_make(SRC_SUB, sx, sy, sw, sh, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
                p.layers = MENU_L_OBJ; p.oam_on = 1; p.oam_lo = p.oam_hi = (uint8_t)(K.cap_oam[c] - 1);
                p.key_backdrop = 1; p.bg = 1; p.fixdest = 1;
                /* the sprite's whole cell centred on the plate (the cap art sits in its cell as the ROM drew it) */
                p.fx = (short)(g.x[j] + pw / 2 - (b[2] * s) / 2 + (sx - b[0]) * s);
                p.fy = (short)(g.y[j] + ph / 2 - (b[3] * s) / 2 + (sy - b[1]) * s);
                p.fw = (short)(sw * s); p.fh = (short)(sh * s);
                menu_add_piece(L, p);
            }
        }
        /* the ROM's cursor, when it is on this cap: left of the plate, as far from it as from the bubble's body on the DS */
        if (c == cur_char) {
            const int16_t *b = K.cur_box;
            int sx = b[0], sy = b[1], sw = b[2], sh = b[3];
            if (sx < 0) { sw += sx; sx = 0; }
            if (sy < 0) { sh += sy; sy = 0; }
            if (sx + sw > 256) sw = 256 - sx;
            if (sy + sh > 192) sh = 192 - sy;
            if (sw > 0 && sh > 0) {
                MenuPiece p = menu_piece_make(SRC_SUB, sx, sy, sw, sh, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
                p.layers = MENU_L_OBJ; p.oam_on = 1; p.oam_lo = p.oam_hi = (uint8_t)(K.cur_oam - 1);
                p.key_backdrop = 1; p.bg = 1; p.fixdest = 1;
                p.fx = (short)(g.x[j] + (sx - bx) * s);
                p.fy = (short)(g.y[j] + (sy - by) * s);
                p.fw = (short)(sw * s); p.fh = (short)(sh * s);
                menu_add_piece(L, p);
            }
        }
    }
}

/* STARSEL2: the character panel (hal/char_strip.cpp draws it, hal/char_door_skin.cpp paints it), one custom piece drawn on every picture */
void starsel2_panel(const MenuLayout *, int, uint32_t *dst, int w, int h, int stride)
{
    port_char_strip_panel_draw(dst, stride, w, h);
}

bool big_star_rect(const MenuFacts &F, int w, int h, int star, int out[4])
{
    if (!F.starsel_valid || star < 0 || star >= F.starsel_n || star >= 8 || w <= 0 || h <= 0) return false;
    const int cur = F.spare[0] - 1;
    const int hw10 = (star == cur) ? kBigSelHalfW10 : kBigHalfW10, hh10 = (star == cur) ? kBigSelHalfH10 : kBigHalfH10;
    const long long cx10 = (long long)(F.starsel_x[star] - 128) * 10;     /* tenths of a DS unit from the centre column */
    /* host = centre + tenths * h / 1920 (one DS unit is h / 192 host pixels); the width and height are exact */
    out[0] = w / 2 + (int)(((cx10 - hw10) * h) / 1920);
    out[1] = (int)((((long long)kBigCy10 - hh10) * h) / 1920);
    out[2] = (int)((2LL * hw10 * h) / 1920);
    out[3] = (int)((2LL * hh10 * h) / 1920);
    return true;
}

void build_starsel(int variant, MenuLayout *L)
{
    if (variant == 2 && !L->facts.blob[6]) {
        /* the character row: the whole bottom screen, half size, low and centred (a scale-down of the fallback) */
        MenuPiece p = menu_piece_make(SRC_SUB, 0, 0, 256, 192, MENU_AX_C, MENU_AY_B, 0, -4, 1, 2);
        p.touch = 1;
        const int pi = menu_add_piece(L, p);
        for (int j = 0; j < 3; ++j)
            for (int i = 0; i < 3; ++i) menu_add_hotspot(L, pi, i * 85, j * 64, 85, 64, -1);
        L->backdrop = MENU_BACKDROP_TOP_AS_IS;
        L->stage = MENU_STAGE_POST_FADE;
        return;
    }
    const MenuFacts &F = L->facts;       /* the live object's fields, read once per tick by starsel_facts */
    L->no_focus_paint = 1;
    starsel2_layout(L);
    starsel2_caps(L);
    {
        /* no live facts (the layout selftest, a course without a star row): one invisible touch rect on the centre star,
           so the layout is never empty */
        const int n = F.starsel_valid ? F.starsel_n : 1, cur = F.starsel_valid ? F.spare[0] - 1 : 0;
        const unsigned mask = F.starsel_valid ? F.starsel_mask : 1u;
        const long long UN = L->unum, UD = L->uden;
        for (int i = 0; i < n && i < 8; ++i) {
            if (!((mask >> i) & 1)) continue;
            /* the star's centre, DS x tenths -> host x (the picture is centred, one unit = UN / UD host pixels) */
            const int cx10 = 1280 + (2 * i - (n - 1)) * kStarDx10 / 2;
            const int x0 = L->w / 2 + (int)(((long long)(cx10 - 1280) - 10 * kStarHalfW) * UN / (10 * UD));
            const int x1 = L->w / 2 + (int)(((long long)(cx10 - 1280) + 10 * kStarHalfW) * UN / (10 * UD));
            const int y0 = (int)(kStarY0 * UN / UD), y1 = (int)(kStarY1 * UN / UD);
            int sx, sy, sw, sh;
            if (i == cur) {
                /* the head's box, kept inside the part the ROM tests */
                int hy = F.starsel_valid ? F.starsel_head_y : 0x28;
                sx = 0x60; sw = 0x40; sy = hy - 0x20; sh = 0x40;
                if (sy < 0) sy = 0;
            } else {
                sx = (F.starsel_valid ? (int)F.starsel_x[i] : 128) - 8; if (sx < 0) sx = 0; if (sx > 240) sx = 240;
                sy = 0; sw = 16; sh = 0x28;
            }
            MenuPiece p = menu_piece_make(SRC_HOSTIMG, sx, sy, sw, sh, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
            p.nodraw = 1; p.touch = 1; p.lden = 1; p.lnum = 1;
            p.fx = (short)x0; p.fy = (short)y0; p.fw = (short)(x1 - x0); p.fh = (short)(y1 - y0);
            const int pi = menu_add_piece(L, p);
            if (pi >= 0) menu_add_hotspot(L, pi, sx, sy, sw, sh, -1);
        }
        /* THE POINTER: the game's own small blue triangle (menu_hostimg.h) over the selected star, bobbing on the
           8-frame cadence. It is a backdrop piece: a click goes through to the star under it. */
        if (F.starsel_valid && cur >= 0 && cur < n && !F.spare[5] && variant != 2 && menu_hostimg_ready(MENU_IMG_ARROW_DOWN)) {
            static const int kBob[8] = { 0, 1, 2, 3, 4, 3, 2, 1 };       /* half units */
            int iw = 1, ih = 1;
            menu_hostimg_size(MENU_IMG_ARROW_DOWN, &iw, &ih);
            const int dw = 14 * (int)UN / (int)UD, dh = (int)((long long)dw * ih / iw);
            MenuPiece ar = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
            ar.bg = 1; ar.hostimg = MENU_IMG_ARROW_DOWN; ar.lden = 1; ar.lnum = 1; ar.fw = (short)dw; ar.fh = (short)dh;
            const int cx10 = (2 * cur - (n - 1)) * kStarDx10 / 2;
            ar.fx = (short)(L->w / 2 + (int)((long long)cx10 * UN / (10 * UD)) - dw / 2);
            int top = (int)(((long long)(2 * (kStarCy - 12) + kBob[(L->facts.spare[2]) & 7]) * UN) / (2 * UD)) - dh;
            if (top < 0) top = 0;
            ar.fy = (short)top;
            menu_add_piece(L, ar);
        }
    }
    if (F.spare[4]) {
        MenuPiece cp = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        cp.bg = 1; cp.lden = 1; cp.lnum = 1; cp.fx = 0; cp.fy = 0; cp.fw = (short)L->w; cp.fh = (short)L->h;
        cp.custom = starsel2_panel;
        menu_add_piece(L, cp);
    }
    L->backdrop = MENU_BACKDROP_TOP_AS_IS;
    L->stage = MENU_STAGE_POST_FADE;
}

/* ------------------------------------------------------------------ game over */

const int kGoScene = 8;

int up_gameover(int scene_id) { return scene_id == kGoScene ? 1 : 0; }

/* THE LAYOUT, BUILT PER ASSET (Tango 10-10). Engine A's layers are taken apart: BG2 is the grey gradient, BG0 is the
   "TOUCH TO SELECT" text, the OBJ layer is the GAME OVER lettering (dropping in) and the two bouncing arrows. The
   compositor keeps one layer per cell, so hiding the text alone would leave the backdrop colour in its cells; the
   gradient is therefore lifted too and drawn here, from one clean column carried across the WHOLE picture (xfill: the
   gradient is the same in every column), so a wide picture has no seam and no navy side bars. The lettering is its own
   piece (rows 0..135, which holds the drop-in and leaves the arrows out) and the text layer is drawn nowhere.
   The two plates (engine-B BG0 labels + BG1 plates) go under the lettering, one piece each, at 3/4 size: Continue is
   bottom rows 0x36..0x5a, Quit 0x66..0x8a (a plate is two rows taller than its touch box on each side,
   dScGameOver_c::Behavior :342 / :364). */
const int kPlYc = 0x36, kPlYq = 0x66, kPlH = 36;
const int kGradCol = 16;      /* a column clear of the arrows (x 0..12), the text (from x 27) and the lettering */

/* HDART2: thunderit's GAME OVER glyph layout (Game Over Sprites.svg, 6 units per DS pixel): x, y, w, h of each glyph picture in his units, and the
   shift from his page to the DS picture (his GAME OVER sits at the DS lettering's place). g_go_dx = how far each ROM glyph is from its settled x. */
const double kGoGlyph[8][4] = {
    { 102.03, 1241.98, 150.28, 174.08 },
    { 247.39, 1249.24, 156.09, 168.96 },
    { 404.39, 1235.53, 166.60, 179.20 },
    { 570.83, 1251.29, 149.12, 163.84 },
    { 804.27, 1259.43, 152.83, 158.72 },
    { 937.21, 1236.25, 162.40, 179.20 },
    { 1104.71, 1250.88, 149.12, 163.84 },
    { 1259.68, 1240.47, 149.60, 174.08 }
};
const double kGoOffX = 2.1, kGoOffY = -125.0 - 64.0;
/* Three equal bands (Tango): rows 0..63 GAME OVER, 64..127 the two planks, 128..191 CLICK TO SELECT. The lettering's ink spans rows 82..111 in the ROM
   (centre 96.5) and the planks' block 118..176 (centre 147.5), so they move up 64 and 48 rows (the Continue plank's bottom edge sits on the midline, row 96); the text is centred on row 160. */
const int kGoLetterUp = 64, kGoPlankUp = 48, kGoPlYcDst = 118 - kGoPlankUp, kGoPlYqDst = 150 - kGoPlankUp, kGoClickCy = 160;
int g_go_dx[8], g_go_dx0[8];     /* now / at the start: how far each ROM glyph is from its settled x (the start table data_ov003_020b1764) */
bool go_letters_hd(const MenuLayout *L)
{
    if (L->facts.spare[3] <= 0 || L->facts.language > 1) return false;
    for (int i = 0; i < 8; ++i) if (!menu_hostimg_ready(MENU_IMG_GO_L0 + i)) return false;
    return true;
}

void build_gameover(int, MenuLayout *L)
{
    /* BG2 rows 174..183 are the band the ROM hides under the text: on some frames (the arrows' bounce) those cells read as
       the backdrop colour or navy. The gradient is drawn from rows 0..173, rows 172..173 repeat over the band's 10 rows,
       and rows 184..191 are drawn as they are. */
    if (menu_hostimg_ready(MENU_IMG_GO_GRAD)) {
        /* HDART1: thunderit's HD gradient (the HD UI Textures mod) over the whole picture, smooth at any size. It fades in with the ROM's own
           gradient (menu_compose go_fade_update reads the lifted BG2 cell), so the screen opens exactly as the DS one does. */
        MenuPiece g = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        g.bg = 1; g.hostimg = MENU_IMG_GO_GRAD; g.lden = 1; g.lnum = 1; g.fx = 0; g.fy = 0; g.fw = (short)L->w; g.fh = (short)L->h;
        menu_add_piece(L, g);
    } else
    for (int k = 0; k < 7; k++) {
        const int sy = k == 0 ? 0 : (k <= 5 ? 172 : 184), sh = k == 0 ? 174 : (k <= 5 ? 2 : 8),
                  dy = k == 0 ? 0 : (k <= 5 ? 174 + (k - 1) * 2 : 184);
        MenuPiece g = menu_piece_make(SRC_TOP_2D, kGradCol, sy, 1, sh, MENU_AX_C, MENU_AY_T, 0, dy, 1, 1);
        g.layers = MENU_L_BG2; g.bg = 1; g.xfill = 1; g.fill_w = (short)L->canvas_w;
        menu_add_piece(L, g);
    }
    if (go_letters_hd(L)) {
        /* HDART2: thunderit's eight GAME OVER glyphs (the HD UI Textures mod), one picture each, placed where his layout has them and slid
           by how far the ROM's own glyph x is from its settled x, so they drop in the way the cartridge's sprites do (from the window edge, see below). The ROM's OBJ lettering
           stays lifted (and hidden) like the text layer. English only: the other languages keep their own ROM lettering. */
        const long long UN = L->unum, UD = L->uden;
        const double k = (double)UN / (double)UD, ox = ((double)L->w - 256.0 * k) * 0.5;
        for (int i = 0; i < 8; ++i) {
            MenuPiece tx = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
            tx.bg = 1; tx.hostimg = MENU_IMG_GO_L0 + i; tx.lden = 1; tx.lnum = 1;
            /* The glyphs come in from the actual window edge, not the DS picture's: the slide (the ROM's dx, from its start x to 0, with the
               ROM's own delay and landing frame) is stretched by f so that at the start the glyph is just outside the window, whatever the
               aspect (extra = DS units the window shows beyond each side of the 256 columns). Resting place is unchanged (dx 0). */
            const double X = kGoGlyph[i][0] / 6.0 + kGoOffX, gw = kGoGlyph[i][2] / 6.0, extra = ox / k + 2.0, d0 = g_go_dx0[i];
            double f = 1.0;
            if (i < 4 && d0 < 0.0) { f = (X + gw + extra) / -d0; if (f < 1.0) f = 1.0; }
            else if (i >= 4 && d0 > 0.0) { f = (256.0 + extra - X) / d0; if (f < 1.0) f = 1.0; }
            tx.fx = (short)(ox + (X + g_go_dx[i] * f) * k + 0.5);
            tx.fy = (short)((kGoGlyph[i][1] / 6.0 + kGoOffY) * k + 0.5);
            tx.fw = (short)(kGoGlyph[i][2] / 6.0 * k + 0.5); tx.fh = (short)(kGoGlyph[i][3] / 6.0 * k + 0.5);
            menu_add_piece(L, tx);
        }
    } else {
        MenuPiece t = menu_piece_make(SRC_TOP_2D, 0, 0, 256, 136, MENU_AX_C, MENU_AY_T, 0, -kGoLetterUp, 1, 1);
        t.layers = MENU_L_OBJ; t.bg = 1;
        menu_add_piece(L, t);
    }
    MenuPiece c = menu_piece_make(SRC_SUB, 0, kPlYc, 256, kPlH, MENU_AX_C, MENU_AY_T, 0, kGoPlYcDst, 3, 4);
    c.layers = MENU_L_BG0 | MENU_L_BG1;
    c.key_backdrop = 1;
    c.touch = 1;
    const int pc = menu_add_piece(L, c);
    MenuPiece q = menu_piece_make(SRC_SUB, 0, kPlYq, 256, kPlH, MENU_AX_C, MENU_AY_T, 0, kGoPlYqDst, 3, 4);
    q.layers = MENU_L_BG0 | MENU_L_BG1;
    q.key_backdrop = 1;
    q.touch = 1;
    const int pq = menu_add_piece(L, q);
    menu_add_hotspot(L, pc, 8, 0x38, 0xf0, 0x20, 0);
    menu_add_hotspot(L, pq, 8, 0x68, 0xf0, 0x20, 1);
    /* THE CURSOR: the ROM has none (it only paints the selected plate pale yellow after the first pad press), so one of the
       game's own small triangles points at the selected plate from the first frame, centred, its tip just inside the
       plate's top edge (for Quit it overlaps the lower edge of the Continue plate). A backdrop piece: a click goes to the
       plate. Only down / up arrows exist; a side-pointing one is an artist ask. */
    if (L->facts.spare[0] > 0 && menu_hostimg_ready(MENU_IMG_ARROW_DOWN)) {
        static const int kBob[8] = { 0, 1, 2, 3, 4, 3, 2, 1 };       /* half units */
        const long long UN = L->unum, UD = L->uden;
        int iw = 1, ih = 1;
        menu_hostimg_size(MENU_IMG_ARROW_DOWN, &iw, &ih);
        const int dw = 14 * (int)UN / (int)UD, dh = (int)((long long)dw * ih / iw);
        MenuPiece ar = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        ar.bg = 1; ar.hostimg = MENU_IMG_ARROW_DOWN; ar.lden = 1; ar.lnum = 1; ar.fw = (short)dw; ar.fh = (short)dh;
        ar.fx = (short)(L->w / 2 - dw / 2);
        const int plate_top = L->facts.spare[0] == 1 ? kGoPlYcDst : kGoPlYqDst;
        int top = (int)(((long long)(2 * (plate_top - 2) + kBob[L->facts.spare[2] & 7]) * UN) / (2 * UD)) - dh;
        if (top < 0) top = 0;
        ar.fy = (short)top;
        menu_add_piece(L, ar);
    }
    /* HDART1: thunderit's "Click to select" (the HD UI Textures mod; the DS text said "Touch to select" and sat just above the plates, where this
       layout has the lettering and the cursor, so it goes in the free band under the Quit plate). Once the plates are in, like the cursor. */
    if (L->facts.spare[0] > 0 && menu_hostimg_ready(MENU_IMG_GO_CLICK)) {
        const long long UN = L->unum, UD = L->uden;
        int iw = 1, ih = 1;
        menu_hostimg_size(MENU_IMG_GO_CLICK, &iw, &ih);
        const int dh = (int)(10 * UN / UD), dw = (int)((long long)dh * iw / ih);
        MenuPiece tx = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        tx.bg = 1; tx.hostimg = MENU_IMG_GO_CLICK; tx.lden = 1; tx.lnum = 1; tx.fw = (short)dw; tx.fh = (short)dh;
        tx.fx = (short)(L->w / 2 - dw / 2); tx.fy = (short)((kGoClickCy * UN - (long long)5 * UN) / UD);   /* 10 units tall, centred on row 160 */
        menu_add_piece(L, tx);
    }
    L->lift_owners = MENU_L_BG2 | MENU_L_OBJ;
    L->hide_owners = MENU_L_BG0;
    L->backdrop = MENU_BACKDROP_TOP_AS_IS;
    L->stage = MENU_STAGE_POST_FADE;
}

/* MENUSPLIT1 facts for the game over cursor: the ROM's own cursor byte (+0x91, 0 Continue, 1 Quit) once the intro is in
   (+0x94 >= 2); spare[0] = 1 + the selected plate, spare[2] = a per-tick counter for the arrow's bob. */
int g_go_shown;      /* the pad cursor has painted its highlight on this screen */

void gameover_facts(MenuFacts *f)
{
    const void *o = port_scene_live_object();
    if (!o || port_scene_live_id() != kGoScene) return;
    if (ob(o, 0x94) >= 2) f->spare[0] = 1 + (ob(o, 0x91) ? 1 : 0);
    { static int calls; f->spare[2] = ++calls; }
    f->language = (int16_t)GetOwnerLanguage();
    /* HDART2: the glyphs' x (dScGameOver_c +0x50, s16 x8) against their settled x (ROM table data_ov003_020b1774) */
    const short *gx = (const short *)((const unsigned char *)o + 0x50);
    for (int i = 0; i < 8; ++i) {
        g_go_dx[i] = (int)gx[i] - (int)data_ov003_020b1774[i];
        g_go_dx0[i] = (int)data_ov003_020b1764[i] - (int)data_ov003_020b1774[i];
    }
    f->spare[3] = 1;
    /* HDART2: a mouse that moved onto Continue or Quit makes that plate the ROM's chosen one, the way the file select does (the hover sound
       itself is played by hal_menu_hover when the pointer arrives on a plate that is not the lit one). The same ROM cursor byte (+0x91) and the
       same repaint (func_ov003_020b0730) the pad uses, under the same gate (intro in, FSM waiting). */
    static unsigned seen;
    int hx, hy;
    unsigned st;
    if (ob(o, 0x94) >= 2 && ob(o, 0x90) == 0 && hal_menu_hover_box(&hx, &hy, &st) && st != seen) {
        seen = st;
        const int want = hy >= 0x60 ? 1 : 0;       /* the Continue plate's touch box is y 0x38, Quit's is 0x68 */
        unsigned char *s = (unsigned char *)port_scene_live_object();
        if (s[0x91] != want || !g_go_shown) {
            g_go_shown = 1;
            s[0x91] = (unsigned char)want;
            _ZN13dScGameOver_c19func_ov003_020b0730Ei(s, 0);
            f->spare[0] = 1 + want;
        }
    }
}

/* The ROM reads the pad only once the intro is in (+0x94 >= 2) and while its FSM waits (+0x90 == 0). */
int pad_ok_gameover(void)
{
    const void *o = port_scene_live_object();
    if (!o || port_scene_live_id() != kGoScene) { g_go_shown = 0; return 0; }
    return ob(o, 0x94) >= 2 && ob(o, 0x90) == 0;
}

/* Move the ROM's own cursor byte and repaint the plates through the ROM's own function. up / left =
   Continue, down / right = Quit. */
int rom_cursor_gameover(int dir, int *sel_out)
{
    void *o = port_scene_live_object();
    if (!o || port_scene_live_id() != kGoScene) return 0;
    unsigned char *s = (unsigned char *)o;
    const int want = (dir == MENU_DIR_UP || dir == MENU_DIR_LEFT) ? 0 : 1;
    int moved = 0;
    if (s[0x91] != want || !g_go_shown) {
        g_go_shown = 1;      /* the plates show no highlight until something paints one: the first press always does */
        s[0x91] = (unsigned char)want;
        _ZN13dScGameOver_c19func_ov003_020b0730Ei(o, 0);
        host_menu_cursor_sound();
        moved = 1;
    }
    if (sel_out) *sel_out = s[0x91];
    return moved;
}

}  // namespace

extern "C++" {
int  menu_starsel_up(int scene_id) { return up_starsel(scene_id); }
void menu_starsel_pad_map(uint16_t *pressed, int host_back) { starsel_keys(pressed, host_back); }
void menu_starsel_build(int variant, MenuLayout *L) { build_starsel(variant, L); }
void menu_starsel_facts(MenuFacts *f) { starsel_facts(f); }
int  menu_gameover_up(int scene_id) { return up_gameover(scene_id); }
void menu_gameover_build(int variant, MenuLayout *L) { build_gameover(variant, L); }
int  menu_gameover_rom_cursor(int dir, int *sel) { return rom_cursor_gameover(dir, sel); }
int  menu_gameover_pad_ok(void) { return pad_ok_gameover(); }
void menu_gameover_facts(MenuFacts *f) { gameover_facts(f); }
int  menu_starsel_big_rect(const MenuFacts *F, int w, int h, int star, int out[4]) { return big_star_rect(*F, w, h, star, out) ? 1 : 0; }
}
