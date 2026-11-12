// KOOPA THE QUICK IN A COOP ROOM. Run coopfix1, lane KTQ1 (Tango's play-test bugs 23 and 24).
//
// THE BUGS (Tango's field test, 1.0.2 / 1.0.3, Bob-omb Battlefield act 2):
//   23. One player started the race; a friend far away correctly did not join it, but on the friend's screen Koopa
//       never ran at all. Each console runs its own Koopa (daRNk_c, class 188), and a Koopa only leaves the start
//       after a talk with that console's own player (FIX9A made class 188 a "talker", so his ClosestPlayer is always
//       the local player). Nothing about the race was on the wire.
//   24. The friend reached the summit and touched the finish flag, and the race-end song played. The flag
//       (daRFlag_c, class 205) counts any Player actor (0xbf) touching it while it is armed, and a friend's body is
//       drawn on my console as a real Player. On the RACER's console the friend's drawn body touched the armed flag:
//       the racer's timer stopped, the finish song played there, and when Koopa arrived the racer was told he had
//       won (mPlayerWon = 1) without ever reaching the flag.
//
// WHAT THIS FILE DOES (coop rooms only; solo, party and VS never reach it):
//   * The racer's console is the one whose own Koopa is in a race state (STATE_RACE or later) with the local player
//     as his racer. It sends SYNW 0x06 RACE_POSE: Koopa's state, position, heading and animation (one of his seven
//     files by index, its frame, speed and play-once bit), every frame while he runs (states 2 to 4) and on every
//     state change, then once a second while he stands at the flag (state 5).
//   * Every other console in the same course and act whose own Koopa is still waiting (state 0: nobody is talking to
//     him there) locks onto the first racer it hears and from then on poses its Koopa from that racer's records each
//     frame: d_a_r_nk.cpp's Behavior asks port_coop_race_follow and, on 1, skips the state handler, the move and
//     the mesh collision, but keeps the animation, footsteps, cylinder and shadow. The pose is extrapolated by the
//     record's own speed and eased (35 percent a frame, snapped past 800 units). So the friend sees Koopa run the
//     course as the racer's game drives him, and cannot start a second race with him while it runs (the waiting
//     state never runs, so no talk starts). No race music, timer or result comes to the friend: he is not in it.
//   * When the racer's records stop for 3 seconds (he left the course, or his game went quiet), the friend's Koopa
//     is let go: put back where he spawned, facing the way he did, idle, waiting; a race can then start fresh for
//     whoever walks up to him there.
//   * The flag (d_a_r_flag.cpp asks port_coop_race_foreign) counts only my own body, and not even that while my
//     Koopa is following a friend's race. A friend never stops my timer, never plays my finish song and never wins
//     my race for me.
//   Two players who both talk to Koopa before either race starts each get their own race on their own console, as
//   in 1.0.3 (each racer's Koopa is in a race state, so neither follows); a third console follows the first racer it
//   heard. The Koopa star is the racer's, through the coop star rules already in place.
//   Bob-omb Battlefield and Tiny-Huge Island run the same class; nothing here reads the level beyond the group key.
//
// MIXED ROOMS. A 1.0.3 console drops SYNW 0x06 as an unknown sub-type (coop_world.cpp's dispatcher counts it, never
// guesses), so a 1.0.3 friend simply keeps the 1.0.3 behaviour; this build with a 1.0.3 racer hears no records and
// its Koopa stays as in 1.0.3. The flag rule is local and needs nothing from the friend.
//
// PORT_HOST_ABI. Offsets are the ROM's (32-bit host): see include/daRNk_c.h, daRFlag_c.h.
//
// ENVIRONMENT (proof knobs, all inert unless named):
//   SM64DS_COOP_KTQ=0          MUST-FAIL control: nothing sent, nothing followed, every flag touch counts (1.0.3)
//   SM64DS_COOP_KTQ_LOG=1      [coopktq] lines (state changes, lock / let go, flag touches, the exit line)
//   SM64DS_COOP_KTQ_TRACE=N    every N frames one [coopktq] trace line (my Koopa, the record I follow, the flag)
//   SM64DS_COOP_KTQ_SHOT=a,b   at course frames a, b... a picture from the game's own frame dump (ktq_fNNNNN.bmp)
//   SM64DS_COOP_KTQ_ARM=F      at course frame F arm my own race flag the way Koopa's talk does (mHasTouchedFlag = 0 on
//                              StartTalk, kept when the offer is declined): the state of a player who talked to Koopa and
//                              said no, which a headless row cannot drive

