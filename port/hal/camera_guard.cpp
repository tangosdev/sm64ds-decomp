// CAMERA GUARD (run coopfix1, lane CAMERA3): the watchdog that frees a stuck game camera, and the one test the analog and
// free cameras use before they copy a number off the game's camera.
//
// The analog cam and the freecam (tests/walk_window.cpp, the rig) draw over the game's own Camera actor and copy three
// things off it: its distance, its look-at (the freecam's centre) and, at every hand-back, its yaw and pitch. Any actor can
// take that camera with Camera::SetFlag_3 (flag 8 at +0x154 stops the camera's own mode, src/engine/view/d_camera.cpp:1146)
// and only the same actor's later step gives it back. In coop a friend's boss or star copy can be forced past that step or
// removed before it (coop_boss.cpp's forced states, coop_world.cpp's removals), and a level exit whose load does not happen
// leaves state 15 with flag 0x10, which vetoes every Camera::ChangeState (d_camera.cpp:1094). The game camera then stays where
// the scene left it for the rest of the visit, and the rig either copies the frozen numbers or steps aside for the frozen state.
//
// Two pieces live here:
//   cam_guard_trust   1 when the game camera is fit to copy from: in a coop room, no actor holds it (flag 8), its look-at is
//                     within 3000 units of my player and its eye-to-look-at length is inside the rig's clamp. Outside a coop
//                     room it is always 1 (SM64DS_CAMWD=1 applies the coop test there too), so single player is as before.
//   cam_guard_frame   once per play frame: counts frames the picture has not followed a moving player with nothing to explain
//                     it (cutscene, pause, star select, level clear, a talk) and, past the limit, frees the game camera (flags
//                     8, 0x10, 0x4000, 0x8000 off, the follow state, the level-start placement behind my player) and logs
//                     one [camwd] line. The caller re-seeds the rig when it returns 1.
//
// Trips (each needs my player to have moved over 200 units since the condition began, so a frozen player never trips one):
//   T1  the shown look-at more than 1500 units from my player while the shown eye and look-at stay put, 150 frames (5 s);
//   T2  flag 8 held 300 frames (10 s);
//   T3  a scripted state (9 first person, 11 into the cannon, 13 / 16 talks, 15 exit look-at, 17 no-control hold) held 300
//       frames.
// Acts in a coop room only; elsewhere it logs the trip and leaves the camera alone. At most one recovery every 10 s.
// SM64DS_CAMWD=0 off entirely, SM64DS_CAMWD=1 acts in single player too (proof rows), SM64DS_CAMWD=L logs trips but never
// acts (the before arm of a proof row: the knobs below still fire).
// Proof knob, off by default: SM64DS_CAMWD_LEAK=<frame>[:8|:15] at that guard frame leaves flag 8 set on my camera (:8, the
// default) or puts it in state 15 looking at the spot my player stood on with flag 0x10 set (:15), the two leaks above.
// SM64DS_CAMWD_TRACE=<every> logs the guard's counters every N frames.
#include "coop_world.h"
#include "coop_wire.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

extern "C" {
extern char *data_0209f318;              // the Camera
extern unsigned char data_0209fc48[];    // the running cutscene script, 0 = none
extern unsigned char data_0209f294[];    // star select
extern unsigned char data_0209f2c4[];    // the pause is up
extern unsigned char data_0209f20c[];    // the level-clear screen owns the game
extern unsigned data_0209b008[4], data_0209b098[4], data_0209b0b8[4], data_0209b0d8[4], data_0209b0f8[4],
    data_0209b108[4], data_0209b118[4];
int _ZN9dCamera_c11ChangeStateEPNS_5StateE(void *self, void *state);
void func_0200cce4(char *cam);
}

