// TP to friends. What it does and the toggle: hal/coop_tp.h.
#include "coop_tp.h"
#include "comms_hub.h"
#include "coop_wire.h"
#include "coop_world.h"
#include "hub_presence.h"
#include "player_list.h"
#include <windows.h>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern signed char data_0209f2f8;
extern int data_0209f220[];
extern signed char data_0209f250;
extern void *data_0209f394[];
extern unsigned char data_0209f2d8[];     // stage mode
extern unsigned char data_0209d660[];     // message window
extern unsigned char data_0209d4b0[];     // transition / hold
extern unsigned char data_0209f294[];     // star select
extern unsigned char data_0209f2c4[];     // pause menu
extern unsigned char data_0209b454[];     // fade / scene change flags
extern unsigned char data_0209fc48[];     // cutscene
extern signed char data_02092118;      // saved sublevel for the no-return chain
extern signed char data_0209211c;      // castle sublevel to come back to
void LoadLevel(unsigned level, unsigned entrance, unsigned star, unsigned unk, unsigned retEntrance);
extern unsigned char data_02092110[];     // pending level change, <0 = none
extern unsigned char data_0209f22c[];     // hold timer
void LoadLevelNoReturn(unsigned level, unsigned entrance, unsigned star, unsigned unk);
int port_level_is_mounted(int level);
void port_chat_system_line(const char *text);
int port_coop_late_joiner(void);
}

