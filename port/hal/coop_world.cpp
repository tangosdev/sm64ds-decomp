// COOP WORLD CORE (phase 1). Run mp1 wave 56, lane COOPWORLD1. See coop_world.h.
//
// WHAT IT KEEPS EQUAL. The death table (data_0209f4f8): three level parts of
// sixteen words, one bit per placed object that died or was collected, indexed
// by the object's spawn-order slot (actor +0x0ce). The level loader hands every
// placed record the same running slot on every console for the same (level,
// act), and dActor_c::BeforeInitResources makes any actor whose bit is up delete
// itself instead of initialising. So:
//   * a LATE JOINER that holds the group's bits BEFORE its objects spawn never
//     sees the dead things (no spawn logic of ours);
//   * a LIVE bit that arrives is set in the table and any live actor in that
//     slot is removed (reconcile, below), so a kill or pickup on one screen
//     happens on every screen.
// Runtime spawns carry slot -1 and are not covered here (phase 2 / 3).
//
// PORT_HOST_ABI. Reads the ROM's globals and the live-actor walk; nothing in
// src/ changes.

#include "coop_world.h"
#include "coop_boot.h"
#include "coop_groups.h"
#include "coop_wire.h"
#include "comms_hub.h"
#include "hub_presence.h"

#include <windows.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern short data_0209f358[];
extern signed char data_0209f2f8;
extern int data_0209f220[];
extern int data_0209f4f8[];
signed char GetLevelPart(int idx);
void *_ZN8dActor_c4NextEPKS_(const void *prev);
void GiveCoins(int idx, int amount);
void LoadLevelNoReturn(unsigned level, unsigned entrance, unsigned star, unsigned unk);
void _ZN8dActor_c24KillAndTrackInDeathTableEv(void *self);
void *_ZN8dActor_c10FindWithIDEj(unsigned id);
void *port_debug_spawn(unsigned id, unsigned param);
void _ZN7fBase_c18MarkForDestructionEv(void *self);
void *_ZN11dCapEnemy_c15RespawnIfHasCapEv(void *self);
void func_ov084_02129498(void *goomba);
}

