// PARTY SHARED STARS. Run mp1 wave 56, lane PSTAR1. The rules, the wire and
// the knobs are in hal/party_stars.h; read that first.
//
// One hook line in tests/walk_window.cpp (port_party_stars_frame, once a
// frame, right after port_hub_presence_frame so the hub's queues were just
// pumped and every presence is fresh). Everything else is in here.
//
// PORT_HOST_ABI. Reads the save record and a handful of the ROM's stage flags;
// writes one thing, the star bit of a friend's star, then calls the ROM's own
// SaveData::SaveCurrentFile.

#include "party_stars.h"
#include "star_popup.h"
#include "world_party.h"

#include "comms_hub.h"
#include "hub_presence.h"

#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern unsigned char data_0209caa0[];     // the save object; +0x14 is data_0209cab4
extern unsigned char data_0209cab4[];     // 30 course bytes, 8 star bits each
extern unsigned char data_0209f250;       // my player slot
extern void *data_0209f394[];             // the player bodies
extern signed char SUBLEVEL_LEVEL_TABLE[];
// the stage flags the ROM's own pause check reads (Stage::Behavior)
extern unsigned char data_0209f2d8[];     // stage mode (1 = normal play)
extern unsigned char data_0209d660[];     // a message window is up
extern unsigned char data_0209d4b0[];     // (s32) a transition / hold
extern unsigned char data_0209f294[];     // star select
extern unsigned char data_0209f2c4[];     // pause menu
extern unsigned char data_0209f20c[];     // level clear
extern unsigned char data_0209b454[];     // (u32) fade / scene change flags
extern unsigned char data_0209fc48[];     // (s32) cutscene
extern unsigned char data_02092110[];     // (s8) pending level change, <0 = none
extern unsigned char data_0209fc68[];     // (s32) Bowser / ending hold
extern unsigned char data_0209f22c[];     // a hold timer
unsigned port_stage_behavior_calls(void);
unsigned char NumStars(void);
void CollectStar(int course, int star);
int _ZN8SaveData15SaveCurrentFileEv(void);

// Both are defined elsewhere in the build now (adv_chat.cpp, player_list.cpp):
// the chat overlay's system line and the "BOB S1" stage code lookup. The old
// /alternatename fallbacks were inert once those landed and the link guard
// rejects them, so they are gone.
void port_chat_system_line(const char *text);
}

namespace port {
const char *player_list_stage_code(int level, int act, char *out);
}

namespace port {
namespace {

enum : unsigned { kCopyDelayMs1 = 200, kCopyDelayMs2 = 600 };

struct Pending {            // the extra copies of a star still owed
    bool     used;
    unsigned char buf[kHubPartyStarBytes];
    unsigned due_ms;
    int      left;          // copies still to send after this one
};

struct State {
    bool inited;
    bool share, accept, log;
    bool registered;
    // observer
    bool     obs_have;
    uint8_t  base[kPsCourses];
    int      obs_slot;
    int      gone_frames;
    // receiver
    PsLimiter   lim;
    PsVisits    visits;
    PsSeqMemory seqs;
    PsQueue     q;
    unsigned    epoch[kPsSeats];
    bool        epoch_have[kPsSeats];
    // frame state
    unsigned frame;
    unsigned beh_last, beh_still;
    bool     beh_seeded;
    int      safe_run;
    int      hold_left;      // after my own star
    unsigned last_apply_ms;
    bool     have_last_apply;
    unsigned seq;
    Pending  pend[4];
    PartyStarsStats st;
    // test knobs
    struct Probe { bool used, done; int course, bit, frame; } probe[8];
    int  force_unsafe_from, force_unsafe_n;
    unsigned skip_mask;      // test: conditions of unsafe_reason to ignore
    const char *trigger_path;   // test: a file that, once it exists, collects a star
    int  slotswap_frame;     // test: flip the open file slot byte at this frame
    bool slotswap_done;
    unsigned last_wait_log;
    bool no_limit;
    bool report_hooked;
    unsigned last_report_ms;
};
State g;

bool party_env() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_PARTY");
        v = (e && *e && !(e[0] == '0' && e[1] == 0)) ? 1 : 0;
    }
    return v == 1;
}