namespace {

enum : int { kFlags = 0x154, kState = 0x138, kAt = 0x80, kEye = 0x8c, kPrevAt = 0xc8, kTarget = 0x11c, kTargetPos = 0x120,
             kExitYaw = 0x186, kPlPos = 0x5c, kPlYaw = 0x8e, kPlTalk = 0x368 };
const long long kUnit = 1 << 12;

int g_env = -1;           // -1 unread, 0 off, 1 coop only, 2 always, 3 log only
int g_leak_frame = -1, g_leak_kind = 8;
unsigned g_trace = 0;
const void *g_cam;        // the camera the counters belong to (a new level makes a new one)
unsigned g_frame, g_since_cam, g_last_fix;
int g_have_fix;
int g_t1, g_t2, g_t3;     // streaks
int g_t1_from[3], g_t2_from[3], g_t3_from[3];
int g_prev_eye[3], g_prev_at[3], g_prev_ok;
int g_leak_spot[3];
unsigned g_trips, g_fixes;

void read_env() {
    if (g_env >= 0) return;
    const char *e = std::getenv("SM64DS_CAMWD");
    g_env = (e && *e == '0') ? 0 : (e && *e == '1') ? 2 : (e && (*e == 'L' || *e == 'l')) ? 3 : 1;
    if (const char *l = std::getenv("SM64DS_CAMWD_LEAK")) {
        g_leak_frame = std::atoi(l);
        if (const char *k = std::strchr(l, ':')) g_leak_kind = std::atoi(k + 1);
    }
    if (const char *t = std::getenv("SM64DS_CAMWD_TRACE")) g_trace = (unsigned)std::atoi(t);
    std::fprintf(stderr, "[camwd] %s (SM64DS_CAMWD=0 off, =1 acts in single player too)\n",
                 g_env == 0 ? "off" : g_env == 2 ? "on, acts everywhere" : g_env == 3 ? "on, logs only (never acts)" : "on, acts in coop rooms");
}

long long dist3(const int *a, const int *b) {      // fixed point in, fixed point out; doubles so far apart cannot overflow
    const double x = (double)a[0] - b[0], y = (double)a[1] - b[1], z = (double)a[2] - b[2];
    return (long long)std::sqrt(x * x + y * y + z * z);
}

int state_idx(const char *cam) {
    const void *st = *(void *const *)(cam + kState);
    if (st == data_0209b008) return 0;
    if (st == data_0209b098) return 9;
    if (st == data_0209b0b8) return 11;
    if (st == data_0209b0d8) return 13;
    if (st == data_0209b0f8) return 15;
    if (st == data_0209b108) return 16;
    if (st == data_0209b118) return 17;
    return -1;   // the follow tunings, the cannon aim (12), the grab-tail (18): never a T3 state
}

// one streak: keep counting while `on`, remember where my player stood when it began
int streak(int &n, int *from, int on, const int *me) {
    if (!on) { n = 0; return 0; }
    if (n == 0) std::memcpy(from, me, sizeof(int) * 3);
    ++n;
    return dist3(me, from) > 200 * kUnit;
}

void leak_knob(char *cam, const int *me) {
    if (g_leak_frame < 0 || (int)g_frame != g_leak_frame) return;
    unsigned *f = (unsigned *)(cam + kFlags);
    if (g_leak_kind == 15) {
        std::memcpy(g_leak_spot, me, sizeof g_leak_spot);
        *(int **)(cam + kTarget) = g_leak_spot;          // what LookAtExit points it at: an actor's position, here a frozen one
        std::memcpy(cam + kTargetPos, g_leak_spot, sizeof g_leak_spot);
        _ZN9dCamera_c11ChangeStateEPNS_5StateE(cam, data_0209b0f8);
        *f |= 0x10u;
    } else {
        *f |= 8u;
    }
    std::fprintf(stderr, "[camwd-knob] leak :%d at guard frame %u (flags %x)\n", g_leak_kind, g_frame, *f);
}

}  // namespace

extern "C" int cam_guard_trust(const void *camv, const void *player) {
    read_env();
    const char *cam = (const char *)camv;
    if (!cam || !player || g_env == 0) return 1;
    if (g_env != 2 && !port::coop_world_active()) return 1;   // single player: the rig copies the game camera as it always did
    if (*(const unsigned *)(cam + kFlags) & 8u) return 0;
    const int *at = (const int *)(cam + kAt), *eye = (const int *)(cam + kEye);
    const int *me = (const int *)((const char *)player + kPlPos);
    if (dist3(at, me) > 3000 * kUnit) return 0;
    const long long d = dist3(eye, at);
    return d >= 0x30000 && d <= 0x2000000;   // the rig's own zoom clamp
}

