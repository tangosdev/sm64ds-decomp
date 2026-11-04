// A COOP FRIEND'S YOSHI EGGS ON MY CONSOLE. Run after093, lane FRIENDVIS1. See coop_ghost_eggs.h.
//
// PORT_HOST_ABI. Reads live actors by raw offset (the port/hal convention): a Player's egg queue pointer +0x588 (five
// egg pointers), its egg payout byte +0x704; an egg's followed Player +0x38c, state +0x3f0, burst flag +0x426; any
// actor's id +0x04, class +0x0c, position +0x5c, yaw +0x8e, flags +0xb0. Writes only the eggs this module laid and,
// for the length of one egg Behavior or one lay call, a ghost body's position (put back at once).

#include "coop_ghost_eggs.h"
#include "coop_world.h"
#include "coop_wire.h"
#include "comms_hub.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern unsigned char data_0209f21c;
void *_ZN8dActor_c4NextEPKS_(const void *prev);
void *_ZN8dActor_c10FindWithIDEj(unsigned id);
void _ZN7fBase_c18MarkForDestructionEv(void *a);
void func_ov002_020d6368(char *player);          // Player: lay one egg (class 9) behind me into my queue
int port_adventure_ghost_seat(int slot);
int port_adventure_ghost_drawpos(int slot, int out[3]);
int _ZN6Player7TryGrabER8dActor_c(void *self, void *actor);
void LoadLevelNoReturn(unsigned level, unsigned entrance, unsigned star, unsigned unk);
}

