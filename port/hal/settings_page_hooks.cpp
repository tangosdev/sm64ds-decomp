/* settings_page_hooks.cpp -- how the settings page opens (run menu1, lane SETMENU1). The ROM-aware half of hal/settings_page.cpp.

   With OneScreenMenus on, the title's WRENCH and the pause menu's OPTIONS plate open the port's settings page instead of the ROM's
   own options screen. Both are caught BEFORE the ROM sees the press, so the ROM never leaves the page it is on and Back returns to it
   exactly as it was:
     - a pad / key confirm: hal_menu_pad_seam (hal/sub_screen.cpp) hands the pressed word here first (settings_page_hook_pad);
     - a mouse click or a synthesised stylus press: poll_touch hands the frame's stylus here (settings_page_hook_touch).
   The ROM facts read here are the ones the two ROM screens themselves test:
     PAUSE  src/_ZN5Stage9PS_UpdateEv.cpp: the main page is state 1 (castle) or 3 (course); the Options plate is selection 2 (data_0209f2e0),
            its touch box x 8..0xf7, y 0x70..0x8f in the castle (:277-299) and y 0x68..0x87 in a course (:485-505); it acts only when the
            input lock after the open (data_0209f300) and the press timers (data_0209f22c, data_0209f244) are 0 (hal/menu_recipe_pause.cpp
            pause_cursor holds the same gate); a VS pause (states 4, 5) has no Options plate.
     TITLE  the main page is top-state 0, element machine 0, no choice taken (req -1) (hal/menu_recipe_title.cpp title_cursor_assist); the
            wrench is element 7 (its box from the ROM's own element list, port_title_elem_boxes, else the dumped 216,0..256,40), and the
            ROM's cursor (data_ov007_02103360) names 7 while it is selected.
   Nothing here draws or reads a device. */
#include "settings_page.h"
#include "menu_online.h"
#include "menu_compose.h"

#include <stdint.h>

extern "C" {
extern unsigned char data_0209f248;    /* the pause state PS_Update is in */
extern unsigned char data_0209f2c4;    /* 0 = playing, else the pause is up */
extern unsigned char data_0209f20c;    /* level-clear owns the game */
extern unsigned char data_0209f2e0;    /* the selected plate */
extern unsigned char data_0209f300;    /* input lock after open */
extern unsigned char data_0209f22c;    /* press hold ticks */
extern unsigned char data_0209f244;    /* press flash ticks */
extern char *data_ov007_02103360;      /* the title's element-cursor struct */
extern unsigned char data_020a0e40;    /* the local player's slot */
extern int data_020a0e58[];            /* PadData[i]: {u16 held, u16 pressed} */
int port_scene_live_id(void);
void func_ov007_020bdeb0(int idx);         /* the title scene's own sound call (overlay 7) */
unsigned int func_02012790(unsigned int snd);   /* the common sound call */
int port_title_elem_boxes(int *slot, int *id, int *x0, int *y0, int *x1, int *y1, int max);
int port_title_page_state(int *top, int *elem, int *req);
void port_host_keys_clear(unsigned short mask);   /* hal/scene_boot.cpp */
int menu_pause_slid_away(void);                   /* CAREERLOCK1: hal/menu_recipe_pause.cpp */
}

