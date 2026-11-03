// FRIENDS' SOUNDS (run after093, lanes MPSOUND1 and MPSOUND2). What and why: hal/coop_sound.h.
//
// PORT_HOST_ABI. Reads Player fields and the camera matrix; never writes a body.
// The ROM calls are the flat Sound::Play (hal/sdat/sound_abi.cpp) and the three
// looping-sound entry points (Sound::PlayLong, func_0201226c, func_02012194 in
// src/engine/sound/Sound.cpp), with vectors of my own, exactly as the VS puppets
// play theirs (hal/vs_arena.cpp pup_play_sounds). A remote loop's vector lives in
// this file's static state, not on the stack: the ROM keeps a loop's position
// pointer for the frame.

#include "coop_sound.h"
#include "comms_hub.h"
#include "hub_presence.h"
#include "vs_arena.h"
#include "world_party.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern "C" {
extern void *data_0209f394[];             // the player bodies
extern unsigned char data_0209f250;       // my player slot
extern unsigned char data_0209f21c;       // number of player bodies
extern signed char data_0209f2f8;         // level (sublevel id)
extern signed char data_02092120;         // area
extern unsigned char data_0209f2d8[];     // game mode: 0 adventure / party / coop, 1 VS, 2 script
extern unsigned char data_0209fc48[];     // (s32) nonzero in a cutscene
extern int data_0209b3ec[];               // the camera matrix (world -> camera space)
void MulVec3Mat4x3(void *v, void *m, void *dst);
void _ZN5Sound4PlayEjjRK7Vector3(unsigned kind, unsigned id, void *v);
int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned handle, unsigned kind, unsigned id, const void *v, short e);
int func_0201226c(int handle, int kind, int id, int v, int a4, short a5);
int func_02012194(char *handle, int kind, int id, int a3, int s0, int v, short s2);
int port_adventure_ghost_seat(int slot);
int port_adventure_ghost_drawpos(int slot, int out[3]);
// src/engine/sound/Sound.cpp host arm (MPSOUND2): the Player an actor's sound is for and
// the actor's world position, set by the actor's own _MSC_VER arm around the call
extern const void *port_coop_sound_owner;
extern const void *port_coop_sound_at;
extern void (*port_coop_sound_loop_hook)(int fn, unsigned handle, unsigned kind, unsigned id, const void *v,
                                         int p0, int p1, int p2);
}
extern "C" unsigned sd_out_frames_rendered(void);   // hal/sdat/out_win.cpp: the WAV dump's sample index
int out_volume_pct(void);   // hal/sdat/out_win.cpp: the game's master volume, 0..100 (C++ linkage)

