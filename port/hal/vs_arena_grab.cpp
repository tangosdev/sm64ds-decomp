// ARENA MODE, the Wario hold (see vs_arena_grab.h). Run mp1, lane GRAB1, F2.
//
// THE SHAPE, a copy of how EAT1 does eating: claim, verdict, then both consoles
// run the ROM's own code.
//
//   THE HOLDER'S CONSOLE (H, a Wario in a punch or a dive). The ROM's attack
//   cylinder (hit flag 0x1000) touches the PUPPET of a stunned player. In the
//   ROM that is func_ov002_020d869c running on the victim; here hit_detect hands
//   the contact to vs_grab_contact(), which notes it. The NEXT frame, before the
//   walk (the punch state has been sent in my SYNS by then, the referee's attack
//   window needs it), the ROM's own Player::TryGrab(me, puppet) runs: the puppet
//   carries the grabbable flag the stun gives it (0x80, set here from the
//   victim's SYNS), my body goes into St_HoldLight and my +0x358 holds the
//   puppet (func_ov002_020dacb4). A CATCH claim goes out at once, so my screen
//   shows the catch with no delay. From then on my own pad drives the ROM's
//   carry (St_HoldLight), spin (St_SwingPlayer) and throw (St_Throw); the puppet
//   rides my hand because the render path of a Player held by another Player
//   places it from the holder's hand matrix (func_ov002_020e3f90, run for the
//   puppet by the puppet code's func_ov002_020e444c). When the ROM lets go of the
//   puppet (my +0x358 clears) the throw is claimed (THROW, or SWING for the spin)
//   with the launch yaw and speed the ROM used, and the victim's console does the
//   rest. A verdict that refuses the catch (DENY) unwinds it: Player::DropActor.
//
//   THE VICTIM'S CONSOLE (V). The referee's GRABBED event is the only thing that
//   puts MY body in a hold: the ROM's func_ov002_020db5f4(me, holder puppet)
//   (+0x35c = the holder, St_Grabbed), the holder puppet's +0x358 = me, my actor
//   flags as the carry leaves them. My own St_Grabbed_Main then counts the
//   buttons I mash and breaks free by itself (it drops the holder, calls
//   func_ov002_020db54c: St_Thrown); I see my +0x35c clear and claim BREAK. The
//   holder's THROWN event (yaw, speed) makes me call func_ov002_020db54c exactly
//   as func_ov002_020daa74 / func_ov002_020d9c70 do on the holder's screen.
//
//   THE OTHER CONSOLES. Both bodies are puppets; the event links them the way the
//   ROM links them (holder +0x358, victim +0x35c, the 0x4000 / 0x100 flags), and
//   the render path rides the victim puppet on the holder puppet's hand.
//
// Nothing here runs unless arena mode is installed with a referee.

#include "vs_arena_grab.h"

#include "comms_hub.h"
#include "vs_arena.h"
#include "vs_arena_ref.h"

#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace arena_wire;

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f2d8;
extern char data_0209f49c[];
extern char data_0209f49e[];
extern unsigned short data_020a0e58[];           // PadData[4]: {held, pressed}
extern int data_020a0e5a[];
extern unsigned short *data_0209214c[];
extern unsigned char data_ov002_02110094[];      // St_Hurt
extern unsigned char data_ov002_0211034c[];      // St_DizzyStars
extern unsigned char data_ov002_0211016c[];      // St_Grabbed
extern unsigned char data_ov002_0211013c[];      // St_Walk
extern unsigned char data_ov002_02110604[];      // St_SwingPlayer
extern unsigned char data_ov002_021101b4[];      // St_Fall

int _ZN6Player7IsStateERNS_5StateE(void *self, void *state);
void _ZN6Player11ChangeStateERNS_5StateE(void *self, void *state);
int _ZN6Player7TryGrabER8dActor_c(void *self, void *actor);   // holder->TryGrab(victim)
int func_ov002_020db5f4(char *victim, char *attacker);        // victim: St_Grabbed, +0x35c = attacker
void func_ov002_020db54c(unsigned char *victim, int speed, int vert, short yaw);
int _ZN6Player9DropActorEv(void *holder);                     // holder lets go
void port_player_set_character(void *player, unsigned ch);    // hal/player_bridges.cpp
char *_ZN8dActor_c10FindWithIDEj(unsigned id);
int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(char *self, void *clsn, char *player);
int _ZN6Player7IsInAirEv(char *player);
int func_ov002_020d9298(char *stomper);                       // the stomper's bounce (+ its voice)
int port_luigi_is_infected_slot(int slot);                    // hal/luigi_infection.cpp (ARENALUIGI1)
void func_ov002_020db8bc(unsigned char *victim, unsigned char val);   // the victim's squash
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned id, int x, int y, int z);
void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned id, const void *pos);
unsigned _ZNK6Player14GetBodyModelIDEjb(void *self, unsigned a, int b);
int _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt(void *self, unsigned a, int b, int c, unsigned short d);
extern int data_ov002_020ff480[];
}

