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
// THE RACING PENGUIN (run coopfix1 lane NPCFIX1, from the AUDIT-NPCS finding H1): Cool Cool Mountain's penguin race had
// the same hole (each console its own penguin, so a friend never saw the race and could start a second one). It rides the
// same sub-type with ver 2 and the same rules; see the penguin section below.
//
// THE SNOWMAN'S ROLLING BODY (run coopfix1 lane SNOWBALL1, the NPCFIX1 leftover): after one player's talk with the
// snowman's body in Cool Cool Mountain the body rolled only on his screen, so a friend could start his own talk a moment
// later and watch a different roll. It rides the same sub-type with ver 3 and the same rules, plus the ending: see the
// snowman section below.
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
//   SM64DS_COOP_PG_RACE=F      at course frame F my idle racing penguin starts his race with my player (state 3, as the
//                              invite's yes leads to); the penguin's lines are [cooppg] and use the KTQ_LOG / KTQ_TRACE knobs
//   SM64DS_COOP_KTQ_ARM=F      at course frame F arm my own race flag the way Koopa's talk does (mHasTouchedFlag = 0 on
//                              StartTalk, kept when the offer is declined): the state of a player who talked to Koopa and
//                              said no, which a headless row cannot drive

#include "coop_wire.h"
#include "coop_world.h"
#include "comms_hub.h"

#include "daRNk_c.h"
#include "daPgRcer_c.h"
#include "daBgSnmBdy_c.h"
#include "daBgSnmHed_c.h"

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
extern int (*port_coop_pgrcer_follow)(void *penguin);
void *port_pgrcer_anim_file(int i);
void *port_pgrcer_tex_file(int i);
void port_pgrcer_reset(void *penguin, short ang);
void port_pgrcer_start(void *penguin, void *talker);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *seq, void *btp, int flags, int speed, unsigned int start);
extern const void *port_level_fb_ptr;
extern int (*port_coop_snmbdy_follow)(void *body);
extern int (*port_coop_snmhed_quiet)(void *head);
void port_snmbdy_scale(void *body, int scale);
void port_snmbdy_cue_head(void *body);
void port_snmbdy_quake(void *body);
void port_snmbdy_reset(void *body);
void port_snmbdy_points(int *reach, int *landing);
}
namespace ntr { struct Framebuffer; bool ppu_write_bmp(const char *path, const Framebuffer &fb); }

namespace port {

// SYNW sub-type kWorldRacePose (0x06) is named in coop_wire.h's SYNW list.

#pragma pack(push, 1)
// RACE_POSE payload (after the 12-byte head), 32 bytes.
struct CoopRaceWire {
    CoopKeyWire key;
    uint8_t  ver;        // 1 Koopa the Quick, 2 the racing penguin (NPCFIX1), 3 the snowman's rolling body (SNOWBALL1);
                         // a 1.0.4 console refuses ver 2 and 3 as bad records, one record each, nothing else lost
    uint8_t  state;      // the racer's Koopa state, 2..5 (ver 3: the body's state, 2..5)
    uint8_t  anim;       // 0 idle, 1 talk, 2 run start, 3 run, 4 jump, 5 land, 6 brake (ver 3: always 0)
    uint8_t  flags;      // bit 0 racing, bit 1 finished, bit 2 the animation plays once
                         // (ver 3: bit 0 rolling, bit 1 the head gave its star, bits 2-3 the body's substate 0..2)
    int32_t  x, y, z;    // Fix12
    int16_t  ang;        // heading
    uint16_t seq;        // the sender's record counter
    int32_t  frame;      // animation frame, 20.12 (ver 3: the body's scale, Fix12 0x800..0x1800)
    int32_t  speed;      // animation speed, 0x1000 = 1.0 (ver 3: the body's roll angle, 0..0xffff)
};
#pragma pack(pop)
static_assert(sizeof(CoopRaceWire) == 32, "race pose is 32 bytes");

bool coop_race_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(CoopRaceWire)) return false;
    CoopRaceWire w; std::memcpy(&w, p, sizeof w);
    const int lim = 0x7d00000;        // 32000 units
    // ver 2 (the penguin) also carries his face in flags bits 3-4 (0 unknown, 1..3 his three BTP files)
    if (!coop_key_ok(w.key) || w.ver < 1 || w.ver > 3 || w.state < 2 || w.state > 5 || w.anim > 6) return false;
    if (w.ver == 3) {
        if (w.anim != 0 || w.flags > 0x0f || ((w.flags >> 2) & 3) > 2 || w.frame < 0x800 || w.frame > 0x1800 || w.speed > 0xffff) return false;
    } else if (w.flags > (w.ver == 2 ? 0x1f : 7) || (w.ver == 2 && (w.flags >> 3) > 3)) {
        return false;
    }
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

struct Stats { unsigned sent, recv, bad, other_key, locks, letgo, followed, flag_refused, flag_counted, pg_sent, pg_locks, pg_letgo, pg_followed,
                sb_sent, sb_locks, sb_letgo, sb_followed, sb_quiet; } g_st = {};

