// Run coopfix1, lane STAYCOURSE1: STAY IN THE COURSE AFTER A STAR (single player, setting StayAfterStar, off by default).
//
// WHAT TANGO ASKED FOR (11-13): players want to stay in a course after getting a star, as a solo-only setting.
//
// HOW. The cartridge already has a "keep playing after a star" flow: in the castle and in the Bowser stages a power star does
// its dance, then the star's talk and save prompt (message 0x187), then the player plays on. The whole flow keys off one test,
// daStar_c::func_ov002_020e9630 ("this level keeps playing", levels 0xf-0x14 and 0x1d; src/game/actors/d_a_star.cpp): through it
// the classifier func_ov002_020e73ac gives talk state 1 instead of 0 (Player::func_ov002_020c94a4 calls ExitLevel only for 0 and
// 3), the collect (func_ov002_020e8ef0) records the coin score and the coin-reward marker at once, and the hold state goes to the
// save prompt and the star's own last step (func_ov002_020e8618), which clears the star-get bits 0x1e / 0x1d so the next star in
// the same visit can be got. The setting adds the course the player stands in to that test, through the _MSC_VER hook
// port_star_stay_hook. Nothing else changes: the star counts and saves the way the castle's stars do.
//
// WHERE IT APPLIES. All of these, else the hook says 0 and the cartridge runs untouched:
//   - the setting is on (host_setting_stay_after_star);
//   - this console plays alone: no room of any kind (coop, party, world / Adventure Online), no VS match. A room has its own
//     star-exit rules (hal/star_popup.cpp) and they must not change;
//   - the course is one of the 15 main courses (SUBLEVEL_LEVEL_TABLE course 0..14, sub-areas included). The castle (29) and
//     the Bowser stages and character bosses (15..20) already keep playing on the cartridge; the secret courses (21..28: the
//     slides, the switches, the aquarium, the moat, the wing cap) keep the cartridge's exit;
//   - the star is a plain power star (class 178) with star number 1..7, not placed by a marker (mHomeState 9) and not a silver
//     star. Star 0 already talks on the cartridge, star 8 is a Bowser key (talk state 3, always exits) and the classifier never
//     asks for the others.
//
// The act of the course does not change mid-visit (the DS loads a course for one act): objects of other acts stay as they are
// until the player leaves and comes back.
//
// Dev-only proof knobs (compiled out of PORT_ROM_CLEAN, all off unless set):
//   SM64DS_STAYCOURSE_LOG=1                       every change of the course's star byte, the star-get bits, my no-control
//                                                 byte and the level, and every stay decision.
//   SM64DS_STAYCOURSE_SPAWN=<level>:<frame>:<n>[,<n>..]   <frame> frames after my body first stands in <level>, PowerStars
//                                                 numbered <n> through the ROM's spawn, in a row 500 units apart beside me.
//   SM64DS_STAYCOURSE_TOUCH=<level>:<frame>:<n>[;..]      <frame> frames into <level>, my body is put on PowerStar <n>
//                                                 (a real touch), retried every 60 frames, 6 tries.
//   SM64DS_STAYCOURSE_EXIT=<level>:<frame>        <frame> frames into <level>: ExitLevel(), the pause menu's Exit Course.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "comms_hub.h"        // port::comms_hub_active
#include "host_settings.h"    // host_setting_stay_after_star
#include "settings_live.h"    // sl_solo_hook

extern "C" {
extern int (*port_star_stay_hook)(const void *star, int course);   // src/game/actors/d_a_star.cpp
extern void (*port_sdat_frame_hook)(void);                           // hal/sdat/consumer.cpp: one call per game frame
extern unsigned char data_0209f2d8[];      // stage mode, 1 = VS
extern signed char data_0209f2f8;          // current level
#ifndef PORT_ROM_CLEAN
extern void *data_0209f394[];
extern unsigned char data_0209f250;        // my player index
extern signed char SUBLEVEL_LEVEL_TABLE[]; // level -> course
extern unsigned char data_0209cab4[];      // the live save block's star bytes, one per course
extern unsigned char data_02092110[];      // pending level change, < 0 = none
int _ZN5Event6GetBitEj(unsigned bit);
void *_ZN8dActor_c4NextEPKS_(const void *prev);
char *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b, const void *pos, const void *v16, int area, int f);
void ExitLevel(void);
#endif
}

