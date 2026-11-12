/* ONLINE JOIN: see hal/online_join.h for the contract. */
#include "online_join.h"

#include <windows.h>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "ntr/backup.h"
#include "host_settings.h"
#include "comms_hub.h"   /* SEAMHUB1: the session record and the teardown */
#include "handoff_poll.h"   /* SEAMVS1: the launcher hand-off's "adventure" kind */

int out_volume_pct(void);   /* hal/sdat/out_win.cpp: the master volume this process plays at right now (0..100) */

extern "C" {
extern int (*port_title_front_door_hook)(void);   /* hal/title_entry.cpp; tests/walk_window.cpp seats it: start the game again at its front door */
extern int (*port_online_join_tick_hook)(void);   /* hal/title_entry.cpp; seated below */
void port_join_plate_raise_black(void);            /* hal/menu_join_plate.cpp (JOINWIN1) */
}

namespace port {
namespace {

const unsigned kPollMs = 250;
const unsigned kWaitMs = 20000;

OnlineJoinState g_state = kJoinIdle;
std::string g_sentence;
unsigned long long g_stamp = 0;
int g_file = 0;
ULONGLONG g_t0 = 0, g_last_poll = 0;

unsigned long long now_ms() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    unsigned long long t = ((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    return t / 10000ull - 11644473600000ull;
}

std::string folder() {
    std::string d = ntr::backup::path();
    const size_t cut = d.find_last_of("/\\");
    d = (cut == std::string::npos ? std::string(".") : d.substr(0, cut)) + "/online";
    return d;
}
std::string req_path() { return folder() + "/join_request.json"; }
std::string plan_path() { return folder() + "/join_plan.json"; }

bool read_file(const std::string &p, std::string *out) {
    FILE *f = std::fopen(p.c_str(), "rb");
    if (!f) return false;
    char buf[4096];
    size_t n = std::fread(buf, 1, sizeof buf - 1, f);
    std::fclose(f);
    buf[n] = 0;
    *out = buf;
    return true;
}

/* tiny extraction, enough for the flat files the launcher writes; a value with a backslash is refused */
bool jfind(const std::string &t, const char *key, size_t *at) {
    std::string k = std::string("\"") + key + "\"";
    size_t p = t.find(k);
    if (p == std::string::npos) return false;
    p += k.size();
    while (p < t.size() && (t[p] == ' ' || t[p] == ':' || t[p] == '\t')) ++p;
    *at = p;
    return p < t.size();
}
bool jstr(const std::string &t, const char *key, std::string *v) {
    size_t p;
    if (!jfind(t, key, &p) || t[p] != '"') return false;
    size_t e = t.find('"', p + 1);
    if (e == std::string::npos) return false;
    *v = t.substr(p + 1, e - p - 1);
    return v->find('\\') == std::string::npos;
}
bool jnum(const std::string &t, const char *key, long long *v) {
    size_t p;
    if (!jfind(t, key, &p)) return false;
    char *end = nullptr;
    long long x = std::strtoll(t.c_str() + p, &end, 10);
    if (end == t.c_str() + p) return false;
    *v = x;
    return true;
}
bool jbool(const std::string &t, const char *key, bool *v) {
    size_t p;
    if (!jfind(t, key, &p)) return false;
    if (t.compare(p, 4, "true") == 0) { *v = true; return true; }
    if (t.compare(p, 5, "false") == 0) { *v = false; return true; }
    return false;
}

bool printable(const std::string &s, size_t lo, size_t hi, bool allow_space = false) {
    if (s.size() < lo || s.size() > hi) return false;
    const unsigned char first = allow_space ? 0x20 : 0x21;
    for (char c : s) if ((unsigned char)c < first || (unsigned char)c > 0x7E || c == '"') return false;
    return true;
}
bool hex6(const std::string &s) {
    if (s.size() != 6) return false;
    for (char c : s) if (!std::isxdigit((unsigned char)c)) return false;
    return true;
}

void fail(const char *s) { g_state = kJoinFailed; g_sentence = s; }

/* SEAMJOIN1: the in-place join (below, after the leave): the plan's values, the profile's pick already applied */
struct JoinPlan {
    std::string relay, code, name, color, shoes;
    long long seat;
    int server;
    bool has_name, has_cols;
    bool no_press = false;   /* SEAMVS1: a launcher hand-off at the title: the hub comes up and the player picks the file */
};
const char *join_in_place(const JoinPlan &p);   /* 0 = joining in this process; else why not (the caller restarts) */
int join_in_place_phase();                      /* 0 none, 1 waiting for the seat / the file select, 2 file pressed, 3 entered */

/* Apply a validated plan: the same names the launcher's BuildAdventureEnv sets, then the front door. */
bool apply_plan(const std::string &t, int server) {
    std::string relay, code, kind, name, color, shoes;
    long long seat = 255;
    jstr(t, "relay", &relay); jstr(t, "code", &code); jstr(t, "kind", &kind);
    jnum(t, "seat", &seat);
    bool has_name = jstr(t, "name", &name);
    const bool has_cols = jstr(t, "color", &color) && jstr(t, "shoes", &shoes);
    bool ok = printable(relay, 1, 253) && printable(code, 1, 8) && kind == "world" &&
              ((seat >= 0 && seat < 32) || seat == 255);
    /* The player's nickname may hold spaces ("Mario Fan"; the launcher keeps 0x20..0x7E). Leading and trailing spaces are
       trimmed; a name of only spaces is treated as no name (the tag falls back to PLAYER n). Relay and code stay strict. */
    if (has_name) {
        size_t b = name.find_first_not_of(' '), e = name.find_last_not_of(' ');
        name = (b == std::string::npos) ? std::string() : name.substr(b, e - b + 1);
        if (name.empty()) has_name = false;
        else if (!printable(name, 1, 24, true)) ok = false;
    }
    if (has_cols && !(hex6(color) && hex6(shoes))) ok = false;
    if (!ok) { fail("The launcher's answer looked wrong. Try again."); return false; }
    if (!port_title_front_door_hook) { fail("This game cannot join from here."); return false; }

    /* SEAMJOIN1: JOIN IN THIS PROCESS FIRST. The same values the restart hands its successor (the plan's, the profile's pick over them) go
       into the session record, the hub is installed live and the file is pressed on this file select; the restart below is the refusal
       path, unchanged, and says why in one line. */
    {
        JoinPlan jp;
        jp.relay = relay; jp.code = code; jp.seat = seat; jp.server = server;
        jp.has_name = has_name; jp.name = name;
        jp.has_cols = has_cols; jp.color = color; jp.shoes = shoes;
        const char *own = host_setting_online_name();
        if (own[0]) { jp.name = own; jp.has_name = true; }
        unsigned body = 0, sh = 0;
        if (host_setting_online_colours(&body, &sh)) {
            char b6[8], s6[8];
            std::snprintf(b6, sizeof b6, "%06X", body & 0xFFFFFFu);
            std::snprintf(s6, sizeof s6, "%06X", sh & 0xFFFFFFu);
            jp.color = b6; jp.shoes = s6; jp.has_cols = true;
        }
        const char *why = join_in_place(jp);
        if (!why) return true;
        std::fprintf(stderr, "[seamless] in place refused: %s; restarting\n", why);
        std::fflush(stderr);
    }

    SetEnvironmentVariableA("SM64DS_ADVENTURE", "1");
    SetEnvironmentVariableA("SM64DS_COMMS_RELAY", relay.c_str());
    SetEnvironmentVariableA("SM64DS_COMMS_CODE", code.c_str());
    SetEnvironmentVariableA("SM64DS_ROOM_KIND", "world");
    SetEnvironmentVariableA("SM64DS_VOICE_ALLOWED", "0");
    if (seat != 255) { char b[8]; std::snprintf(b, sizeof b, "%lld", seat); SetEnvironmentVariableA("SM64DS_HUB_SEAT", b); }
    else SetEnvironmentVariableA("SM64DS_HUB_SEAT", nullptr);
    if (server > 0 && server < 1000) { char w[8]; std::snprintf(w, sizeof w, "%d", server); SetEnvironmentVariableA("SM64DS_WORLD_NO", w); }   /* Rich Presence shows the number */
    if (has_name) SetEnvironmentVariableA("SM64DS_PLAYER_NAME", name.c_str());
    if (has_cols) { SetEnvironmentVariableA("SM64DS_PLAYER_COLOR", color.c_str()); SetEnvironmentVariableA("SM64DS_PLAYER_SHOES", shoes.c_str()); }
    /* PROFILE1: the player's own pick from the in-game popup wins over what the launcher / lobby sent: the saved name is the one every other console
       reads in the identity message, and the saved colour is exported as the same six-hex-digit pair the launcher's own start uses
       (hal/comms_sync.cpp gw_rgb_env), so this player's Yoshi and the figure every other console draws for them show it. Nothing saved (the popup
       always runs before a pick joins, so this is a launcher-started game's own path) leaves the plan's values as they were. */
    {
        const char *own = host_setting_online_name();
        if (own[0]) SetEnvironmentVariableA("SM64DS_PLAYER_NAME", own);
        unsigned body = 0, shoes = 0;
        if (host_setting_online_colours(&body, &shoes)) {
            char b6[8], s6[8];
            std::snprintf(b6, sizeof b6, "%06X", body & 0xFFFFFFu);
            std::snprintf(s6, sizeof s6, "%06X", shoes & 0xFFFFFFu);
            SetEnvironmentVariableA("SM64DS_PLAYER_COLOR", b6);
            SetEnvironmentVariableA("SM64DS_PLAYER_SHOES", s6);
            std::fprintf(stderr, "[join] own profile: name \"%s\" body %s shoes %s\n", own, b6, s6);
        }
    }
#ifndef PORT_ROM_CLEAN
    SetEnvironmentVariableA("SM64DS_JOIN_TEST", nullptr);
#endif
    char v[2] = { (char)('0' + g_file), 0 };
    SetEnvironmentVariableA("SM64DS_BOOT_FILE", v);
    /* JOINFLOW1: THE VOLUME GOES WITH THE PLAYER. The launcher starts a game with SM64DS_VOLUME set to the launcher's own volume, and that variable
       outranks settings.json inside the game for the whole life of the process. A volume the player turned in the in-game settings page since then
       is live in this process (and saved), so the restarted game, which inherits the launcher's number, played at the old volume until the file next
       changed. It is handed the number this process is playing at now. */
    {
        char vol[8];
        std::snprintf(vol, sizeof vol, "%d", out_volume_pct());
        SetEnvironmentVariableA("SM64DS_VOLUME", vol);
    }
    /* JOINFLOW1: this start is a join successor. The restarted game reads (and clears) SM64DS_JOIN_QUIET and runs its quiet entry: a plain "Joining" plate
       over every picture until the adventure is entered (hal/menu_join_plate.cpp). The value is the server number (0 = not known). Games the launcher's own
       Multiplayer window starts never carry it, so they are untouched. */
    {
        char q[8];
        std::snprintf(q, sizeof q, "%d", (server > 0 && server < 1000) ? server : 0);
        SetEnvironmentVariableA("SM64DS_JOIN_QUIET", q);
    }
#ifndef PORT_ROM_CLEAN
    /* SM64DS_JOINFLOW_OFFSCREEN=<bmp name with %d>: the restarted game (only) reads back every picture it presents, from its first one (proof knob, compiled out of the kit) */
    if (const char *dump = std::getenv("SM64DS_JOINFLOW_OFFSCREEN")) {
        SetEnvironmentVariableA("SM64DS_PRESENT_OFFSCREEN", "1");
        SetEnvironmentVariableA("SM64DS_PRESENT_OFFSCREEN_EVERY", "1");
        SetEnvironmentVariableA("SM64DS_PRESENT_OFFSCREEN_BMP", dump);
    }
#endif
    std::fprintf(stderr, "[join] plan applied for server %d, file %d: starting the game again at its front door with the hub environment\n", server, g_file);
    std::fflush(stderr);
#ifndef PORT_ROM_CLEAN
    /* SM64DS_JOIN_TEST_RELAUNCH_FAIL=1: act as if the restart was refused (drill for the clean-up below; compiled out of the kit) */
    const bool relaunched = std::getenv("SM64DS_JOIN_TEST_RELAUNCH_FAIL") ? false : port_title_front_door_hook() != 0;
#else
    const bool relaunched = port_title_front_door_hook() != 0;
#endif
    if (!relaunched) {
        SetEnvironmentVariableA("SM64DS_BOOT_FILE", nullptr);
        /* back to a clean solo process: the hub names set above must not steer the solo file select (online_wanted reads them) */
#ifndef PORT_ROM_CLEAN
        /* SM64DS_JOIN_TEST_RELAUNCH_FAIL=2: keep the names set (replays the behaviour before this clean-up, for the before/after proof) */
        const char *drill = std::getenv("SM64DS_JOIN_TEST_RELAUNCH_FAIL");
        if (!(drill && drill[0] == '2'))
#endif
        online_join_clear_env();
#ifndef PORT_ROM_CLEAN
        char w32[16] = "";
        const DWORD wn = GetEnvironmentVariableA("SM64DS_COMMS_RELAY", w32, sizeof w32);
        std::fprintf(stderr, "[join] restart refused: COMMS_RELAY seen by the process (getenv) %s, by the system block %s"
                            "\n", std::getenv("SM64DS_COMMS_RELAY") ? "SET" : "unset", wn ? "SET" : "unset");
#endif
        fail("The game could not restart to join. Try again.");
        return false;
    }
    g_state = kJoinEntering;
    return true;
}

int tick_hook() {
#ifndef PORT_ROM_CLEAN
    /* SM64DS_JOIN_TEST=<file>: write the request at tick 300 (proof knob, compiled out of the kit; the child clears it) */
    static int ticks = 0;
    static int fired = 0;
    if (!fired) {
        const char *e = std::getenv("SM64DS_JOIN_TEST");
        if (e && *e && ++ticks == 300) {
            fired = 1;
            const int f = (e[0] >= '0' && e[0] <= '2') ? e[0] - '0' : 0;
            online_join_begin(f, "JoinTest");
        }
    }
#endif
    if (g_state == kJoinWaiting) {
#ifndef PORT_ROM_CLEAN
        /* SM64DS_JOIN_TEST_CANCEL=<n>: cancel the wait n ticks after the request (proof knob, compiled out of the kit) */
        static int since = 0;
        ++since;
        const char *c = std::getenv("SM64DS_JOIN_TEST_CANCEL");
        if (c && *c && since == std::atoi(c)) { online_join_cancel(); return 0; }
#endif
        online_join_step();
    }
    /* JOINWIN1: the successor is started. This window goes black for its last pictures (one more tick is drawn black before this process
       stops), so the player sees the file select, then black, then the level: the successor is black from its first picture too. */
    /* SEAMJOIN1: an in-place join has no successor: this process enters the world itself, so the title loop must not stop here */
    if (g_state == kJoinEntering && !join_in_place_phase()) {
        static int black_ticks = 0;
        if (black_ticks++ == 0) { port_join_plate_raise_black(); return 0; }
        return 1;
    }
    return 0;
}
const int g_seated = (port_online_join_tick_hook = tick_hook, 1);

}  // namespace

/* Every name apply_plan sets (SM64DS_VOLUME aside, which is always handed over as the live volume). */
static const char *const k_join_env[] = { "SM64DS_ADVENTURE", "SM64DS_COMMS_RELAY", "SM64DS_COMMS_CODE", "SM64DS_ROOM_KIND",
    "SM64DS_VOICE_ALLOWED", "SM64DS_HUB_SEAT", "SM64DS_PLAYER_NAME", "SM64DS_PLAYER_COLOR", "SM64DS_PLAYER_SHOES", "SM64DS_WORLD_NO",
    "SM64DS_JOIN_QUIET", "SM64DS_BOOT_FILE" };

/* JOINFLOW1: take every name a join sets out of this process again (the restart was refused, or the player cancelled on the Joining plate): the
   solo file select, the mod rule and the hub all read these. One list for both clean-up paths. */
void online_join_clear_env() {
    for (const char *n : k_join_env) SetEnvironmentVariableA(n, nullptr);
}

/* JOINWIN1: LEAVING AN ONLINE SESSION BY ITS QUIT ROW IS AN OFFLINE START. The level-clear menu's Save and Quit and the pause menu's quit rows end
   the session through the front door (tests/walk_window.cpp port_front_end_quit_poll), and the restarted game inherits this process's block. In a
   join successor (or a launcher-started online game) that block still holds the hub: the restart came up as an Adventure Online title with every
   mod off and the hub installed again (the player's report: "all my settings and mods turned off"). Every name apply_plan sets (k_join_env above)
   and every online session name the launcher's own start sets (its AlwaysClear and lobby names) goes, so the restart reads the player's own
   settings.json and mods like any offline boot. The volume is handed over as this process plays it now, the way apply_plan does. A solo process
   holds none of these names: nothing is printed and nothing changes for it. */
/* The launcher's own online session names (its AlwaysClear and lobby names). SEAMHUB1 moved the list out of
   port_online_quit_offline_env so the in-place leave clears exactly the names the restart clears. */
static const char *const k_session_env[] = {
    "SM64DS_COMMS_HOST", "SM64DS_COMMS_ROLE", "SM64DS_COMMS_SLOT", "SM64DS_COMMS_PORT", "SM64DS_COMMS_INPUT_DELAY",
    "SM64DS_HUB_PORT", "SM64DS_HUB_DELAY_MS", "SM64DS_PARTY", "SM64DS_PARTY_SHARE_STARS", "SM64DS_NETMODE",
    "SM64DS_COOP", "SM64DS_COOP_HOST", "SM64DS_COOP_SAVE", "SM64DS_COOP_SAVE_NAME", "SM64DS_COOP_TP",
    "SM64DS_ROOM_COLLISION", "SM64DS_ROOM_INTERACT", "SM64DS_ROOM_DAMAGE", "SM64DS_ROOM_TP", "SM64DS_ROOM_STARSHARE",
    "SM64DS_ROOM_SOLOEXIT",   /* STAREXIT1: only the collector leaves (hal/star_popup.cpp) */
    "SM64DS_VS_PLAYERS", "SM64DS_VS_NAMES", "SM64DS_VS_COLORS", "SM64DS_VS_NETMODE", "SM64DS_SESSION_DIGEST",
    "SM64DS_SKIP_MENU",
    /* JOINWIN2: the launcher's VS child also carries the arena and its star target (hal/comms_hub.cpp and hal/comms_loopback.cpp read
       both); a VS_MAP left set makes the restart a VS boot instead of the title */
    "SM64DS_VS_MAP", "SM64DS_VS_STAR_TARGET",
    /* SEAMHUB1 (REVINT100K): a king match and Luigi Infection carry three more (LobbyLaunch.cs) */
    "SM64DS_VS_KING_TARGET", "SM64DS_VS_LUIGI_INFECTION", "SM64DS_VS_LUIGI_SEED" };

extern "C" int port_online_quit_offline_env(void)
{
    struct List { const char *const *names; size_t count; };
    const List lists[2] = { { k_join_env, sizeof k_join_env / sizeof *k_join_env },
                            { k_session_env, sizeof k_session_env / sizeof *k_session_env } };
#ifndef PORT_ROM_CLEAN
    /* SM64DS_JOINWIN_KEEP_ENV=1: keep every name (replays the behaviour before this clean-up, for the before/after proof; compiled out of the kit) */
    if (const char *keep = std::getenv("SM64DS_JOINWIN_KEEP_ENV")) {
        if (keep[0] == '1') {
            std::fprintf(stderr, "[join] SM64DS_JOINWIN_KEEP_ENV: the restart keeps this process's online names (the behaviour before JOINWIN1)\n");
            return 0;
        }
    }
#endif
    std::string cleared;
    int n = 0;
    char v[8];
    for (const List &l : lists) {
        for (size_t i = 0; i < l.count; ++i) {
            SetLastError(0);
            if (!GetEnvironmentVariableA(l.names[i], v, sizeof v) && GetLastError() == ERROR_ENVVAR_NOT_FOUND) continue;
            SetEnvironmentVariableA(l.names[i], nullptr);
            if (n++) cleared += ' ';
            cleared += l.names[i] + 7;   /* without the SM64DS_ prefix */
        }
    }
    if (!n) return 0;
    std::snprintf(v, sizeof v, "%d", out_volume_pct());
    SetEnvironmentVariableA("SM64DS_VOLUME", v);
    std::fprintf(stderr, "[join] leaving the online session: the restart is an offline start (%d online name(s) cleared: %s; volume %s)\n",
                 n, cleared.c_str(), v);
    std::fflush(stderr);
    return n;
}

/* ==== SEAMHUB1: LEAVING AN ONLINE WORLD IN PLACE (run after093; the one-process plan's M4a) ====================================
   Save and Quit (or a pause quit row) out of an Adventure Online world went back to the solo title by starting the game again
   (port_online_quit_offline_env + the front door): the hub, the mod rule, the online save and the player's online name and colours
   are all chosen when a process starts. SEAMQUIT1 made the offline quit an in-place scene change, the cartridge's own; this does the
   same for the online world, in two halves.

   AT THE QUIT (tests/walk_window.cpp port_front_end_quit_poll asks port_online_leave_refusal, then calls port_online_leave_begin): the
   save is already written (the save menu writes before it asks for scene 1), so the save medium goes back to the single player save
   at once (ntr::backup::select_online, NAME1's run-time switch) and the title the ROM builds next reads its files from it. The quit
   poll then leaves the scene-1 request in place, as for an offline quit, and the ROM's fade and the level-to-scene crossing take the
   game to the title. The hub keeps running through the fade, so the ghosts stay drawn until the Stage and every body are gone.

   AT THE TITLE (the first game frame with the title live; port_sdat_frame_hook, one call per game frame in every loop, chained):
     1. the hub is torn down (comms_hub_teardown: the I/O thread stopped and joined, the socket closed, every seat and queue emptied,
        the install latch forgotten); the ghost layer, the hub body count and presence go back to a process that never joined
        (comms_sync_leave_reset, comms_conductor_leave_reset, hub_presence_leave_reset), with the ROM's body count and input gate
        words put back to the solo values;
     2. the session record is cleared for good (hal/comms_hub.h), and every online name this process holds goes from BOTH copies of the
        environment (the CRT's, which getenv reads, through _putenv_s; the Win32 block a restart would inherit): the same list the
        restart clears (k_join_env + k_session_env). The Adventure flag's env latch is read again (host_setting_adventure_env_reread),
        so the multiplayer mod rule and adventure_ghost_mode() answer as a solo boot does;
     3. the mods are latched again to the player's offline list (hal/mods.cpp port_mods_relatch_after_level, which also counts the
        files a level read as held: SEAMMODS1 note c);
     4. one line says what is now in effect: the save medium, the identity a fresh read gives, the lobby colours, the Loves Me
        setting, the Adventure flag and the mod rule.
   ANYTHING THAT CANNOT BE DONE IN PLACE TAKES THE RESTART, AS BEFORE, saying why in one line ("[seamless] in place refused: <why>;
   restarting"): at the quit a VS / arena / direct-carrier / party / coop session, the lobby colours (read once by hal/fs_mods.cpp), a
   scene-booted process or a loaded code mod; at the title a hub thread that does not stop or a relatch the mods layer refuses (a code
   mod or a held file on the offline list, a change in hd_ui / sound / character mods). The title-time restart goes through
   port_online_leave_restart_hook (tests/walk_window.cpp: the front door, the hand-off wait, the process ends). The join itself still
   restarts (M4b). */
}  // namespace port

namespace ntr { namespace backup { void select_online(int on); bool online_selected(); } }
extern "C" {
extern void (*port_sdat_frame_hook)(void);                    /* hal/sdat/consumer.cpp: one call per game frame */
int port_scene_live_id(void);                                 /* hal/scene_boot.cpp: 1 = the title is the live scene */
int port_scene_is_hosted(int id);                             /* hal/scene_boot.cpp (SEAMCLEAN1) */
int port_mods_relatch_after_level(const char *reason);        /* hal/mods.cpp */
int port_mods_relatch_forced(const char *reason);             /* hal/mods.cpp (SEAMCLEAN1) */
int port_mods_multiplayer(void);
int port_mods_count(void);
const char *port_mods_id(int i);
const char *port_mods_kind(int i);
void host_setting_adventure_env_reread(void);                 /* hal/host_settings.cpp */
int host_setting_lovesme_character(void);
int host_setting_voice_enabled(void);
void port_join_settings_rearm(void);                          /* hal/menu_join_plate.cpp */
extern int port_front_end_in_place;                           /* hal/level_change.cpp (SEAMQUIT1) */
int (*port_online_leave_restart_hook)(void) = 0;              /* tests/walk_window.cpp seats it */
}

namespace port {
struct CommsTransport;
const CommsTransport *comms_transport();   /* hal/comms_seam.h: the DS session carrier, null on the hub */
namespace {
int g_leave = 0;                      /* 0 none, 1 asked at the quit, 2 done at the title */
int g_leave_frame = 0;
void (*g_leave_prev_hook)(void) = 0;

/* a name set in either copy of the environment */
bool env_any(const char *n)
{
    const char *c = std::getenv(n);
    if (c && *c) return true;
    char v[4];
    return GetEnvironmentVariableA(n, v, sizeof v) > 0;
}

/* cleared: the hub is down and the record and the environment are cleared (a refusal of the mods relatch, or a later one), so the
   process is a solo title with the ONLINE mod list latched. If the restart then fails to start, the title must still be playable
   as the player left it: the offline list goes on anyway (SEAMCLEAN1; a code mod still cannot, and then the line says so). */
void leave_restart(const char *why, bool cleared = false)
{
    std::fprintf(stderr, "[seamless] in place refused: %s; restarting\n", why);
    std::fflush(stderr);
    bool started = false;
#ifndef PORT_ROM_CLEAN
    /* dev knob: the successor "could not be started" (row proof only) */
    const bool fake_fail = std::getenv("SM64DS_LEAVE_RESTART_FAIL") != nullptr;
    if (fake_fail) std::fprintf(stderr, "[seamless] SM64DS_LEAVE_RESTART_FAIL: the restart is not attempted\n");
    if (!fake_fail)
#endif
        started = port_online_leave_restart_hook && port_online_leave_restart_hook();
    if (!started) {
        std::fprintf(stderr, "[seamless] the restart could not be started (win32 %lu); this process stays on the title\n",
                     (unsigned long)GetLastError());
        std::fflush(stderr);
        if (cleared) {
            if (port_mods_relatch_forced("the restart could not be started"))
                std::fprintf(stderr, "[seamless] the title stays playable: the offline mod list is latched (%d mod(s) on)\n",
                             port_mods_count());
            else
                std::fprintf(stderr, "[seamless] the title stays on the online mod list (%d mod(s) on); close the game and start it "
                             "again for the full list\n", port_mods_count());
            std::fflush(stderr);
        }
    }
}

void leave_frame();

void leave_at_title()
{
    g_leave = 2;
    /* SEAMCLEAN1 (item 31): only if the hook is still this leave's own; a layer chained above it (the hand-off poll, a coop hook) is
       never dropped. leave_frame stays chained then and does nothing more (g_leave is 2), as the VS end's restore does. */
    if (port_sdat_frame_hook == leave_frame)
        port_sdat_frame_hook = g_leave_prev_hook;
    else
        std::fprintf(stderr, "[seamless] note: another frame hook was chained after the leave's; it stays chained\n");
    std::fprintf(stderr, "[seamless] the title is up (quit at f%d): leaving the online session in this process\n", g_leave_frame);
    if (!comms_hub_teardown("leaving the online world in place")) {
        leave_restart("the hub's I/O thread did not stop");
        return;
    }
    comms_sync_leave_reset();
    comms_conductor_leave_reset();
    hub_presence_leave_reset();
    const int fields = online_session_clear();
    std::string cleared;
    int names = 0;
    for (int pass = 0; pass < 2; ++pass) {
        const char *const *l = pass ? k_session_env : k_join_env;
        const size_t n = pass ? sizeof k_session_env / sizeof *k_session_env : sizeof k_join_env / sizeof *k_join_env;
        for (size_t i = 0; i < n; ++i) {
            if (!env_any(l[i])) continue;
            SetEnvironmentVariableA(l[i], nullptr);
            _putenv_s(l[i], "");
            if (names++) cleared += ' ';
            cleared += l[i] + 7;
        }
    }
    host_setting_adventure_env_reread();
    std::fprintf(stderr, "[seamless] session record cleared (%d field(s)); %d online name(s) cleared from both copies of the "
                 "environment: %s\n", fields, names, names ? cleared.c_str() : "-");
    std::fflush(stderr);
    if (!port_mods_relatch_after_level("leaving the online session")) {
        leave_restart("the mods layer refused the switch back to the offline list", true);
        return;
    }
    char id[200];
    comms_sync_identity_text(id, sizeof id);
    const int lm = host_setting_lovesme_character();
    std::fprintf(stderr, "[seamless] now solo: save medium %s; identity %s; lobby colours %s; Loves Me %d (%s); adventure %d; "
                 "multiplayer mod rule %d; %d mod(s) on; hub %s (install latch reset); in-place quit flag %d until a level is handed back\n",
                 ntr::backup::online_selected() ? "THE ONLINE SAVE" : "the single player save", id,
                 env_any("SM64DS_VS_COLORS") ? "SET" : "none (SM64DS_VS_COLORS unset, as a solo boot)",
                 lm, lm ? "settings.json, read the same as a solo boot" : "Yoshi, the ROM", adventure_ghost_mode() ? 1 : 0,
                 port_mods_multiplayer(), port_mods_count(), comms_hub_active() ? "STILL ACTIVE" : "off", port_front_end_in_place);
    std::fflush(stderr);
    port_join_settings_rearm();
}

void leave_frame()
{
    if (g_leave_prev_hook) g_leave_prev_hook();
    if (g_leave == 1 && port_scene_live_id() == 1) leave_at_title();
}
}  // namespace
}  // namespace port

/* 0 when this process is not an online session at all (the caller keeps its own answer); else 1, with *why 0 when the quit may
   leave in place, or the reason it may not. */
extern "C" int port_online_leave_refusal(const char **why)
{
    using namespace port;
    static char buf[200];
    *why = 0;
    const bool online = comms_hub_active() || online_session_get(kSessAdventure) || online_session_get(kSessRelay) ||
                        online_session_get(kSessHubPort);
    if (!online) return 0;
    /* SEAMCLEAN1: SEAMQUIT1's can-host-title check, which port_quit_in_place_refusal makes before this runs for an offline quit
       and which this test replaces wholesale: a build that does not host the title scene has no title to leave to */
    if (!port_scene_is_hosted(1)) { *why = "this build does not host the title scene"; return 1; }
    static const char *const k_carrier[] = { "SM64DS_VS_MAP", "SM64DS_VS_PLAYERS", "SM64DS_VS_NETMODE", "SM64DS_COMMS_ROLE",
        "SM64DS_COMMS_HOST", "SM64DS_COMMS_SLOT", "SM64DS_NETMODE", "SM64DS_PARTY", "SM64DS_COOP", "SM64DS_COOP_HOST" };
    for (const char *n : k_carrier)
        if (env_any(n)) {
            std::snprintf(buf, sizeof buf, "a VS, direct-carrier, party or coop session (%s is set)", n);
            *why = buf;
            return 1;
        }
    if (comms_hub_arena()) { *why = "a VS arena session"; return 1; }
    if (comms_hub_active() && comms_hub_room_kind() != kHubRoomWorld) { *why = "a party or coop room"; return 1; }
    if (comms_transport()) { *why = "a DS session transport is installed"; return 1; }
    if (env_any("SM64DS_VS_COLORS")) { *why = "the lobby colours were read once at boot (SM64DS_VS_COLORS)"; return 1; }
    if (env_any("SM64DS_SCENE")) { *why = "a scene-booted process"; return 1; }
    for (int i = 0; i < port_mods_count(); ++i) {
        const char *k = port_mods_kind(i);
        if (k && !std::strcmp(k, "code")) {
            std::snprintf(buf, sizeof buf, "a code mod is loaded (%s) and a plugin cannot be unloaded", port_mods_id(i));
            *why = buf;
            return 1;
        }
    }
    {   /* voice chat keeps its own device and thread (hal/voice_chat.cpp); this leave does not tear them down */
        const char *va = online_session_get(kSessVoiceAllowed);
        if (host_setting_voice_enabled() && !(va && !std::strcmp(va, "0"))) { *why = "voice chat is on for this session"; return 1; }
    }
    if (!port_online_leave_restart_hook) { *why = "this build has no restart for a refusal at the title"; return 1; }
    if (g_leave) { *why = "a leave is already under way"; return 1; }
    return 1;
}

extern "C" void port_online_leave_begin(int frame)
{
    using namespace port;
    g_leave = 1;
    g_leave_frame = frame;
    ntr::backup::select_online(0);
    comms_hub_reports_off();
    comms_sync_reports_off();
    g_leave_prev_hook = port_sdat_frame_hook;
    port_sdat_frame_hook = leave_frame;
    std::fprintf(stderr, "[seamless] f%d leaving the online world in this process: the hub stays up through the fade, the title "
                 "tears it down\n", frame);
    std::fflush(stderr);
}

/* ==== SEAMJOIN1: JOINING AN ONLINE WORLD IN PLACE (run after093; the one-process plan's M4b) ====================================
   Picking a file on the online file select asked the launcher for a server and, with the plan, started the game again (apply_plan's
   names, the front door): the successor installed the hub in main(), walked the title hidden behind black, pressed the same file through
   the ROM's own file button handler (hal/title_entry.cpp port_title_skip_tick: func_ov007_020b6134) and fell through to the level boot.
   This process now does the same itself, from the file select it is already on:

     1. the session record is filled from the plan (not from the environment; hal/comms_hub.h), and the same names go into both copies
        of the environment (the CRT's, which the readers outside the hub layer still take through getenv: the mod rule in hal/mods.cpp,
        the Adventure flag in hal/host_settings.cpp, the online save in ntr/backup.cpp, the name and colour in the chat and list
        overlays; and the Win32 block, which a later restart such as a character change hands on as before). The Adventure flag's env
        latch is read again;
     2. the mods are latched again under the multiplayer rule (hal/mods.cpp: the Online Compatible mods kept, every other mod off); at a
        title reached after a level (a second join in this process) the files a level read count as held;
     3. the save medium is the online save (ntr/backup.cpp select_online; the online page already chose it);
     4. the hub is installed live from the record (comms_hub_install_in_place: the socket, the I/O thread; presence registers on its
        first live frame), and the ghost layer is reset and decided again on the hub (comms_sync_join_in_place: the identity, the own
        colour row and the 32 bodies the level boot seats come from the record, as in a successor);
     5. the Joining plate keeps JOINWIN1's look: black until the level, the corner line after 3 s, muted; B / Esc cancels IN PLACE;
     6. once the relay has given a seat and 1 s has passed (so B / Esc on the black can still cancel; without a seat 5 s, about what a
        successor's hidden title walk takes) and the file select is
        settled, the file is pressed through func_ov007_020b6134, the ROM's own handler for the A / B / C buttons: the same call, on the
        same file select, that the successor makes, and from there dScDSMT_c's func_02013c84 and StartFile take the game into the world
        exactly as in the successor (the title's fall-through to the level boot on the first title of the process, the level-to-scene
        crossing on a title reached by an in-place quit).
   The way out is SEAMHUB1's leave, unchanged. EVERYTHING THAT CANNOT BE DONE IN PLACE REFUSES BEFORE ANYTHING IS TOUCHED and takes the
   restart exactly as before, with one line ("[seamless] in place refused: <why>; restarting"): a scene-booted process, a VS / arena /
   direct-carrier / party / coop session (M5), the lobby colours, a loaded code mod (a plugin cannot be unloaded), a hub (or its thread)
   of an earlier session still up, a leave still under way, a title that is not the live scene, and a game heap carved for a solo game
   (main's level boot carves it once, with the hub's 128 KB for 32 bodies only when the hub is installed; a solo level played first
   leaves it too small). A step that fails after the record is filled (the relatch refuses, the hub does not come up, the ghost layer does
   not decide on) undoes what it did and refuses the same way. */
extern "C" {
extern unsigned short data_02092664;                          /* Scene::SetSceneToSpawn's pending id */
extern unsigned char data_02092660;                           /* its "already spawned" latch */
extern char *data_ov007_0210342c;                             /* dScDSMT_c's scene global (hal/title_entry.cpp reads it the same way) */
void func_ov007_020b6134(int slot);                           /* src/unnamed/ov007/func_ov007_020b6134.cpp: the file select's A / B / C button handler */
int port_mods_relatch_at_title(const char *reason);           /* hal/mods.cpp */
void port_join_plate_raise_in_place(int server);              /* hal/menu_join_plate.cpp */
int port_join_plate_inplace_tick(void);
int port_join_plate_inplace_up(void);
void port_join_plate_inplace_end(const char *why);
}

namespace port {
void join_in_place_cancel(const char *why);
void event_client_hub_reset();    /* hal/event_client.cpp: its hub event handler is registered again on the new hub */
void star_tally_session_reset();  /* hal/star_tally.cpp: the name hash and tables of the new session */
namespace {
enum { kIpNone = 0, kIpWaiting = 1, kIpPressed = 2, kIpEntered = 3, kIpAtTitle = 4 };   /* 4: SEAMVS1, joined at the title, no press */
const ULONGLONG kIpSeatMs = 5000;       /* press without a seat after this long (the successor never waited for one) */
const ULONGLONG kIpHoldMs = 1000;       /* and never sooner than this: B / Esc on the black can still cancel (a successor's black lasts several seconds) */
const ULONGLONG kIpSettleMs = 15000;    /* the file select must be settled by then, or the join is withdrawn */
int g_ip = kIpNone;
ULONGLONG g_ip_t0 = 0;
bool g_ip_after_level = false;
bool g_ip_late_said = false;
void (*g_ip_prev_hook)(void) = 0;

const char *join_refusal()
{
    static char buf[200];
    if (g_leave == 1) return "the last online session's leave is still under way";
    if (env_any("SM64DS_SCENE")) return "a scene-booted process";
    static const char *const k_carrier[] = { "SM64DS_VS_MAP", "SM64DS_VS_PLAYERS", "SM64DS_VS_NETMODE", "SM64DS_COMMS_ROLE",
        "SM64DS_COMMS_HOST", "SM64DS_COMMS_SLOT", "SM64DS_NETMODE", "SM64DS_PARTY", "SM64DS_COOP", "SM64DS_COOP_HOST" };
    for (const char *n : k_carrier)
        if (env_any(n)) {
            std::snprintf(buf, sizeof buf, "a VS, direct-carrier, party or coop session (%s is set)", n);
            return buf;
        }
    if (comms_hub_arena()) return "a VS arena session";
    if (comms_transport()) return "a DS session transport is installed";
    if (comms_hub_active()) return "a hub of an earlier session is still installed";
    if (env_any("SM64DS_VS_COLORS")) return "the lobby colours were read once at boot (SM64DS_VS_COLORS)";
    for (int i = 0; i < port_mods_count(); ++i) {
        const char *k = port_mods_kind(i);
        if (k && !std::strcmp(k, "code")) {
            std::snprintf(buf, sizeof buf, "a code mod is loaded (%s) and a plugin cannot be unloaded", port_mods_id(i));
            return buf;
        }
    }
    if (port_scene_live_id() != 1) return "the title is not the live scene";
    unsigned extra = 0;
    if (hub_game_heap_carved(&extra) && extra < hub_heap_need()) {
        std::snprintf(buf, sizeof buf, "the game heap was carved for a solo game (0x%x bytes over the ROM's 0x3b000; 32 bodies need 0x%x): "
                      "a solo level was played before this join", extra, hub_heap_need());
        return buf;
    }
    return 0;
}

/* one name into both copies of the environment (null = out of both) */
void env_put(const char *n, const char *v)
{
    SetEnvironmentVariableA(n, v);
    _putenv_s(n, v ? v : "");
}

/* the record and both environment copies back to a process that never joined */
void ip_undo_env()
{
    online_session_clear();
    for (const char *n : k_join_env) env_put(n, nullptr);
    host_setting_adventure_env_reread();
}

int relatch(const char *why)
{
    return g_ip_after_level ? port_mods_relatch_after_level(why) : port_mods_relatch_at_title(why);
}

/* the file select is up and settled (the same test hal/title_entry.cpp port_title_skip_tick makes before its press) */
bool file_select_settled(int *state, int *elem)
{
    *state = *elem = -1;
    char *g = data_ov007_0210342c;
    if (!g) return false;
    char *sp = *(char **)(g + 8);
    char *ep = *(char **)(g + 4);
    if (!sp || !ep) return false;
    *state = *(short *)sp;
    *elem = *(short *)ep;
    return *(short *)sp == 0 && *(short *)(sp + 2) == -1 && *(short *)ep == 5 && *(short *)(ep + 2) == -1 && *(int *)(sp + 0xc) >= 8;
}

void ip_frame();

void ip_unchain()
{
    if (port_sdat_frame_hook == ip_frame) port_sdat_frame_hook = g_ip_prev_hook;
    g_ip_prev_hook = 0;
}

/* the hub and the ghost layer out again, the record and the environment cleared, the mods back on the offline list (a refusal there
   restarts, as the leave does) */
void ip_take_down(const char *why)
{
    if (comms_hub_active() || comms_hub_state() != kHubOff) comms_hub_teardown(why);
    comms_sync_leave_reset();
    comms_conductor_leave_reset();
    hub_presence_leave_reset();
    ip_undo_env();
    if (!relatch("the online join did not go through"))
        leave_restart("the mods layer refused the switch back to the offline list", true);
}

void ip_frame()
{
    if (g_ip_prev_hook) g_ip_prev_hook();
    if (g_ip == kIpWaiting) {
        if (port_join_plate_inplace_tick()) { join_in_place_cancel("the player cancelled on the Joining plate"); return; }
        const ULONGLONG el = GetTickCount64() - g_ip_t0;
        const bool seated = comms_hub_state() == kHubLive;
        if (el < kIpHoldMs || (!seated && el < kIpSeatMs)) return;
        int st = -1, el5 = -1;
        if (!file_select_settled(&st, &el5)) {
            if (el >= kIpSettleMs) join_in_place_cancel("the file select did not settle for the press");
            return;
        }
        g_ip = kIpPressed;
        func_ov007_020b6134(g_file);
        g_state = kJoinEntering;
        std::fprintf(stderr, "[seamless] pressed file %c on this file select through the ROM's own func_ov007_020b6134(%d), as a "
                     "successor's title walk does (%s after %llu ms, seat %d); in-place quit flag %d\n", 'A' + g_file, g_file,
                     seated ? "the relay gave a seat" : "no seat yet", (unsigned long long)el, comms_hub_my_seat(),
                     port_front_end_in_place);
        std::fflush(stderr);
        return;
    }
    if (g_ip == kIpPressed) {
        if (port_join_plate_inplace_tick() && !g_ip_late_said) {
            g_ip_late_said = true;
            std::fprintf(stderr, "[seamless] cancel asked after the file was pressed: the world is already loading; the way out is "
                         "Save and Quit\n");
        }
        /* the title has finished tearing down and the Stage is asked for: the moment the title's own commit (the first title of the
           process) or the level-to-scene crossing (a title reached by an in-place quit) hands the game to the level path */
        const bool commit_lowered = !port_join_plate_inplace_up();
        if (commit_lowered || (data_02092664 == 3 && data_02092660 == 0)) {
            if (!commit_lowered) port_join_plate_inplace_end("the adventure is entered (the title handed the game to the level)");
            g_ip = kIpEntered;
            ip_unchain();
            char id[200];
            comms_sync_identity_text(id, sizeof id);
            std::fprintf(stderr, "[seamless] joined in this process: the title handed the game to the world (%s); hub state %d seat %d; "
                         "identity %s; adventure %d; multiplayer mod rule %d, %d mod(s) on; save medium %s; in-place quit flag %d\n",
                         commit_lowered ? "the title's own commit lowered the plate" : "the Stage is asked for, the title is gone", comms_hub_state(),
                         comms_hub_my_seat(), id, adventure_ghost_mode() ? 1 : 0, port_mods_multiplayer(), port_mods_count(),
                         ntr::backup::online_selected() ? "the online save" : "THE SINGLE PLAYER SAVE", port_front_end_in_place);
            std::fflush(stderr);
        }
    }
}

const char *join_in_place(const JoinPlan &p)
{
    if (const char *why = join_refusal()) return why;
    unsigned extra = 0;
    g_ip_after_level = hub_game_heap_carved(&extra);
    std::fprintf(stderr, "[seamless] joining server %d in this process (file %c, %s; in-place quit flag %d)\n", p.server, p.no_press ? '-' : 'A' + g_file,
                 g_ip_after_level ? "a title reached after a level" : "the first title of the process", port_front_end_in_place);

    /* 1. the record from the plan, and the same names in both environment copies */
    char seat[8] = "", world[8] = "";
    if (p.seat != 255) std::snprintf(seat, sizeof seat, "%lld", p.seat);
    if (p.server > 0 && p.server < 1000) std::snprintf(world, sizeof world, "%d", p.server);
    struct F { OnlineSessionField f; const char *v; } fields[] = {
        { kSessAdventure, "1" }, { kSessRelay, p.relay.c_str() }, { kSessCode, p.code.c_str() }, { kSessRoomKind, "world" },
        { kSessVoiceAllowed, "0" }, { kSessHubSeat, seat }, { kSessWorldNo, world },
        { kSessPlayerName, p.has_name ? p.name.c_str() : "" },
        { kSessPlayerColor, p.has_cols ? p.color.c_str() : "" }, { kSessPlayerShoes, p.has_cols ? p.shoes.c_str() : "" } };
    online_session_begin();
    int nset = 0;
    for (const F &f : fields) {
        online_session_set(f.f, f.v);
        env_put(online_session_name(f.f), f.v && *f.v ? f.v : nullptr);
        if (f.v && *f.v) ++nset;
    }
    host_setting_adventure_env_reread();
    std::fprintf(stderr, "[seamless] session record filled from the plan (%d field(s): relay withheld, code %s, seat %s, world %s, "
                 "name \"%s\", colours %s / %s); the same names in both environment copies; adventure %d, multiplayer mod rule %d\n",
                 nset, p.code.c_str(), seat[0] ? seat : "any", world[0] ? world : "-", p.has_name ? p.name.c_str() : "",
                 p.has_cols ? p.color.c_str() : "-", p.has_cols ? p.shoes.c_str() : "-", adventure_ghost_mode() ? 1 : 0,
                 port_mods_multiplayer());

    /* 2. the mods under the multiplayer rule */
    if (!relatch("joining an online world")) {
        ip_undo_env();
        return "the mods layer refused the switch to the online list";
    }
    /* 3. the online save */
    ntr::backup::select_online(1);
    /* 4. the hub, then the ghost layer on it */
    comms_conductor_leave_reset();
    hub_presence_leave_reset();
    event_client_hub_reset();
    star_tally_session_reset();
    const char *hwhy = 0;
    if (!comms_hub_install_in_place(&hwhy)) {
        static char buf[200];
        std::snprintf(buf, sizeof buf, "the hub could not be installed in place (%s)", hwhy ? hwhy : "?");
        ip_undo_env();
        if (!relatch("the online join did not go through"))
            std::fprintf(stderr, "[seamless] the mods layer refused the switch back to the offline list; the restart takes over\n");
        return buf;
    }
    if (!comms_sync_join_in_place()) {
        comms_hub_teardown("the ghost layer did not decide on");
        comms_sync_leave_reset();
        ip_undo_env();
        if (!relatch("the online join did not go through"))
            std::fprintf(stderr, "[seamless] the mods layer refused the switch back to the offline list; the restart takes over\n");
        return "the ghost layer did not come up on the hub";
    }
    /* SEAMVS1: a launcher hand-off at the title ends here, where a launcher-started online process stands once main() has installed
       the hub: the title up with the hub, the online save and the multiplayer mods, and the player picks the file (no plate, no press) */
    if (p.no_press) {
        port_join_settings_rearm();
        g_leave = 0;
        g_ip = kIpAtTitle;
        std::fprintf(stderr, "[seamless] joined at the title in place: hub up, ghosts on the hub, the online save, %d mod(s) on; the player "
                     "picks the file as in a launcher-started online game\n", port_mods_count());
        std::fflush(stderr);
        return 0;
    }
    /* 5. the plate, the settings line for the world, the leave armed again */
    port_join_plate_raise_in_place(p.server);
    port_join_settings_rearm();
    g_leave = 0;
    g_ip = kIpWaiting;
    g_ip_t0 = GetTickCount64();
    g_ip_late_said = false;
    if (port_sdat_frame_hook != ip_frame) {
        g_ip_prev_hook = port_sdat_frame_hook;
        port_sdat_frame_hook = ip_frame;
    }
    std::fprintf(stderr, "[seamless] joining in place: hub up, ghosts on the hub, the online save, %d mod(s) on; the file is pressed "
                 "once the relay gives a seat (not before %u ms, at most %u s)\n", port_mods_count(), (unsigned)kIpHoldMs,
                 (unsigned)(kIpSeatMs / 1000));
    std::fflush(stderr);
    return 0;
}

/* 0 = no in-place join in this session (the restart path's rules apply); 1 waiting, 2 pressed, 3 entered (online_join_begin
   resets it for the next join) */
int join_in_place_phase() { return g_ip; }

/* ==== SEAMVS1: THE LAUNCHER HAND-OFF'S "adventure" KIND (hal/handoff_poll.h) ==================================================
   The launcher's online start, handed to a game that is already running at its title (HANDOFF_CONTRACT v1): the request carries
   exactly the names the launcher's BuildAdventureEnv would start a process with. A process started with them installs the hub in main()
   (comms_hub_install_from_env), comes up at the title with the online save, the multiplayer mods and the hub's identity, and the player
   picks a file. In place: the clear list and the set list go into both environment copies (port_handoff_apply_env), and the plan built
   from the same names goes through SEAMJOIN1's join_in_place with no press, which fills the record, relatches the mods under the
   multiplayer rule, switches to the online save, installs the hub live and decides the ghost layer on it. A request that names a file
   (SM64DS_BOOT_FILE, the coop shape) is pressed the way SEAMJOIN1's in-game join presses it. NEEDS THE ONLINE HEAP: join_in_place
   refuses at a title reached after a solo level (the game heap was carved without the hub's 128 KB), and the launcher then starts a
   fresh game. A party or coop room is not this kind's: refused. */
const char *handoff_adventure(const PortHandoffRequest *r)
{
    static char buf[200];
    const char *relay = port_handoff_value(r, "SM64DS_COMMS_RELAY"), *code = port_handoff_value(r, "SM64DS_COMMS_CODE");
    const char *rk = port_handoff_value(r, "SM64DS_ROOM_KIND"), *party = port_handoff_value(r, "SM64DS_PARTY");
    const char *adv = port_handoff_value(r, "SM64DS_ADVENTURE");
    if (!adv || std::strcmp(adv, "1")) return "the adventure request does not set SM64DS_ADVENTURE=1";
    if (!relay || !*relay || !code || !*code) return "the adventure request names no relay or no hub code";
    if (!printable(relay, 1, 253) || !printable(code, 1, 8)) return "the adventure request's relay or code does not read";
    if ((rk && std::strcmp(rk, "world")) || (party && *party && std::strcmp(party, "0"))) {
        std::snprintf(buf, sizeof buf, "a %s room is not joined in place (SM64DS_ROOM_KIND=%s)", rk ? rk : "party", rk ? rk : "-");
        return buf;
    }
    JoinPlan jp;
    jp.relay = relay;
    jp.code = code;
    jp.seat = 255;
    if (const char *s = port_handoff_value(r, "SM64DS_HUB_SEAT")) {
        const int v = std::atoi(s);
        if (*s && v >= 0 && v < 32) jp.seat = v;
    }
    jp.server = 0;
    if (const char *w = port_handoff_value(r, "SM64DS_WORLD_NO")) jp.server = std::atoi(w);
    const char *nm = port_handoff_value(r, "SM64DS_PLAYER_NAME");
    jp.has_name = nm && *nm;
    if (jp.has_name) jp.name = nm;
    const char *col = port_handoff_value(r, "SM64DS_PLAYER_COLOR"), *sh = port_handoff_value(r, "SM64DS_PLAYER_SHOES");
    jp.has_cols = col && sh && hex6(col) && hex6(sh);
    if (jp.has_cols) { jp.color = col; jp.shoes = sh; }
    const char *bf = port_handoff_value(r, "SM64DS_BOOT_FILE");
    jp.no_press = !(bf && bf[0] >= '0' && bf[0] <= '2' && !bf[1]);
    if (!jp.no_press) g_file = bf[0] - '0';
    if (const char *why = join_refusal()) return why;   /* before the environment is touched */
    port_handoff_apply_env(r);
    const char *why = join_in_place(jp);
    if (!why && !jp.no_press) g_state = kJoinWaiting;
    return why;
}
const int g_handoff_adventure_seated = port_handoff_register("adventure", handoff_adventure);
}  // namespace

/* the cancel of an in-place join before the file is pressed: the plate down, the session taken down in this process, the online menu
   shown the sentence (its Back lands on the online file select, where a cancelled wait has always landed) */
void join_in_place_cancel(const char *why)
{
    if (g_ip != kIpWaiting) return;
    g_ip = kIpNone;
    ip_unchain();
    port_join_plate_inplace_end(why);
    std::fprintf(stderr, "[seamless] join cancelled in place (%s): taking the session down in this process\n", why);
    ip_take_down("the in-place join was cancelled");
    DeleteFileA(req_path().c_str());
    DeleteFileA(plan_path().c_str());
    fail("Joining was cancelled.");
    std::fprintf(stderr, "[seamless] back on the online file select: hub %s, adventure %d, multiplayer mod rule %d, %d mod(s) on, "
                 "save medium %s; in-place quit flag %d\n", comms_hub_active() ? "STILL ACTIVE" : "off",
                 adventure_ghost_mode() ? 1 : 0, port_mods_multiplayer(), port_mods_count(),
                 ntr::backup::online_selected() ? "the online save (this page's)" : "the single player save", port_front_end_in_place);
    std::fflush(stderr);
}

}  // namespace port

namespace port {

bool online_join_begin(int file, const char *nick) {
    if (file < 0 || file > 2) file = 0;
    if (join_in_place_phase() == 1 || join_in_place_phase() == 2) {   /* SEAMJOIN1: one join at a time */
        fail("A join is already under way.");
        return false;
    }
    g_ip = kIpNone;   /* SEAMJOIN1: a new join: the last in-place join (if any) is history */
    g_file = file;
    g_sentence.clear();
    CreateDirectoryA(folder().c_str(), nullptr);
    DeleteFileA(plan_path().c_str());
    DeleteFileA(req_path().c_str());
    std::string n;
    for (const char *p = nick ? nick : ""; *p && n.size() < 16; ++p)
        if ((unsigned char)*p >= 0x20 && (unsigned char)*p < 0x7F && *p != '"' && *p != '\\' && *p != ',') n += *p;
    /* An empty nick stays empty: the launcher then fills in the nickname the player set there (JoinHelper.HandleAsync). */
    g_stamp = now_ms();
    char buf[256];
    std::snprintf(buf, sizeof buf, "{\"v\":1,\"mode\":\"adventure\",\"file\":%d,\"nick\":\"%s\",\"stamp\":%llu}\n", file, n.c_str(), g_stamp);
    const std::string tmp = req_path() + ".tmp";
    FILE *f = std::fopen(tmp.c_str(), "wb");
    if (!f) { fail("The game could not ask for a server. Check that its save folder can be written."); return false; }
    std::fputs(buf, f);
    std::fclose(f);
    if (!MoveFileExA(tmp.c_str(), req_path().c_str(), MOVEFILE_REPLACE_EXISTING)) {
        fail("The game could not ask for a server. Check that its save folder can be written.");
        return false;
    }
    g_state = kJoinWaiting;
    g_t0 = GetTickCount64();
    g_last_poll = 0;
    std::fprintf(stderr, "[join] request written: file %d, nick \"%s\", stamp %llu\n", file, n.c_str(), g_stamp);
    std::fflush(stderr);
    return true;
}

OnlineJoinState online_join_step() {
    if (g_state != kJoinWaiting) return g_state;
    if (join_in_place_phase()) return g_state;   /* SEAMJOIN1: the plan is applied; the in-place join's frame step takes it from here */
    const ULONGLONG now = GetTickCount64();
    if (now - g_last_poll < kPollMs) return g_state;
    g_last_poll = now;
    std::string t;
    if (read_file(plan_path(), &t)) {
        long long stamp = 0;
        if (jnum(t, "stamp", &stamp) && (unsigned long long)stamp == g_stamp) {
            DeleteFileA(plan_path().c_str());
            bool ok = false;
            long long server = 0;
            jnum(t, "server", &server);
            if (jbool(t, "ok", &ok) && ok) {
                apply_plan(t, (int)server);
            } else {
                std::string why;
                if (!jstr(t, "why", &why) || why.empty() || why.size() > 160) why = "No server could be found.";
                /* the lobby's refusal of the name itself (the launcher's bad_nick sentence): say where to fix it */
                else if (why.compare(0, 10, "That name ") == 0 && why.find("lobby will not take") != std::string::npos)
                    why = "The lobby refused that nickname. Set one in the launcher's settings.";
                fail(why.c_str());
            }
            std::fprintf(stderr, "[join] plan %s\n", join_in_place_phase() ? "accepted: joining in this process"
                                                    : g_state == kJoinEntering ? "accepted" : g_sentence.c_str());
            std::fflush(stderr);
            return g_state;
        }
        /* stale plan (another stamp): ignored, left for the next begin to replace */
    }
    if (now - g_t0 >= kWaitMs) {
        DeleteFileA(req_path().c_str());
        DeleteFileA(plan_path().c_str());
        fail("No launcher answered. Start the game from the launcher to play online.");
        std::fprintf(stderr, "[join] timed out after 20 s: %s\n", g_sentence.c_str());
        std::fflush(stderr);
    }
    return g_state;
}

void join_in_place_cancel(const char *why);   /* SEAMJOIN1, above */

void online_join_cancel() {
    if (g_state == kJoinWaiting && join_in_place_phase() == 1) { join_in_place_cancel("the online menu withdrew the wait"); return; }
    if (g_state == kJoinWaiting) {
        DeleteFileA(req_path().c_str());
        DeleteFileA(plan_path().c_str());
        g_state = kJoinCancelled;
        std::fprintf(stderr, "[join] cancelled\n");
        std::fflush(stderr);
    }
}

OnlineJoinState online_join_state() { return g_state; }
const char *online_join_sentence() { return g_sentence.c_str(); }
int online_join_seconds_left() {
    if (g_state != kJoinWaiting) return 0;
    const ULONGLONG el = GetTickCount64() - g_t0;
    return el >= kWaitMs ? 0 : (int)((kWaitMs - el + 999) / 1000);
}

}  // namespace port

/* ==== SEAMVS1: what the hand-off poll and the VS match end share with the online session code =================================== */
/* 1 when an online leave or join is under way in this process (*why says which): the hand-off poll waits */
extern "C" int port_online_handoff_busy(const char **why)
{
    using namespace port;
    *why = 0;
    if (g_leave == 1) { *why = "an online leave is under way"; return 1; }
    const int ph = join_in_place_phase();
    if (ph == 1 || ph == 2) { *why = "an online join is under way"; return 1; }
    if (online_join_state() == kJoinWaiting) { *why = "the online menu is waiting for the launcher's plan"; return 1; }
    return 0;
}

/* The session record cleared and every session name this process may hold (the join's 12 and the launcher's 31: the list the restart
   clears) out of BOTH copies of the environment; the Adventure flag's env latch read again. Returns how many names were set; names
   (cap bytes) lists them without the SM64DS_ prefix; *fields is how many record fields were set. Game thread, at the title. */
extern "C" int port_session_env_clear(int *fields, char *names, int cap)
{
    using namespace port;
    *fields = online_session_clear();
    std::string cleared;
    int n = 0;
    for (int pass = 0; pass < 2; ++pass) {
        const char *const *l = pass ? k_session_env : k_join_env;
        const size_t c = pass ? sizeof k_session_env / sizeof *k_session_env : sizeof k_join_env / sizeof *k_join_env;
        for (size_t i = 0; i < c; ++i) {
            if (!env_any(l[i])) continue;
            SetEnvironmentVariableA(l[i], nullptr);
            _putenv_s(l[i], "");
            if (n++) cleared += ' ';
            cleared += l[i] + 7;
        }
    }
    host_setting_adventure_env_reread();
    if (names && cap > 0) std::snprintf(names, (size_t)cap, "%s", n ? cleared.c_str() : "-");
    return n;
}

/* How many names that clear list holds (the 43 the VS match end's line prints). */
extern "C" int port_session_env_list_size(void)
{
    using namespace port;
    return (int)(sizeof k_join_env / sizeof *k_join_env + sizeof k_session_env / sizeof *k_session_env);
}

/* The restart a title-time refusal takes (SEAMHUB1's leave_restart: the front door, the hand-off wait, this process ends). */
extern "C" void port_session_restart(const char *why) { port::leave_restart(why); }