namespace port {
namespace {

enum : int {
    kFlagSound    = 0x80,   // HubChatHead.flags of a sound frame
    kPayloadVer   = 1,
    kMaxPerFrame  = 16,     // entries in one sound frame
    kQueue        = 32,     // my entry groups waiting to go
    kStaleFrames  = 30,     // a group that could not leave within a second is dropped
    kSendPerSec   = 4,      // sound frames a second (the relay allows 8 chat a second)
    kSendBurst    = 2,
    kWinFrames    = 10,     // the many-friends cap: at most kWinMax starts in 10 frames,
    kWinMax       = 8,      // at most kWinSeat from one friend
    kWinSeat      = 4,
    kDupFrames    = 15,     // a sound my console already made at that body this long ago plays once
    kPlayerSound  = 0x74,   // Player+0x74: the camera-space vector Player sounds play at
    kPlayerPos    = 0x5c,
    kBodies       = 32,
    kSeats        = kHubMaxSeats,
    // MPSOUND2 entry kinds (byte 0 of an entry; 0..3 are MPSOUND1's body sounds)
    kEntAtPos     = 0x10,   // 0x10 | kind: a one-shot at a position (two kEntPos entries follow)
    kEntLoop      = 0x20,   // 0x20 | kind: a loop starts or is still on (kEntParA, kEntParB, maybe two kEntPos)
    kEntLoopStop  = 0x30,   // a loop ended: byte 1 = its loop slot
    kEntParA      = 0x41,   // loop argument p0, 24 bits signed
    kEntParB      = 0x42,   // loop arguments p1 (16 bits signed) and p2 (8 bits signed)
    kEntPosA      = 0x48,   // world position x (s16), y low byte
    kEntPosB      = 0x49,   // world position y high byte, z (s16)
    kMyLoops      = 16,     // loops of mine tracked at once (the slot is 4 bits)
    kLoopRefresh  = 20,     // a live loop is sent again this often (frames)
    kLoopGone     = 2,      // frames without the ROM call before my loop counts as ended
    kLoopTimeout  = 60,     // a remote loop with no word for this long ends (lost stop, friend gone)
    kRemoteLoops  = 16,
    kLoopsPerSeat = 4,
};

struct Group   { unsigned char n; unsigned char e[5][4]; unsigned frame; bool age; };
struct Arrived { int seat; unsigned char kind; unsigned short id; unsigned char level, area; bool pos; int at[3]; };
struct Recent  { unsigned char kind; unsigned short id; unsigned frame; };

// one of MY loops (sender side)
struct MyLoop {
    bool live; int fn; unsigned kind, id; bool pos; const void *v;
    int p0, p1, p2; int at[3]; unsigned seen, sent; unsigned char slot;
};
// one of a FRIEND's loops (listener side)
struct RLoop {
    bool live; int seat; unsigned char slot; int fn; unsigned kind, id; bool pos;
    int p0, p1, p2; int at[3]; unsigned char level, area; unsigned last, started; int handle;
    bool farlog;   // a far line was printed since the loop last played
    int vec[3];   // the camera-space vector the ROM plays this loop at (static: the ROM keeps the pointer)
};

struct State {
    int enabled = -1, log = -1, rig = -2, dropstop = -1;
    bool hooked = false;
    unsigned frame = 0;
    Group q[kQueue]; int nq = 0;
    double tokens = kSendBurst; unsigned tok_ms = 0;
    unsigned short seq = 0;
    Arrived in[64]; int nin = 0;
    bool have_seq[kSeats] = {}; unsigned short last_seq[kSeats] = {};
    // the many-friends window: start frames of the last remote sounds
    unsigned win_frame[kWinMax * 4] = {}; int win_seat[kWinMax * 4] = {}; int nwin = 0;
    // sounds my console made at a friend's body or for a friend (its own Behavior), per body slot
    Recent local[kBodies][4] = {}; int nlocal[kBodies] = {};
    MyLoop my[kMyLoops] = {};
    RLoop rl[kRemoteLoops] = {};
    unsigned long long cap = 0, cap_pos = 0, sent_frames = 0, sent_sounds = 0, send_fail = 0, stale = 0, qfull = 0,
                       rx_frames = 0, rx_sounds = 0, rx_bad = 0, rx_old = 0, rx_stranger = 0,
                       played = 0, played_pos = 0, too_far = 0, other_place = 0, no_body = 0, capped = 0, dup = 0,
                       not_playing = 0, ghost_local = 0,
                       loop_cap = 0, loop_sent_start = 0, loop_sent_refresh = 0, loop_sent_stop = 0, loop_full = 0,
                       rloop_start = 0, rloop_stop = 0, rloop_timeout = 0, rloop_gone = 0, rloop_far = 0,
                       rloop_capped = 0, rloop_calls = 0, rloop_full = 0;
    bool reported = false;
} g;

bool env_on(const char *n) { const char *e = std::getenv(n); return e && *e && std::strcmp(e, "0") != 0; }
bool enabled() {
    if (g.enabled < 0) { const char *e = std::getenv("SM64DS_MPSOUND"); g.enabled = !(e && e[0] == '0' && !e[1]); }
    return g.enabled != 0;
}
bool logon() {
    if (g.log < 0) { const char *e = std::getenv("SM64DS_MPSOUND_LOG"); g.log = e && *e && std::strcmp(e, "0") ? (e[0] == '2' ? 2 : 1) : 0; }
    return g.log != 0;
}
// SM64DS_MPSOUND_LOG=2 (MPSOUND2 proof rows): also a [msnd] req / loop line for EVERY sound request this
// console makes, whoever makes it, with where it plays and the callers' addresses (module offsets; read
// them against walk_window.map)
bool logall() { return logon() && g.log == 2; }
// SM64DS_MPSOUND_DROPSTOP=1 (test only): never send a loop's stop entry, so the listener's timeout is what
// ends it (the lost-stop proof row)
bool dropstop() { if (g.dropstop < 0) g.dropstop = env_on("SM64DS_MPSOUND_DROPSTOP") ? 1 : 0; return g.dropstop != 0; }
void log_callers(char *out, int cap) {
    void *fr[6] = {}; const USHORT n = CaptureStackBackTrace(2, 6, fr, nullptr);
    const char *base = (const char *)GetModuleHandleW(nullptr);
    int k = 0; out[0] = 0;
    for (USHORT i = 0; i < n && k < cap - 12; ++i)
        k += std::snprintf(out + k, cap - k, "%s%x", i ? "," : "", (unsigned)((const char *)fr[i] - base));
}

char *my_body() {
    const unsigned s = data_0209f250;
    if (s >= kBodies || s >= data_0209f21c) return nullptr;
    return (char *)data_0209f394[s];
}
int body_count() { return data_0209f21c < kBodies ? data_0209f21c : kBodies; }
// the body slot whose Player+0x74 is v, or whose address is owner; -1 if none
int slot_at(const void *v) {
    const int n = body_count();
    for (int s = 0; s < n; ++s)
        if (data_0209f394[s] && v == (char *)data_0209f394[s] + kPlayerSound) return s;
    return -1;
}
int slot_of_body(const void *b) {
    const int n = body_count();
    for (int s = 0; s < n; ++s)
        if (b && data_0209f394[s] == b) return s;
    return -1;
}
const char *where(const void *v, int *slot) {
    *slot = slot_at(v);
    if (*slot < 0) return "other";
    return *slot == (int)data_0209f250 ? "me" : "friend-body";
}
const char *owner_name() {
    if (!port_coop_sound_owner) return "-";
    return port_coop_sound_owner == my_body() ? "me" : "other";
}

bool hub_live() { return comms_hub_active() && comms_hub_state() == kHubLive; }

// Is this console in a room where friends' sounds go both ways at all?
bool room_on() {
    if (!enabled() || !hub_live()) return false;
    if (data_0209f2d8[0] != 0) return false;            // VS keeps its own path (vs_arena.cpp)
    const HubRoomKind k = comms_hub_room_kind();
    if (k == kHubRoomParty || k == kHubRoomCoop) return true;
    return world_party_in_party();                     // world hub: only inside a party
}
bool seat_heard(int seat) {
    if (seat < 0 || seat >= kSeats || seat == comms_hub_my_seat()) return false;
    const HubRoomKind k = comms_hub_room_kind();
    if (k == kHubRoomParty || k == kHubRoomCoop) return true;
    return world_party_member(seat);
}
bool in_play() { return data_0209f2d8[0] == 0 && *(const int *)data_0209fc48 == 0 && my_body(); }

// ---- the send queue: groups of entries that leave in one frame together ------------
bool enqueue(const unsigned char (*e)[4], int n, bool age) {
    if (g.nq == kQueue) { ++g.qfull; return false; }
    Group &q = g.q[g.nq++];
    q.n = (unsigned char)n; q.frame = g.frame; q.age = age;
    for (int i = 0; i < n; ++i) std::memcpy(q.e[i], e[i], 4);
    return true;
}
short clamp16(int v) { return (short)(v < -32768 ? -32768 : v > 32767 ? 32767 : v); }
void pos_entries(unsigned char (*e)[4], const int *at) {
    const short x = clamp16(at[0] >> 12), y = clamp16(at[1] >> 12), z = clamp16(at[2] >> 12);
    e[0][0] = kEntPosA; e[0][1] = (unsigned char)(x & 0xff); e[0][2] = (unsigned char)((x >> 8) & 0xff); e[0][3] = (unsigned char)(y & 0xff);
    e[1][0] = kEntPosB; e[1][1] = (unsigned char)((y >> 8) & 0xff); e[1][2] = (unsigned char)(z & 0xff); e[1][3] = (unsigned char)((z >> 8) & 0xff);
}
void read_pos(const unsigned char *a, const unsigned char *b, int *at) {
    const short x = (short)(a[1] | (a[2] << 8)), y = (short)(a[3] | (b[1] << 8)), z = (short)(b[2] | (b[3] << 8));
    at[0] = (int)x << 12; at[1] = (int)y << 12; at[2] = (int)z << 12;
}

// ---- capture (src/engine/sound/Sound.cpp Sound::Play host arm, every request) -------
void on_sound(unsigned kind, unsigned id, const void *v) {
    if (logall()) {
        char ra[96]; log_callers(ra, sizeof ra); int sl; const char *w = where(v, &sl);
        const int *c = (const int *)v;
        std::fprintf(stderr, "[msnd] req f=%u kind=%u id=%u src=%s slot=%d owner=%s cam=(%d,%d,%d) ra=%s\n", g.frame, kind, id, w,
                     sl, owner_name(), c ? c[0] >> 12 : 0, c ? c[1] >> 12 : 0, c ? c[2] >> 12 : 0, ra);
    }
    if (!v || kind > 3 || id > 0xffff) return;
    char *me = my_body();
    if (me && v == me + kPlayerSound) {
        if (!room_on() || !in_play()) return;
        ++g.cap;
        if (logon()) std::fprintf(stderr, "[msnd] cap f=%u kind=%u id=%u\n", g.frame, kind, id);
        unsigned char e[1][4] = {{(unsigned char)kind, (unsigned char)(id & 0xff), (unsigned char)(id >> 8), 0}};
        enqueue(e, 1, true);
        return;
    }
    // MPSOUND2: a sound an actor plays for MY player (my egg), sent with the actor's world position
    if (me && port_coop_sound_owner == me && port_coop_sound_at) {
        if (!room_on() || !in_play()) return;
        ++g.cap_pos;
        const int *at = (const int *)port_coop_sound_at;
        if (logon()) std::fprintf(stderr, "[msnd] cap-at f=%u kind=%u id=%u pos=(%d,%d,%d)\n", g.frame, kind, id,
                                  at[0] >> 12, at[1] >> 12, at[2] >> 12);
        unsigned char e[3][4] = {{(unsigned char)(kEntAtPos | kind), (unsigned char)(id & 0xff), (unsigned char)(id >> 8), 0}};
        pos_entries(e + 1, at);
        enqueue(e, 3, true);
        return;
    }
    // a sound my console made at a FRIEND's body, or that an actor of mine played for a friend's
    // body: remembered, so the same sound from his console does not play twice
    int s = slot_at(v);
    if (s < 0 && port_coop_sound_owner) s = slot_of_body(port_coop_sound_owner);
    if (s >= 0 && s != (int)data_0209f250) {
        ++g.ghost_local;
        Recent &r = g.local[s][g.nlocal[s]++ & 3];
        r.kind = (unsigned char)kind; r.id = (unsigned short)id; r.frame = g.frame;
        if (logon()) std::fprintf(stderr, "[msnd] local-at-friend-body f=%u slot=%d kind=%u id=%u\n", g.frame, s, kind, id);
    }
}

// ---- looping sounds (PlayLong and its two siblings; Sound.cpp host arms) ------------
struct LoopSeen { int fn; unsigned kind, id; const void *v; unsigned last; };
LoopSeen g_seen[64]; int g_nseen = 0;
void log_loop(int fn, unsigned handle, unsigned kind, unsigned id, const void *v, int p0, int p1, int p2) {
    // one line when a loop starts (or comes back after a gap), not one a frame
    for (int i = 0; i < g_nseen; ++i) {
        LoopSeen &e = g_seen[i];
        if (e.fn == fn && e.kind == kind && e.id == id && e.v == v) {
            const bool gap = g.frame - e.last > 2; e.last = g.frame;
            if (!gap) return;
            goto print;
        }
    }
    {
        LoopSeen &e = g_seen[g_nseen < 64 ? g_nseen++ : (int)(g.frame & 63)];
        e.fn = fn; e.kind = kind; e.id = id; e.v = v; e.last = g.frame;
    }
print:
    {
        char ra[96]; log_callers(ra, sizeof ra); int sl; const char *w = where(v, &sl);
        std::fprintf(stderr, "[msnd] loop f=%u fn=%d handle=%u kind=%u id=%u p=%d,%d,%d src=%s slot=%d owner=%s ra=%s\n", g.frame,
                     fn, handle, kind, id, p0, p1, p2, w, sl, owner_name(), ra);
    }
}

void on_loop(int fn, unsigned handle, unsigned kind, unsigned id, const void *v, int p0, int p1, int p2) {
    if (logall()) log_loop(fn, handle, kind, id, v, p0, p1, p2);
    if (!v || kind > 3 || id > 0xffff || fn < 0 || fn > 2) return;
    char *me = my_body();
    if (!me) return;
    const bool body = v == me + kPlayerSound;
    const bool mine = !body && port_coop_sound_owner == me && port_coop_sound_at;
    if (!body && !mine) return;
    if (!room_on() || !in_play()) return;
    // the same loop from the same place keeps its slot while the ROM is called for it every frame
    int free_i = -1;
    for (int i = 0; i < kMyLoops; ++i) {
        MyLoop &l = g.my[i];
        if (!l.live) { if (free_i < 0) free_i = i; continue; }
        if (l.fn == fn && l.kind == kind && l.id == id && l.v == v) {
            l.seen = g.frame; l.p0 = p0; l.p1 = p1; l.p2 = p2;
            if (mine) std::memcpy(l.at, port_coop_sound_at, sizeof l.at);
            return;
        }
    }
    if (free_i < 0) { ++g.loop_full; return; }
    MyLoop &l = g.my[free_i];
    l.live = true; l.fn = fn; l.kind = kind; l.id = id; l.pos = mine; l.v = v;
    l.p0 = p0; l.p1 = p1; l.p2 = p2; l.seen = g.frame; l.sent = 0; l.slot = (unsigned char)free_i;
    if (mine) std::memcpy(l.at, port_coop_sound_at, sizeof l.at); else std::memset(l.at, 0, sizeof l.at);
    ++g.loop_cap;
    if (logon()) std::fprintf(stderr, "[msnd] cap-loop f=%u slot=%d fn=%d kind=%u id=%u pos=%d\n", g.frame, free_i, fn, kind, id, (int)mine);
}

// once a frame: start / refresh / stop entries for my loops
void scan_my_loops() {
    const bool on = room_on() && in_play();
    for (int i = 0; i < kMyLoops; ++i) {
        MyLoop &l = g.my[i];
        if (!l.live) continue;
        if (!on || g.frame - l.seen > (unsigned)kLoopGone) {
            l.live = false;
            if (!l.sent || !on) continue;   // never left, or I am not in play (the listener's timeout ends it)
            if (dropstop()) { if (logon()) std::fprintf(stderr, "[msnd] loop-stop-dropped f=%u slot=%d\n", g.frame, i); continue; }
            unsigned char e[1][4] = {{kEntLoopStop, l.slot, 0, 0}};
            if (enqueue(e, 1, false)) {
                ++g.loop_sent_stop;
                if (logon()) std::fprintf(stderr, "[msnd] loop-stop f=%u slot=%d kind=%u id=%u\n", g.frame, i, l.kind, l.id);
            }
            continue;
        }
        if (l.sent && g.frame - l.sent < (unsigned)kLoopRefresh) continue;
        unsigned char e[5][4] = {};
        e[0][0] = (unsigned char)(kEntLoop | l.kind); e[0][1] = (unsigned char)(l.id & 0xff); e[0][2] = (unsigned char)(l.id >> 8);
        e[0][3] = (unsigned char)((l.fn & 3) | (l.pos ? 4 : 0) | (l.slot << 4));
        e[1][0] = kEntParA; e[1][1] = (unsigned char)(l.p0 & 0xff); e[1][2] = (unsigned char)((l.p0 >> 8) & 0xff); e[1][3] = (unsigned char)((l.p0 >> 16) & 0xff);
        const short p1 = clamp16(l.p1); const int p2 = l.p2 < -128 ? -128 : l.p2 > 127 ? 127 : l.p2;
        e[2][0] = kEntParB; e[2][1] = (unsigned char)(p1 & 0xff); e[2][2] = (unsigned char)((p1 >> 8) & 0xff); e[2][3] = (unsigned char)(p2 & 0xff);
        int n = 3;
        if (l.pos) { pos_entries(e + 3, l.at); n = 5; }
        if (!enqueue(e, n, false)) continue;
        if (l.sent) ++g.loop_sent_refresh; else ++g.loop_sent_start;
        l.sent = g.frame;
    }
}

// ---- receive (hal/adv_chat.cpp pump_recv, a chat datagram with flags 0x80) -------
RLoop *find_rloop(int seat, int slot) {
    for (int i = 0; i < kRemoteLoops; ++i)
        if (g.rl[i].live && g.rl[i].seat == seat && g.rl[i].slot == slot) return &g.rl[i];
    return nullptr;
}

void on_wire(int seat, const unsigned char *b, int n) {
    ++g.rx_frames;
    if (!enabled()) return;
    if (n < (int)sizeof(HubChatHead) + 4) { ++g.rx_bad; return; }
    HubChatHead h; std::memcpy(&h, b, sizeof h);
    const unsigned char *p = b + sizeof h;
    const int cnt = p[3];
    if (h.flags != kFlagSound || p[0] != kPayloadVer || cnt < 1 || cnt > kMaxPerFrame ||
        (int)h.text_len != 4 + 4 * cnt || n != (int)sizeof h + h.text_len || seat < 0 || seat >= kSeats) {
        ++g.rx_bad; return;
    }
    if (g.have_seq[seat] && (short)(h.msg_id - g.last_seq[seat]) <= 0 &&
        (short)(h.msg_id - g.last_seq[seat]) > -64) { ++g.rx_old; return; }
    g.have_seq[seat] = true; g.last_seq[seat] = h.msg_id;
    if (!seat_heard(seat)) { ++g.rx_stranger; return; }
    const unsigned char level = p[1], area = p[2];
    for (int i = 0; i < cnt; ++i) {
        const unsigned char *e = p + 4 + 4 * i;
        const unsigned char k = e[0];
        ++g.rx_sounds;
        if (k <= 3 || (k & 0xf0) == kEntAtPos) {
            // a one-shot: at his body (MPSOUND1), or at a position (two position entries follow)
            const bool pos = k > 3;
            int at[3] = {0, 0, 0};
            if (pos) {
                if (i + 2 >= cnt || e[4] != kEntPosA || e[8] != kEntPosB) { ++g.rx_bad; continue; }
                read_pos(e + 4, e + 8, at);
                i += 2;
            }
            if (g.nin == (int)(sizeof g.in / sizeof g.in[0])) { ++g.capped; continue; }
            Arrived &a = g.in[g.nin++];
            a.seat = seat; a.kind = (unsigned char)(k & 3); a.id = (unsigned short)(e[1] | (e[2] << 8));
            a.level = level; a.area = area; a.pos = pos; std::memcpy(a.at, at, sizeof at);
        } else if ((k & 0xf0) == kEntLoop) {
            const int ctl = e[3];
            const bool pos = (ctl & 4) != 0;
            const int need = pos ? 4 : 2;
            if (i + need >= cnt || e[4] != kEntParA || e[8] != kEntParB || (pos && (e[12] != kEntPosA || e[16] != kEntPosB)) ||
                (ctl & 3) == 3) {
                ++g.rx_bad; continue;
            }
            const int slot = ctl >> 4;
            RLoop *l = find_rloop(seat, slot);
            const bool fresh = !l;
            if (!l) {
                int per = 0;
                for (int j = 0; j < kRemoteLoops; ++j) if (g.rl[j].live && g.rl[j].seat == seat) ++per;
                for (int j = 0; j < kRemoteLoops && !l && per < kLoopsPerSeat; ++j) if (!g.rl[j].live) l = &g.rl[j];
                if (!l) { ++g.rloop_full; i += need; continue; }
                std::memset(l, 0, sizeof *l);
                l->live = true; l->seat = seat; l->slot = (unsigned char)slot; l->handle = 0; l->started = 0;
            }
            const unsigned kind = k & 3, id = (unsigned)(e[1] | (e[2] << 8));
            if (!fresh && (l->kind != kind || l->id != id || l->fn != (ctl & 3))) l->handle = 0;   // the slot now holds another loop
            l->fn = ctl & 3; l->kind = kind; l->id = id; l->pos = pos;
            int p0 = e[5] | (e[6] << 8) | (e[7] << 16); if (p0 & 0x800000) p0 |= ~0xffffff;
            l->p0 = p0; l->p1 = (short)(e[9] | (e[10] << 8)); l->p2 = (signed char)e[11];
            if (pos) read_pos(e + 12, e + 16, l->at);
            l->level = level; l->area = area; l->last = g.frame;
            if (fresh && logon())
                std::fprintf(stderr, "[msnd] rloop-on f=%u seat=%d slot=%d fn=%d kind=%u id=%u pos=%d p=%d,%d,%d\n", g.frame, seat, slot,
                             l->fn, kind, id, (int)pos, l->p0, l->p1, l->p2);
            i += need;
        } else if (k == kEntLoopStop) {
            RLoop *l = find_rloop(seat, e[1] & 15);
            if (l) {
                l->live = false; ++g.rloop_stop;
                if (logon()) std::fprintf(stderr, "[msnd] rloop-end f=%u seat=%d slot=%d kind=%u id=%u why=stop frames=%u\n", g.frame, seat,
                                          (int)l->slot, l->kind, l->id, l->started ? g.frame - l->started : 0u);
            }
        } else {
            ++g.rx_bad;   // an entry kind this build does not know: skipped alone
        }
    }
}

// ---- send ---------------------------------------------------------------------
void pump_send() {
    // drop what waited too long: a sound a second late is noise, not a footstep
    int k = 0;
    for (int i = 0; i < g.nq; ++i) {
        if (g.frame - g.q[i].frame > (unsigned)kStaleFrames) { ++g.stale; continue; }
        g.q[k++] = g.q[i];
    }
    g.nq = k;
    if (!g.nq || !room_on() || !in_play()) return;
    const unsigned now = GetTickCount();
    if (g.tok_ms) {
        g.tokens += (double)(now - g.tok_ms) * kSendPerSec / 1000.0;
        if (g.tokens > kSendBurst) g.tokens = kSendBurst;
    }
    g.tok_ms = now;
    if (g.tokens < 1.0) return;
    // whole groups only, up to kMaxPerFrame entries
    int ng = 0, cnt = 0;
    while (ng < g.nq && cnt + g.q[ng].n <= kMaxPerFrame) cnt += g.q[ng++].n;
    if (!cnt) return;
    unsigned char d[sizeof(HubChatHead) + 4 + 4 * kMaxPerFrame];
    HubChatHead h; std::memset(&h, 0, sizeof h);
    h.tag = kHubTagChat; h.version = 1; h.seat = 0xff; h.flags = kFlagSound;
    h.text_len = (unsigned char)(4 + 4 * cnt); h.msg_id = ++g.seq;
    std::memcpy(d, &h, sizeof h);
    unsigned char *p = d + sizeof h;
    p[0] = kPayloadVer; p[1] = (unsigned char)data_0209f2f8; p[2] = (unsigned char)data_02092120; p[3] = (unsigned char)cnt;
    int at = 0;
    for (int gi = 0; gi < ng; ++gi) {
        const Group &q = g.q[gi];
        for (int i = 0; i < q.n; ++i, ++at) {
            unsigned char *e = p + 4 + 4 * at;
            std::memcpy(e, q.e[i], 4);
            if (i == 0 && q.age) {
                const unsigned age = g.frame - q.frame;
                e[3] = (unsigned char)(age > 255 ? 255 : age);
            }
        }
    }
    const int len = (int)sizeof h + h.text_len;
    if (!comms_hub_send(d, len)) { ++g.send_fail; return; }
    g.tokens -= 1.0;
    ++g.sent_frames; g.sent_sounds += (unsigned long long)ng;
    if (logon()) std::fprintf(stderr, "[msnd] send f=%u seq=%u n=%d groups=%d lvl=%d area=%d\n", g.frame, (unsigned)h.msg_id, cnt, ng,
                              (int)data_0209f2f8, (int)data_02092120);
    for (int i = ng; i < g.nq; ++i) g.q[i - ng] = g.q[i];
    g.nq -= ng;
}

// ---- play -----------------------------------------------------------------------
int slot_of_seat(int seat) {
    const int n = body_count();
    for (int s = 0; s < n; ++s)
        if (s != (int)data_0209f250 && port_adventure_ghost_seat(s) == seat) return s;
    return -1;
}

bool window_take(int seat) {
    int k = 0, total = 0, mine = 0;
    for (int i = 0; i < g.nwin; ++i) {
        if (g.frame - g.win_frame[i] >= (unsigned)kWinFrames) continue;
        g.win_frame[k] = g.win_frame[i]; g.win_seat[k] = g.win_seat[i]; ++k;
        ++total; if (g.win_seat[i] == seat) ++mine;
    }
    g.nwin = k;
    if (total >= kWinMax || mine >= kWinSeat || g.nwin >= kWinMax * 4) return false;
    g.win_frame[g.nwin] = g.frame; g.win_seat[g.nwin] = seat; ++g.nwin;
    return true;
}

bool already_local(int slot, unsigned kind, unsigned id) {
    for (int i = 0; i < 4 && i < g.nlocal[slot]; ++i) {
        const Recent &r = g.local[slot][i];
        if (r.kind == kind && r.id == id && g.frame - r.frame <= (unsigned)kDupFrames) return true;
    }
    return false;
}

// the vector the ROM plays a friend's sound at: the source's spot in camera space (what
// dActor_c::BeforeBehavior writes at +0x74 for a body that thinks), its length set so the
// ROM's own fade gives the arena's near..far ramp from MY body. Returns the ROM length.
double rom_vector(const int src[3], double dist, bool range, int vec[3]) {
    vec[0] = vec[1] = vec[2] = 0;
    MulVec3Mat4x3((void *)src, data_0209b3ec, vec);
    if (range) {
        const double romd = vs_puppet_sfx_romdist(dist);
        const double tx = (double)(vec[0] >> 12), ty = (double)(vec[1] >> 12) * 2.0, tz = (double)(vec[2] >> 12);
        const double len = std::sqrt(tx * tx + ty * ty + tz * tz);
        if (len >= 1.0) {
            const double f = romd / len;
            vec[0] = (int)(vec[0] * f); vec[1] = (int)(vec[1] * f); vec[2] = (int)(vec[2] * f);
        } else {
            vec[0] = 0; vec[1] = 0; vec[2] = (int)(romd * 4096.0);
        }
    }
    const double tx = (double)(vec[0] >> 12), ty = (double)(vec[1] >> 12) * 2.0, tz = (double)(vec[2] >> 12);
    return std::sqrt(tx * tx + ty * ty + tz * tz);
}

void play_one(const Arrived &a) {
    const char *why = nullptr;
    double dist = -1.0, gain = 0.0, romlen = -1.0;
    int slot = -1;
    char *me = my_body();
    if (!in_play() || !me) { ++g.not_playing; why = "not-in-play"; }
    else if (a.level != (unsigned char)data_0209f2f8 || a.area != (unsigned char)data_02092120) { ++g.other_place; why = "other-level-or-area"; }
    else if ((slot = slot_of_seat(a.seat)) < 0) { ++g.no_body; why = "no-body"; }
    int src[3] = {0, 0, 0};
    if (!why) {
        if (a.pos) std::memcpy(src, a.at, sizeof src);
        else if (!port_adventure_ghost_drawpos(slot, src)) { ++g.no_body; why = "not-drawn"; }
    }
    if (!why) {
        const int mine[3] = {*(const int *)(me + kPlayerPos), *(const int *)(me + kPlayerPos + 4),
                             *(const int *)(me + kPlayerPos + 8)};
        dist = vs_puppet_sfx_distance(mine, src);
        gain = vs_puppet_sfx_gain(mine, src);
        double nr = 0, fr = 0;
        const bool range = vs_puppet_sfx_rule(&nr, &fr);
        if (range && dist >= fr) { ++g.too_far; why = "far"; }
        else if (already_local(slot, a.kind, a.id)) { ++g.dup; why = "already-played-here"; }
        else if (!window_take(a.seat)) { ++g.capped; why = "capped"; }
        else {
            int vec[3];
            romlen = rom_vector(src, dist, range, vec);
            ++g.played; if (a.pos) ++g.played_pos;
            if (logon())
                std::fprintf(stderr, "[msnd] play f=%u seat=%d slot=%d kind=%u id=%u at=%s dist=%.0f gain=%.2f romvec_len=%.0f vol=%d wav=%u\n",
                             g.frame, a.seat, slot, (unsigned)a.kind, (unsigned)a.id, a.pos ? "pos" : "body", dist, gain, romlen,
                             out_volume_pct(), sd_out_frames_rendered());
            _ZN5Sound4PlayEjjRK7Vector3((unsigned)a.kind, (unsigned)a.id, vec);
            return;
        }
    }
    if (logon())
        std::fprintf(stderr, "[msnd] skip f=%u seat=%d kind=%u id=%u at=%s dist=%.0f gain=%.2f lvl=%d/%d area=%d/%d why=%s wav=%u\n",
                     g.frame, a.seat, (unsigned)a.kind, (unsigned)a.id, a.pos ? "pos" : "body", dist, gain, (int)a.level,
                     (int)data_0209f2f8, (int)a.area, (int)data_02092120, why, sd_out_frames_rendered());
}

// once a frame: every friend's live loop is called into the ROM again (the ROM ends a loop
// that misses a frame's call), at his drawn body or the position he sent
void run_remote_loops() {
    char *me = my_body();
    for (int i = 0; i < kRemoteLoops; ++i) {
        RLoop &l = g.rl[i];
        if (!l.live) continue;
        const char *end = nullptr;
        int slot = -1, src[3] = {0, 0, 0};
        if (g.frame - l.last > (unsigned)kLoopTimeout) { end = "timeout"; ++g.rloop_timeout; }
        else if (!enabled() || !in_play() || !me || !seat_heard(l.seat)) { end = "not-in-play"; ++g.rloop_gone; }
        else if (l.level != (unsigned char)data_0209f2f8 || l.area != (unsigned char)data_02092120) { end = "other-level-or-area"; ++g.rloop_gone; }
        else if ((slot = slot_of_seat(l.seat)) < 0) { end = "no-body"; ++g.rloop_gone; }
        else if (l.pos) std::memcpy(src, l.at, sizeof src);
        else if (!port_adventure_ghost_drawpos(slot, src)) { end = "not-drawn"; ++g.rloop_gone; }
        if (end) {
            l.live = false;
            if (logon()) std::fprintf(stderr, "[msnd] rloop-end f=%u seat=%d slot=%d kind=%u id=%u why=%s frames=%u\n", g.frame, l.seat,
                                      (int)l.slot, l.kind, l.id, end, l.started ? g.frame - l.started : 0);
            continue;
        }
        const int mine[3] = {*(const int *)(me + kPlayerPos), *(const int *)(me + kPlayerPos + 4),
                             *(const int *)(me + kPlayerPos + 8)};
        const double dist = vs_puppet_sfx_distance(mine, src);
        double nr = 0, fr = 0;
        const bool range = vs_puppet_sfx_rule(&nr, &fr);
        if (range && dist >= fr) {
            // out of earshot: no call this frame, so the ROM lets the loop go; it starts again if he comes back
            if (l.started || !l.farlog) {
                ++g.rloop_far; l.started = 0; l.handle = 0; l.farlog = true;
                if (logon()) std::fprintf(stderr, "[msnd] rloop-far f=%u seat=%d slot=%d id=%u dist=%.0f\n", g.frame, l.seat, (int)l.slot, l.id, dist);
            }
            continue;
        }
        if (!l.started) {
            if (!window_take(l.seat)) { ++g.rloop_capped; continue; }
            l.started = g.frame; l.farlog = false; ++g.rloop_start;
            if (logon())
                std::fprintf(stderr, "[msnd] rloop-play f=%u seat=%d slot=%d fn=%d kind=%u id=%u at=%s dist=%.0f gain=%.2f vol=%d wav=%u\n",
                             g.frame, l.seat, (int)l.slot, l.fn, l.kind, l.id, l.pos ? "pos" : "body", dist,
                             vs_puppet_sfx_gain(mine, src), out_volume_pct(), sd_out_frames_rendered());
        }
        rom_vector(src, dist, range, l.vec);
        ++g.rloop_calls;
        if (l.fn == 0)
            l.handle = _ZN5Sound8PlayLongEjjjRK7Vector3s((unsigned)l.handle, l.kind, l.id, l.vec, (short)l.p2);
        else if (l.fn == 1)
            l.handle = func_0201226c(l.handle, (int)l.kind, (int)l.id, (int)(size_t)l.vec, l.p0, (short)l.p1);
        else
            l.handle = func_02012194((char *)(size_t)l.handle, (int)l.kind, (int)l.id, l.p0, l.p1, (int)(size_t)l.vec, (short)l.p2);
    }
}

void report() {
    if (g.reported || !(g.cap || g.rx_frames || g.loop_cap || g.cap_pos)) return;
    g.reported = true;
    std::fprintf(stderr, "[msnd] exit captured=%llu captured_at_pos=%llu sent_frames=%llu sent_groups=%llu send_fail=%llu stale=%llu queue_full=%llu\n",
                 g.cap, g.cap_pos, g.sent_frames, g.sent_sounds, g.send_fail, g.stale, g.qfull);
    std::fprintf(stderr, "[msnd] exit rx_frames=%llu rx_sounds=%llu rx_bad=%llu rx_old=%llu rx_stranger=%llu played=%llu played_pos=%llu "
                 "far=%llu other_place=%llu no_body=%llu capped=%llu dup=%llu not_playing=%llu local_at_friend=%llu\n",
                 g.rx_frames, g.rx_sounds, g.rx_bad, g.rx_old, g.rx_stranger, g.played, g.played_pos, g.too_far, g.other_place,
                 g.no_body, g.capped, g.dup, g.not_playing, g.ghost_local);
    std::fprintf(stderr, "[msnd] exit loops captured=%llu sent_start=%llu sent_refresh=%llu sent_stop=%llu full=%llu | remote start=%llu "
                 "stop=%llu timeout=%llu gone=%llu far=%llu capped=%llu calls=%llu full=%llu\n",
                 g.loop_cap, g.loop_sent_start, g.loop_sent_refresh, g.loop_sent_stop, g.loop_full, g.rloop_start, g.rloop_stop,
                 g.rloop_timeout, g.rloop_gone, g.rloop_far, g.rloop_capped, g.rloop_calls, g.rloop_full);
}

// SM64DS_MPSOUND_RIG_LOOP=<from>,<on>,<off>: from that frame, MY body runs the butt-slide loop (kind 0
// id 228 through func_0201226c, the call St_ButtSlide_Main makes every frame) for <on> frames, then
// rests <off> frames, over and over (the loop proof rows' source; test only)
void rig_loop() {
    static int from = -2, on = 0, off = 0, h = 0;
    if (from == -2) {
        const char *e = std::getenv("SM64DS_MPSOUND_RIG_LOOP");
        from = -1;
        if (e && *e && std::sscanf(e, "%d,%d,%d", &from, &on, &off) != 3) from = -1;
        if (on < 1 || off < 0) from = -1;
    }
    if (from < 0 || (int)g.frame < from) return;
    char *me = my_body();
    if (!me || !in_play()) return;
    if (((int)g.frame - from) % (on + off) >= on) { h = 0; return; }
    h = func_0201226c(h, 0, 228, (int)(size_t)(me + kPlayerSound), 0x8000, 0);
}

// SM64DS_MPSOUND_RIG_EGG=<from>,<ahead>: from that frame, every 150 frames, the egg pop (kind 1 id 259,
// what daYegg_c's burst plays) is asked for as an actor sound for MY player at a spot <ahead> world units
// along +x from my body (test only: the positional one-shot proof at a chosen distance)
void rig_egg() {
    static int from = -2, ahead = 0;
    if (from == -2) {
        const char *e = std::getenv("SM64DS_MPSOUND_RIG_EGG");
        from = -1;
        if (e && *e && std::sscanf(e, "%d,%d", &from, &ahead) != 2) from = -1;
    }
    if (from < 0 || (int)g.frame < from || ((int)g.frame - from) % 150 != 0) return;
    char *me = my_body();
    if (!me || !in_play()) return;
    static int at[3];
    at[0] = *(const int *)(me + kPlayerPos) + (ahead << 12);
    at[1] = *(const int *)(me + kPlayerPos + 4);
    at[2] = *(const int *)(me + kPlayerPos + 8);
    int cam[3] = {0, 0, 0};
    MulVec3Mat4x3(at, data_0209b3ec, cam);
    port_coop_sound_owner = me; port_coop_sound_at = at;
    _ZN5Sound4PlayEjjRK7Vector3(1u, 259u, cam);
    port_coop_sound_owner = nullptr; port_coop_sound_at = nullptr;
}

void rig() {
    rig_loop();
    rig_egg();
    if (g.rig == -2) { const char *e = std::getenv("SM64DS_MPSOUND_RIG"); g.rig = e && *e ? std::atoi(e) : -1; }
    if (g.rig < 0 || (int)g.frame < g.rig || ((int)g.frame - g.rig) % 45 != 0) return;
    char *me = my_body();
    if (!me || !in_play()) return;
    // SM64DS_MPSOUND_RIG_BURST=<n>: n pairs at once instead of one (the many-friends cap row)
    static int burst = -1;
    if (burst < 0) { const char *e = std::getenv("SM64DS_MPSOUND_RIG_BURST"); burst = e && *e ? std::atoi(e) : 1; if (burst < 1 || burst > 16) burst = 1; }
    for (int i = 0; i < burst; ++i) {
        _ZN5Sound4PlayEjjRK7Vector3(0u, 65u, me + kPlayerSound);
        _ZN5Sound4PlayEjjRK7Vector3(1u, 252u, me + kPlayerSound);
    }
}

}  // namespace
}  // namespace port

extern "C" void port_coop_sound_frame(void) {
    using namespace port;
    if (!g.hooked) {
        if (!enabled()) return;
        g.hooked = true;
        port_coop_sound_hook = on_sound;
        port_chat_sound_rx = on_wire;
        port_coop_sound_loop_hook = on_loop;
        std::atexit(report);
    }
    ++g.frame;
    rig();
    scan_my_loops();
    pump_send();
    for (int i = 0; i < g.nin; ++i) play_one(g.in[i]);
    g.nin = 0;
    run_remote_loops();
}
