/* hal/lobby_page.cpp: the in-game lobby page. See lobby_page.h. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "lobby_page.h"
#include "mods.h"
#include "settings_page.h"
#include "ui_text.h"
#include "../third_party/stb/stb_image.h"

/* the build defines LP_CLIENT_REAL=1 when the tree carries hal/lobby_client.h (port/CMakeLists.txt; MSVC's preprocessor here has no
   __has_include to ask) */
#if defined(LP_CLIENT_REAL) && LP_CLIENT_REAL
#  include "lobby_client.h"
#  include "handoff_poll.h"
#else
#  undef LP_CLIENT_REAL
#  define LP_CLIENT_REAL 0
/* the contract of hal/lobby_client.h (lane LOBBYNET1), word for word: a tree without that file builds the page on the stand-in below */
enum LobbyKind  { LOBBY_COOP = 0, LOBBY_VS = 1 };
enum LobbyPhase { LOBBY_NONE, LOBBY_BUSY, LOBBY_IN_ROOM, LOBBY_STARTING, LOBBY_FAILED };
struct LobbySeat { char name[32]; unsigned body_rgb, shoe_rgb; int ready, is_host, is_me, muted; int member; };
struct LobbySetting { const char *key; const char *label; int value, min, max; const char *const *choices; int n_choices; };
struct LobbyState {
    LobbyPhase phase; LobbyKind kind; int am_host;
    char code[8]; char server_name[48]; char error[96];
    int n_seats, max_seats; LobbySeat seats[16];
    int n_settings; LobbySetting settings[16];
    int coop_save_continue;
    int n_chat; char chat[32][96];
    char error_head[40];
};
#endif

/* the stand-in client and the proof knobs exist in dev builds only; the kit has the real client and nothing else */
#if defined(PORT_ROM_CLEAN)
#  define LP_PROOF 0
#else
#  define LP_PROOF 1
#endif

extern "C" int hal_window_focused(void);            /* hal/sub_screen.cpp */
extern "C" int port_host_key_script(int vk);        /* tests/walk_window.cpp: SM64DS_HOST_KEY, 0 when unset */
extern "C" void host_menu_cursor_sound(void);       /* hal/menu_compose.h: the ROM menu cursor sound, once a frame */

namespace {

/* ================================================================ the stand-in client =========================================== */
/* Answers like a room on the lobby server: create / join are BUSY for a short while, then IN_ROOM; a code that is not six letters of
   the room alphabet fails; ready, settings, chat, kick and mute change the room at once. SM64DS_LOBBY_FAKE=<scenario> (dev builds) seeds
   a room with friends in it so every state of the page has a picture without a server. */
const char *const kCoopStarExit[2] = { "Only the collector", "Everyone" };
const char *const kOffOn[2] = { "Off", "On" };
const char *const kVsMode[3] = { "Clock", "First to stars", "Luigi Infection" };

struct Fake {
    LobbyState s;
    int busy_left;         /* frames until a create / join answers */
    int fail_join;         /* the join answers FAILED */
    int start_left;
    int guest_in;          /* frames until a friend joins a hosted room */
} F;

void fk_settings(LobbyKind k)
{
    LobbyState &s = F.s;
    s.n_settings = 0;
    auto add = [&](const char *key, const char *label, int v, int mn, int mx, const char *const *ch, int nch) {
        LobbySetting &t = s.settings[s.n_settings++];
        t.key = key; t.label = label; t.value = v; t.min = mn; t.max = mx; t.choices = ch; t.n_choices = nch;
    };
    if (k == LOBBY_COOP) {
        add("bump", "Bump into players", 1, 0, 1, kOffOn, 2);
        add("hits", "Player hits", 1, 0, 1, kOffOn, 2);
        add("hurt", "Hits hurt", 0, 0, 1, kOffOn, 2);
        add("teleport", "Teleport to friends", 1, 0, 1, kOffOn, 2);
        add("starexit", "Star exit", 0, 0, 1, kCoopStarExit, 2);
    } else {
        add("mode", "Game mode", 1, 0, 2, kVsMode, 3);
        add("stars", "Stars to win", 3, 1, 5, 0, 0);
        add("arena", "Arena", 1, 1, 4, 0, 0);
        add("players", "Players", 4, 2, 16, 0, 0);
        add("bump", "Bump into players", 1, 0, 1, kOffOn, 2);
        add("classic", "Classic netcode", 0, 0, 1, kOffOn, 2);
    }
}

void fk_seat(const char *name, unsigned body, unsigned shoe, int ready, int host, int me)
{
    LobbyState &s = F.s;
    if (s.n_seats >= 16) return;
    LobbySeat &t = s.seats[s.n_seats++];
    memset(&t, 0, sizeof t);
    snprintf(t.name, sizeof t.name, "%s", name);
    t.body_rgb = body; t.shoe_rgb = shoe; t.ready = ready; t.is_host = host; t.is_me = me;
    static int next_member;
    t.member = ++next_member;
}

void fk_chat(const char *line)
{
    LobbyState &s = F.s;
    if (s.n_chat >= 32) { memmove(s.chat[0], s.chat[1], sizeof s.chat[0] * 31); s.n_chat = 31; }
    snprintf(s.chat[s.n_chat++], sizeof s.chat[0], "%s", line);
}

void fk_room(LobbyKind k, int host)
{
    LobbyState &s = F.s;
    memset(&s, 0, sizeof s);
    s.phase = LOBBY_IN_ROOM; s.kind = k; s.am_host = host;
    snprintf(s.code, sizeof s.code, "K7M2QX");
    s.max_seats = k == LOBBY_COOP ? 4 : 8;
    fk_settings(k);
    if (host) fk_seat("Tango", 0x31A431, 0xE66218, 0, 1, 1);
    else fk_seat("Opie", 0xFF3139, 0x004AFF, 0, 1, 0);
}

void fk_scenario(const char *sc, LobbyKind k)
{
    memset(&F, 0, sizeof F);
    if (!strcmp(sc, "busy")) { F.s.phase = LOBBY_BUSY; F.s.kind = k; F.busy_left = 1 << 30; return; }
    if (!strcmp(sc, "failed")) {
        F.s.phase = LOBBY_FAILED; F.s.kind = k;
        snprintf(F.s.error, sizeof F.s.error, "No room has the code K7M2QX. Check the code and try again.");
        return;
    }
    const int host = strcmp(sc, "room_guest") != 0;
    fk_room(k, host);
    if (host) {
        fk_seat("Opie", 0xFF3139, 0x004AFF, 1, 0, 0);
        fk_seat("Thunder", 0x4A52B4, 0xB410B4, 0, 0, 0);
        if (k == LOBBY_VS) { fk_seat("Wario Fan", 0xFFCD00, 0x31BD08, 1, 0, 0); fk_seat("Luigi", 0x20D8E0, 0x0E5E7A, 0, 0, 0); }
    } else {
        fk_seat("Tango", 0x31A431, 0xE66218, 0, 0, 1);
        fk_seat("Thunder", 0x4A52B4, 0xB410B4, 1, 0, 0);
    }
    snprintf(F.s.server_name, sizeof F.s.server_name, "%s", k == LOBBY_COOP ? "Tango's castle run" : "Friday VS night");
    fk_chat("Opie: hi! ready when you are");
    fk_chat("Thunder: give me a sec, getting a drink");
    fk_chat("Opie: no rush");
    if (!strcmp(sc, "all_ready") || !strcmp(sc, "starting"))
        for (int i = 0; i < F.s.n_seats; ++i) F.s.seats[i].ready = 1;
    if (!strcmp(sc, "starting")) { F.s.phase = LOBBY_STARTING; F.start_left = 1 << 30; }
    if (!strcmp(sc, "room_hidden")) { /* the page hides the code itself (SM64DS_LOBBY_SHOW sets the eye) */ }
}

const LobbyState *fk_state(void) { return &F.s; }
void fk_tick(void)
{
    if (F.s.phase == LOBBY_BUSY && F.busy_left > 0 && --F.busy_left == 0) {
        if (F.fail_join) {
            const LobbyKind k = F.s.kind;
            memset(&F.s, 0, sizeof F.s);
            F.s.phase = LOBBY_FAILED; F.s.kind = k;
#if defined(PORT_ROM_CLEAN) && !LP_CLIENT_REAL
            /* a shipped build without the lobby client never shows the stand-in's rooms */
            snprintf(F.s.error, sizeof F.s.error, "Online rooms are not in this build.");
#else
            snprintf(F.s.error, sizeof F.s.error, "No room has that code. Check the code and try again.");
            if (getenv("SM64DS_LOBBY_FAKE_REMOVED")) {     /* proof pictures: the screen a banned player lands on */
                snprintf(F.s.error, sizeof F.s.error, "You are banned from this room.");
                snprintf(F.s.error_head, sizeof F.s.error_head, "Removed from the room");
            }
#endif
        } else {
            const char code[8] = { F.s.code[0], F.s.code[1], F.s.code[2], F.s.code[3], F.s.code[4], F.s.code[5], 0, 0 };
            const int host = F.s.am_host;
            fk_room(F.s.kind, host);
            if (host) F.guest_in = 30;
            if (!host) { snprintf(F.s.code, sizeof F.s.code, "%s", code); fk_seat("Tango", 0x31A431, 0xE66218, 0, 0, 1); }
        }
    }
    if (F.s.phase == LOBBY_IN_ROOM && F.guest_in > 0 && --F.guest_in == 0) {
        fk_seat("Opie", 0xFF3139, 0x004AFF, 1, 0, 0);
        fk_chat("Opie joined the room");
    }
    if (F.s.phase == LOBBY_STARTING && F.start_left > 0 && F.start_left < (1 << 30) && --F.start_left == 0) F.s.phase = LOBBY_NONE;
}
void fk_create(LobbyKind k)
{
    memset(&F, 0, sizeof F); F.s.phase = LOBBY_BUSY; F.s.kind = k; F.s.am_host = 1; F.busy_left = 40;
#if defined(PORT_ROM_CLEAN) && !LP_CLIENT_REAL
    F.fail_join = 1;
#endif
}
int code_char_ok(char c);
void fk_join(const char *code)
{
    memset(&F, 0, sizeof F);
    F.s.phase = LOBBY_BUSY; F.s.kind = LOBBY_COOP; F.busy_left = 40;
    snprintf(F.s.code, sizeof F.s.code, "%s", code);
    int ok = strlen(code) == 6;
    for (int i = 0; code[i]; ++i) ok = ok && code_char_ok(code[i]);
    F.fail_join = !ok || (getenv("SM64DS_LOBBY_FAKE_JOINFAIL") != 0);
#if defined(PORT_ROM_CLEAN) && !LP_CLIENT_REAL
    F.fail_join = 1;
#endif
}
void fk_leave(void) { memset(&F, 0, sizeof F); }
void fk_set_ready(int r) { for (int i = 0; i < F.s.n_seats; ++i) if (F.s.seats[i].is_me) F.s.seats[i].ready = r; }
void fk_start(void) { F.s.phase = LOBBY_STARTING; F.start_left = 60; }
void fk_set_setting(int i, int v) { if (i >= 0 && i < F.s.n_settings) F.s.settings[i].value = v; }
void fk_set_coop_save(int c) { F.s.coop_save_continue = c; }
void fk_set_server_name(const char *n) { snprintf(F.s.server_name, sizeof F.s.server_name, "%s", n); }
void fk_send_chat(const char *t)
{
    char line[96];
    const char *me = "Tango";
    for (int i = 0; i < F.s.n_seats; ++i) if (F.s.seats[i].is_me) me = F.s.seats[i].name;
    snprintf(line, sizeof line, "%s: %s", me, t);
    fk_chat(line);
}
void fk_kick(int seat)
{
    if (seat < 0 || seat >= F.s.n_seats || F.s.seats[seat].is_me) return;
    memmove(&F.s.seats[seat], &F.s.seats[seat + 1], sizeof F.s.seats[0] * (size_t)(F.s.n_seats - seat - 1));
    --F.s.n_seats;
}
void fk_mute(int seat, int m) { if (seat >= 0 && seat < F.s.n_seats) F.s.seats[seat].muted = m; }
void fk_ban(int seat) { fk_kick(seat); }

int g_auto_coop;                   /* SM64DS_LOBBY_SHOW realhost:coop (proof rows): press COOP on the choice */
int g_fake = !LP_CLIENT_REAL;      /* the stand-in answers (always without the real client; with it only when a proof knob asks) */

const LobbyState *C_state(void)
{
#if LP_CLIENT_REAL
    if (!g_fake) return lobby_state();
#endif
    return fk_state();
}
#if LP_CLIENT_REAL
#  define C_CALL(real, fake) do { if (g_fake) { fake; } else { real; } } while (0)
#else
#  define C_CALL(real, fake) do { fake; } while (0)
#endif
void C_tick(void) { C_CALL(lobby_tick(), fk_tick()); }
void C_create(LobbyKind k) { C_CALL(lobby_create(k), fk_create(k)); }
void C_join(const char *c) { C_CALL(lobby_join(c), fk_join(c)); }
void C_leave(void) { C_CALL(lobby_leave(), fk_leave()); }
void C_ready(int r) { C_CALL(lobby_set_ready(r), fk_set_ready(r)); }
void C_start(void) { C_CALL(lobby_start(), fk_start()); }
void C_setting(int i, int v) { C_CALL(lobby_set_setting(i, v), fk_set_setting(i, v)); }
void C_save(int c) { C_CALL(lobby_set_coop_save(c), fk_set_coop_save(c)); }
void C_name(const char *n) { C_CALL(lobby_set_server_name(n), fk_set_server_name(n)); }
void C_chat(const char *t) { C_CALL(lobby_send_chat(t), fk_send_chat(t)); }
void C_kick(int s) { C_CALL(lobby_kick(s), fk_kick(s)); }
void C_mute(int s, int m) { C_CALL(lobby_mute(s, m), fk_mute(s, m)); }
void C_ban(int s) { C_CALL(lobby_ban(s), fk_ban(s)); }

/* ================================================================ keyboard ====================================================== */
/* typed text is read by polling the keys once a logic frame (hal/menu_online_profile.cpp does the same for the NAME popup): the window
   must have focus, or a scripted SM64DS_HOST_KEY press stands in for the key. user32 is loaded by name (tools/sdl_import_guard.py). */
SHORT (WINAPI *p_GetAsyncKeyState)(int);
SHORT (WINAPI *p_GetKeyState)(int);
BOOL (WINAPI *p_OpenClipboard)(HWND);
BOOL (WINAPI *p_EmptyClipboard)(void);
HANDLE (WINAPI *p_SetClipboardData)(UINT, HANDLE);
BOOL (WINAPI *p_CloseClipboard)(void);
void load_user32(void)
{
    static int done;
    if (done) return;
    done = 1;
    if (HMODULE u = LoadLibraryA("user32.dll")) {
        p_GetAsyncKeyState = (SHORT (WINAPI *)(int))GetProcAddress(u, "GetAsyncKeyState");
        p_GetKeyState = (SHORT (WINAPI *)(int))GetProcAddress(u, "GetKeyState");
        p_OpenClipboard = (BOOL (WINAPI *)(HWND))GetProcAddress(u, "OpenClipboard");
        p_EmptyClipboard = (BOOL (WINAPI *)(void))GetProcAddress(u, "EmptyClipboard");
        p_SetClipboardData = (HANDLE (WINAPI *)(UINT, HANDLE))GetProcAddress(u, "SetClipboardData");
        p_CloseClipboard = (BOOL (WINAPI *)(void))GetProcAddress(u, "CloseClipboard");
    }
}
int key_down(int vk)
{
    if (port_host_key_script(vk)) return 1;
    load_user32();
    return p_GetAsyncKeyState && hal_window_focused() && (p_GetAsyncKeyState(vk) & 0x8000) ? 1 : 0;
}

/* the room code alphabet of the lobby server (port/tools/lobby/app/server.py): no I, L, O, 0 or 1 */
const char kCodeAlphabet[] = "ABCDEFGHJKMNPQRSTUVWXYZ23456789";
int code_char_ok(char c) { return c && strchr(kCodeAlphabet, c) != 0; }

int clip_copy(const char *text)
{
    load_user32();
    if (!p_OpenClipboard || !p_EmptyClipboard || !p_SetClipboardData || !p_CloseClipboard) return 0;
    const size_t n = strlen(text) + 1;
    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, n);
    if (!h) return 0;
    if (void *p = GlobalLock(h)) { memcpy(p, text, n); GlobalUnlock(h); }
    if (!p_OpenClipboard(0)) { GlobalFree(h); return 0; }
    p_EmptyClipboard();
    const int ok = p_SetClipboardData(CF_TEXT, h) != 0;
    p_CloseClipboard();
    if (!ok) GlobalFree(h);
    return ok;
}

