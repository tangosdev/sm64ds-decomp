// Run coopfix1, lane STAYSTAR1: a star got by somebody else never stops me getting the next one.
//
// THE BUG (Tango, 11-11, coop room with "Only star getter leaves" on, the default): once one star was got in a course visit, the
// players who stayed could not get any other star in that visit.
//
// THE CAUSE. The star's pickup test (daStar_c::func_ov002_020e930c, src/game/actors/d_a_star.cpp) refuses every touch while event
// bit 0x1e is set: "a star is being got now". The collect (func_ov002_020e8ef0) sets it, together with 0x1d (the HUD's bit: the
// power meter retracts while it is set), and only two things clear them: the end of a collect that does not leave the course
// (func_ov002_020e8618, the star's last step) and the next level load (the star's InitResources). On the cartridge the collector
// always leaves, so the level load always comes. In a coop room the other consoles stay, and on their consoles a collect can
// start that never reaches its own last step: a friend's body touches my copy of his star (his body stands on it on my console
// too), the collect runs with HIS body, and my copy is then removed from outside when his take arrives (hal/coop_spawn.cpp
// kill_quiet), or simply waits for the dance of a body my console does not drive. Either way 0x1e and 0x1d stay set on my console
// for the rest of the visit: every later star touch is refused and my power meter stays hidden.
//
// REVIEW-D found the main road one layer down: hal/coop_switch.cpp copied the whole event word (data_0209f34c) between
// consoles, bits 0x1d and 0x1e included, so the collector's lock reached everyone who stayed, and a clear here went back to the
// collector mid-dance. coop_switch now keeps both bits on their own console (kSwLocalOnly). What is below is the local backstop for
// the other road (a friend's body starting a collect on my copy); its clear stays here, since coop_switch never sends the bits.
//
// THE BACKSTOP. Once per game frame, in a coop or party room: when 0x1e or 0x1d is set and no collect of MINE runs on this console
// (no star or silver star in a collect state whose player is my body, my body not in a no-control state, no level change
// pending) for kQuietFrames frames in a row, both bits are cleared, as the star's own last step clears them. A collect of my own
// keeps them for as long as it runs (its exit then clears them by the level load, as on the cartridge). Single player and VS
// never run it (the room check, and VS: data_0209f2d8 == 1).
//
// Knobs (all off unless set; the fix itself is on in coop and party rooms):
//   SM64DS_COOP_STAYSTAR=0         the fix off (the before rows).
//   SM64DS_COOP_STAYSTAR_LOG=1     every change of the two bits, the cutscene freeze bit, my no-control byte and the course's
//                                  collected stars, with the stars that are in a collect state and whose body runs it.
//   SM64DS_COOP_STAYSTAR_SPAWN=<level>:<frame>:<n>[,<n>...]   proof: <frame> frames after my body first stands in <level>, PowerStars
//                                  (class 178, kind 0, the idle star) numbered <n> through the ROM's spawn, in a row 500 units
//                                  apart beside me. A coop room announces them like any runtime star (hal/coop_spawn.cpp).
//   SM64DS_COOP_STAYSTAR_TOUCH=<level>:<frame>:<n>[;...]   proof: <frame> frames after my body first stands in <level>, my body is
//                                  put on PowerStar <n> (a real touch: on a friend's console my body stands on his copy too).
//                                  Retried every 60 frames, 6 tries, while the star is still idle and not mine.
#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern signed char data_0209f2f8;          // current level
extern unsigned char data_0209f2d8[];      // stage mode, 1 = VS
extern unsigned char data_02092110[];      // pending level change, < 0 = none
extern unsigned int data_0209b454;         // the cutscene freeze word (bit 0x4000000)
int _ZN5Event6GetBitEj(unsigned bit);
void _ZN5Event8ClearBitEj(unsigned bit);
void *_ZN8dActor_c4NextEPKS_(const void *prev);
char *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b, const void *pos, const void *v16, int area, int f);
int IsStarCollectedInCurLevel(int starID);
extern void (*port_sdat_frame_hook)(void);   // hal/sdat/consumer.cpp: one call per game frame, every loop
}

