// COOPENTRY2: spread landing spots when a coop room enters a level together (painting exits, star exits, course starts,
// TP landings). Every console places only its own body (friends' bodies are ghost-driven), so the only thing to decide here
// is where THIS console's body starts. Seat 0 keeps the level's own entrance spot; seats 1..3 are nudged left / right /
// forward of it by 120 units, checked with a ground probe (must be floor within 60 units of the entrance floor) and a line
// probe (nothing solid between the entrance and the spot). If no candidate passes, the entrance point is kept.
//
// How it hooks in: level_boot.cpp's port_load1 calls port_coop_entry_begin right before the ROM's LoadEntranceObjects and
// port_coop_entry_end right after. begin rewrites the x/z of the local body's entrance record in place, end puts the original
// back, so every later reader of the table (and the death-respawn spot, which is read off the body) is consistent. When the
// local body is not seated by the ROM's loop (a slot past the level's records) it is seated by port_vs_spawn_extra_players,
// which asks port_coop_entry_extra for the spot instead.
//
// ON by default for coop / party rooms. SM64DS_COOP_ENTRY=0 turns it off (before / after proof rows).

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "comms_hub.h"

extern "C" {
extern unsigned char data_0209f2d8;   // game mode: 0 single file, 1 VS, 2 script
extern unsigned char data_0209f2fc;   // entry reason: 1 exit course, 2 death
extern unsigned char data_0209f250;   // local body slot

void _ZN9dBgCh_GndC1Ev(void *self);
void _ZN9dBgCh_GndD1Ev(void *self);
void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void *self, const void *pos, void *actor);
int _ZN9dBgCh_Gnd10DetectClsnEv(void *self);
void _ZN9dBgCh_LinC1Ev(void *self);
void _ZN9dBgCh_LinD1Ev(void *self);
void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void *self, const void *a, const void *b, void *actor);
int _ZN9dBgCh_Lin10DetectClsnEv(void *self);
}

