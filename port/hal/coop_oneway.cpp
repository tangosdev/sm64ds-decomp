// COOP: ONE-WAY OBJECT STATES, SHARED. Run coopfix1, lane MOVERS1 (AUDIT-WORLD H8).
//
// The Whomp's Fortress knock-down plank (class 44, daObjBk_Botaosi_c, src/game/actors/d_a_obj_bk_botaosi.cpp) stands until a
// player knocks it over (mState 2, it falls toward mKnockDir, then lies flat in mState 3 for the rest of the visit). The hit
// lands on one console only (the hitter's body is the only one that collides there), so the plank fell on the hitter's
// screen and stood on every friend's, where a friend could still climb it or knock it again.
//
// The rule now: a console whose copy reached "knocked over" tells the group (slot, class, fall direction). Every console keeps
// the furthest state the group reached for the visit, and a copy that still stands is knocked the same way the cartridge's own
// hit handler does it (mKnockDir, mWobbleTimer 0x640, mState 2), so its own Behavior plays the fall, the thud and the
// earthquake. The state never goes back, so there is no owner to agree on and nothing to fight over. Every console repeats its
// list every 2 s (a few bytes), which also reaches a friend who enters later.
//
// WIRE: SYNW sub-type kWorldOneWay (0x13), in the mover range. 1.0.4 counts it as an unknown sub-type and drops it alone.
// INERT outside a coop room. SM64DS_COOP_MOVERS1=0 is the control. SM64DS_COOP_ONEWAY_LOG=1 prints [coneway] lines.
//
// PORT_HOST_ABI. Reads and writes the plank by raw offset (the port/hal convention); the offsets are checked against the
// class's own header below, so a layout change fails the build instead of writing the wrong bytes.

#include "coop_actors.h"
#include "coop_world.h"
#include "coop_wire.h"
#include "comms_hub.h"

#include "daObjBk_Botaosi_c.h"
#include "daObjMaruta_c.h"
#include "daTrsTrap_c.h"
#include "daObjFm_Battan_c.h"
#include "daObjBk_Rotebar_c.h"

#include <windows.h>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// The raw offsets this module and the MOVERS1 profile rows (coop_actor_profiles.inc) and claim rules (coop_actors.cpp) use,
// checked against the host build of each class.
static_assert(offsetof(daObjBk_Botaosi_c, mKnockDir) == 0x396, "plank mKnockDir");
static_assert(offsetof(daObjBk_Botaosi_c, mState) == 0x397, "plank mState");
static_assert(offsetof(daObjBk_Botaosi_c, mWobbleTimer) == 0x394, "plank mWobbleTimer");
static_assert(offsetof(daObjMaruta_c, mRollAngVel) == 0x33c, "log mRollAngVel");
static_assert(offsetof(daObjMaruta_c, mHitByPlayer) == 0x342, "log mHitByPlayer");
static_assert(offsetof(daTrsTrap_c, mStandingActor) == 0x124, "trap mStandingActor");
static_assert(offsetof(daObjFm_Battan_c, mAngVelX) == 0x31e, "squasher mAngVelX");
static_assert(offsetof(daObjFm_Battan_c, mStateTimer) == 0x320, "squasher mStateTimer");
static_assert(offsetof(daObjFm_Battan_c, mState) == 0x322, "squasher mState");
static_assert(offsetof(daObjBk_Rotebar_c, mPauseTimer) == 0x31e, "bridge mPauseTimer");

extern "C" {
void *_ZN8dActor_c4NextEPKS_(const void *prev);
}