/* ================================================================ page state ===================================================== */

enum Screen { SCR_CLOSED = 0, SCR_CHOOSE, SCR_ROOM };
enum Ctl {
    C_NONE = -1,
    C_BACK = 0, C_CODE, C_EYE, C_COPY, C_CHAT, C_NAME, C_SETTINGS, C_SAVE, C_MODS, C_PRIVACY, C_READY,
    C_CHOOSE_COOP, C_CHOOSE_VS, C_FAIL_BACK,
    C_SET_ROW0 = 20,                      /* the settings panel's rows (C_SET_ROW0 + i), its Back */
    C_SET_BACK = 40,
    C_SEAT0 = 50,                         /* a seat's plate (C_SEAT0 + i) */
    C_MUTE0 = 70,                         /* a seat's mute button */
    C_KICK0 = 90,                         /* a seat's kick button (host) */
    C_BAN0 = 110,                         /* a seat's ban button (host) */
    C_COUNT = 130
};
/* the small buttons on a seat's plate: mute, kick, ban */
inline bool seat_button(int c) { return c >= C_MUTE0 && c < C_BAN0 + 16; }

struct Rect { int x, y, w, h; };
inline bool in_rect(const Rect &r, int x, int y) { return r.w > 0 && x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h; }

struct Page {
    int want;              /* 0 none, 1 host (choose), 2 join */
    char want_code[8];
    int screen;
    int focus, hover;      /* Ctl ids */
    int field;             /* the text field being typed into: C_CHAT, C_NAME or 0 */
    char chat_in[80];
    char name_in[48];
    int code_hidden;
    int copied_t;          /* frames left of the "Copied" note */
    int panel;             /* the settings panel is up */
    int start_t;           /* frames the STARTING state has shown */
    int handoff_busy;      /* port_handoff_busy() was non-zero last frame */
    int frames;
    int input_ready;       /* every device was let go once since the page opened */
    unsigned prev_keys, prev_pad;
    int prev_ml;
    unsigned char prev_vk[256];
    int bs_hold;
    int pw, ph;            /* the picture the layout was made for */
    int env_at, env_done, env_frames;
    char env_what[32];
    int seat_menu;         /* the seat whose mute / kick / ban buttons are shown (hover or pad focus), -1 none */
    int ban_arm;           /* BAN pressed once on this seat (-1 none): a second press bans; anything else lets it go */
    int ban_arm_t;         /* frames the armed BAN stays armed */
    int ban_arm_member;    /* the armed seat's player, by the server's seat number and name: a roster change under the arm lets */
    char ban_arm_name[32]; /* it go instead of moving it onto whoever slid into that row */
    int joining;           /* the page was opened by a room code */
    int seen_phase;        /* the client has been out of LOBBY_NONE since the page opened */
    int keep_tick;         /* the page closed for the start: lobby_tick() goes on once a frame until the client is back to NONE or FAILED */
} P = { 0 };

void log_line(const char *what) { fprintf(stderr, "[lobbypage] %s\n", what); }

/* ================================================================ layout ========================================================= */
/* one unit is a 384th of the picture height; the page's width in units follows the picture (512 at 4:3, 683 at 16:9), so every aspect
   shows the same rows with the columns widened. */
struct Geom {
    float u;
    int W;                 /* width in units */
    Rect r[C_COUNT];
    Rect title;            /* where LOBBY is written */
    Rect chat_lines;       /* the chat box's message area */
    Rect seats_box;
    Rect panel;            /* the settings panel's sheet */
    Rect fail_box;
    int n_seat_slots;
};

void make_geom(Geom *g, int w, int h, const LobbyState *s)
{
    memset(g, 0, sizeof *g);
    float u = h / 384.0f;
    if (u < 0.5f) u = 0.5f;
    g->u = u;
    int W = (int)(w / u);
    if (W < 400) W = 400;
    g->W = W;
    auto R = [&](float x, float y, float rw, float rh) { Rect r = { (int)(x * u + 0.5f), (int)(y * u + 0.5f), (int)(rw * u + 0.5f), (int)(rh * u + 0.5f) }; return r; };
    const float mx = 14;
    g->title = R(W * 0.5f - 90, 6, 180, 44);
    g->r[C_BACK] = R(W - mx - 58, 10, 58, 34);
    if (P.screen == SCR_CHOOSE) {
        g->r[C_CHOOSE_COOP] = R(W * 0.5f - 170, 150, 160, 72);
        g->r[C_CHOOSE_VS] = R(W * 0.5f + 10, 150, 160, 72);
        return;
    }
    if (!s || s->phase == LOBBY_BUSY || s->phase == LOBBY_NONE) return;
    if (s->phase == LOBBY_FAILED) {
        g->fail_box = R(W * 0.5f - 180, 110, 360, 150);
        g->r[C_FAIL_BACK] = R(W * 0.5f - 50, 214, 100, 34);
        return;
    }
    /* the code group top left: the code plank, the eye, the copy button */
    const float cw = W >= 600 ? 150 : 124;
    g->r[C_CODE] = R(mx, 10, cw, 34);
    g->r[C_EYE] = R(mx + cw + 4, 10, 34, 34);
    g->r[C_COPY] = R(mx + cw + 42, 10, 34, 34);
    {   /* a narrow window: LOBBY moves right, centred between the copy button and the back arrow, so the code group never covers it */
        const float copy_end = mx + cw + 76 + 8, back_x = W - mx - 58 - 8;
        if (W * 0.5f - 90 < copy_end) g->title = R((copy_end + back_x) * 0.5f - 90, 6, 180, 44);
    }
    const float top = 58, bot = 252;
    const float inner = W - 2 * mx;
    const float chat_w = inner * 0.46f;
    g->r[C_CHAT] = R(mx, top, chat_w, bot - top);
    g->chat_lines = R(mx + 20, top + 16, chat_w - 40, bot - top - 62);
    const float rx = mx + chat_w + 16, rw = W - mx - rx, half = (rw - 10) * 0.5f;
    g->r[C_NAME] = R(rx, top, rw, 30);
    if (s->kind == LOBBY_COOP) {
        g->r[C_SETTINGS] = R(rx, top + 38, half, 30);
        g->r[C_SAVE] = R(rx + half + 10, top + 38, half, 30);
        g->r[C_MODS] = R(rx, top + 76, half, 30);
        g->r[C_PRIVACY] = R(rx + half + 10, top + 76, half, 30);
    } else {
        g->r[C_SETTINGS] = R(rx, top + 38, half, 30);
        g->r[C_PRIVACY] = R(rx + half + 10, top + 38, half, 30);
        g->r[C_MODS] = R(rx, top + 76, rw, 30);       /* VS: no mods box; this row is the match line (drawn, not pressed) */
    }
    g->r[C_READY] = R(rx, top + 118, rw, bot - top - 118);
    /* the seats along the bottom: one row up to 8, two rows above that */
    const int slots = s->max_seats > 0 ? (s->max_seats > 16 ? 16 : s->max_seats) : 4;
    g->n_seat_slots = slots;
    g->seats_box = R(mx, 262, inner, 112);
    const int per_row = slots > 8 ? (slots + 1) / 2 : slots;
    const int rows = slots > 8 ? 2 : 1;
    const float sw = (inner - 16) / per_row, sh = (112 - 12 - (rows - 1) * 4) / (float)rows;
    for (int i = 0; i < slots; ++i) {
        const int row = i / per_row, col = i % per_row;
        const float pad = sw > 60 ? 8 : 3;
        const float x = mx + 8 + col * sw + pad * 0.5f, y = 268 + row * (sh + 4);
        g->r[C_SEAT0 + i] = R(x, y, sw - pad, sh);
        /* three small buttons (mute, kick, ban) in a row on the plate: smaller on a narrow plate so the row stays on it */
        float b = sh > 60 ? 20 : 15;
        if (3 + 3 * (b + 3) > sw - pad) b = (sw - pad - 3) / 3 - 3;
        g->r[C_MUTE0 + i] = R(x + 3, y + 3, b, b);
        g->r[C_KICK0 + i] = R(x + 3 + b + 3, y + 3, b, b);
        g->r[C_BAN0 + i] = R(x + 3 + 2 * (b + 3), y + 3, b, b);
    }
    if (P.panel) {
        const int n = s->n_settings;
        const float ph = 64 + n * 34 + 46;
        g->panel = R(W * 0.5f - 170, 192 - ph * 0.5f, 340, ph);
        for (int i = 0; i < n && i < 16; ++i) g->r[C_SET_ROW0 + i] = R(W * 0.5f - 150, 192 - ph * 0.5f + 52 + i * 34, 300, 30);
        g->r[C_SET_BACK] = R(W * 0.5f - 50, 192 + ph * 0.5f - 44, 100, 32);
    }
}

/* ================================================================ which controls are live ======================================== */

int seat_count(const LobbyState *s) { return s ? (s->n_seats < 16 ? s->n_seats : 16) : 0; }
int my_seat(const LobbyState *s)
{
    for (int i = 0; i < seat_count(s); ++i) if (s->seats[i].is_me) return i;
    return -1;
}
int all_ready(const LobbyState *s)
{
    if (!s || seat_count(s) < 1) return 0;
    for (int i = 0; i < seat_count(s); ++i) if (!s->seats[i].ready) return 0;
    return 1;
}
int in_room(const LobbyState *s) { return s && (s->phase == LOBBY_IN_ROOM || s->phase == LOBBY_STARTING); }

/* a control the focus can sit on and a press does something with */
bool live(const LobbyState *s, int c)
{
    if (c == C_BACK) return P.screen != SCR_CLOSED && !(s && s->phase == LOBBY_FAILED && P.screen == SCR_ROOM);
    if (P.screen == SCR_CHOOSE) return c == C_CHOOSE_COOP || c == C_CHOOSE_VS;
    if (!s) return false;
    if (s->phase == LOBBY_FAILED) return c == C_FAIL_BACK;
    if (!in_room(s)) return false;
    if (P.panel) {
        if (c == C_SET_BACK) return true;
        return c >= C_SET_ROW0 && c < C_SET_ROW0 + s->n_settings && s->am_host;
    }
    switch (c) {
    case C_EYE: case C_COPY: case C_CHAT: case C_SETTINGS: case C_READY: return true;
    case C_NAME: return s->am_host != 0;
    case C_SAVE: return s->kind == LOBBY_COOP && s->am_host;
    default: break;
    }
    if (c >= C_SEAT0 && c < C_SEAT0 + 16) return c - C_SEAT0 < seat_count(s);
    if (c >= C_MUTE0 && c < C_MUTE0 + 16) { const int i = c - C_MUTE0; return i < seat_count(s) && !s->seats[i].is_me && P.seat_menu == i; }
    if (c >= C_KICK0 && c < C_KICK0 + 16) { const int i = c - C_KICK0; return i < seat_count(s) && !s->seats[i].is_me && s->am_host && P.seat_menu == i; }
    if (c >= C_BAN0 && c < C_BAN0 + 16) { const int i = c - C_BAN0; return i < seat_count(s) && !s->seats[i].is_me && s->am_host && P.seat_menu == i; }
    return false;
}

/* the pad's step: the nearest live control whose centre lies in that direction (a cone of about 60 degrees each side), nearest by a
   distance that weighs the sideways offset twice */