namespace port {
void coop_api_test_register();
namespace {

enum : unsigned {
    kHelloMs      = 1000,
    kFullMs       = 1000,
    kEnterWaitMs  = 400,     // no answer in this long = first in
    kEnterResend  = 120,
    kSnapResend   = 250,
    kSnapGiveUpMs = 5000,
    kPresenceMs   = 4000,
    kGraceMs      = 8000,    // a member whose hello or deltas for MY group still arrive stays a member this long
    kReconcileEvery = 20,
};
enum : int { kOutMax = 256, kSnapChunks = 2 };

long long wall_ms() {   // proof logs: one clock for every console on this machine
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    return (long long)((((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000ull);
}
struct Out { uint16_t slot; uint8_t sent; unsigned next; };
enum : int { kOwnerTries = 3, kGuestTries = 40, kStarSeen = 16 };
struct Job { bool on; uint32_t nonce; CoopKey key; unsigned start, last; };
struct Reg { int lo, hi; CoopSubFn fn; void *user; };
enum : int { kMaxRegs = 8, kMaxCbs = 8 };
// STEP-0 callback registries (the phase-2 lanes hang off these; no walk_window lines)
void (*g_cb_frame[kMaxCbs])(unsigned); int g_n_frame = 0;
void (*g_cb_latched[kMaxCbs])(int, int); int g_n_latched = 0;
void (*g_cb_estab[kMaxCbs])(int, int); int g_n_estab = 0;
void (*g_cb_snapreq[kMaxCbs])(int, unsigned); int g_n_snapreq = 0;
void (*g_cb_gone[kMaxCbs])(int); int g_n_gone = 0;
unsigned g_prev_members = 0;

CoopGroups g_grp;
CoopWorldStats g_st = {};
bool g_env_read = false, g_on = false, g_log = false, g_sync = true;
bool g_handlers = false;
// proof-only control: SM64DS_COOP_LEGACY=star,trust,retry,clear,flush,grace switches the matching follow-up fix off (the 1451d2e257 behaviour)
bool g_leg_star = false, g_leg_trust = false, g_leg_retry = false, g_leg_clear = false, g_leg_flush = false, g_leg_grace = false;
struct ReviveKnob { unsigned frame; int slot; int cls; bool done; };
ReviveKnob g_revive = {0, -1, 0, true};
int  g_my = -1;
unsigned g_frame = 0, g_est_frames = 0;
unsigned g_kframe = 0;             // proof-knob clock: frames that really ticked (released, no barrier, no opening playing)

CoopKey  g_key;
CoopKey  g_rejected;           // a level the death table has no part for: never retried
int      g_part = -1;
unsigned g_prev[kCoopWords];       // shadow of my part: what I have already announced or taken
unsigned g_sticky[kCoopWords];     // stars collected here: part of what I hand out, whatever the ROM clears later
void    *g_star_seen[kStarSeen];   // star actors whose collect I already announced
int      g_star_n = 0;
unsigned g_enter_nonce = 0;        // the nonce I asked a snapshot for (0 = never asked)
unsigned g_remote[kCoopWords];     // bits that came from other consoles
bool     g_recon_dirty = false;
unsigned g_last_bits_hash = 0;

Out      g_out[kOutMax];
int      g_out_n = 0;

unsigned g_nonce = 0;              // my enter counter
bool     g_snap_waiting = false;   // the latch hook is waiting for the answer
bool     g_snap_done = false;
uint8_t  g_snap_total = 0;
unsigned g_snap_mask = 0;
unsigned char g_snap_buf[kCoopSnapChunkMax];
int      g_snap_owner = -1;
unsigned g_ack_last = 0;
Job      g_job[CoopGroups::kSeats];
unsigned g_heard[CoopGroups::kSeats];   // when a seat last sent me something for my own group (0 = never)

unsigned g_hello_t = 0, g_full_t = 0;
unsigned g_seq_ctl = 0, g_seq_world = 0;
Reg      g_reg_ctl[kMaxRegs], g_reg_world[kMaxRegs];
int      g_nreg_ctl = 0, g_nreg_world = 0;

// proof knobs
struct KillKnob { unsigned frame; int id; int n; bool done; };
KillKnob g_kill[8]; int g_nkill = 0;
unsigned g_star_frame = 0, g_star_next = 0; int g_star_tries = 0; unsigned long long g_star_base = 0;
bool g_star_done = true, g_star_spawned = false;   // SM64DS_COOP_STAR=<est frame>: the local player touches a PowerStar
unsigned g_goto_frame = 0; int g_goto_id = -1; bool g_goto_done = false; int g_goto_tries = 0; int g_goto_dy = 0;
unsigned g_place_frame = 0; int g_place_xyz[3] = {0, 0, 0}; bool g_place_on = false;   // SM64DS_COOP_PLACE (run after093, lane SEESAW1)
int g_place_level = -1;   // SM64DS_COOP_PLACE ...:L, only in level L (run after093, lane WHOMPBACK1)
int g_trace_id = -1; unsigned g_trace_every = 5; int g_trace_off = -1;   // SM64DS_COOP_TRACE (run after093, lane SEESAW1)
struct WarpKnob { unsigned frame; int level; int act; bool done; };
WarpKnob g_warp[4]; int g_nwarp = 0;
struct ForgeKnob { unsigned frame; char name[16]; bool done; };
ForgeKnob g_forge[12]; int g_nforge = 0;

void read_env() {
    if (g_env_read) return;
    g_env_read = true;
    const char *e = std::getenv("SM64DS_COOP");
    g_on = e && *e && *e != '0';
    const char *l = std::getenv("SM64DS_COOP_LOG");
    g_log = l && *l && *l != '0';
    const char *s = std::getenv("SM64DS_COOP_SYNC");
    g_sync = !(s && *s == '0');
    if (const char *k = std::getenv("SM64DS_COOP_KILL")) {
        const char *p = k;
        while (*p && g_nkill < 8) {
            unsigned f = 0; int id = 0, n = 0, used = 0;
            if (std::sscanf(p, "%u:%i:%i%n", &f, &id, &n, &used) >= 3 && used > 0) {
                g_kill[g_nkill++] = {f, id, n, false};
                p += used;
            }
            while (*p == ';' || *p == ',' || *p == ' ') ++p;
            if (used <= 0) break;
        }
    }
    if (const char *lg = std::getenv("SM64DS_COOP_LEGACY")) {
        g_leg_star = std::strstr(lg, "star") != nullptr;
        g_leg_trust = std::strstr(lg, "trust") != nullptr;
        g_leg_retry = std::strstr(lg, "retry") != nullptr;
        g_leg_clear = std::strstr(lg, "clear") != nullptr;
        g_leg_flush = std::strstr(lg, "flush") != nullptr;
        g_leg_grace = std::strstr(lg, "grace") != nullptr;
    }
    if (const char *rv = std::getenv("SM64DS_COOP_REVIVE")) {
        unsigned f = 0; int sl = 0, cl = 0;
        if (std::sscanf(rv, "%u:%i:%i", &f, &sl, &cl) == 3) g_revive = {f, sl, cl, false};
    }
    if (const char *w = std::getenv("SM64DS_COOP_WARP")) {
        // F:level:act[;F:level:act]  (F = ticked frames since the boot release / the end of a new file's opening)
        const char *p = w;
        while (*p && g_nwarp < 4) {
            unsigned f = 0; int lv = 0, ac = 1, used = 0;
            if (std::sscanf(p, "%u:%i:%i%n", &f, &lv, &ac, &used) < 3 || used <= 0) break;
            g_warp[g_nwarp++] = {f, lv, ac, false};
            p += used;
            while (*p == ';' || *p == ',' || *p == ' ') ++p;
        }
    }
    if (const char *st = std::getenv("SM64DS_COOP_STAR")) {
        unsigned f = 0;
        if (std::sscanf(st, "%u", &f) == 1 && f > 0) { g_star_frame = f; g_star_done = false; }
    }
    if (const char *g = std::getenv("SM64DS_COOP_GOTO")) {
        unsigned f = 0; int id = 0;
        int dy = 0;   // F:class[:dy]  dy units above the actor: the player drops onto it (a stomp)
        if (std::sscanf(g, "%u:%i:%i", &f, &id, &dy) >= 2) { g_goto_frame = f; g_goto_id = id; g_goto_dy = dy; }
    }
    if (const char *pl = std::getenv("SM64DS_COOP_PLACE")) {
        // F:x,y,z (world units): put the local player at that point ONCE, F frames after the group state is established in the
        // level, so a coop proof row can start a real scripted walk from solid ground. Unlike SM64DS_COOP_GOTO it never repeats.
        unsigned f = 0; int x = 0, y = 0, z = 0;
        int lv = -1;
        if (std::sscanf(pl, "%u:%i,%i,%i:%i", &f, &x, &y, &z, &lv) >= 4) { g_place_frame = f; g_place_xyz[0] = x; g_place_xyz[1] = y; g_place_xyz[2] = z; g_place_on = true; g_place_level = lv; }
    }
    if (const char *tr = std::getenv("SM64DS_COOP_TRACE")) {
        // ID[:EVERY[:OFF]]: every EVERY established frames print the local player's place and air flag, and the pose of each live
        // actor of class ID (plus the s16 at OFF when given), with the hub clock, so two consoles' lines can be lined up.
        unsigned ev = 5; int id = -1, off = -1;
        const int n = std::sscanf(tr, "%i:%u:%i", &id, &ev, &off);
        if (n >= 1) { g_trace_id = id; g_trace_every = (n >= 2 && ev) ? ev : 5u; g_trace_off = n >= 3 ? off : -1; }
    }
    if (const char *f = std::getenv("SM64DS_COOP_FORGE")) {
        // F:name[,F:name...]
        const char *p = f;
        while (*p && g_nforge < 12) {
            unsigned fr = 0; char nm[16] = {}; int used = 0;
            if (std::sscanf(p, "%u:%15[a-z]%n", &fr, nm, &used) < 2 || used <= 0) break;
            g_forge[g_nforge].frame = fr;
            std::memcpy(g_forge[g_nforge].name, nm, sizeof nm);
            g_forge[g_nforge].done = false;
            ++g_nforge;
            p += used;
            while (*p == ',' || *p == ';' || *p == ' ') ++p;
        }
    }
}

unsigned *tbl(int part) { return reinterpret_cast<unsigned *>(data_0209f4f8) + part * kCoopWords; }

int part_of_level(int level) {
    if (level < 0 || level > 127) return -1;
    const int p = (int)GetLevelPart(level);
    return (p >= 0 && p <= 2) ? p : -1;
}

int popcnt(unsigned v) { int n = 0; for (; v; v &= v - 1) ++n; return n; }

void stats_line();

bool is_player_ptr(const void *a) {
    for (int i = 0; i < 16; ++i)
        if (data_0209f394[i] == a) return true;
    return false;
}

// ---------------------------------------------------------------------------
// Sending.
// ---------------------------------------------------------------------------
bool send_kind(uint32_t tag, int sub, const void *payload, int plen) {
    unsigned char buf[kHubCoopMaxDatagram];
    unsigned &seq = (tag == kHubTagCoopWorld) ? g_seq_world : g_seq_ctl;
    const int n = hub_coop_encode(tag, sub, ++seq, payload, plen, buf, (int)sizeof buf);
    if (n <= 0) return false;
    return comms_hub_send(buf, n);
}

CoopKeyWire wire_key(const CoopKey &k) {
    CoopKeyWire w; w.level = (uint16_t)k.level; w.act = (uint8_t)k.act; w.reserved = 0;
    return w;
}
bool key_from_wire(const CoopKeyWire &w, CoopKey *k) {
    if (!coop_key_ok(w)) return false;
    *k = CoopKey(w.level, w.act);
    return true;
}

void send_enter() {
    CoopEnterWire w; w.key = wire_key(g_key); w.nonce = g_nonce;
    g_enter_nonce = g_nonce;
    if (send_kind(kHubTagCoopCtl, kCtlLevelEnter, &w, sizeof w)) ++g_st.sent_enter;
}
void send_hello(unsigned now) {
    CoopHelloWire w; std::memset(&w, 0, sizeof w);
    w.key = wire_key(g_key);
    w.established = g_grp.established() ? 1 : 0;
    w.owner = g_grp.owner() >= 0 ? (uint8_t)g_grp.owner() : (uint8_t)kCoopNoSeat;
    w.rank = (uint8_t)(std::min)(16, g_grp.established_count() + 1);
    if (send_kind(kHubTagCoopCtl, kCtlGroupHello, &w, sizeof w)) { ++g_st.sent_hello; g_hello_t = now; }
}
void send_owner_change() {
    CoopOwnerWire w; std::memset(&w, 0, sizeof w);
    w.key = wire_key(g_key); w.owner = (uint8_t)g_my;
    if (send_kind(kHubTagCoopCtl, kCtlOwnerChange, &w, sizeof w)) ++g_st.sent_owner;
}
void send_ack(int owner, uint32_t nonce) {
    CoopAckWire w; std::memset(&w, 0, sizeof w);
    w.key = wire_key(g_key); w.nonce = nonce; w.owner = (uint8_t)owner;
    if (send_kind(kHubTagCoopCtl, kCtlSnapAck, &w, sizeof w)) ++g_st.sent_ack;
}
void send_full(unsigned now) {
    if (g_part < 0) return;
    CoopFullWire w; w.key = wire_key(g_key);
    for (int i = 0; i < kCoopWords; ++i) w.words[i] = tbl(g_part)[i] | g_sticky[i];
    if (send_kind(kHubTagCoopWorld, kWorldDeathFull, &w, sizeof w)) { ++g_st.sent_full; g_full_t = now; }
}

void serve_snapshot(int seat, const Job &j) {
    if (g_part < 0) return;
    unsigned char buf[sizeof(CoopSnapWire) + kCoopSnapChunkMax];
    unsigned t[kCoopWords];
    for (int i = 0; i < kCoopWords; ++i) t[i] = tbl(g_part)[i] | g_sticky[i];
    const int clen = kCoopSnapChunkMax / kSnapChunks;
    for (int n = 0; n < kSnapChunks; ++n) {
        CoopSnapWire h; std::memset(&h, 0, sizeof h);
        h.target = (uint8_t)seat; h.n = (uint8_t)n; h.total = (uint8_t)kSnapChunks;
        h.key = wire_key(g_key); h.nonce = j.nonce;
        h.level_clock = g_est_frames; h.rng_seed = 0;   // level_clock: frames since this group state was established here
        std::memcpy(buf, &h, sizeof h);
        std::memcpy(buf + sizeof h, reinterpret_cast<const unsigned char *>(t) + n * clen, (size_t)clen);
        if (send_kind(kHubTagCoopCtl, kCtlSnapPart, buf, (int)sizeof h + clen)) ++g_st.sent_snap;
    }
}

// ---------------------------------------------------------------------------
// Applying remote bits.
// ---------------------------------------------------------------------------
// TOTALS9 (field item 11): one shared coin count for the players of a stage. A placed coin that a friend collected is credited to MY
// counter (the slot the HUD draws) at the moment its death bit arrives, while the actor is still standing. Value by the ROM's own coin
// type (+0x3a0: 1 red = 2, 2 blue = 5, else yellow = 1; func_ov002_020b16c4 / 020b1674 / 020b1884). Quiet: no sound, no heal, and no
// call of the 100-coin star check (func_ov002_020b18f0): the console whose OWN pickup crosses 100 spawns that star and announces it
// (coop_spawn.cpp), everyone else gets the announced copy. Coins that fall out of enemies have slot -1 and are never in a death bit.
// SM64DS_COOP_COINS=0 turns it off (must-fail control).
bool coins_off() { static const bool off = [] { const char *e = std::getenv("SM64DS_COOP_COINS"); return e && *e == '0'; }(); return off; }
// SM64DS_COOP_COINS=native: the must-fail for the 100-coin rule, credits through the ROM's GiveCoins (which spawns the star at 100 on THIS
// console as well as on the one whose pickup crossed it).
bool coins_native() { static const bool n = [] { const char *e = std::getenv("SM64DS_COOP_COINS"); return e && !std::strcmp(e, "native"); }(); return n; }
unsigned long long g_coin_credits = 0, g_coin_total = 0;
void credit_remote_coins(int w, unsigned fresh) {
    if (coins_off() || !g_sync) return;
    const int me = (int)data_0209f250;
    if (me < 0 || me > 3) return;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        const unsigned cls = *(const unsigned short *)(a + 0x0c);
        if (cls < 288 || cls > 290) continue;
        const short slot = *(const short *)(a + 0xce);
        if (slot < 0 || slot >= kCoopSlots || (slot >> 5) != w || !((fresh >> (slot & 31)) & 1u)) continue;
        if (*(const unsigned char *)(a + 0x0f) || *(const unsigned char *)(a + 0x0e) == 2) continue;      // already going
        const int ty = *(const int *)(a + 0x3a0);
        const int v = ty == 1 ? 2 : ty == 2 ? 5 : 1;
        int n = (int)data_0209f358[me] + v;
        if (n > 0xff) n = 0xff;
        if (coins_native()) { GiveCoins(me, v); n = (int)data_0209f358[me]; }
        else data_0209f358[me] = (short)n;
        ++g_coin_credits; g_coin_total += (unsigned)v;
        if (g_log)
            std::fprintf(stderr, "[coop] coin credit cls=%u slot=%d type=%d +%d count=%d wall=%lld\n", cls, (int)slot, ty, v, n, wall_ms());
    }
}

void merge_remote_word(int w, unsigned bits) {
    unsigned *t = tbl(g_part);
    const unsigned fresh = bits & ~t[w];
    if (!fresh) return;
    t[w] |= fresh;
    credit_remote_coins(w, fresh);
    g_prev[w] |= fresh;          // not mine: never announced back
    g_remote[w] |= fresh;
    g_st.bits_applied += (unsigned)popcnt(fresh);
    g_recon_dirty = true;
}

// The owner's full bitmap shows a bit: a guest's pending announcement of it is done.
void ack_out(const unsigned *words) {
    for (int i = 0; i < g_out_n; ++i)
        if ((words[g_out[i].slot >> 5] >> (g_out[i].slot & 31)) & 1u) g_out[i].sent = 255;
}

bool for_me(const CoopKey &k) {
    return g_part >= 0 && g_key.valid() && k == g_key;
}

// Remove the live actors whose slot a remote bit names. The ROM's own
// kill-and-track (bit already up, actor marked for destruction): no effects
// play on this console, no counters move.
int reconcile() {
    int kills = 0;
    bool any = false;
    for (int w = 0; w < kCoopWords; ++w) any = any || g_remote[w];
    if (!any) return 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a;
         a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        const short slot = *(const short *)(a + 0xce);
        if (slot < 0 || slot >= kCoopSlots) continue;
        if (!((g_remote[slot >> 5] >> (slot & 31)) & 1u)) continue;
        if (*(const unsigned char *)(a + 0x0f) || *(const unsigned char *)(a + 0x0e) == 2) continue;
        if (is_player_ptr(a)) continue;
        _ZN8dActor_c24KillAndTrackInDeathTableEv(a);
        ++kills;
        if (g_log)
            std::fprintf(stderr, "[coop] reconcile kill slot=%d actor=%u wall=%lld\n", (int)slot,
                         (unsigned)*(const unsigned short *)(a + 0x0c), wall_ms());
    }
    g_st.reconcile_kills += (unsigned)kills;
    return kills;
}

// ---------------------------------------------------------------------------
// Swallows (TONGUE3). Yoshi's swallow is Player.cpp func_ov002_020d7430: mObjInMouth->OnTurnIntoEgg(player). The
// default body (dActor_c::OnTurnIntoEgg, d_actor.cpp) and an ordinary Goomba (daKrb_c::OnTurnIntoEgg ->
// func_ov084_02129498, d_a_krb.cpp) KillAndTrackInDeathTable: that bit goes out as a death delta like a stomp and
// every console reconciles its copy away. A Goomba with a cap (mCapId < 6) is removed with MarkForDestruction alone,
// after RespawnIfHasCap left a dormant replacement at its home: no bit, so nothing told the other consoles and their
// copy walked on (out of sight under TONGUEFIX2's park, then back on the ground). For a swallow that left no bit the
// eater's console says so (kWorldSwallowGone) and every other console does to its copy what OnTurnIntoEgg did here,
// minus the eater's rewards. No death bit is set: the ROM's own respawn rule stays.
// ---------------------------------------------------------------------------
enum : int { kWorldSwallowGone = 0x03 };   // SYNW: "my Yoshi swallowed this placed object and it left no death bit"
struct CoopSwallowWire {                   // 8 B
    CoopKeyWire key;
    uint16_t slot;                         // 0..kCoopSlots-1
    uint16_t cls;                          // actor class id, 0..1023
};
static_assert(sizeof(CoopSwallowWire) == 8, "swallow wire is 8 bytes");
enum : int { kKrbCapId = 0x113, kKrbReward = 0x464, kKrbHome = 0x41c, kMouthObj = 0x360, kEggs = 0x588, kSwOutMax = 8 };
struct SwOut { uint16_t slot, cls; uint8_t sent; unsigned next; };
SwOut g_sw_out[kSwOutMax]; int g_sw_n = 0;
char *g_mouth = nullptr; int g_mouth_cls = 0, g_mouth_slot = -1, g_mouth_eggs = 0;
const char *g_mouth_body = nullptr;      // the body whose mouth held g_mouth
unsigned g_sw_census_at = 0;               // proof log: a census this many est frames after a swallow

bool is_goomba(unsigned cls) { return cls >= 200 && cls <= 202; }
char *live_slot(unsigned cls, int slot) {
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if (*(const unsigned short *)(a + 0x0c) == cls && *(const short *)(a + 0xce) == slot) return a;
    return nullptr;
}
bool is_live(const char *p) {
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) if (a == p) return true;
    return false;
}
int eggs_of(const char *body) {
    int n = 0;
    if (const int *q = *(int *const *)(body + kEggs)) for (int i = 0; i < 5; ++i) if (q[i]) ++n;
    return n;
}
bool going(const char *a) { return *(const unsigned char *)(a + 0x0f) || *(const unsigned char *)(a + 0x0e) == 2; }

// Every est frame, after step 1 took this frame's bits: what is in my mouth, and how a mouthful left it.
void watch_swallow(unsigned now) {
    const int me = (int)data_0209f250;
    char *body = (me >= 0 && me < 16) ? (char *)data_0209f394[me] : nullptr;
    char *m = body ? *(char **)(body + kMouthObj) : nullptr;
    if (m == g_mouth && body == g_mouth_body) return;
    if (g_mouth && body != g_mouth_body) {
        // A new body (the level or my player was rebuilt): that mouthful went with the old one, nothing was swallowed.
        if (g_log) std::fprintf(stderr, "[coop] swallow watch: my body changed, class %d slot %d forgotten\n", g_mouth_cls, g_mouth_slot);
        g_mouth = nullptr; g_mouth_slot = -1;
    }
    if (g_mouth && g_mouth_slot >= 0) {
        // the mouthful left: spat or dropped = still standing; swallowed = marked for destruction (or already gone)
        const bool alive = is_live(g_mouth) && !going(g_mouth);
        const int slot = g_mouth_slot;
        const bool bit = (tbl(g_part)[slot >> 5] >> (slot & 31)) & 1u;
        const int eggs = body ? eggs_of(body) : 0;
        if (!alive && !bit) {
            if (g_sw_n < kSwOutMax) g_sw_out[g_sw_n++] = {(uint16_t)slot, (uint16_t)g_mouth_cls, 0, now};
        }
        if (g_log || !alive)
            std::fprintf(stderr, "[coop] swallow watch: class %d slot %d left my mouth %s (eggs %d -> %d)%s wall=%lld\n", g_mouth_cls, slot,
                         alive ? "standing (spat or dropped)" : "removed", g_mouth_eggs, eggs,
                         alive ? "" : bit ? ", death bit up: the death table carries it" : ", no death bit: told the group", wall_ms());
        if (!alive && g_log) g_sw_census_at = g_est_frames + 900;
    }
    g_mouth = nullptr; g_mouth_slot = -1;
    if (m && !is_player_ptr(m)) {
        const short slot = *(const short *)(m + 0xce);
        const unsigned cls = *(const unsigned short *)(m + 0x0c);
        if (slot >= 0 && slot < kCoopSlots && cls < 1024) {
            g_mouth = m; g_mouth_cls = (int)cls; g_mouth_slot = slot; g_mouth_eggs = eggs_of(body); g_mouth_body = body;
            if (g_log && is_goomba(cls))
                std::fprintf(stderr, "[coop] swallow watch: class %u slot %d in my mouth (cap %d reward %d)\n", cls, (int)slot,
                             *(const unsigned char *)(m + kKrbCapId) & 0xf, *(const unsigned char *)(m + kKrbReward));
        }
    }
}

// Three tries each (now, +120 ms, +360 ms), like the owner's deltas: a repeat finds no copy left and does nothing.
void send_swallows(unsigned now) {
    if (g_grp.others() == 0) { g_sw_n = 0; return; }
    int k = 0;
    for (int i = 0; i < g_sw_n; ++i) {
        SwOut &o = g_sw_out[i];
        if ((int)(now - o.next) >= 0) {
            CoopSwallowWire w; std::memset(&w, 0, sizeof w);
            w.key = wire_key(g_key); w.slot = o.slot; w.cls = o.cls;
            if (send_kind(kHubTagCoopWorld, kWorldSwallowGone, &w, (int)sizeof w)) {
                ++o.sent;
                o.next = now + (o.sent == 1 ? 120u : 240u);
            }
        }
        if (o.sent < 3) g_sw_out[k++] = o;
    }
    g_sw_n = k;
}

// A capped Goomba (or a silver-star-on-its-own-star one) that dies any other way (stomp, punch, fire, a fall into water)
// also ends in func_ov084_02129498, which for it is MarkForDestruction only (d_a_krb.cpp:470-476): no death bit, so the
// death table never carries it either. Watch those Goombas while my body stands in the level; one that goes away with no
// bit, not in my mouth and not removed here by a friend's message, is told to the group like a swallow. A level teardown
// marks dozens of placed actors in one frame: that frame (more than kCapMass slotted actors gone) sends nothing.
enum : int { kCapMax = 24, kCapMass = 4 };
struct CapW { char *a; uint16_t slot, cls; };
CapW g_cap[kCapMax]; int g_cap_n = 0; unsigned g_cap_frame = 0; int g_cap_slotted = -1;
uint16_t g_sw_applied[kSwOutMax]; int g_sw_applied_n = 0;   // slots a friend's message removed here: never echoed
bool capped_goomba(const char *a) {
    const unsigned cls = *(const unsigned short *)(a + 0x0c);
    if (!is_goomba(cls)) return false;
    return (*(const unsigned char *)(a + kKrbCapId) & 0xf) < 6 || *(const unsigned char *)(a + kKrbReward) == 2;
}
void cap_reset() { g_cap_n = 0; g_cap_slotted = -1; g_sw_applied_n = 0; }
void watch_capped(unsigned now) {
    CapW cur[kCapMax]; int n = 0, slotted = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        const short slot = *(const short *)(a + 0xce);
        if (slot < 0 || slot >= kCoopSlots || going(a) || is_player_ptr(a)) continue;
        ++slotted;
        if (n < kCapMax && capped_goomba(a)) cur[n++] = {a, (uint16_t)slot, *(const unsigned short *)(a + 0x0c)};
    }
    const bool fresh = g_cap_slotted >= 0 && g_cap_frame + 1 == g_est_frames;
    const bool mass = fresh && g_cap_slotted - slotted > kCapMass;
    for (int i = 0; fresh && !mass && i < g_cap_n; ++i) {
        const CapW &o = g_cap[i];
        bool still = false;
        for (int j = 0; j < n && !still; ++j) still = cur[j].a == o.a && cur[j].slot == o.slot;
        if (still) continue;
        const int slot = o.slot;
        const bool bit = (tbl(g_part)[slot >> 5] >> (slot & 31)) & 1u;
        bool skip = bit || slot == g_mouth_slot;
        for (int k = 0; k < g_sw_applied_n && !skip; ++k) skip = g_sw_applied[k] == o.slot;
        for (int k = 0; k < g_sw_n && !skip; ++k) skip = g_sw_out[k].slot == o.slot;
        if (!skip && g_sw_n < kSwOutMax) g_sw_out[g_sw_n++] = {o.slot, o.cls, 0, now};
        std::fprintf(stderr, "[coop] capped watch: class %u slot %d went away here %s wall=%lld\n", (unsigned)o.cls, slot,
                     bit ? "with a death bit (the death table carries it)" : skip ? "(swallow or a friend's message: not told again)"
                                                                           : "with no death bit: told the group",
                     wall_ms());
        if (!skip && g_log) g_sw_census_at = g_est_frames + 900;
    }
    if (mass && g_log) std::fprintf(stderr, "[coop] capped watch: %d slotted actors went in one frame (a teardown): nothing told\n",
                                    g_cap_slotted - slotted);
    std::memcpy(g_cap, cur, sizeof(CapW) * (size_t)n); g_cap_n = n;
    g_cap_slotted = slotted; g_cap_frame = g_est_frames;
}

// A friend's swallow left no death bit: my copy goes the way his did in OnTurnIntoEgg (d_a_krb.cpp:1635-1678), with
// the rewards left to his console. A Goomba with a cap, or one that gives the silver star only on its own star, goes
// home and RespawnIfHasCap leaves the dormant replacement there; func_ov084_02129498 removes it (MarkForDestruction for a
// capped one, KillAndTrackInDeathTable otherwise). Any other class: MarkForDestruction, what every OnTurnIntoEgg ends in.
void apply_swallow(int seat, int slot, unsigned cls) {
    char *a = live_slot(cls, slot);
    if (!a || going(a) || is_player_ptr(a)) {
        if (g_log) std::fprintf(stderr, "[coop] swallow from seat %d: class %u slot %d, no live copy here\n", seat, cls, slot);
        return;
    }
    if (is_goomba(cls)) {
        const int cap = *(const unsigned char *)(a + kKrbCapId) & 0xf;
        if (cap < 6 || *(const unsigned char *)(a + kKrbReward) == 2) {
            std::memcpy(a + 0x5c, a + kKrbHome, 12);
            _ZN11dCapEnemy_c15RespawnIfHasCapEv(a);
        }
        func_ov084_02129498(a);
    } else {
        _ZN7fBase_c18MarkForDestructionEv(a);
    }
    if (g_sw_applied_n < kSwOutMax) g_sw_applied[g_sw_applied_n++] = (uint16_t)slot;
    else { std::memmove(g_sw_applied, g_sw_applied + 1, sizeof g_sw_applied - sizeof g_sw_applied[0]); g_sw_applied[kSwOutMax - 1] = (uint16_t)slot; }
    std::fprintf(stderr, "[coop] swallow from seat %d: class %u slot %d removed here the way his console removed it (no death bit) wall=%lld\n",
                 seat, cls, slot, wall_ms());
    if (g_log) g_sw_census_at = g_est_frames + 900;
}

// ---------------------------------------------------------------------------
// Handlers.
// ---------------------------------------------------------------------------
bool seat_ok(int seat) {
    if (seat < 0 || seat >= kCoopSeats || seat == g_my) { ++g_st.drop_seat; return false; }
    return true;
}

void on_ctl(int seat, const unsigned char *d, int len, void *) {
    if (!coop_world_active()) { ++g_st.drop_not_coop; return; }
    if (!hub_coop_validate(kHubTagCoopCtl, d, len)) { ++g_st.drop_bad; return; }
    if (!seat_ok(seat)) return;
    const int sub = d[6];
    // The sync-off control switches off the WORLD kinds only; the save and boot ranges registered by the other
    // lanes keep running (the boot barrier needs its GO to get a console out of the title).
    if (!g_sync && sub >= kCtlGroupHello && sub <= kCtlOwnerChange) { ++g_st.drop_sync_off; return; }
    const unsigned char *p = d + kHubCoopHeaderBytes;
    const int pl = len - kHubCoopHeaderBytes;
    const unsigned now = comms_hub_now_ms();
    CoopKey k;
    switch (sub) {
    case kCtlGroupHello: {
        CoopHelloWire w;
        if (!coop_hello_validate(p, pl)) { ++g_st.drop_bad; return; }
        std::memcpy(&w, p, sizeof w);
        key_from_wire(w.key, &k);
        ++g_st.recv_hello;
        if (k == g_key) g_heard[seat] = now | 1u;
        g_grp.note_hello(seat, k, w.established != 0, w.owner == kCoopNoSeat ? -1 : (int)w.owner, now);
        return;
    }
    case kCtlLevelEnter: {
        CoopEnterWire w;
        if (!coop_enter_validate(p, pl)) { ++g_st.drop_bad; return; }
        std::memcpy(&w, p, sizeof w);
        key_from_wire(w.key, &k);
        ++g_st.recv_enter;
        if (!for_me(k)) { ++g_st.drop_group; return; }
        if (!g_grp.i_am_owner()) return;       // the owner answers
        Job &j = g_job[seat];
        j.on = true; j.nonce = w.nonce; j.key = k; j.start = now; j.last = now;
        serve_snapshot(seat, j);
        ++g_st.snaps_served;
        for (int i = 0; i < g_n_snapreq; ++i) g_cb_snapreq[i](seat, w.nonce);
        return;
    }
    case kCtlSnapPart: {
        CoopSnapWire w;
        if (!coop_snap_validate(p, pl)) { ++g_st.drop_bad; return; }
        std::memcpy(&w, p, sizeof w);
        ++g_st.recv_snap;
        if (w.target != g_my) { ++g_st.drop_target; return; }
        key_from_wire(w.key, &k);
        if (!for_me(k)) { ++g_st.drop_group; return; }
        if (w.nonce != g_nonce || (!g_leg_trust && g_enter_nonce != g_nonce)) { ++g_st.drop_nonce; return; }   // an answer to MY enter only
        if (!g_leg_trust && !g_grp.is_member(seat)) { ++g_st.drop_trust; return; }
        if (g_snap_done) {                      // a resend after we already have it: re-ack, rate limited
            if ((unsigned)(now - g_ack_last) >= 100u) { send_ack(seat, g_nonce); g_ack_last = now; }
            return;
        }
        if (g_snap_total && g_snap_total != w.total) { ++g_st.drop_bad; return; }
        g_snap_total = w.total;
        const int clen = kCoopSnapChunkMax / w.total;
        std::memcpy(g_snap_buf + w.n * clen, p + sizeof w, (size_t)clen);
        g_snap_mask |= 1u << w.n;
        if (g_snap_mask == (1u << w.total) - 1u) {
            g_snap_done = true;
            g_snap_owner = seat;
            send_ack(seat, g_nonce);
            g_ack_last = now;
            if (!g_snap_waiting) {              // the hook gave up already: take it as a live merge
                const unsigned *s = reinterpret_cast<const unsigned *>(g_snap_buf);
                for (int i = 0; i < kCoopWords; ++i) merge_remote_word(i, s[i]);
                ++g_st.snaps_applied;
            }
        }
        return;
    }
    case kCtlSnapAck: {
        CoopAckWire w;
        if (!coop_ack_validate(p, pl)) { ++g_st.drop_bad; return; }
        std::memcpy(&w, p, sizeof w);
        ++g_st.recv_ack;
        key_from_wire(w.key, &k);
        if (g_job[seat].on && g_job[seat].nonce == w.nonce && g_job[seat].key == k)
            g_job[seat].on = false;
        return;
    }
    case kCtlOwnerChange: {
        CoopOwnerWire w;
        if (!coop_owner_validate(p, pl)) { ++g_st.drop_bad; return; }
        std::memcpy(&w, p, sizeof w);
        key_from_wire(w.key, &k);
        ++g_st.recv_owner;
        if (w.owner != seat) { ++g_st.drop_bad; return; }   // only a seat can name itself
        g_grp.note_owner_change(seat, k, now);
        return;
    }
    default:
        for (int i = 0; i < g_nreg_ctl; ++i)
            if (sub >= g_reg_ctl[i].lo && sub <= g_reg_ctl[i].hi) {
                g_reg_ctl[i].fn(seat, sub, p, pl, g_reg_ctl[i].user);
                return;
            }
        ++g_st.drop_unknown_sub;
    }
}

void on_world(int seat, const unsigned char *d, int len, void *) {
    if (!coop_world_active()) { ++g_st.drop_not_coop; return; }
    if (!g_sync) { ++g_st.drop_sync_off; return; }
    if (!hub_coop_validate(kHubTagCoopWorld, d, len)) { ++g_st.drop_bad; return; }
    if (!seat_ok(seat)) return;
    const int sub = d[6];
    const unsigned char *p = d + kHubCoopHeaderBytes;
    const int pl = len - kHubCoopHeaderBytes;
    CoopKey k;
    if (sub == kWorldDeathDelta) {
        if (!coop_delta_validate(p, pl)) { ++g_st.drop_bad; return; }
        CoopDeltaWire w; std::memcpy(&w, p, sizeof w);
        key_from_wire(w.key, &k);
        ++g_st.recv_delta;
        if (!for_me(k)) { ++g_st.drop_group; return; }
        g_heard[seat] = comms_hub_now_ms() | 1u;
        if (!g_leg_trust && !g_grp.is_member(seat)) { ++g_st.drop_trust; return; }   // members of this group only
        for (int i = 0; i < (int)w.count; ++i) {
            uint16_t s; std::memcpy(&s, p + sizeof w + 2 * i, 2);
            merge_remote_word(s >> 5, 1u << (s & 31));
        }
        return;
    }
    if (sub == kWorldSwallowGone) {
        CoopSwallowWire w;
        if (pl != (int)sizeof w) { ++g_st.drop_bad; return; }
        std::memcpy(&w, p, sizeof w);
        if (!coop_key_ok(w.key) || w.slot >= kCoopSlots || w.cls >= 1024 || w.cls == 0xbf) { ++g_st.drop_bad; return; }
        key_from_wire(w.key, &k);
        if (!for_me(k)) { ++g_st.drop_group; return; }
        g_heard[seat] = comms_hub_now_ms() | 1u;
        if (!g_leg_trust && !g_grp.is_member(seat)) { ++g_st.drop_trust; return; }   // members of this group only
        apply_swallow(seat, (int)w.slot, (unsigned)w.cls);
        return;
    }
    if (sub == kWorldDeathFull) {
        if (!coop_full_validate(p, pl)) { ++g_st.drop_bad; return; }
        CoopFullWire w; std::memcpy(&w, p, sizeof w);
        key_from_wire(w.key, &k);
        ++g_st.recv_full;
        if (!for_me(k)) { ++g_st.drop_group; return; }
        if (!g_leg_trust) { g_grp.update(comms_hub_now_ms()); if (!g_grp.owner_claimed_by(seat)) { ++g_st.drop_trust; return; } }   // the group owner only
        for (int i = 0; i < kCoopWords; ++i) merge_remote_word(i, w.words[i]);
        ack_out(w.words);
        return;
    }
    for (int i = 0; i < g_nreg_world; ++i)
        if (sub >= g_reg_world[i].lo && sub <= g_reg_world[i].hi) {
            g_reg_world[i].fn(seat, sub, p, pl, g_reg_world[i].user);
            return;
        }
    ++g_st.drop_unknown_sub;
}

void ensure_handlers() {
    if (g_handlers) return;
    hub_on_kind(kHubCoopCtl, on_ctl, nullptr);
    hub_on_kind(kHubCoopWorld, on_world, nullptr);
    g_handlers = true;
    coop_api_test_register();
    g_my = comms_hub_my_seat();
    g_grp.reset(g_my);
    if (g_log) std::atexit([] { stats_line(); });
    std::fprintf(stderr, "[coop] world core on: seat %d, sync %s\n", g_my, g_sync ? "on" : "OFF (control)");
}

// ---------------------------------------------------------------------------
// Presence -> membership.
// ---------------------------------------------------------------------------
void refresh_presence(unsigned now) {
    for (int s = 0; s < kCoopSeats; ++s) {
        if (s == g_my) continue;
        HubPresence p; unsigned age = 0;
        const bool got = hub_presence_get(s, &p, &age) && p.in_level();
        const bool fresh = got && age < kPresenceMs;
        // Heavy loss: presence packets go missing for seconds while the seat's hello and deltas for this very group still
        // arrive. That is proof it is here: keep it a member instead of dropping it and giving up on its kills.
        const bool graced = got && !fresh && !g_leg_grace && g_heard[s] != 0 && (unsigned)(now - g_heard[s]) < kGraceMs;
        const bool have = fresh || graced;
        g_grp.note_presence(s, have, graced ? g_key : have ? CoopKey(p.level, p.act) : CoopKey(), now);
    }
}

void dump_bits(const char *why) {
    if (!g_log || g_part < 0) return;
    const unsigned *t = tbl(g_part);
    int n = 0;
    char hex[kCoopWords * 9 + 1]; hex[0] = 0;
    char *o = hex;
    for (int i = 0; i < kCoopWords; ++i) { n += popcnt(t[i]); o += std::sprintf(o, "%08x", t[i]); if (i + 1 < kCoopWords) *o++ = ' '; }
    std::fprintf(stderr, "[coopbits] %s level=%d act=%d part=%d n=%d bits=%s\n", why, g_key.level, g_key.act, g_part, n, hex);
}

// My unconfirmed kills, once more under the OLD group key, before the key changes: the retry loop
// stops when I leave, and the others would never hear them. Eight copies 20 ms apart (loss is per
// datagram, the owner's apply is idempotent, and 8 in 160 ms stays inside the 40 pps seat cap).
// Kills made in the same tick the level changes are in the table but not yet in g_out (tick_est
// has not run since): pick them up here so the flush below resends them too.
void rescan_before_leave(unsigned now) {
    const unsigned *t = tbl(g_part);
    for (int w = 0; w < kCoopWords; ++w) {
        unsigned nb = t[w] & ~g_prev[w];
        while (nb) {
            const unsigned bit = nb & (~nb + 1u);
            nb &= nb - 1;
            int b = 0; while (!((bit >> b) & 1u)) ++b;
            if (g_out_n < kOutMax) {
                g_out[g_out_n].slot = (uint16_t)(w * 32 + b);
                g_out[g_out_n].sent = 0;
                g_out[g_out_n].next = now;
                ++g_out_n;
            }
            ++g_st.local_bits;
            if (g_log) std::fprintf(stderr, "[coop] local bit slot=%d (leave rescan)\n", w * 32 + b);
        }
        g_prev[w] |= t[w] & ~g_prev[w];
    }
}

void flush_on_leave() {
    if (!g_sync || !g_key.valid() || g_part < 0 || !g_grp.established() || g_grp.others() <= 0) return;
    rescan_before_leave(comms_hub_now_ms());
    int total = 0;
    for (int i = 0; i < g_out_n;) {
        uint16_t slots[kCoopMaxDelta]; int n = 0;
        for (; i < g_out_n && n < kCoopMaxDelta; ++i)
            if (g_out[i].sent != 255) slots[n++] = g_out[i].slot;
        if (n == 0) break;
        unsigned char buf[sizeof(CoopDeltaWire) + 2 * kCoopMaxDelta];
        CoopDeltaWire h; std::memset(&h, 0, sizeof h); h.key = wire_key(g_key); h.count = (uint8_t)n;
        std::memcpy(buf, &h, sizeof h);
        std::memcpy(buf + sizeof h, slots, (size_t)n * 2);
        for (int c = 0; c < 8; ++c) {
            if (c) ::Sleep(20);
            if (send_kind(kHubTagCoopWorld, kWorldDeathDelta, buf, (int)sizeof h + 2 * n)) ++g_st.sent_delta;
        }
        total += n;
    }
    if (total > 0) {
        comms_hub_pump();
        if (g_log) std::fprintf(stderr, "[coop] leave flush: %d unconfirmed kills resent under the old key\n", total);
    }
}

void join_group(const CoopKey &nk) {
    const unsigned now = comms_hub_now_ms();
    if (!g_leg_flush) flush_on_leave();
    g_grp.leave();
    g_out_n = 0;
    g_sw_n = 0; g_mouth = nullptr; g_mouth_slot = -1; g_sw_census_at = 0; cap_reset();
    std::memset(g_remote, 0, sizeof g_remote);
    std::memset(g_sticky, 0, sizeof g_sticky);
    std::memset(g_star_seen, 0, sizeof g_star_seen);
    g_star_n = 0;
    std::memset(g_job, 0, sizeof g_job);
    std::memset(g_heard, 0, sizeof g_heard);
    g_snap_waiting = false; g_snap_done = false; g_snap_total = 0; g_snap_mask = 0; g_snap_owner = -1;
    g_recon_dirty = false;
    g_est_frames = 0;
    for (int i = 0; i < g_nkill; ++i) g_kill[i].done = false;
    g_goto_done = false;
    g_goto_tries = 0;
    g_star_done = g_star_frame == 0;
    g_star_spawned = false;
    g_star_tries = 0; g_star_next = 0; g_star_base = g_st.star_collects;
    g_rejected = CoopKey();
    g_key = nk;
    g_part = part_of_level(nk.level);
    g_grp.enter(nk, now);
    ++g_nonce;
    if (g_part < 0) {   // not a level the table covers: stay out of any group
        g_rejected = nk;
        g_key = CoopKey();
        return;
    }
    comms_hub_pump();
    refresh_presence(now);
    const int peers = g_grp.others();
    int hint = -1;
    if (peers > 0) {
        g_snap_waiting = true;
        const unsigned t0 = comms_hub_now_ms();
        unsigned last_send = 0;
        send_enter(); last_send = t0;
        while ((unsigned)(comms_hub_now_ms() - t0) < kEnterWaitMs && !g_snap_done) {
            ::Sleep(2);
            comms_hub_pump();
            const unsigned n2 = comms_hub_now_ms();
            if ((unsigned)(n2 - last_send) >= kEnterResend) { send_enter(); last_send = n2; }
        }
        g_snap_waiting = false;
        if (g_snap_done) {
            unsigned *t = tbl(g_part);
            const unsigned *s = reinterpret_cast<const unsigned *>(g_snap_buf);
            for (int i = 0; i < kCoopWords; ++i) {
                t[i] = s[i] | g_remote[i];     // the group's state, plus anything heard while waiting
                g_remote[i] |= s[i];           // survivors of the spawn get reconciled
            }
            hint = g_snap_owner;
            ++g_st.snaps_applied;
            g_recon_dirty = true;
            std::fprintf(stderr, "[coop] joined level %d act %d: snapshot from seat %d applied before spawn "
                         "(%d ms)\n", nk.level, nk.act, g_snap_owner,
                         (int)(comms_hub_now_ms() - t0));
        } else {
            ++g_st.snap_timeouts;
            std::fprintf(stderr, "[coop] joined level %d act %d: no snapshot in %d ms, first in\n",
                         nk.level, nk.act, (int)kEnterWaitMs);
        }
    } else {
        ++g_st.first_in;
        std::fprintf(stderr, "[coop] joined level %d act %d: nobody here, first in\n", nk.level, nk.act);
    }
    std::memcpy(g_prev, tbl(g_part), sizeof g_prev);
    g_grp.establish(comms_hub_now_ms(), hint);
    g_grp.update(comms_hub_now_ms());
    g_hello_t = 0; g_full_t = comms_hub_now_ms();
    g_prev_members = g_grp.member_mask();
    dump_bits("established");
    // latched first, then established: a lane resets its per-level state in latched and fills it in established
    for (int i = 0; i < g_n_latched; ++i) g_cb_latched[i](nk.level, nk.act);
    for (int i = 0; i < g_n_estab; ++i) g_cb_estab[i](nk.level, nk.act);
}

// ---------------------------------------------------------------------------
// Proof knobs (inert unless named).
// ---------------------------------------------------------------------------
// The local player touches a PowerStar: the same two words the harness probe writes
// (SM64DS_STAR_TRIGGER, hal/input_probe.cpp), so the star's own collect handler runs.
// Counted in frames since this level was established, so it works after a new file's opening.
void knob_star() {
    if (g_st.star_collects > g_star_base || g_star_tries >= 8) { g_star_done = true; return; }   // collected (or given up)
    if (g_est_frames < g_star_next) return;
    const int me = (int)data_0209f250;
    char *player = (me >= 0 && me < 16) ? (char *)data_0209f394[me] : nullptr;
    if (!player) return;
    char *star = nullptr, *any = nullptr;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (*(const unsigned short *)(a + 0x0c) != 178) continue;
        if (!any) any = a;
        const int st = *(const int *)(a + 0x440);
        if (st == 4 || st == 8) { star = a; break; }
    }
    if (!star) star = any;
    if (!star) {
        if (!g_star_spawned) {
            g_star_spawned = true;
            port_debug_spawn(178, 0);
            std::fprintf(stderr, "[coop-knob] no live PowerStar: spawned one\n");
        }
        return;
    }
    *(unsigned *)(star + 0x134) = *(const unsigned *)(player + 4);
    *(unsigned *)(star + 0x130) |= 0x400000;
    ++g_star_tries;
    g_star_next = g_est_frames + 60;      // the collect needs a few ticks; arm again if it did not take
    std::fprintf(stderr, "[coop-knob] star touch armed (star state %d)\n", *(const int *)(star + 0x440));
}

