/* menu_join_plate.cpp -- the "Joining" plate of a join successor (run after093, lane JOINFLOW1). See menu_join_plate.h.

   WHAT THE PLAYER USED TO SEE. Picking an online file asked the launcher for a server and then started the game again; the restarted game opened its
   own title, ran the main menu and the file select with the scripted presses showing, and only then entered the adventure. Nothing explained that.

   WHAT HAPPENS NOW. The marker SM64DS_JOIN_QUIET (set by online_join.cpp's apply_plan, read and cleared here at start-up) makes this process hold a plain
   plate over every picture it presents. The walk is NOT driven by hand and not skipped: the ROM's title, menu and file select are the machine that
   reaches the file (hal/title_entry.cpp port_title_skip_tick presses the file through the ROM's own handlers), it just is not drawn, and it is not heard
   (sd_host_mute), and the frame pacer is skipped on most frames while it runs so it takes a moment instead of ten seconds. title_entry.cpp's commit ends
   the plate when the ROM asks for the Stage; the first picture after it is the level's own.

   JOINWIN1: the plate is no longer a plate. The window holds black (build below), with one corner line "Joining..." after 3 s.

   THE CANCEL. B (the pad's B, the attack key) or Esc. It clears every name a join set (online_join_clear_env), drops the boot file, and
   starts the game again at its front door as a clean solo title (the same hook the join itself used); this process then ends. A walk that has not reached
   the adventure after 90 s does the same, so the plate can never be a dead end.

   WHAT IT DOES NOT DO. It does not change what the launcher's Multiplayer window starts (party / coop / VS / world rooms): they never carry the marker. */
#include "menu_join_plate.h"
#include "online_join.h"
#include "ui_text.h"
#include "host_settings.h"
#include "mods.h"
#include "comms_hub.h"   /* SEAMHUB1: the session record */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

extern "C" {
extern int  (*port_join_plate_up_hook)(void);            /* hal/sub_screen.cpp: the real mouse is not the stylus while the plate is up */
extern int  (*port_title_front_door_hook)(void);          /* hal/title_entry.cpp; tests/walk_window.cpp seats it */
extern int  (*port_join_quiet_tick_hook)(void);           /* hal/title_entry.cpp */
extern void (*port_join_quiet_end_hook)(const char *);    /* hal/title_entry.cpp */
extern int  port_join_quiet_cancelled;                    /* hal/title_entry.cpp */
void sd_host_mute(int on);                                /* hal/sdat/mixer.cpp */
int  host_setting_enabled_mods(char *bundle, size_t bcap, char *arr, size_t acap);
int  hal_menu_enabled(int key);                           /* hal/menu_compose.cpp */
}

extern "C" int port_join_plate_up(void);