namespace {

struct Entry { unsigned short raw; short x, y, z; short rx, ry, rz; unsigned short param; };
struct ObjSubTable { unsigned char pad0, count, pad2[2]; Entry *entries; };

struct V3 { int x, y, z; };

constexpr int kSpacing = 120;        // world units between seats
constexpr int kGroundTol = 60;       // slot floor may differ from the entrance floor by this much
constexpr int kLineExtra = 50;       // the wall probe runs this far past the slot

bool g_cfg_read = false, g_cfg_on = true;
Entry *g_rec = nullptr;              // record being rewritten (null when nothing to restore)
short g_save_x = 0, g_save_z = 0;
bool g_extra_valid = false;          // the spot for a local body seated by the extra-player pass
int g_extra_slot = -1;
int g_extra_pos[3] = {0, 0, 0};      // 20.12 fixed

void read_cfg() {
    if (g_cfg_read) return;
    g_cfg_read = true;
    if (const char *e = std::getenv("SM64DS_COOP_ENTRY")) g_cfg_on = std::atoi(e) != 0;
}

bool env_on(const char *n) { const char *e = std::getenv(n); return e && std::atoi(e) != 0; }

bool gate() {
    read_cfg();
    if (!g_cfg_on) return false;
    if (!env_on("SM64DS_COOP") && !env_on("SM64DS_PARTY")) return false;
    if (!port::comms_hub_active() || port::comms_hub_arena()) return false;
    if (data_0209f2d8 != 0) return false;
    return true;
}

// Entry type (param >> 7 & 0xf) to the Player entry step the ROM table gives it, with the exit / death adjustments.
int step_of(int sl) {
    static const signed char kStep[20] = {0, 1, 2, 3, 4, 4, 4, 4, 8, 8, 10, 11, 11, 13, 0, 4, 8, 17, 0, 0};
    if (sl < 0 || sl > 19) return -1;
    int s = kStep[sl];
    if (data_0209f2fc == 2) { if (s == 4) s = 5; else if (s == 8) s = 9; else if (s == 11) s = 12; }
    else if (data_0209f2fc == 1) { if (s == 4) s = 15; }
    return s;
}

bool step_allowed(int s) {
    switch (s) { case 0: case 3: case 4: case 5: case 8: case 9: case 10: case 11: case 12: case 15: case 16: return true; }
    return false;
}

// Floor height (20.12) under world point (x,z), probed from refY + 200 units. False when nothing is hit.
bool ground_at(int x, int refY_fx, int z, int *out_y) {
    alignas(16) unsigned char rg[0x60];
    std::memset(rg, 0, sizeof rg);
    V3 pos = {x << 12, refY_fx + (200 << 12), z << 12};
    _ZN9dBgCh_GndC1Ev(rg);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(rg, &pos, nullptr);
    const bool hit = _ZN9dBgCh_Gnd10DetectClsnEv(rg) != 0;
    if (hit) *out_y = *(int *)(rg + 0x44);
    _ZN9dBgCh_GndD1Ev(rg);
    return hit;
}

// True when a solid surface lies on the segment a->b.
bool line_blocked(const V3 &a, const V3 &b) {
    alignas(16) unsigned char ln[0xa0];
    std::memset(ln, 0, sizeof ln);
    _ZN9dBgCh_LinC1Ev(ln);
    _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(ln, &a, &b, nullptr);
    const bool hit = _ZN9dBgCh_Lin10DetectClsnEv(ln) != 0;
    _ZN9dBgCh_LinD1Ev(ln);
    return hit;
}

struct Cand { int dx, dz; };

// Choose the spot for `seat` (1..3) around the entrance record. Returns the number of the candidate used (0 = first choice,
// 1.. = fallbacks), or -1 for "keep the entrance point".
int pick(const Entry *e, int step, int seat, int *ox, int *oz, int *why) {
    const double ang = (double)(unsigned short)e->ry * 6.283185307179586 / 65536.0;
    double fx = std::sin(ang), fz = std::cos(ang);
    if (step == 9 || step == 12) { fx = -fx; fz = -fz; }           // turned 180
    const double rx = fz, rz = -fx;                                  // right of forward
    auto R = [](double v) { return (int)std::lround(v); };
    Cand c[32];
    int n = 0;
    const int S = kSpacing;
    if (seat == 1) {
        c[n++] = {R(-rx * S), R(-rz * S)};                           // left
        c[n++] = {R((fx - rx) * S * 0.7071), R((fz - rz) * S * 0.7071)};
    } else if (seat == 2) {
        c[n++] = {R(rx * S), R(rz * S)};                             // right
        c[n++] = {R((fx + rx) * S * 0.7071), R((fz + rz) * S * 0.7071)};
    } else {
        c[n++] = {R(fx * S), R(fz * S)};                             // front
        c[n++] = {R(-rx * S * 2), R(-rz * S * 2)};
        c[n++] = {R(rx * S * 2), R(rz * S * 2)};
    }
    // More spots when the first choices fail: behind and the two back diagonals at 120, then 60-unit versions of each
    // direction (so a tight painting alcove still gets every seat somewhere apart from the entrance point).
    {
        const double d8[8][2] = {{-fx, -fz}, {-fx - rx, -fz - rz}, {-fx + rx, -fz + rz}, {fx, fz},
                                 {-rx, -rz}, {rx, rz}, {fx - rx, fz - rz}, {fx + rx, fz + rz}};
        const double sc[8] = {1, 0.7071, 0.7071, 1, 1, 1, 0.7071, 0.7071};
        for (int pass = 0; pass < 2; ++pass)
            for (int d = 0; d < 8 && n < 32; ++d) {
                if (pass == 0 && d >= 3) continue;                       // pass 0: behind + back diagonals only
                const int len = pass == 0 ? S : S / 2;
                c[n++] = {R(d8[d][0] * len * sc[d]), R(d8[d][1] * len * sc[d])};
            }
    }
    int gy0 = 0;
    if (!ground_at(e->x, (int)e->y << 12, e->z, &gy0)) { *why = 1; return -1; }
    for (int i = 0; i < n; ++i) {
        const int sx = e->x + c[i].dx, sz = e->z + c[i].dz;
        if (sx < -32000 || sx > 32000 || sz < -32000 || sz > 32000) continue;
        int gy = 0;
        if (!ground_at(sx, gy0, sz, &gy)) continue;
        const int dy = (gy - gy0) >> 12;
        if (dy > kGroundTol || dy < -kGroundTol) continue;
        // wall probe: from the entrance, 60 above its floor, to the spot and 50 past it, at the same height
        const double len = std::sqrt((double)c[i].dx * c[i].dx + (double)c[i].dz * c[i].dz);
        const double k = len > 0 ? (len + kLineExtra) / len : 1.0;
        V3 a = {(int)e->x << 12, gy0 + (60 << 12), (int)e->z << 12};
        V3 b = {(e->x + R(c[i].dx * k)) << 12, gy0 + (60 << 12), (e->z + R(c[i].dz * k)) << 12};
        if (line_blocked(a, b)) continue;
        *ox = sx; *oz = sz;
        return i;
    }
    *why = 2;
    return -1;
}

}  // namespace