// what friends sent
struct Rx {
    bool have; int level, act; CoopRaceWire w; unsigned t_ms;
    bool have_prev; int px, py, pz; unsigned pt_ms;
};
Rx g_rx[kSeats];
Rx g_prx[kSeats];            // the racing penguin's records (ver 2)
Rx g_srx[kSeats];            // the snowman's rolling body's records (ver 3)

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
    Rx &r = (w.ver == 3 ? g_srx : w.ver == 2 ? g_prx : g_rx)[seat];
    const unsigned t = now_ms();
    if (r.have && r.level == w.key.level && r.act == w.key.act && (uint16_t)(w.seq - r.w.seq) >= 0x8000u) return;   // older than what I hold
    const bool changed = !r.have || r.w.state != w.state || (w.ver == 3 && r.w.flags != w.flags);
    if (r.have && r.level == w.key.level && r.act == w.key.act) {
        r.have_prev = true; r.px = r.w.x; r.py = r.w.y; r.pz = r.w.z; r.pt_ms = r.t_ms;
    } else {
        r.have_prev = false;
    }
    r.have = true; r.level = w.key.level; r.act = w.key.act; r.w = w; r.t_ms = t;
    ++g_st.recv;
    if (g_log && changed && w.ver == 3)
        std::fprintf(stderr, "[coopsb] in lf=%u from seat %d: his snowman body state %d sub %d flags %x at (%d,%d,%d) (level %d act %d)\n", g_lf,
                     seat, (int)w.state, (w.flags >> 2) & 3, (unsigned)w.flags, w.x >> 12, w.y >> 12, w.z >> 12, (int)w.key.level,
                     (int)w.key.act);
    else if (g_log && changed && w.ver == 2)
        std::fprintf(stderr, "[cooppg] in lf=%u from seat %d: his penguin state %d at (%d,%d,%d) (level %d act %d)\n", g_lf, seat,
                     (int)w.state, w.x >> 12, w.y >> 12, w.z >> 12, (int)w.key.level, (int)w.key.act);
    else if (g_log && changed)
        std::fprintf(stderr, "[coopktq] in lf=%u from seat %d: his Koopa %s at (%d,%d,%d) (level %d act %d)\n", g_lf, seat,
                     state_name(w.state), w.x >> 12, w.y >> 12, w.z >> 12, (int)w.key.level, (int)w.key.act);
}

void exit_line() {
    std::fprintf(stderr, "[coopktq:exit] sent %u, received %u (bad %u), locks %u, let go %u, frames followed %u, flag touches "
                 "refused %u counted %u%s\n", g_st.sent, g_st.recv, g_st.bad, g_st.locks, g_st.letgo, g_st.followed,
                 g_st.flag_refused, g_st.flag_counted, g_off ? " (CONTROL SM64DS_COOP_KTQ=0)" : "");
    std::fprintf(stderr, "[cooppg:exit] penguin: sent %u, locks %u, let go %u, frames followed %u%s\n", g_st.pg_sent, g_st.pg_locks,
                 g_st.pg_letgo, g_st.pg_followed, g_off ? " (CONTROL SM64DS_COOP_KTQ=0)" : "");
    std::fprintf(stderr, "[coopsb:exit] snowman body: sent %u, locks %u, let go %u, frames followed %u, head talks kept quiet %u%s\n",
                 g_st.sb_sent, g_st.sb_locks, g_st.sb_letgo, g_st.sb_followed, g_st.sb_quiet, g_off ? " (CONTROL SM64DS_COOP_KTQ=0)" : "");
}

void ensure_registered() {
    if (g_regd) return;
    g_regd = true;
    const bool ok = coop_world_register(kWorldRacePose, kWorldRacePose, on_world);
    std::atexit(exit_line);
    if (g_log || !ok) std::fprintf(stderr, "[coopktq] SYNW sub-type 0x%02x %s\n", kWorldRacePose, ok ? "registered" : "NOT registered (range taken)");
}

void pg_reset_visit();
void sb_reset_visit();
void reset_visit() {
    pg_reset_visit();
    sb_reset_visit();
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
    if (g_off || g_m.lock < 0 || !kv || kv != g_m.k) return 0;
    daRNk_c *k = (daRNk_c *)kv;
    // The room ended (or the group went away) or the racer went quiet: send him home first, and skip his own states
    // this one frame, so his waiting code never runs at the posed spot (no talk can start out on the course).
    int lv = -1, ac = 0;
    if (!coop_world_active() || !coop_group_key(&lv, &ac)) { let_go(k, "the coop room ended"); return 1; }
    const Rx &r = g_rx[g_m.lock];
    const unsigned t = now_ms();
    if (!fresh(r, t)) { let_go(k, "his records stopped"); return 1; }
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
                              (int)k->mPlayerWon, !k->mPlayer || k->mState == 0 ? "none" : (const char *)k->mPlayer == me ? "me" : "friend");
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

