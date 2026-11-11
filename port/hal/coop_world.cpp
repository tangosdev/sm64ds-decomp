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
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
void ChangeArea(int areaID);              // COOPBOO2 proof knob: show the Boo's room (doors do this in play)
extern signed char data_02092120;        // the area shown (-1 none)
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
char *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b, const void *pos, const void *v16, int area, int f);
void _ZN7fBase_c18MarkForDestructionEv(void *self);
extern void *STAR_MARKERS[12];          // 0x0209f40c: the actors showing a star glint on the minimap
int _ZN5Event6GetBitEj(unsigned bit);
void _ZN5Event6SetBitEj(unsigned bit);
void _ZN8dActor_c11UntrackStarERa(void *self, signed char &starID);
void *_ZN11dCapEnemy_c15RespawnIfHasCapEv(void *self);
void func_ov084_02129498(void *goomba);
void _ZN6Player19func_ov002_020d718cEv(char *c);      // Yoshi: forget the mouthful (mObjInMouth = 0, mouth model back)
extern unsigned char data_0209f1f0;        // COOPRACE1 proof knob: the act the next level change latches
}

// COOPRACE1 proof knobs: the racers' routines and data they use (see the race knobs below).
extern "C" {
void _ZN5Timer10ResetTimerEv(void *self);
int func_ov019_02111d58(void *penguin);   // the penguin's state 3 (race) set-up
extern char data_0209d4c8[];               // TIME_TIMER, the course timer
extern char data_ov019_0211356c[];         // the penguin's six state records, 16 bytes each
extern int data_ov019_021134e8[3];         // the penguin's finish point
extern int data_ov019_0211353c[3];         // the penguin's shortcut (cheat) volume centre
}

// COOPBOO2: daTrs_c::func_ov063_02116244 (src/game/actors/d_a_trs.cpp), called on a Big Boo: with mBooDefeatCount at 5 or
// more and nothing spawned yet, it brings the Big Boo out (the ghost hunt). Declared bare: only the member's name is needed.
class daTrs_c { public: void func_ov063_02116244(); };

