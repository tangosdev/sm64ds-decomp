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
// ONE ANSWER PER STAR. The answer is taken when the player touches the star (the star's touched state, where the cartridge
// picks the talk state and decides between the exit and the talk) and kept for that star until it is gone, so flipping the
// setting during the dance cannot send one star down half of each path.
//
// WHAT THE PLAYER READS. The castle's talk (message 0x187) opens with the castle's own line ("You're getting stronger from
// the power of the castle"), which reads wrong in a painting world, and the cartridge has no course version of it. For a
// staying star the port shows the same message from its question on: "Do you want to save?" and its two answers. It is done
// by pointing that one message's text entry at its own question line for the length of the talk (every language: the line
// before the first answer), and putting it back as soon as the talk is over or the player leaves; nothing else reads it.
// The star-count line (func_ov002_020c6e14: 1, 3, 8, 12, 30, 50, 80 stars, the doors they open) would show at the end of the
// talk, in the course. The cartridge shows it on the castle arrival after the star's exit, and only when the count is
// exactly the milestone. So a staying star's line is held (port_star_count_msg_hook in src/game/player/Player.cpp) and
// shown on the next castle arrival instead, one per arrival, oldest first; a milestone the arrival shows on its own is
// not shown twice. The pause menu's Exit Course arrives without that test (the cartridge never needed it: no star was got
// in that visit), so there the port opens the held line itself, the same way the test does, once the player stands in the
// castle with control and no box or pause up. Held lines live for the session (not in the save file).
// The 100-coin star's own talk (message 0x186, "You've collected 100 coins!") is the cartridge's, already shown in the course;
// it keeps its own star-count line as the cartridge does, and lines held from earlier stars keep waiting for the castle.
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
//   (TOUCH with <n> = 0 takes a Power Star waiting to be picked up that the level did not place itself: one the game raised.)
//   SM64DS_STAYCOURSE_FLIP=<level>:<frame>:<0|1>   <frame> frames into <level>, the setting is switched (the GAME tab's own setter).
//   SM64DS_STAYCOURSE_COINS=<level>:<frame>:<n>   <frame> frames into <level>, GiveCoins(me, <n>) (the ROM's own writer).
//   SM64DS_STAYCOURSE_VISIT=<level>:<frame>:<class>:<count>:<every>   from <frame>, every <every> frames my body is put on
//                                                 the nearest live actor of <class> (0x120 coin, 0x121 red coin), <count>
//                                                 times: the pickups are the game's own.
//   With the LOG knob the log also names every message box that opens, and once a level the Power Stars in it.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "comms_hub.h"        // port::comms_hub_active
#include "host_settings.h"    // host_setting_stay_after_star
#include "settings_live.h"    // sl_solo_hook

extern "C" {
extern int (*port_star_stay_hook)(const void *star, int course);   // src/game/actors/d_a_star.cpp
int (*port_star_count_msg_hook)(int new_star) = 0;                   // src/game/player/Player.cpp, func_ov002_020c6e14
extern void (*port_sdat_frame_hook)(void);                           // hal/sdat/consumer.cpp: one call per game frame
extern unsigned char data_0209f2d8[];      // stage mode, 1 = VS
extern signed char data_0209f2f8;          // current level
extern signed char SUBLEVEL_LEVEL_TABLE[]; // level -> course
extern int data_0209d6fc[];                // the message bank's base (hal/message_boot.cpp)
extern int data_0209d708[];                // its entry table
extern int data_0209d70c[];                // its info section
int NumStars(void);
void *_ZN8dActor_c4NextEPKS_(const void *prev);
extern void *data_0209f394[];              // the players' bodies
extern unsigned char data_0209f250;        // my player index
extern unsigned char data_0209d660[];      // a message box is open
extern unsigned char data_0209f2c4[];      // the pause screen, 0 = playing
extern void *data_0209f318;                // the camera
extern short data_ov002_020ff1a0[];        // the star-count lines, one per milestone (func_ov002_020c6e14)
int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *thiz, void *actor, unsigned msg, const void *pos, unsigned char d,
                                                   unsigned char e);
