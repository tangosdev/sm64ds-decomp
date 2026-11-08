// COOP SWITCHES AND LEVEL GLOBALS (phase 2). Run mp1, lane COOPSW1. See coop_switch.h.
//
// Everything here hangs off the coop world core's callbacks (coop_world.h):
// a frame hook, the level-latched hook, the established hook and the owner's
// snapshot-request hook. No game file is edited; the ROM's own entries are
// called (the switch's press, the chest's SetState) or its own fields written.
//
// PORT_HOST_ABI. Actor offsets are the ROM's (32-bit host): class +0x0c,
// placed-object slot +0x0ce, dead flags +0x0f / +0x0e.

#include "coop_switch.h"
#include "coop_world.h"
#include "coop_wire.h"
#include "comms_hub.h"
#include "hub_presence.h"

#include <windows.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern int data_0209f34c;
extern unsigned char data_0209f2c0[];
extern unsigned char data_0209caa0[];
void *_ZN8dActor_c4NextEPKS_(const void *prev);
void _ZN13daObjSwitch_c15OnGroundPoundedER8dActor_c(char *self);
// PUZZLES9. Flat C faces of the ROM's own entries (receiver first, cdecl), as the matched src calls them.
int _ZN11daObjTbox_c10InitState0Ev(void *c);       // Entry[0].enter: the model frame back to 0
int _ZN11daObjTbox_c10InitState1Ev(void *c);       // Entry[1].enter: the opening
void _ZN11daObjPile_c19func_ov091_021334b8Ei(char *c, int flag);         // Stump: one pound (flag 0) or the whole post (flag 1)
void _ZN7fBase_c18MarkForDestructionEv(void *self);    // COOPPUZZLE1: a book the receiver's shelf can no longer lead
void ChangeArea(int areaID);                         // COOPPUZZLE1 proof knob: what a door does at the end of its swing
// COOPRIDE1 (G40): the spike bomb's own steps, forwarded at the end of this file (daKirai_c, src/game/actors/d_a_kirai.cpp).
void coopsw_kirai_explode(void *bomb);               // func_ov060_021185c4: Bowser's hit: particles, quake, sound, disarm
void coopsw_kirai_disarm(void *bomb);                // func_ov060_021184bc: give up the slot, stop colliding, state 3
void coopsw_kirai_rearm(void *bomb);                 // func_ov060_021186d8: full size and opacity, state 0, take a slot
extern signed char data_ov100_02148204[];            // door table, 16 bytes a variant: +8 stars, +9 key id, +0xa save bit
extern signed char data_ov100_02148390[];            // star gate table, 6 bytes a variant: +0 stars, +1 save bit
extern char data_ov100_021488d4[];                   // Door node: the star-door unlock (func_ov100_02144bf4)
extern char data_ov100_021488e4[];                   // Door node: the key-door unlock (func_ov100_021449c8)
extern char _ZN12daStarGate_c12ST_UNLOCKINGE[];      // star gate state: the unlock cutscene
int _ZN6Player13TryTalkToDoorEh(void *player, unsigned char a);
int _ZN6Player16TryTalkToKeyDoorEv(void *player);
void _ZN6Player11OpenBigDoorEv(void *player);
int _ZN8daDoor_c19func_ov100_021453d8EPvi(void *door, void *node, int arg);   // set a Door's node and run its enter half
void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);   // dBgActor_c: model matrix from mAngleY + the place (src/runtime/actor/d_bg_actor.cpp)
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);     // dBgActor_c: the mesh collider follows the model
}