// shown_eye / shown_at: the picture this frame (the rig's eye and pivot when the rig drew, the camera's own otherwise).
// host_excused: the host's own reasons (the F5 menu, a rollback replay). Returns 1 when it freed the camera this frame.
extern "C" int cam_guard_frame(void *camv, void *player, const int *shown_eye, const int *shown_at, int host_excused) {
    read_env();
    if (g_env == 0) return 0;
    char *cam = (char *)camv;
    if (!cam || !player || !shown_eye || !shown_at) return 0;
    ++g_frame;
    if (cam != g_cam) {                                   // a new level: a new camera and nothing to hold against it
        g_cam = cam; g_since_cam = 0;
        g_t1 = g_t2 = g_t3 = 0; g_prev_ok = 0;
    }
    ++g_since_cam;
    const int *me = (const int *)((char *)player + kPlPos);
    leak_knob(cam, me);
    const unsigned fl = *(const unsigned *)(cam + kFlags);
    const int st = state_idx(cam);
    const int excused = host_excused || g_since_cam < 60 || data_0209fc48[0] || data_0209fc48[1] || data_0209fc48[2] ||
                        data_0209fc48[3] || data_0209f294[0] || data_0209f2c4[0] || data_0209f20c[0] ||
                        *(void *const *)((char *)player + kPlTalk) != nullptr;
    int still = 0;
    if (g_prev_ok)
        still = dist3(shown_eye, g_prev_eye) < 2 * kUnit && dist3(shown_at, g_prev_at) < 2 * kUnit;
    std::memcpy(g_prev_eye, shown_eye, sizeof g_prev_eye);
    std::memcpy(g_prev_at, shown_at, sizeof g_prev_at);
    g_prev_ok = 1;
    if (excused) { g_t1 = g_t2 = g_t3 = 0; return 0; }
    const int far = dist3(shown_at, me) > 1500 * kUnit;
    const int trip1 = streak(g_t1, g_t1_from, far && still, me) && g_t1 >= 150;
    const int trip2 = streak(g_t2, g_t2_from, (fl & 8u) != 0, me) && g_t2 >= 300;
    const int trip3 = streak(g_t3, g_t3_from, st == 9 || st == 11 || st == 13 || st == 15 || st == 16 || st == 17, me) &&
                      g_t3 >= 300;
    if (g_trace && g_frame % g_trace == 0)
        std::fprintf(stderr, "[camwd] f%u st=%d cf=%x t1=%d t2=%d t3=%d look=%lld still=%d\n", g_frame, st, fl, g_t1, g_t2,
                     g_t3, dist3(shown_at, me) >> 12, still);
    if (!trip1 && !trip2 && !trip3) return 0;
    const int which = trip2 ? 2 : trip3 ? 3 : 1;
    const int acts = g_env == 2 || (g_env == 1 && port::coop_world_active());
    const int rate = g_have_fix && g_frame - g_last_fix < 300;
    ++g_trips;
    std::fprintf(stderr, "[camwd] trip T%d f%u st=%d cf=%x look=%lld me=%d,%d,%d -> %s\n", which, g_frame, st, fl,
                 dist3(shown_at, me) >> 12, me[0] >> 12, me[1] >> 12, me[2] >> 12,
                 !acts ? (g_env == 3 ? "logged (SM64DS_CAMWD=L: left alone)" : "logged (single player: left alone)") : rate ? "held (recovered under 10 s ago)" : "recovered");
    g_t1 = g_t2 = g_t3 = 0;
    if (!acts || rate) return 0;
    *(unsigned *)(cam + kFlags) &= ~(8u | 0x10u | 0x4000u | 0x8000u);
    _ZN9dCamera_c11ChangeStateEPNS_5StateE(cam, data_0209b008);
    func_0200cce4(cam);                                    // the level-start placement: look-at and eye behind my player
    std::memcpy(cam + kPrevAt, cam + kAt, 0x18);           // Render eases from +0xc8..+0xdf: start from here, no swoop
    g_last_fix = g_frame; g_have_fix = 1; ++g_fixes;
    return 1;
}
