// COOP: THE ROLLING IRON BALLS A SPAWNER MAKES, SHARED. Run coopfix1, lane BOBSYNC1. See coop_balls.h.
//
// PORT_HOST_ABI. Reads live actors by raw offset (the port/hal convention): any actor's unique id +0x04, param1 +0x08, class
// +0x0c, area +0xcc, death-table slot +0xce, position +0x5c, facing +0x8c / +0x92 (mPrevAngle*, what the spawner hands Spawn),
// mStateTimer +0x100 (dEnemyBase_c); a daIbl_c's mDispenser +0x3a8, mMinSpawnDist / mMaxSpawnDist +0x3c0 / +0x3c4, mVariant
// +0x3d0, mLiveBalls +0x3d2 (include/daIbl_c.h). Writes only a held spawner's timer, its ball count when this module spawns a
// copy, and a copy's mDispenser (the same three stores daIbl_c::func_ov100_02142b90 makes).

#include "coop_balls.h"
#include "coop_actors.h"
#include "coop_world.h"
#include "comms_hub.h"

#include "dActor_c.h"

#include <windows.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern signed char data_0209f2f8;                       // the current sublevel
void *_ZN8dActor_c4NextEPKS_(const void *prev);
void *_ZN8dActor_c10FindWithIDEj(unsigned id);
void _ZN7fBase_c18MarkForDestructionEv(void *a);
}

