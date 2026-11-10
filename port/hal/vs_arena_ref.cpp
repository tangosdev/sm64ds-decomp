// ARENA MODE, the referee client, implemented. Run mp1, lane STARREF1 (plan A3).
// Read hal/vs_arena_ref.h first for what this layer is and who calls it.
//
// ===========================================================================
// THE SHAPE OF IT
// ===========================================================================
//
// Every console runs the ROM's own VS arena: the ROM's star actors, markers,
// bodies and HUD are the picture. The referee in the relay (referee.py) decides
// what two players could disagree about. So this file never lets the ROM decide
// those things on its own; it asks the ROM what happened on THIS screen, turns
// that into a claim, and applies what the referee says came of it.
//
//   STAR ORDER    The ROM wakes a marker when its star id equals
//                 order[progress] (data_0209f344 / data_0209f208). In an arena
//                 the order is a 256-byte host buffer of 0xFF that the
//                 referee's STAR_WAKE fills one slot at a time and the progress
//                 byte is the referee's, so every console wakes the same star.
//   MARKER GLASS  A marker whose cylinder took my attack is NOT broken here: the
//                 hit is a MARKER_BREAK claim, and the glass breaks (the ROM's
//                 own StarMarker::Collect) when the referee's MARKER_BROKEN
//                 arrives, on every console, mine included. The console that
//                 broke it owns the star's flight (the ROM's own physics) and
//                 streams it as SYNO; the others place their copy of the star
//                 from those frames.
//   COLLECT       The ROM's collect (func_ov002_020e8ef0) runs at once on my
//                 screen: sparkle, sound, the Yoshi in the star pose, the ROM's
//                 own provisional state 5. That IS the "grab" on my screen. The
//                 state-5 body (dispatch entry 5, wrapped here) HOLDS the star
//                 there until the referee's verdict: taken by me = the ROM body
//                 runs on (celebration); taken by somebody else = my Yoshi is
//                 released and the star does a short fly-off to the winner.
//   LEDGER        data_0209f310 mirrors the referee's scores every frame, so a
//                 provisional +1 never survives to the HUD and no console can
//                 count a star the referee did not give it.
//   HITS          A puppet body whose cylinder took MY attack is a hit on my
//                 screen: a HIT claim, and the pair pass's flags are cleared so
//                 the ROM never resolves it by itself. The victim's own console
//                 calls the ROM's Player::Hurt on its own body when the verdict
//                 names it. The star a hit costs is the referee's SPILL event;
//                 the ROM's own drop (func_ov002_020d94cc inside St_Hurt_Main)
//                 is muted by holding my own score at zero for that call.
//   THE END       Only the referee ends a match (MATCH_OVER). The ROM's own
//                 time-up is held until it arrives.
//
// Nothing here runs unless arena mode is installed in its relay shape.

#include "vs_arena_ref.h"

#include "comms_hub.h"
#include "vs_arena.h"
#include "vs_arena_grab.h"
#include "vs_arena_eat.h"

#include <windows.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace arena_wire;

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250[];
extern unsigned char data_0209f2d8;
extern signed char data_0209f310[];
extern unsigned char data_0209f208;
extern int data_0209f40c[];             /* the minimap's three marker arrays: 12 star markers, */
extern unsigned char data_0209f3e8[];   /* 9 caps, */
extern unsigned char data_0209f3a4[];   /* 8 spike bombs (Obj pointers, zeroed by Stage::InitResources) */
extern unsigned char *data_0209f344;
extern unsigned char data_0209f204;
extern unsigned char data_0209fc5c[];

char *_ZN8dActor_c10FindWithIDEj(unsigned id);
char *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned id, char *prev);
void _ZN7fBase_c18MarkForDestructionEv(char *c);
void _ZN12daStarBase_c27LinkSilverStarAndStarMarkerEPc(char *a, char *b);
void _ZN8daStar_c19func_ov002_020e7e58Ev(char *c);
void _ZN8daStar_c19func_ov002_020e9448Ev(unsigned char *p);
void _ZN12daStarBase_c7CollectEv(void *self);
void _ZN8daStar_c19func_ov002_020e930cEv(char *c);    // the ROM touch reader (PowerStar states 4 and 8)
int _ZN5Event6GetBitEj(unsigned bit);
short GetAngleToCamera(int i);
void _ZN6Player11ChangeStateERNS_5StateE(void *self, void *state);
void port_player_set_character(void *player, unsigned ch);   // hal/player_bridges.cpp: the door swap
int _ZN6Player17SetNoControlStateEhih(void *self, unsigned char a, int b, unsigned char c);
void _ZN6Player18SetNewHatCharacterEjjb(void *self, unsigned a, unsigned b, unsigned char c);
int _ZN6Player12Unk_020c9e5cEh(void *self, unsigned char h);
void _ZN6Player19func_ov002_020c9e18Ev(char *c);
int _ZN6Player7IsStateERNS_5StateE(void *self, void *state);
int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, const void *v,
        unsigned a, int knock, unsigned d, unsigned e, unsigned f);
char *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b,
        const void *pos, const void *v16, int area, int f);
void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);
void func_ov002_020d8d10(void *self, void *pos);
void func_020105cc(void *thiz, unsigned flags);
void func_02012790(unsigned id);
extern unsigned short data_ov002_020ff29c[];

// the star dispatch table PowerStar::Behavior reads (port/unmatched/
// PowerStar_States.cpp seats its entries; entry 5 is the collect confirm)
struct PortPmfRef { unsigned fn; int delta; };
extern PortPmfRef data_ov002_021109d8[];
extern void *port_hud_self;      /* hal/sub_actors.cpp: the live HUD (fBase actor) */
extern char data_ov002_0210df54;  /* the cap's "follows the player" state holder (the put-on) */

// state objects: St_Hurt, St_Walk-or-Wait (the neutral state the ROM returns to
// after a no-control sequence), St_NoControl
extern unsigned char data_ov002_02110094[];
extern unsigned char data_ov002_021100ac[];   // St_HurtWater (Init calls the star drop)
extern unsigned char data_ov002_0211010c[];   // St_DeadHit (Main calls the star drop)
extern unsigned char data_ov002_0211013c[];
extern unsigned char data_ov002_0211022c[];
// LUIGI INFECTION (hal/luigi_infection.cpp; lane ARENALUIGI1): the mode switch and the team the
// referee's INFECTED / TEAM_STATE events set there
int port_luigi_enabled(void);
int port_luigi_is_infected_slot(int slot);
void port_luigi_arena_infect(int seat, int cause, int by);
void port_luigi_arena_team(unsigned mask);
}  // extern "C"

