/* menu_online.cpp -- the title's online menu, logic side (run after093, lane NAME1). See menu_online.h.

   THE SAVE MEDIUM FOLLOWS THE PAGE. ADVENTURE ONLINE switches ntr::backup to the signed online save before the ROM builds its file select (the
   title's loader, src/game/stages/d_s_dsmt.cpp arg 20, reads the three files through SaveData::ReadFileData each time the page loads), and the
   next main page that settles switches it back. The solo files are therefore never shown on the online file select and the online ones never
   on the solo one. If a join succeeds the game starts on the online medium the ROM already reads; it stays until the title's main page is up again.

   THE JOIN. Picking an online file swallows the ROM's confirm, writes the launcher's request (hal/online_join.h, lane JOIN1's contract) and shows
   "Finding a server..." with a cancel. JOIN1 applies the plan the way the launcher's own start does: it sets the launcher's environment names and
   starts the game again at its front door, which boots straight into the chosen file with the hub installed (online_join_step answers kJoinEntering
   and this process quits), so the screen stays up until the window closes. A refusal or no answer is a plain sentence and Back. Without JOIN1's
   header in the tree (a build of this lane alone) a pick can only end in that sentence. */
#include "menu_online.h"
#include "lobby_page.h"
#include "pad_keyboard.h"
#include "pad_pointer.h"
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#if defined(__has_include)
#  if __has_include("online_join.h")
#    include "online_join.h"
#    define ONL_JOIN_REAL 1
#  endif
#endif
#ifndef ONL_JOIN_REAL
#  define ONL_JOIN_REAL 0
#endif

extern "C" char *data_ov007_02103360;   /* the title's element-cursor struct (src/func_ov007_020b3708.c): its first two shorts are the cursor element and its target */
namespace ntr { namespace backup { void select_online(int on); } }
namespace ntr { extern int active_w, active_h; }
extern "C" int hal_window_focused(void);           /* hal/sub_screen.cpp */
extern "C" int port_host_key_script(int vk);       /* tests/walk_window.cpp: SM64DS_HOST_KEY, 0 when unset */

extern "C" {
extern unsigned char data_0209caa0[];      /* the current file record; + 0x44 the minigame record, + 0x48 its unlock word */
unsigned char func_ov007_020cc600(int arg);   /* src/game/stages/d_s_dsmt.cpp: the title's client command dispatcher; 20 = read the three files again (allocates a fresh 0xcc byte block) */
int port_title_elem_boxes(int *slot, int *id, int *x0, int *y0, int *x1, int *y1, int max);
void func_ov007_020bdeb0(int idx);   /* the title scene's own sound call (overlay 7) */
}