// ---- the racing penguin (Cool Cool Mountain, class 259; run coopfix1 lane NPCFIX1) -----------------------
// The same shape as Koopa: the console whose own penguin races with the local player as his talker (state 2 the jump
// into the countdown, 3 the race, 4 the walk up, 5 the result) sends RACE_POSE ver 2 every frame in states 2 to 4 and
// on change, once a second in state 5 (he stays at the finish for the rest of the visit, so friends keep seeing him
// there). A console whose own penguin is idle (state 0) locks onto the first racer it hears and poses its penguin
// (d_a_pg_rcer.cpp's Behavior asks port_coop_pgrcer_follow and on 1 skips the states: no talk, no second race, no
// star here). 3 s of silence: let go, back on his path start, idle. The star is the racer's (coop star rules).
// A 1.0.4 console refuses ver 2 as a bad record and keeps the 1.0.4 behaviour (its own penguin, its own race).
enum : int { kPenguinClass = 259 };

struct PgMine {
    daPgRcer_c *p; int last_state; uint16_t seq; unsigned last_send; bool sent_any;
    bool home_set; short hang;
    int lock; bool snapped; uint16_t applied_seq;
};
PgMine g_pm = {nullptr, -1, 0, 0, false, false, 0, -1, false, 0};

void pg_reset_visit() { g_pm = PgMine(); g_pm.p = nullptr; g_pm.last_state = -1; g_pm.lock = -1; }

int pg_anim_index(const daPgRcer_c *p) {
    const void *f = p->mModelAnim.file;
    for (int i = 0; i < 7; ++i) if (f && port_pgrcer_anim_file(i) == f) return i;
    return -1;
}
int pg_tex_index(const daPgRcer_c *p) {
    const void *f = p->mTextureSequence.file;
    for (int i = 0; i < 3; ++i) if (f && port_pgrcer_tex_file(i) == f) return i;
    return -1;
}
int &pg_frame(daPgRcer_c *p) { return *(int *)&p->mModelAnim.currFrame; }
int &pg_speed(daPgRcer_c *p) { return *(int *)&p->mModelAnim.speed; }
unsigned &pg_flags(daPgRcer_c *p) { return *(unsigned *)&p->mModelAnim.numFramesAndFlags; }

void pg_send(daPgRcer_c *p, bool changed) {
    const unsigned t = now_ms();
    const int s = p->mAction;
    const bool running = s >= 2 && s <= 4;
    if (!changed && !running && g_pm.sent_any && t - g_pm.last_send < kBeatMs) return;
    const int ai = pg_anim_index(p), ti = pg_tex_index(p);
    CoopRaceWire w = {};
    w.key.level = (uint16_t)g_lv; w.key.act = (uint8_t)g_ac; w.key.reserved = 0;
    w.ver = 2; w.state = (uint8_t)s; w.anim = (uint8_t)(ai < 0 ? 0 : ai);
    w.flags = (uint8_t)((s == 3 ? 1 : 0) | (s == 5 ? 2 : 0) | ((pg_flags(p) & kOnce) ? 4 : 0) | ((ti < 0 ? 0 : ti + 1) << 3));
    w.x = p->mPosX; w.y = p->mPosY; w.z = p->mPosZ;
    w.ang = p->mAngleY;
    w.seq = ++g_pm.seq;
    w.frame = pg_frame(p) < 0 ? 0 : pg_frame(p);
    w.speed = pg_speed(p) < 0 ? 0 : (pg_speed(p) > 0x10000 ? 0x10000 : pg_speed(p));
    if (coop_send_world(kWorldRacePose, &w, (int)sizeof w)) { ++g_st.pg_sent; g_pm.sent_any = true; g_pm.last_send = t; }
}

void pg_let_go(daPgRcer_c *p, const char *why) {
    if (p) port_pgrcer_reset(p, g_pm.home_set ? g_pm.hang : p->mAngleY);
    ++g_st.pg_letgo;
    if (g_log) std::fprintf(stderr, "[cooppg] lf=%u let go of seat %d's penguin race (%s): my penguin back at his start, waiting\n",
                            g_lf, g_pm.lock, why);
    g_pm.lock = -1; g_pm.snapped = false;
}