namespace port {
namespace {

enum : int { kSubToggle = 0x40, kSettleMs = 1500, kPresMaxAgeMs = 4000, kToggleEveryMs = 3000, kSeats = 4, kHostSeat = 0, kHeardMaxAgeMs = 10000 };

struct Seen { int level, act; unsigned since; bool valid; };

struct State {
    bool env_on;                 // my own env (default on)
    bool host;                   // SM64DS_COOP_HOST
    int  heard;                  // -1 never, else the host's setting 0/1
    unsigned heard_ms;           // when the host last said it (comms_hub_now_ms)
    int  log;
    std::atomic<int> click_seat; // set by the window thread, -1 none
    Seen seen[kSeats];
    unsigned last_tx;
    int last_tx_val;
    CoopTpStats st;
    unsigned tick;
    long long tp_wall; int tp_level, tp_act;   // a TP in flight, for the landing log
    int badge_log[kSeats];
    bool inited;
    int fake_seat; unsigned fake_tick; bool fake_done;
    int follow;                  // REJOIN1 piece 5: 0 not yet looked, 1 armed, 2 done (went, or nothing to do)
    unsigned follow_since;
    int follow_entry;            // which late entry (port_coop_late_joiner) the follow belongs to
};
State g;

long long wall_ms() {
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    return (long long)((((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000ull);
}
unsigned rd32(const unsigned char *p) { unsigned v; std::memcpy(&v, p, 4); return v; }
void *my_body() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < 16) ? data_0209f394[me] : nullptr;
}

void init() {
    if (g.inited) return;
    g.inited = true;
    g.click_seat.store(-1);
    g.heard = -1;
    g.last_tx_val = -1;
    const char *e = std::getenv("SM64DS_COOP_TP");
    g.env_on = !(e && e[0] == '0' && e[1] == 0);
    const char *h = std::getenv("SM64DS_COOP_HOST");
    g.host = h && *h && !(h[0] == '0' && h[1] == 0);
    const char *l = std::getenv("SM64DS_COOP_TP_LOG");
    g.log = (l && *l && *l != '0') ? 1 : 0;
    g.fake_seat = -1;
#ifndef PORT_ROM_CLEAN
    if (const char *f = std::getenv("SM64DS_COOP_TP_FAKE")) {
        int s = -1; unsigned t = 0;
        if (std::sscanf(f, "%d@%u", &s, &t) == 2 && s >= 0 && s < kSeats) { g.fake_seat = s; g.fake_tick = t; }
    }
#endif
}

// The hub (the castle): sublevel ids 1 castle grounds (main_castle), 2 castle 1F (castle_1f, also the
// upper floors' shared overlay), 3 courtyard (main_garden), 4 basement (castle_b1), 5 second/top floor
// (castle_2f). The stage table in hal/player_list.cpp (kStage) and port_level_table in level_boot.cpp agree.
// TP works only while MY player stands in one of these; never inside a course.
bool in_hub() {
    const int lv = (int)data_0209f2f8;
    return lv >= 1 && lv <= 5;
}

bool on_now() {
    init();
    if (!coop_world_active()) return false;
    if (g.host) return g.env_on;
    // the host repeats its setting every kToggleEveryMs; silence (or the host
    // seat gone) means the last word no longer stands and my own env decides
    if (g.heard >= 0 && ((int)(comms_hub_now_ms() - g.heard_ms) > kHeardMaxAgeMs ||
                         !(comms_hub_seat_mask() & (1u << kHostSeat))))
        g.heard = -1;
    return g.heard >= 0 ? g.heard != 0 : g.env_on;
}

void say(const char *t) {
    port_chat_system_line(t);
    if (g.log) std::fprintf(stderr, "[cotp] say: %s wall=%lld\n", t, wall_ms());
}

void seat_name(int seat, char *out, int cap) {
    out[0] = 0;
    PlayerListView v;
    player_list_view(&v);
    for (int i = 0; i < v.n; ++i)
        if (v.row[i].seat == seat) {
            std::snprintf(out, (size_t)cap, "%.12s", v.row[i].name);
            return;
        }
    std::snprintf(out, (size_t)cap, "Player %d", seat + 1);
}

// ---- course -> castle painting table ---------------------------------------
// Read from the ROM's castle sublevels (1 grounds, 2 1F, 3 basement, 4 2F, 5 3F): every class 349 exit object
// whose return entrance is not 0xff is a painting or door into a course. param1 = dest << 24 | destEntrance << 16
// | retEntrance << 8. Sub-areas of a course (a later sublevel reached from inside it) come back through their
// parent course's painting. Courses not listed (the cloud, Bowser fights not entered by a castle door) keep the
// old "no return" load.
struct CourseDoor { signed char course, castle, ret; };
const CourseDoor kDoors[] = {
    // castle 1F (2)
    {6, 2, 8}, {8, 2, 9}, {10, 2, 6}, {7, 2, 7}, {35, 2, 10}, {29, 2, 14}, {30, 2, 13},
    // grounds (1)
    {32, 1, 11},
    // basement (3)
    {42, 3, 6},
    // 2F (4)
    {14, 4, 4}, {16, 4, 6}, {13, 4, 7}, {18, 4, 8}, {37, 4, 9},
    // 3F (5)
    {24, 5, 7}, {21, 5, 4}, {22, 5, 5}, {19, 5, 6}, {25, 5, 7}, {27, 5, 8}, {39, 5, 9}, {28, 5, 10}, {34, 5, 12}, {48, 5, 11},
    // sub-areas, through the parent course's painting
    {9, 2, 9}, {11, 2, 6}, {15, 4, 4}, {17, 4, 6}, {20, 5, 6}, {23, 5, 5}, {26, 5, 7}, {33, 4, 7},
};

// Why this console cannot leave now; null = it can.
const char *my_gate() {
    if (!my_body()) return "nobody";
    if (data_0209f2f8 <= 0) return "not in a level";
    if ((signed char)data_02092110[0] >= 0) return "leaving";
    if (data_0209f2d8[0] == 2) return "mode";
    if (data_0209d660[0]) return "message";
    if (rd32(data_0209d4b0)) return "d4b0";
    if (data_0209f294[0]) return "starselect";
    if (data_0209f2c4[0]) return "pause";
    if (rd32(data_0209b454)) return "fade";
    if (rd32(data_0209fc48)) return "cutscene";
    if (data_0209f22c[0]) return "timer";
    return nullptr;
}

void track_seen(unsigned now) {
    for (int s = 0; s < kSeats; ++s) {
        HubPresence p;
        Seen &z = g.seen[s];
        if (!hub_presence_get(s, &p) || !p.in_level()) { z.valid = false; continue; }
        if (!z.valid || z.level != p.level || z.act != p.act) { z.valid = true; z.level = p.level; z.act = p.act; z.since = now; }
    }
}

void try_tp(int seat, unsigned now) {
    ++g.st.clicks;
    char nm[16];
    seat_name(seat, nm, sizeof nm);
    char t[96];
    if (seat < 0 || seat >= kSeats || seat == comms_hub_my_seat()) return;
    HubPresence p; unsigned age = 0;
    if (!hub_presence_get(seat, &p, &age) || age > (unsigned)kPresMaxAgeMs || !p.in_level()) {
        std::snprintf(t, sizeof t, "Can't TP: %s is not in a level", nm); ++g.st.refused; say(t); return;
    }
    if (p.in_menu() || p.in_cutscene()) {
        std::snprintf(t, sizeof t, "Can't TP: %s is busy", nm); ++g.st.refused; say(t); return;
    }
    const Seen &z = g.seen[seat];
    if (!z.valid || (int)(now - z.since) < kSettleMs || p.level <= 0 || p.level > 51 || p.act < 0 || p.act > 7) {
        std::snprintf(t, sizeof t, "Can't TP: %s is changing level", nm); ++g.st.refused; say(t); return;
    }
    if (!port_level_is_mounted(p.level)) {
        std::snprintf(t, sizeof t, "Can't TP: %s's level is not available", nm); ++g.st.refused; say(t); return;
    }
    if (!in_hub()) { ++g.st.refused; return; }
    if (const char *why = my_gate()) {
        std::snprintf(t, sizeof t, "Can't TP right now"); ++g.st.refused; say(t);
        if (g.log) std::fprintf(stderr, "[cotp] refused: my gate = %s\n", why);
        return;
    }
    if ((int)data_0209f2f8 == p.level && (data_0209f220[0] & 0xff) == p.act) {
        std::snprintf(t, sizeof t, "You are already with %s", nm); ++g.st.refused; say(t); return;
    }
    if (g.log)
        std::fprintf(stderr, "[cotp] TP to seat %d (%s): level %d act %d (from %d act %d) wall=%lld\n", seat, nm, p.level, p.act,
                     (int)data_0209f2f8, data_0209f220[0], wall_ms());
    g.tp_wall = wall_ms(); g.tp_level = p.level; g.tp_act = p.act;
    // Landing: the level's own START (entrance 0), as entering the course from its painting. The friend's
    // position is never read here: only their level and act.
    int castle = 0, ret = 0;
    static const bool old_way = std::getenv("SM64DS_COOP_TP_NORET") != nullptr;   // proof knob: the pre-fix load
    if (!old_way && coop_castle_entrance_for_course(p.level, &castle, &ret)) {
        // Entered like the painting does: the castle sublevel and the painting's entrance are recorded, so a
        // star exit, Exit Course or death puts this player back at that painting.
        LoadLevel((unsigned)p.level, 0, (unsigned)p.act, 0, (unsigned)ret);
        data_0209211c = (signed char)castle;
        data_02092118 = -1;
        if (g.log) std::fprintf(stderr, "[cotp] return set: course %d -> castle sublevel %d entrance %d\n", p.level, castle, ret);
    } else {
        LoadLevelNoReturn((unsigned)p.level, 0, (unsigned)p.act, 0);
        if (g.log) std::fprintf(stderr, "[cotp] no painting known for course %d: no-return load\n", p.level);
    }
    ++g.st.warps;
    std::snprintf(t, sizeof t, "Going to %s", nm);
    say(t);
}

// ---- badge + click (player list API) --------------------------------------
const char *row_extra(int seat) {
    if (!on_now() || !in_hub() || seat == comms_hub_my_seat()) return "";
    HubPresence p;
    const bool show = hub_presence_get(seat, &p) && p.in_level();
    if (seat >= 0 && seat < kSeats && g.log && g.badge_log[seat] != (show ? 1 : 2)) {
        g.badge_log[seat] = show ? 1 : 2;
        std::fprintf(stderr, "[cotp] badge seat %d %s wall=%lld\n", seat, show ? "ON" : "off", wall_ms());
    }
    return show ? "TP" : "";
}
void on_click(int seat) {
    if (!on_now()) return;                     // toggle off: a click does nothing
    if (!in_hub()) {                           // raced the frame that drops the handler: one quiet line, rate-limited
        static unsigned last;
        const unsigned t = comms_hub_now_ms();
        if (!last || (int)(t - last) > 5000) { last = t ? t : 1; say("TP only works in the castle"); }
        return;
    }
    g.click_seat.store(seat);                  // the window thread: the frame acts on it
}

// ---- toggle wire: SYNB 0x40, payload u8 on, u8 host(1), u16 0 -------------
void on_ctl(int seat, int sub, const unsigned char *pl, int len, void *) {
    init();
    if (sub != kSubToggle || len != 4 || !pl || pl[1] != 1 || pl[2] || pl[3] || pl[0] > 1) return;
    if (g.host) return;                        // the host's own setting stands
    if (seat != kHostSeat) return;             // only the host seat sets it for the others
    ++g.st.toggle_rx;
    if (g.heard != (int)pl[0] && g.log)
        std::fprintf(stderr, "[cotp] host setting from seat %d: TP %s wall=%lld\n", seat, pl[0] ? "ON" : "OFF", wall_ms());
    g.heard = pl[0];
    g.heard_ms = comms_hub_now_ms();
}

// REJOIN1 piece 5: a guest that came into a running room (a late joiner, or back through JOIN GAME) lands at the
// castle. When the host is inside a course it is taken there once, by the same TP the badge does (so the castle
// painting return is set too). Off with SM64DS_COOP_FOLLOW_HOST=0, with the TP off, or with SM64DS_COOP_REJOIN=0.
// It waits quietly (no refusal lines) until the host has settled in its course and this player can leave, up to 60 s.
enum : int { kFollowGiveUpMs = 60000 };
void follow_host(unsigned now) {
    if (g.host) return;
    const int entry = port_coop_late_joiner();
    if (entry && entry != g.follow_entry) { g.follow_entry = entry; g.follow = 0; }   // a new late entry: look again
    if (g.follow == 2) return;
    if (g.follow == 0) {
        if (!entry || !in_hub()) return;
        static const char *fe = std::getenv("SM64DS_COOP_FOLLOW_HOST");
        if (!comms_hub_coop_rejoin() || (fe && fe[0] == '0' && fe[1] == 0)) { g.follow = 2; return; }
        g.follow = 1; g.follow_since = now ? now : 1;
        std::fprintf(stderr, "[cotp] REJOIN follow: late joiner at the castle, looking for the host's course\n");
    }
    if (!in_hub()) { g.follow = 2; std::fprintf(stderr, "[cotp] REJOIN follow: I left the castle myself, no follow\n"); return; }
    if ((int)(now - g.follow_since) > kFollowGiveUpMs) {
        g.follow = 2; std::fprintf(stderr, "[cotp] REJOIN follow: the host never settled in a course within 60 s, staying\n"); return;
    }
    if (!on_now()) return;
    HubPresence p; unsigned age = 0;
    if (!hub_presence_get(kHostSeat, &p, &age) || age > (unsigned)kPresMaxAgeMs || !p.in_level()) return;
    if (p.level >= 1 && p.level <= 5) {     // the host is in the castle too: nothing to follow
        g.follow = 2; std::fprintf(stderr, "[cotp] REJOIN follow: the host is in the castle (sublevel %d), no follow\n", p.level); return;
    }
    const Seen &z = g.seen[kHostSeat];
    if (p.in_menu() || p.in_cutscene() || !z.valid || (int)(now - z.since) < kSettleMs || p.level > 51 || p.act < 0 || p.act > 7) return;
    if (!port_level_is_mounted(p.level) || my_gate()) return;
    g.follow = 2;
    std::fprintf(stderr, "[cotp] REJOIN follow: the host is in level %d act %d, going there\n", p.level, p.act);
    try_tp(kHostSeat, now);
}

void on_frame(unsigned now) {
    init();
    ++g.tick;
    if (!coop_world_active()) return;
    // The handler exists only while TP is on in a coop room: with it off (or in a party
    // room, where this never runs) a click on a row is not consumed and goes on to the game.
    const bool on = on_now() && in_hub();
    if (on != player_list_has_click_handler()) player_list_set_click_handler(on ? on_click : nullptr);
    track_seen(now);
    if (g.host && ((int)(now - g.last_tx) >= kToggleEveryMs || g.last_tx_val != (g.env_on ? 1 : 0))) {
        unsigned char pl[4] = {(unsigned char)(g.env_on ? 1 : 0), 1, 0, 0};
        if (coop_send_ctl(kSubToggle, pl, 4)) { g.last_tx = now; g.last_tx_val = g.env_on ? 1 : 0; }
    }
    int s = g.click_seat.exchange(-1);
#ifndef PORT_ROM_CLEAN
    if (g.fake_seat >= 0 && !g.fake_done && g.tick >= g.fake_tick) {
        g.fake_done = true;
        if (on_now()) s = g.fake_seat; else if (g.log) std::fprintf(stderr, "[cotp] fake click ignored: TP is off\n");
    }
#endif
    if (s >= 0) try_tp(s, now);
    else follow_host(now);
}

void on_established(int level, int act) {
    if (!g.tp_wall) return;
    if (g.log)
        std::fprintf(stderr, "[cotp] landed: group established level %d act %d, %lld ms after the click (wanted level %d act %d) wall=%lld\n",
                     level, act, wall_ms() - g.tp_wall, g.tp_level, g.tp_act, wall_ms());
    g.tp_wall = 0;
}

struct Reg {
    Reg() {
        init();
        coop_on_frame(on_frame);
        coop_on_group_established(on_established);
        coop_ctl_register(kSubToggle, kSubToggle, on_ctl, nullptr);
        player_list_set_row_extra(row_extra);
    }
} g_reg;

}  // namespace

CoopTpStats coop_tp_stats() { return g.st; }

bool coop_castle_entrance_for_course(int course, int *castle_sublevel, int *return_entrance) {
    for (const CourseDoor &d : kDoors)
        if (d.course == course) {
            if (castle_sublevel) *castle_sublevel = d.castle;
            if (return_entrance) *return_entrance = d.ret;
            return true;
        }
    return false;
}
bool coop_tp_enabled() { return on_now(); }

}  // namespace port