namespace {

struct St {
    int on;                    /* the online planks are the target */
    unsigned tick;
    unsigned t_toggle;         /* tick of the last toggle (0 = never) */
    int scr;
    unsigned t_scr;
    int flow;                  /* the online file select is the page */
    int med_online;            /* ntr::backup currently reads the online save */
    int entering;              /* JOIN1 started the successor: this process is quitting */
    int pick_slot;
    int help_files;
    int padfocus, pf_cur, pf_kind;   /* PADNAV1: the pad focus on a host plank (ONL_PF_*), the ROM cursor it was set over, the page kind */
    char why[96];
    std::chrono::steady_clock::time_point wait0;
    int last_down;
    unsigned t_flow;           /* tick ADVENTURE ONLINE was pressed: the settled main page does not reset the flow for a while after it */
    int released_since;        /* the touch / key that opened a screen must be let go before it can press that screen's button */
    int touch_hold;            /* INPUTMATRIX1: a stylus / click that closed a screen is kept from the ROM until it is lifted */
    char rc[8];                /* LOBBYUI1: the room code typed into the ROOM CODE plank */
    unsigned char rc_prev[256];
    unsigned rc_closed_tick;   /* LOBBYUI1: the tick the plank let go: the Escape / Backspace that did it is not also the file select's back */
} g;

enum { kWaitCapMs = 22000 };

void medium(int online)
{
    if ((online != 0) == (g.med_online != 0)) return;
    g.med_online = online ? 1 : 0;
    ntr::backup::select_online(online ? 1 : -1);      /* off = what the environment says (a launcher started Adventure Online keeps its own save) */
    /* the title read the three files once, at the scene's start (the log of SM64DS_SAVE_TRACE shows the 0x0000 / 0x0080 / 0x0100 reads long before the
       main page): the ROM's own command 20 reads them again, from the medium that is selected now, and rebuilds the plates' data from them.
       CREDITS1: the shared minigame record first (command 21, ReadMinigameData into data_0209caa0 + 0x44), from the same medium.
       SaveData::ReadFileData ANDs every file's rabbit key word with that record's unlock word (src/engine/save/SaveData.cpp:103,
       `*(buf + 0xc) &= *(data_0209caa0 + 0x48)`), so with the other medium's record still in memory an online file's keys were cut
       down to the keys the single player save had (and the solo file's to the online save's on the way back), and the first save
       wrote the cut keys and that record to the medium for good. The title's own boot reads the record before the files. */
    func_ov007_020cc600(21);
    func_ov007_020cc600(20);
    {
        unsigned w = 0, k = 0;
        memcpy(&w, data_0209caa0 + 0x48, 4);
        memcpy(&k, data_0209caa0 + 0x0c, 4);   /* command 20 leaves the last file read (file C) in the record */
        fprintf(stderr, "[online] save medium -> %s: the minigame record, then the three files, read from it (unlock word %08x, "
                "file C rabbit keys %08x)\n", online ? "the online save" : "the single player save", w, k);
    }
}

int elem_box(int id, int *x0, int *y0, int *x1, int *y1)
{
    int slot[24], ids[24], bx0[24], by0[24], bx1[24], by1[24];
    const int n = port_title_elem_boxes(slot, ids, bx0, by0, bx1, by1, 24);
    for (int k = 0; k < n; ++k)
        if (ids[k] == id) { *x0 = bx0[k]; *y0 = by0[k]; *x1 = bx1[k]; *y1 = by1[k]; return 1; }
    return 0;
}

void set_screen(int s)
{
    g.scr = s;
    g.t_scr = g.tick;
    fprintf(stderr, "[online] screen %d\n", s);
    g.released_since = 0;
}

void say_fail(const char *why)
{
    snprintf(g.why, sizeof g.why, "%s", why && *why ? why : "No server answered. Keep the launcher open and try again.");
    set_screen(ONL_SCR_FAIL);
}

void join_start(int slot)
{
    g.pick_slot = slot;
    g.entering = 0;
    g.wait0 = std::chrono::steady_clock::now();
    set_screen(ONL_SCR_WAIT);
#if ONL_JOIN_REAL
    if (!port::online_join_begin(slot, profile_saved_name())) say_fail(port::online_join_sentence());
#endif
}

void join_cancel()
{
#if ONL_JOIN_REAL
    if (!g.entering) port::online_join_cancel();
#endif
}

void open_profile(int pending)
{
    profile_open(pending);
    set_screen(ONL_SCR_PROFILE);
}

/* LOBBYUI1: THE ROOM CODE PLANK AND HOST. The keyboard types the code straight into the plank (polled once a tick like the NAME popup's field: the window
   must have focus, or a scripted SM64DS_HOST_KEY press stands in): the 31 letters and digits of the lobby server's code alphabet, upper case, six at
   most; Backspace deletes; Enter with six letters asks for the lobby page joining that room; Escape lets go. While it has the keyboard it is a modal
   screen (ONL_SCR_ROOMCODE): the pad and the keys never reach the ROM's file select underneath. HOST opens a coop lobby (PORTMENU1: no Coop / VS choice any more; the title's VS plank opens a VS lobby). */
SHORT (WINAPI *p_GetAsyncKeyState)(int);
int key_down(int vk)
{
    if (port_host_key_script(vk)) return 1;
    static int done;
    if (!done) {
        done = 1;
        if (HMODULE u = LoadLibraryA("user32.dll")) p_GetAsyncKeyState = (SHORT (WINAPI *)(int))GetProcAddress(u, "GetAsyncKeyState");
    }
    return p_GetAsyncKeyState && hal_window_focused() && (p_GetAsyncKeyState(vk) & 0x8000) ? 1 : 0;
}
const char kRcAlphabet[] = "ABCDEFGHJKMNPQRSTUVWXYZ23456789";
void rc_sync_keys()
{
    for (int vk = 8; vk < 256; ++vk) g.rc_prev[vk] = (unsigned char)key_down(vk);
}
/* LOBBYKEYS1: THE LETTER GRID (hal/pad_keyboard.h) over the plank. The pad's A on the focused plank opens the plank WITH the grid (the keyboard and a click
   open it without, as before). The DS words say pad and keyboard alike (W A S D, Space and Enter share bits with the pad), so while a typing key of the
   keyboard is held the words are the keyboard's and the grid leaves them alone. */
const int kRcOwner = 2;
int typing_key_held()
{
    for (int vk = '0'; vk <= 'Z'; ++vk) if ((vk <= '9' || vk >= 'A') && key_down(vk)) return 1;
    for (int vk = VK_NUMPAD0; vk <= VK_DIVIDE; ++vk) if (key_down(vk)) return 1;
    static const int kOther[] = { VK_SPACE, VK_BACK, VK_RETURN, VK_ESCAPE, VK_OEM_1, VK_OEM_2, VK_OEM_3, VK_OEM_4, VK_OEM_5, VK_OEM_6, VK_OEM_7,
                                  VK_OEM_PLUS, VK_OEM_COMMA, VK_OEM_MINUS, VK_OEM_PERIOD };
    for (int vk : kOther) if (key_down(vk)) return 1;
    return 0;
}
void rc_open(int grid = 0)
{
    rc_sync_keys();          /* the press that gave the plank the keyboard (Enter, A) is not typing */
    set_screen(ONL_SCR_ROOMCODE);
    fprintf(stderr, "[online] ROOM CODE plank: typing (%s)%s\n", g.rc, grid ? ", the letter grid is up (pad A)" : "");
    if (grid) pkb_open(kRcOwner, PKB_CODE, PKB_OK_JOIN, 6);
    else if (pkb_owner() == kRcOwner) pkb_close();
}
void rc_close(const char *why)
{
    fprintf(stderr, "[online] ROOM CODE plank: let go (%s), code '%s'\n", why, g.rc);
    if (pkb_owner() == kRcOwner) pkb_close();
    set_screen(ONL_SCR_NONE);
    g.rc_closed_tick = g.tick ? g.tick : 1;
    g.touch_hold = 1;
}
void rc_submit()
{
    if (strlen(g.rc) != 6) { fprintf(stderr, "[online] ROOM CODE: '%s' is not six letters yet\n", g.rc); return; }
    fprintf(stderr, "[online] ROOM CODE %s: the lobby page joins it\n", g.rc);
    if (pkb_owner() == kRcOwner) pkb_close();
    set_screen(ONL_SCR_NONE);
    lobby_page_request_join(g.rc);
}
void rc_tick()
{
    auto fresh = [](int vk) { const int d = key_down(vk); const int f = d && !g.rc_prev[vk]; g.rc_prev[vk] = (unsigned char)d; return f; };
    const int ctl = key_down(VK_CONTROL) || key_down(VK_MENU);
    for (int vk = '0'; vk <= 'Z'; ++vk) {
        if (vk > '9' && vk < 'A') continue;
        if (!fresh(vk) || ctl) continue;
        const char c = (char)vk;
        const size_t n = strlen(g.rc);
        if (strchr(kRcAlphabet, c) && n < 6) { g.rc[n] = c; g.rc[n + 1] = 0; fprintf(stderr, "[online] ROOM CODE typed: %s\n", g.rc); }
    }
    for (int vk = VK_NUMPAD2; vk <= VK_NUMPAD9; ++vk) {
        if (!fresh(vk) || ctl) continue;
        const size_t n = strlen(g.rc);
        if (n < 6) { g.rc[n] = (char)('0' + vk - VK_NUMPAD0); g.rc[n + 1] = 0; }
    }
    if (fresh(VK_RETURN)) rc_submit();
    else if (fresh(VK_ESCAPE)) rc_close("Escape");
}
/* LOBBYKEYS1: what the grid asked for */
void rc_grid_event(int ev, char ch)
{
    const size_t n = strlen(g.rc);
    switch (ev) {
    case PKB_TYPE:
        if (n < 6 && ch && strchr(kRcAlphabet, ch)) { g.rc[n] = ch; g.rc[n + 1] = 0; fprintf(stderr, "[online] ROOM CODE typed (grid): %s\n", g.rc); }
        break;
    case PKB_DELETE:
        if (n) { g.rc[n - 1] = 0; fprintf(stderr, "[online] ROOM CODE deleted (grid): '%s'\n", g.rc); }
        break;
    case PKB_SUBMIT: rc_submit(); break;      /* six letters join, as Enter does; fewer keep the grid up */
    case PKB_CLOSE: rc_close("the grid"); break;
    default: break;
    }
}

/* PORTMENU1: the lobby page closed since this was last asked (a click on its Back, a key, a pad B): whatever press that was is still down and must not
   land as a fresh press on the file select / title under it (it used to open the ? text, the Back button sits where the ? plank is). The press is
   held until it is lifted, and a pad focus the page left behind is dropped. */
void lobby_sync()
{
    static unsigned seen;
    const unsigned n = lobby_page_closed_count();
    if (n == seen) return;
    seen = n;
    g.touch_hold = 1;
    g.padfocus = 0;
    fprintf(stderr, "[online] the lobby page closed: the press that closed it is held back\n");
}

void host_press(const char *how)
{
    fprintf(stderr, "[online] HOST (%s): a coop room\n", how);
    lobby_page_request_host();
}

}  // namespace