namespace {

const ULONGLONG kGiveUpMs = 90000;
const int kFastKeep = 3;                 /* of every 4 frames the pacer is skipped while the walk is hidden */

struct Plate {
    int up;
    int server;
    ULONGLONG t0;
    unsigned frames;
    int said;
    int cancel;                          /* 0 none, 1 player, 2 the walk never finished */
    int armed;                           /* B / Esc have been seen released since the plate came up */
    int prev_esc, prev_b;
    int parent;                          /* JOINWIN1: raised in the process that started the successor, for its last pictures (no input, no cancel) */
    int line;                            /* JOINWIN1: the corner "Joining..." line is in the picture */
    int pw, ph;
    std::vector<uint32_t> px;
    int test_at;                         /* SM64DS_JOIN_QUIET_CANCEL=<frame>[:b|esc] (not in the kit) */
    int test_how;
    int test_file;
    int inplace;                         /* SEAMJOIN1: raised by an in-place join in this process (hal/online_join.cpp drives it, not the title walk) */
    int lowering;                        /* SEAMJOIN2: an in-place join's plate is ended and covers one more picture, the world's first */
} P;

/* JOINWIN1: the join takes longer than this before the corner line shows */
const ULONGLONG kLineMs = 3000;

/* JOINWIN1: NO PLATE. The player's report on the whole-window "Joining server N" plank over a green background: "a really ugly popup". The window
   holds black instead, the colour the ROM's own screen fade ends on, from the parent's last picture until the level's first one: the parent goes
   black the moment the successor is started (port_join_plate_raise_black), the successor is black from its first picture. If the join takes more
   than kLineMs one small line, "Joining...", sits in the bottom-left corner in the online pages' font, no box and no plank. */
void build(int w, int h)
{
    P.px.assign((size_t)w * h, 0xFF000000u);
    if (P.line && ui_text_ready()) {
        const float em = (float)h * 0.045f > 10.f ? (float)h * 0.045f : 10.f;
        const UiSurface sf = { P.px.data(), w, w, h };
        const float m = (float)h * 0.03f;
        ui_text_draw(&sf, m, (float)h - m, "Joining...", em, 0xD0D0D0, 255, 0);
    }
    P.pw = w; P.ph = h;
    fprintf(stderr, "[joinflow] black picture %dx%d built%s\n", w, h, P.line ? " with the corner line" : "");
}

int do_cancel()
{
    const char *why = P.cancel == 2 ? "the walk did not reach the adventure in time" : "the player cancelled";
    fprintf(stderr, "[joinflow] cancel (%s): clearing the join environment and starting the solo title (%u frames in)\n", why, P.frames);
    port::online_join_clear_env();
#ifndef PORT_ROM_CLEAN
    /* SM64DS_JOIN_QUIET_CANCEL_FILE=<0..2>: the solo title that follows presses that file (proof knob, compiled out of the kit) */
    if (P.test_file >= 0) {
        char v[2] = { (char)('0' + P.test_file), 0 };
        SetEnvironmentVariableA("SM64DS_BOOT_FILE", v);
    }
#endif
#ifndef PORT_ROM_CLEAN
    /* a proof run reading back every picture (SM64DS_JOINFLOW_OFFSCREEN): the solo title that follows writes its own pictures, solo_N, not over pic_N */
    {
        char nm[512];
        if (GetEnvironmentVariableA("SM64DS_PRESENT_OFFSCREEN_BMP", nm, sizeof nm) && nm[0]) {
            std::string t(nm);
            const size_t at = t.find("pic_%d");
            if (at != std::string::npos) { t.replace(at, 6, "solo_%d"); SetEnvironmentVariableA("SM64DS_PRESENT_OFFSCREEN_BMP", t.c_str()); }
        }
    }
#endif
    port_join_quiet_cancelled = 1;
    const int ok = port_title_front_door_hook ? port_title_front_door_hook() : 0;
    const unsigned long err = ok ? 0 : GetLastError();
    if (ok) fprintf(stderr, "[joinflow] the solo title was started; this process is quitting\n");
    else fprintf(stderr, "[joinflow] the solo title could not be started (win32 %lu); quitting anyway\n", err);
    fflush(stderr);
    return 1;                                /* the plate stays up on the last picture until the process is gone */
}

int tick()
{
    if (!P.up || P.parent || P.inplace) return 0;   /* SEAMJOIN1: an in-place join ticks its plate itself (port_join_plate_inplace_tick) */
    if (!P.said) {
        P.said = 1;
        fprintf(stderr, "[joinflow] quiet entry: the window holds black until the adventure is entered (server %d)\n", P.server);
        fflush(stderr);
    }
    ++P.frames;
    sd_host_mute(1);
    if (port_join_quiet_cancelled) return 1;
    if (!P.cancel && GetTickCount64() - P.t0 > kGiveUpMs) P.cancel = 2;
#ifndef PORT_ROM_CLEAN
    if (!P.cancel && P.test_at >= 0 && (int)P.frames == P.test_at) {
        if (P.test_how == 1) port_join_plate_input(1, 0);
        else port_join_plate_input(0, 1);
        fprintf(stderr, "[joinflow] test cancel injected at frame %u (%s)\n", P.frames, P.test_how == 1 ? "Esc" : "B");
    }
#endif
    if (P.cancel) return do_cancel();
    return 0;
}

void ended(const char *why)
{
    if (!P.up || port_join_quiet_cancelled) return;
    P.up = 0;
    P.inplace = 0;
    sd_host_mute(0);
    fprintf(stderr, "[joinflow] plate down after %u frames: %s\n", P.frames, why ? why : "");
    fflush(stderr);
}

int boot()
{
    const char *e = getenv("SM64DS_JOIN_QUIET");
    if (e && *e) {
        P.up = 1;
        P.server = atoi(e);
        P.t0 = GetTickCount64();
        P.test_at = -1;
        P.test_file = -1;
        SetEnvironmentVariableA("SM64DS_JOIN_QUIET", nullptr);           /* never inherited by this process's own successors */
        sd_host_mute(1);
#ifndef PORT_ROM_CLEAN
        if (const char *t = getenv("SM64DS_JOIN_QUIET_CANCEL")) {
            P.test_at = atoi(t);
            const char *c = strchr(t, ':');
            P.test_how = (c && !_stricmp(c + 1, "esc")) ? 1 : 0;
        }
        if (const char *f = getenv("SM64DS_JOIN_QUIET_CANCEL_FILE")) P.test_file = atoi(f);
#endif
    }
    port_join_plate_up_hook = port_join_plate_up;
    port_join_quiet_tick_hook = tick;
    port_join_quiet_end_hook = ended;
    return 1;
}
const int g_seated = boot();

}  // namespace

