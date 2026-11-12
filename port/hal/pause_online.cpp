// PAUSE2 (run coopfix1): pause in a coop room. See hal/pause_online.h.
#include "pause_online.h"
#include "comms_hub.h"
#include "hub_presence.h"
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern unsigned char data_0209f2c4[];    // the pause screen's state, 0 = playing (Stage::Behavior)
extern unsigned char data_0209f20c[];    // the level-clear screen
extern unsigned char data_0209f294[];    // the star select / course intro
extern unsigned char data_0209f2d8[];    // stage mode, 1 = VS
extern unsigned char data_0209f2a0[];    // the pause stopped the course timer (PS_Cleanup restarts it)
extern char data_0209d4c8[];             // the course timer (hal/coop_world.cpp TIME_TIMER)
extern signed char data_0209f2f8;        // the level
extern unsigned char data_0209f250;      // the local body slot
extern void *data_0209f394[];            // the body slots
extern const void *port_level_fb_ptr;    // hal/coop_boot.cpp: the level's picture
void _ZN5Sound12UnpauseMusicEv(void);    // Sound::UnpauseMusic, PS_Cleanup's own call
void _ZN5Timer10StartTimerEv(void *self);
void *_ZN8dActor_c4NextEPKS_(const void *prev);
int port_adventure_ghost_seat(int slot);
void port_gw_ident_owe(void);            // hal/comms_sync.cpp: send an identity now
int port_gw_pause2_seat(int seat, int *xyz, int *paused);
void port_gw_pause2_counts(unsigned long long *sent, unsigned long long *sent_paused, unsigned long long *recvd);
}
namespace ntr { struct Framebuffer; bool ppu_write_bmp(const char *path, const Framebuffer &fb); }