int step_focus(const Geom &g, const LobbyState *s, int from, int dx, int dy)
{
    if (from < 0 || !live(s, from)) return -1;
    const Rect &a = g.r[from];
    const float ax = a.x + a.w * 0.5f, ay = a.y + a.h * 0.5f;
    int best = -1;
    float bd = 1e30f;
    for (int c = 0; c < C_COUNT; ++c) {
        if (c == from || !live(s, c) || g.r[c].w <= 0) continue;
        if (c >= C_SEAT0 && c < C_SEAT0 + 16 && from >= C_SEAT0 && from < C_SEAT0 + 16 && dy != 0 && seat_count(s) <= 8) continue;
        const Rect &b = g.r[c];
        const float bx = b.x + b.w * 0.5f, by = b.y + b.h * 0.5f;
        const float along = (bx - ax) * dx + (by - ay) * dy;
        const float side = fabsf((bx - ax) * dy) + fabsf((by - ay) * dx);
        if (along <= 1.0f || side > along * 1.8f) continue;
        const float d = along + 2.0f * side;
        if (d < bd) { bd = d; best = c; }
    }
    return best;
}

int first_focus(const LobbyState *s)
{
    if (P.screen == SCR_CHOOSE) return C_CHOOSE_COOP;
    if (s && s->phase == LOBBY_FAILED) return C_FAIL_BACK;
    if (in_room(s)) return P.panel ? (s->am_host && s->n_settings ? C_SET_ROW0 : C_SET_BACK) : C_READY;
    return C_BACK;
}

/* ================================================================ actions ======================================================== */

void close_page(const char *why)
{
    char b[96];
    snprintf(b, sizeof b, "closed (%s)", why);
    log_line(b);
    P.screen = SCR_CLOSED;
    P.field = 0;
    P.panel = 0;
}

void leave_room(const char *why)
{
    C_leave();
    close_page(why);
}

/* the armed BAN still points at the player it was armed on (same row, same server seat number, same name) */
int ban_arm_holds(const LobbyState *s)
{
    const int i = P.ban_arm;
    return i >= 0 && i < s->n_seats && s->seats[i].member == P.ban_arm_member && !strcmp(s->seats[i].name, P.ban_arm_name);
}

void open_panel(int on)
{
    P.panel = on;
    P.focus = -1;
    log_line(on ? "settings panel open" : "settings panel closed");
}

void press(const LobbyState *s, int c)
{
    char b[128];
    if (c == C_BACK) {
        if (P.screen == SCR_CHOOSE) { close_page("back from the choice"); return; }
        leave_room("leave");
        return;
    }
    if (c == C_CHOOSE_COOP || c == C_CHOOSE_VS) {
        const LobbyKind k = c == C_CHOOSE_COOP ? LOBBY_COOP : LOBBY_VS;
        snprintf(b, sizeof b, "host: creating a %s room", k == LOBBY_COOP ? "coop" : "VS");
        log_line(b);
        C_create(k);
        P.screen = SCR_ROOM;
        P.focus = -1;
        return;
    }
    if (c == C_FAIL_BACK) { leave_room("back after a failure"); return; }
    if (!s) return;
    if (c == C_SET_BACK) { open_panel(0); return; }
    if (c >= C_SET_ROW0 && c < C_SET_ROW0 + 16) {
        const int i = c - C_SET_ROW0;
        const LobbySetting &t = s->settings[i];
        int v = t.value + 1;
        if (v > t.max) v = t.min;
        C_setting(i, v);
        snprintf(b, sizeof b, "setting %d (%s) -> %d", i, t.label ? t.label : "?", v);
        log_line(b);
        return;
    }
    switch (c) {
    case C_EYE: P.code_hidden = !P.code_hidden; log_line(P.code_hidden ? "code hidden" : "code shown"); return;
    case C_COPY: {
#if LP_PROOF
        if (getenv("SM64DS_LOBBY_NO_CLIPBOARD")) {      /* proof rows leave the machine's clipboard alone */
            P.copied_t = 90;
            snprintf(b, sizeof b, "copy code %s: proof run, clipboard left alone", s->code);
            log_line(b);
            return;
        }
#endif
        const int ok = clip_copy(s->code);
        P.copied_t = 90;
        snprintf(b, sizeof b, "copy code %s to the clipboard: %s", s->code, ok ? "ok" : "FAILED");
        log_line(b);
        return;
    }
    case C_CHAT: P.field = C_CHAT; log_line("chat field: typing"); return;
    case C_NAME:
        if (!s->am_host) return;
        P.field = C_NAME;
        snprintf(P.name_in, sizeof P.name_in, "%s", s->server_name);
        log_line("server name field: typing");
        return;
    case C_SETTINGS: open_panel(1); return;
    case C_SAVE:
        C_save(!s->coop_save_continue);
        log_line(!s->coop_save_continue ? "coop save -> Continue" : "coop save -> New");
        return;
    case C_READY: {
        if (s->phase != LOBBY_IN_ROOM) return;
        const int me = my_seat(s);
        if (s->am_host && all_ready(s)) { log_line("START"); C_start(); return; }
        const int r = me >= 0 ? !s->seats[me].ready : 1;
        C_ready(r);
        log_line(r ? "ready" : "not ready");
        return;
    }
    default: break;
    }
    if (c >= C_SEAT0 && c < C_SEAT0 + 16) { P.seat_menu = c - C_SEAT0; return; }
    if (c >= C_MUTE0 && c < C_MUTE0 + 16) {
        const int i = c - C_MUTE0;
        C_mute(i, !s->seats[i].muted);
        snprintf(b, sizeof b, "seat %d (%s) %s", i, s->seats[i].name, !s->seats[i].muted ? "muted" : "unmuted");
        log_line(b);
        return;
    }
    if (c >= C_KICK0 && c < C_KICK0 + 16) {
        const int i = c - C_KICK0;
        snprintf(b, sizeof b, "kick seat %d (%s)", i, s->seats[i].name);
        log_line(b);
        C_kick(i);
        P.seat_menu = -1;
        P.focus = C_READY;
        return;
    }
    if (c >= C_BAN0 && c < C_BAN0 + 16) {
        /* ONE-STEP CONFIRM: the first press arms the button (it turns red and asks "Press again to ban"), the second press on the same
           seat bans. Any other press, a move off the button or about three seconds let it go. A ban lasts until the room closes, so one
           stray click or tap of A must not do it. */
        const int i = c - C_BAN0;
        if (P.ban_arm != i || !ban_arm_holds(s)) {
            P.ban_arm = i;
            P.ban_arm_t = 180;
            P.ban_arm_member = s->seats[i].member;
            snprintf(P.ban_arm_name, sizeof P.ban_arm_name, "%s", s->seats[i].name);
            snprintf(b, sizeof b, "ban armed on seat %d (%s)", i, s->seats[i].name);
            log_line(b);
            return;
        }
        P.ban_arm = -1;
        snprintf(b, sizeof b, "ban seat %d (%s)", i, s->seats[i].name);
        log_line(b);
        C_ban(i);
        P.seat_menu = -1;
        P.focus = C_READY;
        return;
    }
}

/* a setting row stepped left / right on the pad */
void setting_step(const LobbyState *s, int i, int d)
{
    const LobbySetting &t = s->settings[i];
    int v = t.value + d;
    if (v > t.max) v = t.max;
    if (v < t.min) v = t.min;
    if (v == t.value) return;
    C_setting(i, v);
    char b[96];
    snprintf(b, sizeof b, "setting %d (%s) -> %d", i, t.label ? t.label : "?", v);
    log_line(b);
}

/* typing into the field that has it: letters, digits, space and a little punctuation. Enter sends the chat line (the field keeps the
   typing) or stores the server name (the field lets go); Escape lets go; Backspace deletes (held, it repeats). */
void type_frame(const LobbyState *s)
{
    char *buf = P.field == C_CHAT ? P.chat_in : P.name_in;
    const size_t cap = P.field == C_CHAT ? sizeof P.chat_in : sizeof P.name_in;
    const int shift = key_down(VK_SHIFT);
    const int caps = (p_GetKeyState && (p_GetKeyState(VK_CAPITAL) & 1)) ? 1 : 0;
    const int ctl = key_down(VK_CONTROL) || key_down(VK_MENU);
    auto fresh = [&](int vk) { const int d = key_down(vk); const int f = d && !P.prev_vk[vk]; P.prev_vk[vk] = (unsigned char)d; return f; };
    auto put = [&](char c) { const size_t n = strlen(buf); if (n + 1 < cap) { buf[n] = c; buf[n + 1] = 0; } };
    for (int vk = 'A'; vk <= 'Z'; ++vk) if (fresh(vk) && !ctl) put((char)((shift ^ caps) ? vk : vk + 32));
    static const char kShiftDigit[] = ")!@#$%^&*(";
    for (int vk = '0'; vk <= '9'; ++vk) if (fresh(vk) && !ctl) put(shift ? kShiftDigit[vk - '0'] : (char)vk);
    for (int vk = VK_NUMPAD0; vk <= VK_NUMPAD9; ++vk) if (fresh(vk) && !ctl) put((char)('0' + vk - VK_NUMPAD0));
    struct Oem { int vk; char lo, hi; };
    static const Oem kOem[] = { { VK_SPACE, ' ', ' ' }, { VK_OEM_PERIOD, '.', '>' }, { VK_OEM_COMMA, ',', '<' }, { VK_OEM_2, '/', '?' },
                                { VK_OEM_1, ';', ':' }, { VK_OEM_7, '\'', '"' }, { VK_OEM_MINUS, '-', '_' }, { VK_OEM_PLUS, '=', '+' } };
    for (const Oem &o : kOem) if (fresh(o.vk) && !ctl) put(shift ? o.hi : o.lo);
    /* Backspace: a press deletes one, a hold repeats after a third of a second */
    if (key_down(VK_BACK)) {
        if (P.bs_hold == 0 || (P.bs_hold > 20 && (P.bs_hold % 3) == 0)) { const size_t n = strlen(buf); if (n) buf[n - 1] = 0; }
        ++P.bs_hold;
    } else P.bs_hold = 0;
    P.prev_vk[VK_BACK] = (unsigned char)key_down(VK_BACK);
    if (fresh(VK_RETURN)) {
        if (P.field == C_CHAT) {
            if (buf[0]) { char b[128]; snprintf(b, sizeof b, "chat sent: %s", buf); log_line(b); C_chat(buf); buf[0] = 0; }
        } else {
            C_name(buf);
            char b[96];
            snprintf(b, sizeof b, "server name stored: %s", buf);
            log_line(b);
            P.field = 0;
        }
    }
    if (fresh(VK_ESCAPE)) { log_line("field let go (Escape)"); P.field = 0; }
    (void)s;
}

void sync_prev_vk(void)
{
    for (int vk = 8; vk < 256; ++vk) P.prev_vk[vk] = (unsigned char)key_down(vk);
}

Geom g_geom;

}  // namespace

/* ================================================================ the seat's calls =============================================== */

extern "C" void lobby_page_request_host(void) { P.want = 1; log_line("request: HOST (the Coop / VS choice)"); }
extern "C" void lobby_page_request_join(const char *code)
{
    P.want = 2;
    snprintf(P.want_code, sizeof P.want_code, "%s", code ? code : "");
    char b[64];
    snprintf(b, sizeof b, "request: join room %s", P.want_code);
    log_line(b);
}
extern "C" int lobby_page_pending(void) { return P.want != 0; }
extern "C" void lobby_page_clear_request(void) { P.want = 0; }
extern "C" int lobby_page_open(void) { return P.screen != SCR_CLOSED; }

extern "C" void lobby_page_open_now(void)
{
    const int want = P.want;
    P.want = 0;
    P.focus = -1;
    P.hover = -1;
    P.field = 0;
    P.panel = 0;
    P.start_t = 0;
    P.handoff_busy = 1;      /* the first frame takes the baseline */
    P.frames = 0;
    P.input_ready = 0;
    P.copied_t = 0;
    P.seat_menu = -1;
    P.ban_arm = -1;
    P.chat_in[0] = 0;
    load_user32();
    sync_prev_vk();
    P.joining = want == 2;
    P.seen_phase = 0;
    if (want == 2) {
        C_join(P.want_code);
        P.screen = SCR_ROOM;
        log_line("open: joining");
    } else if (want == 3) {
        P.screen = SCR_ROOM;          /* proof rows: the stand-in's scenario is already set */
        log_line("open: a proof scenario");
    } else {
        P.screen = SCR_CHOOSE;
        log_line("open: the Coop / VS choice");
    }
}

extern "C" void lobby_page_cancel(void)
{
    if (P.screen == SCR_CLOSED) return;
    const LobbyState *s = C_state();
    if (s && (s->phase == LOBBY_BUSY || s->phase == LOBBY_IN_ROOM)) C_leave();
    close_page("the pause was taken away");
}

