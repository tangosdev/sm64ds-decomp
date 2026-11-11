// PARTY BODIES: membership by seat, the SYNL gate, the solid flag per body. See
// hal/party.h. The cylinder and the spike's lab live in hal/party_cyl.cpp.
//
// Knobs (all off by default; the first four are the proof's):
//   SM64DS_PARTY=1            this console is in a party room (the launcher sets it)
//   SM64DS_COOP=1             coop room: the same solid bodies (FIX9A), hidden in another level / area / act
//   SM64DS_PARTY_MEMBERS=1,2  lab: only these SEATS are party, the rest stay ghosts
//   SM64DS_PARTY_GATE=0       lab: ignore SYNL (the ghost frames alone decide)
//   SM64DS_PARTY_LOG=1        a [party] line every 60 follower passes, and the
//                             gate / cylinder events
//   SM64DS_PARTY_SEP=1        a [partysep] line per solid body per frame (the
//                             separation to my body, for the push rows)

#include "party.h"
#include "hub_presence.h"
#include "comms_hub.h"
#include "player_fields.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern unsigned char data_0209f21c;
extern signed char data_0209f2f8;
extern signed char data_02092120;
extern int data_0209f220[];
unsigned _ZNK6Player14GetBodyModelIDEjb(void *self, unsigned a, int b);
int port_player_render_hidden(const void *player);
int port_adventure_ghost_palette(int slot);
}