namespace {

// daStar_c (include/daStar_c.h): +0x0c class id, +0x444 mHomeState, +0x49d mStarID
enum : int { kClsPowerStar = 178, kOffCls = 0x0c, kOffHome = 0x444, kOffStarId = 0x49d, kHomeMarker = 9 };

bool env_set(const char *n) {
    const char *e = std::getenv(n);
    return e && *e && !(e[0] == '0' && e[1] == 0);
}

// Alone: no room of any kind and no VS match. The environment names are the ones the launcher and the in-game lobby put in
// for every room (the same test hal/coop_staystar.cpp uses), the hub covers a room joined in place.
int solo() {
    if (data_0209f2d8[0] == 1) return 0;
    if (port::comms_hub_active()) return 0;
    if (env_set("SM64DS_COOP") || env_set("SM64DS_PARTY") || env_set("SM64DS_VS_MAP")) return 0;
    const char *k = std::getenv("SM64DS_ROOM_KIND");
    if (k && *k) return 0;
    return 1;
}

#ifndef PORT_ROM_CLEAN
bool g_log;
const void *g_last_star; int g_last_ans = -1;
#endif

int stay_hook(const void *star, int course) {
    int ans = 0;
    if (host_setting_stay_after_star() && solo() && course >= 0 && course <= 14 && star) {
        const char *a = (const char *)star;
        const unsigned cls = *(const unsigned short *)(a + kOffCls);
        const unsigned id = *(const unsigned char *)(a + kOffStarId);
        const int home = *(const int *)(a + kOffHome);
        ans = (cls == kClsPowerStar && id >= 1 && id <= 7 && home != kHomeMarker) ? 1 : 0;
    }
#ifndef PORT_ROM_CLEAN
    if (g_log && (star != g_last_star || ans != g_last_ans)) {
        g_last_star = star; g_last_ans = ans;
        std::fprintf(stderr, "[staycourse] star %d course %d level %d: %s\n", star ? (int)*((const unsigned char *)star + kOffStarId) : -1,
                     course, (int)data_0209f2f8, ans ? "STAY (talk and save prompt, play on)" : "cartridge");
    }
#endif
    return ans;
}

#ifndef PORT_ROM_CLEAN
enum : int { kMaxSteps = 8, kTouchTries = 6 };
struct Knobs {
    bool inited;
    int lvl_seen, lvl_frames;
    int sp_lvl, sp_fr, sp_n[kMaxSteps], sp_cnt; bool sp_done;
    struct Touch { int lvl, fr, n, tries; bool done; } tc[kMaxSteps]; int ntc;
    int ex_lvl, ex_fr; bool ex_done;
    int l_level, l_stars, l_b1e, l_b1d, l_noctl;
};
Knobs K;

char *my_body() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < 16) ? (char *)data_0209f394[me] : nullptr;
}
char *find_star(int n) {
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if (*(const unsigned short *)(a + kOffCls) == kClsPowerStar && *(const unsigned char *)(a + 0x0f) == 0 &&
            (*(const unsigned char *)(a + kOffStarId) & 0xf) == (unsigned)n) return a;
    return nullptr;
}

void knobs_init() {
    K.inited = true;
    K.lvl_seen = -999; K.sp_lvl = -1; K.ex_lvl = -1;
    K.l_level = K.l_stars = K.l_b1e = K.l_b1d = K.l_noctl = -9;
    if (const char *e = std::getenv("SM64DS_STAYCOURSE_SPAWN")) {
        int used = 0;
        if (std::sscanf(e, "%d:%d:%n", &K.sp_lvl, &K.sp_fr, &used) == 2 && used > 0) {
            const char *p = e + used;
            while (*p && K.sp_cnt < kMaxSteps) {
                char *end; const long v = std::strtol(p, &end, 10);
                if (end == p) break;
                K.sp_n[K.sp_cnt++] = (int)v;
                p = end; if (*p == ',') ++p;
            }
        } else K.sp_lvl = -1;
    }
    if (const char *e = std::getenv("SM64DS_STAYCOURSE_TOUCH")) {
        const char *p = e; int used = 0;
        while (K.ntc < kMaxSteps) {
            Knobs::Touch t{}; used = 0;
            if (std::sscanf(p, "%d:%d:%d%n", &t.lvl, &t.fr, &t.n, &used) != 3 || used <= 0) break;
            K.tc[K.ntc++] = t;
            p += used;
            if (*p != ';' && *p != ',') break;
            ++p;
        }
    }
    if (const char *e = std::getenv("SM64DS_STAYCOURSE_EXIT"))
        if (std::sscanf(e, "%d:%d", &K.ex_lvl, &K.ex_fr) != 2) K.ex_lvl = -1;
    std::fprintf(stderr, "[staycourse] knobs: log=%d spawn=%d touches=%d exit=%d:%d setting=%d solo=%d\n", (int)g_log, K.sp_cnt, K.ntc,
                 K.ex_lvl, K.ex_fr, host_setting_stay_after_star(), solo());
}