bool env_flag(const char *name, bool dflt) {
    const char *e = std::getenv(name);
    if (!e || !*e) return dflt;
    return !(e[0] == '0' && e[1] == 0);
}

void report(const char *tag);
void at_exit_report() { report("exit"); }

void init_once() {
    if (g.inited) return;
    g.inited = true;
    g.share = env_flag("SM64DS_PARTY_SHARE_STARS", true);
    g.accept = env_flag("SM64DS_PARTY_ACCEPT_STARS", true);
    g.log = env_flag("SM64DS_PARTY_STARS_LOG", false);
    g.no_limit = env_flag("SM64DS_PARTY_STARS_NO_LIMIT", false);
    g.lim.reset();
    g.visits.reset();
    g.seqs.reset();
    g.q.reset();
    g.force_unsafe_from = -1;
    g.slotswap_frame = -1;
    g.trigger_path = std::getenv("SM64DS_PARTY_STARS_TRIGGER");
    if (g.trigger_path && !*g.trigger_path) g.trigger_path = nullptr;
    if (const char *e = std::getenv("SM64DS_PARTY_STARS_SKIP")) g.skip_mask = (unsigned)std::strtoul(e, nullptr, 0);
    if (const char *e = std::getenv("SM64DS_PARTY_STARS_SLOTSWAP")) g.slotswap_frame = std::atoi(e);
    if (const char *e = std::getenv("SM64DS_PARTY_STARS_HOLD")) {
        int f = 0, n = 0;
        if (std::sscanf(e, "%d:%d", &f, &n) == 2 && f >= 0 && n > 0) {
            g.force_unsafe_from = f;
            g.force_unsafe_n = n;
        }
    }
    if (const char *e = std::getenv("SM64DS_PARTY_STARS_PROBE")) {
        const char *p = e;
        int k = 0;
        while (*p && k < 8) {
            int c = 0, b = 0, f = 0, used = 0;
            if (std::sscanf(p, "star:%d:%d@%d%n", &c, &b, &f, &used) == 3 &&
                c >= 0 && c < kPsCourses && b >= 0 && b < kPsStarBits && f >= 0) {
                g.probe[k] = {true, false, c, b, f};
                ++k;
                p += used;
            } else {
                break;
            }
            if (*p == ';') ++p;
        }
    }
    std::fprintf(stderr, "[pstar] on: share=%d accept=%d room=%d party_env=%d "
                 "no_limit=%d\n", g.share, g.accept, (int)comms_hub_room_kind(),
                 env_flag("SM64DS_PARTY", false), g.no_limit);
}

// Is this console in a live hub that is not an arena?
bool hub_live() {
    return comms_hub_active() && !comms_hub_arena() &&
           comms_hub_state() == kHubLive;
}

// Is this console in a live WORLD room with a party of its own (WPARTY2)? The
// party is the stars' audience there: a star is sent only while I am in a party
// and taken only from a seat in mine (world_party_member). Elsewhere in a world
// room nothing is sent and nothing taken.
bool world_party_room() {
    return hub_live() && comms_hub_room_kind() == kHubRoomWorld &&
           world_party_active() && world_party_in_party();
}

// Is this console in a live party room (SM64DS_PARTY, a party room), or in a
// world room while in a world party? Both share stars; the receive side adds
// the membership test for a world room (seat_ok).
bool party_room() {
    if (world_party_room()) return true;
    if (!party_env()) return false;
    return hub_live() && comms_hub_room_kind() == kHubRoomParty;
}

// May a star from `seat` be taken? A party room: any seat (the relay's room is
// the audience). A world room: only a current member of my world party.
bool seat_ok(int seat) {
    if (comms_hub_room_kind() == kHubRoomWorld) return world_party_member(seat);
    return true;
}

int my_seat() { return comms_hub_my_seat(); }

bool body_alive() {
    const int me = (int)data_0209f250;
    return me >= 0 && me < 16 && data_0209f394[me] != nullptr;
}

// The record is a loaded save file when it starts with the file magic "8000".
bool file_open() {
    return data_0209caa0[0] == '8' && data_0209caa0[1] == '0' &&
           data_0209caa0[2] == '0' && data_0209caa0[3] == '0';
}
int file_slot() { return (int)data_0209caa0[0x328]; }

