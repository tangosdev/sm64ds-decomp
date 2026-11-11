// A COOP FRIEND'S LOST CAP ON MY CONSOLE. Run coopfix1, lane CAPLOSS1.
//
// THE BUG (Tango's field test on 1.0.2): when Klepto took a player's cap, the other consoles showed that cap wrong.
// What the code did, per console A watching friend B:
//   1. B was drawn on A WEARING his cap while it was lost, and if A had lost his own cap every friend was drawn
//      without one: comms_sync.cpp latched the ghost's no-cap byte (+0x701) from Player::HasNoCap, which reads the
//      LOCAL save's cap-lost bit (SaveData::HasPlayerLostCap). B's cap state was never on the wire.
//   2. A's own Klepto, the wind (daHyuhyu_c) and the Snowman's Land breath take the player from a collider, and a
//      friend's drawn body counts: on A they set A's OWN cap-lost bit and spawned a cap of B's character on A.
//   3. That stray cap was a real lost cap on A: A could pick it up and turn into B's character, and B's drawn body
//      could pick up A's own lost cap on A.
//   4. B's real lost cap (in his Klepto's claws, then on the ground) did not exist on A at all.
//
// WHAT THIS FILE DOES (coop rooms only; a solo, party or VS run never reaches it):
//   * port_coop_cap_foreign (defined in src/game/actors/d_a_obj_mario_cap.cpp, read there and by d_a_jango.cpp,
//     d_a_hyuhyu.cpp, d_a_snm_bth.cpp under _MSC_VER): 1 for a friend's body. A cap is lost and picked up only by the
//     console that runs that player. A friend can never pick up my cap, so he never becomes my character by it (the
//     cartridge has one player; this is the coop rule chosen: a lost cap belongs to its owner).
//   * SYNW 0x68 LOST_CAP: every console sends, for its own player, HasNoCap and where its lost cap is (none, held by
//     something, or loose with a position). On change (three times, 100 ms apart), at 10 Hz while a loose cap moves,
//     and once a second otherwise. An old build drops the sub-type as unknown.
//   * coop_cap_seat_nocap(): comms_sync.cpp's ghost latch uses the friend's own HasNoCap instead of my save.
//   * A COPY of the friend's cap (class 269, OBJ_MARIO_CAP, type 13: the cartridge's dormant cap, which has no touch
//     check) while his is lost: held by my own Klepto while his says held (my Klepto then carries it in its claws,
//     the ROM's own carry), at his position while it is loose, gone the moment he has it back. It is taken off the
//     cap-icon list (no map icon, no part in the per-character cap choice), Yoshi cannot eat it, and nobody on my
//     console can pick it up.
//
// PORT_HOST_ABI. Actor offsets are the ROM's (32-bit host): uniqueID +0x04, param1 +0x08, class +0x0c, position
// +0x5c, area +0xcc, the cap's override matrix +0xc8 (CAP_OVERRIDE_MATRIX in d_a_obj_mario_cap.cpp).
//
// ENVIRONMENT (proof knobs, all inert unless named):
//   SM64DS_COOP_CAP=0           MUST-FAIL control: no hook, no wire, no copy, the old ghost latch (the 1.0.2 behaviour)
//   SM64DS_COOP_CAP_LOG=1       [coopcap] lines
//   SM64DS_COOP_CAP_TRACE=N     every N frames one [coopcap] trace line (my state, every friend's, every copy, Klepto)
//   SM64DS_COOP_CAP_STEAL=F[:friend]  at frame F the first Klepto runs its own contact test (func_ov062_0211b51c) in
//                               SWOOP with cylinder 2 touched by my body (or the first friend's body): the theft path
//   SM64DS_COOP_CAP_HIT=F       at frame F the first Klepto runs its own contact test with cylinder 1 punched by my body
//   SM64DS_COOP_CAP_GOTO=F[:n]  from frame F for n frames (default 4) put my body on my own lost cap, or else on the first
//                               copy of a friend's cap (the ROM's own touch then runs, or does not)
//   SM64DS_COOP_CAP_SHOT=f1,f2  capl_f<N>.bmp (the top screen) at those frames of this module

#include "coop_wire.h"
#include "coop_world.h"
#include "comms_hub.h"