// Behavior's question: 1 when this penguin is posed from a friend's race this frame.
int pg_follow(void *pv) {
    read_env();
    if (g_off || g_pm.lock < 0 || !pv || pv != g_pm.p) return 0;
    daPgRcer_c *p = (daPgRcer_c *)pv;
    int lv = -1, ac = 0;
    if (!coop_world_active() || !coop_group_key(&lv, &ac)) { pg_let_go(p, "the coop room ended"); return 1; }
    const Rx &r = g_prx[g_pm.lock];
    const unsigned t = now_ms();
    if (!fresh(r, t)) { pg_let_go(p, "his records stopped"); return 1; }
    double tx = r.w.x, ty = r.w.y, tz = r.w.z;
    if (r.have_prev && r.t_ms != r.pt_ms && r.t_ms - r.pt_ms < 500u) {
        const double dt = (double)(r.t_ms - r.pt_ms);
        double age = (double)(t - r.t_ms); if (age > 250.0) age = 250.0;
        tx += (r.w.x - r.px) / dt * age; ty += (r.w.y - r.py) / dt * age; tz += (r.w.z - r.pz) / dt * age;
    }
    const double dx = tx - p->mPosX, dy = ty - p->mPosY, dz = tz - p->mPosZ;
    const double d = std::sqrt(dx * dx + dy * dy + dz * dz) / 4096.0;
    if (!g_pm.snapped || d > 800.0) {
        p->mPosX = (int)tx; p->mPosY = (int)ty; p->mPosZ = (int)tz;
        g_pm.snapped = true;
    } else {
        p->mPosX += (int)(dx * 0.35); p->mPosY += (int)(dy * 0.35); p->mPosZ += (int)(dz * 0.35);
    }
    p->mAngleY = p->mPrevAngleY = r.w.ang;
    p->mHorzSpeed = 0; p->mVertSpeed = 0;
    p->mAction = r.w.state;            // Behavior's follow branch makes the slide's dust and sound in state 3
    void *file = port_pgrcer_anim_file(r.w.anim);
    if (file && (void *)p->mModelAnim.file != file) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&p->mModelAnim, file, (r.w.flags & 4) ? kOnce : 0, r.w.speed, 0);
        pg_frame(p) = r.w.frame;
        g_pm.applied_seq = r.w.seq;
    } else if (g_pm.applied_seq != r.w.seq) {
        const int fd = pg_frame(p) - r.w.frame;
        if (fd > (3 << 12) || fd < -(3 << 12)) pg_frame(p) = r.w.frame;
        pg_speed(p) = r.w.speed;
        g_pm.applied_seq = r.w.seq;
    }
    const int ti = (r.w.flags >> 3) - 1;
    void *tex = ti >= 0 ? port_pgrcer_tex_file(ti) : nullptr;
    if (tex && (void *)p->mTextureSequence.file != tex)
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&p->mTextureSequence, tex, 0, 0x1000, 0);
    ++g_st.pg_followed;
    return 1;
}
struct PgFollowHook { PgFollowHook() { port_coop_pgrcer_follow = pg_follow; } } g_pg_follow_hook;

// SM64DS_COOP_PG_RACE=F (proof only): at course frame F my idle penguin starts his race with my player, the state the
// invite's yes leads to (a headless row cannot press A in a talk).
void pg_start_knob(daPgRcer_c *p) {
    static int at = -2;
    if (at == -2) { at = -1; if (const char *e = std::getenv("SM64DS_COOP_PG_RACE")) at = std::atoi(e); }
    if (at < 0 || g_lf != (unsigned)at) return;
    char *me = local_body();
    const bool ok = p && me && p->mAction == 0 && g_pm.lock < 0;
    if (ok) port_pgrcer_start(p, me);
    std::fprintf(stderr, "[cooppg] KNOB race lf=%u: %s\n", g_lf, ok ? "my penguin races my player (state 3, as after the invite's yes)"
                 : !p ? "no penguin" : g_pm.lock >= 0 ? "not started: my penguin follows a friend's race" : "not started: my penguin is not idle");
}

void pg_trace(daPgRcer_c *p) {
    if (g_trace <= 0 || (g_lf % (unsigned)g_trace) != 0) return;
    char line[512]; int n = 0;
    const char *me = local_body();
    n += std::snprintf(line + n, sizeof line - n, "[cooppg] trace lf=%u", g_lf);
    if (me) n += std::snprintf(line + n, sizeof line - n, " me (%d,%d,%d)", *(const int *)(me + 0x5c) >> 12,
                               *(const int *)(me + 0x60) >> 12, *(const int *)(me + 0x64) >> 12);
    if (p) n += std::snprintf(line + n, sizeof line - n, " | penguin state %d step %d at (%d,%d,%d) ang %04x anim %d face %d frame %d node %d talker %s",
                              p->mAction, (int)p->mActionStep, p->mPosX >> 12, p->mPosY >> 12, p->mPosZ >> 12,
                              (unsigned)(unsigned short)p->mAngleY, pg_anim_index(p), pg_tex_index(p), pg_frame(p) >> 12,
                              p->mPathNodeIndex, !p->mTalkPlayer ? "none" : (const char *)p->mTalkPlayer == me ? "me" : "friend");
    else n += std::snprintf(line + n, sizeof line - n, " | no penguin");
    if (g_pm.lock >= 0) {
        const Rx &r = g_prx[g_pm.lock];
        n += std::snprintf(line + n, sizeof line - n, " | FOLLOW seat %d: his state %d at (%d,%d,%d) %u ms old", g_pm.lock,
                           (int)r.w.state, r.w.x >> 12, r.w.y >> 12, r.w.z >> 12, now_ms() - r.t_ms);
    }
    std::fprintf(stderr, "%s\n", line);
}