namespace port {
namespace {

enum : int { kSeats = 16, kBodies = 16, kQueue = 5, kFl = 2, kBurstList = 16, kStaleFrames = 180, kFlStale = 45,
             kUndrawnDrop = 15, kPartyThrowWait = 12 };
enum : uint8_t { kFHand = 1, kFFly = 2, kFBurst = 4 };

int rd32(const void *a, int off) { int v; std::memcpy(&v, (const char *)a + off, 4); return v; }
void wr32(void *a, int off, int v) { std::memcpy((char *)a + off, &v, 4); }
unsigned uid_of(const void *a) { return (unsigned)rd32(a, 0x04); }
unsigned cls_of(const void *a) { unsigned short c; std::memcpy(&c, (const char *)a + 0x0c, 2); return c; }
void *player_of(const void *egg) { void *p; std::memcpy(&p, (const char *)egg + 0x38c, sizeof p); return p; }
int *queue_of(void *pl) { int *q; std::memcpy(&q, (char *)pl + 0x588, sizeof q); return q; }
bool alive(unsigned uid, const void *a) { return uid && a && _ZN8dActor_c10FindWithIDEj(uid) == a; }

bool g_env = false, g_off = false, g_log = false, g_regd = false;
unsigned g_lf = 0;
// party rooms: the level party_hits gives me (act 0) and its record sender; coop rooms use the coop group's key
bool g_party = false; int g_party_level = -1;
void (*g_party_send)(const GhostEggRec &, int, int, int) = nullptr;
bool key_now(int *lv, int *ac) {
    if (g_party) { *lv = g_party_level; *ac = 0; return g_party_level >= 0; }
    return coop_group_key(lv, ac);
}

struct Stats { unsigned sent, recv, bad, laid, lay_fail, thrown_shown, bursts, quiet, flights_seen, other_level; } g_st = {};

// ---- my side: what I send ----
struct TxFl { bool used; unsigned uid; uint8_t id, flags; int x, y, z; short yaw; int burst_left; int px, py, pz; bool flew, start_sent; };
TxFl g_tx[kFl];
int g_tx_eggs = -1; unsigned g_tx_last = 0; uint8_t g_seq = 0;

// ---- a friend's side: what I draw ----
struct RxFl { bool used; uint8_t id, flags; int x, y, z, px, py, pz; unsigned t, tp; short yaw; unsigned egg_uid; void *egg; bool burst; bool party; };
struct RxSeat {
    bool have; int level, act, eggs; unsigned rx_lf;
    RxFl fl[kFl];
    unsigned trail_uid[kQueue]; void *trail[kQueue]; int ntrail;
    int undrawn;
    unsigned drop_lf;      // party: when his count last fell (the throw's start comes a frame or two after it)
};
RxSeat g_rx[kSeats];
struct Pend { unsigned uid; void *egg; };
Pend g_burst[kBurstList];          // flights that ended: the next Behavior bursts them (mode 3)

void read_env() {
    if (g_env) return;
    g_env = true;
    const char *e = std::getenv("SM64DS_GHOST_EGGS");
    g_off = e && e[0] == '0' && !e[1];
    const char *l = std::getenv("SM64DS_GHOST_EGGS_LOG");
    g_log = l && l[0] && l[0] != '0';
}

void exit_line() {
    std::fprintf(stderr, "[geggs:exit] sent %u, received %u (bad %u, other level %u), laid %u (failed %u), thrown shown %u (flights seen %u), "
                 "bursts %u, quiet removals %u%s\n", g_st.sent, g_st.recv, g_st.bad, g_st.other_level, g_st.laid, g_st.lay_fail,
                 g_st.thrown_shown, g_st.flights_seen, g_st.bursts, g_st.quiet, g_off ? " (CONTROL SM64DS_GHOST_EGGS=0)" : "");
}

void on_world(int seat, int sub, const unsigned char *p, int len, void *);
void ensure_registered() {
    if (g_regd) return;
    g_regd = true;
    const bool ok = coop_world_register(kWorldGhostEggs, kWorldGhostEggs, on_world);
    std::atexit(exit_line);
    if (g_log || !ok) std::fprintf(stderr, "[geggs] SYNW sub-type 0x%02x %s\n", kWorldGhostEggs, ok ? "registered" : "NOT registered (range taken)");
}

void push_burst(void *egg, unsigned uid) {
    for (Pend &b : g_burst) if (!b.uid) { b.uid = uid; b.egg = egg; return; }
    _ZN7fBase_c18MarkForDestructionEv(egg); ++g_st.quiet;     // list full: it just goes
}

// ---- sending ---------------------------------------------------------------------------------------
void send_tick(char *me) {
    int lv = -1, ac = 0;
    if (!key_now(&lv, &ac)) return;
    int *q = queue_of(me);
    int eggs = 0;
    if (q) for (int i = 0; i < kQueue; ++i) if (q[i]) ++eggs;
    if (g_log && q && q[0] && (g_lf % 30) == 0) {
        const void *e = (const void *)(intptr_t)q[0];
        std::fprintf(stderr, "[geggs] my trail lf=%u egg %u at (%d,%d,%d), me at (%d,%d,%d)\n", g_lf, uid_of(e) & 0xff,
                     rd32(e, 0x5c) >> 12, rd32(e, 0x60) >> 12, rd32(e, 0x64) >> 12, rd32(me, 0x5c) >> 12, rd32(me, 0x60) >> 12, rd32(me, 0x64) >> 12);
    }
    // my eggs out of the queue (the one in my mouth for the throw, the flying ones): class 9, following me, state 1
    for (TxFl &f : g_tx) {
        if (!f.used || f.burst_left) continue;
        void *a = _ZN8dActor_c10FindWithIDEj(f.uid);
        if (!a || cls_of(a) != 9 || ((const unsigned char *)a)[0x426]) { f.burst_left = 3; f.flags = kFBurst; }
    }
    for (void *a = _ZN8dActor_c4NextEPKS_(0); a; a = _ZN8dActor_c4NextEPKS_(a)) {
        if (cls_of(a) != 9 || player_of(a) != me || rd32(a, 0x3f0) != 1 || ((const unsigned char *)a)[0x426]) continue;
        bool queued = false;
        if (q) for (int i = 0; i < kQueue; ++i) if (q[i] == (int)(intptr_t)a) queued = true;
        if (queued) continue;
        const unsigned u = uid_of(a);
        bool known = false;
        for (TxFl &f : g_tx) if (f.used && f.uid == u) known = true;
        if (known) continue;
        for (TxFl &f : g_tx) if (!f.used) { f = TxFl(); f.used = true; f.uid = u; f.id = (uint8_t)u; break; }
    }
    int nrec = 0, nlive = 0;
    unsigned char buf[sizeof(GhostEggHead) + kFl * sizeof(GhostEggRec)];
    for (TxFl &f : g_tx) {
        if (!f.used) continue;
        if (!f.burst_left) {
            void *a = _ZN8dActor_c10FindWithIDEj(f.uid);
            const unsigned fl = (unsigned)rd32(a, 0xb0);
            f.flags = (uint8_t)(((fl & 0x100) ? kFHand : 0) | ((fl & 0x400) ? kFFly : 0));
            const bool flying_before = f.flew;
            f.px = f.x; f.py = f.y; f.pz = f.z;
            f.x = rd32(a, 0x5c) >> 12; f.y = rd32(a, 0x60) >> 12; f.z = rd32(a, 0x64) >> 12;
            f.flew = (f.flags & kFFly) != 0;
            if (f.flew && !flying_before) f.start_sent = false;   // its first flying frame: the speed is known from the next
            if (!flying_before) f.px = f.x, f.py = f.y, f.pz = f.z;
            std::memcpy(&f.yaw, (const char *)a + 0x8e, 2);
            ++nlive;
        }
        GhostEggRec r;
        r.id = f.id; r.flags = f.flags;
        r.x = (int16_t)f.x; r.y = (int16_t)f.y; r.z = (int16_t)f.z; r.yaw = f.yaw;
        std::memcpy(buf + sizeof(GhostEggHead) + nrec * sizeof r, &r, sizeof r);
        ++nrec;
    }
    const bool changed = eggs != g_tx_eggs;
    const unsigned beat = eggs ? 30u : 90u;
    if (g_party) {
        // The count rides party_hits' kHeldState. SYNH is capped at 10 a second per sender (relay), shared with the hit
        // claims, so a throw costs TWO messages a friend: its start once it has flown a frame (position and speed per
        // frame; the friend's console flies it on from there) and its burst once.
        for (TxFl &f : g_tx) {
            if (!f.used || !g_party_send) continue;
            GhostEggRec r; r.id = f.id; r.flags = f.flags; r.x = (int16_t)f.x; r.y = (int16_t)f.y; r.z = (int16_t)f.z; r.yaw = f.yaw;
            if (f.burst_left) {
                if (f.start_sent) { g_party_send(r, 0, 0, 0); ++g_st.sent; }
                if (g_log) std::fprintf(stderr, "[geggs] party out lf=%u: egg %d burst%s\n", g_lf, f.id, f.start_sent ? "" : " (its start never went: nothing sent)");
                f = TxFl();
            } else if (f.flew && !f.start_sent && (f.x != f.px || f.y != f.py || f.z != f.pz)) {
                const int vx = f.x - f.px, vy = f.y - f.py, vz = f.z - f.pz;
                g_party_send(r, vx, vy, vz); ++g_st.sent; f.start_sent = true;
                if (g_log) std::fprintf(stderr, "[geggs] party out lf=%u: egg %d thrown at (%d,%d,%d) speed (%d,%d,%d)\n", g_lf, f.id, f.x, f.y, f.z, vx, vy, vz);
            }
        }
        (void)nrec; (void)nlive; (void)changed; (void)beat;
        g_tx_eggs = eggs;
        return;
    }
    const bool due = changed || (nrec && (g_lf & 1) == 0) || (g_lf - g_tx_last >= beat);
    if (!due) return;
    GhostEggHead h;
    h.key.level = (uint16_t)lv; h.key.act = (uint8_t)ac; h.key.reserved = 0;
    h.ver = 1; h.eggs = (uint8_t)eggs; h.n = (uint8_t)nrec; h.seq = ++g_seq;
    std::memcpy(buf, &h, sizeof h);
    if (coop_send_world(kWorldGhostEggs, buf, (int)(sizeof h + nrec * sizeof(GhostEggRec)))) ++g_st.sent;
    if (g_log && (changed || nrec))
        std::fprintf(stderr, "[geggs] out lf=%u: eggs %d, records %d (live %d)%s\n", g_lf, eggs, nrec, nlive, changed ? " (changed)" : "");
    g_tx_eggs = eggs; g_tx_last = g_lf;
    for (TxFl &f : g_tx) if (f.used && f.burst_left && --f.burst_left == 0) f = TxFl();
}

// ---- receiving -------------------------------------------------------------------------------------
void apply_rec(RxSeat &s, const GhostEggRec &r) {
    RxFl *f = nullptr;
    for (RxFl &e : s.fl) if (e.used && e.id == r.id) f = &e;
    if (!f) {
        if (r.flags & kFBurst) return;                  // the end of a flight I never saw begin
        for (RxFl &e : s.fl) if (!e.used) { e = RxFl(); e.used = true; e.id = r.id; f = &e; ++g_st.flights_seen; break; }
        if (!f) return;
        f->x = f->px = r.x * 0x1000; f->y = f->py = r.y * 0x1000; f->z = f->pz = r.z * 0x1000; f->t = f->tp = g_lf;
    }
    if (f->burst) return;
    f->px = f->x; f->py = f->y; f->pz = f->z; f->tp = f->t;
    f->x = r.x * 0x1000; f->y = r.y * 0x1000; f->z = r.z * 0x1000; f->t = g_lf; f->yaw = r.yaw; f->flags = r.flags;
    if (r.flags & kFBurst) f->burst = true;
}

void on_world(int seat, int sub, const unsigned char *p, int len, void *) {
    if (sub != kWorldGhostEggs || g_off) return;
    if (!ghost_eggs_validate(p, len) || seat < 0 || seat >= kSeats || seat == comms_hub_my_seat()) { ++g_st.bad; return; }
    GhostEggHead h; std::memcpy(&h, p, sizeof h);
    RxSeat &s = g_rx[seat];
    ++g_st.recv;
    if (g_log && (!s.have || s.eggs != h.eggs || h.n))
        std::fprintf(stderr, "[geggs] in lf=%u from seat %d: eggs %d, records %d (level %d act %d)\n", g_lf, seat, h.eggs, h.n, h.key.level, h.key.act);
    s.have = true; s.level = h.key.level; s.act = h.key.act; s.eggs = h.eggs; s.rx_lf = g_lf;
    for (int k = 0; k < h.n; ++k) {
        GhostEggRec r; std::memcpy(&r, p + sizeof h + k * sizeof r, sizeof r);
        apply_rec(s, r);
    }
}

int body_of_seat(int seat) {
    const int nb = data_0209f21c < kBodies ? data_0209f21c : kBodies;
    for (int i = 0; i < nb; ++i)
        if (i != (int)data_0209f250 && data_0209f394[i] && port_adventure_ghost_seat(i) == seat) return i;
    return -1;
}

void drop_trail(RxSeat &s, bool quiet_all) {
    for (int i = 0; i < s.ntrail; ++i)
        if (alive(s.trail_uid[i], s.trail[i])) { _ZN7fBase_c18MarkForDestructionEv(s.trail[i]); ++g_st.quiet; }
    s.ntrail = 0;
    if (quiet_all)
        for (RxFl &f : s.fl) {
            if (f.used && alive(f.egg_uid, f.egg)) { _ZN7fBase_c18MarkForDestructionEv(f.egg); ++g_st.quiet; }
            f = RxFl();
        }
}

// One egg laid on the ghost by the ROM's own call, with the ghost at its drawn spot and a payout byte of 1 (param1 0x10:
// the round egg; 0 would make the other, box-shaped variant). Taken back out of
// the ghost's queue at once: this module keeps the list, so nothing else ever finds my copies through the ghost.
void *lay(char *ghost, const int dp[3]) {
    int *q = queue_of(ghost);
    if (!q) { ++g_st.lay_fail; return nullptr; }
    int before[kQueue]; std::memcpy(before, q, sizeof before);
    int k = -1;
    for (int i = 0; i < kQueue; ++i) if (!q[i]) { k = i; break; }
    if (k < 0) { ++g_st.lay_fail; return nullptr; }
    int sv[3] = { rd32(ghost, 0x5c), rd32(ghost, 0x60), rd32(ghost, 0x64) };
    const unsigned char pay = ((unsigned char *)ghost)[0x704];
    wr32(ghost, 0x5c, dp[0]); wr32(ghost, 0x60, dp[1]); wr32(ghost, 0x64, dp[2]);
    ((unsigned char *)ghost)[0x704] = 1;      // egg param 1 << 4: the round egg (variant bits 1), no coins, no blue coin, no star
    func_ov002_020d6368(ghost);
    wr32(ghost, 0x5c, sv[0]); wr32(ghost, 0x60, sv[1]); wr32(ghost, 0x64, sv[2]);
    ((unsigned char *)ghost)[0x704] = pay;
    void *egg = (void *)(intptr_t)q[k];
    if (q[k] == before[k] || !egg) { ++g_st.lay_fail; return nullptr; }
    q[k] = 0;
    ++g_st.laid;
    return egg;
}

void bind_flight(RxFl &f, void *egg) {
    f.egg = egg; f.egg_uid = uid_of(egg);
    wr32(egg, 0xb0, (int)((unsigned)rd32(egg, 0xb0) & ~0x100u));
    wr32(egg, 0x5c, f.x); wr32(egg, 0x60, f.y); wr32(egg, 0x64, f.z);
    ++g_st.thrown_shown;
}

void recv_tick() {
    int lv = -1, ac = 0;
    if (!key_now(&lv, &ac)) return;
    for (Pend &b : g_burst) if (b.uid && !alive(b.uid, b.egg)) b = Pend();     // gone before its burst ran (a level change)
    for (int seat = 0; seat < kSeats; ++seat) {
        RxSeat &s = g_rx[seat];
        if (!s.have) continue;
        if (g_lf - s.rx_lf > (unsigned)kStaleFrames) { drop_trail(s, true); s = RxSeat(); continue; }
        const int b = body_of_seat(seat);
        int dp[3];
        const bool drawn = b >= 0 && port_adventure_ghost_drawpos(b, dp);
        if (!drawn || s.level != lv || s.act != ac) {
            if (s.level != lv || s.act != ac) ++g_st.other_level;
            if (++s.undrawn > kUndrawnDrop) drop_trail(s, true);
            continue;
        }
        s.undrawn = 0;
        char *ghost = (char *)data_0209f394[b];
        int n = 0;                                          // the trail, minus copies that are gone
        for (int i = 0; i < s.ntrail; ++i)
            if (alive(s.trail_uid[i], s.trail[i]) && player_of(s.trail[i]) == ghost) { s.trail[n] = s.trail[i]; s.trail_uid[n] = s.trail_uid[i]; ++n; }
        s.ntrail = n;
        // flights: a new one takes the FRONT egg of the trail (the ROM throws queue[0] and shifts the rest)
        for (RxFl &f : s.fl) {
            if (!f.used) continue;
            if (f.egg && !alive(f.egg_uid, f.egg)) { f = RxFl(); continue; }
            const bool over = f.burst || g_lf - f.t > (unsigned)kFlStale;
            if (!f.egg && !over) {
                void *egg = nullptr;
                if (s.ntrail > 0 && s.ntrail > s.eggs) {
                    egg = s.trail[0];
                    for (int i = 1; i < s.ntrail; ++i) { s.trail[i - 1] = s.trail[i]; s.trail_uid[i - 1] = s.trail_uid[i]; }
                    --s.ntrail;
                } else {
                    egg = lay(ghost, dp);
                }
                if (egg) {
                    bind_flight(f, egg);
                    if (g_log) std::fprintf(stderr, "[geggs] lf=%u seat %d's egg %d is thrown: mine flies on his path from (%d,%d,%d)\n",
                                            g_lf, seat, f.id, f.x >> 12, f.y >> 12, f.z >> 12);
                }
            }
            if (over) {
                if (f.egg) { push_burst(f.egg, f.egg_uid); ++g_st.bursts; }
                if (g_log) std::fprintf(stderr, "[geggs] lf=%u seat %d's egg %d %s\n", g_lf, seat, f.id, f.burst ? "burst" : "went quiet: burst");
                f = RxFl();
            }
        }
        // the trail: as many as his queue holds
        // party rooms: his count falls at once (kHeldState) and the throw's start comes once the egg has flown a frame, so a
        // surplus egg waits kPartyThrowWait frames for it (the flight takes it) before it counts as gone without one
        while (s.ntrail > s.eggs && !(g_party && g_lf - s.drop_lf < (unsigned)kPartyThrowWait)) {   // one went without a flight record (a hurt, a lost message): it bursts
            push_burst(s.trail[0], s.trail_uid[0]); ++g_st.bursts;
            for (int i = 1; i < s.ntrail; ++i) { s.trail[i - 1] = s.trail[i]; s.trail_uid[i - 1] = s.trail_uid[i]; }
            --s.ntrail;
        }
        while (s.ntrail < s.eggs && s.ntrail < kQueue) {
            void *egg = lay(ghost, dp);
            if (!egg) break;
            s.trail[s.ntrail] = egg; s.trail_uid[s.ntrail] = uid_of(egg); ++s.ntrail;
            if (g_log) std::fprintf(stderr, "[geggs] lf=%u laid an egg on seat %d's body %d (now %d of %d) at (%d,%d,%d)\n",
                                    g_lf, seat, b, s.ntrail, s.eggs, dp[0] >> 12, dp[1] >> 12, dp[2] >> 12);
        }
    }
}

}  // namespace

bool ghost_eggs_validate(const void *payload, int len) {
    if (!payload || len < (int)sizeof(GhostEggHead)) return false;
    GhostEggHead h; std::memcpy(&h, payload, sizeof h);
    if (!coop_key_ok(h.key) || h.ver != 1 || h.eggs > kQueue || h.n > kFl) return false;
    if (len != (int)(sizeof h + h.n * sizeof(GhostEggRec))) return false;
    for (int k = 0; k < h.n; ++k) {
        GhostEggRec r; std::memcpy(&r, (const unsigned char *)payload + sizeof h + k * sizeof r, sizeof r);
        if (r.flags > 7) return false;
    }
    return true;
}

// LAB (row knob, off by default) SM64DS_GHOST_LAB_GRAB=<class>[,<units>]: the ROM's own Player::TryGrab on the nearest actor of
// that class within <units> (default 60) while I hold nothing. TryGrab itself refuses unless my body is in a real punch the
// pad pressed (or a swim / crouch) and the actor carries the grab flag 0x80, so this only stands in for the attack cylinder
// touching it (the scripted walk keeps kicking a Bob-omb instead of meeting it with a standing punch).
void lab_grab(char *me) {
    static int cls = -2, units = 60;
    static unsigned logs = 0;
    if (cls == -2) { cls = -1; if (const char *e = std::getenv("SM64DS_GHOST_LAB_GRAB")) std::sscanf(e, "%d,%d", &cls, &units); }
    if (cls < 0 || rd32(me, 0x358)) return;
    void *best = nullptr; long long bd = (long long)units * units;
    const int mx = rd32(me, 0x5c) >> 12, my = rd32(me, 0x60) >> 12, mz = rd32(me, 0x64) >> 12;
    for (void *a = _ZN8dActor_c4NextEPKS_(0); a; a = _ZN8dActor_c4NextEPKS_(a)) {
        if ((int)cls_of(a) != cls) continue;
        // a Bob-omb that blew up waits dormant at home, undrawn (daBmb_c mState 5, mShouldRender 0): never that one
        if (cls == 206 && (rd32(a, 0x3dc) == 5 || !((const unsigned char *)a)[0x3f3])) continue;
        const long long dx = (rd32(a, 0x5c) >> 12) - mx, dy = (rd32(a, 0x60) >> 12) - my, dz = (rd32(a, 0x64) >> 12) - mz;
        const long long d = dx * dx + dz * dz;
        if (d <= bd && dy > -80 && dy < 120) { bd = d; best = a; }
    }
    if (!best) return;
    // units >= 1000 is the FETCH form: the scripted walk keeps missing the Bob-omb, so set it down at my feet first
    if (units >= 1000) { wr32(best, 0x5c, rd32(me, 0x5c)); wr32(best, 0x60, rd32(me, 0x60)); wr32(best, 0x64, rd32(me, 0x64)); }
    const int ok = _ZN6Player7TryGrabER8dActor_c(me, best);
    if (ok || (++logs % 30) == 1) {
        short slot; std::memcpy(&slot, (const char *)best + 0xce, 2);
        std::fprintf(stderr, "[geggs] LAB grab lf=%u: class %d slot %d at %d units, its flags 0x%x, TryGrab %s\n", g_lf, cls, slot,
                     (int)std::sqrt((double)bd), (unsigned)rd32(best, 0xb0), ok ? "TOOK IT" : "refused");
    }
}

// LAB (row knob, off by default) SM64DS_GHOST_LAB_FACE=<units>[,<from frame>]: the picture rows' watcher. From that frame of
// this module on, I stand <units> in front of the nearest drawn friend, facing him (set down again whenever he moved more
// than 120 units off that spot), so the camera behind me has him and what he carries or throws in view.
void lab_face(char *me) {
    static int units = -2, from = 0;
    static unsigned logs = 0;
    if (units == -2) { units = -1; if (const char *e = std::getenv("SM64DS_GHOST_LAB_FACE")) std::sscanf(e, "%d,%d", &units, &from); }
    if (units < 0 || g_lf < (unsigned)from) return;
    const int nb = data_0209f21c < kBodies ? data_0209f21c : kBodies;
    int best = -1, bdp[3] = {0, 0, 0}; long long bd = -1;
    for (int i = 0; i < nb; ++i) {
        if (i == (int)data_0209f250 || !data_0209f394[i] || port_adventure_ghost_seat(i) < 0) continue;
        int dp[3];
        if (!port_adventure_ghost_drawpos(i, dp)) continue;
        const long long dx = (long long)(dp[0] - rd32(me, 0x5c)) >> 12, dz = (long long)(dp[2] - rd32(me, 0x64)) >> 12;
        if (bd < 0 || dx * dx + dz * dz < bd) { bd = dx * dx + dz * dz; best = i; std::memcpy(bdp, dp, sizeof dp); }
    }
    if (best < 0) return;
    unsigned short gy; std::memcpy(&gy, (const char *)data_0209f394[best] + 0x8e, 2);
    const double a = (double)gy * 3.14159265358979323846 / 32768.0;
    const int sx = bdp[0] + (int)std::lround(std::sin(a) * units) * 0x1000, sz = bdp[2] + (int)std::lround(std::cos(a) * units) * 0x1000;
    const long long ox = (long long)(sx - rd32(me, 0x5c)) >> 12, oz = (long long)(sz - rd32(me, 0x64)) >> 12;
    if (ox * ox + oz * oz > 120LL * 120LL) {
        wr32(me, 0x5c, sx); wr32(me, 0x60, bdp[1]); wr32(me, 0x64, sz);
        wr32(me, 0x98, 0); wr32(me, 0xa8, 0);
        if ((++logs % 20) == 1)
            std::fprintf(stderr, "[geggs] LAB face lf=%u: set down %d units in front of body %d at (%d,%d,%d)\n", g_lf, units, best,
                         sx >> 12, bdp[1] >> 12, sz >> 12);
    }
    const unsigned short face = (unsigned short)(gy + 0x8000);
    std::memcpy(me + 0x8e, &face, 2);
}

void ghost_eggs_frame(unsigned) {
    read_env();
    if (g_off) return;
    ensure_registered();
    ++g_lf;
    const int me = (int)data_0209f250;
    char *mb = (me >= 0 && me < kBodies) ? (char *)data_0209f394[me] : nullptr;
    if (mb) { send_tick(mb); lab_grab(mb); lab_face(mb); }
    recv_tick();
}

// LAB (row knob, off by default) SM64DS_GHOST_LAB_WARP=<frame>:<level>:<act>: a party room has no coop warp knob, so the
// picture rows reach a course this way: the game's own level change, once, that many party frames in.
void lab_warp() {
    static int at = -2, lv = 0, ac = 1;
    if (at == -2) { at = -1; if (const char *e = std::getenv("SM64DS_GHOST_LAB_WARP")) std::sscanf(e, "%d:%d:%d", &at, &lv, &ac); }
    if (at < 0 || g_lf != (unsigned)at) return;
    std::fprintf(stderr, "[geggs] LAB warp lf=%u to level %d act %d\n", g_lf, lv, ac);
    LoadLevelNoReturn((unsigned)lv, 0, (unsigned)ac, 1);
}

void ghost_eggs_party_frame(char *me, int level, void (*send)(const GhostEggRec &rec, int vx, int vy, int vz)) {
    read_env();
    if (g_off) return;
    if (!g_party) { g_party = true; std::atexit(exit_line); if (g_log) std::fprintf(stderr, "[geggs] party room: eggs ride SYNH (kHeldState count, kEggFlight records)\n"); }
    g_party_level = level; g_party_send = send;
    ++g_lf;
    if (me) { send_tick(me); lab_grab(me); lab_face(me); }
    recv_tick();
    lab_warp();
}

void ghost_eggs_party_in(int seat, int eggs) {
    if (!g_party || g_off || seat < 0 || seat >= kSeats || seat == comms_hub_my_seat() || eggs < 0 || eggs > kQueue) { ++g_st.bad; return; }
    RxSeat &s = g_rx[seat];
    ++g_st.recv;
    if (g_log && (!s.have || s.eggs != eggs)) std::fprintf(stderr, "[geggs] party in lf=%u from seat %d: eggs %d\n", g_lf, seat, eggs);
    if (s.have && eggs < s.eggs) s.drop_lf = g_lf;
    s.have = true; s.level = g_party_level; s.act = 0; s.eggs = eggs; s.rx_lf = g_lf;
}

void ghost_eggs_party_flight(int seat, const GhostEggRec &r, int vx, int vy, int vz) {
    if (!g_party || g_off || seat < 0 || seat >= kSeats || seat == comms_hub_my_seat() || r.flags > 7) { ++g_st.bad; return; }
    RxSeat &s = g_rx[seat];
    ++g_st.recv;
    if (!s.have) { s.have = true; s.eggs = 0; }
    s.level = g_party_level; s.act = 0; s.rx_lf = g_lf;
    if (g_log) std::fprintf(stderr, "[geggs] party flight in lf=%u from seat %d: egg %d flags %d at (%d,%d,%d)\n", g_lf, seat, r.id, r.flags, r.x, r.y, r.z);
    apply_rec(s, r);
    for (RxFl &f : s.fl)
        if (f.used && f.id == r.id && !f.burst) {     // its start: the speed it left with, per frame
            f.party = true;
            f.px = f.x - vx * 0x1000; f.py = f.y - vy * 0x1000; f.pz = f.z - vz * 0x1000; f.tp = f.t - 1;
        }
}

void ghost_eggs_member_gone(int seat) {
    if (seat < 0 || seat >= kSeats) return;
    drop_trail(g_rx[seat], true);
    g_rx[seat] = RxSeat();
}

}  // namespace port