#include "coop_wire.h"
#include "coop_world.h"
#include "comms_hub.h"

#include "daRNk_c.h"

#include <windows.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern unsigned char data_0209f21c;
char *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned id, char *prev);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned short);
int port_adventure_ghost_seat(int slot);
extern int (*port_coop_race_follow)(void *koopa);
extern int (*port_coop_race_foreign)(const void *toucher);
void *port_rnk_anim_file(int i);
extern const void *port_level_fb_ptr;
}
namespace ntr { struct Framebuffer; bool ppu_write_bmp(const char *path, const Framebuffer &fb); }

namespace port {

enum : uint8_t { kWorldRacePose = 0x06 };   // coop_world.cpp left 0x06 for the race (COINS10's note)

#pragma pack(push, 1)
// RACE_POSE payload (after the 12-byte head), 32 bytes.
struct CoopRaceWire {
    CoopKeyWire key;
    uint8_t  ver;        // 1
    uint8_t  state;      // the racer's Koopa state, 2..5
    uint8_t  anim;       // 0 idle, 1 talk, 2 run start, 3 run, 4 jump, 5 land, 6 brake
    uint8_t  flags;      // bit 0 racing, bit 1 finished, bit 2 the animation plays once
    int32_t  x, y, z;    // Fix12
    int16_t  ang;        // heading
    uint16_t seq;        // the sender's record counter
    int32_t  frame;      // animation frame, 20.12
    int32_t  speed;      // animation speed, 0x1000 = 1.0
};
#pragma pack(pop)
static_assert(sizeof(CoopRaceWire) == 32, "race pose is 32 bytes");

bool coop_race_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(CoopRaceWire)) return false;
    CoopRaceWire w; std::memcpy(&w, p, sizeof w);
    const int lim = 0x7d00000;        // 32000 units
    if (!coop_key_ok(w.key) || w.ver != 1 || w.state < 2 || w.state > 5 || w.anim > 6 || w.flags > 7) return false;
    if (w.x < -lim || w.x > lim || w.y < -lim || w.y > lim || w.z < -lim || w.z > lim) return false;
    if (w.frame < 0 || w.frame > (4096 << 12) || w.speed < 0 || w.speed > 0x10000) return false;
    return true;
}

namespace {

enum : int { kSeats = 16, kBodies = 16, kKoopaClass = 188, kOnce = 0x40000000 };
enum : unsigned { kStaleMs = 3000, kBeatMs = 1000 };

bool g_env = false, g_off = false, g_log = false, g_regd = false;
int g_trace = 0;
unsigned g_lf = 0;
int g_lv = -1, g_ac = -1;

struct Stats { unsigned sent, recv, bad, other_key, locks, letgo, followed, flag_refused, flag_counted; } g_st = {};

// what friends sent
struct Rx {
    bool have; int level, act; CoopRaceWire w; unsigned t_ms;
    bool have_prev; int px, py, pz; unsigned pt_ms;
};
Rx g_rx[kSeats];

// my side
struct Mine {
    daRNk_c *k;           // my Koopa this visit (nullptr until seen)
    int last_state;       // for the change log and the send-on-change
    uint16_t seq;
    unsigned last_send;
    bool sent_any;
    // where he waits, kept from the first frame he is seen waiting (the let-go puts him back there)
    bool home_set; int hx, hy, hz; short hang;
    // following
    int lock;             // seat I follow, -1 none
    bool snapped;         // the first follow frame snaps
    uint16_t applied_seq;
} g_m = {nullptr, -1, 0, 0, false, false, 0, 0, 0, 0, -1, false, 0};

unsigned now_ms() { return GetTickCount(); }

char *local_body() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < kBodies) ? (char *)data_0209f394[me] : nullptr;
}
int body_seat(const void *pl) {
    const int nb = (int)data_0209f21c < kBodies ? (int)data_0209f21c : kBodies;
    for (int i = 0; i < nb; ++i) if (data_0209f394[i] == pl) return i;
    return -1;
}

