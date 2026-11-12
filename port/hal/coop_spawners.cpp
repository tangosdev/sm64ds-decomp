// COOP: THINGS A SPAWNER MAKES, ONE STREAM FOR THE ROOM. Run coopfix1, lane SPAWNERS1 (AUDIT-WORLD H10).
//
// HAZY MAZE CAVE ROLLING ROCKS (daGrock_c, class 221, src/game/actors/d_a_grock.cpp). A kind 0 / kind 1 spawner is an
// invisible placed actor that makes a rolling rock every 0x70 (or 0xe0) frames while the camera player is more than 1500
// units away; kind 1's fifth rock carries a star (the star rock, kind 3, which tracks the star marker and makes the star
// when it is broken by a Mega hit or Wario's punch). The rocks are runtime actors (slot -1), so before this file every
// console rolled its own stream from its own timer: the star rock came at a different moment on every screen, a Wario who
// broke his made a star the others saw pop out of nothing while their own star rock and its marker rolled on, and a second
// break made the star a second time (the coop spawn sync then had to throw the double away).
//
// The rule now. One console makes a spawner's rocks: its OWNER, the lowest seat in the group whose spawner is awake (the
// spawner of a console whose player is in another area of the level sleeps; a seat with no word for 3 s does not count). The
// owner tells the room each rock it made (spawner slot, its id, its facing, star or not, the spawner's rock count); every
// other console's spawner is held (the hook in daGrock_c::Behavior), and each friend makes the same rock the same way
// (port_grock_spawn_copy: the spawner branch's own Spawn with the owner's facing). A rock's path is a pure function of its
// facing (it rolls on fixed ground and nothing pushes it), so the copies roll the same path, a message delay behind.
// A rock broken on one console (the hit handler's two break branches) is announced; the others break their copy with the
// same dust and sound, and a star rock lets its star out there too. That star is linked to the level's placed star marker
// (slot 123 in act 1), the coop spawn sync never announces a linked star, and a take on any console removes the others'
// (its linked-star gone rule), so the room still gets one star. The friend's copy never plays the star camera
// (coop_camshield_mark).
//
// WIRE: SYNW sub-type kWorldSpawners = 0x6e, one datagram per message, three ops (hello, rock, break). 1.0.4 and older
// builds have no handler for 0x6e: coop_world.cpp counts it as drop_unknown_sub and drops that one datagram, nothing else.
// An old build never says hello, so it is never an owner here, and its own spawners roll their own rocks as before.
//
// INERT outside a coop room (the hooks are null until this file's static installer runs, and every hook returns at once
// unless the coop group is established). SM64DS_COOP_SPAWNERS=0 is the control (the old behaviour: every console rolls its
// own). SM64DS_COOP_SPAWNERS_LOG=1 prints [cspawn] lines. Proof knobs (dev builds only, compiled out of the kit):
// SM64DS_COOP_SPAWNERS_SAMPLE=N prints a [rockpos] line every N frames; SM64DS_COOP_SPAWNERS_BREAK=<frames> breaks every
// star rock on this console once it has rolled that many frames, through the same break the Mega hit runs.
//
// PORT_HOST_ABI. Reads any actor's unique id +0x04, class +0x0c, area +0xcc, slot +0xce, position +0x5c and a dEnemyBase_c's
// mStateTimer +0x100 (the spawner's own frame counter: it moves only while the spawner's Behavior runs). The daGrock_c fields
// are read through port_grock_info in the actor file.

#include "coop_world.h"
#include "coop_wire.h"
#include "comms_hub.h"

#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];          // the player bodies
extern unsigned char data_0209f250;    // my player's slot
void *_ZN8dActor_c4NextEPKS_(const void *prev);
void *_ZN8dActor_c10FindWithIDEj(unsigned id);
// src/game/actors/d_a_grock.cpp, host only
extern int (*port_coop_grock_held)(void *spawner);
extern void (*port_coop_grock_spawned)(void *spawner, void *rock, const void *rot);
extern void (*port_coop_grock_broken)(void *rock);
void *port_grock_spawn_copy(void *spawner, const void *rot, int star, int count);
void *port_grock_remote_break(void *rock);
void coop_camshield_mark(void *a, const char *why);      // hal/coop_camshield.cpp (CANNON2): a friend's star never films here
void port_grock_local_break(void *rock);
int port_grock_info(const void *g, int *type, int *count, int *star_out, int *star_idx);
}

