/* hal/rich_presence.cpp -- Discord Rich Presence for the real window.

   WHAT A PLAYER SEES. While Discord is running on the same PC, the player's
   profile says what the game is doing:
       details  the course name, "In the menus" or "Playing a minigame"
       state    the mode ("Solo", "Adventure Online, server 2", "VS", "Coop",
                "Party") and, in a course outside VS, the star count
   and a timer for how long the game has been open. Nothing else leaves the
   machine: no nickname, no room code, no relay address.

   HOW. discord_presence.cpp (SahilKDas, PR 3569) speaks Discord's local pipe
   (\\.\pipe\discord-ipc-N). No library, no DLL: plain Win32 file calls, so the
   kit's file list does not change. This file owns the when and the what.

   A WORKER THREAD DOES ALL OF IT. The game thread only stores the scene it
   crossed into (port_rich_presence_scene, one atomic store). The thread wakes
   twice a second, reads the level id (data_0209f2f8, what the game itself keys
   level changes on) and the save's star total (NumStars, the number the HUD
   prints), and sends when something changed. Pipe calls can block (Discord
   stalled, a handshake that never answers), and none of that may cost a game
   frame, so none of it runs on the game thread.

   THE RATE RULE. Discord allows one presence update per 15 seconds. At most one
   is sent per 15 s; a change inside the window waits for the window to end and
   then sends the latest state only.

   DISCORD NOT RUNNING. Connecting is one failed open of ten pipe names: no
   error shown, one log line, and the next look is 60 s later (not a loop). If
   Discord is started later the next look finds it. A Discord that answers the
   handshake with a refusal is looked at again only every 10 minutes.

   OFF SWITCHES. settings.json RichPresence (the launcher's checkbox, ON when
   absent) and SM64DS_RICH_PRESENCE=0. Test and scripted runs (no focus, minimized,
   selftest, scripted pad, no dialog) never post to a player's profile unless
   SM64DS_RICH_PRESENCE=1 asks for it.

   THE APPLICATION ID is the Discord application the presence belongs to; its
   name is what Discord prints ("Playing Super Mario 64 DS"). kApplicationId
   below is that id. Empty means presence stays off. SM64DS_RICH_PRESENCE_APP
   overrides it for a test and SM64DS_RICH_PRESENCE_PIPE points the pipe name
   prefix at a stand-in. */

#include "discord_presence.h"
#include "host_settings.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>

extern signed char data_0209f2f8;          /* the level this console is in */
unsigned char NumStars(void);              /* the save's star total (HUD::RenderStarCount) */
extern "C" int port_scene_is_menu(int scene);   /* hal/scene_boot.cpp */