void pg_frame_tick() {
    daPgRcer_c *p = (daPgRcer_c *)_ZN8dActor_c15FindWithActorIDEjPS_(kPenguinClass, nullptr);
    if (p != g_pm.p) {
        if (g_pm.lock >= 0 && g_log) std::fprintf(stderr, "[cooppg] lf=%u my penguin changed: following stopped\n", g_lf);
        g_pm.p = p; g_pm.last_state = -1; g_pm.lock = -1; g_pm.snapped = false; g_pm.home_set = false;
    }
    if (!p) return;
    pg_start_knob(p);
    if (!g_pm.home_set && p->mAction == 0 && g_pm.lock < 0) { g_pm.home_set = true; g_pm.hang = p->mAngleY; }
    const char *me = local_body();
    const int s = p->mAction;
    const bool changed = s != g_pm.last_state;
    if (changed && g_log && g_pm.last_state >= 0 && g_pm.lock < 0)
        std::fprintf(stderr, "[cooppg] lf=%u my penguin state %d -> %d (talker %s) at (%d,%d,%d)\n", g_lf, g_pm.last_state, s,
                     !p->mTalkPlayer ? "none" : (const char *)p->mTalkPlayer == me ? "me" : "friend", p->mPosX >> 12,
                     p->mPosY >> 12, p->mPosZ >> 12);
    g_pm.last_state = s;
    if (g_off) { pg_trace(p); return; }
    const unsigned t = now_ms();
    if (s >= 2 && g_pm.lock < 0 && me && (const char *)p->mTalkPlayer == me) {
        pg_send(p, changed);
        pg_trace(p);
        return;
    }
    if (g_pm.lock >= 0) {
        if (!fresh(g_prx[g_pm.lock], t)) pg_let_go(p, "his records stopped");
    } else if (s == 0) {
        for (int seat = 0; seat < kSeats; ++seat) {
            if (seat == comms_hub_my_seat() || !fresh(g_prx[seat], t)) continue;
            g_pm.lock = seat; g_pm.snapped = false; g_pm.applied_seq = 0;
            ++g_st.pg_locks;
            if (g_log) std::fprintf(stderr, "[cooppg] lf=%u seat %d races the penguin: my penguin now shows that race (his state %d at (%d,%d,%d))\n",
                                    g_lf, seat, (int)g_prx[seat].w.state, g_prx[seat].w.x >> 12, g_prx[seat].w.y >> 12,
                                    g_prx[seat].w.z >> 12);
            break;
        }
    }
    pg_trace(p);
}


// ---- the snowman's rolling body (Cool Cool Mountain, class 274; run coopfix1 lane SNOWBALL1) -------------
// The body talks (state 1), then rolls down its path (state 2). If its talker kept up it turns to the landing point,
// cues the head (class 273) to jump, lands under it (state 3, substates 0, 1, 2) and the head, now on its body, gives
// the star in a talk; otherwise it rolls on off the edge (state 4) and goes home out of sight (state 5, then 0).
// Each console runs its own body, and NPCFIX1 only locked the TALK to one player: once the talk ended, the roll ran
// on the talker's screen only and every friend's body still waited, so a friend could start his own talk and roll.
// Now, the same shape as the penguin:
//   * The talker's console (his own body in state 2 or later with the local player as its talker) sends RACE_POSE
//     ver 3: state, substate, position, heading, roll angle, scale, rolling bit and "the head gave its star" bit,
//     every frame while it moves (states 2 and 4, state 3 before it lands) and on every change, else once a second.
//   * A console whose own body still waits (state 0) locks onto the first roller it hears and poses its body from his
//     records (d_a_bg_snm_bdy.cpp's Behavior asks port_coop_snmbdy_follow and skips the states: no talk, no roll and
//     no ending of its own; it keeps the dust, the rolling sound, the cylinder and the hurt on touch while rolling).
//     When his body cues the head, mine is cued too (the head jumps and lands on my body as on his screen), and when
//     his body lands, mine shakes the same way.
//   * The head's star talk (d_a_bg_snm_hed.cpp asks port_coop_snmhed_quiet) belongs to the roller's console: on a
//     console whose ending came from a friend's roll the head smiles and stays quiet, so the star is made once for the
//     room, on the roller's console, and reaches friends through the coop star rules (SNOWMAN2's taken guard kept).
//     If the roller's records stop before his head gave the star (he left or dropped), my head offers it instead;
//     if they stop after my head was cued but before his landing, my body goes home and my head offers the star once
//     my own roll lands under it.
//   * 3 s of silence before the landing: let go, my body back home, waiting (the roll fell off, or the roller left).
//     After the landing my body stays on its spot under the head for the rest of the visit: no second talk or roll.
// A 1.0.4 console refuses ver 3 as a bad record and keeps the 1.0.4 behaviour (its own body, its own roll).
enum : int { kSnowBodyClass = 274, kSnowHeadClass = 273 };

struct SbMine {
    daBgSnmBdy_c *b; int last_state, last_sub, last_flags; uint16_t seq; unsigned last_send; bool sent_any;
    int lock; bool snapped, cued, quaked, done, head_by_friend, offered;
};
SbMine g_sm = {nullptr, -1, -1, -1, 0, 0, false, -1, false, false, false, false, false, false};

void sb_reset_visit() {
    g_sm = SbMine();
    g_sm.b = nullptr; g_sm.last_state = -1; g_sm.last_sub = -1; g_sm.last_flags = -1; g_sm.lock = -1;
}

