// COOP SHARED TARGETING. Run mp1, lane BOSSCORE1. See coop_target.h for the design.
//
// PORT_HOST_ABI. The only thing patched is the entry of dActor_c::ClosestPlayer, and only when a coop room goes
// live. Raw offsets are the established convention in port/hal; nothing in src/ changes.

#include "coop_target.h"
#include "coop_boss.h"
#include "coop_world.h"
#include "coop_groups.h"
#include "coop_wire.h"
#include "comms_hub.h"

#include "dActor_c.h"
#include "Player.h"
#include "daBtn_c.h"

#include <windows.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern unsigned char data_0209f21c;
extern void *data_0209b458;
extern void *data_0209b450;
extern int data_0208e380;
extern int data_0208e37c;
extern short data_02092144[];
int Vec3_Dist(const void *a, const void *b);
int Vec3_HorzDist(const void *a, const void *b);
short Vec3_HorzAngle(const void *a, const void *b);
int _ZN8dActor_c14GetSubtractionEss(void *self, short a, short b);
void _ZN7daBtn_c19func_ov079_02123f34Ev(char *self);
int port_coop_boot_opening_hold(void);
void *_ZN8dActor_c4NextEPKS_(const void *prev);
int port_adventure_ghost_seat(int slot);
int port_adventure_ghost_drawpos(int slot, int out[3]);
}