void _ZN9dCamera_c9SetFlag_3Ev(void *cam);
#ifndef PORT_ROM_CLEAN
extern unsigned char data_0209cab4[];      // the live save block's star bytes, one per course
extern unsigned char data_02092110[];      // pending level change, < 0 = none
extern short data_0209d6d4;                // the open box's message id
extern unsigned char data_0209f2ac;        // the last star got was a new one
extern unsigned char data_0209f228;        // the last star got (its number)
extern unsigned char data_0209f20c;        // the level-clear screen owns the game
extern unsigned char data_0209f2fc;        // how this level was entered: 1 exit course, 2 death
extern short data_0209f358[];              // coins
extern signed char data_0209f30c[];        // red coins
extern int data_0209f32c[];                // the water surface, 0x80000000 = none
int _ZN5Event6GetBitEj(unsigned bit);
void GiveCoins(int idx, int amount);
char *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b, const void *pos, const void *v16, int area, int f);
void ExitLevel(void);
#endif
}

namespace {

// daStar_c (include/daStar_c.h): +0x0c class id, +0x440 mState, +0x444 mHomeState, +0x49b mTalkStep, +0x49d mStarID
enum : int { kClsPowerStar = 178, kOffCls = 0x0c, kOffState = 0x440, kOffHome = 0x444, kOffTalkStep = 0x49b, kOffStarId = 0x49d,
             kHomeMarker = 9, kStateTouched = 10, kStateSaveMessage = 11 };
enum : int { kMsgCastleStar = 0x187 };

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

int course_of(int level) { return (level >= 0 && level < 52) ? (int)SUBLEVEL_LEVEL_TABLE[level] : -1; }

bool star_alive(const void *star) {
    for (void *a = _ZN8dActor_c4NextEPKS_(0); a; a = _ZN8dActor_c4NextEPKS_(a))
        if (a == star) return true;
    return false;
}

// ---- the castle talk's text, from its question on -------------------------------------------------------------------
struct MsgPatch { bool on; unsigned *entry; unsigned old_off; };
MsgPatch g_msg;

void msg_restore() {
    if (!g_msg.on) return;
    *g_msg.entry = g_msg.old_off;
    g_msg.on = false;
}

// Points message 0x187 at its own question line: the line before the first answer (0xfe, the choice command), whatever
// the language. Lines end in 0xfd, the text in 0xff.
void msg_patch() {
    if (g_msg.on) return;
    const char *info = (const char *)(size_t)data_0209d70c[0];
    const char *base = (const char *)(size_t)data_0209d6fc[0];
    char *entries = (char *)(size_t)data_0209d708[0];
    if (!info || !base || !entries) return;
    if (*(const unsigned short *)(info + 8) <= kMsgCastleStar) return;
    unsigned *entry = (unsigned *)(entries + kMsgCastleStar * 8);
    const unsigned char *t = (const unsigned char *)(base + 0x28 + *(const unsigned *)(info + 4) + *entry);
    int choice = -1;
    for (int i = 0; i < 1024 && t[i] != 0xff; ++i)
        if (t[i] == 0xfe) { choice = i; break; }
    if (choice <= 0) return;
    int q = choice;
    while (q > 0 && t[q - 1] == 0xfd) --q;             // the end of the question line
    int start = q;
    while (start > 0 && t[start - 1] != 0xfd) --start;  // its start
    if (start <= 0 || start >= q) return;
    g_msg.entry = entry; g_msg.old_off = *entry; g_msg.on = true;
    *entry = g_msg.old_off + (unsigned)start;
}

// ---- one answer per star, taken at the touch ---------------------------------------------------------------------------
struct Latch { const void *star; int ans; int level; };
Latch g_latch[4];
const void *g_stay_star; int g_stay_level = -1;   // the staying star whose talk is on, and where

int decide(const void *star, int course) {
    if (!(host_setting_stay_after_star() && solo() && course >= 0 && course <= 14 && star)) return 0;
    const char *a = (const char *)star;
    const unsigned cls = *(const unsigned short *)(a + kOffCls);
    const unsigned id = *(const unsigned char *)(a + kOffStarId);
    const int home = *(const int *)(a + kOffHome);
    return (cls == kClsPowerStar && id >= 1 && id <= 7 && home != kHomeMarker) ? 1 : 0;
}

void latch_put(const void *star, int ans, int lv) {
    Latch *slot = nullptr;
    for (Latch &l : g_latch) if (l.star == star) { slot = &l; break; }
    if (!slot) for (Latch &l : g_latch) if (!l.star) { slot = &l; break; }
    if (!slot) {   // all four in use: drop the oldest
        for (int i = 0; i < 3; ++i) g_latch[i] = g_latch[i + 1];
        slot = &g_latch[3];
    }
    *slot = Latch{ star, ans, lv };
}

int stay_hook(const void *star, int course) {
    int ans = -1;
    const int lv = (int)data_0209f2f8;
    const bool touched = star && *(const int *)((const char *)star + kOffState) == kStateTouched;
    if (!touched)
        for (const Latch &l : g_latch)
            if (l.star && l.star == star && l.level == lv) { ans = l.ans; break; }
    if (ans < 0) {
        ans = decide(star, course);
        if (star) latch_put(star, ans, lv);
        if (ans) { g_stay_star = star; g_stay_level = lv; msg_patch(); }
    }
#ifndef PORT_ROM_CLEAN
    if (g_log && (star != g_last_star || ans != g_last_ans)) {
        g_last_star = star; g_last_ans = ans;
        std::fprintf(stderr, "[staycourse] star %d course %d level %d: %s%s\n", star ? (int)*((const unsigned char *)star + kOffStarId) : -1,
                     course, lv, ans ? "STAY (talk and save prompt, play on)" : "cartridge", touched ? " (taken at the touch)" : " (kept)");
    }
#endif
    return ans;
}

// ---- the star-count line --------------------------------------------------------------------------------------------
int g_held[8]; int g_nheld;

int count_index(int stars) {
    switch (stars) {
    case 1: return 0; case 3: return 1; case 8: return 2; case 12: return 3; case 30: return 4; case 50: return 5; case 80: return 6;
    }
    return -1;
}

bool stay_talk_on() {
    return g_stay_star && g_stay_level == (int)data_0209f2f8 && star_alive(g_stay_star) &&
           *(const int *)((const char *)g_stay_star + kOffState) == kStateSaveMessage;
}

int count_hook(int new_star) {
    if (stay_talk_on()) {
        msg_restore();                      // the talk is over by the time it asks
        const int idx = new_star ? count_index(NumStars()) : -1;
        if (idx >= 0 && g_nheld < 8) {
            bool dup = false;
            for (int i = 0; i < g_nheld; ++i) dup |= g_held[i] == idx;
            if (!dup) g_held[g_nheld++] = idx;
        }
#ifndef PORT_ROM_CLEAN
        if (g_log) std::fprintf(stderr, "[staycourse] star-count line %d held for the castle (stars %d, held %d)\n", idx, NumStars(), g_nheld);
#endif
        return -1;
    }
    if (g_nheld == 0) return -2;
    if (course_of((int)data_0209f2f8) != 29) return -2;   // a talk the cartridge itself runs in a course (the 100-coin star): its own line, held ones wait
    const int own = new_star ? count_index(NumStars()) : -1;
    int out;
    if (own >= 0) {
        out = -2;
        for (int i = 0; i < g_nheld; ++i)
            if (g_held[i] == own) { for (int j = i; j + 1 < g_nheld; ++j) g_held[j] = g_held[j + 1]; --g_nheld; break; }
    } else {
        out = g_held[0];
        for (int j = 0; j + 1 < g_nheld; ++j) g_held[j] = g_held[j + 1];
        --g_nheld;
    }
#ifndef PORT_ROM_CLEAN
    if (g_log) std::fprintf(stderr, "[staycourse] castle arrival: star-count line %d (%s), %d still held\n", out >= 0 ? out : own,
                            out >= 0 ? "a held one" : "the cartridge's own", g_nheld);
#endif
    return out;
}

// A held star-count line, opened in the castle once the player can take it (see the head of the file).
int g_tick_lv = -999, g_tick_frames;
void held_line_tick() {
    if (g_nheld == 0 || course_of((int)data_0209f2f8) != 29 || g_tick_frames < 90 || g_tick_frames % 30 != 0) return;
    if (data_0209d660[0] || data_0209f2c4[0]) return;
    const int me = (int)data_0209f250;
    char *body = (me >= 0 && me < 16) ? (char *)data_0209f394[me] : nullptr;
    if (!body || *(const unsigned char *)(body + 0x709) != 0) return;   // Player::mIsNoControl
    const int idx = g_held[0];
    if (!_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(body, body, (unsigned)(unsigned short)data_ov002_020ff1a0[idx], nullptr, 0, 2))
        return;   // not now (in the air, say): the next try
    _ZN9dCamera_c9SetFlag_3Ev(data_0209f318);
    for (int j = 0; j + 1 < g_nheld; ++j) g_held[j] = g_held[j + 1];
    --g_nheld;
#ifndef PORT_ROM_CLEAN
    if (g_log) std::fprintf(stderr, "[staycourse] castle: held star-count line %d opened (message 0x%x), %d still held\n", idx,
                            (unsigned)(unsigned short)data_ov002_020ff1a0[idx], g_nheld);
#endif
}

// Once a frame: the text patch and the latches never outlive their star or their level.
void stay_tick() {
    const int lv = (int)data_0209f2f8;
    if (lv != g_tick_lv) { g_tick_lv = lv; g_tick_frames = 0; }
    ++g_tick_frames;
    held_line_tick();
    if (g_stay_star && (g_stay_level != lv || !star_alive(g_stay_star))) { g_stay_star = nullptr; g_stay_level = -1; }
    if (g_msg.on && !g_stay_star) msg_restore();
    for (Latch &l : g_latch)
        if (l.star && l.level != lv) l = Latch{};
}

#ifndef PORT_ROM_CLEAN
enum : int { kMaxSteps = 8, kTouchTries = 6 };
struct Knobs {
    bool inited;
    int lvl_seen, lvl_frames;
    int sp_lvl, sp_fr, sp_n[kMaxSteps], sp_cnt; bool sp_done;
    struct Touch { int lvl, fr, n, tries; bool done; } tc[kMaxSteps]; int ntc;
    int ex_lvl, ex_fr; bool ex_done;
    int co_lvl, co_fr, co_n; bool co_done;
    int fl_lvl, fl_fr, fl_v; bool fl_done;
    int vi_lvl, vi_fr, vi_cls, vi_count, vi_every, vi_done;
    int l_msg_open, l_msg_id, l_coins, l_red, census_lvl;
    unsigned census_mask;               // the stars the level placed itself (TOUCH 0 skips them)
    int global;                         // frames since the first one (the picture knobs count these)
    int l_level, l_stars, l_b1e, l_b1d, l_noctl;
};
Knobs K;

char *my_body() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < 16) ? (char *)data_0209f394[me] : nullptr;
}
char *find_star(int n) {
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if (*(const unsigned short *)(a + kOffCls) == kClsPowerStar && *(const unsigned char *)(a + 0x0f) == 0) {
            const unsigned id = *(const unsigned char *)(a + kOffStarId) & 0xf;
            if (n == 0 ? (*(const int *)(a + kOffState) == 4 && !(K.census_mask & (1u << id))) : id == (unsigned)n) return a;
        }
    return nullptr;
}