namespace port {
namespace {

long long wall_ms() {
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    return (long long)((((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000ull);
}

enum : unsigned {
    kResendMs      = 100,      // each change goes out 3 times: now, +100, +200
    kHoldMs        = 10000,    // COOPPUZZLE2: how long a field change waits for somebody to be listed
    kTieMs         = 500,      // COOPPUZZLE3: a single-value field change this young still wins a tie against a higher seat
    kWordOwnerMs   = 1000,
    kWordGuestMs   = 3000,
    kSettleFrames  = 20,       // frames after the objects exist before the word is trusted as ours
    kGlobalsWait   = 180,      // a joiner stops waiting for the owner's globals after this many frames
    kStatesAge     = 900,      // pending object states are dropped after this many frames
    kJobMs         = 150,
};
enum : int { kPendMax = 64, kObjOutMax = 48 };

// ---------------------------------------------------------------------------
// State.
// ---------------------------------------------------------------------------
bool g_env = false, g_log = false;
struct Off { bool word, press, kick, field, globals; } g_off = {};
CoopSwitchStats g_st = {};
bool g_registered = false, g_hooked = false;
int g_my = -1;

// the current group
bool     g_have_key = false;
int      g_lvl = -1, g_act = 0;
unsigned g_gen = 0;             // bumped at every level latch
unsigned g_frames = 0;          // frames since the group was established and the player existed
unsigned g_now = 0;             // COOPPUZZLE2: the clock of the last on_frame (a remote apply runs before this frame's on_frame)
bool g_keep_local = true;       // COOPPUZZLE2: SM64DS_SW_KEEP_LOCAL=0 is the control (a remote apply swallows an unsent local change)
bool g_hold_field = true;       // COOPPUZZLE2: SM64DS_SW_HOLD_FIELD=0 is the control (a field change with nobody listed is dropped)
bool     g_new_level = true;
bool     g_armed = false;
bool     g_have_globals = false;
unsigned g_prev_word = 0;       // what the word was when I last looked
unsigned g_shadow = 0;          // the word I have announced or applied
unsigned g_counter = 0;         // my Lamport counter
SwBitModel g_bit[kSwBits];
unsigned g_word_t = 0;

struct PendBit { bool on; uint8_t val; unsigned stamp; int sent; unsigned next; };
PendBit g_pb[kSwBits];

// object state: trackers by slot, outgoing messages, pending states from the owner
struct Trk { void *actor; unsigned short cls; int last; bool valid; bool mine_on; unsigned mine_at; };   // mine_*: my last field send
Trk g_trk[kSwSlots];
struct ObjOut { bool on; uint8_t kind; uint16_t slot, cls; uint8_t state; int32_t value; uint16_t seq; int sent; unsigned next; unsigned born; };
ObjOut g_oo[kObjOutMax];
uint16_t g_seq = 0;
uint16_t g_lastseq[kCoopMaxSeats][kSwSlots];
struct Pend { bool on; uint8_t kind; uint16_t slot, cls; uint8_t state; int32_t value; unsigned born; };
Pend g_pend[kPendMax];

// the owner's answers to snapshot requests
struct Job { bool on; int seat; uint32_t nonce; int sent; unsigned next; unsigned born; };
Job g_job[kCoopMaxSeats];
unsigned g_states_t = 0;

// an early GLOBALS / STATES from a sender not yet a member
struct Early { bool on; int seat; int kind; int lvl, act; unsigned char buf[sizeof(SwStatesHead) + kSwMaxStateEnt * sizeof(SwStateEntry)]; int len; };
Early g_early[2];

// proof knobs
struct Knob { unsigned frame; int bit; bool set; bool done; };
Knob g_knob[8]; int g_nknob = 0;
struct Forge { unsigned frame; char name[12]; bool done; };
Forge g_forge[8]; int g_nforge = 0;
struct PressKnob { unsigned frame; int cls; int nth; bool done; };
PressKnob g_press[4]; int g_npress = 0;
struct WatchK { int cls, off, size; };
WatchK g_watch = {-1, 0, 0};
struct WatchTrk { void *a; unsigned v; bool valid; };
WatchTrk g_wtrk[kSwSlots];
unsigned g_rec[3] = {0, 0, 0}; bool g_rec_on = false; int g_rec_n = 0;

// ---------------------------------------------------------------------------
// Env.
// ---------------------------------------------------------------------------
void read_env() {
    if (g_env) return;
    g_env = true;
    const char *l = std::getenv("SM64DS_COOP_LOG");
    g_log = l && *l && *l != '0';
    if (const char *k = std::getenv("SM64DS_SW_KEEP_LOCAL")) g_keep_local = *k != '0';
    if (const char *h = std::getenv("SM64DS_SW_HOLD_FIELD")) g_hold_field = *h != '0';
    if (const char *o = std::getenv("SM64DS_SW_OFF")) {
        g_off.word = std::strstr(o, "word") != nullptr;
        g_off.press = std::strstr(o, "press") != nullptr;
        g_off.kick = std::strstr(o, "kick") != nullptr;
        g_off.field = std::strstr(o, "field") != nullptr;
        g_off.globals = std::strstr(o, "globals") != nullptr;
    }
    for (int pass = 0; pass < 2; ++pass) {
        const char *e = std::getenv(pass ? "SM64DS_SW_CLEAR" : "SM64DS_SW_SET");
        if (!e) continue;
        const char *p = e;
        while (*p && g_nknob < 8) {
            unsigned f = 0; int b = 0, used = 0;
            if (std::sscanf(p, "%u:%i%n", &f, &b, &used) < 2 || used <= 0) break;
            g_knob[g_nknob++] = {f, b, pass == 0, false};
            p += used;
            while (*p == ';' || *p == ',' || *p == ' ') ++p;
        }
    }
    if (const char *f = std::getenv("SM64DS_SW_FORGE")) {
        const char *p = f;
        while (*p && g_nforge < 8) {
            unsigned fr = 0; char nm[12] = {}; int used = 0;
            if (std::sscanf(p, "%u:%11[a-z0-9]%n", &fr, nm, &used) < 2 || used <= 0) break;
            g_forge[g_nforge].frame = fr; std::memcpy(g_forge[g_nforge].name, nm, sizeof nm);
            g_forge[g_nforge].done = false; ++g_nforge;
            p += used;
            while (*p == ',' || *p == ';' || *p == ' ') ++p;
        }
    }
    if (const char *pk = std::getenv("SM64DS_SW_PRESS")) {
        // F:cls[:nth][;F:cls[:nth]]  press the nth (lowest slot first) live actor of the class at group frame F
        const char *p = pk;
        while (*p && g_npress < 4) {
            unsigned f = 0; int c = 0, n = 0, used = 0;
            if (std::sscanf(p, "%u:%i:%i%n", &f, &c, &n, &used) < 3 || used <= 0) {
                used = 0;
                if (std::sscanf(p, "%u:%i%n", &f, &c, &used) < 2 || used <= 0) break;
            }
            g_press[g_npress++] = {f, c, n, false};
            p += used;
            while (*p == ';' || *p == ',' || *p == ' ') ++p;
        }
    }
    if (const char *w = std::getenv("SM64DS_SW_WATCH")) {
        int c = 0, o = 0, s = 0;
        if (std::sscanf(w, "%i:%i:%i", &c, &o, &s) == 3 && (s == 1 || s == 2 || s == 4) && o >= 0 && o < 0x800)
            g_watch = {c, o, s};
    }
    if (const char *r = std::getenv("SM64DS_COOP_REC")) {
        unsigned a = 0, b = 0, c = 0;
        if (std::sscanf(r, "%u:%u:%u", &a, &b, &c) == 3 && a < b && b < c) { g_rec[0] = a; g_rec[1] = b; g_rec[2] = c; g_rec_on = true; }
    }
}

// ---------------------------------------------------------------------------
// Actors.
// ---------------------------------------------------------------------------
inline unsigned short a_cls(const char *a) { return *(const unsigned short *)(a + 0x0c); }
inline short a_slot(const char *a) { return *(const short *)(a + 0xce); }
inline bool a_dead(const char *a) { return *(const unsigned char *)(a + 0x0f) || *(const unsigned char *)(a + 0x0e) == 2; }
bool is_player_ptr(const void *a) {
    for (int i = 0; i < 16; ++i) if (data_0209f394[i] == a) return true;
    return false;
}

// ---- the watch table: how to read a state and how to put one in ------------
// kind press: announce 0 -> non-zero, replay = the class's own press entry
// kind kick:  announce 0 -> 1, replay = the class's own trigger
// kind field: announce every change, replay = write the field
struct Watch {
    unsigned short cls; int kind; const char *name;
    int (*read)(const char *a);
    bool (*apply)(char *a, int state, int value);   // false = refused (not idle)
};
int rd_i32(const char *a, int off) { return *(const int *)(a + off); }
int rd_u8(const char *a, int off) { return *(const unsigned char *)(a + off); }
int r_starsw(const char *a) { return rd_i32(a, 0x340); }
bool ap_starsw(char *a, int, int) {
    if (rd_i32(a, 0x340) != 0) return false;     // the ROM's own early return makes a repeat a no-op
    _ZN13daObjSwitch_c15OnGroundPoundedER8dActor_c(a);
    return true;
}
int r_bcsw(const char *a) { return rd_u8(a, 0x32c); }
bool ap_bcsw(char *a, int, int) {
    if (rd_u8(a, 0x32c) != 0) return false;
    *(unsigned char *)(a + 0x32c) = 1;            // what the switch's collision callback stores
    return true;
}
int r_donut(const char *a) { return rd_u8(a, 0x4ea); }
bool ap_donut(char *a, int state, int) {
    if (rd_u8(a, 0x4ea) != 0) return false;       // idle only
    *(unsigned char *)(a + 0x4ea) = (unsigned char)state;
    return true;
}
// daObjFallBlock_c family (WF 45, BBH 66, LLL 83, BFS 139): state at +0x33c, idle 0; the shake starts from
// case 0 of its own Behavior (ready off, timer 4, state 1), which is what the replay does.
int r_fall(const char *a) { return rd_u8(a, 0x33c); }
bool ap_fall(char *a, int, int) {
    if (rd_u8(a, 0x33c) != 0) return false;
    *(unsigned char *)(a + 0x340) = 0;
    *(unsigned short *)(a + 0x33a) = 4;
    *(unsigned char *)(a + 0x33c) = 1;
    return true;
}
// CANNON_HATCH (14): +0x32c mOpening starts the slide (what func_ov002_020bc990 stores after the sound;
// the cannon-unlocked save bit itself travels with the save record).
int r_hatch(const char *a) { return rd_u8(a, 0x32c); }
bool ap_hatch(char *a, int, int) {
    if (rd_u8(a, 0x32c) != 0) return false;
    *(unsigned char *)(a + 0x32c) = 1;
    return true;
}
// The big "?" switch (HATENA_SWITCH 26): the shared fact is bit 31 of the save record word [1]
// (the ? blocks read it); the press itself is +0x71a with the 8-frame countdown at +0x718.
int r_hatena(const char *) { return (int)((*(const unsigned *)(data_0209caa0 + 4)) >> 31); }
bool ap_hatena(char *a, int, int) {
    if ((*(const unsigned *)(data_0209caa0 + 4)) & 0x80000000u) return false;
    *(unsigned char *)(a + 0x718) = 0;
    *(unsigned char *)(a + 0x71a) = 1;
    return true;
}
int r_water(const char *a) { return rd_i32(a, 0x334); }
bool ap_water(char *a, int, int value) {
    if (rd_i32(a, 0x334) == value) return false;
    *(int *)(a + 0x334) = value;                  // the water moves toward it on its own
    return true;
}
// ---- PUZZLES9 ---------------------------------------------------------------------------------------------------
// TREASURE_CHEST (13): mState +0x16c (0 closed, 1 opening, 2 open), mOrder +0x172, mIsLastChest +0x173. Every chest
// counts the OTHER chests whose mState is 1 or 2 (State0), so the opened set must be equal on every console. Shared as
// a field: value = (state != 0) | last << 1; the replay is the ROM's own enter half of state 1 (InitState1), or of
// state 0 for the reset the wrong chest does to the others. The last chest then runs State2 natively and spawns the
// star only on the console whose player opened it (the replay zeroes the timer; coop_spawn announces that star).
int r_chest(const char *a) {
    if (rd_i32(a, 0x16c) == 0) return 0;
    return 1 | (rd_u8(a, 0x173) ? 2 : 0);
}
bool ap_chest(char *a, int, int value) {
    const int st = rd_i32(a, 0x16c);
    if (value & 1) {
        if (st != 0) return false;                    // already opening or open
        *(unsigned char *)(a + 0x173) = (unsigned char)((value >> 1) & 1);
        *(int *)(a + 0x16c) = 1;
        _ZN11daObjTbox_c10InitState1Ev(a);
        // The star: only the console whose player opened the last chest spawns it (State2 does it when mStateTimer, which
        // InitState1 set to 0x2d for the last chest, runs out) and coop_spawn announces it. A chest replayed from a friend
        // must not also spawn its own: both would, and every console would end with two stars (measured, chest row b1).
        *(unsigned short *)(a + 0x170) = 0;
        return true;
    }
    if (st == 0) return false;
    *(int *)(a + 0x16c) = 0;                          // SetState(0): the wrong-chest reset
    _ZN11daObjTbox_c10InitState0Ev(a);
    return true;
}
// STUMP (27, the ground-pound post of BOB's Chain Chomp): the pound counter +0x31e counts 3 -> 0. One pound is
// func_ov091_021334b8(post, 0): the post sinks 0x3c000 and the counter drops; the pound that empties it spawns the
// coins, marks the post in the death table and releases the chomp (func_ov091_02133498 sets the chomp's flag).
int r_stump(const char *a) { return rd_u8(a, 0x31e); }
bool ap_stump(char *a, int, int value) {
    if (rd_u8(a, 0x31e) <= value) return false;
    for (int guard = 0; guard < 8 && rd_u8(a, 0x31e) > value && rd_u8(a, 0x31e) != 0; ++guard)
        _ZN11daObjPile_c19func_ov091_021334b8Ei(a, 0);
    return true;
}
// COOPPUZZLE1 (G21): the BBH library BOOKSHELF (158, daTrsTrap_c mIndex 2) and its three BOOK_SWITCH books (213, daBook_c,
// spawned by the shelf, mLinkedActorID = the shelf's uniqueID). A pushed book ORs 1 << param1 into the shelf's mBookFlags +0x157
// (func_ov063_0211cae8); the shelf's state 0 keeps 1 and 3, clears any other order (the books whose bit went retract) and at 7
// goes to mState +0x150 = 1, then 2 (slides away; the DONE books slide off with it) and 3 (removed) (src/game/actors/
// d_a_trs_trap.cpp, d_a_book.cpp). All of it ran on the pusher's console only. Shared as a field: value = (mState != 0) << 3 |
// (mBookFlags & 7); bit 3 of mBookFlags (the puzzle armed by a nearby player) stays local. While the shelf's room is not showing,
// its Behavior zeroes mBookFlags every frame: that is not progress, so the reader keeps the last value it saw in view.
char *find_actor(int slot, int cls);
struct ShelfSeen { const void *a; unsigned uid; int v; };
ShelfSeen g_shelf_seen[4];
int r_shelf(const char *a) {
    if (rd_i32(a, 0x140) != 2) return 0;
    ShelfSeen *e = nullptr;
    const unsigned uid = (unsigned)rd_i32(a, 4);
    for (ShelfSeen &q : g_shelf_seen) if (q.a == a && q.uid == uid) { e = &q; break; }
    if (!e) { e = &g_shelf_seen[0]; for (ShelfSeen &q : g_shelf_seen) if (!q.a) { e = &q; break; } e->a = a; e->uid = uid; e->v = 0; }
    const bool showing = *(const signed char *)(a + 0xcc) == -1;    // the shelf's Behavior parks mAreaId at -1 while its room shows
    const int v = (rd_u8(a, 0x150) != 0 ? 8 : 0) | (rd_u8(a, 0x157) & 7);
    if (!showing && rd_u8(a, 0x150) == 0) return e ? e->v : v;
    if (e) e->v = v;
    return v;
}
bool ap_shelf(char *a, int, int value) {
    if (rd_i32(a, 0x140) != 2) return false;
    const bool solved = (value & 8) != 0;
    const int bits = solved ? 7 : (value & 7);
    const bool showing = *(const signed char *)(a + 0xcc) == -1;
    if (solved) {
        if (rd_u8(a, 0x150) != 0) return false;                        // already opening here
        *(unsigned char *)(a + 0x150) = 1;                             // the verdict: state 1 counts, chimes, then slides (2)
        *(unsigned short *)(a + 0x14c) = 0;
    } else if (!showing || (rd_u8(a, 0x157) & 7) == bits) {
        return false;                                                  // out of view the shelf forgets books anyway
    }
    *(unsigned char *)(a + 0x157) = (unsigned char)((rd_u8(a, 0x157) & 8) | bits);
    const unsigned uid = (unsigned)rd_i32(a, 4);
    for (char *b = (char *)_ZN8dActor_c4NextEPKS_(0); b; b = (char *)_ZN8dActor_c4NextEPKS_(b)) {
        if (a_cls(b) != 213 || a_dead(b) || (unsigned)rd_i32(b, 0x418) != uid) continue;
        const int idx = rd_i32(b, 8) & 7;
        const int st = rd_i32(b, 0x424);
        if (!((bits >> idx) & 1) || st < 6 || st > 9) continue;       // a cleared bit: a DONE book retracts on its own
        if (solved && !showing) { _ZN7fBase_c18MarkForDestructionEv(b); continue; }   // the shelf leaves before this room shows
        *(int *)(b + 0x64) = rd_i32(b, 0x434);                         // what STATE_SWITCH_PUSH ends with: home, collider off,
        *(unsigned *)(b + 0x234) |= 1u;                                // no toucher, STATE_SWITCH_DONE (follows the verdict)
        *(void **)(b + 0x41c) = nullptr;
        *(int *)(b + 0x424) = 10;
    }
    return true;
}
// COOPPUZZLE1 (G12): the SSL pyramid TOP (85, daObjDlPyramid_c) and its four PYRAMID_TAGs (86, daObjDlPyramidDummy_c,
// mPyramidTopID +0x108). A tag a body touches adds one to the top's mNumTagsTriggered +0x3b6 and removes itself
// (MarkForDestruction, not the death table, so coop_world never shares it); at 4 the top's state 0 moves on, it descends,
// removes itself and raises Event bit 0xe (src/game/actors/d_a_obj_dl_pyramid.cpp). A friend's touches counted on his console
// only, so my top never came down. Shared as a field on the top: value = the mask of its tags that are gone here (the tags go
// by their placed slots, sorted: the same on every console). A receiver removes those tags and lifts the count to the number
// of tags gone on either side; the top's own state 0 then does the rest.
struct TopTags { const void *top; unsigned uid; short slot[4]; int n; unsigned remote; };
TopTags g_tops[2];
TopTags *top_entry(const char *a) {
    for (TopTags &t : g_tops) if (t.top == a && t.uid == (unsigned)rd_i32(a, 4)) return &t;
    TopTags *f = &g_tops[1];                         // a new level's top: a free entry, else the second one is reused
    for (TopTags &t : g_tops) if (!t.top) { f = &t; break; }
    *f = TopTags{a, (unsigned)rd_i32(a, 4), {0, 0, 0, 0}, 0, 0};
    return f;
}
unsigned top_gone(TopTags &t) {
    if (t.n < 4) {                                   // learn the four tags once, while all of them stand
        short s[4]; int n = 0;
        for (char *b = (char *)_ZN8dActor_c4NextEPKS_(0); b && n < 4; b = (char *)_ZN8dActor_c4NextEPKS_(b))
            if (a_cls(b) == 86 && !a_dead(b) && (unsigned)rd_i32(b, 0x108) == t.uid && a_slot(b) >= 0) s[n++] = (short)a_slot(b);
        if (n < 4) return 0;
        for (int i = 0; i < 4; ++i) for (int j = i + 1; j < 4; ++j) if (s[j] < s[i]) { short x = s[i]; s[i] = s[j]; s[j] = x; }
        for (int i = 0; i < 4; ++i) t.slot[i] = s[i];
        t.n = 4;
        if (g_log) std::fprintf(stderr, "[coopsw] pyramid top uid=%u tags slots %d %d %d %d wall=%lld\n", t.uid, s[0], s[1], s[2], s[3], wall_ms());
    }
    unsigned m = 0;
    for (int i = 0; i < 4; ++i) if (!find_actor(t.slot[i], 86)) m |= 1u << i;
    return m;
}
void top_count(char *a, TopTags &t, unsigned gone) {
    const unsigned all = gone | t.remote;
    int c = 0; for (int i = 0; i < 4; ++i) c += (all >> i) & 1;
    if (rd_u8(a, 0x3b7) == 0 && rd_u8(a, 0x3b6) < c) *(unsigned char *)(a + 0x3b6) = (unsigned char)c;
}
int r_top(const char *a) {
    TopTags *t = top_entry(a);
    if (!t) return 0;
    const unsigned gone = top_gone(*t);
    if (t->n == 4) top_count(const_cast<char *>(a), *t, gone);
    return (int)gone;
}
bool ap_top(char *a, int, int value) {
    TopTags *t = top_entry(a);
    if (!t) return false;
    top_gone(*t);
    if (t->n < 4) return false;
    const unsigned add = (unsigned)value & 15u & ~t->remote;
    t->remote |= (unsigned)value & 15u;
    for (int i = 0; i < 4; ++i)
        if ((add >> i) & 1u) if (char *g = find_actor(t->slot[i], 86)) _ZN7fBase_c18MarkForDestructionEv(g);
    top_count(a, *t, top_gone(*t) | t->remote);
    return add != 0;
}
// DOOR (353) and STAR_DOOR (354): the START of an unlock. The ROM writes the save bit when the cutscene ENDS; the
// kick fires when the unlock state begins and puts the same bit in the other consoles' record at once. Nothing here
// hands a Player to a door state.
unsigned *save_flags() { return (unsigned *)(data_0209caa0 + 4); }
// Doors have no placed-object slot (the plain Door and the star gate are created by the area, slot -1): they are named on
// the wire by their table index (param1), which is the same on every console and is all that decides the save bit.
bool is_door_cls(int c) { return c == 353 || c == 354; }
bool door_idx_ok(int cls, int idx) { return idx >= 0 && idx < (cls == 354 ? 4 : 64); }
bool door_bit_idx(int cls, int idx, int state, unsigned *bit) {
    if (!door_idx_ok(cls, idx)) return false;
    if (cls == 354) {
        const int sh = data_ov100_02148390[idx * 6 + 1];
        if (state != 1 || sh < 0 || sh > 8) return false;
        *bit = 0x8000u << sh;
        return true;
    }
    const signed char *e = data_ov100_02148204 + idx * 16;
    const int sh = e[0xa];
    if (state == 1) { if (e[8] < 0 || sh < 0 || sh > 8) return false; *bit = 0x8000u << sh; return true; }
    if (state == 2) { if (e[8] >= 0 || e[9] < 0 || sh < 0 || sh > 15) return false; *bit = 0x100u << sh; return true; }
    return false;
}
int r_door(const char *a) {
    const int idx = rd_i32(a, 8);
    if (!door_idx_ok(353, idx)) return 0;
    const signed char *e = data_ov100_02148204 + idx * 16;
    const void *node = *(void *const *)(a + 0x140);
    if (e[8] >= 0) return node == (const void *)data_ov100_021488d4 ? 1 : 0;
    if (e[9] >= 0) return node == (const void *)data_ov100_021488e4 ? 2 : 0;
    return 0;
}
int r_gate(const char *a) { return *(void *const *)(a + 0x110) == (const void *)_ZN12daStarGate_c12ST_UNLOCKINGE ? 1 : 0; }
// the table index of a door-like actor as sent (a gate's param1 is already shifted down by its InitResources)
int door_wire_idx(const char *a) { return rd_i32(a, 8); }
// true when the bit was newly put in the record
bool apply_door_idx(int cls, int idx, int state) {
    unsigned bit = 0;
    if (!door_bit_idx(cls, idx, state, &bit)) return false;
    if (*save_flags() & bit) return false;
    *save_flags() |= bit;
    return true;
}
// COOPRIDE1 (G40): SPIKE_BOMB (284, daKirai_c), mStateIndex +0x170: 0 armed, 1 / 2 going off, 3 spent. A bomb went off only on the
// console whose Bowser (or player) touched it, and once every sibling was spent each console re-armed the bomb nearest ITS own
// player. Shared as a field: 0 armed, 1 not armed. A friend's 1 sets an armed bomb off with Bowser's own step; a friend's 0
// re-arms a spent one (one still going off is disarmed first). The re-arm choice belongs to the lowest seat in the room: a
// higher seat's own re-arm is undone in scan_objects and never sent, and the lowest seat's choice arrives as a field.
int r_kirai(const char *a) { return rd_i32(a, 0x170) == 0 ? 0 : 1; }
bool ap_kirai(char *a, int, int value) {
    const int st = rd_i32(a, 0x170);
    if (value != 0) { if (st != 0) return false; coopsw_kirai_explode(a); return true; }
    if (st == 0) return false;
    if (st != 3) coopsw_kirai_disarm(a);
    coopsw_kirai_rearm(a);
    return true;
}
bool ap_door(char *, int, int) { return false; }      // never reached: apply_entry takes door classes by index
bool ap_gate(char *, int, int) { return false; }
const Watch kWatch[] = {
    {10,  kSwKindPress, "BC_SWITCH",    r_bcsw,   ap_bcsw},
    {11,  kSwKindPress, "EXCL_SWITCH",  r_starsw, ap_starsw},
    {12,  kSwKindPress, "STAR_SWITCH",  r_starsw, ap_starsw},
    {126, kSwKindKick,  "DONUT_BLOCK",  r_donut,  ap_donut},
    {14,  kSwKindKick,  "CANNON_HATCH",   r_hatch, ap_hatch},
    {26,  kSwKindKick,  "HATENA_SWITCH", r_hatena, ap_hatena},
    {45,  kSwKindKick,  "FALL_BLOCK_WF",  r_fall, ap_fall},
    {66,  kSwKindKick,  "FALL_BLOCK_BBH", r_fall, ap_fall},
    {83,  kSwKindKick,  "FALL_BLOCK_LLL", r_fall, ap_fall},
    {139, kSwKindKick,  "FALL_BLOCK_BFS", r_fall, ap_fall},   // daObjKm2_Fall_Block_c: a daObjFallBlock_c with its own files only (src/game/actors/d_a_obj_km2_fall_block.cpp)
    {101, kSwKindField, "WDW_WATER",    r_water,  ap_water},
    {13,  kSwKindField, "TREASURE_CHEST", r_chest, ap_chest},
    {27,  kSwKindField, "STUMP",        r_stump,  ap_stump},
    {158, kSwKindField, "BOOKSHELF",    r_shelf,  ap_shelf},
    {85,  kSwKindField, "PYRAMID_TOP",  r_top,    ap_top},
    {284, kSwKindField, "SPIKE_BOMB",   r_kirai,  ap_kirai},
    {353, kSwKindKick,  "DOOR",         r_door,   ap_door},
    {354, kSwKindKick,  "STAR_DOOR",    r_gate,   ap_gate},
};
const Watch *find_watch(int cls) {
    for (const Watch &w : kWatch) if (w.cls == cls) return &w;
    return nullptr;
}

// The slot a watched actor goes by on the wire. A placed object: its own slot. The Chain Chomp's stake (a class 27 post the chomp
// spawns itself in its InitResources, so slot -1, the same two spawns on every console) goes by a position code in 504..511.
int wire_slot(const char *a, int cls) {
    const int s = a_slot(a);
    if (s >= 0 || (cls != 27 && cls != 158)) return s;
    const int *q = (const int *)(a + 0x5c);
    if (cls == 158) return 496 + ((((q[0] >> 12) * 7) + ((q[2] >> 12) * 13)) & 7);   // COOPPUZZLE1: if the area spawns the shelf
    return 504 + ((((q[0] >> 12) * 7) + ((q[2] >> 12) * 13)) & 7);
}

char *find_actor(int slot, int cls) {
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if (a_cls(a) == cls && wire_slot(a, cls) == slot && !a_dead(a) && !is_player_ptr(a)) return a;
    return nullptr;
}

// ---------------------------------------------------------------------------
// Sending.
// ---------------------------------------------------------------------------
CoopKeyWire wkey() { CoopKeyWire k; k.level = (uint16_t)g_lvl; k.act = (uint8_t)g_act; k.reserved = 0; return k; }
bool others_here() { return (coop_group_member_mask() & ~(1u << g_my)) != 0; }
int lowest_here() { const unsigned m = coop_group_member_mask() | (1u << g_my); for (int i = 0; i < 32; ++i) if ((m >> i) & 1u) return i; return g_my; }
int g_kirai_undo = 0;

bool send_world(int sub, const void *p, int n) { return coop_send_world(sub, p, n); }
bool send_ctl(int sub, const void *p, int n) { return coop_send_ctl(sub, p, n); }

void send_bits(unsigned now) {
    SwBitEntry ent[kSwMaxBitEnt]; int idx[kSwMaxBitEnt]; int n = 0;
    for (int b = 0; b < kSwBits && n < kSwMaxBitEnt; ++b) {
        PendBit &p = g_pb[b];
        if (!p.on || (int)(now - p.next) < 0) continue;
        ent[n] = {(uint8_t)b, p.val, (uint16_t)p.stamp}; idx[n++] = b;
    }
    if (!n) return;
    unsigned char buf[sizeof(SwBitHead) + kSwMaxBitEnt * sizeof(SwBitEntry)];
    SwBitHead h; std::memset(&h, 0, sizeof h); h.key = wkey(); h.count = (uint8_t)n;
    std::memcpy(buf, &h, sizeof h); std::memcpy(buf + sizeof h, ent, (size_t)n * sizeof(SwBitEntry));
    const bool ok = others_here() && send_world(kSwWorldBit, buf, (int)sizeof h + n * (int)sizeof(SwBitEntry));
    if (ok) ++g_st.sent_bit;
    for (int i = 0; i < n; ++i) {
        PendBit &p = g_pb[idx[i]];
        if (!ok && others_here()) continue;
        if (++p.sent >= 3 || !others_here()) p.on = false; else p.next = now + kResendMs;
    }
}

void send_word(unsigned now) {
    SwWordWire w; std::memset(&w, 0, sizeof w);
    w.key = wkey(); w.word = (unsigned)data_0209f34c;
    bool any = false;
    for (int i = 0; i < kSwBits; ++i) { w.stamp[i] = (uint16_t)g_bit[i].stamp; any = any || g_bit[i].stamp; }
    g_word_t = now;
    if (!any || !others_here()) return;
    if (send_world(kSwWorldWord, &w, sizeof w)) ++g_st.sent_word;
}

void queue_obj(int kind, int slot, int cls, int state, int value, unsigned now) {
    ++g_seq; if (!g_seq) g_seq = 1;
    int free_i = -1;
    for (int i = 0; i < kObjOutMax; ++i) {      // a newer message for the same object replaces the old one
        if (g_oo[i].on && g_oo[i].kind == kind && g_oo[i].slot == slot && g_oo[i].cls == cls) { free_i = i; break; }
        if (!g_oo[i].on && free_i < 0) free_i = i;
    }
    if (free_i < 0) free_i = 0;
    g_oo[free_i] = {true, (uint8_t)kind, (uint16_t)slot, (uint16_t)cls, (uint8_t)state, (int32_t)value, g_seq, 0, now, now};
}

void send_objs(unsigned now) {
    for (int i = 0; i < kObjOutMax; ++i) {
        ObjOut &o = g_oo[i];
        if (!o.on || (int)(now - o.next) < 0) continue;
        if (!others_here()) {
            // COOPPUZZLE2: a field value is a state, not an event: one made while nobody is listed yet (the first moments of a
            // level) waits up to kHoldMs for the friend instead of being lost. Press and kick still go.
            if (g_hold_field && o.kind == kSwKindField && (unsigned)(now - o.born) < (unsigned)kHoldMs) { o.next = now + kResendMs; continue; }
            o.on = false; continue;
        }
        SwObjWire w; std::memset(&w, 0, sizeof w);
        w.key = wkey(); w.slot = o.slot; w.cls = o.cls; w.state = o.state; w.seq = o.seq; w.value = o.value;
        const int sub = o.kind == kSwKindPress ? kSwWorldPress : o.kind == kSwKindKick ? kSwWorldKick : kSwWorldField;
        if (send_world(sub, &w, sizeof w)) {
            if (o.kind == kSwKindPress) ++g_st.sent_press; else if (o.kind == kSwKindKick) ++g_st.sent_kick; else ++g_st.sent_field;
            if (g_log && o.sent == 0)
                std::fprintf(stderr, "[coopsw] send %s slot=%d cls=%d state=%d value=%d seq=%u wall=%lld\n",
                             o.kind == kSwKindPress ? "press" : o.kind == kSwKindKick ? "kick" : "field",
                             (int)o.slot, (int)o.cls, (int)o.state, (int)o.value, (unsigned)o.seq, wall_ms());
        }
        if (++o.sent >= 3) o.on = false; else o.next = now + kResendMs;
    }
}

// What the owner tells a joiner: the globals and every non-idle watched object.
int collect_states(SwStateEntry *out, int cap) {
    int n = 0;
    for (int s = 0; s < kSwSlots && n < cap; ++s) {
        const Trk &t = g_trk[s];
        if (!t.valid) continue;
        const Watch *w = find_watch(t.cls);
        if (!w || !t.actor) continue;
        if (w->kind == kSwKindField) { out[n++] = {(uint16_t)s, t.cls, (uint8_t)kSwKindField, 0, t.last}; continue; }
        if (t.last == 0) continue;
        if (is_door_cls(t.cls)) { out[n++] = {(uint16_t)door_wire_idx((const char *)t.actor), t.cls, (uint8_t)w->kind, (uint8_t)(t.last > 3 ? 3 : t.last), 0}; continue; }
        out[n++] = {(uint16_t)s, t.cls, (uint8_t)w->kind, (uint8_t)(w->kind == kSwKindPress ? 1 : (t.last > 3 ? 3 : t.last)), 0};
    }
    return n;
}

void serve_job(Job &j) {
    SwGlobalsWire g; std::memset(&g, 0, sizeof g);
    g.key = wkey(); g.nonce = j.nonce; g.target = (uint8_t)j.seat; g.word = (unsigned)data_0209f34c;
    std::memcpy(g.globals, data_0209f2c0, 4);
    for (int i = 0; i < 4; ++i) if (g.globals[i] > 7) g.globals[i] = 0;
    for (int i = 0; i < kSwBits; ++i) g.stamp[i] = (uint16_t)g_bit[i].stamp;
    if (send_ctl(kSwCtlGlobals, &g, sizeof g)) ++g_st.sent_globals;
    unsigned char buf[sizeof(SwStatesHead) + kSwMaxStateEnt * sizeof(SwStateEntry)];
    SwStateEntry ent[kSwMaxStateEnt];
    const int n = collect_states(ent, kSwMaxStateEnt);
    SwStatesHead h; std::memset(&h, 0, sizeof h);
    h.key = wkey(); h.nonce = j.nonce; h.target = (uint8_t)j.seat; h.count = (uint8_t)n;
    std::memcpy(buf, &h, sizeof h); std::memcpy(buf + sizeof h, ent, (size_t)n * sizeof(SwStateEntry));
    if (send_ctl(kSwCtlStates, buf, (int)sizeof h + n * (int)sizeof(SwStateEntry))) ++g_st.sent_states;
    if (g_log && j.sent == 0)
        std::fprintf(stderr, "[coopsw] serve globals to seat %d: word=%08x f2c0=%d,%d,%d,%d states=%d wall=%lld\n",
                     j.seat, (unsigned)data_0209f34c, data_0209f2c0[0], data_0209f2c0[1], data_0209f2c0[2], data_0209f2c0[3], n, wall_ms());
}

// ---------------------------------------------------------------------------
// The word.
// ---------------------------------------------------------------------------
void log_word(const char *why) {
    if (g_log)
        std::fprintf(stderr, "[coopsw] word %s=%08x frame=%u armed=%d wall=%lld\n", why, (unsigned)data_0209f34c, g_frames, (int)g_armed, wall_ms());
}

void set_word_bit(int b, int v) {
    const unsigned m = 1u << b;
    data_0209f34c = (int)((((unsigned)data_0209f34c) & ~m) | (v ? m : 0u));
}

unsigned next_stamp() {
    if (g_counter < 0xFFF) ++g_counter;
    else if (g_log) std::fprintf(stderr, "[coopsw] stamp counter exhausted (4095 changes in this course): later changes may not win\n");
    return sw_stamp_make(g_counter, g_my);
}

// A remote bit change. Highest stamp wins; the stamp's low nibble is the seat, so no ties.
void adopt_bit(int b, int v, unsigned stamp) {
    if (!sw_adopt(g_bit[b], stamp, v)) { if (stamp != g_bit[b].stamp) ++g_st.drop_old; return; }
    if (sw_stamp_counter(stamp) > g_counter) g_counter = sw_stamp_counter(stamp);
    const int cur = (data_0209f34c >> b) & 1;
    if (cur != v) { set_word_bit(b, v); ++g_st.bits_applied; }
    g_shadow = (g_shadow & ~(1u << b)) | (v ? (1u << b) : 0u);
}

void local_word_changes(unsigned now) {
    const unsigned cur = (unsigned)data_0209f34c;
    unsigned d = cur ^ g_shadow;
    if (!d) return;
    for (int b = 0; b < kSwBits; ++b) {
        if (!((d >> b) & 1u)) continue;
        const int v = (cur >> b) & 1;
        const unsigned st = next_stamp();
        g_bit[b].stamp = st; g_bit[b].val = (uint8_t)v;
        g_pb[b] = {true, (uint8_t)v, st, 0, now};
        ++g_st.bits_local;
        if (g_log) std::fprintf(stderr, "[coopsw] local bit %d -> %d stamp=%04x wall=%lld\n", b, v, st, wall_ms());
    }
    g_shadow = cur;
    log_word("local");
}

// During the settle window the word is forced to what the group said, because
// the level's own InitResources (StarSwitch, Key, PowerStar) clear bits as they
// spawn and would otherwise undo the snapshot.
void settle_word() {
    bool changed = false;
    for (int b = 0; b < kSwBits; ++b)
        if (g_bit[b].stamp) {
            const int cur = (data_0209f34c >> b) & 1;
            if (cur != g_bit[b].val) { set_word_bit(b, g_bit[b].val); changed = true; }
        }
    g_shadow = (unsigned)data_0209f34c;
    if (changed) log_word("settle");
}

// ---------------------------------------------------------------------------
// Object states (press / kick / field).
// ---------------------------------------------------------------------------
// COOPPUZZLE3: the fields whose changes add up. The pyramid top ORs the gone masks and the stump only lowers its pound
// counter, so a receiver can take a remote value on top of its own in either order. The others hold one value each (the
// WDW water target, the bookshelf's book bits and state, a chest's open state): the last write wins, so two writes that
// cross must be settled by a fixed rule or each console ends with the other's choice.
bool field_adds(int cls) { return cls == 85 || cls == 27; }

void apply_to(Trk &t, char *a, const Watch *w, int state, int value, int seat) {
    // COOPPUZZLE2: remote messages are applied after the actors ran and before this frame's scan, so a field this console
    // changed in the same frame is already in the read below; taking it as t.last would swallow it and it was never sent
    // (a pyramid tag touched here in the frame the friend's touch arrived: his top stuck at 3). For a field that adds up,
    // send it first and then take the remote one: both consoles end with both changes.
    // COOPPUZZLE3: for a single-value field the lower seat wins. My unsent change, or one I sent less than kTieMs ago,
    // beats a higher seat's value: I keep it and send it (again), and the friend takes it because it comes from a lower
    // seat. A lower seat's value is taken and my unsent change is dropped. One choice is lost; both screens agree.
    // S-PZ3: a change in the first half second of a level is taken as the start value; no known puzzle is close enough to a start spot to hit it.
    if (g_keep_local && w->kind == kSwKindField && t.valid && t.actor == a) {
        const int pre = w->read(a);
        const int slot = (int)(&t - g_trk);
        if (field_adds(t.cls)) {
            if (pre != t.last) {
                t.last = pre;
                queue_obj(kSwKindField, slot, (int)t.cls, 0, pre, g_now);
                if (g_log) std::fprintf(stderr, "[coopsw] keep local %s slot=%d value=%d before a remote %d wall=%lld\n", w->name, slot, pre, value, wall_ms());
            }
        } else if (seat >= 0 && seat != g_my && pre != value) {
            const bool unsent = pre != t.last;
            const bool young = !unsent && t.mine_on && (unsigned)(g_now - t.mine_at) < (unsigned)kTieMs;
            if ((unsent || young) && g_my < seat) {
                t.last = pre;
                queue_obj(kSwKindField, slot, (int)t.cls, 0, pre, g_now);
                if (unsent) { t.mine_on = true; t.mine_at = g_now; }
                if (g_log) std::fprintf(stderr, "[coopsw] tie %s slot=%d mine=%d (%s) beats seat %d value=%d wall=%lld\n", w->name, slot, pre,
                                        unsent ? "unsent" : "young", seat, value, wall_ms());
                return;
            }
            if (unsent && g_log) std::fprintf(stderr, "[coopsw] tie %s slot=%d mine=%d loses to seat %d value=%d wall=%lld\n", w->name, slot, pre, seat, value, wall_ms());
        }
    }
    if (w->apply(a, state, value)) {
        t.last = w->read(a);
        t.mine_on = false;
        if (w->kind == kSwKindPress) ++g_st.press_applied; else if (w->kind == kSwKindKick) ++g_st.kick_applied; else ++g_st.field_applied;
        if (g_log)
            std::fprintf(stderr, "[coopsw] apply %s slot=%d cls=%d state=%d value=%d wall=%lld\n", w->name,
                         (int)a_slot(a), (int)a_cls(a), state, value, wall_ms());
    }
}

// Returns true when the actor was found (the entry is done, whatever its state).
// seat: the sender of a live message, or -1 for a snapshot or a pending entry (applied as is).
bool apply_entry(int kind, int slot, int cls, int state, int value, int seat) {
    const Watch *w = find_watch(cls);
    if (!w || w->kind != kind) { ++g_st.drop_class; return true; }
    if (is_door_cls(cls)) {           // by table index: the door needs no actor, and no Player is ever involved
        if (apply_door_idx(cls, slot, state)) {
            ++g_st.kick_applied;
            if (g_log) std::fprintf(stderr, "[coopsw] apply %s idx=%d state=%d flags=%08x wall=%lld\n", w->name, slot, state, *save_flags(), wall_ms());
        }
        return true;
    }
    char *a = find_actor(slot, cls);
    if (!a) return false;
    Trk &t = g_trk[slot];
    if (!t.valid || t.actor != a) { t.actor = a; t.cls = (unsigned short)cls; t.last = w->read(a); t.valid = true; t.mine_on = false; }
    apply_to(t, a, w, state, value, seat);
    return true;
}

// PUSH_BLOCK (192) placed by the friend's push. Its pose row (coop_actor_profiles.inc) writes the place, but the block's own
// Behavior rebuilds its model matrix and collider only while it is being pushed (mHorzSpeed != 0, src/game/actors/
// d_a_obj_pushblock.cpp), so a copy moved by the row kept drawing and colliding at the old spot. When the model's translation
// (dBgActor_c mModel.mat4x3 m[9..11] at +0x114, the place >> 3 as UpdateModelPosAndRotY stores it) no longer matches the place,
// run the same two calls the block's Behavior runs after a push. A block the local player pushes is already refreshed by its
// Behavior and is left alone.
unsigned g_pb_refresh = 0;
void pushblock_follow() {
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (a_cls(a) != 192 || a_dead(a)) continue;
        const int *pos = (const int *)(a + 0x5c);
        const int *mt = (const int *)(a + 0x114);
        if (mt[0] == (pos[0] >> 3) && mt[1] == (pos[1] >> 3) && mt[2] == (pos[2] >> 3)) continue;
        _ZN10dBgActor_c21UpdateModelPosAndRotYEv(a);
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(a);
        if (g_log && g_pb_refresh < 20)
            std::fprintf(stderr, "[coopsw] push block slot=%d follows its synced place %d,%d,%d (model was at %d,%d,%d) wall=%lld\n", (int)a_slot(a),
                         pos[0] >> 12, pos[1] >> 12, pos[2] >> 12, mt[0] >> 9, mt[1] >> 9, mt[2] >> 9, wall_ms());
        ++g_pb_refresh;
    }
}

void scan_objects(unsigned now) {
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        const unsigned short cls = a_cls(a);
        const Watch *w = find_watch(cls);
        if (!w || a_dead(a) || is_player_ptr(a)) continue;
        short slot = (short)wire_slot(a, cls);
        if (is_door_cls(cls)) {          // slot -1: track by position (a tracker slot above the placed range), send by table index
            const int *q = (const int *)(a + 0x5c);
            slot = (short)(256 + ((((q[0] >> 12) * 7) + ((q[1] >> 12) * 3) + ((q[2] >> 12) * 13) + rd_i32(a, 8) * 5) & 255));
            if (!door_idx_ok(cls, rd_i32(a, 8))) continue;
        } else if (slot < 0 || slot >= kSwSlots) continue;
        const int cur = w->read(a);
        Trk &t = g_trk[slot];
        if (!t.valid || t.actor != a || t.cls != cls) {
            t.actor = a; t.cls = cls; t.last = cur; t.valid = true; t.mine_on = false;
            if (g_log && (cls == 13 || cls == 27 || cls == 158 || cls == 353 || cls == 354)) {
                const int *p = (const int *)(a + 0x5c);
                std::fprintf(stderr, "[coopsw] track %s slot=%d aslot=%d uid=%u cls=%d param=%d pos=%d,%d,%d read=%d node=%p unlocking=%p wall=%lld\n", w->name,
                             (int)slot, (int)a_slot(a), (unsigned)rd_i32(a, 4), (int)cls, rd_i32(a, 8), p[0] >> 12, p[1] >> 12, p[2] >> 12, cur,
                             cls == 354 ? *(void *const *)(a + 0x110) : cls == 353 ? *(void *const *)(a + 0x140) : nullptr,
                             (void *)_ZN12daStarGate_c12ST_UNLOCKINGE, wall_ms());
                if (cls == 353 && door_idx_ok(353, rd_i32(a, 8))) {
                    const signed char *e = data_ov100_02148204 + rd_i32(a, 8) * 16;
                    std::fprintf(stderr, "[coopsw] track DOOR idx=%d stars=%d key=%d bit=%d\n", rd_i32(a, 8), (int)e[8], (int)e[9], (int)e[0xa]);
                }
            }
            continue;
        }
        if (cur == t.last) continue;
        if (cls == 284 && cur == 0 && t.last != 0 && lowest_here() < g_my) {   // G40: a re-arm is the lowest seat's choice
            coopsw_kirai_disarm(a);
            if (g_log && g_kirai_undo++ < 20)
                std::fprintf(stderr, "[coopsw] spike bomb slot=%d re-armed here: undone, seat %d picks wall=%lld\n", (int)slot, lowest_here(), wall_ms());
            continue;
        }
        const int prev = t.last;
        t.last = cur;
        if (w->kind == kSwKindField) { queue_obj(kSwKindField, slot, cls, 0, cur, now); t.mine_on = true; t.mine_at = now; continue; }
        if (prev == 0 && cur != 0)
            queue_obj(w->kind, is_door_cls(cls) ? door_wire_idx(a) : slot, cls, w->kind == kSwKindPress ? 1 : (cur > 3 ? 3 : cur), 0, now);
    }
}

void drain_pending(unsigned) {
    for (Pend &p : g_pend) {
        if (!p.on) continue;
        if (g_frames - p.born > kStatesAge) { p.on = false; continue; }
        if (apply_entry(p.kind, p.slot, p.cls, p.state, p.value, -1)) { p.on = false; ++g_st.states_applied; }
    }
}

void put_pending(const SwStateEntry &e) {
    int free_i = -1;
    for (int i = 0; i < kPendMax; ++i) {
        if (g_pend[i].on && g_pend[i].kind == e.kind && g_pend[i].slot == e.slot && g_pend[i].cls == e.cls) { free_i = i; break; }
        if (!g_pend[i].on && free_i < 0) free_i = i;
    }
    if (free_i < 0) return;
    g_pend[free_i] = {true, e.kind, e.slot, e.cls, e.state, e.value, g_frames};
    ++g_st.states_pending;
}

// ---------------------------------------------------------------------------
// Receiving.
// ---------------------------------------------------------------------------
bool live_key_is(const CoopKeyWire &k) { int l = -1, a = 0; return coop_group_key(&l, &a) && (int)k.level == l && (int)k.act == a; }
bool ready_for(const CoopKeyWire &k) { return g_have_key && (int)k.level == g_lvl && (int)k.act == g_act; }
bool is_member(int seat) { return seat >= 0 && seat < kCoopMaxSeats && ((coop_group_member_mask() >> seat) & 1u); }

void apply_globals(const SwGlobalsWire &w) {
    if (g_off.globals) { ++g_st.drop_off; return; }
    std::memcpy(data_0209f2c0, w.globals, 4);
    for (int i = 0; i < kSwBits; ++i) {
        const unsigned s = w.stamp[i];
        if (!s) continue;
        adopt_bit(i, (w.word >> i) & 1u, s);
    }
    const bool first = !g_have_globals;
    g_have_globals = true;
    ++g_st.globals_applied;
    if (first) log_word("globals");
    if (g_log && first)
        std::fprintf(stderr, "[coopsw] globals applied: f2c0=%d,%d,%d,%d wall=%lld\n", data_0209f2c0[0], data_0209f2c0[1], data_0209f2c0[2], data_0209f2c0[3], wall_ms());
}

void apply_states(const unsigned char *p, int len) {
    SwStatesHead h; std::memcpy(&h, p, sizeof h);
    for (int i = 0; i < (int)h.count; ++i) {
        SwStateEntry e; std::memcpy(&e, p + sizeof h + i * sizeof e, sizeof e);
        if (e.kind == kSwKindPress && g_off.press) continue;
        if (e.kind == kSwKindKick && g_off.kick) continue;
        if (e.kind == kSwKindField && g_off.field) continue;
        put_pending(e);
    }
    (void)len;
}

void on_ctl(int seat, int sub, const unsigned char *p, int len, void *) {
    if (!coop_world_active()) return;
    if (sub == kSwCtlGlobals) {
        ++g_st.recv_globals;
        if (!sw_globals_validate(p, len)) { ++g_st.drop_bad; return; }
        SwGlobalsWire w; std::memcpy(&w, p, sizeof w);
        if (seat == g_my || w.target != g_my) return;
        if (!live_key_is(w.key)) { ++g_st.drop_group; return; }
        if (!ready_for(w.key) || !is_member(seat)) {   // still loading, or the sender is not a known member yet: keep the newest
            Early &e = g_early[0]; e.on = true; e.seat = seat; e.kind = sub; e.lvl = w.key.level; e.act = w.key.act; e.len = len; std::memcpy(e.buf, p, (size_t)len);
            return;
        }
        apply_globals(w);
        return;
    }
    if (sub == kSwCtlStates) {
        ++g_st.recv_states;
        if (!sw_states_validate(p, len)) { ++g_st.drop_bad; return; }
        SwStatesHead h; std::memcpy(&h, p, sizeof h);
        if (seat == g_my || h.target != g_my) return;
        if (!live_key_is(h.key)) { ++g_st.drop_group; return; }
        if (!ready_for(h.key) || !is_member(seat)) {
            Early &e = g_early[1]; e.on = true; e.seat = seat; e.kind = sub; e.lvl = h.key.level; e.act = h.key.act; e.len = len; std::memcpy(e.buf, p, (size_t)len);
            return;
        }
        apply_states(p, len);
        return;
    }
    ++g_st.drop_bad;
}

void on_world(int seat, int sub, const unsigned char *p, int len, void *) {
    if (!coop_world_active()) return;
    if (seat == g_my || seat < 0 || seat >= kCoopMaxSeats) return;
    switch (sub) {
    case kSwWorldBit: {
        ++g_st.recv_bit;
        if (!sw_bit_validate(p, len)) { ++g_st.drop_bad; return; }
        SwBitHead h; std::memcpy(&h, p, sizeof h);
        if (!live_key_is(h.key)) { ++g_st.drop_group; return; }
        if (!is_member(seat)) { ++g_st.drop_member; return; }
        if (g_off.word) { ++g_st.drop_off; return; }
        const unsigned before = (unsigned)data_0209f34c;
        for (int i = 0; i < (int)h.count; ++i) {
            SwBitEntry e; std::memcpy(&e, p + sizeof h + i * sizeof e, sizeof e);
            if ((e.stamp & 15u) != (unsigned)seat) { ++g_st.drop_bad; continue; }   // a stamp names its sender
            // a live change cannot be far ahead of the counter I follow: a forged top stamp would pin the bit for good
            if (sw_stamp_counter(e.stamp) > g_counter + 512u) { ++g_st.drop_bad; continue; }
            adopt_bit(e.bit, e.value, e.stamp);
        }
        if ((unsigned)data_0209f34c != before) log_word("remote");
        return;
    }
    case kSwWorldWord: {
        ++g_st.recv_word;
        if (!sw_word_validate(p, len)) { ++g_st.drop_bad; return; }
        SwWordWire w; std::memcpy(&w, p, sizeof w);
        if (!live_key_is(w.key)) { ++g_st.drop_group; return; }
        if (!is_member(seat)) { ++g_st.drop_member; return; }
        if (g_off.word) { ++g_st.drop_off; return; }
        const unsigned before = (unsigned)data_0209f34c;
        for (int i = 0; i < kSwBits; ++i)
            if (w.stamp[i]) adopt_bit(i, (w.word >> i) & 1u, w.stamp[i]);
        if ((unsigned)data_0209f34c != before) log_word("remote-full");
        return;
    }
    case kSwWorldPress: case kSwWorldKick: case kSwWorldField: {
        const int kind = sub == kSwWorldPress ? kSwKindPress : sub == kSwWorldKick ? kSwKindKick : kSwKindField;
        if (kind == kSwKindPress) ++g_st.recv_press; else if (kind == kSwKindKick) ++g_st.recv_kick; else ++g_st.recv_field;
        if (!sw_obj_validate(kind, p, len)) { ++g_st.drop_bad; return; }
        SwObjWire w; std::memcpy(&w, p, sizeof w);
        if (!live_key_is(w.key)) { ++g_st.drop_group; return; }
        if (!is_member(seat)) { ++g_st.drop_member; return; }
        if ((kind == kSwKindPress && g_off.press) || (kind == kSwKindKick && g_off.kick) || (kind == kSwKindField && g_off.field)) { ++g_st.drop_off; return; }
        uint16_t &last = g_lastseq[seat][w.slot];
        if (!sw_seq_newer(w.seq, last)) { ++g_st.drop_old; return; }
        last = w.seq;
        if (g_log)
            std::fprintf(stderr, "[coopsw] recv %s from seat %d slot=%d cls=%d state=%d value=%d wall=%lld\n",
                         kind == kSwKindPress ? "press" : kind == kSwKindKick ? "kick" : "field", seat,
                         (int)w.slot, (int)w.cls, (int)w.state, (int)w.value, wall_ms());
        // The actor may not exist yet (still loading): keep it as a pending state.
        if (!apply_entry(kind, w.slot, w.cls, kind == kSwKindField ? 0 : w.state, w.value, seat)) {
            SwStateEntry e = {w.slot, w.cls, (uint8_t)kind, kind == kSwKindField ? (uint8_t)0 : w.state, w.value};
            put_pending(e);
        }
        return;
    }
    default:
        ++g_st.drop_bad;
    }
}

// ---------------------------------------------------------------------------
// Knobs: recorder, watch, forge, setters.
// ---------------------------------------------------------------------------
struct RecSample {
    unsigned word; unsigned char g[4]; unsigned char save[0x80];
    struct A { void *a; unsigned short cls; short slot; unsigned char b[0x330]; bool ok; };
    A act[600]; int n;
};
RecSample g_rs[3];

bool safe_copy(void *dst, const void *src, size_t n) {
    __try { std::memcpy(dst, src, n); return true; } __except (1) { return false; }
}

void rec_take(int i) {
    RecSample &r = g_rs[i];
    r.word = (unsigned)data_0209f34c;
    std::memcpy(r.g, data_0209f2c0, 4);
    if (!safe_copy(r.save, data_0209caa0, sizeof r.save)) std::memset(r.save, 0, sizeof r.save);
    r.n = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && r.n < 600; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        RecSample::A &q = r.act[r.n++];
        q.a = a; q.cls = a_cls(a); q.slot = a_slot(a);
        q.ok = safe_copy(q.b, a + 0xd0, sizeof q.b);
    }
}

void rec_report() {
    const RecSample &a = g_rs[0], &b = g_rs[1], &c = g_rs[2];
    std::fprintf(stderr, "[rec] word %08x %08x -> %08x\n", a.word, b.word, c.word);
    std::fprintf(stderr, "[rec] f2c0 %d,%d,%d,%d -> %d,%d,%d,%d\n", b.g[0], b.g[1], b.g[2], b.g[3], c.g[0], c.g[1], c.g[2], c.g[3]);
    for (int o = 0; o < (int)sizeof b.save; o += 4) {
        unsigned x, y, z; std::memcpy(&x, a.save + o, 4); std::memcpy(&y, b.save + o, 4); std::memcpy(&z, c.save + o, 4);
        if (y != z) std::fprintf(stderr, "[rec] save +0x%02x %08x -> %08x%s\n", o, y, z, x != y ? " (was moving)" : "");
    }
    int nchg = 0;
    for (int i = 0; i < b.n; ++i) {
        const RecSample::A &q = b.act[i];
        const RecSample::A *r = nullptr, *p = nullptr;
        for (int j = 0; j < c.n; ++j) if (c.act[j].a == q.a && c.act[j].cls == q.cls) { r = &c.act[j]; break; }
        for (int j = 0; j < a.n; ++j) if (a.act[j].a == q.a && a.act[j].cls == q.cls) { p = &a.act[j]; break; }
        if (!r) { std::fprintf(stderr, "[rec] actor cls=%d slot=%d GONE\n", (int)q.cls, (int)q.slot); ++nchg; continue; }
        if (!q.ok || !r->ok) continue;
        for (int o = 0; o < (int)sizeof q.b; o += 4) {
            unsigned y, z, x = 0; std::memcpy(&y, q.b + o, 4); std::memcpy(&z, r->b + o, 4);
            if (p && p->ok) std::memcpy(&x, p->b + o, 4);
            if (y != z) {
                const bool moving = p && p->ok && x != y;
                std::fprintf(stderr, "[rec] actor cls=%d slot=%d +0x%03x %08x -> %08x%s\n", (int)q.cls, (int)q.slot, o + 0xd0, y, z, moving ? " (was moving)" : "");
                ++nchg;
            }
        }
    }
    for (int j = 0; j < c.n; ++j) {
        bool had = false;
        for (int i = 0; i < b.n; ++i) if (b.act[i].a == c.act[j].a) { had = true; break; }
        if (!had) std::fprintf(stderr, "[rec] actor cls=%d slot=%d NEW\n", (int)c.act[j].cls, (int)c.act[j].slot);
    }
    std::fprintf(stderr, "[rec] done: %d changed words, %d actors before, %d after\n", nchg, b.n, c.n);
}

void knob_forge(const char *name, unsigned now) {
    std::fprintf(stderr, "[coopsw-knob] forge %s\n", name);
    unsigned char b[128]; std::memset(b, 0, sizeof b);
    if (!std::strcmp(name, "bit32")) {
        SwBitHead h; std::memset(&h, 0, sizeof h); h.key = wkey(); h.count = 1;
        SwBitEntry e = {32, 1, (uint16_t)sw_stamp_make(1, g_my)};
        std::memcpy(b, &h, sizeof h); std::memcpy(b + sizeof h, &e, sizeof e);
        send_world(kSwWorldBit, b, (int)(sizeof h + sizeof e));
    } else if (!std::strcmp(name, "bit5")) {            // a valid bit, but the stamp names another seat
        SwBitHead h; std::memset(&h, 0, sizeof h); h.key = wkey(); h.count = 1;
        SwBitEntry e = {5, 1, (uint16_t)sw_stamp_make(7, (g_my + 1) & 3)};
        std::memcpy(b, &h, sizeof h); std::memcpy(b + sizeof h, &e, sizeof e);
        send_world(kSwWorldBit, b, (int)(sizeof h + sizeof e));
    } else if (!std::strcmp(name, "count")) {           // says 3, one present
        SwBitHead h; std::memset(&h, 0, sizeof h); h.key = wkey(); h.count = 3;
        SwBitEntry e = {1, 1, (uint16_t)sw_stamp_make(1, g_my)};
        std::memcpy(b, &h, sizeof h); std::memcpy(b + sizeof h, &e, sizeof e);
        send_world(kSwWorldBit, b, (int)(sizeof h + sizeof e));
    } else if (!std::strcmp(name, "badkey")) {
        SwBitHead h; std::memset(&h, 0, sizeof h); h.key = wkey(); h.key.act = (uint8_t)((g_act + 1) & 7); h.count = 1;
        SwBitEntry e = {2, 1, (uint16_t)sw_stamp_make(1, g_my)};
        std::memcpy(b, &h, sizeof h); std::memcpy(b + sizeof h, &e, sizeof e);
        send_world(kSwWorldBit, b, (int)(sizeof h + sizeof e));
    } else if (!std::strcmp(name, "slot") || !std::strcmp(name, "class") || !std::strcmp(name, "state") || !std::strcmp(name, "zero")) {
        SwObjWire w; std::memset(&w, 0, sizeof w);
        w.key = wkey(); w.slot = 3; w.cls = 11; w.state = 1; w.seq = 1;
        if (!std::strcmp(name, "slot")) w.slot = 600;
        if (!std::strcmp(name, "class")) w.cls = 207;
        if (!std::strcmp(name, "state")) w.state = 9;
        if (!std::strcmp(name, "zero")) w.seq = 0;
        send_world(kSwWorldPress, &w, sizeof w);
    } else if (!std::strcmp(name, "short")) {
        send_world(kSwWorldPress, b, 9);
    } else if (!std::strcmp(name, "long")) {
        send_world(kSwWorldWord, b, 100);
    } else if (!std::strcmp(name, "water")) {            // a class-101 field value far outside any level
        SwObjWire w; std::memset(&w, 0, sizeof w);
        w.key = wkey(); w.slot = 4; w.cls = 101; w.seq = 1; w.value = 0x7ffffff0;
        send_world(kSwWorldField, &w, sizeof w);
    }
    (void)now;
}

// ---- PUZZLES9 proof knobs: what the player's touch does, without a player -----------------------------------------
// chest (13): State0's own rule. The chest opens only when the chests already open number mOrder - 1; the last chest
// is the one whose mOrder equals the chest count. A wrong chest resets every other chest (the ROM also shocks the
// player; no player here). Returns true when the chest opened.
bool knob_chest(char *a) {
    int count = 0, sb = 0;
    for (char *c = (char *)_ZN8dActor_c4NextEPKS_(0); c; c = (char *)_ZN8dActor_c4NextEPKS_(c)) {
        if (a_cls(c) != 13 || a_dead(c) || is_player_ptr(c)) continue;
        ++count;
        if (c != a && (rd_i32(c, 0x16c) == 1 || rd_i32(c, 0x16c) == 2)) ++sb;
    }
    const int order = rd_u8(a, 0x172);
    if (sb + 1 == order) {
        if (count == order) *(unsigned char *)(a + 0x173) = 1;
        *(int *)(a + 0x16c) = 1;
        _ZN11daObjTbox_c10InitState1Ev(a);
        return true;
    }
    for (char *c = (char *)_ZN8dActor_c4NextEPKS_(0); c; c = (char *)_ZN8dActor_c4NextEPKS_(c)) {
        if (a_cls(c) != 13 || a_dead(c) || is_player_ptr(c) || c == a || rd_i32(c, 0x16c) == 0) continue;
        *(int *)(c + 0x16c) = 0;
        _ZN11daObjTbox_c10InitState0Ev(c);
    }
    return false;
}
// door / gate unlock START with the real player: the ROM's own TryTalkToDoor / TryTalkToKeyDoor (the player takes the
// talk state and runs the unlock cutscene, the ROM writes the save bit at its end), then the node the ROM sets.
// state arg: 1 star door (353), 2 key door (353), 3 star gate (354). Returns true when the player took the state.
// The ROM does this inside the door's own tick: the door asks the Player for the talk state (the Player takes it in its own
// tick, a moment later), and the unlock node then waits for that state (func_ov100_02144bf4 / 021449c8, St_Unlocking_Main) before
// it writes the save bit. So the harness does the same in two steps: stage 0 asks, every frame, until the player is standing and
// the ask succeeds; stage 1, one frame later (the Player has ticked), puts the door into its unlock node / state.
struct UlKnob { bool on; int cls; char *door; int stage; unsigned t0; int idx, cand, of; } g_ul = {};
void *local_player() {
    const int mi = (int)data_0209f250;
    return (mi >= 0 && mi < 16) ? data_0209f394[mi] : nullptr;
}
void ul_step() {
    if (!g_ul.on) return;
    char *a = g_ul.door;
    void *me = local_player();
    if (!a || !me || a_dead(a)) { g_ul.on = false; return; }
    if (g_ul.stage == 0) {
        bool ok = false;
        if (g_ul.cls == 353) {
            const int idx = rd_i32(a, 8);
            const signed char *e = data_ov100_02148204 + idx * 16;
            if (e[8] >= 0) ok = _ZN6Player13TryTalkToDoorEh(me, 0) != 0;
            else if (e[9] >= 0) ok = _ZN6Player16TryTalkToKeyDoorEv(me) != 0;
        } else {
            ok = _ZN6Player13TryTalkToDoorEh(me, 1) != 0;
            if (ok) _ZN6Player11OpenBigDoorEv(me);
        }
        const bool giveup = !ok && g_frames - g_ul.t0 > 900;
        if (ok || giveup) {
            std::fprintf(stderr, "[coopsw-knob] press cls=%d slot=-1 (candidate %d of %d) param=%d order=0 did=%d waited=%u wall=%lld\n",
                         g_ul.cls, g_ul.cand, g_ul.of, rd_i32(a, 8), (int)ok, g_frames - g_ul.t0, wall_ms());
            if (ok) g_ul.stage = 1; else g_ul.on = false;
        }
        return;
    }
    // stage 1
    if (g_ul.cls == 353) {
        const signed char *e = data_ov100_02148204 + rd_i32(a, 8) * 16;
        _ZN8daDoor_c19func_ov100_021453d8EPvi(a, e[8] >= 0 ? data_ov100_021488d4 : data_ov100_021488e4, (int)(intptr_t)me);
    } else {
        *(void **)(a + 0x110) = (void *)_ZN12daStarGate_c12ST_UNLOCKINGE;      // ChangeState(&ST_UNLOCKING)
    }
    std::fprintf(stderr, "[coopsw-knob] unlock node set cls=%d idx=%d wall=%lld\n", g_ul.cls, rd_i32(a, 8), wall_ms());
    g_ul.on = false;
}

void knob_press(PressKnob &k) {
    if (k.cls == 13 || k.cls == 27 || k.cls == 353 || k.cls == 354) {
        char *c[64]; int n = 0;
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && n < 64; a = (char *)_ZN8dActor_c4NextEPKS_(a))
            if (a_cls(a) == k.cls && (a_slot(a) >= 0 || is_door_cls(k.cls) || (k.cls == 27 && k.nth >= 100)) && !a_dead(a) && !is_player_ptr(a)) c[n++] = a;
        std::sort(c, c + n, [](char *x, char *y) {
            if (a_slot(x) != a_slot(y)) return a_slot(x) < a_slot(y);
            const int *p = (const int *)(x + 0x5c), *q = (const int *)(y + 0x5c);
            return p[0] != q[0] ? p[0] < q[0] : p[2] < q[2];
        });
        if (n == 0) { std::fprintf(stderr, "[coopsw-knob] press cls=%d: no live actor\n", k.cls); return; }
        char *a = c[k.nth % n];
        if (k.cls == 27 && k.nth >= 100) {                // nth = 100 + k: the chomp's stake (slot -1), the k-th such post
            a = nullptr; int kk = 0;
            for (int i = 0; i < n; ++i) if (a_slot(c[i]) < 0 && kk++ == k.nth - 100) { a = c[i]; break; }
            if (!a) { std::fprintf(stderr, "[coopsw-knob] press cls=27: no runtime post\n"); return; }
        }
        if (k.cls == 353 && k.nth >= 100) {              // nth = 100 + the door's table index (param1)
            a = nullptr;
            for (int i = 0; i < n; ++i) if (rd_i32(c[i], 8) == k.nth - 100) { a = c[i]; break; }
            if (!a) { std::fprintf(stderr, "[coopsw-knob] press cls=353: no door of index %d\n", k.nth - 100); return; }
        }
        if (k.cls == 13 && k.nth >= 100) {              // nth = 100 + the chest's order (mOrder): the order the player must take them in
            a = nullptr;
            for (int i = 0; i < n; ++i) if (rd_u8(c[i], 0x172) == k.nth - 100) a = c[i];
            if (!a) { std::fprintf(stderr, "[coopsw-knob] press cls=13: no chest of order %d\n", k.nth - 100); return; }
        }
        bool did = false;
        if (k.cls == 13) did = knob_chest(a);
        else if (k.cls == 27) { did = rd_u8(a, 0x31e) != 0; if (did) _ZN11daObjPile_c19func_ov091_021334b8Ei(a, 0); }
        else {
            g_ul.on = true; g_ul.cls = k.cls; g_ul.door = a; g_ul.stage = 0; g_ul.t0 = g_frames; g_ul.cand = k.nth % n; g_ul.of = n;
            return;                                          // ul_step reports
        }
        std::fprintf(stderr, "[coopsw-knob] press cls=%d slot=%d (candidate %d of %d) param=%d order=%d did=%d wall=%lld\n", k.cls, (int)a_slot(a), k.nth % n, n,
                     rd_i32(a, 8), k.cls == 13 ? rd_u8(a, 0x172) : 0, (int)did, wall_ms());
        return;
    }
    if (k.cls == 213 || k.cls == 86 || k.cls == 158) {
        // COOPPUZZLE1 proof knobs. 213: the book of param1 == nth takes a hit (what daBook_c's READY state does when
        // func_ov020_021115ac reports an attack: collider on, STATE_SWITCH_PUSH; the book's own PUSH state then checks the
        // order and calls the shelf). 86: the nth tag (placed slots sorted) is touched (what its Behavior does when a body
        // stands in its cylinder: the top it names counts one, the tag goes). For 86, nth is the tag's placed slot. 158: the
        // shelf's room is entered (what a door does at the end of its swing: ChangeArea to the far side's area), so a
        // placed player stands in a room that shows.
        char *a = nullptr;
        for (char *b = (char *)_ZN8dActor_c4NextEPKS_(0); b; b = (char *)_ZN8dActor_c4NextEPKS_(b)) {
            if (a_cls(b) != k.cls || a_dead(b)) continue;
            if (k.cls == 213 && (rd_i32(b, 8) & 7) == k.nth) a = b;
            if (k.cls == 86 && a_slot(b) == k.nth) a = b;
            if (k.cls == 158 && rd_i32(b, 0x140) == 2) a = b;
        }
        if (!a) { std::fprintf(stderr, "[coopsw-knob] press cls=%d nth=%d: no live actor\n", k.cls, k.nth); return; }
        bool did = false;
        if (k.cls == 213) {
            did = rd_i32(a, 0x424) == 8;
            if (did) { *(unsigned *)(a + 0x234) |= 1u; *(int *)(a + 0x424) = 9; }
        } else if (k.cls == 158) {
            ChangeArea((signed char)rd_u8(a, 0x158));
            did = true;
        } else {
            const unsigned tid = (unsigned)rd_i32(a, 0x108);
            for (char *b = (char *)_ZN8dActor_c4NextEPKS_(0); b; b = (char *)_ZN8dActor_c4NextEPKS_(b))
                if (a_cls(b) == 85 && !a_dead(b) && (unsigned)rd_i32(b, 4) == tid) { ++*(unsigned char *)(b + 0x3b6); did = true; break; }
            _ZN7fBase_c18MarkForDestructionEv(a);
        }
        std::fprintf(stderr, "[coopsw-knob] press cls=%d slot=%d nth=%d state=%d did=%d wall=%lld\n", k.cls, (int)a_slot(a), k.nth,
                     k.cls == 213 ? rd_i32(a, 0x424) : 0, (int)did, wall_ms());
        return;
    }
    const Watch *w = find_watch(k.cls);
    char *c[64]; int n = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && n < 64; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if (a_cls(a) == k.cls && a_slot(a) >= 0 && !a_dead(a) && !is_player_ptr(a)) c[n++] = a;
    std::sort(c, c + n, [](char *x, char *y) { return a_slot(x) < a_slot(y); });
    if (!w || n == 0) { std::fprintf(stderr, "[coopsw-knob] press cls=%d: no live actor\n", k.cls); return; }
    char *a = c[k.nth % n];
    const bool did = w->apply(a, 1, 0);
    std::fprintf(stderr, "[coopsw-knob] press %s slot=%d (candidate %d of %d) did=%d wall=%lld\n", w->name, (int)a_slot(a), k.nth % n, n, (int)did, wall_ms());
}

unsigned g_rec_flags = 0; bool g_rec_flags_init = false;
void watch_record() {
    const unsigned f = *(const unsigned *)(data_0209caa0 + 4);
    if (!g_rec_flags_init) { g_rec_flags_init = true; g_rec_flags = f; return; }
    if (f != g_rec_flags) {
        if (g_log) std::fprintf(stderr, "[coopsw] record flags %08x -> %08x frame=%u wall=%lld\n", g_rec_flags, f, g_frames, wall_ms());
        g_rec_flags = f;
    }
}

void watch_field(unsigned) {
    if (g_watch.cls < 0) return;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if ((int)a_cls(a) != g_watch.cls) continue;
        const short slot = a_slot(a);
        if (slot < 0 || slot >= kSwSlots) continue;
        unsigned v = 0;
        if (!safe_copy(&v, a + g_watch.off, (size_t)g_watch.size)) continue;
        if (g_watch.size == 1) v &= 0xff; else if (g_watch.size == 2) v &= 0xffff;
        WatchTrk &t = g_wtrk[slot];
        if (!t.valid || t.a != a) { t.a = a; t.v = v; t.valid = true; std::fprintf(stderr, "[coopsw] watch cls=%d slot=%d start=%x frame=%u wall=%lld\n", g_watch.cls, (int)slot, v, g_frames, wall_ms()); continue; }
        if (v != t.v) {
            std::fprintf(stderr, "[coopsw] watch cls=%d slot=%d %x -> %x frame=%u wall=%lld\n", g_watch.cls, (int)slot, t.v, v, g_frames, wall_ms());
            t.v = v;
        }
    }
}