namespace port {
namespace {

enum : int {
    kClassStar = 0xb2,           // PowerStar (VS)
    kClassMarker = 0xb4,         // StarMarker
    kClassPlayer = 0xbf,
    kClassHud = 0x14e,
    kClassCap = 0x10d,           // daObjMarioCap_c (VSREF3)
    kMaxCaps = 256,
    kCapPlacedMax = 31,
    kCapDropBase = 32,
    kMaxInst = 256,
    kMaxClaims = 16,
    kStHurt = 7, kStHurtWater = 8, kStElectrocute = 9, kStBurnFire = 10,
    kStBurnLava = 11, kStDeadPit = 13,
    kAttackMask = 0x27fe0,       // F2: the hit flags a punch / kick / pound has
    kMarkerBreakMask = 0x408000, // StarMarker::Behavior's own test
    kStarTouchMask = 0x400000,   // _ZN8daStar_c19func_ov002_020e930cEv: a body touched it
    kHoldMs = 500,               // no verdict: treated as denied (4.3)
    kFlyoffFrames = 10,
    kShowMaxMs = 3000,           // V4: a collected star stays over the winner's head this long at most
    kShowMinMs = 700,            // ... and at least this long (the winner's pose reaches us a trip late)
    kShowNoPoseMs = 1500,        // ... or until here when the winner's stream never shows the pose
    kShowPoseGapFrames = 8,      // frames out of the pose that end it
    kStarPoseState = 24,         // St_NoControl: the star-get pose (data_ov002_0211022c)
    kHeadAbove = 0xc8000,        // the ROM's own hold offset over the head (star5_arena)
    kHitRetryFrames = 8,
    kLocalTimeupGraceMs = 3000,
    kReportMs = 10000,
};

struct PClaim {
    bool used;
    unsigned short cid;
    unsigned char kind, target;
    unsigned short aux;
    unsigned stamp, my_frame, ref_frame;
    unsigned first_ms, last_ms;
    int result;                  // 0 pending, 1 named, -1 denied, -2 timeout
    int reason, winner;
    unsigned char inst;          // star instance it concerns (grab / break)
    bool resend;                 // false: no resend (QUIT after the first)
};

struct Inst {
    bool used;
    unsigned char star_id;       // the marker / star id (0..7)
    unsigned char marker;        // destination marker for a spill
    unsigned char state;         // 1 waking, 2 free, 3 taken
    unsigned char owner;         // seat that runs the flight; 0xFF none
    bool spilled;
    unsigned uid;                // the local PowerStar's actor uid, 0 none yet
    bool holding;                // my console grabbed it, verdict pending
    bool claimed;                // the grab claim was made (once per touch)
    bool pending;                // VSSTAR2: touched and claimed, the ROM star-get NOT started (held for the verdict)
    unsigned pend_flags, pend_id;// the ROM's touch (star +0x130 / +0x134) kept back until the verdict says it is mine
    unsigned retry_ms;           // no new claim for this star before this local time (after a loss)
    int result;                  // 0 pending, 1 mine, 2 lost, 3 timeout
    int winner;
    unsigned short grab_cid;
    int deny_reason;             // the referee's reason for the last refused grab (F9-b log)
    unsigned hold_ms;
    unsigned direct_n;           // VSHAND1: frames the ROM touch reader was called directly
    int fly;                     // overrule fly-off frames left
    int fly_from[3], fly_to[3];
    // the flight, as streamed (owner) or as received (the others)
    bool have_pos;
    int pos[3], last_pos[3];
    unsigned rx_ms, rx_frame;
    int still;
    unsigned last_tx;
    unsigned char tx_state;
    unsigned born_ms;
    unsigned t_broken;           // referee ms (my base) the glass was broken
    bool rig_break_sent, rig_race_done;
    unsigned rig_hurt_ref;       // hurtgrab rig: referee ms seat 1 hurt itself on this star
    bool rig_second;             // ... and went back for it
    // V4 (VSREF3): the star another seat took stays over that seat's head for its
    // star-get dance (no cylinder, never collectible), then goes out
    bool shown;
    unsigned char shown_seat;
    unsigned shown_ms;
    bool shown_seen;             // the winner's stream has shown the star-get pose
    int shown_off;               // frames since it left the pose
};

// One cap the referee knows by id (VSREF3): a cap the arena places (ids 1..31, cut
// from the level at the match's start) or one a hit knocked off a head (32..255).
struct CapRec {
    bool used;
    bool placed;
    int origin[3];               // placed: the ROM's own respawn point (Fix12)
    unsigned param1;             // placed: the param the ROM respawns it with
    int area;
    unsigned short hash;         // placed: the place hash a claim carries
    unsigned char look;          // 0..2, 0xFF unknown
    unsigned uid;                // the local actor, 0 none
    bool gone;                   // taken by a seat: off the field until back_ms
    unsigned back_ms;            // my ms the placed cap is spawned again (0 never)
    bool mine;                   // my body's pickup of it waits for its verdict
    unsigned short cid;          // ... and the claim that asked
    unsigned claim_ms;
    unsigned born_ms;
};

struct Ref {
    bool installed = false;
    bool active = false;         // arena + relay
    bool inited = false;         // the walk-time one-shots done
    int seat = -1;
    int bodies = 0;
    VsRefStats st{};
    // the stream
    unsigned short applied = 0;
    bool adopted = false;
    unsigned snap_mask = 0;
    unsigned char ledger_live = 0;
    signed char ledger[16];
    unsigned short ledger_seq = 0;
    unsigned char match_state = 0;
    unsigned short roster = 0;
    unsigned ever_roster = 0;
    unsigned short roster_now = 0;
    unsigned hdr_t0 = 0, hdr_match_ms = 0;
    unsigned char progress = 0;
    // READY / START
    bool ready_sent = false, ready_ok = false;
    unsigned short ready_cid = 0;
    unsigned ready_last = 0;
    bool session_up = false;
    bool started = false;
    unsigned t0 = 0;             // referee ms of match frame 0, my base
    int first_to_n = 0, time_s = 0, markers = 0;
    unsigned seed = 0;
    int vs_frames = 0;
    bool hud_set = false;
    int hud_units = 0;
    // countdown
    int cd_count = -1;
    bool cd_music = false;
    // the end
    bool over = false;
    VsRefOver over_v{};
    bool local_timeup = false;
    unsigned local_timeup_ms = 0;
    bool over_printed = false;
    // claims
    PClaim claims[kMaxClaims];
    unsigned short next_cid = 1;
    // stars
    Inst inst[kMaxInst];
    unsigned char order[256];
    bool order_installed = false;
    unsigned last_post_ms = 0;
    unsigned char syno_rr = 0;
    // hits
    unsigned last_hit_claim_frame[16];
    unsigned hit_applied_frame = 0;
    int prev_state = 15;
    // wraps
    unsigned star5_orig = 0;
    unsigned hurt_orig = 0;
    unsigned hurtwater_orig = 0;
    unsigned swim_clear_at = 0;
    unsigned deadhit_orig = 0;
    VsRefEventSink sink = nullptr;
    unsigned last_report = 0;
    bool log = false;
    // caps (VSREF3)
    CapRec caps[kMaxCaps];
    bool caps_ready = false;
    int caps_n = 0;
    int caps_try = 0;
    bool caps_off = false;       // SM64DS_ARENA_NOCAPS=1: the old behaviour (the control row)
    unsigned drop_uid = 0;       // the cap my own Hurt popped, until the referee names it
    unsigned drop_cid = 0;
    unsigned ignored[16];        // knocked-off caps seen and not mine (a ring)
    unsigned ignored_n = 0;
    unsigned prev_look = 3;
    int rig_drop_id = 0;         // capdrop rig: the knocked-off cap seat 1 goes for
    unsigned rig_drop_at = 0;
    unsigned rig_tongue_until = 0;   // captongue rig: seat 1 keeps tongueing the knocked-off cap until this ms
    unsigned rig_tongue_next = 0;
    // lab rigs (SM64DS_ARENA_RIG)
    int rig = 0;                 // 0 off, 1 race, 2 hit
    unsigned rig_seed = 1;
    int rig_n = 0;
    unsigned rig_next = 0;       // referee ms of the next scripted step
    int rig_press_frames = 0;
    int rig_press_total = 3;
    bool rig_real = false;       // chainr / hitr: no injected pair-pass words, a Mario-capped punch
    int rig_real_n = 0;
    unsigned rig_cap_frame = 0;
    int rig_inject = 0;
    int rig_follow = 0;          // frames left to keep my body at rig_dist units from the victim's puppet
    int rig_dist = 0;
    int rig_phase = 0;
    unsigned chain_hit_at = 0;   // referee ms (my base)
    int rig_home[3] = {0, 0, 0}; // seat 1's spawn point: where seat 0's (unmoved) copy of it stands
    bool rig_home_set = false;
    // LUIGI INFECTION (mode 2, lane ARENALUIGI1). The referee's team is the only truth: lmask
    // is the Luigi mask its INFECTED / TEAM_STATE events carry; the game's team bits and the
    // Luigi bodies follow it (hal/luigi_infection.cpp).
    bool luigi = false;          // this match is Luigi Infection (READY mode byte 2)
    unsigned lmask = 0;          // bit = seat, the referee's Luigis
    bool picked = false;         // the first Luigi has been picked (T0)
    unsigned long long tags_claimed = 0, infected_seen = 0, luigi_gated = 0, luigi_cap_blocks = 0;
    unsigned last_tag_claim_frame[16];
    int rig_lphase = 0;          // the luigi lab rig's step
    unsigned rig_lnext = 0;
    int rig_lvictim = -1;
    int rig_cd_pos[3] = {0, 0, 0};   // luigi rigs: my position when the countdown walk began
    int rig_cd_phase = 0;
    int rig_lclose = 0;              // luigipunch: frames until the swing is carried onto the victim
    int rig_lclose_n = 0;
};
Ref g;

// ---------------------------------------------------------------------------
// small helpers
// ---------------------------------------------------------------------------
inline unsigned now_ms() { return comms_hub_now_ms(); }

bool env_on(const char *n) {
    const char *s = std::getenv(n);
    return s && *s && !(s[0] == '0' && s[1] == 0);
}

int env_int(const char *n, int def, int lo, int hi) {
    const char *s = std::getenv(n);
    if (!s || !*s) return def;
    char *end = nullptr;
    const long v = std::strtol(s, &end, 10);
    if (end == s || *end || v < lo || v > hi) return def;
    return (int)v;
}

inline unsigned char *u8p(char *c, int off) { return (unsigned char *)(c + off); }
inline unsigned short &u16r(char *c, int off) { return *(unsigned short *)(c + off); }
inline unsigned &u32r(char *c, int off) { return *(unsigned *)(c + off); }
inline int &s32r(char *c, int off) { return *(int *)(c + off); }
inline short &s16r(char *c, int off) { return *(short *)(c + off); }
inline unsigned actor_uid(const char *c) { return *(const unsigned *)(c + 4); }
inline unsigned actor_class(const char *c) { return *(const unsigned short *)(c + 0xc); }

char *my_body() {
    if (g.seat < 0 || g.seat >= kArenaSeats) return nullptr;
    return (char *)data_0209f394[g.seat];
}

char *body_of(int seat) {
    if (seat < 0 || seat >= kArenaSeats) return nullptr;
    return (char *)data_0209f394[seat];
}

// ---------------------------------------------------------------------------
// FIELD4 F9-b / F9-c: the referee's "may this body act / be hit" tests, mirrored
// here so a console never starts what the referee will refuse (a collect pose
// followed by a tossed-back star, a hit spark with no hit). The lists are
// referee.py's DISABLED_STATES (state indices 5 7..13 16 20 24 32 36 54).
// ---------------------------------------------------------------------------
bool state_disabled(int st) {
    switch (st) {
    case 5: case 7: case 8: case 9: case 10: case 11: case 12: case 13:
    case 16: case 20: case 24: case 32: case 36: case 54:
        return true;
    }
    return false;
}

// May my own body collect a star / break a marker right now?  The same test the
// referee runs on my SYNS sample (_usable): a disabled state (hurt, burning, a
// mouth, the star-get pose ...), mIsTakingDamage, mIsNoControl or a body that
// is inside somebody's mouth.
bool luigi_before_t0();
bool my_body_may_act() {
    // LUIGI INFECTION: everyone runs during the five-second countdown (Tango 11-09), and the
    // referee takes no claim before T0 (DENY_STALE): a star or marker touch then is not made
    if (luigi_before_t0()) return false;
    char *me = my_body();
    if (!me) return true;
    const int st = vs_arena_state_index(me);
    if (st != 0xFF && state_disabled(st)) return false;
    if (*u8p(me, 0x708) || *u8p(me, 0x709)) return false;
    if (*(void **)(me + 0xd0)) return false;
    return true;
}

// Can a hit on this puppet land?  The referee's victim test (_hit) on the
// sample of the frame I drew: disabled state, hurt / no-control, invulnerable,
// mega / vanish / metal, or in a mouth.  Judged on the stream I already hold.
bool puppet_hit_proof(int seat, unsigned drawn, int *why_st, int *why_inv) {
    ArenaSample smp[kArenaRing];
    const int n = vs_arena_samples(seat, smp, kArenaRing);
    if (n <= 0) return false;
    const ArenaState *best = &smp[n - 1].s;          // oldest, if none is old enough
    for (int k = 0; k < n; ++k)
        if (smp[k].s.frame <= drawn) { best = &smp[k].s; break; }   // newest first
    *why_st = best->state_idx;
    *why_inv = best->invuln;
    return state_disabled(best->state_idx) ||
           (best->flags & (kArenaInHurt | kArenaNoControl)) != 0 ||
           best->invuln > 0 ||
           (best->power & (kArenaMega | kArenaVanish | kArenaMetal)) != 0 ||
           best->eaten_by != 0;
}

// every live actor of one class, by the ROM's own list walk (the same one the
// ROM's FindWithActorID callers use)
template <typename F>
void each_class(unsigned cls, F f) {
    char *a = nullptr;
    for (int guard = 0; guard < 64; ++guard) {
        a = _ZN8dActor_c15FindWithActorIDEjPS_(cls, a);
        if (!a) break;
        f(a);
    }
}

char *find_class(unsigned cls, char *prev) {
    return _ZN8dActor_c15FindWithActorIDEjPS_(cls, prev);
}

// my referee-clock estimate right now, my base
bool ref_now(unsigned *out) {
    int rtt;
    return vs_arena_referee_now(out, &rtt);
}

// the referee-clock stamp of a past local instant
bool stamp_for(unsigned local_ms, unsigned *stamp) {
    unsigned r;
    if (!ref_now(&r)) return false;
    const unsigned now = now_ms();
    *stamp = r - (now - local_ms);
    return true;
}

int popcount16(unsigned m) {
    int n = 0;
    for (; m; m &= m - 1) ++n;
    return n;
}

// LUIGI INFECTION helpers: whose team a seat is on (the referee's mask), and the countdown
bool is_luigi(int seat) { return g.luigi && seat >= 0 && seat < 32 && (g.lmask >> seat & 1u) != 0; }
bool me_luigi() { return is_luigi(g.seat); }
bool luigi_before_t0() {
    if (!g.luigi) return false;
    unsigned r;
    return !g.started || !ref_now(&r) || (int)(r - g.t0) < 0;
}

unsigned short seq_next(unsigned short s) { return s == 0xFFFF ? 1 : (unsigned short)(s + 1); }
unsigned short seq_prev(unsigned short s) { return s <= 1 ? 0xFFFF : (unsigned short)(s - 1); }
bool seq_old(unsigned short applied, unsigned short s) {
    const unsigned short d = (unsigned short)(applied - s);
    return d < 0x8000u;           // s is at or before applied
}

// ---------------------------------------------------------------------------
// CLAIMS
// ---------------------------------------------------------------------------
PClaim *claim_by_id(unsigned short cid) {
    for (auto &c : g.claims)
        if (c.used && c.cid == cid) return &c;
    return nullptr;
}

PClaim *claim_slot() {
    for (auto &c : g.claims)
        if (!c.used) return &c;
    // recycle the oldest finished one
    PClaim *best = nullptr;
    for (auto &c : g.claims)
        if (c.result != 0 && (!best || c.first_ms < best->first_ms)) best = &c;
    return best;
}

void send_claim(PClaim &c) {
    RefClaim m;
    std::memset(&m, 0, sizeof m);
    m.tag = kTagE;
    m.version = 1;
    m.seat = 0xFF;                // the hub stamps it
    m.kind = c.kind;
    m.target = c.target;
    m.claim_id = c.cid;
    m.aux = c.aux;
    m.stamp = c.stamp;
    m.my_frame = c.my_frame;
    m.ref_frame = c.ref_frame;
    comms_hub_send(&m, (int)sizeof m);
    c.last_ms = now_ms();
}

unsigned short new_cid() {
    unsigned short c = g.next_cid;
    g.next_cid = seq_next(g.next_cid);
    return c;
}

const char *claim_name(int k) {
    static const char *n[] = {"?", "READY", "MARKER_BREAK", "GRAB", "HIT",
                              "HURT_SELF", "PIT", "QUIT", "EAT", "SPIT",
                              "SWALLOW", "ESCAPE", "CATCH", "THROW", "SWING",
                              "BREAK", "STOMP", "CAP_TAKE", "CAP_DROP", "TAG"};
    return k >= 1 && k <= 19 ? n[k] : "?";
}

PClaim *make_claim(unsigned char kind, unsigned char target, unsigned short aux,
                  unsigned stamp, unsigned my_frame, unsigned ref_frame) {
    PClaim *c = claim_slot();
    if (!c) return nullptr;
    std::memset(c, 0, sizeof *c);
    c->used = true;
    c->cid = new_cid();
    c->kind = kind;
    c->target = target;
    c->aux = aux;
    c->stamp = stamp;
    c->my_frame = my_frame;
    c->ref_frame = ref_frame;
    c->first_ms = now_ms();
    c->resend = true;
    ++g.st.claims_sent;
    send_claim(*c);
    if (g.log)
        std::fprintf(stderr, "[vsarena-ref] claim %s id=%u target=%u aux=%u "
                     "stamp=%u my_frame=%u ref_frame=%u\n", claim_name(kind),
                     c->cid, target, aux, stamp, my_frame, ref_frame);
    return c;
}

void pump_claims() {
    const unsigned now = now_ms();
    for (auto &c : g.claims) {
        if (!c.used || c.result != 0) continue;
        if (now - c.first_ms >= (unsigned)kVerdictTimeoutMs &&
            c.kind != CLAIM_READY) {
            c.result = -2;
            continue;
        }
        if (c.resend && now - c.last_ms >= (unsigned)kClaimResendMs) {
            send_claim(c);
            ++g.st.claims_resent;
        }
    }
}

void finish_claim(unsigned short cid, int result, int reason, int winner) {
    PClaim *c = claim_by_id(cid);
    // a verdict the referee repeats (it answers every resend of a claim) is
    // counted once: a claim already named, or already denied, stays as it is
    if (!c || c->result > 0 || (c->result < 0 && result < 0)) return;
    c->result = result;
    c->reason = reason;
    c->winner = winner;
    ++g.st.verdicts;
    if (result < 0) ++g.st.denied;
}

// ---------------------------------------------------------------------------
// THE ORDER BUFFER + the ROM's progress byte
// ---------------------------------------------------------------------------
void install_order() {
    if (g.order_installed) return;
    std::memset(g.order, 0xFF, sizeof g.order);
    data_0209f344 = g.order;
    g.order_installed = true;
}

// ---------------------------------------------------------------------------
// ACTOR LOOKUPS
// ---------------------------------------------------------------------------
char *marker_by_id(int id) {
    char *hit = nullptr;
    each_class(kClassMarker, [&](char *a) {
        if (!hit && *u8p(a, 0x1d9) == id && *u8p(a, 0x1d8) != 0) hit = a;
    });
    return hit;
}

int count_markers() {
    int top = -1;
    each_class(kClassMarker, [&](char *a) {
        if (*u8p(a, 0x1d8) != 0 && *u8p(a, 0x1d9) < 8 &&
            *u8p(a, 0x1d9) > top) top = *u8p(a, 0x1d9);
    });
    return top + 1;
}

char *star_by_uid(unsigned uid) {
    return uid ? _ZN8dActor_c10FindWithIDEj(uid) : nullptr;
}

bool uid_taken(unsigned uid) {
    for (auto &i : g.inst)
        if (i.used && i.uid == uid) return true;
    return false;
}

// the level-placed PowerStar of a star id, still dormant (state 9)
char *dormant_star(int star_id) {
    char *hit = nullptr;
    each_class(kClassStar, [&](char *a) {
        if (hit) return;
        if (*u8p(a, 0x49d) != star_id) return;
        if (s32r(a, 0x440) != 9) return;
        if (uid_taken(actor_uid(a))) return;
        hit = a;
    });
    return hit;
}

Inst *inst_by_uid(unsigned uid) {
    if (!uid) return nullptr;
    for (auto &i : g.inst)
        if (i.used && i.uid == uid) return &i;
    return nullptr;
}

Inst *inst_by_id(unsigned id) {
    if (id == 0 || id >= (unsigned)kMaxInst) return nullptr;
    return g.inst[id].used ? &g.inst[id] : nullptr;
}

void ensure_star_uid(Inst &i) {
    if (i.uid || i.spilled) return;
    if (char *s = dormant_star(i.star_id)) i.uid = actor_uid(s);
}

// ---------------------------------------------------------------------------
// STAR EFFECTS
// ---------------------------------------------------------------------------
void unlink_and_destroy_star(char *s) {
    if (!s) return;
    char *m = _ZN8dActor_c10FindWithIDEj(u32r(s, 0x434));
    if (m) _ZN12daStarBase_c27LinkSilverStarAndStarMarkerEPc(m, nullptr);
    _ZN8daStar_c19func_ov002_020e7e58Ev(s);
    _ZN7fBase_c18MarkForDestructionEv(s);
}

void break_marker(int inst_id, int star_id, int breaker) {
    char *m = marker_by_id(star_id);
    if (!m) return;
    if (*u8p(m, 0x1db) & 1) return;                 // already broken
    // the ROM's star pops "away from the breaker's camera"; the flight is the
    // owner's (the breaker's console runs it), so any body will do as the hit
    // actor on the others, and mine is always a valid camera index.
    (void)breaker;
    s32r(m, 0x1d0) = (int)(size_t)my_body();
    _ZN12daStarBase_c7CollectEv(m);
    if (g.log)
        std::fprintf(stderr, "[vsarena-ref] marker %d broken (inst %d, breaker "
                     "seat %d)\n", star_id, inst_id, breaker);
}

// a dropped star: the ROM's own spawn (func_ov002_020d94cc's) with the
// referee's marker instead of its RNG
// a marker a dropped star can land on, the ROM's own rule (func_ov002_020e7554):
// broken glass with no star linked. The referee's pick stands when it is one;
// otherwise the lowest-numbered free one (the same on every console, since the
// glass states agree).
int pick_free_marker(int want) {
    auto is_free = [](int id) {
        char *m = marker_by_id(id);
        return m && (*u8p(m, 0x1db) & 1) &&
               _ZN8dActor_c10FindWithIDEj(u32r(m, 0x1cc)) == nullptr;
    };
    if (is_free(want)) return want;
    for (int id = 0; id < 8; ++id)
        if (is_free(id)) return id;
    return want;
}

char *spawn_spill_star(int marker_id, const int *origin, int victim_seat) {
    marker_id = pick_free_marker(marker_id);
    int pos[3] = { origin[0], origin[1] + 0x82000, origin[2] };
    char *vb = body_of(victim_seat);
    const int area = vb ? (signed char)*u8p(vb, 0xcc) : 0;
    char *s = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        kClassStar, 0x10, pos, nullptr, area, -1);
    if (!s) return nullptr;
    if (vb) {
        s16r(s, 0x92) = s16r(vb, 0x92);
        s16r(s, 0x94) = s16r(vb, 0x94);
        s16r(s, 0x96) = s16r(vb, 0x96);
        s32r(s, 0x98) = s32r(vb, 0x98);
    }
    char *m = marker_by_id(marker_id);
    if (m) {
        u32r(s, 0x434) = actor_uid(m);
        _ZN12daStarBase_c27LinkSilverStarAndStarMarkerEPc(m, s);
    }
    u16r(s, 0x4a2) |= 0x40;
    return s;
}

// ---------------------------------------------------------------------------
// EVENTS
// ---------------------------------------------------------------------------
const char *reason_name(int r) {
    static const char *n[] = {"?", "STALE", "GONE", "LOST", "RANGE", "STATE",
                              "INVULN", "MISMATCH", "UNSUPPORTED", "BAD",
                              "TOO_OLD", "SELF", "TEAM"};
    return r >= 1 && r <= 12 ? n[r] : "?";
}


// ---------------------------------------------------------------------------
// BOUNDS ON EVERYTHING THE WIRE GIVES US (the standing security law)
//
// The hub checks a SYNR / SYNO only for length, version and shape; what is IN
// the fields is this layer's to check, before any of it indexes a table, spawns
// a star, writes the ledger or hurts a body. A relay we trust still forwards
// what a bug or a stranger's forged datagram puts in a field, so every field is
// range-checked here and a frame / event / object frame that fails any check is
// dropped whole and counted (VsRefStats frames_bad / events_bad / syno_bad).
// An event dropped inside the ordered stream still advances the sequence (the
// stream must not stall behind one bad record), it just changes nothing.
// ---------------------------------------------------------------------------
enum : int { kPosLim = 32768 };            // units, like arena_state_valid
inline bool pos_ok(int v) { return v >= -(kPosLim << 12) && v <= (kPosLim << 12); }
inline int seat_lim() { return g.bodies > 0 && g.bodies <= kArenaSeats ? g.bodies : 4; }
inline bool seat_ok(unsigned s) { return s < (unsigned)seat_lim(); }
inline bool seat_or_none(unsigned s) { return s == 0xFF || seat_ok(s); }
inline bool score_ok(int v) { return v >= 0 && v <= 127; }   // the referee's ledger: never negative, capped at 127

bool ref_event_valid(const RefEvent &e) {
    const unsigned rmask = (1u << seat_lim()) - 1u;
    switch (e.type) {
    case EV_START:
        return e.seat == 0xFF && e.a <= 5 && e.b <= 99 && e.c <= 3600 &&
               (e.d & ~rmask) == 0 && e.e >= 1 && e.e <= 8 && e.y >= 0 && e.y <= 15;
    case EV_STAR_WAKE:
        return e.seat == 0xFF && e.a != 0 && e.b < 8 && e.c < 256;
    case EV_MARKER_BROKEN:
        return seat_ok(e.seat) && e.a != 0 && e.b < 8 && pos_ok(e.x) &&
               pos_ok(e.y) && pos_ok(e.z);
    case EV_STAR_TAKEN:
        return seat_ok(e.seat) && e.a != 0 && score_ok(e.b) &&
               e.e < 256 && pos_ok(e.x) && pos_ok(e.y) && pos_ok(e.z);
    case EV_DENY:   // (contract 2: claim kinds up to TAG 19, reasons up to DENY_TEAM 12)
        return seat_ok(e.seat) && e.a >= 1 && e.a <= CLAIM_TAG && e.b >= 1 && e.b <= DENY_TEAM &&
               (e.d == 0xFF || e.d < 16);
    case EV_HIT:
        return seat_ok(e.seat) && seat_ok(e.a) && e.b <= 1 && e.d <= 0x2000 &&
               e.e < 256 && pos_ok(e.x) && pos_ok(e.y) && pos_ok(e.z);
    case EV_CLASH:
        return seat_ok(e.seat) && seat_ok(e.a) && pos_ok(e.x) && pos_ok(e.y) &&
               pos_ok(e.z);
    case EV_SPILL:
        return seat_or_none(e.seat) && e.a != 0 && e.b < 8 && e.c >= 1 && e.c <= 4 &&
               score_ok(e.d) && pos_ok(e.x) && pos_ok(e.y) && pos_ok(e.z);
    case EV_MATCH_OVER:
        // reasons 5 / 6 (LUIGIS_WIN / SURVIVORS_WIN) only in a Luigi match. The winners mask
        // is bounded by the arena's bodies, never by the live roster: SURVIVORS_WIN names
        // the survivors at the bell, and one who left inside the hold is still a winner
        return e.seat == 0xFF && e.a >= 1 &&
               e.a <= (g.luigi ? OVER_SURVIVORS_WIN : OVER_EVERYONE_LEFT) &&
               e.b <= (unsigned)seat_lim() && (e.c & ~rmask) == 0;
    case EV_LEAVE:
        return seat_ok(e.seat) && e.a >= 1 && e.a <= 3 && e.d < 256;
    case EV_JOIN:
        return seat_ok(e.seat) && e.a <= 1;
    case EV_STAR_STATE:
        return seat_or_none(e.seat) && e.a != 0 && e.b >= 1 && e.b <= 2 &&
               e.c < 8 && e.d < 256 && pos_ok(e.x) && pos_ok(e.y) && pos_ok(e.z);
    case EV_ACCEPTED:
        return seat_ok(e.seat) && e.a >= 1 && e.a <= CLAIM_TAG && e.d < 256;
    case EV_CAP_TAKEN:
        return seat_ok(e.seat) && e.a != 0 && (e.b <= 2 || e.b == 0xFF) &&
               e.d <= 10000 && pos_ok(e.x) && pos_ok(e.y) && pos_ok(e.z);
    case EV_CAP_DROPPED:
        return seat_ok(e.seat) && e.a >= kCapDropBase && e.b <= 2 &&
               pos_ok(e.x) && pos_ok(e.y) && pos_ok(e.z);
    case EV_CAP_STATE:
        return e.seat == 0xFF && e.a != 0 &&
               ((e.b == 1 && e.a <= kCapPlacedMax) || (e.b == 2 && e.a >= kCapDropBase)) &&
               e.c <= 20000 && (e.d <= 2 || e.d == 0xFF) &&
               pos_ok(e.x) && pos_ok(e.y) && pos_ok(e.z);
    case EV_EATEN:
    case EV_RELEASED:
        return seat_ok(e.seat) && seat_ok(e.a) && e.b <= 4 && pos_ok(e.x) &&
               pos_ok(e.y) && pos_ok(e.z);
    case EV_GRABBED:
        return seat_ok(e.seat) && seat_ok(e.a) && e.seat != e.a && pos_ok(e.x) &&
               pos_ok(e.y) && pos_ok(e.z);
    case EV_THROWN:
        return seat_ok(e.seat) && seat_ok(e.a) && e.seat != e.a && e.b >= 1 && e.b <= 4 &&
               e.x >= 0 && e.x <= kGrabSpeedMax && e.y == 0 && e.z == 0;
    case EV_STOMPED:
        return seat_ok(e.seat) && seat_ok(e.a) && e.seat != e.a && pos_ok(e.x) &&
               pos_ok(e.y) && pos_ok(e.z);
    case EV_INFECTED:
        // only in a Luigi match; seat = the new Luigi, a = the tagger or 0xFF, b = INF_PICK..
        // INF_RETURN, d = the whole Luigi mask (it holds the new Luigi), e = survivors left
        return g.luigi && seat_ok(e.seat) && seat_or_none(e.a) && e.a != e.seat &&
               e.b >= INF_PICK && e.b <= INF_RETURN && (e.d & ~rmask) == 0 &&
               (e.d >> e.seat & 1u) != 0 && e.e <= (unsigned)seat_lim() &&
               pos_ok(e.x) && pos_ok(e.y) && pos_ok(e.z);
    case EV_TEAM_STATE:
        // a snapshot record of a Luigi match: a = picked yet, c = the Luigi mask, d = the roster
        return g.luigi && e.seat == 0xFF && e.a <= 1 && (e.c & ~rmask) == 0 &&
               (e.d & ~rmask) == 0;
    default:
        return false;
    }
}

bool ref_frame_valid(const RefFrame &f) {
    if (f.tag != kTagR || f.version != 1 || f.seat != kRefereeMark) return false;
    if (f.zero != 0 || f.n_events > kSynrEvents || f.match_state > MS_OVER) return false;
    if (f.flags & ~(SYNR_F_SNAPSHOT | SYNR_F_DIRECTED | SYNR_F_RESEND)) return false;
    if (f.first_to_n > 99 || f.order_row > 5 || f.map > 15) return false;
    if (f.match_ms > 3600u * 1000u) return false;
    const unsigned rmask = (1u << seat_lim()) - 1u;
    if (f.roster & ~rmask) return false;
    for (int k = 0; k < 16; ++k) {
        if (!score_ok(f.scores[k])) return false;
        if (k >= seat_lim() && f.scores[k] != 0) return false;
    }
    const int idx = (f.part >> 4) & 0xF, cnt = f.part & 0xF;
    if (f.flags & SYNR_F_SNAPSHOT) {
        if (cnt < 1 || idx >= cnt) return false;
    } else if (f.part != 0) {
        return false;
    }
    return true;
}

bool syno_valid(const ObjFrame &o, int seat) {
    if (o.tag != kTagO || o.version != 1 || o.obj_class != 1) return false;
    if (o.obj_id == 0 || !seat_ok((unsigned)seat)) return false;
    if (o.state < 1 || o.state > 3 || (o.flags & ~1u)) return false;
    return pos_ok(o.pos[0]) && pos_ok(o.pos[1]) && pos_ok(o.pos[2]);
}

void session_up(const char *why) {
    if (g.session_up) return;
    g.session_up = true;
    std::fprintf(stderr, "[vsarena] session up players=%d (%s)\n",
                 g.roster ? popcount16(g.roster) : g.bodies, why);
    std::fflush(stderr);
}

void on_start(const RefEvent &e) {
    if (g.started || g.over) return;
    g.started = true;
    g.t0 = e.t;
    g.progress = 0;
    g.first_to_n = e.b;
    g.time_s = e.c;
    g.roster = (unsigned short)e.d;
    g.markers = e.e;
    g.seed = (unsigned)e.x;
    ++g.st.events_applied;
    session_up("START");
    std::fprintf(stderr, "[vsarena] START t0=%u first_to=%d time=%ds roster=0x%04x "
                 "markers=%d seed=%u map=%d\n", e.t, e.b, e.c, e.d, e.e,
                 (unsigned)e.x, e.y);
}

void on_wake(const RefEvent &e) {
    Inst *i = &g.inst[e.a ? e.a : 1];
    if (e.a == 0) return;
    std::memset(i, 0, sizeof *i);
    i->used = true;
    i->star_id = e.b;
    i->marker = e.b;
    i->state = 1;
    i->owner = 0xFF;
    i->born_ms = now_ms();
    if (e.c < sizeof g.order) {
        g.order[e.c] = e.b;
        g.progress = (unsigned char)e.c;
        data_0209f208 = (unsigned char)e.c;
    }
    ensure_star_uid(*i);
    std::fprintf(stderr, "[vsarena-ref] STAR_WAKE inst=%d star=%d progress=%d\n",
                 e.a, e.b, e.c);
}

void on_marker_broken(const RefEvent &e) {
    Inst *i = inst_by_id(e.a);
    if (!i) {
        // a console that missed the wake: make the entry from the event
        if (e.a == 0) return;
        i = &g.inst[e.a];
        std::memset(i, 0, sizeof *i);
        i->used = true;
        i->star_id = e.b;
        i->marker = e.b;
    }
    i->state = 2;
    i->owner = e.seat;
    i->born_ms = now_ms();
    i->t_broken = e.t;
    ensure_star_uid(*i);
    break_marker(e.a, e.b, e.seat);
    if (e.seat == g.seat) finish_claim(e.c, 1, 0, 0xFF);
}

// ---------------------------------------------------------------------------
// V4: THE HELD STAR ON THE OTHER CONSOLES. The ROM draws the star over the
// collector's head on every DS; here only the collector's console owns the
// actor, and the others used to destroy theirs the instant the verdict named
// the winner. Now they keep it, switch its cylinder off (nobody can touch it),
// pin it to the winner's puppet at the ROM's own hold height and let it go out
// when the winner's stream leaves the star-get pose (at most kShowMaxMs).
// ---------------------------------------------------------------------------
unsigned my_frame_now();

bool winner_in_star_pose(int seat) {
    ArenaSample smp[2];
    if (vs_arena_samples(seat, smp, 1) <= 0) return false;
    return (smp[0].s.flags & kArenaStarLock) != 0 ||
           smp[0].s.state_idx == kStarPoseState;
}

bool show_star_start(Inst &i, char *s, int winner) {
    static const bool off = env_on("SM64DS_ARENA_NOHELDSTAR");   // the control row: the old behaviour
    if (off || !g.active) return false;
    char *wb = body_of(winner);
    if (!wb || winner == g.seat) return false;
    // the marker lets go of the star at once, so a spill can use it again; the
    // star's own effect actor is released when it goes out
    if (char *m = _ZN8dActor_c10FindWithIDEj(u32r(s, 0x434)))
        _ZN12daStarBase_c27LinkSilverStarAndStarMarkerEPc(m, nullptr);
    s32r(s, 0x128) |= 1;                  // its cylinder off
    i.shown = true;
    i.shown_seat = (unsigned char)winner;
    i.shown_ms = now_ms();
    i.shown_seen = false;
    i.shown_off = 0;
    ++g.st.stars_shown;
    std::fprintf(stderr, "[vsarena-ref] HELD STAR inst=%d: kept over the head of seat %d "
                 "(frame %u)\n", (int)(&i - g.inst), winner, my_frame_now());
    return true;
}

void show_star_end(Inst &i, char *s, const char *why) {
    i.shown = false;
    std::fprintf(stderr, "[vsarena-ref] HELD STAR inst=%d out after %u ms (%s, pose seen %d)\n",
                 (int)(&i - g.inst), now_ms() - i.shown_ms, why, i.shown_seen ? 1 : 0);
    if (s) unlink_and_destroy_star(s);
}

void shown_stars() {
    for (int k = 1; k < kMaxInst; ++k) {
        Inst &i = g.inst[k];
        if (!i.used || !i.shown) continue;
        char *s = star_by_uid(i.uid);
        if (!s) { i.shown = false; continue; }
        char *wb = body_of(i.shown_seat);
        const unsigned age = now_ms() - i.shown_ms;
        if (!wb) { show_star_end(i, s, "winner gone"); continue; }
        if (age >= (unsigned)kShowMaxMs) { show_star_end(i, s, "time limit"); continue; }
        if (winner_in_star_pose(i.shown_seat)) { i.shown_seen = true; i.shown_off = 0; }
        else if (i.shown_seen) ++i.shown_off;
        if (age >= (unsigned)kShowMinMs &&
            ((i.shown_seen && i.shown_off >= kShowPoseGapFrames) ||
             (!i.shown_seen && age >= (unsigned)kShowNoPoseMs))) {
            show_star_end(i, s, i.shown_seen ? "winner left the pose" : "pose never seen");
            continue;
        }
        s32r(s, 0x5c) = s32r(wb, 0x5c);
        s32r(s, 0x60) = s32r(wb, 0x60) + kHeadAbove;
        s32r(s, 0x64) = s32r(wb, 0x64);
        s32r(s, 0x98) = 0;
        s32r(s, 0xa8) = 0;
        s32r(s, 0x128) |= 1;
        if (g.log && (g.st.shown_frames % 10) == 0)
            std::fprintf(stderr, "[vsarena-ref] HELD STAR inst=%d frame +%u ms: star at (%d,%d,%d), winner's "
                         "body at (%d,%d,%d)\n", k, age, s32r(s, 0x5c) >> 12, s32r(s, 0x60) >> 12,
                         s32r(s, 0x64) >> 12, s32r(wb, 0x5c) >> 12, s32r(wb, 0x60) >> 12, s32r(wb, 0x64) >> 12);
        ++g.st.shown_frames;
    }
}

// ---------------------------------------------------------------------------
// CAP PROBE (lab only, SM64DS_CAP_CENSUS=1): every cap actor (class 269) with its
// ROM fields, and one line whenever one is born, dies or changes its state
// holder / the player it follows. The census the referee's cap table is cut from,
// and the timeline of a pickup (follow state, put-on, respawn) on this console.
// ---------------------------------------------------------------------------
struct CapSeen { unsigned uid; unsigned holder, follow; int type; bool live; };
CapSeen g_cap_seen[48];

void cap_probe() {
    static const bool on = env_on("SM64DS_CAP_CENSUS");
    if (!on) return;
    bool seen_now[48] = {};
    each_class(kClassCap, [&](char *a) {
        const unsigned uid = actor_uid(a);
        int slot = -1, freeslot = -1;
        for (int k = 0; k < 48; ++k) {
            if (g_cap_seen[k].live && g_cap_seen[k].uid == uid) { slot = k; break; }
            if (!g_cap_seen[k].live && freeslot < 0) freeslot = k;
        }
        const unsigned holder = u32r(a, 0x3bc), follow = u32r(a, 0x3c0);
        const int type = s32r(a, 0x3f0);
        if (slot < 0 && freeslot >= 0) {
            slot = freeslot;
            g_cap_seen[slot] = { uid, ~0u, ~0u, type, true };
            std::fprintf(stderr, "[cap] f=%u BORN uid=%u pos=(%d,%d,%d) param=0x%x type=%d model=%d area=%d "
                         "death=%d unk400=%u timer=%u\n", my_frame_now(), uid, s32r(a, 0x5c) >> 12,
                         s32r(a, 0x60) >> 12, s32r(a, 0x64) >> 12, u32r(a, 8), type, s32r(a, 0x3f4),
                         (int)(signed char)*u8p(a, 0xcc), (int)(signed char)*u8p(a, 0xce),
                         (unsigned)*u8p(a, 0x400), (unsigned)u16r(a, 0x100));
        }
        if (slot < 0) return;
        seen_now[slot] = true;
        CapSeen &c = g_cap_seen[slot];
        if (c.holder != holder || c.follow != follow) {
            std::fprintf(stderr, "[cap] f=%u uid=%u state holder %08x -> %08x follows=%08x (me=%08x) type=%d "
                         "pos=(%d,%d,%d) timer=%u\n", my_frame_now(), uid,
                         c.holder, holder, follow, (unsigned)(size_t)my_body(), type,
                         s32r(a, 0x5c) >> 12, s32r(a, 0x60) >> 12, s32r(a, 0x64) >> 12,
                         (unsigned)u16r(a, 0x100));
            c.holder = holder;
            c.follow = follow;
        }
    });
    for (int k = 0; k < 48; ++k)
        if (g_cap_seen[k].live && !seen_now[k]) {
            std::fprintf(stderr, "[cap] f=%u DIED uid=%u type=%d\n", my_frame_now(), g_cap_seen[k].uid,
                         g_cap_seen[k].type);
            g_cap_seen[k].live = false;
        }
}

// ---------------------------------------------------------------------------
// CAPS (VSREF3). The referee owns every cap the way it owns every star.
//
//   PLACED CAPS   The arena's caps get ids 1..31 at the match's start (sorted by
//                 x, z, y, the order the referee's table is cut in). My own body
//                 touching one runs the ROM's pickup at once (the optimistic
//                 half) and sends CAP_TAKE; the referee grants the earliest
//                 stamp. Everybody else's console deletes its copy on CAP_TAKEN,
//                 and every console (the taker's too) spawns the cap again
//                 CAP_TAKEN.d ms after the event's stamp, the ROM's own
//                 respawn (its type 4 puts a new one at its origin when the
//                 put-on is done) moved from "each console when its own
//                 animation ends" to one instant they all agree on. The taker's
//                 copy is changed to type 5 so the ROM does not respawn it twice.
//   A PUPPET never picks a cap up on my screen: the contact a puppet's body
//                 leaves on a cap cylinder is cleared before the walk.
//   LOST RACE     The loser's console reverts: the cap that follows it goes, the
//                 hat morph is undone.
//   KNOCKED-OFF   My own Player::Hurt pops my cap (func_ov002_020d93ac, from
//                 St_Hurt_Init / St_HurtWater_Init) on my console only. I see the
//                 new cap actor appear over my head, claim CAP_DROP with the look
//                 and the three angles, and when the referee names it (id 32..255)
//                 every other console spawns the same cap with the ROM's own
//                 parameters (param (look << 8) | 5, speed 0xb000). It is then a
//                 cap like any other: first claim takes it, the rest delete theirs.
// ---------------------------------------------------------------------------
void release_my_yoshi();
unsigned g_trace_uid = 0;      // lab: the knocked-off copy whose flight is traced
int g_trace_n = 0;

// the place hash a CAP_TAKE carries: the death-table slot and x ^ z in whole units
// (referee.py cap_hash)
unsigned short cap_hash16(int slot, const int *pos) {
    return (unsigned short)((slot & 0xFF) | ((((pos[0] >> 12) ^ (pos[2] >> 12)) & 0xFF) << 8));
}

CapRec *cap_rec(unsigned id) {
    if (id == 0 || id >= (unsigned)kMaxCaps) return nullptr;
    return g.caps[id].used ? &g.caps[id] : nullptr;
}

CapRec *cap_rec_by_uid(unsigned uid) {
    if (!uid) return nullptr;
    for (int k = 1; k < kMaxCaps; ++k)
        if (g.caps[k].used && g.caps[k].uid == uid) return &g.caps[k];
    return nullptr;
}

// the record of a placed cap's actor: the actor I know by uid (registered, or spawned back
// by me), else the level's own death-table slot (id = slot + 1)
CapRec *cap_rec_of_actor(const char *a) {
    if (CapRec *r = cap_rec_by_uid(actor_uid(a))) return r;
    const int slot = (signed char)*(const unsigned char *)(a + 0xce);
    if (slot >= 0 && slot < kCapPlacedMax && g.caps[slot + 1].used && g.caps[slot + 1].placed)
        return &g.caps[slot + 1];
    return nullptr;
}

char *cap_actor_of(const CapRec &r) {
    char *a = star_by_uid(r.uid);
    return a && actor_class(a) == (unsigned)kClassCap ? a : nullptr;
}

unsigned local_from_stamp(unsigned t) {
    unsigned r;
    if (!ref_now(&r)) return now_ms();
    return now_ms() - (unsigned)(int)(r - t);
}

void cap_remove_actor(CapRec &r) {
    if (char *a = cap_actor_of(r)) _ZN7fBase_c18MarkForDestructionEv(a);
    r.uid = 0;
}

// The minimap walks three marker arrays (stars, caps, spike bombs) of actor pointers and reads
// each one's area byte (GetMinimapID).  An entry whose actor is gone, or whose area byte is not
// one of the level's areas, crashes it (GetMinimapID+0x33).  The ROM takes an entry out when the
// actor goes (dCapIcon_c::Unlink); the actors this layer makes and destroys on its own
// (copies of other seats' caps, a held star kept after its verdict) must leave no entry behind
// either.  Checked once a frame before the walk; SM64DS_CAP_SLOTS=1 logs every change,
// SM64DS_CAP_NOSANITIZE=1 leaves a bad entry in place (the control).
int g_marker_cleared = 0;

void marker_slots_pass() {
    static const bool log = env_on("SM64DS_CAP_SLOTS");
    static const bool keep = env_on("SM64DS_CAP_NOSANITIZE");
    static unsigned prev[29];
    void **tabs[3] = { (void **)data_0209f40c, (void **)data_0209f3e8, (void **)data_0209f3a4 };
    static const int cnt[3] = { 12, 9, 8 };
    static const char *nm[3] = { "stars", "caps", "bombs" };
    int base = 0;
    for (int t = 0; t < 3; ++t) {
        for (int i = 0; i < cnt[t]; ++i) {
            char *p = (char *)tabs[t][i];
            const unsigned pv = (unsigned)(size_t)p;
            bool bad = false;
            const char *why = "";
            if (p) {
                if (_ZN8dActor_c10FindWithIDEj(actor_uid(p)) != (void *)p) { bad = true; why = "actor gone"; }
                else if ((signed char)*u8p(p, 0xcc) > 7) { bad = true; why = "area byte out of range"; }
            }
            if (log && pv != prev[base + i])
                std::fprintf(stderr, "[cap] SLOT %s[%d] %08x -> %08x (class %u, area byte %d, type %d)%s%s (frame %u)\n", nm[t],
                             i, prev[base + i], pv, p ? actor_class(p) : 0u, p ? (int)(signed char)*u8p(p, 0xcc) : 0,
                             p ? s32r(p, 0x3f0) : 0, bad ? " BAD: " : "", why, my_frame_now());
            prev[base + i] = pv;
            if (bad && !keep) {
                tabs[t][i] = nullptr;
                prev[base + i] = 0;
                ++g_marker_cleared;
                std::fprintf(stderr, "[cap] SLOT %s[%d] held %08x (%s): taken out (%d so far, frame %u)\n", nm[t], i, pv,
                             why, g_marker_cleared, my_frame_now());
            }
        }
        base += cnt[t];
    }
}

// the caps the level placed, once, when the match starts
void caps_register() {
    struct T { int o[3]; unsigned p1; int area; unsigned uid; unsigned char look; int slot; };
    T t[48];
    int n = 0;
    each_class(kClassCap, [&](char *a) {
        if (n >= 48) return;
        const int type = s32r(a, 0x3f0);
        if (type == 5 || type == 18) return;            // a knocked-off cap is not placed
        T &x = t[n++];
        x.o[0] = s32r(a, 0x3c4); x.o[1] = s32r(a, 0x3c8); x.o[2] = s32r(a, 0x3cc);
        if (!x.o[0] && !x.o[1] && !x.o[2]) { x.o[0] = s32r(a, 0x5c); x.o[1] = s32r(a, 0x60); x.o[2] = s32r(a, 0x64); }
        x.p1 = u32r(a, 8);
        x.area = (signed char)*u8p(a, 0xcc);
        x.uid = actor_uid(a);
        x.look = (unsigned char)(s32r(a, 0x3f4) < 3 ? s32r(a, 0x3f4) : 0xFF);
        x.slot = (signed char)*u8p(a, 0xce);
    });
    if (n == 0) { if (++g.caps_try > 300) g.caps_ready = true; return; }
    g.caps_ready = true;
    g.caps_n = 0;
    for (int i = 0; i < n; ++i) {
        // a cap's id is its death-table slot + 1 (distinct for every cap of a map; five
        // caps stacked on one spot stay apart); a slot out of range takes a free id
        int id = t[i].slot >= 0 && t[i].slot < kCapPlacedMax ? t[i].slot + 1 : 0;
        if (id && g.caps[id].used) id = 0;
        for (int k = 1; !id && k <= kCapPlacedMax; ++k)
            if (!g.caps[k].used) id = k;
        if (!id) continue;
        CapRec &r = g.caps[id];
        std::memset(&r, 0, sizeof r);
        r.used = true;
        r.placed = true;
        std::memcpy(r.origin, t[i].o, sizeof r.origin);
        r.param1 = t[i].p1;
        r.area = t[i].area;
        r.hash = cap_hash16(id - 1, r.origin);
        r.look = t[i].look;
        r.uid = t[i].uid;
        ++g.caps_n;
        std::fprintf(stderr, "[cap] TABLE id=%d slot=%d pos=(%d,%d,%d) param1=0x%x look=%d area=%d hash=0x%04x\n",
                     id, t[i].slot, r.origin[0] >> 12, r.origin[1] >> 12, r.origin[2] >> 12, r.param1,
                     (int)r.look, r.area, (unsigned)r.hash);
    }
    std::fprintf(stderr, "[cap] %d placed caps registered\n", g.caps_n);
}

// the cap actor of a placed record: its uid, or the one standing at its origin
char *cap_find_placed(CapRec &r) {
    if (char *a = cap_actor_of(r)) return a;
    const int slot = (int)(&r - g.caps) - 1;
    char *hit = nullptr;
    each_class(kClassCap, [&](char *a) {
        if (hit) return;
        if (s32r(a, 0x3f0) == 5 || (signed char)*u8p(a, 0xce) != slot) return;
        hit = a;
    });
    if (hit) r.uid = actor_uid(hit);
    return hit;
}

// undo my ROM pickup: the cap that follows me goes, the hat morph stops, the look is
// my own again
void cap_revert(CapRec &r, const char *why) {
    r.mine = false;
    ++g.st.cap_lost;
    char *me = my_body();
    cap_remove_actor(r);
    if (!me) return;
    const unsigned real = *u8p(me, 0x6d9);
    const unsigned look = (unsigned)(s32r(me, 8) & 0xff);
    const unsigned flags = u16r(me, 0x73c);
    u16r(me, 0x73c) = 0;                           // the hat morph's state machine stops
    *u8p(me, 0x6dc) = (unsigned char)real;         // mPrevCharacter
    *u8p(me, 0x6dd) = (unsigned char)real;         // mHatCharacter
    if (look != real) port_player_set_character(me, real);
    release_my_yoshi();
    g.prev_look = real;
    std::fprintf(stderr, "[vsarena-ref] CAP lost (%s): look %u -> %u, cap flags 0x%04x cleared, "
                 "body state %d\n", why, look, (unsigned)(s32r(me, 8) & 0xff), flags,
                 vs_arena_state_index(me));
}

void cap_on_deny(unsigned short cid, int reason, int winner) {
    for (int k = 1; k < kMaxCaps; ++k) {
        CapRec &r = g.caps[k];
        if (r.used && r.mine && r.cid == cid) {
            std::fprintf(stderr, "[vsarena-ref] CAP_TAKE of cap %d refused: %s (winner %d)\n", k,
                         reason_name(reason), winner);
            cap_revert(r, reason_name(reason));
        }
    }
}

// The frame of the last CAP_TAKEN the referee gave ME (0 = none). The tongued-cap watch below swaps my
// character outright only after one: a cap the referee gave to someone else must not change me.
unsigned g_cap_taken_me_frame = 0;

void on_cap_taken(const RefEvent &e) {
    ++g.st.cap_taken;
    if (e.seat == g.seat) g_cap_taken_me_frame = my_frame_now() ? my_frame_now() : 1u;
    std::fprintf(stderr, "[vsarena-ref] CAP_TAKEN cap=%d by seat %d look=%d back_in=%d claim=%d stamp=%u\n",
                 e.a, e.seat, (int)(signed char)e.b, e.d, e.c, e.t);
    if (e.seat == g.seat) finish_claim(e.c, 1, 0, 0xFF);
    CapRec *r = cap_rec(e.a);
    if (g.caps_off) return;
    if (!r) return;
    r->gone = true;
    if (e.b <= 2) r->look = (unsigned char)e.b;
    if (e.seat == g.seat) {
        r->mine = false;                           // my pickup stands; the follower finishes by itself
    } else {
        if (r->mine) cap_revert(*r, "taken by another seat");
        if (cap_actor_of(*r) || (r->placed && cap_find_placed(*r))) {
            cap_remove_actor(*r);
            ++g.st.cap_removed;
        }
    }
    if (r->placed) r->back_ms = e.d ? local_from_stamp(e.t) + e.d : 0;
    else {
        if (e.seat == g.seat) r->uid = 0;           // (a knocked-off cap I took: the follower dies alone)
        std::memset(r, 0, sizeof *r);
    }
}

char *spawn_cap_copy(unsigned look, const int *pos, int area, int angx, int angy) {
    char *s = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        kClassCap, ((look & 3u) << 8) | 5u, pos, nullptr, area, -1);
    if (!s) return nullptr;
    s16r(s, 0x92) = (short)angx;
    s16r(s, 0x94) = (short)angy;
    s16r(s, 0x96) = 0;
    s32r(s, 0x98) = 0xb000;
    return s;
}