void read_env() {
    if (g_env) return;
    g_env = true;
    const char *e = std::getenv("SM64DS_COOP_KTQ");
    g_off = e && e[0] == '0' && !e[1];
    const char *l = std::getenv("SM64DS_COOP_KTQ_LOG");
    g_log = l && l[0] && l[0] != '0';
    if (const char *t = std::getenv("SM64DS_COOP_KTQ_TRACE")) g_trace = std::atoi(t);
    if (g_trace > 0) g_log = true;
}

int anim_index(const daRNk_c *k) {
    const void *f = k->mModelAnim.file;
    for (int i = 0; i < 7; ++i) if (f && port_rnk_anim_file(i) == f) return i;
    return -1;
}
int &anim_frame(daRNk_c *k) { return *(int *)&k->mModelAnim.currFrame; }
int &anim_speed(daRNk_c *k) { return *(int *)&k->mModelAnim.speed; }
unsigned &anim_flags(daRNk_c *k) { return *(unsigned *)&k->mModelAnim.numFramesAndFlags; }

const char *state_name(int s) {
    switch (s) {
    case 0: return "WAIT"; case 1: return "OFFER"; case 2: return "RACE"; case 3: return "PULL_UP"; case 4: return "STOP";
    case 5: return "POST_RACE_TALK";
    }
    return "?";
}

bool fresh(const Rx &r, unsigned t) { return r.have && t - r.t_ms < kStaleMs && r.level == g_lv && r.act == g_ac; }

void on_world(int seat, int sub, const unsigned char *p, int len, void *) {
    if (sub != kWorldRacePose || seat < 0 || seat >= kSeats || seat == comms_hub_my_seat() || !coop_race_validate(p, len)) {
        ++g_st.bad;
        return;
    }
    CoopRaceWire w; std::memcpy(&w, p, sizeof w);
    Rx &r = g_rx[seat];
    const unsigned t = now_ms();
    if (r.have && r.level == w.key.level && r.act == w.key.act && (uint16_t)(w.seq - r.w.seq) >= 0x8000u) return;   // older than what I hold
    const bool changed = !r.have || r.w.state != w.state;
    if (r.have && r.level == w.key.level && r.act == w.key.act) {
        r.have_prev = true; r.px = r.w.x; r.py = r.w.y; r.pz = r.w.z; r.pt_ms = r.t_ms;
    } else {
        r.have_prev = false;
    }
    r.have = true; r.level = w.key.level; r.act = w.key.act; r.w = w; r.t_ms = t;
    ++g_st.recv;
    if (g_log && changed)
        std::fprintf(stderr, "[coopktq] in lf=%u from seat %d: his Koopa %s at (%d,%d,%d) (level %d act %d)\n", g_lf, seat,
                     state_name(w.state), w.x >> 12, w.y >> 12, w.z >> 12, (int)w.key.level, (int)w.key.act);
}

void exit_line() {
    std::fprintf(stderr, "[coopktq:exit] sent %u, received %u (bad %u), locks %u, let go %u, frames followed %u, flag touches "
                 "refused %u counted %u%s\n", g_st.sent, g_st.recv, g_st.bad, g_st.locks, g_st.letgo, g_st.followed,
                 g_st.flag_refused, g_st.flag_counted, g_off ? " (CONTROL SM64DS_COOP_KTQ=0)" : "");
}

void ensure_registered() {
    if (g_regd) return;
    g_regd = true;
    const bool ok = coop_world_register(kWorldRacePose, kWorldRacePose, on_world);
    std::atexit(exit_line);
    if (g_log || !ok) std::fprintf(stderr, "[coopktq] SYNW sub-type 0x%02x %s\n", kWorldRacePose, ok ? "registered" : "NOT registered (range taken)");
}

void reset_visit() {
    g_m = Mine();
    g_m.k = nullptr; g_m.last_state = -1; g_m.lock = -1;
    g_lf = 0;
}

daRNk_c *find_koopa() {
    return (daRNk_c *)_ZN8dActor_c15FindWithActorIDEjPS_(kKoopaClass, nullptr);
}