namespace port {
namespace party {
namespace {

enum { kMaxBodies = 16, kStackUnits = 10, kUnstickUnits = 110 };

int g_read = 0;
int g_all = 0;                 // SM64DS_PARTY=1
int g_off = 0;                 // SM64DS_ROOM_COLLISION=0: room toggle, friends stay ghosts (party and coop)
int g_coop = 0;                // SM64DS_COOP=1 (FIX9A: coop seats are solid bodies too)
bool g_have_list = false;
unsigned g_mask = 0;           // SM64DS_PARTY_MEMBERS, seats
unsigned g_rt_mask = 0;        // set_member(): seats made members at run time (in-world party invites)
int g_gate_on = 1;
int g_log = 0;
int g_sep = 0;
bool g_solid[kMaxBodies];
Stats g_st = {};
unsigned g_pass = 0;

void read_env() {
    if (g_read) return;
    g_read = 1;
    if (const char *e = std::getenv("SM64DS_PARTY")) g_all = std::atoi(e) != 0;
    if (const char *e = std::getenv("SM64DS_COOP")) g_coop = std::atoi(e) != 0;
    if (const char *e = std::getenv("SM64DS_PARTY_MEMBERS")) {
        g_have_list = true;
        for (const char *q = e; *q;) {
            char *end;
            const long v = std::strtol(q, &end, 10);
            if (end == q) break;
            if (v >= 0 && v < 32) g_mask |= (1u << (unsigned)v);
            q = *end == ',' ? end + 1 : end;
        }
    }
    if (const char *e = std::getenv("SM64DS_ROOM_COLLISION")) g_off = e[0] == '0' && e[1] == 0;
    std::fprintf(stderr, "[party] room collision %s\n", g_off ? "OFF (ghosts)" : "ON (solid bodies)");
    if (const char *e = std::getenv("SM64DS_PARTY_GATE")) g_gate_on = std::atoi(e) != 0;
    if (const char *e = std::getenv("SM64DS_PARTY_LOG")) g_log = std::atoi(e);
    if (const char *e = std::getenv("SM64DS_PARTY_SEP")) g_sep = std::atoi(e);
}

}  // namespace

bool active() {
    read_env();
    if (g_off) return false;
    return g_all != 0 || g_coop != 0 || g_have_list || g_rt_mask != 0;
}

bool seat_is_party(int seat) {
    read_env();
    if (seat < 0 || seat >= kHubMaxSeats) return false;
    if (seat == comms_hub_my_seat()) return false;
    if (g_off) return false;
    if ((g_rt_mask >> seat) & 1u) return true;
    if (g_have_list) return ((g_mask >> seat) & 1u) != 0;
    return g_all != 0 || g_coop != 0;
}

void set_member(int seat, bool on) {
    if (seat < 0 || seat >= kHubMaxSeats || seat >= 32) return;
    if (on) g_rt_mask |= (1u << (unsigned)seat); else g_rt_mask &= ~(1u << (unsigned)seat);
}

bool is_member(int seat) { return seat_is_party(seat); }

int gate(int seat) {
    read_env();
    if (!g_gate_on) return kGateOk;
    HubPresence p;
    unsigned age = 0;
    // No word from him yet (or none for 3 s): his ghost frames decide, as they do
    // for an adventure ghost. The relay sends SYNL on every change and at 2 Hz.
    if (!hub_presence_get(seat, &p, &age) || age > 3000u) return kGateOk;
    // COOPYOSHI2: a menu alone (the pause menu) no longer hides him; it is kGatePaused below, after the level checks.
    static const bool old_gate = [] { const char *e = std::getenv("SM64DS_COOP_DRAWGATE"); return e && e[0] == '0' && !e[1]; }();
    if (!p.in_level() || p.in_cutscene() || (old_gate && p.in_menu())) {
        ++g_st.gate_hide;
        return kGateHide;
    }
    if (p.level != (int)(unsigned char)data_0209f2f8) {
        ++g_st.gate_level;
        return kGateLevel;
    }
    if (p.area != (int)data_02092120) {
        ++g_st.gate_area;
        return kGateArea;
    }
    // Coop: the group key is level + act, so a friend in another act of this level is in another world.
    if (g_coop && p.act > 0 && (data_0209f220[0] & 0xff) > 0 && p.act != (data_0209f220[0] & 0xff)) {
        ++g_st.gate_level;
        return kGateLevel;
    }
    if (p.in_menu()) return kGatePaused;
    return kGateOk;
}

void prepare(void *a) {
    char *c = (char *)a;
    // mAttachedActor (+0x35c): func_ov002_020e3f90 (called by the placement, func_ov002_020e444c) takes the body's
    // matrix from that actor instead of from the body's own position. A body seated at a level entrance that
    // attaches the player (a door, a painting, a pad) and then stops running keeps it for ever.
    if (*(void **)(c + 0x35c)) {
        *(void **)(c + 0x35c) = nullptr;
        ++g_st.detached;
    }
}

void draw_diag(void *a) {
    read_env();
    if (g_log < 2 || !a) return;
    static unsigned n = 0;
    if ((n++ % 45u) != 0) return;
    const char *c = (const char *)a;
    const unsigned id = _ZNK6Player14GetBodyModelIDEjb(const_cast<char *>(c), *(const int *)(c + 8) & 0xff, 0);
    const char *mdl = *(const char *const *)(c + 0xdc + id * 4);
    int mx = 0, my = 0, mz = 0;
    if (mdl) { mx = *(const int *)(mdl + 0x1c + 36) >> 12; my = *(const int *)(mdl + 0x1c + 40) >> 12; mz = *(const int *)(mdl + 0x1c + 44) >> 12; }
    // the polygon attribute word of the first materials of the body model (alpha bits 16-20, mode 4-5, cull 6-7, id 24-29)
    unsigned attr[4] = {0, 0, 0, 0};
    unsigned nmat = 0;
    if (mdl) {
        const int *mp = (const int *)(mdl + 8);
        const unsigned char *hdr = (const unsigned char *)mp[0];
        const char *rec = (const char *)mp[1];
        if (hdr && rec) {
            std::memcpy(&nmat, hdr + 0x24, 4);
            for (unsigned i = 0; i < 4 && i < nmat; ++i) std::memcpy(&attr[i], rec + i * 0x30 + 0x24, 4);
        }
    }
    std::fprintf(stderr, "[partydraw] materials=%u attr=%08x,%08x,%08x,%08x ", nmat, attr[0], attr[1], attr[2], attr[3]);
    const int pal = port_adventure_ghost_palette((int)*(const unsigned char *)(c + 0x6d8));
    const unsigned short *row = pal >= 0 ? (const unsigned short *)(0x06880000u + (unsigned)pal * 16u) : nullptr;
    std::fprintf(stderr, "[partydraw] pal=%d row=%04x,%04x,%04x,%04x ", pal, row ? row[0] : 0u, row ? row[1] : 0u,
                 row ? row[2] : 0u, row ? row[3] : 0u);
    std::fprintf(stderr, "[partydraw] t=%u slot=%u pos=(%d,%d,%d) matrix_t=(%d,%d,%d) opacity=%u vanish=%u flags_b0=%08x "
                 "scale=(%d,%d,%d) attached=%p inv=%u off68c=(%d,%d,%d) model=%p hidden=%d\n", comms_hub_now_ms(),
                 (unsigned)*(const unsigned char *)(c + 0x6d8), *(const int *)(c + 0x5c) >> 12, *(const int *)(c + 0x60) >> 12,
                 *(const int *)(c + 0x64) >> 12, mx, my, mz, (unsigned)*(const unsigned char *)(c + 0x6f5),
                 (unsigned)*(const unsigned char *)(c + 0x6fb), *(const unsigned *)(c + 0xb0), *(const int *)(c + 0x80),
                 *(const int *)(c + 0x84), *(const int *)(c + 0x88), *(void *const *)(c + 0x35c),
                 (unsigned)*(const unsigned short *)(c + player::kInvincibleTimer), *(const int *)(c + 0x68c),
                 *(const int *)(c + 0x690), *(const int *)(c + 0x694), (const void *)mdl, port_player_render_hidden(c));
}

void clear(int body) {
    if (body >= 0 && body < kMaxBodies) g_solid[body] = false;
    lab_tick(body);
}

bool body_solid(int body) {
    return body >= 0 && body < kMaxBodies && g_solid[body];
}

// PBODY1_FIX / FIX9A: friends that spawn on one spot and stand idle. The ROM pair pass ASSIGNS a node's pushback
// (it does not add), so with three or more bodies on one spot each console is pushed away from only the last friend in
// the list, often the same way and by the same amount as another console: that pair never separates. Only the HIGHER
// seat of a pair moves, along a direction of its own (a quarter turn per seat), once, by more than the radius sum;
// every pair of seats ends further apart than the radius sum whatever the list order. Two bodies are not touched
// until they are within kStackUnits of each other, so the 2-body push numbers do not change.
void unstick(void *a, int seat) {
    if (seat >= comms_hub_my_seat()) return;            // the lower seat stays where it is
    const int me = (int)data_0209f250;
    char *mb = (me >= 0 && me < kMaxBodies) ? (char *)data_0209f394[me] : nullptr;
    if (!mb || mb == (char *)a) return;
    if (*(const int *)(mb + 0x37c) != 0) return;         // my body does not integrate its position now
    const long long dx = (long long)(*player::pos_x(a) - *player::pos_x(mb)) >> 12;
    const long long dz = (long long)(*player::pos_z(a) - *player::pos_z(mb)) >> 12;
    const long long dy = (long long)(*player::pos_y(a) - *player::pos_y(mb)) >> 12;
    if (dy > 150 || dy < -150) return;
    if (dx * dx + dz * dz >= (long long)kStackUnits * kStackUnits) return;
    // Only a stack of three or more (me, this friend and at least one more on the same spot): two bodies keep the ROM push.
    int third = 0;
    for (int i = 0; i < (int)data_0209f21c && i < kMaxBodies; ++i) {
        char *o = (char *)data_0209f394[i];
        if (!o || o == mb || o == (char *)a || !g_solid[i]) continue;   // only bodies follow() took through gate()
        const long long ex = (long long)(*player::pos_x(o) - *player::pos_x(mb)) >> 12;
        const long long ez = (long long)(*player::pos_z(o) - *player::pos_z(mb)) >> 12;
        const long long ey = (long long)(*player::pos_y(o) - *player::pos_y(mb)) >> 12;
        if (ey <= 150 && ey >= -150 && ex * ex + ez * ez < (long long)kStackUnits * kStackUnits) ++third;
    }
    if (!third) return;
    // Not sure the step is safe unless my body stands on something: in the air (a ledge, a drop) skip it
    // and try again next frame. The step goes into the pushback word, which the ROM integrates through its
    // own wall and floor collision, not a position write.
    if (!player::on_ground(mb)) return;
    static const int ux[4] = {1, 0, -1, 0}, uz[4] = {0, 1, 0, -1};
    const int k = comms_hub_my_seat() & 3;
    *(int *)(mb + 0x2e0) += ux[k] * kUnstickUnits * 4096;
    *(int *)(mb + 0x2e8) += uz[k] * kUnstickUnits * 4096;
    ++g_st.unstick;
    if (g_log > 0) std::fprintf(stderr, "[party] unstick: my seat %d steps %d units from the stack of seat %d\n", comms_hub_my_seat(), kUnstickUnits, seat);
}

bool follow(void *a, int body, int seat) {
    read_env();
    if (!a || body < 0 || body >= kMaxBodies) return false;
    // The invincibility timer (+0x6a0) is not counted down on a body whose Behavior never runs, and
    // a course entry leaves it set: the ROM's blink gate (port_player_render_hidden, bit 0) then hides
    // the body on every frame when the value is odd. A party body is not hurt here, so it carries none.
    const unsigned inv = *(const unsigned short *)((const char *)a + player::kInvincibleTimer);
    *(unsigned short *)((char *)a + player::kInvincibleTimer) = 0;
    lab_before_register(a, body);
    g_solid[body] = true;
    ++g_st.solid_frames;
    ++g_pass;
    cyl_assist(a);
    unstick(a, seat);
    // SHOVE9: in a coop room the LOWER seat wins a push. A friend of a higher seat than mine has no cylinder on this
    // console: his console moves him off my ghost, mine does not move off his. Without this, two bodies that start on one
    // spot (every group entrance and the opening's spawn) are pushed the same way by the pair pass and each sees the
    // other's delayed ghost behind it, so both keep pushing west (measured: 12 units a frame, 2700 units, to the wall).
    static const bool prio_off = std::getenv("SM64DS_SHOVE_PRIO") && std::getenv("SM64DS_SHOVE_PRIO")[0] == '0';   // proof control
    bool reg;
    if (g_coop && !prio_off && seat > comms_hub_my_seat()) {
        cyl_withhold(a);
        reg = false;
        static unsigned skips;
        if (g_log > 0 && (skips++ % 600u) == 0) std::fprintf(stderr, "[party] shove: no cylinder for seat %d (mine is %d), lower seat wins the push\n", seat, comms_hub_my_seat());
    } else {
        reg = cyl_register(a, seat);
    }
    if (g_sep > 0) {
        const int me = (int)data_0209f250;
        void *mb = (me >= 0 && me < kMaxBodies) ? data_0209f394[me] : nullptr;
        if (mb) {
            const long long dx = (long long)(*player::pos_x(a) - *player::pos_x(mb));
            const long long dz = (long long)(*player::pos_z(a) - *player::pos_z(mb));
            const long long dy = (long long)(*player::pos_y(a) - *player::pos_y(mb));
            // horizontal distance in whole units (Fix12 >> 12), integer sqrt
            long long d2 = (dx >> 4) * (dx >> 4) + (dz >> 4) * (dz >> 4);   // (Fix12 / 16)^2
            long long r = 0, lo = 0, hi = 1LL << 31;
            while (lo < hi) {
                const long long mid = (lo + hi + 1) / 2;
                if (mid * mid <= d2) lo = mid; else hi = mid - 1;
            }
            r = lo;                                  // Fix12 / 16
            std::fprintf(stderr, "[partysep] t=%u seat=%d body=%d horiz=%d.%02d dy=%d me=(%d,%d,%d) "
                         "p=(%d,%d,%d) reg=%d push=(%d,%d,%d) hit=%x own=%u inv=%u\n", comms_hub_now_ms(), seat, body,
                         (int)(r * 16 / 4096), (int)((r * 16 * 100 / 4096) % 100), (int)(dy >> 12),
                         *player::pos_x(mb) >> 12, *player::pos_y(mb) >> 12, *player::pos_z(mb) >> 12,
                         *player::pos_x(a) >> 12, *player::pos_y(a) >> 12, *player::pos_z(a) >> 12,
                         reg ? 1 : 0,
                         *(const int *)((const char *)mb + 0x2e0), *(const int *)((const char *)mb + 0x2e4),
                         *(const int *)((const char *)mb + 0x2e8), *(const unsigned *)((const char *)mb + 0x2f4),
                         *(const unsigned *)((const char *)mb + 0x2f8), inv);
        }
    }
    if (g_log > 0 && (g_pass % 60u) == 0)
        std::fprintf(stderr, "[party] t=%u seat=%d body=%d solid_frames=%llu registers=%llu guard_skips=%llu "
                     "gate(hide=%llu level=%llu area=%llu) listnodes_at_follow=%llu pairs_body=%llu pairs_other=%llu assists=%llu detached=%llu\n",
                     comms_hub_now_ms(), seat, body, g_st.solid_frames, g_st.registers, g_st.guard_skips,
                     g_st.gate_hide, g_st.gate_level, g_st.gate_area, g_st.audit_nodes, g_st.pairs_body,
                     g_st.pairs_other, g_st.assists, g_st.detached);
    return reg;
}

const Stats &stats() { return g_st; }

Stats &stats_rw() { return g_st; }

int log_level() { read_env(); return g_log; }

}  // namespace party
}  // namespace port

extern "C" int port_party_member(int slot) {
    return port::party::body_solid(slot) ? 1 : 0;
}

extern "C" void party_set_member(int seat, bool on) { port::party::set_member(seat, on); }
extern "C" bool party_is_member(int seat) { return port::party::is_member(seat); }
extern "C" bool party_active() { return port::party::active(); }
