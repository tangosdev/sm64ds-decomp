// See star_popup.h. Run mp1, lane STARPOP1.
#include "star_popup.h"

#include "comms_hub.h"
#include "coop_wire.h"
#include "coop_world.h"
#include "hub_presence.h"
#include "world_party.h"

#include <windows.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern signed char data_0209f2f8;
extern int data_0209f220[];
extern signed char SUBLEVEL_LEVEL_TABLE[];
extern signed char data_0209211c;         // next level (set by SetNextLevel)
extern unsigned char data_0209f200;       // its entrance
extern unsigned char data_0209f2d8[];     // stage mode
extern unsigned char data_0209d660[];     // message window
extern unsigned char data_0209d4b0[];     // transition / hold
extern unsigned char data_0209f294[];     // star select
extern unsigned char data_0209f2c4[];     // pause menu
extern unsigned char data_0209f20c[];     // level clear
extern unsigned char data_0209b454[];     // fade / scene change flags
extern unsigned char data_0209fc48[];     // cutscene
extern unsigned char data_02092110[];     // pending level change, <0 = none
extern unsigned char data_0209f22c[];     // hold timer
extern unsigned char data_0209f228;       // the star number the clear screen names
extern short data_02092144[];             // per-player health word (wedges in bits 8..15)
extern unsigned char data_0209fc68[];     // (s32) Bowser / ending hold (6 = the ending)
extern unsigned char data_0209f2fc;       // the entry reason of this level (1 exit course, 2 death)
extern unsigned char data_ov002_02111178; // the HUD's life / star slide state (2..5 = the course-clear banner)
extern unsigned char data_0209f248;       // the state PS_Update is in
void *_ZN8dActor_c4NextEPKS_(const void *prev);
void _ZN8dActor_c24KillAndTrackInDeathTableEv(void *self);
int _ZN8daStar_c19func_ov002_020e73acEv(char *star);      // the ROM's star classifier
void ExitLevel(void);
void LoadLevelNoReturn(unsigned level, unsigned entrance, unsigned star, unsigned unk);
extern int data_0209d6fc[];               // the message bank (hal/message_boot.cpp): base, entries, info section
extern int data_0209d708[];
extern int data_0209d70c[];
void port_chat_system_line(const char *text);
char *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b, const void *pos, const void *v16, int area, int f);
void _ZN7fBase_c18MarkForDestructionEv(char *c);
extern unsigned char data_020a1164[];     // the per-player camera records (stride 0x24); +0 = the angle the stick is read against
// STAREXIT2: the Grand Star collect (src/game/actors/d_a_obj_key.cpp daObjKey_c::func_ov089_02131dcc) and the state it puts the
// collector in (St_EndingFly, src/game/player/Player.cpp func_ov002_020c3dbc)
void func_ov002_020c3dbc(void *player);
extern int data_ov002_0211058c[];
void _ZN5Event6SetBitEj(unsigned b);
void port_player_set_character(void *player, unsigned ch);   // hal/player_bridges.cpp: the character doors' own in-place swap
// STAREXIT3: the two bytes Player::SetRealCharacter writes besides the body (src/game/player/Player.cpp:1224-1225): the
// per-player table data_02092128[mPlayerNo] and the save file's character data_0209caa0[0x41] (spelled at its owner
// data_0209cad2, [0x0f], as hal/player_bridges.cpp does). The next level's spawn reads the save byte (LoadEntranceObjects).
extern unsigned char data_02092128[];
extern unsigned char data_0209cad2[];
extern void (*port_sdat_frame_hook)(void);   // hal/sdat/consumer.cpp: one call per game frame, every loop (the title's too)
}