extern "C" int port_join_plate_up(void) { return P.up; }

/* KNOWNNET1: an in-place plate whose join has ended (cancelled, or the title walking to the world) only waits to cover one more
   picture (SEAMJOIN2's lowering, plate_lowered in port_join_plate_picture). A window that presents nothing (minimised) never draws
   that picture, so the plate stayed "up" and hal/handoff_poll.cpp refused every hand-off after a B / Esc cancel until the window
   was shown again: the launcher's next join ran out its deadline once. A lowering plate no longer holds the title's hand-off poll;
   the presenter still draws it once (port_join_plate_up is unchanged). */
extern "C" int port_join_plate_holds_handoff(void) { return (P.up && !(P.inplace && P.lowering)) ? 1 : 0; }

static void plate_lowered(void);   /* SEAMJOIN2: below, with the in-place plate's end */

extern "C" const uint32_t *port_join_plate_picture(int w, int h)
{
    if (!P.up || w < 32 || h < 32) return nullptr;
    const int line = (!P.parent && GetTickCount64() - P.t0 > kLineMs) ? 1 : 0;
    if (P.pw != w || P.ph != h || P.px.empty() || P.line != line) { P.line = line; build(w, h); }
    if (P.lowering) plate_lowered();   /* SEAMJOIN2: this picture is the last the plate covers (px stays allocated for this present) */
    return P.px.data();
}

/* JOINWIN1: the process that has just started a join successor holds black over its last pictures (the parent half of the hand-off), so the
   window goes from the online file select to black and stays black through the successor's walk. */
extern "C" void port_join_plate_raise_black(void)
{
    if (P.up) return;
    P.up = 1;
    P.parent = 1;
    P.t0 = GetTickCount64();
    P.px.clear();
    fprintf(stderr, "[joinflow] the successor is starting: this window holds black until it closes\n");
    fflush(stderr);
}

/* SEAMJOIN1: THE PLATE OF AN IN-PLACE JOIN. Joining from the online file select no longer starts a successor: this process installs the hub
   and enters the world itself (hal/online_join.cpp). The player sees what a successor showed: black from the moment the plan is applied until
   the level, the corner line after 3 s, the title muted, B or Esc cancels. The cancel does not start a solo title any more: online_join.cpp
   polls it (port_join_plate_inplace_tick), takes the session down in this process and lowers the plate (port_join_plate_inplace_end). */