daBgSnmHed_c *sb_head() { return (daBgSnmHed_c *)_ZN8dActor_c15FindWithActorIDEjPS_(kSnowHeadClass, nullptr); }

int sb_sub(const daBgSnmBdy_c *b) { return b->mStateValue == 3 ? (b->mSubstate > 2 ? 2 : (int)b->mSubstate) : 0; }
bool sb_moving(int state, int sub) { return state == 2 || state == 4 || (state == 3 && sub < 2); }

int sb_flags(const daBgSnmBdy_c *b) {
    const daBgSnmHed_c *h = sb_head();
    const bool star = h && h->mStateValue == 3 && h->mSubstate >= 3;      // head State3 substate 3: the star was spawned
    return (b->mHorzSpeed != 0 ? 1 : 0) | (star ? 2 : 0) | (sb_sub(b) << 2);
}

void sb_send(daBgSnmBdy_c *b, bool changed) {
    const unsigned t = now_ms();
    const int s = b->mStateValue, sub = sb_sub(b);
    if (!changed && !sb_moving(s, sub) && g_sm.sent_any && t - g_sm.last_send < kBeatMs) return;
    CoopRaceWire w = {};
    w.key.level = (uint16_t)g_lv; w.key.act = (uint8_t)g_ac; w.key.reserved = 0;
    w.ver = 3; w.state = (uint8_t)s; w.anim = 0;
    w.flags = (uint8_t)sb_flags(b);
    w.x = b->mPosX; w.y = b->mPosY; w.z = b->mPosZ;
    w.ang = b->mAngleY;
    w.seq = ++g_sm.seq;
    const int sc = b->mScaleX;
    w.frame = sc < 0x800 ? 0x800 : sc > 0x1800 ? 0x1800 : sc;
    w.speed = (int32_t)(unsigned short)b->mAngleX;
    if (coop_send_world(kWorldRacePose, &w, (int)sizeof w)) { ++g_st.sb_sent; g_sm.sent_any = true; g_sm.last_send = t; }
}

void sb_let_go(daBgSnmBdy_c *b, const char *why) {
    if (b) port_snmbdy_reset(b);
    ++g_st.sb_letgo;
    if (g_log) std::fprintf(stderr, "[coopsb] lf=%u let go of seat %d's snowman roll (%s): my snowman body back home, waiting\n",
                            g_lf, g_sm.lock, why);
    g_sm.lock = -1; g_sm.snapped = false; g_sm.cued = false; g_sm.quaked = false; g_sm.offered = false;
}

// Behavior's question: 0 run the states (not following), 1 posed from a friend's roll, 2 posed and rolling.
int sb_follow(void *bv) {
    read_env();
    if (g_off || g_sm.lock < 0 || !bv || bv != g_sm.b) return 0;
    daBgSnmBdy_c *b = (daBgSnmBdy_c *)bv;
    int lv = -1, ac = 0;
    if (!coop_world_active() || !coop_group_key(&lv, &ac)) { g_sm.done = false; sb_let_go(b, "the coop room ended"); return 1; }
    const Rx &r = g_srx[g_sm.lock];
    const unsigned t = now_ms();
    const bool fr = fresh(r, t);
    // before the landing a silent roller is let go; after it the last record (the landed body) holds for the visit
    if (!fr && !g_sm.done) { sb_let_go(b, "his records stopped"); return 1; }
    double tx = r.w.x, ty = r.w.y, tz = r.w.z;
    if (fr && r.have_prev && r.t_ms != r.pt_ms && r.t_ms - r.pt_ms < 500u) {
        const double dt = (double)(r.t_ms - r.pt_ms);
        double age = (double)(t - r.t_ms); if (age > 250.0) age = 250.0;
        tx += (r.w.x - r.px) / dt * age; ty += (r.w.y - r.py) / dt * age; tz += (r.w.z - r.pz) / dt * age;
    }
    const double dx = tx - b->mPosX, dy = ty - b->mPosY, dz = tz - b->mPosZ;
    const double d = std::sqrt(dx * dx + dy * dy + dz * dz) / 4096.0;
    if (!g_sm.snapped || d > 800.0) {
        b->mPosX = (int)tx; b->mPosY = (int)ty; b->mPosZ = (int)tz;
        g_sm.snapped = true;
    } else {
        b->mPosX += (int)(dx * 0.35); b->mPosY += (int)(dy * 0.35); b->mPosZ += (int)(dz * 0.35);
    }
    const int sub = (r.w.flags >> 2) & 3;
    const bool rolling = (r.w.flags & 1) != 0;
    b->mAngleY = b->mPrevAngleY = r.w.ang;
    b->mAngleX = b->mPrevAngleX = (short)(unsigned short)r.w.speed;
    b->mHorzSpeed = rolling ? 0x28000 : 0;      // HurtPlayer hurts on touch only while this is not 0, as on his screen
    b->mVertSpeed = 0;
    if (b->mScaleX != r.w.frame) port_snmbdy_scale(b, r.w.frame);
    b->mStateValue = r.w.state; b->mSubstate = (unsigned char)(r.w.state == 3 ? sub : 0);
    b->mFlags &= ~1u;                            // as every state after 0 has it
    if (r.w.state == 3 && sub >= 1 && !g_sm.cued) {
        g_sm.cued = true; g_sm.head_by_friend = true;
        port_snmbdy_cue_head(b);
        if (g_log) std::fprintf(stderr, "[coopsb] lf=%u seat %d's snowman body reached its head: my head is cued to jump onto my body\n",
                                g_lf, g_sm.lock);
    }
    if (r.w.state == 3 && sub >= 2 && !g_sm.quaked) {
        g_sm.quaked = true; g_sm.done = true;
        port_snmbdy_quake(b);
        if (g_log) std::fprintf(stderr, "[coopsb] lf=%u seat %d's snowman body landed at (%d,%d,%d): mine landed there too, and stays for the visit\n",
                                g_lf, g_sm.lock, b->mPosX >> 12, b->mPosY >> 12, b->mPosZ >> 12);
    }
    ++g_st.sb_followed;
    return rolling ? 2 : 1;
}
struct SbFollowHook { SbFollowHook() { port_coop_snmbdy_follow = sb_follow; } } g_sb_follow_hook;