#include "daObjMarioCap_c.h"
#include "daJango_c.h"
#include "Player.h"

#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern unsigned char data_0209f21c;
char *_ZN8dActor_c10FindWithIDEj(unsigned id);
char *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned id, char *prev);
char *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b, const void *pos, const void *v16, int area, int f);
void _ZN7fBase_c18MarkForDestructionEv(char *c);
int _ZN6Player8HasNoCapEv(void *self);
int _ZN8SaveData16HasPlayerLostCapEv(void);
int port_adventure_ghost_seat(int slot);
extern int (*port_coop_cap_foreign)(const void *player);
extern const void *port_level_fb_ptr;
}
namespace ntr { struct Framebuffer; bool ppu_write_bmp(const char *path, const Framebuffer &fb); }

// Klepto's state records (src/game/actors/d_a_jango.cpp): HURT and SWOOP, by their own types so the names link.
typedef int (daJango_c::*JangoPMF)();
extern JangoPMF data_ov062_0211e14c[2];
struct JangoSwoopRec;
extern JangoSwoopRec data_ov062_0211e18c;

namespace port {

enum : uint8_t { kWorldLostCap = 0x68 };
enum : uint8_t { kCapNone = 0, kCapHeld = 1, kCapLoose = 2 };

#pragma pack(push, 1)
// LOST_CAP payload (after the 12-byte head), 20 bytes.
struct CoopCapWire {
    CoopKeyWire key;
    uint8_t ver;         // 1
    uint8_t state;       // kCapNone / kCapHeld / kCapLoose
    uint8_t model;       // the cap's character 0..2 (0 with kCapNone)
    uint8_t flags;       // bit 0: the sender's Player::HasNoCap
    int32_t x, y, z;     // Fix12, the cap's position (0 with kCapNone)
};
#pragma pack(pop)
static_assert(sizeof(CoopCapWire) == 20, "lost cap is 20 bytes");

bool coop_cap_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(CoopCapWire)) return false;
    CoopCapWire w; std::memcpy(&w, p, sizeof w);
    const int lim = 0x7d00000;        // func_02010da4's clamp: 32000 units
    if (!coop_key_ok(w.key) || w.ver != 1 || w.state > kCapLoose || w.model > 2 || w.flags > 1) return false;
    if (w.x < -lim || w.x > lim || w.y < -lim || w.y > lim || w.z < -lim || w.z > lim) return false;
    if (w.state == kCapNone && (w.model || w.x || w.y || w.z)) return false;
    return true;
}

namespace {

enum : int { kSeats = 16, kBodies = 16, kCapClass = 269, kKleptoClass = 239, kPlayerClass = 191 };
enum : unsigned { kFreshMs = 4000, kBeatMs = 1000, kMoveMs = 100, kResendMs = 100 };

bool g_env = false, g_off = false, g_log = false, g_regd = false;
int g_trace = 0;
unsigned g_lf = 0;

struct Stats { unsigned sent, recv, bad, spawned, removed, held_by_klepto, foreign_refused; } g_st = {};

// ---- what I send ----
struct Tx { bool have; uint8_t state, model, flags; int x, y, z; unsigned last_ms, resend_ms; int resends; } g_tx = {};

// ---- what friends sent, and my copy of each friend's cap ----
struct Rx { bool have; int level, act; uint8_t state, model, flags; int x, y, z; unsigned t_ms; };
Rx g_rx[kSeats];
struct Copy { unsigned uid; char *a; uint8_t model; };
Copy g_copy[kSeats];

unsigned now_ms() { return GetTickCount(); }
int rd32(const void *a, int off) { int v; std::memcpy(&v, (const char *)a + off, 4); return v; }
void wr32(void *a, int off, int v) { std::memcpy((char *)a + off, &v, 4); }
unsigned uid_of(const void *a) { return (unsigned)rd32(a, 0x04); }
bool alive(unsigned uid, const void *a) { return uid && a && _ZN8dActor_c10FindWithIDEj(uid) == a; }
char *local_body() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < kBodies) ? (char *)data_0209f394[me] : nullptr;
}
bool is_copy(const void *a) {
    for (const Copy &c : g_copy) if (c.uid && c.a == a) return true;
    return false;
}