void on_cap_dropped(const RefEvent &e) {
    ++g.st.cap_dropped;
    std::fprintf(stderr, "[vsarena-ref] CAP_DROPPED cap=%d victim seat %d look=%d at (%d,%d,%d) claim=%d\n",
                 e.a, e.seat, e.b, e.x >> 12, e.y >> 12, e.z >> 12, e.c);
    if (e.seat == g.seat) finish_claim(e.c, 1, 0, 0xFF);
    if (g.caps_off) return;
    CapRec &r = g.caps[e.a];
    std::memset(&r, 0, sizeof r);
    r.used = true;
    r.placed = false;
    r.look = (unsigned char)e.b;
    r.born_ms = now_ms();
    if (e.seat == g.seat) {
        r.uid = g.drop_uid;                         // the cap my own console popped
        g.drop_uid = 0;
        return;
    }
    char *vb = body_of(e.seat);
    const int area = vb ? (signed char)*u8p(vb, 0xcc) : 0;
    const int pos[3] = { e.x, e.y, e.z };
    if (char *s = spawn_cap_copy(e.b, pos, area, (short)e.e, (short)e.d)) {
        r.uid = actor_uid(s);
        ++g.st.cap_copies;
        g_trace_uid = r.uid;
        g_trace_n = 0;
    }
    if (g.rig == 8) { g.rig_drop_id = e.a; g.rig_drop_at = now_ms() + 1600u; }
    if (g.rig == 10) {
        static const int ms = env_int("SM64DS_RIG_TONGUE_MS", 300, 0, 5000);
        g.rig_drop_id = e.a;
        g.rig_drop_at = now_ms() + (unsigned)ms;
        g.rig_tongue_until = now_ms() + 6500u;
    }
}

void on_cap_state(const RefEvent &e) {
    if (g.caps_off) return;
    CapRec *r = cap_rec(e.a);
    if (e.b == 1) {                                 // a placed cap that is taken, back in e.c ms
        if (!r) return;
        r->gone = true;
        cap_remove_actor(*r);
        if (char *a = cap_find_placed(*r)) { _ZN7fBase_c18MarkForDestructionEv(a); r->uid = 0; }
        r->back_ms = now_ms() + e.c;
        return;
    }
    if (r && r->used) return;                       // a knocked-off cap I already know
    CapRec &n = g.caps[e.a];
    std::memset(&n, 0, sizeof n);
    n.used = true;
    n.look = (unsigned char)(e.d <= 2 ? e.d : 0xFF);
    n.born_ms = now_ms();
    const int pos[3] = { e.x, e.y, e.z };
    if (char *s = spawn_cap_copy(n.look <= 2 ? n.look : 0, pos, 0, 0, 0)) {
        n.uid = actor_uid(s);
        s32r(s, 0x98) = 0;
        ++g.st.cap_copies;
    }
}


// LAB WATCH (SM64DS_CAP_WATCH=1, or REFLOG): is a cap still lying where another seat
// just picked one up?  Triggered by the other seat's teleport flag (the lab rigs put
// a body on a cap in one step); frames 15..42 later (0.5 s to 1.4 s, after the
// referee's verdict and before the cap is back) it counts the frames a cap actor
// stands within 120 units of that spot.  The old behaviour (every console keeps its
// own copy) leaves the cap there for the whole window; the shared cap leaves none.
// Also traces a knocked-off cap copy's flight.
int caps_near(const int *loc) {
    int n = 0;
    each_class(kClassCap, [&](char *a) {
        const long long dx = (long long)(s32r(a, 0x5c) - loc[0]) >> 12;
        const long long dz = (long long)(s32r(a, 0x64) - loc[2]) >> 12;
        const long long dy = (long long)(s32r(a, 0x60) - loc[1]) >> 12;
        if (dx * dx + dz * dz <= 120 * 120 && dy > -300 && dy < 300) ++n;
    });
    return n;
}

void cap_watch() {
    static const bool on = env_on("SM64DS_CAP_WATCH") || env_on("SM64DS_ARENA_REFLOG");
    if (!on || !g.started) return;
    struct W { bool active; int frames, near_frames, start_near; int loc[3]; unsigned f0; };
    static W w[kArenaSeats];
    static bool prev_tp[kArenaSeats];
    for (int k = 0; k < g.bodies && k < kArenaSeats; ++k) {
        if (k == g.seat) continue;
        ArenaSample smp[1];
        if (vs_arena_samples(k, smp, 1) <= 0) continue;
        const bool tp = (smp[0].s.flags & kArenaTeleport) != 0;
        W &x = w[k];
        if (tp && !prev_tp[k] && !x.active) {
            std::memset(&x, 0, sizeof x);
            x.active = true;
            x.loc[0] = smp[0].s.x; x.loc[1] = smp[0].s.y; x.loc[2] = smp[0].s.z;
            x.f0 = my_frame_now();
            x.start_near = caps_near(x.loc);
        }
        prev_tp[k] = tp;
        if (!x.active) continue;
        ++x.frames;
        if (x.frames >= 15 && x.frames <= 42 && caps_near(x.loc) > 0) ++x.near_frames;
        if (x.frames == 42) {
            x.active = false;
            std::fprintf(stderr, "[cap] WATCH seat=%d jumped at frame %u to (%d,%d,%d): caps within 120 units "
                         "then=%d, frames 15..42 later with a cap there=%d of 28\n", k, x.f0,
                         x.loc[0] >> 12, x.loc[1] >> 12, x.loc[2] >> 12, x.start_near, x.near_frames);
        }
    }
    if (g_trace_uid) {
        char *a = star_by_uid(g_trace_uid);
        if (!a) { std::fprintf(stderr, "[cap] copy uid %u gone after %d frames\n", g_trace_uid, g_trace_n); g_trace_uid = 0; }
        else if (g_trace_n % 4 == 0 || g_trace_n < 2) {
            std::fprintf(stderr, "[cap] copy uid %u frame +%d pos=(%d,%d,%d) vy=%d speed=%d type=%d\n", g_trace_uid,
                         g_trace_n, s32r(a, 0x5c) >> 12, s32r(a, 0x60) >> 12, s32r(a, 0x64) >> 12,
                         s32r(a, 0xa8), s32r(a, 0x98), s32r(a, 0x3f0));
        }
        if (g_trace_uid && ++g_trace_n > 40) g_trace_uid = 0;
    }
}

// once a frame, before the walk
void caps_pass() {
    if (!g.started) return;
    if (!g.caps_ready) caps_register();      // (the lab rigs read the table too)
    if (g.caps_off) return;
    char *me = my_body();
    // a puppet never picks a cap up on my screen: the contact its body left on the
    // cap's cylinder is cleared (the referee orders who wears it)
    each_class(kClassCap, [&](char *a) {
        if (!u32r(a, 0x130) || !u32r(a, 0x134)) return;
        char *o = _ZN8dActor_c10FindWithIDEj(u32r(a, 0x134));
        if (!o || actor_class(o) != (unsigned)kClassPlayer) return;
        if (o == me) {
            // LUIGI INFECTION (contract 2, DENY_TEAM): a Luigi takes no cap (it stays Luigi) and a
            // survivor never takes the Luigi cap (nobody fakes a team). The ROM's pickup starts from
            // this contact, so it is cleared before the cap's own Behavior reads it: no put-on, no
            // morph, no claim the referee would refuse.
            if (!g.luigi || (!me_luigi() && (s32r(a, 0x3f4) & 0xff) != 1)) return;
            u32r(a, 0x130) = 0;
            u32r(a, 0x134) = 0;
            if ((g.luigi_cap_blocks++ & 63) == 0)
                std::fprintf(stderr, "[vsarena-ref] cap uid %u (look %d) not taken: %s (frame %u)\n",
                             actor_uid(a), s32r(a, 0x3f4) & 0xff,
                             me_luigi() ? "a Luigi takes no cap" : "a survivor never takes the Luigi cap",
                             my_frame_now());
            return;
        }
        u32r(a, 0x130) = 0;
        u32r(a, 0x134) = 0;
        ++g.st.cap_puppet_touches;
    });
    // the placed caps that are back
    const unsigned now = now_ms();
    for (int k = 1; k <= kCapPlacedMax; ++k) {
        CapRec &r = g.caps[k];
        if (!r.used || !r.placed || !r.gone) continue;
        if (r.back_ms && (int)(now - r.back_ms) < 0) continue;
        if (!r.back_ms) continue;                    // never comes back
        if (cap_actor_of(r)) continue;               // the taker's follower is still putting it on
        const int pos[3] = { r.origin[0], r.origin[1], r.origin[2] };
        char *s = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(kClassCap, r.param1, pos,
                                                                  nullptr, r.area, -1);
        r.gone = false;
        r.back_ms = 0;
        if (s) {
            r.uid = actor_uid(s);
            ++g.st.cap_respawns;
            std::fprintf(stderr, "[vsarena-ref] cap %d is back (spawned, frame %u)\n", k, my_frame_now());
        }
    }
    // a pickup that never got a verdict is treated as granted (nothing was refused)
    for (int k = 1; k < kMaxCaps; ++k) {
        CapRec &r = g.caps[k];
        if (r.used && r.mine && now - r.claim_ms > 2000u) r.mine = false;
    }
}

// once a frame, after the walk: my own pickup, my own cap popping
void caps_detect() {
    if (g.caps_off || !g.caps_ready) return;
    char *me = my_body();
    if (!me) return;
    each_class(kClassCap, [&](char *a) {
        if (u32r(a, 0x3bc) != (unsigned)(size_t)&data_ov002_0210df54) return;   // not putting on
        if ((char *)(size_t)u32r(a, 0x3c0) != me) return;
        const bool knocked = s32r(a, 0x3f0) == 5;
        CapRec *r = knocked ? cap_rec_by_uid(actor_uid(a)) : cap_rec_of_actor(a);
        if (!r || r->mine || r->gone) return;
        if (g.luigi && (me_luigi() || (s32r(a, 0x3f4) & 0xff) == 1)) {
            // LUIGI INFECTION: a pickup the team rule forbids (a tongue can still pull one in):
            // undone here, no CAP_TAKE (the referee would answer DENY_TEAM)
            r->uid = actor_uid(a);
            r->mine = true;
            cap_revert(*r, "Luigi Infection team rule");
            return;
        }
        unsigned stamp;
        if (!stamp_for(g.last_post_ms ? g.last_post_ms : now_ms(), &stamp)) return;
        const unsigned id = (unsigned)(r - g.caps);
        PClaim *c = make_claim(CLAIM_CAP_TAKE, (unsigned char)id, r->placed ? r->hash : 0, stamp,
                              my_frame_now(), 0);
        if (!c) return;
        r->mine = true;
        r->cid = c->cid;
        r->claim_ms = now_ms();
        r->uid = actor_uid(a);
        ++g.st.cap_claims;
        s32r(a, 0x3f0) = 5;                          // (the ROM's own respawn is the referee's now)
        std::fprintf(stderr, "[vsarena-ref] CAP_TAKE claim %u for cap %u (look %d, frame %u)\n",
                     (unsigned)c->cid, id, s32r(a, 0x3f4), my_frame_now());
    });
    // my own cap popping off: a new cap of my look appears over my head while I am hurt
    const int st = vs_arena_state_index(me);
    const int look = s32r(me, 8) & 0xff;
    if (me_luigi() && (st == kStHurt || st == kStHurtWater || st == 12)) {
        // LUIGI INFECTION: a Luigi's cap never comes off (no Luigi cap on the field for a survivor
        // to wear; the referee refuses a Luigi's CAP_DROP): the cap a hazard hurt popped goes
        each_class(kClassCap, [&](char *a) {
            if (s32r(a, 0x3f0) != 5 || (s32r(a, 0x3f4) & 0xff) != 1 || cap_rec_by_uid(actor_uid(a))) return;
            const long long dx = (long long)(s32r(a, 0x5c) - s32r(me, 0x5c)) >> 12;
            const long long dy = (long long)(s32r(a, 0x60) - s32r(me, 0x60)) >> 12;
            const long long dz = (long long)(s32r(a, 0x64) - s32r(me, 0x64)) >> 12;
            if (dx * dx + dz * dz > 60 * 60 || dy < 60 || dy > 220) return;
            std::fprintf(stderr, "[vsarena-ref] my Luigi cap popped (state %d): removed, no CAP_DROP "
                         "(frame %u)\n", st, my_frame_now());
            _ZN7fBase_c18MarkForDestructionEv(a);
        });
        return;
    }
    if (look <= 2 && (st == kStHurt || st == kStHurtWater || st == 12)) {
        each_class(kClassCap, [&](char *a) {
            if (g.drop_uid) return;
            if (s32r(a, 0x3f0) != 5 || (s32r(a, 0x3f4) & 0xff) != look) return;
            const unsigned uid = actor_uid(a);
            if (cap_rec_by_uid(uid)) return;
            for (unsigned k = 0; k < 16 && k < g.ignored_n; ++k)
                if (g.ignored[k] == uid) return;
            const long long dx = (long long)(s32r(a, 0x5c) - s32r(me, 0x5c)) >> 12;
            const long long dy = (long long)(s32r(a, 0x60) - s32r(me, 0x60)) >> 12;
            const long long dz = (long long)(s32r(a, 0x64) - s32r(me, 0x64)) >> 12;
            g.ignored[g.ignored_n++ & 15] = uid;
            if (dx * dx + dz * dz > 60 * 60 || dy < 60 || dy > 220) return;
            unsigned stamp;
            if (!stamp_for(g.last_post_ms ? g.last_post_ms : now_ms(), &stamp)) return;
            const unsigned angy = (unsigned short)s16r(me, 0x94);
            const unsigned angx = (unsigned short)s16r(me, 0x92), angz = (unsigned short)s16r(me, 0x96);
            PClaim *c = make_claim(CLAIM_CAP_DROP, (unsigned char)look, (unsigned short)angy, stamp,
                                  my_frame_now(), angx | (angz << 16));
            if (!c) return;
            g.drop_uid = uid;
            g.drop_cid = c->cid;
            ++g.st.cap_drop_claims;
            std::fprintf(stderr, "[vsarena-ref] my cap popped (look %d): CAP_DROP claim %u, the cap is "
                         "uid %u (frame %u)\n", look, (unsigned)c->cid, uid, my_frame_now());
        });
    }
}


// ---------------------------------------------------------------------------
// A TONGUED CAP (FIX9C). Yoshi's tongue can pull a cap in (daObjMarioCap_c's
// OnYoshiTryEat answers 4), and the swallow then calls the cap's OnTurnIntoEgg:
// SetNoControlState(8) and, only if that took, SetNewHatCharacter. When the
// first call is refused (Swallow_Init switched interaction off: mIsNoControl is
// 1 and a kind above 3 is refused while it is set) the cap still goes into its
// put-on state, follows the player, blinks and dies after 150 frames, and the
// player never changes: Tango's "tongued a Luigi cap, blinked, never changed".
// The touch path does not have the problem because it simply asks again every
// frame until the call takes. This does the same for a cap that follows MY
// body in the put-on state and has not started its morph: from the 4th frame
// on, once the swallow is over (nothing in my mouth), ask again; when it takes,
// set the hat exactly as OnTurnIntoEgg does. If it still has not taken by frame
// 60 the character is swapped outright (the door swap) and the cap removed.
// SM64DS_CAP_TONGUE_FIX=0 leaves the ROM's behaviour alone (the control row);
// SM64DS_CAP_TONGUE_LOG=1 (or REFLOG) prints the first 45 frames of every such cap.
// ---------------------------------------------------------------------------
struct TongueCap { unsigned uid = 0; int frames = 0; int tries = 0; bool hat_set = false, swapped = false; } g_tc;
int g_tc_fixed = 0, g_tc_swapped = 0;