// ---- the racer's side ----------------------------------------------------------------------------------
void send_pose(daRNk_c *k, bool changed) {
    const unsigned t = now_ms();
    const int s = k->mState;
    const bool running = s >= 2 && s <= 4;
    if (!changed && !running && g_m.sent_any && t - g_m.last_send < kBeatMs) return;
    const int ai = anim_index(k);
    CoopRaceWire w = {};
    w.key.level = (uint16_t)g_lv; w.key.act = (uint8_t)g_ac; w.key.reserved = 0;
    w.ver = 1; w.state = (uint8_t)s; w.anim = (uint8_t)(ai < 0 ? 3 : ai);
    w.flags = (uint8_t)((k->mIsRacing ? 1 : 0) | (k->mHasFinished ? 2 : 0) | ((anim_flags(k) & kOnce) ? 4 : 0));
    w.x = k->mPosX; w.y = k->mPosY; w.z = k->mPosZ;
    w.ang = k->mAngleY;
    w.seq = ++g_m.seq;
    w.frame = anim_frame(k) < 0 ? 0 : anim_frame(k);
    w.speed = anim_speed(k) < 0 ? 0 : (anim_speed(k) > 0x10000 ? 0x10000 : anim_speed(k));
    if (coop_send_world(kWorldRacePose, &w, (int)sizeof w)) { ++g_st.sent; g_m.sent_any = true; g_m.last_send = t; }
}

// ---- the friend's side ---------------------------------------------------------------------------------
void let_go(daRNk_c *k, const char *why) {
    if (k && g_m.home_set) {
        k->mPosX = g_m.hx; k->mPosY = g_m.hy; k->mPosZ = g_m.hz;
        k->mAngleY = k->mPrevAngleY = k->mTargetAngleY = g_m.hang;
        k->mHorzSpeed = 0; k->mVertSpeed = 0;
        k->mState = daRNk_c::STATE_WAIT_FOR_PLAYER; k->mStep = 0;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&k->mModelAnim, port_rnk_anim_file(0), 0, 0x1000, 0);
    }
    ++g_st.letgo;
    if (g_log) std::fprintf(stderr, "[coopktq] lf=%u let go of seat %d's race (%s): my Koopa back at his start, waiting\n", g_lf, g_m.lock, why);
    g_m.lock = -1; g_m.snapped = false;
}

// Behavior's question: 1 when this Koopa is posed from a friend's race this frame.
int race_follow(void *kv) {
    read_env();
    if (g_off || g_m.lock < 0 || !kv || kv != g_m.k || !coop_world_active()) return 0;
    daRNk_c *k = (daRNk_c *)kv;
    const Rx &r = g_rx[g_m.lock];
    const unsigned t = now_ms();
    if (!fresh(r, t)) return 0;     // the frame callback lets go next frame
    // where the racer's Koopa is now: his last record moved on by his own speed since it arrived
    double tx = r.w.x, ty = r.w.y, tz = r.w.z;
    if (r.have_prev && r.t_ms != r.pt_ms && r.t_ms - r.pt_ms < 500u) {
        const double dt = (double)(r.t_ms - r.pt_ms);
        double age = (double)(t - r.t_ms); if (age > 250.0) age = 250.0;
        tx += (r.w.x - r.px) / dt * age; ty += (r.w.y - r.py) / dt * age; tz += (r.w.z - r.pz) / dt * age;
    }
    const double dx = tx - k->mPosX, dy = ty - k->mPosY, dz = tz - k->mPosZ;
    const double d = std::sqrt(dx * dx + dy * dy + dz * dz) / 4096.0;
    if (!g_m.snapped || d > 800.0) {
        k->mPosX = (int)tx; k->mPosY = (int)ty; k->mPosZ = (int)tz;
        g_m.snapped = true;
    } else {
        k->mPosX += (int)(dx * 0.35); k->mPosY += (int)(dy * 0.35); k->mPosZ += (int)(dz * 0.35);
    }
    k->mAngleY = k->mPrevAngleY = k->mTargetAngleY = r.w.ang;
    k->mHorzSpeed = 0; k->mVertSpeed = 0;
    void *file = port_rnk_anim_file(r.w.anim);
    if (file && (void *)k->mModelAnim.file != file) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&k->mModelAnim, file, (r.w.flags & 4) ? kOnce : 0, r.w.speed, 0);
        anim_frame(k) = r.w.frame;
        g_m.applied_seq = r.w.seq;
    } else if (g_m.applied_seq != r.w.seq) {
        const int fd = anim_frame(k) - r.w.frame;
        if (fd > (3 << 12) || fd < -(3 << 12)) anim_frame(k) = r.w.frame;
        anim_speed(k) = r.w.speed;
        g_m.applied_seq = r.w.seq;
    }
    ++g_st.followed;
    return 1;
}
struct FollowHook { FollowHook() { port_coop_race_follow = race_follow; } } g_follow_hook;

