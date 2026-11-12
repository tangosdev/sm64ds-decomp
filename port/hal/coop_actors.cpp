// COOP PHASE 2: LIVE ENEMIES AND MOVING PLATFORMS KEPT IN STEP. Run mp1, lane COOPMOVE1. See coop_actors.h.
//
// PORT_HOST_ABI. Reads and writes live actors by raw offset (the established convention in port/hal); nothing in
// src/ changes. Every write is range-checked: an offset is accepted only inside 0xd0..0x600 and aligned to its
// width, a position only inside +-kMoverMaxUnits world units, and only after the record passed the validator.

#include "coop_actors.h"
#include "coop_world.h"
#include "coop_groups.h"
#include "coop_wire.h"
#include "comms_hub.h"
#include "hub_presence.h"
#include "coop_target.h"
#include "coop_boss.h"
#include "coop_stats_log.h"
#include "coop_ghost_eggs.h"

#include <windows.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern short data_02092144[];
void ChangeArea(int areaID);
// Player::State objects (ov002 bss, the order of hal/vs_arena.cpp kStateObjects): St_BurnLava (11), St_DeadHit (12), St_DeadPit (13).
extern unsigned char data_ov002_021100f4[], data_ov002_0211010c[], data_ov002_02110124[];
void *_ZN8dActor_c4NextEPKS_(const void *prev);
}
// proof pictures (SM64DS_COOP_MOVE_SHOT): the level loop's top-screen framebuffer, written by the same writer coop_boot.cpp uses
namespace ntr { struct Framebuffer; bool ppu_write_bmp(const char *path, const Framebuffer &fb); }
extern "C" const void *port_level_fb_ptr;   // coop_boot.cpp