unsigned rd32(const unsigned char *p) {
    unsigned v;
    std::memcpy(&v, p, 4);
    return v;
}

// SAFE FRAME: the ROM's own "may the pause menu open" test (Stage::Behavior)
// plus a normal play mode. Nothing here moves for a star-get, a level clear, a
// save prompt, a menu, a fade, a cutscene or a pending level change.
// The conditions, one bit each in SM64DS_PARTY_STARS_SKIP (a test knob: a bit
// set there ignores that condition, so a proof can tell which one is blocking).
enum : unsigned {
    kUnBody, kUnBehavior, kUnForced, kUnMode, kUnMessage, kUnD4b0, kUnStarSel,
    kUnPause, kUnClear, kUnFade, kUnCutscene, kUnChange, kUnFc68, kUnTimer,
};
const char *unsafe_reason() {
    const unsigned skip = g.skip_mask;
#define PS_UN(bit, cond, name) if (!(skip & (1u << (bit))) && (cond)) return name
    PS_UN(kUnBody, !body_alive(), "nobody");
    PS_UN(kUnBehavior, g.beh_still >= 2, "behavior");       // Stage::Behavior is not running
    if (g.force_unsafe_from >= 0 && (int)g.frame >= g.force_unsafe_from &&
        (int)g.frame < g.force_unsafe_from + g.force_unsafe_n)
        return "forced";
    PS_UN(kUnMode, data_0209f2d8[0] == 2, "mode");          // 2 = the no-pause mode (cutscene / ending)
    PS_UN(kUnMessage, data_0209d660[0] != 0, "message");    // message window
    PS_UN(kUnD4b0, rd32(data_0209d4b0) != 0, "d4b0");
    PS_UN(kUnStarSel, data_0209f294[0] != 0, "starselect"); // star select
    PS_UN(kUnPause, data_0209f2c4[0] != 0, "pause");        // pause menu
    PS_UN(kUnClear, data_0209f20c[0] != 0, "levelclear");   // level clear
    PS_UN(kUnFade, rd32(data_0209b454) != 0, "fade");       // fade / scene change
    PS_UN(kUnCutscene, rd32(data_0209fc48) != 0, "cutscene");   // cutscene
    PS_UN(kUnChange, (signed char)data_02092110[0] >= 0, "levelchange");   // level change pending
    PS_UN(kUnFc68, rd32(data_0209fc68) == 6, "fc68");       // 1 is the ordinary adventure value; 6 is the ending hold
    PS_UN(kUnTimer, data_0209f22c[0] != 0, "timer");
#undef PS_UN
    return nullptr;
}
bool frame_is_safe() { return unsafe_reason() == nullptr; }

void cleaned_name(int seat, char *out, int cap) {
    out[0] = 0;
    unsigned char id[64];
    const int n = comms_hub_latest(kHubIdent, seat, id, (int)sizeof id, nullptr);
    if (n >= (int)sizeof(HubIdentHead)) {
        HubIdentHead h;
        std::memcpy(&h, id, sizeof h);
        int nl = h.name_len;
        if (nl > kHubIdentNameMax) nl = kHubIdentNameMax;
        if ((int)sizeof h + nl > n) nl = n - (int)sizeof h;
        int k = 0;
        for (int i = 0; i < nl && k < cap - 1 && k < 16; ++i) {
            const unsigned char c = id[sizeof h + i];
            if (c >= 0x20 && c < 0x7f) out[k++] = (char)c;
        }
        out[k] = 0;
    }
    if (!out[0]) std::snprintf(out, (size_t)cap, "Player %d", seat + 1);
}

int sublevel_of_course(int course) {
    for (int i = 0; i < 52; ++i)
        if (SUBLEVEL_LEVEL_TABLE[i] == (signed char)course) return i;
    return -1;
}