void stats_line() {
    if (!g_log) return;
    const CoopSwitchStats &s = g_st;
    std::fprintf(stderr,
        "[coopswstat] seat=%d word=%08x armed=%d sent bit%llu word%llu press%llu kick%llu field%llu glob%llu states%llu "
        "recv bit%llu word%llu press%llu kick%llu field%llu glob%llu states%llu applied bits=%llu press=%llu kick=%llu field=%llu "
        "glob=%llu states=%llu pend=%llu local=%llu drop bad=%llu group=%llu member=%llu class=%llu old=%llu off=%llu\n",
        g_my, (unsigned)data_0209f34c, (int)g_armed, s.sent_bit, s.sent_word, s.sent_press, s.sent_kick, s.sent_field, s.sent_globals, s.sent_states,
        s.recv_bit, s.recv_word, s.recv_press, s.recv_kick, s.recv_field, s.recv_globals, s.recv_states,
        s.bits_applied, s.press_applied, s.kick_applied, s.field_applied, s.globals_applied, s.states_applied, s.states_pending, s.bits_local,
        s.drop_bad, s.drop_group, s.drop_member, s.drop_class, s.drop_old, s.drop_off);
}

// ---------------------------------------------------------------------------
// Level lifecycle and the frame.
// ---------------------------------------------------------------------------
void new_level() {
    Early keep[2];
    for (int i = 0; i < 2; ++i) { keep[i] = g_early[i]; if (keep[i].on && !(keep[i].lvl == g_lvl && keep[i].act == g_act)) keep[i].on = false; }
    std::memset(g_bit, 0, sizeof g_bit);
    std::memset(g_pb, 0, sizeof g_pb);
    std::memset(g_trk, 0, sizeof g_trk);
    std::memset(g_oo, 0, sizeof g_oo);
    std::memset(g_pend, 0, sizeof g_pend);
    std::memset(g_lastseq, 0, sizeof g_lastseq);
    std::memset(g_job, 0, sizeof g_job);
    std::memset(g_wtrk, 0, sizeof g_wtrk);
    g_counter = 0; g_frames = 0; g_armed = false; g_have_globals = false;
    g_shadow = (unsigned)data_0209f34c;
    g_word_t = 0;
    for (int i = 0; i < g_nknob; ++i) g_knob[i].done = false;
    for (int i = 0; i < g_nforge; ++i) g_forge[i].done = false;
    for (int i = 0; i < g_npress; ++i) g_press[i].done = false;
    g_rec_n = 0;
    g_new_level = false;
    for (int i = 0; i < 2; ++i) g_early[i] = keep[i];
}