void log_tick(char *me) {
    const int lv = (int)data_0209f2f8;
    const int course = (lv >= 0 && lv < 52) ? (int)SUBLEVEL_LEVEL_TABLE[lv] : -1;
    const int stars = (course >= 0 && course < 30) ? (int)data_0209cab4[course] : -1;
    const int b1e = _ZN5Event6GetBitEj(0x1e) ? 1 : 0, b1d = _ZN5Event6GetBitEj(0x1d) ? 1 : 0;
    const int noctl = me ? (int)*(const unsigned char *)(me + 0x709) : -1;
    if (lv == K.l_level && stars == K.l_stars && b1e == K.l_b1e && b1d == K.l_b1d && noctl == K.l_noctl) return;
    K.l_level = lv; K.l_stars = stars; K.l_b1e = b1e; K.l_b1d = b1d; K.l_noctl = noctl;
    std::fprintf(stderr, "[staycourse] state level=%d course=%d course_stars=%02x bit1e=%d bit1d=%d my_noctl=%d frames_in_level=%d\n", lv,
                 course, stars & 0xff, b1e, b1d, noctl, K.lvl_frames);
}

void knob_tick(char *me) {
    const int lv = (int)data_0209f2f8;
    if (lv != K.lvl_seen) { K.lvl_seen = lv; K.lvl_frames = 0; }
    ++K.lvl_frames;
    if (K.sp_lvl == lv && !K.sp_done && K.lvl_frames >= K.sp_fr) {
        K.sp_done = true;
        const int *mp = (const int *)(me + 0x5c);
        for (int i = 0; i < K.sp_cnt; ++i) {
            const int pos[3] = { mp[0] + (i + 1) * 500 * 4096, mp[1] + 120 * 4096, mp[2] };
            char *a = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(kClsPowerStar, (unsigned)(K.sp_n[i] & 0xf), pos, nullptr,
                                                                     (signed char)me[0xcc], -1);
            std::fprintf(stderr, "[staycourse] SPAWN knob: star %d at %d,%d,%d -> %p\n", K.sp_n[i], pos[0] >> 12, pos[1] >> 12,
                         pos[2] >> 12, (void *)a);
        }
    }
    for (int i = 0; i < K.ntc; ++i) {
        Knobs::Touch &t = K.tc[i];
        if (t.done || t.lvl != lv || K.lvl_frames < t.fr || (K.lvl_frames - t.fr) % 60 != 0) continue;
        char *s = find_star(t.n);
        const unsigned st = s ? *(const unsigned *)(s + 0x440) : 0u;
        if (s && *(void *const *)(s + 0x438) == me && st >= 5) {
            t.done = true;
            std::fprintf(stderr, "[staycourse] TOUCH knob: star %d is being got by me (state %u)\n", t.n, st);
            continue;
        }
        if (t.tries >= kTouchTries) {
            t.done = true;
            std::fprintf(stderr, "[staycourse] TOUCH knob: star %d NOT got after %d tries (%s, state %u, bit1e=%d)\n", t.n, t.tries,
                         s ? "still here" : "gone", st, _ZN5Event6GetBitEj(0x1e) ? 1 : 0);
            continue;
        }
        ++t.tries;
        if (!s) { std::fprintf(stderr, "[staycourse] TOUCH knob: no star %d here (try %d)\n", t.n, t.tries); continue; }
        int *mp = (int *)(me + 0x5c);
        const int *sp = (const int *)(s + 0x5c);
        mp[0] = sp[0]; mp[1] = sp[1] - 40 * 4096; mp[2] = sp[2];
        std::fprintf(stderr, "[staycourse] TOUCH knob: me put on star %d (state %u) at %d,%d,%d, try %d\n", t.n, st, sp[0] >> 12,
                     sp[1] >> 12, sp[2] >> 12, t.tries);
    }
    if (K.ex_lvl == lv && !K.ex_done && K.lvl_frames >= K.ex_fr && (signed char)data_02092110[0] < 0) {
        K.ex_done = true;
        std::fprintf(stderr, "[staycourse] EXIT knob: ExitLevel() (the pause menu's Exit Course) in level %d\n", lv);
        ExitLevel();
    }
}
#endif  // PORT_ROM_CLEAN

#ifndef PORT_ROM_CLEAN
void (*g_prev)(void) = nullptr;

void frame() {
    if (g_prev) g_prev();
    if (!K.inited) knobs_init();
    char *me = my_body();
    if (!me) return;
    if (g_log) log_tick(me);
    knob_tick(me);
}
#endif

struct Install {
    Install() {
#ifndef PORT_ROM_CLEAN
        g_log = env_set("SM64DS_STAYCOURSE_LOG");
        g_prev = port_sdat_frame_hook; port_sdat_frame_hook = frame;
#endif
        port_star_stay_hook = &stay_hook;
        sl_solo_hook = &solo;
    }
} g_install;

}  // namespace