void read_env() {
    if (g_env) return;
    g_env = true;
    const char *e = std::getenv("SM64DS_COOP_CAP");
    g_off = e && e[0] == '0' && !e[1];
    const char *l = std::getenv("SM64DS_COOP_CAP_LOG");
    g_log = l && l[0] && l[0] != '0';
    if (const char *t = std::getenv("SM64DS_COOP_CAP_TRACE")) g_trace = std::atoi(t);
    if (g_trace > 0) g_log = true;
}

// 1 for a friend's body in a coop room (the src hooks' question).
int cap_foreign(const void *pl) {
    if (!pl) return 0;
    read_env();
    if (g_off || !coop_world_active()) return 0;
    const char *me = local_body();
    if (pl == me) return 0;
    const int nb = (int)data_0209f21c < kBodies ? (int)data_0209f21c : kBodies;
    for (int i = 0; i < nb; ++i)
        if (data_0209f394[i] == pl) {
            ++g_st.foreign_refused;
            if (g_log && (g_st.foreign_refused <= 20 || (g_st.foreign_refused % 100) == 0))
                std::fprintf(stderr, "[coopcap] lf=%u a cap touch / theft on body %d (seat %d) refused: not my player (%u so far)\n",
                             g_lf, i, port_adventure_ghost_seat(i), g_st.foreign_refused);
            return 1;
        }
    return 0;
}
struct ForeignHook { ForeignHook() { port_coop_cap_foreign = cap_foreign; } } g_foreign_hook;

daObjMarioCap_c *as_cap(char *a) { return (daObjMarioCap_c *)a; }
daJango_c *first_klepto() { return (daJango_c *)_ZN8dActor_c15FindWithActorIDEjPS_(kKleptoClass, nullptr); }

// My own lost cap: a cap actor of the lost-cap types (0 Klepto / dropped, 1 wind, 2 Ukiki, 3, 12 fall) that is not a
// copy of a friend's and is not dormant. Held = something set its override matrix (Klepto, Ukiki).
char *my_lost_cap(bool *held) {
    for (char *a = _ZN8dActor_c15FindWithActorIDEjPS_(kCapClass, nullptr); a; a = _ZN8dActor_c15FindWithActorIDEjPS_(kCapClass, a)) {
        if (is_copy(a)) continue;
        daObjMarioCap_c *c = as_cap(a);
        const int t = c->mType;
        if (t != 0 && t != 1 && t != 2 && t != 3 && t != 12) continue;
        if (c->mDormant == 1) continue;
        *held = rd32(a, 0xc8) != 0;
        return a;
    }
    return nullptr;
}

void on_world(int seat, int sub, const unsigned char *p, int len, void *) {
    if (sub != kWorldLostCap || seat < 0 || seat >= kSeats || seat == comms_hub_my_seat() || !coop_cap_validate(p, len)) {
        ++g_st.bad;
        return;
    }
    CoopCapWire w; std::memcpy(&w, p, sizeof w);
    Rx &r = g_rx[seat];
    const bool changed = !r.have || r.state != w.state || r.flags != w.flags;
    r.have = true; r.level = w.key.level; r.act = w.key.act; r.state = w.state; r.model = w.model; r.flags = w.flags;
    r.x = w.x; r.y = w.y; r.z = w.z; r.t_ms = now_ms();
    ++g_st.recv;
    if (g_log && changed)
        std::fprintf(stderr, "[coopcap] in lf=%u from seat %d: cap %s model %d at (%d,%d,%d), nocap %d\n", g_lf, seat,
                     w.state == kCapNone ? "none" : w.state == kCapHeld ? "HELD" : "LOOSE", w.model, w.x >> 12, w.y >> 12,
                     w.z >> 12, w.flags & 1);
}

void exit_line() {
    std::fprintf(stderr, "[coopcap:exit] sent %u, received %u (bad %u), copies spawned %u removed %u, carried by my Klepto %u, "
                 "friend-body cap touches refused %u%s\n", g_st.sent, g_st.recv, g_st.bad, g_st.spawned, g_st.removed,
                 g_st.held_by_klepto, g_st.foreign_refused, g_off ? " (CONTROL SM64DS_COOP_CAP=0)" : "");
}