// The head's question (State3, before its star talk): 1 keeps my head quiet, the roller's head gives the star.
int sb_head_quiet(void *) {
    read_env();
    if (g_off || !g_sm.head_by_friend || !coop_world_active()) return 0;
    if (g_sm.lock < 0) {
        // The roller dropped out after his body cued my head and before it landed (a quit, crash or disconnect):
        // my body went home and my head already sits in its ending. My own roll's landing under it gives the star
        // talk back to my head (once; SNOWMAN2's taken list keeps any repeat to one star).
        const daBgSnmBdy_c *b = g_sm.b;
        if (b && b->mStateValue == 3 && b->mSubstate >= 2) {
            if (!g_sm.offered && g_log)
                std::fprintf(stderr, "[coopsb] lf=%u my own snowman body landed under my head (its roller dropped out): my head offers the star\n", g_lf);
            g_sm.offered = true;
            return 0;
        }
        if ((g_st.sb_quiet++ % 600) == 0 && g_log)
            std::fprintf(stderr, "[coopsb] lf=%u my head waits for my own snowman body to land (its roller dropped out before the landing; %u frames so far)\n",
                         g_lf, g_st.sb_quiet);
        return 1;
    }
    {
        const Rx &r = g_srx[g_sm.lock];
        if (!fresh(r, now_ms()) && !(r.w.flags & 2)) {
            if (!g_sm.offered && g_log)
                std::fprintf(stderr, "[coopsb] lf=%u seat %d's records stopped before his head gave the star: my head offers it\n", g_lf, g_sm.lock);
            g_sm.offered = true;
            return 0;
        }
    }
    if ((g_st.sb_quiet++ % 600) == 0 && g_log)
        std::fprintf(stderr, "[coopsb] lf=%u my head stays quiet: seat %d's console gives the snowman's star (%u frames so far)\n", g_lf,
                     g_sm.lock, g_st.sb_quiet);
    return 1;
}
struct SbHeadHook { SbHeadHook() { port_coop_snmhed_quiet = sb_head_quiet; } } g_sb_head_hook;

#ifndef PORT_ROM_CLEAN
// SM64DS_COOP_SNOWBALL_ESCORT=1 (proof only, not in the kit build): a headless row cannot run after the snowball, so
// while my own body rolls with me as its talker my player is carried: over the body until it passes the reach point,
// then beside the landing point (inside the 768 units State2 asks for) until it lands, then once beside the head for
// its star talk.
void sb_escort(daBgSnmBdy_c *b, char *me) {
    static int on = -1, phase = 0;
    if (on < 0) { const char *e = std::getenv("SM64DS_COOP_SNOWBALL_ESCORT"); on = e && e[0] == '1'; }
    if (!on || !me || (const char *)b->mTalkPlayer != me || g_sm.lock >= 0) return;
    const int s = b->mStateValue, sub = sb_sub(b);
    int reach[3], land[3];
    port_snmbdy_points(reach, land);
    int x, y, z;
    if (s == 2 && !b->mPlayerReachedPath) {
        x = b->mPosX; y = b->mPosY + (b->mRadius << 1) + (150 << 12); z = b->mPosZ;
        if (phase != 1) { phase = 1; std::fprintf(stderr, "[coopsb] KNOB escort lf=%u: carried over my rolling body\n", g_lf); }
    } else if (s == 2 || (s == 3 && sub < 2)) {
        x = land[0] + (500 << 12); y = land[1] + (300 << 12); z = land[2];
        if (phase != 2) { phase = 2; std::fprintf(stderr, "[coopsb] KNOB escort lf=%u: carried beside the landing point\n", g_lf); }
    } else if (s == 3 && phase == 2) {
        const daBgSnmHed_c *h = sb_head();
        if (!h) return;
        x = h->mPosX + (200 << 12); y = h->mPosY + (100 << 12); z = h->mPosZ;
        phase = 3;
        std::fprintf(stderr, "[coopsb] KNOB escort lf=%u: set down beside the head at (%d,%d,%d)\n", g_lf, x >> 12, y >> 12, z >> 12);
    } else {
        return;
    }
    *(int *)(me + 0x5c) = x; *(int *)(me + 0x60) = y; *(int *)(me + 0x64) = z;
    *(int *)(me + 0x98) = 0; *(int *)(me + 0xa4) = 0; *(int *)(me + 0xa8) = 0; *(int *)(me + 0xac) = 0;
}
#endif