void stage_text(int course, int star, char *out, int cap) {
    // STAREXIT1 S2: the star's own name, the one the coop and popup lines use (hal/star_popup.cpp star_title). The save bit
    // IS the ROM's star number (SaveData CollectStar sets 1 << star; star 7 of Bob-omb Battlefield is Behind Chain-Chomp's
    // Gate), so the old stage code "S<star + 1>" named the next star ("BOB S8"). The stage code stays as the fallback.
    star_popup_star_name(course, star, out, cap);
    if (out[0]) return;
    char code[16] = "";
    const int sub = sublevel_of_course(course);
    if (sub >= 0 && star >= 1) player_list_stage_code(sub, star, code);
    if (code[0] && std::strcmp(code, "---") != 0)
        std::snprintf(out, (size_t)cap, "%s", code);
    else
        std::snprintf(out, (size_t)cap, "course %d star %d", course, star);
}

void count_drop(PsDrop r) { ++g.st.dropped[r]; }

const char *drop_name(int r) {
    static const char *n[kPsDropReasons] = {
        "ok", "room", "accept", "bad", "seat", "nopres", "dupseq", "course",
        "nofile", "have", "queued", "gap", "seatcap", "roomcap", "full",
        "slot", "stale"};
    return (r >= 0 && r < kPsDropReasons) ? n[r] : "?";
}

void rx_log(int seat, int course, int star, PsDrop r) {
    if (g.log)
        std::fprintf(stderr, "[pstar] rx seat=%d course=%d star=%d -> %s\n",
                     seat, course, star, drop_name(r));
}

// ---------------------------------------------------------------------------
// RECEIVER: the handler for 'SYNX' (runs on the game thread, from the pump)
// ---------------------------------------------------------------------------
void refresh_seat(int seat, unsigned now) {
    // join epoch: a seat that left and came back starts fresh
    HubSeatInfo si;
    if (comms_hub_seat_info(seat, &si) && si.present) {
        if (!g.epoch_have[seat] || g.epoch[seat] != si.join_epoch) {
            g.visits.reset_seat(seat);
            std::memset(g.seqs.s[seat], 0, sizeof g.seqs.s[seat]);
            g.seqs.n[seat] = g.seqs.head[seat] = 0;
            g.epoch[seat] = si.join_epoch;
            g.epoch_have[seat] = true;
        }
    }
    HubPresence p;
    if (hub_presence_get(seat, &p, nullptr) && p.in_level()) {
        const int c = ps_course_of_level(SUBLEVEL_LEVEL_TABLE, p.level);
        if (c >= 0) g.visits.note(seat, c, now);
    }
}

void on_star(int seat, const unsigned char *d, int len, void *) {
    if (star_popup_party_rx(seat, d, len)) return;     // STARPOP1: a popup event, not a star to save
    ++g.st.recv;
    int course = -1, star = -1;
    PsDrop r = kPsOk;
    const unsigned now = comms_hub_now_ms();
    if (!party_room()) r = kPsDropRoom;
    else if (!g.accept) r = kPsDropAccept;
    else if (!hub_party_star_validate(d, len)) r = kPsDropBad;
    if (r == kPsOk) {
        course = d[6];
        star = d[7];
        if (seat < 0 || seat >= kPsSeats || seat == my_seat() || !seat_ok(seat)) r = kPsDropSeat;
    }
    if (r == kPsOk) {
        refresh_seat(seat, now);
        HubSeatInfo si;
        HubPresence p;
        if (!comms_hub_seat_info(seat, &si) || !si.present ||
            !hub_presence_get(seat, &p, nullptr))
            r = kPsDropNoPres;
    }
    if (r == kPsOk && g.seqs.seen_or_add(seat, hub_rd16(d, 10)))
        r = kPsDropDupSeq;
    if (r == kPsOk && !g.visits.within(seat, course, now, (uint32_t)kPsVisitMs))
        r = kPsDropCourse;
    if (r == kPsOk && !file_open()) r = kPsDropNoFile;
    if (r == kPsOk && (data_0209cab4[course] & (1u << star))) r = kPsDropHave;
    if (r == kPsOk && g.q.has(course, star)) r = kPsDropQueued;
    if (r == kPsOk && g.q.n >= kPsQueueMax) r = kPsDropFull;
    if (r == kPsOk && !g.no_limit) r = g.lim.check(seat, now);
    if (r != kPsOk) {
        count_drop(r);
        rx_log(seat, course, star, r);
        return;
    }
    g.lim.take(seat, now);
    PsStar s = {seat, course, star, file_slot(), now};
    g.q.push(s);
    ++g.st.queued;
    rx_log(seat, course, star, kPsOk);
}

