// COOP: A WALKING BOB-OMB IS AWAKE, LIT OR GONE ON EVERY SCREEN AT ONCE. Run coopfix1, lane BOBSYNC1 (Tango's bug 19).
//
// The mover sync (hal/coop_actors.cpp) already pins a placed Bob-omb's PLACE to its owner's copy (the nearest player's
// console). What it did not carry is the Bob-omb's life cycle, and that is decided per console: after a Bob-omb blows up it
// waits at home, undrawn (daBmb_c mState 5), until the CAMERA player of that console is 2000 units away
// (daBmb_c::State5). A friend far away woke his copy at once while the owner's copy still slept, so on one screen a
// Bob-omb walked, chased and blew up where on the other there was nothing (bombs rows: 39 of 207 paired samples with the
// state and drawn flag different, the far copy 1356 units off its owner's invisible one).
//
// The rule now: the owner sends each owned placed Bob-omb's {state, drawn, fuse} when it changes and every second. A
// friend's copy follows the owner's at the edges the cartridge itself takes:
//   * owner awake, mine asleep          -> mine wakes at home (State5's own wake: func_ov102_0214c0b8, flags, drawn)
//   * owner's fuse lit, mine not         -> mine is lit with the owner's count (func_ov102_0214b384, the notice's own call)
//   * owner just went to sleep (it blew up) while mine is awake -> mine's fuse is cut to 4 (it blows up the cartridge's way,
//     with its blast hurting whoever stands there: enemies hurt in coop)
//   * owner asleep for good while mine is awake (I woke it alone) -> mine goes back to sleep at home, quietly
//     (func_ov102_0214baa0, the Bob-omb's own "back home" step)
// A Bob-omb carried by my own player is left alone (the carry is mine), as is one in a death or a Yoshi mouth.
// King Bob-omb's bombs are runtime actors with no placed slot: never touched here (lane KBO2).
//
// WIRE: SYNW 0x11 BOMB_STATE inside the mover range this lane family owns (0x10-0x1f), so an older build's mover handler
// drops it (counted as drop_bad there) and never applies it; a friend on an older build never sends it, so my copies
// keep the old behaviour for the Bob-ombs he owns.
//   head 8 B: CoopKeyWire key, u8 count (1..32), u8 ver (1), u16 reserved (0)
//   rec 6 B:  u16 slot (< 512), u8 state (0..5), u8 drawn (0..1), u16 fuse (0..0x96)
//
// INERT outside a coop room. SM64DS_COOP_BOMBS=0 is the control (the old per-console life cycle). SM64DS_COOP_BALLS_LOG=1
// prints [cbomb] lines too.

#include "coop_balls.h"
#include "coop_actors.h"
#include "coop_world.h"
#include "coop_wire.h"
#include "comms_hub.h"

#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;
void *_ZN8dActor_c4NextEPKS_(const void *prev);
void func_ov102_0214c0b8(void *bmb);                       // daBmb_c: back to walking (State5's wake calls it)
void func_ov102_0214baa0(char *bmb);                       // daBmb_c: asleep at home, undrawn (the end of a blast)
void func_ov102_0214b384(void *bmb, unsigned int fuse);    // daBmb_c: light / shorten the fuse
extern int data_0208e380;                                  // the distance dActor_c::ClosestPlayer leaves behind
}

// Only the name, to take the address of dActor_c::DistToCPlayer (src/runtime/actor/d_actor.cpp) for the patch below.
struct dActor_c { int DistToCPlayer(); };