using namespace port;

extern "C" int port_ghost_egg_begin(void *egg, int saved[4]) {
    if (!egg || g_off) return 0;
    const unsigned u = uid_of(egg);
    for (Pend &b : g_burst)
        if (b.uid == u && b.egg == egg) { b = Pend(); return 3; }
    for (RxSeat &s : g_rx) {
        if (!s.have) continue;
        for (RxFl &f : s.fl) {
            if (!f.used || f.egg != egg || f.egg_uid != u) continue;
            // on his path: the newest sent spot, carried on by its own speed for up to 6 frames
            const int cap = f.party ? 40 : 6;     // a party flight comes as its start only: flown on for its whole life
            const int dt = (int)(f.t - f.tp), ahead = (int)(g_lf - f.t) < cap ? (int)(g_lf - f.t) : cap;
            int x = f.x, y = f.y, z = f.z;
            if (dt > 0 && dt < 30) { x += (f.x - f.px) / dt * ahead; y += (f.y - f.py) / dt * ahead; z += (f.z - f.pz) / dt * ahead; }
            wr32(egg, 0x5c, x); wr32(egg, 0x60, y); wr32(egg, 0x64, z);
            std::memcpy((char *)egg + 0x8e, &f.yaw, 2);
            return 2;
        }
        for (int i = 0; i < s.ntrail; ++i) {
            if (s.trail[i] != egg || s.trail_uid[i] != u) continue;
            char *ghost = (char *)player_of(egg);
            const int nb = data_0209f21c < kBodies ? data_0209f21c : kBodies;
            for (int b = 0; b < nb; ++b) {
                if (data_0209f394[b] != ghost || b == (int)data_0209f250) continue;
                int dp[3];
                if (!port_adventure_ghost_drawpos(b, dp)) return 0;
                saved[0] = rd32(ghost, 0x5c); saved[1] = rd32(ghost, 0x60); saved[2] = rd32(ghost, 0x64); saved[3] = 1;
                if (g_log && (g_lf % 30) == (u % 30))
                    std::fprintf(stderr, "[geggs] trail lf=%u egg %u (#%d of seat body %d) at (%d,%d,%d), ghost drawn at (%d,%d,%d), ghost opacity %d, egg flags 0x%x\n",
                                 g_lf, u & 0xff, i, b, rd32(egg, 0x5c) >> 12, rd32(egg, 0x60) >> 12, rd32(egg, 0x64) >> 12, dp[0] >> 12, dp[1] >> 12, dp[2] >> 12,
                                 ((const unsigned char *)ghost)[0x6f5], (unsigned)rd32(egg, 0xb0));
                wr32(ghost, 0x5c, dp[0]); wr32(ghost, 0x60, dp[1]); wr32(ghost, 0x64, dp[2]);
                return 1;
            }
            return 0;
        }
    }
    return 0;
}

extern "C" void port_ghost_egg_end(void *egg, const int saved[4]) {
    if (!egg || !saved[3]) return;
    char *ghost = (char *)player_of(egg);
    if (!ghost) return;
    wr32(ghost, 0x5c, saved[0]); wr32(ghost, 0x60, saved[1]); wr32(ghost, 0x64, saved[2]);
}