void ensure_registered() {
    if (g_regd) return;
    g_regd = true;
    const bool ok = coop_world_register(kWorldLostCap, kWorldLostCap, on_world);
    std::atexit(exit_line);
    if (g_log || !ok) std::fprintf(stderr, "[coopcap] SYNW sub-type 0x%02x %s\n", kWorldLostCap, ok ? "registered" : "NOT registered (range taken)");
}

// ---- sending ---------------------------------------------------------------------------------------
void send_tick(char *me, int lv, int ac) {
    bool held = false;
    char *cap = my_lost_cap(&held);
    Tx n = {};
    n.flags = (uint8_t)(_ZN6Player8HasNoCapEv(me) ? 1 : 0);
    if (cap) {
        n.state = held ? kCapHeld : kCapLoose;
        n.model = (uint8_t)(as_cap(cap)->mModelIndex & 3);
        n.x = rd32(cap, 0x5c); n.y = rd32(cap, 0x60); n.z = rd32(cap, 0x64);
    }
    const unsigned t = now_ms();
    const bool changed = !g_tx.have || n.state != g_tx.state || n.flags != g_tx.flags || n.model != g_tx.model;
    const long long dx = (long long)(n.x - g_tx.x) >> 12, dy = (long long)(n.y - g_tx.y) >> 12, dz = (long long)(n.z - g_tx.z) >> 12;
    const bool moved = n.state != kCapNone && dx * dx + dy * dy + dz * dz > 20 * 20;
    bool due = changed || (t - g_tx.last_ms >= kBeatMs) || (moved && t - g_tx.last_ms >= kMoveMs);
    if (!changed && g_tx.resends > 0 && t - g_tx.resend_ms >= kResendMs) due = true;
    if (!due) return;
    CoopCapWire w = {};
    w.key.level = (uint16_t)lv; w.key.act = (uint8_t)ac; w.key.reserved = 0;
    w.ver = 1; w.state = n.state; w.model = n.state ? n.model : 0; w.flags = n.flags;
    if (n.state) { w.x = n.x; w.y = n.y; w.z = n.z; }
    if (coop_send_world(kWorldLostCap, &w, (int)sizeof w)) ++g_st.sent;
    if (g_log && changed)
        std::fprintf(stderr, "[coopcap] out lf=%u: my cap %s model %d at (%d,%d,%d), nocap %d, my save lost bit %d\n", g_lf,
                     n.state == kCapNone ? "none" : n.state == kCapHeld ? "HELD" : "LOOSE", n.model, n.x >> 12, n.y >> 12, n.z >> 12,
                     n.flags, _ZN8SaveData16HasPlayerLostCapEv() ? 1 : 0);
    const int left = changed ? 2 : (g_tx.resends > 0 ? g_tx.resends - 1 : 0);
    n.have = true; n.last_ms = t; n.resend_ms = t; n.resends = left;
    g_tx = n;
}

// ---- my copies of friends' caps --------------------------------------------------------------------
void drop_copy(int seat, const char *why) {
    Copy &c = g_copy[seat];
    if (!c.uid) return;
    if (alive(c.uid, c.a)) {
        daJango_c *k = first_klepto();
        if (k && (unsigned)k->mHeldActorID == c.uid) k->mHeldActorID = 0;
        _ZN7fBase_c18MarkForDestructionEv(c.a);
    }
    ++g_st.removed;
    if (g_log) std::fprintf(stderr, "[coopcap] lf=%u copy of seat %d's cap removed (%s)\n", g_lf, seat, why);
    c = Copy();
}