namespace port {
namespace {

enum : int {
    kSeats = 4, kPlayerMax = 16,
    kDecideMs = 100, kBeatMs = 1500, kFreshMs = 4000, kRangeUnits = 4000,
    kPayloadMax = kHubCoopMaxDatagram - kHubCoopHeaderBytes,
};

CoopTargetStats g_st = {};
bool g_env = false, g_on = true, g_log = false;
bool g_logall = false;   // SM64DS_COOP_TARGET_CLS=all: log every class (COOPSPAWN9 proof)
unsigned g_hold_ms = 1000;
unsigned g_logcls[16]; int g_nlogcls = 0;
struct ForgeKnob { unsigned tick; char name[12]; bool done; };
ForgeKnob g_forge[8]; int g_nforge = 0;

unsigned g_stand_tick = 0, g_stand_every = 0; int g_stand_cls = 0, g_stand_dist = 0; bool g_stand_on = false;
int g_sample_cls = 0; unsigned g_sample_t = 0;
bool g_regd = false, g_patched = false, g_patch_failed = false;
int  g_my = -1;
CoopKey g_key;
unsigned g_ticks = 0;
unsigned g_last_decide = 0, g_last_stat = 0;
unsigned g_tok_t = 0; double g_tokens = 4.0;

struct Ent { uint8_t seat; unsigned t; };        // seat 0xff = none
Ent g_tgt[kTargetSlots];
struct Dec { int cur; unsigned cur_t; int cand; unsigned cand_t; int pub; unsigned pub_t; };
Dec g_dec[kTargetSlots];
short g_lastuse[kTargetSlots];                    // the seat ClosestPlayer last returned for the slot (log)

long long wall_ms() {
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    return (long long)((((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000ull);
}
inline int32_t rd32(const char *a, int off) { int32_t v; std::memcpy(&v, a + off, 4); return v; }
inline int16_t rd16(const char *a, int off) { int16_t v; std::memcpy(&v, a + off, 2); return v; }
inline void wr32(char *a, int off, int32_t v) { std::memcpy(a + off, &v, 4); }

void read_env() {
    if (g_env) return;
    g_env = true;
    auto flag = [](const char *n, bool d) { const char *e = std::getenv(n); return (e && *e) ? *e != '0' : d; };
    g_on = flag("SM64DS_COOP_TARGET", true);
    g_log = flag("SM64DS_COOP_TARGET_LOG", false);
    if (const char *h = std::getenv("SM64DS_COOP_TARGET_HOLD_MS")) { int v = std::atoi(h); if (v >= 0 && v <= 60000) g_hold_ms = (unsigned)v; }
    if (const char *st = std::getenv("SM64DS_COOP_TARGET_STAND")) {
        unsigned f = 0, ev = 0; int cl = 0, d = 0;
        const int n = std::sscanf(st, "%u:%i:%i:%u", &f, &cl, &d, &ev);
        if (n >= 3) { g_stand_tick = f; g_stand_cls = cl; g_stand_dist = d; g_stand_on = true; g_stand_every = n >= 4 ? ev : 0; }
    }
    if (const char *sm = std::getenv("SM64DS_COOP_TARGET_SAMPLE")) g_sample_cls = std::atoi(sm);
    const char *c = std::getenv("SM64DS_COOP_TARGET_CLS");
    if (!c || !*c) c = "165,189,200";
    if (!std::strcmp(c, "all")) { g_logall = true; c = ""; }
    for (const char *p = c; *p && g_nlogcls < 16;) {
        char *end = nullptr; unsigned long v = std::strtoul(p, &end, 0);
        if (end == p) break;
        g_logcls[g_nlogcls++] = (unsigned)v;
        p = end; while (*p == ',' || *p == ' ') ++p;
    }
    if (const char *f = std::getenv("SM64DS_COOP_TARGET_FORGE")) {
        const char *p = f;
        while (*p && g_nforge < 8) {
            unsigned fr = 0; char nm[12] = {}; int used = 0;
            if (std::sscanf(p, "%u:%11[a-z]%n", &fr, nm, &used) < 2 || used <= 0) break;
            g_forge[g_nforge].tick = fr; std::memcpy(g_forge[g_nforge].name, nm, sizeof nm); g_forge[g_nforge].done = false;
            ++g_nforge; p += used; while (*p == ',' || *p == ';' || *p == ' ') ++p;
        }
    }
    std::fprintf(stderr, "[cotgt] targeting %s hold=%ums%s\n", g_on ? "on" : "OFF (control)", g_hold_ms, g_log ? " log" : "");
}

bool logged_cls(unsigned c) {
    if (g_logall) return true;
    for (int i = 0; i < g_nlogcls; ++i) if (g_logcls[i] == c) return true;
    return false;
}

// ---------------------------------------------------------------------------
// Players I can place: myself exactly, friends at the pose the follower drew them at.
// ---------------------------------------------------------------------------
struct PInfo { int seat; char *body; int idx; int32_t x, y, z; };

// The follower can miss a frame (a teleport, a character swap) and then reports no drawn pose; a friend is still where he was
// for a little while, so the last pose counts for 400 ms.
struct Sticky { bool have; int32_t p[3]; unsigned t; };
Sticky g_sticky[kPlayerMax];
bool drawpos_sticky(int idx, int out[3]) {
    const unsigned now = comms_hub_now_ms();
    if (idx < 0 || idx >= kPlayerMax) return false;
    if (port_adventure_ghost_drawpos(idx, out)) {
        g_sticky[idx].have = true; std::memcpy(g_sticky[idx].p, out, sizeof g_sticky[idx].p); g_sticky[idx].t = now;
        return true;
    }
    if (g_sticky[idx].have && (unsigned)(now - g_sticky[idx].t) < 400u) { std::memcpy(out, g_sticky[idx].p, sizeof g_sticky[idx].p); return true; }
    return false;
}

char *local_body() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < kPlayerMax) ? (char *)data_0209f394[me] : nullptr;
}

int gather(PInfo *out) {
    int n = 0;
    char *lb = local_body();
    if (lb) {
        const int hp = (int)(signed char)(data_02092144[data_0209f250 & 15] >> 8);
        if (hp > 0) {
            out[n++] = { g_my, lb, (int)data_0209f250, rd32(lb, 0x5c), rd32(lb, 0x60), rd32(lb, 0x64) };
        }
    }
    const unsigned mask = coop_group_member_mask();
    const int nb = (int)data_0209f21c < kPlayerMax ? (int)data_0209f21c : kPlayerMax;
    for (int i = 0; i < nb && n < kSeats; ++i) {
        if (i == (int)data_0209f250) continue;
        char *b = (char *)data_0209f394[i];
        if (!b) continue;
        const int s = port_adventure_ghost_seat(i);
        if (s < 0 || s >= kSeats || s == g_my || !((mask >> s) & 1u)) continue;
        int dp[3];
        if (!drawpos_sticky(i, dp)) continue;
        out[n++] = { s, b, i, dp[0], dp[1], dp[2] };
    }
    return n;
}

double dist_units(int32_t ax, int32_t ay, int32_t az, int32_t bx, int32_t by, int32_t bz) {
    const double dx = ((double)ax - (double)bx) / 4096.0, dy = ((double)ay - (double)by) / 4096.0, dz = ((double)az - (double)bz) / 4096.0;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

bool live() {
    return g_on && coop_world_active() && coop_group_established();
}

// Put every friend I can see at his drawn pose (so the ROM's loop measures the real place).
void place_friends() {
    PInfo p[kSeats];
    const int n = gather(p);
    for (int i = 0; i < n; ++i)
        if (p[i].seat != g_my) { wr32(p[i].body, 0x5c, p[i].x); wr32(p[i].body, 0x60, p[i].y); wr32(p[i].body, 0x64, p[i].z); p[i].body[0x6fb] = 0; }
}

bool fresh(const Ent &e, unsigned now) { return e.seat != kTargetNoSeat && (unsigned)(now - e.t) <= (unsigned)kFreshMs; }

short actor_slot(const char *a) { return rd16(a, 0xce); }

// ---------------------------------------------------------------------------
// The ClosestPlayer replacement (thiscall: this in ecx, nothing on the stack).
// ---------------------------------------------------------------------------
void *table_body(const char *self, int *seat_out) {
    const short slot = actor_slot(self);
    if (slot < 0 || slot >= kTargetSlots) return nullptr;
    const Ent &e = g_tgt[slot];
    if (!fresh(e, comms_hub_now_ms())) return nullptr;
    void *b = coop_target_body_for_seat(e.seat);
    if (b && seat_out) *seat_out = e.seat;
    return b;
}

// ---------------------------------------------------------------------------
// TALKERS (FIX9A). An actor that opens a message, starts a camera or cutscene scene, or is an NPC must run its whole
// conversation with MY player: Player::StartTalk on a friend's body (held, never running) refuses for ever, and the actor
// waits on a talk that never begins (the castle intro Lakitu softlock). Enemies and bosses keep the shared target.
// Evidence: out FIX9A TALKERS.md (every ClosestPlayer caller in src/ that reaches StartTalk / a camera call).
// Class ids are the ROM spawn table's (dActor_c +0x0c).
// ---------------------------------------------------------------------------
const unsigned short kTalkerClasses[] = {
    26,   // QUESTION_SWITCH   d_a_obj_hatena_switch.cpp:231 ClosestPlayer -> StartTalk
    38,   // PEACH_PAINTING    PeachPainting::Behavior
    181,  // BOB_OMB_BUDDY     Behavior / InitResources
    183, 184, 299, 300,  // WALL_SIGN, SIGN_POST (Behavior), ARROW_SIGN_LEFT / RIGHT (messages)
    185,  // TOAD              Behavior / InitResources
    186,  // PRINCESS_PEACH    UpdateLookAt, letter scene
    187,  // RABBIT (MIPS)     daMip_c.cpp:510,616 ClosestPlayer -> StartTalk
    188,  // KOOPA_THE_QUICK   func_ov062_0211a9c4 (+0x398 = ClosestPlayer -> StartTalk)
    234,  // HOOT_THE_OWL      func_ov094_02135c28 ClosestPlayer -> StartTalk
    235,  // LAKITU_BRO        func_ov085_0212d5dc / 0212df84: ClosestPlayer + camera set (castle intro)
    257,  // MOTHER_PENGUIN    d_a_pg_mthr.cpp:359,395 ClosestPlayer -> StartTalk
    259,  // RACING_PENGUIN    func_ov019_02112168 ClosestPlayer -> StartTalk
    265,  // LAKITU            Lakitu::Behavior
    267, 268,  // UKIKI_THIEF / UKIKI_STAR  daMky_c.cpp:435,864,883 ClosestPlayer -> StartTalk
    272, 273, 274, 275,  // SNOWMAN, HEAD, BODY, BREATH  daBgSnmBdy_c.cpp:279, SnowmanHead State0/3, SnowmanBreath
    229,  // RABBIT_KEY (TOTALS9, field item 18)  func_ov085_0212d9b8 / 0212dbdc / 0212dd10: ClosestPlayer EVERY frame of the caught cutscene, pose
          // (func_ov002_020c3ea0) at one step and release (020c3f18) at a later one: a friend closer to the key at the release step leaves
          // the first body in the pose for ever. The cutscene runs on MY body on every console.
    282, 283,  // KEY, LAST_STAR   func_ov089_0213162c camera set
};

bool class_is_talker(unsigned cls) {
    for (unsigned short c : kTalkerClasses) if (c == cls) return true;
    return false;
}

// Actors that asked a friend's body for a talk (see the StartTalk guard below): local player for a while.
struct Learned { const void *actor; unsigned t; };
Learned g_learned[16];
const unsigned kLearnedMs = 8000;
bool learned_actor(const void *a, unsigned now) {
    for (const Learned &l : g_learned) if (l.actor == a && (unsigned)(now - l.t) < kLearnedMs) return true;
    return false;
}
void learn_actor(const void *a, unsigned now) {
    int slot = -1;
    for (int i = 0; i < 16; ++i) {
        if (g_learned[i].actor == a) { slot = i; break; }
        if (slot < 0 && (!g_learned[i].actor || (unsigned)(now - g_learned[i].t) >= kLearnedMs)) slot = i;
    }
    if (slot < 0) slot = 0;
    g_learned[slot] = { a, now };
}

bool is_talker(const char *self) {
    if (!self) return false;
    if (class_is_talker(*(const unsigned short *)(self + 0x0c))) return true;
    const unsigned now = comms_hub_now_ms();
    if (learned_actor(self, now)) return true;
    const char *lb = local_body();      // my player is already in a talk with this actor
    return lb && *(const void *const *)(lb + 0x368) == self;
}

void *cp_impl(void *self, bool nonvanish) {
    ++g_st.cp_calls;
    if (!data_0209b458) {
        data_0208e380 = 0x7fffffff;
        data_0208e37c = (int)0x80000000;
        // COOPSPAWN9: from a new file's opening start until its seen bit the ROM's own pick runs (the hand-over and the cast
        // work on the local body, as for one player); coop_boot.cpp owns the flag.
        const bool lv = live() && !port_coop_boot_opening_hold();
        void *want = nullptr; int wseat = -1;
        if (lv) {
            place_friends();
            want = table_body((const char *)self, &wseat);
            if (coop_boss_prefer_local((const char *)self) || is_talker((const char *)self)) { want = local_body(); wseat = g_my; }   // Bowser's defeat talk, NPCs, camera / talk actors: my own player
            if (want && nonvanish && *((const unsigned char *)want + 0x6fb)) want = nullptr;   // a vanished player is skipped by this entry
        }
        for (int i = 0; i < (int)data_0209f21c; i++) {
            void *p = data_0209f394[i];
            if (!p) continue;
            if (nonvanish && *((unsigned char *)p + 0x6fb)) continue;
            const int d = Vec3_Dist((char *)self + 0x5c, (char *)p + 0x5c);
            if (d < data_0208e380) { data_0208e380 = d; data_0209b458 = p; }
            if (d > data_0208e37c) { data_0208e37c = d; data_0209b450 = p; }
        }
        if (want) {
            data_0209b458 = want;
            data_0208e380 = Vec3_Dist((char *)self + 0x5c, (char *)want + 0x5c);
            ++g_st.cp_used_table;
        } else if (lv) {
            ++g_st.cp_natural;
        }
        if (coop_world_active() && g_log) {
            const short slot = actor_slot((const char *)self);
            const unsigned cls = *(const unsigned short *)((const char *)self + 0x0c);
            if (slot >= 0 && slot < kTargetSlots && logged_cls(cls)) {
                const int s = coop_target_seat_of_body(data_0209b458);
                if (s != g_lastuse[slot]) {
                    g_lastuse[slot] = (short)s;
                    std::fprintf(stderr, "[cotgt] use seat=%d slot=%d cls=%u via=%s tick=%u wall=%lld\n", s, (int)slot, cls,
                                 want ? "table" : (lv ? "nearest" : "rom"), g_ticks, wall_ms());
                }
            }
        }
    }
    return data_0209b458;
}
void *__fastcall cp_replacement(void *self, void * /*edx*/) { return cp_impl(self, false); }
void *__fastcall cpnv_replacement(void *self, void * /*edx*/) { return cp_impl(self, true); }


// ---------------------------------------------------------------------------
// WHOMP / WHOMP KING DO NOT ASK ClosestPlayer. Whomp::InitResources copies the player table into the actor (+0x39c, four
// pointers) and func_ov079_02123f34 (src/func_ov079_02123f34.c) picks the nearest by HORIZONTAL distance every frame:
// +0x3ec the index, +0x3f4 the distance, +0x3b8 the angle, and +0x3f0 / +0x3f8 the nearest one that is roughly in front.
// A parked friend body shares my x and z, so the strict "<" always kept the lowest index. This is the same function with
// one difference: when the table names a seat for this king, that seat's body is the one chosen.
// ---------------------------------------------------------------------------
struct V3 { int x, y, z; };
void __fastcall whomp_replacement(char *self, void * /*edx*/) {   // MPSYNC1: daBtn_c's member, receiver in ECX
    ++g_st.cp_calls;
    const bool lv = live();
    void *want = nullptr; int wseat = -1;
    if (lv) {
        place_friends();
        want = table_body(self, &wseat);
    }
    short minAngle;
    int minDist, minDist2;
    *(short *)(self + 0x3b8) = *(short *)(self + 0x94);
    minAngle = *(short *)(self + 0x94);
    minDist2 = 0x7fffffff;
    minDist = 0x7fffffff;
    int widx = -1, wdist = 0; short wang = 0;
    for (int i = 0; i < 4; i++) {
        char *obj = *(char **)(self + 0x39c + i * 4);
        if (obj == 0) continue;
        V3 v; std::memcpy(&v, obj + 0x5c, sizeof v);
        const int dist = Vec3_HorzDist(self + 0x5c, &v);
        const short angle = Vec3_HorzAngle(self + 0x5c, &v);
        if (dist < minDist) {
            *(int *)(self + 0x3ec) = i;
            minDist = dist;
            minAngle = angle;
        }
        if (want && obj == (char *)want) { widx = i; wdist = dist; wang = angle; }
        if (_ZN8dActor_c14GetSubtractionEss(self, angle, *(short *)(self + 0x94)) < 0x2000) {
            if (dist < minDist2) {
                minDist2 = dist;
                *(int *)(self + 0x3f0) = i;
            }
        }
    }
    const char *how = lv ? "nearest" : "rom";
    if (want && widx >= 0) {
        *(int *)(self + 0x3ec) = widx;
        minDist = wdist;
        minAngle = wang;
        ++g_st.cp_used_table;
        how = "table";
    } else if (lv) {
        ++g_st.cp_natural;
        if (want) ++g_st.cp_unavailable;       // the table's seat is not in this king's own copy of the table
    }
    *(int *)(self + 0x3f4) = minDist;
    *(int *)(self + 0x3f8) = minDist2;
    *(short *)(self + 0x3b8) = minAngle;
    if (coop_world_active() && g_log) {
        const short slot = actor_slot(self);
        if (slot >= 0 && slot < kTargetSlots) {
            const int idx = *(int *)(self + 0x3ec);
            char *obj = (idx >= 0 && idx < 4) ? *(char **)(self + 0x39c + idx * 4) : nullptr;
            const int s = coop_target_seat_of_body(obj);
            if (s != g_lastuse[slot]) {
                g_lastuse[slot] = (short)s;
                std::fprintf(stderr, "[cotgt] use seat=%d slot=%d cls=%u via=%s idx=%d tick=%u wall=%lld\n", s, (int)slot,
                             (unsigned)*(const unsigned short *)(self + 0x0c), how, idx, g_ticks, wall_ms());
            }
        }
    }
}


// ---------------------------------------------------------------------------
// Player::StartTalk guard (FIX9A). Any actor, not only the listed classes, that asks a FRIEND's body to start a talk
// (a boss's pre-fight or defeat talk, a class nobody listed) is answered "not yet" and remembered: its next
// ClosestPlayer answer is my own player for kLearnedMs (is_talker), so the talk starts on this console. A call on my own
// body goes straight to the ROM's function (the five patched bytes are put back around the call; one thread).
// ---------------------------------------------------------------------------
unsigned char *g_st_addr = nullptr;
unsigned char g_st_orig[5];
unsigned char g_st_jmp[5];
typedef int (__fastcall *StartTalkFn)(void *, void *, void *, bool);

void write_code(unsigned char *at, const unsigned char *b) {
    DWORD old = 0;
    if (!VirtualProtect(at, 8, PAGE_EXECUTE_READWRITE, &old)) return;
    std::memcpy(at, b, 5);
    VirtualProtect(at, 8, old, &old);
    FlushInstructionCache(GetCurrentProcess(), at, 8);
}

bool is_player_body(const void *a) {
    for (int i = 0; i < (int)data_0209f21c && i < kPlayerMax; ++i) if (data_0209f394[i] == a) return true;
    return false;
}

int __fastcall starttalk_replacement(void *self, void *edx, void *actor, bool b) {
    if (live() && self != (void *)local_body() && is_player_body(self)) {
        learn_actor(actor, comms_hub_now_ms());
        ++g_st.talk_redirects;
        if (g_log) std::fprintf(stderr, "[cotgt] talk guard: actor %p cls=%u asked a friend's body, now local\n", actor,
                                actor ? (unsigned)*(const unsigned short *)((const char *)actor + 0x0c) : 0u);
        return 0;
    }
    write_code(g_st_addr, g_st_orig);
    const int r = ((StartTalkFn)g_st_addr)(self, edx, actor, b);
    write_code(g_st_addr, g_st_jmp);
    return r;
}

void *addr_of_pmf(Player *(dActor_c::*pmf)()) {
    static_assert(sizeof(pmf) >= sizeof(void *), "a pointer to member holds at least the address");
    void *p; std::memcpy(&p, &pmf, sizeof p);     // a non-virtual member: the first word is its address
    return p;
}

void *addr_of_btn_pmf(void (daBtn_c::*pmf)()) {
    void *p; std::memcpy(&p, &pmf, sizeof p);     // a non-virtual member: the first word is its address
    return p;
}

bool jump_patch(void *from_v, void *to_v, const char *what) {
    unsigned char *from = (unsigned char *)from_v;
    DWORD old = 0;
    if (!VirtualProtect(from, 8, PAGE_EXECUTE_READWRITE, &old)) {
        std::fprintf(stderr, "[cotgt] cannot patch %s (VirtualProtect %lu)\n", what, GetLastError());
        return false;
    }
    unsigned char b[5];
    b[0] = 0xE9;
    const int rel = (int)((unsigned char *)to_v - from - 5);
    std::memcpy(b + 1, &rel, 4);
    std::memcpy(from, b, 5);
    VirtualProtect(from, 8, old, &old);
    FlushInstructionCache(GetCurrentProcess(), from, 8);
    std::fprintf(stderr, "[cotgt] %s replaced at %p\n", what, (void *)from);
    return true;
}

void install_patch() {
    if (g_patched || g_patch_failed) return;
    if (!jump_patch(addr_of_pmf(&dActor_c::ClosestPlayer), (void *)&cp_replacement, "ClosestPlayer")) { g_patch_failed = true; return; }
    jump_patch(addr_of_pmf(&dActor_c::ClosestNonVanishPlayer), (void *)&cpnv_replacement, "ClosestNonVanishPlayer");
    jump_patch(addr_of_btn_pmf(&daBtn_c::func_ov079_02123f34), (void *)&whomp_replacement, "Whomp nearest-player pick");
    {
        int (Player::*pmf)(fBase_c &, bool) = &Player::StartTalk;
        void *a; std::memcpy(&a, &pmf, sizeof a);
        g_st_addr = (unsigned char *)a;
        std::memcpy(g_st_orig, g_st_addr, 5);
        if (jump_patch(a, (void *)&starttalk_replacement, "Player::StartTalk guard")) std::memcpy(g_st_jmp, g_st_addr, 5);
        else g_st_addr = nullptr;
    }
    g_patched = true;
    ++g_st.patched;
}

// ---------------------------------------------------------------------------
// The decider (group owner only) and the table.
// ---------------------------------------------------------------------------
void reset_level() {
    for (int i = 0; i < kTargetSlots; ++i) { g_tgt[i].seat = kTargetNoSeat; g_tgt[i].t = 0; g_dec[i] = { -1, 0, -1, 0, -2, 0 }; g_lastuse[i] = -2; }
}

CoopKeyWire wire_key() {
    CoopKeyWire w; w.level = (uint16_t)g_key.level; w.act = (uint8_t)g_key.act; w.reserved = 0;
    return w;
}

bool is_player_ptr(const void *a) {
    for (int i = 0; i < kPlayerMax; ++i) if (data_0209f394[i] == a) return true;
    return false;
}

bool take_token(unsigned now) {
    if (g_tok_t == 0) g_tok_t = now;
    const unsigned dt = now - g_tok_t; g_tok_t = now;
    g_tokens = (std::min)(6.0, g_tokens + dt * 0.012);       // 12 datagrams per second, burst 6
    if (g_tokens < 1.0) return false;
    g_tokens -= 1.0;
    return true;
}

void send_recs(const TargetRec *r, int n) {
    unsigned char buf[kPayloadMax];
    TargetHead h; std::memset(&h, 0, sizeof h);
    h.key = wire_key(); h.count = (uint8_t)n;
    std::memcpy(buf, &h, sizeof h);
    std::memcpy(buf + sizeof h, r, sizeof(TargetRec) * (size_t)n);
    if (coop_send_world(kWorldTargetSet, buf, (int)sizeof h + (int)sizeof(TargetRec) * n)) { ++g_st.sent; g_st.recs_sent += (unsigned)n; }
}

void decide(unsigned now) {
    PInfo pl[kSeats];
    const int np = gather(pl);
    TargetRec out[kTargetMaxRecs]; int nout = 0;
    bool seen[kTargetSlots]; std::memset(seen, 0, sizeof seen);
    if (np > 0) {
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
            const short slot = actor_slot(a);
            if (slot < 0 || slot >= kTargetSlots || seen[slot]) continue;
            if (is_player_ptr(a)) continue;
            if (*(const unsigned char *)(a + 0x0f) || *(const unsigned char *)(a + 0x0e) == 2) continue;
            const int ax = rd32(a, 0x5c), ay = rd32(a, 0x60), az = rd32(a, 0x64);
            int best = -1; double bd = 1e18;
            for (int i = 0; i < np; ++i) {
                const double d = dist_units(ax, ay, az, pl[i].x, pl[i].y, pl[i].z);
                if (d < bd || (d == bd && best >= 0 && pl[i].seat < pl[best].seat)) { bd = d; best = i; }
            }
            if (best < 0 || bd > kRangeUnits) continue;
            seen[slot] = true;
            ++g_st.decisions;
            const int nearest = pl[best].seat;
            Dec &d = g_dec[slot];
            bool cur_ok = false;
            for (int i = 0; i < np; ++i) if (pl[i].seat == d.cur) cur_ok = true;
            if (!cur_ok) {
                if (d.cur >= 0) ++g_st.drops_at_once;
                if (g_log && logged_cls(*(const unsigned short *)(a + 0x0c)))
                    std::fprintf(stderr, "[cotgt] pick slot=%d cls=%u %d->%d (no hold) tick=%u wall=%lld\n", (int)slot,
                                 (unsigned)*(const unsigned short *)(a + 0x0c), d.cur, nearest, g_ticks, wall_ms());
                d.cur = nearest; d.cur_t = now; d.cand = -1;
            } else if (nearest != d.cur) {
                if (d.cand != nearest) { d.cand = nearest; d.cand_t = now; ++g_st.holds; }
                else if ((unsigned)(now - d.cand_t) >= g_hold_ms) {
                    if (g_log && logged_cls(*(const unsigned short *)(a + 0x0c)))
                        std::fprintf(stderr, "[cotgt] switch slot=%d cls=%u %d->%d after %ums tick=%u wall=%lld\n", (int)slot,
                                     (unsigned)*(const unsigned short *)(a + 0x0c), d.cur, nearest, now - d.cand_t, g_ticks, wall_ms());
                    d.cur = nearest; d.cur_t = now; d.cand = -1; ++g_st.switches;
                }
            } else {
                d.cand = -1;
            }
            g_tgt[slot].seat = (uint8_t)d.cur; g_tgt[slot].t = now;
            if (d.pub != d.cur || (unsigned)(now - d.pub_t) >= (unsigned)kBeatMs) {
                if (nout < kTargetMaxRecs) {
                    out[nout].slot = (uint16_t)slot; out[nout].seat = (uint8_t)d.cur; out[nout].reserved = 0; ++nout;
                    d.pub = d.cur; d.pub_t = now;
                }
            }
        }
    }
    // entries whose actor is gone or out of range: forget them here (receivers let them time out)
    for (int s = 0; s < kTargetSlots; ++s)
        if (!seen[s] && g_tgt[s].seat != kTargetNoSeat && (unsigned)(now - g_tgt[s].t) > 500u) { g_tgt[s].seat = kTargetNoSeat; g_dec[s].cur = -1; g_dec[s].cand = -1; g_dec[s].pub = -2; }
    if (nout > 0 && take_token(now)) send_recs(out, nout);
}

// ---------------------------------------------------------------------------
// Receiving.
// ---------------------------------------------------------------------------
void on_target(int seat, const unsigned char *p, int len) {
    if (!g_on) { ++g_st.drop_off; return; }
    if (seat < 0 || seat >= kSeats || seat == g_my) { ++g_st.drop_seat; return; }
    if (!coop_target_validate(p, len)) { ++g_st.drop_bad; return; }
    ++g_st.rx;
    TargetHead h; std::memcpy(&h, p, sizeof h);
    if (!g_key.valid() || h.key.level != (uint16_t)g_key.level || h.key.act != (uint8_t)g_key.act) { ++g_st.drop_group; return; }
    if (!((coop_group_member_mask() >> seat) & 1u)) { ++g_st.drop_member; return; }
    if (seat != coop_group_owner()) { ++g_st.drop_notowner; return; }
    const unsigned now = comms_hub_now_ms();
    for (int i = 0; i < (int)h.count; ++i) {
        TargetRec r; std::memcpy(&r, p + sizeof h + sizeof(TargetRec) * (size_t)i, sizeof r);
        ++g_st.recs_rx;
        if (r.seat != kTargetNoSeat && !((coop_group_member_mask() >> r.seat) & 1u) && r.seat != g_my) continue;   // a seat I do not know
        const bool changed = g_tgt[r.slot].seat != r.seat;
        g_tgt[r.slot].seat = r.seat; g_tgt[r.slot].t = now;
        g_dec[r.slot].cur = r.seat == kTargetNoSeat ? -1 : r.seat; g_dec[r.slot].cur_t = now; g_dec[r.slot].cand = -1;
        g_dec[r.slot].pub = r.seat == kTargetNoSeat ? -2 : r.seat; g_dec[r.slot].pub_t = now;
        if (changed) ++g_st.applied;
    }
}

void on_world(int seat, int sub, const unsigned char *p, int len, void *) {
    if (sub == kWorldTargetSet) on_target(seat, p, len);
    else if (sub >= kWorldBossState && sub <= kWorldBossDefeat) coop_boss_on_world(seat, sub, p, len);
    else ++g_st.drop_bad;
}

void ensure_registered() {
    if (g_regd) return;
    g_regd = true;
    g_my = comms_hub_my_seat();
    coop_world_register(kWorldTargetSet, kWorldBossLast, on_world);
}

void do_forge(const char *name) {
    unsigned char b[kPayloadMax]; std::memset(b, 0, sizeof b);
    TargetHead h; std::memset(&h, 0, sizeof h);
    h.key = wire_key(); h.count = 1;
    TargetRec r; std::memset(&r, 0, sizeof r);
    r.slot = 1; r.seat = (uint8_t)g_my;
    int len = (int)sizeof h + (int)sizeof r;
    if (!std::strcmp(name, "short")) len -= 2;
    else if (!std::strcmp(name, "long")) len += 3;
    else if (!std::strcmp(name, "count")) h.count = 9;
    else if (!std::strcmp(name, "seat")) r.seat = 9;
    else if (!std::strcmp(name, "slot")) r.slot = 600;
    else if (!std::strcmp(name, "key")) h.key.act = (uint8_t)((g_key.act + 1) & 7);
    else if (!std::strcmp(name, "flags")) h.flags = 1;
    else if (!std::strcmp(name, "nonowner")) { /* a well-formed set from whoever runs the knob */ }
    else return;
    std::fprintf(stderr, "[cotgt-knob] forge %s\n", name);
    std::memcpy(b, &h, sizeof h); std::memcpy(b + sizeof h, &r, sizeof r);
    coop_send_world(kWorldTargetSet, b, len);
}

// Proof knobs for the per-boss check: SM64DS_COOP_TARGET_STAND=F:cls:dist[:every] puts the local player dist units beside the
// first actor of class cls at tick F (and again every N ticks); SM64DS_COOP_TARGET_SAMPLE=cls logs [cts] lines for every
// actor of that class every 500 ms: wall ms, slot, position, the seat the table gives it.
void proof_knobs(unsigned now) {
    char *pl = local_body();
    if (!pl) return;
    if (g_stand_on && g_ticks >= g_stand_tick) {
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
            if ((int)*(const unsigned short *)(a + 0x0c) != g_stand_cls || is_player_ptr(a)) continue;
            const int32_t x = rd32(a, 0x5c) + (g_stand_dist << 12), y = rd32(a, 0x60) + 0x10000, z = rd32(a, 0x64), zero = 0;
            std::memcpy(pl + 0x5c, &x, 4); std::memcpy(pl + 0x60, &y, 4); std::memcpy(pl + 0x64, &z, 4); std::memcpy(pl + 0x98, &zero, 4);
            std::fprintf(stderr, "[cts-knob] stand local player %d units from class %d slot=%d at %d,%d,%d wall=%lld\n", g_stand_dist, g_stand_cls,
                         (int)actor_slot(a), x >> 12, y >> 12, z >> 12, wall_ms());
            if (g_stand_every) g_stand_tick = g_ticks + g_stand_every; else g_stand_on = false;
            break;
        }
    }
    if (g_sample_cls && (unsigned)(now - g_sample_t) >= 500u) {
        g_sample_t = now;
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
            if ((int)*(const unsigned short *)(a + 0x0c) != g_sample_cls || is_player_ptr(a)) continue;
            std::fprintf(stderr, "[cts] w=%lld slot=%d cls=%d pos=%d,%d,%d tgt=%d me=%d\n", wall_ms(), (int)actor_slot(a), g_sample_cls,
                         rd32(a, 0x5c) >> 12, rd32(a, 0x60) >> 12, rd32(a, 0x64) >> 12, coop_target_seat_of_slot(actor_slot(a)), g_my);
        }
    }
}

void on_frame(unsigned now) {
    read_env();
    if (!coop_world_active()) return;
    ensure_registered();
    install_patch();   // also with targeting OFF: then the replacement IS the ROM loop (the control logs what each console picks)
    int lv = -1, ac = 0;
    if (!coop_group_established() || !coop_group_key(&lv, &ac)) return;
    const CoopKey nk(lv, ac);
    if (nk != g_key) { reset_level(); g_key = nk; }
    ++g_ticks; ++g_st.ticks;
    if (g_stand_on || g_sample_cls) proof_knobs(now);
    if (!g_on) return;
    if ((unsigned)(now - g_last_decide) >= (unsigned)kDecideMs) {
        g_last_decide = now;
        if (coop_group_i_am_owner()) decide(now);
    }
    for (int i = 0; i < g_nforge; ++i)
        if (!g_forge[i].done && g_ticks >= g_forge[i].tick) { g_forge[i].done = true; do_forge(g_forge[i].name); }
    if (g_log && (unsigned)(now - g_last_stat) >= 4000u) {
        g_last_stat = now;
        const CoopTargetStats &s = g_st;
        std::fprintf(stderr, "[cotgtstat] seat=%d owner=%d ticks=%llu dec=%llu sw=%llu hold=%llu atonce=%llu sent=%llu recs=%llu rx=%llu rrx=%llu app=%llu "
                     "cp=%llu table=%llu nat=%llu drop bad%llu grp%llu own%llu mem%llu off%llu seat%llu\n",
                     g_my, coop_group_owner(), s.ticks, s.decisions, s.switches, s.holds, s.drops_at_once, s.sent, s.recs_sent, s.rx, s.recs_rx, s.applied,
                     s.cp_calls, s.cp_used_table, s.cp_natural, s.drop_bad, s.drop_group, s.drop_notowner, s.drop_member, s.drop_off, s.drop_seat);
    }
}

void on_member_gone(int seat) {
    if (seat < 0 || seat >= kSeats) return;
    for (int i = 0; i < kTargetSlots; ++i) {
        if (g_tgt[i].seat == (uint8_t)seat) g_tgt[i].seat = kTargetNoSeat;
        if (g_dec[i].cur == seat) { g_dec[i].cur = -1; g_dec[i].cand = -1; g_dec[i].pub = -2; }
    }
    if (g_log) std::fprintf(stderr, "[cotgt] seat %d gone: its targets drop at once wall=%lld\n", seat, wall_ms());
}

struct Registrar {
    Registrar() {
        reset_level();
        coop_on_frame(on_frame);
        coop_on_member_gone(on_member_gone);
    }
} g_registrar;

}  // namespace