void knob_kill(const KillKnob &k) {
    // Nearest to the local player first (a proof wants the kill on screen).
    struct C { short slot; char *a; long long d2; };
    C c[128]; int n = 0;
    const char *pl = (const char *)data_0209f394[data_0209f250 & 15];
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && n < 128; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (*(const unsigned short *)(a + 0x0c) != (unsigned)k.id) continue;
        const short slot = *(const short *)(a + 0xce);
        if (slot < 0 || slot >= kCoopSlots) continue;
        if (*(const unsigned char *)(a + 0x0f) || *(const unsigned char *)(a + 0x0e) == 2) continue;
        if (is_player_ptr(a)) continue;
        long long d2 = 0;
        if (pl) for (int q = 0; q < 3; ++q) {
            const long long dd = (long long)*(const int *)(a + 0x5c + 4 * q) - *(const int *)(pl + 0x5c + 4 * q);
            d2 += (dd >> 8) * (dd >> 8);
        }
        c[n++] = {slot, a, d2};
    }
    std::sort(c, c + n, [](const C &x, const C &y) { return x.d2 != y.d2 ? x.d2 < y.d2 : x.slot < y.slot; });
    int killed = 0;
    for (int i = 0; i < n && killed < k.n; ++i, ++killed) {
        std::fprintf(stderr, "[coop-knob] kill class=%d slot=%d\n", k.id, (int)c[i].slot);
        _ZN8dActor_c24KillAndTrackInDeathTableEv(c[i].a);
    }
    std::fprintf(stderr, "[coop-knob] kill class=%d asked %d did %d (live candidates %d)\n", k.id, k.n, killed, n);
}