const OnlFacts *onl_facts_of(const MenuLayout *L)
{
    const OnlFacts *f = (const OnlFacts *)(L->facts.blob + ONL_FACTS_OFF);
    return f->magic == ONL_MAGIC ? f : 0;
}

int menu_online_enabled(void) { return hal_menu_enabled(MENU_KEY_ONE_SCREEN_MENUS); }
int menu_online_on(void) { return g.on; }
int menu_online_modal(void) { return g.scr != ONL_SCR_NONE; }

void menu_online_facts(OnlFacts *f)
{
    memset(f, 0, sizeof *f);
    f->magic = ONL_MAGIC;
    f->on = g.on;
    f->age = g.t_toggle ? (int)(g.tick - g.t_toggle) : 999;
    if (f->age > 999) f->age = 999;
    f->scr = g.scr;
    f->scr_age = (int)(g.tick - g.t_scr);
    f->flow = g.flow;
    f->help_files = g.help_files;
    f->padfocus = g.scr == ONL_SCR_NONE ? g.padfocus : 0;
    memcpy(f->why, g.why, sizeof f->why);
    f->why[sizeof f->why - 1] = 0;
    memcpy(f->rc, g.rc, sizeof f->rc - 1);
    f->rc[sizeof f->rc - 1] = 0;
    f->rc_caret = (unsigned char)(g.scr == ONL_SCR_ROOMCODE && ((g.tick / 20) & 1) == 0);
    f->rc_grid = (unsigned char)(g.scr == ONL_SCR_ROOMCODE && pkb_owner() == kRcOwner);
    f->rc_key = (unsigned char)(f->rc_grid ? pkb_key() : 0);
    profile_facts(&f->prof);
}