void cap_tongue_watch() {
    if (!g.started) return;
    static const bool fix = env_int("SM64DS_CAP_TONGUE_FIX", 1, 0, 1) != 0;
    static const bool lg = env_on("SM64DS_CAP_TONGUE_LOG") || env_on("SM64DS_ARENA_REFLOG");
    char *me = my_body();
    if (!me) return;
    {
        static int last_look = -1;
        const int lk = s32r(me, 8) & 0xff;
        if (last_look >= 0 && lk != last_look)
            std::fprintf(stderr, "[captongue] f=%u my look %d -> %d (hat %d)\n", my_frame_now(), last_look, lk, (int)*u8p(me, 0x6dd));
        last_look = lk;
    }
    bool seen = false;
    each_class(kClassCap, [&](char *a) {
        if (u32r(a, 0x3bc) != (unsigned)(size_t)&data_ov002_0210df54) return;   // not putting on
        if ((char *)(size_t)u32r(a, 0x3c0) != me) return;
        seen = true;
        const unsigned uid = actor_uid(a);
        if (uid != g_tc.uid) g_tc = TongueCap{}, g_tc.uid = uid;
        const int model = s32r(a, 0x3f4) & 0xff;
        const bool morphing = *u8p(a, 0x3fe) != 0 || *u8p(a, 0x401) != 0;
        ++g_tc.frames;
        if (lg && g_tc.frames <= 45)
            std::fprintf(stderr, "[captongue] f=%u cap uid=%u type=%d model=%d t3fe=%d t401=%d | me state=%d step=%d "
                         "noctl=%d kind=%d inmouth=%p morph8=%d look=%d hat=%d prev=%d%s\n", my_frame_now(), uid,
                         s32r(a, 0x3f0), model, (int)*u8p(a, 0x3fe), (int)*u8p(a, 0x401), vs_arena_state_index(me),
                         (int)*u8p(me, 0x6e3), (int)*u8p(me, 0x709), (int)*u8p(me, 0x70a), (void *)(size_t)u32r(me, 0x360),
                         _ZN6Player12Unk_020c9e5cEh(me, 8), s32r(me, 8) & 0xff, (int)*u8p(me, 0x6dd), (int)*u8p(me, 0x6dc),
                         morphing ? " (morph running)" : "");
        if (!fix || morphing || model > 2 || (s32r(me, 8) & 0xff) == (unsigned)model) return;
        if (g_tc.frames < 4 || u32r(me, 0x360) != 0) return;        // the swallow is not over yet
        if (_ZN6Player12Unk_020c9e5cEh(me, 8)) return;               // the morph state is up: the cap takes it from here
        // the outright swap needs the referee's say-so: a CAP_TAKEN for me within the last ~10 s
        const bool granted = g_cap_taken_me_frame && (unsigned)(my_frame_now() - g_cap_taken_me_frame) < 600u;
        if (g_tc.frames >= 60 && !g_tc.swapped && granted) {
            g_tc.swapped = true;
            ++g_tc_swapped;
            std::fprintf(stderr, "[captongue] f=%u the ROM's own put-on never started in %d frames (%d tries): "
                         "swapping my character to %d outright\n", my_frame_now(), g_tc.frames, g_tc.tries, model);
            port_player_set_character(me, (unsigned)model);
            _ZN7fBase_c18MarkForDestructionEv(a);
            return;
        }
        if (g_tc.swapped || g_tc.tries >= 40) return;
        ++g_tc.tries;
        if (*u8p(me, 0x709) == 1) _ZN6Player19func_ov002_020c9e18Ev(me);            // the swallow's "interaction off"
        const int r = _ZN6Player17SetNoControlStateEhih(me, 8, -1, 0);
        if (r == 1 && *u8p(me, 0x6dd) != (unsigned)model) {
            _ZN6Player18SetNewHatCharacterEjjb(me, (unsigned)model, 1u, 0);
            g_tc.hat_set = true;
            ++g_tc_fixed;
        }
        std::fprintf(stderr, "[captongue] f=%u try %d (cap frame %d): SetNoControlState(8) = %d, hat %s; "
                     "state=%d noctl=%d\n", my_frame_now(), g_tc.tries, g_tc.frames, r,
                     r == 1 ? "set" : "not set", vs_arena_state_index(me), (int)*u8p(me, 0x709));
    });
    if (!seen && g_tc.uid) g_tc = TongueCap{};
}

void on_star_taken(const RefEvent &e) {
    Inst *i = inst_by_id(e.a);
    ++g.st.stars_taken;
    g.progress = (unsigned char)e.e;
    data_0209f208 = (unsigned char)e.e;
    if (e.seat == g.seat) finish_claim(e.c, 1, 0, 0xFF);
    std::fprintf(stderr, "[vsarena-ref] STAR_TAKEN inst=%d winner=%d score=%d "
                 "claim=%d progress=%d stamp=%u\n", e.a, e.seat, e.b, e.c, e.e,
                 e.t);
    if (!i) return;
    i->state = 3;
    if (g.rig == 3 && e.seat == 1) g.chain_hit_at = e.t + 3000u;
    if (i->holding) {
        if (e.seat == g.seat) { i->result = 1; }
        else { i->result = 2; i->winner = e.seat; }
        return;
    }
    // everybody else: the star is not collectible any more, but it is shown over
    // the winner's head for the length of the star-get dance (V4)
    if (char *s = star_by_uid(i->uid)) {
        if (!show_star_start(*i, s, e.seat)) unlink_and_destroy_star(s);
    }
}

void on_deny(const RefEvent &e) {
    if (e.seat != g.seat) return;
    const int kind = e.a, reason = e.b, cid = e.c, winner = e.d;
    // The referee answers every RESEND of a claim with the same directed
    // verdict (the client resends every 60 ms and the round trip can be longer):
    // print and count each claim's denial once (FIELD4 F9-c).
    static unsigned short seen[8];
    static unsigned seen_n = 0;
    bool first = true;
    for (unsigned k = 0; k < 8 && k < seen_n; ++k)
        if (seen[k] == (unsigned short)cid) first = false;
    if (first) seen[seen_n++ & 7] = (unsigned short)cid;
    if (first) {
        std::fprintf(stderr, "[vsarena-ref] DENY kind=%d reason=%s(%d) claim=%d "
                     "winner=%d target=%d\n", kind, reason_name(reason), reason, cid,
                     winner, e.e);
        if (kind == CLAIM_HIT) ++g.st.hits_denied;
    }
    finish_claim((unsigned short)cid, -1, reason, winner);
    if (kind == CLAIM_CAP_TAKE) cap_on_deny((unsigned short)cid, reason, winner);
    // a refused CAP_DROP names no cap (no CAP_DROPPED will come for it): let my next pop be seen
    if (kind == CLAIM_CAP_DROP && (unsigned)cid == g.drop_cid) g.drop_uid = 0;
    if (kind == CLAIM_GRAB)
        for (auto &i : g.inst)
            if (i.used && i.holding && i.grab_cid == cid) {
                i.result = 2;
                i.winner = winner;
                i.deny_reason = reason;
            }
}

void on_accepted(const RefEvent &e) {
    if (e.seat != g.seat) return;
    finish_claim((unsigned short)e.c, 1, 0, 0xFF);
    if (e.a == CLAIM_READY) {
        g.ready_ok = true;
        session_up("READY accepted");
    }
}

void apply_hit_on_me(const RefEvent &e);
void apply_hit_effects(const RefEvent &e) {
    ++g.st.hits_landed;
    if (e.seat == g.seat) apply_hit_on_me(e);
    if (e.a == g.seat) finish_claim(e.c, 1, 0, 0xFF);
    std::fprintf(stderr, "[vsarena-ref] HIT victim=%d attacker=%d trade=%d "
                 "claim=%d flags=0x%x spilled=%d\n", e.seat, e.a, e.b, e.c,
                 (unsigned)e.d << 5, e.e);
}

void on_spill(const RefEvent &e) {
    ++g.st.spills;
    Inst *i = &g.inst[e.a ? e.a : 1];
    if (e.a == 0) return;
    std::memset(i, 0, sizeof *i);
    i->used = true;
    i->spilled = true;
    i->star_id = 0xFF;
    i->marker = e.b;
    i->state = 2;
    i->born_ms = now_ms();
    // who streams this flight: the seat that lost the star; a leaver's stars
    // are launched by the lowest seat left in the match
    int owner = e.seat;
    if (e.c == 2 || e.seat == 0xFF) {
        owner = 0xFF;
        for (int k = 0; k < kArenaSeats; ++k)
            if ((g.roster >> k & 1u) && k != e.seat) { owner = k; break; }
    }
    i->owner = (unsigned char)owner;
    i->t_broken = e.t;               // the chain rig times the pick-up from it
    const int origin[3] = { e.x, e.y, e.z };
    char *s = spawn_spill_star(e.b, origin, e.seat);
    if (s) i->uid = actor_uid(s);
    std::fprintf(stderr, "[vsarena-ref] SPILL inst=%d seat=%d marker=%d cause=%d "
                 "score=%d owner=%d spawned=%d\n", e.a, e.seat, e.b, e.c, e.d,
                 owner, s ? 1 : 0);
}

void on_match_over(const RefEvent &e) {
    if (g.over) return;
    g.over = true;
    g.over_v.reason = e.a;
    g.over_v.winners = e.b;
    g.over_v.winners_mask = e.c;
    g.over_v.roster_mask = g.roster_now;
    g.over_v.seen_mask = (int)(g.ever_roster | g.roster_now);
    std::memcpy(g.over_v.scores, g.ledger, 16);
    g.over_v.seq = e.seq;
    g.over_v.local_timeout = false;
    std::fprintf(stderr, "[vsarena] MATCH OVER ref reason=%d winners=0x%04x "
                 "count=%d scores=%d,%d,%d,%d seq=%u\n", e.a, e.c, e.b,
                 g.ledger[0], g.ledger[1], g.ledger[2], g.ledger[3], e.seq);
}

// LUIGI INFECTION: the referee made e.seat a Luigi. The team bit and the Luigi body at once, on
// every console (mine and the puppets alike); the whole mask the event carries is applied too,
// so a console that missed an event catches up on the next one. The tagger's claim ends here
// (INFECTED is its terminal verdict: no HIT, no SPILL, no STOMPED with it).
const char *infect_name(int b) {
    static const char *n[] = {"?", "PICK", "HIT", "STOMP", "TAG", "HANDOFF", "LATE_JOIN", "RETURN"};
    return b >= 1 && b <= 7 ? n[b] : "?";
}

void on_infected(const RefEvent &e) {
    ++g.infected_seen;
    g.lmask |= (unsigned)e.d | (1u << e.seat);
    g.picked = true;
    if (e.a == g.seat && e.c) finish_claim(e.c, 1, 0, 0xFF);
    std::fprintf(stderr, "[vsarena-ref] INFECTED seat=%d by=%d cause=%s(%d) claim=%d luigis=0x%04x "
                 "survivors=%d stamp=%u%s\n", e.seat, e.a == 0xFF ? -1 : (int)e.a, infect_name(e.b), e.b,
                 e.c, (unsigned)e.d, e.e, e.t, e.seat == g.seat ? " (me)" : "");
    port_luigi_arena_infect(e.seat, e.b, e.a == 0xFF ? -1 : (int)e.a);
    port_luigi_arena_team(g.lmask);
}

void on_team_state(const RefEvent &e) {
    g.lmask |= (unsigned)e.c;
    if (e.a) g.picked = true;
    std::fprintf(stderr, "[vsarena-ref] TEAM_STATE picked=%d luigis=0x%04x roster=0x%04x t0=%u\n",
                 e.a, e.c, e.d, e.t);
    port_luigi_arena_team(g.lmask);
}

void on_star_state(const RefEvent &e) {
    // snapshot record: instance e.a is in state e.b (1 waking, 2 free)
    if (e.a == 0) return;
    Inst *i = &g.inst[e.a];
    if (i->used) return;
    std::memset(i, 0, sizeof *i);
    i->used = true;
    i->state = e.b;
    i->marker = (unsigned char)e.c;
    i->star_id = (unsigned char)e.d;
    i->owner = e.seat;
    if (e.b == 1 && g.progress < sizeof g.order) g.order[g.progress] = (unsigned char)e.d;
    ensure_star_uid(*i);
}

void apply_event(const RefEvent &e, bool directed) {
    if (!ref_event_valid(e)) {
        ++g.st.events_bad;      // dropped whole; the stream's sequence still moves on
        return;
    }
    switch (e.type) {
    case EV_START:          on_start(e); break;
    case EV_STAR_WAKE:      on_wake(e); break;
    case EV_MARKER_BROKEN:  on_marker_broken(e); break;
    case EV_STAR_TAKEN:     on_star_taken(e); break;
    case EV_DENY:           on_deny(e); break;
    case EV_HIT:            apply_hit_effects(e); break;
    case EV_CLASH:
        if (e.a == g.seat) finish_claim(e.c, 1, 0, 0xFF);
        std::fprintf(stderr, "[vsarena-ref] CLASH victim=%d attacker=%d\n",
                     e.seat, e.a);
        break;
    case EV_SPILL:          on_spill(e); break;
    case EV_MATCH_OVER:     on_match_over(e); break;
    case EV_LEAVE:
        std::fprintf(stderr, "[vsarena-ref] LEAVE seat=%d why=%d spilled=%d\n",
                     e.seat, e.a, e.d);
        break;
    case EV_JOIN:
        std::fprintf(stderr, "[vsarena-ref] JOIN seat=%d mid=%d\n", e.seat, e.a);
        break;
    case EV_STAR_STATE:     on_star_state(e); break;
    case EV_ACCEPTED:       on_accepted(e); break;
    case EV_CAP_TAKEN:      on_cap_taken(e); break;
    case EV_CAP_DROPPED:    on_cap_dropped(e); break;
    case EV_CAP_STATE:      on_cap_state(e); break;
    case EV_EATEN:
    case EV_RELEASED:
        if (e.a == g.seat && e.type == EV_EATEN) finish_claim(e.c, 1, 0, 0xFF);
        break;
    case EV_GRABBED:
        if (e.a == g.seat) finish_claim(e.c, 1, 0, 0xFF);
        break;
    case EV_STOMPED:
        if (e.a == g.seat) finish_claim(e.c, 1, 0, 0xFF);
        break;
    case EV_THROWN:
        // THROW / SWING are the holder's claims (a), BREAK the victim's (seat); a forced end has no claim
        if (((e.b == 1 || e.b == 2) && e.a == g.seat) || (e.b == 3 && e.seat == g.seat))
            finish_claim(e.c, 1, 0, 0xFF);
        break;
    case EV_INFECTED:       on_infected(e); break;
    case EV_TEAM_STATE:     on_team_state(e); break;
    default: break;
    }
    ++g.st.events_applied;
    if (g.sink) g.sink(&e, g.seat, directed);
    vs_grab_on_event(&e, g.seat, directed);       // GRAB1
}

void apply_frame(const RefFrame &f) {
    ++g.st.frames_rx;
    if (!ref_frame_valid(f)) { ++g.st.frames_bad; return; }
    g.match_state = f.match_state;
    g.hdr_t0 = f.t0;
    g.hdr_match_ms = f.match_ms;
    if (!g.started || !g.roster) g.roster = f.roster;
    g.roster_now = f.roster;
    g.ever_roster |= f.roster;
    // the ledger: the newest header wins
    if (!g.ledger_live || seq_old(f.newest_seq, g.ledger_seq)) {
        std::memcpy(g.ledger, f.scores, 16);
        g.ledger_seq = f.newest_seq;
        g.ledger_live = 1;
    }
    const bool snapshot = (f.flags & SYNR_F_SNAPSHOT) != 0;
    // directed records (seq 0): DENY, ACCEPTED, snapshot records
    for (int k = f.n_events - 1; k >= 0; --k)
        if (f.ev[k].seq == 0) apply_event(f.ev[k], true);
    if (snapshot) {
        const int idx = (f.part >> 4) & 0xF, cnt = f.part & 0xF;
        if (cnt) {
            g.snap_mask |= 1u << idx;
            if (g.snap_mask == (1u << cnt) - 1u) {
                g.snap_mask = 0;
                g.applied = f.newest_seq;
                g.adopted = true;
                ++g.st.gaps;
                std::fprintf(stderr, "[vsarena-ref] snapshot applied: seq now %u\n",
                             g.applied);
            }
        }
        return;
    }
    // the stream: oldest first
    int oldest = -1;
    for (int k = f.n_events - 1; k >= 0; --k)
        if (f.ev[k].seq != 0) { oldest = k; break; }
    if (oldest < 0) return;
    if (!g.adopted) {
        // the first frame this console sees: take the stream from its oldest
        // event (anything before is another seat's JOIN)
        g.applied = seq_prev(f.ev[oldest].seq);
        g.adopted = true;
    }
    bool progress = true;
    while (progress) {
        progress = false;
        for (int k = f.n_events - 1; k >= 0; --k) {
            const RefEvent &e = f.ev[k];
            if (e.seq == 0) continue;
            if (e.seq == seq_next(g.applied)) {
                apply_event(e, false);
                g.applied = e.seq;
                progress = true;
            } else if (seq_old(g.applied, e.seq)) {
                if (e.seq != g.applied) ++g.st.events_dup;
            }
        }
    }
    if (!seq_old(g.applied, f.newest_seq)) {
        ++g.st.gaps;     // events between were missed: the referee resends
    }
    g.st.applied_seq = g.applied;
}

void process_synr() {
    unsigned char buf[kSynrBytes];
    int seat;
    unsigned t;
    int n;
    unsigned short before = g.applied;
    while ((n = comms_hub_recv_t(kHubArenaRef, buf, (int)sizeof buf, &seat,
                                 &t)) > 0) {
        if (n != kSynrBytes) { ++g.st.frames_bad; continue; }
        RefFrame f;
        std::memcpy(&f, buf, sizeof f);
        apply_frame(f);       // checks the header and every field itself
    }
    if (g.applied != before || (g.adopted && !g.ready_sent)) {
        vs_arena_set_ack(g.applied);
        vs_arena_send_now();
    }
}

// ---------------------------------------------------------------------------
// READY
// ---------------------------------------------------------------------------
void read_hud_clock() {
    char *hud = (char *)port_hud_self;   // an fBase actor: not in the dActor list
    if (!hud) return;
    unsigned short &units = u16r(hud, 0x60);
    if (!g.hud_set) {
        g.hud_set = true;
        const int ov = env_int("SM64DS_ARENA_TIME_S", 0, 3, 3600);
        if (ov) {
            units = (unsigned short)((ov + 2) / 3);
            std::fprintf(stderr, "[vsarena] SM64DS_ARENA_TIME_S=%d: the match "
                         "clock starts at %u units (3 s each)\n", ov,
                         (unsigned)units);
        }
    }
    g.hud_units = units;
}

void maybe_send_ready() {
    // LUIGI INFECTION: a console that comes back (or in) after START learns the match from the
    // snapshot, so g.started is already true; the referee only puts a seat back in the match on
    // its READY (contract 2: a returner keeps its team, a newcomer is a Luigi), so READY still
    // goes while my seat is not in the roster. Classic keeps the old rule.
    const bool rejoining = g.luigi && g.started && !(g.roster_now >> g.seat & 1u);
    if (g.ready_ok || (g.started && !rejoining) || g.over) return;
    if (g.vs_frames < 8) return;
    char *me = my_body();
    if (!me) return;
    read_hud_clock();
    if (!g.hud_units && g.vs_frames < 300) return;   // wait for the match clock
    const unsigned now = now_ms();
    if (!g.ready_sent) {
        const int markers = count_markers();
        if (markers <= 0) return;
        const int map = env_int("SM64DS_VS_MAP", 0, 0, 15);
        const int cap = env_int("SM64DS_VS_PLAYERS", g.bodies, 1, 16);
        // LUIGI INFECTION: rules byte 1 = MODE_LUIGI (2); stars decide nothing there, so the
        // first-to-N byte is 0 (the referee ignores it in mode 2 anyway)
        const unsigned mode = g.luigi ? (unsigned)MODE_LUIGI : (unsigned)MODE_CLASSIC;
        const int first_to = g.luigi ? 0 : env_int("SM64DS_VS_STAR_TARGET", 0, 0, 99);
        const int time_s = g.hud_units ? g.hud_units * 3 : 0;
        const unsigned digest = (unsigned)std::strtoul(
            std::getenv("SM64DS_SESSION_DIGEST") ? std::getenv("SM64DS_SESSION_DIGEST") : "0",
            nullptr, 10);
        const unsigned rules = (unsigned)first_to | (mode << 8) | ((unsigned)cap << 16) |
                               ((unsigned)(markers > 8 ? 8 : markers) << 24);
        PClaim *c = make_claim(CLAIM_READY, (unsigned char)map,
                              (unsigned short)(time_s > 3600 ? 3600 : time_s),
                              rules, digest, 0);
        if (!c) return;
        g.ready_sent = true;
        g.ready_cid = c->cid;
        g.ready_last = now;
        std::fprintf(stderr, "[vsarena] READY sent: map=%d time=%ds first_to=%d "
                     "capacity=%d markers=%d digest=%u%s\n", map, time_s, first_to,
                     cap, markers, digest, g.luigi ? " mode=2 (Luigi Infection)" : "");
        return;
    }
    PClaim *c = claim_by_id(g.ready_cid);
    if (c && c->result < 0 && now - g.ready_last >= 2000) {
        // denied (mismatch, or the referee had not met us yet): ask again
        g.ready_last = now;
        c->result = 0;
        c->first_ms = now;
        send_claim(*c);
    }
}

// ---------------------------------------------------------------------------
// THE STARS, once a frame
// ---------------------------------------------------------------------------
unsigned my_frame_now() { return vs_arena_stats().frame; }

void neutralise_foreign_star_touches() {
    char *me = my_body();
    each_class(kClassStar, [&](char *a) {
        const unsigned flags = u32r(a, 0x130);
        if (!(flags & (kStarTouchMask | 0x8000))) return;
        char *o = _ZN8dActor_c10FindWithIDEj(u32r(a, 0x134));
        if (!o || actor_class(o) != (unsigned)kClassPlayer) return;
        if (o == me) {
            // MY touch: let the ROM collect only if the referee would take it
            // (F9-b: a hurt body that starts a collect gets the Yoshi star pose
            // and then the star flung away when the claim is refused)
            static const bool off = env_on("SM64DS_ARENA_NOTOUCHGATE");
            if (off || my_body_may_act()) {
                // VSSTAR2: the touch only CLAIMS the star. The ROM's star-get (the
                // Yoshi pose, the dance, the fanfare) must not start before the
                // referee says the star is mine, or a tie's loser would pose and
                // then be pulled out of it. The touch is kept (flags + id) and
                // handed back to the star by hold_step() on my verdict.
                static const bool hold_off = env_on("SM64DS_ARENA_NOHOLDTOUCH");   // control row: the old early star-get
                Inst *i = hold_off ? nullptr : inst_by_uid(actor_uid(a));
                if (!i) return;
                if (i->holding && !i->pending) return;     // the ROM's own collect is running (after my verdict)
                if (!i->holding && i->state == 2 && !i->claimed && now_ms() >= i->retry_ms) {
                    unsigned stamp;
                    if (stamp_for(g.last_post_ms ? g.last_post_ms : now_ms(), &stamp)) {
                        PClaim *c = make_claim(CLAIM_GRAB, (unsigned char)(i - g.inst), 0, stamp,
                                              my_frame_now(), 0);
                        if (c) {
                            c->inst = (unsigned char)(i - g.inst);
                            i->holding = i->claimed = i->pending = true;
                            i->result = 0;
                            i->grab_cid = c->cid;
                            i->hold_ms = now_ms();
                            i->pend_flags = flags & (kStarTouchMask | 0x8000);
                            i->pend_id = u32r(a, 0x134);
                            std::fprintf(stderr, "[vsarena-ref] TOUCH inst=%d seat=%d (claim only, star-get "
                                         "held for the verdict) frame=%u stamp=%u\n",
                                         (int)(i - g.inst), g.seat, my_frame_now(), stamp);
                        }
                    }
                }
                // (no claim made, or one is pending: the ROM must not collect this frame either)
            } else {
            ++g.st.touches_gated;
            if (g.log && (g.st.touches_gated % 30) == 1)
                std::fprintf(stderr, "[vsarena-ref] star touch held back: my body is in "
                             "state %d (no_control %d, hurt %d); the referee would refuse "
                             "the grab (%llu so far)\n", vs_arena_state_index(me),
                             (int)*u8p(me, 0x709), (int)*u8p(me, 0x708),
                             g.st.touches_gated);
            }
        }
        u32r(a, 0x130) &= ~(unsigned)(kStarTouchMask | 0x8000);
        u32r(a, 0x134) = 0;
    });
}