void knob_goto(int id, int tries) {
    // Put the local player on the (tries % n)-th live actor of class `id`, lowest slot first, so the ROM's own touch
    // runs. Called again every 90 frames while the player is still in the course, trying the next candidate.
    void *pl = data_0209f394[data_0209f250 & 15];
    if (!pl) return;
    char *c[32]; int n = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && n < 32; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (*(const unsigned short *)(a + 0x0c) != (unsigned)id || is_player_ptr(a)) continue;
        if (*(const unsigned char *)(a + 0x0f) || *(const unsigned char *)(a + 0x0e) == 2) continue;
        c[n++] = a;
    }
    std::sort(c, c + n, [](char *x, char *y) { return *(const short *)(x + 0xce) < *(const short *)(y + 0xce); });
    if (n == 0) { std::fprintf(stderr, "[coop-knob] goto class=%d: no live actor\n", id); return; }
    char *a = c[tries % n];
    std::memcpy((char *)pl + 0x5c, a + 0x5c, 12);
    *(int *)((char *)pl + 0x60) += g_goto_dy << 12;
    std::fprintf(stderr, "[coop-knob] goto class=%d try %d: candidate %d of %d slot=%d pos=%d,%d,%d\n", id, tries,
                 tries % n, n, (int)*(const short *)(a + 0xce), *(const int *)(a + 0x5c) >> 12,
                 *(const int *)(a + 0x60) >> 12, *(const int *)(a + 0x64) >> 12);
}