void tend_copy(int seat, const Rx &r, char *me) {
    Copy &c = g_copy[seat];
    if (c.uid && (!alive(c.uid, c.a) || c.model != r.model)) {
        if (alive(c.uid, c.a)) drop_copy(seat, "model changed");
        else { c = Copy(); if (g_log) std::fprintf(stderr, "[coopcap] lf=%u copy of seat %d's cap went away on its own\n", g_lf, seat); }
    }
    if (!c.uid) {
        int pos[3] = {r.x, r.y, r.z};
        const int area = (int)*(const signed char *)(me + 0xcc);
        char *a = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(kCapClass, ((unsigned)r.model << 8) | 13u, pos, nullptr, area, -1);
        if (!a) return;
        c.uid = uid_of(a); c.a = a; c.model = r.model;
        ++g_st.spawned;
        if (g_log) std::fprintf(stderr, "[coopcap] lf=%u copy of seat %d's cap spawned (model %d, uid %u) at (%d,%d,%d)\n", g_lf, seat,
                                r.model, c.uid, r.x >> 12, r.y >> 12, r.z >> 12);
    }
    daObjMarioCap_c *cap = as_cap(c.a);
    // keep it a still, untouchable, icon-free model (type 13's Dormant state runs no touch check)
    if (cap->mIconKind != 0xff) { cap->mCapIcon.Unlink(); cap->mIconKind = 0xff; }
    cap->mDormant = 0;
    cap->mType = 13;
    cap->mdCcAc_c.vulnFlags &= ~0x8000u;
    cap->mScaleX = cap->mScaleY = cap->mScaleZ = 0x1000;
    cap->mVertSpeed = 0; cap->mVertAccel = 0; cap->mHorzSpeed = 0;
    daJango_c *k = first_klepto();
    const bool mine_held = k && (unsigned)k->mHeldActorID == c.uid;
    if (r.state == kCapHeld && k) {
        if (!mine_held && k->mHeldActorID == 0 && k->mState != (void *)data_ov062_0211e14c) {
            k->mHeldActorID = (s32)c.uid;
            ++g_st.held_by_klepto;
            if (g_log) std::fprintf(stderr, "[coopcap] lf=%u my Klepto carries the copy of seat %d's cap (Klepto at (%d,%d,%d))\n", g_lf,
                                    seat, k->mPosX >> 12, k->mPosY >> 12, k->mPosZ >> 12);
            return;
        }
        if (mine_held) return;      // Klepto moves it to its claws and gives it its matrix (func_ov062_0211c6a8)
    }
    if (mine_held) {
        k->mHeldActorID = 0;
        if (g_log) std::fprintf(stderr, "[coopcap] lf=%u my Klepto lets go of the copy of seat %d's cap (it is %s there)\n", g_lf, seat,
                                r.state == kCapLoose ? "loose" : "held by something else");
    }
    wr32(c.a, 0xc8, 0);
    wr32(c.a, 0x5c, r.x); wr32(c.a, 0x60, r.y); wr32(c.a, 0x64, r.z);
}

void recv_tick(char *me, int lv, int ac) {
    const unsigned t = now_ms();
    const int mine = comms_hub_my_seat();
    for (int s = 0; s < kSeats; ++s) {
        if (s == mine) continue;
        Rx &r = g_rx[s];
        const bool fresh = r.have && t - r.t_ms < kFreshMs;
        const bool here = fresh && r.level == lv && r.act == ac;
        if (here && r.state != kCapNone) tend_copy(s, r, me);
        else if (g_copy[s].uid) drop_copy(s, !fresh ? "no word from him" : !here ? "he is not in my level" : "he has it back");
    }
}

// ---- proof knobs -----------------------------------------------------------------------------------
char *first_friend_body() {
    const int nb = (int)data_0209f21c < kBodies ? (int)data_0209f21c : kBodies;
    for (int i = 0; i < nb; ++i)
        if (i != (int)data_0209f250 && data_0209f394[i] && port_adventure_ghost_seat(i) >= 0) return (char *)data_0209f394[i];
    return nullptr;
}