namespace {

enum { TGT_NONE = 0, TGT_PAUSE = 1, TGT_TITLE = 2 };

int armed_target()
{
    if (settings_page_open()) return TGT_NONE;
    if (!hal_menu_enabled(MENU_KEY_ONE_SCREEN_MENUS)) return TGT_NONE;
    const int scene = port_scene_live_id();
    if ((scene == 3 || scene == -1) && data_0209f2c4 && !data_0209f20c) {
        const unsigned st = data_0209f248;
        /* CAREERLOCK1: not while the pause is slid to the tracker or the Career page, or sliding: the Options plate is off screen, the
           ROM's chosen plate can still be it, and A there opened the settings page the player never picked (it slides back instead) */
        if (menu_pause_slid_away()) return TGT_NONE;
        if ((st == 1 || st == 3) && data_0209f300 == 0 && data_0209f22c == 0 && data_0209f244 == 0) return TGT_PAUSE;
        return TGT_NONE;
    }
    if (scene == 1) {
        int top, elem, req;
        if (port_title_page_state(&top, &elem, &req) && top == 0 && elem == 0 && req == -1) return TGT_TITLE;
    }
    return TGT_NONE;
}

int wrench_box(int *x0, int *y0, int *x1, int *y1)
{
    int slot[24], ids[24], bx0[24], by0[24], bx1[24], by1[24];
    const int n = port_title_elem_boxes(slot, ids, bx0, by0, bx1, by1, 24);
    for (int k = 0; k < n; ++k)
        if (ids[k] == 7) { *x0 = bx0[k]; *y0 = by0[k]; *x1 = bx1[k]; *y1 = by1[k]; return 1; }
    *x0 = 216; *y0 = 0; *x1 = 256; *y1 = 40;
    return 1;
}

/* MENUS093 (D5): the title's MP / VS button (element 1) takes no press: multiplayer is not in this build and the ROM's answer (a
   page change to a heap the host does not mount) restarted the title in the middle of the main menu. The press is taken away before the
   ROM sees it, in either menu mode: a stylus on the button's box, or a confirm while the ROM's cursor is on it. */
int title_main_page()
{
    if (port_scene_live_id() != 1) return 0;
    int top, elem, req;
    return port_title_page_state(&top, &elem, &req) && top == 0 && elem == 0 && req == -1;
}

/* NAME1: the file select page itself (top 0, element machine 5), and the ROM cursor's element (0xff = not shown) */
int title_file_page()
{
    if (port_scene_live_id() != 1) return 0;
    int top, elem, req;
    return port_title_page_state(&top, &elem, &req) && top == 0 && elem == 5;
}

int title_cursor_elem()
{
    if (!data_ov007_02103360) return -1;
    const short *p0 = *(const short **)data_ov007_02103360;
    return p0 ? (int)p0[0] : -1;
}

int vs_box(int *x0, int *y0, int *x1, int *y1)
{
    int slot[24], ids[24], bx0[24], by0[24], bx1[24], by1[24];
    const int n = port_title_elem_boxes(slot, ids, bx0, by0, bx1, by1, 24);
    for (int k = 0; k < n; ++k)
        if (ids[k] == 1) { *x0 = bx0[k]; *y0 = by0[k]; *x1 = bx1[k]; *y1 = by1[k]; return 1; }
    *x0 = 2; *y0 = 148; *x1 = 66; *y1 = 188;
    return 1;
}

int title_cursor_on_vs()
{
    if (!data_ov007_02103360) return 0;
    const short *p0 = *(const short **)data_ov007_02103360;
    return p0 && p0[0] == 1;
}

int title_cursor_on_wrench()
{
    if (!data_ov007_02103360) return 0;
    const short *p0 = *(const short **)data_ov007_02103360;
    return p0 && p0[0] == 7;
}

unsigned short *g_strip_conf;   /* INPUTMATRIX1: the pad hook's confirm strip (see settings_page_hook_pad) */

}  // namespace

/* INPUTMATRIX1: the press (select) sound of a button on the settings page (RESET). The same sounds the page's open uses for the control that
   asked for it: on the title the wrench's press (title sound index 0x14), elsewhere the pause Options plate's press (func_02012790 0x54). */
extern "C" void settings_page_press_sound(void)
{
    if (port_scene_live_id() == 1) func_ov007_020bdeb0(0x14);
    else func_02012790(0x54);
}

/* the pad seam's pressed word, before hal_menu_pad: 1 when this press opened the page (the bits are taken out of the pressed AND the held
   word, so the ROM, which builds its key record from the held word on the title, sees no confirm) */