// ---------------------------------------------------------------------------
// APPLY: one queued star on a safe frame
// ---------------------------------------------------------------------------
void rebaseline() {
    std::memcpy(g.base, data_0209cab4, kPsCourses);
    g.obs_slot = file_slot();
    g.obs_have = true;
    g.gone_frames = 0;
    ++g.st.rebaselines;
    if (g.log)
        std::fprintf(stderr, "[pstar] baseline f=%u slot=%d stars=%d\n", g.frame,
                     g.obs_slot, (int)NumStars());
}

void apply_one(unsigned now) {
    const PsStar s = g.q.pop();
    const int before = (int)NumStars();
    data_0209cab4[s.course] = (uint8_t)(data_0209cab4[s.course] | (1u << s.star));
    const int mid = (int)NumStars();
    const int ok = _ZN8SaveData15SaveCurrentFileEv();
    rebaseline();                               // never echoed
    g.last_apply_ms = now;
    g.have_last_apply = true;
    ++g.st.landed;
    char nm[24], code[40], line[96];
    cleaned_name(s.seat, nm, (int)sizeof nm);
    stage_text(s.course, s.star, code, (int)sizeof code);
    std::snprintf(line, sizeof line, "%s got %s", nm, code);
    port_chat_system_line(line);
    std::fprintf(stderr, "[pstar] LAND seat=%d course=%d star=%d stars=%d->%d "
                 "slot=%d save_ok=%d waited_ms=%u line=\"%s\"\n", s.seat,
                 s.course, s.star, before, mid, file_slot(), ok,
                 (unsigned)(now - s.t), line);
}

void tick_apply(unsigned now, bool safe) {
    if (g.hold_left > 0) --g.hold_left;
    g.safe_run = safe ? g.safe_run + 1 : 0;
    // age out
    for (int i = 0; i < g.q.n;) {
        if ((unsigned)(now - g.q.e[i].t) > (unsigned)kPsStaleMs) {
            count_drop(kPsDropStale);
            for (int k = i + 1; k < g.q.n; ++k) g.q.e[k - 1] = g.q.e[k];
            --g.q.n;
        } else {
            ++i;
        }
    }
    if (g.q.n == 0) return;
    if (!file_open()) return;                   // title / file select: wait
    // the open slot is not the one these stars arrived under: drop them all
    if (file_slot() != g.q.e[0].slot) {
        for (int i = 0; i < g.q.n; ++i) count_drop(kPsDropSlot);
        std::fprintf(stderr, "[pstar] queue dropped: slot %d -> %d (%d stars)\n",
                     g.q.e[0].slot, file_slot(), g.q.n);
        g.q.reset();
        return;
    }
    if (!safe || g.safe_run < kPsSafeFrames || g.hold_left > 0 ||
        (g.have_last_apply && (unsigned)(now - g.last_apply_ms) < (unsigned)kPsApplyGapMs)) {
        ++g.st.hold_waits;
        if (g.log && (unsigned)(g.frame - g.last_wait_log) >= 60) {
            g.last_wait_log = g.frame;
            const char *why = unsafe_reason();
            std::fprintf(stderr, "[pstar] wait f=%u q=%d why=%s safe_run=%d hold=%d | mode=%u msg=%u d4b0=%u sel=%u "
                         "pause=%u clear=%u fade=%u cut=%u chg=%d fc68=%u tmr=%u beh=%u\n", g.frame, g.q.n,
                         why ? why : (g.hold_left > 0 ? "own-star-hold" : (g.safe_run < kPsSafeFrames ? "settling" : "gap")),
                         g.safe_run, g.hold_left, (unsigned)data_0209f2d8[0], (unsigned)data_0209d660[0],
                         rd32(data_0209d4b0), (unsigned)data_0209f294[0], (unsigned)data_0209f2c4[0],
                         (unsigned)data_0209f20c[0], rd32(data_0209b454), rd32(data_0209fc48),
                         (int)(signed char)data_02092110[0], rd32(data_0209fc68), (unsigned)data_0209f22c[0],
                         g.beh_still);
        }
        return;
    }
    if (data_0209cab4[g.q.e[0].course] & (1u << g.q.e[0].star)) {
        g.q.pop();                              // got it meanwhile
        count_drop(kPsDropHave);
        return;
    }
    apply_one(now);
}