namespace port {
namespace {

enum : int { kBmb = 206, kSlots = 512, kSeats = 4, kMaxRecs = 32, kVer = 1, kBeatMs = 1000, kMaxFuse = 0x96 };

#pragma pack(push, 1)
struct BombHead { CoopKeyWire key; uint8_t count, ver; uint16_t reserved; };
struct BombRec { uint16_t slot; uint8_t state, drawn; uint16_t fuse; };
#pragma pack(pop)
static_assert(sizeof(BombHead) == 8, "bomb head is 8 bytes");
static_assert(sizeof(BombRec) == 6, "bomb record is 6 bytes");

bool g_env = false, g_on = true, g_log = false;
int g_lv = -1, g_ac = -1;
struct Tx { uint8_t state, drawn; uint16_t fuse; unsigned t; bool valid; };
Tx g_tx[kSlots];
struct Rx { uint8_t state, drawn; uint16_t fuse; unsigned t; bool valid, fresh, prev_awake; };
Rx g_rx[kSlots][kSeats];
struct Stats { unsigned sent, recs, rx, bad, woke, lit, blown, slept, carried_skip, held; } g_st = {};

int rd32(const void *a, int off) { int v; std::memcpy(&v, (const char *)a + off, 4); return v; }
short rd16(const void *a, int off) { short v; std::memcpy(&v, (const char *)a + off, 2); return v; }
void wr32(void *a, int off, int v) { std::memcpy((char *)a + off, &v, 4); }
unsigned cls_of(const void *a) { unsigned short c; std::memcpy(&c, (const char *)a + 0x0c, 2); return c; }
int state_of(const void *a) { return rd32(a, 0x3dc); }
int drawn_of(const void *a) { return ((const unsigned char *)a)[0x3f3]; }
unsigned fuse_of(const void *a) { return (unsigned short)rd16(a, 0x3ea); }
const void *carrier_of(const void *a) { const void *p; std::memcpy(&p, (const char *)a + 0x390, sizeof p); return p; }
long long wall_ms() {
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    return (long long)((((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000ull);
}

void read_env() {
    if (g_env) return;
    g_env = true;
    const char *e = std::getenv("SM64DS_COOP_BOMBS");
    if (e && *e == '0') g_on = false;
    const char *a = std::getenv("SM64DS_COOP_ACT");
    if (a && *a == '0') g_on = false;
    const char *l = std::getenv("SM64DS_COOP_BALLS_LOG");
    g_log = l && *l && *l != '0';
    if (g_log) std::fprintf(stderr, "[cbomb] shared Bob-omb life cycle %s\n", g_on ? "ON" : "OFF (control)");
}

bool validate(const unsigned char *p, int len) {
    if (!p || len < (int)(sizeof(BombHead) + sizeof(BombRec))) return false;
    BombHead h; std::memcpy(&h, p, sizeof h);
    if (!coop_key_ok(h.key) || h.ver != kVer || h.reserved != 0 || h.count < 1 || h.count > kMaxRecs) return false;
    if (len != (int)(sizeof h + h.count * sizeof(BombRec))) return false;
    for (int i = 0; i < h.count; ++i) {
        BombRec r; std::memcpy(&r, p + sizeof h + i * sizeof r, sizeof r);
        if (r.slot >= kSlots || r.state > 5 || r.drawn > 1 || r.fuse > kMaxFuse) return false;
    }
    return true;
}

// The bombs my own player's hands hold: the carry is mine, a friend's record never moves it.
bool mine_carried(const void *a) {
    const void *c = carrier_of(a);
    return c && c == data_0209f394[data_0209f250 & 15];
}

// A friend's copy stays asleep while the owner's sleeps. State5 wakes a Bob-omb on THIS console's terms (its camera player
// 2000 units away and the Bob-omb off screen), so a far friend's copy woke alone while the owner's slept on, and putting it
// back to sleep only made it wake again the next frame. State5 is inlined into the Bob-omb's state switch, so the gate sits
// in the one call it makes, dActor_c::DistToCPlayer: for a sleeping placed Bob-omb owned by another console whose fresh
// record says asleep, the distance reads 0 and State5 stays put. Every other caller, solo play, a quiet owner and an older
// build's owner (no records) get the cartridge's body unchanged (ClosestPlayer, then its distance).
bool hold_asleep(const void *a) {
    if (!g_on || cls_of(a) != kBmb || state_of(a) != 5 || !coop_group_established()) return false;
    const short slot = rd16(a, 0xce);
    if (slot < 0 || slot >= kSlots) return false;
    const int owner = coop_move_owner(slot), me = comms_hub_my_seat();
    if (owner < 0 || owner >= kSeats || owner == me) return false;
    const Rx &x = g_rx[slot][owner];
    return x.valid && comms_hub_now_ms() - x.t <= 3000u && x.state == 5;
}

typedef void *(__fastcall *ClosestFn)(void *self, void *edx);
ClosestFn g_closest = nullptr;

int __fastcall dist_replacement(void *self, void * /*edx*/) {
    if (hold_asleep(self)) {
        if (g_log && (g_st.held++ % 300u) == 0)
            std::fprintf(stderr, "[cbomb] slot %d: would wake here, the owner's sleeps: held asleep (%u) wall=%lld\n", rd16(self, 0xce), g_st.held, wall_ms());
        return 0;
    }
    g_closest(self, nullptr);
    return data_0208e380;
}

bool g_dist_patched = false;
void install_dist_patch() {
    if (g_dist_patched) return;
    g_dist_patched = true;
    int (dActor_c::*pmf)() = &dActor_c::DistToCPlayer;
    unsigned char *from = nullptr; std::memcpy(&from, &pmf, sizeof from);
    if (from[0] == 0xE9) {                                   // an incremental-link thunk: follow it to the body
        int r; std::memcpy(&r, from + 1, 4); from = from + 5 + r;
    }
    if (from[0] != 0xE8) { std::fprintf(stderr, "[cbomb] DistToCPlayer is not the expected call+load, not patched\n"); return; }
    int rel0; std::memcpy(&rel0, from + 1, 4);
    g_closest = (ClosestFn)(from + 5 + rel0);                // dActor_c::ClosestPlayer, the body's own first call
    DWORD old = 0;
    if (!VirtualProtect(from, 8, PAGE_EXECUTE_READWRITE, &old)) { std::fprintf(stderr, "[cbomb] cannot patch DistToCPlayer\n"); return; }
    unsigned char b[5]; b[0] = 0xE9;
    const int rel = (int)((unsigned char *)&dist_replacement - from - 5);
    std::memcpy(b + 1, &rel, 4);
    std::memcpy(from, b, 5);
    VirtualProtect(from, 8, old, &old);
    FlushInstructionCache(GetCurrentProcess(), from, 8);
    if (g_log) std::fprintf(stderr, "[cbomb] dActor_c::DistToCPlayer gated at %p (ClosestPlayer %p)\n", (void *)from, (void *)g_closest);
}

void apply(char *a, int slot, Rx &x) {
    if (*(const unsigned char *)(a + 0x0f) || *(const unsigned char *)(a + 0x0e) == 2) return;   // dying / dead
    if ((unsigned)rd32(a, 0xb0) & 0x60000u) return;                                             // in a Yoshi mouth
    if (carrier_of(a)) { if (mine_carried(a)) ++g_st.carried_skip; return; }
    const int st = state_of(a);
    const bool owner_awake = x.state != 5;
    if (owner_awake && st == 5 && x.fuse == 0) {         // (a lit owner's Bob-omb is about to blow: mine, already blown, stays home)
        func_ov102_0214c0b8(a);                              // State5's own wake
        wr32(a, 0xb0, rd32(a, 0xb0) | 0x10000001);
        ((unsigned char *)a)[0x3f3] = 1;
        ++g_st.woke;
        if (g_log) std::fprintf(stderr, "[cbomb] slot %d: the owner's Bob-omb is awake, mine woke at home wall=%lld\n", slot, wall_ms());
        return;
    }
    if (owner_awake && st <= 2 && x.fuse > 4 && fuse_of(a) == 0) {
        func_ov102_0214b384(a, x.fuse);
        ++g_st.lit;
        if (g_log) std::fprintf(stderr, "[cbomb] slot %d: the owner's fuse is lit (%u), mine lit to match wall=%lld\n", slot, (unsigned)x.fuse, wall_ms());
        return;
    }
    if (!owner_awake && st != 5) {
        if (x.prev_awake && x.fresh) {
            // the owner's Bob-omb just blew up: mine blows up the cartridge's way
            if (fuse_of(a) == 0 || fuse_of(a) > 4) {
                func_ov102_0214b384(a, 4);
                ++g_st.blown;
                if (g_log) std::fprintf(stderr, "[cbomb] slot %d: the owner's Bob-omb blew up, mine's fuse cut to 4 wall=%lld\n", slot, wall_ms());
            }
            return;
        }
        if (fuse_of(a) != 0) return;                         // mine is going off by itself already
        {   // the Bob-omb's own back-home step only parks variants 0, 1 and 5; any other variant it removes through the death table
            const unsigned char v = ((const unsigned char *)a)[0x3f5];
            if (!(v < 2 || v == 5)) return;
        }
        func_ov102_0214baa0(a);                              // I woke it alone: back to sleep at home
        ++g_st.slept;
        if (g_log) std::fprintf(stderr, "[cbomb] slot %d: the owner's Bob-omb sleeps, mine went back home wall=%lld\n", slot, wall_ms());
    }
}

void on_world(int seat, const unsigned char *p, int len) {
    if (seat < 0 || seat >= kSeats || seat == comms_hub_my_seat()) return;
    if (!validate(p, len)) { ++g_st.bad; return; }
    BombHead h; std::memcpy(&h, p, sizeof h);
    if (h.key.level != (uint16_t)g_lv || h.key.act != (uint8_t)g_ac) return;
    if (!((coop_group_member_mask() >> seat) & 1u)) return;
    const unsigned now = comms_hub_now_ms();
    for (int i = 0; i < h.count; ++i) {
        BombRec r; std::memcpy(&r, p + sizeof h + i * sizeof r, sizeof r);
        Rx &x = g_rx[r.slot][seat];
        const bool was_awake = x.valid ? x.state != 5 : false;
        // a transition stays "just happened" until it is applied
        if (!(x.fresh && x.prev_awake)) x.prev_awake = was_awake;
        x.state = r.state; x.drawn = r.drawn; x.fuse = r.fuse; x.t = now; x.valid = true; x.fresh = true;
        ++g_st.rx;
    }
}

}  // namespace

void coop_bombs_on_world(int seat, const void *payload, int len) {
    read_env();
    if (!g_on) return;
    on_world(seat, static_cast<const unsigned char *>(payload), len);
}

void coop_bombs_frame(unsigned now) {
    read_env();
    if (!g_on) return;
    install_dist_patch();
    int lv = -1, ac = -1;
    if (!coop_group_established() || !coop_group_key(&lv, &ac)) return;
    if (lv != g_lv || ac != g_ac) { std::memset(g_tx, 0, sizeof g_tx); std::memset(g_rx, 0, sizeof g_rx); g_lv = lv; g_ac = ac; }
    const int me = comms_hub_my_seat();
    if (me < 0 || me >= kSeats) return;
    unsigned char buf[sizeof(BombHead) + kMaxRecs * sizeof(BombRec)];
    int n = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (cls_of(a) != kBmb) continue;
        const short slot = rd16(a, 0xce);
        if (slot < 0 || slot >= kSlots) continue;            // King Bob-omb's bombs and other runtime ones: not mine to share
        const int owner = coop_move_owner(slot);
        if (owner == me) {
            int st = state_of(a); if (st < 0 || st > 5) continue;
            unsigned fu = fuse_of(a); if (fu > kMaxFuse) fu = kMaxFuse;
            const uint8_t dr = (uint8_t)(drawn_of(a) ? 1 : 0);
            Tx &t = g_tx[slot];
            const bool changed = !t.valid || t.state != st || t.drawn != dr || ((t.fuse == 0) != (fu == 0));
            if ((changed || now - t.t >= (unsigned)kBeatMs) && n < kMaxRecs) {
                BombRec r; r.slot = (uint16_t)slot; r.state = (uint8_t)st; r.drawn = dr; r.fuse = (uint16_t)fu;
                std::memcpy(buf + sizeof(BombHead) + n * sizeof r, &r, sizeof r); ++n;
                t.state = (uint8_t)st; t.drawn = dr; t.fuse = (uint16_t)fu; t.t = now; t.valid = true;
            }
        } else if (owner >= 0 && owner < kSeats) {
            Rx &x = g_rx[slot][owner];
            if (!x.valid || now - x.t > 3000u) continue;     // a quiet owner (or an older build): my copy runs as before
            apply(a, slot, x);
            x.fresh = false; x.prev_awake = false;
        }
    }
    if (n > 0) {
        BombHead h; h.key.level = (uint16_t)lv; h.key.act = (uint8_t)ac; h.key.reserved = 0; h.count = (uint8_t)n; h.ver = kVer; h.reserved = 0;
        std::memcpy(buf, &h, sizeof h);
        if (coop_send_world(kWorldBombState, buf, (int)(sizeof h + n * sizeof(BombRec)))) { ++g_st.sent; g_st.recs += (unsigned)n; }
    }
    static unsigned last = 0;
    if (g_log && now - last >= 10000u) {
        last = now;
        std::fprintf(stderr, "[cbomb] stats sent=%u recs=%u rx=%u bad=%u woke=%u lit=%u blown=%u slept=%u carried_skip=%u held=%u\n", g_st.sent, g_st.recs,
                     g_st.rx, g_st.bad, g_st.woke, g_st.lit, g_st.blown, g_st.slept, g_st.carried_skip, g_st.held);
    }
}

}  // namespace port