extern "C" int lobby_page_env_poll(void)
{
    if (P.keep_tick && P.screen == SCR_CLOSED) {      /* after the start the client's polling still runs from here, once a frame */
        C_tick();
        const LobbyState *s = C_state();
        if (s->phase == LOBBY_NONE || s->phase == LOBBY_FAILED) { P.keep_tick = 0; log_line("the client is done with the room: no more ticks"); }
    }
#if LP_PROOF
    if (P.env_done) return 0;
    if (!P.env_at) {
        const char *e = getenv("SM64DS_LOBBY_SHOW");
        if (!e || !*e) { P.env_done = 1; return 0; }
        const char *at = strchr(e, '@');
        P.env_at = at ? atoi(at + 1) : 60;
        if (P.env_at < 1) P.env_at = 1;
        snprintf(P.env_what, sizeof P.env_what, "%.*s", at ? (int)(at - e) : (int)strlen(e), e);
    }
    if (++P.env_frames < P.env_at) return 0;
    if (!strcmp(P.env_what, "realjoin:FILE")) {
        /* joint rows: the code the host's page wrote (SM64DS_LOBBY_CODEFILE on the host) is read from SM64DS_LOBBY_JOINFILE; wait for it */
        const char *jf = getenv("SM64DS_LOBBY_JOINFILE");
        char code[16] = { 0 };
        FILE *f = jf ? fopen(jf, "rb") : 0;
        if (f) { const size_t n = fread(code, 1, 6, f); code[n] = 0; fclose(f); }
        if (strlen(code) != 6) return 0;
        snprintf(P.env_what, sizeof P.env_what, "realjoin:%s", code);
    }
    P.env_done = 1;
    char b[96];
    snprintf(b, sizeof b, "SM64DS_LOBBY_SHOW %s at frame %d", P.env_what, P.env_frames);
    log_line(b);
    if (!strcmp(P.env_what, "choose")) { P.want = 1; return 1; }
    if (!strncmp(P.env_what, "join:", 5)) { g_fake = 1; P.want = 2; snprintf(P.want_code, sizeof P.want_code, "%s", P.env_what + 5); return 1; }
    if (!strncmp(P.env_what, "realjoin:", 9)) { P.want = 2; snprintf(P.want_code, sizeof P.want_code, "%s", P.env_what + 9); return 1; }
    if (!strcmp(P.env_what, "realhost")) { P.want = 1; return 1; }
    if (!strcmp(P.env_what, "realhost:coop")) { P.want = 1; g_auto_coop = 1; return 1; }   /* the choice answered COOP by itself */
    /* <coop|vs>:<scenario> on the stand-in */
    const char *colon = strchr(P.env_what, ':');
    const LobbyKind k = !strncmp(P.env_what, "vs", 2) ? LOBBY_VS : LOBBY_COOP;
    g_fake = 1;
    fk_scenario(colon ? colon + 1 : "room_host", k);
    P.want = 3;
    if (colon && !strcmp(colon + 1, "room_hidden")) P.code_hidden = 1;
    return 1;
#else
    return 0;
#endif
}

#if LP_PROOF
/* SM64DS_LOBBY_REACH=1 (proof rows): once the room has stood for 60 frames, walk the pad's own step (step_focus, the seat menu rule of
   lobby_page_frame) breadth first from the first focus, with the settings panel shut and open, and log which live controls no chain of
   D-pad steps reaches. A press of SETTINGS opens the panel and a seat's focus shows its buttons, as in play. */
void reach_report(const LobbyState *s)
{
    const int saved_menu = P.seat_menu, saved_panel = P.panel;
    for (int panel = 0; panel < 2; ++panel) {
        P.panel = panel;
        P.seat_menu = -1;
        Geom g;
        make_geom(&g, P.pw > 0 ? P.pw : 512, P.ph > 0 ? P.ph : 384, s);
        static unsigned char seen[C_COUNT][17];
        static short qf[C_COUNT * 17], qm[C_COUNT * 17];
        memset(seen, 0, sizeof seen);
        int qh = 0, qt = 0;
        const int start = first_focus(s);
        if (start < 0 || start >= C_COUNT) continue;
        seen[start][16] = 1; qf[qt] = (short)start; qm[qt] = -1; ++qt;
        while (qh < qt) {
            const int f = qf[qh], m = qm[qh]; ++qh;
            for (int d = 0; d < 4; ++d) {
                const int dx = d == 0 ? 1 : d == 1 ? -1 : 0, dy = d == 2 ? 1 : d == 3 ? -1 : 0;
                P.seat_menu = m;
                const int n = step_focus(g, s, f, dx, dy);
                if (n < 0 || n >= C_COUNT) continue;
                int nm = m;
                if (n >= C_SEAT0 && n < C_SEAT0 + 16) nm = n - C_SEAT0;
                else if (!seat_button(n)) nm = -1;
                if (seen[n][nm < 0 ? 16 : nm]) continue;
                seen[n][nm < 0 ? 16 : nm] = 1;
                qf[qt] = (short)n; qm[qt] = (short)nm; ++qt;
            }
        }
        char b[400];
        int nl_ = 0, nr = 0, len = 0;
        char miss[256]; miss[0] = 0;
        for (int c = 0; c < C_COUNT; ++c) {
            int lv = 0, rc = 0;
            for (int m = -1; m < 16; ++m) {
                P.seat_menu = m;
                if (live(s, c)) lv = 1;
                if (seen[c][m < 0 ? 16 : m]) rc = 1;
            }
            if (!lv) continue;
            ++nl_;
            if (rc) ++nr;
            else if (len < 240) len += snprintf(miss + len, sizeof miss - (size_t)len, " %d", c);
        }
        snprintf(b, sizeof b, "pad reach (%s, %s, panel %s): %d live controls, %d reached from the first focus, missing:%s",
                 s->kind == LOBBY_VS ? "vs" : "coop", s->am_host ? "host" : "guest", panel ? "open" : "shut", nl_, nr, nr == nl_ ? " none" : miss);
        log_line(b);
    }
    P.seat_menu = saved_menu;
    P.panel = saved_panel;
}
#endif

extern "C" int lobby_page_frame(const SpInput *in, unsigned pad_buttons)
{
    if (P.screen == SCR_CLOSED) return 0;
    ++P.frames;
    C_tick();
    const LobbyState *s = C_state();
#if LP_PROOF
    if (g_auto_coop && P.screen == SCR_CHOOSE && P.frames > 30) { g_auto_coop = 0; log_line("realhost:coop presses COOP"); press(s, C_CHOOSE_COOP); }
    if (P.frames == 2) {      /* proof pictures: the hover buttons of a seat, the focus on a control, the settings panel */
        const char *e1 = getenv("SM64DS_LOBBY_SEATMENU"), *e2 = getenv("SM64DS_LOBBY_FOCUS"), *e3 = getenv("SM64DS_LOBBY_PANEL");
        if (e1 && *e1) P.seat_menu = atoi(e1);
        if (e2 && *e2) P.focus = atoi(e2);
        if (e3 && *e3 == '1') P.panel = 1;
    }
#endif
    {   /* every phase change of the client, in the log */
        static int last_phase = -1;
        if ((int)s->phase != last_phase) {
            char b[64];
            snprintf(b, sizeof b, "client phase %d -> %d (page frame %d)", last_phase, (int)s->phase, P.frames);
            log_line(b);
            last_phase = (int)s->phase;
        }
    }
    /* the client's state moves the page */
    if (P.screen == SCR_ROOM) {
        if (s->phase != LOBBY_NONE) P.seen_phase = 1;
        if (s->phase == LOBBY_NONE && (P.seen_phase || P.frames > 300)) { close_page("the client has no room"); return 0; }
#if LP_CLIENT_REAL
        /* a coop start taken IN PLACE goes back to IN_ROOM inside the same lobby_tick (lobby_client.cpp: the room lives on after the game), so
           the page never sees STARTING: what it sees is the hand-off taken, a session up where none was (port_handoff_busy turns non-zero) */
        {
            const int busy_now = !g_fake && port_handoff_busy() != 0;
            if (busy_now && !P.handoff_busy && in_room(s)) {
                P.keep_tick = 1;
                close_page("the game starts (the hand-off was taken)");
                return 0;
            }
            P.handoff_busy = busy_now;
        }
#endif
        if (s->phase == LOBBY_STARTING) {
            /* the pause is let go the frame the start is known: the coop hand-off walks the title to the room in place on the next frames,
               and a title held still under the page never got there (joint row r4: both consoles stopped at "pressed file A") */
            P.keep_tick = 1;
            close_page("the game starts");
            return 0;
        }
        if (!in_room(s)) { P.panel = 0; P.field = 0; }
#if LP_PROOF
        {
            static int room_frames;
            room_frames = in_room(s) ? room_frames + 1 : 0;
            if (room_frames == 60 && getenv("SM64DS_LOBBY_REACH")) reach_report(s);
            static int code_written;
            const char *cf = getenv("SM64DS_LOBBY_CODEFILE");      /* joint rows: the host's page hands its code to the joiner's */
            if (cf && *cf && !code_written && in_room(s) && s->am_host && strlen(s->code) == 6) {
                if (FILE *f = fopen(cf, "wb")) { fwrite(s->code, 1, 6, f); fclose(f); code_written = 1; log_line("room code written for the joint row"); }
            }
        }
#endif
        if (P.seat_menu >= seat_count(s)) P.seat_menu = -1;
        /* a player leaving shifts the rows: an armed BAN must not move onto whoever is in that row now */
        if (P.ban_arm >= 0 && !ban_arm_holds(s)) { P.ban_arm = -1; log_line("ban let go (the player in that row changed)"); }
#if LP_PROOF
        {   /* SM64DS_LOBBY_ARMBAN=<row>@<seats>+<frames>: once the room holds <seats> players the host arms BAN on <row>, and presses it
               again <frames> later (the G3 row: a player leaving in between must cost the arm, not ban the next player) */
            static int armed_at = -1, pressed2;
            const char *ab = getenv("SM64DS_LOBBY_ARMBAN");
            int row = 0, want = 0, later = 0;
            if (ab && sscanf(ab, "%d@%d+%d", &row, &want, &later) == 3 && in_room(s) && s->am_host) {
                if (armed_at < 0 && s->n_seats >= want && row < s->n_seats) {
                    P.seat_menu = row; P.focus = C_BAN0 + row; press(s, C_BAN0 + row); armed_at = P.frames;
                } else if (armed_at >= 0 && !pressed2 && P.frames - armed_at >= later && row < s->n_seats) {
                    pressed2 = 1; P.seat_menu = row; P.focus = C_BAN0 + row;
                    log_line("ARMBAN second press");
                    press(s, C_BAN0 + row);
                }
            }
        }
#endif
    }
    make_geom(&g_geom, P.pw > 0 ? P.pw : 512, P.ph > 0 ? P.ph : 384, s);
    const Geom &g = g_geom;
    unsigned k = in ? in->keys : 0;
    const int ml = in && in->mouse_l;
    /* every device let go once first: the press that opened the page (the HOST plank, Enter on the code) is not a press here */
    if (!P.input_ready) {
        if (!k && !ml && !(pad_buttons & 0x300F)) P.input_ready = 1;
        P.prev_keys = k; P.prev_pad = pad_buttons; P.prev_ml = ml;
        return 1;
    }
    /* a text field takes the keyboard: the page's keys (W A S D are the D-pad, Space and Enter are A, Backspace is B) are not read; the
       pad still is, and B or a pad step lets the field go */
    const int had_field = P.field;
    unsigned edge;
    if (had_field) {
        type_frame(s);
        const unsigned pe = pad_buttons & ~P.prev_pad;
        edge = 0;
        if (pe & 0x2000) { P.field = 0; log_line("field let go (pad B)"); }
        else if (pe & 0x000F) { P.field = 0; edge = (pe & 1 ? SPK_UP : 0) | (pe & 2 ? SPK_DOWN : 0) | (pe & 4 ? SPK_LEFT : 0) | (pe & 8 ? SPK_RIGHT : 0); }
        P.prev_keys = k;          /* the keys the typing held are not fresh page keys once the field lets go */
    } else {
        for (int vk = 8; vk < 256; ++vk) P.prev_vk[vk] = (unsigned char)key_down(vk);
        edge = k & ~P.prev_keys;
        P.prev_keys = k;
    }
    /* the mouse */
    int hov = -1;
    if (in && in->mouse_valid) {
        for (int c = 0; c < C_COUNT; ++c) {
            if (!live(s, c) || !in_rect(g.r[c], in->mouse_x, in->mouse_y)) continue;
            if (hov < 0 || seat_button(c)) hov = c;       /* the small seat buttons sit on the plate */
        }
        /* a hover on a seat shows its buttons */
        if (P.screen == SCR_ROOM && in_room(s) && !P.panel) {
            for (int i = 0; i < seat_count(s); ++i)
                if (in_rect(g.r[C_SEAT0 + i], in->mouse_x, in->mouse_y) && !s->seats[i].is_me) {
                    P.seat_menu = i;
                    for (int c : { C_MUTE0 + i, C_KICK0 + i, C_BAN0 + i })
                        if (live(s, c) && in_rect(g.r[c], in->mouse_x, in->mouse_y)) hov = c;
                }
        }
    }
    if (hov >= 0 && hov != P.hover) {
        if (hov != P.focus) host_menu_cursor_sound();
        P.focus = hov;
    }
    P.hover = hov;
    const int click = ml && !P.prev_ml;
    P.prev_ml = ml;
    if (click) {
        if (P.field && hov != P.field) { P.field = 0; log_line("field let go (a click elsewhere)"); }
        if (hov >= 0) {
            char b[64];
            snprintf(b, sizeof b, "click on control %d", hov);
            log_line(b);
            press(s, hov);
            P.prev_pad = pad_buttons;
            return P.screen != SCR_CLOSED;
        }
        if (P.panel && !in_rect(g.panel, in->mouse_x, in->mouse_y)) open_panel(0);
    }
    if (in && in->mouse_r && P.panel) open_panel(0);
    /* the pad and the keyboard */
    if (P.focus < 0 || !live(s, P.focus)) {
        if (edge & (SPK_UP | SPK_DOWN | SPK_LEFT | SPK_RIGHT | SPK_ACCEPT)) { P.focus = first_focus(s); host_menu_cursor_sound(); P.prev_pad = pad_buttons; return 1; }
    }
    if (P.panel && P.focus >= C_SET_ROW0 && P.focus < C_SET_ROW0 + 16 && (edge & (SPK_LEFT | SPK_RIGHT))) {
        setting_step(s, P.focus - C_SET_ROW0, (edge & SPK_RIGHT) ? 1 : -1);
        P.prev_pad = pad_buttons;
        return 1;
    }
    const int dx = (edge & SPK_RIGHT) ? 1 : (edge & SPK_LEFT) ? -1 : 0, dy = (edge & SPK_DOWN) ? 1 : (edge & SPK_UP) ? -1 : 0;
    if ((dx || dy) && P.focus >= 0) {
        const int n = step_focus(g, s, P.focus, dx, dy);
        if (n >= 0) {
            P.focus = n;
            host_menu_cursor_sound();
            { char b[48]; snprintf(b, sizeof b, "focus on control %d", n); log_line(b); }
            if (n >= C_SEAT0 && n < C_SEAT0 + 16) P.seat_menu = n - C_SEAT0;
            else if (!seat_button(n)) P.seat_menu = -1;
        }
    }
    /* an armed BAN lets go when the focus leaves it, the seat menu closes, or its time runs out (a roster change: above, every frame) */
    if (P.ban_arm >= 0 && (P.focus != C_BAN0 + P.ban_arm || P.seat_menu != P.ban_arm || --P.ban_arm_t <= 0)) {
        P.ban_arm = -1;
        log_line("ban let go");
    }
    if ((edge & SPK_ACCEPT) && P.focus >= 0 && live(s, P.focus)) {
        char b[48];
        snprintf(b, sizeof b, "accept on control %d", P.focus);
        log_line(b);
        press(s, P.focus);
    }
    else if (edge & SPK_BACK) {
        if (P.panel) open_panel(0);
        else if (P.seat_menu >= 0 && P.focus >= C_MUTE0) { P.focus = C_SEAT0 + P.seat_menu; }
        else if (P.screen == SCR_CHOOSE) close_page("back from the choice");
        else if (s->phase == LOBBY_FAILED) leave_room("back after a failure");
        else leave_room("back");
    }
    if (P.copied_t > 0) --P.copied_t;
    P.prev_pad = pad_buttons;
    return P.screen != SCR_CLOSED;
}