extern "C" void port_join_plate_raise_in_place(int server)
{
    P.up = 1;
    P.parent = 0;
    P.inplace = 1;
    P.lowering = 0;
    P.server = server;
    P.t0 = GetTickCount64();
    P.frames = 0;
    P.said = 0;
    P.cancel = 0;
    P.armed = 0;
    P.prev_esc = P.prev_b = 0;
    P.line = 0;
    P.px.clear();
    P.test_at = -1;
    P.test_file = -1;
#ifndef PORT_ROM_CLEAN
    static int drilled;   /* the drill cancels the first in-place join only, so the row can join again afterwards */
    const char *t = drilled ? 0 : getenv("SM64DS_JOIN_QUIET_CANCEL");
    if (t) {   /* the successor's cancel drill, read here for the in-place join */
        drilled = 1;
        P.test_at = atoi(t);
        const char *c = strchr(t, ':');
        P.test_how = (c && !_stricmp(c + 1, "esc")) ? 1 : 0;
    }
#endif
    sd_host_mute(1);
    fprintf(stderr, "[joinflow] in place: the window holds black until the adventure is entered (server %d)\n", server);
    fflush(stderr);
}

/* once a game frame while an in-place join waits for the world: 1 = the player asked to cancel */
extern "C" int port_join_plate_inplace_tick(void)
{
    if (!P.up || !P.inplace || P.lowering) return 0;
    ++P.frames;
    sd_host_mute(1);
#ifndef PORT_ROM_CLEAN
    if (!P.cancel && P.test_at >= 0 && (int)P.frames == P.test_at) {
        if (P.test_how == 1) port_join_plate_input(1, 0);
        else port_join_plate_input(0, 1);
        fprintf(stderr, "[joinflow] test cancel injected at frame %u (%s)\n", P.frames, P.test_how == 1 ? "Esc" : "B");
    }
#endif
    return P.cancel ? 1 : 0;
}

extern "C" int port_join_plate_inplace_up(void) { return P.up && P.inplace && !P.lowering; }

/* SEAMJOIN2: THE PLATE COVERS THE WORLD'S FIRST PICTURE, as a successor's does. The end is called on the game frame the title hands the game
   to the level, and lowering the plate there let the next picture through: a full white frame where the restart path still shows black,
   then the same pictures on both paths from the second on. So the end keeps the plate for one more picture and port_join_plate_picture
   takes it down after drawing it; the sound comes back here, at the end, as before. */
static void plate_lowered(void)
{
    P.up = 0;
    P.inplace = 0;
    P.lowering = 0;
    P.cancel = 0;
}

extern "C" void port_join_plate_inplace_end(const char *why)
{
    if (!P.up || !P.inplace || P.lowering) return;
    P.lowering = 1;
    P.cancel = 0;
    sd_host_mute(0);
    fprintf(stderr, "[joinflow] plate down after %u frames (it covers one more picture): %s\n", P.frames, why ? why : "");
    fflush(stderr);
}

extern "C" int port_join_plate_click(int cx, int cy)
{
    (void)cx; (void)cy;
    return P.up ? 1 : 0;                     /* JOINWIN1: nothing to click on the black (B / Esc cancel); the click still reaches nothing behind it */
}

extern "C" void port_join_plate_input(int esc_down, int b_down)
{
    if (!P.up || P.parent) return;
    /* a key or button already down when the plate came up cannot cancel until it has been let go */
    if (!P.armed) {
        if (!esc_down && !b_down) P.armed = 1;
        P.prev_esc = esc_down; P.prev_b = b_down;
        return;
    }
    if (!P.cancel && ((esc_down && !P.prev_esc) || (b_down && !P.prev_b))) {
        P.cancel = 1;
        fprintf(stderr, "[joinflow] %s pressed\n", esc_down && !P.prev_esc ? "Esc" : "B");
    }
    P.prev_esc = esc_down; P.prev_b = b_down;
}