void menu_online_scene_left(void)
{
    g.on = 0;
    g.t_toggle = 0;
    g.help_files = 0;
    if (g.scr == ONL_SCR_WAIT) join_cancel();
    if (g.scr == ONL_SCR_PROFILE) profile_back();
    g.scr = ONL_SCR_NONE;
    /* the medium and g.flow stay: a started online game reads the online save; the next settled main page switches back */
}

void menu_online_tick(int top, int elem, int req)
{
    ++g.tick;
    if (g.scr == ONL_SCR_PROFILE) profile_tick();
    if (g.scr == ONL_SCR_ROOMCODE) rc_tick();
    if (g.scr != ONL_SCR_ROOMCODE && pkb_owner() == kRcOwner) pkb_close();      /* LOBBYKEYS1: the plank let go some other way (the scene left) */
    if (g.scr != ONL_SCR_NONE && g.tick - g.t_scr > 8) g.released_since = 1;
    if (top == 0 && elem == 0 && req == -1) {            /* the main page is settled */
        if ((g.flow || g.med_online) && g.tick - g.t_flow > 40) {
            g.flow = 0;
            medium(0);
            if (g.scr == ONL_SCR_WAIT) { join_cancel(); g.scr = ONL_SCR_NONE; }
        }
    }
    if (g.scr == ONL_SCR_WAIT && !g.entering) {
        int r = 0;
        const char *why = "";
#if ONL_JOIN_REAL
        const port::OnlineJoinState st = port::online_join_step();
        if (st == port::kJoinEntering) r = 1;
        else if (st == port::kJoinFailed) { r = -1; why = port::online_join_sentence(); }
#endif
        const long ms = (long)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - g.wait0).count();
        if (r == 0 && ms > kWaitCapMs) { join_cancel(); r = -1; why = ""; }
        if (r > 0) {
            g.entering = 1;      /* the plan is applied and the game starts again: the wait screen stays up until this window closes */
            fprintf(stderr, "[online] plan applied for file %d, the game starts again\n", g.pick_slot);
        } else if (r < 0) {
            fprintf(stderr, "[online] join failed: %s\n", why[0] ? why : "(no answer)");
            say_fail(why);
        }
    }
}

/* PROFILE1: the popup finished (OK saved the name and colour, or BACK). The screen closes; an OK with a file waiting starts the join. */
void menu_online_profile_done(int ok, int pending_slot)
{
    if (g.scr == ONL_SCR_PROFILE) { fprintf(stderr, "[online] profile popup closed (%s)\n", ok ? "ok" : "back"); set_screen(ONL_SCR_NONE); }
    g.touch_hold = 1;      /* the press that closed it is kept from the ROM underneath until it is lifted */
    if (ok && pending_slot >= 0) join_start(pending_slot);
}

static void toggle_on()
{
    g.on = !g.on;
    g.t_toggle = g.tick ? g.tick : 1;
    fprintf(stderr, "[online] planks -> %s\n", g.on ? "online" : "solo");
}

static void close_screen()
{
    if (g.scr == ONL_SCR_ROOMCODE) { rc_close("back"); return; }
    if (g.scr == ONL_SCR_WAIT) join_cancel();
    fprintf(stderr, "[online] screen %d closed\n", g.scr);
    set_screen(ONL_SCR_NONE);
}

/* a press of the modal screen's button */
static void modal_button()
{
    if (g.scr == ONL_SCR_NONE || !g.released_since) return;
    fprintf(stderr, "[online] screen %d closed by its button\n", g.scr);
    close_screen();
}

/* PADNAV1: a confirm while the pad focus is on a host plank (the ? or NAME): it opens that plank's screen; the ROM never sees it */
static int pad_focus_press(int kind, int edge)
{
    if (!g.padfocus || g.scr != ONL_SCR_NONE) return 0;
    if (edge) {
        if (hal_menu_trace_on()) fprintf(stderr, "[padnav] confirm on the focused plank %d\n", g.padfocus);
        if (g.padfocus == ONL_PF_HELP) { g.help_files = kind == 2; set_screen(ONL_SCR_HELP); }
        else if (g.padfocus == ONL_PF_NAME && kind == 2) { fprintf(stderr, "[online] NAME plank: the name popup (pad)\n"); open_profile(-1); }
        else if (g.padfocus == ONL_PF_ROOMCODE && kind == 2) rc_open(hal_padptr_pad_confirm() && !typing_key_held());   /* LOBBYKEYS1: the controller's own A brings the letter grid (a key or mouse Jump does not) */
        else if (g.padfocus == ONL_PF_HOST && kind == 2) host_press("pad");
    }
    return 1;
}