namespace port {
namespace {

enum : int {
    kHeldObj = 0x358,            // mHeldObj: what I carry
    kGrabbedBy = 0x35c,          // mGrabbedByActor: who carries me
    kFlags = 0xb0,               // the actor flags
    kStateTimer = 0x6a4,         // mStateTimer
    kSwingSpeed = 0x69c,         // mAngleYSpeed (the spin)
    kStHurt = 7,
    kStGrabbed = 16,
    kStDizzy = 36,
    kStHoldLight = 37,
    kStPunchKick = 59,
    kStDive = 62,
    kStThrow = 63,
    kStSwing = 65,
    kPadSlotBytes = 0x18,
    kSeatNone = -1,
    kLookWario = 2,
    kSilentMs = 2000,            // a partner's SYNS silent this long: its console is gone
    kRetryFrames = 45,
    kLetGoRetryMs = 260,         // a THROW denied "too soon" is sent again
    kLetGoTries = 5,
    kVictimMissingFrames = 90,
};
enum : unsigned {
    kFlagCarried = 0x100,        // being carried (the holder sets it)
    kFlagHeldOut = 0x4000,       // the hand matrix is placed (St_HoldLight at anim frame 6)
    kFlagGrabbable = 0x80,       // the stun's grabbable bit
    kFlagThrown = 0x400,
    kFlagReleased = 0x2000,
    kFlagPinned = 0x800,
    kFlagsHoldMask = 0x100 | 0x400 | 0x800 | 0x1000 | 0x2000 | 0x4000,
    kBodyClsnFlags = 0x2d4 + 0x18,   // the body cylinder's flags (dCc_c +0x18)
    kClsnNoPush = 2,                 // St_Grabbed_Init sets it: this cylinder neither pushes nor is pushed
};
enum : unsigned { kAttackMask = 0x27fe0 };       // vs_arena_ref.cpp's: the hit flags of an attack

struct Holder {                  // I am the holder
    bool active = false;
    int k = kSeatNone;           // the victim's seat
    unsigned short cid = 0;      // the CATCH claim
    int phase = 0;               // 0 waiting for the verdict, 1 GRABBED, 2 let go, waiting for THROWN
    unsigned t0_ms = 0;
    unsigned epoch = 0, seen = 0;
    int last_state = -1;
    // the let-go claim
    int how = 0;                 // 1 THROW, 2 SWING
    unsigned speed = 0;
    unsigned short angle = 0;
    unsigned short let_cid = 0;
    unsigned let_ms = 0;
    int let_tries = 0;
    bool late = false;           // the GRABBED came after the ROM had let go
};
struct Victim {                  // I am the victim
    bool active = false;
    int h = kSeatNone;           // the holder's seat
    unsigned frame0 = 0, absent = 0;
    unsigned epoch = 0, seen = 0;
    unsigned short break_cid = 0;
    unsigned break_ms = 0;
    int break_tries = 0;
    bool break_sent = false;
};
struct Pending {                 // a contact waiting for the next frame's walk
    bool on = false;
    int k = kSeatNone;
    unsigned flags = 0;
    unsigned frame = 0;
};

struct Stomp {                   // I stomped on the puppet of seat k and wait for the verdict
    bool on = false;
    int k = kSeatNone;
    unsigned short cid = 0;
    unsigned t0_ms = 0;
};

struct Grab {
    bool log = false;
    bool stomp_off = false;
    Stomp st;
    unsigned stomp_cool = 0;
    unsigned long long stomp_claims = 0, stomp_ok = 0, stomp_denied = 0, stomp_applied = 0;
    // the F9-d probe on the VICTIM's console: another body's contact on mine with no attack flag
    // (what the ROM's own stomp test would see) and whether the squash flag ever went up
    unsigned long long touch_on_me = 0, squash_seen = 0, rig_drops = 0;
    bool squash_prev = false;
    bool force = false;          // lab: the client's own catch gate off, the referee decides alone
    bool off = false;
    Holder h;
    Victim v;
    Pending p;
    int pair_h[kArenaSeats];     // victim seat -> holder seat (-1 none), every console's view
    bool sink_logged = false;
    unsigned cool_until = 0;
    // lab rig
    int rig = 0;                 // 1 wario (seat 0) + victim (seat 1)
    int rig_var = 0;             // 0 throw, 1 swing, 2 letgo (release button), 3 struggle, 4 hold, 5 nowario, 6 nostun
    int rig_hold_at = 45;
    unsigned rig_frames = 0;
    unsigned rig_hold = 0;
    int rig_press = 0;
    unsigned short rig_mask = 0;
    int rig_pin = 0;
    bool vp_set = false;
    int vp[3] = {0, 0, 0};
    bool rig_look_set = false;
    // counters
    unsigned long long contacts = 0, tries = 0, claims = 0, confirmed = 0, denied = 0, unwound = 0,
                       thrown_sent = 0, v_grabbed = 0, v_released = 0, v_breaks = 0, h_dropped = 0,
                       bystander_links = 0, v_refused = 0, throws_applied = 0, rig_catches = 0;
    unsigned long long how_count[5] = {0, 0, 0, 0, 0};
    unsigned long long deny_reason[16] = {0};
    bool inited = false;
};
Grab g;

int &s32r(char *b, int off) { return *(int *)(b + off); }
short &s16r(char *b, int off) { return *(short *)(b + off); }
unsigned &u32r(char *b, int off) { return *(unsigned *)(b + off); }
char *&ptr(char *b, int off) { return *(char **)(b + off); }

char *my_body() {
    const int s = vs_arena_my_seat();
    return s >= 0 && s < kArenaSeats ? (char *)data_0209f394[s] : nullptr;
}
char *body_of(int seat) {
    return seat >= 0 && seat < kArenaSeats ? (char *)data_0209f394[seat] : nullptr;
}
bool on() { return !g.off && vs_ref_active(); }

void init_once() {
    if (g.inited) return;
    g.inited = true;
    for (int &p : g.pair_h) p = kSeatNone;
    g.log = std::getenv("SM64DS_GRAB_LOG") && *std::getenv("SM64DS_GRAB_LOG") == '1';
    g.force = std::getenv("SM64DS_GRAB_FORCE") && *std::getenv("SM64DS_GRAB_FORCE") == '1';
    g.off = std::getenv("SM64DS_GRAB") && *std::getenv("SM64DS_GRAB") == '0';
    g.stomp_off = std::getenv("SM64DS_STOMP") && *std::getenv("SM64DS_STOMP") == '0';
    if (const char *r = std::getenv("SM64DS_GRAB_RIG")) {
        g.rig = 1;
        if (std::strstr(r, "swing")) g.rig_var = 1;
        else if (std::strstr(r, "letgo")) g.rig_var = 2;
        else if (std::strstr(r, "struggle")) g.rig_var = 3;
        else if (std::strstr(r, "hold")) g.rig_var = 4;
        else if (std::strstr(r, "nowario")) g.rig_var = 5;
        else if (std::strstr(r, "nostun")) g.rig_var = 6;
        else if (std::strstr(r, "stomp")) g.rig_var = 7;
        if (const char *c = std::strchr(r, '@')) g.rig_hold_at = std::atoi(c + 1);
    }
    std::fprintf(stderr, "[grab] Wario hold %s%s%s\n", g.off ? "OFF (SM64DS_GRAB=0)" : "on",
                 g.force ? ", client gate off (SM64DS_GRAB_FORCE)" : "", g.rig ? ", rig on" : "");
}

unsigned drawn_frame(int seat) {      // the victim frame I am drawing (A3's rule)
    ArenaSeatView v;
    if (!vs_arena_seat_view(seat, &v) || !v.has_state) return 0;
    long long t = (long long)comms_hub_now_ms() - v.clock_offset_ms - 50;
    long long f = t * 30 / 1000;
    ArenaSample s[kArenaRing];
    const int n = vs_arena_samples(seat, s, kArenaRing);
    if (n > 0) {
        const long long newest = s[0].s.frame, oldest = s[n - 1].s.frame;
        if (f > newest) f = newest;
        if (f < oldest) f = oldest;
    }
    return f < 0 ? 0 : (unsigned)f;
}

// the victim's last reported state says a Wario hold can take it
bool seat_catchable(int seat) {
    ArenaSample s[2];
    if (vs_arena_samples(seat, s, 1) < 1) return false;
    const ArenaState &a = s[0].s;
    if (!(a.flags & kArenaLive) || a.eaten_by) return false;
    if (a.power & (kArenaMega | kArenaVanish | kArenaMetal)) return false;
    return a.state_idx == kStHurt || a.state_idx == kStDizzy;
}

// ---------------------------------------------------------------------------
// THE ROM'S LINKS between a holder and the one it carries
// ---------------------------------------------------------------------------
// holder +0x358 = carried, carried +0x35c = holder, the carried one's actor
// flags as func_ov002_020dacb4 / the HoldLight state leave them.
void link_pair(char *hb, char *vb, bool ready) {
    ptr(hb, kHeldObj) = vb;
    ptr(vb, kGrabbedBy) = hb;
    unsigned &f = u32r(vb, kFlags);
    f &= ~(0x400u | 0x2000u);
    f |= kFlagCarried;
    if (ready) f |= kFlagHeldOut;
    // the carried body does not push the carrier (St_Grabbed_Init does this on the
    // victim's own body; a puppet never runs it)
    u32r(vb, kBodyClsnFlags) |= kClsnNoPush;
}
// The carry pose. The ROM's func_ov002_020dacb4 loads the holder's upper-body "carrying" animation
// into its model's second layer (ModelAnim2::Func_020162C4 with the table entry for its character)
// and the pose code then shows it while +0x358 is set (func_ov002_020e4990). A puppet's SYNS carries
// only the main animation id, so a remote holder would stand with its arms down while the carried
// body rides the hand: load the same layer on the puppet, as the ROM does for a real body.
void puppet_carry_overlay(char *hb, char *vb) {
    int offset = 0xc4;
    if (u32r(vb, kFlags) & 0x8000) {
        offset = 0x224;
        if (s32r(hb, 8) == 2 && *(unsigned short *)(vb + 0xc) == 0xce) offset = 0xc4;
    }
    const int var = s32r(hb, 8);
    if (var < 0 || var > 3) return;
    const unsigned idx = _ZNK6Player14GetBodyModelIDEjb(hb, (unsigned)var & 0xffu, 0);
    char *model = ptr(hb, 0xdc + (int)idx * 4);
    const int rec = data_ov002_020ff480[offset + var];
    if (!model || !rec) return;
    _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt(model, (unsigned)*(int *)((char *)(size_t)rec + 4), 0, 0x1000, 0);
}

// ... and as func_ov002_020da95c / DropActor leave them when the hold ends
void unlink_pair(char *hb, char *vb) {
    if (hb && ptr(hb, kHeldObj) == vb) ptr(hb, kHeldObj) = nullptr;
    if (vb) {
        if (ptr(vb, kGrabbedBy) == hb) ptr(vb, kGrabbedBy) = nullptr;
        u32r(vb, kFlags) &= ~kFlagsHoldMask;
        u32r(vb, kBodyClsnFlags) &= ~(unsigned)kClsnNoPush;
    }
}

// ---------------------------------------------------------------------------
// THE HOLDER'S SIDE
// ---------------------------------------------------------------------------
void h_finish(const char *why) {
    if (g.log || g.h.active)
        std::fprintf(stderr, "[grab] H: hold of seat %d over (%s)\n", g.h.k, why);
    g.h = Holder();
    g.cool_until = vs_arena_frame() + kRetryFrames;
}

// the ROM's own let-go for a carried puppet, without ceremony
void h_drop_puppet(const char *why) {
    char *me = my_body();
    char *pb = body_of(g.h.k);
    if (me && pb && ptr(me, kHeldObj) == pb) _ZN6Player9DropActorEv(me);
    if (pb && ptr(pb, kGrabbedBy) == me) ptr(pb, kGrabbedBy) = nullptr;
    if (pb) u32r(pb, kFlags) &= ~kFlagsHoldMask;
    ++g.h_dropped;
    std::fprintf(stderr, "[grab] H: puppet of seat %d out of my hands (%s)\n", g.h.k, why);
}

void h_send_letgo() {
    const unsigned short c = vs_ref_claim(g.h.how == 2 ? CLAIM_SWING : CLAIM_THROW,
                                          (unsigned char)g.h.k, g.h.angle, g.h.speed, 0,
                                          vs_arena_frame());
    g.h.let_cid = c;
    g.h.let_ms = comms_hub_now_ms();
    ++g.h.let_tries;
    if (g.h.let_tries == 1) {
        ++g.thrown_sent;
        ++g.how_count[g.h.how];
    }
    std::fprintf(stderr, "[grab] H: %s claim %u for seat %d (yaw %u, speed 0x%x, try %d)\n",
                 g.h.how == 2 ? "SWING" : "THROW", (unsigned)c, g.h.k, (unsigned)g.h.angle,
                 g.h.speed, g.h.let_tries);
}

// the contact was seen last frame: now, before the walk, do the ROM's own catch
void h_try_catch() {
    if (!g.p.on) return;
    const Pending p = g.p;
    g.p = Pending();
    char *me = my_body();
    char *pb = body_of(p.k);
    if (!me || !pb || g.h.active || g.v.active) return;
    if ((s32r(me, 8) & 7) != kLookWario && !g.force) return;
    if (ptr(me, kHeldObj) != nullptr || ptr(me, 0x360) != nullptr) return;
    if (vs_arena_frame() < g.cool_until) return;
    if (!g.force && !seat_catchable(p.k)) return;
    if (vs_arena_body_held(p.k) || ptr(pb, kGrabbedBy) != nullptr || s32r(pb, 0xd0) != 0) return;
    ++g.tries;
    // the stun's grabbable bit: the victim's own St_Hurt sets it every frame; on a
    // puppet nothing runs, so it is set for this one call (the victim's SYNS said
    // it is in the stun, and the victim's console checks again at the verdict)
    u32r(pb, kFlags) |= kFlagGrabbable;
    const int r = _ZN6Player7TryGrabER8dActor_c(me, pb);
    if (!r) {
        u32r(pb, kFlags) &= ~kFlagGrabbable;
        if (g.log)
            std::fprintf(stderr, "[grab] H: TryGrab refused (my state %d, step %d)\n",
                         vs_arena_state_index(me), (int)*(unsigned char *)(me + 0x6e3));
        return;
    }
    // func_ov002_020db5f4 would put the victim in St_Grabbed on the victim's own
    // screen; on mine the puppet only needs the link the ROM's hand placement reads
    ptr(pb, kGrabbedBy) = me;
    u32r(pb, kFlags) &= ~kFlagGrabbable;
    const unsigned short cid = vs_ref_claim(CLAIM_CATCH, (unsigned char)p.k, 0, drawn_frame(p.k), 0,
                                            vs_arena_frame());
    if (!cid) {
        h_drop_puppet("no claim could be made");
        return;
    }
    g.h.active = true;
    g.h.k = p.k;
    g.h.cid = cid;
    g.h.phase = 0;
    g.h.t0_ms = comms_hub_now_ms();
    ArenaSeatView sv;
    g.h.epoch = vs_arena_seat_view(p.k, &sv) ? sv.join_epoch : 0;
    g.h.seen = 0;
    g.h.last_state = vs_arena_state_index(me);
    ++g.claims;
    std::fprintf(stderr, "[grab] H: TryGrab on the puppet of seat %d: CATCH claim %u sent, holding it now "
                 "(my state %d)\n", p.k, (unsigned)cid, g.h.last_state);
}

void h_monitor() {
    if (!g.h.active) return;
    char *me = my_body();
    char *pb = body_of(g.h.k);
    if (!me || !pb) { g.h = Holder(); return; }
    if (g.h.phase == 0) {
        int reason = 0, winner = 0;
        const int r = vs_ref_claim_result(g.h.cid, &reason, &winner);
        const unsigned age = comms_hub_now_ms() - g.h.t0_ms;
        if (r == 1) {
            g.h.phase = 1;
            ++g.confirmed;
            std::fprintf(stderr, "[grab] H: CATCH verdict GRABBED for seat %d after %u ms\n", g.h.k, age);
        } else if (r == -1 || (r == -2 && age > 700u)) {
            ++g.denied;
            if (reason >= 0 && reason < 16) ++g.deny_reason[reason];
            std::fprintf(stderr, "[grab] H: CATCH claim %u for seat %d %s (reason %d, %u ms): the puppet "
                         "leaves my hands\n", (unsigned)g.h.cid, g.h.k, r == -1 ? "denied" : "unanswered",
                         reason, age);
            h_drop_puppet("denied");
            ++g.unwound;
            h_finish("denied");
            return;
        }
    }
    if (g.h.phase == 2) {
        // my let-go claim: a "too soon" (STATE) denial is sent again; any other
        // answer ends it (the THROWN event itself finishes the hold)
        int reason = 0, winner = 0;
        const int r = vs_ref_claim_result(g.h.let_cid, &reason, &winner);
        const unsigned age = comms_hub_now_ms() - g.h.let_ms;
        if (r == -1 && reason == DENY_STATE && g.h.let_tries < kLetGoTries && age >= (unsigned)kLetGoRetryMs) {
            h_send_letgo();
        } else if (r == -1 || (r == -2 && age > 700u)) {
            std::fprintf(stderr, "[grab] H: let-go claim %u %s (reason %d)\n", (unsigned)g.h.let_cid,
                         r == -1 ? "denied" : "unanswered", reason);
            h_finish("let-go refused");
            return;
        }
    }
    {
        // the victim's console left and came back before the referee noticed, or went quiet
        ArenaSeatView sv;
        if (vs_arena_seat_view(g.h.k, &sv) && sv.samples > g.h.seen) g.h.seen = sv.samples;
        const bool gone = !(vs_arena_roster_mask() >> g.h.k & 1u) || vs_arena_body_held(g.h.k);
        if (gone || (vs_arena_seat_view(g.h.k, &sv) &&
                     (sv.join_epoch != g.h.epoch || sv.newest_age_ms > (unsigned)kSilentMs ||
                      sv.samples + 4 < g.h.seen))) {
            h_drop_puppet(gone ? "the victim left" : "the victim's console re-joined or fell silent");
            if (g.h.phase != 2) {
                g.h.how = 1;
                g.h.speed = 0x10000;
                g.h.angle = (unsigned short)s16r(me, 0x8e);
                h_send_letgo();      // the referee may still have the pair
            }
            h_finish("victim gone");
            return;
        }
    }
}

// after the walk: did the ROM let go of the puppet this frame? (St_Throw ->
// func_ov002_020daa74, the spin's release -> func_ov002_020d9c70, the release
// button -> Player_ReleaseHeldActor)
void h_after_walk() {
    if (!g.h.active) return;
    char *me = my_body();
    char *pb = body_of(g.h.k);
    if (!me || !pb) return;
    const int cur = vs_arena_state_index(me);
    const int prev = g.h.last_state;
    g.h.last_state = cur;
    if (g.h.phase == 2 || ptr(me, kHeldObj) == pb) {
        if (g.h.phase != 2 && cur == kStSwing) {
            // remember what the spin's release would read (func_ov002_020d9c70)
        }
        return;
    }
    // the ROM let go: how?
    unsigned speed = 0x10000;
    unsigned short angle = (unsigned short)s16r(me, 0x8e);
    int how = 1;
    if (prev == kStThrow || cur == kStThrow) {
        speed = 0x30000;
    } else if (prev == kStSwing || cur == kStSwing) {
        // func_ov002_020d9c70: A = |spin| * 0x28000 (Fix12 multiply), yaw +- |spin| * 0x1c71
        const int v = s16r(me, kSwingSpeed);
        const int av = v < 0 ? (short)(-v) : v;
        speed = (unsigned)(int)(((long long)av * 0x28000 + 0x800) >> 12);
        const short b = (short)(int)(((long long)av * 0x1c71 + 0x800) >> 12);
        const short yaw = s16r(me, 0x8e);
        angle = (unsigned short)(short)(v < 0 ? (short)(yaw - b) : (short)(yaw + b));
        how = 2;
    }
    g.h.how = how;
    g.h.speed = speed;
    g.h.angle = angle;
    g.h.phase = 2;
    g.h.let_tries = 0;
    if (ptr(pb, kGrabbedBy) == me) ptr(pb, kGrabbedBy) = nullptr;    // the ROM's own db54c did this already
    h_send_letgo();
}

// ---------------------------------------------------------------------------
// THE VICTIM'S SIDE
// ---------------------------------------------------------------------------
void v_send_break(const char *why) {
    const unsigned short c = vs_ref_claim(CLAIM_BREAK, (unsigned char)g.v.h, 0, 0, 0, vs_arena_frame());
    g.v.break_cid = c;
    g.v.break_ms = comms_hub_now_ms();
    g.v.break_sent = true;
    ++g.v.break_tries;
    if (g.v.break_tries == 1) ++g.v_breaks;
    std::fprintf(stderr, "[grab] V: BREAK claim %u for seat %d (%s, try %d)\n", (unsigned)c, g.v.h, why,
                 g.v.break_tries);
}

// the ROM's own soft let-go for my body (what a mash-free or a forced end does)
void v_soft_release(char *me, char *hb) {
    if (me && ptr(me, kGrabbedBy) != nullptr) {
        func_ov002_020db54c((unsigned char *)me, 0x10000, 0x10000, s16r(me, 0x8e));
    } else if (me && _ZN6Player7IsStateERNS_5StateE(me, data_ov002_0211016c)) {
        _ZN6Player11ChangeStateERNS_5StateE(me, data_ov002_0211013c);
    }
    if (hb) unlink_pair(hb, me);
}

void v_enter(int holder, const RefEvent &ev) {
    char *me = my_body();
    char *hb = body_of(holder);
    if (!me || !hb) return;
    if (g.v.active) {
        std::fprintf(stderr, "[grab] V: GRABBED by seat %d while already held by seat %d: ignored\n",
                     holder, g.v.h);
        return;
    }
    const bool stun = _ZN6Player7IsStateERNS_5StateE(me, data_ov002_02110094) ||
                      _ZN6Player7IsStateERNS_5StateE(me, data_ov002_0211034c);
    const int st = vs_arena_state_index(me);
    // the ROM's gate here is "in the stun": the referee judged my last SYNS. A state the
    // ROM cannot hold at all (dead, respawning, in a cannon) says no and breaks the pair up.
    const bool never = st == 12 || st == 13 || st == 54 || st == 51 || st == 29 || st == 31 ||
                       st == 5 || st == 4 || st == 3;
    if (ptr(me, kGrabbedBy) != nullptr || never) {
        ++g.v_refused;
        std::fprintf(stderr, "[grab] V: GRABBED by seat %d but my state %d cannot be held: refusing "
                     "(BREAK follows)\n", holder, st);
        g.v.active = true;           // so the BREAK path owns the cleanup
        g.v.h = holder;
        g.v.frame0 = vs_arena_frame();
        v_send_break("refused");
        return;
    }
    // the ROM's own order (func_ov002_020d85fc): the holder's side first (TryGrab ->
    // func_ov002_020dacb4: +0x358, the carried flags), then func_ov002_020db5f4 on me,
    // which sets my +0x35c itself and refuses when it is already set
    ptr(hb, kHeldObj) = me;
    {
        unsigned &f = u32r(me, kFlags);
        f &= ~(0x400u | 0x2000u);
        f |= kFlagCarried | kFlagHeldOut;
    }
    puppet_carry_overlay(hb, me);
    const int ok = func_ov002_020db5f4((char *)me, hb);
    g.v.active = true;
    g.v.h = holder;
    g.v.frame0 = vs_arena_frame();
    g.v.absent = 0;
    g.v.break_sent = false;
    g.v.break_tries = 0;
    ArenaSeatView sv;
    g.v.epoch = vs_arena_seat_view(holder, &sv) ? sv.join_epoch : 0;
    g.v.seen = 0;
    ++g.v_grabbed;
    std::fprintf(stderr, "[grab] V: held by seat %d (event seq %u): func_ov002_020db5f4 %s, my state was "
                 "%s %d, now %d\n", holder, (unsigned)ev.seq, ok ? "ok" : "refused", stun ? "stun" : "other",
                 st, vs_arena_state_index(me));
}

void v_exit(const RefEvent &ev) {
    char *me = my_body();
    char *hb = body_of(g.v.h);
    const int how = ev.b;
    if (how == 3) {                      // my own break, already applied by my own St_Grabbed_Main
        if (hb) unlink_pair(hb, me);
        ++g.v_released;
        g.v = Victim();
        return;
    }
    if (me && ptr(me, kGrabbedBy) != nullptr && how != 4) {
        // the holder's throw / swing: the ROM's own launch on my body (func_ov002_020daa74 /
        // func_ov002_020d9c70 on the holder's screen)
        const int vert = how == 2 ? 0x20000 : 0x10000;
        const int speed = ev.x < 0 ? 0 : ev.x;
        std::fprintf(stderr, "[grab] V: %s by seat %d: launch speed 0x%x vertical 0x%x yaw %u\n",
                     how == 2 ? "swung off" : "thrown", g.v.h, (unsigned)speed, vert, (unsigned)ev.d);
        // func_ov002_020daa74's order: the carried flags first (a throw marks 0x400, which
        // St_Thrown_Init reads), then the launch
        u32r(me, kFlags) &= ~(kFlagHeldOut | kFlagCarried);
        if (how == 1 && speed >= 0x30000) u32r(me, kFlags) |= kFlagThrown;
        func_ov002_020db54c((unsigned char *)me, speed, vert, (short)ev.d);
        ++g.throws_applied;
    } else {
        std::fprintf(stderr, "[grab] V: let go of by seat %d (how %d)\n", g.v.h, how);
        v_soft_release(me, hb);
    }
    if (hb && ptr(hb, kHeldObj) == me) ptr(hb, kHeldObj) = nullptr;
    if (me) u32r(me, kFlags) &= ~(kFlagHeldOut | kFlagCarried);
    ++g.v_released;
    g.v = Victim();
}

void v_monitor() {
    if (!g.v.active) return;
    char *me = my_body();
    char *hb = body_of(g.v.h);
    if (!me || !hb) { g.v = Victim(); return; }
    const unsigned now = comms_hub_now_ms();
    if (g.v.break_sent) {
        int reason = 0, winner = 0;
        const int r = vs_ref_claim_result(g.v.break_cid, &reason, &winner);
        const unsigned age = now - g.v.break_ms;
        if (r == -1 && reason == DENY_STATE && g.v.break_tries < kLetGoTries && age >= (unsigned)kLetGoRetryMs) {
            v_send_break("again");
        } else if (r == -1 && reason == DENY_GONE) {
            std::fprintf(stderr, "[grab] V: BREAK answered GONE: the hold had ended already\n");
            if (hb) unlink_pair(hb, me);
            g.v = Victim();
            return;
        } else if (r == -1 || (r == -2 && age > 900u)) {
            std::fprintf(stderr, "[grab] V: BREAK claim %u %s (reason %d)\n", (unsigned)g.v.break_cid,
                         r == -1 ? "denied" : "unanswered", reason);
            if (hb) unlink_pair(hb, me);
            g.v = Victim();
            return;
        }
        return;
    }
    // keep the links the render path reads (the holder puppet carries me, my hand-out bit)
    if (ptr(hb, kHeldObj) != me) ptr(hb, kHeldObj) = me;
    u32r(me, kFlags) |= kFlagCarried | kFlagHeldOut;
    if (ptr(me, kGrabbedBy) == nullptr || !_ZN6Player7IsStateERNS_5StateE(me, data_ov002_0211016c)) {
        // my own St_Grabbed_Main broke free (the ROM's mash count ran out: it dropped the
        // holder and threw me off, func_ov002_020db54c) or something else took me out of it
        std::fprintf(stderr, "[grab] V: out of the hold of seat %d after %u frames (my state now %d)\n",
                     g.v.h, vs_arena_frame() - g.v.frame0, vs_arena_state_index(me));
        if (hb && ptr(hb, kHeldObj) == me) ptr(hb, kHeldObj) = nullptr;
        u32r(me, kFlags) &= ~(kFlagHeldOut | kFlagCarried);
        v_send_break("broke free");
        return;
    }
    ArenaSeatView sv;
    if (vs_arena_seat_view(g.v.h, &sv) && sv.samples > g.v.seen) g.v.seen = sv.samples;
    if (vs_arena_seat_view(g.v.h, &sv) &&
        (sv.join_epoch != g.v.epoch || sv.newest_age_ms > (unsigned)kSilentMs || sv.samples + 4 < g.v.seen)) {
        std::fprintf(stderr, "[grab] V: the holder's console re-joined or fell silent: out of the hold\n");
        v_soft_release(me, hb);
        v_send_break("holder gone");
        return;
    }
    if (!(vs_arena_roster_mask() >> g.v.h & 1u) || vs_arena_body_held(g.v.h)) {
        if (++g.v.absent > (unsigned)kVictimMissingFrames) {
            std::fprintf(stderr, "[grab] V: the holder's seat is gone: out of the hold\n");
            v_soft_release(me, hb);
            v_send_break("holder seat gone");
        }
    } else {
        g.v.absent = 0;
    }
}

// ---------------------------------------------------------------------------
// THE OTHER CONSOLES: both bodies are puppets
// ---------------------------------------------------------------------------
void pair_set(int victim, int holder) {
    if (victim < 0 || victim >= kArenaSeats) return;
    g.pair_h[victim] = holder;
}
void bystander_apply() {
    const int me = vs_arena_my_seat();
    for (int v = 0; v < kArenaSeats; ++v) {
        const int h = g.pair_h[v];
        if (h == kSeatNone || v == me || h == me) continue;
        char *hb = body_of(h);
        char *vb = body_of(v);
        if (!hb || !vb) continue;
        if (ptr(hb, kHeldObj) != vb || ptr(vb, kGrabbedBy) != hb) {
            if (ptr(hb, kHeldObj) != vb) {
                ++g.bystander_links;
                puppet_carry_overlay(hb, vb);
            }
            link_pair(hb, vb, true);
        }
    }
}

// SM64DS_GRAB_LOG=1: where the carried body is against its carrier, every 10 frames of a hold (the
// ROM places it at the holder's hand in the render path: the distance says whether that works on
// this console)
void hold_probe() {
    if (!g.log) return;
    static unsigned last = 0;
    const unsigned fr = vs_arena_frame();
    if (fr - last < 10u) return;
    const int me = vs_arena_my_seat();
    int hs = kSeatNone, vs = kSeatNone;
    const char *role = nullptr;
    if (g.h.active && g.h.phase == 1) { hs = me; vs = g.h.k; role = "H"; }
    else if (g.v.active && !g.v.break_sent) { hs = g.v.h; vs = me; role = "V"; }
    else {
        for (int v = 0; v < kArenaSeats; ++v)
            if (g.pair_h[v] != kSeatNone && v != me && g.pair_h[v] != me) { hs = g.pair_h[v]; vs = v; role = "B"; break; }
    }
    if (!role) return;
    char *hb = body_of(hs);
    char *vb = body_of(vs);
    if (!hb || !vb) return;
    last = fr;
    std::fprintf(stderr, "[grab] pos %s f=%u holder(seat %d)=(%d,%d,%d) carried(seat %d)=(%d,%d,%d) "
                 "d=(%d,%d,%d) hold358=%d grab35c=%d flags=%08x st=%d/%d anim=%d/%d\n", role, fr, hs,
                 s32r(hb, 0x5c) >> 12, s32r(hb, 0x60) >> 12, s32r(hb, 0x64) >> 12, vs,
                 s32r(vb, 0x5c) >> 12, s32r(vb, 0x60) >> 12, s32r(vb, 0x64) >> 12,
                 (s32r(vb, 0x5c) - s32r(hb, 0x5c)) >> 12, (s32r(vb, 0x60) - s32r(hb, 0x60)) >> 12,
                 (s32r(vb, 0x64) - s32r(hb, 0x64)) >> 12, ptr(hb, kHeldObj) == vb ? 1 : 0,
                 ptr(vb, kGrabbedBy) == hb ? 1 : 0, u32r(vb, kFlags), vs_arena_state_index(hb),
                 vs_arena_state_index(vb), s32r(hb, 0x63c) >> 2, s32r(vb, 0x63c) >> 2);
}

// ---------------------------------------------------------------------------
// REFEREE EVENTS
// ---------------------------------------------------------------------------
void on_grabbed(const RefEvent &ev) {
    const int victim = ev.seat, holder = ev.a, me = vs_arena_my_seat();
    pair_set(victim, holder);
    if (holder == me) {
        if (g.h.active && g.h.k == victim && g.h.cid == ev.c) {
            if (g.h.phase == 0) {
                g.h.phase = 1;
                ++g.confirmed;
                std::fprintf(stderr, "[grab] H: GRABBED event for seat %d\n", victim);
            }
        } else {
            // my claim was dropped (I lost the puppet before the verdict): undo the pair
            std::fprintf(stderr, "[grab] H: GRABBED for seat %d but I hold nobody: letting go\n", victim);
            ++g.h_dropped;
            g.h.active = true;
            g.h.k = victim;
            g.h.phase = 2;
            g.h.how = 1;
            g.h.speed = 0x10000;
            g.h.angle = my_body() ? (unsigned short)s16r(my_body(), 0x8e) : 0;
            h_send_letgo();
        }
    } else if (victim == me) {
        v_enter(holder, ev);
    } else {
        char *hb = body_of(holder);
        char *vb = body_of(victim);
        if (hb && vb) { puppet_carry_overlay(hb, vb); link_pair(hb, vb, true); ++g.bystander_links; }
        std::fprintf(stderr, "[grab] seat %d is held by seat %d (bystander)\n", victim, holder);
    }
}

void on_thrown(const RefEvent &ev) {
    const int victim = ev.seat, holder = ev.a, how = ev.b, me = vs_arena_my_seat();
    pair_set(victim, kSeatNone);
    if (holder == me) {
        if (g.h.active && g.h.k == victim) {
            char *mb = my_body();
            char *pb = body_of(victim);
            if (how >= 3) {
                // the victim broke free (3) or the referee ended the hold (4): drop it the ROM's way
                if (mb && pb && ptr(mb, kHeldObj) == pb) _ZN6Player9DropActorEv(mb);
                std::fprintf(stderr, "[grab] H: hold of seat %d ended by the %s\n", victim,
                             how == 3 ? "victim's break" : "referee");
            } else {
                std::fprintf(stderr, "[grab] H: THROWN event for seat %d (how %d)\n", victim, how);
            }
            if (pb) unlink_pair(mb, pb);
            h_finish(how == 3 ? "victim broke free" : how == 4 ? "forced" : "thrown");
        }
    } else if (victim == me) {
        if (g.v.active && g.v.h == holder) v_exit(ev);
    } else {
        char *hb = body_of(holder);
        char *vb = body_of(victim);
        if (hb && vb) unlink_pair(hb, vb);
    }
}

// ---------------------------------------------------------------------------
// STOMPS (F9-d). The ROM resolves a stomp in the VICTIM's Behavior (func_ov002_020d869c,
// the branch with no attack flag): dActor_c::JumpedOnByPlayer(victim body, attacker),
// the attacker in the air, then func_ov002_020d9298 on the ATTACKER (its bounce and
// voice) and func_ov002_020db8bc on the victim (the squash) with a puff and a sound.
// A puppet's Behavior never runs, so a stomp on a puppet did nothing at all on either
// screen. Here the stomper's own console runs the ROM's test against the puppet (the
// pair pass has written the contact on the puppet's body cylinder), bounces at once and
// claims STOMP; the referee's STOMPED event makes the victim's console squash.
// ---------------------------------------------------------------------------
bool seat_stompable(int seat) {
    ArenaSample s[2];
    if (vs_arena_samples(seat, s, 1) < 1) return false;
    const ArenaState &a = s[0].s;
    return (a.flags & kArenaLive) && !(a.flags & kArenaNoControl) && !a.eaten_by &&
           a.state_idx != kStGrabbed;
}

void stomp_scan() {
    if (g.stomp_off || g.st.on) return;
    char *me = my_body();
    if (!me || vs_arena_frame() < g.stomp_cool) return;
    if (s32r(me, 0xa8) >= 0) return;                 // only a falling body can stomp (JumpedOnByPlayer)
    const ArenaStats as = vs_arena_stats();
    for (int k = 0; k < as.bodies && k < kArenaSeats; ++k) {
        if (k == as.seat) continue;
        char *pb = body_of(k);
        if (!pb || vs_arena_body_held(k)) continue;
        const unsigned other = u32r(pb, 0x2f8);
        if (!other || (u32r(pb, 0x2f4) & kAttackMask)) continue;
        if (_ZN8dActor_c10FindWithIDEj(other) != me) continue;
        if (!seat_stompable(k) || ptr(pb, kGrabbedBy) != nullptr) continue;
        // ARENALUIGI1: LUIGI INFECTION. Classic's team guard (port/unmatched/LuigiInfection_Hit.cpp)
        // runs before the resolver's stomp branch: a Luigi landing on a survivor tags it (no bounce,
        // no squash) and anything landing on a Luigi does nothing. So no local stomp here when either
        // body is a Luigi; the Luigi's own contact goes to the referee as TAG (hal/vs_arena_ref.cpp
        // hit_detect). Inert unless the mode is armed (the test reads 0 for every seat then).
        if (port_luigi_is_infected_slot(as.seat) || port_luigi_is_infected_slot(k)) continue;
        if (!_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(pb, pb + 0x2d4, me)) continue;
        if (!_ZN6Player7IsInAirEv(me)) continue;
        if (!func_ov002_020d9298(me)) continue;       // (a ground pound is not a stomp: it returns 0)
        func_ov002_020db8bc((unsigned char *)pb, 0);  // the puppet squashes on my screen at once
        const unsigned short cid = vs_ref_claim(CLAIM_STOMP, (unsigned char)k, 0, drawn_frame(k), 0,
                                                vs_arena_frame());
        g.stomp_cool = vs_arena_frame() + 10;
        if (!cid) return;
        g.st.on = true;
        g.st.k = k;
        g.st.cid = cid;
        g.st.t0_ms = comms_hub_now_ms();
        ++g.stomp_claims;
        std::fprintf(stderr, "[grab] S: stomped on the puppet of seat %d: STOMP claim %u sent, bounced\n", k,
                     (unsigned)cid);
        return;
    }
}

void stomp_monitor() {
    if (!g.st.on) return;
    int reason = 0, winner = 0;
    const int r = vs_ref_claim_result(g.st.cid, &reason, &winner);
    const unsigned age = comms_hub_now_ms() - g.st.t0_ms;
    if (r == 1) {
        ++g.stomp_ok;
        std::fprintf(stderr, "[grab] S: STOMP verdict STOMPED for seat %d after %u ms\n", g.st.k, age);
        g.st = Stomp();
    } else if (r == -1 || (r == -2 && age > 700u)) {
        ++g.stomp_denied;
        if (reason >= 0 && reason < 16) ++g.deny_reason[reason];
        std::fprintf(stderr, "[grab] S: STOMP claim %u %s (reason %d, %u ms)\n", (unsigned)g.st.cid,
                     r == -1 ? "denied" : "unanswered", reason, age);
        g.st = Stomp();
    }
}

void on_stomped(const RefEvent &ev) {
    const int victim = ev.seat, stomper = ev.a, me = vs_arena_my_seat();
    char *vb = body_of(victim);
    if (!vb) return;
    if (victim == me) {
        // the ROM's gate on the victim: not eaten, body collision on, not in no-control
        if (s32r(vb, 0xd0) != 0 || *(unsigned char *)(vb + 0x713) == 0 || *(unsigned char *)(vb + 0x709) != 0) {
            std::fprintf(stderr, "[grab] S: stomped by seat %d but my body cannot be squashed now\n", stomper);
            return;
        }
        ++g.stomp_applied;
        std::fprintf(stderr, "[grab] S: stomped by seat %d: squash\n", stomper);
    }
    func_ov002_020db8bc((unsigned char *)vb, 0);
    if (victim == me) {
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, s32r(vb, 0x5c), s32r(vb, 0x60) + 0x82000,
                                                       s32r(vb, 0x64));
        _ZN5Sound9PlayBank0EjRK7Vector3(0x12, vb + 0x74);
    }
}

// ---------------------------------------------------------------------------
// LAB RIG, SM64DS_GRAB_RIG=throw | swing | letgo | struggle | hold | nowario | nostun [@frames]
// ---------------------------------------------------------------------------
void rig_press(int slot, unsigned short ctrl_mask, bool first) {
    if (slot < 0 || slot > 3) return;
    const unsigned char mode = *(unsigned char *)(data_0209f49c + slot * kPadSlotBytes + 0x16);
    const unsigned short *map = data_0209214c[mode < 3 ? mode : 0];
    unsigned short ds = 0;
    for (int j = 0; j < 16; ++j)
        if (map[j] & ctrl_mask) ds |= (unsigned short)(1u << j);
    data_020a0e58[slot * 2] |= ds;
    if (first) {
        data_020a0e58[slot * 2 + 1] |= ds;
        *(unsigned short *)((char *)data_020a0e5a + slot * 4) |= ds;
    }
}

void rig_place_behind(char *mb, char *vb) {
    int *p = (int *)(mb + 0x5c);
    p[0] = s32r(vb, 0x5c);
    p[1] = s32r(vb, 0x60);
    p[2] = s32r(vb, 0x64) - (40 << 12);
    s32r(mb, 0x98) = 0;
    s32r(mb, 0xa8) = 0;
    s16r(mb, 0x8e) = 0;
    s16r(mb, 0x94) = 0;
}

// F9-d probe: what my own body saw of another body this frame (no attack flag: the ROM's stomp test
// reads exactly this) and whether func_ov002_020db8bc ever raised my squash flag
void stomp_probe() {
    char *me = my_body();
    if (!me) return;
    const unsigned other = u32r(me, 0x2d4 + 0x24);
    if (other && !(u32r(me, 0x2d4 + 0x20) & kAttackMask)) {
        char *o = _ZN8dActor_c10FindWithIDEj(other);
        if (o && o != me && *(unsigned short *)(o + 0xc) == 0xbf) ++g.touch_on_me;
    }
    const bool sq = *(unsigned char *)(me + 0x6f2) != 0;
    if (sq && !g.squash_prev) {
        ++g.squash_seen;
        std::fprintf(stderr, "[grab] S: my squash flag went up (func_ov002_020db8bc ran on my body)\n");
    }
    g.squash_prev = sq;
}

void rig_step() {
    const int me = vs_arena_my_seat();
    if (data_0209f2d8 != 1 || me < 0 || me > 1 || !vs_ref_stats().started) return;
    ++g.rig_frames;
    char *mb = body_of(me);
    char *vb = body_of(1 - me);
    if (!mb || !vb) return;
    const unsigned cycle = 420;
    const unsigned cf = g.rig_frames % cycle;
    if (me == 1) {
        // the victim: stands where it first stood, pinned for the start of every cycle, and
        // (struggle) mashes while it is held
        if (g.rig_var == 3 && g.v.active && !g.v.break_sent)
            rig_press(1, 0x0001 | 0x0002 | 0x0400 | 0x0800, true);
        if (!g.vp_set) {
            for (int i = 0; i < 3; ++i) g.vp[i] = s32r(mb, 0x5c + 4 * i);
            g.vp_set = true;
        }
        const bool window = g.rig_var == 7 || cf >= 370u || cf <= 20u;
        const int st = vs_arena_state_index(mb);
        if (st == kStHurt && !g.v.active && g.rig_var != 7) {
            // a stunned victim that does not mash: the selftest's pad tape would shorten the stun
            // (func_ov002_020d98b4 counts every button a frame off the stun timer)
            data_020a0e58[2] = 0;
            data_020a0e58[3] = 0;
            *(unsigned short *)((char *)data_020a0e5a + 4) = 0;
        }
        if (window && !g.v.active && st != kStHurt && st != kStGrabbed && st != 20) {
            for (int i = 0; i < 3; ++i) s32r(mb, 0x5c + 4 * i) = g.vp[i];
            s32r(mb, 0x98) = 0;
        }
        return;
    }
    // the Wario (seat 0). Its look first: Wario is the metal-cap look (2).
    if (!g.rig_look_set && g.rig_frames >= 5) {
        g.rig_look_set = true;
        port_player_set_character(mb, kLookWario);
        std::fprintf(stderr, "[grab] rig: my look is now Wario\n");
    }
    if (g.rig_var == 5 && g.rig_look_set) {
        // nowario: the stunning hit is a Wario's, then the catch is tried as a Mario (the referee must
        // refuse: the claimant's SYNS character is not 2 at that frame), and the look is Wario again
        // for the next cycle's hit
        if (cf == 60u) {
            port_player_set_character(mb, 0);
            std::fprintf(stderr, "[grab] rig: my look is now Mario (the catch is tried as a Mario)\n");
        } else if (cf == 210u) {
            port_player_set_character(mb, kLookWario);
        }
    }
    if (g.rig_pin > 0) {
        --g.rig_pin;
        rig_place_behind(mb, vb);
        // (the selftest pad tape walks the body; the pin undoes it)
    }
    if (g.rig_press > 0) {
        rig_press(me, g.rig_mask, g.rig_press == 3);
        --g.rig_press;
        return;
    }
    if (g.rig_frames < 240) return;
    // while holding: the scripted end of the hold
    if (g.h.active && g.h.phase == 1 && ptr(mb, kHeldObj)) {
        ++g.rig_hold;
        if (g.rig_var == 1 && g.rig_hold == (unsigned)(g.rig_hold_at / 2)) {
            // the spin: force the swing state with a fast turn (the pad's circular motion is not scripted)
            _ZN6Player11ChangeStateERNS_5StateE(mb, data_ov002_02110604);
            s16r(mb, kSwingSpeed) = 0x1800;
            std::fprintf(stderr, "[grab] rig: spin\n");
        }
        // (pressed every 12 frames until the ROM lets go: the pickup and the spin's wind-up ignore the button)
        if ((g.rig_var == 0 || g.rig_var == 1 || g.rig_var == 2) && g.rig_hold >= (unsigned)g.rig_hold_at &&
            (g.rig_hold - (unsigned)g.rig_hold_at) % 12u == 0) {
            g.rig_mask = g.rig_var == 2 ? 0x0400 : 0x0001;
            g.rig_press = 3;
            std::fprintf(stderr, "[grab] rig: %s after %u frames of holding\n",
                         g.rig_var == 2 ? "release" : g.rig_var == 1 ? "spin release" : "throw",
                         g.rig_hold);
        }
        return;
    }
    g.rig_hold = 0;
    if (g.rig_var == 7) {
        // the stomp rig: every 60 frames drop onto the victim's puppet from 260 units up
        if (cf % 60u == 5u) {
            int *p = (int *)(mb + 0x5c);
            p[0] = s32r(vb, 0x5c);
            p[1] = s32r(vb, 0x60) + (260 << 12);
            p[2] = s32r(vb, 0x64);
            s32r(mb, 0x98) = 0;
            s32r(mb, 0xa8) = -0x8000;
            _ZN6Player11ChangeStateERNS_5StateE(mb, data_ov002_021101b4);
            ++g.rig_drops;
        }
        return;
    }
    // the approach: a punch to stun, then punches at the stunned puppet until it is caught
    const bool victim_pinned_stun = cf >= 70u && cf < 200u;
    if (cf == 0u || cf == 2u) {
        rig_place_behind(mb, vb);
        g.rig_pin = 12;
        if (cf == 2u && g.rig_var != 6) { g.rig_mask = 0x0001; g.rig_press = 3; ++g.rig_catches; }
        std::fprintf(stderr, "[grab] rig: cycle punch %u (var %d)\n", g.rig_frames / cycle, g.rig_var);
    }
    if (g.rig_var == 6 && (cf == 70u || cf == 90u || cf == 110u)) {
        // nostun: punches at a standing victim (the client gate says no, the lab knob forces the claim)
        rig_place_behind(mb, vb);
        g.rig_pin = 8;
        g.rig_mask = 0x0001;
        g.rig_press = 3;
    }
    if (victim_pinned_stun && g.rig_var != 6 && !g.h.active && (cf == 70u || cf == 86u || cf == 102u || cf == 118u)) {
        rig_place_behind(mb, vb);
        g.rig_pin = 10;
        g.rig_mask = 0x0001;
        g.rig_press = 3;
    }
}

void report_at_exit() {
    std::fprintf(stderr, "[grab] stomps claims=%llu ok=%llu denied=%llu applied=%llu touch_on_me=%llu "
                 "squash_seen=%llu rig_drops=%llu\n", g.stomp_claims, g.stomp_ok, g.stomp_denied,
                 g.stomp_applied, g.touch_on_me, g.squash_seen, g.rig_drops);
    std::fprintf(stderr, "[grab] exit contacts=%llu tries=%llu claims=%llu confirmed=%llu denied=%llu "
                 "unwound=%llu thrown_sent=%llu throws=%llu swings=%llu | v_grabbed=%llu v_released=%llu "
                 "v_breaks=%llu v_refused=%llu throws_applied=%llu | h_dropped=%llu bystander_links=%llu "
                 "| rig_catches=%llu | still_holding=%d still_held=%d\n",
                 g.contacts, g.tries, g.claims, g.confirmed, g.denied, g.unwound, g.thrown_sent,
                 g.how_count[1], g.how_count[2], g.v_grabbed, g.v_released, g.v_breaks, g.v_refused,
                 g.throws_applied, g.h_dropped, g.bystander_links, g.rig_catches, g.h.active ? 1 : 0,
                 g.v.active ? 1 : 0);
}

}  // namespace