namespace port {
namespace pauseonline {
namespace {

int env_int(const char *k, int dflt) {
    const char *e = std::getenv(k);
    return (e && *e) ? std::atoi(e) : dflt;
}

struct SeatMenu {
    bool aware;          // his identities carry kIdentPauseAware
    bool menu;           // his newest identity says the pause menu is open
    unsigned last_ms;    // GetTickCount of that identity
};
SeatMenu g_seat[kHubMaxSeats];

bool g_soft;             // the soft pause is on this frame
bool g_menu_was;         // the menu bit last sent
unsigned g_soft_frames;  // frames the world ran under the menu (log)

// ---- the proof driver ------------------------------------------------------
// SM64DS_PAUSE2_TEST=<open>,<close>[,<open>,<close>...]  frames counted in the
//   level SM64DS_PAUSE2_LEVEL (any level but the castle grounds when unset):
//   a START press (held 3 frames) on each.
// SM64DS_PAUSE2_SHOTS=<f>,<f>,...  a picture from the game's own frame dump at
//   each of those frames, pause2_s<seat>_f<frame>.bmp.
// SM64DS_PAUSE2_LOG=<n>  a [pause2] line every n frames in the level (and on
//   every pause edge); SM64DS_PAUSE2_CLASSES=<id>,<id>  actor classes whose
//   first instances the line lists (BoB balls / Bob-ombs, bug 15 / 19).
// All inert unset.
int g_test[16], g_ntest = -1, g_shots[32], g_nshots = -1, g_classes[4], g_nclasses = -1;
unsigned g_lvl_frames;   // Stage::Behavior calls in the test level, this visit
int g_lvl_was = -1000;

void parse_list(const char *k, int *out, int cap, int *n) {
    *n = 0;
    const char *e = std::getenv(k);
    if (!e) return;
    char buf[512];
    std::snprintf(buf, sizeof buf, "%s", e);
    for (char *t = std::strtok(buf, ", "); t && *n < cap; t = std::strtok(nullptr, ", ")) out[(*n)++] = std::atoi(t);
}
void parse_once() {
    if (g_ntest >= 0) return;
    parse_list("SM64DS_PAUSE2_TEST", g_test, 16, &g_ntest);
    parse_list("SM64DS_PAUSE2_SHOTS", g_shots, 32, &g_nshots);
    parse_list("SM64DS_PAUSE2_CLASSES", g_classes, 4, &g_nclasses);
}
bool in_test_level() {
    const int want = env_int("SM64DS_PAUSE2_LEVEL", -1);
    const int lvl = (int)data_0209f2f8;
    return want >= 0 ? lvl == want : lvl != 1;
}

int rd(const void *a, int off) { int v; std::memcpy(&v, (const char *)a + off, 4); return v; }

void log_line(const char *why) {
    const int me = (int)data_0209f250;
    const void *a = (me >= 0 && me < 16) ? data_0209f394[me] : nullptr;
    char rest[700];
    int w = 0;
    rest[0] = 0;
    /* every body slot but mine (as drawn here), then each seat's newest ghost frame and the send / receive counts since
       the last line: a friend sent at full rate shows ~30 (moving) or ~10 (standing) frames a second, a paused one 2 */
    for (int i = 0; i < 16 && w < 600; ++i) {
        const void *b = data_0209f394[i];
        if (!b || i == me) continue;
        w += std::snprintf(rest + w, sizeof rest - (size_t)w, " body%d[s%d]=(%d,%d,%d)", i, port_adventure_ghost_seat(i),
                           rd(b, 0x5c) >> 12, rd(b, 0x60) >> 12, rd(b, 0x64) >> 12);
    }
    for (int s = 0; s < 4 && w < 600; ++s) {
        int xyz[3], pz = 0;
        if (s == comms_hub_my_seat() || !port_gw_pause2_seat(s, xyz, &pz)) continue;
        w += std::snprintf(rest + w, sizeof rest - (size_t)w, " ghost%d=(%d,%d,%d)%s tag=%d", s, xyz[0], xyz[1], xyz[2],
                           pz ? "P" : "", mark(s, pz != 0) ? 1 : 0);
    }
    {
        static unsigned long long ls, lp, lr;
        static unsigned lt;
        unsigned long long cs, cp, cr;
        port_gw_pause2_counts(&cs, &cp, &cr);
        const unsigned now = GetTickCount();
        w += std::snprintf(rest + w, sizeof rest - (size_t)w, " sent+%llu(paused+%llu) recvd+%llu in %ums", cs - ls,
                           cp - lp, cr - lr, lt ? now - lt : 0u);
        ls = cs; lp = cp; lr = cr; lt = now;
    }
    for (int c = 0; c < g_nclasses && w < 600; ++c) {
        int k = 0;
        for (void *x = _ZN8dActor_c4NextEPKS_(nullptr); x && k < 4 && w < 640; x = _ZN8dActor_c4NextEPKS_(x)) {
            if (*(const unsigned short *)((const char *)x + 0xc) != (unsigned short)g_classes[c]) continue;
            w += std::snprintf(rest + w, sizeof rest - (size_t)w, " c%d.%d=(%d,%d,%d)", g_classes[c], k,
                               rd(x, 0x5c) >> 12, rd(x, 0x60) >> 12, rd(x, 0x64) >> 12);
            ++k;
        }
    }
    std::fprintf(stderr, "[pause2] %s f=%u t=%u lvl=%d f2c4=%u soft=%d legacy=%d me=(%d,%d,%d)%s\n", why,
                 g_lvl_frames, (unsigned)GetTickCount(), (int)data_0209f2f8, (unsigned)data_0209f2c4[0], g_soft ? 1 : 0,
                 legacy() ? 1 : 0, a ? rd(a, 0x5c) >> 12 : 0, a ? rd(a, 0x60) >> 12 : 0, a ? rd(a, 0x64) >> 12 : 0,
                 rest);
}

}  // namespace

bool legacy() {
    static int v = -1;
    if (v < 0) {
        v = env_int("SM64DS_PAUSE_LEGACY", 0) != 0 ? 1 : 0;
        if (v) std::fprintf(stderr, "[pause2] SM64DS_PAUSE_LEGACY=1: the 1.0.3 pause (world stops, away rule, blocking drag)\n");
    }
    return v != 0;
}

// The coop room this console is live in, outside VS and the level clear.
static bool coop_live() {
    return comms_hub_room_kind() == kHubRoomCoop && comms_hub_state() == kHubLive && data_0209f2d8[0] != 1;
}

bool soft_world() { return g_soft; }

unsigned char ident_flags() {
    if (legacy()) return 0;
    return (unsigned char)(kIdentPauseAware | (g_menu_was ? kIdentPauseMenu : 0));
}

void ident_recv(int seat, unsigned char flags) {
    if (seat < 0 || seat >= kHubMaxSeats) return;
    SeatMenu &z = g_seat[seat];
    const bool menu = (flags & kIdentPauseMenu) != 0;
    if (menu != z.menu || !z.aware)
        std::fprintf(stderr, "[pause2] seat %d identity: pause-aware %d, pause menu %s\n", seat,
                     (flags & kIdentPauseAware) ? 1 : 0, menu ? "OPEN" : "shut");
    z.aware = (flags & kIdentPauseAware) != 0;
    z.menu = z.aware && menu;
    z.last_ms = GetTickCount();
}

bool mark(int seat, bool legacy_paused) {
    if (legacy() || seat < 0 || seat >= kHubMaxSeats) return legacy_paused;
    const SeatMenu &z = g_seat[seat];
    /* a friend on this build: his identity's menu bit and nothing else (a
       loading hold or a stall is not a pause); identities come every 2 s, so
       one older than 6 s is a friend gone quiet and shows nothing */
    if (z.aware) return z.menu && (unsigned)(GetTickCount() - z.last_ms) < 6000u;
    return legacy_paused;    // a 1.0.3 friend: his paused frames, as 1.0.3 shows them
}

}  // namespace pauseonline
}  // namespace port