void knobs(char *me) {
    static int steal_at = -2, hit_at = -2, goto_at = -2, goto_n = 4; static bool steal_friend = false;
    if (steal_at == -2) {
        steal_at = hit_at = goto_at = -1;
        if (const char *e = std::getenv("SM64DS_COOP_CAP_STEAL")) {
            steal_at = std::atoi(e);
            steal_friend = std::strstr(e, ":friend") != nullptr;
        }
        if (const char *e = std::getenv("SM64DS_COOP_CAP_HIT")) hit_at = std::atoi(e);
        if (const char *e = std::getenv("SM64DS_COOP_CAP_GOTO")) { std::sscanf(e, "%d:%d", &goto_at, &goto_n); }
    }
    if (steal_at >= 0 && g_lf == (unsigned)steal_at) {
        daJango_c *k = first_klepto();
        char *who = steal_friend ? first_friend_body() : me;
        if (!k || !who) {
            std::fprintf(stderr, "[coopcap] KNOB steal lf=%u: no Klepto (%p) or no body (%p)\n", g_lf, (void *)k, (void *)who);
        } else {
            const int lost0 = _ZN8SaveData16HasPlayerLostCapEv() ? 1 : 0;
            k->func_ov062_0211c658(&data_ov062_0211e18c);          // SWOOP, the ROM's own state setter
            k->mdCcAc_c2.otherOwner = uid_of(who);
            k->func_ov062_0211b51c();                              // the ROM's own contact test
            char *held = k->mHeldActorID ? _ZN8dActor_c10FindWithIDEj((unsigned)k->mHeldActorID) : nullptr;
            std::fprintf(stderr, "[coopcap] KNOB steal lf=%u: Klepto at (%d,%d,%d) touched by %s body (char %d); my save lost bit %d -> %d; "
                         "Klepto holds %s (class %d model %d)\n", g_lf, k->mPosX >> 12, k->mPosY >> 12, k->mPosZ >> 12,
                         steal_friend ? "a FRIEND's" : "MY", ((Player *)who)->mCharacter, lost0, _ZN8SaveData16HasPlayerLostCapEv() ? 1 : 0,
                         held ? "a cap" : "nothing", held ? rd32(held, 0x0c) & 0xffff : -1, held ? as_cap(held)->mModelIndex : -1);
        }
    }
    if (hit_at >= 0 && g_lf == (unsigned)hit_at) {
        daJango_c *k = first_klepto();
        if (k) {
            k->mdCcAc_c1.otherOwner = uid_of(me);
            k->mdCcAc_c1.hitFlags = 0x40;                          // a punch
            k->func_ov062_0211b51c();
            std::fprintf(stderr, "[coopcap] KNOB hit lf=%u: Klepto at (%d,%d,%d) punched, now %s, holds %d\n", g_lf, k->mPosX >> 12,
                         k->mPosY >> 12, k->mPosZ >> 12, k->mState == (void *)data_ov062_0211e14c ? "HURT" : "not hurt", k->mHeldActorID);
        }
    }
    if (goto_at >= 0 && g_lf >= (unsigned)goto_at && g_lf < (unsigned)(goto_at + goto_n)) {
        bool held = false;
        char *dst = my_lost_cap(&held);
        const char *what = "my own lost cap";
        if (!dst) for (Copy &c : g_copy) if (c.uid && alive(c.uid, c.a)) { dst = c.a; what = "a COPY of a friend's cap"; break; }
        if (dst) {
            wr32(me, 0x5c, rd32(dst, 0x5c)); wr32(me, 0x60, rd32(dst, 0x60)); wr32(me, 0x64, rd32(dst, 0x64));
            wr32(me, 0x98, 0); wr32(me, 0xa8, 0);
            if (g_lf == (unsigned)goto_at)
                std::fprintf(stderr, "[coopcap] KNOB goto lf=%u: my body (char %d) put on %s at (%d,%d,%d)\n", g_lf, ((Player *)me)->mCharacter,
                             what, rd32(dst, 0x5c) >> 12, rd32(dst, 0x60) >> 12, rd32(dst, 0x64) >> 12);
        } else if (g_lf == (unsigned)goto_at) {
            std::fprintf(stderr, "[coopcap] KNOB goto lf=%u: no cap to stand on\n", g_lf);
        }
    }
    if (const char *sh = std::getenv("SM64DS_COOP_CAP_SHOT")) {
        for (const char *q = sh; *q;) {
            char *end = nullptr;
            const long n = std::strtol(q, &end, 10);
            if (end == q) break;
            if (g_lf == (unsigned)n) {
                char nm[64]; std::snprintf(nm, sizeof nm, "capl_f%ld.bmp", n);
                const void *lfb = ::port_level_fb_ptr;
                const bool w = lfb && ::ntr::ppu_write_bmp(nm, *(const ::ntr::Framebuffer *)lfb);
                std::fprintf(stderr, "[coopcap] picture %s %s\n", nm, w ? "written" : "FAILED");
            }
            q = end; while (*q == ',' || *q == ' ') ++q;
        }
    }
}