namespace port {
namespace {

enum Mode { kNone, kParty, kCoop };
enum : int { kKindStar = 1, kKindKey = 2, kKindGrand = 3 };   // 3: STAREXIT2, the Grand Star (an old receiver drops it)
enum : unsigned { kFlagExits = 1u };
enum : int { kCtlStarPop = 0x1e };
enum : unsigned { kCountMs = 5000, kShowAfterMs = 2500, kNoteMs = 4000 };
enum : int { kSeenMax = 16, kPendMax = 8, kSeqMemory = 12, kLedgerMax = 4, kRecentMax = 8, kResendN = 6 };
// STARGATE1: re-sends after the first send, ms from the event (the first copy goes out at +0, then these); a console that was
// loading or in star select when the event arrived still gets one. The receiver de-duplicates by sequence number.
const unsigned kResendAt[kResendN] = { 250, 700, 1500, 3000, 5000, 8000 };
enum : unsigned { kLedgerMs = 8000, kGuardMs = 2000, kSoftMs = 4000, kForceMs = 12000 };
// SYNX reserved word
enum : unsigned { kXStar = 1u, kXKey = 2u, kXNoExit = 4u, kXGrand = 3u };
// kXGrand (STAREXIT2) is the low two bits both set, a value no older sender makes: a 0.9.3.6 receiver drops it (its check wants
// exactly 1 or 2) and the relay and comms_hub pass it (they police only bits above 2: reserved & ~7 == 0).

#pragma pack(push, 1)
struct PopWire {            // SYNB 0x1e payload, 8 B
    CoopKeyWire key;        // sender's level + act
    uint8_t kind;           // 1 star, 2 key, 3 the Grand Star (STAREXIT2)
    uint8_t flags;          // bit0 the collector exits
    uint8_t star;           // star number 0..8
    uint8_t seq;
};
#pragma pack(pop)
static_assert(sizeof(PopWire) == 8, "popup event is 8 bytes");

struct Pending { bool used; unsigned t0; int idx; unsigned char buf[16]; int len; bool coop; PopWire w; };
// An event that arrived while I was not in the collector's course and act yet (loading, star select): kept for kLedgerMs and
// re-checked every frame.
struct Ledger { bool used; int seat, kind, star, course, lvl, act; bool exits; unsigned t; bool arena; };
// (arena: STAREXIT1, only a console in the collector's own level leaves for it)
// Recent collections (mine and everyone's) for the single-collector guard.
struct Recent { bool used; int key, star, kind; unsigned t; };
struct Fake { bool used, done; int kind, exits; unsigned frame; };
struct SeqMem { int seat; unsigned seq; };

struct State {
    bool inited, enabled, log;
    unsigned frame;
    // sender
    void *seen[kSeenMax]; int nseen;
    int  lvl, act; bool have_key;
    unsigned seq;
    Pending pend[kPendMax];
    Ledger led[kLedgerMax];
    Recent rec[kRecentMax]; int nrec;
    // receiver
    SeqMem mem[kSeqMemory]; int nmem;
    bool counting; unsigned t_end; bool exit_pending; bool exit_key; int exit_star;
    bool exit_grand;                 // STAREXIT2: the exit is the Grand Star's (my body takes the ending flight)
    bool grand_sent, grand_forced;   // STAREXIT2: this level: my own Grand Star event went out / I was put on the ending flight
    char line[kStarPopupLines - 1][kStarPopupLineMax]; int nline;
    bool note_only;                  // STAREXIT1: the lines are a note (no countdown, no exit)
    bool choice;                     // STARPOP2: the countdown is a choice: UP leaves, running out stays
    bool up_armed;                   // STARPOP2: UP was seen released since the choice came up (a held d-pad is not a press)
    unsigned auto_up_ms;             // proof knob SM64DS_STAR_POPUP_AUTOUP=<ms>: press UP that long into the choice
    unsigned t_last;
    bool registered;
    StarPopupStats st;
    // knobs
    Fake fake[6];
    int hold_from, hold_n;
    unsigned last_wait_log;
    unsigned t_pend;                 // when the exit started waiting for a safe frame
    int fake_star;                   // proof knob SM64DS_STAR_POPUP_FAKE_STAR=<1..7>: which star a fake event names (default 1)
    unsigned stale_frame; bool stale_done;   // proof knob SM64DS_STAR_POPUP_STALE=<frame>: leave data_0209d4b0 non-zero
    bool cleared_d4b0;
    bool no_timeout;                 // proof knob SM64DS_STAR_POPUP_NOTIMEOUT=1: the old behaviour (every gate waits for ever)
    // proof knob SM64DS_STAR_POPUP_STOP: end the selftest at a chosen moment (a picture)
    int stop_kind, stop_val; bool armed, stop_flag, stop_timer; unsigned stop_at;
};
State g;

unsigned now_ms() { return (unsigned)GetTickCount64(); }
long long wall_ms() {
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    return (long long)((((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000ull);
}

bool env_on(const char *name, bool dflt) {
    const char *e = std::getenv(name);
    if (!e || !*e) return dflt;
    return !(e[0] == '0' && e[1] == 0);
}

void init_once() {
    if (g.inited) return;
    g.inited = true;
    g.enabled = env_on("SM64DS_STAR_POPUP", true);
    g.log = env_on("SM64DS_STAR_POPUP_LOG", false);
    g.hold_from = -1;
    if (const char *e = std::getenv("SM64DS_STAR_POPUP_HOLD")) {
        int f = 0, n = 0;
        if (std::sscanf(e, "%d:%d", &f, &n) == 2 && f >= 0 && n > 0) { g.hold_from = f; g.hold_n = n; }
    }
    if (const char *e = std::getenv("SM64DS_STAR_POPUP_FAKE")) {
        const char *p = e; int k = 0;
        while (*p && k < 6) {
            int kd = 0, ex = 0, f = 0, used = 0;
            if (std::sscanf(p, "%d:%d@%d%n", &kd, &ex, &f, &used) == 3 && (kd == 1 || kd == 2) && f >= 0) {
                g.fake[k] = { true, false, kd, ex ? 1 : 0, (unsigned)f }; ++k; p += used;
            } else break;
            if (*p == ';') ++p;
        }
    }
    if (const char *e = std::getenv("SM64DS_STAR_POPUP_STOP")) {
        int v = 0;
        if (std::sscanf(e, "cd%d", &v) == 1) { g.stop_kind = 1; g.stop_val = v; }       // the countdown shows v
        else if (std::sscanf(e, "tx%d", &v) == 1) { g.stop_kind = 2; g.stop_val = v; }  // v ms after my own event
        else if (std::sscanf(e, "lv%d", &v) == 1) { g.stop_kind = 3; g.stop_val = v; }  // v ms after the level changed following an event
    }
    g.no_timeout = env_on("SM64DS_STAR_POPUP_NOTIMEOUT", false);
    if (const char *e = std::getenv("SM64DS_STAR_POPUP_FAKE_STAR")) { const int n = std::atoi(e); if (n >= 1 && n <= 7) g.fake_star = n; }
    if (const char *e = std::getenv("SM64DS_STAR_POPUP_AUTOUP")) { const int ms = std::atoi(e); if (ms > 0) g.auto_up_ms = (unsigned)ms; }
    if (const char *e = std::getenv("SM64DS_STAR_POPUP_STALE")) { const int f = std::atoi(e); if (f > 0) g.stale_frame = (unsigned)f; }
    std::fprintf(stderr, "[starpop] on: enabled=%d only_collector=%d\n", (int)g.enabled, (int)env_on("SM64DS_ROOM_SOLOEXIT", true));
}

Mode mode_now() {
    if (!g.enabled) return kNone;
    if (!comms_hub_active() || comms_hub_arena() || comms_hub_state() != kHubLive) return kNone;
    const HubRoomKind k = comms_hub_room_kind();
    if (k == kHubRoomParty) {
        const char *e = std::getenv("SM64DS_PARTY");
        return (e && *e && !(e[0] == '0' && e[1] == 0)) ? kParty : kNone;
    }
    if (k == kHubRoomCoop) return coop_world_active() ? kCoop : kNone;
    // WPARTY2: a world room, while I am in a world party (kParty; rx adds the member test)
    if (k == kHubRoomWorld) return (world_party_active() && world_party_in_party()) ? kParty : kNone;
    return kNone;
}

// ---- STAREXIT1: the room rule "only the collector leaves" ------------------------------------------------------------------
// The one place the rule lives. The launcher hands it over as SM64DS_ROOM_SOLOEXIT (lobby room setting "soloexit"; the coop and
// world hand-offs put every request name into the environment), read on every event so an in-place hand-off to another room is
// seen. A missing or empty name is ON (an older launcher sends none). star_popup_set_only_collector() overrides it (the in-game
// lobby); -1 goes back to the environment.
int g_only_override = -1;
bool only_collector() {
    if (g_only_override >= 0) return g_only_override != 0;
    return env_on("SM64DS_ROOM_SOLOEXIT", true);
}
// A friend's star counts for me only where it is shared: coop always (the room shares one save), a party room when its stars are
// shared and taken (hal/party_stars.cpp reads the same two names), a world party never (hal/party_stars.cpp takes party rooms only).
bool star_shared_here(Mode m) {
    if (m == kCoop) return true;
    return comms_hub_room_kind() == kHubRoomParty && env_on("SM64DS_PARTY_SHARE_STARS", true) &&
           env_on("SM64DS_PARTY_ACCEPT_STARS", true);
}

int my_seat() { return comms_hub_my_seat(); }
void *my_body() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < 16) ? data_0209f394[me] : nullptr;
}
unsigned rd32(const unsigned char *p) { unsigned v; std::memcpy(&v, p, 4); return v; }

void seat_name(int seat, char *out, int cap) {
    out[0] = 0;
    unsigned char id[64];
    const int n = comms_hub_latest(kHubIdent, seat, id, (int)sizeof id, nullptr);
    if (n >= (int)sizeof(HubIdentHead)) {
        HubIdentHead h; std::memcpy(&h, id, sizeof h);
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

// ---- the exit -------------------------------------------------------------
// func_ov002_020c94a4 (Player, after the star-get dance): sub 0 = star, sub 3 = key.
void landing_overrides(bool key) {
    if (!key) {
        if (data_0209f2f8 == 0x21) { data_0209211c = 4; data_0209f200 = 7; }
        return;
    }
    switch (data_0209f2f8) {
    case 0x24: data_0209211c = 2;    data_0209f200 = 0xa; break;
    case 0x26: data_0209211c = 4;    data_0209f200 = 9;   break;
    case 0x2d: data_0209211c = 0x32; data_0209f200 = 3;   break;
    case 0x2f: data_0209211c = 3;    data_0209f200 = 0;   break;
    case 0x31: data_0209211c = 5;    data_0209f200 = 0xb; break;
    default: break;
    }
}

// 0 = go, 1 = wait, 2 = already leaving (drop the exit). *hard = the gate is a real state (a pause menu, a star select,
// no body yet): only the 12 s limit forces it. Every other gate is soft and times out after kSoftMs (STARGATE1).
int exit_gate(const char **why, bool *hard) {
    *why = ""; *hard = false;
    if (!my_body()) { *why = "nobody"; *hard = true; return 1; }
    if ((signed char)data_02092110[0] >= 0) { *why = "leaving"; return 2; }
    if (g.hold_from >= 0 && (int)g.frame >= g.hold_from && (int)g.frame < g.hold_from + g.hold_n) { *why = "forced"; *hard = true; return 1; }
    if (data_0209f294[0]) { *why = "starselect"; *hard = true; return 1; }
    if (data_0209f2c4[0]) { *why = "pause"; *hard = true; return 1; }
    if (data_0209f2d8[0] == 2) { *why = "mode"; return 1; }
    if (data_0209d660[0]) { *why = "message"; return 1; }
    if (rd32(data_0209d4b0)) { *why = "d4b0"; return 1; }
    // data_0209f20c (level clear) is NOT a gate: the Bowser arenas hold it at 1 for the whole fight, and a level clear that
    // matters always comes with a fade, a cutscene or a pending level change, which are gates.
    if (rd32(data_0209b454)) { *why = "fade"; return 1; }
    if (rd32(data_0209fc48)) { *why = "cutscene"; return 1; }
    if (data_0209f22c[0]) { *why = "timer"; return 1; }
    return 0;
}

// ---- names from the ROM's own message bank -------------------------------------------------------------------------------
// The bank's text is a glyph-index stream (src/func_0201b7cc.c); decoded to ASCII the way the pause menu's star tracker does
// (hal/menu_recipe_pause.cpp msg_text). Course title = message 406 + course, star name = 0x1b4 + course * 7 + (star - 1).
int msg_text(int id, char *out, int cap) {
    out[0] = 0;
    const char *info = (const char *)(size_t)(unsigned)data_0209d70c[0];
    const char *base = (const char *)(size_t)(unsigned)data_0209d6fc[0];
    const char *entries = (const char *)(size_t)(unsigned)data_0209d708[0];
    if (!info || !base || !entries) return 0;
    const unsigned count = *(const unsigned short *)(info + 8);
    if (id < 0 || (unsigned)id >= count) return 0;
    const int secsz = *(const int *)(info + 4);
    const int off = *(const int *)(entries + id * 8);
    const unsigned char *p = (const unsigned char *)(base + 0x28 + secsz + off);
    int n = 0;
    for (int guard = 0; guard < 120 && *p != 0xff && n < cap - 1; ++guard, ++p) {
        const unsigned c = *p;
        char o;
        if (c <= 9) o = (char)('0' + c);
        else if (c <= 0x23) o = (char)('A' + c - 10);
        else if (c >= 0x2d && c <= 0x46) o = (char)('a' + c - 0x2d);
        else switch (c) {
            case 0x26: o = '?'; break;
            case 0x27: o = '!'; break;
            case 0x29: o = ','; break;
            case 0x47: o = '-'; break;
            case 0x48: o = '.'; break;
            case 0x49: o = '\''; break;
            case 0x4a: o = '!'; break;
            case 0x4c: o = '&'; break;
            case 0x4d: o = ' '; break;
            default: o = '?'; break;
        }
        out[n++] = o;
    }
    out[n] = 0;
    return n;
}

void title_case(char *s) {
    static const char *const smallw[] = { "a", "an", "the", "of", "on", "in", "to", "for", "with", "at", "from", "and", "or", "over", "by" };
    char *w = s;
    int first = 1;
    while (*w) {
        while (*w == ' ') ++w;
        char *e = w;
        while (*e && *e != ' ') ++e;
        const char save = *e;
        *e = 0;
        char low[40];
        std::snprintf(low, sizeof low, "%s", w);
        for (char *q = low; *q; ++q) if (*q >= 'A' && *q <= 'Z') *q = (char)(*q - 'A' + 'a');
        char core[40];
        std::snprintf(core, sizeof core, "%s", low);
        for (int k = (int)std::strlen(core) - 1; k >= 0 && (core[k] == ',' || core[k] == '!' || core[k] == '.' || core[k] == '?'); --k) core[k] = 0;
        int is_small = 0;
        for (unsigned k = 0; k < sizeof smallw / sizeof smallw[0]; ++k) if (!std::strcmp(core, smallw[k])) is_small = 1;
        for (char *q = w; *q; ++q) *q = low[q - w];
        if ((!is_small || first) && *w >= 'a' && *w <= 'z') *w = (char)(*w - 'a' + 'A');
        if (!std::strncmp(low, "chain-chomp", 11)) { *w = 'C'; w[6] = 'C'; }
        *e = save;
        w = e;
        first = 0;
    }
}

// course = the ROM's course number (SUBLEVEL_LEVEL_TABLE value), -1 unknown. Empty string when the bank is not loaded.
void course_title(int course, char *out, int cap) {
    out[0] = 0;
    if (course < 0 || course > 29) return;
    char raw[80];
    if (!msg_text(406 + course, raw, sizeof raw)) return;
    const char *p = raw;
    while (*p == ' ' || (*p >= '0' && *p <= '9')) ++p;
    std::snprintf(out, (size_t)cap, "%s", p);
    // trailing blanks (the Sky title has one)
    for (int k = (int)std::strlen(out) - 1; k >= 0 && out[k] == ' '; --k) out[k] = 0;
    title_case(out);
}

void star_title(int course, int star, char *out, int cap) {
    out[0] = 0;
    if (course < 0 || course > 14 || star < 1 || star > 7) return;
    if (!msg_text(0x1b4 + course * 7 + (star - 1), out, cap)) return;
    title_case(out);
}

// "Name got <star name> (<course>)" / "Name got the key (<course>)"; the names are the ROM's. Fallbacks keep the sentence whole.
void describe(const char *name, int kind, int course, int star, char *out, size_t cap, bool full = true) {
    // The chat font has no brackets, so the course goes after "in" / "from".
    char c[64], st[48];
    course_title(course, c, sizeof c);
    if (kind == kKindGrand) { std::snprintf(out, cap, "%s got the Grand Star", name); return; }   // STAREXIT2
    if (!full) {        // the popup: one short line (the chat line has the course)
        if (kind == kKindKey) std::snprintf(out, cap, "%s got the key", name);
        else {
            star_title(course, star, st, sizeof st);
            if (st[0]) std::snprintf(out, cap, "%s got %s", name, st); else std::snprintf(out, cap, "%s got a star", name);
        }
        return;     // STARPOP2: a long line is drawn smaller (star_popup_paint.h), never cut
    }
    if (kind == kKindKey) {
        const char *prep = (course >= 15 && course <= 17) ? "from" : "in";      // courses 15 to 17 are the three Bowser fights
        if (c[0]) std::snprintf(out, cap, "%s got the key %s %s", name, prep, c); else std::snprintf(out, cap, "%s got the key", name);
        return;
    }
    star_title(course, star, st, sizeof st);
    if (st[0] && c[0]) std::snprintf(out, cap, "%s got %s in %s", name, st, c);
    else if (st[0]) std::snprintf(out, cap, "%s got %s", name, st);
    else if (c[0]) std::snprintf(out, cap, "%s got a star in %s", name, c);
    else std::snprintf(out, cap, "%s got a star", name);
}

// ---- the exit unit: course + act, not level id ---------------------------------------------------------------------------
int course_of(int level) { return (level >= 0 && level < 52) ? (int)SUBLEVEL_LEVEL_TABLE[level] : -1; }
// JRB (8) and its Sunken Ship (9) are one course; the castle floors (course 29) and unknown levels only match themselves.
bool same_unit(int la, int lb) {
    if (la == lb) return true;
    const int ca = course_of(la), cb = course_of(lb);
    return ca >= 0 && ca != 29 && ca == cb;
}
int guard_key(int level, int course) { return (course >= 0 && course != 29) ? course : 100 + level; }

// Proof knob SM64DS_STAR_POPUP_STALE=<frame>: leave data_0209d4b0 (the bottom-screen fader pointer the respawn sets and the
// ROM clears when the fade ends) non-zero, as a stale one would be. The object is a zeroed block whose three virtuals do nothing,
// so every reader of the pointer (func_02018ec0 calls vt[2]) stays harmless.
void stale_noop(void *) {}
void *g_stale_vt[8] = { (void *)stale_noop, (void *)stale_noop, (void *)stale_noop, (void *)stale_noop,
                        (void *)stale_noop, (void *)stale_noop, (void *)stale_noop, (void *)stale_noop };
void *g_stale_obj[8] = { g_stale_vt, 0, 0, 0, 0, 0, 0, 0 };
void stale_d4b0_plant() {
    const unsigned v = (unsigned)(size_t)g_stale_obj;
    std::memcpy(data_0209d4b0, &v, 4);
    std::fprintf(stderr, "[starpop] STALE knob: data_0209d4b0 set to %08x wall=%lld\n", v, wall_ms());
}

// What the ROM does when the fader that data_0209d4b0 points at is done (FUN_02029a68 / FUN_02029934): the pointer to zero and the
// blend register to zero. Called only for a pointer that has outlived kSoftMs of the exit waiting on it.
void clear_stale_d4b0() {
    const unsigned old = rd32(data_0209d4b0);
    const unsigned zero = 0;
    std::memcpy(data_0209d4b0, &zero, 4);
    *(volatile unsigned short *)0x4000050 = 0;
    g.cleared_d4b0 = true;
    std::fprintf(stderr, "[starpop] cleared stale data_0209d4b0 (was %08x) wall=%lld\n", old, wall_ms());
}

void clear_receiver() {
    g.counting = false; g.exit_pending = false; g.nline = 0; g.t_end = 0; g.note_only = false; g.exit_grand = false;
    g.choice = false; g.up_armed = false;
}

// ---- STAREXIT2: the Grand Star -----------------------------------------------------------------------------------------------
// The ROM's collect (daObjKey_c::func_ov089_02131dcc, the LAST_STAR arm of StateStarJump step 3) is three calls: the collector's
// Player goes to St_EndingFly (func_ov002_020c3dbc), event bit EVENT_KEY (0x1d) is set, the star marks itself for destruction.
// St_EndingFly flies the body out of the arena and the ROM then loads the ending (castle grounds). The collector's console runs
// that untouched; a console told about it does the same three things with its own body and its own copy of the star.
enum : unsigned { kEventKey = 0x1d };
bool on_ending_flight(const char *b) { return b && *(void *const *)(b + 0x370) == (const void *)data_ov002_0211058c; }
// The ending is Mario's. Its Kuppa script finds "player 0" through func_0200e6d8(0): the local body when its character
// (param1, +8) is 0 = Mario, else a Mario the script spawned itself; a coop or party body that is Luigi, Wario or Yoshi finds
// neither and the ending's first camera step (func_0200814c) reads through a null player as the ending level loads (c0000005,
// measured with both consoles as Yoshi). On the cartridge only Mario reaches this arena. So the body on the ending flight is
// made Mario first, through the doors' own swap (model, state and position carry across).
//
// STAREXIT3: that swap is the doors' SetRealCharacter, which also writes the save file's character byte and my per-player
// table byte (src/game/player/Player.cpp:1224-1225), so a Luigi, Wario or Yoshi player's SAVED character became Mario after
// the ending, and the character colours and the next coop save followed it. Only the body is the ending's: the two bytes are
// written back at once. They cannot simply stay that way, though: the ending and every credits scene are level loads, and
// each load spawns my body from the save byte (LoadEntranceObjects' f2, hal/level_boot.cpp), where the ending's script needs
// a Mario again. So while the ending runs, a level change that is pending (data_02092110 >= 0, held by the exit fade until
// it is applied) gets Mario in both bytes, and every other frame gets my own character back. The keep ends once a level change
// has happened and no level change, Kuppa script (data_0209fc48) or ending stage mode (data_0209f2d8 == 2) has been seen for
// kKeepQuietFrames frames in a level other than the arena.
// SM64DS_GRAND_KEEP=0 keeps the old behaviour (the swap's writes stand) and SM64DS_GRAND_KEEP=plain writes the bytes back
// once right after the swap and never again, so a row can show either next to the fix in one exe. Test-only.
enum : unsigned { kKeepQuietFrames = 120, kKeepNoBodyFrames = 120 };
// It also ends when the ending is left another way (REVSTAREXIT3 note 2): the stage mode leaves 2 after the ending began (a file
// started again: StartFile sets it 0), or somebody else wrote the save byte (a file was read), or the room is gone while I am no
// longer in the ending (no body for kKeepNoBodyFrames frames, as at a title, or neither the ending mode, the ending flight nor a
// pending level change). A credits scene's own load also has no body for a frame or two, which must not count. Then my own character goes back into both bytes if a level change window
// left Mario there, unless a file read already replaced them, and the keep is cleared, so a later room in this process never sees
// stale values. A room lost while the ending still runs here (mode 2, a body) keeps the keep running: the credits scenes still load
// and still need Mario for their spawn.
struct Keep { bool on, pending, changed, ending, room_lost_noted; unsigned char save, tbl, last; int slot, from_level; unsigned quiet, writes, windows, nobody; };
Keep g_keep;
int keep_mode() {
    static int m = -1;
    if (m < 0) {
        const char *e = std::getenv("SM64DS_GRAND_KEEP");
        m = !e || !*e ? 1 : (*e == '0' ? 0 : (*e == 'p' ? 2 : 1));
    }
    return m;
}
void keep_write(bool mario) {
    data_0209cad2[0x0f] = mario ? 0 : g_keep.save;
    data_02092128[g_keep.slot] = mario ? 0 : g_keep.tbl;
    g_keep.last = data_0209cad2[0x0f];
    ++g_keep.writes;
}
bool keep_room_alive() {
    if (!comms_hub_active() || comms_hub_state() != kHubLive) return false;
    if (std::getenv("SM64DS_COOP") || std::getenv("SM64DS_PARTY")) return true;
    return world_party_active() && comms_hub_room_kind() == kHubRoomWorld;
}
// Clear the keep. own: put my own character back first, but only while the save byte is still the one I wrote last (a file read
// since then owns both bytes).
void keep_drop(const char *why, bool own) {
    const bool mine = data_0209cad2[0x0f] == g_keep.last;
    if (own && mine) keep_write(false);
    std::fprintf(stderr, "[starpop] GRAND keep: dropped (%s; level %d, mode %u, body %s, %u writes): %s; save character %u, table[%d] %u"
                 " wall=%lld\n", why, (int)data_0209f2f8, (unsigned)data_0209f2d8[0], my_body() ? "yes" : "none", g_keep.writes,
                 own && mine ? "my own character written back" : mine ? "nothing written" : "the bytes were rewritten since (a file read): left alone",
                 (unsigned)data_0209cad2[0x0f], g_keep.slot, (unsigned)data_02092128[g_keep.slot], wall_ms());
    g_keep = Keep();
}
// The drop tests, also run from the per-frame sound hook (port_sdat_frame_hook, chained when the keep is first armed), because a
// title reached in place runs its own loop where port_star_popup_frame is not called.
bool keep_guard() {
    if (!g_keep.on) return false;
    if (data_0209cad2[0x0f] != g_keep.last) { keep_drop("the save byte was written by someone else", false); return false; }
    const unsigned mode = data_0209f2d8[0];
    if (mode == 2) g_keep.ending = true;
    if (g_keep.ending && mode != 2) { keep_drop("the stage mode left the ending", true); return false; }
    if (!keep_room_alive()) {
        const char *b = (const char *)my_body();
        const bool ending_now = mode == 2 || (signed char)data_02092110[0] >= 0 || (b && on_ending_flight(b));
        const bool in_ending = ending_now && (b || g_keep.nobody < kKeepNoBodyFrames);
        if (!in_ending) { keep_drop("the room is gone", true); return false; }
        if (!g_keep.room_lost_noted) {
            g_keep.room_lost_noted = true;
            std::fprintf(stderr, "[starpop] GRAND keep: the room is gone but the ending runs on here (level %d): the keep stays wall=%lld\n",
                         (int)data_0209f2f8, wall_ms());
        }
    }
    return true;
}
void (*g_keep_prev_hook)(void) = nullptr;
bool g_keep_chained = false;
void keep_frame_hook() {
    if (g_keep_prev_hook) g_keep_prev_hook();
    if (g_keep.on) g_keep.nobody = my_body() ? 0 : g_keep.nobody + 1;   // once per frame in every loop
    keep_guard();
}
void keep_tick() {
    if (!keep_guard()) return;
    const bool pend = (signed char)data_02092110[0] >= 0;
    if (pend != g_keep.pending)
        std::fprintf(stderr, "[starpop] GRAND keep: %s (level %d -> %d): save character %u, table[%d] %u wall=%lld\n",
                     pend ? "a level change is pending, Mario for its spawn" : "no level change pending, my own character back",
                     (int)data_0209f2f8, (int)(signed char)data_02092110[0], pend ? 0u : (unsigned)g_keep.save, g_keep.slot,
                     pend ? 0u : (unsigned)g_keep.tbl, wall_ms());
#ifndef PORT_ROM_CLEAN
    // Proof knob SM64DS_GRAND_KEEP_QUIT=<n> (dev only): at the start of the n-th level change window of the ending, end the coop
    // room (the hub torn down, as a lost connection tears it down), so a row can end the room inside a window.
    if (pend && !g_keep.pending) {
        static int quit_n = -2;
        if (quit_n == -2) { const char *e = std::getenv("SM64DS_GRAND_KEEP_QUIT"); quit_n = (e && *e) ? std::atoi(e) : -1; }
        if (quit_n > 0 && ++g_keep.windows == (unsigned)quit_n) {
            std::fprintf(stderr, "[starpop] GRAND keep: proof knob: the room ends in level change window %d wall=%lld\n", quit_n, wall_ms());
            comms_hub_teardown("proof knob: the room ends inside a level change window");
        }
    }
#endif
    g_keep.pending = pend;
    if (pend) { g_keep.changed = true; g_keep.quiet = 0; keep_write(true); return; }
    keep_write(false);
    const bool ending = rd32(data_0209fc48) != 0 || data_0209f2d8[0] == 2;
    if (g_keep.changed && (int)data_0209f2f8 != g_keep.from_level && !ending && my_body()) {
        if (++g_keep.quiet >= kKeepQuietFrames) keep_drop("the ending is over", true);
    } else {
        g_keep.quiet = 0;
    }
}
void ending_as_mario(char *b, const char *why) {
    if (!b || *(const unsigned *)(b + 8) == 0u) return;
    const unsigned was = *(const unsigned *)(b + 8);
    const int slot = *(const unsigned char *)(b + 0x6d8) & 0x3f;     // mPlayerNo, the table index SetRealCharacter writes
    const unsigned char save0 = data_0209cad2[0x0f], tbl0 = data_02092128[slot];
    port_player_set_character(b, 0u);
    const unsigned char save1 = data_0209cad2[0x0f], tbl1 = data_02092128[slot];
    const int km = keep_mode();
    if (km != 0 && !g_keep.on) {
        g_keep = Keep();
        g_keep.on = true; g_keep.save = save0; g_keep.tbl = tbl0; g_keep.slot = slot; g_keep.from_level = (int)data_0209f2f8;
        keep_write(false);
        if (km == 2) g_keep = Keep();
        if (!g_keep_chained) { g_keep_chained = true; g_keep_prev_hook = port_sdat_frame_hook; port_sdat_frame_hook = keep_frame_hook; }
    }
    std::fprintf(stderr, "[starpop] GRAND %s: my body was character %u, now %u for the ending; save character %u -> %u -> %u,"
                 " table[%d] %u -> %u -> %u (%s) wall=%lld\n", why, was, *(const unsigned *)(b + 8), (unsigned)save0, (unsigned)save1,
                 (unsigned)data_0209cad2[0x0f], slot, (unsigned)tbl0, (unsigned)tbl1, (unsigned)data_02092128[slot],
                 km == 0 ? "keep off" : km == 2 ? "plain restore" : "kept", wall_ms());
}
// My copies of the Grand Star (class 283), removed the way the ROM removes the collected one.
int kill_grand() {
    int n = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (*(const unsigned short *)(a + 0x0c) != 283 || *(const unsigned char *)(a + 0x0f)) continue;
        _ZN7fBase_c18MarkForDestructionEv(a);
        ++n;
    }
    return n;
}

void toast(const char *name, int kind, int course, int star) {
    char t[160];
    describe(name, kind, course, star, t, sizeof t);
    port_chat_system_line(t);
    ++g.st.toast;
    std::fprintf(stderr, "[starpop] chat: %s\n", t);
}

bool seen_seq(int seat, unsigned seq) {
    for (int i = 0; i < g.nmem; ++i) if (g.mem[i].seat == seat && g.mem[i].seq == seq) return true;
    if (g.nmem < kSeqMemory) { g.mem[g.nmem].seat = seat; g.mem[g.nmem].seq = seq; ++g.nmem; }
    else {
        std::memmove(g.mem, g.mem + 1, sizeof g.mem[0] * (kSeqMemory - 1));
        g.mem[kSeqMemory - 1].seat = seat; g.mem[kSeqMemory - 1].seq = seq;
    }
    return false;
}

// SINGLE COLLECTOR GUARD. Two collections of one star within kGuardMs (mine and a friend's, or two friends') are ONE star-get:
// the later one sends nothing / shows nothing. (A star-get animation already running on a console cannot be taken back; this
// keeps the second one from reaching the wire, the popup, the count-down and the chat.) Returns true if it is a duplicate.
bool guard_dup(int key, int star, int kind, bool record) {
    const unsigned now = now_ms();
    for (Recent &r : g.rec)
        if (r.used && r.key == key && r.star == (star & 7) && r.kind == kind && (unsigned)(now - r.t) < kGuardMs) return true;
    if (record) {
        Recent &r = g.rec[g.nrec++ % kRecentMax];
        r = { true, key, star & 7, kind, now };
    }
    return false;
}

// The popup + countdown for an event that is for my course and act.
void start_popup(const char *name, int kind, int course, int star) {
    const unsigned now = now_ms();
    if (g.nline < kStarPopupLines - 1) {
        char l[160];
        describe(name, kind, course, star, l, sizeof l, false);
        std::snprintf(g.line[g.nline], kStarPopupLineMax, "%s", l);
        ++g.nline;
    }
    g.t_last = now;
    ++g.st.shown;
    g.note_only = false;
    if (!g.counting && !g.exit_pending) {
        g.counting = true;
        g.t_end = now + kCountMs;
        g.exit_key = (kind == kKindKey);
        g.exit_star = (kind == kKindStar && star >= 1 && star <= 7) ? star : -1;   // the first star is the one the clear screen names
        g.exit_grand = (kind == kKindGrand);
        // STARPOP2: a star asks (UP leaves, running out stays); a key's boss room and the Grand Star still take everyone out
        g.choice = (kind == kKindStar);
        g.up_armed = false;
    } else if (kind == kKindKey) {
        g.exit_key = true;       // a key anywhere in the batch: use the key landing
        g.choice = false;
    } else if (kind == kKindGrand) {
        g.exit_grand = true;     // the Grand Star wins over any other exit in the batch: it is the end of the game
        g.choice = false;
    }
}

bool in_unit(int lvl, int act) {
    return lvl >= 0 && my_body() && act == (data_0209f220[0] & 0xf) && same_unit(lvl, (int)data_0209f2f8 & 0xff);
}

// STAREXIT1: a key's exit is for the boss room only: I stand in the collector's own level (the arena), not just its course.
bool in_arena(int lvl) {
    return lvl >= 0 && my_body() && lvl == ((int)data_0209f2f8 & 0xff);
}

// STAREXIT1: the popup's lines without the countdown (rule ON, a friend's star in my course and act). Never starts an exit.
void note_line(const char *name, int kind, int course, int star) {
    if (g.counting || g.exit_pending) { start_popup(name, kind, course, star); return; }   // an exit already runs: one more line
    if (g.nline >= kStarPopupLines - 1) { std::memmove(g.line, g.line + 1, sizeof g.line[0] * (kStarPopupLines - 2)); --g.nline; }
    char l[160];
    describe(name, kind, course, star, l, sizeof l, false);
    std::snprintf(g.line[g.nline], kStarPopupLineMax, "%s", l);
    ++g.nline;
    g.t_last = now_ms();
    g.note_only = true;
    ++g.st.shown;
    ++g.st.notes;
}

// STAREXIT1, party with shared stars: a friend took star `star` of my course and act, so my copy of it goes, the way coop's
// reconcile removes a collected star (hal/coop_world.cpp reconcile: KillAndTrackInDeathTable). A PowerStar (class 178) with that
// star number (+0x49d, the number scan_local sends) that is not being collected right now (+0x440 != 5) and is not a silver / VS
// star (the ROM's classifier, 2). The save bit itself arrives through hal/party_stars.cpp on a safe frame.
int kill_local_star(int star) {
    int n = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; ) {
        char *next = (char *)_ZN8dActor_c4NextEPKS_(a);
        if (*(const unsigned short *)(a + 0x0c) == 178 && *(const unsigned *)(a + 0x440) != 5u &&
            (*(const unsigned char *)(a + 0x49d) & 7) == (star & 7) && _ZN8daStar_c19func_ov002_020e73acEv(a) != 2) {
            _ZN8dActor_c24KillAndTrackInDeathTableEv(a);
            ++n;
        }
        a = next;
    }
    return n;
}

// An event arrived from `seat`; the sender stood in `lvl` / `act` (lvl < 0 = unknown), course `course` (-1 unknown).
void on_event(int seat, int kind, bool exits, int lvl, int act, int course, int star) {
    char name[24];
    seat_name(seat, name, sizeof name);
    ++g.st.rx;
    if (exits && guard_dup(guard_key(lvl, course), star, kind, true)) {
        ++g.st.dup;
        std::fprintf(stderr, "[starpop] guard: second collection of course %d star %d within %u ms (from seat %d) ignored wall=%lld\n", course, star, kGuardMs, seat, wall_ms());
        return;
    }
    toast(name, kind, course, star);
    if (kind == kKindGrand) {
        // STAREXIT2 (lead default): everyone in the final arena (the collector's own level) leaves with the collector, whatever the
        // room rule: their popup counts down and their body takes the ending flight, and their copy of the star is gone. Elsewhere
        // the room rule decides: ON = a note line, I stay; OFF = the chat line only (as any event outside my course and act).
        // A party room without shared stars (or a world party) gets the chat line only: it is not their star.
        const Mode m = mode_now();
        const bool shared = star_shared_here(m);
        const bool here = in_arena(lvl);
        const char *me = (const char *)my_body();
        int removed = 0;
        const char *what = "chat only";
        if (!exits || !shared) what = "not shared: chat only";
        else if (here && on_ending_flight(me)) what = "already on the ending flight";
        else if (here) { removed = kill_grand(); start_popup(name, kind, course, star); what = "in the arena: popup, countdown, ending"; }
        else if (only_collector()) { note_line(name, kind, course, star); what = "elsewhere, rule on: note line, I stay"; }
        std::fprintf(stderr, "[starpop] rx seat=%d GRAND level=%d (mine %d) shared=%d only=%d removed=%d: %s wall=%lld\n", seat, lvl,
                     (int)data_0209f2f8, (int)shared, (int)only_collector(), removed, what, wall_ms());
        if (exits && shared && !here && lvl >= 0) {     // I may be on my way into the arena: the ledger checks the level
            for (Ledger &l : g.led)
                if (!l.used) { l = { true, seat, kind, star, course, lvl, act, exits, now_ms(), true }; break; }
        }
        return;
    }
    // STAREXIT1: with the room rule ON only the collector leaves. A key still empties its boss room (an arena has no other
    // way out); everyone else in the course gets the note and plays on.
    const bool only = only_collector();
    const bool arena = only && kind == kKindKey;
    if (exits && only && !(arena && in_arena(lvl))) {
        const Mode m = mode_now();
        const bool unit = in_unit(lvl, act);
        const bool shared = star_shared_here(m);
        int killed = 0;
        if (unit && (shared || arena)) note_line(name, kind, course, star);
        if (unit && kind == kKindStar && shared && m == kParty) killed = kill_local_star(star);
        std::fprintf(stderr, "[starpop] rx seat=%d kind=%d level=%d act=%d star=%d: only the collector leaves (unit=%d shared=%d "
                     "note=%d removed=%d) wall=%lld\n", seat, kind, lvl, act, star, (int)unit, (int)shared,
                     (int)(unit && (shared || arena)), killed, wall_ms());
        if (arena && lvl >= 0) {            // I may be on my way into the arena: keep it, the ledger checks the level
            for (Ledger &l : g.led)
                if (!l.used) { l = { true, seat, kind, star, course, lvl, act, exits, now_ms(), true }; break; }
        }
        return;
    }
    const bool same = exits && (arena ? in_arena(lvl) : in_unit(lvl, act));
    if (!same) {
        if (g.log) std::fprintf(stderr, "[starpop] rx seat=%d kind=%d exits=%d same=0 -> toast wall=%lld\n", seat, kind, (int)exits, wall_ms());
        if (exits && lvl >= 0) {            // keep it: this console may still be loading or in star select
            for (Ledger &l : g.led)
                if (!l.used) { l = { true, seat, kind, star, course, lvl, act, exits, now_ms(), false }; break; }
        }
        return;
    }
    start_popup(name, kind, course, star);
    if (g.log) std::fprintf(stderr, "[starpop] rx seat=%d kind=%d exits=1 same=1 -> popup, countdown %s wall=%lld\n", seat, kind,
                            g.counting ? "running" : "pending exit", wall_ms());
}

// Every frame: an event kept in the ledger that now fits my course and act starts its popup (no second chat line).
void ledger_tick() {
    const unsigned now = now_ms();
    for (Ledger &l : g.led) {
        if (!l.used) continue;
        if ((unsigned)(now - l.t) > kLedgerMs) { l.used = false; continue; }
        if (l.arena ? !in_arena(l.lvl) : !in_unit(l.lvl, l.act)) continue;
        if (g.counting || g.exit_pending) { l.used = false; continue; }
        char name[24];
        seat_name(l.seat, name, sizeof name);
        if (l.kind == kKindGrand) {
            if (on_ending_flight((const char *)my_body())) { l.used = false; continue; }
            std::fprintf(stderr, "[starpop] late Grand Star: %d removed wall=%lld\n", kill_grand(), wall_ms());
        }
        start_popup(name, l.kind, l.course, l.star);
        std::fprintf(stderr, "[starpop] late event from seat %d now fits my course and act (%u ms after it arrived): popup wall=%lld\n", l.seat, now - l.t, wall_ms());
        l.used = false;
    }
}

// ---- wire -----------------------------------------------------------------
void queue_copies(const Pending &src) {
    for (Pending &p : g.pend) {
        if (p.used) continue;
        p = src; p.used = true; p.idx = 0; p.t0 = now_ms();
        return;
    }
}

void send_event(Mode m, int kind, bool exits, int star) {
    const int lv0 = data_0209f2f8;
    const int ckey = guard_key(lv0, course_of(lv0));
    if (exits && guard_dup(ckey, star, kind, false)) {      // a friend's collection of this star is already in: mine is the second one
        ++g.st.dup;
        std::fprintf(stderr, "[starpop] guard: my collection of course %d star %d came within %u ms of another; nothing sent wall=%lld\n", course_of(lv0), star, kGuardMs, wall_ms());
        return;
    }
    if (exits) guard_dup(ckey, star, kind, true);
    g.seq = (g.seq + 1) & 0xffu;
    Pending p; std::memset(&p, 0, sizeof p);
    if (m == kCoop) {
        PopWire w; std::memset(&w, 0, sizeof w);
        w.key.level = (uint16_t)(data_0209f2f8 & 0xff); w.key.act = (uint8_t)(data_0209f220[0] & 0xf);
        w.kind = (uint8_t)kind; w.flags = exits ? kFlagExits : 0; w.star = (uint8_t)(star & 0xf); w.seq = (uint8_t)g.seq;
        p.coop = true; p.w = w;
        coop_send_ctl(kCtlStarPop, &w, sizeof w);
    } else {
        const int lv = data_0209f2f8;
        int course = (lv >= 0 && lv < 52) ? SUBLEVEL_LEVEL_TABLE[lv] : 0;
        if (course < 0 || course > 29) course = 0;
        unsigned char b[kHubPartyStarBytes];
        if (hub_party_star_encode(course, star & 7, g.seq, b) != kHubPartyStarBytes) return;
        const unsigned rsv = (kind == kKindGrand ? kXGrand : kind == kKindKey ? kXKey : kXStar) | (exits ? 0u : kXNoExit);
        b[8] = (unsigned char)rsv; b[9] = 0;
        std::memcpy(p.buf, b, sizeof b); p.len = (int)sizeof b; p.coop = false;
        comms_hub_send(b, (int)sizeof b);
    }
    ++g.st.tx;
    g.armed = true;
    if (g.stop_kind == 2 && !g.stop_timer) { g.stop_timer = true; g.stop_at = now_ms() + (unsigned)g.stop_val; }
    queue_copies(p);
    std::fprintf(stderr, "[starpop] tx mode=%s kind=%d exits=%d star=%d level=%d act=%d seq=%u wall=%lld\n", m == kCoop ? "coop" : "party",
                 kind, (int)exits, star, (int)data_0209f2f8, data_0209f220[0], g.seq, wall_ms());
}

void send_copies() {
    const unsigned now = now_ms();
    for (Pending &p : g.pend) {
        if (!p.used) continue;
        if (p.idx >= kResendN) { p.used = false; continue; }
        if ((unsigned)(now - p.t0) < kResendAt[p.idx]) continue;
        if (p.coop) coop_send_ctl(kCtlStarPop, &p.w, sizeof p.w);
        else comms_hub_send(p.buf, p.len);
        ++p.idx;
    }
}

void on_ctl(int seat, int sub, const unsigned char *pl, int len, void *) {
    if (sub != kCtlStarPop) { ++g.st.dropped; return; }
    if (!g.enabled || mode_now() != kCoop) return;
    if (!pl || len != (int)sizeof(PopWire)) { ++g.st.dropped; return; }
    PopWire w; std::memcpy(&w, pl, sizeof w);
    if (!coop_key_ok(w.key) || (w.kind != kKindStar && w.kind != kKindKey && w.kind != kKindGrand) || (w.flags & ~kFlagExits) || w.star > 8 ||
        seat < 0 || seat >= kCoopMaxSeats || seat == my_seat()) { ++g.st.dropped; return; }
    if (seen_seq(seat, w.seq)) return;
    on_event(seat, w.kind, (w.flags & kFlagExits) != 0, (int)w.key.level, (int)w.key.act, course_of((int)w.key.level), w.star);
}

void scan_local(Mode m) {
    void *me = my_body();
    if (!me) return;
    // STAREXIT2: the Grand Star leaves no actor behind to watch (the collect marks it for destruction at the touch), so the
    // collect is seen on my own body: it entered St_EndingFly, which only the Grand Star's collect hands out. Not when I was put
    // there by a friend's event (grand_forced). Once per level.
    if (!g.grand_sent && !g.grand_forced && on_ending_flight((const char *)me)) {
        g.grand_sent = true;
        send_event(m, kKindGrand, true, 0);
        ending_as_mario((char *)me, "collect");
    }
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        const unsigned cls = *(const unsigned short *)(a + 0x0c);
        int kind = 0, star = 0;
        bool exits = false;
        if (cls == 178) {
            if (*(const unsigned *)(a + 0x440) != 5u || *(void *const *)(a + 0x438) != me) continue;
            const int r4 = _ZN8daStar_c19func_ov002_020e73acEv(a);
            if (r4 == 2) continue;                       // silver / VS: not a collect
            star = *(const unsigned char *)(a + 0x49d);
            kind = (r4 == 3) ? kKindKey : kKindStar;
            exits = (r4 == 0 || r4 == 3);
        } else if (cls == 282) {
            if (*(const int *)(a + 0x448) != 3 || *(void *const *)(a + 0x110) != me) continue;
            kind = kKindKey; exits = true; star = 8;
        } else continue;
        bool seen = false;
        for (int i = 0; i < g.nseen; ++i) seen = seen || g.seen[i] == a;
        if (seen || g.nseen >= kSeenMax) continue;
        g.seen[g.nseen++] = a;
        send_event(m, kind, exits, star);
    }
}

// Proof knob SM64DS_STAR_POPUP_CENSUS=<every>: every <every> frames in a level, my body's position and every PowerStar's
// (class 178) position, star number and take state, so a row can walk a scripted pad to a real star (STAREXIT1).
void census() {
    static int every = -1;
    if (every < 0) { const char *e = std::getenv("SM64DS_STAR_POPUP_CENSUS"); every = (e && *e) ? std::atoi(e) : 0; }
    static unsigned f = 0;
    if (every <= 0 || (++f % (unsigned)every) != 0) return;
    const char *me = (const char *)my_body();
    if (!me) return;
    const int *mp = (const int *)(me + 0x5c);
    std::fprintf(stderr, "[starpop] census f=%u level=%d act=%d me=%d,%d,%d yaw=%04x", f, (int)data_0209f2f8, data_0209f220[0] & 0xf,
                 mp[0] >> 12, mp[1] >> 12, mp[2] >> 12, (unsigned)(unsigned short)*(const short *)(me + 0x8e));
    // STAREXIT3: who my body is, the save file's character and my table byte, plus the ending's words (pending level change,
    // the running Kuppa script, the stage mode)
    std::fprintf(stderr, " chr body=%u save=%u tbl=%u lc=%d cut=%08x mode=%u", *(const unsigned *)(me + 8) & 0xffu, (unsigned)data_0209cad2[0x0f],
                 (unsigned)data_02092128[*(const unsigned char *)(me + 0x6d8) & 0x3f], (int)(signed char)data_02092110[0], rd32(data_0209fc48),
                 (unsigned)data_0209f2d8[0]);
    // CREDITS1: my health (wedges), my body's state routine and the Bowser / ending hold word, to see a hurt or a death in the ending
    std::fprintf(stderr, " hp=%d st=%p fc68=%u why=%u hud=%u ps=%u/%u lc=%u", (int)((data_02092144[(int)data_0209f250] >> 8) & 0xff),
                 *(void *const *)(me + 0x370), rd32(data_0209fc68), (unsigned)data_0209f2fc, (unsigned)data_ov002_02111178,
                 (unsigned)data_0209f2c4[0], (unsigned)data_0209f248, (unsigned)data_0209f20c[0]);
    for (const char *a = (const char *)_ZN8dActor_c4NextEPKS_(0); a; a = (const char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (*(const unsigned short *)(a + 0x0c) != 178) continue;
        const int *p = (const int *)(a + 0x5c);
        std::fprintf(stderr, " star(n=%u st=%u at %d,%d,%d)", (unsigned)*(const unsigned char *)(a + 0x49d), *(const unsigned *)(a + 0x440),
                     p[0] >> 12, p[1] >> 12, p[2] >> 12);
    }
    for (const char *a = (const char *)_ZN8dActor_c4NextEPKS_(0); a; a = (const char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (*(const unsigned short *)(a + 0x0c) != 283) continue;      // STAREXIT2: the Grand Star (LAST_STAR)
        const int *p = (const int *)(a + 0x5c);
        std::fprintf(stderr, " grand(at %d,%d,%d dead=%u)", p[0] >> 12, p[1] >> 12, p[2] >> 12, (unsigned)*(const unsigned char *)(a + 0x0f));
    }
    std::fprintf(stderr, " wall=%lld\n", wall_ms());
}

// Proof knob SM64DS_STAR_POPUP_NEAR=<level>:<frame>:<star n>:<dist>:<yaw hex>: <frame> frames into level <level>, put my body
// <dist> units BEFORE PowerStar number <n> along <yaw> (yaw 0 = +z, 4000 = +x), on the star's height, facing it. It never
// touches the star: the row's scripted pad walks the last <dist> units, so the ROM's own touch collects it (STAREXIT1).
// <dist> 0 arms the touch stand-in instead (below). Retried every 60 frames from <frame>.
// STAREXIT2: <n> -1 is the Grand Star (class 283); my body keeps its own height (the star hovers above the floor), and <yaw>
// is taken RELATIVE TO MY CAMERA: the ROM walks the body along (camera angle + stick angle), stick up being 0x8000, so <yaw> 0
// puts me where a pushed-up stick walks straight at the star, whatever way the camera happens to face.
void near_knob() {
    static int lvl = -2, fr = 0, n = 0, dist = 0; static unsigned yaw = 0; static bool done = false;
    if (lvl == -2) {
        lvl = -1;
        const char *e = std::getenv("SM64DS_STAR_POPUP_NEAR");
        if (!e || std::sscanf(e, "%d:%d:%d:%d:%x", &lvl, &fr, &n, &dist, &yaw) != 5) lvl = -1;
    }
    static int cur = -1; static int t = 0;
    if (lvl < 0 || done) return;
    if ((int)data_0209f2f8 != cur) { cur = (int)data_0209f2f8; t = 0; }
    char *me = (char *)my_body();
    static int tries = 0;
    if (cur != lvl || !me) return;
    ++t;
    if (t < fr || (t - fr) % 60 != 0) return;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (n == -1 ? *(const unsigned short *)(a + 0x0c) != 283
                    : (*(const unsigned short *)(a + 0x0c) != 178 || *(const unsigned char *)(a + 0x49d) != (unsigned)n)) continue;
        const int *p = (const int *)(a + 0x5c);
        if (dist == 0 && n != -1) {
            // <dist> 0: the touch stand-in instead (hal/coop_world.cpp knob_star, hal/input_probe.cpp SM64DS_STAR_TRIGGER):
            // the star's toucher word and touch flag, so the star's OWN collect handler runs. Re-armed every 60 frames, 8 tries.
            if (*(const unsigned *)(a + 0x440) == 5u || tries >= 8) { done = true; return; }
            *(unsigned *)(a + 0x134) = *(const unsigned *)(me + 4);
            *(unsigned *)(a + 0x130) |= 0x400000;
            ++tries;
            std::fprintf(stderr, "[starpop] TOUCH knob: star %d (state %u) touch armed, try %d wall=%lld\n", n,
                         *(const unsigned *)(a + 0x440), tries, wall_ms());
            return;
        }
        unsigned w = yaw;
        if (n == -1) {
            const int pi = (int)data_0209f250;
            const unsigned cam = (pi >= 0 && pi < 16) ? *(const unsigned short *)(data_020a1164 + pi * 0x24) : 0u;
            w = (cam + 0x8000u + yaw) & 0xffffu;
            std::fprintf(stderr, "[starpop] NEAR knob: camera angle %04x, walk line %04x\n", cam, w);
        }
        const double r = (double)(w & 0xffff) * 6.283185307179586 / 65536.0;
        int *mp = (int *)(me + 0x5c);
        mp[0] = p[0] - (int)(std::sin(r) * dist * 4096.0);
        if (n != -1) mp[1] = p[1];
        mp[2] = p[2] - (int)(std::cos(r) * dist * 4096.0);
        *(short *)(me + 0x8e) = (short)w;
        done = true;
        std::fprintf(stderr, "[starpop] NEAR knob: me at %d,%d,%d yaw %04x, %d units before star %d at %d,%d,%d wall=%lld\n",
                     mp[0] >> 12, mp[1] >> 12, mp[2] >> 12, w & 0xffff, dist, n, p[0] >> 12, p[1] >> 12, p[2] >> 12, wall_ms());
        return;
    }
    std::fprintf(stderr, "[starpop] NEAR knob: no star %d in level %d\n", n, cur);
    done = true;
}

// Proof knob SM64DS_STAR_POPUP_WARP=<frame>:<level>:<act>[;...] (up to 4 steps): <frame> frames of standing after the last step (or after my body first stands anywhere), the game's own
// level change call (hal/coop_world.cpp SM64DS_COOP_WARP is the same for coop rooms only), so a party row reaches a course
// without a walk through a painting (STAREXIT1). Once.
void warp_knob() {
    struct Step { int fr, lv, ac; };
    static Step st[4]; static int n = -1, k = 0, t = 0;
    if (n < 0) {
        n = 0;
        if (const char *e = std::getenv("SM64DS_STAR_POPUP_WARP")) {
            const char *p = e; int used = 0;
            while (n < 4 && std::sscanf(p, "%d:%d:%d%n", &st[n].fr, &st[n].lv, &st[n].ac, &used) == 3 && st[n].fr >= 0) {
                ++n; p += used;
                if (*p != ';' && *p != ',') break;
                ++p;
            }
        }
    }
    if (k >= n || !my_body() || ++t < st[k].fr) return;
    std::fprintf(stderr, "[starpop] WARP knob: to level %d act %d wall=%lld\n", st[k].lv, st[k].ac, wall_ms());
    LoadLevelNoReturn((unsigned)st[k].lv, 0, (unsigned)st[k].ac, 1);
    ++k;
    t = 0;
}

// Proof knob SM64DS_STAR_POPUP_GRAND=<level>:<frame>:<x>:<y>:<z> (STAREXIT2): <frame> frames into level <level>, the Grand Star
// (LAST_STAR, class 0x11b, parameter 7) at <x>,<y>,<z> (units) through the ROM's own spawn, the call Bowser's defeat makes
// (src/game/actors/d_a_kpa.cpp: Spawn(BOWSER_ACTOR_LAST_STAR, 7, pos, 0, mAreaId, -1), 160 units above him). A stand-in for the
// fight ONLY: the star then runs its own StateStarJump and is collected by its own touch. Once per level entry.
// Bowser himself (BOWSER 279, BOWSER_TAIL 278, BOWSER_FIRE 280) is marked for destruction first, as if beaten: the knob runs
// before the fight's own intro, and a live Bowser holds the player in his talk and stands where the star lands.
void grand_knob() {
    static int lvl = -2, fr = 0, x = 0, y = 0, z = 0; static int cur = -1, t = 0; static bool done = false;
    if (lvl == -2) {
        lvl = -1;
        const char *e = std::getenv("SM64DS_STAR_POPUP_GRAND");
        if (!e || std::sscanf(e, "%d:%d:%d:%d:%d", &lvl, &fr, &x, &y, &z) != 5) lvl = -1;
    }
    if (lvl < 0) return;
    if ((int)data_0209f2f8 != cur) { cur = (int)data_0209f2f8; t = 0; done = false; }
    const char *me = (const char *)my_body();
    if (cur != lvl || !me || done || ++t < fr) return;
    done = true;
    int gone = 0;
    for (char *b = (char *)_ZN8dActor_c4NextEPKS_(0); b; b = (char *)_ZN8dActor_c4NextEPKS_(b)) {
        const unsigned c = *(const unsigned short *)(b + 0x0c);
        if ((c == 278 || c == 279 || c == 280) && !*(const unsigned char *)(b + 0x0f)) { _ZN7fBase_c18MarkForDestructionEv(b); ++gone; }
    }
    const int pos[3] = { x * 4096, y * 4096, z * 4096 };
    char *a = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x11b, 7, pos, nullptr, (signed char)me[0xcc], -1);
    std::fprintf(stderr, "[starpop] GRAND knob: Bowser actors removed %d, Grand Star spawned at %d,%d,%d -> %p (level %d frame %d) wall=%lld\n",
                 gone, x, y, z, (void *)a, cur, t, wall_ms());
}

void new_level_check() {
    const int lv = data_0209f2f8, ac = data_0209f220[0];
    if (!g.have_key || lv != g.lvl || ac != g.act) {
        if (g.have_key && g.armed && g.stop_kind == 3 && !g.stop_timer) { g.stop_timer = true; g.stop_at = now_ms() + (unsigned)g.stop_val; }
        g.have_key = true; g.lvl = lv; g.act = ac;
        g.nseen = 0;
        g.grand_sent = false; g.grand_forced = false;
        if ((g.counting || g.exit_pending || g.nline) && g.log)
            std::fprintf(stderr, "[starpop] level changed (%d/%d): popup cleared\n", lv, ac);
        clear_receiver();
    }
}

}  // namespace

bool star_popup_party_rx(int seat, const unsigned char *d, int len) {
    if (!d || len != kHubPartyStarBytes || hub_rd16(d, 8) == 0) return false;
    init_once();
    const unsigned rsv = hub_rd16(d, 8);
    if (!g.enabled || mode_now() != kParty) return true;
    if ((rsv & ~7u) || (rsv & 3u) == 0u || seat < 0 || seat >= 16 || seat == my_seat() ||
        (comms_hub_room_kind() == kHubRoomWorld && !world_party_member(seat))) {
        ++g.st.dropped;
        return true;
    }
    if (seen_seq(seat, hub_rd16(d, 10))) return true;
    HubPresence p;
    unsigned age = 0;
    const bool pres = hub_presence_get(seat, &p, &age);
    const bool here = pres && age < 5000 && p.in_level();
    HubPartyStarWire pw; std::memcpy(&pw, d, sizeof pw);
    on_event(seat, (rsv & 3u) == kXGrand ? kKindGrand : (rsv & 3u) == kXKey ? kKindKey : kKindStar, (rsv & kXNoExit) == 0, here ? p.level : -1, here ? p.act : -1,
             (int)pw.course, d[7]);
    return true;
}

// STARPOP2: UP during the choice: the exit starts now, through the same safe-frame gates the old countdown's end used.
void choose_exit(unsigned now) {
    g.counting = false; g.choice = false; g.exit_pending = true; g.t_pend = now; g.last_wait_log = 0; g.cleared_d4b0 = false;
    ++g.st.ups;
    std::fprintf(stderr, "[starpop] UP: I leave the stage wall=%lld\n", wall_ms());
}

unsigned star_popup_exit_input(unsigned pad_buttons, unsigned up_mask, bool key_down) {
    if (!g.counting || !g.choice) return 0;
    const unsigned mine = pad_buttons & up_mask;
    const bool down = mine != 0 || key_down;
    if (!down) { g.up_armed = true; return 0; }
    if (g.up_armed) choose_exit(now_ms());
    g.up_armed = false;
    return mine;            // held or pressed, the choice's UP never also walks
}

void star_popup_view(StarPopupView *out) {
    out->n = 0;
    out->count = 0;
    if (!g.counting && !g.exit_pending && g.nline == 0) return;
    const unsigned now = now_ms();
    for (int i = 0; i < g.nline && out->n < kStarPopupLines; ++i)
        std::snprintf(out->line[out->n++], kStarPopupLineMax, "%s", g.line[i]);
    if (g.counting && out->n < kStarPopupLines) {
        int left = ((int)(g.t_end - now) + 999) / 1000;
        if (left < 1) left = 1;
        if (left > 5) left = 5;
        if (g.choice) {
            std::snprintf(out->line[out->n++], kStarPopupLineMax, "Press \x01 to exit stage");
            out->count = left;
        } else {
            std::snprintf(out->line[out->n++], kStarPopupLineMax, "Exiting in %d...", left);
        }
        if (g.stop_kind == 1 && left == g.stop_val && !g.stop_timer) { g.stop_timer = true; g.stop_at = now + 150; }
    } else if (g.exit_pending && out->n < kStarPopupLines) {
        std::snprintf(out->line[out->n++], kStarPopupLineMax, "Exiting in 1...");
    }
}

StarPopupStats star_popup_stats() { return g.st; }

void star_popup_star_name(int course, int star, char *out, int cap) { star_title(course, star, out, cap); }
bool star_popup_only_collector() { return only_collector(); }
void star_popup_set_only_collector(int on) {
    g_only_override = on < 0 ? -1 : (on ? 1 : 0);
    std::fprintf(stderr, "[starpop] only-the-collector-leaves rule set to %d (-1 = the room's SM64DS_ROOM_SOLOEXIT)\n", g_only_override);
}

}  // namespace port

