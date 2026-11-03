// FRIENDS' SOUNDS (run after093, lane MPSOUND1). What and why: hal/coop_sound.h.
//
// PORT_HOST_ABI. Reads Player fields and the camera matrix; never writes a body.
// The one ROM call is the flat Sound::Play (hal/sdat/sound_abi.cpp), with a
// vector of my own on the stack, exactly as the VS puppets play theirs
// (hal/vs_arena.cpp pup_play_sounds).

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
int port_adventure_ghost_seat(int slot);
int port_adventure_ghost_drawpos(int slot, int out[3]);
}
extern "C" unsigned sd_out_frames_rendered(void);   // hal/sdat/out_win.cpp: the WAV dump's sample index
int out_volume_pct(void);   // hal/sdat/out_win.cpp: the game's master volume, 0..100 (C++ linkage)

namespace port {
namespace {

enum : int {
    kFlagSound    = 0x80,   // HubChatHead.flags of a sound frame
    kPayloadVer   = 1,
    kMaxPerFrame  = 16,     // sounds in one sound frame
    kQueue        = 32,     // my sounds waiting to go
    kStaleFrames  = 30,     // a sound that could not leave within a second is dropped
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
};

struct Pending { unsigned char kind; unsigned short id; unsigned frame; };
struct Arrived { int seat; unsigned char kind; unsigned short id; unsigned char level, area; };
struct Recent  { unsigned char kind; unsigned short id; unsigned frame; };

struct State {
    int enabled = -1, log = -1, rig = -2;
    bool hooked = false;
    unsigned frame = 0;
    Pending q[kQueue]; int nq = 0;
    double tokens = kSendBurst; unsigned tok_ms = 0;
    unsigned short seq = 0;
    Arrived in[64]; int nin = 0;
    bool have_seq[kSeats] = {}; unsigned short last_seq[kSeats] = {};
    // the many-friends window: start frames of the last remote sounds
    unsigned win_frame[kWinMax * 4] = {}; int win_seat[kWinMax * 4] = {}; int nwin = 0;
    // sounds my console made at a friend's body (its own Behavior), per body slot
    Recent local[kBodies][4] = {}; int nlocal[kBodies] = {};
    unsigned long long cap = 0, sent_frames = 0, sent_sounds = 0, send_fail = 0, stale = 0, qfull = 0,
                       rx_frames = 0, rx_sounds = 0, rx_bad = 0, rx_old = 0, rx_stranger = 0,
                       played = 0, too_far = 0, other_place = 0, no_body = 0, capped = 0, dup = 0,
                       not_playing = 0, ghost_local = 0;
    bool reported = false;
} g;

bool env_on(const char *n) { const char *e = std::getenv(n); return e && *e && std::strcmp(e, "0") != 0; }
bool enabled() {
    if (g.enabled < 0) { const char *e = std::getenv("SM64DS_MPSOUND"); g.enabled = !(e && e[0] == '0' && !e[1]); }
    return g.enabled != 0;
}
bool logon() { if (g.log < 0) g.log = env_on("SM64DS_MPSOUND_LOG") ? 1 : 0; return g.log != 0; }

char *my_body() {
    const unsigned s = data_0209f250;
    if (s >= kBodies || s >= data_0209f21c) return nullptr;
    return (char *)data_0209f394[s];
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

// ---- capture (hal/sdat/sound_abi.cpp, every accepted request) -----------------
void on_sound(unsigned kind, unsigned id, const void *v) {
    if (!v || kind > 3 || id > 0xffff) return;
    char *me = my_body();
    if (me && v == me + kPlayerSound) {
        if (!room_on() || !in_play()) return;
        ++g.cap;
        if (logon()) std::fprintf(stderr, "[msnd] cap f=%u kind=%u id=%u\n", g.frame, kind, id);
        if (g.nq == kQueue) { ++g.qfull; return; }
        g.q[g.nq].kind = (unsigned char)kind; g.q[g.nq].id = (unsigned short)id; g.q[g.nq].frame = g.frame;
        ++g.nq;
        return;
    }
    // a sound my console made at a FRIEND's body (its own Behavior ran): remembered,
    // so the same sound from his console does not play twice
    const int n = data_0209f21c < kBodies ? data_0209f21c : kBodies;
    for (int s = 0; s < n; ++s) {
        if (s == (int)data_0209f250 || !data_0209f394[s]) continue;
        if (v != (char *)data_0209f394[s] + kPlayerSound) continue;
        ++g.ghost_local;
        Recent &r = g.local[s][g.nlocal[s]++ & 3];
        r.kind = (unsigned char)kind; r.id = (unsigned short)id; r.frame = g.frame;
        if (logon()) std::fprintf(stderr, "[msnd] local-at-friend-body f=%u slot=%d kind=%u id=%u\n", g.frame, s, kind, id);
        return;
    }
}

// ---- receive (hal/adv_chat.cpp pump_recv, a chat datagram with flags 0x80) -------
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
    for (int i = 0; i < cnt; ++i) {
        const unsigned char *e = p + 4 + 4 * i;
        ++g.rx_sounds;
        if (e[0] > 3) { ++g.rx_bad; continue; }   // the four sound kinds the ROM has (an id is bounds-checked by func_020523e0)
        if (g.nin == (int)(sizeof g.in / sizeof g.in[0])) { ++g.capped; continue; }
        Arrived &a = g.in[g.nin++];
        a.seat = seat; a.kind = e[0]; a.id = (unsigned short)(e[1] | (e[2] << 8));
        a.level = p[1]; a.area = p[2];
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
    const int cnt = g.nq < kMaxPerFrame ? g.nq : kMaxPerFrame;
    unsigned char d[sizeof(HubChatHead) + 4 + 4 * kMaxPerFrame];
    HubChatHead h; std::memset(&h, 0, sizeof h);
    h.tag = kHubTagChat; h.version = 1; h.seat = 0xff; h.flags = kFlagSound;
    h.text_len = (unsigned char)(4 + 4 * cnt); h.msg_id = ++g.seq;
    std::memcpy(d, &h, sizeof h);
    unsigned char *p = d + sizeof h;
    p[0] = kPayloadVer; p[1] = (unsigned char)data_0209f2f8; p[2] = (unsigned char)data_02092120; p[3] = (unsigned char)cnt;
    for (int i = 0; i < cnt; ++i) {
        unsigned char *e = p + 4 + 4 * i;
        const unsigned age = g.frame - g.q[i].frame;
        e[0] = g.q[i].kind; e[1] = (unsigned char)(g.q[i].id & 0xff); e[2] = (unsigned char)(g.q[i].id >> 8);
        e[3] = (unsigned char)(age > 255 ? 255 : age);
    }
    const int len = (int)sizeof h + h.text_len;
    if (!comms_hub_send(d, len)) { ++g.send_fail; return; }
    g.tokens -= 1.0;
    ++g.sent_frames; g.sent_sounds += (unsigned long long)cnt;
    if (logon()) std::fprintf(stderr, "[msnd] send f=%u seq=%u n=%d lvl=%d area=%d\n", g.frame, (unsigned)h.msg_id, cnt,
                              (int)data_0209f2f8, (int)data_02092120);
    for (int i = cnt; i < g.nq; ++i) g.q[i - cnt] = g.q[i];
    g.nq -= cnt;
}

// ---- play -----------------------------------------------------------------------
int slot_of_seat(int seat) {
    const int n = data_0209f21c < kBodies ? data_0209f21c : kBodies;
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

void play_one(const Arrived &a) {
    const char *why = nullptr;
    double dist = -1.0, gain = 0.0, romlen = -1.0;
    int slot = -1;
    char *me = my_body();
    if (!in_play() || !me) { ++g.not_playing; why = "not-in-play"; }
    else if (a.level != (unsigned char)data_0209f2f8 || a.area != (unsigned char)data_02092120) { ++g.other_place; why = "other-level-or-area"; }
    else if ((slot = slot_of_seat(a.seat)) < 0) { ++g.no_body; why = "no-body"; }
    int src[3] = {0, 0, 0};
    if (!why && !port_adventure_ghost_drawpos(slot, src)) { ++g.no_body; why = "not-drawn"; }
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
            // the vector the ROM plays at: the friend's drawn spot in camera space (what
            // dActor_c::BeforeBehavior writes at +0x74 for a body that thinks), its length
            // set so the ROM's own fade gives the arena's near..far ramp from MY body
            int vec[3] = {0, 0, 0};
            MulVec3Mat4x3(src, data_0209b3ec, vec);
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
            {
                const double tx = (double)(vec[0] >> 12), ty = (double)(vec[1] >> 12) * 2.0, tz = (double)(vec[2] >> 12);
                romlen = std::sqrt(tx * tx + ty * ty + tz * tz);
            }
            ++g.played;
            if (logon())
                std::fprintf(stderr, "[msnd] play f=%u seat=%d slot=%d kind=%u id=%u dist=%.0f gain=%.2f romvec_len=%.0f vol=%d wav=%u\n",
                             g.frame, a.seat, slot, (unsigned)a.kind, (unsigned)a.id, dist, gain, romlen, out_volume_pct(),
                             sd_out_frames_rendered());
            _ZN5Sound4PlayEjjRK7Vector3((unsigned)a.kind, (unsigned)a.id, vec);
            return;
        }
    }
    if (logon())
        std::fprintf(stderr, "[msnd] skip f=%u seat=%d kind=%u id=%u dist=%.0f gain=%.2f lvl=%d/%d area=%d/%d why=%s wav=%u\n",
                     g.frame, a.seat, (unsigned)a.kind, (unsigned)a.id, dist, gain, (int)a.level, (int)data_0209f2f8,
                     (int)a.area, (int)data_02092120, why, sd_out_frames_rendered());
}

void report() {
    if (g.reported || !(g.cap || g.rx_frames)) return;
    g.reported = true;
    std::fprintf(stderr, "[msnd] exit captured=%llu sent_frames=%llu sent_sounds=%llu send_fail=%llu stale=%llu queue_full=%llu\n",
                 g.cap, g.sent_frames, g.sent_sounds, g.send_fail, g.stale, g.qfull);
    std::fprintf(stderr, "[msnd] exit rx_frames=%llu rx_sounds=%llu rx_bad=%llu rx_old=%llu rx_stranger=%llu played=%llu "
                 "far=%llu other_place=%llu no_body=%llu capped=%llu dup=%llu not_playing=%llu local_at_friend=%llu\n",
                 g.rx_frames, g.rx_sounds, g.rx_bad, g.rx_old, g.rx_stranger, g.played, g.too_far, g.other_place,
                 g.no_body, g.capped, g.dup, g.not_playing, g.ghost_local);
}

void rig() {
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
        std::atexit(report);
    }
    ++g.frame;
    rig();
    pump_send();
    for (int i = 0; i < g.nin; ++i) play_one(g.in[i]);
    g.nin = 0;
}