int menu_online_press_main(int elem, int edge)
{
    lobby_sync();
    if (lobby_page_open()) return 1;      /* LOBBYUI1: the lobby page is up over the title: no press reaches the menu under it */
    if (!menu_online_enabled()) return elem == 1 ? 1 : 0;       /* the classic title: the MP / VS plate takes no press, as before */
    if (pad_focus_press(1, edge)) return 1;
    if (g.scr != ONL_SCR_NONE) {
        if (edge && g.scr != ONL_SCR_PROFILE) { if (g.released_since) modal_button(); }
        return 1;
    }
    if (elem == 1) { if (edge) toggle_on(); return 1; }
    if (elem == 2 && g.on) {
        if (edge) { fprintf(stderr, "[online] VS plank: a VS room\n"); lobby_page_request_vs(); }      /* PORTMENU1: was the "made in the launcher" note */
        return 1;
    }
    if (elem == 0 && g.on) {
        if (edge) {
            g.flow = 1;
            g.t_flow = g.tick;
            medium(1);
            fprintf(stderr, "[online] ADVENTURE ONLINE: the file select reads the online save\n");
        }
        return 0;
    }
    return 0;
}

int menu_online_press_file(int elem, int edge)
{
    lobby_sync();
    if (lobby_page_open()) return 1;      /* LOBBYUI1: as above (a click on the page landed on file A under it and opened the name popup) */
    if (!g.flow) return 0;
    if (g.scr == ONL_SCR_ROOMCODE) {
        /* LOBBYUI1: a confirm bit while the plank has the keyboard. The typing's own keys (letters, Enter, Space, Escape) are read by rc_tick, so only a
           press with none of them down is the pad's: six letters join, fewer let the plank go (a pad cannot type) */
        if (edge) {
            int typing = key_down(VK_RETURN) || key_down(VK_SPACE) || key_down(VK_ESCAPE);
            for (int vk = '0'; vk <= 'Z' && !typing; ++vk) typing = key_down(vk);
            if (!typing) { if (strlen(g.rc) == 6) rc_submit(); else rc_close("a pad press"); }
        }
        return 1;
    }
    if (pad_focus_press(2, edge)) return 1;
    if (edge) fprintf(stderr, "[online] confirm on the online file select, ROM cursor element %d\n", elem);
    if (g.scr != ONL_SCR_NONE) {
        if (edge && g.scr != ONL_SCR_PROFILE) { if (g.released_since) modal_button(); }
        return 1;
    }
    if (elem == 255 || elem < 0) {                       /* the ROM's cursor is not shown yet: the first confirm only shows it, on file A */
        if (edge && data_ov007_02103360) {
            short *p0 = *(short **)data_ov007_02103360;
            if (p0) p0[1] = 14;
        }
        return 1;
    }
    if (elem >= 14 && elem <= 16) {
        if (edge) {
            if (profile_name_needed()) {          /* PROFILE1: the first time, the name and colour popup comes before the join */
                fprintf(stderr, "[online] file %d picked: no name saved yet, the name popup first\n", elem - 14);
                open_profile(elem - 14);
            } else {
                fprintf(stderr, "[online] file %d picked: asking the launcher for a server\n", elem - 14);
                join_start(elem - 14);
            }
        }
        return 1;
    }
    return 0;
}

static int in_box(int sx, int sy, int x, int y, int w, int h) { return sx >= x && sx < x + w && sy >= y && sy < y + h; }