bool vs_grab_contact(int victim, unsigned flags) {
    init_once();
    if (!on()) return false;
    // what I carry is not a target of my own spin (the swing's attack cylinder touches it)
    if (g.h.active && g.h.k == victim) return true;
    if (!(flags & 0x1000u)) return false;
    char *me = my_body();
    if (!me) return false;
    ++g.contacts;
    if (g.p.on || g.h.active || g.v.active) return false;
    if ((s32r(me, 8) & 7) != kLookWario && !g.force) return false;
    if (!g.force && !seat_catchable(victim)) return false;
    g.p.on = true;
    g.p.k = victim;
    g.p.flags = flags;
    g.p.frame = vs_arena_frame();
    if (g.log)
        std::fprintf(stderr, "[grab] contact: my attack (flags 0x%x) on the stunned puppet of seat %d: "
                     "TryGrab next frame\n", flags, victim);
    return true;
}

void vs_grab_on_event(const RefEvent *ev, int my_seat, bool directed) {
    (void)my_seat;
    if (!ev || !on()) return;
    init_once();
    if (directed) {
        if (ev->type == EV_DENY && ev->seat == (unsigned char)vs_arena_my_seat() && ev->a >= CLAIM_CATCH)
            ++g.deny_reason[ev->b < 16 ? ev->b : 0];
        return;
    }
    if (ev->type == EV_GRABBED) on_grabbed(*ev);
    else if (ev->type == EV_THROWN) on_thrown(*ev);
    else if (ev->type == EV_STOMPED) on_stomped(*ev);
}