namespace port {
namespace {

enum : int {
    kCls = 220, kBmb = 206,
    kBallSlot0 = 480, kSpawners = 4, kRing = 6,          // virtual slots 480..503 (the Big Bully's three are 505..507)
    kFreshUnits = 250,                                   // a record this close to the spawner is a ball that was just made
    kStaleUnits = 600,                                   // a local copy this far from such a record is an old ball in a reused slot
    kRxFreshMs = 400, kCopyGapFrames = 12, kOpportunities = 2,
};

bool g_env = false, g_on = true, g_log = false;
unsigned g_sample = 0;
int g_my = -1;
int g_lv = -1, g_ac = -1;
unsigned g_lf = 0;
bool g_clash = false;

struct Sp { char *a; unsigned uid; short slot; unsigned copy_lf; int opp; bool held_logged; };
Sp g_sp[kSpawners]; int g_nsp = 0;
struct Map { char *a; unsigned uid; };
Map g_map[kSpawners][kRing];
bool g_shares[4], g_noshare[4];
struct Stats { unsigned assigned, copies, copy_fail, replaced, held_frames, opp, noshare, clash; } g_st = {};

int rd32(const void *a, int off) { int v; std::memcpy(&v, (const char *)a + off, 4); return v; }
short rd16(const void *a, int off) { short v; std::memcpy(&v, (const char *)a + off, 2); return v; }
void wr16(void *a, int off, short v) { std::memcpy((char *)a + off, &v, 2); }
unsigned cls_of(const void *a) { unsigned short c; std::memcpy(&c, (const char *)a + 0x0c, 2); return c; }
unsigned uid_of(const void *a) { return (unsigned)rd32(a, 0x04); }
char *dispenser_of(const void *a) { char *p; std::memcpy(&p, (const char *)a + 0x3a8, sizeof p); return p; }
unsigned char variant_of(const void *a) { return ((const unsigned char *)a)[0x3d0]; }
bool alive(unsigned uid, const void *a) { return uid && a && _ZN8dActor_c10FindWithIDEj(uid) == a; }
double units(const void *a, int x, int y, int z) {
    const double dx = ((double)rd32(a, 0x5c) - x) / 4096.0, dy = ((double)rd32(a, 0x60) - y) / 4096.0, dz = ((double)rd32(a, 0x64) - z) / 4096.0;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}
double units_aa(const void *a, const void *b) { return units(a, rd32(b, 0x5c), rd32(b, 0x60), rd32(b, 0x64)); }
long long wall_ms() {
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    return (long long)((((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000ull);
}

void read_env() {
    if (g_env) return;
    g_env = true;
    const char *e = std::getenv("SM64DS_COOP_BALLS");
    if (e && *e == '0') g_on = false;
    const char *a = std::getenv("SM64DS_COOP_ACT");      // the mover sync's own control switches this off too
    if (a && *a == '0') g_on = false;
    const char *l = std::getenv("SM64DS_COOP_BALLS_LOG");
    g_log = l && *l && *l != '0';
    if (const char *s = std::getenv("SM64DS_COOP_BALLS_SAMPLE")) g_sample = (unsigned)std::strtoul(s, nullptr, 10);
    if (g_log) std::fprintf(stderr, "[cball] shared spawner balls %s\n", g_on ? "ON" : "OFF (control)");
}

void reset_level() {
    std::memset(g_sp, 0, sizeof g_sp); g_nsp = 0;
    std::memset(g_map, 0, sizeof g_map);
    std::memset(g_shares, 0, sizeof g_shares); std::memset(g_noshare, 0, sizeof g_noshare);
    g_lf = 0; g_clash = false;
}

int sp_index_of(const void *spawner) {
    for (int i = 0; i < g_nsp; ++i) if (g_sp[i].a == spawner) return i;
    return -1;
}

// The spawner's own test (daIbl_c::func_ov100_02142b90) past its timer: would it make a ball now?
bool spawn_test(char *sp) {
    const unsigned cap = data_0209f2f8 == 6 ? 3u : 6u;
    if (((unsigned char *)sp)[0x3d2] >= cap) return false;
    const char *pl = (const char *)((dActor_c *)sp)->ClosestPlayer();
    if (!pl) return false;
    const unsigned param = (unsigned)rd32(sp, 0x08);
    if ((param & 0xf) != 4 && rd32(pl, 0x60) >= rd32(sp, 0x60) - 0x28000) return false;
    const double dx = ((double)rd32(pl, 0x5c) - rd32(sp, 0x5c)), dz = ((double)rd32(pl, 0x64) - rd32(sp, 0x64));
    const double d = std::sqrt(dx * dx + dz * dz);
    if (d < (double)rd32(sp, 0x3c0) || d > (double)rd32(sp, 0x3c4)) return false;
    if (data_0209f2f8 == 0x16 && rd32(pl, 0x60) < (int)0xff63c000) return false;
    return true;
}
short reload() { return data_0209f2f8 == 0x16 ? (short)0x3f : (short)0x7f; }

// The spawner's own Spawn call, made for a ball the owner's spawner made.
char *spawn_copy(Sp &s) {
    char *sp = s.a;
    Vector3 pos; std::memcpy((void *)&pos, sp + 0x5c, 12);
    char *b = (char *)dActor_c::Spawn((u32)kCls, (u32)rd32(sp, 0x08), pos, (const Vector3_16 *)(sp + 0x92), (s8)sp[0xcc], (s16)-1);
    if (!b) return nullptr;
    ((unsigned char *)sp)[0x3d2] = (unsigned char)(((unsigned char *)sp)[0x3d2] + 1);
    std::memcpy(b + 0x3a8, &sp, sizeof sp);
    wr16(sp, 0x100, reload());                           // the owner's spawner reloaded its timer here: so does mine
    return b;
}

void scan() {
    g_nsp = 0;
    Sp found[kSpawners]; int nf = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        const short slot = rd16(a, 0xce);
        if (slot >= kBallSlot0 && slot < kBallSlot0 + kSpawners * kRing && !g_clash) {
            g_clash = true; ++g_st.clash;
            std::fprintf(stderr, "[cball] placed actor class %u sits in virtual ball slot %d: shared balls OFF for this level\n", cls_of(a), (int)slot);
        }
        if (cls_of(a) != kCls || slot < 0 || variant_of(a) != 0) continue;
        if (nf < kSpawners) { found[nf].a = a; found[nf].uid = uid_of(a); found[nf].slot = slot; ++nf; }
    }
    // spawners in slot order: every console numbers them the same
    for (int i = 0; i < nf; ++i)
        for (int j = i + 1; j < nf; ++j) if (found[j].slot < found[i].slot) { Sp t = found[i]; found[i] = found[j]; found[j] = t; }
    static Sp prev[kSpawners]; static int nprev = 0;
    for (int i = 0; i < nf; ++i) {
        Sp n = found[i]; n.copy_lf = 0; n.opp = 0; n.held_logged = false;
        for (int k = 0; k < nprev; ++k) if (prev[k].uid == n.uid) { n.copy_lf = prev[k].copy_lf; n.opp = prev[k].opp; n.held_logged = prev[k].held_logged; }
        g_sp[i] = n;
    }
    g_nsp = nf;
    std::memcpy(prev, g_sp, sizeof prev); nprev = nf;
    for (int i = 0; i < kSpawners; ++i)
        for (int k = 0; k < kRing; ++k) {
            Map &m = g_map[i][k];
            if (m.a && (!alive(m.uid, m.a) || i >= g_nsp || dispenser_of(m.a) != g_sp[i].a)) { m.a = nullptr; m.uid = 0; }
        }
}

bool mapped(const void *a) {
    for (int i = 0; i < kSpawners; ++i) for (int k = 0; k < kRing; ++k) if (g_map[i][k].a == a) return true;
    return false;
}

// Owner side: the balls my spawner made this frame take the lowest free ring index.
void assign_mine(int i) {
    Sp &s = g_sp[i];
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (cls_of(a) != kCls || rd16(a, 0xce) >= 0 || dispenser_of(a) != s.a || mapped(a)) continue;
        if (units_aa(a, s.a) > kFreshUnits) continue;   // an old ball from before I owned the spawner: not shared
        int k = 0;
        while (k < kRing && g_map[i][k].a) ++k;
        if (k >= kRing) return;
        g_map[i][k].a = a; g_map[i][k].uid = uid_of(a);
        ++g_st.assigned;
        if (g_log) std::fprintf(stderr, "[cball] my spawner %d (slot %d) made a ball: virtual slot %d uid %u lf=%u wall=%lld\n", i, (int)s.slot,
                                kBallSlot0 + i * kRing + k, uid_of(a), g_lf, wall_ms());
    }
}

// Friend side: hold my spawner and spawn the owner's balls.
void follow(int i, int owner, unsigned now) {
    Sp &s = g_sp[i];
    // 1. a ball the owner just made: spawn my copy at the spawner
    for (int k = 0; k < kRing; ++k) {
        const int slot = kBallSlot0 + i * kRing + k;
        CoopMoveRxPose x;
        if (!coop_move_rx_pose(slot, owner, &x) || x.cls != kCls || x.age_ms > (unsigned)kRxFreshMs) continue;
        const double from_sp = units(s.a, x.x, x.y, x.z);
        Map &m = g_map[i][k];
        if (m.a) {
            if (from_sp > kFreshUnits || units(m.a, x.x, x.y, x.z) < kStaleUnits) continue;
            // my copy in this slot is an older ball (it outlived the owner's): the owner's new ball takes the slot
            if (g_log) std::fprintf(stderr, "[cball] virtual slot %d: my old ball uid %u is %.0f units from the owner's new one: replaced\n", slot, m.uid, units(m.a, x.x, x.y, x.z));
            _ZN7fBase_c18MarkForDestructionEv(m.a);
            m.a = nullptr; m.uid = 0; ++g_st.replaced;
        }
        if (from_sp > kFreshUnits) continue;            // already rolling: a late copy would start from the wrong path node
        if (s.copy_lf && g_lf - s.copy_lf < (unsigned)kCopyGapFrames) continue;
        char *b = spawn_copy(s);
        if (!b) { ++g_st.copy_fail; continue; }
        m.a = b; m.uid = uid_of(b); s.copy_lf = g_lf;
        ++g_st.copies;
        if (g_log) std::fprintf(stderr, "[cball] seat %d's spawner %d made a ball: my copy uid %u in virtual slot %d (record %.0f units from the spawner, %u ms old) lf=%u wall=%lld\n",
                                owner, i, m.uid, slot, from_sp, x.age_ms, g_lf, wall_ms());
    }
    // 2. hold my spawner: its timer never reaches 0 while the owner makes the balls
    if (g_noshare[owner]) return;
    short t = rd16(s.a, 0x100);
    if (t != 0) return;
    ++g_st.held_frames;
    if (!g_shares[owner] && spawn_test(s.a)) {
        ++s.opp; ++g_st.opp;
        if (g_log) std::fprintf(stderr, "[cball] spawner %d held: it would make a ball now (%d without a record from seat %d) wall=%lld\n", i, s.opp, owner, wall_ms());
        if (s.opp >= kOpportunities) {
            g_noshare[owner] = true; ++g_st.noshare;
            std::fprintf(stderr, "[cball] seat %d owns spawner %d but sends no balls (an older build?): my spawners roll their own for it\n", owner, i);
            return;
        }
        wr16(s.a, 0x100, reload());
        return;
    }
    wr16(s.a, 0x100, 1);
    (void)now;
}

void sample() {
    char line[4096]; int n = std::snprintf(line, sizeof line, "[ballpos] w=%lld lf=%u seat=%d", wall_ms(), g_lf, g_my);
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && n < (int)sizeof line - 64; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        const unsigned c = cls_of(a);
        if (c == kCls) {
            if (rd16(a, 0xce) >= 0) continue;
            int vs = coop_ball_virtual_slot(a);
            n += std::snprintf(line + n, sizeof line - n, " b%d:%d:%d:%d", vs, rd32(a, 0x5c) >> 12, rd32(a, 0x60) >> 12, rd32(a, 0x64) >> 12);
        } else if (c == kBmb) {
            n += std::snprintf(line + n, sizeof line - n, " m%d:%d:%d:%d:s%d:r%d:f%d", (int)rd16(a, 0xce), rd32(a, 0x5c) >> 12, rd32(a, 0x60) >> 12, rd32(a, 0x64) >> 12,
                               rd32(a, 0x3dc), (int)((const unsigned char *)a)[0x3f3], (int)(unsigned short)rd16(a, 0x3ea));
        }
    }
    std::fprintf(stderr, "%s\n", line);
}

}  // namespace

int coop_ball_virtual_slot(const void *actor) {
    if (!g_on || g_clash || !actor) return -1;
    for (int i = 0; i < kSpawners; ++i)
        for (int k = 0; k < kRing; ++k) if (g_map[i][k].a == actor) return kBallSlot0 + i * kRing + k;
    return -1;
}

int coop_ball_claim_seat(int slot) {
    if (!g_on || g_clash || slot < kBallSlot0 || slot >= kBallSlot0 + kSpawners * kRing) return -1;
    const int i = (slot - kBallSlot0) / kRing;
    if (i >= g_nsp) return -1;
    return coop_move_owner(g_sp[i].slot);
}

void coop_balls_frame(unsigned now) {
    read_env();
    int lv = -1, ac = -1;
    if (!coop_group_established() || !coop_group_key(&lv, &ac)) return;
    if (lv != g_lv || ac != g_ac) { reset_level(); g_lv = lv; g_ac = ac; }
    ++g_lf;
    g_my = comms_hub_my_seat();
    {   // proof knob SM64DS_COOP_BALLS_STALL=<frame>:<ms>: this console's whole game stops once for <ms> at that frame of the level
        // (what a pause, a window drag or a slow star select did to the world clock). Off unless named.
        static int at = -2, ms = 0;
        if (at == -2) { at = -1; if (const char *e = std::getenv("SM64DS_COOP_BALLS_STALL")) std::sscanf(e, "%d:%d", &at, &ms); }
        if (at >= 0 && g_lf == (unsigned)at && ms > 0 && ms <= 20000) {
            std::fprintf(stderr, "[cball] STALL knob: stopping %d ms at frame %u wall=%lld\n", ms, g_lf, wall_ms());
            Sleep((DWORD)ms);
        }
    }
    if (g_sample && (g_lf % g_sample) == 0) { scan(); sample(); }
    if (!g_on) return;
    scan();
    if (g_clash || g_nsp == 0 || g_my < 0) return;
    // who shares: any record for a ball slot from a seat
    for (int q = 0; q < 4; ++q) {
        if (q == g_my || g_shares[q]) continue;
        for (int s = kBallSlot0; s < kBallSlot0 + kSpawners * kRing && !g_shares[q]; ++s) {
            CoopMoveRxPose x;
            if (coop_move_rx_pose(s, q, &x) && x.cls == kCls) {
                g_shares[q] = true;
                if (g_log) std::fprintf(stderr, "[cball] seat %d shares its spawner balls wall=%lld\n", q, wall_ms());
            }
        }
    }
    const unsigned mask = coop_group_member_mask();
    for (int i = 0; i < g_nsp; ++i) {
        const int o = coop_move_owner(g_sp[i].slot);
        if (o >= 0 && o < 4 && o != g_my && ((mask >> o) & 1u)) follow(i, o, now);
        else assign_mine(i);                            // mine, or nobody's yet: my spawner runs as the cartridge's
    }
    static unsigned last = 0;
    if (g_log && now - last >= 10000u) {
        last = now;
        std::fprintf(stderr, "[cball] stats spawners=%d assigned=%u copies=%u copy_fail=%u replaced=%u held=%u opp=%u noshare=%u clash=%u\n", g_nsp,
                     g_st.assigned, g_st.copies, g_st.copy_fail, g_st.replaced, g_st.held_frames, g_st.opp, g_st.noshare, g_st.clash);
    }
}

}  // namespace port