namespace port {
namespace {

enum : uint8_t { kWorldSpawners = 0x6e };
enum : uint8_t { kOpHello = 1, kOpRock = 2, kOpBreak = 3 };
enum : int {
    kRockCls = 221,
    kMaxSp = 8, kMaxRocks = 64, kMaxQ = 32, kGoneRing = 32,
    kFreshMs = 3000,          // a seat's hello (or rock) this old still counts
    kHelloMs = 1000,
};

#pragma pack(push, 1)
struct Head { uint8_t op, level, act, ver; };
struct HelloW { Head h; uint8_t nsp, awake; uint16_t reserved; };                  // 8 bytes
struct RockW { Head h; uint16_t slot; uint16_t ctr; int16_t rot[3]; uint8_t star, count; };   // 16 bytes
struct BreakW { Head h; uint16_t slot; uint16_t ctr; uint8_t origin, reserved[3]; };          // 12 bytes
#pragma pack(pop)
static_assert(sizeof(HelloW) == 8 && sizeof(RockW) == 16 && sizeof(BreakW) == 12, "spawner wire sizes");

bool g_env = false, g_on = true, g_log = false;
unsigned g_sample = 0;
int g_break_after = -1;
int g_take_after = -1; bool g_take_armed = false; unsigned g_take_lf = 0; int g_take_pos[3] = {0, 0, 0}; int g_take_hold = 0;
int g_my = -1, g_lv = -1, g_ac = -1;
unsigned g_lf = 0;
bool g_active = false;                         // established coop group this frame

struct Sp { char *a; unsigned uid; short slot; unsigned short tprev; bool awake; int owner; };
Sp g_sp[kMaxSp]; int g_nsp = 0;
struct Peer { bool hello; unsigned hello_ms; uint8_t awake; bool rock; unsigned rock_ms; };
Peer g_peer[kCoopMaxSeats];
struct Rk { bool used; char *a; unsigned uid; short slot; uint8_t origin; uint16_t ctr; bool star; unsigned born_lf; bool broke_knob; };
Rk g_rk[kMaxRocks];
struct Q { bool used; uint8_t op; int seat; RockW r; BreakW b; };
Q g_q[kMaxQ];
struct Gone { short slot; uint8_t origin; uint16_t ctr; bool used; };
Gone g_gone[kGoneRing]; int g_gone_at = 0;
uint16_t g_ctr = 0;
unsigned g_hello_last = 0; bool g_hello_sent = false; uint8_t g_hello_mask = 0;
struct Stats { unsigned made, sent, copies, copy_fail, ignored, breaks_sent, breaks_applied, held, owner_ch, hello_rx; } g_st = {};

long long wall_ms() {
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    return (long long)((((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000ull);
}
int rd32(const void *a, int off) { int v; std::memcpy(&v, (const char *)a + off, 4); return v; }
short rd16(const void *a, int off) { short v; std::memcpy(&v, (const char *)a + off, 2); return v; }
unsigned cls_of(const void *a) { unsigned short c; std::memcpy(&c, (const char *)a + 0x0c, 2); return c; }
unsigned uid_of(const void *a) { return (unsigned)rd32(a, 0x04); }
bool alive(unsigned uid, const void *a) { return uid && a && _ZN8dActor_c10FindWithIDEj(uid) == a; }
bool fresh(bool has, unsigned at, unsigned now) { return has && (int)(now - at) >= 0 && (int)(now - at) <= kFreshMs; }

void read_env() {
    if (g_env) return;
    g_env = true;
    const char *e = std::getenv("SM64DS_COOP_SPAWNERS");
    if (e && *e == '0') g_on = false;
    const char *l = std::getenv("SM64DS_COOP_SPAWNERS_LOG");
    g_log = l && *l && *l != '0';
#ifndef PORT_ROM_CLEAN
    if (const char *s = std::getenv("SM64DS_COOP_SPAWNERS_SAMPLE")) g_sample = (unsigned)std::strtoul(s, nullptr, 10);
    if (const char *b = std::getenv("SM64DS_COOP_SPAWNERS_BREAK")) g_break_after = (int)std::strtol(b, nullptr, 10);
    if (const char *t = std::getenv("SM64DS_COOP_SPAWNERS_TAKE")) g_take_after = (int)std::strtol(t, nullptr, 10);
#endif
    if (g_log) std::fprintf(stderr, "[cspawn] shared spawner rocks %s\n", g_on ? "ON" : "OFF (control)");
}

void reset_level() {
    std::memset(g_sp, 0, sizeof g_sp); g_nsp = 0;
    std::memset(g_peer, 0, sizeof g_peer);
    std::memset(g_rk, 0, sizeof g_rk);
    std::memset(g_q, 0, sizeof g_q);
    std::memset(g_gone, 0, sizeof g_gone); g_gone_at = 0;
    g_hello_sent = false; g_hello_mask = 0; g_lf = 0;
}

int sp_index(const void *a) {
    for (int i = 0; i < g_nsp; ++i) if (g_sp[i].a == a) return i;
    return -1;
}
int sp_by_slot(int slot) {
    for (int i = 0; i < g_nsp; ++i) if (g_sp[i].slot == slot) return i;
    return -1;
}
Head head(uint8_t op) { Head h; h.op = op; h.level = (uint8_t)g_lv; h.act = (uint8_t)g_ac; h.ver = 1; return h; }
bool head_ok(const Head &h) { return h.ver == 1 && h.level == (uint8_t)g_lv && h.act == (uint8_t)g_ac; }

// Every spawner of the level in slot order (the same numbering on every console); a spawner keeps its owner across scans.
void scan() {
    Sp found[kMaxSp]; int nf = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (cls_of(a) != kRockCls || rd16(a, 0xce) < 0) continue;
        int type = -1;
        port_grock_info(a, &type, nullptr, nullptr, nullptr);
        if (type > 1 || nf >= kMaxSp) continue;
        Sp n = {}; n.a = a; n.uid = uid_of(a); n.slot = rd16(a, 0xce); n.owner = -1;
        n.tprev = (unsigned short)rd16(a, 0x100);
        for (int k = 0; k < g_nsp; ++k)
            if (g_sp[k].uid == n.uid && g_sp[k].a == a) { n.owner = g_sp[k].owner; n.awake = n.tprev != g_sp[k].tprev; }
        found[nf++] = n;
    }
    for (int i = 0; i < nf; ++i)
        for (int j = i + 1; j < nf; ++j) if (found[j].slot < found[i].slot) { Sp t = found[i]; found[i] = found[j]; found[j] = t; }
    std::memcpy(g_sp, found, sizeof(Sp) * nf); g_nsp = nf;
}

bool seat_valid(int seat, int i, unsigned now) {
    if (seat == g_my) return g_sp[i].awake;
    if (seat < 0 || seat >= kCoopSeats || !((coop_group_member_mask() >> seat) & 1u)) return false;
    const Peer &p = g_peer[seat];
    if (fresh(p.hello, p.hello_ms, now)) return ((p.awake >> i) & 1u) != 0;
    return fresh(p.rock, p.rock_ms, now);
}

void pick_owners(unsigned now) {
    for (int i = 0; i < g_nsp; ++i) {
        Sp &s = g_sp[i];
        int pick = -1;
        for (int q = 0; q < kCoopSeats && pick < 0; ++q) if (seat_valid(q, i, now)) pick = q;
        if (pick != s.owner) {
            ++g_st.owner_ch;
            if (g_log) std::fprintf(stderr, "[cspawn] spawner slot %d: owner %d -> %d (%s) lf=%u wall=%lld\n", (int)s.slot, s.owner, pick,
                                    pick == g_my ? "mine" : pick < 0 ? "nobody awake" : "a friend's", g_lf, wall_ms());
            s.owner = pick;
        }
    }
}

Rk *rk_find(short slot, uint8_t origin, uint16_t ctr) {
    for (Rk &r : g_rk) if (r.used && r.slot == slot && r.origin == origin && r.ctr == ctr) return &r;
    return nullptr;
}
Rk *rk_by_actor(const void *a) {
    for (Rk &r : g_rk) if (r.used && r.a == a) return &r;
    return nullptr;
}
Rk *rk_alloc() {
    for (Rk &r : g_rk) if (!r.used) return &r;
    for (Rk &r : g_rk) if (!alive(r.uid, r.a)) { r = Rk(); return &r; }
    return nullptr;
}
bool gone_has(short slot, uint8_t origin, uint16_t ctr) {
    for (const Gone &g : g_gone) if (g.used && g.slot == slot && g.origin == origin && g.ctr == ctr) return true;
    return false;
}
void gone_add(short slot, uint8_t origin, uint16_t ctr) {
    Gone &g = g_gone[g_gone_at]; g_gone_at = (g_gone_at + 1) % kGoneRing;
    g.used = true; g.slot = slot; g.origin = origin; g.ctr = ctr;
}

// ---- hooks (called from daGrock_c inside the actor frame) -------------------------------------------------------------

int hook_held(void *spawner) {
    if (!g_on || !g_active) return 0;
    const int i = sp_index(spawner);
    if (i < 0) return 0;
    const int o = g_sp[i].owner;
    if (o >= 0 && o != g_my) { ++g_st.held; return 1; }
    return 0;
}

void hook_spawned(void *spawner, void *rock, const void *rot) {
    if (!g_on || !g_active || !rock) return;
    const int i = sp_index(spawner);
    if (i < 0) return;
    Sp &s = g_sp[i];
    if (s.owner != g_my) {
        ++g_st.owner_ch;
        if (g_log) std::fprintf(stderr, "[cspawn] spawner slot %d: owner %d -> %d (mine made a rock) wall=%lld\n", (int)s.slot, s.owner, g_my, wall_ms());
        s.owner = g_my;
    }
    int type = 0, count = 0;
    port_grock_info(rock, &type, nullptr, nullptr, nullptr);
    port_grock_info(spawner, nullptr, &count, nullptr, nullptr);
    RockW w = {}; w.h = head(kOpRock); w.slot = (uint16_t)s.slot; w.ctr = ++g_ctr; std::memcpy(w.rot, rot, 6);
    w.star = type == 3 ? 1 : 0; w.count = (uint8_t)count;
    if (Rk *r = rk_alloc()) {
        *r = Rk(); r->used = true; r->a = (char *)rock; r->uid = uid_of(rock); r->slot = s.slot; r->origin = (uint8_t)g_my; r->ctr = w.ctr;
        r->star = w.star != 0; r->born_lf = g_lf;
    }
    ++g_st.made;
    if (coop_send_world(kWorldSpawners, &w, sizeof w)) ++g_st.sent;
    if (g_log) std::fprintf(stderr, "[cspawn] my spawner slot %d made rock %d:%u%s facing %d count %d lf=%u wall=%lld\n", (int)s.slot, g_my, (unsigned)w.ctr,
                            w.star ? " (THE STAR ROCK)" : "", (int)w.rot[1], count, g_lf, wall_ms());
}

void hook_broken(void *rock) {
    if (!g_on || !g_active) return;
    Rk *r = rk_by_actor(rock);
    const short pslot = rd16(rock, 0xce);
    if (!r && pslot >= 0) {
        // A PLACED rock (kind 4, the still rock that holds a star, slot 138 in act 1) broken here: the room breaks its copy too and
        // lets the same star out there (origin 0xff = a placed rock, found by its slot).
        BreakW w = {}; w.h = head(kOpBreak); w.slot = (uint16_t)pslot; w.ctr = 0; w.origin = 0xff;
        ++g_st.breaks_sent;
        coop_send_world(kWorldSpawners, &w, sizeof w);
        if (g_log) std::fprintf(stderr, "[cspawn] placed rock slot %d broken here: told the room lf=%u wall=%lld\n", (int)pslot, g_lf, wall_ms());
        if (g_take_after >= 0 && !g_take_armed) { g_take_armed = true; g_take_lf = g_lf; }
        return;
    }
    if (!r || !alive(r->uid, r->a)) return;
    BreakW w = {}; w.h = head(kOpBreak); w.slot = (uint16_t)r->slot; w.ctr = r->ctr; w.origin = r->origin;
    ++g_st.breaks_sent;
    coop_send_world(kWorldSpawners, &w, sizeof w);
    if (g_log) std::fprintf(stderr, "[cspawn] rock %d:%u%s broken here: told the room lf=%u wall=%lld\n", (int)r->origin, (unsigned)r->ctr,
                            r->star ? " (THE STAR ROCK)" : "", g_lf, wall_ms());
    if (r->star && g_take_after >= 0 && !g_take_armed) { g_take_armed = true; g_take_lf = g_lf; std::memcpy(g_take_pos, (char *)rock + 0x5c, 12); }
    gone_add(r->slot, r->origin, r->ctr);
    r->used = false;
}

// ---- wire --------------------------------------------------------------------------------------------------------------

void on_world(int seat, int sub, const unsigned char *p, int len, void *) {
    if (sub != kWorldSpawners || !p || len < (int)sizeof(Head) || seat < 0 || seat >= kCoopSeats) return;
    read_env();
    if (!g_on || !g_active || seat == g_my) return;
    Head h; std::memcpy(&h, p, sizeof h);
    if (!head_ok(h)) return;                               // another level / act, or a newer version
    const unsigned now = comms_hub_now_ms();
    if (h.op == kOpHello && len == (int)sizeof(HelloW)) {
        HelloW w; std::memcpy(&w, p, sizeof w);
        if (w.nsp != (uint8_t)g_nsp) return;               // not the same spawner set: ignore
        Peer &pr = g_peer[seat];
        if (!pr.hello && g_log) std::fprintf(stderr, "[cspawn] seat %d shares its spawners (awake %02x) wall=%lld\n", seat, (unsigned)w.awake, wall_ms());
        pr.hello = true; pr.hello_ms = now; pr.awake = w.awake;
        ++g_st.hello_rx;
        return;
    }
    if ((h.op == kOpRock && len == (int)sizeof(RockW)) || (h.op == kOpBreak && len == (int)sizeof(BreakW))) {
        for (Q &q : g_q) {
            if (q.used) continue;
            q.used = true; q.op = h.op; q.seat = seat;
            if (h.op == kOpRock) std::memcpy(&q.r, p, sizeof q.r); else std::memcpy(&q.b, p, sizeof q.b);
            if (h.op == kOpRock) { g_peer[seat].rock = true; g_peer[seat].rock_ms = now; }
            return;
        }
        ++g_st.ignored;
    }
}

void apply_queue() {
    for (int pass = 0; pass < 2; ++pass)                  // copies first, then breaks (a break can follow its rock in one frame)
        for (Q &q : g_q) {
            if (!q.used || (pass == 0) != (q.op == kOpRock)) continue;
            q.used = false;
            if (q.op == kOpRock) {
                const RockW &w = q.r;
                const int i = sp_by_slot((short)w.slot);
                if (i < 0) { ++g_st.ignored; continue; }
                Sp &s = g_sp[i];
                // Every friend's rock is copied, even one made while two consoles both thought they owned the spawner (the
                // owner is the lowest seat whose spawner is awake, so that only lasts until his hello arrives): the rock already
                // rolls on his screen, so it rolls on mine too.
                if (rk_find(s.slot, (uint8_t)q.seat, w.ctr) || gone_has(s.slot, (uint8_t)q.seat, w.ctr)) continue;
                char *b = (char *)port_grock_spawn_copy(s.a, w.rot, w.star, w.count);
                if (!b) { ++g_st.copy_fail; continue; }
                if (Rk *r = rk_alloc()) {
                    *r = Rk(); r->used = true; r->a = b; r->uid = uid_of(b); r->slot = s.slot; r->origin = (uint8_t)q.seat; r->ctr = w.ctr;
                    r->star = w.star != 0; r->born_lf = g_lf;
                }
                ++g_st.copies;
                if (g_log) std::fprintf(stderr, "[cspawn] seat %d's spawner slot %d made rock %d:%u%s: my copy uid %u facing %d count %d lf=%u wall=%lld\n", q.seat,
                                        (int)s.slot, q.seat, (unsigned)w.ctr, w.star ? " (THE STAR ROCK)" : "", uid_of(b), (int)w.rot[1], (int)w.count, g_lf, wall_ms());
            } else {
                const BreakW &w = q.b;
                if (w.origin == 0xff) {                     // a placed rock (see hook_broken)
                    char *pr = nullptr;
                    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && !pr; a = (char *)_ZN8dActor_c4NextEPKS_(a))
                        if (cls_of(a) == kRockCls && rd16(a, 0xce) == (short)w.slot && !((const unsigned char *)a)[0x0f]) pr = a;
                    int type = -1;
                    if (pr) port_grock_info(pr, &type, nullptr, nullptr, nullptr);
                    if (!pr || type != 4) { if (g_log) std::fprintf(stderr, "[cspawn] seat %d broke placed rock slot %d: none here\n", q.seat, (int)w.slot); continue; }
                    if (g_take_after >= 0 && !g_take_armed) { g_take_armed = true; g_take_lf = g_lf; }
                    void *star = port_grock_remote_break(pr);
                    if (star) coop_camshield_mark(star, "a friend broke the rock that holds a star");
                    ++g_st.breaks_applied;
                    if (g_log) std::fprintf(stderr, "[cspawn] seat %d broke placed rock slot %d: mine breaks too%s lf=%u wall=%lld\n", q.seat, (int)w.slot,
                                            star ? " and lets out its star" : "", g_lf, wall_ms());
                    continue;
                }
                gone_add((short)w.slot, w.origin, w.ctr);
                Rk *r = rk_find((short)w.slot, w.origin, w.ctr);
                if (!r) continue;
                const bool live = alive(r->uid, r->a);
                if (live && r->star && g_take_after >= 0 && !g_take_armed) { g_take_armed = true; g_take_lf = g_lf; std::memcpy(g_take_pos, r->a + 0x5c, 12); }
                void *star = live ? port_grock_remote_break(r->a) : nullptr;
                if (star) coop_camshield_mark(star, "a friend broke the star rock");
                ++g_st.breaks_applied;
                if (g_log) std::fprintf(stderr, "[cspawn] seat %d broke rock %d:%u%s: %s lf=%u wall=%lld\n", q.seat, (int)w.origin, (unsigned)w.ctr,
                                        r->star ? " (THE STAR ROCK)" : "", !live ? "my copy was already gone" : star ? "my copy breaks too and lets out its star (linked to the placed marker)" : "my copy breaks too", g_lf, wall_ms());
                r->used = false;
            }
        }
}

void hello(unsigned now) {
    uint8_t mask = 0;
    for (int i = 0; i < g_nsp && i < 8; ++i) if (g_sp[i].awake) mask |= (uint8_t)(1u << i);
    if (g_hello_sent && mask == g_hello_mask && (int)(now - g_hello_last) < kHelloMs) return;
    HelloW w = {}; w.h = head(kOpHello); w.nsp = (uint8_t)g_nsp; w.awake = mask;
    coop_send_world(kWorldSpawners, &w, sizeof w);
    g_hello_sent = true; g_hello_last = now; g_hello_mask = mask;
}

void sample() {
    char line[4096];
    int n = std::snprintf(line, sizeof line, "[rockpos] w=%lld lf=%u seat=%d", wall_ms(), g_lf, g_my);
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && n < (int)sizeof line - 64; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (cls_of(a) != kRockCls || rd16(a, 0xce) >= 0) continue;
        int type = 0; port_grock_info(a, &type, nullptr, nullptr, nullptr);
        const Rk *r = rk_by_actor(a);
        char id[24];
        if (r) std::snprintf(id, sizeof id, "%d:%u", (int)r->origin, (unsigned)r->ctr); else std::snprintf(id, sizeof id, "-");
        n += std::snprintf(line + n, sizeof line - n, " r%s%s:%d:%d:%d", id, type == 3 ? "*" : "", rd32(a, 0x5c) >> 12, rd32(a, 0x60) >> 12, rd32(a, 0x64) >> 12);
    }
    std::fprintf(stderr, "%s\n", line);
}

// Proof knob SM64DS_COOP_SPAWNERS_BREAK: every star rock on this console breaks once it has rolled that many frames.
void break_knob() {
    if (g_break_after < 0) return;
    static unsigned seen_lf[256]; static unsigned seen_uid[256]; static int nseen = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (cls_of(a) != kRockCls || ((const unsigned char *)a)[0x0f]) continue;
        int type = 0, sidx = 0xff; port_grock_info(a, &type, nullptr, nullptr, &sidx);
        if (!(type == 3 && rd16(a, 0xce) < 0) && !(type == 4 && sidx != 0xff)) continue;   // the star rock, or the still rock that holds a star
        const unsigned uid = uid_of(a);
        int k = 0;
        while (k < nseen && seen_uid[k] != uid) ++k;
        if (k == nseen) { if (nseen >= 256) continue; seen_uid[nseen] = uid; seen_lf[nseen] = g_lf; ++nseen; continue; }
        if (seen_lf[k] == 0xffffffffu || g_lf - seen_lf[k] < (unsigned)g_break_after) continue;
        seen_lf[k] = 0xffffffffu;
        std::fprintf(stderr, "[cspawn] BREAK knob: breaking star rock uid %u here (the Mega hit's break) lf=%u wall=%lld\n", uid, g_lf, wall_ms());
        port_grock_local_break(a);
    }
}

// Proof knob SM64DS_COOP_SPAWNERS_TAKE=<frames>: that many frames after a star rock broke here (my break or a friend's), my
// player is put on the star it let out (the newest runtime star here; it flies from the rock to its marker's spot) for up to 60 frames, so the
// ROM's own collect runs.
void take_knob() {
    if (!g_take_armed || g_take_hold >= 60 || g_lf - g_take_lf < (unsigned)g_take_after) return;
    void *pl = data_0209f394[data_0209f250 & 15];
    if (!pl) return;
    char *best = nullptr; unsigned bu = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (cls_of(a) != 178 || rd16(a, 0xce) >= 0 || ((const unsigned char *)a)[0x0f]) continue;
        if (uid_of(a) > bu) { bu = uid_of(a); best = a; }
    }
    if (!best) { if (g_take_hold++ == 0) std::fprintf(stderr, "[cspawn] TAKE knob: no runtime star here lf=%u wall=%lld\n", g_lf, wall_ms()); return; }
    if (g_take_hold++ == 0)
        std::fprintf(stderr, "[cspawn] TAKE knob: my player goes onto star uid %u at %d,%d,%d lf=%u wall=%lld\n", bu, rd32(best, 0x5c) >> 12, rd32(best, 0x60) >> 12,
                     rd32(best, 0x64) >> 12, g_lf, wall_ms());
    std::memcpy((char *)pl + 0x5c, best + 0x5c, 12);
}