/* ================================================================ drawing ======================================================== */
namespace {

struct Img {
    int w = 0, h = 0;
    std::vector<uint32_t> px;           /* premultiplied 0xAARRGGBB */
    bool ok() const { return w > 0 && h > 0 && !px.empty(); }
};

void load_png(const char *path, Img *out)
{
    *out = Img();
    FILE *f = fopen(path, "rb");
    if (!f) return;
    fseek(f, 0, SEEK_END);
    const long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (n <= 0 || n > (16L << 20)) { fclose(f); return; }
    std::vector<unsigned char> buf((size_t)n);
    const size_t got = fread(&buf[0], 1, (size_t)n, f);
    fclose(f);
    if (got != (size_t)n) return;
    int w = 0, h = 0, comp = 0;
    unsigned char *p = stbi_load_from_memory(&buf[0], (int)n, &w, &h, &comp, 4);
    if (!p) return;
    if (w <= 0 || h <= 0 || w > 4096 || h > 4096) { stbi_image_free(p); return; }
    out->w = w; out->h = h;
    out->px.resize((size_t)w * h);
    for (int i = 0; i < w * h; ++i) {
        const unsigned r = p[i * 4], g = p[i * 4 + 1], b = p[i * 4 + 2], a = p[i * 4 + 3];
        out->px[i] = (a << 24) | (((r * a + 127) / 255) << 16) | (((g * a + 127) / 255) << 8) | ((b * a + 127) / 255);
    }
    stbi_image_free(p);
}

struct Surf { uint32_t *px; int stride, w, h; };

inline uint32_t over(uint32_t d, int sr, int sg, int sb, int sa)
{
    const int ia = 255 - sa;
    const int r = sr + (((d >> 16) & 255) * ia + 127) / 255, g = sg + (((d >> 8) & 255) * ia + 127) / 255, b = sb + ((d & 255) * ia + 127) / 255;
    return 0xFF000000u | (uint32_t)((r > 255 ? 255 : r) << 16) | (uint32_t)((g > 255 ? 255 : g) << 8) | (uint32_t)(b > 255 ? 255 : b);
}
inline void put_pm(const Surf &s, int x, int y, uint32_t c, int a = 255)
{
    if (x < 0 || y < 0 || x >= s.w || y >= s.h) return;
    int sa = (int)(c >> 24), sr = (int)((c >> 16) & 255), sg = (int)((c >> 8) & 255), sb = (int)(c & 255);
    if (a != 255) { sa = sa * a / 255; sr = sr * a / 255; sg = sg * a / 255; sb = sb * a / 255; }
    if (sa == 0) return;
    uint32_t *d = &s.px[(size_t)y * s.stride + x];
    *d = sa == 255 ? (0xFF000000u | (c & 0xFFFFFF)) : over(*d, sr, sg, sb, sa);
}

void scale_rect(const Img &I, double sx, double sy, double sw, double sh, int dw, int dh, std::vector<uint32_t> *out)
{
    out->assign((size_t)(dw > 0 ? dw : 0) * (dh > 0 ? dh : 0), 0);
    if (!I.ok() || dw < 1 || dh < 1 || sw <= 0 || sh <= 0) return;
    const double kx = sw / dw, ky = sh / dh;
    int nx = (int)ceil(kx), ny = (int)ceil(ky);
    if (nx < 1) nx = 1;
    if (ny < 1) ny = 1;
    if (nx > 6) nx = 6;
    if (ny > 6) ny = 6;
    for (int y = 0; y < dh; ++y)
        for (int x = 0; x < dw; ++x) {
            float a = 0, r = 0, g = 0, b = 0;
            for (int j = 0; j < ny; ++j)
                for (int i = 0; i < nx; ++i) {
                    double fx = sx + (x + (i + 0.5) / nx) * kx - 0.5, fy = sy + (y + (j + 0.5) / ny) * ky - 0.5;
                    if (fx < 0) fx = 0;
                    if (fy < 0) fy = 0;
                    if (fx > I.w - 1) fx = I.w - 1;
                    if (fy > I.h - 1) fy = I.h - 1;
                    const int x0 = (int)fx, y0 = (int)fy;
                    const int x1 = x0 + 1 < I.w ? x0 + 1 : x0, y1 = y0 + 1 < I.h ? y0 + 1 : y0;
                    const float tx = (float)(fx - x0), ty = (float)(fy - y0);
                    const uint32_t c00 = I.px[(size_t)y0 * I.w + x0], c10 = I.px[(size_t)y0 * I.w + x1];
                    const uint32_t c01 = I.px[(size_t)y1 * I.w + x0], c11 = I.px[(size_t)y1 * I.w + x1];
                    const float w00 = (1 - tx) * (1 - ty), w10 = tx * (1 - ty), w01 = (1 - tx) * ty, w11 = tx * ty;
                    a += w00 * (c00 >> 24) + w10 * (c10 >> 24) + w01 * (c01 >> 24) + w11 * (c11 >> 24);
                    r += w00 * ((c00 >> 16) & 255) + w10 * ((c10 >> 16) & 255) + w01 * ((c01 >> 16) & 255) + w11 * ((c11 >> 16) & 255);
                    g += w00 * ((c00 >> 8) & 255) + w10 * ((c10 >> 8) & 255) + w01 * ((c01 >> 8) & 255) + w11 * ((c11 >> 8) & 255);
                    b += w00 * (c00 & 255) + w10 * (c10 & 255) + w01 * (c01 & 255) + w11 * (c11 & 255);
                }
            const float inv = 1.0f / (nx * ny);
            int ia = (int)(a * inv + 0.5f), ir = (int)(r * inv + 0.5f), ig = (int)(g * inv + 0.5f), ib = (int)(b * inv + 0.5f);
            if (ia > 255) ia = 255;
            if (ir > ia) ir = ia;
            if (ig > ia) ig = ia;
            if (ib > ia) ib = ia;
            (*out)[(size_t)y * dw + x] = ((uint32_t)ia << 24) | ((uint32_t)ir << 16) | ((uint32_t)ig << 8) | (uint32_t)ib;
        }
}

struct Piece { int kind, a, w, h; std::vector<uint32_t> px; };
std::vector<Piece> g_pieces;
Piece *find_piece(int kind, int a, int w, int h)
{
    for (Piece &p : g_pieces) if (p.kind == kind && p.a == a && p.w == w && p.h == h) return &p;
    return 0;
}
Piece *new_piece(int kind, int a, int w, int h)
{
    if (g_pieces.size() > 120) g_pieces.clear();
    g_pieces.push_back(Piece());
    Piece &p = g_pieces.back();
    p.kind = kind; p.a = a; p.w = w; p.h = h;
    return &p;
}
void blit(const Surf &s, int x, int y, const Piece &p, int a = 255)
{
    for (int j = 0; j < p.h; ++j)
        for (int i = 0; i < p.w; ++i) put_pm(s, x + i, y + j, p.px[(size_t)j * p.w + i], a);
}

/* the HD UI mod's art */
enum { A_CORK, A_PLANK, A_KEY, A_PARCH, A_BACK, A_COUNT };
const char *const ART_REL[A_COUNT] = { "minimap/SM64DSMapBase.png", "menu/settings/plank.png", "menu/settings/key.png", "menu/settings/parchment.png",
                                        "menu/settings/back.png" };
Img g_art[A_COUNT];
int g_hd = -1;
void decide_skin(void)
{
    if (g_hd >= 0) return;
    g_hd = 0;
    const char *e = getenv("SM64DS_LOBBY_PLAIN");
    if (e && *e && *e != '0') { log_line("skin: plain (SM64DS_LOBBY_PLAIN)"); return; }
    int ok = 1;
    for (int i = 0; i < A_COUNT && ok; ++i) {
        char path[1024];
        if (!port_mods_ui_file(ART_REL[i], path, sizeof path)) { ok = 0; break; }
        load_png(path, &g_art[i]);
        if (!g_art[i].ok()) ok = 0;
    }
    g_hd = ok;
    log_line(ok ? "skin: HD UI Textures (cork, planks)" : "skin: plain (no HD UI Textures mod, or its art is missing)");
}

/* colours of the plain skin: Tango's mock (orange boxes, dark brown edges, a yellow cork) */
const uint32_t C_BOX = 0xD86000u, C_BOXEDGE = 0x6A3208u, C_INK = 0x3A1C06u, C_DIMINK = 0x7A4A20u, C_CREAM = 0xFFF1C9u, C_WOODTXT = 0xFFF3CCu;

void rrect(const Surf &s, int x, int y, int w, int h, float r, uint32_t rgb, int alpha, const Piece *pic = 0)
{
    if (w < 1 || h < 1) return;
    if (r > w * 0.5f) r = w * 0.5f;
    if (r > h * 0.5f) r = h * 0.5f;
    const int r8 = (rgb >> 16) & 255, g8 = (rgb >> 8) & 255, b8 = rgb & 255;
    for (int j = 0; j < h; ++j) {
        const int yy = y + j;
        if (yy < 0 || yy >= s.h) continue;
        for (int i = 0; i < w; ++i) {
            const int xx = x + i;
            if (xx < 0 || xx >= s.w) continue;
            const float px = i + 0.5f, py = j + 0.5f;
            float dx = 0, dy = 0;
            if (px < r) dx = r - px; else if (px > w - r) dx = px - (w - r);
            if (py < r) dy = r - py; else if (py > h - r) dy = py - (h - r);
            float cov = 1.0f;
            if (dx > 0 && dy > 0) {
                cov = r - sqrtf(dx * dx + dy * dy) + 0.5f;
                if (cov <= 0) continue;
                if (cov > 1) cov = 1;
            }
            const int ca = (int)(cov * alpha + 0.5f);
            if (ca <= 0) continue;
            if (pic && i < pic->w && j < pic->h) put_pm(s, xx, yy, pic->px[(size_t)j * pic->w + i], ca);
            else s.px[(size_t)yy * s.stride + xx] = over(s.px[(size_t)yy * s.stride + xx], r8 * ca / 255, g8 * ca / 255, b8 * ca / 255, ca);
        }
    }
}
void framed(const Surf &s, int x, int y, int w, int h, float r, int edge, uint32_t fill, uint32_t edge_rgb, int alpha = 255)
{
    rrect(s, x, y, w, h, r, edge_rgb, alpha);
    rrect(s, x + edge, y + edge, w - 2 * edge, h - 2 * edge, r > edge ? r - edge : 0, fill, alpha);
}

const Piece *nine(int art, int dw, int dh, int il, int it, int ir, int ib, float f)
{
    if (Piece *p = find_piece(1, art, dw, dh)) return p;
    Piece *p = new_piece(1, art, dw, dh);
    p->px.assign((size_t)dw * dh, 0);
    const Img &I = g_art[art];
    int cl = (int)(il * f + 0.5f), ct = (int)(it * f + 0.5f), cr = (int)(ir * f + 0.5f), cb = (int)(ib * f + 0.5f);
    if (cl + cr > dw) { cl = dw / 2; cr = dw - cl; }
    if (ct + cb > dh) { ct = dh / 2; cb = dh - ct; }
    const int xs[4] = { 0, cl, dw - cr, dw }, ys[4] = { 0, ct, dh - cb, dh };
    const double sxs[4] = { 0, (double)il, (double)(I.w - ir), (double)I.w }, sys_[4] = { 0, (double)it, (double)(I.h - ib), (double)I.h };
    std::vector<uint32_t> tmp;
    for (int j = 0; j < 3; ++j)
        for (int i = 0; i < 3; ++i) {
            const int w = xs[i + 1] - xs[i], h = ys[j + 1] - ys[j];
            if (w < 1 || h < 1) continue;
            scale_rect(I, sxs[i], sys_[j], sxs[i + 1] - sxs[i], sys_[j + 1] - sys_[j], w, h, &tmp);
            for (int y = 0; y < h; ++y)
                for (int x = 0; x < w; ++x) p->px[(size_t)(ys[j] + y) * dw + xs[i] + x] = tmp[(size_t)y * w + x];
        }
    return p;
}

/* text: ui_text (the mod's DS lettering, else the port's plain pixel font); em in units */
struct Font {
    float u;
    float width(const char *s, float em) const { return ui_text_width(s, em * u); }
    float cap(float em) const { return ui_text_cap_height(em * u); }
    /* the top of a capital at y */
    void draw(const Surf &s, float x, float y, const char *str, float em, uint32_t rgb, int alpha = 255, const int *clip = 0) const
    {
        UiSurface us = { s.px, s.stride, s.w, s.h };
        ui_text_draw(&us, x, y + ui_text_cap_height(em * u), str, em * u, rgb, alpha, clip);
    }
    void centred(const Surf &s, const Rect &r, const char *str, float em, uint32_t rgb, int alpha = 255) const
    {
        draw(s, r.x + (r.w - width(str, em)) * 0.5f, r.y + (r.h - cap(em)) * 0.5f, str, em, rgb, alpha);
    }
};
std::string fit(const Font &f, const char *s, float em, float w)
{
    std::string r = s;
    if (f.width(r.c_str(), em) <= w) return r;
    while (r.size() > 1) {
        r.erase(r.size() - 1);
        const std::string t = r + "..";
        if (f.width(t.c_str(), em) <= w) return t;
    }
    return r;
}
void wrap(const Font &f, const char *s, float em, float w, std::vector<std::string> *out)
{
    out->clear();
    std::string cur, word, text = s;
    text += ' ';
    for (size_t i = 0; i < text.size();) {
        word.clear();
        while (i < text.size() && text[i] != ' ') word += text[i++];
        ++i;
        if (word.empty()) continue;
        const std::string t = cur.empty() ? word : cur + " " + word;
        if (f.width(t.c_str(), em) <= w) { cur = t; continue; }
        if (!cur.empty()) out->push_back(cur);
        cur = fit(f, word.c_str(), em, w);
    }
    if (!cur.empty()) out->push_back(cur);
}

/* the cork: the HD UI minimap's base tile (it repeats every 32 pixels), a sixth of the picture height a tile; the plain skin's own
   speckled yellow cork */
void draw_cork(const Surf &s, bool hd)
{
    if (hd) {
        int t = s.h / 10;
        if (t < 16) t = 16;
        Piece *p = find_piece(5, 0, t, t);
        if (!p) {
            p = new_piece(5, 0, t, t);
            scale_rect(g_art[A_CORK], 32, 32, 32, 32, t, t, &p->px);
        }
        for (int y = 0; y < s.h; ++y) {
            const uint32_t *src = &p->px[(size_t)(y % t) * t];
            uint32_t *d = &s.px[(size_t)y * s.stride];
            for (int x = 0; x < s.w; ++x) d[x] = 0xFF000000u | src[x % t];
        }
        return;
    }
    const int k = s.h / 192 > 1 ? s.h / 192 : 1;      /* a speck is one DS pixel */
    for (int y = 0; y < s.h; ++y) {
        uint32_t *d = &s.px[(size_t)y * s.stride];
        const unsigned yy = (unsigned)(y / k);
        for (int x = 0; x < s.w; ++x) {
            unsigned hsh = (unsigned)(x / k) * 374761393u + yy * 668265263u;
            hsh = (hsh ^ (hsh >> 13)) * 1274126177u;
            hsh ^= hsh >> 16;
            static const uint32_t kSpeck[8] = { 0xE8C878, 0xE0BC66, 0xF0D48A, 0xD8B060, 0xE9CC80, 0xC9A458, 0xF4DC98, 0xDDB86C };
            d[x] = 0xFF000000u | kSpeck[hsh & 7];
        }
    }
}

/* the inner panel of the cream plaque (kind 1): on the HD art (menu/settings/key.png, 300 x 171) the cream face runs from x 54 to 246 and
   y 44 to 129, scaled as draw_box scales the nine-slice; the plain skin's box has a 3 unit edge */
Rect key_inner(const Rect &r, float u, bool hd)
{
    if (!hd) { const int e = (int)(3 * u) + 1; const Rect q = { r.x + e, r.y + e, r.w - 2 * e, r.h - 2 * e }; return q; }
    const int kh = (int)(46 * u);
    const float k = (float)(r.h < kh ? r.h : kh) / 171.0f;
    const int l = (int)(54 * k + 0.5f), t = (int)(44 * k + 0.5f), b = (int)(42 * k + 0.5f);
    const Rect q = { r.x + l, r.y + t, r.w - 2 * l, r.h - t - b };
    return q;
}

void rim(const Surf &s, float u, const Rect &r);
/* a box: kind 0 = a wooden plank (a button), 1 = the cream plaque (a field to type in / read), 2 = the parchment sheet (a panel) */
void draw_box(const Surf &s, const Font &f, bool hd, const Rect &r, int kind, int lit, int dim = 0)
{
    const int a = dim ? 140 : 255;
    if (hd) {
        if (kind == 0) { if (const Piece *p = nine(A_PLANK, r.w, r.h, 18, 18, 18, 18, (float)r.h / 150.0f)) blit(s, r.x, r.y, *p, a); }
        else if (kind == 1) { const int kh = (int)(46 * f.u); const float k = (float)(r.h < kh ? r.h : kh) / 171.0f; if (const Piece *p = nine(A_KEY, r.w, r.h, 70, 55, 70, 55, k)) blit(s, r.x, r.y, *p, a); }
        else {
            const int edge = f.u * 3 < 2 ? 2 : (int)(f.u * 3);
            Piece *p = find_piece(3, 0, r.w - 2 * edge, r.h - 2 * edge);
            if (!p) {
                p = new_piece(3, 0, r.w - 2 * edge, r.h - 2 * edge);
                const Img &I = g_art[A_PARCH];
                const double ar = (double)(r.w - 2 * edge) / (r.h - 2 * edge);
                double cw = I.w, ch = I.w / ar;
                if (ch > I.h) { ch = I.h; cw = I.h * ar; }
                scale_rect(I, (I.w - cw) / 2, (I.h - ch) / 2, cw, ch, r.w - 2 * edge, r.h - 2 * edge, &p->px);
            }
            rrect(s, r.x + (int)(2 * f.u), r.y + (int)(3 * f.u), r.w, r.h, 12 * f.u, 0, 50);
            rrect(s, r.x, r.y, r.w, r.h, 12 * f.u, 0x7A4614u, 255);
            rrect(s, r.x + edge, r.y + edge, r.w - 2 * edge, r.h - 2 * edge, 12 * f.u - edge, 0, 255, p);
        }
    } else {
        const int edge = f.u * 3 < 2 ? 2 : (int)(f.u * 3);
        const uint32_t fill = kind == 1 ? 0xE87A1Cu : kind == 2 ? 0xE07020u : C_BOX;
        framed(s, r.x, r.y, r.w, r.h, 6 * f.u, edge, fill, C_BOXEDGE, a);
    }
    if (lit) rim(s, f.u, r);
}

/* the focus: a bright rim inside a box's edge */
void rim(const Surf &s, float u, const Rect &r)
{
    {
        const int e = (int)(2 * u) > 1 ? (int)(2 * u) : 1;
        struct { float u; } f = { u };
        for (int i = 0; i < e; ++i) {
            const Rect q = { r.x + (int)(3 * f.u) + i, r.y + (int)(3 * f.u) + i, r.w - 2 * ((int)(3 * f.u) + i), r.h - 2 * ((int)(3 * f.u) + i) };
            for (int x = q.x; x < q.x + q.w; ++x) { put_pm(s, x, q.y, 0xFFFFFFFFu, 200); put_pm(s, x, q.y + q.h - 1, 0xFFFFFFFFu, 200); }
            for (int y = q.y; y < q.y + q.h; ++y) { put_pm(s, q.x, y, 0xFFFFFFFFu, 200); put_pm(s, q.x + q.w - 1, y, 0xFFFFFFFFu, 200); }
        }
    }
}

/* text colour on a box of that kind */
uint32_t ink_on(bool hd, int kind) { return hd ? (kind == 0 ? 0x5A2A0Au : C_INK) : (kind == 0 ? C_WOODTXT : C_INK); }

/* the little icons: the eye (open / shut), two squares (copy), a speaker (mute), a cross (kick), a ring with a bar (ban), a star (host) */
void line(const Surf &s, float x0, float y0, float x1, float y1, float wdt, uint32_t rgb)
{
    const int minx = (int)floorf((x0 < x1 ? x0 : x1) - wdt - 1), maxx = (int)ceilf((x0 > x1 ? x0 : x1) + wdt + 1);
    const int miny = (int)floorf((y0 < y1 ? y0 : y1) - wdt - 1), maxy = (int)ceilf((y0 > y1 ? y0 : y1) + wdt + 1);
    const float dx = x1 - x0, dy = y1 - y0, L2 = dx * dx + dy * dy;
    for (int y = miny; y <= maxy; ++y)
        for (int x = minx; x <= maxx; ++x) {
            float t = L2 > 0 ? ((x + 0.5f - x0) * dx + (y + 0.5f - y0) * dy) / L2 : 0;
            t = t < 0 ? 0 : t > 1 ? 1 : t;
            const float px = x0 + t * dx - (x + 0.5f), py = y0 + t * dy - (y + 0.5f);
            const float d = sqrtf(px * px + py * py);
            float c = wdt * 0.5f - d + 0.5f;
            if (c <= 0) continue;
            if (c > 1) c = 1;
            put_pm(s, x, y, 0xFF000000u | rgb, (int)(c * 255));
        }
}
void ring(const Surf &s, float cx, float cy, float rx, float ry, float wdt, uint32_t rgb, int fill)
{
    const int x0 = (int)floorf(cx - rx - wdt), x1 = (int)ceilf(cx + rx + wdt), y0 = (int)floorf(cy - ry - wdt), y1 = (int)ceilf(cy + ry + wdt);
    for (int y = y0; y <= y1; ++y)
        for (int x = x0; x <= x1; ++x) {
            const float nx = (x + 0.5f - cx) / rx, ny = (y + 0.5f - cy) / ry;
            const float d = (sqrtf(nx * nx + ny * ny) - 1.0f) * (rx < ry ? rx : ry);
            float c = fill ? (0.5f - d) : (wdt * 0.5f - fabsf(d) + 0.5f);
            if (c <= 0) continue;
            if (c > 1) c = 1;
            put_pm(s, x, y, 0xFF000000u | rgb, (int)(c * 255));
        }
}
void icon_eye(const Surf &s, const Rect &r, int shut, uint32_t rgb, float u)
{
    const float cx = r.x + r.w * 0.5f, cy = r.y + r.h * 0.5f, rx = r.w * 0.32f, ry = r.h * 0.17f, wd = 2.0f * u > 1.2f ? 2.0f * u : 1.2f;
    if (shut) {
        for (int i = 0; i <= 16; ++i) {
            const float a0 = 3.14159f * i / 16, a1 = 3.14159f * (i + 1) / 16;
            line(s, cx - cosf(a0) * rx, cy + sinf(a0) * ry * 0.6f, cx - cosf(a1) * rx, cy + sinf(a1) * ry * 0.6f, wd, rgb);
        }
        for (int i = -1; i <= 1; ++i) line(s, cx + i * rx * 0.55f, cy + ry * 0.55f, cx + i * rx * 0.75f, cy + ry * 1.5f, wd, rgb);
        return;
    }
    ring(s, cx, cy, rx, ry, wd, rgb, 0);
    ring(s, cx, cy, ry * 0.65f, ry * 0.65f, wd, rgb, 1);
}
void icon_copy(const Surf &s, const Rect &r, uint32_t rgb, float u)
{
    const float wd = 2.0f * u > 1.2f ? 2.0f * u : 1.2f, k = r.w * 0.22f;
    const float x0 = r.x + r.w * 0.30f, y0 = r.y + r.h * 0.24f;
    auto sq = [&](float x, float y) { line(s, x, y, x + 1.6f * k, y, wd, rgb); line(s, x + 1.6f * k, y, x + 1.6f * k, y + 1.6f * k, wd, rgb);
                                      line(s, x + 1.6f * k, y + 1.6f * k, x, y + 1.6f * k, wd, rgb); line(s, x, y + 1.6f * k, x, y, wd, rgb); };
    sq(x0 + 0.55f * k, y0);
    sq(x0 - 0.15f * k, y0 + 0.7f * k);
}
void icon_speaker(const Surf &s, const Rect &r, int muted, uint32_t rgb, float u)
{
    const float wd = 1.6f * u > 1.0f ? 1.6f * u : 1.0f;
    const float x = r.x + r.w * 0.2f, cy = r.y + r.h * 0.5f, h = r.h * 0.18f;
    for (float yy = -h; yy <= h; yy += 0.5f) line(s, x, cy + yy, x + r.w * 0.16f, cy + yy, 1.0f, rgb);
    line(s, x + r.w * 0.16f, cy - h, x + r.w * 0.40f, cy - h * 2.0f, wd, rgb);
    line(s, x + r.w * 0.16f, cy + h, x + r.w * 0.40f, cy + h * 2.0f, wd, rgb);
    line(s, x + r.w * 0.40f, cy - h * 2.0f, x + r.w * 0.40f, cy + h * 2.0f, wd, rgb);
    if (muted) { line(s, x + r.w * 0.50f, cy - h, x + r.w * 0.74f, cy + h, wd, 0xB01010); line(s, x + r.w * 0.74f, cy - h, x + r.w * 0.50f, cy + h, wd, 0xB01010); }
    else { line(s, x + r.w * 0.52f, cy - h * 0.8f, x + r.w * 0.58f, cy, wd, rgb); line(s, x + r.w * 0.58f, cy, x + r.w * 0.52f, cy + h * 0.8f, wd, rgb); }
}
void icon_cross(const Surf &s, const Rect &r, uint32_t rgb, float u)
{
    const float wd = 2.0f * u > 1.2f ? 2.0f * u : 1.2f, m = r.w * 0.28f;
    line(s, r.x + m, r.y + m, r.x + r.w - m, r.y + r.h - m, wd, rgb);
    line(s, r.x + r.w - m, r.y + m, r.x + m, r.y + r.h - m, wd, rgb);
}
/* the ban sign: a ring with one bar across it */
void icon_ban(const Surf &s, const Rect &r, uint32_t rgb, float u)
{
    const float wd = 1.6f * u > 1.2f ? 1.6f * u : 1.2f, cx = r.x + r.w * 0.5f, cy = r.y + r.h * 0.5f, R = r.w * 0.30f;
    for (int i = 0; i < 20; ++i) {
        const float a0 = i * 0.31415927f, a1 = (i + 1) * 0.31415927f;
        line(s, cx + R * cosf(a0), cy + R * sinf(a0), cx + R * cosf(a1), cy + R * sinf(a1), wd, rgb);
    }
    line(s, cx - R * 0.70710678f, cy - R * 0.70710678f, cx + R * 0.70710678f, cy + R * 0.70710678f, wd, rgb);
}
void icon_star(const Surf &s, float cx, float cy, float R, uint32_t rgb)
{
    float vx[10], vy[10];
    for (int i = 0; i < 10; ++i) {
        const float a = -1.5707963f + i * 0.6283185f, rr = (i & 1) ? R * 0.45f : R;
        vx[i] = cx + cosf(a) * rr; vy[i] = cy + sinf(a) * rr;
    }
    for (int y = (int)(cy - R - 1); y <= (int)(cy + R + 1); ++y)
        for (int x = (int)(cx - R - 1); x <= (int)(cx + R + 1); ++x) {
            int hit = 0;
            for (int sub = 0; sub < 4; ++sub) {
                const float px = x + 0.25f + (sub & 1) * 0.5f, py = y + 0.25f + (sub >> 1) * 0.5f;
                bool in = false;
                for (int i = 0, j = 9; i < 10; j = i++)
                    if (((vy[i] > py) != (vy[j] > py)) && (px < (vx[j] - vx[i]) * (py - vy[i]) / (vy[j] - vy[i]) + vx[i])) in = !in;
                hit += in;
            }
            if (hit) put_pm(s, x, y, 0xFF000000u | rgb, hit * 255 / 4);
        }
}
void icon_check(const Surf &s, float x, float y, float k, uint32_t rgb, float u)
{
    const float wd = 2.4f * u > 1.4f ? 2.4f * u : 1.4f;
    line(s, x, y + k * 0.5f, x + k * 0.38f, y + k * 0.88f, wd, rgb);
    line(s, x + k * 0.38f, y + k * 0.88f, x + k, y, wd, rgb);
}
/* the back arrow: a curled arrow pointing left */
void icon_back(const Surf &s, const Rect &r, uint32_t rgb, float u)
{
    const float wd = 3.0f * u > 1.6f ? 3.0f * u : 1.6f;
    const float cx = r.x + r.w * 0.56f, cy = r.y + r.h * 0.52f, R = r.h * 0.24f;
    for (int i = 0; i < 14; ++i) {
        const float a0 = -1.5707963f + 3.14159f * i / 14, a1 = -1.5707963f + 3.14159f * (i + 1) / 14;
        line(s, cx + cosf(a0) * R, cy + sinf(a0) * R, cx + cosf(a1) * R, cy + sinf(a1) * R, wd, rgb);
    }
    line(s, cx, cy - R, cx - R * 1.2f, cy - R, wd, rgb);
    line(s, cx - R * 1.2f, cy - R, cx - R * 0.6f, cy - R * 1.6f, wd, rgb);
    line(s, cx - R * 1.2f, cy - R, cx - R * 0.6f, cy - R * 0.4f, wd, rgb);
    line(s, cx, cy + R, cx - R * 0.8f, cy + R, wd, rgb);
}

uint32_t contrast_ink(uint32_t rgb)
{
    const int r = (rgb >> 16) & 255, g = (rgb >> 8) & 255, b = rgb & 255;
    return (r * 299 + g * 587 + b * 114) / 1000 > 140 ? 0x1A1006u : 0xFFFFFFu;
}

int chat_muted(const LobbyState *s, const char *line)
{
    for (int i = 0; i < seat_count(s); ++i) {
        if (!s->seats[i].muted) continue;
        const size_t n = strlen(s->seats[i].name);
        if (n && !strncmp(line, s->seats[i].name, n) && line[n] == ':') return 1;
    }
    return 0;
}

const char *setting_value(const LobbySetting &t, char *buf, size_t cap)
{
    if (t.choices && t.value >= 0 && t.value < t.n_choices) return t.choices[t.value];
    snprintf(buf, cap, "%d", t.value);
    return buf;
}

void draw_title(const Surf &s, const Font &f, const Geom &g, const char *word)
{
    const float em = g.W >= 600 ? 46.0f : 38.0f;
    const float tw = f.width(word, em);
    const float x = g.title.x + g.title.w * 0.5f - tw * 0.5f, y = 12 * g.u;      /* g.title: the middle, or right of the code group on a narrow window */
    f.draw(s, x + 2 * g.u, y + 2 * g.u, word, em, 0x7A4614u, 150);
    f.draw(s, x, y, word, em, 0x2A1404u);
}

void draw_back(const Surf &s, const Font &f, bool hd, const Geom &g, int lit)
{
    const Rect &r = g.r[C_BACK];
    draw_box(s, f, hd, r, 0, lit);
    icon_back(s, r, hd ? 0xC0500Cu : C_WOODTXT, g.u);
}

void draw_room(const Surf &s, const Font &f, bool hd, const Geom &g, const LobbyState *st)
{
    const float u = g.u;
    char buf[160];
    const int focus = P.focus;
    auto lit = [&](int c) { return focus == c && live(st, c) ? 1 : 0; };
    /* the code group */
    {
        const Rect &r = g.r[C_CODE];
        draw_box(s, f, hd, r, 1, 0);
        const char *shown = P.code_hidden ? "******" : st->code;
        const Rect in = key_inner(r, u, hd);
        float em = 17.0f, lem = 11.0f;
        const float gap = 6 * u, pad = 4 * u;
        float tw = f.width("CODE", lem) + gap + f.width(shown, em);
        if (tw > in.w - 2 * pad) { const float k = (in.w - 2 * pad) / tw; em *= k; lem *= k; tw = f.width("CODE", lem) + gap + f.width(shown, em); }
        const float x0 = in.x + (in.w - tw) * 0.5f, cy = in.y + in.h * 0.5f;
        f.draw(s, x0, cy - f.cap(lem) * 0.5f, "CODE", lem, C_DIMINK);
        f.draw(s, x0 + f.width("CODE", lem) + gap, cy - f.cap(em) * 0.5f, shown, em, C_INK);
        draw_box(s, f, hd, g.r[C_EYE], 0, lit(C_EYE));
        icon_eye(s, g.r[C_EYE], P.code_hidden, hd ? 0x5A2A0Au : C_WOODTXT, u);
        draw_box(s, f, hd, g.r[C_COPY], 0, lit(C_COPY));
        icon_copy(s, g.r[C_COPY], hd ? 0x5A2A0Au : C_WOODTXT, u);
        if (P.copied_t > 0) {
            const Rect &c = g.r[C_COPY];
            f.draw(s, c.x + c.w + 6 * u, c.y + (c.h - f.cap(13.0f)) * 0.5f, "Copied", 13.0f, 0x2A1404u, P.copied_t > 20 ? 255 : P.copied_t * 12);
        }
    }
    /* chat */
    {
        const Rect &r = g.r[C_CHAT];
        draw_box(s, f, hd, r, 1, lit(C_CHAT) || P.field == C_CHAT);
        const Rect in = key_inner(r, u, hd);
        const int pad = (int)(6 * u);
        const float em = 14.0f, lh = 18.0f * u;
        const int input_h = (int)(lh + 6 * u);      /* the input line and the divider over it */
        const Rect L = { in.x + pad, in.y + pad, in.w - 2 * pad, in.h - 2 * pad - input_h };
        std::vector<std::string> lines;
        for (int i = 0; i < st->n_chat && i < 32; ++i) {
            if (chat_muted(st, st->chat[i])) continue;
            std::vector<std::string> w;
            wrap(f, st->chat[i], em, (float)L.w, &w);
            for (auto &x : w) lines.push_back(x);
        }
        const int fit_n = (int)(L.h / lh);
        const int first = (int)lines.size() > fit_n ? (int)lines.size() - fit_n : 0;
        const int clip[4] = { L.x, L.y, L.x + L.w, L.y + L.h };
        for (int i = first; i < (int)lines.size(); ++i)
            f.draw(s, (float)L.x, L.y + (i - first) * lh, lines[i].c_str(), em, C_INK, 255, clip);
        /* the line being typed, or "type to chat": the last line of the inner panel, under a divider as wide as the text */
        const float iy = (float)(in.y + in.h - pad) - f.cap(em) - 2 * u;
        for (int x = L.x; x < L.x + L.w; ++x) put_pm(s, x, (int)(iy - 4 * u), 0xFF000000u | C_DIMINK, 120);
        if (P.field == C_CHAT) {
            std::string t = P.chat_in;
            while (t.size() > 1 && f.width(t.c_str(), em) > L.w - 10 * u) t.erase(0, 1);
            if ((P.frames / 20) & 1) t += "_";
            f.draw(s, (float)L.x, iy, t.c_str(), em, C_INK);
        } else {
            f.draw(s, (float)L.x, iy, "type to chat", em, C_DIMINK);
        }
    }
    /* the right column */
    {
        const Rect &r = g.r[C_NAME];
        draw_box(s, f, hd, r, 1, lit(C_NAME) || P.field == C_NAME);
        const float em = 14.0f;
        f.draw(s, r.x + 12 * u, r.y + (r.h - f.cap(em)) * 0.5f, "Server name:", em, C_DIMINK);
        const float x = r.x + 12 * u + f.width("Server name:", em) + 8 * u;
        std::string v = P.field == C_NAME ? std::string(P.name_in) + (((P.frames / 20) & 1) ? "_" : "") : std::string(st->server_name);
        v = fit(f, v.c_str(), em, r.x + r.w - 12 * u - x);
        f.draw(s, x, r.y + (r.h - f.cap(em)) * 0.5f, v.c_str(), em, C_INK);
    }
    /* the right column's planks share ONE text size: the largest (14 down to 10) at which every label of the group fits its plank */
    char save_txt[32];
    snprintf(save_txt, sizeof save_txt, "Save file: %s", st->coop_save_continue ? "Continue" : "New");
    float plank_em = 14.0f;
    {
        const struct { int c; const char *t; } grp[4] = { { C_SETTINGS, st->kind == LOBBY_COOP ? "Room rules" : "Game settings" },
                                                          { C_SAVE, st->kind == LOBBY_COOP ? save_txt : 0 },
                                                          { C_MODS, st->kind == LOBBY_COOP ? "Mods: Soon" : 0 },
                                                          { C_PRIVACY, "Privacy: Code" } };
        for (const auto &e : grp) {
            if (!e.t || g.r[e.c].w <= 0) continue;
            while (plank_em > 10.0f && f.width(e.t, plank_em) > g.r[e.c].w - 16 * u) plank_em -= 0.5f;
        }
    }
    auto label_box = [&](int c, const char *text, int dim, int kind) {
        const Rect &r = g.r[c];
        if (r.w <= 0) return;
        draw_box(s, f, hd, r, kind, lit(c), dim);
        float em = plank_em;
        if (kind != 0) { em = 14.0f; while (em > 10.0f && f.width(text, em) > r.w - 16 * u) em -= 0.5f; }   /* the VS match line is its own plaque */
        const std::string t = fit(f, text, em, r.w - 16 * u);
        f.centred(s, r, t.c_str(), em, ink_on(hd, kind), dim ? 150 : 255);
    };
    label_box(C_SETTINGS, st->kind == LOBBY_COOP ? "Room rules" : "Game settings", 0, 0);
    if (st->kind == LOBBY_COOP) {
        snprintf(buf, sizeof buf, "Save file: %s", st->coop_save_continue ? "Continue" : "New");
        label_box(C_SAVE, buf, !st->am_host, 0);
        label_box(C_MODS, "Mods: Soon", 1, 0);
    } else {
        /* the match line: mode, arena, players (from the settings by their keys) */
        const char *mode = "?";
        int arena = 0, players = 0, stars = 0;
        char tmp[16];
        for (int i = 0; i < st->n_settings; ++i) {
            const LobbySetting &t = st->settings[i];
            if (!t.key) continue;
            if (!strcmp(t.key, "mode")) mode = setting_value(t, tmp, sizeof tmp);
            else if (!strcmp(t.key, "arena")) arena = t.value;
            else if (!strcmp(t.key, "players")) players = t.value;
            else if (!strcmp(t.key, "stars")) stars = t.value;
        }
        if (!strcmp(mode, "First to stars")) snprintf(buf, sizeof buf, "First to %d stars  |  Arena %d  |  %d players", stars, arena, players);
        else snprintf(buf, sizeof buf, "%s  |  Arena %d  |  %d players", mode, arena, players);
        label_box(C_MODS, buf, 0, 1);
    }
    label_box(C_PRIVACY, "Privacy: Code", 0, 0);
    /* READY / START */
    {
        const Rect &r = g.r[C_READY];
        const int me = my_seat(st);
        const int ready = me >= 0 && st->seats[me].ready;
        const int can_start = st->am_host && all_ready(st);
        const char *word = st->phase == LOBBY_STARTING ? "STARTING..." : can_start ? "START" : ready ? "READY!" : "READY";
        draw_box(s, f, hd, r, 0, lit(C_READY));
        if (ready && !can_start && st->phase != LOBBY_STARTING) rrect(s, r.x + (int)(6 * u), r.y + (int)(6 * u), r.w - (int)(12 * u), r.h - (int)(12 * u), 8 * u, 0x40C040u, 70);
        if (can_start && st->phase != LOBBY_STARTING) rrect(s, r.x + (int)(6 * u), r.y + (int)(6 * u), r.w - (int)(12 * u), r.h - (int)(12 * u), 8 * u, 0xFFE040u, 80);
        const float em = 30.0f, sem = 12.0f;
        int nr = 0;
        for (int i = 0; i < seat_count(st); ++i) nr += st->seats[i].ready != 0;
        buf[0] = 0;
        if (st->phase == LOBBY_IN_ROOM) {      /* under the word, what it is waiting for */
            if (can_start) snprintf(buf, sizeof buf, "Everyone is ready");
            else if (st->am_host) snprintf(buf, sizeof buf, "%d of %d ready", nr, seat_count(st));
            else snprintf(buf, sizeof buf, "%d of %d ready, the host starts", nr, seat_count(st));
        }
        /* the face: inside the plank's bevel (the HD plank's 18 px border at r.h / 150, the plain box's 3 unit edge) */
        const int bev = hd ? (int)(18.0f * r.h / 150.0f) : (int)(3 * u);
        const Rect face = { r.x + bev, r.y + bev, r.w - 2 * bev, r.h - 2 * bev };
        const float gap = 6 * u;
        const float total = f.cap(em) + (buf[0] ? gap + f.cap(sem) : 0.f);
        const float y0 = face.y + (face.h - total) * 0.5f;
        f.draw(s, face.x + (face.w - f.width(word, em)) * 0.5f, y0, word, em, ink_on(hd, 0));
        if (ready && !can_start && st->phase != LOBBY_STARTING) icon_check(s, r.x + r.w * 0.5f + f.width(word, em) * 0.5f + 10 * u, y0 + f.cap(em) * 0.5f - 9 * u, 18 * u, 0x1E7A1Eu, u);
        if (buf[0]) {
            const std::string t = fit(f, buf, sem, face.w - 8 * u);
            f.draw(s, face.x + (face.w - f.width(t.c_str(), sem)) * 0.5f, y0 + f.cap(em) + gap, t.c_str(), sem, ink_on(hd, 0), 200);
        }
    }
    /* the seats */
    draw_box(s, f, hd, g.seats_box, 2, 0);
    /* ONE name size and ONE status size for the whole row: the largest that fits every name (down to a readable 8, cut only below that)
       and "Not ready", the wider status, in every card */
    float row_em = 15.0f, row_sem = 11.0f;
    if (seat_count(st) > 0) {
        const Rect &r0 = g.r[C_SEAT0];
        const int edge0 = (int)(3 * u) > 2 ? (int)(3 * u) : 2;
        row_em = r0.h > 60 * u ? 15.0f : 11.0f;
        row_sem = r0.h > 60 * u ? 11.0f : 8.5f;
        for (int i = 0; i < seat_count(st); ++i)
            while (row_em > 8.0f && f.width(st->seats[i].name, row_em) > g.r[C_SEAT0 + i].w - 12 * u) row_em -= 0.5f;
        while (row_sem > 7.0f && f.width("Not ready", row_sem) > r0.w - 2 * edge0 - 12 * u) row_sem -= 0.5f;
    }
    for (int i = 0; i < g.n_seat_slots; ++i) {
        const Rect &r = g.r[C_SEAT0 + i];
        if (i >= seat_count(st)) {
            rrect(s, r.x, r.y, r.w, r.h, 8 * u, 0x7A4614u, 50);
            const char *open = "open";
            f.centred(s, r, open, 12.0f, 0x7A4614u, 140);
            continue;
        }
        const LobbySeat &t = st->seats[i];
        const int sel = lit(C_SEAT0 + i) || P.seat_menu == i;
        const int edge = (int)(3 * u) > 2 ? (int)(3 * u) : 2;
        framed(s, r.x, r.y, r.w, r.h, 8 * u, edge, t.body_rgb & 0xFFFFFF, t.shoe_rgb & 0xFFFFFF);
        rrect(s, r.x + edge, r.y + r.h - edge - (int)(r.h * 0.26f), r.w - 2 * edge, (int)(r.h * 0.26f), 4 * u, t.shoe_rgb & 0xFFFFFF, 255);
        if (sel) rim(s, u, r);
        const uint32_t ink = contrast_ink(t.body_rgb);
        const float em = row_em;
        const float name_w = r.w - 12 * u;
        std::string nm = fit(f, t.name, em, name_w);
        const float ny = r.y + r.h * 0.42f - f.cap(em) * 0.5f;
        f.draw(s, r.x + (r.w - f.width(nm.c_str(), em)) * 0.5f, ny, nm.c_str(), em, ink);
        const char *status = t.ready ? "Ready" : "Not ready";
        const float sem = row_sem;
        const uint32_t sink = contrast_ink(t.shoe_rgb);
        const Rect band = { r.x, r.y + r.h - edge - (int)(r.h * 0.26f), r.w, (int)(r.h * 0.26f) };
        f.centred(s, band, status, sem, sink, t.ready ? 255 : 190);
        if (t.is_host) icon_star(s, r.x + r.w - 10 * u, r.y + 10 * u, 7 * u, 0xFFD23Cu);
        if (t.is_me) f.draw(s, r.x + 6 * u, r.y + 5 * u, "you", 10.0f, ink, 200);
        if (t.muted) { const Rect q = { r.x + r.w - (int)(40 * u), r.y + (int)(2 * u), (int)(18 * u), (int)(18 * u) }; icon_speaker(s, q, 1, ink, u); }
        if (P.seat_menu == i && !t.is_me && !P.panel) {
            const Rect &m = g.r[C_MUTE0 + i];
            framed(s, m.x, m.y, m.w, m.h, 3 * u, (int)u > 1 ? (int)u : 1, lit(C_MUTE0 + i) ? 0xFFF6D8u : 0x5A3010u, 0x1A0C02u);
            icon_speaker(s, m, t.muted, lit(C_MUTE0 + i) ? 0x3A1C06u : 0xFFF3CCu, u);
            if (st->am_host) {
                const Rect &k = g.r[C_KICK0 + i];
                framed(s, k.x, k.y, k.w, k.h, 3 * u, (int)u > 1 ? (int)u : 1, lit(C_KICK0 + i) ? 0xFFF6D8u : 0x5A3010u, 0x1A0C02u);
                icon_cross(s, k, lit(C_KICK0 + i) ? 0xB01010u : 0xFFF3CCu, u);
                const Rect &bn = g.r[C_BAN0 + i];
                const int armed = P.ban_arm == i;
                const int e1 = (int)u > 1 ? (int)u : 1;
                /* armed: a white ring outside the dark edge, so the red button stands off any player colour (a red plate too) */
                if (armed) rrect(s, bn.x - 2 * e1, bn.y - 2 * e1, bn.w + 4 * e1, bn.h + 4 * e1, 3 * u + 2 * e1, 0xFFFFFFu, 255);
                framed(s, bn.x, bn.y, bn.w, bn.h, 3 * u, armed ? 2 * e1 : e1, armed ? 0xB01010u : lit(C_BAN0 + i) ? 0xFFF6D8u : 0x5A3010u,
                       0x1A0C02u);
                icon_ban(s, bn, armed ? 0xFFF3CCu : lit(C_BAN0 + i) ? 0xB01010u : 0xFFF3CCu, u);
            }
            const char *tip = P.ban_arm == i ? (f.width("Press again to ban", 10.0f) <= r.w - 2 * edge - 6 * u ? "Press again to ban" : "Again to ban") :
                              lit(C_BAN0 + i) ? "Ban" : lit(C_KICK0 + i) ? "Kick" : lit(C_MUTE0 + i) ? (t.muted ? "Unmute" : "Mute") : 0;
            if (tip) {
                /* the tip sits right of the last button when it fits on the plate; otherwise (narrow plates, the armed BAN's question)
                   on a dark strip over the plate's Ready band */
                const float tx = (float)(st->am_host ? g.r[C_BAN0 + i].x + g.r[C_BAN0 + i].w : m.x + m.w) + 4 * u;
                if (P.ban_arm != i && tx + f.width(tip, 11.0f) <= r.x + r.w - 2 * u) {
                    f.draw(s, tx, m.y + (m.h - f.cap(11.0f)) * 0.5f, tip, 11.0f, ink);
                } else {
                    const int bh = (int)(r.h * 0.26f);      /* over the plate's status band, so the name stays in view */
                    const Rect band = { r.x + edge, r.y + r.h - edge - bh, r.w - 2 * edge, bh };
                    rrect(s, band.x, band.y, band.w, band.h, 3 * u, 0x1A0C02u, 215);
                    const std::string a = fit(f, tip, 10.0f, band.w - 6 * u);
                    f.centred(s, band, a.c_str(), 10.0f, 0xFFF3CCu);
                }
            }
        }
    }
    /* the settings panel over everything */
    if (P.panel) {
        for (int y = 0; y < s.h; ++y) for (int x = 0; x < s.w; ++x) put_pm(s, x, y, 0xFF000000u, 90);
        draw_box(s, f, hd, g.panel, 2, 0);
        const char *head = st->kind == LOBBY_COOP ? "Room rules" : "Game settings";
        const Rect hr = { g.panel.x, g.panel.y + (int)(10 * u), g.panel.w, (int)(30 * u) };
        f.centred(s, hr, head, 22.0f, C_INK);
        for (int i = 0; i < st->n_settings && i < 16; ++i) {
            const Rect &r = g.r[C_SET_ROW0 + i];
            const LobbySetting &t = st->settings[i];
            const int on = lit(C_SET_ROW0 + i);
            if (on) rrect(s, r.x, r.y, r.w, r.h, 6 * u, 0xFFFFFFu, 90);
            f.draw(s, r.x + 8 * u, r.y + (r.h - f.cap(15.0f)) * 0.5f, t.label ? t.label : "?", 15.0f, C_INK);
            char vb[16];
            const char *v = setting_value(t, vb, sizeof vb);
            const float vw = f.width(v, 15.0f);
            const float vx = r.x + r.w - 26 * u - vw;
            f.draw(s, vx, r.y + (r.h - f.cap(15.0f)) * 0.5f, v, 15.0f, st->am_host ? 0x8A2A00u : C_DIMINK);
            if (st->am_host && on) {
                f.draw(s, vx - 16 * u, r.y + (r.h - f.cap(15.0f)) * 0.5f, "<", 15.0f, 0x8A2A00u);
                f.draw(s, r.x + r.w - 18 * u, r.y + (r.h - f.cap(15.0f)) * 0.5f, ">", 15.0f, 0x8A2A00u);
            }
        }
        if (!st->am_host) {
            const char *note = "Only the host changes these";
            f.draw(s, g.panel.x + (g.panel.w - f.width(note, 12.0f)) * 0.5f, g.r[C_SET_BACK].y - 18 * u, note, 12.0f, C_DIMINK);
        }
        draw_box(s, f, hd, g.r[C_SET_BACK], 0, lit(C_SET_BACK));
        f.centred(s, g.r[C_SET_BACK], "Back", 16.0f, ink_on(hd, 0));
    }
}

}  // namespace