void trace(char *me) {
    if (g_trace <= 0 || (g_lf % (unsigned)g_trace) != 0) return;
    bool held = false;
    char *cap = my_lost_cap(&held);
    char line[640]; int k = 0;
    k += std::snprintf(line + k, sizeof line - k, "[coopcap] trace lf=%u me char %d at (%d,%d,%d) nocap %d savebit %d mycap %s", g_lf,
                       ((Player *)me)->mCharacter, rd32(me, 0x5c) >> 12, rd32(me, 0x60) >> 12, rd32(me, 0x64) >> 12,
                       _ZN6Player8HasNoCapEv(me) ? 1 : 0, _ZN8SaveData16HasPlayerLostCapEv() ? 1 : 0,
                       cap ? (held ? "HELD" : "LOOSE") : "none");
    if (cap) k += std::snprintf(line + k, sizeof line - k, "@(%d,%d,%d)", rd32(cap, 0x5c) >> 12, rd32(cap, 0x60) >> 12, rd32(cap, 0x64) >> 12);
    daJango_c *kl = first_klepto();
    if (kl) k += std::snprintf(line + k, sizeof line - k, " | klepto (%d,%d,%d) holds %d%s", kl->mPosX >> 12, kl->mPosY >> 12,
                               kl->mPosZ >> 12, kl->mHeldActorID, kl->mState == (void *)data_ov062_0211e14c ? " HURT" : "");
    const int nb = (int)data_0209f21c < kBodies ? (int)data_0209f21c : kBodies;
    for (int i = 0; i < nb && k < (int)sizeof line - 120; ++i) {
        if (i == (int)data_0209f250 || !data_0209f394[i]) continue;
        const int s = port_adventure_ghost_seat(i);
        if (s < 0 || s >= kSeats) continue;
        const char *b = (const char *)data_0209f394[i];
        k += std::snprintf(line + k, sizeof line - k, " | seat %d drawn nocap %d (said %d) cap %s", s, b[0x701],
                           g_rx[s].have ? (g_rx[s].flags & 1) : -1,
                           !g_rx[s].have ? "?" : g_rx[s].state == kCapNone ? "none" : g_rx[s].state == kCapHeld ? "HELD" : "LOOSE");
        if (g_rx[s].have && g_rx[s].state) k += std::snprintf(line + k, sizeof line - k, "@(%d,%d,%d)", g_rx[s].x >> 12, g_rx[s].y >> 12, g_rx[s].z >> 12);
        const Copy &c = g_copy[s];
        if (c.uid && alive(c.uid, c.a))
            k += std::snprintf(line + k, sizeof line - k, " copy@(%d,%d,%d)%s", rd32(c.a, 0x5c) >> 12, rd32(c.a, 0x60) >> 12,
                               rd32(c.a, 0x64) >> 12, rd32(c.a, 0xc8) ? " in claws" : "");
    }
    std::fprintf(stderr, "%s\n", line);
}

}  // namespace

// The ghost latch's question (comms_sync.cpp): the friend's own HasNoCap, or -1 when I have no fresh word from him in my
// level (then the old latch stands).
int coop_cap_seat_nocap(int seat) {
    read_env();
    if (g_off || seat < 0 || seat >= kSeats || !coop_world_active()) return -1;
    const Rx &r = g_rx[seat];
    int lv = -1, ac = 0;
    if (!r.have || now_ms() - r.t_ms >= kFreshMs || !coop_group_key(&lv, &ac) || r.level != lv || r.act != ac) return -1;
    return r.flags & 1;
}

// Once a frame in an established coop group (hal/coop_actors.cpp's frame, next to the ghost eggs).
void coop_cap_frame(unsigned) {
    read_env();
    if (g_off) return;
    ensure_registered();
    int lv = -1, ac = 0;
    if (!coop_group_key(&lv, &ac)) return;
    char *me = local_body();
    if (!me) return;
    ++g_lf;
    send_tick(me, lv, ac);
    recv_tick(me, lv, ac);
    knobs(me);
    trace(me);
}

}  // namespace port