void sb_trace(daBgSnmBdy_c *b) {
    if (g_trace <= 0 || (g_lf % (unsigned)g_trace) != 0) return;
    char line[512]; int n = 0;
    const char *me = local_body();
    n += std::snprintf(line + n, sizeof line - n, "[coopsb] trace lf=%u", g_lf);
    if (me) n += std::snprintf(line + n, sizeof line - n, " me (%d,%d,%d)", *(const int *)(me + 0x5c) >> 12,
                               *(const int *)(me + 0x60) >> 12, *(const int *)(me + 0x64) >> 12);
    n += std::snprintf(line + n, sizeof line - n, " | body state %d sub %d at (%d,%d,%d) scale %04x roll %04x node %u talker %s",
                       b->mStateValue, sb_sub(b), b->mPosX >> 12, b->mPosY >> 12, b->mPosZ >> 12, (unsigned)b->mScaleX,
                       (unsigned)(unsigned short)b->mAngleX, (unsigned)b->mPathNode,
                       !b->mTalkPlayer ? "none" : (const char *)b->mTalkPlayer == me ? "me" : "friend");
    if (const daBgSnmHed_c *h = sb_head())
        n += std::snprintf(line + n, sizeof line - n, " | head state %d sub %d", h->mStateValue, (int)h->mSubstate);
    if (g_sm.lock >= 0) {
        const Rx &r = g_srx[g_sm.lock];
        n += std::snprintf(line + n, sizeof line - n, " | FOLLOW seat %d: his state %d sub %d at (%d,%d,%d) %u ms old%s", g_sm.lock,
                           (int)r.w.state, (r.w.flags >> 2) & 3, r.w.x >> 12, r.w.y >> 12, r.w.z >> 12, now_ms() - r.t_ms,
                           g_sm.done ? " (landed: kept)" : "");
    }
    std::fprintf(stderr, "%s\n", line);
}

void sb_frame_tick() {
    daBgSnmBdy_c *b = (daBgSnmBdy_c *)_ZN8dActor_c15FindWithActorIDEjPS_(kSnowBodyClass, nullptr);
    if (b != g_sm.b) {
        if (g_sm.lock >= 0 && g_log) std::fprintf(stderr, "[coopsb] lf=%u my snowman body changed: following stopped\n", g_lf);
        sb_reset_visit();
        g_sm.b = b;
    }
    if (!b) return;
    char *me = local_body();
#ifndef PORT_ROM_CLEAN
    sb_escort(b, me);
#endif
    const int s = b->mStateValue, sub = sb_sub(b), fl = sb_flags(b);
    const bool changed = s != g_sm.last_state || sub != g_sm.last_sub || fl != g_sm.last_flags;
    if (changed && g_log && g_sm.last_state >= 0 && g_sm.lock < 0)
        std::fprintf(stderr, "[coopsb] lf=%u my snowman body state %d sub %d -> state %d sub %d flags %x (talker %s) at (%d,%d,%d)\n", g_lf,
                     g_sm.last_state, g_sm.last_sub, s, sub, (unsigned)fl,
                     !b->mTalkPlayer ? "none" : (const char *)b->mTalkPlayer == me ? "me" : "friend", b->mPosX >> 12, b->mPosY >> 12,
                     b->mPosZ >> 12);
    g_sm.last_state = s; g_sm.last_sub = sub; g_sm.last_flags = fl;
    if (g_off) { sb_trace(b); return; }
    const unsigned t = now_ms();
    if (s >= 2 && g_sm.lock < 0 && me && (const char *)b->mTalkPlayer == me) {
        sb_send(b, changed);
        sb_trace(b);
        return;
    }
    if (g_sm.lock >= 0) {
        if (!g_sm.done && !fresh(g_srx[g_sm.lock], t)) sb_let_go(b, "his records stopped");
    } else if (s == 0) {
        for (int seat = 0; seat < kSeats; ++seat) {
            if (seat == comms_hub_my_seat() || !fresh(g_srx[seat], t)) continue;
            g_sm.lock = seat; g_sm.snapped = false;
            ++g_st.sb_locks;
            if (g_log) std::fprintf(stderr, "[coopsb] lf=%u seat %d rolls the snowman's body: my body now shows that roll (his state %d at (%d,%d,%d))\n",
                                    g_lf, seat, (int)g_srx[seat].w.state, g_srx[seat].w.x >> 12, g_srx[seat].w.y >> 12,
                                    g_srx[seat].w.z >> 12);
            break;
        }
    }
    sb_trace(b);
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
    pg_frame_tick();                  // NPCFIX1: the racing penguin, same shape
    sb_frame_tick();                  // SNOWBALL1: the snowman's rolling body, same shape
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