extern "C" void lobby_page_draw(uint32_t *px, int stride, int w, int h)
{
    if (P.screen == SCR_CLOSED || !px || w < 64 || h < 48) return;
    P.pw = w; P.ph = h;
    decide_skin();
    const bool hd = g_hd != 0;
    const Surf s = { px, stride, w, h };
    const LobbyState *st = C_state();
    Geom g;
    make_geom(&g, w, h, st);
    Font f;
    f.u = g.u;
    draw_cork(s, hd);
    draw_title(s, f, g, "LOBBY");
    const int focus = P.focus;
    if (P.screen == SCR_CHOOSE) {
        draw_back(s, f, hd, g, focus == C_BACK);
        const Rect sub = { 0, (int)(90 * g.u), w, (int)(30 * g.u) };
        f.centred(s, sub, "Host a room", 20.0f, 0x2A1404u);
        draw_box(s, f, hd, g.r[C_CHOOSE_COOP], 0, focus == C_CHOOSE_COOP);
        f.centred(s, g.r[C_CHOOSE_COOP], "COOP", 30.0f, ink_on(hd, 0));
        draw_box(s, f, hd, g.r[C_CHOOSE_VS], 0, focus == C_CHOOSE_VS);
        f.centred(s, g.r[C_CHOOSE_VS], "VS", 30.0f, ink_on(hd, 0));
        const Rect n1 = { g.r[C_CHOOSE_COOP].x, g.r[C_CHOOSE_COOP].y + g.r[C_CHOOSE_COOP].h + (int)(6 * g.u), g.r[C_CHOOSE_COOP].w, (int)(20 * g.u) };
        const Rect n2 = { g.r[C_CHOOSE_VS].x, n1.y, g.r[C_CHOOSE_VS].w, n1.h };
        f.centred(s, n1, "Adventure together", 12.0f, 0x5A2A0Au);
        f.centred(s, n2, "Battle in an arena", 12.0f, 0x5A2A0Au);
        return;
    }
    if (st->phase == LOBBY_FAILED) {
        draw_box(s, f, hd, g.fail_box, 2, 0);
        const Rect hr = { g.fail_box.x, g.fail_box.y + (int)(12 * g.u), g.fail_box.w, (int)(28 * g.u) };
        f.centred(s, hr, st->error_head[0] ? st->error_head : "Could not get into the room", 19.0f, C_INK);
        std::vector<std::string> ln;
        wrap(f, st->error[0] ? st->error : "The lobby server did not answer.", 14.0f, g.fail_box.w - 40 * g.u, &ln);
        for (size_t i = 0; i < ln.size() && i < 3; ++i) {
            const Rect lr = { g.fail_box.x, g.fail_box.y + (int)((50 + 20 * (int)i) * g.u), g.fail_box.w, (int)(18 * g.u) };
            f.centred(s, lr, ln[i].c_str(), 14.0f, C_INK);
        }
        draw_box(s, f, hd, g.r[C_FAIL_BACK], 0, focus == C_FAIL_BACK);
        f.centred(s, g.r[C_FAIL_BACK], "Back", 16.0f, ink_on(hd, 0));
        return;
    }
    draw_back(s, f, hd, g, focus == C_BACK);
    if (!in_room(st)) {
        const Rect box = { (int)(w * 0.5f - 170 * g.u), (int)(160 * g.u), (int)(340 * g.u), (int)(60 * g.u) };
        draw_box(s, f, hd, box, 1, 0);
        char b[96];
        static const char kDots[4][4] = { "", ".", "..", "..." };
        if (P.joining) snprintf(b, sizeof b, "Joining room %s%s", P.want_code, kDots[(P.frames / 15) & 3]);
        else snprintf(b, sizeof b, "Making a %s room%s", st->kind == LOBBY_VS ? "VS" : "coop", kDots[(P.frames / 15) & 3]);
        f.centred(s, box, b, 18.0f, C_INK);
        return;
    }
    draw_room(s, f, hd, g, st);
}