using namespace port;
using namespace port::pauseonline;

extern "C" unsigned char port_pause_think_byte(void) {
    return g_soft ? 0 : data_0209f2c4[0];
}

extern "C" int port_pause_online_after_stage(void) {
    parse_once();
    /* THE SOFT PAUSE. Stage::Behavior has just run: when it opened the pause
       this frame (Stage::Behavior :206-243, data_0209f2c4 = 1, then PS_Init)
       or the menu is still up, and this is a coop room, the world keeps
       running under it from this frame's actor walk on. */
    const bool up = data_0209f2c4[0] != 0 && data_0209f20c[0] == 0;
    const bool soft = up && !legacy() && coop_live();
    if (soft && !g_soft) {
        /* PS_Init paused the music and Stage::Behavior stopped the course
           timer; online the world runs on, so both run on too. PS_Cleanup's
           own restart is skipped once data_0209f2a0 is clear. */
        _ZN5Sound12UnpauseMusicEv();
        if (data_0209f2a0[0]) {
            _ZN5Timer10StartTimerEv(data_0209d4c8);
            data_0209f2a0[0] = 0;
        }
        g_soft_frames = 0;
        std::fprintf(stderr, "[pause2] pause menu OPEN in a coop room: the world keeps running under it\n");
    }
    if (!soft && g_soft)
        std::fprintf(stderr, "[pause2] pause menu shut after %u frames of running world\n", g_soft_frames);
    g_soft = soft;
    if (g_soft) ++g_soft_frames;

    /* THE TAG. The menu bit goes out in the identity at once on each edge
       (and on every 2 s identity after). A 1.0.3 room pause (legacy) sends
       nothing new. */
    const bool menu = !legacy() && up && comms_hub_state() == kHubLive;
    if (menu != g_menu_was) {
        g_menu_was = menu;
        port_gw_ident_owe();
        std::fprintf(stderr, "[pause2] my pause menu %s: identity sent with the menu bit %d\n", menu ? "OPEN" : "shut",
                     menu ? 1 : 0);
    }

    /* the proof driver's frame count and pictures */
    {
        const int lvl = (int)data_0209f2f8;
        if (lvl != g_lvl_was) { g_lvl_was = lvl; g_lvl_frames = 0; }
        if (in_test_level()) {
            ++g_lvl_frames;
            for (int i = 0; i < g_nshots; ++i)
                if ((int)g_lvl_frames == g_shots[i] && port_level_fb_ptr) {
                    char nm[96];
                    std::snprintf(nm, sizeof nm, "pause2_s%d_f%05d.bmp", comms_hub_my_seat(), g_shots[i]);
                    const bool okw = ntr::ppu_write_bmp(nm, *(const ntr::Framebuffer *)port_level_fb_ptr);
                    std::fprintf(stderr, "[pause2] picture %s %s\n", nm, okw ? "written" : "FAILED");
                    log_line("shot");
                }
            const int every = env_int("SM64DS_PAUSE2_LOG", 0);
            if (every > 0 && (g_lvl_frames % (unsigned)every) == 0) log_line("tick");
        }
    }
    /* the menu has the buttons and the stick while the world runs: the local
       player gets none of them this frame */
    return g_soft ? 1 : 0;
}

extern "C" unsigned port_pause_online_test_pad(void) {
    parse_once();
    if (g_ntest <= 0 || !in_test_level()) return 0;
    for (int i = 0; i < g_ntest; ++i) {
        const int d = (int)g_lvl_frames - g_test[i];
        if (d < 0 || d > 2) continue;
        static int said = -1;
        if (said != i) {
            said = i;
            std::fprintf(stderr, "[pause2] test: START pressed at level frame %u (%s)\n", g_lvl_frames,
                         (i & 1) ? "close" : "open");
        }
        return 0x0010;   // XINPUT_GAMEPAD_START, held 3 level frames
    }
    return 0;
}

/* the proof rows' clock: Stage::Behavior calls in the test level this visit (0 outside it) */
extern "C" unsigned port_pause_online_level_frames(void) {
    parse_once();
    return in_test_level() ? g_lvl_frames : 0;
}