void knob_forge(const char *name) {
    unsigned char b[64]; std::memset(b, 0, sizeof b);
    CoopKeyWire k = wire_key(g_key);
    std::fprintf(stderr, "[coop-knob] forge %s\n", name);
    if (!std::strcmp(name, "oob")) {          // a slot past the end of the part
        CoopDeltaWire w; std::memset(&w, 0, sizeof w); w.key = k; w.count = 2;
        std::memcpy(b, &w, sizeof w); uint16_t s[2] = {5, 600}; std::memcpy(b + sizeof w, s, 4);
        send_kind(kHubTagCoopWorld, kWorldDeathDelta, b, (int)sizeof w + 4);
    } else if (!std::strcmp(name, "group")) { // right shape, someone else's level
        CoopDeltaWire w; std::memset(&w, 0, sizeof w); w.key = k; w.key.act = (uint8_t)((k.act + 1) & 7); w.count = 1;
        std::memcpy(b, &w, sizeof w); uint16_t s = 7; std::memcpy(b + sizeof w, &s, 2);
        send_kind(kHubTagCoopWorld, kWorldDeathDelta, b, (int)sizeof w + 2);
    } else if (!std::strcmp(name, "count")) { // count says 5, one slot present
        CoopDeltaWire w; std::memset(&w, 0, sizeof w); w.key = k; w.count = 5;
        std::memcpy(b, &w, sizeof w); uint16_t s = 7; std::memcpy(b + sizeof w, &s, 2);
        send_kind(kHubTagCoopWorld, kWorldDeathDelta, b, (int)sizeof w + 2);
    } else if (!std::strcmp(name, "snapbad")) { // chunk 3 of 2
        CoopSnapWire w; std::memset(&w, 0, sizeof w); w.target = (uint8_t)(g_my ^ 1); w.n = 3; w.total = 2; w.key = k;
        std::memcpy(b, &w, sizeof w);
        send_kind(kHubTagCoopCtl, kCtlSnapPart, b, (int)sizeof w + 32);
    } else if (!std::strcmp(name, "snapgroup")) { // a whole valid snapshot for another group, aimed at the other seat
        CoopSnapWire w; std::memset(&w, 0, sizeof w); w.target = (uint8_t)(g_my ^ 1); w.n = 0; w.total = 2; w.key = k;
        w.key.level = (uint16_t)((k.level + 1) & 0x7f);
        std::memcpy(b, &w, sizeof w);
        send_kind(kHubTagCoopCtl, kCtlSnapPart, b, (int)sizeof w + 32);
    } else if (!std::strcmp(name, "snapnonce")) { // a whole valid snapshot for the right group with a nonce nobody asked for
        CoopSnapWire w; std::memset(&w, 0, sizeof w); w.target = (uint8_t)(g_my ^ 1); w.n = 0; w.total = 2; w.key = k;
        w.nonce = 0xdeadbeefu;
        std::memcpy(b, &w, sizeof w);
        send_kind(kHubTagCoopCtl, kCtlSnapPart, b, (int)sizeof w + 32);
    } else if (!std::strcmp(name, "sub")) {    // a SYNB sub-type nobody owns
        send_kind(kHubTagCoopCtl, 0x1e, b, 4);
    } else if (!std::strcmp(name, "trunc")) {  // a delta cut short (head says 3 slots, 1 follows)
        CoopDeltaWire w; std::memset(&w, 0, sizeof w); w.key = k; w.count = 3;
        std::memcpy(b, &w, sizeof w);
        send_kind(kHubTagCoopWorld, kWorldDeathDelta, b, (int)sizeof w + 2);
    }
}