int menu_online_touch(int kind, int down, int sx, int sy)
{
    lobby_sync();
    if (lobby_page_open()) { g.last_down = down; return 1; }      /* LOBBYUI1 */
    const int edge = down && !g.last_down;
    g.last_down = down;
    if (edge) g.padfocus = 0;       /* PADNAV1: a mouse press is not the pad focus */
    if (!down) {
        g.released_since = 1;
        g.touch_hold = 0;
        return 0;
    }
    if (!menu_online_enabled()) return 0;
    if (g.scr == ONL_SCR_PROFILE) {         /* PROFILE1: the popup takes every stylus state; the press that closes it is held back like the other screens' */
        if (edge && g.released_since) profile_touch(sx, sy, 1);
        return 1;
    }
    if (g.scr == ONL_SCR_ROOMCODE) {        /* LOBBYUI1: a click on the plank keeps the typing; a click anywhere else lets it go */
        if (pkb_owner() == kRcOwner && pkb_in_panel((float)sx, (float)sy)) {      /* LOBBYKEYS1: a click on the letter grid works its key */
            if (edge) { char ch = 0; const int ev = pkb_click((float)sx, (float)sy, (int)strlen(g.rc), &ch); rc_grid_event(ev, ch); }
            return 1;
        }
        if (edge && !in_box(sx, sy, ONL_RC_X, ONL_RC_Y, ONL_RC_W, ONL_RC_H)) rc_close("a click elsewhere");
        return 1;
    }
    if (g.scr != ONL_SCR_NONE) {
        /* the press that closes the screen keeps being swallowed until the finger is up: the screen is gone on the next frame and the ROM's
           title underneath (the face, the planks) would otherwise see the rest of that same press as a fresh touch */
        if (edge && g.released_since && in_box(sx, sy, ONL_BTN_X, ONL_BTN_Y, ONL_BTN_W, ONL_BTN_H)) { modal_button(); g.touch_hold = 1; }
        return 1;
    }
    if (g.touch_hold) return 1;
    if (kind == 1) {
        if (g.on && in_box(sx, sy, ONL_HELP_X, ONL_HELP_Y, ONL_HELP_W, ONL_HELP_H)) {
            if (edge) { g.help_files = 0; set_screen(ONL_SCR_HELP); }
            return 1;
        }
        int x0, y0, x1, y1;
        for (int id = 0; id < 3; ++id) {
            if (!elem_box(id, &x0, &y0, &x1, &y1) || !in_box(sx, sy, x0, y0, x1 - x0, y1 - y0)) continue;
            return menu_online_press_main(id == 0 ? 0 : id, edge);        /* ids 0, 1, 2 */
        }
        return 0;
    }
    if (kind == 2 && g.flow) {
        if (in_box(sx, sy, ONL_HELP_X, ONL_HELP_Y, ONL_HELP_W, ONL_HELP_H)) {
            if (edge) { g.help_files = 1; set_screen(ONL_SCR_HELP); }
            return 1;
        }
        if (in_box(sx, sy, ONL_RC_X, ONL_RC_Y, ONL_RC_W, ONL_RC_H)) {                   /* LOBBYUI1: the ROOM CODE plank */
            if (edge) rc_open();
            return 1;
        }
        if (in_box(sx, sy, ONL_HOST_X, ONL_HOST_Y, ONL_HOST_W, ONL_HOST_H)) {           /* LOBBYUI1: HOST */
            if (edge) host_press("click");
            return 1;
        }
        if (in_box(sx, sy, ONL_NAME_X, ONL_NAME_Y, ONL_NAME_W, ONL_NAME_H)) {           /* PROFILE1: the NAME plank */
            if (edge) { fprintf(stderr, "[online] NAME plank: the name popup\n"); open_profile(-1); }
            return 1;
        }
        int x0, y0, x1, y1;
        for (int id = 14; id <= 16; ++id) {
            if (!elem_box(id, &x0, &y0, &x1, &y1) || !in_box(sx, sy, x0, y0, x1 - x0, y1 - y0)) continue;
            return menu_online_press_file(id, edge);
        }
    }
    return 0;
}

void menu_online_pad_frame(int kind, int cursor)
{
    if (kind != g.pf_kind) { g.pf_kind = kind; g.padfocus = 0; }
    if (g.padfocus && cursor != g.pf_cur) g.padfocus = 0;     /* the mouse moved the ROM cursor */
}

/* DPADSND1: A STEP THE HOST TAKES SOUNDS LIKE THE ROM'S. The pad graph below takes the D-pad away from the ROM on the file select (and for the ? on the
   online title), so the ROM's own step never runs and neither does its sound. The cartridge's sounds for these keys: the first key with the cursor
   hidden shows it on file A with func_ov007_020bdeb0(3) (src/unnamed/ov007/func_ov007_020b3190.c, state 5), and a step that changes the cursor's
   target plays func_ov007_020bdeb0(4), or 5 while the struct's +8 word is 2 (src/unnamed/ov007/func_ov007_020b3708.c); a step against an edge
   changes nothing and plays nothing. The mouse hover already plays the same move sound (hal/menu_recipe_title.cpp title_cursor_snd). */
static void step_sound(int idx)
{
    if (hal_menu_trace_on()) fprintf(stderr, "[padnav] step sound func_ov007_020bdeb0(%d)\n", idx);
    func_ov007_020bdeb0(idx);
}
static int move_sound_idx(void) { return data_ov007_02103360 && *(int *)(data_ov007_02103360 + 8) == 2 ? 5 : 4; }

static void rom_cursor_to(int id)
{
    if (!data_ov007_02103360) return;
    short *p0 = *(short **)data_ov007_02103360;
    if (p0) p0[1] = (short)id;
}