namespace port {
void coop_api_test_register();
namespace {

enum : unsigned {
    kHelloMs      = 1000,
    kFullMs       = 1000,
    kEnterWaitMs  = 400,     // no answer in this long = first in (nobody established here, or a fast link)
    kEnterWaitCap = 1500,    // COOPCOUNT1: the longest wait, for an established member on a slow link
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
struct KillKnob { unsigned frame; int id; int n; bool done; bool gone; };
KillKnob g_kill[8]; int g_nkill = 0;
unsigned g_star_frame = 0, g_star_next = 0; int g_star_tries = 0; unsigned long long g_star_base = 0;
bool g_star_done = true, g_star_spawned = false;   // SM64DS_COOP_STAR=<est frame>: the local player touches a PowerStar
unsigned g_goto_frame = 0; int g_goto_id = -1; bool g_goto_done = false; int g_goto_tries = 0; int g_goto_dy = 0;
int g_goto_max = 6;   // COOPCOUNT1: SM64DS_COOP_GOTO's optional 4th field, how many tries (default 6)
// COOPCOUNT2 proof knob SM64DS_COOP_GOTO_AFTER=N: the goto waits until this console's secret leader count is at least N, so a row
// can place a take right after a friend's take arrives (est frames start well before a joiner is in the group). Inert unless set.
int g_goto_after = 0;
// COINS10 proof knob SM64DS_COOP_LOOSE=F:n[:every]: from est frame F, n times every `every` frames (default 45), spawn a coin with
// no slot the way an enemy drops one (class 0x120, param 2, the Bob-omb's drop, d_a_bmb.cpp:1214) at my player and keep my player
// on it until the ROM's own touch collects it. Inert unless set.
unsigned g_loose_frame = 0, g_loose_every = 45, g_loose_uid = 0; int g_loose_left = 0, g_loose_done = 0, g_loose_hold = 0;
unsigned g_place_frame = 0; int g_place_xyz[3] = {0, 0, 0}; bool g_place_on = false;   // SM64DS_COOP_PLACE (run after093, lane SEESAW1)
int g_place_level = -1;   // SM64DS_COOP_PLACE ...:L, only in level L (run after093, lane WHOMPBACK1)
int g_trace_id = -1; unsigned g_trace_every = 5; int g_trace_off = -1;   // SM64DS_COOP_TRACE (run after093, lane SEESAW1)
struct WarpKnob { unsigned frame; int level; int act; bool done; int entrance; };
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
    // SM64DS_COOP_GONE has the same form as SM64DS_COOP_KILL but removes with MarkForDestruction alone (no death bit),
    // the end a broken wall, a toppled pillar or a beaten Big Mr. I reaches, so a proof can drive the swallow path.
    for (int g = 0; g < 2; ++g) if (const char *k = std::getenv(g ? "SM64DS_COOP_GONE" : "SM64DS_COOP_KILL")) {
        const char *p = k;
        while (*p && g_nkill < 8) {
            unsigned f = 0; int id = 0, n = 0, used = 0;
            if (std::sscanf(p, "%u:%i:%i%n", &f, &id, &n, &used) >= 3 && used > 0) {
                g_kill[g_nkill++] = {f, id, n, false, g == 1};
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
        // F:level:act[:entrance][;F:level:act[:entrance]]  (F = ticked frames since the boot release / the end of a new
        // file's opening; entrance 0 unless given)
        const char *p = w;
        while (*p && g_nwarp < 4) {
            unsigned f = 0; int lv = 0, ac = 1, used = 0, en = 0, used2 = 0;
            if (std::sscanf(p, "%u:%i:%i%n", &f, &lv, &ac, &used) < 3 || used <= 0) break;
            p += used;
            if (*p == ':' && std::sscanf(p, ":%i%n", &en, &used2) >= 1 && used2 > 0) p += used2;
            g_warp[g_nwarp++] = {f, lv, ac, false, en};
            while (*p == ';' || *p == ',' || *p == ' ') ++p;
        }
    }
    if (const char *st = std::getenv("SM64DS_COOP_STAR")) {
        unsigned f = 0;
        if (std::sscanf(st, "%u", &f) == 1 && f > 0) { g_star_frame = f; g_star_done = false; }
    }
    if (const char *g = std::getenv("SM64DS_COOP_GOTO_AFTER")) g_goto_after = std::atoi(g);
    if (const char *g = std::getenv("SM64DS_COOP_LOOSE")) {   // COINS10
        unsigned f = 0, every = 45; int n = 0;
        if (std::sscanf(g, "%u:%i:%u", &f, &n, &every) >= 2 && n > 0 && n <= 64) {
            g_loose_frame = f; g_loose_left = n; g_loose_every = every >= 20 && every <= 600 ? every : 45;
        }
    }
    if (const char *g = std::getenv("SM64DS_COOP_GOTO")) {
        unsigned f = 0; int id = 0;
        int dy = 0, tries = 6;   // F:class[:dy[:tries]]  dy units above the actor: the player drops onto it (a stomp)
        if (std::sscanf(g, "%u:%i:%i:%i", &f, &id, &dy, &tries) >= 2) {
            g_goto_frame = f; g_goto_id = id; g_goto_dy = dy; g_goto_max = tries > 0 && tries <= 32 ? tries : 6;
        }
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

// A friend's message is about to remove my copy of an actor (reconcile, apply_swallow). If a Yoshi here holds that copy
// in his mouth, the cartridge never deletes it under him: a mouthful that ends on its own lets the eater go first
// (d_a_bmb.cpp:1245-1252: Player::func_ov002_020d718c, then mEatingPlayer = 0) and only then goes away. Without this the
// body's mObjInMouth (+0x360) keeps the freed actor and St_YoshiPower_Main's next mouthful->v18() calls through the dead
// vtable (0.9.3.7 field crash, Bob-omb Battlefield host). Returns true when a mouth was emptied.
bool release_mouth_of(char *a) {
    bool any = false;
    for (int i = 0; i < 16; ++i) {
        char *p = (char *)data_0209f394[i];
        if (!p || *(char **)(p + 0x360) != a) continue;
        _ZN6Player19func_ov002_020d718cEv(p);
        if (*(char **)(a + 0xd0) == p) *(int *)(a + 0xd0) = 0;   // mEatingPlayer (dEnemyBase_c and the other eatables)
        any = true;
        std::fprintf(stderr, "[coop] mouthful removed by a friend's message: body %d let go of class %u slot %d first\n", i,
                     (unsigned)*(const unsigned short *)(a + 0x0c), (int)*(const short *)(a + 0xce));
    }
    return any;
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

void send_coin_count(unsigned now);   // COOPCOUNT1, below
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
    send_coin_count(comms_hub_now_ms());   // COOPCOUNT1: the joiner takes the level's coin count with the snapshot
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
// COOPCOUNT2: coins a raise to the group's count already gave me, not yet matched by a friend's death bit (see coin_tick).
int g_coin_unpaid = 0;
unsigned long long g_coin_paid = 0;
// COINS10 (audit G17, G26). Coins with no slot (enemy drops, runtime coins, the coin a Bob-omb or a mushroom gives) never
// leave a death bit. Each console keeps the coin value ITS player took from them this visit (g_loose_mine) and tells the
// group the running total (kWorldCoinLoose); a receiver credits the part above the last total it saw from that seat. My
// counter is then: placed coins (mine, and friends' by their death bits or a raise) + every member's loose total. My own
// gains are read off the counter: whatever it rose by that my own credits did not write (g_coin_last), less the value of
// the placed coins whose death bit I set myself that frame (g_slot_val, read off the coins while they stand).
int g_coin_last = -1;            // my counter after my last look or write; -1: take the next value as is
int g_own_pend = 0, g_placed_pend = 0;
unsigned g_loose_mine = 0, g_loose_in = 0, g_loose_visit = 0, g_loose_visits = 0;
struct LooseSeen { unsigned visit; unsigned total; };
LooseSeen g_loose_seen[CoopGroups::kSeats];
unsigned char g_slot_val[kCoopSlots];          // a placed coin's value (1, 2, 5) as last seen standing here, else 0
unsigned short g_slot_tmr[kCoopSlots];         // its disappear timer then (+0x3a8): 1..3 means it ran out, nobody took it
unsigned long long g_loose_sends = 0, g_loose_recvs = 0, g_loose_credits = 0, g_blue_skips = 0;
void coin_observe() {
    const int me = (int)data_0209f250;
    if (me < 0 || me > 3) return;
    const int cur = (int)data_0209f358[me];
    if (g_coin_last >= 0 && cur > g_coin_last) g_own_pend += cur - g_coin_last;
    g_coin_last = cur;
}
void coin_wrote() {
    const int me = (int)data_0209f250;
    if (me >= 0 && me <= 3) g_coin_last = (int)data_0209f358[me];
}
// A switch's blue coin (class 290, spawn filter under 8: it waits for the switch's event bit, d_a_coin.cpp:744-764)
// whose disappear timer is about to run out. Every console runs its own timer and its run-out sets the death bit
// (d_a_coin.cpp:736-742), so a friend's run-out reaching me first is not a coin he took.
bool blue_running_out(const char *a) {
    if (*(const unsigned short *)(a + 0x0c) != 290 || *(const unsigned char *)(a + 0x3ab) >= 8) return false;
    const unsigned t = *(const unsigned short *)(a + 0x3a8);
    return t > 0 && t <= 60;
}
// A death bit I set this frame (tick_est): a placed coin my player took, unless it ran out.
void coin_local_bit(int slot) {
    if (slot < 0 || slot >= kCoopSlots || !g_slot_val[slot]) return;
    if (g_slot_tmr[slot] >= 1 && g_slot_tmr[slot] <= 3) return;
    g_placed_pend += g_slot_val[slot];
}
void credit_remote_coins(int w, unsigned fresh) {
    if (coins_off() || !g_sync) return;
    const int me = (int)data_0209f250;
    if (me < 0 || me > 3) return;
    coin_observe();   // COINS10: my own pickups since the last look stay mine
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        const unsigned cls = *(const unsigned short *)(a + 0x0c);
        if (cls < 288 || cls > 290) continue;
        const short slot = *(const short *)(a + 0xce);
        if (slot < 0 || slot >= kCoopSlots || (slot >> 5) != w || !((fresh >> (slot & 31)) & 1u)) continue;
        if (*(const unsigned char *)(a + 0x0f) || *(const unsigned char *)(a + 0x0e) == 2) continue;      // already going
        if (!coins_native() && blue_running_out(a)) {   // COINS10: a friend's blue coin ran out before mine did
            ++g_blue_skips;
            if (g_log) std::fprintf(stderr, "[coop] coin credit skipped: switch blue coin slot=%d ran out (timer %d here) wall=%lld\n", (int)slot,
                                    (int)*(const unsigned short *)(a + 0x3a8), wall_ms());
            continue;
        }
        const int ty = *(const int *)(a + 0x3a0);
        const int v = ty == 1 ? 2 : ty == 2 ? 5 : 1;
        // COOPCOUNT2: a raise that came first already counted this coin (its count message beat its death bit, or the death bit was
        // lost and came back in the 1 Hz full bitmap): it pays off that raise instead of adding again.
        const int paid = coins_native() ? 0 : (v < g_coin_unpaid ? v : g_coin_unpaid);
        if (paid > 0) {
            g_coin_unpaid -= paid; g_coin_paid += (unsigned)paid;
            if (g_log)
                std::fprintf(stderr, "[coop] coin credit cls=%u slot=%d type=%d paid %d of a raise (owed now %d) count=%d wall=%lld\n", cls,
                             (int)slot, ty, paid, g_coin_unpaid, (int)data_0209f358[me] + v - paid, wall_ms());
            if (paid == v) continue;
        }
        int n = (int)data_0209f358[me] + v - paid;
        if (n > 0xff) n = 0xff;
        if (coins_native()) { GiveCoins(me, v); n = (int)data_0209f358[me]; }
        else data_0209f358[me] = (short)n;
        coin_wrote();
        ++g_coin_credits; g_coin_total += (unsigned)v;
        if (g_log)
            std::fprintf(stderr, "[coop] coin credit cls=%u slot=%d type=%d +%d count=%d wall=%lld\n", cls, (int)slot, ty, v - paid, n, wall_ms());
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
    // A death bit kills some holders of it on their own next Behavior (daCoin_c::func_ov002_020b10a0, d_a_coin.cpp:
    // KillAndTrackInDeathTable when GetBitInDeathTable is up, in a mouth or not), before reconcile sees them: a Yoshi
    // here holding one lets go now, the way release_mouth_of does for reconcile.
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        const short slot = *(const short *)(a + 0xce);
        if (slot >> 5 == w && slot >= 0 && ((fresh >> (slot & 31)) & 1u) && !is_player_ptr(a)) release_mouth_of(a);
    }
}

// The owner's full bitmap shows a bit: a guest's pending announcement of it is done.
void ack_out(const unsigned *words) {
    for (int i = 0; i < g_out_n; ++i)
        if ((words[g_out[i].slot >> 5] >> (g_out[i].slot & 31)) & 1u) g_out[i].sent = 255;
}

bool for_me(const CoopKey &k) {
    return g_part >= 0 && g_key.valid() && k == g_key;
}

// COOPBOO2: Boos that report to a Big Boo (daTrs_c, class 209, mBigBooID at +0x490). The killer's console adds each one to
// the Big Boo's mBooDefeatCount (+0x180): variant 2 (Big Boo's Haunt lobby) in its state-5 talk (d_a_trs.cpp:2003-2015), the
// others in func_ov063_0211a3d0 (d_a_trs.cpp:2598-2606), and the Big Boo comes out at 5. A friend's copy removed here never
// ran that code, so each console counted only its own kills and a split pair saw no Big Boo at all. boo_credit is the same
// add, for a copy this console removes because a friend killed it; then the ROM's own func_ov063_02116244 decides.
enum : int { kBooCount = 0x180, kBooTalk = 0x488, kBooBig = 0x490, kBooVar = 0x5cf, kBooState = 0x5cc, kBooSub = 0x5ce };
// A lobby Boo (variant 2) always reports; the others only once they have found their Big Boo. A lobby Boo whose room this
// console has not shown yet never ran its home state, so it has not looked its Big Boo up (func_ov063_02119c58): big_boo_of
// then does the same lookup.
bool count_boo(const char *a) {
    if (*(const unsigned short *)(a + 0x0c) != 209) return false;
    const unsigned char v = *(const unsigned char *)(a + kBooVar);
    return v == 2 || (v != 0 && *(const unsigned *)(a + kBooBig) != 0);
}
// A Boo in its hit states (3 fade, 4, 5 talk: d_a_trs.cpp func_ov063_021162c8 and on) is being beaten on this console
// by the cartridge's own code, which counts it.
bool boo_beaten_here(const char *a) { const unsigned st = *(const unsigned char *)(a + kBooState); return st >= 3 && st <= 5; }
char *lobby_big_boo() {
    for (char *b = (char *)_ZN8dActor_c4NextEPKS_(0); b; b = (char *)_ZN8dActor_c4NextEPKS_(b)) {   // func_ov063_02119c58's pick
        if (*(const unsigned short *)(b + 0x0c) != 210) continue;
        const unsigned char v = *(const unsigned char *)(b + kBooVar);
        if (v == 0xd || v == 0xe) return b;
    }
    return nullptr;
}
char *big_boo_of(const char *a) {
    const unsigned id = *(const unsigned *)(a + kBooBig);
    return id ? (char *)_ZN8dActor_c10FindWithIDEj(id) : lobby_big_boo();
}
// A lobby Boo is counted once per console and visit, whoever beat it: the set holds the slots already counted here (my own
// kills, which the cartridge counts, and the friends' kills boo_credit added). A friend's console can be missing a lobby
// Boo's copy altogether (seen on a launcher-path guest: 2 of the 6 stood), so a friend's kill with no copy here counts on
// the Big Boo directly; a copy that stands later is then only removed.
uint16_t g_boo_counted[16]; int g_boo_counted_n = 0;
bool boo_counted(int slot) {
    for (int i = 0; i < g_boo_counted_n; ++i) if (g_boo_counted[i] == (uint16_t)slot) return true;
    return false;
}
void boo_mark(int slot) { if (!boo_counted(slot) && g_boo_counted_n < 16) g_boo_counted[g_boo_counted_n++] = (uint16_t)slot; }
bool boo_credit_nocopy(int slot, int seat, const char *why) {
    char *big = lobby_big_boo();
    if (!big) return false;
    int &cnt = *(int *)(big + kBooCount);
    ++cnt;
    reinterpret_cast<::daTrs_c *>(big)->func_ov063_02116244();
    boo_mark(slot);
    if (g_log) std::fprintf(stderr, "[coop] boo: seat %d's Boo slot %d (no copy here) counted for the Big Boo here (%s): count %d wall=%lld\n", seat, slot,
                 why, cnt, wall_ms());
    return true;
}
bool boo_credit(const char *a, int seat, const char *why) {
    char *big = big_boo_of(a);
    if (!big || *(const unsigned short *)(big + 0x0c) != 210) return false;
    int &cnt = *(int *)(big + kBooCount);
    ++cnt;
    reinterpret_cast<::daTrs_c *>(big)->func_ov063_02116244();
    boo_mark(*(const short *)(a + 0xce));
    if (g_log) std::fprintf(stderr, "[coop] boo: seat %d's Boo slot %d (variant %u) counted for the Big Boo here (%s): count %d wall=%lld\n", seat,
                 (int)*(const short *)(a + 0xce), (unsigned)*(const unsigned char *)(a + kBooVar), why, cnt, wall_ms());
    return true;
}

// Remove the live actors whose slot a remote bit names. The ROM's own
// kill-and-track (bit already up, actor marked for destruction): no effects
// play on this console, no counters move (COOPBOO2: but a Boo the friend killed still counts for my Big Boo).
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
        release_mouth_of(a);
        if (count_boo(a) && *(const unsigned char *)(a + kBooVar) != 2) boo_credit(a, -1, "death bit");
        if (is_goomba(*(const unsigned short *)(a + 0x0c))) goomba_glint_off(a, false, "death bit");     // COOPSILVER3
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
// COOPBOO2: a Boo beaten for its Big Boo (a lobby Boo of Big Boo's Haunt) goes out as the swallow head plus the Big Boo count
// on the sender's console (log only): 12 bytes, a length 0.9.3.8 refuses as bad, so an older friend keeps his own Boos and
// count exactly as before (he can still beat all five himself). Class 209 only.
struct CoopBooWire { CoopSwallowWire head; int32_t count; };
static_assert(sizeof(CoopBooWire) == 12, "boo wire is 12 bytes");
enum : int { kKrbCapId = 0x113, kKrbReward = 0x464, kKrbHome = 0x41c, kMouthObj = 0x360, kEggs = 0x588, kSwOutMax = 32 };
struct SwOut { uint16_t slot, cls; uint8_t sent; unsigned next; uint8_t boo; };   // boo: send the 12-byte Boo form (COOPBOO2)
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

// Breakable blocks (run after093, lane COOPSYNC1). daObjBlockL_c::Kill (src/game/actors/d_a_obj_block_l.cpp:270-341) is the
// one break path of the brick block (15), the big brick block (16), the black brick block (17, which is also the star block:
// a star inside when its param's low byte is not 0xff) and the Whomp's Fortress
// tower wall (46): coins / star / poof, then MarkForDestruction only, no death bit. So a block broken on one console stayed
// whole and solid on every other one (the castle garden block over the grate into the secret course). The console where a
// placed block goes away with no bit tells the group with the swallow message (same 8 bytes; older builds already apply it
// as MarkForDestruction), every friend removes his copy, and every member keeps the slot on a list for this (level, act):
// a copy that stands up later (a friend who enters after the break) is removed as soon as it is there, and a member who
// joins the group is sent the list again. The coins and the star stay the breaker's. The 0x13 switch block (19) only flags
// itself broken and comes back from its switch: not watched.
// COOPWALLS1: the same shape, no death bit and nothing inside but an announced star, for Whomp's Fortress' two walls
// (daObjBk_Kabe_c, src/game/actors/d_a_obj_bk_kabe.cpp: 48 BK_KABE00, broken by a cannon shot, raises its star and then
// MarkForDestruction; 47 BK_KABE01 goes at once in Kill) and Jolly Roger Bay's stone pillars (58, daObjKi_Hasira_c,
// d_a_obj_ki_hasira.cpp:220: it topples when a player comes within 1300 units and ends in MarkForDestruction; nothing
// counts the pillars, no star waits on them). They go on the group list and the room list like the blocks.
bool breakable_block(unsigned cls) { return cls == 15 || cls == 16 || cls == 17 || cls == 46 || cls == 47 || cls == 48 || cls == 58; }
// COOPWALLS1: star holders that also leave with no death bit: Big Mr. I (263, daEykn_c, d_a_eykn.cpp:470-474: a death bit
// only in level 0x2e) and the Chill Bully (217, daIDonketu_c, d_a_i_donketu.cpp:137). Beaten on one console they are
// removed on the others and kept on the group's list for this visit (a member who joins the group is sent it), but never
// on the room's list: the star they hold is the breaker's and is announced once, and a later visit after the level emptied
// finds them as the cartridge builds it. (The Moneybag, 261, dies through KillAndTrackInDeathTable: the death table has it.)
bool live_gone(unsigned cls) { return cls == 263 || cls == 217; }
// REVCOOPWALLS1: in the act whose star they hold, these two own a minimap star marker from InitResources (TrackStar);
// the cartridge frees it in UntrackAndSpawnStar on the defeat, which a friend's copy never runs. Free it before the
// removal, the way coop_boss.cpp does for the big bosses, or the minimap and camera keep reading the freed boss.
void untrack_markers(void *a) {
    for (int m = 0; m < 12; ++m)
        if (STAR_MARKERS[m] == a) { signed char sl = (signed char)m; _ZN8dActor_c11UntrackStarERa(a, sl); }
}
// COOPSILVER3: silver star glints held by actors a friend's console removed. Two holders free their glint only on the way
// that hands the star out, which a copy removed from outside never takes:
//  - the silver star Goomba (daKrb_c, mStarTracked +0x465: TrackStar in InitResources for REWARD_SILVER_STAR, and in
//    func_ov084_02129864 for REWARD_SILVER_STAR_IF_CURRENT; UntrackStar only in func_ov084_021296cc and the swallow,
//    d_a_krb.cpp:594/613/1653/1661; CleanupResources has none, so a removed copy left the slot pointing at a freed Goomba);
//  - the brick block's silver star tag (daObjBlockItemTag_c 0x144 BLK_SLVSTAR_TAG, mTrackStarID +0xd5, attached to the
//    block through daObjBlockL_c::mLinkedActor +0x328): UntrackStar only in SpawnSilverStar, run when the block's Kill
//    fires it. A block removed from outside left its tag standing, attached and glinting, for the whole visit.
// The star itself is announced by the breaker / beater (COOPSPAWN1), so here the holder only lets go of its glint.
enum : int { kKrbStarTracked = 0x465, kKrbParam = 0x08, kTagTrack = 0xd5, kTagAttached = 0xd8, kBlockLinked = 0x328, kClsSilverTag = 0x144 };
bool g_glint_off = false, g_glint_env = false;     // SM64DS_COOP_GLINT_OFF=1: must-fail control, removed holders keep their glint
bool glint_off() {
    if (!g_glint_env) { g_glint_env = true; const char *e = std::getenv("SM64DS_COOP_GLINT_OFF"); g_glint_off = e && *e && *e != '0'; }
    return g_glint_off;
}
int glints_held() { int n = 0; for (int m = 0; m < 12; ++m) if (STAR_MARKERS[m]) ++n; return n; }
void untrack_field(char *a, int off) {
    signed char &sl = *(signed char *)(a + off);
    if (sl >= 0 && sl < 12 && STAR_MARKERS[(int)sl] == a) _ZN8dActor_c11UntrackStarERa(a, sl);
    else if (sl >= 0) sl = -1;
}
// A friend's Goomba is removed here: let go of its glint. With `respawn` (a capped Goomba about to leave its dormant
// replacement at home, RespawnIfHasCap), clear the reward from its parameter the way func_ov084_021296cc does when the
// star comes out, so the replacement does not claim a glint again.
void goomba_glint_off(char *a, bool respawn, const char *why) {
    if (glint_off()) return;
    const signed char tracked = *(const signed char *)(a + kKrbStarTracked);
    const unsigned reward = *(const unsigned char *)(a + kKrbReward);
    const bool had = tracked >= 0;
    if (had) untrack_field(a, kKrbStarTracked);
    if (respawn && (reward == 1 || (reward == 2 && had))) *(unsigned *)(a + kKrbParam) &= 0xff0fu;
    if (had || (respawn && reward && reward != 3))
        std::fprintf(stderr, "[coopsilver] glint: Goomba slot %d (reward %u) let go of its silver star glint (%s)%s: %d held wall=%lld\n",
                     (int)*(const short *)(a + 0xce), reward, why, respawn ? ", replacement without the reward" : "", glints_held(), wall_ms());
}
// A friend broke this brick block: its silver star tag lets go of the glint and goes (the star is the breaker's, announced).
void block_glint_off(char *blk, const char *why) {
    if (glint_off()) return;
    const unsigned cls = *(const unsigned short *)(blk + 0x0c);
    if (cls != 15 && cls != 16 && cls != 17) return;
    char *tag = *(char **)(blk + kBlockLinked);
    if (tag && !(is_live(tag) && *(const unsigned short *)(tag + 0x0c) == kClsSilverTag)) tag = nullptr;
    if (!tag) {      // not attached yet: the tag's Behavior attaches to a block within 0x32000 of it (d_a_obj_block_item_tag.cpp)
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && !tag; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
            if (*(const unsigned short *)(a + 0x0c) != kClsSilverTag || going(a)) continue;
            long long d2 = 0;
            for (int i = 0; i < 3; ++i) { const long long d = (long long)*(const int *)(a + 0x5c + 4 * i) - *(const int *)(blk + 0x5c + 4 * i); d2 += d * d; }
            if (d2 < 0x32000ll * 0x32000ll) tag = a;
        }
    }
    if (!tag || going(tag)) return;
    untrack_field(tag, kTagTrack);
    untrack_markers(tag);
    _ZN7fBase_c18MarkForDestructionEv(tag);
    std::fprintf(stderr, "[coopsilver] glint: block class %u slot %d: its silver star tag let go of the glint and went (%s): %d held wall=%lld\n",
                 cls, (int)*(const short *)(blk + 0xce), why, glints_held(), wall_ms());
}
// A holder of those two classes that is on its way out while still holding a glint (a tag whose block was already gone
// when it looked for it ends itself in Behavior; a Goomba removed by a path above this one) lets go of it now, before the
// slot points at freed memory. Only live actors are read.
void glint_sweep() {
    if (glint_off()) return;
    for (int m = 0; m < 12; ++m) {
        char *p = (char *)STAR_MARKERS[m];
        if (!p || !is_live(p) || !going(p)) continue;
        const unsigned cls = *(const unsigned short *)(p + 0x0c);
        int off = -1;
        if (cls == kClsSilverTag) off = kTagTrack;
        else if (is_goomba(cls)) off = kKrbStarTracked;
        if (off < 0) continue;
        signed char sl = (signed char)m;
        _ZN8dActor_c11UntrackStarERa(p, sl);
        if (*(const signed char *)(p + off) == m) *(signed char *)(p + off) = -1;
        std::fprintf(stderr, "[coopsilver] glint: class %u leaving with glint slot %d: freed wall=%lld\n", cls, m, wall_ms());
    }
}
enum : int { kGoneMax = 32, kGonePushes = 4 };
struct Gone { uint16_t slot, cls; };
Gone g_gone[kGoneMax]; int g_gone_n = 0;
unsigned g_gone_push_at = 0; int g_gone_push_left = 0;
void gone_note(int slot, unsigned cls) {
    for (int i = 0; i < g_gone_n; ++i) if (g_gone[i].slot == slot && g_gone[i].cls == cls) return;
    if (g_gone_n < kGoneMax) g_gone[g_gone_n++] = {(uint16_t)slot, (uint16_t)cls};
}
// The ROOM keeps them too, per (level, act): the breaker usually drops straight through the hole he opened, so by the time
// his friend walks in nobody is left in the level to tell him. Every console stores every break it hears about (its own
// group's or another level's) and starts each level visit with that level's entries. They last only while somebody in the
// room has that level loaded (room_forget below): once it has been empty for a moment every console forgets them, and the
// next visit finds every block whole, star blocks and silver star blocks with their items, as on the cartridge.
enum : int { kRoomGoneMax = 96 };
struct RoomGone { int16_t level; uint8_t act; uint16_t slot, cls; };
RoomGone g_room_gone[kRoomGoneMax]; int g_room_gone_n = 0;
void room_gone_note(const CoopKey &k, int slot, unsigned cls) {
    for (int i = 0; i < g_room_gone_n; ++i) {
        const RoomGone &r = g_room_gone[i];
        if (r.level == k.level && r.act == (uint8_t)k.act && r.slot == slot && r.cls == cls) return;
    }
    if (g_room_gone_n < kRoomGoneMax) g_room_gone[g_room_gone_n++] = {(int16_t)k.level, (uint8_t)k.act, (uint16_t)slot, (uint16_t)cls};
}
// Who still has a level. A seat has (level, act) while it is in it, and it keeps the level it came straight from while it
// is in a course it reached from there (not one of the castle levels 1 to 5): the breaker who drops through the garden
// grate into the course below, or goes through the freed cage into Big Boo's Haunt, still has the garden under him, so a
// friend who walks in later finds it the way he left it. Walking out into the castle lets go of it. The level it came from
// is matched by level alone: the act a console shows can move to the next one just before it leaves.
bool castle_level(int level) { return level >= 1 && level <= 5; }
bool seat_has(const CoopKey &cur, const CoopKey &prev, const CoopKey &k) {
    return cur == k || (prev.valid() && prev.level == k.level && cur.valid() && cur.level != k.level && !castle_level(cur.level));
}
CoopKey g_pres_cur[kCoopSeats], g_pres_prev[kCoopSeats];   // every seat's level and the one before it, from presence
CoopKey g_my_cur, g_my_prev;                                 // mine, from my own level changes
// Every room_forget run: note each seat's level change (a new level, not a new act in the same level).
void pres_track() {
    for (int s = 0; s < kCoopSeats; ++s) {
        if (s == g_my) continue;
        HubPresence p; unsigned age = 0;
        if (!hub_presence_get(s, &p, &age) || !p.in_level() || age >= kPresenceMs) continue;
        const CoopKey k(p.level, p.act);
        if (k.level != g_pres_cur[s].level) g_pres_prev[s] = g_pres_cur[s];
        g_pres_cur[s] = k;
    }
}
// A seat other than mine that has (level, act), from its fresh presence (a seat that dropped out goes stale and stops
// counting), or -1.
int room_seat_in(const CoopKey &k) {
    for (int s = 0; s < kCoopSeats; ++s) {
        if (s == g_my) continue;
        HubPresence p; unsigned age = 0;
        if (!hub_presence_get(s, &p, &age) || !p.in_level() || age >= kPresenceMs) continue;
        const CoopKey cur(p.level, p.act);
        const CoopKey &prev = cur.level == g_pres_cur[s].level ? g_pres_prev[s] : g_pres_cur[s];
        if (seat_has(cur, prev, k)) return s;
    }
    return -1;
}
bool i_have(const CoopKey &k) { return seat_has(g_my_cur, g_my_prev, k); }
void room_gone_load(const CoopKey &k) {   // a new level visit: its broken blocks onto the group list
    for (int i = 0; i < g_room_gone_n; ++i) {
        const RoomGone &r = g_room_gone[i];
        if (r.level == k.level && r.act == (uint8_t)k.act) gone_note(r.slot, r.cls);
    }
    if (g_gone_n) {   // a death or a reload while a friend stayed: the level never emptied, so its blocks stay broken
        const int s = room_seat_in(k);
        if (s >= 0) std::fprintf(stderr, "[coop] block list: level %d act %d starts with %d broken block(s) from the room (seat %d still has it"
                                         " loaded, it never emptied) wall=%lld\n", k.level, k.act, g_gone_n, s, wall_ms());
        else std::fprintf(stderr, "[coop] block list: level %d act %d starts with %d broken block(s) from the room wall=%lld\n", k.level, k.act,
                          g_gone_n, wall_ms());
    }
}

// COOPBOO1: the room's freed cage Boo cages, per (level, act), filled and read by the cage code further down.
enum : int { kRoomCageMax = 16 };
struct RoomCage { int16_t level; uint8_t act; uint16_t slot; int pos[3]; int8_t area; };
RoomCage g_room_cage[kRoomCageMax]; int g_room_cage_n = 0;

// Tango's rule: the room remembers a level's broken blocks (and its freed cage) only while somebody has that level loaded,
// and the next visit after it emptied starts the way the cartridge builds it. Every console judges it alone from what it
// already has (its own level and every seat's presence), so a console that left earlier forgets too, a seat that drops out
// stops counting when its presence goes stale, and nothing new goes on the wire. kRoomEmptyMs of nobody rides out a
// presence packet or two going missing. "Has it loaded" includes the course a player dropped into from it (seat_has).
// A stale push cannot bring entries back: the list is only sent to a member joining a
// group, and a group has somebody in it.
enum : unsigned { kRoomEmptyMs = 3000, kRoomEmptyMax = 16 };
struct RoomEmpty { CoopKey k; unsigned since; };
RoomEmpty g_room_empty[kRoomEmptyMax]; int g_room_empty_n = 0;
void room_forget_key(const CoopKey &k) {
    int blocks = 0, cages = 0;
    for (int i = 0; i < g_room_gone_n; ++i)
        if (g_room_gone[i].level == k.level && g_room_gone[i].act == (uint8_t)k.act) { g_room_gone[i--] = g_room_gone[--g_room_gone_n]; ++blocks; }
    for (int i = 0; i < g_room_cage_n; ++i)
        if (g_room_cage[i].level == k.level && g_room_cage[i].act == (uint8_t)k.act) { g_room_cage[i--] = g_room_cage[--g_room_cage_n]; ++cages; }
    std::fprintf(stderr, "[coop] block list: nobody in the room has level %d act %d loaded: forgot %d broken block(s) and %d freed cage(s),"
                         " the next visit starts whole wall=%lld\n", k.level, k.act, blocks, cages, wall_ms());
}
void room_forget(unsigned now) {
    pres_track();
    CoopKey keys[kRoomGoneMax + kRoomCageMax]; int nk = 0;
    for (int i = 0; i < g_room_gone_n + g_room_cage_n; ++i) {
        const CoopKey k = i < g_room_gone_n ? CoopKey(g_room_gone[i].level, g_room_gone[i].act)
                                            : CoopKey(g_room_cage[i - g_room_gone_n].level, g_room_cage[i - g_room_gone_n].act);
        bool seen = false;
        for (int j = 0; j < nk && !seen; ++j) seen = keys[j] == k;
        if (!seen) keys[nk++] = k;
    }
    for (int i = 0; i < g_room_empty_n; ++i) {   // a level that has nothing left to forget, or has somebody again
        bool listed = false;
        for (int j = 0; j < nk && !listed; ++j) listed = keys[j] == g_room_empty[i].k;
        if (!listed || i_have(g_room_empty[i].k) || room_seat_in(g_room_empty[i].k) >= 0) g_room_empty[i--] = g_room_empty[--g_room_empty_n];
    }
    for (int j = 0; j < nk; ++j) {
        if (i_have(keys[j]) || room_seat_in(keys[j]) >= 0) continue;
        int e = -1;
        for (int i = 0; i < g_room_empty_n && e < 0; ++i) if (g_room_empty[i].k == keys[j]) e = i;
        if (e < 0) { if (g_room_empty_n < (int)kRoomEmptyMax) g_room_empty[g_room_empty_n++] = {keys[j], now}; continue; }
        if ((unsigned)(now - g_room_empty[e].since) < kRoomEmptyMs) continue;
        room_forget_key(keys[j]);
        g_room_empty[e] = g_room_empty[--g_room_empty_n];
    }
}

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
    // Alone in the level: a broken block still goes out, so a friend on another level files it for a later visit (COOPSYNC1).
    const bool alone = g_grp.others() == 0;
    int k = 0;
    for (int i = 0; i < g_sw_n; ++i) {
        SwOut &o = g_sw_out[i];
        if (alone && !breakable_block(o.cls) && !o.boo) continue;
        if ((int)(now - o.next) >= 0) {
            CoopBooWire w; std::memset(&w, 0, sizeof w);
            w.head.key = wire_key(g_key); w.head.slot = o.slot; w.head.cls = o.cls; w.count = -1;
            if (send_kind(kHubTagCoopWorld, kWorldSwallowGone, &w, o.boo ? (int)sizeof w : (int)sizeof w.head)) {
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
enum : int { kCapMax = 64, kCapMass = 4 };
struct CapW { char *a; uint16_t slot, cls; };
CapW g_cap[kCapMax]; int g_cap_n = 0; unsigned g_cap_frame = 0; int g_cap_slotted = -1;
uint16_t g_sw_applied[kSwOutMax]; int g_sw_applied_n = 0;   // slots a friend's message removed here: never echoed
bool capped_goomba(const char *a) {
    const unsigned cls = *(const unsigned short *)(a + 0x0c);
    if (!is_goomba(cls)) return false;
    return (*(const unsigned char *)(a + kKrbCapId) & 0xf) < 6 || *(const unsigned char *)(a + kKrbReward) == 2;
}
void cage_reset();
void boo_reset();
void cap_reset() { g_cap_n = 0; g_cap_slotted = -1; g_sw_applied_n = 0; g_gone_n = 0; g_gone_push_left = 0; cage_reset(); boo_reset(); }
void watch_capped(unsigned now) {
    CapW cur[kCapMax]; int n = 0, slotted = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        const short slot = *(const short *)(a + 0xce);
        if (slot < 0 || slot >= kCoopSlots || going(a) || is_player_ptr(a)) continue;
        ++slotted;
        if (n < kCapMax && (capped_goomba(a) || breakable_block(*(const unsigned short *)(a + 0x0c)) ||
                            live_gone(*(const unsigned short *)(a + 0x0c))))
            cur[n++] = {a, (uint16_t)slot, *(const unsigned short *)(a + 0x0c)};
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
        if (!bit && breakable_block(o.cls)) { gone_note(slot, o.cls); room_gone_note(g_key, slot, o.cls); }
        if (!bit && live_gone(o.cls)) gone_note(slot, o.cls);   // COOPWALLS1: this visit's list only
        std::fprintf(stderr, "[coop] %s watch: class %u slot %d went away here %s wall=%lld\n",
                     breakable_block(o.cls) ? "block" : live_gone(o.cls) ? "boss" : "capped", (unsigned)o.cls, slot,
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
void apply_swallow(int seat, int slot, unsigned cls, bool boo_beaten = false) {
    char *a = live_slot(cls, slot);
    if (boo_beaten) {   // COOPBOO2: a Boo the friend beat (no death bit): count it here, then remove my copy
        gone_note(slot, cls); room_gone_note(g_key, slot, cls);
        const bool live = a && !going(a) && !is_player_ptr(a);
        if (live && boo_beaten_here(a)) {   // both hit it: this console's own defeat counts it once
            // COOPBOO3: not marked here; watch_boos marks it when my count really rises, and if my talk ends uncounted,
            // gone_apply counts it as his kill (the slot is on the gone list).
            if (g_log) std::fprintf(stderr, "[coop] boo: seat %d beat Boo slot %d, it is beaten here too (counted once) wall=%lld\n", seat, slot, wall_ms());
            return;
        }
        if (boo_counted(slot)) {   // a repeat of the message, or a copy that stood after the count: remove it only
            if (live) _ZN7fBase_c18MarkForDestructionEv(a);
            return;
        }
        if (!live) {   // no copy here: count it on the Big Boo now, or (no Big Boo yet) gone_apply does
            if (!boo_credit_nocopy(slot, seat, "his kill"))
                if (g_log) std::fprintf(stderr, "[coop] boo: seat %d beat Boo slot %d, no copy and no Big Boo here yet wall=%lld\n", seat, slot, wall_ms());
            return;
        }
        if (count_boo(a) && !boo_credit(a, seat, "his kill")) return;   // no Big Boo yet: gone_apply counts it once it stands
        _ZN7fBase_c18MarkForDestructionEv(a);
        if (g_sw_applied_n < kSwOutMax) g_sw_applied[g_sw_applied_n++] = (uint16_t)slot;
        if (g_log) std::fprintf(stderr, "[coop] boo: seat %d beat Boo slot %d, removed here wall=%lld\n", seat, slot, wall_ms());
        return;
    }
    if (breakable_block(cls)) { gone_note(slot, cls); room_gone_note(g_key, slot, cls); }   // kept even with no copy standing yet
    if (live_gone(cls)) gone_note(slot, cls);   // COOPWALLS1: Big Mr. I / Chill Bully, this visit only
    if (!a || going(a) || is_player_ptr(a)) {
        if (g_log) std::fprintf(stderr, "[coop] swallow from seat %d: class %u slot %d, no live copy here\n", seat, cls, slot);
        return;
    }
    release_mouth_of(a);
    if (is_goomba(cls)) {
        const int cap = *(const unsigned char *)(a + kKrbCapId) & 0xf;
        goomba_glint_off(a, cap < 6 || *(const unsigned char *)(a + kKrbReward) == 2, "swallow message");     // COOPSILVER3
        if (cap < 6 || *(const unsigned char *)(a + kKrbReward) == 2) {
            std::memcpy(a + 0x5c, a + kKrbHome, 12);
            _ZN11dCapEnemy_c15RespawnIfHasCapEv(a);
        }
        func_ov084_02129498(a);
    } else {
        if (live_gone(cls)) untrack_markers(a);
        block_glint_off(a, "friend's break");      // COOPSILVER3
        _ZN7fBase_c18MarkForDestructionEv(a);
    }
    if (g_sw_applied_n < kSwOutMax) g_sw_applied[g_sw_applied_n++] = (uint16_t)slot;
    else { std::memmove(g_sw_applied, g_sw_applied + 1, sizeof g_sw_applied - sizeof g_sw_applied[0]); g_sw_applied[kSwOutMax - 1] = (uint16_t)slot; }
    std::fprintf(stderr, "[coop] swallow from seat %d: class %u slot %d removed here the way his console removed it (no death bit) wall=%lld\n",
                 seat, cls, slot, wall_ms());
    if (g_log) g_sw_census_at = g_est_frames + 900;
}

// COOPSYNC1: a block on the group's broken list that stands here (it spawned after the break: I entered later, or the
// message came before my objects) goes the same way. Every 10 est frames; a block never respawns inside one level visit.
unsigned g_boo_visit = 0;   // COOPBOO2: est frames of this visit (boo_reset clears it)
void gone_apply() {
    for (int i = 0; i < g_gone_n; ++i) {
        char *a = live_slot(g_gone[i].cls, g_gone[i].slot);
        if (g_gone[i].cls == 209 && !boo_counted(g_gone[i].slot) && (!a || going(a)) && g_boo_visit >= 60)
            boo_credit_nocopy(g_gone[i].slot, -1, "beaten earlier");   // no copy stands here: count it once the Big Boo stands
        if (!a || going(a) || is_player_ptr(a)) continue;
        if (g_gone[i].cls == 209) {   // COOPBOO2: a lobby Boo a friend beat earlier in this visit: count it, then remove it
            if (boo_beaten_here(a)) continue;   // its own defeat runs on this console (my kill): the cartridge counts it
            if (boo_counted(g_gone[i].slot)) {
                _ZN7fBase_c18MarkForDestructionEv(a);
                if (g_log) std::fprintf(stderr, "[coop] boo: slot %d already counted here, its copy removed wall=%lld\n", (int)g_gone[i].slot, wall_ms());
                continue;
            }
            if (!count_boo(a) || !boo_credit(a, -1, "beaten earlier")) { if (g_boo_visit < 300) continue; }   // wait up to 5 s for the Big Boo
            _ZN7fBase_c18MarkForDestructionEv(a);
            if (g_log) std::fprintf(stderr, "[coop] boo: slot %d beaten in this group, removed here wall=%lld\n", (int)g_gone[i].slot, wall_ms());
            continue;
        }
        if (live_gone(g_gone[i].cls)) untrack_markers(a);
        block_glint_off(a, "broken list");      // COOPSILVER3
        _ZN7fBase_c18MarkForDestructionEv(a);
        if (g_sw_applied_n < kSwOutMax) g_sw_applied[g_sw_applied_n++] = g_gone[i].slot;
        else { std::memmove(g_sw_applied, g_sw_applied + 1, sizeof g_sw_applied - sizeof g_sw_applied[0]); g_sw_applied[kSwOutMax - 1] = g_gone[i].slot; }
        std::fprintf(stderr, "[coop] block list: class %u slot %d broken in this group, removed here wall=%lld\n",
                     (unsigned)g_gone[i].cls, (int)g_gone[i].slot, wall_ms());
    }
}

// COOPSYNC1: a member joined my group: send him the broken list (the swallow message once per block, kGonePushes rounds
// 700 ms apart from 300 ms after he showed up, so one round lands after his objects stand). A repeat finds no copy.
void gone_push(unsigned now) {
    if (g_gone_push_left <= 0 || g_gone_n == 0 || (int)(now - g_gone_push_at) < 0) return;
    if (g_grp.others() == 0) { g_gone_push_left = 0; return; }
    for (int i = 0; i < g_gone_n; ++i) {
        CoopBooWire w; std::memset(&w, 0, sizeof w);
        w.head.key = wire_key(g_key); w.head.slot = g_gone[i].slot; w.head.cls = g_gone[i].cls; w.count = -1;
        send_kind(kHubTagCoopWorld, kWorldSwallowGone, &w, g_gone[i].cls == 209 ? (int)sizeof w : (int)sizeof w.head);
    }
    if (g_log) std::fprintf(stderr, "[coop] block list: %d broken block(s) sent to the group (round %d)\n", g_gone_n,
                            kGonePushes - g_gone_push_left + 1);
    --g_gone_push_left;
    g_gone_push_at = now + 700u;
}

// ---------------------------------------------------------------------------
// The courtyard's cage Boo (run after093, lane COOPBOO1). In the castle garden (level 3, 15 stars or more) the variant-5 Boo
// (daTrs_c, class 209, src/game/actors/d_a_trs.cpp) carries the cage (T_BASKET, class 212) that is the way into Big Boo's Haunt.
// When a player hits it, func_ov063_02116e14 spawns the cage at mCarriedPosX (unk_0a4 0, mVertSpeed 0x32000, unk_0ac 0), clears
// `carrying` and goes to state 3, which fades the Boo and ends in MarkForDestruction: no death bit. So the friend's Boo kept
// flying with its cage and the friend had no way in. The console where the cage Boo drops its cage says so with the swallow
// message plus the cage's spawn point (24 bytes: an older build drops a message of that length as malformed and keeps its own
// behaviour); every console in the level runs the same drop on its copy (same spawn, same fields, the Boo's own state 3), and
// the room keeps it for the session like the garden block, so a friend who comes in later finds the cage the way the cartridge
// leaves it when the player comes back out of the Haunt (InitResources, LAST_ENTRANCE_ID 0: the cage stands at once, its
// secret jingle muted, the Boo removed). Two players beating their copies at once each keep their own cage: nothing doubles.
// ---------------------------------------------------------------------------
enum : int { kBooCls = 209, kCageCls = 212, kTrsState = 0x5cc, kTrsVariant = 0x5cf, kTrsFlags = 0x5d4, kTrsCarrying = 0x2,
             kBasketMute = 0x37e, kCageWireLen = 24, kCagePosLimit = 1 << 28 };
struct CoopCageWire {                      // 24 B: the swallow message (cls 209) and where the cage was spawned
    CoopSwallowWire head;
    int32_t x, y, z;                       // the cage's spawn point, ROM fixed point
    int8_t area;                           // the area the Boo spawned it in (its mAreaIdx)
    uint8_t pad[3];
};
static_assert(sizeof(CoopCageWire) == kCageWireLen, "cage wire is 24 bytes");
struct CageState { bool free; int slot; int pos[3]; int area; };
CageState g_cage = {};                     // this level visit: the cage is out (dropped here, by a friend, or earlier in the room)
char *g_cage_boo = nullptr; unsigned g_cage_boo_uid = 0; bool g_cage_boo_carry = false;
int g_cage_send_left = 0; unsigned g_cage_send_at = 0, g_cage_send_gap = 120;

void cage_reset() { g_cage = CageState(); g_cage_boo = nullptr; g_cage_boo_uid = 0; g_cage_boo_carry = false; g_cage_send_left = 0; }
void room_cage_note(const CoopKey &k, int slot, const int *pos, int area) {
    for (int i = 0; i < g_room_cage_n; ++i)
        if (g_room_cage[i].level == k.level && g_room_cage[i].act == (uint8_t)k.act && g_room_cage[i].slot == slot) return;
    if (g_room_cage_n < kRoomCageMax)
        g_room_cage[g_room_cage_n++] = {(int16_t)k.level, (uint8_t)k.act, (uint16_t)slot, {pos[0], pos[1], pos[2]}, (int8_t)area};
}
void room_cage_load(const CoopKey &k) {   // a new level visit: a cage freed here earlier in the room is out again
    for (int i = 0; i < g_room_cage_n; ++i) {
        const RoomCage &r = g_room_cage[i];
        if (r.level != k.level || r.act != (uint8_t)k.act) continue;
        g_cage.free = true; g_cage.slot = r.slot; g_cage.area = r.area;
        for (int j = 0; j < 3; ++j) g_cage.pos[j] = r.pos[j];
        if (g_log) std::fprintf(stderr, "[coop] cage: level %d act %d: the cage Boo's cage was freed earlier in this room\n", k.level, k.act);
    }
}
bool cage_is_boo(const char *a) {
    return *(const unsigned short *)(a + 0x0c) == kBooCls && *(const unsigned char *)(a + kTrsVariant) == 5 &&
           *(const short *)(a + 0xce) >= 0 && *(const short *)(a + 0xce) < kCoopSlots;
}
bool cage_carrying(const char *a) { return (*(const unsigned short *)(a + kTrsFlags) & kTrsCarrying) != 0; }
char *cage_newest() {
    char *best = nullptr; unsigned uid = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if (*(const unsigned short *)(a + 0x0c) == kCageCls && !going(a) && *(const unsigned *)(a + 0x04) >= uid) { best = a; uid = *(const unsigned *)(a + 0x04); }
    return best;
}
// The cage the way func_ov063_02116e14 spawns it (drop) or InitResources' free cage (muted).
char *cage_spawn(const int *pos, int area, bool mute) {
    char *c = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as((unsigned)kCageCls, 0, pos, nullptr, area, -1);
    if (c) {
        *(int *)(c + 0xa4) = 0; *(int *)(c + 0xa8) = 0x32000; *(int *)(c + 0xac) = 0;
        if (mute) *(unsigned char *)(c + kBasketMute) = 1;
    }
    return c;
}
void cage_send(unsigned now) {
    if (g_cage_send_left <= 0 || !g_cage.free || (int)(now - g_cage_send_at) < 0) return;
    CoopCageWire w; std::memset(&w, 0, sizeof w);
    w.head.key = wire_key(g_key); w.head.slot = (uint16_t)g_cage.slot; w.head.cls = (uint16_t)kBooCls;
    w.x = g_cage.pos[0]; w.y = g_cage.pos[1]; w.z = g_cage.pos[2]; w.area = (int8_t)g_cage.area;
    if (send_kind(kHubTagCoopWorld, kWorldSwallowGone, &w, (int)sizeof w)) {
        --g_cage_send_left;
        g_cage_send_at = now + g_cage_send_gap;
        if (g_cage_send_gap < 700u) g_cage_send_gap = 240u;
    }
}
// A friend's console (or the room) says the cage is out: my copy of the cage Boo drops it the same way.
void cage_apply_remote(int seat, int slot, const int *pos, int area) {
    if (g_cage.free) {
        if (g_log) std::fprintf(stderr, "[coop] cage: from seat %d: my cage is already out\n", seat);
        return;
    }
    g_cage.free = true; g_cage.slot = slot; g_cage.area = area;
    for (int j = 0; j < 3; ++j) g_cage.pos[j] = pos[j];
    room_cage_note(g_key, slot, pos, area);
    char *b = live_slot((unsigned)kBooCls, slot);
    if (b && !going(b) && cage_is_boo(b) && cage_carrying(b)) {
        cage_spawn(pos, area, false);
        *(unsigned short *)(b + kTrsFlags) &= (unsigned short)~kTrsCarrying;
        *(unsigned char *)(b + kTrsState) = 3;
        g_cage_boo = b; g_cage_boo_uid = *(const unsigned *)(b + 0x04); g_cage_boo_carry = false;
        std::fprintf(stderr, "[coop] cage: from seat %d: cage Boo slot %d dropped its cage here at %d,%d,%d the way his did wall=%lld\n",
                     seat, slot, pos[0] >> 12, pos[1] >> 12, pos[2] >> 12, wall_ms());
    } else if (!cage_newest()) {
        cage_spawn(pos, area, false);
        std::fprintf(stderr, "[coop] cage: from seat %d: no carrying cage Boo here, the cage spawned at %d,%d,%d wall=%lld\n",
                     seat, pos[0] >> 12, pos[1] >> 12, pos[2] >> 12, wall_ms());
    }
}
// COOPBOO2: every est frame: a lobby Boo (variant 2) I beat goes into its state-5 talk with my player and ends in
// MarkForDestruction with no death bit. Tell the group once (the 12-byte Boo form of the swallow message, which an older build ignores),
// and keep it on the group and room lists so a member who comes in later counts it too.
uint16_t g_boo_told[16]; int g_boo_told_n = 0;
void boo_reset() { g_boo_told_n = 0; g_boo_visit = 0; g_boo_counted_n = 0; }
int g_me_char = -1, g_me_param = -1;
void watch_boos(unsigned now) {
    ++g_boo_visit;
    const int me = (int)data_0209f250;
    const char *body = (me >= 0 && me < 16) ? (const char *)data_0209f394[me] : nullptr;
    if (!body) return;
    {   // COOPBOO3: the character fields the cap enemy checks read (fBase_c param1 and Player::mCharacter), logged on a change
        const int ch = *(const unsigned char *)(body + 0x6d9), pa = (int)(*(const unsigned *)(body + 0x08) & 0xff);
        if (ch != g_me_char || pa != g_me_param) {
            g_me_char = ch; g_me_param = pa;
            if (g_log) std::fprintf(stderr, "[coop] me: my body's param1 %d, character %d (est %u)\n", pa, ch, g_est_frames);
        }
    }
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (*(const unsigned short *)(a + 0x0c) != 209 || *(const unsigned char *)(a + kBooVar) != 2) continue;
        const short slot = *(const short *)(a + 0xce);
        if (slot < 0 || slot >= kCoopSlots || !*(const unsigned *)(a + kBooBig)) continue;
        if (*(const unsigned char *)(a + kBooState) != 5 || *(const char *const *)(a + kBooTalk) != body) continue;
        // COOPBOO3: the cartridge adds the Boo to its Big Boo's count in talk substate 1 (once StartTalk took) and moves to 2. A
        // Boo still waiting in substate 0 can leave the talk uncounted (seen when the player moved away right after a second
        // kill), so it is told and marked only once it is really counted here.
        if (*(const unsigned char *)(a + kBooSub) < 2) continue;
        bool told = false;
        for (int i = 0; i < g_boo_told_n && !told; ++i) told = g_boo_told[i] == (uint16_t)slot;
        if (told) continue;
        if (g_boo_told_n < 16) g_boo_told[g_boo_told_n++] = (uint16_t)slot;
        boo_mark(slot);
        gone_note(slot, 209); room_gone_note(g_key, slot, 209);
        if (g_sw_n < kSwOutMax) g_sw_out[g_sw_n++] = {(uint16_t)slot, 209, 0, now, 1};
        const char *big = big_boo_of(a);
        if (g_log) std::fprintf(stderr, "[coop] boo: I beat Boo slot %d (Big Boo count %d here): told the group wall=%lld\n", (int)slot,
                     big ? *(const int *)(big + kBooCount) : -1, wall_ms());
    }
}

// Proof knob SM64DS_COOP_BOOHIT=EST:SLOT[,SLOT...] (COOPBOO2): from that est frame, the first listed Boo (class 209) still
// standing is hunted: every 150 est frames my player is put 80 units east of it and its room is shown (Big Boo's Haunt
// keeps a Boo at home while its room is hidden, and only doors show rooms), and while it chases (state 1) its collider
// reads as punched by my player, so the cartridge's own hit and defeat path runs (the talk, the count, the Big Boo).
// Once none of them stands, my player waits 400 units from the Big Boo the small Boos report to (it only shows itself to a
// player within 742 units, func_ov063_0211ad00), with its room shown, so its coming out is on this console's pictures.
// SM64DS_COOP_BOOHIT_ALL=1 (COOPBOO3): every listed Boo that chases is punched in the same frame, not only the first, so
// two of them are beaten together (the second waits for its talk behind the first one's message box).
struct BooHitKnob { unsigned at; int n; short slot[8]; bool all; };
BooHitKnob g_boohit = {0, 0, {}, false};
void boohit_knob() {
    static const bool parsed = [] {
        if (const char *e = std::getenv("SM64DS_COOP_BOOHIT")) {
            unsigned at = 0; int used = 0;
            if (std::sscanf(e, "%u:%n", &at, &used) >= 1 && used > 0) {
                g_boohit.at = at;
                for (const char *p = e + used; *p && g_boohit.n < 8;) {
                    int v = 0, u = 0;
                    if (std::sscanf(p, "%d%n", &v, &u) < 1 || u <= 0) break;
                    g_boohit.slot[g_boohit.n++] = (short)v; p += u;
                    while (*p == ',') ++p;
                }
            }
        }
        const char *all = std::getenv("SM64DS_COOP_BOOHIT_ALL");
        g_boohit.all = all && *all && *all != '0';
        return true;
    }();
    (void)parsed;
    if (!g_boohit.n || !g_boohit.at || g_est_frames < g_boohit.at) return;
    const int me = (int)data_0209f250;
    char *body = (me >= 0 && me < 16) ? (char *)data_0209f394[me] : nullptr;
    if (!body) return;
    if (g_boohit.all)
        for (int i = 0; i < g_boohit.n; ++i) {
            char *b = live_slot(209u, g_boohit.slot[i]);
            if (!b || going(b) || *(const unsigned char *)(b + kBooState) != 1) continue;
            *(unsigned *)(b + 0x184 + 0x20) |= 0x40u;
            *(unsigned *)(b + 0x184 + 0x24) = *(const unsigned *)(body + 0x04);
        }
    for (int i = 0; i < g_boohit.n; ++i) {
        char *b = live_slot(209u, g_boohit.slot[i]);
        if (!b || going(b) || *(const unsigned char *)(b + kBooState) >= 3) continue;
        if (*(const unsigned char *)(b + kBooState) == 1) {
            *(unsigned *)(b + 0x184 + 0x20) |= 0x40u;
            *(unsigned *)(b + 0x184 + 0x24) = *(const unsigned *)(body + 0x04);
            static unsigned last = 0;
            if (last + 60u < g_est_frames || last > g_est_frames) {
                last = g_est_frames;
                std::fprintf(stderr, "[coop-knob] boo hit: Boo slot %d reads as punched by my player (est %u)" "\n", (int)g_boohit.slot[i], g_est_frames);
            }
        } else if ((g_est_frames - g_boohit.at) % 150u == 0) {
            const int *bp = (const int *)(b + 0x5c);
            int *pp = (int *)(body + 0x5c);
            pp[0] = bp[0] + (80 << 12); pp[1] = bp[1]; pp[2] = bp[2];
            const signed char area = *(const signed char *)(b + 0x5d0);
            if (area >= 0 && data_02092120 != area) ChangeArea(area);
            std::fprintf(stderr, "[coop-knob] boo hunt: put by Boo slot %d at %d,%d,%d (est %u)" "\n", (int)g_boohit.slot[i], bp[0] >> 12, bp[1] >> 12, bp[2] >> 12, g_est_frames);
        }
        return;
    }
    if ((g_est_frames - g_boohit.at) % 150u != 0) return;
    for (char *b = (char *)_ZN8dActor_c4NextEPKS_(0); b; b = (char *)_ZN8dActor_c4NextEPKS_(b)) {
        if (*(const unsigned short *)(b + 0x0c) != 210) continue;
        const unsigned char v = *(const unsigned char *)(b + 0x5cf);
        if (v != 0xd && v != 0xe) continue;
        const int *bp = (const int *)(b + 0x5c);
        int *pp = (int *)(body + 0x5c);
        const long long dx = (long long)(pp[0] - bp[0]) >> 12, dz = (long long)(pp[2] - bp[2]) >> 12;
        if (dx * dx + dz * dz < 600LL * 600LL) return;
        pp[0] = bp[0]; pp[1] = bp[1]; pp[2] = bp[2] + (400 << 12);
        const signed char area = *(const signed char *)(b + 0x5d0);
        if (area >= 0 && data_02092120 != area) ChangeArea(area);
        std::fprintf(stderr, "[coop-knob] boo hunt: my Boos are beaten, waiting by the Big Boo at %d,%d,%d (est %u)" "\n", bp[0] >> 12, bp[1] >> 12, bp[2] >> 12, g_est_frames);
        return;
    }
}

// COOPBOO2: the log line for the Big Boo (class 210, variant 0xd or 0xe) on this console: its Boo count, and when it is out (the cartridge
// spawned its icon actor, mSpawnedActorID at +0x49c). Logged when either changes, so both consoles can be read side by side.
int g_big_cnt = -1; bool g_big_out = false;
void log_big_boo() {
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (*(const unsigned short *)(a + 0x0c) != 210) continue;
        const unsigned char v = *(const unsigned char *)(a + 0x5cf);
        if (v != 0xd && v != 0xe) continue;   // the Big Boos small Boos report to (func_ov063_02119c58)
        const int cnt = *(const int *)(a + 0x180);
        const bool out = *(const unsigned *)(a + 0x49c) != 0;
        if (cnt != g_big_cnt || out != g_big_out) {
            g_big_cnt = cnt; g_big_out = out;
            if (g_log) std::fprintf(stderr, "[coop] boo: Big Boo here: count %d%s est %u wall=%lld\n", cnt, out ? ", it is out" : "", g_est_frames, wall_ms());
        }
        return;
    }
    g_big_cnt = -1; g_big_out = false;
}

// Every est frame: did my cage Boo drop its cage (tell the room), or does it still carry one that is already out (free it).
void watch_cage(unsigned now) {
    char *boo = nullptr;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && !boo; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if (cage_is_boo(a) && !going(a) && !is_player_ptr(a)) boo = a;
    if (!boo) { g_cage_boo = nullptr; g_cage_boo_carry = false; return; }
    const bool same = boo == g_cage_boo && *(const unsigned *)(boo + 0x04) == g_cage_boo_uid;
    const bool carry = cage_carrying(boo);
    // Proof knob SM64DS_COOP_CAGEHIT=EST: from that est frame, while my cage Boo chases (state 1) with its cage, its
    // collider reads as punched by my player, so the cartridge's own hit path (func_ov063_0211a0dc, 02116e14) runs.
    static const int hit_at = [] { const char *e = std::getenv("SM64DS_COOP_CAGEHIT"); return e ? std::atoi(e) : 0; }();
    if (hit_at > 0 && carry && (int)g_est_frames >= hit_at && *(const unsigned char *)(boo + kTrsState) == 1) {
        const int me = (int)data_0209f250;
        if (const char *body = (me >= 0 && me < 16) ? (const char *)data_0209f394[me] : nullptr) {
            *(unsigned *)(boo + 0x184 + 0x20) |= 0x40u;
            *(unsigned *)(boo + 0x184 + 0x24) = *(const unsigned *)(body + 0x04);
            static int told = 0;
            if (told++ < 3) std::fprintf(stderr, "[coop-knob] cage hit: cage Boo slot %d reads as punched by my player wall=%lld\n",
                                         (int)*(const short *)(boo + 0xce), wall_ms());
        }
    }
    if (same && g_cage_boo_carry && !carry && !g_cage.free) {
        if (char *c = cage_newest()) {
            g_cage.free = true; g_cage.slot = *(const short *)(boo + 0xce); g_cage.area = *(const signed char *)(c + 0xcc);
            for (int j = 0; j < 3; ++j) g_cage.pos[j] = ((const int *)(c + 0x5c))[j];
            room_cage_note(g_key, g_cage.slot, g_cage.pos, g_cage.area);
            g_cage_send_left = 3; g_cage_send_at = now; g_cage_send_gap = 120u;
            std::fprintf(stderr, "[coop] cage: cage Boo slot %d dropped its cage here at %d,%d,%d: told the room wall=%lld\n", g_cage.slot,
                         g_cage.pos[0] >> 12, g_cage.pos[1] >> 12, g_cage.pos[2] >> 12, wall_ms());
        }
    } else if (g_cage.free && carry) {
        if (!cage_newest()) cage_spawn(g_cage.pos, g_cage.area, true);
        _ZN7fBase_c18MarkForDestructionEv(boo);
        std::fprintf(stderr, "[coop] cage: freed earlier in this room: cage stands at %d,%d,%d, cage Boo slot %d removed here wall=%lld\n",
                     g_cage.pos[0] >> 12, g_cage.pos[1] >> 12, g_cage.pos[2] >> 12, (int)*(const short *)(boo + 0xce), wall_ms());
        g_cage_boo = nullptr; g_cage_boo_carry = false;
        return;
    }
    g_cage_boo = boo; g_cage_boo_uid = *(const unsigned *)(boo + 0x04); g_cage_boo_carry = carry;
}

// ---------------------------------------------------------------------------
// COOPCOUNT1 (run after093; audit G16 G17 G09). The level coin count and the secret coins, for every player.
//
// Coin count. The death-bit credit above shares a placed coin a friend took while my copy stands. It misses coins a friend
// took before I came in (they never spawn here) and coins that fall out of enemies (slot -1, never in a death bit). So
// every member of a group also tells its counter (kWorldCoinCount: at 1 Hz, when it changes, and the owner right after a
// joiner's snapshot) and every console takes max(mine, the group's), the way coopdx shares its level count. The raise is
// quiet: it never calls the ROM's 100-coin check (func_ov002_020b18f0), so the 100-coin star still comes from the console
// whose OWN pickup crosses 100 and is announced from there (coop_spawn.cpp). The message carries that console's "100-coin
// star is out" event bit (0x1f), so a console raised past 100 by the group never makes a second star at its next pickup.
// SM64DS_COOP_COINCOUNT=0 turns it off (must-fail control); SM64DS_COOP_COINS=0 turns off both.
//
// Secret coins (daSCoin_c, class 329, src/game/actors/d_a_s_coin.cpp:203-223). The five secrets' count lives in the
// group leader coin on each console and a collect ends in MarkForDestruction with no death bit, so a friend's secrets
// stayed on my screen and my count did not move. The console whose player collects one (its leader's count rises) tells
// the group the slot (kWorldSecret); every other console does to its copy what Collect does there, without the jingle and
// the number: the leader's mCollectedCount + 1, the coin's touch collider off, and the coin gone. Only the console whose
// player takes the fifth runs its 30-frame timer and spawns the star (SpawnStar, at the STARBASE marker); that star is a
// runtime spawn, so coop_spawn announces it and every other console gets that one copy (measured: a timer on every console
// made one star each, and each announced its own). The group keeps the taken secrets for this visit and sends them to a
// member who joins (like the broken blocks), who applies them as his coins stand up.
// ---------------------------------------------------------------------------
bool coin_count_off() {
    static const bool off = [] { const char *e = std::getenv("SM64DS_COOP_COINCOUNT"); return e && *e == '0'; }();
    return off;
}
int g_coin_group = 0;            // the highest count a member of my group told me for this (level, act)
bool g_coin_star = false;        // a member's 100-coin star is out
unsigned g_coin_sent_t = 0; int g_coin_sent_v = -1, g_coin_seen_v = -1;
unsigned long long g_coin_sends = 0, g_coin_recvs = 0, g_coin_raises = 0;
void coin_reset() {
    g_coin_group = 0; g_coin_unpaid = 0; g_coin_star = false; g_coin_sent_t = 0; g_coin_sent_v = -1; g_coin_seen_v = -1;
    // COINS10: a new visit: my loose total starts again under a new id, and so does what I saw from the others
    g_coin_last = -1; g_own_pend = 0; g_placed_pend = 0; g_loose_mine = 0; g_loose_in = 0;
    g_loose_visit = ((comms_hub_now_ms() << 8) ^ ((unsigned)g_my << 4) ^ ++g_loose_visits) | 1u;
    std::memset(g_loose_seen, 0, sizeof g_loose_seen);
    std::memset(g_slot_val, 0, sizeof g_slot_val);
    std::memset(g_slot_tmr, 0, sizeof g_slot_tmr);
}
int my_coin_count() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < 4) ? (int)data_0209f358[me] : 0;
}
// COINS10: my counter without the loose coins in it (mine and the ones the group told me): the part the count message
// carries and the group's max() works on, so a raise never holds someone's loose coins.
int my_placed_part() {
    const int p = my_coin_count() - (int)(g_loose_mine + g_loose_in);
    return p < 0 ? 0 : p;
}
// COINS10: split what my counter rose by since the last look into placed coins (their death bits went out this frame)
// and loose ones, and remember each standing placed coin's value for the next frame.
void coin_classify() {
    coin_observe();
    const int loose = g_own_pend - g_placed_pend;
    if (loose > 0) {
        g_loose_mine += (unsigned)loose;
        if (g_log) std::fprintf(stderr, "[coop] loose coins: I took %d (my total %u, placed %d) wall=%lld\n", loose, g_loose_mine,
                                g_placed_pend, wall_ms());
    }
    g_own_pend = 0; g_placed_pend = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        const unsigned cls = *(const unsigned short *)(a + 0x0c);
        if (cls < 288 || cls > 290) continue;
        const short slot = *(const short *)(a + 0xce);
        if (slot < 0 || slot >= kCoopSlots) continue;
        const int ty = *(const int *)(a + 0x3a0);
        g_slot_val[slot] = (unsigned char)(ty == 1 ? 2 : ty == 2 ? 5 : 1);
        g_slot_tmr[slot] = *(const unsigned short *)(a + 0x3a8);
    }
}
void send_loose() {
    if (!g_loose_visit) return;
    CoopLooseWire w; std::memset(&w, 0, sizeof w);
    w.key = wire_key(g_key); w.visit = g_loose_visit;
    w.total = (uint16_t)(g_loose_mine > 0xffffu ? 0xffffu : g_loose_mine);
    if (send_kind(kHubTagCoopWorld, kWorldCoinLoose, &w, (int)sizeof w)) ++g_loose_sends;
}
void send_coin_count(unsigned now) {
    if (coins_off() || coin_count_off() || !g_key.valid()) return;
    send_loose();   // COINS10: first, so a receiver has my loose total before my placed part
    CoopCoinWire w; std::memset(&w, 0, sizeof w);
    w.key = wire_key(g_key);
    int c = my_placed_part(); if (c > 0xff) c = 0xff;   // COINS10: the placed part (it was the whole counter)
    w.count = (uint16_t)c;
    w.flags = _ZN5Event6GetBitEj(0x1f) ? 1 : 0;
    if (send_kind(kHubTagCoopWorld, kWorldCoinCount, &w, (int)sizeof w)) { g_coin_sent_t = now; g_coin_sent_v = my_coin_count(); ++g_coin_sends; }
}
// COINS10: a member's loose total. Quiet like the other credits (no GiveCoins, so no 100-coin check) and never paid
// into the owed balance: the owed balance only holds raises of the placed part.
void apply_loose(int seat, const CoopLooseWire &w) {
    if (seat < 0 || seat >= CoopGroups::kSeats) return;
    LooseSeen &s = g_loose_seen[seat];
    if (s.visit != w.visit) { s.visit = w.visit; s.total = 0; }
    if ((unsigned)w.total <= s.total) return;
    const unsigned d = (unsigned)w.total - s.total;
    s.total = w.total;
    const int me = (int)data_0209f250;
    if (me < 0 || me > 3) return;
    coin_observe();
    int n = (int)data_0209f358[me] + (int)d;
    if (n > 0xff) n = 0xff;
    data_0209f358[me] = (short)n;
    coin_wrote();
    g_loose_in += d; ++g_loose_credits;
    if (g_log) std::fprintf(stderr, "[coop] loose coins from seat %d: +%u (its total %u) count=%d wall=%lld\n", seat, d, s.total, n, wall_ms());
}
// After the death delta of this frame went out (so a friend credits the placed coin before he hears the new count).
void coin_tick(unsigned now) {
    if (coins_off() || coin_count_off() || !g_sync) return;
    const int me = (int)data_0209f250;
    if (me < 0 || me > 3) return;
    coin_classify();   // COINS10
    if (g_coin_star && !_ZN5Event6GetBitEj(0x1f)) {
        _ZN5Event6SetBitEj(0x1f);
        std::fprintf(stderr, "[coop] coin count: a friend's 100-coin star is out: event bit 0x1f set here wall=%lld\n", wall_ms());
    }
    // COINS10: the max() works on the placed part; the loose coins sit on top of it
    const int placed = my_placed_part();
    if (g_coin_group > placed && (int)data_0209f358[me] < 0xff) {
        int n = (int)data_0209f358[me] + (g_coin_group - placed);
        if (n > 0xff) n = 0xff;
        std::fprintf(stderr, "[coop] coin count: the group has %d, mine was %d: took %d (placed part %d, loose %u) wall=%lld\n", g_coin_group,
                     (int)data_0209f358[me], n, placed, g_loose_mine + g_loose_in, wall_ms());
        g_coin_unpaid += n - (int)data_0209f358[me];   // COOPCOUNT2: owed to the death bits still on their way
        data_0209f358[me] = (short)n;
        coin_wrote();
        ++g_coin_raises;
    }
    const int c = my_coin_count();
    if (g_log && c != g_coin_seen_v) {
        std::fprintf(stderr, "[coop] coin count now %d (est %u) wall=%lld\n", c, g_est_frames, wall_ms());
        g_coin_seen_v = c;
    }
    if (g_grp.others() > 0 && ((unsigned)(now - g_coin_sent_t) >= kFullMs || (c != g_coin_sent_v && (unsigned)(now - g_coin_sent_t) >= 250u)))
        send_coin_count(now);
}

enum : int { kSecretCls = 329, kScLeaderUid = 0x108, kScRole = 0x10f, kScCount = 0x110, kScTimer = 0x113,
             kScClsnFlags = 0xd4 + 0x18, kSecMax = 16 };
// COOPCOUNT2: mine = my player took it (I resend it once a second); seat / cnt = who took it and the leader count his console saw at the
// take (cnt 0: not known yet), for the rule below when two takes reach five together.
struct SecTaken { uint16_t slot; bool here; bool mine; int8_t seat; uint8_t cnt; };
SecTaken g_sec[kSecMax]; int g_sec_n = 0;
int g_sec_count = 0;                     // the leader's count as last seen here
short g_sec_prev[kSecMax]; int g_sec_prev_n = 0;
unsigned g_sec_push_at = 0; int g_sec_push_left = 0;
unsigned g_sec_resend_t = 0;
void sec_reset() { g_sec_n = 0; g_sec_count = 0; g_sec_prev_n = 0; g_sec_push_left = 0; g_sec_resend_t = 0; }
SecTaken *sec_find(int slot) { for (int i = 0; i < g_sec_n; ++i) if (g_sec[i].slot == slot) return &g_sec[i]; return nullptr; }
SecTaken *sec_note(int slot, bool here) {
    SecTaken *t = sec_find(slot);
    if (!t && g_sec_n < kSecMax) { t = &g_sec[g_sec_n++]; t->slot = (uint16_t)slot; t->here = false; t->mine = false; t->seat = -1; t->cnt = 0; }
    if (t && here) t->here = true;
    return t;
}
char *sec_leader() {
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if (*(const unsigned short *)(a + 0x0c) == kSecretCls && !going(a) && *(const unsigned char *)(a + kScRole) == 1) return a;
    return nullptr;
}
void sec_send(int slot, int count) {
    CoopSecretWire w; std::memset(&w, 0, sizeof w);
    w.key = wire_key(g_key); w.slot = (uint16_t)slot; w.count = (uint8_t)count;
    send_kind(kHubTagCoopWorld, kWorldSecret, &w, (int)sizeof w);
}
bool sec_took_any() { for (int i = 0; i < g_sec_n; ++i) if (g_sec[i].mine) return true; return false; }
// Collect's effect on my copy of the coin in `slot`, without its jingle and number. False when it cannot run yet. `their` is the leader
// count the taker's console saw at the take (0: not known).
bool sec_apply(int slot, int seat, int their) {
    char *a = live_slot(kSecretCls, slot);
    if (!a || going(a)) return false;
    if (*(const unsigned *)(a + kScClsnFlags) & 1u) return true;          // already collected here
    char *ld = (char *)_ZN8dActor_c10FindWithIDEj(*(const unsigned *)(a + kScLeaderUid));
    if (!ld || *(const unsigned short *)(ld + 0x0c) != kSecretCls) ld = sec_leader();
    if (!ld) return false;                                                 // the group has no leader yet (first frame)
    unsigned char &cnt = *(unsigned char *)(ld + kScCount);
    if (cnt >= 5) return true;
    ++cnt;
    *(unsigned *)(a + kScClsnFlags) |= 1u;
    // The fifth taken elsewhere: no star timer here. The collector's console spawns the star and coop_spawn announces it
    // (it is a runtime star, not linked to a placed marker), so every console gets that one star and none makes its own.
    // A leader at 5 removes itself in its Behavior.
    // COOPCOUNT2: unless that console never saw five. Two takes inside one message delay (the 4th and the 5th, or a lost message
    // and a take) leave each taker at four after his own take, so neither Collect started the timer. Then the lower seat of two
    // takers starts Collect's 30-frame timer on this coin (d_a_s_coin.cpp: the fifth's mDeathTimer = 0x1e, Behavior spawns the
    // star at 0), the higher seat does not: every console of the pair reaches five here, and exactly one of them makes the star.
    const bool make = cnt == 5 && their > 0 && their < 5 && seat > g_my && sec_took_any();
    if (make) *(unsigned char *)(a + kScTimer) = 0x1e;
    else if (*(const unsigned char *)(a + kScRole) == 2) _ZN7fBase_c18MarkForDestructionEv(a);
    g_sec_count = cnt;
    std::fprintf(stderr, "[coop] secret: slot %d taken by seat %d (its count %d), removed here; leader count now %d%s wall=%lld\n", slot,
                 seat, their, (int)cnt, make ? " (the fifth, no console saw five on its own take: I am the lower taker, I make the star)"
                 : cnt == 5 ? " (the fifth: the collector's console makes the star)" : "", wall_ms());
    return true;
}
void watch_secrets(unsigned now) {
    short live[kSecMax]; int nl = 0; char *ld = nullptr;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (*(const unsigned short *)(a + 0x0c) != kSecretCls) continue;
        const short s = *(const short *)(a + 0xce);
        if (s < 0 || s >= kCoopSlots) continue;
        if (!going(a) && nl < kSecMax) live[nl++] = s;
        if (!going(a) && *(const unsigned char *)(a + kScRole) == 1) ld = a;
    }
    const int L = ld ? (int)*(const unsigned char *)(ld + kScCount) : g_sec_count;
    if (L > g_sec_count) {
        // my player's collect(s): the coin whose collider Collect switched off, else a coin that was here last frame
        int n = L - g_sec_count;
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && n > 0; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
            if (*(const unsigned short *)(a + 0x0c) != kSecretCls) continue;
            const short s = *(const short *)(a + 0xce);
            if (s < 0 || s >= kCoopSlots || !(*(const unsigned *)(a + kScClsnFlags) & 1u)) continue;
            const SecTaken *t = sec_find(s);
            if (t && t->here) continue;
            if (SecTaken *m = sec_note(s, true)) { m->mine = true; m->seat = (int8_t)g_my; m->cnt = (uint8_t)L; }
            --n;
            if (g_grp.others() > 0) sec_send(s, L);
            std::fprintf(stderr, "[coop] secret: I took slot %d (leader count %d): told the group wall=%lld\n", (int)s, L, wall_ms());
        }
        for (int i = 0; i < g_sec_prev_n && n > 0; ++i) {
            const short s = g_sec_prev[i];
            bool still = false;
            for (int j = 0; j < nl; ++j) if (live[j] == s) still = true;
            const SecTaken *t = sec_find(s);
            if (still || (t && t->here)) continue;
            if (SecTaken *m = sec_note(s, true)) { m->mine = true; m->seat = (int8_t)g_my; m->cnt = (uint8_t)L; }
            --n;
            if (g_grp.others() > 0) sec_send(s, L);
            std::fprintf(stderr, "[coop] secret: I took slot %d (leader count %d, gone this frame): told the group wall=%lld\n", (int)s, L,
                         wall_ms());
        }
        g_sec_count = L;
    }
    std::memcpy(g_sec_prev, live, sizeof(short) * (size_t)nl); g_sec_prev_n = nl;
    // what the group took that stands here (I came in later, or the message came before my coins)
    if ((g_est_frames % 10) == 0)
        for (int i = 0; i < g_sec_n; ++i)
            if (!g_sec[i].here && sec_apply(g_sec[i].slot, g_sec[i].seat, g_sec[i].cnt)) g_sec[i].here = true;
    // COOPCOUNT2: my own takes again once a second while friends are here, so a lost "took a secret" heals (a repeat is a no-op
    // on a console that has it: the coin's collider flag is already set, or the slot is already noted as here)
    if (g_grp.others() > 0 && (unsigned)(now - g_sec_resend_t) >= kFullMs) {
        g_sec_resend_t = now;
        for (int i = 0; i < g_sec_n; ++i) if (g_sec[i].mine) sec_send(g_sec[i].slot, g_sec[i].cnt);
    }
    // a member joined: send him the taken ones, kGonePushes rounds 700 ms apart
    if (g_sec_push_left > 0 && g_sec_n && (int)(now - g_sec_push_at) >= 0) {
        if (g_grp.others() == 0) { g_sec_push_left = 0; return; }
        for (int i = 0; i < g_sec_n; ++i) sec_send(g_sec[i].slot, g_sec_count > 0 ? (g_sec_count > 5 ? 5 : g_sec_count) : 1);
        if (g_log) std::fprintf(stderr, "[coop] secret: %d taken secret(s) sent to the group (round %d)\n", g_sec_n,
                                kGonePushes - g_sec_push_left + 1);
        --g_sec_push_left;
        g_sec_push_at = now + 700u;
    }
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

// ---------------------------------------------------------------------------
// COOPRACE1 proof knobs (run after093). Koopa the Quick (daRNk_c, class 188) and the racing penguin (daPgRcer_c, class
// 259) only race after a talk and its answer, which a headless row cannot drive. Inert unless named:
//   SM64DS_COOP_RACE=F        at est frame F start the race with the nearest racer (within 3000 units) the cartridge's
//                             way minus the talk: Koopa armed flag, my player, STATE_RACE step 1 (d_a_r_nk.cpp); the
//                             penguin's talk player and its state 3 set-up (d_a_pg_rcer.cpp)
//   SM64DS_COOP_RACEGO=F:f|c  at est frame F put my player at the finish (f: Koopa's flag, the penguin's finish point)
//                             or in the penguin's shortcut volume (c)
// ---------------------------------------------------------------------------

enum : int {
    kKoopaCls = 188, kPenguinCls = 259, kRaceFlagCls = 205,
    // daRNk_c (include/daRNk_c.h)
    kRnAnim = 0x300, kRnState = 0x38c, kRnStep = 0x390, kRnFlagID = 0x394, kRnPlayer = 0x398, kRnCannon = 0x3ac,
    kRnWon = 0x3af, kRnFinished = 0x3b4, kRnRacing = 0x3b5, kRnTimer = 0x100,
    // daPgRcer_c (include/daPgRcer_c.h)
    kPgAnim = 0xd4, kPgDesc = 0x370, kPgAction = 0x374, kPgTalk = 0x378, kPgStep = 0x38f, kPgPenFin = 0x392,
    kPgPlFin = 0x393, kPgCheat = 0x394, kPgPenWon = 0x395,
    // daRFlag_c (include/daRFlag_c.h)
    kFlagVictory = 0x16c, kFlagTouched = 0x16e,
    // ModelAnim: the frame controller at +0x50 (flags +0x54, frame +0x58, speed +0x5c), the file at +0x60
    kMaFlags = 0x54, kMaFrame = 0x58, kMaSpeed = 0x5c, kMaFile = 0x60,
    // dActor_c
    kAcPos = 0x5c, kAcAngY = 0x8e, kAcPrevAngY = 0x94, kAcHSpeed = 0x98, kAcVSpeed = 0xa8,
    kRaceSendEvery = 2,       // est frames between state records (about 15 Hz)
    kRaceOverMs = 1000,       // the authority keeps telling the result this long
    kRaceSilentMs = 3000,     // a race whose authority went quiet this long is over (not won)
    kRaceMax = 2,
};

unsigned g_race_go_at = 0; char g_race_go_what = 0; bool g_race_go_done = true;
unsigned g_race_knob_at = 0; bool g_race_knob_done = true;

void race_read_knobs() {
    static bool read = false;
    if (read) return;
    read = true;
    if (const char *e = std::getenv("SM64DS_COOP_RACE")) { g_race_knob_at = (unsigned)std::atoi(e); g_race_knob_done = g_race_knob_at == 0; }
    if (const char *e = std::getenv("SM64DS_COOP_RACEGO")) {
        unsigned f = 0; char w = 0;
        if (std::sscanf(e, "%u:%c", &f, &w) == 2 && (w == 'f' || w == 'c')) { g_race_go_at = f; g_race_go_what = w; g_race_go_done = false; }
    }
}

char *my_body() { const int me = (int)data_0209f250; return (me >= 0 && me < 16) ? (char *)data_0209f394[me] : nullptr; }

int *apos(char *a) { return (int *)(a + kAcPos); }

double dist_units(const int *a, const int *b) {
    const double dx = (a[0] - b[0]) / 4096.0, dy = (a[1] - b[1]) / 4096.0, dz = (a[2] - b[2]) / 4096.0;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

char *race_flag() {
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if (*(const unsigned short *)(a + 0x0c) == kRaceFlagCls && !going(a)) return a;
    return nullptr;
}

void race_knobs(unsigned now) {
    (void)now;
    char *me = my_body();
    if (!me) return;
    if (!g_race_knob_done && g_est_frames >= g_race_knob_at) {
        g_race_knob_done = true;
        char *best = nullptr; double bd = 3000.0;
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
            const unsigned c = *(const unsigned short *)(a + 0x0c);
            if ((c != kKoopaCls && c != kPenguinCls) || going(a)) continue;
            const double d = dist_units(apos(a), apos(me));
            if (d < bd) { bd = d; best = a; }
        }
        if (!best) std::fprintf(stderr, "[race-knob] start: no racer within 3000 units\n");
        else if (*(const unsigned short *)(best + 0x0c) == kKoopaCls) {
            if (*(const int *)(best + kRnState) != 0) std::fprintf(stderr, "[race-knob] start: Koopa is not waiting (state %d)\n", *(const int *)(best + kRnState));
            else if (char *f = race_flag()) {
                *(unsigned *)(best + kRnFlagID) = *(const unsigned *)(f + 0x04);
                *(unsigned char *)(f + kFlagTouched) = 0;
                *(char **)(best + kRnPlayer) = me;
                *(int *)(best + kRnState) = 2; *(unsigned char *)(best + kRnStep) = 1; *(unsigned short *)(best + kRnTimer) = 0x32;
                _ZN5Timer10ResetTimerEv(data_0209d4c8);
                std::fprintf(stderr, "[race-knob] start: Koopa slot %d races my player (%.0f units away) est %u\n", (int)*(const short *)(best + 0xce), bd, g_est_frames);
            }
        } else {
            if (*(const int *)(best + kPgAction) != 0) std::fprintf(stderr, "[race-knob] start: the penguin is not waiting (state %d)\n", *(const int *)(best + kPgAction));
            else {
                *(char **)(best + kPgTalk) = me;
                *(char **)(best + kPgDesc) = data_ov019_0211356c + 3 * 16;
                func_ov019_02111d58(best);
                std::fprintf(stderr, "[race-knob] start: the penguin slot %d races my player (%.0f units away) est %u\n", (int)*(const short *)(best + 0xce), bd, g_est_frames);
            }
        }
    }
    if (!g_race_go_done && g_est_frames >= g_race_go_at) {
        g_race_go_done = true;
        int to[3] = {0, 0, 0}; bool ok = false;
        if (char *f = race_flag()) { to[0] = apos(f)[0]; to[1] = apos(f)[1] + (40 << 12); to[2] = apos(f)[2]; ok = g_race_go_what == 'f'; }
        else {
            const int *src = g_race_go_what == 'f' ? data_ov019_021134e8 : data_ov019_0211353c;
            to[0] = src[0]; to[1] = src[1] + (60 << 12); to[2] = src[2]; ok = true;
        }
        if (ok) {
            for (int j = 0; j < 3; ++j) apos(me)[j] = to[j];
            std::fprintf(stderr, "[race-knob] go: my player put at the %s %d,%d,%d est %u\n", g_race_go_what == 'f' ? "finish" : "shortcut",
                         to[0] >> 12, to[1] >> 12, to[2] >> 12, g_est_frames);
        }
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
    if (sub == kWorldSwallowGone && pl == kCageWireLen) {   // COOPBOO1: the cage Boo dropped its cage
        CoopCageWire w; std::memcpy(&w, p, sizeof w);
        if (!coop_key_ok(w.head.key) || w.head.slot >= kCoopSlots || w.head.cls != kBooCls || w.area < -1 || w.area > 7 ||
            w.x <= -kCagePosLimit || w.x >= kCagePosLimit || w.y <= -kCagePosLimit || w.y >= kCagePosLimit ||
            w.z <= -kCagePosLimit || w.z >= kCagePosLimit) { ++g_st.drop_bad; return; }
        key_from_wire(w.head.key, &k);
        const int pos[3] = {w.x, w.y, w.z};
        if (!for_me(k)) { if (k.valid()) room_cage_note(k, (int)w.head.slot, pos, w.area); ++g_st.drop_group; return; }
        g_heard[seat] = comms_hub_now_ms() | 1u;
        if (!g_leg_trust && !g_grp.is_member(seat)) { ++g_st.drop_trust; return; }
        cage_apply_remote(seat, (int)w.head.slot, pos, w.area);
        return;
    }
    if (sub == kWorldSwallowGone && pl == (int)sizeof(CoopBooWire)) {   // COOPBOO2: a friend beat a Boo of my Big Boo
        CoopBooWire w; std::memcpy(&w, p, sizeof w);
        if (!coop_key_ok(w.head.key) || w.head.slot >= kCoopSlots || w.head.cls != 209) { ++g_st.drop_bad; return; }
        key_from_wire(w.head.key, &k);
        if (!for_me(k)) { if (k.valid()) room_gone_note(k, (int)w.head.slot, 209u); ++g_st.drop_group; return; }   // filed for my visit
        g_heard[seat] = comms_hub_now_ms() | 1u;
        if (!g_leg_trust && !g_grp.is_member(seat)) { ++g_st.drop_trust; return; }
        apply_swallow(seat, (int)w.head.slot, 209u, true);
        return;
    }
    if (sub == kWorldSwallowGone) {
        CoopSwallowWire w;
        if (pl != (int)sizeof w) { ++g_st.drop_bad; return; }
        std::memcpy(&w, p, sizeof w);
        if (!coop_key_ok(w.key) || w.slot >= kCoopSlots || w.cls >= 1024 || w.cls == 0xbf) { ++g_st.drop_bad; return; }
        key_from_wire(w.key, &k);
        if (!for_me(k)) {
            // COOPSYNC1: a block broken in a level I am not in: the room's list keeps it for my visit there
            if (breakable_block(w.cls) && k.valid()) room_gone_note(k, (int)w.slot, (unsigned)w.cls);
            ++g_st.drop_group; return;
        }
        g_heard[seat] = comms_hub_now_ms() | 1u;
        if (!g_leg_trust && !g_grp.is_member(seat)) { ++g_st.drop_trust; return; }   // members of this group only
        apply_swallow(seat, (int)w.slot, (unsigned)w.cls);
        return;
    }
    if (sub == kWorldCoinCount) {   // COOPCOUNT1
        if (!coop_coin_validate(p, pl)) { ++g_st.drop_bad; return; }
        CoopCoinWire w; std::memcpy(&w, p, sizeof w);
        key_from_wire(w.key, &k);
        if (!for_me(k)) { ++g_st.drop_group; return; }
        g_heard[seat] = comms_hub_now_ms() | 1u;
        if (!g_leg_trust && !g_grp.is_member(seat)) { ++g_st.drop_trust; return; }
        ++g_coin_recvs;
        if (coins_off() || coin_count_off()) return;
        if ((int)w.count > g_coin_group) {
            if (g_log) std::fprintf(stderr, "[coop] coin count from seat %d: %d%s wall=%lld\n", seat, (int)w.count,
                                    (w.flags & 1) ? " (its 100-coin star is out)" : "", wall_ms());
            g_coin_group = (int)w.count;
        }
        if (w.flags & 1) g_coin_star = true;
        return;
    }
    if (sub == kWorldCoinLoose) {   // COINS10
        if (!coop_loose_validate(p, pl)) { ++g_st.drop_bad; return; }
        CoopLooseWire w; std::memcpy(&w, p, sizeof w);
        key_from_wire(w.key, &k);
        if (!for_me(k)) { ++g_st.drop_group; return; }
        g_heard[seat] = comms_hub_now_ms() | 1u;
        if (!g_leg_trust && !g_grp.is_member(seat)) { ++g_st.drop_trust; return; }
        ++g_loose_recvs;
        if (coins_off() || coin_count_off()) return;
        apply_loose(seat, w);
        return;
    }
    if (sub == kWorldSecret) {      // COOPCOUNT1
        if (!coop_secret_validate(p, pl)) { ++g_st.drop_bad; return; }
        CoopSecretWire w; std::memcpy(&w, p, sizeof w);
        key_from_wire(w.key, &k);
        if (!for_me(k)) { ++g_st.drop_group; return; }
        g_heard[seat] = comms_hub_now_ms() | 1u;
        if (!g_leg_trust && !g_grp.is_member(seat)) { ++g_st.drop_trust; return; }
        SecTaken *t = sec_note((int)w.slot, false);
        if (t && t->seat < 0) { t->seat = (int8_t)seat; t->cnt = w.count; }
        if (t && !t->here) {
            if (sec_apply((int)w.slot, t->seat, t->cnt)) t->here = true;
            else if (g_log) std::fprintf(stderr, "[coop] secret: slot %d taken by seat %d (its count %d), no copy here yet: kept\n",
                                         (int)w.slot, seat, (int)w.count);
        }
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
    coin_reset(); sec_reset();   // COOPCOUNT1
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
    if (nk.level != g_my_cur.level) g_my_prev = g_my_cur;   // Tango's rule: the level I came straight from
    g_my_cur = nk;
    g_key = nk;
    g_part = part_of_level(nk.level);
    room_gone_load(nk);   // COOPSYNC1: blocks broken here earlier in this room
    room_cage_load(nk);   // COOPBOO1: and the courtyard cage, if it was freed
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
        // COOPCOUNT1 (audit G18): the answer needs one round trip and the owner's next frame. 400 ms was too short on a slow
        // link: the joiner started the level from a blank state and the 1 Hz full bitmap tore things down a second later.
        // Wait twice the slowest member's measured round trip plus 300 ms, at least 400 ms; while a member says it holds the
        // group's state (established), wait up to the cap. A fast answer ends the wait at once either way.
        int rtt = -1;
        const unsigned mm = g_grp.member_mask();
        for (int s2 = 0; s2 < kCoopMaxSeats; ++s2) {
            HubSeatInfo si;
            if (((mm >> s2) & 1u) && comms_hub_seat_info(s2, &si) && si.rtt_ms > rtt) rtt = si.rtt_ms;
        }
        unsigned wait_ms = kEnterWaitMs;
        if (rtt >= 0 && (unsigned)(2 * rtt + 300) > wait_ms) wait_ms = (unsigned)(2 * rtt + 300);
        if (g_grp.established_count() > 0) wait_ms = kEnterWaitCap;
        if (wait_ms > kEnterWaitCap) wait_ms = kEnterWaitCap;
        if (const char *fw = std::getenv("SM64DS_COOP_ENTERWAIT")) { const int v = std::atoi(fw); if (v > 0 && v <= 5000) wait_ms = (unsigned)v; }   // proof knob
        if (g_log) std::fprintf(stderr, "[coop] joining level %d act %d: waiting up to %u ms for the snapshot (rtt %d ms, %d established)\n",
                                nk.level, nk.act, wait_ms, rtt, g_grp.established_count());
        g_snap_waiting = true;
        const unsigned t0 = comms_hub_now_ms();
        unsigned last_send = 0;
        send_enter(); last_send = t0;
        while ((unsigned)(comms_hub_now_ms() - t0) < wait_ms && !g_snap_done) {
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
                         nk.level, nk.act, (int)wait_ms);
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
        std::fprintf(stderr, "[coop-knob] %s class=%d slot=%d\n", k.gone ? "gone" : "kill", k.id, (int)c[i].slot);
        if (k.gone) _ZN7fBase_c18MarkForDestructionEv(c[i].a);
        else _ZN8dActor_c24KillAndTrackInDeathTableEv(c[i].a);
    }
    std::fprintf(stderr, "[coop-knob] %s class=%d asked %d did %d (live candidates %d)\n", k.gone ? "gone" : "kill", k.id,
                 k.n, killed, n);
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
        if (id == 329 && (*(const unsigned *)(a + 0xd4 + 0x18) & 1u)) continue;   // COOPCOUNT1: a secret already collected here
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

void knob_loose() {
    char *pl = (char *)data_0209f394[data_0209f250 & 15];
    if (!pl) return;
    if (g_loose_uid) {   // keep my player on the coin until it is collected (or 60 frames went by)
        char *a = (char *)_ZN8dActor_c10FindWithIDEj(g_loose_uid);
        if (a && *(const unsigned short *)(a + 0x0c) == 0x120 && !*(const unsigned char *)(a + 0x0f) && ++g_loose_hold < 60) {
            std::memcpy(pl + 0x5c, a + 0x5c, 12);
            return;
        }
        std::fprintf(stderr, "[coop-knob] loose coin %d %s after %d frames (count %d) est %u\n", g_loose_done,
                     a && *(const unsigned char *)(a + 0x0f) ? "going" : a ? "still standing" : "gone", g_loose_hold,
                     (int)data_0209f358[data_0209f250 & 3], g_est_frames);
        g_loose_uid = 0;
    }
    if (g_loose_left <= 0 || g_est_frames < g_loose_frame + g_loose_every * (unsigned)g_loose_done) return;
    int pos[3]; std::memcpy(pos, pl + 0x5c, 12);
    char *a = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x120, 2, pos, nullptr, -1, -1);
    --g_loose_left; ++g_loose_done; g_loose_hold = 0;
    g_loose_uid = a ? *(const unsigned *)(a + 0x04) : 0;
    std::fprintf(stderr, "[coop-knob] loose coin %d spawned %s slot=%d at %d,%d,%d est %u\n", g_loose_done, a ? "yes" : "no",
                 a ? (int)*(const short *)(a + 0xce) : -99, pos[0] >> 12, pos[1] >> 12, pos[2] >> 12, g_est_frames);
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
                coin_local_bit(w * 32 + b);   // COINS10
                if (g_log) std::fprintf(stderr, "[coop] local bit slot=%d\n", w * 32 + b);
            }
            g_prev[w] = t[w];
        }
        // 1b. how a mouthful left my Yoshi's mouth (TONGUE3), and the swallows the death table cannot carry
        watch_swallow(now);
        if (g_gone_n && (g_est_frames % 10) == 0) gone_apply();
        glint_sweep();     // COOPSILVER3
        watch_capped(now);
        send_swallows(now);
        gone_push(now);
        watch_cage(now);   // COOPBOO1
        watch_boos(now);   // COOPBOO2
        boohit_knob();     // COOPBOO2 proof knob, inert unless SM64DS_COOP_BOOHIT is set
        log_big_boo();     // COOPBOO2
        cage_send(now);
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
    if (g_sync) { coin_tick(now); watch_secrets(now); }   // COOPCOUNT1
    race_read_knobs(); race_knobs(now);   // COOPRACE1 proof knobs
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
    if (g_goto_id >= 0 && g_goto_tries < g_goto_max && g_est_frames >= g_goto_frame + 90u * (unsigned)g_goto_tries &&
        g_sec_count >= g_goto_after) knob_goto(g_goto_id, g_goto_tries++);
    if (knobs_on && !g_star_done && g_est_frames >= g_star_frame) knob_star();
    if (knobs_on && (g_loose_left > 0 || g_loose_uid)) knob_loose();   // COINS10
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
    g_room_gone_n = 0;   // COOPSYNC1: a new room starts with every block whole
    g_room_empty_n = 0;
    for (int s = 0; s < kCoopSeats; ++s) g_pres_cur[s] = g_pres_prev[s] = CoopKey();
    g_my_cur = g_my_prev = CoopKey();
    g_room_cage_n = 0;   // COOPBOO1: and with the cage Boo carrying its cage
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
    coin_reset(); sec_reset();   // COOPCOUNT1
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

extern "C" int (*port_coop_cap_keep_all)(void);   // src/runtime/actor/d_cap_enemy_arm9.cpp, host arm (COOPBOO3)
bool coop_world_active();
int cap_keep_all() { return coop_world_active() ? 1 : 0; }
struct CapKeepHook { CapKeepHook() { port_coop_cap_keep_all = cap_keep_all; } } g_cap_keep_hook;

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
    if (g_frame % 30 == 0) room_forget(now);   // Tango's rule: a level nobody has loaded loses its broken blocks
    for (int i = 0; i < g_n_frame; ++i) g_cb_frame[i](now);
    {   // members that left my group since last frame
        g_grp.update(now);
        const unsigned m = g_grp.member_mask();
        const unsigned gone = g_prev_members & ~m;
        if ((m & ~g_prev_members) && g_gone_n) { g_gone_push_at = now + 300u; g_gone_push_left = kGonePushes; }   // COOPSYNC1
        if ((m & ~g_prev_members) && g_sec_n) { g_sec_push_at = now + 300u; g_sec_push_left = kGonePushes; }       // COOPCOUNT1
        if (m & ~g_prev_members) g_coin_sent_t = now - kFullMs;                                                   // COOPCOUNT1: tell my count
        if ((m & ~g_prev_members) && g_cage.free) { g_cage_send_at = now + 300u; g_cage_send_left = kGonePushes; g_cage_send_gap = 700u; }   // COOPBOO1
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
            LoadLevelNoReturn((unsigned)g_warp[i].level, (unsigned)g_warp[i].entrance, (unsigned)g_warp[i].act, 1);
            // COOPRACE1 proof knob SM64DS_COOP_WARP_ACT=1: the warp's act really is the act (Koopa the Quick is in Bob-omb
            // Battlefield's act 2 only). Without it the change keeps the act it always had here (1): the star it stages
            // is not the one the latch takes, measured "[adventure table branch, star=1]" for a warp to act 2.
            static const bool warp_act = [] { const char *e = std::getenv("SM64DS_COOP_WARP_ACT"); return e && *e == '1'; }();
            if (warp_act && g_warp[i].act > 0 && g_warp[i].act < 8) data_0209f1f0 = (unsigned char)g_warp[i].act;
        }
    // No body (a level unload or reload): whatever was in my mouth left with the level, not down my throat.
    if (!in_level && g_mouth) { g_mouth = nullptr; g_mouth_slot = -1; }
    if (!in_level) g_cap_slotted = -1;
    g_grp.update(now);
    if (in_level && g_key.valid() && g_part >= 0 && g_grp.established()) tick_est(now);
}