extern "C" int settings_page_hook_pad(uint16_t *pressed)
{
    {
        /* INPUTMATRIX1 (Thunder, wrench via controller): a confirm this hook took (the wrench, the ? text's close, an online plank) is kept out
           of the pressed AND the held word for as long as the key stays down. It used to be taken on its edge frame only: the settings page opens
           a quarter second later, a real press lasts longer than a frame, and the held word came back on the next frame as a fresh edge for the
           ROM, whose cursor was still on the wrench (its own OPTIONS screen opened behind our page). The same stripping as the D-pad steps below. */
        static unsigned short strip_conf;
        unsigned short *held = (unsigned short *)((char *)data_020a0e58 + data_020a0e40 * 4);
        g_strip_conf = hal_menu_enabled(MENU_KEY_ONE_SCREEN_MENUS) ? &strip_conf : 0;
        /* with the one-screen menus off the ROM's own title runs and menu_title_pad_map_rom rebuilds its pad words every frame before this hook; the
           edge-frame swallow is what that mode was proven with (PADMENUS1 T4), so only the one-screen pages keep the strip */
        if (!g_strip_conf) strip_conf = 0;
        strip_conf = (unsigned short)(strip_conf & (*held | *pressed));
        if (strip_conf) {
            *pressed = (uint16_t)(*pressed & ~strip_conf);
            *held = (unsigned short)(*held & ~strip_conf);
            port_host_keys_clear(strip_conf);
        }
        /* PROFILE1: THE NAME AND COLOUR POPUP TAKES THE PAD HERE, INSIDE THIS FIRST BLOCK AND BEFORE ANY LATER BLOCK, so it is the first to see the words and
           the later modal-pad blocks of this hook (INPUTMATRIX1's "the pad is modal for the ? text, the VS note and the wait screen", which zeroes the pad for
           every online screen) never run while the popup is up: this block returns first. It takes the D-pad and the confirm keys (A / Space, Start / Enter),
           kept out of the ROM's key words for as long as they are down (strip_conf, as above). Pad B and Backspace are not taken here: they come through
           the recipe's back route (menu_title_pad_map), which is why the early return leaves them in the words. */
        {
            const int up = profile_active();          /* before the call: an OK or BACK inside it closes the popup */
            const unsigned take = profile_pad(*pressed, *held);
            if (up) {
                strip_conf = (unsigned short)(strip_conf | (take & (*pressed | *held)));
                *pressed = (uint16_t)(*pressed & ~take);
                *held = (unsigned short)(*held & ~take);
                port_host_keys_clear((unsigned short)take);
                return 1;
            }
        }
    }
    {
        /* INPUTMATRIX1: THE ? TEXT, THE VS NOTE AND THE WAIT SCREEN ARE MODAL FOR THE PAD. While one is up its close button is the only thing the
           pad can reach: A, B and Start close it, and nothing else of the pad (the D-pad, the other buttons) reaches the ROM's title or file select
           underneath until it is closed (the mouse already behaved so). The closing key stays out of the ROM until it is up (strip_conf). */
        const int mk = title_main_page() ? 1 : title_file_page() ? 2 : 0;
        if (mk && menu_online_modal_up()) {
            unsigned short *held = (unsigned short *)((char *)data_020a0e58 + data_020a0e40 * 4);
            const uint16_t cb = 0x0001 | 0x0002 | 0x0008;
            if (*pressed & cb) {
                if (mk == 1) menu_online_press_main(title_cursor_elem(), 1);
                else menu_online_press_file(title_cursor_elem(), 1);
            }
            if (g_strip_conf) *g_strip_conf = (unsigned short)(*g_strip_conf | (cb & (*pressed | *held)));
            *pressed = 0;
            *held = 0;
            port_host_keys_clear(0xffff);
            return 1;
        }
    }
    {
        /* NAME1: the D-pad's way to the ? button. The ROM's cursor has nowhere to go from the wrench to the left, nor up from the file row, so the
           step is the online menu's; it is kept out of the held word for as long as the key is down (the ROM builds its key record from it). */
        static unsigned short strip_dirs;
        unsigned short *held = (unsigned short *)((char *)data_020a0e58 + data_020a0e40 * 4);
        strip_dirs = (unsigned short)(strip_dirs & *held);
        const unsigned dirs = *pressed & 0x00f0u;
        const int kind = title_main_page() ? 1 : title_file_page() ? 2 : 0;
        if (menu_online_enabled()) menu_online_pad_frame(kind, title_cursor_elem());
        if (kind && dirs && menu_online_dir(kind, title_cursor_elem(), dirs)) strip_dirs = (unsigned short)(strip_dirs | dirs);
        if (strip_dirs) {
            *pressed = (uint16_t)(*pressed & ~strip_dirs);
            *held = (unsigned short)(*held & ~strip_dirs);
            port_host_keys_clear(strip_dirs);
        }
    }
    {
        /* MENUS093: a confirm on the title's MP button does nothing. MENUS093B: it is the HELD word the ROM's key record is built from
           (current & ~previous), so a press that lasts more than one frame must be taken out of the held word on every frame it is down, not
           only on the frame of its edge: the guard used to read the pressed word alone, the held word came back on the second frame and the
           ROM saw a fresh edge (a keyboard or pad press is never one frame long). */
        /* NAME1: on the file select 0x0001 (pad B, the port's punch) is the back arrow, not a confirm (hal/menu_recipe_title.cpp title_pad_keys) */
        /* INPUTMATRIX1: pad B (the port's punch bit 0x0001) is never a confirm on the title's host pages: A and Start are. On the main page it is
           the back key (the online planks turn back; solo it does nothing) and stays out of the ROM; on the file select it is the back arrow
           (hal/menu_recipe_title.cpp title_pad_keys). */
        const int onmain = title_main_page() && hal_menu_enabled(MENU_KEY_ONE_SCREEN_MENUS);   /* the ROM two-screen title maps A to the DS A bit before this hook runs, so there 0x0001 is the converted A */
        if (onmain) {
            unsigned short *held = (unsigned short *)((char *)data_020a0e58 + data_020a0e40 * 4);
            if ((*pressed | *held) & 0x0001) {
                if (*pressed & 0x0001) menu_online_back(1);
                if (g_strip_conf) *g_strip_conf = (unsigned short)(*g_strip_conf | 0x0001);
                *pressed = (uint16_t)(*pressed & ~0x0001);
                *held = (unsigned short)(*held & ~0x0001);
                port_host_keys_clear(0x0001);
            }
        }
        const uint16_t bits = (title_file_page() || onmain) ? (uint16_t)(0x0002 | 0x0008) : (uint16_t)(0x0001 | 0x0002 | 0x0008);
        unsigned short *held = (unsigned short *)((char *)data_020a0e58 + data_020a0e40 * 4);
        if ((*pressed | *held) & bits) {
            /* NAME1 (run after093): the MP plank is ONLINE now; its press, the planks it turns, the modal screens and the online file select's
               picks are the online menu's (hal/menu_online.cpp), which says whether the ROM may see this press. A press it keeps is taken out
               of the pressed and the held word on every frame it is down, as the MP swallow always was. */
            const int edge = (*pressed & bits) != 0;
            int swallow = 0;
            if (title_main_page()) swallow = menu_online_press_main(title_cursor_elem(), edge);
            else if (title_file_page()) swallow = menu_online_press_file(title_cursor_elem(), edge);
            if (swallow) {
                if (g_strip_conf) *g_strip_conf = (unsigned short)(*g_strip_conf | (bits & (*pressed | *held)));
                *pressed = (uint16_t)(*pressed & ~bits);
                *held = (unsigned short)(*held & ~bits);
                port_host_keys_clear(bits);
                return 1;
            }
        }
    }
    const int tgt = armed_target();
    if (!tgt) return 0;
    const uint16_t confirm = 0x0002 | 0x0008;           /* the port's jump (pad A, Space) and Start (Enter) */
    if (!(*pressed & confirm)) return 0;
    if (tgt == TGT_PAUSE) {
        if (data_0209f2e0 != 2) return 0;
    } else {
        if (!title_cursor_on_wrench()) return 0;
    }
    unsigned short *held = (unsigned short *)((char *)data_020a0e58 + data_020a0e40 * 4);
    if (g_strip_conf) *g_strip_conf = (unsigned short)(*g_strip_conf | (confirm & (*pressed | *held)));
    *pressed = (uint16_t)(*pressed & ~confirm);
    *held = (unsigned short)(*held & ~confirm);
    if (tgt == TGT_TITLE) port_host_keys_clear(confirm);   /* MENUS093: the title reads the host key word too */
    settings_page_set_source(tgt);
    settings_page_request();
    return 1;
}