void on_latched(int level, int act) {
    if (!coop_world_active()) return;
    read_env();
    g_my = comms_hub_my_seat();
    g_lvl = level; g_act = act; g_have_key = true;
    ++g_gen;
    new_level();
    if (g_log) std::fprintf(stderr, "[coopsw] latched level=%d act=%d word=%08x wall=%lld\n", level, act, (unsigned)data_0209f34c, wall_ms());
}

void on_established(int level, int act) {
    if (!coop_world_active()) return;
    g_lvl = level; g_act = act; g_have_key = true;
}

void on_snapshot_request(int seat, unsigned nonce) {
    if (!coop_world_active() || seat < 0 || seat >= kCoopMaxSeats || seat == g_my) return;
    Job &j = g_job[seat];
    j.on = true; j.seat = seat; j.nonce = nonce; j.sent = 0; j.next = 0; j.born = comms_hub_now_ms();
}

void on_member_gone(int seat) {
    if (seat >= 0 && seat < kCoopMaxSeats) { std::memset(g_lastseq[seat], 0, sizeof g_lastseq[seat]); g_job[seat].on = false; }
}

void register_handlers() {
    if (g_registered) return;
    g_registered = true;
    const bool a = coop_ctl_register(kSwCtlGlobals, kSwCtlLast, on_ctl, nullptr);
    const bool b = coop_world_register(kSwWorldBit, kSwWorldLast, on_world, nullptr);
    std::fprintf(stderr, "[coopsw] switch module on: ctl %s, world %s\n", a ? "ok" : "REFUSED", b ? "ok" : "REFUSED");
}