/* PADNAV1: THE PAD'S D-PAD GRAPH of the online menu's pages, by element, never by pixel distance (the planks sit at the window edges on wide windows).
   dirs are DS bits 0x10 right, 0x20 left, 0x40 up, 0x80 down. 1 = the step is the host's (the ROM never sees it).
   File select (solo and online), the ROM's elements 14 / 15 / 16 = files A / B / C, 21 = FILE OPTIONS, 20 = the back arrow; online also the NAME and ? planks:
       A: right B, down FILE OPTIONS, up NAME        NAME: right ?, down B
       LOBBYUI1 (online only): A up = ROOM CODE, ROOM CODE down = A; a wide window: HOST left of FILE OPTIONS (A left / FILE OPTIONS left = HOST, HOST up =
       A, right = FILE OPTIONS), ROOM CODE right = NAME, NAME left = ROOM CODE; 4:3: the top row is ROOM CODE, HOST, NAME, ?
       B: left A, right C, down FILE OPTIONS, up NAME   ?: left NAME, down C
       C: left B, down back arrow, up ?
       FILE OPTIONS: up A, right back arrow      back arrow: up C, left FILE OPTIONS
   Online title: the ? stands right of the wrench (flush at the window's right edge, the wrench one ? width in from it), so from the wrench RIGHT goes to the ?
   and from the ? LEFT comes back to the wrench, DOWN to the right plank (RIGHT and UP stay); every other step on the planks and the wrench (the wrench's LEFT
   included) is the ROM's own.
   Nothing opens a screen by a step: A on the focused ? / NAME does (pad_focus_press). The first step with the ROM's cursor hidden shows it on file A. */
int menu_online_dir(int kind, int cursor, unsigned dirs)
{
    if (lobby_page_open()) return dirs ? 1 : 0;      /* LOBBYUI1 */
    if (!menu_online_enabled() || g.scr != ONL_SCR_NONE || !dirs) return 0;
    const int d = (dirs & 0x10) ? 0x10 : (dirs & 0x20) ? 0x20 : (dirs & 0x40) ? 0x40 : (dirs & 0x80) ? 0x80 : 0;
    if (!d) return 0;
    if (kind == 1) {
        if (!g.on) return 0;
        if (g.padfocus == ONL_PF_HELP) {
            if (hal_menu_trace_on()) fprintf(stderr, "[padnav] title: focus 100 (the ?), step 0x%x -> %d\n", d, d == 0x20 ? 7 : d == 0x80 ? 2 : 100);
            if (d == 0x20) { g.padfocus = 0; rom_cursor_to(7); step_sound(move_sound_idx()); }
            else if (d == 0x80) { g.padfocus = 0; rom_cursor_to(2); step_sound(move_sound_idx()); }
            return 1;
        }
        if (cursor == 7 && d == 0x10) { g.padfocus = ONL_PF_HELP; g.pf_cur = cursor; if (hal_menu_trace_on()) fprintf(stderr, "[padnav] title: focus 7 (wrench), step 0x10 -> 100 (the ?)\n"); step_sound(move_sound_idx()); return 1; }
        return 0;
    }
    if (kind != 2) return 0;
    int node = g.padfocus ? g.padfocus : cursor;
    const int wide = menu_online_wide_row();
    if (!g.padfocus && (cursor == 255 || cursor < 0)) { rom_cursor_to(14); step_sound(3); return 1; }
    if (!g.padfocus && cursor != 14 && cursor != 15 && cursor != 16 && cursor != 20 && cursor != 21) return 0;
    int next = -1;
    switch (node) {          /* ROM elements: 14 / 15 / 16 files, 21 FILE OPTIONS, 20 the back arrow (the order of the page's hotspots, hal/menu_recipe_title.cpp fids) */
    case 14: next = d == 0x10 ? 15 : d == 0x80 ? 21 : d == 0x40 ? (g.flow ? ONL_PF_ROOMCODE : -1) : d == 0x20 ? (g.flow && wide ? ONL_PF_HOST : -1) : -1; break;
    case 15: next = d == 0x20 ? 14 : d == 0x10 ? 16 : d == 0x80 ? 21 : d == 0x40 ? (g.flow ? ONL_PF_NAME : -1) : -1; break;
    case 16: next = d == 0x20 ? 15 : d == 0x80 ? 20 : d == 0x40 ? (g.flow ? ONL_PF_HELP : -1) : -1; break;
    case 21: next = d == 0x40 ? 14 : d == 0x10 ? 20 : d == 0x20 ? (g.flow && wide ? ONL_PF_HOST : -1) : -1; break;
    case 20: next = d == 0x40 ? 16 : d == 0x20 ? 21 : -1; break;
    case ONL_PF_NAME: next = d == 0x10 ? ONL_PF_HELP : d == 0x80 ? 15 : d == 0x20 ? (wide ? ONL_PF_ROOMCODE : ONL_PF_HOST) : -1; break;
    case ONL_PF_ROOMCODE: next = d == 0x10 ? (wide ? ONL_PF_NAME : ONL_PF_HOST) : d == 0x80 ? 14 : -1; break;
    case ONL_PF_HOST: next = wide ? (d == 0x10 ? 21 : d == 0x40 ? 14 : -1) : (d == 0x20 ? ONL_PF_ROOMCODE : d == 0x10 ? ONL_PF_NAME : d == 0x80 ? 14 : -1); break;
    case ONL_PF_HELP: next = d == 0x20 ? ONL_PF_NAME : d == 0x80 ? 16 : -1; break;
    default: break;
    }
    if (hal_menu_trace_on()) fprintf(stderr, "[padnav] file select: focus %d, step 0x%x -> %d%s\n", node, d, next >= 0 ? next : node, next >= 0 ? "" : " (edge, stays)");
    if (next >= 100) { g.padfocus = next; g.pf_cur = cursor; }
    else if (next >= 0) { g.padfocus = 0; rom_cursor_to(next); }
    if (next >= 0 && next != node) step_sound(move_sound_idx());
    return 1;
}