#ifndef PORT_ROM_CLEAN
extern "C" int port_adventure_ghost_seat(int slot);
// CREDITS1 proof knob SM64DS_CREDITS_WATCH=1 (dev only): one [crwatch] line whenever the level-clear flag (data_0209f20c), the
// pause / save menu state (data_0209f2c4), my health or my state routine changes, with every live body's state routine, so a row
// can tell which body started a course-clear and when my player was hurt or died.
static void credits_watch() {
    using namespace port;
    static int on = -1;
    if (on < 0) { const char *e = std::getenv("SM64DS_CREDITS_WATCH"); on = (e && *e && *e != '0') ? 1 : 0; }
    if (!on) return;
    static unsigned last_lc = 0, last_ps = 0, frame = 0; static int last_hp = -1, last_lv = -1; static const void *last_st = nullptr;
    ++frame;
    const int me = (int)data_0209f250;
    const char *b = (me >= 0 && me < 32) ? (const char *)data_0209f394[me] : nullptr;
    const unsigned lc = data_0209f20c[0], ps = data_0209f2c4[0];
    const int hp = (me >= 0 && me < 32) ? (int)((data_02092144[me] >> 8) & 0xff) : -1;
    const void *st = b ? *(void *const *)(b + 0x370) : nullptr;
    const int lv = (int)data_0209f2f8;
    if (lc == last_lc && ps == last_ps && hp == last_hp && st == last_st && lv == last_lv) return;
    std::fprintf(stderr, "[crwatch] f%u level %d mode %u cut %08x: levelclear %u->%u menu %u->%u hp %d->%d my state %p->%p why %u hud %u |",
                 frame, lv, (unsigned)data_0209f2d8[0], rd32(data_0209fc48), last_lc, lc, last_ps, ps, last_hp, hp, last_st, st,
                 (unsigned)data_0209f2fc, (unsigned)data_ov002_02111178);
    if (lc != last_lc || ps != last_ps)
        for (int i = 0; i < 32; ++i) {
            const char *a = (const char *)data_0209f394[i];
            if (!a) continue;
            const int *p = (const int *)(a + 0x5c);
            std::fprintf(stderr, " b%d(seat %d st %p at %d,%d,%d)", i, i == me ? -1 : port_adventure_ghost_seat(i), *(void *const *)(a + 0x370),
                         p[0] >> 12, p[1] >> 12, p[2] >> 12);
            if (i > 4) break;
        }
    if (b && st != last_st && data_0209f2d8[0] == 2) {    // a state change in the ending: what stands right under / around me
        const int *mp = (const int *)(b + 0x5c);
        std::fprintf(stderr, " me at %d,%d,%d near:", mp[0] >> 12, mp[1] >> 12, mp[2] >> 12);
        int n = 0;
        for (const char *a = (const char *)_ZN8dActor_c4NextEPKS_(0); a && n < 12; a = (const char *)_ZN8dActor_c4NextEPKS_(a)) {
            if (a == b) continue;
            const int *p = (const int *)(a + 0x5c);
            const int dx = (p[0] - mp[0]) >> 12, dy = (p[1] - mp[1]) >> 12, dz = (p[2] - mp[2]) >> 12;
            if (dx > 600 || dx < -600 || dz > 600 || dz < -600 || dy > 600 || dy < -900) continue;
            std::fprintf(stderr, " cls%u(%d,%d,%d)", (unsigned)*(const unsigned short *)(a + 0x0c), dx, dy, dz);
            ++n;
        }
    }
    std::fprintf(stderr, " wall=%lld\n", wall_ms());
    last_lc = lc; last_ps = ps; last_hp = hp; last_st = st; last_lv = lv;
}
#endif