namespace port {
namespace {

enum : int { kPlank = 44, kSlots = 512, kSeats = 4, kMaxRecs = 32, kVer = 1, kBeatMs = 2000,
             kOffDir = 0x396, kOffState = 0x397, kOffWobble = 0x394, kKnocked = 2 };

#pragma pack(push, 1)
struct OwHead { CoopKeyWire key; uint8_t count, ver; uint16_t reserved; };
struct OwRec { uint16_t slot, cls; uint8_t state, extra; uint16_t reserved; };
#pragma pack(pop)
static_assert(sizeof(OwHead) == 8, "one-way head is 8 bytes");
static_assert(sizeof(OwRec) == 8, "one-way record is 8 bytes");

bool g_env = false, g_on = true, g_log = false;
int g_lv = -1, g_ac = -1;
struct Known { bool valid, mine, applied; uint8_t extra; int from; };
Known g_known[kSlots];
unsigned g_beat = 0;
struct Stats { unsigned sent, rx, bad, applied, local; } g_st = {};

unsigned cls_of(const void *a) { unsigned short c; std::memcpy(&c, (const char *)a + 0x0c, 2); return c; }
short slot_of(const void *a) { short s; std::memcpy(&s, (const char *)a + 0xce, 2); return s; }
long long wall_ms() {
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    return (long long)((((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000ull);
}

void read_env() {
    if (g_env) return;
    g_env = true;
    g_on = coop_movers1_on();
    const char *a = std::getenv("SM64DS_COOP_ACT");
    if (a && *a == '0') g_on = false;
#ifndef PORT_ROM_CLEAN
    const char *l = std::getenv("SM64DS_COOP_ONEWAY_LOG");
    g_log = l && *l && *l != '0';
#endif
    if (g_log) std::fprintf(stderr, "[coneway] shared one-way states %s\n", g_on ? "ON" : "OFF (control)");
}

bool valid_dir(uint8_t d) { return d == 1 || d == 0xff; }

bool validate(const unsigned char *p, int len) {
    if (!p || len < (int)(sizeof(OwHead) + sizeof(OwRec))) return false;
    OwHead h; std::memcpy(&h, p, sizeof h);
    if (!coop_key_ok(h.key) || h.ver != kVer || h.reserved != 0 || h.count < 1 || h.count > kMaxRecs) return false;
    if (len != (int)(sizeof h + h.count * sizeof(OwRec))) return false;
    for (int i = 0; i < h.count; ++i) {
        OwRec r; std::memcpy(&r, p + sizeof h + i * sizeof r, sizeof r);
        if (r.slot >= kSlots || r.cls != kPlank || r.state != kKnocked || !valid_dir(r.extra) || r.reserved != 0) return false;
    }
    return true;
}

void send_all() {
    unsigned char buf[sizeof(OwHead) + kMaxRecs * sizeof(OwRec)];
    int n = 0;
    for (int s = 0; s < kSlots && n < kMaxRecs; ++s) {
        if (!g_known[s].valid) continue;
        OwRec r; std::memset(&r, 0, sizeof r);
        r.slot = (uint16_t)s; r.cls = kPlank; r.state = kKnocked; r.extra = g_known[s].extra;
        std::memcpy(buf + sizeof(OwHead) + n * sizeof r, &r, sizeof r); ++n;
    }
    if (n == 0) return;
    OwHead h; std::memset(&h, 0, sizeof h);
    h.key.level = (uint16_t)g_lv; h.key.act = (uint8_t)g_ac; h.count = (uint8_t)n; h.ver = kVer;
    std::memcpy(buf, &h, sizeof h);
    if (coop_send_world(kWorldOneWay, buf, (int)(sizeof h + n * sizeof(OwRec)))) ++g_st.sent;
}

// Proof knob SM64DS_COOP_MOVERS1_POKE=F:what[,F:what...] (off unless set; for rows only). F frames after this console's group
// took the level: `whomp` / `book` remove the placed plain Whomp (164) / bookend (327) nearest my player with MarkForDestruction
// and no death bit (what their own defeat ends in, d_a_btn.cpp:1346, d_a_book.cpp:710); `plank` knocks the placed plank nearest
// my player over toward -1 the way its hit handler does; `census` prints every placed 164 / 327 / 44 here with its state.
struct Poke { unsigned f; char what[12]; bool done; };
Poke g_poke[12]; int g_npoke = 0; bool g_poke_read = false; unsigned g_lvf = 0;
void poke_read() {
    if (g_poke_read) return;
    g_poke_read = true;
#ifndef PORT_ROM_CLEAN
    const char *e = std::getenv("SM64DS_COOP_MOVERS1_POKE");
#else
    const char *e = nullptr;   // the kit cannot set the proof knob
#endif
    for (const char *q = e; q && *q && g_npoke < 12;) {
        unsigned f = 0; char w[12] = {}; int used = 0;
        if (std::sscanf(q, "%u:%11[a-z]%n", &f, w, &used) < 2 || used <= 0) break;
        g_poke[g_npoke].f = f; std::memcpy(g_poke[g_npoke].what, w, sizeof w); g_poke[g_npoke].done = false; ++g_npoke;
        q += used; while (*q == ',' || *q == ' ') ++q;
    }
}
extern "C" { extern void *data_0209f394[]; extern unsigned char data_0209f250; void _ZN7fBase_c18MarkForDestructionEv(void *a); }
char *nearest_placed(unsigned cls) {
    const char *pl = (const char *)data_0209f394[data_0209f250 & 15];
    if (!pl) return nullptr;
    char *best = nullptr; double bd = 1e300;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (cls_of(a) != cls || slot_of(a) < 0 || a[0x0f]) continue;
        double d = 0;
        for (int k = 0; k < 3; ++k) { int u, v; std::memcpy(&u, a + 0x5c + 4 * k, 4); std::memcpy(&v, pl + 0x5c + 4 * k, 4); d += ((double)u - v) * ((double)u - v); }
        if (d < bd) { bd = d; best = a; }
    }
    return best;
}
void poke_frame() {
    poke_read();
    ++g_lvf;
    for (int i = 0; i < g_npoke; ++i) {
        Poke &k = g_poke[i];
        if (k.done || g_lvf < k.f) continue;
        k.done = true;
        if (!std::strcmp(k.what, "census")) {
            for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
                const unsigned c = cls_of(a);
                if ((c != 164 && c != 327 && c != kPlank) || slot_of(a) < 0) continue;
                int x, y, z; std::memcpy(&x, a + 0x5c, 4); std::memcpy(&y, a + 0x60, 4); std::memcpy(&z, a + 0x64, 4);
                short ax; std::memcpy(&ax, a + 0x8c, 2);
                std::fprintf(stderr, "[m1census] lf=%u class %u slot %d at %d,%d,%d alive%s%s wall=%lld\n", g_lvf, c, (int)slot_of(a), x >> 12, y >> 12, z >> 12,
                             a[0x0f] ? " (going)" : "", c == kPlank ? (((unsigned char *)a)[kOffState] >= kKnocked ? " plank DOWN" : " plank STANDING") : "", wall_ms());
                (void)ax;
            }
            std::fprintf(stderr, "[m1census] lf=%u end wall=%lld\n", g_lvf, wall_ms());
            continue;
        }
        const unsigned cls = !std::strcmp(k.what, "whomp") ? 164u : !std::strcmp(k.what, "book") ? 327u : !std::strcmp(k.what, "plank") ? (unsigned)kPlank : 0u;
        char *a = cls ? nearest_placed(cls) : nullptr;
        if (!a) { std::fprintf(stderr, "[m1poke] %s: no placed class %u here\n", k.what, cls); continue; }
        if (cls == kPlank) {
            *((signed char *)a + kOffDir) = -1;
            const short wob = 0x640; std::memcpy(a + kOffWobble, &wob, 2);
            ((unsigned char *)a)[kOffState] = kKnocked;
        } else _ZN7fBase_c18MarkForDestructionEv(a);
        std::fprintf(stderr, "[m1poke] %s: class %u slot %d %s here (proof knob) wall=%lld\n", k.what, cls, (int)slot_of(a),
                     cls == kPlank ? "knocked over" : "removed with no death bit", wall_ms());
    }
}

// Proof knob SM64DS_COOP_MOVERS1_SAMPLE=N (off unless set): every N frames one [m1s] line with every placed MOVERS1 mover here
// (slot:class:x:y:z:angleX:angleY:angleZ:first field), so a row can compare two consoles' copies by wall time.
unsigned g_sample = 0; bool g_sample_read = false;
void sample_frame() {
#ifndef PORT_ROM_CLEAN
    if (!g_sample_read) { g_sample_read = true; if (const char *e = std::getenv("SM64DS_COOP_MOVERS1_SAMPLE")) g_sample = (unsigned)std::strtoul(e, nullptr, 10); }
#endif
    if (!g_sample || (g_lvf % g_sample) != 0) return;
    char line[4096]; int n = std::snprintf(line, sizeof line, "[m1s] w=%lld lf=%u seat=%d", wall_ms(), g_lvf, comms_hub_my_seat());
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && n < (int)sizeof line - 96; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        const unsigned c = cls_of(a);
        if (c != 70 && c != 102 && c != 160 && c != 159 && c != 84 && c != 51 && c != kPlank) continue;
        if (slot_of(a) < 0) continue;
        int x, y, z; std::memcpy(&x, a + 0x5c, 4); std::memcpy(&y, a + 0x60, 4); std::memcpy(&z, a + 0x64, 4);
        unsigned short ax, ay, az; std::memcpy(&ax, a + 0x8c, 2); std::memcpy(&ay, a + 0x8e, 2); std::memcpy(&az, a + 0x90, 2);
        int f = 0;
        if (c == 70 || c == 102) { short v; std::memcpy(&v, a + 0x33c, 2); f = v; }
        else if (c == 84) f = ((unsigned char *)a)[0x322];
        else if (c == 51) f = ((signed char *)a)[0x31e];
        else if (c == kPlank) f = ((unsigned char *)a)[kOffState];
        n += std::snprintf(line + n, sizeof line - n, " %d:%u:%d:%d:%d:%u:%u:%u:%d", (int)slot_of(a), c, x >> 12, y >> 12, z >> 12, ax, ay, az, f);
    }
    std::fprintf(stderr, "%s\n", line);
}

}  // namespace

void coop_oneway_on_world(int seat, const void *payload, int len) {
    read_env();
    if (!g_on) return;
    const unsigned char *p = static_cast<const unsigned char *>(payload);
    if (seat < 0 || seat >= kSeats || seat == comms_hub_my_seat() || !validate(p, len)) { ++g_st.bad; return; }
    if (!((coop_group_member_mask() >> seat) & 1u)) return;
    OwHead h; std::memcpy(&h, p, sizeof h);
    if (g_lv < 0 || h.key.level != (uint16_t)g_lv || h.key.act != (uint8_t)g_ac) return;
    ++g_st.rx;
    for (int i = 0; i < h.count; ++i) {
        OwRec r; std::memcpy(&r, p + sizeof h + i * sizeof r, sizeof r);
        Known &k = g_known[r.slot];
        if (k.valid) continue;                              // the first word on a slot stands: the plank fell one way only
        k.valid = true; k.mine = false; k.applied = false; k.extra = r.extra; k.from = seat;
        if (g_log) std::fprintf(stderr, "[coneway] seat %d: plank slot %u was knocked over (dir %d) wall=%lld\n", seat, (unsigned)r.slot,
                                (int)(signed char)r.extra, wall_ms());
    }
}

void coop_oneway_frame(unsigned now) {
    read_env();
    int lv = -1, ac = -1;
    if (!coop_group_established() || !coop_group_key(&lv, &ac)) return;
    if (!g_on) {                                             // the control: the proof knob still runs, nothing is shared
        if (lv != g_lv || ac != g_ac) { g_lv = lv; g_ac = ac; g_lvf = 0; }
        poke_frame();
        sample_frame();
        return;
    }
    if (lv != g_lv || ac != g_ac) { std::memset(g_known, 0, sizeof g_known); g_lv = lv; g_ac = ac; g_beat = now; g_lvf = 0; }
    poke_frame();
    sample_frame();
    bool news = false;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (cls_of(a) != kPlank) continue;
        const short slot = slot_of(a);
        if (slot < 0 || slot >= kSlots) continue;
        unsigned char *st = (unsigned char *)a + kOffState;
        Known &k = g_known[slot];
        if (*st >= kKnocked) {
            if (!k.valid) {                                  // knocked here first: tell the group now
                const signed char d = *((signed char *)a + kOffDir);
                k.valid = true; k.mine = true; k.applied = true; k.extra = (uint8_t)(d < 0 ? 0xff : 1); k.from = -1;
                news = true; ++g_st.local;
                std::fprintf(stderr, "[coneway] plank slot %d knocked over here (dir %d): told the group wall=%lld\n", (int)slot, (int)d, wall_ms());
            }
            continue;
        }
        if (!k.valid || k.applied) continue;
        // a friend's plank fell: knock mine the way the cartridge's hit handler does (func_ov015_02111414 / OnHitByMegaChar)
        *((signed char *)a + kOffDir) = (signed char)k.extra;
        const short wob = 0x640; std::memcpy((char *)a + kOffWobble, &wob, 2);
        *st = kKnocked;
        k.applied = true; ++g_st.applied;
        std::fprintf(stderr, "[coneway] plank slot %d: seat %d knocked it over (dir %d), mine falls the same way wall=%lld\n", (int)slot, k.from,
                     (int)(signed char)k.extra, wall_ms());
    }
    if (news || now - g_beat >= (unsigned)kBeatMs) { g_beat = now; send_all(); }
    static unsigned last = 0;
    if (g_log && now - last >= 10000u) {
        last = now;
        std::fprintf(stderr, "[coneway] stats sent=%u rx=%u bad=%u applied=%u local=%u\n", g_st.sent, g_st.rx, g_st.bad, g_st.applied, g_st.local);
    }
}

}  // namespace port