// VSSTAR2: the verdict on a held touch. WON: the touch goes back to the star and the
// ROM's own star-get runs (the winner's pose / dance / fanfare / count, one round trip
// late). LOST or denied: nothing was ever started, so there is nothing to undo; a star
// the referee gave to another seat is put over that seat's head like on any other console.
void hold_step() {
    for (int k = 1; k < kMaxInst; ++k) {
        Inst &i = g.inst[k];
        if (i.used && i.holding && !i.pending && i.result == 1) {
            // VSHAND1: my verdict is in and the touch was handed back, but a one-shot write is
            // lost whenever the star is not reading touches that frame (its state, the +0x100
            // post-bounce counter, the +0x4a2 bit) or the collision pass rewrites +0x130. So the
            // touch is re-armed every frame until the ROM takes the star, and from the 4th frame
            // on the ROM's own touch reader (func_ov002_020e930c) is called here, which is the same
            // call PowerStar states 4 and 8 make, so the star-get starts whatever state it is in.
            char *c = star_by_uid(i.uid);
            const int st = c ? s32r(c, 0x440) : -1;
            const unsigned el = now_ms() - i.hold_ms;
            if (c && st != 5 && st != 6 && st != 7 && st != 10 && el < (unsigned)kHoldMs) {
                u32r(c, 0x130) |= i.pend_flags;
                u32r(c, 0x134) = i.pend_id;
                if (el >= 60) {
                    char *b = my_body();
                    if (i.direct_n == 0)
                        std::fprintf(stderr, "[vsarena-ref] hand-back of inst %d not taken after %u ms: star st=%d 43c=%d +100=%d "
                                     "4a2=%04x body709=%d evt1e=%d; calling the ROM touch reader\n", k, el, st,
                                     (int)s32r(c, 0x43c), (int)*(unsigned short *)(c + 0x100), (unsigned)*(unsigned short *)(c + 0x4a2),
                                     b ? (int)*u8p(b, 0x709) : -1, _ZN5Event6GetBitEj(0x1e));
                    ++i.direct_n;
                    _ZN8daStar_c19func_ov002_020e930cEv(c);
                }
            } else if (st != 5 && st != 6 && st != 7 && st != 10) {
                i.holding = false;              // my verdict, handed back, yet the ROM never took the star
                vs_arena_set_star_lock(false);
                char *b = my_body();
                if (c) {
                    std::fprintf(stderr, "[vsarena-ref] held touch of inst %d: end state st=%d 43c=%d +100=%d 4a2=%04x body709=%d evt1e=%d direct=%u\n",
                                 k, st, (int)s32r(c, 0x43c), (int)*(unsigned short *)(c + 0x100), (unsigned)*(unsigned short *)(c + 0x4a2),
                                 b ? (int)*u8p(b, 0x709) : -1, _ZN5Event6GetBitEj(0x1e), i.direct_n);
                    unlink_and_destroy_star(c);
                }
                std::fprintf(stderr, "[vsarena-ref] held touch of inst %d: the ROM never started the star-get; star removed\n", k);
            } else if (i.direct_n) {
                std::fprintf(stderr, "[vsarena-ref] hand-back of inst %d taken: st=%d after %u ms (%u direct calls)\n", k, st, el, i.direct_n);
                i.direct_n = 0;
            }
        }
        if (!i.used || !i.pending) continue;
        char *s = star_by_uid(i.uid);
        if (i.result == 0) {
            if (now_ms() - i.hold_ms < (unsigned)kHoldMs) continue;
            i.result = 3;                           // no verdict in time: treated as denied
            std::fprintf(stderr, "[vsarena-ref] held touch of inst %d: no verdict in %d ms; dropped\n",
                         k, kHoldMs);
        }
        i.pending = false;
        if (i.result == 1 && s) {
            u32r(s, 0x130) |= i.pend_flags;
            u32r(s, 0x134) = i.pend_id;
            vs_arena_set_star_lock(true);
            std::fprintf(stderr, "[vsarena-ref] held touch of inst %d is mine after %u ms: ROM star-get starts\n",
                         k, now_ms() - i.hold_ms);
            i.hold_ms = now_ms();                   // (the ROM has kHoldMs to take it from here)
            i.direct_n = 0;
            continue;                               // holding stays: star5_arena sees result 1
        }
        i.holding = false;
        i.claimed = false;
        i.retry_ms = now_ms() + 250;
        std::fprintf(stderr, "[vsarena-ref] held touch of inst %d lost (winner %d, reason %s): no star-get ever "
                     "started here\n", k, i.winner, reason_name(i.deny_reason));
        if (i.state == 3 && s && i.winner >= 0 && !show_star_start(i, s, i.winner))
            unlink_and_destroy_star(s);
    }
}

void marker_pass() {
    char *me = my_body();
    each_class(kClassMarker, [&](char *m) {
        const unsigned char state = *u8p(m, 0x1d8);
        if (state == 0 || state == 2) return;
        const unsigned flags = u32r(m, 0xf4);
        const unsigned other = u32r(m, 0xf8);
        if (!other || !(flags & kMarkerBreakMask)) return;
        char *o = _ZN8dActor_c10FindWithIDEj(other);
        // never let the ROM break the glass itself: the referee orders it
        u32r(m, 0xf4) = 0;
        u32r(m, 0xf8) = 0;
        if (!o || o != me) return;
        static const bool gate_off = env_on("SM64DS_ARENA_NOTOUCHGATE");
        if (!gate_off && !my_body_may_act()) {
            ++g.st.touches_gated;       // the referee refuses a hurt body's break (STATE)
            return;
        }
        if (!(*u8p(m, 0x1db) & 2)) return;          // not awake
        if (*u8p(m, 0x1db) & 1) return;             // already broken
        const int id = *u8p(m, 0x1d9);
        for (int k = 1; k < kMaxInst; ++k) {
            Inst &i = g.inst[k];
            if (!i.used || i.state != 1 || i.star_id != id) continue;
            // one claim per marker until a verdict (the resend carries it)
            for (auto &c : g.claims)
                if (c.used && c.kind == CLAIM_MARKER_BREAK && c.inst == k &&
                    c.result == 0) return;
            unsigned stamp;
            if (!stamp_for(g.last_post_ms ? g.last_post_ms : now_ms(), &stamp))
                return;
            const unsigned short yaw = (unsigned short)GetAngleToCamera(g.seat);
            PClaim *c = make_claim(CLAIM_MARKER_BREAK, (unsigned char)k, yaw,
                                  stamp, my_frame_now(), 0);
            if (c) c->inst = (unsigned char)k;
            return;
        }
    });
}

void puppet_stars() {
    for (int k = 1; k < kMaxInst; ++k) {
        Inst &i = g.inst[k];
        if (!i.used || i.state != 2) continue;
        if (i.owner == g.seat || i.holding || i.fly > 0) continue;
        if (!i.have_pos) continue;
        char *s = star_by_uid(i.uid);
        if (!s) continue;
        const int st = s32r(s, 0x440);
        if (st == 5 || st == 6 || st == 7 || st == 10) continue;   // collected here
        s32r(s, 0x5c) = i.pos[0];
        s32r(s, 0x60) = i.pos[1];
        s32r(s, 0x64) = i.pos[2];
        s32r(s, 0x98) = 0;
        s32r(s, 0xa8) = 0;
    }
}

// the star state-5 body, wrapped (dispatch entry 5)
void star5_orig_call(char *c) {
    ((void (__fastcall *)(void *, void *))(size_t)g.star5_orig)(c, nullptr);
}

void release_my_yoshi() {
    char *me = my_body();
    if (!me) return;
    if (_ZN6Player7IsStateERNS_5StateE(me, data_ov002_0211022c))
        _ZN6Player11ChangeStateERNS_5StateE(me, data_ov002_0211013c);
}

void star5_arena(char *c) {
    Inst *i = inst_by_uid(actor_uid(c));
    char *me = my_body();
    if (!i || !i->holding || (char *)(size_t)u32r(c, 0x438) != me) {
        star5_orig_call(c);
        return;
    }
    if (i->result == 0 && now_ms() - i->hold_ms >= (unsigned)kHoldMs) {
        i->result = 3;                          // no verdict in time: denied
        i->winner = -1;                         // nobody won it: the star is tossed back (REVSTARREF1 F2)
        std::fprintf(stderr, "[vsarena-ref] grab of inst %d: no verdict in %d ms; "
                     "treated as denied\n", (int)(i - g.inst), kHoldMs);
    }
    if (i->result == 0) {
        // THE HOLD: the ROM's own waiting arm (state 5, the Yoshi still in the
        // star pose): the star rides over my head and nothing is decided
        int *src = (int *)(me + 0x5c);
        s32r(c, 0x5c) = src[0];
        s32r(c, 0x60) = src[1] + 0xc8000;
        s32r(c, 0x64) = src[2];
        s32r(c, 0x4b8) = 0;
        s32r(c, 0x4b4) = 0;
        return;
    }
    if (i->result == 1) {
        i->holding = false;
        vs_arena_set_star_lock(false);
        star5_orig_call(c);                     // the ROM's state 5 -> 6
        return;
    }
    // OVERRULED (or timed out). The Yoshi goes back to its pad, the star flies
    // to the winner's head and goes out; with no winner named it is tossed back
    // onto the field the way the ROM's own failure arm does.
    if (i->fly == 0) {
        release_my_yoshi();
        vs_arena_set_star_lock(false);
        char *wb = i->winner >= 0 && i->winner < kArenaSeats ? body_of(i->winner) : nullptr;
        if (wb && i->winner != g.seat) {
            i->fly = kFlyoffFrames;
            for (int k = 0; k < 3; ++k) {
                i->fly_from[k] = s32r(c, 0x5c + 4 * k);
                i->fly_to[k] = s32r(wb, 0x5c + 4 * k);
            }
            i->fly_to[1] += 0xc8000;
            s32r(c, 0x128) |= 1;                // its cylinder off
            std::fprintf(stderr, "[vsarena-ref] OVERRULE: star %d goes to seat %d\n",
                         (int)(i - g.inst), i->winner);
        } else {
            i->holding = false;
            i->claimed = false;
            s32r(c, 0x440) = 8;
            s32r(c, 0xa8) = 0x20000;
            s32r(c, 0x98) = 0xc000;
            _ZN8daStar_c19func_ov002_020e9448Ev((unsigned char *)c);
            s32r(c, 0x128) &= ~1;
            // (the reason, and how far my body and the star were apart on my own
            // screen: the referee's own line holds its copy of the same numbers)
            const int *sp = (const int *)(c + 0x5c), *bp = (const int *)(me + 0x5c);
            const double dx = (double)(((long long)sp[0] - bp[0]) >> 12),
                         dy = (double)(((long long)sp[1] - bp[1]) >> 12),
                         dz = (double)(((long long)sp[2] - bp[2]) >> 12);
            std::fprintf(stderr, "[vsarena-ref] DENIED grab of star %d (reason %s): "
                         "tossed back; on my screen the star was %d units from my body "
                         "(my state %d)\n", (int)(i - g.inst),
                         i->result == 3 ? "TIMEOUT" : reason_name(i->deny_reason),
                         (int)std::sqrt(dx * dx + dy * dy + dz * dz),
                         vs_arena_state_index(me));
            return;
        }
    }
    if (i->fly > 0) {
        --i->fly;
        const int total = kFlyoffFrames;
        const int done = total - i->fly;
        for (int k = 0; k < 3; ++k) {
            const long long d = (long long)i->fly_to[k] - i->fly_from[k];
            s32r(c, 0x5c + 4 * k) = i->fly_from[k] + (int)(d * done / total);
        }
        if (i->fly == 0) {
            i->holding = false;
            unlink_and_destroy_star(c);
        }
    }
}

void __fastcall vs_ref_star5_face(void *self, void *) {
    star5_arena((char *)self);
}

// ---------------------------------------------------------------------------
// MY BODY'S EDGES
// ---------------------------------------------------------------------------
void grab_detect() {
    char *me = my_body();
    if (!me) return;
    for (int k = 1; k < kMaxInst; ++k) {
        Inst &i = g.inst[k];
        if (!i.used || i.state != 2 || i.holding || i.claimed || !i.uid) continue;
        char *s = star_by_uid(i.uid);
        if (!s) continue;
        if (s32r(s, 0x440) != 5) continue;
        if ((char *)(size_t)u32r(s, 0x438) != me) continue;
        // the ROM's collect ran on my screen: that is the touch
        const unsigned touch_ms = g.last_post_ms ? g.last_post_ms : now_ms();
        unsigned stamp;
        if (!stamp_for(touch_ms, &stamp)) {
            // no referee clock: nothing can be claimed; give the star back
            std::fprintf(stderr, "[vsarena-ref] grab without a referee clock\n");
            continue;
        }
        PClaim *c = make_claim(CLAIM_GRAB, (unsigned char)k, 0, stamp,
                              my_frame_now(), 0);
        if (!c) continue;
        c->inst = (unsigned char)k;
        i.holding = true;
        i.claimed = true;
        i.result = 0;
        i.grab_cid = c->cid;
        i.hold_ms = now_ms();
        vs_arena_set_star_lock(true);
        std::fprintf(stderr, "[vsarena-ref] TOUCH inst=%d seat=%d touch_ms=%u "
                     "frame=%u stamp=%u\n", k, g.seat, touch_ms, my_frame_now(),
                     stamp);
    }
}

void syno_send() {
    char *me = my_body();
    (void)me;
    // one object frame a frame at most (the relay's per-kind cap is 35 / s)
    for (int n = 0; n < kMaxInst; ++n) {
        const int k = 1 + (g.syno_rr + n) % (kMaxInst - 1);
        Inst &i = g.inst[k];
        if (!i.used || i.state != 2 || i.owner != g.seat || i.holding) continue;
        char *s = star_by_uid(i.uid);
        if (!s) continue;
        const int st = s32r(s, 0x440);
        if (st == 5 || st == 6 || st == 7 || st == 10) continue;
        const int pos[3] = { s32r(s, 0x5c), s32r(s, 0x60), s32r(s, 0x64) };
        const unsigned fr = my_frame_now();
        const bool moved = !i.have_pos || pos[0] != i.last_pos[0] ||
                           pos[1] != i.last_pos[1] || pos[2] != i.last_pos[2];
        if (moved) i.still = 0; else ++i.still;
        const bool due = moved || i.still <= 3 || (fr - i.last_tx) >= 30;
        if (!due) continue;
        ObjFrame o;
        std::memset(&o, 0, sizeof o);
        o.tag = kTagO;
        o.version = 1;
        o.seat = 0xFF;
        o.obj_class = 1;
        o.obj_id = (unsigned char)k;
        o.frame = fr;
        for (int a = 0; a < 3; ++a) {
            o.pos[a] = pos[a];
            const int d = i.have_pos ? (pos[a] - i.last_pos[a]) >> 6 : 0;
            o.vel[a] = (short)(d > 32767 ? 32767 : d < -32767 ? -32767 : d);
        }
        o.state = moved ? 1 : 2;
        o.flags = 0;
        if (comms_hub_send(&o, (int)sizeof o)) ++g.st.syno_tx;
        for (int a = 0; a < 3; ++a) i.last_pos[a] = pos[a];
        i.have_pos = true;
        i.last_tx = fr;
        g.syno_rr = (unsigned char)(k % (kMaxInst - 1));
        return;
    }
}

void syno_recv() {
    unsigned char buf[64];
    int seat;
    unsigned t;
    int n;
    while ((n = comms_hub_recv_t(kHubArenaObject, buf, (int)sizeof buf, &seat,
                                 &t)) > 0) {
        if (n != kSynoBytes) { ++g.st.syno_bad; continue; }
        ObjFrame o;
        std::memcpy(&o, buf, sizeof o);
        if (!syno_valid(o, seat)) { ++g.st.syno_bad; continue; }
        Inst *i = inst_by_id(o.obj_id);
        if (!i || i->state != 2) continue;
        if (i->owner != 0xFF && i->owner != (unsigned char)seat) continue;
        if (i->owner == g.seat) continue;
        if (i->have_pos && o.frame <= i->rx_frame && i->rx_frame - o.frame < 300)
            continue;
        i->pos[0] = o.pos[0]; i->pos[1] = o.pos[1]; i->pos[2] = o.pos[2];
        i->have_pos = true;
        i->rx_ms = t;
        i->rx_frame = o.frame;
        ++g.st.syno_rx;
    }
}

// ---------------------------------------------------------------------------
// HITS
// ---------------------------------------------------------------------------
unsigned drawn_frame(int seat) {
    ArenaSeatView v;
    if (!vs_arena_seat_view(seat, &v) || !v.has_state) return 0;
    long long t = (long long)now_ms() - v.clock_offset_ms - 50;
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

// WHEN THIS RUNS MATTERS (lane HITSPILL1, G2). The pair pass (CylinderClsn::Process,
// in the render half of the frame) writes an attack's flags and the attacker's
// uid onto the TARGET's body cylinder (+0x2f4 / +0x2f8). A puppet's body cylinder is
// Clear()ed and re-registered by the puppet hold at the top of every frame
// (vs_arena_before_walk, which runs BEFORE this file's pre-walk half), and
// dCc_c::Clear zeroes those two words. So read in the pre-walk half the contact of
// the frame before is already gone and a real punch never made a claim (the lab
// rigs' injected words were wiped the same way). The post-walk half runs after the
// pair pass and before the next frame's hold, so that is where the contact is read;
// the pre-walk call is kept for bodies the hold does not own.
// LUIGI INFECTION: my body's contact with the puppet of seat k (the pair pass wrote my uid on
// its body cylinder, with or without attack flags). Classic's guard (port/unmatched/
// LuigiInfection_Hit.cpp) runs on ANY player contact, before the attack and stomp branches:
//   a Luigi touching a survivor tags it: an attack goes to the referee as HIT (as today), a
//     plain body touch as TAG (contract 2, CLAIM_TAG 19); neither waits on the survivor's
//     blink, power or state (Tango 11-09, "tag anytime": the referee infects a downed,
//     respawning or squished survivor too, so nothing is held back here);
//   anything touching a Luigi does nothing (immune), and Luigi on Luigi does nothing.
// Before T0 the referee takes no claim, so none is made. Returns true when it handled the pair.
bool luigi_contact(int k, char *b, unsigned flags, unsigned fr, const char *where) {
    char *me = my_body();
    const bool attack = (flags & ~0x1000u) != 0;
    auto clear = [&]() { if (flags) { u32r(b, 0x2f4) = 0; u32r(b, 0x2f8) = 0; } };
    if (luigi_before_t0()) { clear(); return true; }
    if (!me_luigi()) {
        if (!is_luigi(k)) return false;                  // survivor on survivor: the usual rules
        clear();
        if (attack) {
            ++g.luigi_gated;
            if (g.log)
                std::fprintf(stderr, "[vsarena-ref] contact on seat %d not claimed: it is a Luigi "
                             "(immune), read in %s\n", k, where);
        }
        return true;
    }
    clear();
    if (is_luigi(k)) return true;                        // Luigi on Luigi: nothing
    unsigned stamp;
    if (!stamp_for(g.last_post_ms ? g.last_post_ms : now_ms(), &stamp)) return true;
    if (attack) {
        if (fr - g.last_hit_claim_frame[k] < (unsigned)kHitRetryFrames && g.last_hit_claim_frame[k])
            return true;
        PClaim *c = make_claim(CLAIM_HIT, (unsigned char)k, (unsigned short)((flags >> 5) & 0xFFFF),
                               stamp, fr, drawn_frame(k));
        if (c) {
            g.last_hit_claim_frame[k] = fr ? fr : 1;
            ++g.st.hits_claimed;
            std::fprintf(stderr, "[vsarena-ref] LUIGI hit on seat %d: HIT claim %u (flags 0x%x, %s)\n",
                         k, (unsigned)c->cid, flags, where);
        }
        return true;
    }
    if (fr - g.last_tag_claim_frame[k] < (unsigned)kHitRetryFrames && g.last_tag_claim_frame[k])
        return true;
    PClaim *c = make_claim(CLAIM_TAG, (unsigned char)k, 0, stamp, fr, drawn_frame(k));
    if (c) {
        g.last_tag_claim_frame[k] = fr ? fr : 1;
        ++g.tags_claimed;
        std::fprintf(stderr, "[vsarena-ref] LUIGI touch on seat %d: TAG claim %u (%s, frame %u)\n",
                     k, (unsigned)c->cid, where, fr);
    }
    return true;
}

void hit_detect(const char *where) {
    char *me = my_body();
    if (!me) return;
    const unsigned fr = my_frame_now();
    for (int k = 0; k < g.bodies && k < kArenaSeats; ++k) {
        if (k == g.seat) continue;
        char *b = body_of(k);
        if (!b) continue;
        const unsigned flags = u32r(b, 0x2f4) & kAttackMask;
        const unsigned other = u32r(b, 0x2f8);
        if (g.luigi && other && _ZN8dActor_c10FindWithIDEj(other) == me &&
            luigi_contact(k, b, flags, fr, where))
            continue;
        if (!other || !flags) continue;
        // never let the ROM resolve a hit on a puppet by itself
        u32r(b, 0x2f4) = 0;
        u32r(b, 0x2f8) = 0;
        char *o = _ZN8dActor_c10FindWithIDEj(other);
        if (o != me) continue;
        if (g.log)
            std::fprintf(stderr, "[vsarena-ref] contact: my attack touched the body of seat %d "
                         "(flags 0x%x, read in %s, frame %u)\n", k, flags, where, fr);
        if (vs_grab_contact(k, flags)) continue;     // GRAB1: a Wario's catch, not a hit
        if ((flags & ~0x1000u) == 0) continue;
        // (a lab rig's tongue eats the copy by itself; the rig still wants its hit claim)
        if (!g.rig && vs_arena_body_held(k)) continue;
        if (fr - g.last_hit_claim_frame[k] < (unsigned)kHitRetryFrames &&
            g.last_hit_claim_frame[k]) continue;
        // F9-c: the victim is inside its hit immunity (or hurt, or in a mouth) on
        // the frame I drew: the referee would refuse (INVULN), so no claim and no
        // false spark. A lab knob keeps the old behaviour for the control row.
        {
            static const bool gate_off = env_on("SM64DS_ARENA_NOIMMUNEGATE");
            int why_st = 0, why_inv = 0;
            if (!gate_off && puppet_hit_proof(k, drawn_frame(k), &why_st, &why_inv)) {
                ++g.st.hits_gated;
                if (g.log)
                    std::fprintf(stderr, "[vsarena-ref] contact on seat %d not claimed: it cannot be "
                                 "hit now (state %d, invuln %d)\n", k, why_st, why_inv);
                continue;
            }
        }
        unsigned stamp;
        if (!stamp_for(g.last_post_ms ? g.last_post_ms : now_ms(), &stamp)) continue;
        PClaim *c = make_claim(CLAIM_HIT, (unsigned char)k,
                              (unsigned short)((flags >> 5) & 0xFFFF), stamp,
                              fr, drawn_frame(k));
        if (c) {
            g.last_hit_claim_frame[k] = fr ? fr : 1;
            ++g.st.hits_claimed;
        }
    }
}

void apply_hit_on_me(const RefEvent &e) {
    char *me = my_body();
    if (!me) return;
    const unsigned flags = (unsigned)e.d << 5;
    const unsigned flag = flags & 0x380;
    if (flag) { *u8p(me, 0x6de) = 1; *u8p(me, 0x6df) = 0; }
    int v[3] = { e.x, e.y, e.z };
    char *ab = body_of(e.a);
    const unsigned param = ab ? (unsigned)s32r(ab, 8) : 3u;
    // LAB (SM64DS_ARENA_SWIMHIT=1): the victim is in water, so the hit goes through
    // St_HurtWater (whose Init also calls the ROM's star drop); mIsUnderwater is
    // put back 90 frames later (hit_apply_tick)
    static const bool swim = env_on("SM64DS_ARENA_SWIMHIT");
    if (swim) { *u8p(me, 0x706) = 1; g.swim_clear_at = my_frame_now() + 90u; }
    // the ROM's own damage, from the attacker's position
    const int hr = _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
        me, v, 1u, 0, 0u, (param + 1) & 0xffu, 1u);
    g.hit_applied_frame = my_frame_now();
    if (g.log)
        std::fprintf(stderr, "[vsarena-ref] hit applied on me: Hurt returned %d, my state idx now %d\n",
                     hr, vs_arena_state_index(me));
    if (!hr) {
        std::fprintf(stderr, "[vsarena-ref] HIT verdict: Player::Hurt refused on my "
                     "body (already hurt?)\n");
        return;
    }
    // ... and the knockback, as func_ov002_020d8360 does after Hurt
    int tr[3] = { 0, 0x50000, 0x64000 };
    int out[3], cp[3];
    Vec3_RotateYAndTranslate(out, (int *)(me + 0x5c), s16r(me, 0x8e), tr);
    cp[0] = out[0]; cp[1] = out[1]; cp[2] = out[2];
    func_ov002_020d8d10(me, cp);
    func_020105cc(me, flags);
    unsigned idx = (unsigned short)(s32r(me, 8) + (int)(param << 2));
    if (*u8p(me, 0x6f9)) idx = 0x11;
    const int t98 = (int)data_ov002_020ff29c[idx] << 3;
    s32r(me, 0x98) = t98;
    *u8p(me, 0x6de) = 1;
    *u8p(me, 0x6df) = 0;
    if (flag) {
        s32r(me, 0xa8) = t98 + 0x20000;
        s32r(me, 0x98) = s32r(me, 0x98) >> 1;
    } else {
        s32r(me, 0xa8) = 0x18000;
    }
}