// ---------------------------------------------------------------------------
// SENDER
// ---------------------------------------------------------------------------
void send_copies(unsigned now) {
    for (Pending &p : g.pend) {
        if (!p.used || (int)(now - p.due_ms) < 0) continue;
        if (comms_hub_send(p.buf, kHubPartyStarBytes)) ++g.st.sent_datagrams;
        if (--p.left <= 0) p.used = false;
        else p.due_ms = now + kCopyDelayMs2 - kCopyDelayMs1;
    }
}

void announce(int course, int star, unsigned now) {
    unsigned char buf[kHubPartyStarBytes];
    g.seq = (g.seq + 1) & 0xffffu;
    if (hub_party_star_encode(course, star, g.seq, buf) != kHubPartyStarBytes)
        return;
    if (comms_hub_send(buf, kHubPartyStarBytes)) ++g.st.sent_datagrams;
    ++g.st.sent_stars;
    for (Pending &p : g.pend) {
        if (p.used) continue;
        p.used = true;
        std::memcpy(p.buf, buf, sizeof buf);
        p.left = kPsCopies - 1;
        p.due_ms = now + kCopyDelayMs1;
        break;
    }
    g.hold_left = kPsHoldFrames;
    std::fprintf(stderr, "[pstar] tx course=%d star=%d seq=%u stars=%d\n",
                 course, star, g.seq, (int)NumStars());
}

void observe(unsigned now) {
    if (!file_open()) { g.obs_have = false; return; }
    if (!body_alive()) {
        if (++g.gone_frames > 45) g.obs_have = false;
        if (!g.obs_have) return;
    } else {
        g.gone_frames = 0;
    }
    if (!g.obs_have) {
        if (body_alive()) rebaseline();
        return;
    }
    if (file_slot() != g.obs_slot) { rebaseline(); return; }
    const PsDiff d = ps_diff(g.base, data_0209cab4);
    if (d.added == 0 && d.cleared == 0) return;
    if (d.cleared > 0 || d.added != 1) {
        if (d.added > 1) ++g.st.observed_multi;
        if (g.log)
            std::fprintf(stderr, "[pstar] observer rebaseline: +%d -%d\n",
                         d.added, d.cleared);
        rebaseline();
        return;
    }
    std::memcpy(g.base, data_0209cab4, kPsCourses);
    if (g.share && party_room()) announce(d.course, d.star, now);
    else if (g.log)
        std::fprintf(stderr, "[pstar] star course=%d star=%d collected, not "
                     "shared (share=%d)\n", d.course, d.star, g.share);
}

void run_probes() {
    for (auto &p : g.probe) {
        if (!p.used || p.done || (int)g.frame < p.frame || !body_alive() ||
            !file_open()) continue;
        p.done = true;
        CollectStar(p.course, p.bit);
        const int ok = _ZN8SaveData15SaveCurrentFileEv();
        std::fprintf(stderr, "[pstar] PROBE star course %d bit %d collected "
                     "(CollectStar + SaveCurrentFile ok=%d) stars=%d slot=%d\n",
                     p.course, p.bit, ok, (int)NumStars(), file_slot());
        break;                                  // one a frame
    }
    // SM64DS_PARTY_STARS_TRIGGER=<path>: when that file exists (a proof script
    // makes it) its first line "<course> <bit>" is collected the same way and
    // the file is removed. Wall-clock sequencing between consoles.
    if (g.trigger_path && (g.frame % 15u) == 0 && body_alive() && file_open()) {
        std::FILE *f = std::fopen(g.trigger_path, "rb");
        if (f) {
            int c = -1, b = -1;
            const int n = std::fscanf(f, "%d %d", &c, &b);
            std::fclose(f);
            std::remove(g.trigger_path);
            if (n == 2 && c >= 0 && c < kPsCourses && b >= 0 && b < kPsStarBits) {
                CollectStar(c, b);
                const int ok = _ZN8SaveData15SaveCurrentFileEv();
                std::fprintf(stderr, "[pstar] TRIGGER star course %d bit %d collected "
                             "(CollectStar + SaveCurrentFile ok=%d) stars=%d slot=%d\n",
                             c, b, ok, (int)NumStars(), file_slot());
            }
        }
    }
}