namespace {

/* The Discord application. Not a secret: it is public by design (it is in every
   game that shows presence). Set from the Discord developer portal. */
constexpr const char *kApplicationId = "1563680817765683321";

constexpr ULONGLONG kMinSendMs = 15000;       /* Discord's rule */
constexpr ULONGLONG kRetryMs = 60000;         /* Discord not running */
constexpr ULONGLONG kRefusedRetryMs = 600000; /* Discord said no */
constexpr DWORD kPollMs = 500;

std::atomic<int> g_scene{-2};   /* -2 nothing yet, -1 a course, else the menu / minigame scene id */
std::atomic<int> g_started{0};

struct CourseName { int id; const char *name; };
/* retail course names by ROM level id, as the cartridge's own course list
   spells them (data/message/msg_data_eng.bin, the block from " 1 BOB-OMB
   BATTLEFIELD"; course per level from SUBLEVEL_LEVEL_TABLE at 0x02075298) (the same ids as walk_window.cpp's debug
   level select, whose derivation is written beside port_level_table[] in
   hal/level_boot.cpp). An id not listed says "In a course". */
const CourseName kCourses[] = {
    {  1, "Castle Grounds" },
    {  2, "Castle, First Floor" },
    {  3, "Castle Garden" },
    {  4, "Castle Basement" },
    {  5, "Castle, Second Floor" },
    {  6, "Bob-omb Battlefield" },
    {  7, "Whomp's Fortress" },
    {  8, "Jolly Roger Bay" },
    {  9, "Jolly Roger Bay, Sunken Ship" },
    { 10, "Cool, Cool Mountain" },
    { 11, "Cool, Cool Mountain Slide" },
    { 12, "Big Boo's Haunt" },
    { 13, "Hazy Maze Cave" },
    { 14, "Lethal Lava Land" },
    { 15, "Lethal Lava Land, Volcano" },
    { 16, "Shifting Sand Land" },
    { 17, "Shifting Sand Land, Pyramid" },
    { 18, "Dire, Dire Docks" },
    { 19, "Snowman's Land" },
    { 20, "Snowman's Land, Igloo" },
    { 21, "Wet-Dry World" },
    { 22, "Tall, Tall Mountain" },
    { 23, "Tall, Tall Mountain Slide" },
    { 24, "Tiny-Huge Island, Huge" },
    { 25, "Tiny-Huge Island, Tiny" },
    { 26, "Tiny-Huge Island, Cave" },
    { 27, "Tick Tock Clock" },
    { 28, "Rainbow Ride" },
    { 30, "The Secret Aquarium" },
    { 31, "? Switch" },
    { 32, "The Secret Under the Moat" },
    { 33, "Behind the Waterfall" },
    { 34, "Over the Rainbows" },
    { 35, "Bowser in the Dark World" },
    { 36, "Bowser in the Dark World, Boss" },
    { 37, "Bowser in the Fire Sea" },
    { 38, "Bowser in the Fire Sea, Boss" },
    { 39, "Bowser in the Sky" },
    { 40, "Bowser in the Sky, Boss" },
    { 44, "Goomboss Battle" },
    { 45, "Goomboss Battle, Boss" },
    { 46, "Big Boo Battle" },
    { 47, "Big Boo Battle, Boss" },
    { 48, "Chief Chilly Challenge" },
    { 49, "Chief Chilly Challenge, Boss" },
    { 50, "Rec Room" },
};

const char *course_name(int id)
{
    for (const CourseName &c : kCourses)
        if (c.id == id)
            return c.name;
    return nullptr;
}

const char *env(const char *name)
{
    const char *e = std::getenv(name);
    return e && *e ? e : nullptr;
}

/* The mode, from what the launcher started the game with. It never changes
   during a run, so it is read once. The world number comes from the launcher
   (SM64DS_WORLD_NO) and nothing else about the room is read. */
std::string mode_text(bool *is_vs)
{
    *is_vs = false;
    const bool online = env("SM64DS_COMMS_RELAY") != nullptr;
    if (online && env("SM64DS_VS_MAP")) {
        *is_vs = true;
        return "VS";
    }
    if (online && env("SM64DS_ADVENTURE")) {
        const char *kind = env("SM64DS_ROOM_KIND");
        if (kind && !std::strcmp(kind, "coop"))
            return "Coop";
        if (kind && !std::strcmp(kind, "party"))
            return "Party";
        std::string s = "Adventure Online";
        if (const char *w = env("SM64DS_WORLD_NO")) {
            const int n = std::atoi(w);
            if (n > 0 && n < 1000)
                s += ", server " + std::to_string(n);
        }
        return s;
    }
    return "Solo";
}

bool wanted()
{
    if (!host_setting_rich_presence())
        return false;                       /* the setting or SM64DS_RICH_PRESENCE=0 */
    const char *force = env("SM64DS_RICH_PRESENCE");
    if (force && *force == '1')
        return true;
    static const char *const kTestKnobs[] = {
        "SM64DS_NO_FOCUS", "SM64DS_MINIMIZED", "SM64DS_WINDOW_SELFTEST",
        "SM64DS_NO_DIALOG", "SM64DS_HOST_PAD", "SM64DS_PROBE_INPUT",
    };
    for (const char *k : kTestKnobs)
        if (env(k))
            return false;
    return true;
}

double since(ULONGLONG t0)
{
    return (double)(GetTickCount64() - t0) / 1000.0;
}

DWORD WINAPI presence_thread(LPVOID)
{
    using namespace sm64ds::discord;
    const char *app = env("SM64DS_RICH_PRESENCE_APP");
    if (!app)
        app = kApplicationId;
    if (!*app) {
        std::fprintf(stderr, "[rpc] no Discord application id built in: presence stays off\n");
        return 0;
    }
    const char *pipe = env("SM64DS_RICH_PRESENCE_PIPE");
    bool is_vs = false;
    const std::string mode = mode_text(&is_vs);
    const long long start = (long long)std::time(nullptr);
    const ULONGLONG t0 = GetTickCount64();

    Presence presence;
    ULONGLONG next_connect = 0, last_send = 0;
    bool sent_once = false;
    std::string last_details, last_state;
    std::fprintf(stderr, "[rpc] presence on, mode \"%s\"\n", mode.c_str());

    for (;;) {
        Sleep(kPollMs);

        /* what to say now */
        const int scene = g_scene.load();
        std::string details, state = mode;
        if (scene == -1) {
            const char *n = course_name((int)data_0209f2f8);
            details = n ? n : "In a course";
            if (!is_vs) {
                const int stars = (int)NumStars();
                state += ", " + std::to_string(stars) + (stars == 1 ? " star" : " stars");
            }
        } else if (scene >= 0 && !port_scene_is_menu(scene)) {
            details = "Playing a minigame";
        } else if (scene == -2 && course_name((int)data_0209f2f8)) {
            /* no scene crossing seen yet: a level the game booted straight into */
            details = course_name((int)data_0209f2f8);
            if (!is_vs) {
                const int stars = (int)NumStars();
                state += ", " + std::to_string(stars) + (stars == 1 ? " star" : " stars");
            }
        } else {
            details = "In the menus";
        }

        const ULONGLONG now = GetTickCount64();
        if (!presence.connected()) {
            if (now < next_connect)
                continue;
            if (!presence.connect(app, pipe)) {
                const std::string &why = presence.error();
                const bool refused = why.compare(0, 17, "Discord refused: ") == 0;
                const ULONGLONG wait_ms = refused ? kRefusedRetryMs : kRetryMs;
                std::fprintf(stderr, "[rpc] t=%.1f not connected: %s (next look in %d s)\n", since(t0), why.c_str(),
                             (int)(wait_ms / 1000));
                next_connect = now + wait_ms;
                continue;
            }
            std::fprintf(stderr, "[rpc] t=%.1f connected\n", since(t0));
            last_details.clear();               /* say it again on a fresh pipe */
            last_state.clear();
        }

        if (details == last_details && state == last_state)
            continue;
        if (sent_once && now - last_send < kMinSendMs)
            continue;                           /* the 15 s rule: latest state goes at the end of the window */
        Activity a;
        a.details = details;
        a.state = state;
        a.start_timestamp = start;
        if (presence.update(a)) {
            last_send = now;
            sent_once = true;
            last_details = details;
            last_state = state;
            std::fprintf(stderr, "[rpc] t=%.1f sent: \"%s\" | \"%s\"\n", since(t0),
                         details.c_str(), state.c_str());
        } else {
            std::fprintf(stderr, "[rpc] t=%.1f lost the pipe; looking again in 60 s\n", since(t0));
            next_connect = now + kRetryMs;
        }
    }
}

} // namespace

/* Called from the crossing hook in tests/walk_window.cpp each time the window
   re-shapes for a scene (scene < 0 is a course). The first call starts the
   thread; a run with presence off never starts it. */
extern "C" void port_rich_presence_scene(int scene)
{
    if (scene != -3)                    /* -3: just make sure the thread is up */
        g_scene.store(scene);
    if (g_started.exchange(1))
        return;
    if (!wanted()) {
        std::fprintf(stderr, "[rpc] presence off for this run\n");
        return;
    }
    HANDLE h = CreateThread(nullptr, 0, presence_thread, nullptr, 0, nullptr);
    if (h)
        CloseHandle(h);
}