void census_line() {
    // Placed actors by class (those with a death-table slot): the proof reads
    // class ids for its kill / goto knobs from this.
    int cnt[1024]; std::memset(cnt, 0, sizeof cnt);
    int slotted = 0, total = 0, krb = 0, krb_rt = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        ++total;
        const short slot = *(const short *)(a + 0xce);
        if (is_goomba(*(const unsigned short *)(a + 0x0c)) && !going(a)) { ++krb; if (slot < 0) ++krb_rt; }
        if (slot < 0 || slot >= kCoopSlots) continue;
        const unsigned id = *(const unsigned short *)(a + 0x0c);
        ++slotted;
        if (id < 1024) ++cnt[id];
    }
    std::fprintf(stderr, "[coop] census level=%d act=%d actors=%d slotted=%d classes:", g_key.level, g_key.act, total, slotted);
    for (int i = 0; i < 1024; ++i) if (cnt[i]) std::fprintf(stderr, " %d:%d", i, cnt[i]);
    std::fprintf(stderr, " | goombas live %d (no slot %d) est=%u\n", krb, krb_rt, g_est_frames);
    if (g_est_frames != 40) return;
    std::fprintf(stderr, "[coop] census goombas with a cap (slot:cap):");     // their swallow leaves no death bit
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if (is_goomba(*(const unsigned short *)(a + 0x0c)) && (*(const unsigned char *)(a + kKrbCapId) & 0xf) < 6)
            std::fprintf(stderr, " %d:%d", (int)*(const short *)(a + 0xce), *(const unsigned char *)(a + kKrbCapId) & 0xf);
    std::fputc('\n', stderr);
}

void stats_line() {
    if (!g_log) return;
    const CoopWorldStats &s = g_st;
    int rn = 0;
    for (int i = 0; i < kCoopWords; ++i) rn += popcnt(g_remote[i]);
    std::fprintf(stderr,
        "[coopstat] seat=%d owner=%d est=%d members=%u sent h%llu e%llu s%llu a%llu o%llu d%llu f%llu "
        "recv h%llu e%llu s%llu a%llu o%llu d%llu f%llu applied=%llu recon=%llu local=%llu served=%llu "
        "snaps=%llu firstin=%llu timeouts=%llu drop bad=%llu group=%llu target=%llu nonce=%llu sub=%llu "
        "notcoop=%llu seat=%llu syncoff=%llu trust=%llu stars=%llu clears=%llu resends=%llu remote=%d\n",
        g_my, g_grp.owner(), (int)g_grp.established(), g_grp.member_mask(),
        s.sent_hello, s.sent_enter, s.sent_snap, s.sent_ack, s.sent_owner, s.sent_delta, s.sent_full,
        s.recv_hello, s.recv_enter, s.recv_snap, s.recv_ack, s.recv_owner, s.recv_delta, s.recv_full,
        s.bits_applied, s.reconcile_kills, s.local_bits, s.snaps_served, s.snaps_applied, s.first_in,
        s.snap_timeouts, s.drop_bad, s.drop_group, s.drop_target, s.drop_nonce, s.drop_unknown_sub,
        s.drop_not_coop, s.drop_seat, s.drop_sync_off, s.drop_trust, s.star_collects, s.local_clears, s.out_resends, rn);
}