/* poll_touch's stylus: 1 when the page takes it (the press that opens it, then everything while it is up and until the finger is lifted) */
/* CHARDOOR1: the white door's picker (hal/char_door.cpp seats this) takes the stylus too: everything while it is up, and until the finger
   is lifted that closed it, so a click on a row is not also a press on the ROM's bottom screen */
extern "C" int (*port_char_door_touch_open)(void) = 0;
extern "C" int settings_page_hook_touch(unsigned char down, unsigned char sx, unsigned char sy)
{
    {
        static int cd_hold;
        if (port_char_door_touch_open && port_char_door_touch_open()) { cd_hold = 1; return 1; }
        if (cd_hold) { if (!down) cd_hold = 0; return 1; }
    }
    if (settings_page_swallow_touch(down)) return 1;
    {   /* NAME1: the online menu reads every stylus state on the main page and the file select (its screens take everything while up) */
        const int kind = title_main_page() ? 1 : title_file_page() ? 2 : 0;
        if (kind && menu_online_touch(kind, down, sx, sy)) return 1;
    }
    if (!down) return 0;
    if (title_main_page()) {                           /* MENUS093: a stylus / click on the title's MP button does nothing */
        int vx0, vy0, vx1, vy1;
        vs_box(&vx0, &vy0, &vx1, &vy1);
        if (sx >= vx0 && sx < vx1 && sy >= vy0 && sy < vy1) return 1;
    }
    const int tgt = armed_target();
    if (!tgt) return 0;
    if (tgt == TGT_PAUSE) {
        const unsigned st = data_0209f248;
        const unsigned y0 = st == 1 ? 0x70u : 0x68u;
        if ((unsigned char)(sx - 8) < 0xf0 && (unsigned char)(sy - y0) < 0x20) { settings_page_set_source(tgt); settings_page_request(); return 1; }
    } else {
        int x0, y0, x1, y1;
        wrench_box(&x0, &y0, &x1, &y1);
        if (sx >= x0 && sx < x1 && sy >= y0 && sy < y1) { settings_page_set_source(tgt); settings_page_request(); return 1; }
    }
    return 0;
}

/* SETPAGEFIX1 / PAUSEFIX1: called once a frame from the idle side of the page's frame; builds the page's caches in small slices a while
   after start, so the first open does not stall and no frame pays more than a slice. The slice runs only when the shared warm-up slot
   (menu_warm_claim, menu_recipe_pause.cpp: one slot per few ms for the pause and the page together, none while the game is hitching)
   is free. Nothing at all when OneScreenMenus is off. */
extern "C" int menu_warm_claim(void);
static int done_w, done_h;
/* lane SEAMART2: hal/settings_page.cpp's HD UI art reset drops what the warm-up built; this lets the warm-up run again at that size */
extern "C" void settings_page_warm_rearm(void) { done_w = done_h = 0; }
extern "C" void settings_page_idle_tick(int idle_frames, int w, int h)
{
    if (idle_frames < 120) return;
    if (!hal_menu_enabled(MENU_KEY_ONE_SCREEN_MENUS)) return;
    if (done_w == w && done_h == h) return;
    if (settings_page_warm_waiting()) return;
    if (!menu_warm_claim()) return;
    if (settings_page_warm_step(w, h)) { done_w = w; done_h = h; }
}