void on_frame(unsigned now) {
    read_env();
    int lv = -1, ac = -1;
    g_active = false;
    if (!coop_group_established() || !coop_group_key(&lv, &ac)) return;
    if (lv != g_lv || ac != g_ac) { reset_level(); g_lv = lv; g_ac = ac; }
    ++g_lf;
    g_my = comms_hub_my_seat();
    if (g_my < 0 || g_my >= kCoopSeats) return;
    scan();
    if (g_sample && (g_lf % g_sample) == 0) sample();
    if (!g_on || g_nsp == 0) { break_knob(); return; }
    g_active = true;
    break_knob();
    take_knob();
    for (Rk &r : g_rk) if (r.used && !alive(r.uid, r.a)) r.used = false;
    apply_queue();
    pick_owners(now);
    hello(now);
    static unsigned last = 0; static bool have_last = false;
    if (g_log && (!have_last || (int)(now - last) >= 10000)) {
        have_last = true; last = now;
        int owners[kMaxSp]; for (int i = 0; i < kMaxSp; ++i) owners[i] = i < g_nsp ? g_sp[i].owner : -9;
        std::fprintf(stderr, "[cspawn] stats spawners=%d owners=%d,%d,%d,%d made=%u sent=%u copies=%u copy_fail=%u ignored=%u breaks_sent=%u breaks_applied=%u held=%u owner_ch=%u hello_rx=%u\n",
                     g_nsp, owners[0], owners[1], owners[2], owners[3], g_st.made, g_st.sent, g_st.copies, g_st.copy_fail, g_st.ignored, g_st.breaks_sent,
                     g_st.breaks_applied, g_st.held, g_st.owner_ch, g_st.hello_rx);
    }
}

struct Install {
    Install() {
        port_coop_grock_held = hook_held;
        port_coop_grock_spawned = hook_spawned;
        port_coop_grock_broken = hook_broken;
        const bool f = coop_on_frame(on_frame);
        const bool w = coop_world_register(kWorldSpawners, kWorldSpawners, on_world);
        if (!f || !w) std::fprintf(stderr, "[cspawn] NOT registered (frame %d, sub 0x6e %d): shared spawner rocks off\n", (int)f, (int)w);
    }
} g_install;

}  // namespace
}  // namespace port