char *nearest_of(int cls, const char *me) {
    char *best = nullptr; double bd = 0;
    const int *mp = (const int *)(me + 0x5c);
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (*(const unsigned short *)(a + kOffCls) != (unsigned)cls || *(const unsigned char *)(a + 0x0f) != 0) continue;
        const int *ap = (const int *)(a + 0x5c);
        const double dx = (ap[0] - mp[0]) / 4096.0, dy = (ap[1] - mp[1]) / 4096.0, dz = (ap[2] - mp[2]) / 4096.0;
        const double d = dx * dx + dy * dy + dz * dz;
        if (d < 1.0) continue;   // the one I stand on is being picked up
        if (!best || d < bd) { best = a; bd = d; }
    }
    return best;
}

void knobs_init() {
    K.inited = true;
    K.lvl_seen = -999; K.sp_lvl = -1; K.ex_lvl = -1; K.co_lvl = -1; K.vi_lvl = -1; K.fl_lvl = -1;
    K.l_msg_open = K.l_msg_id = K.l_coins = K.l_red = -9; K.census_lvl = -999;
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
    if (const char *e = std::getenv("SM64DS_STAYCOURSE_FLIP"))
        if (std::sscanf(e, "%d:%d:%d", &K.fl_lvl, &K.fl_fr, &K.fl_v) != 3) K.fl_lvl = -1;
    if (const char *e = std::getenv("SM64DS_STAYCOURSE_COINS"))
        if (std::sscanf(e, "%d:%d:%d", &K.co_lvl, &K.co_fr, &K.co_n) != 3) K.co_lvl = -1;
    if (const char *e = std::getenv("SM64DS_STAYCOURSE_VISIT"))
        if (std::sscanf(e, "%d:%d:%i:%d:%d", &K.vi_lvl, &K.vi_fr, &K.vi_cls, &K.vi_count, &K.vi_every) != 5 || K.vi_every <= 0) K.vi_lvl = -1;
    std::fprintf(stderr, "[staycourse] knobs: log=%d spawn=%d touches=%d exit=%d:%d coins=%d:%d:%d visit=%d:%d:0x%x:%d:%d setting=%d solo=%d\n",
                 (int)g_log, K.sp_cnt, K.ntc, K.ex_lvl, K.ex_fr, K.co_lvl, K.co_fr, K.co_n, K.vi_lvl, K.vi_fr, K.vi_cls, K.vi_count,
                 K.vi_every, host_setting_stay_after_star(), solo());
}