void report(const char *tag) {
    if (!g.inited) return;
    const PartyStarsStats &s = g.st;
    std::fprintf(stderr, "[pstar] report %s: sent_stars=%llu dgrams=%llu recv=%llu "
                 "queued=%llu landed=%llu multi=%llu rebase=%llu waits=%llu "
                 "q=%d |", tag, s.sent_stars, s.sent_datagrams, s.recv, s.queued,
                 s.landed, s.observed_multi, s.rebaselines, s.hold_waits, g.q.n);
    for (int i = 1; i < kPsDropReasons; ++i)
        std::fprintf(stderr, " %s=%llu", drop_name(i), s.dropped[i]);
    std::fprintf(stderr, "\n");
}

}  // namespace

PartyStarsStats party_stars_stats() { return g.st; }
bool party_stars_enabled() { return g.inited && party_room(); }

// SEAMJOIN2: A NEW HUB SESSION IN THIS PROCESS STARTS FROM NOTHING. The in-place
// leave tears the hub down with every hub_on_kind handler (comms_hub_teardown),
// and the star handler was registered once per process (g.registered), so a
// world party formed in a second in-place session never got its stars. Called
// by hal/comms_conductor.cpp's comms_conductor_leave_reset, which the in-place
// leave and the in-place join both make (hal/online_join.cpp). Everything goes
// but the exit report's atexit, which is registered once per process.
static int g_star_handler_sessions;   // how many times the star handler went onto a hub (the proof line)
void party_stars_session_reset() {
    const bool hooked = g.report_hooked;
    const bool had = g.registered;
    g = State{};
    g.report_hooked = hooked;
    if (had)
        std::fprintf(stderr, "[pstar] session reset: the star handler goes onto the next hub again\n");
}

}  // namespace port

// THE ONE HOOK LINE (tests/walk_window.cpp, once a frame, after
// port_hub_presence_frame). Global, no namespace, like the other port_* calls.
extern "C" void port_party_stars_frame(void) {
    using namespace port;
    // not a party launch (SM64DS_PARTY) and not in a world room: nothing runs
    if (!party_env() && !(world_party_active() && comms_hub_room_kind() == kHubRoomWorld)) return;
    init_once();
    if (!g.report_hooked) {
        g.report_hooked = true;
        std::atexit(at_exit_report);
    }
    ++g.frame;
    if (!party_room()) return;
    if (!g.registered) {
        hub_on_kind(kHubPartyStar, on_star, nullptr);
        g.registered = true;
        std::fprintf(stderr, "[pstar] star handler registered on the hub (%d in this process)\n", ++g_star_handler_sessions);
    }
    const unsigned now = comms_hub_now_ms();
    // Stage::Behavior ran since last frame?
    const unsigned beh = port_stage_behavior_calls();
    if (g.beh_seeded && beh == g.beh_last) {
        if (g.beh_still < 1000) ++g.beh_still;
    } else {
        g.beh_still = 0;
    }
    g.beh_last = beh;
    g.beh_seeded = true;
    // who has been where
    for (int s = 0; s < kPsSeats; ++s)
        if (s != my_seat()) refresh_seat(s, now);
    run_probes();
    if (g.slotswap_frame >= 0 && !g.slotswap_done && (int)g.frame >= g.slotswap_frame && file_open()) {
        g.slotswap_done = true;
        data_0209caa0[0x328] = (unsigned char)(data_0209caa0[0x328] ^ 1);
        std::fprintf(stderr, "[pstar] SLOTSWAP: the open file slot byte is now %d\n", file_slot());
    }
    observe(now);
    send_copies(now);
    tick_apply(now, frame_is_safe());
    if ((unsigned)(now - g.last_report_ms) > 10000u) {
        g.last_report_ms = now;
        if (g.st.recv || g.st.sent_stars) report("10s");
    }
}