// The flag's question: 1 for a touch that must not count here.
int race_foreign(const void *pl) {
    read_env();
    if (!pl || !coop_world_active()) return 0;
    const char *me = local_body();
    const bool friend_body = pl != me;
    const bool following = g_m.lock >= 0;
    const bool refuse = !g_off && (friend_body || following);
    if (refuse) ++g_st.flag_refused; else ++g_st.flag_counted;
    if (g_log && (g_st.flag_refused + g_st.flag_counted <= 40 || ((g_st.flag_refused + g_st.flag_counted) % 100) == 0)) {
        const int b = body_seat(pl);
        std::fprintf(stderr, "[coopktq] lf=%u the armed race flag touched by %s (body %d, seat %d)%s: %s\n", g_lf,
                     friend_body ? "a FRIEND's body" : "my body", b, b >= 0 ? port_adventure_ghost_seat(b) : -1,
                     following && !friend_body ? " while I follow a friend's race" : "",
                     refuse ? "REFUSED (no timer stop, no song, no win)" : g_off && friend_body ? "COUNTED (control: the 1.0.3 rule)" : "counted");
    }
    return refuse ? 1 : 0;
}
struct ForeignHook { ForeignHook() { port_coop_race_foreign = race_foreign; } } g_foreign_hook;

void shots() {
    static bool read = false; static char spec[256] = {0};
    if (!read) { read = true; if (const char *s = std::getenv("SM64DS_COOP_KTQ_SHOT")) std::strncpy(spec, s, sizeof spec - 1); }
    if (!spec[0]) return;
    for (const char *q = spec; *q;) {
        char *end = nullptr;
        const long n = std::strtol(q, &end, 10);
        if (end == q) break;
        if (g_lf == (unsigned)n) {
            char nm[64]; std::snprintf(nm, sizeof nm, "ktq_f%05ld.bmp", n);
            const void *lfb = ::port_level_fb_ptr;
            const bool w = lfb && ::ntr::ppu_write_bmp(nm, *(const ::ntr::Framebuffer *)lfb);
            std::fprintf(stderr, "[coopktq] picture %s %s (lf=%u)\n", nm, w ? "written" : "FAILED", g_lf);
        }
        q = end; while (*q == ',' || *q == ' ') ++q;
    }
}

void arm_knob() {
    static int at = -2;
    if (at == -2) { at = -1; if (const char *e = std::getenv("SM64DS_COOP_KTQ_ARM")) at = std::atoi(e); }
    if (at < 0 || g_lf != (unsigned)at) return;
    char *f = _ZN8dActor_c15FindWithActorIDEjPS_(205, nullptr);
    if (f) *(unsigned char *)(f + 0x16e) = 0;
    std::fprintf(stderr, "[coopktq] KNOB arm lf=%u: my race flag %s\n", g_lf, f ? "armed (as after a declined talk)" : "not found");
}