extern "C" int port_join_plate_fast(void)
{
    if (!P.up || port_join_quiet_cancelled) return 0;
    if (P.inplace) return 0;                 /* SEAMJOIN1: nothing is hidden behind an in-place join's black: the frames keep their pace */
#ifndef PORT_ROM_CLEAN
    static int off = -1;
    if (off < 0) { const char *e = getenv("SM64DS_JOIN_QUIET_PACED"); off = (e && *e) ? 1 : 0; }
    if (off) return 0;
#endif
    return (P.frames & 3u) < (unsigned)kFastKeep;
}

static int g_settings_done;   /* SEAMHUB1: once per session, re-armed by the in-place leave */

/* SEAMHUB1: an online world left in place (hal/online_join.cpp) is a new solo session in this process: the next level prints the
   SETTINGS line again (tests/walk_window.cpp, when the quit poll sees a level handed back). */
extern "C" void port_join_settings_rearm(void) { g_settings_done = 0; }
extern "C" int port_join_settings_armed(void) { return !g_settings_done; }

extern "C" void port_join_settings_dump(int cw, int ch, int fullscreen, int maximised, int hand_sized)
{
    int &done = g_settings_done;
    if (done) return;
    done = 1;
    char winbuf[32];
    if (cw > 0 && ch > 0) snprintf(winbuf, sizeof winbuf, "%dx%d", cw, ch); else snprintf(winbuf, sizeof winbuf, "minimised");
    char bundle[260] = "", arr[512] = "";
    host_setting_enabled_mods(bundle, sizeof bundle, arr, sizeof arr);
    {   /* the launcher writes settings.json pretty printed: keep the array on this one line */
        char *w = arr;
        for (const char *r = arr; *r; ++r)
            if ((unsigned char)*r > 0x20) *w++ = *r;
        *w = 0;
    }
    std::string ids;
    for (int i = 0; i < port_mods_count(); ++i) { if (i) ids += ","; ids += port_mods_id(i); }
    fprintf(stderr,
            "[joinflow] SETTINGS process=%s window=%s fullscreen=%d maximised=%d hand_sized=%d frame_rate=%d smooth_motion=%d render_scale=%d "
            "hd_textures=%d smooth_models=%d texture_filter=%d anti_aliasing=%d present_backend=%d present_filter=%d vsync=%d renderer=%d "
            "one_screen_menus=%d hud_layout=%d volume=%d draw_distance=%d camera_mode=%d start_fullscreen=%d adventure_ghosts=%d name_tags=%d "
            "improved_minimap=%d save_menu_on_top=%d character_strip=%d keep_lives=%d skip_menu=%d skip_intro=%d aspect_env=%s dual_screen_env=%s "
            "mods_in_file=%s mods_in_effect=%d[%s] multiplayer=%d\n",
            port::online_session_get(port::kSessAdventure) ? "adventure" : "solo", winbuf, fullscreen, maximised, hand_sized,
            host_setting_frame_rate(), host_setting_smooth_motion(), host_setting_render_scale(), host_setting_hd_textures(),
            host_setting_smooth_models(), host_setting_texture_filter(), host_setting_anti_aliasing(), host_setting_present_backend(),
            host_setting_present_filter(), host_setting_vsync(), host_setting_renderer(), host_setting_one_screen_menus(), host_setting_hud_layout(),
            host_setting_volume(), host_setting_draw_distance_pct(), host_setting_camera_mode(), host_setting_start_fullscreen(),
            host_setting_adventure_ghosts(), host_setting_name_tags(), host_setting_improved_minimap(), host_setting_save_menu_on_top(),
            host_setting_character_strip(), host_setting_keep_lives(), getenv("SM64DS_SKIP_MENU") ? 1 : 0, getenv("SM64DS_SKIP_INTRO") ? 1 : 0,
            getenv("SM64DS_ASPECT") ? getenv("SM64DS_ASPECT") : "-", getenv("SM64DS_DUAL_SCREEN") ? getenv("SM64DS_DUAL_SCREEN") : "-",
            arr[0] ? arr : "-", port_mods_count(), ids.c_str(), port_mods_multiplayer());
    fflush(stderr);
}