namespace {

enum : unsigned { kBitTaking = 0x1e, kBitHud = 0x1d, kCutscene = 0x4000000u };
enum : int { kQuietFrames = 30, kTouchTries = 6, kMaxSteps = 8 };

// daStar_c (include/daStar_c.h): class 178 PowerStar, 179 silver star; +0x0f dead byte, +0x440 mState, +0x438 mPlayer, +0x49d mStarID
enum : int { kOffState = 0x440, kOffPlayer = 0x438, kOffStarId = 0x49d, kOffNoCtl = 0x709 /* Player::mIsNoControl */ };

long long wall_ms() {
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    return (long long)((((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000ull);
}
bool env_on(const char *n, bool d) {
    const char *e = std::getenv(n);
    if (!e || !*e) return d;
    return !(e[0] == '0' && e[1] == 0);
}
char *my_body() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < 16) ? (char *)data_0209f394[me] : nullptr;
}
int seat_of(const void *p) {
    if (!p) return -1;
    for (int i = 0; i < 16; ++i) if (data_0209f394[i] == p) return i;
    return -2;
}
unsigned cls_of(const char *a) { return *(const unsigned short *)(a + 0x0c); }
bool dead(const char *a) { return *(const unsigned char *)(a + 0x0f) != 0; }
bool collect_state(unsigned st) {   // TOUCHED, COLLECT_BEGIN / HOLD / TALK, SAVE_MESSAGE, PLAY_AND_END, SPIN_AND_END
    return st == 5 || st == 6 || st == 7 || st == 10 || st == 11 || st == 12 || st == 13;
}

struct State {
    bool inited, on, log;
    int quiet;
    unsigned fixes;
    // log
    int l_taking, l_hud, l_cut, l_noctl, l_mask, l_level;
    // knobs
    int lvl_seen, lvl_frames;
    int sp_lvl, sp_fr; int sp_n[kMaxSteps]; int sp_cnt; bool sp_done;
    struct Touch { int lvl, fr, n, tries; bool done; } tc[kMaxSteps]; int ntc;
};
State g;

void init_once() {
    if (g.inited) return;
    g.inited = true;
    g.on = env_on("SM64DS_COOP_STAYSTAR", true);
    g.log = env_on("SM64DS_COOP_STAYSTAR_LOG", false);
    g.l_taking = g.l_hud = g.l_cut = g.l_noctl = g.l_mask = g.l_level = -9;
    g.lvl_seen = -1;
    g.sp_lvl = -1;
    if (const char *e = std::getenv("SM64DS_COOP_STAYSTAR_SPAWN")) {
        int used = 0;
        if (std::sscanf(e, "%d:%d:%n", &g.sp_lvl, &g.sp_fr, &used) == 2 && used > 0) {
            const char *p = e + used;
            while (*p && g.sp_cnt < kMaxSteps) {
                char *end; const long v = std::strtol(p, &end, 10);
                if (end == p) break;
                g.sp_n[g.sp_cnt++] = (int)v;
                p = end; if (*p == ',') ++p;
            }
        } else g.sp_lvl = -1;
    }
    if (const char *e = std::getenv("SM64DS_COOP_STAYSTAR_TOUCH")) {
        const char *p = e; int used = 0;
        while (g.ntc < kMaxSteps) {
            State::Touch t{}; used = 0;
            if (std::sscanf(p, "%d:%d:%d%n", &t.lvl, &t.fr, &t.n, &used) != 3 || used <= 0) break;
            g.tc[g.ntc++] = t;
            p += used;
            if (*p != ';' && *p != ',') break;
            ++p;
        }
    }
    std::fprintf(stderr, "[staystar] on=%d log=%d spawn=%d touches=%d wall=%lld\n", (int)g.on, (int)g.log, g.sp_cnt, g.ntc, wall_ms());
}

bool in_room() {
    if (std::getenv("SM64DS_COOP") || std::getenv("SM64DS_PARTY")) return true;
    const char *k = std::getenv("SM64DS_ROOM_KIND");
    return k && *k;
}

int collected_mask() {
    int m = 0;
    for (int i = 0; i < 8; ++i) if (IsStarCollectedInCurLevel(i)) m |= 1 << i;
    return m;
}

// Is a collect of MINE running on this console? Also lists every collect state (for the log).
bool mine_running(const char *me, char *why, int cap) {
    bool mine = false;
    int k = 0;
    why[0] = 0;
    for (const char *a = (const char *)_ZN8dActor_c4NextEPKS_(0); a; a = (const char *)_ZN8dActor_c4NextEPKS_(a)) {
        const unsigned c = cls_of(a);
        if ((c != 178 && c != 179) || dead(a)) continue;
        const unsigned st = *(const unsigned *)(a + kOffState);
        if (!collect_state(st)) continue;
        const void *pl = *(void *const *)(a + kOffPlayer);
        if (pl && pl == me) mine = true;
        if (k < cap - 40) k += std::snprintf(why + k, (size_t)(cap - k), " %s%u(st=%u by seat %d)", c == 178 ? "star" : "silver",
                                             (unsigned)*(const unsigned char *)(a + kOffStarId), st, seat_of(pl));
    }
    return mine;
}

void log_tick(const char *me) {
    const int taking = _ZN5Event6GetBitEj(kBitTaking) ? 1 : 0, hud = _ZN5Event6GetBitEj(kBitHud) ? 1 : 0;
    const int cut = (data_0209b454 & kCutscene) ? 1 : 0, noctl = me ? (int)*(const unsigned char *)(me + kOffNoCtl) : -1;
    const int mask = collected_mask(), lv = (int)data_0209f2f8;
    if (taking == g.l_taking && hud == g.l_hud && cut == g.l_cut && noctl == g.l_noctl && mask == g.l_mask && lv == g.l_level) return;
    g.l_taking = taking; g.l_hud = hud; g.l_cut = cut; g.l_noctl = noctl; g.l_mask = mask; g.l_level = lv;
    char why[256];
    mine_running(me, why, (int)sizeof why);
    std::fprintf(stderr, "[staystar] state level=%d bit1e=%d bit1d=%d freeze=%d my_noctl=%d collected=%02x collects:%s wall=%lld\n",
                 lv, taking, hud, cut, noctl, mask, why[0] ? why : " none", wall_ms());
}

void fix_tick(char *me) {
    const bool set = _ZN5Event6GetBitEj(kBitTaking) || _ZN5Event6GetBitEj(kBitHud);
    if (!set) { g.quiet = 0; return; }
    char why[256];
    const bool busy = mine_running(me, why, (int)sizeof why) || (me && *(const unsigned char *)(me + kOffNoCtl) != 0) ||
                      (signed char)data_02092110[0] >= 0;
    if (busy) { g.quiet = 0; return; }
    if (++g.quiet < kQuietFrames) return;
    g.quiet = 0;
    const int t = _ZN5Event6GetBitEj(kBitTaking) ? 1 : 0, h = _ZN5Event6GetBitEj(kBitHud) ? 1 : 0;
    _ZN5Event8ClearBitEj(kBitTaking);
    _ZN5Event8ClearBitEj(kBitHud);
    ++g.fixes;
    std::fprintf(stderr, "[staystar] FIX: star-get bits left set with no collect of mine running (bit1e=%d bit1d=%d, other collects:%s): "
                 "cleared, level %d, fix #%u wall=%lld\n", t, h, why[0] ? why : " none", (int)data_0209f2f8, g.fixes, wall_ms());
}

char *find_star(int n) {
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if (cls_of(a) == 178 && !dead(a) && (*(const unsigned char *)(a + kOffStarId) & 0xf) == (unsigned)n) return a;
    return nullptr;
}

void knob_tick(char *me) {
    if (g.sp_lvl < 0 && g.ntc == 0) return;
    const int lv = (int)data_0209f2f8;
    if (lv != g.lvl_seen) { g.lvl_seen = lv; g.lvl_frames = 0; }
    ++g.lvl_frames;
    if (g.sp_lvl == lv && !g.sp_done && g.lvl_frames >= g.sp_fr) {
        g.sp_done = true;
        const int *mp = (const int *)(me + 0x5c);
        for (int i = 0; i < g.sp_cnt; ++i) {
            const int pos[3] = { mp[0] + (i + 1) * 500 * 4096, mp[1] + 120 * 4096, mp[2] };
            char *a = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(178, (unsigned)(g.sp_n[i] & 0xf), pos, nullptr, (signed char)me[0xcc], -1);
            std::fprintf(stderr, "[staystar] SPAWN knob: star %d at %d,%d,%d -> %p wall=%lld\n", g.sp_n[i], pos[0] >> 12, pos[1] >> 12,
                         pos[2] >> 12, (void *)a, wall_ms());
        }
    }
    for (int i = 0; i < g.ntc; ++i) {
        State::Touch &t = g.tc[i];
        if (t.done || t.lvl != lv || g.lvl_frames < t.fr || (g.lvl_frames - t.fr) % 60 != 0) continue;
        char *s = find_star(t.n);
        const unsigned st = s ? *(const unsigned *)(s + kOffState) : 0u;
        if (s && *(void *const *)(s + kOffPlayer) == me && collect_state(st)) {
            t.done = true;
            std::fprintf(stderr, "[staystar] TOUCH knob: star %d is being got by me (state %u) wall=%lld\n", t.n, st, wall_ms());
            continue;
        }
        if (t.tries >= kTouchTries) {
            t.done = true;
            std::fprintf(stderr, "[staystar] TOUCH knob: star %d NOT got after %d tries (%s, state %u, bit1e=%d) wall=%lld\n", t.n, t.tries,
                         s ? "still here" : "gone", st, _ZN5Event6GetBitEj(kBitTaking) ? 1 : 0, wall_ms());
            continue;
        }
        if (!s) { ++t.tries; std::fprintf(stderr, "[staystar] TOUCH knob: no star %d here (try %d) wall=%lld\n", t.n, t.tries, wall_ms()); continue; }
        int *mp = (int *)(me + 0x5c);
        const int *sp = (const int *)(s + 0x5c);
        mp[0] = sp[0]; mp[1] = sp[1] - 40 * 4096; mp[2] = sp[2];
        ++t.tries;
        std::fprintf(stderr, "[staystar] TOUCH knob: me put on star %d (state %u) at %d,%d,%d, try %d, bit1e=%d wall=%lld\n", t.n, st,
                     sp[0] >> 12, sp[1] >> 12, sp[2] >> 12, t.tries, _ZN5Event6GetBitEj(kBitTaking) ? 1 : 0, wall_ms());
    }
}

void (*g_prev)(void) = nullptr;

void frame() {
    if (g_prev) g_prev();
    if (!in_room()) return;
    init_once();
    char *me = my_body();
    if (!me || data_0209f2d8[0] == 1) { g.quiet = 0; return; }
    if (g.log) log_tick(me);
    knob_tick(me);
    if (g.on) fix_tick(me);
}

struct Seat {
    Seat() { g_prev = port_sdat_frame_hook; port_sdat_frame_hook = frame; }
} g_seat;

}  // namespace