void vs_grab_before_walk(int frame) {
    (void)frame;
    if (!vs_arena_active()) return;
    init_once();
    if (!vs_ref_active() || data_0209f2d8 != 1) return;
    static bool reg = false;
    if (!reg) { reg = true; std::atexit(report_at_exit); }
    if (!g.off) {
        h_try_catch();
        h_monitor();
        v_monitor();
        bystander_apply();
        stomp_monitor();
        hold_probe();
    }
    stomp_probe();
    if (g.rig) rig_step();       // (the lab rig runs with the layer off too: the old behaviour's control row)
}

void vs_grab_after_walk(int frame) {
    (void)frame;
    if (g.off || !g.inited || !vs_ref_active()) return;
    h_after_walk();
    stomp_scan();
}

VsGrabStats vs_grab_stats() {
    VsGrabStats s;
    s.wrapped = g.inited;
    s.contacts = g.contacts;
    s.tries = g.tries;
    s.claims = g.claims;
    s.confirmed = g.confirmed;
    s.denied = g.denied;
    s.unwound = g.unwound;
    s.thrown_sent = g.thrown_sent;
    s.v_grabbed = g.v_grabbed;
    s.v_released = g.v_released;
    s.v_breaks = g.v_breaks;
    s.h_dropped = g.h_dropped;
    s.bystander_links = g.bystander_links;
    return s;
}

// INTERACT9 exports (party / coop holds reuse the arena's link, unlink and carry-pose helpers; nothing else of
// this file runs outside arena mode).
void vs_grab_export_link(char *holder, char *carried, bool ready) { link_pair(holder, carried, ready); }
void vs_grab_export_unlink(char *holder, char *carried) { unlink_pair(holder, carried); }
void vs_grab_export_overlay(char *holder, char *carried) { puppet_carry_overlay(holder, carried); }


// SEAMVS1: a VS match that ended in place leaves this process at the title; the next match handed to it starts the grab
// state from nothing (who holds whom, the pending contact, the stomp wait). The lab knobs and the counters stay.
void vs_grab_match_reset() {
    g.h = Holder();
    g.v = Victim();
    g.p = Pending();
    g.st = Stomp();
    g.stomp_cool = 0;
    g.cool_until = 0;
    for (int &p : g.pair_h) p = kSeatNone;
}

}  // namespace port