void on_frame(unsigned now) {
    g_now = now;
    read_env();
    if (!coop_world_active()) return;
    register_handlers();
    g_my = comms_hub_my_seat();
    int lv = -1, ac = 0;
    if (!coop_group_key(&lv, &ac) || !coop_group_established()) return;
    if (!g_have_key || lv != g_lvl || ac != g_act) {          // a level change that missed the latch
        g_lvl = lv; g_act = ac; g_have_key = true; ++g_gen; new_level();
    }
    const int me = (int)data_0209f250;
    if (me < 0 || me >= 16 || !data_0209f394[me]) return;
    ++g_frames;
    if (g_frames == 1) g_shadow = (unsigned)data_0209f34c;

    // early arrivals from a sender that was not yet a member
    for (Early &e : g_early) {
        if (!e.on || !is_member(e.seat)) continue;
        e.on = false;
        if (e.lvl != g_lvl || e.act != g_act) continue;
        if (e.kind == kSwCtlGlobals) { SwGlobalsWire w; std::memcpy(&w, e.buf, sizeof w); apply_globals(w); }
        else apply_states(e.buf, e.len);
    }

    // arm: the word is the group's once the joiner has been told (or nobody could tell it)
    if (!g_armed) {
        const bool alone_or_owner = coop_group_i_am_owner();
        const bool ready = (g_have_globals || alone_or_owner || g_frames >= kGlobalsWait) && g_frames >= kSettleFrames;
        if (g_have_globals) settle_word();
        if (ready) { g_armed = true; g_shadow = (unsigned)data_0209f34c; if (g_log) std::fprintf(stderr, "[coopsw] armed frame=%u word=%08x have_globals=%d wall=%lld\n", g_frames, g_shadow, (int)g_have_globals, wall_ms()); }
    } else {
        local_word_changes(now);
    }
    if (g_frames >= 3) scan_objects(now);
    static const bool pb_follow = [] { const char *e = std::getenv("SM64DS_COOP_PUSHBLOCK_FOLLOW"); return !(e && *e == '0'); }();
    if (g_frames >= 3 && pb_follow) pushblock_follow();   // SM64DS_COOP_PUSHBLOCK_FOLLOW=0 is the control: the stale model
    if (g_frames >= 3 && (g_frames % 15u) == 0) drain_pending(now);

    // proof setters (they do what a ROM setter does)
    for (int i = 0; i < g_nknob; ++i)
        if (!g_knob[i].done && g_armed && g_frames >= g_knob[i].frame) {
            g_knob[i].done = true;
            if (g_knob[i].bit >= 0 && g_knob[i].bit < 32) set_word_bit(g_knob[i].bit, g_knob[i].set ? 1 : 0);
            std::fprintf(stderr, "[coopsw-knob] %s bit %d wall=%lld\n", g_knob[i].set ? "set" : "clear", g_knob[i].bit, wall_ms());
            log_word("knob");
        }
    for (int i = 0; i < g_nforge; ++i)
        if (!g_forge[i].done && g_armed && g_frames >= g_forge[i].frame) { g_forge[i].done = true; knob_forge(g_forge[i].name, now); }
    for (int i = 0; i < g_npress; ++i)
        if (!g_press[i].done && g_armed && g_frames >= g_press[i].frame) { g_press[i].done = true; knob_press(g_press[i]); }
    watch_record();
    watch_field(now);
    ul_step();
    if (g_rec_on) {
        for (int i = 0; i < 3; ++i) if (g_rec_n == i && g_frames >= g_rec[i]) { rec_take(i); ++g_rec_n; if (i == 2) rec_report(); }
    }

    // outgoing
    send_bits(now);
    send_objs(now);
    if (g_armed && (unsigned)(now - g_word_t) >= (coop_group_i_am_owner() ? kWordOwnerMs : kWordGuestMs)) send_word(now);
    if (coop_group_i_am_owner())
        for (Job &j : g_job) {
            if (!j.on || (int)(now - j.next) < 0) continue;
            if (!is_member(j.seat)) { if ((unsigned)(now - j.born) > 5000u) j.on = false; continue; }
            serve_job(j);
            ++j.sent;
            if (j.sent >= 4) j.on = false; else j.next = now + (j.sent == 1 ? kJobMs : 300u);
        }
    if (g_log && (g_frames % 120u) == 0) stats_line();
}

struct Reg {
    Reg() {
        coop_on_frame(on_frame);
        coop_on_group_latched(on_latched);
        coop_on_group_established(on_established);
        coop_on_snapshot_request(on_snapshot_request);
        coop_on_member_gone(on_member_gone);
    }
} g_reg;

}  // namespace

CoopSwitchStats coop_switch_stats() { return g_st; }

}  // namespace port

// COOPRIDE1 (G40): flat faces of the spike bomb's member steps for the field above (the bodies are the matched TU's).
#include "daKirai_c.h"
extern "C" void coopsw_kirai_explode(void *bomb) { ((daKirai_c *)bomb)->func_ov060_021185c4(); }
extern "C" void coopsw_kirai_disarm(void *bomb) { ((daKirai_c *)bomb)->func_ov060_021184bc(); }
extern "C" void coopsw_kirai_rearm(void *bomb) { ((daKirai_c *)bomb)->func_ov060_021186d8(); }