void log_tick(char *me) {
    const int lv = (int)data_0209f2f8;
    const int mo = data_0209d660[0] ? 1 : 0, mid = mo ? (int)data_0209d6d4 : -1;
    if (mo != K.l_msg_open || mid != K.l_msg_id) {
        K.l_msg_open = mo; K.l_msg_id = mid;
        std::fprintf(stderr, "[staycourse] message %s 0x%x (level %d, frame %d, global %d, new_star=%d last_star=%d clear=%d entry=%d stars=%d held=%d)\n",
                     mo ? "OPEN" : "closed", mid & 0xffff, lv, K.lvl_frames, K.global, (int)data_0209f2ac, (int)data_0209f228, (int)data_0209f20c,
                     (int)data_0209f2fc, NumStars(), g_nheld);
    }
    const int me_i = (int)data_0209f250;
    const int coins = data_0209f358[me_i], red = data_0209f30c[me_i];
    if (coins != K.l_coins || red != K.l_red) {
        K.l_coins = coins; K.l_red = red;
        std::fprintf(stderr, "[staycourse] coins %d red %d (level %d, frame %d)\n", coins, red, lv, K.lvl_frames);
    }
    if (K.census_lvl != lv && K.lvl_frames >= 120) {
        K.census_lvl = lv;
        K.census_mask = 0;
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a))
            if (*(const unsigned short *)(a + kOffCls) == kClsPowerStar) {
                K.census_mask |= 1u << (*(const unsigned char *)(a + kOffStarId) & 0xf);
                const int *ap = (const int *)(a + 0x5c);
                std::fprintf(stderr, "[staycourse] census level %d: PowerStar %d state %d home %d at %d,%d,%d (water surface %d)\n", lv,
                             (int)*(const unsigned char *)(a + kOffStarId), *(const int *)(a + kOffState), *(const int *)(a + kOffHome),
                             ap[0] >> 12, ap[1] >> 12, ap[2] >> 12, data_0209f32c[0] == (int)0x80000000 ? -99999 : data_0209f32c[0] >> 12);
            }
    }
    const int course = (lv >= 0 && lv < 52) ? (int)SUBLEVEL_LEVEL_TABLE[lv] : -1;
    const int stars = (course >= 0 && course < 30) ? (int)data_0209cab4[course] : -1;
    const int b1e = _ZN5Event6GetBitEj(0x1e) ? 1 : 0, b1d = _ZN5Event6GetBitEj(0x1d) ? 1 : 0;
    const int noctl = me ? (int)*(const unsigned char *)(me + 0x709) : -1;
    if (lv == K.l_level && stars == K.l_stars && b1e == K.l_b1e && b1d == K.l_b1d && noctl == K.l_noctl) return;
    K.l_level = lv; K.l_stars = stars; K.l_b1e = b1e; K.l_b1d = b1d; K.l_noctl = noctl;
    std::fprintf(stderr, "[staycourse] state level=%d course=%d course_stars=%02x bit1e=%d bit1d=%d my_noctl=%d frames_in_level=%d global=%d\n", lv,
                 course, stars & 0xff, b1e, b1d, noctl, K.lvl_frames, K.global);
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
        if (t.done || t.lvl != lv || K.lvl_frames < t.fr) continue;
        const int phase = (K.lvl_frames - t.fr) % 60;   // each try holds me on the star for 20 frames (water lifts a swimmer off it)
        if (phase >= 20) continue;
        char *s = find_star(t.n);
        const unsigned st = s ? *(const unsigned *)(s + 0x440) : 0u;
        if (s && *(void *const *)(s + 0x438) == me && st >= 5) {
            t.done = true;
            std::fprintf(stderr, "[staycourse] TOUCH knob: star %d is being got by me (state %u)\n", t.n, st);
            continue;
        }
        if (phase == 0 && t.tries >= kTouchTries) {
            t.done = true;
            std::fprintf(stderr, "[staycourse] TOUCH knob: star %d NOT got after %d tries (%s, state %u, bit1e=%d)\n", t.n, t.tries,
                         s ? "still here" : "gone", st, _ZN5Event6GetBitEj(0x1e) ? 1 : 0);
            continue;
        }
        if (phase == 0) ++t.tries;
        if (!s) { if (phase == 0) std::fprintf(stderr, "[staycourse] TOUCH knob: no star %d here (try %d)\n", t.n, t.tries); continue; }
        int *mp = (int *)(me + 0x5c);
        const int *sp = (const int *)(s + 0x5c);
        mp[0] = sp[0]; mp[1] = sp[1] - 40 * 4096; mp[2] = sp[2];
        if (phase == 0) std::fprintf(stderr, "[staycourse] TOUCH knob: me put on star %d (state %u) at %d,%d,%d, try %d\n", t.n, st, sp[0] >> 12,
                     sp[1] >> 12, sp[2] >> 12, t.tries);
    }
    if (K.fl_lvl == lv && !K.fl_done && K.lvl_frames >= K.fl_fr) {
        K.fl_done = true;
        host_setting_set_stay_after_star(K.fl_v);
        std::fprintf(stderr, "[staycourse] FLIP knob: the setting is now %d (as the GAME tab would set it)\n", host_setting_stay_after_star());
    }
    if (K.co_lvl == lv && !K.co_done && K.lvl_frames >= K.co_fr) {
        K.co_done = true;
        GiveCoins((int)data_0209f250, K.co_n);
        std::fprintf(stderr, "[staycourse] COINS knob: GiveCoins(me, %d) -> %d\n", K.co_n, (int)data_0209f358[(int)data_0209f250]);
    }
    if (K.vi_lvl == lv && K.vi_done < K.vi_count && K.lvl_frames >= K.vi_fr && (K.lvl_frames - K.vi_fr) % K.vi_every == 0) {
        ++K.vi_done;
        if (char *a = nearest_of(K.vi_cls, me)) {
            int *mp = (int *)(me + 0x5c);
            const int *ap = (const int *)(a + 0x5c);
            mp[0] = ap[0]; mp[1] = ap[1] - 20 * 4096; mp[2] = ap[2];
            std::fprintf(stderr, "[staycourse] VISIT knob %d/%d: me put on class 0x%x at %d,%d,%d\n", K.vi_done, K.vi_count, K.vi_cls,
                         ap[0] >> 12, ap[1] >> 12, ap[2] >> 12);
        } else {
            std::fprintf(stderr, "[staycourse] VISIT knob %d/%d: no class 0x%x left\n", K.vi_done, K.vi_count, K.vi_cls);
        }
    }
    if (K.ex_lvl == lv && !K.ex_done && K.lvl_frames >= K.ex_fr && (signed char)data_02092110[0] < 0) {
        K.ex_done = true;
        std::fprintf(stderr, "[staycourse] EXIT knob: ExitLevel() (the pause menu's Exit Course) in level %d\n", lv);
        ExitLevel();
    }
}
#endif  // PORT_ROM_CLEAN

void (*g_prev)(void) = nullptr;

void frame() {
    if (g_prev) g_prev();
    stay_tick();
#ifndef PORT_ROM_CLEAN
    if (!K.inited) knobs_init();
    ++K.global;
    char *me = my_body();
    if (!me) return;
    if (g_log) log_tick(me);
    knob_tick(me);
#endif
}

struct Install {
    Install() {
#ifndef PORT_ROM_CLEAN
        g_log = env_set("SM64DS_STAYCOURSE_LOG");
#endif
        g_prev = port_sdat_frame_hook; port_sdat_frame_hook = frame;
        port_star_stay_hook = &stay_hook;
        port_star_count_msg_hook = &count_hook;
        sl_solo_hook = &solo;
    }
} g_install;

}  // namespace