int menu_online_help_pad(int kind)
{
    if (lobby_page_open()) return 1;      /* LOBBYUI1 */
    if (!menu_online_enabled() || g.scr != ONL_SCR_NONE) return 0;
    if (kind == 1 && g.on) { g.help_files = 0; set_screen(ONL_SCR_HELP); return 1; }
    if (kind == 2 && g.flow) { g.help_files = 1; set_screen(ONL_SCR_HELP); return 1; }
    return 0;
}

int menu_online_padfocus(void) { return menu_online_enabled() && g.scr == ONL_SCR_NONE ? g.padfocus : 0; }

void menu_online_hover_focus(int kind, int pf, int cursor)
{
    if (lobby_page_open()) return;      /* LOBBYUI1 */
    if (!menu_online_enabled() || g.scr != ONL_SCR_NONE) return;
    if (pf == ONL_PF_HELP && !((kind == 1 && g.on) || (kind == 2 && g.flow))) return;      /* the ? is up on the online title and the online file select only */
    if (pf == ONL_PF_NAME && !(kind == 2 && g.flow)) return;                                /* NAME only on the online file select */
    if ((pf == ONL_PF_ROOMCODE || pf == ONL_PF_HOST) && !(kind == 2 && g.flow)) return;      /* LOBBYUI1: the online file select only */
    if (pf != ONL_PF_HELP && pf != ONL_PF_NAME && pf != ONL_PF_ROOMCODE && pf != ONL_PF_HOST) pf = 0;
    if (pf != g.padfocus && hal_menu_trace_on()) fprintf(stderr, "[padnav] hover: focus %d -> %d (page %d, cursor %d)\n", g.padfocus, pf, kind, cursor);
    g.padfocus = pf; g.pf_cur = cursor; g.pf_kind = kind;
}

int menu_online_modal_up(void) { return menu_online_enabled() && g.scr != ONL_SCR_NONE; }

int menu_online_rc_pad(unsigned pressed, unsigned held)
{
    if (g.scr != ONL_SCR_ROOMCODE || pkb_owner() != kRcOwner) return 0;
    PkbPad gp;
    memset(&gp, 0, sizeof gp);
    if (!typing_key_held()) {         /* DS words: 0x40 up, 0x80 down, 0x20 left, 0x10 right; 0x0002 A, 0x0001 B (the port's punch), 0x0008 Start */
        gp.dirs = (held & 0x40 ? 1u : 0) | (held & 0x80 ? 2u : 0) | (held & 0x20 ? 4u : 0) | (held & 0x10 ? 8u : 0);
        gp.a = (pressed & 0x0002) != 0;
        gp.b = (pressed & 0x0001) != 0;
        gp.start = (pressed & 0x0008) != 0;
    }
    char ch = 0;
    const int ev = pkb_pad(&gp, (int)strlen(g.rc), &ch);      /* the event first: ch is written by the call */
    rc_grid_event(ev, ch);
    return 1;
}

int menu_online_rc_backspace(void)
{
    if (g.scr != ONL_SCR_ROOMCODE) return g.rc_closed_tick && g.tick - g.rc_closed_tick < 12 ? 1 : 0;
    if (key_down(VK_ESCAPE)) { rc_close("Escape"); return 1; }
    const size_t n = strlen(g.rc);
    if (n) { g.rc[n - 1] = 0; fprintf(stderr, "[online] ROOM CODE backspace: '%s'\n", g.rc); }
    else rc_close("Backspace on an empty code");
    return 1;
}

int menu_online_wide_row(void) { return ntr::active_w * 3 > ntr::active_h * 4 + 8; }
int menu_online_flow_now(void) { return g.flow && menu_online_enabled(); }

int menu_online_back(int kind)
{
    lobby_sync();
    if (lobby_page_open()) return 1;      /* LOBBYUI1 */
    if (!menu_online_enabled()) return 0;
    if (g.scr == ONL_SCR_PROFILE) { profile_back(); return 1; }
    if (g.scr != ONL_SCR_NONE) { close_screen(); return 1; }
    if (kind == 1 && g.on) { toggle_on(); return 1; }
    return 0;
}