// St_Hurt's Main, wrapped: the ROM drops a star from inside it (func_ov002_020d94cc
// reads my carried count and refuses at zero); in an arena the referee already
// spilled it, so the count reads zero for that one call.
// func_ov002_020d94cc has three callers: St_Hurt_Main, St_HurtWater_Init and
// St_DeadHit_Main. Each state's entry is wrapped the same way, so a hit that lands
// while swimming (or a fatal one) cannot give the victim a star of its own on top
// of the referee's spill (REVSTARREF1 F1).
int muted_state_call(unsigned orig, void *self, void *edx) {
    char *b = (char *)self;
    if (!g.active || b != my_body())
        return ((int (__fastcall *)(void *, void *))(size_t)orig)(self, edx);
    const int idx = *u8p(b, 0x6d8);
    // (SM64DS_ARENA_NOMUTE=1 is the lab's NEGATIVE CONTROL for the star census:
    // the ROM's own drop runs, and the census then sees a star no referee
    // instance owns)
    static const bool nomute = env_on("SM64DS_ARENA_NOMUTE");
    const signed char keep = idx < 16 ? data_0209f310[idx] : 0;
    if (idx < 16 && !nomute) data_0209f310[idx] = 0;
    const int r = ((int (__fastcall *)(void *, void *))(size_t)orig)(self, edx);
    if (idx < 16) data_0209f310[idx] = keep;
    return r;
}
int __fastcall vs_ref_hurt_main_face(void *self, void *edx) {
    return muted_state_call(g.hurt_orig, self, edx);
}
int __fastcall vs_ref_hurtwater_init_face(void *self, void *edx) {
    return muted_state_call(g.hurtwater_orig, self, edx);
}
int __fastcall vs_ref_deadhit_main_face(void *self, void *edx) {
    return muted_state_call(g.deadhit_orig, self, edx);
}

void my_state_edges() {
    char *me = my_body();
    if (!me || !g.started || g.over) return;
    if (g.swim_clear_at && (int)(my_frame_now() - g.swim_clear_at) >= 0) {
        g.swim_clear_at = 0;
        *u8p(me, 0x706) = 0;
    }
    int st = vs_arena_state_index(me);
    if (st == 0xFF) return;
    const bool hurt_now = st == kStHurt || st == kStHurtWater ||
                          st == kStElectrocute || st == kStBurnFire ||
                          st == kStBurnLava;
    const bool hurt_before = g.prev_state == kStHurt || g.prev_state == kStHurtWater ||
                             g.prev_state == kStElectrocute ||
                             g.prev_state == kStBurnFire || g.prev_state == kStBurnLava;
    const unsigned fr = my_frame_now();
    if (hurt_now && !hurt_before && fr > g.hit_applied_frame + 6u) {
        // a hazard (chomp, iron ball, lava): always accepted, costs one star
        unsigned stamp;
        if (stamp_for(g.last_post_ms ? g.last_post_ms : now_ms(), &stamp)) {
            make_claim(CLAIM_HURT_SELF, 0xFF, 3, stamp, fr, 0);
            std::fprintf(stderr, "[vsarena-ref] hazard hurt (state %d): HURT_SELF\n", st);
        }
    }
    if (st == kStDeadPit && g.prev_state != kStDeadPit) {
        unsigned stamp;
        if (stamp_for(g.last_post_ms ? g.last_post_ms : now_ms(), &stamp)) {
            make_claim(CLAIM_PIT, 0xFF, 0, stamp, fr, 0);
            std::fprintf(stderr, "[vsarena-ref] pit fall: PIT (sanctions the respawn)\n");
        }
    }
    g.prev_state = st;
}

// ---------------------------------------------------------------------------
// QUIT
// ---------------------------------------------------------------------------
void quit_at_exit() {
    if (!g.active || !g.started || g.over || g.seat < 0) return;
    unsigned stamp = 0;
    stamp_for(now_ms(), &stamp);
    PClaim *c = make_claim(CLAIM_QUIT, 0xFF, 0, stamp, my_frame_now(), 0);
    if (c) {
        for (int i = 0; i < 3; ++i) { ::Sleep(15); send_claim(*c); }
    }
    std::fprintf(stderr, "[vsarena-ref] QUIT sent at exit\n");
}

// ---------------------------------------------------------------------------
// LAB RIGS (SM64DS_ARENA_RIG=race:<seed> | hit:<seed>). Scripted consoles for the
// proof rows: no human, no pad tape for the interesting moments. They move the
// body and press the pad at referee-clock instants every console agrees on;
// the ROM and the referee do everything else.
//   race  two consoles (seats 0 and 1): seat 0 breaks each marker (a claim, no
//         punch), and 4.5 s after the glass breaks both bodies are put on the
//         star at referee instants a scripted gap apart (-125..+125 ms).
//   hit   seat 0 punches seat 1's standing body: valid hits, then invalid ones
//         (a claim from out of reach, a repeat inside the hit immunity).
// ---------------------------------------------------------------------------
extern "C" char data_0209f49c[];
extern "C" char data_0209f49e[];
extern "C" char data_020a0e58[];   // PadData[4] {u16 held, u16 pressed}, the DS key words
extern "C" char data_020a0e5a[];   // the split symbol: PadData[i].pressed

unsigned rig_rand(unsigned seed, unsigned n) {
    unsigned x = seed * 2654435761u + n * 40503u + 12345u;
    x ^= x >> 15; x *= 2246822519u; x ^= x >> 13; x *= 3266489917u; x ^= x >> 16;
    return x;
}

void rig_parse() {
    const char *e = std::getenv("SM64DS_ARENA_RIG");
    if (!e || !*e) return;
    if (std::strncmp(e, "race", 4) == 0) g.rig = 1;
    else if (std::strncmp(e, "hit", 3) == 0) g.rig = 2;
    else if (std::strncmp(e, "chain", 5) == 0) g.rig = 3;
    else if (std::strncmp(e, "hurtgrab", 8) == 0) g.rig = 5;
    else if (std::strncmp(e, "caprace", 7) == 0) g.rig = 7;
    else if (std::strncmp(e, "capdrop", 7) == 0) g.rig = 8;
    else if (std::strncmp(e, "capforge", 8) == 0) g.rig = 9;
    else if (std::strncmp(e, "captongue", 9) == 0) g.rig = 10;
    else if (std::strncmp(e, "luigitouch", 10) == 0) g.rig = 11;
    else if (std::strncmp(e, "luigipunch", 10) == 0) g.rig = 12;
    else if (std::strncmp(e, "luigiwait", 9) == 0) g.rig = 13;
    else if (std::strncmp(e, "cap", 3) == 0) g.rig = 6;
    if (g.rig == 2 || g.rig == 3) g.rig_real = e[g.rig == 2 ? 3 : 5] == 'r';   // hitr / chainr
    else if (std::strncmp(e, "fuzz", 4) == 0) g.rig = 4;
    const char *c = std::strchr(e, ':');
    if (c) g.rig_seed = (unsigned)std::strtoul(c + 1, nullptr, 10);
    if (g.rig)
        std::fprintf(stderr, "[rig] SM64DS_ARENA_RIG=%s: scripted %s rig, seed %u\n",
                     e, g.rig == 1 ? "contention" : g.rig == 2 ? "hit" : g.rig == 5 ? "hurtgrab" :
                     g.rig == 6 ? "cap pickup" : g.rig == 7 ? "cap race" : g.rig == 8 ? "cap drop" :
                     g.rig == 9 ? "cap forgery" : g.rig == 10 ? "cap tongue" :
                     g.rig == 11 ? "Luigi touch" : g.rig == 12 ? "Luigi punch" :
                     g.rig == 13 ? "Luigi countdown" : "chain",
                     g.rig_seed);
}

// A DS key (0x0001 = the DS A key, which this ROM maps to the attack button; 0x0002 is jump) into the slot's PadData, where
// Stage::CheckInput (which runs inside the actor walk, AFTER this file's
// pre-walk half) turns it into the ROM's own held / pressed words. Writing the
// processed words (data_0209f49c / e) directly never worked: CheckInput rebuilds
// them every frame. `edge` = the first frame of the press.
void rig_press(int slot, unsigned short mask, bool edge) {
    *(unsigned short *)(data_020a0e58 + slot * 4) |= mask;
    if (edge) {
        *(unsigned short *)(data_020a0e58 + slot * 4 + 2) |= mask;
        *(unsigned short *)(data_020a0e5a + slot * 4) |= mask;
    }
}

void rig_teleport(char *me, const int *to) {
    int *p = (int *)(me + 0x5c);
    p[0] = to[0]; p[1] = to[1]; p[2] = to[2];
    s32r(me, 0x98) = 0;
    s32r(me, 0xa8) = 0;
}

void rig_race() {
    if (!g.started || g.over) return;
    unsigned r;
    if (!ref_now(&r)) return;
    char *me = my_body();
    if (!me) return;
    for (int k = 1; k < kMaxInst; ++k) {
        Inst &i = g.inst[k];
        if (!i.used || i.spilled) continue;
        if (i.state == 1 && g.seat == 0 && !i.rig_break_sent &&
            now_ms() - i.born_ms > 2000 && g.vs_frames > 120 &&
            (int)(r - g.t0) > 500) {  /* claims count only once the clock runs */
            // the breaker: a claim straight to the referee (the glass breaks on
            // the event, everywhere)
            unsigned stamp;
            if (!stamp_for(now_ms(), &stamp)) continue;
            PClaim *c = make_claim(CLAIM_MARKER_BREAK, (unsigned char)k,
                                   (unsigned short)GetAngleToCamera(g.seat), stamp,
                                   my_frame_now(), 0);
            if (c) { c->inst = (unsigned char)k; i.rig_break_sent = true; }
            std::fprintf(stderr, "[rig] seat 0 breaks the marker of inst %d\n", k);
        }
        if (i.state == 2 && i.t_broken && !i.rig_race_done) {
            char *s = star_by_uid(i.uid);
            if (!s) continue;
            // two seats: a scripted gap in -160..+160 ms (the plan's M3 model);
            // more: every seat's own offset in 0..160 ms. Every console computes
            // the same numbers from (seed, instance, seat).
            int gap = 0, off = 0;
            if (g.bodies == 2) {
                gap = (int)(rig_rand(g.rig_seed, (unsigned)k) % 321u) - 160;
                off = g.seat == 0 ? (gap < 0 ? -gap : 0) : (gap > 0 ? gap : 0);
            } else {
                off = (int)(rig_rand(g.rig_seed ^ ((unsigned)g.seat * 7919u),
                                     (unsigned)k) % 161u);
            }
            const unsigned at = i.t_broken + 3500u + (unsigned)off;
            if ((int)(r - at) < 0) continue;
            const int to[3] = { s32r(s, 0x5c), s32r(s, 0x60), s32r(s, 0x64) };
            rig_teleport(me, to);
            i.rig_race_done = true;
            ++g.rig_n;
            std::fprintf(stderr, "[rig] race inst=%d seat=%d gap=%d off=%d at_ref=%u "
                         "local_ms=%u star=(%d,%d,%d)\n", k, g.seat, gap, off, r,
                         now_ms(), to[0] >> 12, to[1] >> 12, to[2] >> 12);
        }
    }
}

// the punch every hit step uses: stand `dist` units behind seat 1 (in z), face
// it, press B for three frames; three frames after the press the pair pass's two
// words are written onto seat 1's body cylinder (what the ROM's overlap test would
// write when the attack touches), and the ordinary detection makes the claim
void rig_punch_start(int dist_units) {
    char *me = my_body();
    char *vb = body_of(1);
    if (!me || !vb) return;
    int to[3] = { s32r(vb, 0x5c), s32r(vb, 0x60), s32r(vb, 0x64) - (dist_units << 12) };
    rig_teleport(me, to);
    s16r(me, 0x8e) = 0;               // facing +Z, toward the victim
    s16r(me, 0x94) = 0;
    g.rig_press_total = g.rig_real ? 1 : 3;
    g.rig_press_frames = g.rig_press_total;
    g.rig_dist = dist_units;
    g.rig_follow = 0;       // (kept for a walking victim; with the puppets folded in, set > 0: the ROM's tongue
                            //  eats the copy and drags it along, which makes a follower run away from it)
}

// THE REAL PUNCH (rigs hitr / chainr). Seat 0 wears Mario's cap (the ROM's own
// character swap, the door path: param1 0 = Mario) so the attack
// button throws a punch whose attack cylinder really overlaps the puppet's body
// cylinder; nothing is written onto the victim, the pair pass does it all.
void rig_wear_cap() {
    if (!g.rig_real || g.seat != 0) return;
    char *me = my_body();
    if (!me) return;
    const unsigned fr = my_frame_now();
    if ((s32r(me, 8) & 0xff) == 0 || fr - g.rig_cap_frame < 30u) return;
    g.rig_cap_frame = fr ? fr : 1;
    port_player_set_character(me, 0);
    std::fprintf(stderr, "[rig] seat 0 puts on Mario's cap (param1 now %d)\n", s32r(me, 8));
}

// the distances a real punch is tried from: the first ones that touch decide
int rig_real_dist() {
    static const int tab[5] = { 55, 45, 65, 40, 75 };
    const int ov = env_int("SM64DS_RIG_DIST", 0, 1, 400);
    if (ov) return ov;
    return tab[g.rig_real_n++ % 5];
}

void rig_punch_tick(bool inject_words) {
    char *me = my_body();
    if (g.rig_follow > 0) {
        --g.rig_follow;
        char *vb = body_of(1);
        if (me && vb) {
            const int to[3] = { s32r(vb, 0x5c), s32r(vb, 0x60), s32r(vb, 0x64) - (g.rig_dist << 12) };
            rig_teleport(me, to);
        }
    }
    if (me && (g.rig_press_frames > 0 || g.rig_inject > 0))
        std::fprintf(stderr, "[rig] press frame: my state idx %d, ctrl held=%04x pressed=%04x\n",
                     vs_arena_state_index(me),
                     (unsigned)*(unsigned short *)(data_0209f49c + g.seat * 0x18),
                     (unsigned)*(unsigned short *)(data_0209f49e + g.seat * 0x18));
    if (g.rig_press_frames > 0) {
        rig_press(g.seat, 0x0001, g.rig_press_frames == g.rig_press_total);
        --g.rig_press_frames;
        if (g.rig_press_frames == 0 && inject_words) g.rig_inject = 3;
    } else if (g.rig_inject > 0 && --g.rig_inject == 0) {
        char *vb = body_of(1);
        if (vb && me) {
            u32r(vb, 0x2f4) = 0x40;
            u32r(vb, 0x2f8) = actor_uid(me);
            std::fprintf(stderr, "[rig] hit: the pair pass result written onto "
                         "seat 1's body cylinder (flags 0x40)\n");
        }
    }
}

void rig_direct_hit_claim(const char *what) {
    unsigned stamp;
    if (!stamp_for(now_ms(), &stamp)) return;
    PClaim *c = make_claim(CLAIM_HIT, 1, 0x40 >> 5, stamp, my_frame_now(), drawn_frame(1));
    std::fprintf(stderr, "[rig] hit: %s claim sent (id %u)\n", what, c ? c->cid : 0u);
}

// Until the puppets (lane PUPPET1) are folded in, seat 0's copy of seat 1 does not
// move: it stands at seat 1's spawn point. A hit is range-checked by the referee
// against seat 1's OWN reported position, so the rigs keep seat 1 at its spawn
// point whenever seat 0 punches (the walking selftest would take it away).
void rig_home_capture() {
    char *me = my_body();
    if (g.seat != 1 || g.rig_home_set || !me) return;
    for (int a = 0; a < 3; ++a) g.rig_home[a] = s32r(me, 0x5c + 4 * a);
    g.rig_home_set = true;
}
void rig_hold_home() {
    char *me = my_body();
    if (g.seat != 1 || !g.rig_home_set || !me) return;
    rig_teleport(me, g.rig_home);
}

// hit rig: seat 0 hits seat 1, standing. phase 0 a valid punch; 1 a repeat claim
// inside the hit immunity; 2 an attack from out of reach (a claim the referee
// range-checks against both bodies' own reported positions); then again.
void rig_hit() {
    if (!g.started || g.over || g.seat > 1) return;
    unsigned r;
    if (!ref_now(&r)) return;
    if (!my_body()) return;
    if (g.rig_next == 0) g.rig_next = g.t0 + 6000u;
    rig_home_capture();
    rig_hold_home();
    rig_wear_cap();
    rig_punch_tick(!g.rig_real);
    if (g.seat != 0 || (int)(r - g.rig_next) < 0) return;
    if (g.rig_phase == 0) {
        std::fprintf(stderr, "[rig] hit cycle %d: valid attack (attack button pressed, ref %u)\n", g.rig_n, r);
        rig_punch_start(g.rig_real ? rig_real_dist() : 90);
        g.rig_phase = 1;
        g.rig_next = r + 450u;
    } else if (g.rig_phase == 1) {
        // SM64DS_RIG_REPUNCH=1 (real rigs): a second REAL punch inside the victim's hit
        // immunity, the thing a player does when it keeps attacking (FIELD4 F9-c: the
        // referee refuses it INVULN; with the gate no claim is made at all)
        static const bool repunch = env_on("SM64DS_RIG_REPUNCH");
        if (g.rig_real && repunch) {
            std::fprintf(stderr, "[rig] hit cycle %d: second real punch inside the victim's immunity\n", g.rig_n);
            rig_punch_start(rig_real_dist());
        } else {
            rig_direct_hit_claim("repeat (inside the hit immunity)");
        }
        g.rig_phase = 2;
        g.rig_next = r + 3000u;
    } else if (g.rig_phase == 2) {
        std::fprintf(stderr, "[rig] hit cycle %d: out of reach (600 units, attack button pressed)\n", g.rig_n);
        rig_punch_start(600);
        g.rig_phase = 3;
        g.rig_next = r + 300u;
    } else {
        rig_direct_hit_claim("out-of-reach");
        g.rig_phase = 0;
        g.rig_next = r + 2500u;
        ++g.rig_n;
    }
}

// chain rig: seat 1 collects each star, seat 0 punches it (the star spills), seat 0
// collects the dropped star. Proves the grab, the hit verdict, the spill seam and
// the grab of a spilled star end to end.
void rig_chain() {
    if (!g.started || g.over || g.seat > 1) return;
    unsigned r;
    if (!ref_now(&r)) return;
    char *me = my_body();
    if (!me) return;
    rig_wear_cap();
    rig_punch_tick(!g.rig_real);
    rig_home_capture();
    if (g.chain_hit_at && (int)(r - (g.chain_hit_at - 2000u)) >= 0 &&
        (int)(r - (g.chain_hit_at + 1500u)) < 0)
        rig_hold_home();            // seat 1 stands where seat 0's copy of it does
    for (int k = 1; k < kMaxInst; ++k) {
        Inst &i = g.inst[k];
        if (!i.used) continue;
        if (i.state == 1 && g.seat == 0 && !i.rig_break_sent && !i.spilled &&
            now_ms() - i.born_ms > 2000 && g.vs_frames > 120 &&
            (int)(r - g.t0) > 500) {  /* claims count only once the clock runs */
            unsigned stamp;
            if (!stamp_for(now_ms(), &stamp)) continue;
            PClaim *c = make_claim(CLAIM_MARKER_BREAK, (unsigned char)k,
                                   (unsigned short)GetAngleToCamera(g.seat), stamp,
                                   my_frame_now(), 0);
            if (c) { c->inst = (unsigned char)k; i.rig_break_sent = true; }
            std::fprintf(stderr, "[rig] seat 0 breaks the marker of inst %d\n", k);
        }
        // seat 1 collects the order star 3.5 s after its glass broke
        if (i.state == 2 && !i.spilled && i.t_broken && !i.rig_race_done && g.seat == 1) {
            char *s = star_by_uid(i.uid);
            if (!s || (int)(r - (i.t_broken + 3500u)) < 0) continue;
            const int to[3] = { s32r(s, 0x5c), s32r(s, 0x60), s32r(s, 0x64) };
            rig_teleport(me, to);
            i.rig_race_done = true;
            std::fprintf(stderr, "[rig] chain: seat 1 goes to star inst %d (ref %u)\n", k, r);
        }
        // seat 0 collects a dropped star 4.5 s after the spill
        if (i.state == 2 && i.spilled && !i.rig_race_done && g.seat == 0) {
            char *s = star_by_uid(i.uid);
            // SM64DS_RIG_REGRAB_MS: how long after the spill seat 0 stands on the dropped
            // star (default 4.5 s, the star at rest; 500..2000 = still in flight)
            static const int regrab = env_int("SM64DS_RIG_REGRAB_MS", 4500, 0, 20000);
            if (!s || (int)(r - (i.t_broken + (unsigned)regrab)) < 0) continue;
            const int to[3] = { s32r(s, 0x5c), s32r(s, 0x60), s32r(s, 0x64) };
            rig_teleport(me, to);
            i.rig_race_done = true;
            std::fprintf(stderr, "[rig] chain: seat 0 goes to the dropped star inst %d (ref %u, %d ms after "
                         "the spill, star at (%d,%d,%d))\n", k, r, regrab, to[0] >> 12, to[1] >> 12, to[2] >> 12);
        }
    }
    // seat 0 punches seat 1 three seconds after a star was taken by seat 1
    if (g.seat == 0 && g.chain_hit_at && (int)(r - g.chain_hit_at) >= 0) {
        // SM64DS_RIG_HITMIN=n: only punch a victim that holds n stars or more (the
        // duplicate-star rows need a victim that still holds one after the spill,
        // which is what the ROM's own drop would be paid out of)
        static const int hitmin = env_int("SM64DS_RIG_HITMIN", 1, 1, 8);
        if (g.ledger[1] < hitmin) { g.chain_hit_at = 0; return; }   // (the next star seat 1 takes arms it again)
        // a real punch needs a puppeteer that can throw one: not in the star-get pose
        // (St_NoControl, 24) it is left in for a moment after a collect, not hurt,
        // not in the air; wait for the walk / wait states, for up to 4.5 s
        if (g.rig_real) {
            static unsigned wait_since = 0;
            const int ms = vs_arena_state_index(me);
            if (!wait_since) wait_since = r;
            if (ms != 14 && ms != 15 && (int)(r - wait_since) < 4500) { g.chain_hit_at = r + 200u; return; }
            wait_since = 0;
        }
        g.chain_hit_at = 0;
        const int dist = g.rig_real ? rig_real_dist() : 90;
        std::fprintf(stderr, "[rig] chain: seat 0 punches seat 1 (ref %u, from %d units, %s)\n", r, dist,
                     g.rig_real ? "real contact" : "injected pair-pass words");
        rig_punch_start(dist);
    }
}


// NEGATIVE ROW (SM64DS_ARENA_RIG=fuzz): every kind of malformed referee frame /
// event / object frame goes through the real receive path (apply_frame, the
// directed-event path, syno_valid) and must be dropped and counted, the client's
// state left exactly as it was; then a random-bytes and a mutated-frame sweep
// runs through the validators alone (not applied) to show they never fault.
unsigned fz_state_hash() {
    unsigned h = 2166136261u;
    auto mix = [&](unsigned v) { h = (h ^ v) * 16777619u; };
    mix(g.applied); mix(g.started); mix(g.over); mix(g.progress); mix(g.roster);
    mix(g.roster_now); mix(g.ever_roster); mix(g.match_state); mix(g.hdr_t0);
    mix(g.hdr_match_ms); mix(g.ledger_seq);
    for (int k = 0; k < 16; ++k) mix((unsigned char)g.ledger[k]);
    int used = 0;
    for (int k = 0; k < kMaxInst; ++k) if (g.inst[k].used) ++used;
    mix((unsigned)used);
    return h;
}

unsigned fz_rand(unsigned &st) {
    st ^= st << 13; st ^= st >> 17; st ^= st << 5;
    return st;
}

void fz_frame_template(RefFrame &f) {
    std::memset(&f, 0, sizeof f);
    f.tag = kTagR;
    f.version = 1;
    f.seat = kRefereeMark;
    f.match_state = g.match_state;
    f.newest_seq = g.ledger_seq;
    f.roster = g.roster_now;
    f.t0 = g.hdr_t0;
    f.match_ms = g.hdr_match_ms;
    std::memcpy(f.scores, g.ledger, 16);
}