// ---------------------------------------------------------------------------
// Public.
// ---------------------------------------------------------------------------
CoopTargetStats coop_target_stats() { return g_st; }

bool coop_target_validate(const void *payload, int len) {
    if (!payload || len < (int)sizeof(TargetHead) + (int)sizeof(TargetRec)) return false;
    const unsigned char *p = static_cast<const unsigned char *>(payload);
    TargetHead h; std::memcpy(&h, p, sizeof h);
    if (!coop_key_ok(h.key) || h.flags != 0 || h.reserved != 0) return false;
    if (h.count < 1 || h.count > kTargetMaxRecs) return false;
    if (len != (int)sizeof h + (int)sizeof(TargetRec) * (int)h.count) return false;
    for (int i = 0; i < (int)h.count; ++i) {
        TargetRec r; std::memcpy(&r, p + sizeof h + sizeof(TargetRec) * (size_t)i, sizeof r);
        if (r.slot >= kTargetSlots || r.reserved != 0) return false;
        if (r.seat >= kSeats && r.seat != kTargetNoSeat) return false;
    }
    return true;
}

int coop_target_seat_of_slot(int slot) {
    if (slot < 0 || slot >= kTargetSlots) return -1;
    return fresh(g_tgt[slot], comms_hub_now_ms()) ? (int)g_tgt[slot].seat : -1;
}