void trace(daRNk_c *k) {
    if (g_trace <= 0 || (g_lf % (unsigned)g_trace) != 0) return;
    char line[512]; int n = 0;
    const char *me = local_body();
    n += std::snprintf(line + n, sizeof line - n, "[coopktq] trace lf=%u", g_lf);
    if (me) n += std::snprintf(line + n, sizeof line - n, " me (%d,%d,%d)", *(const int *)(me + 0x5c) >> 12,
                               *(const int *)(me + 0x60) >> 12, *(const int *)(me + 0x64) >> 12);
    if (k) n += std::snprintf(line + n, sizeof line - n, " | koopa %s step %d at (%d,%d,%d) ang %04x anim %d frame %d racing %d won %d racer %s",
                              state_name(k->mState), (int)k->mStep, k->mPosX >> 12, k->mPosY >> 12, k->mPosZ >> 12,
                              (unsigned)(unsigned short)k->mAngleY, anim_index(k), anim_frame(k) >> 12, (int)k->mIsRacing,
                              (int)k->mPlayerWon, !k->mPlayer ? "none" : (const char *)k->mPlayer == me ? "me" : "friend");
    else n += std::snprintf(line + n, sizeof line - n, " | no koopa");
    if (g_m.lock >= 0) {
        const Rx &r = g_rx[g_m.lock];
        n += std::snprintf(line + n, sizeof line - n, " | FOLLOW seat %d: his %s at (%d,%d,%d) %u ms old", g_m.lock,
                           state_name(r.w.state), r.w.x >> 12, r.w.y >> 12, r.w.z >> 12, now_ms() - r.t_ms);
    }
    if (char *f = _ZN8dActor_c15FindWithActorIDEjPS_(205, nullptr))
        n += std::snprintf(line + n, sizeof line - n, " | flag armed %d", (int)*(const unsigned char *)(f + 0x16e));
    std::fprintf(stderr, "%s\n", line);
}

}  // namespace

// Once a frame in an established coop group (coop_world.cpp's tick, next to the race proof knobs).
void coop_race_frame(unsigned) {
    read_env();
    if (!g_off) ensure_registered();
    int lv = -1, ac = 0;
    if (!coop_group_key(&lv, &ac)) return;
    if (lv != g_lv || ac != g_ac) { reset_visit(); g_lv = lv; g_ac = ac; }
    ++g_lf;
    shots();
    arm_knob();
    daRNk_c *k = find_koopa();
    if (k != g_m.k) {                 // a new visit's Koopa (or he went away)
        if (g_m.lock >= 0 && g_log) std::fprintf(stderr, "[coopktq] lf=%u my Koopa changed: following stopped\n", g_lf);
        g_m.k = k; g_m.last_state = -1; g_m.lock = -1; g_m.snapped = false; g_m.home_set = false;
    }
    if (!k) { trace(nullptr); return; }
    if (!g_m.home_set && k->mState == daRNk_c::STATE_WAIT_FOR_PLAYER && g_m.lock < 0) {
        g_m.home_set = true; g_m.hx = k->mPosX; g_m.hy = k->mPosY; g_m.hz = k->mPosZ; g_m.hang = k->mAngleY;
    }
    const char *me = local_body();
    const int s = k->mState;
    const bool changed = s != g_m.last_state;
    if (changed && g_log && g_m.last_state >= 0)
        std::fprintf(stderr, "[coopktq] lf=%u my Koopa %s -> %s (racing %d, player won %d, racer %s) at (%d,%d,%d)\n", g_lf,
                     state_name(g_m.last_state), state_name(s), (int)k->mIsRacing, (int)k->mPlayerWon,
                     !k->mPlayer ? "none" : (const char *)k->mPlayer == me ? "me" : "friend", k->mPosX >> 12, k->mPosY >> 12,
                     k->mPosZ >> 12);
    g_m.last_state = s;
    if (g_off) { trace(k); return; }
    const unsigned t = now_ms();
    // my own race: I am the racer, I tell
    if (s >= daRNk_c::STATE_RACE && g_m.lock < 0 && me && (const char *)k->mPlayer == me) {
        send_pose(k, changed);
        trace(k);
        return;
    }
    // a friend's race I follow
    if (g_m.lock >= 0) {
        if (!fresh(g_rx[g_m.lock], t)) let_go(k, "his records stopped");
    } else if (s == daRNk_c::STATE_WAIT_FOR_PLAYER) {
        for (int seat = 0; seat < kSeats; ++seat) {
            if (seat == comms_hub_my_seat() || !fresh(g_rx[seat], t)) continue;
            g_m.lock = seat; g_m.snapped = false; g_m.applied_seq = 0;
            ++g_st.locks;
            if (g_log) std::fprintf(stderr, "[coopktq] lf=%u seat %d races Koopa: my Koopa now shows that race (his %s at (%d,%d,%d))\n",
                                    g_lf, seat, state_name(g_rx[seat].w.state), g_rx[seat].w.x >> 12, g_rx[seat].w.y >> 12,
                                    g_rx[seat].w.z >> 12);
            break;
        }
    }
    trace(k);
}

}  // namespace port