void rig_fuzz() {
    if (!g.started || g.over || g.rig_phase != 0) return;
    if (!g.rig_next) g.rig_next = g.t0 + 4000u;
    unsigned r;
    if (!ref_now(&r) || (int)(r - g.rig_next) < 0) return;
    g.rig_phase = 1;
    const unsigned h0 = fz_state_hash();
    int sent = 0, rejected = 0, leaked = 0;
    auto inject = [&](const RefFrame &f, const char *what) {
        const unsigned long long fb = g.st.frames_bad, eb = g.st.events_bad;
        apply_frame(f);
        ++sent;
        if (g.st.frames_bad > fb || g.st.events_bad > eb) ++rejected;
        else { ++leaked; std::fprintf(stderr, "[rig] fuzz LEAK: %s was NOT rejected\n", what); }
    };
    RefFrame f;
#define BADFRAME(what, stmt) do { fz_frame_template(f); stmt; inject(f, what); } while (0)
    BADFRAME("tag", f.tag = 0x41414141u);
    BADFRAME("version", f.version = 2);
    BADFRAME("referee mark", f.seat = 1);
    BADFRAME("zero byte", f.zero = 1);
    BADFRAME("n_events 9", f.n_events = 9);
    BADFRAME("n_events 255", f.n_events = 255);
    BADFRAME("match_state 4", f.match_state = 4);
    BADFRAME("flags 0x80", f.flags = 0x80);
    BADFRAME("first_to_n 100", f.first_to_n = 100);
    BADFRAME("order_row 6", f.order_row = 6);
    BADFRAME("map 16", f.map = 16);
    BADFRAME("match_ms huge", f.match_ms = 3601u * 1000u);
    BADFRAME("roster beyond the bodies", f.roster = (unsigned short)(f.roster | 0x8000));
    BADFRAME("score -1", f.scores[0] = -1);
    BADFRAME("score -100", f.scores[0] = -100);
    BADFRAME("score of an absent seat", f.scores[15] = 1);
    BADFRAME("part on a stream frame", f.part = 0x31);
    BADFRAME("snapshot with no parts", f.flags = SYNR_F_SNAPSHOT; f.part = 0);
    BADFRAME("snapshot part index >= count", f.flags = SYNR_F_SNAPSHOT; f.part = 0x32);
    // events ride in a valid DIRECTED carrier (seq 0), so a drop leaves the
    // stream's sequence alone
#define BADEV(what, stmt) do { fz_frame_template(f); f.n_events = 1; f.flags = SYNR_F_DIRECTED; \
        RefEvent &e = f.ev[0]; std::memset(&e, 0, sizeof e); e.seat = 0; stmt; inject(f, what); } while (0)
    const int big = 0x7fffffff;
    BADEV("START seat", e.type = EV_START; e.seat = 1; e.e = 5);
    BADEV("START markers 0", e.type = EV_START; e.seat = 0xFF; e.e = 0);
    BADEV("START time 3601", e.type = EV_START; e.seat = 0xFF; e.e = 5; e.c = 3601);
    BADEV("START roster beyond", e.type = EV_START; e.seat = 0xFF; e.e = 5; e.d = 0x8000);
    BADEV("WAKE instance 0", e.type = EV_STAR_WAKE; e.seat = 0xFF; e.a = 0);
    BADEV("WAKE star id 8", e.type = EV_STAR_WAKE; e.seat = 0xFF; e.a = 1; e.b = 8);
    BADEV("WAKE progress 256", e.type = EV_STAR_WAKE; e.seat = 0xFF; e.a = 1; e.c = 256);
    BADEV("MARKER seat 15", e.type = EV_MARKER_BROKEN; e.seat = 15; e.a = 1);
    BADEV("MARKER x huge", e.type = EV_MARKER_BROKEN; e.a = 1; e.x = big);
    BADEV("MARKER z huge", e.type = EV_MARKER_BROKEN; e.a = 1; e.z = -big);
    BADEV("TAKEN score 128", e.type = EV_STAR_TAKEN; e.a = 1; e.b = 128);
    BADEV("TAKEN score 200", e.type = EV_STAR_TAKEN; e.a = 1; e.b = 200);
    BADEV("TAKEN progress 300", e.type = EV_STAR_TAKEN; e.a = 1; e.e = 300);
    BADEV("DENY kind 20", e.type = EV_DENY; e.a = 20; e.b = 1; e.d = 0xFF);
    BADEV("DENY reason 13", e.type = EV_DENY; e.a = 3; e.b = 13; e.d = 0xFF);
    BADEV("DENY reason 0", e.type = EV_DENY; e.a = 3; e.b = 0; e.d = 0xFF);
    BADEV("DENY winner 40", e.type = EV_DENY; e.a = 3; e.b = 3; e.d = 40);
    BADEV("HIT victim 0xFF", e.type = EV_HIT; e.seat = 0xFF; e.a = 0);
    BADEV("HIT attacker 9", e.type = EV_HIT; e.a = 9);
    BADEV("HIT flags 0x2001", e.type = EV_HIT; e.a = 1; e.d = 0x2001);
    BADEV("HIT spill inst 300", e.type = EV_HIT; e.a = 1; e.e = 300);
    BADEV("HIT y huge", e.type = EV_HIT; e.a = 1; e.y = big);
    BADEV("CLASH attacker 200", e.type = EV_CLASH; e.a = 200);
    BADEV("SPILL cause 0", e.type = EV_SPILL; e.a = 1; e.c = 0);
    BADEV("SPILL cause 5", e.type = EV_SPILL; e.a = 1; e.c = 5);
    BADEV("SPILL marker 8", e.type = EV_SPILL; e.a = 1; e.b = 8; e.c = 1);
    BADEV("SPILL score 200", e.type = EV_SPILL; e.a = 1; e.c = 1; e.d = 200);
    BADEV("SPILL instance 0", e.type = EV_SPILL; e.a = 0; e.c = 1);
    BADEV("SPILL origin huge", e.type = EV_SPILL; e.a = 1; e.c = 1; e.x = big);
    BADEV("OVER reason 0", e.type = EV_MATCH_OVER; e.seat = 0xFF; e.a = 0);
    BADEV("OVER reason 5", e.type = EV_MATCH_OVER; e.seat = 0xFF; e.a = 5);
    BADEV("OVER winners mask beyond", e.type = EV_MATCH_OVER; e.seat = 0xFF; e.a = 1; e.c = 0xFFFF);
    BADEV("OVER winner count 17", e.type = EV_MATCH_OVER; e.seat = 0xFF; e.a = 1; e.b = 17);
    BADEV("LEAVE cause 0", e.type = EV_LEAVE; e.a = 0);
    BADEV("LEAVE seat 15", e.type = EV_LEAVE; e.seat = 15; e.a = 1);
    BADEV("JOIN mid 2", e.type = EV_JOIN; e.a = 2);
    BADEV("STAR_STATE state 3", e.type = EV_STAR_STATE; e.seat = 0xFF; e.a = 1; e.b = 3);
    BADEV("STAR_STATE marker 8", e.type = EV_STAR_STATE; e.seat = 0xFF; e.a = 1; e.b = 1; e.c = 8);
    BADEV("STAR_STATE position", e.type = EV_STAR_STATE; e.seat = 0xFF; e.a = 1; e.b = 1; e.y = -big);
    BADEV("ACCEPTED kind 0", e.type = EV_ACCEPTED; e.a = 0);
    BADEV("ACCEPTED spill 300", e.type = EV_ACCEPTED; e.a = 3; e.d = 300);
    BADEV("EATEN b 5", e.type = EV_EATEN; e.a = 1; e.b = 5);
    BADEV("RELEASED eater 99", e.type = EV_RELEASED; e.a = 99);
    BADEV("unknown type 0", e.type = 0);
    BADEV("unknown type 24", e.type = 24);
    BADEV("unknown type 255", e.type = 255);
    // Luigi Infection events in a match that is not one (this rig's match is Classic)
    BADEV("INFECTED outside a Luigi match", e.type = EV_INFECTED; e.a = 0xFF; e.b = 1; e.d = 1);
    BADEV("TEAM_STATE outside a Luigi match", e.type = EV_TEAM_STATE; e.seat = 0xFF);
    BADEV("seat 16", e.type = EV_JOIN; e.seat = 16);
#undef BADEV
#undef BADFRAME
    const unsigned h1 = fz_state_hash();
    // the object frames, through the real validator
    int ob_sent = 0, ob_bad = 0;
    auto obj = [&](const char *what, const ObjFrame &o, int seat) {
        ++ob_sent;
        if (!syno_valid(o, seat)) ++ob_bad;
        else std::fprintf(stderr, "[rig] fuzz LEAK: SYNO %s was NOT rejected\n", what);
    };
    ObjFrame base;
    std::memset(&base, 0, sizeof base);
    base.tag = kTagO; base.version = 1; base.seat = 0; base.obj_class = 1;
    base.obj_id = 1; base.state = 2;
    ObjFrame o;
    o = base; o.tag = 0x41414141u;  obj("tag", o, 0);
    o = base; o.version = 2;        obj("version", o, 0);
    o = base; o.obj_class = 2;      obj("class", o, 0);
    o = base; o.obj_id = 0;         obj("id 0", o, 0);
    o = base; o.state = 0;          obj("state 0", o, 0);
    o = base; o.state = 4;          obj("state 4", o, 0);
    o = base; o.flags = 2;          obj("flags", o, 0);
    o = base; o.pos[0] = big;       obj("x huge", o, 0);
    o = base; o.pos[1] = -big;      obj("y huge", o, 0);
    o = base; o.pos[2] = big;       obj("z huge", o, 0);
    o = base;                       obj("seat 15", o, 15);
    o = base;                       obj("seat -1", o, -1);
    // the sweep: validators only, never applied
    unsigned st = g.rig_seed * 2654435761u | 1u;
    long rnd_pass = 0, mut_pass = 0, ev_pass = 0, ob_pass = 0;
    const int N = 100000;
    for (int n = 0; n < N; ++n) {
        unsigned char b[kSynrBytes];
        for (int k = 0; k < kSynrBytes; ++k) b[k] = (unsigned char)fz_rand(st);
        RefFrame q;
        std::memcpy(&q, b, sizeof q);
        if (ref_frame_valid(q)) ++rnd_pass;
        for (int k = 0; k < 8; ++k) if (ref_event_valid(q.ev[k])) ++ev_pass;
        fz_frame_template(q);
        q.n_events = 8;
        for (int k = 0; k < 8; ++k) {
            q.ev[k].type = (unsigned char)(1 + k);
            q.ev[k].seat = 0; q.ev[k].a = 1; q.ev[k].c = 1; q.ev[k].e = 1;
        }
        unsigned char m[kSynrBytes];
        std::memcpy(m, &q, sizeof m);
        const int muts = 1 + (int)(fz_rand(st) % 6);
        for (int k = 0; k < muts; ++k) m[fz_rand(st) % kSynrBytes] = (unsigned char)fz_rand(st);
        std::memcpy(&q, m, sizeof q);
        if (ref_frame_valid(q)) ++mut_pass;
        unsigned char ob[kSynoBytes];
        for (int k = 0; k < kSynoBytes; ++k) ob[k] = (unsigned char)fz_rand(st);
        ObjFrame ro;
        std::memcpy(&ro, ob, sizeof ro);
        if (syno_valid(ro, (int)(fz_rand(st) % 24))) ++ob_pass;
    }
    std::fprintf(stderr, "[rig] fuzz negative: frames+events injected %d, rejected %d, "
                 "leaked %d, state %s (hash %08x -> %08x); SYNO bad frames %d, rejected %d; "
                 "sweep %d: random SYNR passing the frame check %ld (their events passing %ld of %d), "
                 "mutated-valid SYNR passing %ld, random SYNO passing %ld; no fault\n",
                 sent, rejected, leaked, h0 == h1 ? "UNCHANGED" : "CHANGED", h0, h1,
                 ob_sent, ob_bad, N, rnd_pass, ev_pass, N * 8, mut_pass, ob_pass);
    std::fprintf(stderr, "[rig] fuzz %s\n", (leaked == 0 && h0 == h1 && rejected == sent &&
                 ob_bad == ob_sent) ? "PASS" : "FAIL");
}

// HURT-GRAB RIG (SM64DS_ARENA_RIG=hurtgrab): FIELD4 F9-b's STATE refusal. Seat 0
// breaks each marker; 3.5 s later seat 1 hurts itself (the ROM's own Player::Hurt)
// and stands on the star in the same frame, so its body touches the star while it is
// in St_Hurt. With the local gate the ROM never starts the collect (no star pose, no
// toss) and the grab lands when the hurt state is over; with
// SM64DS_ARENA_NOTOUCHGATE=1 (the control) the collect starts, the referee answers
// STATE and the star is tossed back.
void rig_hurtgrab() {
    if (!g.started || g.over) return;
    unsigned r;
    if (!ref_now(&r)) return;
    char *me = my_body();
    if (!me) return;
    for (int k = 1; k < kMaxInst; ++k) {
        Inst &i = g.inst[k];
        if (!i.used || i.spilled) continue;
        if (i.state == 1 && g.seat == 0 && !i.rig_break_sent &&
            now_ms() - i.born_ms > 2000 && g.vs_frames > 120 &&
            (int)(r - g.t0) > 500) {
            unsigned stamp;
            if (!stamp_for(now_ms(), &stamp)) continue;
            PClaim *c = make_claim(CLAIM_MARKER_BREAK, (unsigned char)k,
                                   (unsigned short)GetAngleToCamera(g.seat), stamp,
                                   my_frame_now(), 0);
            if (c) { c->inst = (unsigned char)k; i.rig_break_sent = true; }
            std::fprintf(stderr, "[rig] seat 0 breaks the marker of inst %d\n", k);
        }
        if (i.state == 2 && i.t_broken && !i.rig_race_done && g.seat == 1) {
            char *s = star_by_uid(i.uid);
            if (!s || (int)(r - (i.t_broken + 3500u)) < 0) continue;
            const int to[3] = { s32r(s, 0x5c), s32r(s, 0x60), s32r(s, 0x64) };
            int from[3] = { s32r(me, 0x5c) + 0x50000, s32r(me, 0x60), s32r(me, 0x64) };
            const int hr = _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(me, from, 1u, 0, 0u, 4u, 1u);
            g.hit_applied_frame = my_frame_now();      // (not a hazard: no HURT_SELF claim)
            rig_teleport(me, to);
            i.rig_race_done = true;
            i.rig_hurt_ref = r ? r : 1;
            ++g.rig_n;
            std::fprintf(stderr, "[rig] hurtgrab: seat 1 hurt itself (Hurt returned %d, state %d) and "
                         "stands on star inst %d (ref %u)\n", hr, vs_arena_state_index(me), k, r);
        }
        // 2.5 s later, healthy: stand on the star again (a legitimate grab must still land)
        if (i.state == 2 && i.rig_hurt_ref && !i.rig_second && g.seat == 1 &&
            (int)(r - (i.rig_hurt_ref + 2500u)) >= 0) {
            char *s = star_by_uid(i.uid);
            if (!s) continue;
            const int to[3] = { s32r(s, 0x5c), s32r(s, 0x60), s32r(s, 0x64) };
            rig_teleport(me, to);
            i.rig_second = true;
            std::fprintf(stderr, "[rig] hurtgrab: seat 1 healthy, stands on star inst %d again (ref %u, "
                         "state %d)\n", k, r, vs_arena_state_index(me));
        }
    }
}


// CAP RIGS (SM64DS_ARENA_RIG=cap | caprace | capdrop | capforge). Scripted consoles for
// the proof rows. Every 6 s (9 s for capdrop) of referee time, from 4 s after the
// match started, a cycle runs; the ROM and the referee do everything else.
//   cap       seat 0 puts its body on cap 1 + (cycle % caps): a pickup.
//   caprace   every seat puts its body on the same cap, a scripted gap apart
//             (-160..+160 ms for two seats): exactly one may get it.
//   capdrop   seat 0 takes a cap, 3.5 s later hurts itself (the ROM pops the cap), seat 1
//             goes for the knocked-off cap 1.6 s after the event.
//   capforge  seat 1 sends claims the referee must refuse (a cap that is not there, a cap
//             too far away, a wrong place hash, id 0, a drop with no cap on).
// the n-th placed cap of this map (ids are the death-table slots + 1, so they do not start at 1)
int rig_cap_id(int n) {
    for (int k = 1; k <= kCapPlacedMax; ++k)
        if (g.caps[k].used && g.caps[k].placed && n-- == 0) return k;
    return 0;
}

int g_rig_cap_hold_id = 0, g_rig_cap_hold_left = 0;

// the body lingers on the cap for a few frames (a one-frame contact can be missed)
void rig_cap_hold_tick() {
    if (g_rig_cap_hold_left <= 0) return;
    --g_rig_cap_hold_left;
    CapRec *r = cap_rec(g_rig_cap_hold_id);
    char *me = my_body();
    if (!r || !me || r->mine || r->gone) { g_rig_cap_hold_left = 0; return; }
    const int to[3] = { r->origin[0], r->origin[1], r->origin[2] };
    rig_teleport(me, to);
}

void rig_cap_goto(int id) {
    CapRec *r = cap_rec(id);
    char *me = my_body();
    if (!r || !me) return;
    const int to[3] = { r->origin[0], r->origin[1], r->origin[2] };
    rig_teleport(me, to);
    g_rig_cap_hold_id = id;
    g_rig_cap_hold_left = 12;
    std::fprintf(stderr, "[rig] cap: seat %d goes to cap %d at (%d,%d,%d) (frame %u)\n", g.seat, id,
                 to[0] >> 12, to[1] >> 12, to[2] >> 12, my_frame_now());
}

void rig_cap() {
    if (!g.started || g.over || !g.caps_ready || g.caps_n <= 0) return;
    rig_cap_hold_tick();
    unsigned r;
    if (!ref_now(&r)) return;
    if (!my_body()) return;
    const int rel = (int)(r - g.t0) - 4000;
    if (rel < 0) return;
    const int period = 6000;
    const int k = rel / period, within = rel % period;
    static int did = -1;
    int off = 0;
    if (g.rig == 7) {
        if (g.bodies == 2) {
            static const int fixed = env_int("SM64DS_RIG_CAPGAP", -1000, -1000, 1000);
            const int gap = fixed != -1000 ? (k % 2 ? -fixed : fixed)
                                           : (int)(rig_rand(g.rig_seed, (unsigned)k) % 321u) - 160;
            off = g.seat == 0 ? (gap < 0 ? -gap : 0) : (gap > 0 ? gap : 0);
        } else {
            off = (int)(rig_rand(g.rig_seed ^ ((unsigned)g.seat * 7919u), (unsigned)k) % 161u);
        }
    } else if (g.seat != 0) {
        // PICTURES (SM64DS_RIG_CAPOBS=1..4): the other seat waits 450 units from the cap
        // (E, W, S, N) so its camera has the cap in view when seat 0 picks it up
        static const int obs = env_int("SM64DS_RIG_CAPOBS", 0, 0, 4);
        static int did_obs = -1;
        if (g.rig == 6 && obs && k != did_obs && within >= 300) {
            did_obs = k;
            if (CapRec *c = cap_rec(rig_cap_id(k % g.caps_n))) {
                static const int dx[4] = { 450, -450, 0, 0 }, dz[4] = { 0, 0, 450, -450 };
                const int to[3] = { c->origin[0] + (dx[obs - 1] << 12), c->origin[1], c->origin[2] + (dz[obs - 1] << 12) };
                rig_teleport(my_body(), to);
                std::fprintf(stderr, "[rig] cap: observer %d waits 450 units from cap %d\n", g.seat, (int)(c - g.caps));
            }
        }
        return;
    }
    {
        static const int obs = env_int("SM64DS_RIG_CAPOBS", 0, 0, 4);
        if (g.rig == 6 && obs && g.seat == 0) off = 3500;
    }
    if (k == did || within < off) return;
    did = k;
    rig_cap_goto(rig_cap_id(k % g.caps_n));
    ++g.rig_n;
}

void rig_capdrop() {
    if (!g.started || g.over || !g.caps_ready || g.caps_n <= 0) return;
    rig_cap_hold_tick();
    unsigned r;
    if (!ref_now(&r)) return;
    char *me = my_body();
    if (!me) return;
    if (g.seat == 1) {
        // PICTURES (SM64DS_RIG_CAPOBS=1..4): seat 1 stands 400 units from seat 0's body when it gets hurt
        static const int obs = env_int("SM64DS_RIG_CAPOBS", 0, 0, 4);
        static int did_obs = -1;
        const int ko = ((int)(r - g.t0) - 4000) / 9000, wo = ((int)(r - g.t0) - 4000) % 9000;
        if (obs && (int)(r - g.t0) >= 4000 && ko != did_obs && wo >= 2900) {
            if (char *b0 = body_of(0)) {
                did_obs = ko;
                static const int dx[4] = { 400, -400, 0, 0 }, dz[4] = { 0, 0, 400, -400 };
                const int to[3] = { s32r(b0, 0x5c) + (dx[obs - 1] << 12), s32r(b0, 0x60), s32r(b0, 0x64) + (dz[obs - 1] << 12) };
                rig_teleport(me, to);
                std::fprintf(stderr, "[rig] capdrop: observer seat 1 waits 400 units from seat 0\n");
            }
        }
        if (g.rig == 10) {
            // FIX9C: the knocked-off cap is TONGUED, not walked into (Yoshi, look 3), again every 40 frames
            // until my character changes or the cap is gone
            if (g.rig_drop_id && (int)(now_ms() - g.rig_drop_at) >= 0 && (int)(now_ms() - g.rig_tongue_until) < 0 &&
                (int)(now_ms() - g.rig_tongue_next) >= 0) {
                CapRec *c = cap_rec(g.rig_drop_id);
                char *a = c ? cap_actor_of(*c) : nullptr;
                if (a && (s32r(me, 8) & 0xff) == 3) {
                    const int to[3] = { s32r(a, 0x5c), s32r(a, 0x60), s32r(a, 0x64) };
                    static const int back = env_int("SM64DS_RIG_TONGUE_BACK", 60, 20, 200);
                    vs_eat_tongue_at(to, back);
                    g.rig_tongue_next = now_ms() + 700u;
                    std::fprintf(stderr, "[rig] captongue: seat 1 tongues the knocked-off cap %d (frame %u, look %d)\n",
                                 g.rig_drop_id, my_frame_now(), s32r(me, 8) & 0xff);
                } else if (!a) {
                    std::fprintf(stderr, "[rig] captongue: no actor for the knocked-off cap (gone or taken)\n");
                    g.rig_drop_id = 0;
                }
            }
            return;
        }
        if (g.rig_drop_id && (int)(now_ms() - g.rig_drop_at) >= 0) {
            CapRec *c = cap_rec(g.rig_drop_id);
            char *a = c ? cap_actor_of(*c) : nullptr;
            g.rig_drop_id = 0;
            if (a) {
                const int to[3] = { s32r(a, 0x5c), s32r(a, 0x60), s32r(a, 0x64) };
                rig_teleport(me, to);
                std::fprintf(stderr, "[rig] capdrop: seat 1 goes to the knocked-off cap at (%d,%d,%d)\n",
                             to[0] >> 12, to[1] >> 12, to[2] >> 12);
            } else {
                std::fprintf(stderr, "[rig] capdrop: seat 1 has no actor for the knocked-off cap\n");
            }
        }
        return;
    }
    if (g.seat != 0) return;
    const int rel = (int)(r - g.t0) - 4000;
    if (rel < 0) return;
    const int period = 9000;
    const int k = rel / period, within = rel % period;
    static int took = -1, hurt = -1;
    if (k != took && within >= 0) {
        took = k;
        rig_cap_goto(rig_cap_id(k % g.caps_n));
    }
    if (k != hurt && within >= 3500 && (within - 3500) % 100 < 34) {
        // (retried for a few seconds: Player::Hurt answers 0 while the put-on is still running)
        int from[3] = { s32r(me, 0x5c) + 0x50000, s32r(me, 0x60), s32r(me, 0x64) };
        const int hr = _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(me, from, 1u, 0, 0u, 4u, 1u);
        g.hit_applied_frame = my_frame_now();          // (not a hazard: no HURT_SELF claim)
        std::fprintf(stderr, "[rig] capdrop: seat 0 hurts itself with the cap on (Hurt returned %d, look %d, "
                     "state %d; gate 6a0=%x 703=%x 73c=%x 6fb=%x 708=%x 709=%x)\n", hr, s32r(me, 8) & 0xff,
                     vs_arena_state_index(me), (unsigned)u16r(me, 0x6a0), (unsigned)*u8p(me, 0x703),
                     (unsigned)u16r(me, 0x73c), (unsigned)*u8p(me, 0x6fb), (unsigned)*u8p(me, 0x708),
                     (unsigned)*u8p(me, 0x709));
        if (hr) { hurt = k; ++g.rig_n; }
        else if (within > 8500) hurt = k;
    }
}

void rig_capforge() {
    if (!g.started || g.over || !g.caps_ready || g.caps_n <= 0 || g.seat != 1) return;
    unsigned r;
    if (!ref_now(&r)) return;
    char *me = my_body();
    if (!me) return;
    const int rel = (int)(r - g.t0) - 5000;
    if (rel < 0) return;
    static int step = 0;
    if (step >= 5 || rel < step * 700) return;
    unsigned stamp;
    if (!stamp_for(now_ms(), &stamp)) return;
    unsigned char target = 0;
    unsigned short aux = 0;
    unsigned char kind = CLAIM_CAP_TAKE;
    const char *what = "";
    int far_id = 1;
    long long far_d = -1;
    for (int k = 1; k <= kCapPlacedMax; ++k) {
        if (!g.caps[k].used || !g.caps[k].placed) continue;
        const long long dx = (long long)(g.caps[k].origin[0] - s32r(me, 0x5c)) >> 12;
        const long long dz = (long long)(g.caps[k].origin[2] - s32r(me, 0x64)) >> 12;
        if (dx * dx + dz * dz > far_d) { far_d = dx * dx + dz * dz; far_id = k; }
    }
    switch (step) {
    case 0: target = 99; what = "a cap that is not there (99): expect GONE"; break;
    case 1: target = (unsigned char)far_id; aux = g.caps[far_id].hash;
            what = "the cap farthest from my body, right hash: expect RANGE"; break;
    case 2: target = (unsigned char)rig_cap_id(0); aux = (unsigned short)(g.caps[target].hash ^ 0x55);
            what = "the first cap with a wrong place hash: expect BAD"; break;
    case 3: target = 0; what = "cap id 0: expect BAD"; break;
    case 4: kind = CLAIM_CAP_DROP; target = 2; aux = 0x1234;
            what = "CAP_DROP with no cap on and not hurt: expect STATE"; break;
    }
    PClaim *c = make_claim(kind, target, aux, stamp, my_frame_now(), 0);
    std::fprintf(stderr, "[rig] capforge step %d: %s (claim %u)\n", step, what, c ? (unsigned)c->cid : 0u);
    ++step;
    ++g.rig_n;
}