// A star the local player just collected. The ROM only records it in the death
// table when the whole star-get flow completes (func_ov002_020e8ef0, the
// TrackInDeathTable after the message), and the grabber's course exit can clear
// the table first, so the others kept seeing a star somebody took. The collect
// itself is visible at once: the handler (e8ef0) sets the star's state word
// (+0x440) to 5 and its toucher (+0x438) to the player. Raise the star's own
// slot (and the linked actor's, when the ROM would track that one too) in my
// table now; the ordinary local-bit path announces it. The bits are also kept
// sticky so a snapshot or full bitmap I hand out holds them even if the ROM
// clears them again later.
void watch_stars() {
    unsigned *t = tbl(g_part);
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        const unsigned cls = *(const unsigned short *)(a + 0x0c);
        if (cls != 0xb2 && cls != 0xb3) continue;
        if (*(const unsigned *)(a + 0x440) != 5u) continue;
        if (!is_player_ptr(*(void *const *)(a + 0x438))) continue;
        bool seen = false;
        for (int i = 0; i < g_star_n; ++i) seen = seen || g_star_seen[i] == a;
        if (seen || g_star_n >= kStarSeen) continue;
        g_star_seen[g_star_n++] = a;
        int slots[2] = { (int)*(const short *)(a + 0xce), -1 };
        char *found = (char *)_ZN8dActor_c10FindWithIDEj(*(const unsigned *)(a + 0x434));
        if (found && ((*(const unsigned short *)(a + 0x4a2) >> 12) & 1u)) slots[1] = (int)*(const short *)(found + 0xce);
        for (int i = 0; i < 2; ++i) {
            const int sl = slots[i];
            if (sl < 0 || sl >= kCoopSlots) continue;
            t[sl >> 5] |= 1u << (sl & 31);
            g_sticky[sl >> 5] |= 1u << (sl & 31);
        }
        ++g_st.star_collects;
        std::fprintf(stderr, "[coop] star collected: class=%u slot=%d linked slot=%d wall=%lld\n", cls, slots[0], slots[1], wall_ms());
    }
}

void tick_est(unsigned now) {
    ++g_est_frames;
    if (g_sync && !g_leg_star) watch_stars();
    // 1. what I killed or collected since last frame
    if (g_sync) {
        unsigned *t = tbl(g_part);
        for (int w = 0; w < kCoopWords; ++w) {
            // Bits the game cleared on its own (a respawned cap enemy, a StarMarker going away): that is
            // the new truth for my copy. Stop treating them as remote kills, stop announcing them.
            unsigned cl = g_prev[w] & ~t[w] & ~g_sticky[w];
            if (g_leg_clear) cl = 0;
            if (cl) {
                g_remote[w] &= ~cl;
                for (int i = 0; i < g_out_n; ++i)
                    if ((g_out[i].slot >> 5) == w && ((cl >> (g_out[i].slot & 31)) & 1u)) g_out[i].sent = 255;
                g_st.local_clears += (unsigned long long)popcnt(cl);
                if (g_log)
                    for (int b = 0; b < 32; ++b)
                        if ((cl >> b) & 1u) std::fprintf(stderr, "[coop] local clear slot=%d\n", w * 32 + b);
            }
            unsigned nb = t[w] & ~g_prev[w];
            while (nb) {
                const unsigned bit = nb & (~nb + 1u);
                nb &= nb - 1;
                int b = 0; while (!((bit >> b) & 1u)) ++b;
                if (g_out_n < kOutMax) {
                    g_out[g_out_n].slot = (uint16_t)(w * 32 + b);
                    g_out[g_out_n].sent = 0;
                    g_out[g_out_n].next = now;
                    ++g_out_n;
                }
                ++g_st.local_bits;
                if (g_log) std::fprintf(stderr, "[coop] local bit slot=%d\n", w * 32 + b);
            }
            g_prev[w] = t[w];
        }
        // 1b. how a mouthful left my Yoshi's mouth (TONGUE3), and the swallows the death table cannot carry
        watch_swallow(now);
        watch_capped(now);
        send_swallows(now);
        // 2. announce them (three tries each: now, +120 ms, +360 ms)
        unsigned char buf[sizeof(CoopDeltaWire) + 2 * kCoopMaxDelta];
        uint16_t slots[kCoopMaxDelta]; int idx[kCoopMaxDelta]; int n = 0;
        for (int i = 0; i < g_out_n && n < kCoopMaxDelta; ++i)
            if ((int)(now - g_out[i].next) >= 0) { slots[n] = g_out[i].slot; idx[n++] = i; }
        if (n > 0 && g_grp.others() > 0) {
            CoopDeltaWire h; std::memset(&h, 0, sizeof h); h.key = wire_key(g_key); h.count = (uint8_t)n;
            std::memcpy(buf, &h, sizeof h);
            std::memcpy(buf + sizeof h, slots, (size_t)n * 2);
            if (send_kind(kHubTagCoopWorld, kWorldDeathDelta, buf, (int)sizeof h + 2 * n)) {
                ++g_st.sent_delta;
                for (int i = 0; i < n; ++i) {
                    Out &o = g_out[idx[i]];
                    ++o.sent;
                    o.next = now + (o.sent == 1 ? 120u : o.sent == 2 ? 240u : 250u);
                    if (o.sent > kOwnerTries) ++g_st.out_resends;
                }
            }
        }
        // retire the finished ones (compact)
        int k = 0;
        // The owner's 1 Hz full bitmap carries the rest; a guest keeps telling until that bitmap shows the bit.
        const int limit = (g_grp.i_am_owner() || g_leg_retry) ? (int)kOwnerTries : (int)kGuestTries;
        for (int i = 0; i < g_out_n; ++i) if (g_out[i].sent < limit) g_out[k++] = g_out[i];
        if (g_grp.others() == 0) k = 0;      // nobody to tell: the owner-less table is the whole truth
        g_out_n = k;
    }
    // 3. hello, owner full resend, owner announcement
    if ((unsigned)(now - g_hello_t) >= kHelloMs) send_hello(now);
    g_grp.update(now);
    if (g_grp.take_owner_announce()) { send_owner_change(); send_full(now); send_hello(now); }
    if (g_grp.i_am_owner() && g_grp.others() > 0 && (unsigned)(now - g_full_t) >= kFullMs) send_full(now);
    // 4. snapshots I owe
    for (int s = 0; s < CoopGroups::kSeats; ++s) {
        Job &j = g_job[s];
        if (!j.on) continue;
        if (!g_grp.i_am_owner() || !g_grp.is_member(s) || (unsigned)(now - j.start) > kSnapGiveUpMs || j.key != g_key) { j.on = false; continue; }
        if ((unsigned)(now - j.last) >= kSnapResend) { serve_snapshot(s, j); j.last = now; }
    }
    // 5. reconcile
    if (g_recon_dirty || (g_est_frames % kReconcileEvery) == 0) {
        g_recon_dirty = false;
        reconcile();
    }
    // 6. proof knobs
    if (!g_revive.done && g_est_frames >= g_revive.frame && g_revive.slot >= 0 && g_revive.slot < kCoopSlots) {
        // What a cap-enemy respawn does: the ROM clears the death-table bit and a live actor stands in that slot again.
        g_revive.done = true;
        tbl(g_part)[g_revive.slot >> 5] &= ~(1u << (g_revive.slot & 31));
        char *na = (char *)port_debug_spawn((unsigned)g_revive.cls, 0);
        if (na) *(short *)(na + 0xce) = (short)g_revive.slot;
        std::fprintf(stderr, "[coop-knob] revive slot=%d class=%d actor=%s\n", g_revive.slot, g_revive.cls, na ? "yes" : "no");
    }
    const bool knobs_on = coop_boot_ticking();     // not during a new file's opening or a boot barrier
    for (int i = 0; knobs_on && i < g_nkill; ++i)
        if (!g_kill[i].done && g_est_frames >= g_kill[i].frame) { g_kill[i].done = true; knob_kill(g_kill[i]); }
    if (g_goto_id >= 0 && g_goto_tries < 6 && g_est_frames >= g_goto_frame + 90u * (unsigned)g_goto_tries) knob_goto(g_goto_id, g_goto_tries++);
    if (knobs_on && !g_star_done && g_est_frames >= g_star_frame) knob_star();
    if (knobs_on && g_place_on && g_est_frames >= g_place_frame && (g_place_level < 0 || (int)data_0209f2f8 == g_place_level)) {
        g_place_on = false;
        if (char *me = (char *)data_0209f394[data_0209f250 & 15]) {
            for (int q = 0; q < 3; ++q) *(int *)(me + 0x5c + 4 * q) = g_place_xyz[q] << 12;
            std::fprintf(stderr, "[coop-knob] place at %d,%d,%d (frame %u)\n", g_place_xyz[0], g_place_xyz[1], g_place_xyz[2], g_est_frames);
        }
    }
    if (g_trace_id >= 0 && (g_est_frames % g_trace_every) == 0) {
        const char *me = (const char *)data_0209f394[data_0209f250 & 15];
        char line[1024]; int n = 0; line[0] = 0;
        if (me) n += std::snprintf(line + n, sizeof line - n, "me (%d,%d,%d) air %d", *(const int *)(me + 0x5c) >> 12,
                                   *(const int *)(me + 0x60) >> 12, *(const int *)(me + 0x64) >> 12, (int)*(const unsigned char *)(me + 0x6de));
        // run after093, lane WHOMPBACK1: every player body slot's place as this console holds it (friends' bodies included)
        for (int q = 0; q < 4 && n < 600; ++q)
            if (const char *pb = (const char *)data_0209f394[q])
                n += std::snprintf(line + n, sizeof line - n, " b%d(%d,%d,%d)", q, *(const int *)(pb + 0x5c) >> 12, *(const int *)(pb + 0x60) >> 12,
                                   *(const int *)(pb + 0x64) >> 12);
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && n < 880; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
            if (*(const unsigned short *)(a + 0x0c) != (unsigned)g_trace_id || is_player_ptr(a)) continue;
            n += std::snprintf(line + n, sizeof line - n, " | cls %d slot %d ang %04x %04x %04x", g_trace_id, (int)*(const short *)(a + 0xce),
                               (unsigned)(unsigned short)*(const short *)(a + 0x8c), (unsigned)(unsigned short)*(const short *)(a + 0x8e),
                               (unsigned)(unsigned short)*(const short *)(a + 0x90));
            if (g_trace_off >= 0) n += std::snprintf(line + n, sizeof line - n, " f%03x %d", g_trace_off, (int)*(const short *)(a + g_trace_off));
            // run after093, lane WHOMPBACK1: the actor's place and clip radius (world units), so a row can tell how far each player is
            n += std::snprintf(line + n, sizeof line - n, " pos %d,%d,%d r %d", *(const int *)(a + 0x5c) >> 12, *(const int *)(a + 0x60) >> 12,
                               *(const int *)(a + 0x64) >> 12, *(const int *)(a + 0xb8) >> 12);
        }
        std::fprintf(stderr, "[coop-trace] est %u t %u %s\n", g_est_frames, (unsigned)now, line);
    }
    for (int i = 0; i < g_nforge; ++i)
        if (!g_forge[i].done && g_est_frames >= g_forge[i].frame) { g_forge[i].done = true; knob_forge(g_forge[i].name); }
    // 7. log
    if (g_log && (g_est_frames == 40 || g_est_frames == 300 || g_est_frames == 800 || g_est_frames == g_sw_census_at)) census_line();
    if (g_log) {
        const unsigned *t = tbl(g_part);
        unsigned h = 2166136261u;
        for (int i = 0; i < kCoopWords; ++i) { h ^= t[i]; h *= 16777619u; }
        if (h != g_last_bits_hash) { g_last_bits_hash = h; dump_bits("changed"); }
        if ((g_est_frames % 120) == 0) stats_line();
    }
}

}  // namespace