int coop_target_seat_of_actor(const void *actor) {
    return actor ? coop_target_seat_of_slot(actor_slot((const char *)actor)) : -1;
}

void *coop_target_body_for_seat(int seat) {
    if (seat < 0 || seat >= kSeats) return nullptr;
    if (seat == g_my) return local_body();
    if (!((coop_group_member_mask() >> seat) & 1u)) return nullptr;
    const int nb = (int)data_0209f21c < kPlayerMax ? (int)data_0209f21c : kPlayerMax;
    for (int i = 0; i < nb; ++i) {
        if (i == (int)data_0209f250 || !data_0209f394[i]) continue;
        if (port_adventure_ghost_seat(i) != seat) continue;
        int dp[3];
        if (!drawpos_sticky(i, dp)) return nullptr;
        char *b = (char *)data_0209f394[i];
        wr32(b, 0x5c, dp[0]); wr32(b, 0x60, dp[1]); wr32(b, 0x64, dp[2]); b[0x6fb] = 0;
        return b;
    }
    return nullptr;
}

void *coop_target_player_for(const void *actor) {
    int s = -1;
    return actor ? table_body((const char *)actor, &s) : nullptr;
}

int coop_target_seat_of_body(const void *body) {
    if (!body) return -1;
    if (body == local_body()) return g_my;
    const int nb = (int)data_0209f21c < kPlayerMax ? (int)data_0209f21c : kPlayerMax;
    for (int i = 0; i < nb; ++i)
        if (data_0209f394[i] == body) return port_adventure_ghost_seat(i);
    return -1;
}

}  // namespace port