extern "C" void port_coop_entry_begin(void *tbl, unsigned p3, int local, int hold, int level) {
    g_rec = nullptr;
    g_extra_valid = false;
    g_extra_slot = -1;
    if (!gate()) return;
    const int seat = port::comms_hub_my_seat();
    ObjSubTable *t = (ObjSubTable *)tbl;
    const unsigned idx = p3 + (hold ? 0u : (unsigned)local);
    if (!t || !t->entries || (unsigned)t->count <= idx) return;
    Entry *e = t->entries + idx;
    const int sl = (e->param >> 7) & 0xf;
    const int entr = (e->param >> 3) & 0xf;
    const int step = step_of(sl);
    const char *res = "ok";
    int nx = e->x, nz = e->z, which = -2, why = 0;
    if (seat < 1 || seat > 3) res = "seat0";
    else if (!step_allowed(step)) res = "skip-step";
    else {
        which = pick(e, step, seat, &nx, &nz, &why);
        if (which < 0) { res = why == 1 ? "fallback-no-ground" : "fallback-no-slot"; nx = e->x; nz = e->z; }
    }
    std::fprintf(stderr,
                 "[coopentry] level %d entrance %d sl %d step %d seat %d local %d hold %d rec (%d,%d,%d) yaw %d -> (%d,%d) cand %d %s\n",
                 level, entr, sl, step, seat, local, hold, (int)e->x, (int)e->y, (int)e->z, (int)(unsigned short)e->ry, nx, nz,
                 which, res);
    if (nx == e->x && nz == e->z) return;
    if (hold) {
        g_extra_valid = true;
        g_extra_slot = local;
        g_extra_pos[0] = nx << 12;
        g_extra_pos[1] = (int)e->y << 12;
        g_extra_pos[2] = nz << 12;
        return;
    }
    g_rec = e;
    g_save_x = e->x;
    g_save_z = e->z;
    e->x = (short)nx;
    e->z = (short)nz;
}

extern "C" void port_coop_entry_end() {
    if (g_rec) {
        g_rec->x = g_save_x;
        g_rec->z = g_save_z;
        g_rec = nullptr;
    }
}

// Called by port_vs_spawn_extra_players for each slot it seats; returns true and fills pos (20.12) for the local body.
extern "C" int port_coop_entry_extra(int slot, int *pos) {
    if (!g_extra_valid || slot != g_extra_slot) return 0;
    pos[0] = g_extra_pos[0];
    pos[1] = g_extra_pos[1];
    pos[2] = g_extra_pos[2];
    g_extra_valid = false;
    return 1;
}