// ---------------------------------------------------------------------------
// Public.
// ---------------------------------------------------------------------------
// SEAMCOOP1 (run after093, the one-process plan's M5b): a coop room left or entered inside a running game. Everything this room
// learned goes back to a process that never was in one: the env is read again (so SM64DS_COOP, cleared or set by hal/coop_boot.cpp,
// decides), the hub handlers are installed again on the next room's hub (comms_hub_teardown forgot them), the group, its shadows, the
// outgoing queue, the snapshot and the per-seat jobs are emptied. The phase-2 registries (coop_on_*, coop_ctl_register) are
// registrations made once per process and stay; the enter nonce and the sequence numbers keep counting (a stale answer from the last
// room can never match).
void coop_world_session_reset() {
    g_env_read = false;
    g_on = false;
    g_handlers = false;
    g_prev_members = 0;
    g_grp.reset(-1);
    g_st = CoopWorldStats{};
    g_my = -1;
    g_frame = g_est_frames = g_kframe = 0;
    g_key = CoopKey();
    g_rejected = CoopKey();
    g_part = -1;
    std::memset(g_prev, 0, sizeof g_prev);
    std::memset(g_sticky, 0, sizeof g_sticky);
    std::memset(g_star_seen, 0, sizeof g_star_seen);
    g_star_n = 0;
    g_enter_nonce = 0;
    std::memset(g_remote, 0, sizeof g_remote);
    g_recon_dirty = false;
    g_last_bits_hash = 0;
    g_out_n = 0;
    g_sw_n = 0; g_mouth = nullptr; g_mouth_slot = -1; g_sw_census_at = 0; cap_reset();
    g_snap_waiting = false;
    g_snap_done = false;
    g_snap_total = 0;
    g_snap_mask = 0;
    g_snap_owner = -1;
    g_ack_last = 0;
    for (Job &j : g_job) j = Job{};
    std::memset(g_heard, 0, sizeof g_heard);
    g_hello_t = g_full_t = 0;
}

bool coop_world_active() {
    read_env();
    if (!g_on) return false;
    return comms_hub_active() && !comms_hub_arena() && comms_hub_state() == kHubLive &&
           comms_hub_room_kind() == kHubRoomCoop;
}

CoopWorldStats coop_world_stats() { return g_st; }

static bool register_range(Reg *arr, int *n, int lo, int hi, int mine_lo, int mine_hi,
                           CoopSubFn fn, void *user) {
    if (!fn || lo < 0 || hi > 255 || lo > hi) return false;
    if (!(hi < mine_lo || lo > mine_hi)) return false;
    for (int i = 0; i < *n; ++i) {
        // SEAMCOOP1: the very same range and handler again (a once-per-room latch reset for a second room in place): already there
        if (arr[i].lo == lo && arr[i].hi == hi && arr[i].fn == fn && arr[i].user == user) return true;
        if (!(hi < arr[i].lo || lo > arr[i].hi)) return false;
    }
    if (*n >= kMaxRegs) return false;
    arr[(*n)++] = {lo, hi, fn, user};
    return true;
}
bool coop_ctl_register(int lo, int hi, CoopSubFn fn, void *user) {
    return register_range(g_reg_ctl, &g_nreg_ctl, lo, hi, kCtlGroupHello, kCtlOwnerChange, fn, user);
}
bool coop_world_register(int lo, int hi, CoopSubFn fn, void *user) {
    return register_range(g_reg_world, &g_nreg_world, lo, hi, kWorldDeathDelta, kWorldDeathFull, fn, user);
}
bool coop_group_key(int *level, int *act) {
    if (!coop_world_active() || !g_key.valid()) return false;
    if (level) *level = g_key.level;
    if (act) *act = g_key.act;
    return true;
}
bool coop_group_established() { return coop_world_active() && g_key.valid() && g_grp.established(); }
int coop_group_owner() { return coop_group_established() ? g_grp.owner() : -1; }
bool coop_group_i_am_owner() { return coop_group_established() && g_grp.i_am_owner(); }
unsigned coop_group_member_mask() { return coop_group_established() ? g_grp.member_mask() : 0u; }
template <class F, int N> static bool add_cb(F (&arr)[N], int *n, F fn) {
    if (!fn || *n >= N) return false;
    for (int i = 0; i < *n; ++i) if (arr[i] == fn) return false;
    arr[(*n)++] = fn;
    return true;
}
bool coop_on_frame(void (*fn)(unsigned now_ms)) { return add_cb(g_cb_frame, &g_n_frame, fn); }
bool coop_on_group_latched(void (*fn)(int level, int act)) { return add_cb(g_cb_latched, &g_n_latched, fn); }
bool coop_on_group_established(void (*fn)(int level, int act)) { return add_cb(g_cb_estab, &g_n_estab, fn); }
bool coop_on_snapshot_request(void (*fn)(int seat, unsigned nonce)) { return add_cb(g_cb_snapreq, &g_n_snapreq, fn); }
bool coop_on_member_gone(void (*fn)(int seat)) { return add_cb(g_cb_gone, &g_n_gone, fn); }

// Proof only: SM64DS_COOP_API_TEST=1 registers one callback of each kind and logs what the registries hand out.
static void api_t_frame(unsigned) {
    static unsigned n; ++n;
    if (n == 1) std::fprintf(stderr, "[coop-api] frame callback runs\n");
}
static void api_t_latched(int l, int a) { std::fprintf(stderr, "[coop-api] latched %d %d est=%d\n", l, a, (int)coop_group_established()); }
static void api_t_estab(int l, int a) {
    int kl = -1, ka = -1; coop_group_key(&kl, &ka);
    std::fprintf(stderr, "[coop-api] established %d %d key=%d/%d owner=%d iam=%d members=%x\n", l, a, kl, ka, coop_group_owner(), (int)coop_group_i_am_owner(), coop_group_member_mask());
}
static void api_t_snap(int seat, unsigned nonce) { std::fprintf(stderr, "[coop-api] snapshot request from seat %d nonce %u\n", seat, nonce); }
static void api_t_gone(int seat) { std::fprintf(stderr, "[coop-api] member gone seat %d\n", seat); }
void coop_api_test_register() {
    const char *e = std::getenv("SM64DS_COOP_API_TEST");
    if (!(e && *e && *e != '0')) return;
    bool r = coop_on_frame(api_t_frame) && coop_on_group_latched(api_t_latched) && coop_on_group_established(api_t_estab)
          && coop_on_snapshot_request(api_t_snap) && coop_on_member_gone(api_t_gone);
    bool dup = coop_on_frame(api_t_frame);     // a repeat is refused
    bool nul = coop_on_frame(nullptr);
    std::fprintf(stderr, "[coop-api] registered %d, duplicate refused %d, null refused %d\n", (int)r, (int)!dup, (int)!nul);
}
bool coop_send_ctl(int subtype, const void *payload, int payload_len) {
    return coop_world_active() && send_kind(kHubTagCoopCtl, subtype, payload, payload_len);
}
bool coop_send_world(int subtype, const void *payload, int payload_len) {
    return coop_world_active() && send_kind(kHubTagCoopWorld, subtype, payload, payload_len);
}

}  // namespace port

extern "C" void port_coop_world_level_latched(void) {
    using namespace port;
    if (!coop_world_active()) return;
    ensure_handlers();
    join_group(CoopKey((int)data_0209f2f8, data_0209f220[0] & 0xff));   // fires latched, then established
}

extern "C" void port_coop_world_frame(void) {
    using namespace port;
    if (!coop_world_active()) return;
    ensure_handlers();
    ++g_frame;
    const bool ticking = coop_boot_ticking();
    if (ticking) ++g_kframe;
    const unsigned now = comms_hub_now_ms();
    refresh_presence(now);
    for (int i = 0; i < g_n_frame; ++i) g_cb_frame[i](now);
    {   // members that left my group since last frame
        g_grp.update(now);
        const unsigned m = g_grp.member_mask();
        const unsigned gone = g_prev_members & ~m;
        g_prev_members = m;
        for (int sdx = 0; gone && sdx < 16; ++sdx)
            if ((gone >> sdx) & 1u)
                for (int i = 0; i < g_n_gone; ++i) g_cb_gone[i](sdx);
    }
    const int me = (int)data_0209f250;
    const bool in_level = me >= 0 && me < 16 && data_0209f394[me] != nullptr;
    if (in_level) {
        // A boot that did not come through the latch (a direct level boot, or a
        // missed hook): join now. The snapshot, if one comes, merges live.
        const CoopKey rk((int)data_0209f2f8, data_0209f220[0] & 0xff);
        if ((!g_key.valid() || rk != g_key) && rk != g_rejected) join_group(rk);
    }
    // Proof knob: the game's own level-change call with a chosen act (the scripted warp
    // chain of the harness always asks for act 1). Counted in ticked frames (g_kframe): never
    // while a boot barrier holds this console (a level load with no game tick behind it
    // faults the renderer) and not during a new file's opening.
    for (int i = 0; in_level && ticking && i < g_nwarp; ++i)
        if (!g_warp[i].done && g_kframe >= g_warp[i].frame) {
            g_warp[i].done = true;
            std::fprintf(stderr, "[coop-knob] warp to level %d act %d\n", g_warp[i].level, g_warp[i].act);
            LoadLevelNoReturn((unsigned)g_warp[i].level, 0, (unsigned)g_warp[i].act, 1);
        }
    // No body (a level unload or reload): whatever was in my mouth left with the level, not down my throat.
    if (!in_level && g_mouth) { g_mouth = nullptr; g_mouth_slot = -1; }
    if (!in_level) g_cap_slotted = -1;
    g_grp.update(now);
    if (in_level && g_key.valid() && g_part >= 0 && g_grp.established()) tick_est(now);
}