// ---------------------------------------------------------------------------
// PLATFORM COLLIDER RANGE (run after093, lane SEESAW1). dBgActor_c::IsClsnInRange / IsClsnInRangeOnScreen keep a
// platform's mesh collider in the world only while ClosestPlayer is within range (src/runtime/actor/d_bg_actor.cpp).
// In a coop room ClosestPlayer answers the shared target seat, which holds for 1000 ms and can be the friend at the far
// end, so the collider dropped under the player walking onto the Bob-omb Battlefield seesaw (proof: out/SEESAW1, rows
// cb3 / cb4, collider slot byte +0x138 = 0x18 while the player stood over it). The range test asks this instead: the
// ROM's own nearest-of-all-players loop, with every friend at his drawn pose. ClosestPlayer still runs first, so its
// cache and the two distance words are what the cartridge leaves behind.
// ---------------------------------------------------------------------------
struct dBgActor_c;
extern "C" Player *port_bg_range_player(dBgActor_c *self) {
    dActor_c *const a = (dActor_c *)self;
    Player *const cp = a->ClosestPlayer();
    if (!port::live()) return cp;
    port::place_friends();
    void *best = nullptr;
    int bd = 0x7fffffff;
    for (int i = 0; i < (int)data_0209f21c; i++) {
        void *p = data_0209f394[i];
        if (!p) continue;
        const int d = Vec3_Dist((const char *)a + 0x5c, (const char *)p + 0x5c);
        if (d < bd) { bd = d; best = p; }
    }
    return best ? (Player *)best : cp;
}