// LUIGI RIGS (SM64DS_ARENA_RIG=luigitouch | luigipunch | luigiwait, lane ARENALUIGI1).
// Every console: walks right on the D-pad for the middle of the 5 s countdown and
// prints how far it got (Tango's rule: everyone can run during the countdown; the
// ROM's entry step holds the body without the lift). After T0 the referee has
// picked the Luigi:
//   a survivor's console punches the Luigi first (wears Mario's cap for a real
//   punch, the hitr way): the Luigi is immune, so no claim may leave;
//   then the Luigi's console steps onto the survivor (touch: bodies overlap, no
//   button) or punches it (punch: the Luigi's own A button), and the referee's
//   INFECTED makes the survivor a Luigi on both screens.
// luigiwait does the countdown walk only (the time-up and the leave rows).
extern "C" unsigned char data_0209f2bc;   // the VS start countdown (3, 2, 1, 0)
int rig_luigi_victim() {
    for (int k = 0; k < g.bodies && k < 16; ++k)
        if (k != g.seat && (g.roster_now >> k & 1u) && !is_luigi(k) && body_of(k)) return k;
    return -1;
}
int rig_luigi_any() {
    for (int k = 0; k < g.bodies && k < 16; ++k)
        if (k != g.seat && (g.roster_now >> k & 1u) && is_luigi(k) && body_of(k)) return k;
    return -1;
}
void rig_luigi_stand_by(int seat, int dist_units) {
    char *me = my_body();
    char *vb = body_of(seat);
    if (!me || !vb) return;
    const int to[3] = { s32r(vb, 0x5c), s32r(vb, 0x60), s32r(vb, 0x64) - (dist_units << 12) };
    rig_teleport(me, to);
    s16r(me, 0x8e) = 0;
    s16r(me, 0x94) = 0;
}
// a survivor's punch needs a fist: Yoshi's attack button is the tongue (and a tongue on a
// Luigi bounces off, vs_arena_eat.cpp). Mario's look through the door swap, as rig_wear_cap.
void rig_wear_cap_luigi() {
    char *me = my_body();
    if (!me || me_luigi() || (s32r(me, 8) & 0xff) == 0) return;
    const unsigned fr = my_frame_now();
    if (fr - g.rig_cap_frame < 30u) return;
    g.rig_cap_frame = fr ? fr : 1;
    port_player_set_character(me, 0);
    std::fprintf(stderr, "[rig] survivor seat %d puts on Mario's cap (param1 now %d)\n", g.seat, s32r(me, 8));
}
void rig_luigi() {
    if (!g.started || g.over) return;
    unsigned r;
    if (!ref_now(&r)) return;
    char *me = my_body();
    if (!me) return;
    // the countdown walk: D-pad right from T0-4000 to T0-1000
    if (g.rig_cd_phase == 0 && (int)(r - (g.t0 - 4000u)) >= 0 && (int)(r - g.t0) < 0) {
        for (int a = 0; a < 3; ++a) g.rig_cd_pos[a] = s32r(me, 0x5c + 4 * a);
        g.rig_cd_phase = 1;
        std::fprintf(stderr, "[rig] luigi countdown walk starts (ref %u, T0-%d ms, state idx %d)\n",
                     r, (int)(g.t0 - r), vs_arena_state_index(me));
    }
    if (g.rig_cd_phase == 1) {
        if ((int)(r - (g.t0 - 1000u)) < 0) {
            rig_press(g.seat, 0x0010, false);
        } else {
            long long d2 = 0;
            for (int a = 0; a < 3; ++a) {
                const long long d = (long long)((s32r(me, 0x5c + 4 * a) - g.rig_cd_pos[a]) >> 12);
                d2 += d * d;
            }
            int dist = 0;
            while ((long long)(dist + 1) * (dist + 1) <= d2) ++dist;
            std::fprintf(stderr, "[rig] luigi countdown walk: moved %d units in the countdown "
                         "(state idx %d, count %d) %s\n", dist, vs_arena_state_index(me),
                         (int)data_0209f2bc, dist >= 50 ? "MOVED" : "HELD");
            g.rig_cd_phase = 2;
        }
    }
    if (g.rig == 13 || !g.picked || (int)(r - g.t0) < 0) return;
    // SM64DS_RIG_LUIGI_DELAY_MS: hold the scripted punches and tags back (the late-joiner row)
    if (g.rig_lnext == 0)
        g.rig_lnext = g.t0 + 2500u + (unsigned)env_int("SM64DS_RIG_LUIGI_DELAY_MS", 0, 0, 120000);
    rig_wear_cap_luigi();
    rig_punch_tick(false);
    if (g.rig_lclose > 0 && --g.rig_lclose == 0 && g.rig_lvictim >= 0) {
        rig_luigi_stand_by(g.rig_lvictim, 60);
        std::fprintf(stderr, "[rig] luigi punch: stepped in to 60 units of seat %d (state idx %d)\n",
                     g.rig_lvictim, vs_arena_state_index(me));
    }
    if ((int)(r - g.rig_lnext) < 0) return;
    // a punch is thrown from beyond body contact (a body touch is a TAG by the Luigi's
    // console, on purpose: Tango's rule): the first distances that land decide
    static const int kPunchDist[5] = { 85, 95, 75, 105, 70 };
    // the punch rig leaves the immunity step to the touch rig: a survivor next to the Luigi is
    // touched (a TAG) before the Luigi throws its own punch
    if (g.rig == 12 && g.rig_lphase < 3) g.rig_lphase = 3;
    if (g.rig_lphase <= 2) {
        // the immunity step, three tries: the lowest survivor punches the lowest Luigi
        const int lu = rig_luigi_any();
        if (!me_luigi() && lu >= 0) {
            bool lowest = true;
            for (int k = 0; k < g.seat; ++k)
                if ((g.roster_now >> k & 1u) && !is_luigi(k)) lowest = false;
            if (lowest) {
                const int d = kPunchDist[g.rig_lphase % 5];
                std::fprintf(stderr, "[rig] luigi immunity: survivor seat %d punches Luigi seat %d "
                             "from %d units (ref %u)\n", g.seat, lu, d, r);
                rig_luigi_stand_by(lu, d);
                g.rig_press_total = 1;
                g.rig_press_frames = 1;
            }
        }
        ++g.rig_lphase;
        g.rig_lnext = r + (g.rig_lphase == 3 ? 2000u : 900u);
    } else if (g.rig_lphase == 3) {
        if (me_luigi()) {
            const int v = rig_luigi_victim();
            if (v >= 0) {
                g.rig_lvictim = v;
                if (g.rig == 11) {
                    std::fprintf(stderr, "[rig] luigi touch: Luigi seat %d steps onto survivor seat %d "
                                 "(ref %u)\n", g.seat, v, r);
                    rig_luigi_stand_by(v, 20);
                    g.rig_follow = 0;
                } else {
                    // bodies touch from ~85 units (a TAG, first): the swing starts out of reach
                    // and is carried in while the fist is out, so the first contact is the attack
                    static const int kClose[5] = { 3, 5, 2, 7, 4 };
                    const int fr = kClose[g.rig_lclose_n++ % 5];
                    std::fprintf(stderr, "[rig] luigi punch: Luigi seat %d swings at survivor seat %d "
                                 "from 300 units, stepping in after %d frames (ref %u)\n", g.seat, v, fr, r);
                    rig_luigi_stand_by(v, 300);
                    g.rig_press_total = 1;
                    g.rig_press_frames = 1;
                    g.rig_lclose = fr;
                }
            }
        }
        g.rig_lphase = 4;
        g.rig_lnext = r + (g.rig == 11 ? 3000u : 1200u);
    } else {
        // again, for the next survivor (or the same one if the punch missed)
        g.rig_lphase = 3;
        g.rig_lnext = r + 500u;
    }
}

void rig_step() {
    if (g.rig == 1) rig_race();
    else if (g.rig == 2) rig_hit();
    else if (g.rig == 3) rig_chain();
    else if (g.rig == 4) rig_fuzz();
    else if (g.rig == 5) rig_hurtgrab();
    else if (g.rig == 6 || g.rig == 7) rig_cap();
    else if (g.rig == 8 || g.rig == 10) rig_capdrop();
    else if (g.rig == 9) rig_capforge();
    else if (g.rig >= 11 && g.rig <= 13) rig_luigi();
}

// ---------------------------------------------------------------------------
// ONE-SHOTS AT THE FIRST ARENA FRAME
// ---------------------------------------------------------------------------
void init_once() {
    if (g.inited) return;
    g.inited = true;
    install_order();
    // dispatch entry 5 (the collect confirm window)
    if (!g.star5_orig) {
        g.star5_orig = data_ov002_021109d8[5].fn;
        data_ov002_021109d8[5].fn = (unsigned)(size_t)&vs_ref_star5_face;
    }
    // St_Hurt's Main (the cell at +8 of the state object)
    if (!g.hurt_orig) {
        unsigned *cell = (unsigned *)(data_ov002_02110094 + 8);
        g.hurt_orig = cell[0];
        cell[0] = (unsigned)(size_t)&vs_ref_hurt_main_face;
    }
    if (!g.hurtwater_orig) {                       // Init is cell 0, Main cell 1 (8 bytes each)
        unsigned *cell = (unsigned *)(data_ov002_021100ac + 0);
        g.hurtwater_orig = cell[0];
        cell[0] = (unsigned)(size_t)&vs_ref_hurtwater_init_face;
    }
    if (!g.deadhit_orig) {
        unsigned *cell = (unsigned *)(data_ov002_0211010c + 8);
        g.deadhit_orig = cell[0];
        cell[0] = (unsigned)(size_t)&vs_ref_deadhit_main_face;
    }
    static bool quit_hooked = false;   // SEAMVS1: once per process, though a later match in place runs init_once again
    if (!quit_hooked) { quit_hooked = true; std::atexit(quit_at_exit); }
    rig_parse();
    std::fprintf(stderr, "[vsarena] referee client up: seat %d, %d bodies, order "
                 "buffer installed, collect + hurt seams wrapped\n", g.seat,
                 g.bodies);
}

// STAR CENSUS (the "no duplicate star" row, lab only: SM64DS_ARENA_REFLOG=1).
// Every PowerStar actor that is not a dormant level copy must belong to an
// instance the referee named (an order star, or a SPILL). A star the ROM drops
// by itself (func_ov002_020d94cc, muted in vs_ref_hurt_main_face) is one no
// instance owns: an ORPHAN. A line is printed when the numbers have held still
// for 20 frames and differ from the last line.
void star_census() {
    if (!g.log || !g.started) return;
    int live = 0, orphans = 0, free_known = 0, missing = 0;
    each_class(kClassStar, [&](char *a) {
        if (s32r(a, 0x440) == 9) return;             // dormant level copy
        ++live;
        if (!inst_by_uid(actor_uid(a))) ++orphans;
    });
    int sum = 0;
    for (int k = 0; k < 16; ++k) sum += g.ledger[k];
    for (int k = 1; k < kMaxInst; ++k) {
        const Inst &i = g.inst[k];
        if (!i.used || i.state != 2) continue;
        ++free_known;
        // (an instance that never had an actor of its own is the ROM's: past the
        // fifth star the order repeats a star id whose dormant copy is spent)
        if ((i.spilled || i.uid) && !star_by_uid(i.uid)) ++missing;
    }
    static int cur[5], held = 0, shown[5] = {-1, -1, -1, -1, -1};
    const int now[5] = { live, orphans, free_known, missing, sum };
    if (std::memcmp(now, cur, sizeof now) != 0) {
        std::memcpy(cur, now, sizeof now);
        held = 0;
        return;
    }
    if (++held != 20 || std::memcmp(now, shown, sizeof now) == 0) return;
    std::memcpy(shown, now, sizeof now);
    std::fprintf(stderr, "[vsarena-ref] CENSUS seat=%d f=%u stars_live=%d orphans=%d "
                 "free_instances=%d missing=%d ledger_sum=%d ledger=%d,%d,%d,%d hud=%d,%d,%d,%d\n",
                 g.seat, my_frame_now(), live, orphans, free_known, missing, sum,
                 g.ledger[0], g.ledger[1], g.ledger[2], g.ledger[3],
                 data_0209f310[0], data_0209f310[1], data_0209f310[2], data_0209f310[3]);
}

void report_maybe() {
    const unsigned now = now_ms();
    if (!g.last_report) g.last_report = now;
    if (now - g.last_report >= (unsigned)kReportMs) {
        g.last_report = now;
        vs_ref_report("periodic");
    }
}

}  // namespace

// ===========================================================================
// PUBLIC
// ===========================================================================
bool vs_ref_active() {
    if (g.installed) return g.active;
    if (!vs_arena_active()) return false;
    const CommsHubStats h = comms_hub_stats();
    if (!h.installed) return false;
    g.installed = true;
    g.active = h.mode == 1;           // relay mode only: the mesh has no referee
    g.log = env_on("SM64DS_ARENA_REFLOG");
    g.caps_off = env_on("SM64DS_ARENA_NOCAPS");
    // LUIGI INFECTION rides the same switch Classic reads (the lobby's plan exports it)
    g.luigi = g.active && port_luigi_enabled() != 0;
    for (int k = 0; k < 16; ++k) g.ledger[k] = 0;
    std::fprintf(stderr, "[vsarena] referee client %s (hub mode %d)\n",
                 g.active ? "ACTIVE: the referee in the relay decides stars, hits, "
                            "the clock and the winner"
                          : "inert: no relay, no referee (loopback mesh)", h.mode);
    if (g.luigi)
        std::fprintf(stderr, "[vsarena] Luigi Infection on the referee: READY asks for mode 2; the "
                     "referee picks the Luigis, the game swaps the bodies on its INFECTED events\n");
    return g.active;
}

void vs_ref_pre_walk(int frame) {
    (void)frame;
    if (!vs_ref_active() || data_0209f2d8 != 1) return;
    const ArenaStats s = vs_arena_stats();
    if (s.seat < 0) return;
    g.seat = s.seat;
    g.bodies = s.bodies;
    ++g.vs_frames;
    init_once();
    process_synr();
    syno_recv();
    pump_claims();
    maybe_send_ready();
    if (g.over) return;
    // the ledger and the progress byte are the referee's
    if (g.ledger_live) {
        for (int k = 0; k < 16; ++k)
            if (g.started || g.ledger[k] == 0) data_0209f310[k] = g.ledger[k];
    }
    if (g.started) data_0209f208 = g.progress;
    for (int k = 1; k < kMaxInst; ++k)
        if (g.inst[k].used && g.inst[k].state == 1) ensure_star_uid(g.inst[k]);
    if (g.started) {
        neutralise_foreign_star_touches();
        hold_step();
        marker_pass();
        puppet_stars();
        shown_stars();
        cap_probe();
        marker_slots_pass();
        caps_pass();
        cap_watch();
        hit_detect("pre-walk");
        rig_step();
    }
}

// VS9 (V3): the UNCHAINED_CHOMP of arena 1 carries one of the match's stars. In a VS match the ROM
// (src/game/actors/d_a_wanwan2.cpp func_ov100_021435e8) spawns a PowerStar (id 0x30 | param) and its
// StarMarker at the chain's last link and then, every frame until the star is collected, writes the
// star's position over the link's. The marker stays where it was spawned; the star rides the chain
// at 100+ units a second across the arena, and the chomp chases the NEAREST player of each console,
// so it is a different place on every screen. Nobody could reach it, the referee's range check
// could not agree with any screen, and the star the order wakes third sat there for the whole match.
// The star is an ordinary arena star: put it on its marker (the way every other level star is
// placed) and latch the chomp's carry (mChainEndDone, the state its own code reaches when the star
// is collected) so the chain never moves it again. SM64DS_CHOMP_STAR_PIN=1 leaves the ROM's carry
// alone (the control row).
void chomp_star_unpin() {
    static const bool keep = env_on("SM64DS_CHOMP_STAR_PIN");
    if (keep) return;
    each_class(337, [&](char *c) {                      // UNCHAINED_CHOMP
        if (*u8p(c, 0x6c8) != 0) return;               // mChainEndDone
        const unsigned uid = u32r(c, 0x6d0);           // mChainEndActorID
        if (!uid) return;
        char *s = star_by_uid(uid);
        if (!s) return;
        char *m = marker_by_id(*u8p(s, 0x49d));
        if (!m) return;                                  // (the marker is spawned a line after the star)
        s32r(s, 0x5c) = s32r(m, 0x5c);
        s32r(s, 0x60) = s32r(m, 0x60);
        s32r(s, 0x64) = s32r(m, 0x64);
        *u8p(c, 0x6c8) = 1;
        u32r(c, 0x6d0) = 0;
        std::fprintf(stderr, "[vsarena-ref] chomp star (id %d) put on its marker at (%d,%d,%d); the chain no longer carries it\n",
                     (int)*u8p(s, 0x49d), s32r(s, 0x5c) >> 12, s32r(s, 0x60) >> 12, s32r(s, 0x64) >> 12);
    });
}

// VS9 lab: SM64DS_ARENA_STARLOG=1 prints, every 30 frames, each PowerStar actor (uid, star
// id, ROM state, position, touch words), each marker (id, live, glass bit, position) and
// the UNCHAINED_CHOMP (position, chain-end latch), with the instance that owns each star.
void star_dump(unsigned frame) {
    static const bool on = env_on("SM64DS_ARENA_STARLOG");
    if (!on || frame % 30 != 0) return;
    each_class(kClassStar, [&](char *a) {
        const Inst *i = inst_by_uid(actor_uid(a));
        std::fprintf(stderr, "[starlog] f=%u star uid=%u id=%d st=%d pos=(%d,%d,%d) t130=%x inst=%d\n",
                     frame, actor_uid(a), (int)*u8p(a, 0x49d), s32r(a, 0x440), s32r(a, 0x5c) >> 12,
                     s32r(a, 0x60) >> 12, s32r(a, 0x64) >> 12, u32r(a, 0x130),
                     i ? (int)(i - g.inst) : -1);
    });
    each_class(kClassMarker, [&](char *a) {
        std::fprintf(stderr, "[starlog] f=%u marker id=%d live=%d glass=%02x pos=(%d,%d,%d)\n", frame,
                     (int)*u8p(a, 0x1d9), (int)*u8p(a, 0x1d8), (unsigned)*u8p(a, 0x1db),
                     s32r(a, 0x5c) >> 12, s32r(a, 0x60) >> 12, s32r(a, 0x64) >> 12);
    });
    each_class(337, [&](char *a) {
        std::fprintf(stderr, "[starlog] f=%u chomp pos=(%d,%d,%d) endactor=%u done=%d\n", frame,
                     s32r(a, 0x5c) >> 12, s32r(a, 0x60) >> 12, s32r(a, 0x64) >> 12,
                     u32r(a, 0x6d0), (int)*u8p(a, 0x6c8));
    });
    for (int k = 1; k < kMaxInst; ++k) {
        const Inst &i = g.inst[k];
        if (i.used)
            std::fprintf(stderr, "[starlog] f=%u inst=%d star_id=%d state=%d uid=%u claimed=%d holding=%d spilled=%d\n",
                         frame, k, (int)i.star_id, (int)i.state, i.uid, (int)i.claimed, (int)i.holding, (int)i.spilled);
    }
}

void vs_ref_post_walk(int frame) {
    (void)frame;
    if (!vs_ref_active() || data_0209f2d8 != 1 || !g.inited) return;
    chomp_star_unpin();
    if (g.started && !g.over) {
        if (g.ledger_live)
            for (int k = 0; k < 16; ++k) data_0209f310[k] = g.ledger[k];
        hit_detect("post-walk");
        grab_detect();
        caps_detect();
        cap_tongue_watch();
        syno_send();
        my_state_edges();
        star_census();
        star_dump(my_frame_now());
    }
    g.last_post_ms = now_ms();
    report_maybe();
}

bool vs_ref_countdown(int *count, int *f304, int *beep, int *music) {
    *beep = 0;
    *music = 0;
    if (!vs_ref_active() || data_0209f2d8 != 1 || !g.inited) return false;
    if (g.over) return false;
    if (!g.started) {
        *count = 3;
        *f304 = 0x2a;
        return true;
    }
    unsigned r;
    if (!ref_now(&r)) {
        *count = 3;
        *f304 = 0x2a;
        return true;
    }
    const int left = (int)(g.t0 - r);             // ms to the match's frame 0
    int c;
    if (left > 2000) c = 3;
    else if (left > 1000) c = 2;
    else if (left > 0) c = 1;
    else c = 0;
    if (c != g.cd_count) {
        if (c == 0) { *beep = 0x2a; *music = 0x4d; }
        else *beep = 0x2b;
        g.cd_count = c;
    }
    *count = c;
    *f304 = c != 0 ? 0x2a : (left > -1000 ? 0x28 : 0);
    if (c == 0 && left <= -1000) return false;     // the START glyph is done
    return true;
}

void vs_ref_note_local_timeup(int frame) {
    (void)frame;
    if (!g.active || g.local_timeup) return;
    g.local_timeup = true;
    g.local_timeup_ms = now_ms();
}

bool vs_ref_match_over(VsRefOver *out) {
    if (!g.active) return false;
    if (!g.over && g.local_timeup &&
        now_ms() - g.local_timeup_ms >= (unsigned)kLocalTimeupGraceMs) {
        g.over = true;
        g.over_v = VsRefOver();
        g.over_v.reason = OVER_TIME_UP;
        g.over_v.roster_mask = g.roster_now ? g.roster_now : g.roster;
        g.over_v.seen_mask = (int)(g.ever_roster | g.roster);
        std::memcpy(g.over_v.scores, g.ledger, 16);
        int top = -128;
        for (int k = 0; k < 16; ++k)
            if ((g.roster >> k & 1u) && g.ledger[k] > top) top = g.ledger[k];
        for (int k = 0; k < 16; ++k)
            if ((g.roster >> k & 1u) && g.ledger[k] == top) {
                g.over_v.winners_mask |= 1 << k;
                ++g.over_v.winners;
            }
        g.over_v.local_timeout = true;
        std::fprintf(stderr, "[vsarena] connection lost: no MATCH OVER from the "
                     "referee %d ms after the clock ran out; ending on my last "
                     "ledger\n", kLocalTimeupGraceMs);
    }
    if (!g.over) return false;
    if (out) *out = g.over_v;
    return true;
}

bool vs_ref_ledger(signed char out[16]) {
    if (!g.active || !g.ledger_live) return false;
    std::memcpy(out, g.ledger, 16);
    return true;
}

unsigned short vs_ref_claim(unsigned char kind, unsigned char target,
                            unsigned short aux, unsigned ref_frame,
                            unsigned stamp, unsigned my_frame) {
    if (!g.active) return 0;
    if (!stamp) {
        if (!stamp_for(g.last_post_ms ? g.last_post_ms : now_ms(), &stamp)) return 0;
    }
    PClaim *c = make_claim(kind, target, aux, stamp,
                          my_frame ? my_frame : my_frame_now(), ref_frame);
    return c ? c->cid : 0;
}

int vs_ref_claim_result(unsigned short cid, int *reason, int *winner) {
    PClaim *c = claim_by_id(cid);
    if (!c) return -2;
    if (reason) *reason = c->reason;
    if (winner) *winner = c->winner;
    return c->result;
}

void vs_ref_set_event_sink(VsRefEventSink sink) { g.sink = sink; }

bool vs_ref_stamp_for(unsigned local_ms, unsigned *stamp) {
    return g.active && stamp_for(local_ms, stamp);
}

VsRefStats vs_ref_stats() {
    VsRefStats s = g.st;
    s.active = g.active;
    s.started = g.started;
    s.over = g.over;
    s.applied_seq = g.applied;
    return s;
}

void vs_ref_report(const char *tag) {
    if (!g.active) return;
    std::fprintf(stderr, "[vsarena-ref] %s seat=%d started=%d over=%d seq=%u "
                 "frames=%llu events=%llu dup=%llu gaps=%llu claims=%llu resent=%llu "
                 "verdicts=%llu denied=%llu taken=%llu hits=%llu/%llu/%llu spills=%llu "
                 "syno=%llu/%llu bad=%llu/%llu/%llu ledger=%d,%d,%d,%d progress=%u gated=%llu/%llu\n", tag ? tag : "",
                 g.seat, g.started, g.over, (unsigned)g.applied, g.st.frames_rx,
                 g.st.events_applied, g.st.events_dup, g.st.gaps, g.st.claims_sent,
                 g.st.claims_resent, g.st.verdicts, g.st.denied, g.st.stars_taken,
                 g.st.hits_claimed, g.st.hits_landed, g.st.hits_denied, g.st.spills,
                 g.st.syno_tx, g.st.syno_rx, g.st.frames_bad, g.st.events_bad,
                 g.st.syno_bad, g.ledger[0], g.ledger[1], g.ledger[2],
                 g.ledger[3], (unsigned)g.progress, g.st.touches_gated, g.st.hits_gated,
                 g.st.stars_shown, g.st.shown_frames);
    std::fprintf(stderr, "[vsarena-ref] %s caps: claims=%llu lost=%llu taken=%llu removed=%llu "
                 "dropped=%llu drop_claims=%llu copies=%llu respawns=%llu puppet_touches=%llu\n",
                 tag ? tag : "", g.st.cap_claims, g.st.cap_lost, g.st.cap_taken, g.st.cap_removed,
                 g.st.cap_dropped, g.st.cap_drop_claims, g.st.cap_copies, g.st.cap_respawns,
                 g.st.cap_puppet_touches);
}


// SEAMVS1: the referee client of a match that ended in place, back to a client that never saw a match, for the next match
// handed to this process. The four ROM seams init_once wrapped stay wrapped (their original words are kept, so init_once's
// guards do not wrap them twice), and so does the event sink; the order buffer is filled again by the next init_once.
void vs_ref_match_reset() {
    const unsigned s5 = g.star5_orig, hu = g.hurt_orig, hw = g.hurtwater_orig, dh = g.deadhit_orig;
    const VsRefEventSink sink = g.sink;
    g = Ref();
    g.star5_orig = s5;
    g.hurt_orig = hu;
    g.hurtwater_orig = hw;
    g.deadhit_orig = dh;
    g.sink = sink;
    for (CapSeen &c : g_cap_seen) c = CapSeen();
    g_tc = TongueCap();
}

}  // namespace port