extern "C" void port_star_popup_frame(void) {
    using namespace port;
#ifndef PORT_ROM_CLEAN
    credits_watch();
#endif
    census();          // proof knobs, solo too
    keep_tick();       // STAREXIT3: the saved character through the ending, before every return (the ending is no popup, and a
                       // lost room must neither freeze nor leave it armed)
    near_knob();
    warp_knob();
    grand_knob();
    if (!std::getenv("SM64DS_PARTY") && !std::getenv("SM64DS_COOP") &&
        !(world_party_active() && comms_hub_room_kind() == kHubRoomWorld)) return;
    init_once();
    ++g.frame;
    const Mode m = mode_now();
    if (m == kNone) { if (g.counting || g.exit_pending || g.nline) clear_receiver(); return; }
    if (!g.registered && m == kCoop) {
        g.registered = coop_ctl_register(kCtlStarPop, kCtlStarPop, on_ctl, nullptr);
        std::fprintf(stderr, "[starpop] coop wire %s\n", g.registered ? "registered" : "REFUSED");
    }
    new_level_check();
    for (Fake &f : g.fake)
        if (f.used && !f.done && g.frame >= f.frame && my_body()) {
            f.done = true;
            send_event(m, f.kind, f.exits != 0, f.kind == kKindKey ? 8 : (g.fake_star ? g.fake_star : 1));
        }
    scan_local(m);
    send_copies();
    const unsigned now = now_ms();
    ledger_tick();
    if (g.stale_frame && !g.stale_done && g.frame >= g.stale_frame && my_body()) {
        g.stale_done = true;
        stale_d4b0_plant();
    }
    if (g.stop_timer && !g.stop_flag && (int)(now - g.stop_at) >= 0) {
        g.stop_flag = true;
        std::fprintf(stderr, "[starpop] STOP requested (picture) wall=%lld\n", wall_ms());
    }
    if (g.counting && g.choice && g.auto_up_ms && (int)(now - (g.t_end - kCountMs + g.auto_up_ms)) >= 0) {
        std::fprintf(stderr, "[starpop] proof knob: UP pressed %u ms into the choice wall=%lld\n", g.auto_up_ms, wall_ms());
        choose_exit(now);
    }
    if (g.counting && (int)(now - g.t_end) >= 0) {
        if (g.choice) {         // STARPOP2: nobody pressed UP: I stay in the stage, the box goes
            std::fprintf(stderr, "[starpop] no UP within %u ms: I stay wall=%lld\n", (unsigned)kCountMs, wall_ms());
            ++g.st.stays;
            clear_receiver();
        } else {
            g.counting = false; g.exit_pending = true; g.t_pend = now; g.last_wait_log = 0; g.cleared_d4b0 = false;
        }
    }
    if (g.exit_pending) {
        const char *why = "";
        bool hard = false;
        const int gate = exit_gate(&why, &hard);
        const unsigned waited = now - g.t_pend;
        if (gate == 2) {
            std::fprintf(stderr, "[starpop] already leaving: exit dropped wall=%lld\n", wall_ms());
            clear_receiver();
        } else {
            // soft gates give up after kSoftMs (a stuck fader pointer is cleared first, as the ROM would when its fade ends);
            // anything still in the way after kForceMs is forced, unless the console has no body at all (loading: the level
            // change clears the exit by itself).
            bool go = (gate == 0);
            if (gate == 1 && g.no_timeout) {
                // control: no limit
            } else if (gate == 1 && !hard && waited >= kSoftMs) {
                if (!std::strcmp(why, "d4b0")) clear_stale_d4b0();
                std::fprintf(stderr, "[starpop] soft gate '%s' timed out after %u ms: exit goes ahead wall=%lld\n", why, waited, wall_ms());
                go = true;
            } else if (gate == 1 && waited >= kForceMs) {
                if (!my_body()) {
                    std::fprintf(stderr, "[starpop] no body after %u ms: exit dropped wall=%lld\n", waited, wall_ms());
                    clear_receiver();
                    return;
                }
                std::fprintf(stderr, "[starpop] hard gate '%s' still up after %u ms: exit forced wall=%lld\n", why, waited, wall_ms());
                go = true;
            }
            if (!go) {
                if ((unsigned)(now - g.last_wait_log) > 1000u) {
                    g.last_wait_log = now;
                    std::fprintf(stderr, "[starpop] exit waits: %s%s %u ms (msg=%u d4b0=%08x starsel=%u pause=%u lc=%u fade=%u cut=%u tmr=%u mode=%u) wall=%lld\n",
                                 why, hard ? " (hard)" : "", waited,
                                 (unsigned)data_0209d660[0], rd32(data_0209d4b0), (unsigned)data_0209f294[0], (unsigned)data_0209f2c4[0],
                                 (unsigned)data_0209f20c[0], rd32(data_0209b454), rd32(data_0209fc48), (unsigned)data_0209f22c[0],
                                 (unsigned)data_0209f2d8[0], wall_ms());
                }
            } else if (g.exit_grand) {
                // STAREXIT2: the Grand Star's collect with my body (see kill_grand): the ending flight, the event bit, my copy gone.
                char *me = (char *)my_body();
                const int removed = kill_grand();
                ending_as_mario(me, "exit");
                func_ov002_020c3dbc(me);
                _ZN5Event6SetBitEj(kEventKey);
                if (on_ending_flight(me)) {
                    g.grand_forced = true;
                    ++g.st.exits;
                    g.armed = true;
                    std::fprintf(stderr, "[starpop] EXIT GRAND from level %d: my body is on the ending flight (removed %d) after %u ms of waiting wall=%lld\n",
                                 (int)data_0209f2f8, removed, waited, wall_ms());
                    g.exit_pending = false; g.nline = 0; g.exit_grand = false;
                } else if ((unsigned)(now - g.last_wait_log) > 1000u) {      // the state change was refused this frame: again next frame
                    g.last_wait_log = now;
                    std::fprintf(stderr, "[starpop] GRAND: the ending flight was refused (state %p), retrying wall=%lld\n",
                                 me ? *(void **)(me + 0x370) : nullptr, wall_ms());
                }
            } else {
                landing_overrides(g.exit_key);
                if (g.exit_star >= 0) data_0209f228 = (unsigned char)g.exit_star;   // the course-clear screen names the star the collector took (the ROM sets it at the touch)
                const int from = data_0209f2f8;
                ExitLevel();
                ++g.st.exits;
                g.armed = true;
                std::fprintf(stderr, "[starpop] EXIT from level %d act %d key=%d after %u ms of waiting (ROM override globals now %d/%u, -1/0 = default exit) wall=%lld\n", from, data_0209f220[0],
                             (int)g.exit_key, waited, (int)data_0209211c, (unsigned)data_0209f200, wall_ms());
                g.exit_pending = false; g.nline = 0;
            }
        }
    }
    if (!g.counting && !g.exit_pending && g.nline &&
        (unsigned)(now - g.t_last) > (g.note_only ? (unsigned)kNoteMs : (unsigned)(kShowAfterMs + kCountMs))) { g.nline = 0; g.note_only = false; }
}

extern "C" int port_star_popup_stop_now(void) { return port::g.stop_flag ? 1 : 0; }