namespace port { void coop_cap_frame(unsigned now_ms); }   // hal/coop_cap.cpp (CAPLOSS1)
namespace port {
namespace {

// ---------------------------------------------------------------------------
// Profiles.
// ---------------------------------------------------------------------------
CoopMoveStats g_st = {};
bool g_guard = true;          // SM64DS_COOP_MOVE_GUARD=0 is the proof CONTROL: no size / value guard (the pre-fix behaviour)
enum : unsigned { kFPhase = 1u, kFSlave = 2u, kFBoss = 4u, kFSampleOnly = 8u };   // kFBoss (MOVERS9): the pose owner is the boss's TARGET seat, the same rule as the boss lane's authority   // kFSlave: the non-owner pins the pose every frame to the owner's, extrapolated
// A phase field: where it sits, how wide it is, and the range a received value must lie in (signed for width 4, unsigned for 1 / 2).
struct Field { uint16_t off; uint8_t size; int32_t lo, hi; };
struct Prof {
    uint16_t id;
    const char *name;
    uint16_t period, band, snap;
    uint8_t flags, nf;
    Field f[kMoverMaxFields];
    uint16_t hold;          // 0 = none; else a pointer word that is non-zero while something carries the actor
    uint16_t size;          // the class's allocation size (coop_actor_sizes.inc), 0 = not in the table
    bool     dead;          // switched off at run time (the live block turned out smaller than a field needs)
    bool     live_seen;     // the live block size of this class was read and logged once
};
struct SizeRow { uint16_t id, bytes; };
#define SIZE(id, bytes, name) { id, bytes },
const SizeRow kSizes[] = {
#include "coop_actor_sizes.inc"
};
#undef SIZE
uint16_t class_size(unsigned id) {
    for (const SizeRow &r : kSizes) if (r.id == id) return r.bytes;
    return 0;
}

#define ROW(id, name, period, band, snap, flags, nf, ...) { id, name, period, band, snap, flags, nf, { __VA_ARGS__ } },
Prof g_prof[256] = {
#include "coop_actor_profiles.inc"
};
#undef ROW
int g_nprof = 0;
// MOVERS9: always-on per-class sync numbers (read by coop_stats_log.cpp). Cheap: a few adds per actor per tick.
struct CStat { unsigned ents_max, pub, applied, slaved, owner_ch, pin_n, pin_big, held_pins; double pin_sum, pin_max; };
CStat g_cs[256];
bool g_hold_mips = true;      // SM64DS_MOVERS_MIPSHOLD=0 is the MUST-FAIL control: MIPS (187) is not on the carrier list
bool g_new_rows = true;       // SM64DS_MOVERS_NEWROWS=0 is the MUST-FAIL control: the 12 MOVERS9 classes are only SAMPLED (free-running, as with no row)
bool g_boss_own = true;       // SM64DS_MOVERS_BOSSOWN=0 is the MUST-FAIL control: boss poses keep the proximity owner
short g_prof_of[1024];      // class id -> index into g_prof, -1 none
char  g_prof_name[256][40]; // names of rows added from the profile file

enum : int {
    kSlots = 512, kSeats = 4,
    kFieldLo = 0xd0, kFieldHi = 0x600,   // COOPRIDE1: 0x600 so the Tox box roll state (0x558..0x58b) fits
    kStaleMs = 3000, kHeartbeatMs = 1500, kDwellMs = 2000, kRxKeepMs = 4000,
    kPayloadMax = kHubCoopMaxDatagram - kHubCoopHeaderBytes,
};

// A field must lie inside the CLASS's own allocation (size = bytes the ROM's operator new hands out for it); size 0 = the
// class is not in the table, so only the header words (below 0xd0, in every actor) are allowed.
bool field_ok(const Field &f, unsigned size) {
    if (f.size != 1 && f.size != 2 && f.size != 4) return false;
    if (f.lo > f.hi) return false;
    const bool speed_words = f.off >= 0x98 && (int)f.off + f.size <= 0xb0;       // mHorzSpeed .. unk_0ac
    if (!speed_words) {
        if (f.off < kFieldLo || (int)f.off + f.size > kFieldHi) return false;
        if (g_guard && (size == 0 || (unsigned)f.off + f.size > size)) return false;
    }
    return (f.off % f.size) == 0;
}

// A received phase value must lie in the field's range (a forged record must not write an arbitrary value).
bool value_in(const Field &f, uint32_t v) {
    long long x;
    if (f.size == 4) x = (int32_t)v; else if (f.size == 2) x = (uint16_t)v; else x = (uint8_t)v;
    return x >= f.lo && x <= f.hi;
}
bool value_ok(const Field &f, uint32_t v) { return !g_guard || value_in(f, v); }

void prof_index() {
    for (int i = 0; i < 1024; ++i) g_prof_of[i] = -1;
    g_nprof = 0;
    while (g_nprof < 256 && g_prof[g_nprof].id != 0) ++g_nprof;
    for (int i = 0; i < g_nprof; ++i) {
        Prof &p = g_prof[i];
        bool ok = p.id < 1024 && p.nf <= kMoverMaxFields;
        p.size = ok ? class_size(p.id) : 0;
        for (int k = 0; ok && k < p.nf; ++k) {
            ok = field_ok(p.f[k], p.size);
            if (!ok) std::fprintf(stderr, "[coopmove] size guard: row %d (class %d) field %#x+%d past the class size %#x (or bad): row dropped\n",
                                  i, (int)p.id, (unsigned)p.f[k].off, (int)p.f[k].size, (unsigned)p.size);
        }
        if (ok && p.hold && (p.size == 0 || p.hold + 4u > p.size)) ok = false;
        if (!ok) { std::fprintf(stderr, "[coopmove] profile row %d (class %d) rejected\n", i, (int)p.id); ++g_st.rows_dropped; p.id = 0; continue; }
        if (p.size == 0) std::fprintf(stderr, "[coopmove] size guard: class %d is not in the size table (pose only, no fields)\n", (int)p.id);
        g_prof_of[p.id] = (short)i;
    }
    if (!g_new_rows) {
        static const uint16_t kNew[] = { 262, 250, 251, 236, 240, 238, 315, 234, 18, 194, 96, 72 };
        for (uint16_t id : kNew) if (g_prof_of[id] >= 0) g_prof[g_prof_of[id]].flags |= kFSampleOnly;
    }
    // classes a player can pick up: their pose is the carrier's, not the Behavior's
    static const struct { uint16_t id, off; } kHold[] = { { 206, 0x390 }, { 187, 0x45c } };   // 206 Bob-omb mCarrier; 187 MIPS: the carrier Player* at +0x45c (daMip_c UpdateGrab sets it, StateReleased clears it)
    for (const auto &h : kHold)
        if (h.id == 187 && !g_hold_mips) continue;
        else if (h.id < 1024 && g_prof_of[h.id] >= 0 && g_prof[g_prof_of[h.id]].hold == 0) {
            Prof &hp = g_prof[g_prof_of[h.id]];
            if (hp.size != 0 && h.off + 4u <= hp.size) hp.hold = h.off;
            else { std::fprintf(stderr, "[coopmove] size guard: class %d carried-test word %#x past the class size: not used\n", (int)h.id, (unsigned)h.off); ++g_st.rows_dropped; }
        }
}

// SM64DS_COOP_MOVE_PROFILE=<file>: lines `id period band snap flags nf off:size ...`, '#' comments.
void load_profile_file(const char *path) {
    FILE *fp = std::fopen(path, "r");
    if (!fp) { std::fprintf(stderr, "[coopmove] profile file %s: cannot open\n", path); return; }
    char line[512];
    int added = 0;
    while (std::fgets(line, sizeof line, fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        int id, period, band, snap, flags, nf, used = 0;
        if (std::sscanf(line, "%i %i %i %i %i %i%n", &id, &period, &band, &snap, &flags, &nf, &used) < 6) continue;
        if (id <= 0 || id >= 1024 || nf < 0 || nf > kMoverMaxFields || period < 20 || period > 5000) continue;
        Prof np; std::memset(&np, 0, sizeof np);
        np.id = (uint16_t)id; np.period = (uint16_t)period; np.band = (uint16_t)band; np.snap = (uint16_t)snap;
        np.flags = (uint8_t)flags; np.nf = (uint8_t)nf;
        const char *p = line + used;
        bool ok = true;
        for (int k = 0; k < nf; ++k) {
            int off, sz, u2 = 0;
            if (std::sscanf(p, " %i:%i%n", &off, &sz, &u2) < 2) { ok = false; break; }
            p += u2;
            np.f[k].off = (uint16_t)off; np.f[k].size = (uint8_t)sz;
            // optional range `:lo:hi`; without it the full range of the width (a developer knob, never on the wire)
            int lo = 0, hi = 0, u3 = 0;
            if (sz == 4) { lo = (int)0x80000000; hi = 0x7fffffff; } else if (sz == 2) { lo = 0; hi = 0xffff; } else { lo = 0; hi = 0xff; }
            if (std::sscanf(p, ":%i:%i%n", &lo, &hi, &u3) >= 2) p += u3;
            np.f[k].lo = lo; np.f[k].hi = hi;
            np.size = class_size((unsigned)id);
            if (!field_ok(np.f[k], np.size)) {
                std::fprintf(stderr, "[coopmove] size guard: file row class %d field %#x+%d past the class size %#x (or bad): row dropped\n",
                             id, off, sz, (unsigned)np.size);
                ++g_st.rows_dropped;
                ok = false; break;
            }
        }
        if (ok) if (const char *h = std::strstr(p, "hold=")) np.hold = (uint16_t)std::strtoul(h + 5, nullptr, 0);
        if (!ok) { std::fprintf(stderr, "[coopmove] profile file: bad row for class %d\n", id); continue; }
        np.size = class_size((unsigned)id);
        if (np.hold && (np.size == 0 || np.hold + 4u > np.size)) { std::fprintf(stderr, "[coopmove] size guard: file row class %d carried-test word past the class size: row dropped\n", id); ++g_st.rows_dropped; continue; }
        int at = -1;
        for (int i = 0; i < g_nprof; ++i) if (g_prof[i].id == id) at = i;
        if (at < 0) {
            if (g_nprof >= 255) continue;
            at = g_nprof++;
            std::snprintf(g_prof_name[at], sizeof g_prof_name[at], "FILE_%d", id);
            np.name = g_prof_name[at];
        } else np.name = g_prof[at].name;
        g_prof[at] = np;
        g_prof_of[id] = (short)at;
        ++added;
    }
    std::fclose(fp);
    std::fprintf(stderr, "[coopmove] profile file %s: %d rows applied\n", path, added);
}

// ---------------------------------------------------------------------------
// State.
// ---------------------------------------------------------------------------
struct Slot {
    char    *a;               // the local actor in this slot this tick, or null
    short    prof;
    unsigned seen;            // consecutive ticks the actor has been seen
    int      owner;           // -1 unknown, my seat, or a remote seat
    unsigned owner_t;         // when it last changed
    unsigned last_pub;
    unsigned pin_lf;          // TONGUE1: g_lf + 1 when a friend's held-object pin last moved this copy, 0 never, kPinStay muted
    bool     lvalid;
    int32_t  lx, ly, lz;
    int16_t  lax, lay, laz;
    uint32_t lf[kMoverMaxFields];
    bool     heard;           // a remote record for this slot was ever taken
    bool     hconf;           // COOPDIAMOND2: the copy's heading was checked against an owner's record (class 135)
    bool     bvalid;          // COOPDIAMOND2: bx/by/bz hold the spawn point a cycle mover's place is measured from
    int32_t  bx, by, bz;
};
struct Rx {
    bool     valid, fresh, snap;
    unsigned t;
    uint16_t cls, cd;
    int32_t  x, y, z;
    int16_t  ax, ay, az;
    uint8_t  rflags, nf;
    uint32_t f[kMoverMaxFields];
    bool     pvalid;          // the previous record of this slot from this seat (velocity for the lead)
    int32_t  px, py, pz;
    int16_t  pax, pay, paz;
    unsigned pt;
    uint32_t pf0;             // COOPDIAMOND2: the previous record's first phase field (the diamond lift's turn timer)
};
struct Job { bool on; int passes; unsigned next; int cursor; };

bool g_env = false, g_log = false, g_act = true, g_snap_on = true, g_hyst = true, g_stale_on = true;
bool g_dead_guard = true;     // COOPPUZZLE1: SM64DS_COOP_DEAD_GUARD=0 is the control (a dying body claims by distance, as before)
bool g_rider_own = true;      // COOPROWS1: SM64DS_COOP_SEESAW_RIDER=0 is the control (nearest to the pivot owns the plank, as before)
bool g_lift_heading = true;  // SM64DS_COOP_LIFT_HEADING=0 is the MUST-FAIL control (run after093, lane COOPPLAT1): a sliding slab keeps its own heading
unsigned g_lift_flips = 0;
bool g_dia_heading = true;   // SM64DS_COOP_DIAMOND_HEADING=0 is the MUST-FAIL control (run coopfix1, lane DIAMOND2): the diamond lift keeps its own heading
unsigned g_dia_flips = 0, g_dia_ok = 0, g_dia_held = 0;
bool g_derive = true;        // SM64DS_COOP_MOVER_DERIVE=0 is the MUST-FAIL control (lane DIAMOND2): cycle movers keep the place the pin left them
unsigned long long g_der_fix = 0; double g_der_max = 0; unsigned g_der_logs = 0, g_der_refused = 0;
unsigned long long g_der_hold = 0;
bool g_linked = true;        // SM64DS_COOP_LINKED=0 is the MUST-FAIL control (lane DIAMOND2): each linked BFS pole / floor has its own owner
unsigned g_link_sets = 0, g_link_follow = 0;
unsigned g_sample_every = 0;
unsigned g_dump_at[4]; int g_ndump = 0;
unsigned g_shot_at[16]; int g_nshot = 0;   // SM64DS_COOP_MOVE_SHOT=t1,t2,...: top-screen pictures that many ticks after the movers show up (proof only, lane DIAMOND2)
unsigned g_area_at = 0; int g_area_to = -1; bool g_area_done = false;   // SM64DS_COOP_MOVE_AREA=F:area (proof only, lane DIAMOND2)
struct ForgeKnob { unsigned tick; char name[12]; bool done; };
ForgeKnob g_forge[12]; int g_nforge = 0;

bool g_regd = false;
int  g_my = -1;
CoopKey g_key;
unsigned g_lf = 0;                // ticks since this group key
const unsigned kPinStay = 0xffffffffu;   // TONGUE1: a parked copy left behind by a finished hold
unsigned g_pin_skips = 0;         // TONGUE1: publishes held back because a friend's held pin moved the copy
unsigned g_spawn_lf = 0; bool g_spawned = false;
unsigned g_ents = 0;
Slot g_slot[kSlots];
Rx   g_rx[kSlots][kSeats];
Job  g_job[kSeats];
unsigned g_req_nonce = 0, g_req_last = 0; int g_req_tries = 0; bool g_req_done = false;
unsigned long long g_snap_base = 0;
bool g_god = false, g_god_set = false;
char g_god_pos[12];               // SM64DS_COOP_MOVE_GOD=1 (proof only)
bool g_hold = false;              // a joiner publishes nothing until the group's snapshot is in (or it gave up)
double g_tokens = 6.0; unsigned g_tok_t = 0;
unsigned g_stat_t = 0;
int g_lead_ms = 0;                // receivers aim this far ahead along the owner's last velocity
int g_hp_prev = -99;

long long wall_ms() {
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    return (long long)((((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000ull);
}

void read_env() {
    if (g_env) return;
    g_env = true;
    auto flag = [](const char *n, bool dflt) { const char *e = std::getenv(n); return (e && *e) ? *e != '0' : dflt; };
    g_log = flag("SM64DS_COOP_MOVE_LOG", false);
    g_lift_heading = flag("SM64DS_COOP_LIFT_HEADING", true);
    g_dia_heading = flag("SM64DS_COOP_DIAMOND_HEADING", true);
    g_derive = flag("SM64DS_COOP_MOVER_DERIVE", true);
    g_linked = flag("SM64DS_COOP_LINKED", true);
    g_boss_own = flag("SM64DS_MOVERS_BOSSOWN", true);
    g_new_rows = flag("SM64DS_MOVERS_NEWROWS", true);
    g_hold_mips = flag("SM64DS_MOVERS_MIPSHOLD", true);
    g_act = flag("SM64DS_COOP_ACT", true);
    g_guard = flag("SM64DS_COOP_MOVE_GUARD", true);
    g_snap_on = flag("SM64DS_COOP_MOVE_SNAP", true);
    g_god = flag("SM64DS_COOP_MOVE_GOD", false);
    g_hyst = flag("SM64DS_COOP_MOVE_HYST", true);
    g_stale_on = flag("SM64DS_COOP_MOVE_STALE", true);
    g_dead_guard = flag("SM64DS_COOP_DEAD_GUARD", true);
    g_rider_own = flag("SM64DS_COOP_SEESAW_RIDER", true);
    if (const char *ld = std::getenv("SM64DS_COOP_MOVE_LEAD")) g_lead_ms = std::atoi(ld);
    if (const char *s = std::getenv("SM64DS_COOP_MOVE_SAMPLE")) g_sample_every = (unsigned)std::strtoul(s, nullptr, 0);
    if (const char *ar = std::getenv("SM64DS_COOP_MOVE_AREA")) {
        // F:area: F ticks after the movers first show up in a level, put the local player in that area the way a CHANGE_ROOM trigger does
        // (src/game/actors/d_a_ch_room.cpp: player->mAreaId, then ChangeArea), so a proof row whose player was placed by a knob runs
        // the area-bound actors there (dActor_c::BeforeBehavior skips an actor whose area is not showing).
        unsigned f = 0; int to = -1;
        if (std::sscanf(ar, "%u:%i", &f, &to) == 2 && to >= 0 && to < 8) { g_area_at = f; g_area_to = to; }
    }
    if (const char *sh = std::getenv("SM64DS_COOP_MOVE_SHOT")) {
        for (const char *q = sh; *q && g_nshot < 16;) {
            char *end = nullptr;
            const unsigned long v = std::strtoul(q, &end, 10);
            if (end == q) break;
            g_shot_at[g_nshot++] = (unsigned)v;
            q = end; while (*q == ',' || *q == ' ') ++q;
        }
    }
    if (const char *d = std::getenv("SM64DS_COOP_MOVE_DUMP")) {
        const char *p = d;
        while (*p && g_ndump < 4) {
            char *end = nullptr;
            const unsigned long v = std::strtoul(p, &end, 0);
            if (end == p) break;
            g_dump_at[g_ndump++] = (unsigned)v;
            p = end;
            while (*p == ',' || *p == ';' || *p == ' ') ++p;
        }
    }
    if (const char *f = std::getenv("SM64DS_COOP_MOVE_FORGE")) {
        const char *p = f;
        while (*p && g_nforge < 12) {
            unsigned fr = 0; char nm[12] = {}; int used = 0;
            if (std::sscanf(p, "%u:%11[a-z]%n", &fr, nm, &used) < 2 || used <= 0) break;
            g_forge[g_nforge].tick = fr;
            std::memcpy(g_forge[g_nforge].name, nm, sizeof nm);
            g_forge[g_nforge].done = false;
            ++g_nforge;
            p += used;
            while (*p == ',' || *p == ';' || *p == ' ') ++p;
        }
    }
    prof_index();
    if (!g_guard) std::fprintf(stderr, "[coopmove] GUARD OFF (proof control): no class-size or value checks\n");
    if (const char *pf = std::getenv("SM64DS_COOP_MOVE_PROFILE")) if (*pf) load_profile_file(pf);
    std::fprintf(stderr, "[coopmove] phase 2 %s: %d classes%s%s%s%s\n", g_act ? "on" : "OFF (control)", g_nprof,
                 g_snap_on ? "" : ", snapshot OFF", g_hyst ? "" : ", hysteresis OFF", g_stale_on ? "" : ", stale-owner expiry OFF",
                 g_log ? ", log" : "");
}

// ---------------------------------------------------------------------------
// Raw actor access.
// ---------------------------------------------------------------------------
inline int32_t rd32(const char *a, int off) { int32_t v; std::memcpy(&v, a + off, 4); return v; }
inline int16_t rd16(const char *a, int off) { int16_t v; std::memcpy(&v, a + off, 2); return v; }
inline void wr32(char *a, int off, int32_t v) { std::memcpy(a + off, &v, 4); }
inline void wr16(char *a, int off, int16_t v) { std::memcpy(a + off, &v, 2); }

bool is_player_ptr(const void *a) {
    for (int i = 0; i < 16; ++i)
        if (data_0209f394[i] == a) return true;
    return false;
}

char *local_player() {
    const int me = (int)data_0209f250;
    if (me < 0 || me >= 16) return nullptr;
    return (char *)data_0209f394[me];
}

double dist3(const char *a, int32_t x, int32_t y, int32_t z) {
    const double dx = ((double)rd32(a, 0x5c) - (double)x) / 4096.0;
    const double dy = ((double)rd32(a, 0x60) - (double)y) / 4096.0;
    const double dz = ((double)rd32(a, 0x64) - (double)z) / 4096.0;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

// Dead, dying, being eaten or being carried: the actor's position is not its Behavior's business.
bool skip_state(const char *a, const Prof &p) {
    if (p.hold && rd32(a, p.hold) != 0) return true;
    if (*(const unsigned char *)(a + 0x0f) || *(const unsigned char *)(a + 0x0e) == 2) return true;
    const unsigned fl = (unsigned)rd32(a, 0xb0);
    if (fl & 0x60000u) return true;                       // yoshi mouth states
    // Chain Chomp breaking free (state 3, sub-states 0..9): the release only advances when its own position reaches its target.
    if (*(const unsigned short *)(a + 0x0c) == 219 && rd32(a, 0x610) == 3) return true;
    return false;
}
bool frozen_state(const char *a) {
    const unsigned fl = (unsigned)rd32(a, 0xb0);
    return (fl & 0x10000u) && (fl & 0x38u);               // Behavior suppressed while culled
}

uint32_t read_field(const char *a, const Field &f) {
    uint32_t v = 0;
    std::memcpy(&v, a + f.off, f.size);                   // little-endian host: the low bytes
    return v;
}
// Last line of defence: the live heap block of THIS actor must hold the field (the ROM's block header keeps the size three words
// before the object), whatever the table says.
bool live_holds(const char *a, const Field &f) {
    const uint32_t live = (uint32_t)rd32(a, -12);
    const bool sane = live >= 0xd0 && live <= 0x4000 && (live & 3u) == 0;
    return !sane || (unsigned)f.off + f.size <= live;
}
void write_field(char *a, const Prof &p, const Field &f, uint32_t v) {
    if (p.dead || !field_ok(f, p.size) || !value_ok(f, v)) { ++g_st.write_refused; return; }   // (guard off: only the header-range test applies)
    if (f.off >= kFieldLo && !live_holds(a, f)) {
        if (g_guard) { ++g_st.write_refused; return; }
        if (++g_st.overrun <= 8)
            std::fprintf(stderr, "[coopmove] OVERRUN (guard off) class %d writes %#x+%d past its live block %#x\n", (int)p.id, (unsigned)f.off,
                         (int)f.size, (unsigned)rd32(a, -12));
    }
    std::memcpy(a + f.off, &v, f.size);
}

int clampi(long long v, int lo, int hi) { return (int)(v < lo ? lo : v > hi ? hi : v); }

// ---------------------------------------------------------------------------
// Sending.
// ---------------------------------------------------------------------------
CoopKeyWire wire_key() {
    CoopKeyWire w; w.level = (uint16_t)g_key.level; w.act = (uint8_t)g_key.act; w.reserved = 0;
    return w;
}

void refill(unsigned now) {
    if (g_tok_t == 0) g_tok_t = now;
    const unsigned dt = now - g_tok_t;
    g_tok_t = now;
    g_tokens = (std::min)(8.0, g_tokens + dt * 0.028);      // 28 datagrams per second, burst 8
}

int fill_rec(unsigned char *out, int slot, const Slot &s, unsigned my_cd, bool frozen) {
    const Prof &p = g_prof[s.prof];
    const char *a = s.a;
    MoverRec r; std::memset(&r, 0, sizeof r);
    r.slot = (uint16_t)slot;
    r.cls = (uint16_t)p.id;
    r.cd = (uint16_t)(my_cd > 65535u ? 65535u : my_cd);
    r.x = rd32(a, 0x5c); r.y = rd32(a, 0x60); r.z = rd32(a, 0x64);
    r.ax = rd16(a, 0x8c); r.ay = rd16(a, 0x8e); r.az = rd16(a, 0x90);
    r.rflags = frozen ? 1 : 0;
    r.nf = p.nf;
    std::memcpy(out, &r, sizeof r);
    for (int k = 0; k < p.nf; ++k) {
        const uint32_t v = read_field(a, p.f[k]);
        std::memcpy(out + sizeof r + 4 * k, &v, 4);
    }
    return (int)sizeof r + 4 * p.nf;
}

int rec_bytes(int prof) { return (int)sizeof(MoverRec) + 4 * g_prof[prof].nf; }

// ---------------------------------------------------------------------------
// Receiving.
// ---------------------------------------------------------------------------
bool prof_known(unsigned cls, int nf) {
    return cls < 1024 && g_prof_of[cls] >= 0 && g_prof[g_prof_of[cls]].nf == nf;
}

void on_world(int seat, int sub, const unsigned char *p, int len, void *);
void on_ctl(int seat, int sub, const unsigned char *p, int len, void *);

void ensure_registered() {
    if (g_regd) return;
    g_regd = true;
    g_my = comms_hub_my_seat();
    coop_world_register(kWorldMoverBatch, 0x1f, on_world);
    coop_ctl_register(kCtlMoverSnapReq, 0x0b, on_ctl);
}

bool member_ok(int seat) {
    return seat >= 0 && seat < kSeats && seat != g_my && ((coop_group_member_mask() >> seat) & 1u);
}

void on_world(int seat, int sub, const unsigned char *p, int len, void *) {
    if (!g_act) { ++g_st.drop_off; return; }
    if (seat < 0 || seat >= kSeats || seat == g_my) { ++g_st.drop_seat; return; }
    if (sub != kWorldMoverBatch) { ++g_st.drop_bad; return; }
    if (!coop_mover_batch_validate(p, len)) { ++g_st.drop_bad; return; }
    MoverBatchHead h; std::memcpy(&h, p, sizeof h);
    if (h.pver != kMoverProfileVer) { ++g_st.drop_ver; return; }
    ++g_st.batches_rx;
    if (!g_key.valid() || h.key.level != (uint16_t)g_key.level || h.key.act != (uint8_t)g_key.act) { ++g_st.drop_group; return; }
    if (!member_ok(seat)) { ++g_st.drop_member; return; }
    if ((h.flags & 1) && !g_snap_on) return;
    const unsigned now = comms_hub_now_ms();
    int off = (int)sizeof h;
    for (int i = 0; i < (int)h.count; ++i) {
        MoverRec r; std::memcpy(&r, p + off, sizeof r);
        Rx &x = g_rx[r.slot][seat];
        if (x.valid && !(h.flags & 1) && !x.snap) { x.pvalid = true; x.px = x.x; x.py = x.y; x.pz = x.z; x.pax = x.ax; x.pay = x.ay; x.paz = x.az; x.pt = x.t; x.pf0 = x.f[0]; } else x.pvalid = false;
        x.valid = true; x.fresh = true; x.snap = (h.flags & 1) != 0; x.t = now;
        x.cls = r.cls; x.cd = r.cd; x.x = r.x; x.y = r.y; x.z = r.z;
        x.ax = r.ax; x.ay = r.ay; x.az = r.az; x.rflags = r.rflags; x.nf = r.nf;
        std::memcpy(x.f, p + off + sizeof r, 4u * r.nf);
        off += (int)sizeof r + 4 * r.nf;
        ++g_st.recs_rx;
        g_slot[r.slot].heard = true;
    }
}

void on_ctl(int seat, int sub, const unsigned char *p, int len, void *) {
    if (!g_act) { ++g_st.drop_off; return; }
    if (seat < 0 || seat >= kSeats || seat == g_my) { ++g_st.drop_seat; return; }
    if (sub != kCtlMoverSnapReq || !coop_mover_req_validate(p, len)) { ++g_st.drop_bad; return; }
    MoverSnapReq q; std::memcpy(&q, p, sizeof q);
    if (!g_snap_on) return;
    if (!g_key.valid() || q.key.level != (uint16_t)g_key.level || q.key.act != (uint8_t)g_key.act) { ++g_st.drop_group; return; }
    if (!member_ok(seat)) { ++g_st.drop_member; return; }
    if (!coop_group_established()) return;
    Job &j = g_job[seat];
    j.on = true; j.passes = 2; j.next = 0; j.cursor = 0;
    ++g_st.req_served;
}

// ---------------------------------------------------------------------------
// Ownership: the member closest to an actor publishes it (PLAN 1.3). Pure.
// ---------------------------------------------------------------------------
int decide_owner(int cur, int me, unsigned my_cd, bool cur_fresh, unsigned cur_cd, int best, unsigned best_cd, bool hyst) {
    const double F = hyst ? 1.75 : 1.0;
    if (cur >= 0 && cur != me && !cur_fresh) cur = -1;           // the owner went silent
    if (best < 0 && (cur < 0 || cur == me)) return me;           // nobody else claims it
    if (cur == me) {
        if (best >= 0) {
            if (best_cd * F < (double)my_cd) return best;
            if (!(my_cd * F < (double)best_cd) && best < me) return best;   // a tie: the lower seat
        }
        return me;
    }
    if (cur >= 0) {                                              // a fresh remote owner
        if (my_cd * F < (double)cur_cd) return me;
        if (best >= 0 && best != cur && best_cd * F < (double)cur_cd) return best;
        return cur;
    }
    // no owner yet, a remote claim exists
    if (my_cd * F < (double)best_cd) return me;
    if (best_cd * F < (double)my_cd) return best;
    return best < me ? best : me;
}

// ---------------------------------------------------------------------------
// Applying one record.
// ---------------------------------------------------------------------------
int ang_err(int16_t a, int16_t b) {
    int d = (int)(uint16_t)(a - b);
    if (d > 0x8000) d = 0x10000 - d;
    return d;
}

void ease_angle(char *a, int off, int16_t cur, int16_t want) {
    const int e = ang_err(cur, want);
    if (e > 0x2000) wr16(a, off, want);
    else if (e > 0x400) wr16(a, off, (int16_t)(cur + (int16_t)(want - cur) / 2));
}

// Run after093, lane COOPPLAT1. daObjSimpleLift_c (src/game/actors/d_a_obj_simple_lift.cpp, every sliding slab: classes 55 124
// 138 146 147 154 155) slides along mPrevAngleY (+0x94, the heading dActor_c::UpdatePosWithHorzSpeedAndAng reads,
// src/runtime/actor/d_actor.cpp:628) and Behavior turns that heading by 0x8000 each time mMoveTimer runs out. The record carries
// the owner's pose and its two timers (+0x31c / +0x320) but not the heading, and a copy that spawned later has turned a different
// number of times, so half the time the owner's timers land on a copy that slides the other way. The pin hides it while the owner
// streams; once the owner leaves the copy runs free one whole leg off its track (a BFS square half inside the walkway).
// The heading is not on the wire, but the record fixes it: the slab is on its way out (heading H0, InitResources:180) when its
// distance from the spawn point is the travel length minus what the timer still has to run, and on its way back when that distance
// is what the timer still has to run. The owner's record gives both numbers; the copy takes the matching heading.
extern "C" { extern unsigned short data_ov091_02134504[]; extern unsigned short data_ov091_02134514[]; }
bool is_simple_lift(unsigned id) {
    return id == 55 || id == 124 || id == 138 || id == 146 || id == 147 || id == 154 || id == 155;
}
void fix_lift_heading(int slot, char *a, const Prof &p, const Rx &x) {
    if (!g_lift_heading || !is_simple_lift(p.id) || p.nf < 2) return;
    const unsigned variant = (unsigned)(uint8_t)a[0x322];
    if (variant > 6) return;
    const int speed = rd32(a, 0x98) >> 12;                                  // mHorzSpeed, 0xa000 = 10 units a frame (InitResources:185)
    const int travel = (int)data_ov091_02134504[variant];
    if (speed <= 0 || travel <= 0 || travel > 1000) return;
    const int16_t angx = rd16(a, 0x8c), angy = rd16(a, 0x8e);
    const int16_t h0 = (int16_t)(angx != 0 ? angy + angx : angy + (int16_t)data_ov091_02134514[variant]);   // InitResources:180-182
    const double rad = (double)(uint16_t)h0 * (6.283185307179586 / 65536.0);
    const double dx = std::sin(rad), dz = std::cos(rad);                     // X with the sine, Z with the cosine (the ROM table pair)
    const double ox = (double)(x.x - rd32(a, 0x324)) / 4096.0, oz = (double)(x.z - rd32(a, 0x32c)) / 4096.0;   // from mBasePos
    const double along = ox * dx + oz * dz;
    const int timer = (int)(int16_t)(x.f[1] & 0xffffu);                       // the owner's mMoveTimer (+0x320, low half)
    const double rest = (double)timer * speed, len = (double)travel * speed;
    const double e_out = std::fabs(along - (len - rest)), e_back = std::fabs(along - rest);
    if (std::fabs(e_out - e_back) < 2.0 * speed) return;                    // the middle of a leg: both fit, keep what we have
    const int16_t want = (int16_t)(e_out < e_back ? h0 : h0 + (int16_t)0x8000);
    const int16_t cur = rd16(a, 0x94);
    if (cur == want) return;
    wr16(a, 0x94, want);
    ++g_lift_flips;
    if (g_lift_flips <= 40)
        std::fprintf(stderr, "[coopmove] lift heading: class %d slot %d had %04x, the owner's record (along %.0f of %.0f, timer %d) says %04x\n",
                     (int)p.id, slot, (unsigned)(uint16_t)cur, along, len, timer, (unsigned)(uint16_t)want);
}

// Run coopfix1, lane DIAMOND2. The BitDW diamond lift (135, daObjKm1_Ukishima_c, src/game/actors/d_a_obj_km1_ukishima.cpp)
// drives along mPrevAngleY (+0x94) at 10 units a frame and adds a quarter turn each time its 60-frame mTurnTimer (+0x31e) runs
// out, so it traces a 600-unit square. The record carries its pose and the timer but not the heading, and the heading lies below
// the field window. A copy that spawned later, or took a timer write across a turn, faces another side of the square: the pin
// hides it while the owner streams, but the copy carries its rider the wrong way, and once its console takes the platform over
// the room follows that copy off its square. Each handover can turn the square again, so the lift wanders off.
// The owner's own move says the heading: between two stream records on one leg (the timer went down, no reset) it moved along
// one of the four headings mAngleY + k * 0x4000 (InitResources sets mPrevAngleY = mAngleY). The copy takes the nearest one,
// using the ROM's own table pair the way dActor_c::UpdatePosWithHorzSpeedAndAng does (velocity x from [j], z from [j + 1]).
extern "C" { extern short data_02082214[]; }
bool dia_solve_heading(const Slot &s, const char *a, const Rx &x, int16_t *want);
void fix_diamond_heading(int slot, Slot &s, char *a, const Prof &p, const Rx &x) {
    if (!g_dia_heading || p.id != 135 || p.nf < 1) return;
    auto take = [&](int16_t want, const char *how, int pt, int t, double units) {
        const int16_t cur = rd16(a, 0x94);
        s.hconf = true;
        if (cur == want) { ++g_dia_ok; return; }
        wr16(a, 0x94, want);
        ++g_dia_flips;
        if (g_dia_flips <= 40)
            std::fprintf(stderr, "[coopmove] diamond heading: slot %d had %04x, the owner's %s (timer %d to %d, %.0f units) says %04x wall=%lld\n",
                         slot, (unsigned)(uint16_t)cur, how, pt, t, units, (unsigned)(uint16_t)want, wall_ms());
    };
    // With the spawn point known (derive_base), the record's place and timer name the side of the square outright: the one
    // whose cycle place for that timer is where the owner says the lift is. No second record and no leg rule needed, so a
    // timer write that lands across a turn is matched with its own heading in the same breath.
    {
        int16_t want = 0;
        if (dia_solve_heading(s, a, x, &want)) { take(want, "place", (int)(x.f[0] & 0xffu), (int)(x.f[0] & 0xffu), 0.0); return; }
    }
    if (x.snap || !x.pvalid) return;
    const unsigned dtm = x.t - x.pt;
    if (dtm < 20u || dtm > 1000u) return;
    const int t = (int)(x.f[0] & 0xffu), pt = (int)(x.pf0 & 0xffu);
    if (!(pt > t && pt <= 0x3c)) return;                                   // a turn between the two records, or not this row's timer
    const double dx = (double)(x.x - x.px), dz = (double)(x.z - x.pz);
    const double mag = std::sqrt(dx * dx + dz * dz);
    if (mag < 4.0 * 4096.0) return;                                         // too short a move to read
    int best = -1; double bdot = -1e300;
    for (int k = 0; k < 4; ++k) {
        const unsigned h = (uint16_t)(x.ay + k * 0x4000);
        const int j = (int)((h >> 4) << 1);
        const double dot = dx * (double)data_02082214[j] + dz * (double)data_02082214[j + 1];
        if (dot > bdot) { bdot = dot; best = k; }
    }
    if (best < 0 || bdot < 0.94 * mag * 4096.0) return;                     // not along one heading (about 20 degrees): leave it
    take((int16_t)(uint16_t)(x.ay + best * 0x4000), "move", pt, t, mag / 4096.0);
}

// Run coopfix1, lane DIAMOND2. THE CYCLE MOVERS' PLACE IS A FUNCTION OF THEIR CYCLE STATE, so a copy never keeps a stray offset.
// These movers integrate: each frame adds a step to the place, the step chosen by a counter, so the cartridge's place is always
// spawn point + (sum of the steps the counter has been through):
//   135 diamond lift (d_a_obj_km1_ukishima.cpp): 10 units a frame along mPrevAngleY, a quarter turn when mTurnTimer runs out;
//   142 pole lift (d_a_obj_km2_ami_bou.cpp): y += 7 * sin(mHeightAng) (or -= 3 * sin), then mHeightAng += 0x100;
//   75 / 76 / 145 floating floors (d_a_obj_ukiyuka.cpp): mBobPhase += 0x100, y -= amp * sin(mBobPhase), rest 60 frames at mRestY.
// Each cycle sums to exactly zero (checked against the sine table data_02082214), which is what brings the cartridge's platforms
// back to the same place forever. A coop copy breaks that: the pin writes the owner's place extrapolated along its velocity (a few
// units off on a curve or at a corner), and a copy that runs free afterwards (owner stale, area left, handover, a death and a
// re-entry) keeps that offset and adds the next one, so the platforms creep: the floors sink under the lava, the poles open a gap
// at the chain, the diamond lifts leave their squares. Here every console (the owner too) puts each such mover at spawn point +
// the sum for its counter every frame. The counter is synced (phase fields); the spawn point is the copy's own, taken the first
// tick it is seen, before any record touches it (the floors keep theirs in mRestY).
int sin_at(unsigned h) { return data_02082214[((h & 0xffffu) >> 4) << 1]; }
int cos_at(unsigned h) { return data_02082214[(((h & 0xffffu) >> 4) << 1) + 1]; }
int rnd12(long long v) { return (int)((v + 0x800) >> 12); }
bool is_bobber(unsigned id) { return id == 75 || id == 76 || id == 145; }
bool is_derived(unsigned id) { return id == 135 || id == 142 || is_bobber(id); }
// 142: how far the pole has moved from its spawn height at mHeightAng (Behavior uses the angle, then adds 0x100)
bool pole_rise(const char *a, int *out) {
    const unsigned ph = (uint16_t)rd16(a, 0x354);
    if (ph & 0xffu) return false;
    const bool up = (uint32_t)rd32(a, 0x08) != 0xffffu;                     // param1 != 0xffff: lowered 300 and rising by 7
    const int amp = up ? 0x7000 : 0x3000;
    long long sum = 0;
    for (unsigned p = 0; p != ph; p = (p + 0x100) & 0xffffu) sum += rnd12((long long)amp * sin_at(p));
    *out = (int)(up ? sum : -sum);
    return true;
}
// 75 / 76 / 145: the floor's height at mBobPhase (Behavior adds 0x100, then subtracts)
bool bob_y(const char *a, int *out) {
    const unsigned ph = (uint16_t)rd16(a, 0x328);
    const int amp = rd32(a, 0x324);
    if ((ph & 0xffu) || amp <= 0 || amp > 0x10000) return false;
    long long sum = 0;
    for (unsigned p = ph; p != 0; p = (p - 0x100) & 0xffffu) sum += rnd12((long long)amp * sin_at(p));
    *out = rd32(a, 0x320) - (int)sum;
    return true;
}
// 135: the lift's x / z offset from its spawn point for (quarter turns since mAngleY, mTurnTimer). The first leg is one step short
// (InitResources arms 60 and Behavior counts before it moves), so corner 0 sits one step behind the spawn point.
void dia_off_k(unsigned h0, int k, int t, int sp, int *ox, int *oz) {
    long long x = 0, z = 0;
    auto step = [&](int q, int n) {
        const unsigned hq = (h0 + (unsigned)q * 0x4000u) & 0xffffu;            // velocity x from [j], z from [j + 1] (UpdatePosWithHorzSpeedAndAng)
        x += (long long)n * rnd12((long long)sp * sin_at(hq));
        z += (long long)n * rnd12((long long)sp * cos_at(hq));
    };
    step(0, -1);
    for (int q = 0; q < k; ++q) step(q, 60);
    step(k, 61 - t);
    *ox = (int)x; *oz = (int)z;
}
bool dia_off(const char *a, int *ox, int *oz) {
    const unsigned h0 = (uint16_t)rd16(a, 0x8e), dk = ((unsigned)(uint16_t)rd16(a, 0x94) - h0) & 0xffffu;
    const int t = *(const unsigned char *)(a + 0x31e), sp = rd32(a, 0x98);
    if ((dk & 0x3fffu) || t < 1 || t > 60 || sp <= 0 || sp > 0x40000) return false;
    dia_off_k(h0, (int)(dk >> 14), t, sp, ox, oz);
    return true;
}
bool dia_solve_heading(const Slot &s, const char *a, const Rx &x, int16_t *want) {
    if (!g_derive || !s.bvalid) return false;
    const unsigned h0 = (uint16_t)rd16(a, 0x8e);
    const int t = (int)(x.f[0] & 0xffu), sp = rd32(a, 0x98);
    if (t < 1 || t > 60 || sp <= 0 || sp > 0x40000) return false;
    int best = -1; double bd = 1e300;
    for (int k = 0; k < 4; ++k) {
        int ox, oz;
        dia_off_k(h0, k, t, sp, &ox, &oz);
        const double ex = ((double)x.x - s.bx - ox) / 4096.0, ez = ((double)x.z - s.bz - oz) / 4096.0;
        const double e = std::sqrt(ex * ex + ez * ez);
        if (e < bd) { bd = e; best = k; }
    }
    if (best < 0 || bd > 40.0) return false;
    *want = (int16_t)(uint16_t)(h0 + (unsigned)best * 0x4000u);
    return true;
}
void derive_base(Slot &s) {
    const Prof &p = g_prof[s.prof];
    const char *a = s.a;
    if (s.bvalid || !is_derived(p.id)) return;
    if (p.id == 135) {
        int ox, oz;
        if (!dia_off(a, &ox, &oz)) return;
        s.bx = rd32(a, 0x5c) - ox; s.bz = rd32(a, 0x64) - oz; s.by = 0;
    } else if (p.id == 142) {
        int r;
        if (!pole_rise(a, &r)) return;
        s.by = rd32(a, 0x60) - r; s.bx = s.bz = 0;
    }
    s.bvalid = true;
}
void derive_apply(int slot, Slot &s, unsigned now) {
    if (!g_derive || !s.bvalid) return;
    const Prof &p = g_prof[s.prof];
    char *a = s.a;
    // A busy owner sends these counters every period. When its records stop while the owner is still fresh, its copy is
    // asleep (out of its camera's range, frozen_state: it does not publish) or stopped, and its counters are standing still.
    // My own copy may be awake and would count on alone, so it waits on the owner's last counters instead (and, for the
    // diamond lift, the heading that goes with them). Past kStaleMs the owner is gone and the ownership moves as before.
    if (s.owner >= 0 && s.owner != g_my && p.nf > 0) {
        const Rx &x = g_rx[slot][s.owner];
        const unsigned age = now - x.t, hold = (std::max)(3u * (unsigned)p.period, 120u);
        if (x.valid && x.cls == p.id && x.nf == p.nf && age > hold && age <= (unsigned)kStaleMs) {
            for (int k = 0; k < p.nf; ++k) write_field(a, p, p.f[k], x.f[k]);
            if (p.id == 135) { int16_t want; if (dia_solve_heading(s, a, x, &want)) wr16(a, 0x94, want); }
            ++g_der_hold;
        }
    }
    int wx = rd32(a, 0x5c), wy = rd32(a, 0x60), wz = rd32(a, 0x64);
    if (p.id == 135) {
        int ox, oz;
        if (!dia_off(a, &ox, &oz)) return;
        wx = s.bx + ox; wz = s.bz + oz;
    } else if (p.id == 142) {
        int r;
        if (!pole_rise(a, &r)) return;
        wy = s.by + r;
    } else if (is_bobber(p.id)) {
        if (!bob_y(a, &wy)) return;
    } else return;
    const double d = dist3(a, wx, wy, wz);
    if (d == 0.0) return;
    if (d > 3000.0) {                                    // not the same cycle at all: leave it and say so
        if (++g_der_refused <= 8)
            std::fprintf(stderr, "[coopmove] derive: class %d slot %d is %.0f units from its cycle place, left alone wall=%lld\n",
                         (int)p.id, slot, d, wall_ms());
        return;
    }
    wr32(a, 0x5c, wx); wr32(a, 0x60, wy); wr32(a, 0x64, wz);
    ++g_der_fix;
    if (d > g_der_max) g_der_max = d;
    if (d >= 4.0 && ++g_der_logs <= 40)
        std::fprintf(stderr, "[coopmove] derive: class %d slot %d put back %.1f units on its cycle (owner %d) wall=%lld\n",
                     (int)p.id, slot, d, s.owner, wall_ms());
}

void apply_rec(int slot, Slot &s, Rx &x) {
    const Prof &p = g_prof[s.prof];
    char *a = s.a;
    if (x.cls != p.id || x.nf != p.nf) { ++g_st.cls_mismatch; return; }
    if (s.pin_lf == kPinStay) s.pin_lf = 0;   // TONGUE1: an owner's record replaces the parked copy
    if (skip_state(a, p)) { ++g_st.skipped_state; return; }
    const bool force = x.snap;
    const bool phase = (p.flags & kFPhase) && p.nf > 0;
    // aim a little ahead of the owner's last pose along its last velocity (a chaser is 2 to 6 frames old on arrival)
    int32_t tx = x.x, ty = x.y, tz = x.z;
    if (!force && !phase && x.pvalid && g_lead_ms > 0) {
        const int dt = (int)(x.t - x.pt);
        if (dt >= 30 && dt <= 400) {
            const long long lim = 80LL * 4096;
            tx += (int32_t)(std::max)(-lim, (std::min)(lim, (long long)(x.x - x.px) * g_lead_ms / dt));
            ty += (int32_t)(std::max)(-lim, (std::min)(lim, (long long)(x.y - x.py) * g_lead_ms / dt));
            tz += (int32_t)(std::max)(-lim, (std::min)(lim, (long long)(x.z - x.pz) * g_lead_ms / dt));
        }
    }
    const double err = dist3(a, tx, ty, tz);
    if (!force && err > 6000.0) { ++g_st.drop_far; return; }
    if (!force && (p.flags & kFSlave) && p.nf == 0) { ++g_st.in_band; return; }   // pinned every frame by slave()
    if (force && err > 20000.0) { ++g_st.drop_far; return; }
    const int16_t cax = rd16(a, 0x8c), cay = rd16(a, 0x8e), caz = rd16(a, 0x90);
    const int aerr = (std::max)({ ang_err(cax, x.ax), ang_err(cay, x.ay), ang_err(caz, x.az) });
    if (!force && !phase && err <= p.band && aerr <= 0x400) { ++g_st.in_band; return; }
    const bool write = force || phase || err > p.snap;
    if (!force && phase && (p.flags & kFSlave)) {            // slave() owns the pose; the counters are written here
        if (g_lift_heading && is_simple_lift(p.id)) {        // COOPPLAT1: the slab's place, timers and heading are one state: write them together
            const double gap = dist3(a, x.x, x.y, x.z);
            CStat &c = g_cs[s.prof];
            ++c.pin_n; c.pin_sum += gap; if (gap > c.pin_max) c.pin_max = gap; if (gap > 40.0) ++c.pin_big;
            wr32(a, 0x5c, clampi(x.x, -kMoverMaxUnits * 4096, kMoverMaxUnits * 4096));
            wr32(a, 0x60, clampi(x.y, -kMoverMaxUnits * 4096, kMoverMaxUnits * 4096));
            wr32(a, 0x64, clampi(x.z, -kMoverMaxUnits * 4096, kMoverMaxUnits * 4096));
            wr16(a, 0x8c, x.ax); wr16(a, 0x8e, x.ay); wr16(a, 0x90, x.az);
        }
        for (int k = 0; k < p.nf; ++k) write_field(a, p, p.f[k], x.f[k]);
        fix_lift_heading(slot, a, p, x);
        fix_diamond_heading(slot, s, a, p, x);
        ++g_st.applied_write; ++g_cs[s.prof].applied;
        s.lvalid = false;
        return;
    }
    if (write) {
        wr32(a, 0x5c, clampi(tx, -kMoverMaxUnits * 4096, kMoverMaxUnits * 4096));
        wr32(a, 0x60, clampi(ty, -kMoverMaxUnits * 4096, kMoverMaxUnits * 4096));
        wr32(a, 0x64, clampi(tz, -kMoverMaxUnits * 4096, kMoverMaxUnits * 4096));
        wr16(a, 0x8c, x.ax); wr16(a, 0x8e, x.ay); wr16(a, 0x90, x.az);
        for (int k = 0; k < p.nf; ++k) write_field(a, p, p.f[k], x.f[k]);
        fix_lift_heading(slot, a, p, x);
        ++g_st.applied_write; ++g_cs[s.prof].applied;
        if (force) ++g_st.snap_applied;
    } else {
        if (err > p.band) {      // inside the snap radius: close half of the gap per record
            wr32(a, 0x5c, rd32(a, 0x5c) + (int32_t)(((long long)tx - rd32(a, 0x5c)) / 2));
            wr32(a, 0x60, rd32(a, 0x60) + (int32_t)(((long long)ty - rd32(a, 0x60)) / 2));
            wr32(a, 0x64, rd32(a, 0x64) + (int32_t)(((long long)tz - rd32(a, 0x64)) / 2));
        }
        ease_angle(a, 0x8c, cax, x.ax); ease_angle(a, 0x8e, cay, x.ay); ease_angle(a, 0x90, caz, x.az);
        ++g_st.applied_ease; ++g_cs[s.prof].applied;
    }
    s.lvalid = false;           // the actor was moved under the publisher's cache
    if (g_log && (g_st.applied_write + g_st.applied_ease) <= 40)
        std::fprintf(stderr, "[coopmove] apply slot=%d cls=%d err=%.1f %s%s wall=%lld\n", slot, (int)p.id, err,
                     write ? "write" : "ease", force ? " snap" : "", wall_ms());
}

// ---------------------------------------------------------------------------
// The per-tick work.
// ---------------------------------------------------------------------------
void reset_level() {
    std::memset(g_slot, 0, sizeof g_slot);
    for (int i = 0; i < kSlots; ++i) { g_slot[i].owner = -1; g_slot[i].prof = -1; }
    std::memset(g_rx, 0, sizeof g_rx);
    std::memset(g_job, 0, sizeof g_job);
    g_god_set = false;
    g_lf = 0; g_spawned = false; g_spawn_lf = 0; g_ents = 0;
    g_req_done = false; g_req_tries = 0; g_req_last = 0; g_req_nonce = 0; g_snap_base = g_st.snap_applied; g_hold = g_snap_on;
    g_area_done = false;
    for (int i = 0; i < g_nforge; ++i) g_forge[i].done = false;
}

// The first live actor of a class: its heap block says how big the class really is. A block smaller than the table (or smaller
// than a field needs) switches the row off; a block that disagrees is logged for the size audit.
void check_live(Prof &p, const char *a) {
    if (p.live_seen) return;
    p.live_seen = true;
    const uint32_t live = (uint32_t)rd32(a, -12);
    const bool sane = live >= 0xd0 && live <= 0x4000 && (live & 3u) == 0;
    if (g_log) std::fprintf(stderr, "[coopmove] size cls=%d table=%#x live=%#x%s\n", (int)p.id, (unsigned)p.size, (unsigned)live, sane ? "" : " (unreadable)");
    if (!sane) return;
    if (p.size != 0 && live != p.size) { ++g_st.size_mismatch; std::fprintf(stderr, "[coopmove] size guard: class %d table %#x but live block %#x\n", (int)p.id, (unsigned)p.size, (unsigned)live); }
    unsigned use = p.size ? (live < p.size ? live : p.size) : live;
    bool ok = true;
    for (int k = 0; k < p.nf; ++k) ok = ok && field_ok(p.f[k], use);
    if (p.hold && p.hold + 4u > use) ok = false;
    if (!ok) {
        p.dead = true; ++g_st.rows_dropped;
        if (p.id < 1024) g_prof_of[p.id] = -1;
        std::fprintf(stderr, "[coopmove] size guard: class %d row switched off (live block %#x)\n", (int)p.id, (unsigned)live);
    }
}

void scan_ents() {
    for (int i = 0; i < kSlots; ++i) { g_slot[i].a = nullptr; }
    unsigned n = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        const unsigned id = *(const unsigned short *)(a + 0x0c);
        if (id >= 1024 || g_prof_of[id] < 0) continue;
        check_live(g_prof[g_prof_of[id]], a);
        if (g_prof_of[id] < 0) continue;
        short slot = rd16(a, 0xce);
        if (slot < 0 && id == 215) slot = (short)coop_bully_virtual_slot(a);   // coopbully1: the Big Bully's three runtime bullies
        if (slot < 0 || slot >= kSlots) continue;
        if (is_player_ptr(a)) continue;
        Slot &s = g_slot[slot];
        if (s.a) continue;                                    // two actors in one slot: the first wins
        if (s.prof != g_prof_of[id]) {                        // a different class (or first sight): start clean
            s.prof = g_prof_of[id]; s.seen = 0; s.lvalid = false; s.owner = -1; s.last_pub = 0; s.pin_lf = 0; s.hconf = false; s.bvalid = false;
        }
        s.a = a;
        ++s.seen;
        derive_base(s);                                       // COOPDIAMOND2: first sight, before any record of this tick is applied
        ++n;
    }
    for (int i = 0; i < kSlots; ++i) if (!g_slot[i].a) g_slot[i].seen = 0;
    g_ents = n;
    g_st.ents = n;
    {
        unsigned per[256] = {};
        for (int i = 0; i < kSlots; ++i) if (g_slot[i].a && g_slot[i].prof >= 0) ++per[g_slot[i].prof];
        for (int i = 0; i < g_nprof; ++i) if (per[i] > g_cs[i].ents_max) g_cs[i].ents_max = per[i];
    }
}

bool alive_rx(const Rx &x, unsigned now) {
    return x.valid && (!g_stale_on || (unsigned)(now - x.t) <= (unsigned)kStaleMs);
}

void send_batch(bool snap, const unsigned char *recs, int nrecs, int bytes) {
    unsigned char buf[kPayloadMax];
    MoverBatchHead h; std::memset(&h, 0, sizeof h);
    h.key = wire_key(); h.flags = snap ? 1 : 0; h.count = (uint8_t)nrecs; h.pver = kMoverProfileVer;
    std::memcpy(buf, &h, sizeof h);
    std::memcpy(buf + sizeof h, recs, (size_t)bytes);
    if (coop_send_world(kWorldMoverBatch, buf, (int)sizeof h + bytes)) {
        if (snap) { ++g_st.snap_batches_sent; g_st.snap_recs_sent += (unsigned)nrecs; }
        else { ++g_st.batches_sent; g_st.recs_sent += (unsigned)nrecs; }
    }
}

// The non-owner pins a slaved actor to the owner's pose every frame: the owner's last record plus its last velocity
// times (age of the record + the lead). The actor's own Behavior still runs (collision, carrying, sounds, animation);
// this only overwrites where it ended up.
int16_t ang_extrap(int16_t cur, int16_t prev, long long lead, int dt) {
    const int d = (int)(int16_t)(cur - prev);
    long long v = (long long)d * lead / dt;
    if (v > 0x2000) v = 0x2000; else if (v < -0x2000) v = -0x2000;
    return (int16_t)(cur + (int16_t)v);
}
void slave(Slot &s, const Rx &x, unsigned now) {
    const Prof &p = g_prof[s.prof];
    if (!x.valid || x.cls != p.id || x.nf != p.nf) return;
    const unsigned age = now - x.t;
    if (x.snap && age > 120u) return;                        // a snapshot pose is a starting point, not a live stream: the local copy, now in phase, runs on from it
    // run after093, lane COOPPLAT1: a sliding slab takes the owner's whole state at each record (apply_rec) and then runs on in phase, so it
    // is never pinned: a pin to an extrapolated or stale pose would put it ahead of, or hold it against, its own timers, and it would
    // keep that offset for the rest of the visit once the owner leaves
    if (g_lift_heading && is_simple_lift(p.id)) return;
    if (age > (unsigned)kStaleMs) return;                    // the owner went quiet for good: let the local copy run (a still actor heartbeats every 1.5 s)
    char *a = s.a;
    if (skip_state(a, p)) return;
    if (p.id == 187 && rd32(a, 0x45c) != 0) ++g_cs[s.prof].held_pins;     // proof: the pin fought a Player that is carrying the rabbit
    int32_t tx = x.x, ty = x.y, tz = x.z;
    int16_t tax = x.ax, tay = x.ay, taz = x.az;
    const long long lead = (long long)(std::min)(age, 150u) + g_lead_ms;   // extrapolate at most 150 ms past the last record
    const int dt = x.pvalid ? (int)(x.t - x.pt) : 0;
    if (x.pvalid && dt >= 30 && dt <= 400) {
        const long long lim = 100LL * 4096;
        tx += (int32_t)(std::max)(-lim, (std::min)(lim, (long long)(x.x - x.px) * lead / dt));
        ty += (int32_t)(std::max)(-lim, (std::min)(lim, (long long)(x.y - x.py) * lead / dt));
        tz += (int32_t)(std::max)(-lim, (std::min)(lim, (long long)(x.z - x.pz) * lead / dt));
        tax = ang_extrap(x.ax, x.pax, lead, dt); tay = ang_extrap(x.ay, x.pay, lead, dt); taz = ang_extrap(x.az, x.paz, lead, dt);
    }
    const double pin = dist3(a, tx, ty, tz);
    if (pin > 6000.0) return;
    {
        CStat &c = g_cs[s.prof];            // how far the pin had to move the local copy: the per-class desync number
        ++c.pin_n; c.pin_sum += pin; if (pin > c.pin_max) c.pin_max = pin; if (pin > 40.0) ++c.pin_big;
    }
    wr32(a, 0x5c, clampi(tx, -kMoverMaxUnits * 4096, kMoverMaxUnits * 4096));
    wr32(a, 0x60, clampi(ty, -kMoverMaxUnits * 4096, kMoverMaxUnits * 4096));
    wr32(a, 0x64, clampi(tz, -kMoverMaxUnits * 4096, kMoverMaxUnits * 4096));
    wr16(a, 0x8c, tax); wr16(a, 0x8e, tay); wr16(a, 0x90, taz);
    s.lvalid = false;
    if (s.pin_lf == kPinStay) s.pin_lf = 0;   // TONGUE1: an owner's pose replaced the parked copy
    ++g_st.slaved;
    ++g_cs[s.prof].slaved;
}

// The claim distance this console uses and sends for a slot: how far its player is from the actor.
// COOPROWS1: the rider owns a seesaw. daObjSeesaw_c (39, 128, 133, 143, 149, 150) sets mPoundedThisFrame (+0x326) in its stood-on
// callback and clears it as the last statement of Behavior (src/game/actors/d_a_obj_seesaw.cpp), so it is set here only when a body
// of THIS console stood on the plank this frame (a friend's body is parked away between draws). Nearest-to-the-pivot handed the plank
// to a friend standing by the axle, whose pin then erased the rider's own tilt. SM64DS_COOP_SEESAW_RIDER=0 is the control.
// COOPPUZZLE1: a dead or dying body claims nothing. My own player bouncing in lava (St_BurnLava), dying (St_DeadHit,
// St_DeadPit) sends the farthest claim, so a friend takes the object outright and keeps it. Without it his claim
// swung across the 1.75x band with every lava bounce and the owner flipped each dwell, both consoles pinned to the other's old
// record and the plank froze (BFS 143, COOPROWS2 kba2). Airborne or falling is NOT dying: a rider hopping keeps the plank.
bool local_body_dying(const char *pl) {
    const void *st = *(const void *const *)(pl + 0x370);  // Player::mState
    return st == data_ov002_021100f4 || st == data_ov002_0211010c || st == data_ov002_02110124;
}
unsigned claim_cd(const char *pl, const Slot &s) {
    const Prof &p = g_prof[s.prof];
    if (g_dead_guard && local_body_dying(pl)) return 65535;
    if (g_rider_own && (p.id == 39 || p.id == 128 || p.id == 133 || p.id == 143 || p.id == 149 || p.id == 150) && p.size >= 0x327 &&
        *(const unsigned char *)(s.a + 0x326) != 0)
        return 0;
    // COOPRIDE1: the same rule for the two BFS movers that move only under a rider. The elevator (140) sets mStoodOn +0x326 and
    // the tilting platform (141) mBumped +0x34c from their mesh callbacks; each Behavior reads and clears its flag.
    if (g_rider_own && ((p.id == 140 && p.size >= 0x327 && *(const unsigned char *)(s.a + 0x326) != 0) ||
                        (p.id == 141 && p.size >= 0x34d && *(const unsigned char *)(s.a + 0x34c) != 0)))
        return 0;
    return (unsigned)(std::min)(65535.0, dist3(pl, rd32(s.a, 0x5c), rd32(s.a, 0x60), rd32(s.a, 0x64)));
}

// COOPDIAMOND2: linked BFS movers. The three pole lifts at the yellow chain (142, rising, spawn y 3500) and the two
// floating floors at y 3400 (145) spawn on the same tick and run in step on the cartridge. In co-op each had its own
// owner; when one owner's copy slept its counter stood while the others ran on, and that pole left the chain (bfs_f3:
// seam median 234 units). A set is the copies of one class with the same param, the same base height and the same z
// (within 64 units); its lead is the lowest slot. Every member takes the lead's owner, the lead's claim distance is the
// nearest member's, and every member runs on the lead's counters. Every console builds the same sets from the same spawns.
const int kLinkMax = 32;
int g_link_lead[kSlots];
int g_link_idx[kLinkMax], g_link_n = 0;
bool link_key(const Slot &s, int32_t *ky) {
    if (!s.a) return false;
    const Prof &p = g_prof[s.prof];
    if (p.id == 142) { if (!s.bvalid) return false; *ky = s.by; return true; }
    if (p.id == 145) { *ky = rd32(s.a, 0x320); return true; }   // mRestY
    return false;
}
void link_build() {
    static bool init = false;
    if (!init) { for (int i = 0; i < kSlots; ++i) g_link_lead[i] = -1; init = true; }
    for (int i = 0; i < g_link_n; ++i) g_link_lead[g_link_idx[i]] = -1;
    g_link_n = 0;
    if (!g_linked) return;
    for (int i = 0; i < kSlots && g_link_n < kLinkMax; ++i) {
        int32_t k;
        if (link_key(g_slot[i], &k)) { g_link_idx[g_link_n++] = i; g_link_lead[i] = -1; }
    }
    unsigned sets = 0, sig = 0;
    for (int a = 0; a < g_link_n; ++a) {
        const int i = g_link_idx[a];
        const Slot &si = g_slot[i];
        int32_t ki;
        if (g_link_lead[i] >= 0 || !link_key(si, &ki)) continue;
        bool any = false;
        for (int b = a + 1; b < g_link_n; ++b) {
            const int j = g_link_idx[b];
            const Slot &sj = g_slot[j];
            int32_t kj;
            if (g_link_lead[j] >= 0 || sj.prof != si.prof || !link_key(sj, &kj) || kj != ki) continue;
            if (rd32(sj.a, 0x08) != rd32(si.a, 0x08)) continue;
            const int dz = rd32(sj.a, 0x64) - rd32(si.a, 0x64);
            if (dz > 0x40000 || dz < -0x40000) continue;
            g_link_lead[j] = i; any = true; sig = sig * 31u + (unsigned)j;
        }
        if (any) { g_link_lead[i] = i; ++sets; sig = sig * 31u + (unsigned)i; }
    }
    static unsigned last_sig = 0;
    if (sig != last_sig) {
        last_sig = sig;
        g_link_sets = sets;
        if (g_log)
            for (int a = 0; a < g_link_n; ++a) {
                const int i = g_link_idx[a];
                if (g_link_lead[i] != i) continue;
                std::fprintf(stderr, "[coopmove] linked set: class %d lead slot %d, members", (int)g_prof[g_slot[i].prof].id, i);
                for (int b = 0; b < g_link_n; ++b)
                    if (g_link_lead[g_link_idx[b]] == i) std::fprintf(stderr, " %d", g_link_idx[b]);
                std::fprintf(stderr, " wall=%lld\n", wall_ms());
            }
    }
}
int link_lead(int slot) { return g_linked ? g_link_lead[slot] : -1; }
// the claim distance a linked copy carries: the nearest member's (the same number for every member, on every console)
unsigned link_cd(const char *pl, int slot) {
    const int ld = link_lead(slot);
    if (ld < 0) return claim_cd(pl, g_slot[slot]);
    unsigned best = 65535;
    for (int b = 0; b < g_link_n; ++b)
        if (g_link_lead[g_link_idx[b]] == ld) best = (std::min)(best, claim_cd(pl, g_slot[g_link_idx[b]]));
    return best;
}
// a member runs on the lead's counters (the lead's copy here already took this tick's record)
void link_follow(int slot, Slot &s) {
    const int ld = link_lead(slot);
    if (ld < 0 || ld == slot || !g_slot[ld].a) return;
    const Prof &p = g_prof[s.prof];
    bool moved = false;
    for (int k = 0; k < p.nf; ++k) {
        const uint32_t v = read_field(g_slot[ld].a, p.f[k]);
        if (read_field(s.a, p.f[k]) != v) { write_field(s.a, p, p.f[k], v); moved = true; }
    }
    if (moved) ++g_link_follow;
}

// Ownership, apply, publish: one pass over the slots that exist here.
void process(unsigned now) {
    char *pl = local_player();
    if (!pl) return;
    refill(now);
    {
        static bool dying_was = false;
        const bool dying = g_dead_guard && local_body_dying(pl);
        if (g_log && dying != dying_was)
            std::fprintf(stderr, "[coopmove] my body %s wall=%lld\n", dying ? "is dying: it claims nothing" : "claims again", wall_ms());
        dying_was = dying;
    }
    link_build();
    unsigned char recs[kPayloadMax]; int nrec = 0, bytes = 0;
    struct Due { int slot; unsigned age; };
    Due due[kSlots]; int ndue = 0;
    for (int slot = 0; slot < kSlots; ++slot) {
        Slot &s = g_slot[slot];
        if (!s.a) continue;
        const Prof &p = g_prof[s.prof];
        if (p.flags & kFSampleOnly) continue;          // the control: seen and sampled, never owned, applied, pinned or published
        const unsigned my_cd = link_cd(pl, slot);
        // 1. who owns this actor
        int best = -1; unsigned best_cd = 0xffffffffu; unsigned cur_cd = 0xffffffffu; bool cur_fresh = false;
        for (int q = 0; q < kSeats; ++q) {
            if (q == g_my) continue;
            const Rx &x = g_rx[slot][q];
            if (!alive_rx(x, now)) continue;                  // (a snapshot record claims the actor too: its sender owns it)
            if (q == s.owner) { cur_fresh = true; cur_cd = x.cd; }
            if (best < 0 || x.cd < best_cd) { best = q; best_cd = x.cd; }
        }
        const int was = s.owner;
        int nown = decide_owner(s.owner, g_my, my_cd, cur_fresh, cur_cd, best, best_cd, g_hyst);
        if (g_hyst && was != nown && was >= 0 && (unsigned)(now - s.owner_t) < (unsigned)kDwellMs && ((was == g_my && s.last_pub != 0) || (was != g_my && cur_fresh)))
            nown = was;                                            // dwell: no flip within a second
        // COOPDIAMOND2: a diamond lift copy whose heading no record has checked yet does not take the lift from a live owner (it would
        // drive the room's lift off along its own side of the square); it confirms from that owner's next records, a few frames.
        if (g_dia_heading && p.id == 135 && nown == g_my && was >= 0 && was != g_my && cur_fresh && !s.hconf) { nown = was; ++g_dia_held; }
        if (g_boss_own && (p.flags & kFBoss)) {       // MOVERS9: who steers the pose = who the boss chases (the boss lane's authority rule)
            int ts = coop_target_seat_of_slot(slot);
            if (p.id == 189) { const int ds = coop_boss_driver_seat(slot); if (ds >= 0) ts = ds; }   // kbo2: King Bob-omb's driver (talk claim, defeat owner)
            if (ts == g_my) nown = g_my;
            else if (ts >= 0 && ts < kSeats && ((coop_group_member_mask() >> ts) & 1u) && alive_rx(g_rx[slot][ts], now)) nown = ts;
        }
        if (p.id == 215 || p.id == 216) {             // coopbully1: the player who knocked a bully steers it while the knock lasts (no dwell)
            const int cs = coop_boss_bully_claim_seat(slot);
            if (cs == g_my) nown = g_my;
            else if (cs >= 0 && cs < kSeats && ((coop_group_member_mask() >> cs) & 1u) && alive_rx(g_rx[slot][cs], now)) nown = cs;
        }
        {   // COOPDIAMOND2: a linked member has the owner its lead has (the lead is a lower slot, decided earlier in this pass)
            const int ld = link_lead(slot);
            if (ld >= 0 && ld != slot && g_slot[ld].owner >= 0) nown = g_slot[ld].owner;
        }
        if (nown != was) {
            s.owner = nown; s.owner_t = now;
            if (was >= 0) {
                ++g_st.owner_changes;
                ++g_cs[s.prof].owner_ch;
                if (g_log && g_st.owner_changes <= 400)
                    std::fprintf(stderr, "[coopmove] owner slot=%d cls=%d %d->%d mycd=%u wall=%lld\n", slot, (int)p.id, was, nown, my_cd, wall_ms());
            }
        }
        // 2. apply what the owner (or a snapshot) sent
        for (int q = 0; q < kSeats; ++q) {
            if (q == g_my) continue;
            Rx &x = g_rx[slot][q];
            if (!x.valid || !x.fresh) continue;
            if ((unsigned)(now - x.t) > (unsigned)kRxKeepMs) { x.fresh = false; continue; }
            if (x.snap) {
                if (s.seen < 2) continue;                           // let the object finish its first tick
            } else if (s.owner != q) { x.fresh = false; continue; }
            apply_rec(slot, s, x);
            x.fresh = false;
        }
        if (g_act && (p.flags & kFSlave) && s.owner >= 0 && s.owner != g_my && s.seen >= 2) slave(s, g_rx[slot][s.owner], now);
        if (g_act) link_follow(slot, s);
        if (g_act) derive_apply(slot, s, now);
        // 3. publish if I own it
        if (!g_act) continue;
        if (!g_hold && s.owner == g_my && s.seen >= 3 && !skip_state(s.a, p) && !frozen_state(s.a)) {
            if (s.pin_lf == kPinStay || (s.pin_lf && g_lf - (s.pin_lf - 1) <= 1u)) { ++g_pin_skips; continue; }   // a friend's held pin owns the pose
            const unsigned age = now - s.last_pub;
            if (age >= p.period) {
                bool changed = !s.lvalid || rd32(s.a, 0x5c) != s.lx || rd32(s.a, 0x60) != s.ly || rd32(s.a, 0x64) != s.lz ||
                               rd16(s.a, 0x8c) != s.lax || rd16(s.a, 0x8e) != s.lay || rd16(s.a, 0x90) != s.laz;
                for (int k = 0; !changed && k < p.nf; ++k) changed = read_field(s.a, p.f[k]) != s.lf[k];
                if (changed || age >= (unsigned)kHeartbeatMs) { due[ndue].slot = slot; due[ndue].age = age; ++ndue; }
            }
        }
    }
    // publish, most overdue first (every third tick, so a datagram carries a whole batch)
    if ((g_lf % 3) != 0) ndue = 0;
    std::sort(due, due + ndue, [](const Due &a, const Due &b) { return a.age > b.age; });
    int i = 0;
    while (i < ndue) {
        nrec = 0; bytes = 0;
        int first = i;
        while (i < ndue && nrec < kMoverMaxRecs && bytes + rec_bytes(g_slot[due[i].slot].prof) + (int)sizeof(MoverBatchHead) <= kPayloadMax) {
            Slot &s = g_slot[due[i].slot];
            const unsigned my_cd = link_cd(pl, due[i].slot);
            bytes += fill_rec(recs + bytes, due[i].slot, s, my_cd, false);
            ++nrec; ++i;
        }
        if (nrec == 0) { ++i; continue; }
        if (g_tokens < 1.0) { ++g_st.throttled; break; }          // the rest wait for the next tick
        g_tokens -= 1.0;
        send_batch(false, recs, nrec, bytes);
        for (int k = first; k < i; ++k) {
            Slot &s = g_slot[due[k].slot];
            s.last_pub = now;
            ++g_cs[s.prof].pub;
            s.lvalid = true; s.lx = rd32(s.a, 0x5c); s.ly = rd32(s.a, 0x60); s.lz = rd32(s.a, 0x64);
            s.lax = rd16(s.a, 0x8c); s.lay = rd16(s.a, 0x8e); s.laz = rd16(s.a, 0x90);
            for (int f = 0; f < g_prof[s.prof].nf; ++f) s.lf[f] = read_field(s.a, g_prof[s.prof].f[f]);
        }
    }
    // snapshot replies
    for (int q = 0; q < kSeats; ++q) {
        Job &j = g_job[q];
        if (!j.on) continue;
        if (j.next && (int)(now - j.next) < 0) continue;
        if (!member_ok(q)) { j.on = false; continue; }
        int emitted = 0;
        while (j.cursor < kSlots && emitted < 3) {
            nrec = 0; bytes = 0;
            const int c0 = j.cursor;
            while (j.cursor < kSlots && nrec < kMoverMaxRecs) {
                Slot &s = g_slot[j.cursor];
                const int slot = j.cursor;
                bool mine = s.a && s.seen >= 3 && !skip_state(s.a, g_prof[s.prof]) && s.owner == g_my && (s.heard || coop_group_i_am_owner()) &&
                            !(s.pin_lf == kPinStay || (s.pin_lf && g_lf - (s.pin_lf - 1) <= 1u));
                if (mine && bytes + rec_bytes(s.prof) + (int)sizeof(MoverBatchHead) > kPayloadMax) break;
                ++j.cursor;
                if (!mine) continue;
                const unsigned my_cd = link_cd(pl, slot);
                bytes += fill_rec(recs + bytes, slot, s, my_cd, frozen_state(s.a));
                ++nrec;
            }
            if (nrec > 0) {
                if (g_tokens < 1.0) { j.cursor = c0; ++g_st.throttled; break; }
                g_tokens -= 1.0;
                send_batch(true, recs, nrec, bytes);
                ++emitted;
            }
        }
        if (j.cursor >= kSlots) {
            if (--j.passes > 0) { j.cursor = 0; j.next = now + 250; } else j.on = false;
        }
    }
}

// ---------------------------------------------------------------------------
// Late-join request.
// ---------------------------------------------------------------------------
void maybe_request(unsigned now) {
    if (!g_snap_on || !g_act || g_req_done || !g_spawned) return;
    if (g_lf < g_spawn_lf + 1) return;
    if (coop_group_member_mask() == 0) { g_req_done = true; g_hold = false; return; }   // alone: nobody to ask
    // any snapshot record taken counts as an answer
    if (g_st.snap_applied > g_snap_base) { g_req_done = true; g_hold = false; return; }
    if (g_req_tries >= 5 && (unsigned)(now - g_req_last) >= 300u) { g_req_done = true; g_hold = false; return; }
    if (g_req_tries > 0 && (unsigned)(now - g_req_last) < 300u) return;
    MoverSnapReq q; std::memset(&q, 0, sizeof q);
    q.key = wire_key(); q.nonce = ++g_req_nonce;
    if (coop_send_ctl(kCtlMoverSnapReq, &q, (int)sizeof q)) {
        ++g_st.req_sent; ++g_req_tries; g_req_last = now;
        if (g_log) std::fprintf(stderr, "[coopmove] snapshot request %d wall=%lld\n", g_req_tries, wall_ms());
    }
}

// ---------------------------------------------------------------------------
// Proof knobs.
// ---------------------------------------------------------------------------
void do_sample() {
    char line[8192]; int n = std::snprintf(line, sizeof line, "[mvs] w=%lld lf=%u seat=%d", wall_ms(), g_lf, g_my);
    for (int slot = 0; slot < kSlots && n < (int)sizeof line - 80; ++slot) {
        const Slot &s = g_slot[slot];
        if (!s.a) continue;
        n += std::snprintf(line + n, sizeof line - n, " %d:%d:%d:%d:%d:%d:%d", slot, (int)g_prof[s.prof].id,
                           rd32(s.a, 0x5c) >> 12, rd32(s.a, 0x60) >> 12, rd32(s.a, 0x64) >> 12,
                           (int)(uint16_t)rd16(s.a, 0x8e), (int)(uint16_t)rd16(s.a, 0x90));
    }
    std::fprintf(stderr, "%s\n", line);
}

void do_dump(unsigned at) {
    for (int slot = 0; slot < kSlots; ++slot) {
        const Slot &s = g_slot[slot];
        if (!s.a) continue;
        char line[kFieldHi * 2 + 96]; int n = std::snprintf(line, sizeof line, "[mvd] lv=%d lf=%u slot=%d cls=%d w=", g_key.level, at, slot, (int)g_prof[s.prof].id);
        const int hi = (g_prof[s.prof].size && g_prof[s.prof].size < kFieldHi) ? g_prof[s.prof].size : kFieldHi;
        for (int off = kFieldLo; off < hi; off += 4) n += std::snprintf(line + n, sizeof line - n, "%08x", (unsigned)rd32(s.a, off));
        std::fprintf(stderr, "%s\n", line);
    }
}

void do_forge(const char *name) {
    unsigned char b[kPayloadMax]; std::memset(b, 0, sizeof b);
    MoverBatchHead h; std::memset(&h, 0, sizeof h);
    h.key = wire_key(); h.count = 1; h.pver = kMoverProfileVer;
    MoverRec r; std::memset(&r, 0, sizeof r);
    int slot = -1, cls = 0;
    for (int i = 0; i < kSlots; ++i) if (g_slot[i].a) { slot = i; cls = g_prof[g_slot[i].prof].id; break; }
    if (slot < 0) { std::fprintf(stderr, "[coopmove-knob] forge %s: no actor\n", name); return; }
    r.slot = (uint16_t)slot; r.cls = (uint16_t)cls; r.cd = 5;
    r.x = rd32(g_slot[slot].a, 0x5c); r.y = rd32(g_slot[slot].a, 0x60); r.z = rd32(g_slot[slot].a, 0x64);
    r.nf = g_prof[g_slot[slot].prof].nf;
    int len = (int)sizeof h + (int)sizeof r + 4 * r.nf;
    bool snapreq = false;
    if (!std::strcmp(name, "cls")) r.cls = (uint16_t)(cls == 200 ? 206 : 200);
    else if (!std::strcmp(name, "slot")) r.slot = 600;
    else if (!std::strcmp(name, "far")) r.x = 100 * 4096 * 1000;                 // 100000 units
    else if (!std::strcmp(name, "nf")) r.nf = (uint8_t)(r.nf + 1);
    else if (!std::strcmp(name, "pver")) h.pver = 9;
    else if (!std::strcmp(name, "key")) h.key.act = (uint8_t)((g_key.act + 1) & 7);
    else if (!std::strcmp(name, "bits")) h.reserved = 1;
    else if (!std::strcmp(name, "short")) len -= 6;
    else if (!std::strcmp(name, "long")) len += 5;
    else if (!std::strcmp(name, "req")) snapreq = true;
    else if (!std::strcmp(name, "val")) {      // a phase field value out of its range, on a class that has fields (prefer one this seat owns)
        int fs = -1;
        for (int i = 0; i < kSlots; ++i)
            if (g_slot[i].a && g_prof[g_slot[i].prof].nf > 0 && (fs < 0 || g_slot[i].owner == g_my)) { fs = i; if (g_slot[i].owner == g_my) break; }
        if (fs < 0) { std::fprintf(stderr, "[coopmove-knob] forge val: no actor with phase fields\n"); return; }
        slot = fs; r.slot = (uint16_t)fs; r.cls = (uint16_t)g_prof[g_slot[fs].prof].id; r.nf = g_prof[g_slot[fs].prof].nf;
        r.x = rd32(g_slot[fs].a, 0x5c); r.y = rd32(g_slot[fs].a, 0x60); r.z = rd32(g_slot[fs].a, 0x64);
        len = (int)sizeof h + (int)sizeof r + 4 * r.nf;
        const uint32_t bad = 0x7ffffff0u;
        std::memcpy(b + sizeof h + sizeof r, &bad, 4);
    }
    else return;
    std::fprintf(stderr, "[coopmove-knob] forge %s\n", name);
    if (snapreq) {
        MoverSnapReq q; std::memset(&q, 0, sizeof q); q.key = wire_key(); q.key.reserved = 1;   // reserved byte set: invalid
        coop_send_ctl(kCtlMoverSnapReq, &q, (int)sizeof q);
        return;
    }
    std::memcpy(b, &h, sizeof h);
    std::memcpy(b + sizeof h, &r, sizeof r);
    if (len > kPayloadMax) len = kPayloadMax;
    coop_send_world(kWorldMoverBatch, b, len > 0 ? len : 1);
}

// Proof line: the local player lost health. Enemies hurt players in coop (the ROM's own damage is never touched here);
// the line names the nearest synced actor and who owns it, so a row can show a non-owner console taking the hit once.
void watch_hurt(char *pl) {
    const int hp = (int)(signed char)(data_02092144[data_0209f250 & 15] >> 8);
    if (g_hp_prev != -99 && hp < g_hp_prev) {
        int bs = -1; double bd = 1e18;
        for (int i = 0; i < kSlots; ++i)
            if (g_slot[i].a) {
                const double d = dist3(g_slot[i].a, rd32(pl, 0x5c), rd32(pl, 0x60), rd32(pl, 0x64));
                if (d < bd) { bd = d; bs = i; }
            }
        std::fprintf(stderr, "[coopmove] hurt hp %d->%d nearest slot=%d cls=%d dist=%.0f owner=%d me=%d wall=%lld\n", g_hp_prev, hp, bs,
                     bs >= 0 ? (int)g_prof[g_slot[bs].prof].id : -1, bd < 1e17 ? bd : -1.0, bs >= 0 ? g_slot[bs].owner : -1, g_my, wall_ms());
    }
    g_hp_prev = hp;
}

void stats_line() {
    const CoopMoveStats &s = g_st;
    std::fprintf(stderr,
        "[movestat] seat=%d ents=%llu sent b%llu r%llu sb%llu sr%llu req%llu served%llu rx b%llu r%llu applied w%llu e%llu band%llu "
        "skip%llu clsmis%llu snapapp%llu noent%llu owner_ch%llu stale%llu thr%llu slv%llu drop bad%llu group%llu member%llu ver%llu far%llu off%llu seat%llu val%llu rowsdrop%llu wrefused%llu sizemis%llu overrun%llu\n",
        g_my, s.ents, s.batches_sent, s.recs_sent, s.snap_batches_sent, s.snap_recs_sent, s.req_sent, s.req_served,
        s.batches_rx, s.recs_rx, s.applied_write, s.applied_ease, s.in_band, s.skipped_state, s.cls_mismatch, s.snap_applied,
        s.no_ent, s.owner_changes, s.owner_stale, s.throttled, s.slaved, s.drop_bad, s.drop_group, s.drop_member, s.drop_ver, s.drop_far,
        s.drop_off, s.drop_seat, s.drop_val, s.rows_dropped, s.write_refused, s.size_mismatch, s.overrun);
    if (g_dia_flips || g_dia_ok || g_dia_held)
        std::fprintf(stderr, "[movestat] diamond heading: turned %u, already right %u, handovers held %u\n", g_dia_flips, g_dia_ok, g_dia_held);
    if (g_link_sets)
        std::fprintf(stderr, "[movestat] linked sets %u, members put on their lead's counters %u times\n", g_link_sets, g_link_follow);
    if (g_der_fix || g_der_refused || g_der_hold)
        std::fprintf(stderr, "[movestat] cycle movers: put back %llu times, largest %.1f units, refused %u, held on a quiet owner %llu\n", g_der_fix, g_der_max, g_der_refused, g_der_hold);
}

// ---------------------------------------------------------------------------
// Hooks from coop_world.
// ---------------------------------------------------------------------------
void on_frame(unsigned now) {
    read_env();
    coop_stats_log_tick(now);
    if (!coop_world_active()) return;
    ensure_registered();
    int lv = -1, ac = 0;
    if (!coop_group_established() || !coop_group_key(&lv, &ac)) return;
    char *pl = local_player();
    if (!pl) return;
    const CoopKey nk(lv, ac);
    if (nk != g_key) { reset_level(); g_key = nk; }
    ++g_lf; ++g_st.ticks;
    scan_ents();
    if (!g_spawned && g_ents > 0) { g_spawned = true; g_spawn_lf = g_lf; }
    maybe_request(now);
    process(now);
    ghost_eggs_frame(now);     // FRIENDVIS1: a friend's Yoshi eggs on my console (hal/coop_ghost_eggs.h)
    coop_cap_frame(now);       // CAPLOSS1: a friend's lost cap on my console (hal/coop_cap.cpp)
    if (g_god) {      // lab knob: the proof player does not die and does not fall out of a course whose warp lands in the air
        data_02092144[data_0209f250 & 15] = (short)(8 << 8);
        if (g_spawned && g_lf - g_spawn_lf == 30) { std::memcpy(g_god_pos, pl + 0x5c, 12); g_god_set = true; }
        if (g_god_set) { std::memcpy(pl + 0x5c, g_god_pos, 12); wr32(pl, 0xa8, 0); wr32(pl, 0x98, 0); }
    }
    if (g_log) watch_hurt(pl);
    if (g_spawned) {
        const unsigned since = g_lf - g_spawn_lf;
        if (g_sample_every && (since % g_sample_every) == 0) do_sample();
        if (g_area_to >= 0 && !g_area_done && since >= g_area_at) {
            g_area_done = true;
            *(signed char *)(pl + 0xcc) = (signed char)g_area_to;
            ChangeArea(g_area_to);
            std::fprintf(stderr, "[coopmove-knob] area %d shown (tick %u)\n", g_area_to, since);
        }
        for (int i = 0; i < g_ndump; ++i) if (since == g_dump_at[i]) do_dump(since);
        for (int i = 0; i < g_nshot; ++i)
            if (since == g_shot_at[i] && ::port_level_fb_ptr) {
                char nm[64]; std::snprintf(nm, sizeof nm, "movshot_f%u_t%u.bmp", g_lf, since);
                const bool w = ::ntr::ppu_write_bmp(nm, *(const ::ntr::Framebuffer *)::port_level_fb_ptr);
                std::fprintf(stderr, "[coopmove-knob] picture %s %s (tick %u)\n", nm, w ? "written" : "FAILED", since);
            }
        for (int i = 0; i < g_nforge; ++i)
            if (!g_forge[i].done && since >= g_forge[i].tick) { g_forge[i].done = true; do_forge(g_forge[i].name); }
    }
    if (g_log && (unsigned)(now - g_stat_t) >= 4000u) { g_stat_t = now; stats_line(); }
}

void on_member_gone(int seat) {
    ghost_eggs_member_gone(seat);   // FRIENDVIS1
    if (seat < 0 || seat >= kSeats || !g_stale_on) return;     // (the stale control keeps a dead owner)
    for (int i = 0; i < kSlots; ++i) {
        g_rx[i][seat].valid = false; g_rx[i][seat].fresh = false;
        if (g_slot[i].owner == seat) { g_slot[i].owner = -1; g_slot[i].owner_t = 0; }
    }
    g_job[seat].on = false;
    if (g_log) std::fprintf(stderr, "[coopmove] seat %d gone, its actors are up for adoption wall=%lld\n", seat, wall_ms());
}

struct Registrar {
    Registrar() {
        coop_on_frame(on_frame);
        coop_on_member_gone(on_member_gone);
    }
} g_registrar;

}  // namespace

// ---------------------------------------------------------------------------
// Public.
// ---------------------------------------------------------------------------
CoopMoveStats coop_move_stats() { return g_st; }

bool coop_move_i_own(int slot, unsigned cls) {
    if (slot < 0 || slot >= kSlots || g_my < 0) return false;
    const Slot &s = g_slot[slot];
    if (!s.a || *(const unsigned short *)(s.a + 0x0c) != cls) return false;
    return s.owner == g_my;
}

int coop_move_owner(int slot) {
    if (slot < 0 || slot >= kSlots || !g_slot[slot].a) return -1;
    return g_slot[slot].owner;
}

void coop_move_pinned(int slot, bool stay) {
    if (slot >= 0 && slot < kSlots && g_slot[slot].a) g_slot[slot].pin_lf = stay ? kPinStay : g_lf + 1;
}

void coop_move_unpin(int slot) {
    if (slot >= 0 && slot < kSlots) g_slot[slot].pin_lf = 0;
}

bool coop_move_pin_stays(int slot) {
    return slot >= 0 && slot < kSlots && g_slot[slot].a && g_slot[slot].pin_lf == kPinStay;
}

unsigned coop_move_pin_skips() { return g_pin_skips; }

int coop_move_class_stats(CoopClassStat *out, int max) {
    int n = 0;
    for (int i = 0; i < g_nprof && n < max; ++i) {
        const CStat &c = g_cs[i];
        if (g_prof[i].id == 0 || (c.ents_max == 0 && c.pub == 0 && c.applied == 0)) continue;
        CoopClassStat &o = out[n++];
        o.id = g_prof[i].id; o.name = g_prof[i].name ? g_prof[i].name : "?";
        o.ents_max = c.ents_max; o.pub = c.pub; o.applied = c.applied; o.slaved = c.slaved; o.owner_ch = c.owner_ch;
        o.pin_n = c.pin_n; o.pin_big = c.pin_big; o.held_pins = c.held_pins; o.pin_avg = c.pin_n ? c.pin_sum / c.pin_n : 0.0; o.pin_max = c.pin_max;
    }
    return n;
}

bool coop_mover_req_validate(const void *p, int len) {
    if (!p || len != (int)sizeof(MoverSnapReq)) return false;
    MoverSnapReq q; std::memcpy(&q, p, sizeof q);
    return coop_key_ok(q.key);
}

bool coop_mover_batch_validate(const void *payload, int len) {
    if (!payload || len < (int)sizeof(MoverBatchHead) + (int)sizeof(MoverRec) || len > kPayloadMax) return false;
    const unsigned char *p = static_cast<const unsigned char *>(payload);
    MoverBatchHead h; std::memcpy(&h, p, sizeof h);
    if (!coop_key_ok(h.key) || h.flags > 1 || h.reserved != 0) return false;
    if (h.count < 1 || h.count > kMoverMaxRecs) return false;
    if (g_nprof == 0) prof_index();
    int off = (int)sizeof h;
    for (int i = 0; i < (int)h.count; ++i) {
        if (off + (int)sizeof(MoverRec) > len) return false;
        MoverRec r; std::memcpy(&r, p + off, sizeof r);
        if (r.slot >= kSlots || r.rflags > 1) return false;
        if (!prof_known(r.cls, r.nf)) return false;
        if (off + (int)sizeof r + 4 * r.nf > len) return false;
        {
            const Prof &vp = g_prof[g_prof_of[r.cls]];
            for (int k = 0; k < r.nf; ++k) {
                uint32_t v; std::memcpy(&v, p + off + sizeof r + 4 * k, 4);
                if (!value_ok(vp.f[k], v)) { ++g_st.drop_val; return false; }
                if (!g_guard && !value_in(vp.f[k], v) && ++g_st.overrun <= 8)
                    std::fprintf(stderr, "[coopmove] (guard off) out-of-range value %#x accepted for class %d field %#x\n", (unsigned)v, (int)vp.id, (unsigned)vp.f[k].off);
            }
        }
        const long long lim = (long long)kMoverMaxUnits * 4096;
        if (r.x < -lim || r.x > lim || r.y < -lim || r.y > lim || r.z < -lim || r.z > lim) return false;
        off += (int)sizeof r + 4 * r.nf;
        if (off > len) return false;
    }
    return off == len;
}

}  // namespace port
