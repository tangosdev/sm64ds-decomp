// THE ADVENTURE HUB CARRIER, implemented. Run mp1, lane HUB1.
// The contract, the env knobs and the wire table are in comms_hub.h; the design
// is runs mp1 NETARCH1 design.txt sections 1, 3 and 6.
//
// PORT_HOST_ABI: host plumbing. Nothing in src/ knows this file exists, and
// nothing here reads or writes a byte of game state: it moves datagrams that
// other port layers build (hal/comms_sync.cpp ghosts and identity, the chat
// overlay, hal/voice_chat.cpp) and answers who is in the room.
//
// ===========================================================================
// WHY A SEPARATE CARRIER AND NOT AN "ADVENTURE MODE" OF comms_loopback.cpp
// ===========================================================================
//
// Up to 0.5.3 an adventure console opened the VS carrier and rode its aux
// channel. That carrier is a DS transport: installing it seats the DS
// multiplayer role, the conductor waits for the session before it seats a
// world, and the round exchange runs underneath even though adventure never
// consumes a round. GHOSTSCOUT measured what that costs: a 4-player adventure
// parent stalled 615 frames and "rewound" its own SOLO world 42 times, because
// one slow console held the rounds everyone was exchanging for nothing.
//
// Adventure has no shared simulation, so it needs no clock. This carrier has
// no rounds at all. It is never handed to comms_set_transport(), so
// comms_transport() stays null, and every conductor path already answers
// "solo" for a null transport: no seat request, no world-seat wait, no pump,
// no fan-out. The ROM runs the single-player game it was always running.
//
// ===========================================================================
// THE I/O THREAD
// ===========================================================================
//
// One host thread owns the socket's receive side, the relay keepalive (HELLO
// every 1 s; the relay forgets a hub member after 10 s of quiet), the 2 Hz
// ping, and the presence sweep. Callers only copy in and out of queues under
// one lock. Two reasons it is a thread and not a per-frame poll:
//   * presence and the keepalive must not depend on the game's frame rate; a
//     console sitting in a long load or a menu must keep its seat;
//   * nothing is added to the frame loop, which keeps the only walk_window.cpp
//     change this lane makes to the one install call.
// The thread never touches game memory.
//
// ws2_32 IS LoadLibrary'd, never imported (comms_loopback.cpp's banner has the
// measurement: a static ws2_32 import loses the DS fixed ranges). This file
// keeps its own small table rather than reaching into the loopback carrier's,
// so neither file can change the other's load order.

#include "comms_hub.h"
#include "hitch_probe.h"   // HITCH9C: named costs for the hitch log

#include "host_settings.h"   // adventure_ghost_mode(), host_setting_voice_enabled

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>

#ifndef SIO_UDP_CONNRESET
#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace port {
namespace {

// ---------------------------------------------------------------------------
// ws2_32 by hand
// ---------------------------------------------------------------------------
struct HubWs {
    int    (WSAAPI *WSAStartup)(WORD, LPWSADATA);
    SOCKET (WSAAPI *socket)(int, int, int);
    int    (WSAAPI *WSAIoctl)(SOCKET, DWORD, LPVOID, DWORD, LPVOID, DWORD,
                              LPDWORD, LPWSAOVERLAPPED,
                              LPWSAOVERLAPPED_COMPLETION_ROUTINE);
    int    (WSAAPI *ioctlsocket)(SOCKET, long, u_long *);
    int    (WSAAPI *setsockopt)(SOCKET, int, int, const char *, int);
    int    (WSAAPI *bind)(SOCKET, const struct sockaddr *, int);
    int    (WSAAPI *sendto)(SOCKET, const char *, int, int,
                            const struct sockaddr *, int);
    int    (WSAAPI *recvfrom)(SOCKET, char *, int, int, struct sockaddr *,
                              int *);
    int    (WSAAPI *select)(int, fd_set *, fd_set *, fd_set *,
                            const struct timeval *);
    int    (WSAAPI *closesocket)(SOCKET);
    int    (WSAAPI *WSAGetLastError)(void);
    unsigned long (WSAAPI *inet_addr)(const char *);
    struct hostent *(WSAAPI *gethostbyname)(const char *);
    int    (WSAAPI *getsockname)(SOCKET, struct sockaddr *, int *);   // SEAMJOIN1
};
HubWs WS;

bool ws_up() {
    static int state = 0;   // 0 untried, 1 up, -1 refused
    if (state) return state > 0;
    state = -1;
    HMODULE m = ::LoadLibraryA("ws2_32.dll");
    if (!m) {
        std::fprintf(stderr, "[hub] ws2_32.dll would not load; adventure "
                     "online is off, the game plays solo\n");
        return false;
    }
    bool ok = true;
#define HUB_WS(n) \
    *(FARPROC *)&WS.n = ::GetProcAddress(m, #n); if (!WS.n) ok = false;
    HUB_WS(WSAStartup) HUB_WS(socket) HUB_WS(WSAIoctl) HUB_WS(ioctlsocket)
    HUB_WS(setsockopt) HUB_WS(bind) HUB_WS(sendto) HUB_WS(recvfrom)
    HUB_WS(select) HUB_WS(closesocket) HUB_WS(WSAGetLastError)
    HUB_WS(inet_addr) HUB_WS(gethostbyname) HUB_WS(getsockname)
#undef HUB_WS
    if (!ok) {
        std::fprintf(stderr, "[hub] ws2_32.dll is missing an entry point; "
                     "adventure online is off\n");
        return false;
    }
    WSADATA wsa;
    if (WS.WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::fprintf(stderr, "[hub] WSAStartup failed; adventure online is "
                     "off\n");
        return false;
    }
    state = 1;   // never WSACleanup'd: process exit does it, and the VS
                 // carrier may share Winsock in the same process
    return true;
}

inline unsigned short hton16(unsigned short v) {
    return (unsigned short)((v >> 8) | (v << 8));
}

// Milliseconds from the performance counter: GetTickCount steps in ~16 ms,
// which is a whole ghost frame and too coarse for an RTT readout.
unsigned now_ms() {
    static LARGE_INTEGER f = {};
    if (!f.QuadPart) ::QueryPerformanceFrequency(&f);
    LARGE_INTEGER c;
    ::QueryPerformanceCounter(&c);
    return (unsigned)((unsigned long long)c.QuadPart * 1000ull /
                      (unsigned long long)f.QuadPart);
}

int str_int(const char *name, const char *s, int def, int lo, int hi) {
    if (!s || !*s) return def;
    char *end = nullptr;
    const long v = std::strtol(s, &end, 10);
    if (end == s || v < lo || v > hi) {
        std::fprintf(stderr, "[hub] %s=%s is out of range %d..%d; using %d\n",
                     name, s, lo, hi, def);
        return def;
    }
    return (int)v;
}

int env_int(const char *name, int def, int lo, int hi) {
    return str_int(name, std::getenv(name), def, lo, hi);
}

// SEAMHUB1: a session record field, parsed like env_int
int sess_int(OnlineSessionField f, int def, int lo, int hi) {
    return str_int(online_session_name(f), online_session_get(f), def, lo, hi);
}

bool parse_host_port(const char *s, int default_port, sockaddr_in *out) {
    char host[256];
    int port = default_port;
    const char *colon = std::strrchr(s, ':');
    const size_t hlen = colon ? (size_t)(colon - s) : std::strlen(s);
    if (hlen == 0 || hlen >= sizeof host) return false;
    std::memcpy(host, s, hlen);
    host[hlen] = 0;
    if (colon) {
        port = std::atoi(colon + 1);
        if (port <= 0 || port > 65535) return false;
    }
    std::memset(out, 0, sizeof *out);
    out->sin_family = AF_INET;
    out->sin_port = hton16((unsigned short)port);
    const unsigned long v = WS.inet_addr(host);
    if (v != INADDR_NONE) { out->sin_addr.s_addr = v; return true; }
    struct hostent *he = WS.gethostbyname(host);
    if (he && he->h_addrtype == AF_INET && he->h_addr_list &&
        he->h_addr_list[0]) {
        std::memcpy(&out->sin_addr.s_addr, he->h_addr_list[0], 4);
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// THE KIND TABLE. One validator for both directions, so the hub can never
// send what it would refuse to receive (or what relay v2 would drop).
// ---------------------------------------------------------------------------
enum : int { kKindPing = kHubKinds, kKindPong = kHubKinds + 1,
             kKindRefPing = kHubKinds + 2,   // arena: 'SYNP' v2, 20 bytes
             kKindRefPong = kHubKinds + 3,   // arena: 'SYNQ' v2, 20 bytes
             kKindNone = -1 };

enum Why : int { kOk = 0, kBadTag, kBadLen, kBadVersion, kBadField };

Why classify(const unsigned char *p, int len, int *kind) {
    *kind = kKindNone;
    if (len < kHubEnvelopeBytes) return kBadTag;
    unsigned tag;
    std::memcpy(&tag, p, 4);
    switch (tag) {
    case kHubTagGhost:
        *kind = kHubGhost;
        if (len != (int)sizeof(HubGhostFrame)) return kBadLen;
        if (p[4] != 1) return kBadVersion;
        return kOk;
    case kHubTagIdent: {
        *kind = kHubIdent;
        if (len < 24 || len > 64 || (len & 3)) return kBadLen;
        if (p[4] != 1) return kBadVersion;
        const int name_len = p[6];
        if (name_len > kHubIdentNameMax ||
            (int)sizeof(HubIdentHead) + name_len > len) return kBadLen;
        return kOk;
    }
    case kHubTagChat: {
        *kind = kHubChat;
        if (len < 12 || len > 12 + kHubChatTextMax) return kBadLen;
        if (p[4] != 1) return kBadVersion;
        const int text_len = p[7];
        // design.txt 6.3 addendum (run mp1, lane BUBBLE1): the TYPING FRAME is
        // the bare 12-byte head with flags bit0 set and text_len 0. The chat
        // layer (hal/adv_chat.cpp) checks its flags, state and sequence.
        if (text_len == 0 && len == (int)sizeof(HubChatHead) && (p[6] & 1))
            return kOk;
        if (text_len < 1 || text_len > kHubChatTextMax ||
            (int)sizeof(HubChatHead) + text_len != len) return kBadLen;
        return kOk;
    }
    case kHubTagVoice:
        *kind = kHubVoice;
        if (len != 176 && len != 340) return kBadLen;
        if (p[4] != 1) return kBadVersion;
        return kOk;
    case kHubTagPing:
        *kind = kKindPing;
        if (len == 20) {                     // arena: the referee clock ping
            *kind = kKindRefPing;
            return p[4] == 2 ? kOk : kBadVersion;
        }
        if (len != 12) return kBadLen;
        if (p[4] != 0) return kBadVersion;   // SyncPingV1.kind = ping
        return kOk;
    case kHubTagPong:
        *kind = kKindPong;
        if (len == 20) {                     // arena: the referee clock pong
            *kind = kKindRefPong;
            return p[4] == 2 ? kOk : kBadVersion;
        }
        if (len != 12) return kBadLen;
        if (p[4] != 1) return kBadVersion;   // SyncPingV1.kind = pong
        return kOk;
    // THE ARENA KINDS (lane ARENACORE1). Exact lengths, version 1; which shape
    // may send or receive them is shape_ok()'s business, below.
    case kHubTagArenaState:
        *kind = kHubArenaState;
        if (len != 48) return kBadLen;
        return p[4] == 1 ? kOk : kBadVersion;
    case kHubTagArenaObject:
        *kind = kHubArenaObject;
        if (len != 32) return kBadLen;
        return p[4] == 1 ? kOk : kBadVersion;
    case kHubTagArenaRef:
        *kind = kHubArenaRef;
        if (len != 272) return kBadLen;
        return p[4] == 1 ? kOk : kBadVersion;
    case kHubTagArenaClaim:
        *kind = kHubArenaClaim;
        if (len != 24) return kBadLen;
        return p[4] == 1 ? kOk : kBadVersion;
    // THE PARTY / COOP KINDS (lane WIRE1; layouts and rules in
    // hal/hub_presence.h, the same checks the relay runs). Which ROOM may
    // send or take them is shape_ok()'s business, below.
    case kHubTagPresence:
        *kind = kHubPresence;
        if (len != (int)sizeof(HubPresenceWire)) return kBadLen;
        if (p[4] != 1) return kBadVersion;
        return hub_presence_validate(p, len) ? kOk : kBadField;
    case kHubTagPartyStar:
        *kind = kHubPartyStar;
        if (len != (int)sizeof(HubPartyStarWire)) return kBadLen;
        if (p[4] != 1) return kBadVersion;
        return hub_party_star_validate(p, len) ? kOk : kBadField;
    case kHubTagPartyHit:
        *kind = kHubPartyHit;
        if (len != (int)sizeof(HubPartyHitWire)) return kBadLen;
        if (p[4] != 1) return kBadVersion;
        return hub_party_hit_validate(p, len) ? kOk : kBadField;
    case kHubTagEvent:
        *kind = kHubEvent;
        if (len != 96) return kBadLen;
        if (p[4] != 1) return kBadVersion;
        return (p[6] >= 1 && p[6] <= 6 && p[22] <= 70) ? kOk : kBadField;
    case kHubTagCoopWorld:
    case kHubTagCoopCtl:
        *kind = tag == kHubTagCoopWorld ? kHubCoopWorld : kHubCoopCtl;
        if (len < kHubCoopMinDatagram || len > kHubCoopMaxDatagram)
            return kBadLen;
        if (p[4] != 1) return kBadVersion;
        return hub_coop_validate(tag, p, len) ? kOk : kBadField;
    default:
        return kBadTag;
    }
}

// WHICH SHAPE OWNS WHICH KIND. The adventure hub never sends or takes an
// arena kind and an arena console never sends or takes a ghost frame ('SYNG'
// is adventure's). Within the arena: 'SYNR' and the 20-byte referee pong come
// only from the referee, and a console never sends them; a claim 'SYNE' and
// the 20-byte referee ping only go up, so receiving one is unexpected.
bool arena_kind(int kind) {
    return kind == kHubArenaState || kind == kHubArenaObject ||
           kind == kHubArenaRef || kind == kHubArenaClaim ||
           kind == kKindRefPing || kind == kKindRefPong;
}
// THE ROOM GATES (lane WIRE1), the relay's own table: presence in world,
// party and coop rooms; the party star in party and world rooms, the hit in party rooms only; the coop
// world and control kinds in coop rooms only; none of them in an arena.
bool room_kind_ok(int kind, bool arena, int room) {
    switch (kind) {
    case kHubPresence: return !arena;
    case kHubPartyStar:   // WPARTY2: also in a world room (the relay forwards SYNX to hub sessions); the
                          // receiver only takes it from a world-party member
        return !arena && (room == kHubRoomParty || room == kHubRoomWorld);
    case kHubPartyHit: return !arena && room == kHubRoomParty;
    case kHubCoopWorld:
    case kHubCoopCtl: return !arena && room == kHubRoomCoop;
    case kHubEvent: return !arena && room == kHubRoomWorld;
    }
    return true;
}
bool shape_ok_send(int kind, bool arena, int room) {
    if (kind == kHubEvent) return false;      // the relay speaks it; a console never does
    if (kind == kHubGhost) return !arena;
    if (kind == kHubArenaRef || kind == kKindRefPong) return false;
    if (arena_kind(kind)) return arena;
    return room_kind_ok(kind, arena, room);
}
bool shape_ok_recv(int kind, bool arena, bool from_referee, int room) {
    if (kind == kHubGhost) return !arena;
    if (kind == kHubArenaRef || kind == kKindRefPong)
        return arena && from_referee;
    if (kind == kHubArenaClaim || kind == kKindRefPing) return false;
    if (arena_kind(kind)) return arena;
    return room_kind_ok(kind, arena, room);
}

// ---------------------------------------------------------------------------
// STATE. Everything below g_lock is guarded by it.
// ---------------------------------------------------------------------------
enum : int { kModeNone = 0, kModeRelay = 1, kModeMesh = 2 };

// ghost, ident, chat, voice; then the arena's state, object, ref, claim (the
// claim queue is never filled: claims only go up)
// then presence, party star, party hit, coop world, coop control
// MOVERS9: the coop world kind (index 11) holds ONE-SHOT events (deaths, spawns, stars, switches), so it is 128 deep;
// the mover batches (SYNW sub-type 0x10, 10 Hz state that the next batch replaces) go to their own 48-deep queue, so a
// load or a star select can no longer evict a one-shot event with a burst of poses.
const int kFifoDepth[kHubKinds] = { 64, 32, 32, 32, 64, 64, 32, 1,
                                    16, 16, 16, 128, 32, 16 };
enum : int { kFifoMaxDepth = 128, kMoverFifoDepth = 48, kMoverSubtype = 0x10 };

struct Msg {
    unsigned t;        // arrival ms
    int len;
    int seat;
    unsigned char b[kHubMaxDatagram];
};
struct Fifo {
    Msg slot[kFifoMaxDepth];
    int head, count;
};
struct Seat {
    bool present;
    unsigned join_epoch;
    unsigned heard;               // ms of the newest valid datagram
    unsigned ghost_t;
    bool has_ghost;
    unsigned short ghost_seq;
    int rtt_ms;
    int latest_len[kHubKinds];
    unsigned latest_t[kHubKinds];
    unsigned char latest[kHubKinds][kHubMaxDatagram];
};

SRWLOCK g_lock = SRWLOCK_INIT;
bool g_installed = false;
bool g_arena = false;       // installed in the arena shape (lane ARENACORE1)
int g_room = kHubRoomWorld; // world / party / coop (lane WIRE1); the HELLO role
struct KindHandler { HubKindFn fn; void *user; };
KindHandler g_handler[kHubKinds];      // hub_on_kind (game thread registers)
int g_mode = kModeNone;
int g_state = kHubOff;
int g_seat = -1;
SOCKET g_sock = INVALID_SOCKET;
sockaddr_in g_relay = {};
int g_mesh_base = 0;
unsigned char g_code[8] = {};
int g_want_seat = 0xFF;
unsigned g_seat_taken_since = 0;   // arena: when the first status 4 came (0 = none)
const unsigned kArenaSeatRetryMs = 6000;   // how long an arena pin is kept against status 4
bool g_ever_paired = false;
bool g_exit_full = false;   // set under the lock, acted on outside it
volatile LONG g_stop = 0;   // process exit: the I/O thread returns
HANDLE g_thread = nullptr;
unsigned g_last_ack = 0;
unsigned g_roster_version = 0;
int g_ping_rr = 0;          // the seat the last ping asked to answer
Fifo g_fifo[kHubKinds];
Fifo g_mfifo;                                 // MOVERS9: coop mover batches
unsigned long long g_qdrop[kHubKinds + 1];    // MOVERS9: queue evictions per kind; the last slot = the mover queue
Seat g_seats[kHubMaxSeats];
CommsHubStats g_st = {};
unsigned long long g_drop_stale = 0;
unsigned long long g_drop_field = 0, g_dispatched = 0;

// THE REFEREE CLOCK (arena, relay mode; plan 4.2, arena_wire.h RefPing /
// RefPong). Eight newest pongs; the estimate is the minimum-RTT one.
struct RefSample { bool used; int rtt; unsigned t_ref; unsigned recv; };
enum : int { kRefSamples = 8 };
RefSample g_ref[kRefSamples];
int g_ref_next = 0;
unsigned g_ref_last_t_ref = 0;     // echoed in the next ping
unsigned g_ref_last_recv = 0;
bool g_ref_have = false;
int g_ref_cap_ms = -1;

// test knobs
int g_delay_ms = 0, g_loss_pct = 0, g_report_s = 10;
unsigned g_rng = 0x2545F491u;

// REJOIN1 (run coopfix1): SM64DS_HUB_BLACKOUT=<at_ms>:<for_ms>, off by default. From at_ms after the install, for for_ms,
// this console neither sends nor takes a datagram (a pulled cable): the proof rows of a dropped connection. Up to four
// windows, separated by ';' or ','.
enum : int { kBlackoutMax = 4 };
unsigned g_bo_from[kBlackoutMax], g_bo_to[kBlackoutMax];
int g_bo_n = 0;
unsigned g_install_ms = 0;
LONG g_bo_on = 0;                   // the I/O thread's view, for the transition lines
volatile LONG g_bo_dropped = 0;     // sends dropped by the blackout
bool blackout_now(unsigned now) {
    if (!g_bo_n) return false;
    const unsigned t = now - g_install_ms;
    for (int i = 0; i < g_bo_n; ++i)
        if (t >= g_bo_from[i] && t < g_bo_to[i]) return true;
    return false;
}
void blackout_parse() {
    g_bo_n = 0;
    const char *e = std::getenv("SM64DS_HUB_BLACKOUT");
    if (!e || !*e) return;
    const char *q = e;
    while (*q && g_bo_n < kBlackoutMax) {
        char *end = nullptr;
        const long at = std::strtol(q, &end, 10);
        if (!end || *end != ':') break;
        const long len = std::strtol(end + 1, &end, 10);
        if (at >= 0 && len > 0) {
            g_bo_from[g_bo_n] = (unsigned)at;
            g_bo_to[g_bo_n] = (unsigned)(at + len);
            std::fprintf(stderr, "[hub] test: blackout %d from %ld ms for %ld ms after the install\n", g_bo_n, at, len);
            ++g_bo_n;
        }
        q = end;
        while (*q == ';' || *q == ',' || *q == ' ') ++q;
    }
}

// REJOIN1: SM64DS_COOP_REJOIN=0 is the old behaviour (the before rows): a coop hub re-pairs for any seat, and a guest
// ends the room 1.5 s after the host's seat goes quiet. Default on.
bool coop_rejoin_on() {
    static int v = -1;
    if (v < 0) { const char *e = std::getenv("SM64DS_COOP_REJOIN"); v = (e && *e == '0') ? 0 : 1; }
    return v != 0;
}
const unsigned kCoopSeatRetryMs = 12000;   // REJOIN1: the relay frees a silent coop seat after 10 s (relay.py hub idle)

struct Held { unsigned due; int len; sockaddr_in from; unsigned char b[kHubMaxDatagram]; };
enum : int { kHeldRing = 512 };
Held g_held[kHeldRing];   // I/O thread only
int g_held_head = 0, g_held_count = 0;

struct Lock {
    Lock() { ::AcquireSRWLockExclusive(&g_lock); }
    ~Lock() { ::ReleaseSRWLockExclusive(&g_lock); }
};

const char *state_name(int s) {
    switch (s) {
    case kHubOff: return "off";
    case kHubPairing: return "pairing";
    case kHubLive: return "live";
    case kHubRelayTooOld: return "relay-too-old";
    case kHubRefused: return "refused";
    case kHubFull: return "full";
    case kHubFailed: return "failed";
    }
    return "?";
}

// ---------------------------------------------------------------------------
// SENDING (any thread). Destinations are copied under the lock; the sendto
// calls happen outside it.
// ---------------------------------------------------------------------------
void raw_send_to(const unsigned char *p, int len, const sockaddr_in &to) {
    if (g_bo_n && blackout_now(now_ms())) { ::InterlockedIncrement(&g_bo_dropped); return; }   // REJOIN1 test knob
    WS.sendto(g_sock, (const char *)p, len, 0, (const sockaddr *)&to,
              (int)sizeof to);
}

sockaddr_in mesh_addr(int seat) {
    sockaddr_in a = {};
    a.sin_family = AF_INET;
    a.sin_port = hton16((unsigned short)(g_mesh_base + seat));
    a.sin_addr.s_addr = 0x0100007Fu;   // 127.0.0.1, network order
    return a;
}

// to_seat < 0: everyone else. Caller holds no lock.
void send_out(const unsigned char *p, int len, int to_seat, int mode, int me,
              const sockaddr_in &relay) {
    if (mode == kModeRelay) {
        raw_send_to(p, len, relay);   // the relay routes (pong: byte 6)
        return;
    }
    if (to_seat >= 0) { raw_send_to(p, len, mesh_addr(to_seat)); return; }
    for (int k = 0; k < kHubMaxSeats; ++k)
        if (k != me) raw_send_to(p, len, mesh_addr(k));
}

void send_hello() {
    // role 2 = hub member (adventure), role 3 = arena member (arena_wire.h
    // kRoleArena: the relay's "arena" session, the referee's)
    unsigned char h[16] = { 'S', 'M', 'R', 'C', 2, 2, 0, 0 };
    if (g_arena) h[5] = 3;
    else if (g_room == kHubRoomParty) h[5] = 4;   // lane WIRE1: party room
    else if (g_room == kHubRoomCoop) h[5] = 5;    // lane WIRE1: coop room
    h[6] = (unsigned char)g_want_seat;
    h[7] = host_setting_voice_enabled() ? 1 : 0;
    std::memcpy(h + 8, g_code, 8);
    raw_send_to(h, 16, g_relay);
    ++g_st.hellos;
}

// THE PING ASKS ONE SEAT TO ANSWER, in rotation (byte 6; 0xFF = nobody).
// Every ping still reaches everyone (it is the presence heartbeat), but only
// the named seat echoes it. An echo-from-everyone ping is fine at 2 players
// and breaks at 16: fifteen peers pinging at 2 Hz would make every seat send
// 30 pongs a second, five times relay v2's 6-per-second pong cap, so most RTT
// samples would die at the relay. Rotating keeps each seat near 2 pongs a
// second at any room size; a seat's RTT refreshes every (peers / 2) seconds.
void send_ping(int me, int mode, const sockaddr_in &relay, int ask) {
    unsigned char p[12] = { 'S', 'Y', 'N', 'P', 0, 0, 0xFF, 0 };
    p[5] = (unsigned char)me;
    p[6] = (unsigned char)(ask >= 0 ? ask : 0xFF);
    const unsigned t = now_ms();
    std::memcpy(p + 8, &t, 4);
    send_out(p, 12, -1, mode, me, relay);
}

// THE REFEREE CLOCK PING (arena, relay mode): 'SYNP' v2, 20 bytes, byte 6 =
// 0xFE, which the relay answers itself (arena_wire.h RefPing). It carries the
// newest pong's referee time back and how long I held it, so the referee can
// measure my round trip too. I/O thread only; the caller holds no lock.
void send_ref_ping(const sockaddr_in &relay, int me) {
    unsigned char p[20] = { 'S', 'Y', 'N', 'P', 2, 0, (unsigned char)kHubRefereeSeat, 0 };
    p[5] = (unsigned char)me;
    const unsigned t = now_ms();
    unsigned echo = 0, held = 0xFFFFu;
    {
        Lock l;
        if (g_ref_have) {
            echo = g_ref_last_t_ref;
            const unsigned h = t - g_ref_last_recv;
            held = h < 0xFFFFu ? h : 0xFFFEu;
        }
        ++g_st.ref_pings;
    }
    const unsigned short held16 = (unsigned short)held;
    std::memcpy(p + 8, &t, 4);
    std::memcpy(p + 12, &echo, 4);
    std::memcpy(p + 16, &held16, 2);
    raw_send_to(p, 20, relay);
}

// ---------------------------------------------------------------------------
// RECEIVING (I/O thread)
// ---------------------------------------------------------------------------
void seat_join(int s, unsigned now) {
    Seat &z = g_seats[s];
    z.present = true;
    ++z.join_epoch;
    z.heard = now;
    z.has_ghost = false;
    z.rtt_ms = -1;
    for (int k = 0; k < kHubKinds; ++k) z.latest_len[k] = 0;
    ++g_roster_version;
    ++g_st.joins;
    std::fprintf(stderr, "[hub] seat %d joined (epoch %u, roster %u)\n", s,
                 z.join_epoch, g_roster_version);
}

void seat_leave(int s, const char *why) {
    Seat &z = g_seats[s];
    z.present = false;
    z.has_ghost = false;
    for (int k = 0; k < kHubKinds; ++k) z.latest_len[k] = 0;
    ++g_roster_version;
    ++g_st.leaves;
    std::fprintf(stderr, "[hub] seat %d left (%s, roster %u)\n", s, why,
                 g_roster_version);
}

void fifo_push(int kind, const unsigned char *p, int len, int seat,
               unsigned now) {
    // MOVERS9: a mover batch (coop world, sub-type 0x10) has its own queue and its own depth
    static int split = -1;             // SM64DS_HUB_SPLIT=0 is the MUST-FAIL control: one shared 32-deep world queue (the r3 behaviour)
    if (split < 0) { const char *e = std::getenv("SM64DS_HUB_SPLIT"); split = (e && *e == '0') ? 0 : 1; }
    const bool mover = split && kind == kHubCoopWorld && len > 6 && p[6] == kMoverSubtype;
    Fifo &f = mover ? g_mfifo : g_fifo[kind];
    const int depth = mover ? (int)kMoverFifoDepth : (kind == kHubCoopWorld && !split ? 32 : kFifoDepth[kind]);
    if (f.count == depth) {            // newest wins: drop the oldest
        f.head = (f.head + 1) % depth;
        --f.count;
        ++g_st.drop_queue;
        ++g_qdrop[mover ? kHubKinds : kind];
    }
    Msg &m = f.slot[(f.head + f.count) % depth];
    m.t = now;
    m.len = len;
    m.seat = seat;
    std::memcpy(m.b, p, (size_t)len);
    ++f.count;
}

// A datagram that passed the source check; `seat` is the stamped sender.
void accept_datagram(unsigned char *p, int len, int seat, unsigned now,
                     int *pong_to, unsigned char *pong) {
    int kind;
    const Why w = classify(p, len, &kind);
    if (w == kBadTag) { ++g_st.drop_tag; return; }
    if (w == kBadLen) { ++g_st.drop_len; return; }
    if (w == kBadVersion) { ++g_st.drop_version; return; }
    if (w == kBadField) { ++g_drop_field; return; }
    const bool from_ref = (g_mode == kModeRelay && seat == kHubRefereeSeat);
    // HOST EVENTS (lane EVENTS1): 'SYNZ' is the relay's own and nothing else
    // is ever taken from seat 0xFD. A member cannot forge one: the relay
    // stamps byte 5 with the member's seat, which is never 0xFD (seats are
    // 0..31), and a console that is not in relay mode never takes one at all.
    if (seat == kHubEventSeat && g_mode == kModeRelay) {
        if (kind != kHubEvent || !shape_ok_recv(kind, g_arena, false, g_room)) {
            ++g_st.drop_shape;
            return;
        }
        ++g_st.recvd[kind];
        fifo_push(kind, p, len, kHubEventSeat, now);
        return;
    }
    if (kind == kHubEvent) { ++g_st.drop_shape; return; }
    if (!shape_ok_recv(kind, g_arena, from_ref, g_room)) {
        ++g_st.drop_shape;
        return;
    }
    if (from_ref) {
        // THE REFEREE (arena): only its event frames and its clock pong are
        // expected from 0xFE; it is not a seat and never joins the roster.
        if (kind == kKindRefPong) {
            if (p[6] != (unsigned char)g_seat) { ++g_st.drop_seat; return; }
            unsigned t_cons, t_ref;
            unsigned short cap;
            std::memcpy(&t_cons, p + 8, 4);
            std::memcpy(&t_ref, p + 12, 4);
            std::memcpy(&cap, p + 18, 2);
            const unsigned rtt = now - t_cons;
            if (rtt >= 10000u) { ++g_drop_stale; return; }
            RefSample &r = g_ref[g_ref_next];
            g_ref_next = (g_ref_next + 1) % kRefSamples;
            r.used = true;
            r.rtt = (int)rtt;
            r.t_ref = t_ref;
            r.recv = now;
            g_ref_last_t_ref = t_ref;
            g_ref_last_recv = now;
            g_ref_have = true;
            g_ref_cap_ms = cap == 0xFFFFu ? -1 : (int)cap;
            ++g_st.ref_pongs;
            return;
        }
        if (kind != kHubArenaRef) { ++g_st.drop_seat; return; }
        ++g_st.recvd[kind];
        fifo_push(kind, p, len, kHubRefereeSeat, now);
        return;
    }
    if (seat < 0 || seat >= kHubMaxSeats) { ++g_st.drop_seat; return; }
    // a party / coop room has seats 0..3 only (the relay refuses more; the
    // test mesh is held to the same rule)
    if (g_room != kHubRoomWorld && seat >= kHubPartySeats) {
        ++g_st.drop_seat;
        return;
    }
    if (seat == g_seat) { ++g_st.drop_self; return; }
    p[5] = (unsigned char)seat;   // the carrier's stamp is the only truth
    if (kind == kHubPartyHit &&
        (p[6] != (unsigned char)g_seat || p[6] == (unsigned char)seat)) {
        // a hit is for ONE seat: mine. (The relay routes it by byte 6; the
        // mesh sends it to the victim only. Anything else is a forgery or a
        // routing error and is not delivered.)
        ++g_st.drop_seat;
        return;
    }

    Seat &z = g_seats[seat];
    if (!z.present) seat_join(seat, now);
    z.heard = now;

    if (kind == kKindPing) {
        if (p[6] != (unsigned char)g_seat) return;   // not asked this time
        // Echo to the pinger only: 'SYNQ', kind 1, from me, to it, its time.
        std::memcpy(pong, p, 12);
        pong[3] = 'Q';
        pong[4] = 1;
        pong[5] = (unsigned char)g_seat;
        pong[6] = (unsigned char)seat;
        *pong_to = seat;
        return;
    }
    if (kind == kKindPong) {
        if (p[6] != (unsigned char)g_seat) { ++g_st.drop_seat; return; }
        unsigned t;
        std::memcpy(&t, p + 8, 4);
        const unsigned rtt = now - t;
        if (rtt < 60000u) z.rtt_ms = (int)rtt;
        ++g_st.pongs_recvd;
        return;
    }
    if (kind == kHubGhost) {
        unsigned short seq;
        std::memcpy(&seq, p + 8, 2);
        // Late or duplicate: newest wins. Only within 16 frames (about 1 s at
        // 15 Hz, far past any real reordering), though: a console that
        // restarts and lands on the same seat before its old presence lapses
        // starts again from seq 0, and a plain wrap compare would hold every
        // frame it sends as "stale" until it passed the old count (measured:
        // a restart 4 s in lost its first 52 frames with a 64-frame window).
        // A jump back past the window is a new sender.
        const short d = (short)(seq - z.ghost_seq);
        if (z.has_ghost && d <= 0 && d > -16) {
            ++g_drop_stale;
            return;
        }
        z.has_ghost = true;
        z.ghost_seq = seq;
        z.ghost_t = now;
    }
    std::memcpy(z.latest[kind], p, (size_t)len);
    z.latest_len[kind] = len;
    z.latest_t[kind] = now;
    ++g_st.recvd[kind];
    fifo_push(kind, p, len, seat, now);
}

// Returns the stamped seat, or -1 when the source is not one we expect.
int source_seat(const unsigned char *p, int len, const sockaddr_in &from) {
    if (g_mode == kModeRelay) {
        if (from.sin_addr.s_addr != g_relay.sin_addr.s_addr ||
            from.sin_port != g_relay.sin_port) return -1;
        return len > 5 ? p[5] : -1;   // relay v2 stamped it
    }
    if (from.sin_addr.s_addr != 0x0100007Fu) return -1;
    const int port = hton16(from.sin_port);
    const int s = port - g_mesh_base;
    return (s >= 0 && s < kHubMaxSeats) ? s : -1;
}

void on_ack(const unsigned char *p, unsigned now) {
    ++g_st.acks;
    if (std::memcmp(p + 8, g_code, 8) != 0) return;   // not our code
    const int ver = p[4], status = p[5];
    if (ver != 2) {
        if (g_state != kHubRelayTooOld)
            std::fprintf(stderr, "[hub] the relay answered as version %d: "
                         "adventure online needs relay v2. Playing solo; the "
                         "hub keeps asking once a second.\n", ver);
        g_state = kHubRelayTooOld;
        return;
    }
    switch (status) {
    case 0: {
        const int seat = p[6];
        if (seat >= kHubMaxSeats) { ++g_st.drop_seat; return; }
        if (g_state != kHubLive || seat != g_seat)
            std::fprintf(stderr, "[hub] paired: relay v2 hub, seat %d%s\n",
                         seat, g_ever_paired ? " (re-paired)" : "");
        if (g_seat >= 0 && seat != g_seat && g_seats[seat].present)
            seat_leave(seat, "that seat is mine now");
        g_seat_taken_since = 0;
        g_seat = seat;
        g_state = kHubLive;
        g_ever_paired = true;
        g_last_ack = now;
        return;
    }
    case 1:
        if (!g_ever_paired) {
            g_exit_full = true;
            return;
        }
        if (g_state != kHubFull)
            std::fprintf(stderr, "[hub] the room filled while re-pairing; "
                         "playing solo and asking again once a second\n");
        g_state = kHubFull;
        return;
    case 4: {
        // ARENA: the seat is pinned to a body, and the relay holds a console's
        // seat until it has been silent 3 s (relay.py SEAT_RECLAIM_S, HELLO role 3). A
        // console that comes back faster than that (a restart is usually 4 s)
        // is refused with 4 a few times and is given its own seat as soon as
        // the old one times out, so
        // keep the pin and ask again (the HELLO repeats once a second). Bounded:
        // a seat that is still taken after kArenaSeatRetryMs is a live console,
        // and the old behaviour (ask for any free seat) takes over. Lockstep
        // and adventure hubs never keep the pin.
        const bool coop_pin = !g_arena && g_room == kHubRoomCoop && coop_rejoin_on();   // REJOIN1: my old coop seat
        if ((g_arena || coop_pin) && g_want_seat != 0xFF) {
            const unsigned retry = coop_pin ? kCoopSeatRetryMs : kArenaSeatRetryMs;
            if (g_seat_taken_since == 0) {
                g_seat_taken_since = now ? now : 1;
                std::fprintf(stderr, "[hub] seat %d is still held by the "
                             "console that was here; asking again for it "
                             "(up to %u ms)\n", g_want_seat,
                             retry);
                return;
            }
            if (now - g_seat_taken_since < retry) return;
            std::fprintf(stderr, "[hub] seat %d is held by a live console; "
                         "asking for any free seat\n", g_want_seat);
            g_want_seat = 0xFF;
            g_seat_taken_since = 0;
            return;
        }
        std::fprintf(stderr, "[hub] seat %d is taken; asking for any free "
                     "seat\n", g_want_seat);
        g_want_seat = 0xFF;
        return;
    }
    default:
        if (g_state != kHubRefused) {
            if (g_arena)
                std::fprintf(stderr, "[hub] the relay refused the ARENA HELLO "
                             "(status %d: %s). No arena match without the "
                             "relay; asking again every 5 s.\n", status,
                             status == 2 ? "this relay predates arena mode"
                             : status == 3 ? "this code is a v1 session"
                             : status == 5 ? "this code is a hub or star "
                                             "session"
                             : "refused");
            else
                std::fprintf(stderr, "[hub] the relay refused the hub HELLO "
                             "(status %d: %s). Playing solo.\n", status,
                             status == 3 ? "this code is a v1 session"
                             : status == 5 ? "this code is a VS session or "
                                             "another kind of room"
                             : status == 2 && g_room != kHubRoomWorld
                                 ? "this relay predates party and coop rooms"
                                 : "bad request");
        }
        g_state = kHubRefused;
        return;
    }
}

void presence_sweep(unsigned now) {
    for (int s = 0; s < kHubMaxSeats; ++s)
        if (g_seats[s].present && now - g_seats[s].heard > kHubPresenceMs)
            seat_leave(s, "unheard for 3 s");
}

void report_locked(const char *tag) {
    const unsigned now = now_ms();
    // THE ARENA TAIL: the four arena kinds and the referee clock, only on an
    // arena console, so the adventure line keeps its exact shape.
    char arena_tail[200] = "";
    if (g_arena) {
        int best = -1;
        for (int i = 0; i < kRefSamples; ++i)
            if (g_ref[i].used && (best < 0 || g_ref[i].rtt < g_ref[best].rtt))
                best = i;
        std::snprintf(arena_tail, sizeof arena_tail,
                      " arena sent s/o/e=%llu/%llu/%llu recvd s/o/r=%llu/%llu/"
                      "%llu shape=%llu ref ping=%llu pong=%llu rtt=%d cap=%d",
                      g_st.sent[kHubArenaState], g_st.sent[kHubArenaObject],
                      g_st.sent[kHubArenaClaim], g_st.recvd[kHubArenaState],
                      g_st.recvd[kHubArenaObject], g_st.recvd[kHubArenaRef],
                      g_st.drop_shape, g_st.ref_pings, g_st.ref_pongs,
                      best >= 0 ? g_ref[best].rtt : -1, g_ref_cap_ms);
    }
    // THE ROOM TAIL (lane WIRE1): only in a party or coop room, so the world
    // and arena lines keep their exact shape.
    char room_tail[200] = "";
    if (g_room != kHubRoomWorld)
        std::snprintf(room_tail, sizeof room_tail,
                      " room=%s sent L/X/H/W/B=%llu/%llu/%llu/%llu/%llu "
                      "recvd=%llu/%llu/%llu/%llu/%llu field=%llu disp=%llu",
                      g_room == kHubRoomParty ? "party" : "coop",
                      g_st.sent[kHubPresence], g_st.sent[kHubPartyStar],
                      g_st.sent[kHubPartyHit], g_st.sent[kHubCoopWorld],
                      g_st.sent[kHubCoopCtl], g_st.recvd[kHubPresence],
                      g_st.recvd[kHubPartyStar], g_st.recvd[kHubPartyHit],
                      g_st.recvd[kHubCoopWorld], g_st.recvd[kHubCoopCtl],
                      g_drop_field, g_dispatched);
    unsigned mask = 0;
    int n = 0;
    for (int s = 0; s < kHubMaxSeats; ++s)
        if (g_seats[s].present) { mask |= 1u << s; ++n; }
    char rtt[16 * 8] = "";
    int o = 0;
    for (int s = 0; s < kHubMaxSeats && o < (int)sizeof rtt - 8; ++s)
        if (g_seats[s].present)
            o += std::snprintf(rtt + o, sizeof rtt - (size_t)o, "%s%d:%d",
                               o ? "," : "", s, g_seats[s].rtt_ms);
    std::fprintf(stderr,
                 "[hub] %s t=%u mode=%s state=%s seat=%d mask=0x%04x peers=%d "
                 "sent g/i/t/v=%llu/%llu/%llu/%llu recvd=%llu/%llu/%llu/%llu "
                 "ping=%llu pong=%llu/%llu rtt=[%s] refused=%llu "
                 "drop tag/len/ver/src/self/seat/queue/cap/stale/test="
                 "%llu/%llu/%llu/%llu/%llu/%llu/%llu/%llu/%llu/%llu "
                 "joins=%llu leaves=%llu hello=%llu ack=%llu%s%s\n",
                 tag, now,
                 g_mode == kModeRelay ? "relay" : g_mode == kModeMesh ? "mesh"
                                                                      : "none",
                 state_name(g_state), g_seat, mask, n,
                 g_st.sent[0], g_st.sent[1], g_st.sent[2], g_st.sent[3],
                 g_st.recvd[0], g_st.recvd[1], g_st.recvd[2], g_st.recvd[3],
                 g_st.pings_sent, g_st.pongs_sent, g_st.pongs_recvd, rtt,
                 g_st.send_refused, g_st.drop_tag, g_st.drop_len,
                 g_st.drop_version, g_st.drop_source, g_st.drop_self,
                 g_st.drop_seat, g_st.drop_queue, g_st.drop_cap, g_drop_stale,
                 g_st.drop_test, g_st.joins, g_st.leaves, g_st.hellos,
                 g_st.acks, arena_tail, room_tail);
}

// One datagram off the wire (or out of the test hold ring).
void process(unsigned char *p, int len, const sockaddr_in &from,
             unsigned now) {
    int pong_to = -1;
    unsigned char pong[12];
    int mode = kModeNone, me = -1;
    sockaddr_in relay = {};
    {
        Lock l;
        if (g_mode == kModeRelay && len == 16 && std::memcmp(p, "SMRA", 4) == 0
            && from.sin_addr.s_addr == g_relay.sin_addr.s_addr
            && from.sin_port == g_relay.sin_port) {
            on_ack(p, now);
            if (!g_exit_full) return;
        } else {
            const int seat = source_seat(p, len, from);
            if (seat < 0) { ++g_st.drop_source; return; }
            if (g_state != kHubLive) { ++g_st.drop_source; return; }
            accept_datagram(p, len, seat, now, &pong_to, pong);
            mode = g_mode; me = g_seat; relay = g_relay;
            if (pong_to >= 0) ++g_st.pongs_sent;
        }
    }
    if (g_exit_full) {
        // THE FIRST PAIRING met a full room (design.txt section 5): exit 4 so
        // the launcher asks the lobby for another world. Never after a pairing
        // has succeeded: that player is mid-game and keeps playing solo.
        std::fprintf(stderr, "[hub] the relay says this room is full. Exiting "
                     "with code 4 so the launcher can ask for another "
                     "room.\n");
        std::fflush(stderr);
        std::fflush(stdout);
        ::ExitProcess(4);
    }
    if (pong_to >= 0) send_out(pong, 12, pong_to, mode, me, relay);
}

DWORD WINAPI hub_thread(void *) {
    unsigned last_hello = 0, last_ping = 0, last_report = now_ms();
    unsigned last_ref_ping = 0;
    bool first_hello = true;
    unsigned char buf[2048];
    for (;;) {
        if (::InterlockedCompareExchange(&g_stop, 0, 0)) return 0;
        const unsigned now = now_ms();
        int mode, me, state;
        sockaddr_in relay;
        {
            Lock l;
            mode = g_mode; me = g_seat; state = g_state; relay = g_relay;
            if (mode == kModeRelay) {
                const unsigned every = (state == kHubRefused) ? 5000u : 1000u;
                if (first_hello || now - last_hello >= every) {
                    send_hello();
                    last_hello = now;
                    first_hello = false;
                }
                if (state == kHubLive && now - g_last_ack > 5000u) {
                    std::fprintf(stderr, "[hub] no ACK from the relay for 5 s;"
                                 " re-pairing\n");
                    g_state = kHubPairing;
                    if (!g_arena && g_room == kHubRoomCoop && coop_rejoin_on() && g_seat >= 0 && g_seat < kHubPartySeats) {
                        // REJOIN1: a coop console comes back on the seat it had, so nothing keyed on seats shuffles
                        g_want_seat = g_seat;
                        g_seat_taken_since = 0;
                        std::fprintf(stderr, "[hub] coop: asking the relay for my old seat %d again\n", g_seat);
                    }
                    state = g_state;
                }
            }
            presence_sweep(now);
            if (g_report_s > 0 && now - last_report >= (unsigned)g_report_s * 1000u) {
                report_locked("periodic");
                last_report = now;
            }
        }
        if (state == kHubLive && now - last_ping >= 500u) {
            int ask = -1;
            {
                Lock l;
                for (int i = 1; i <= kHubMaxSeats && ask < 0; ++i) {
                    const int s = (g_ping_rr + i) % kHubMaxSeats;
                    if (g_seats[s].present && s != me) ask = s;
                }
                if (ask >= 0) g_ping_rr = ask;
                ++g_st.pings_sent;
                last_ping = now;
            }
            send_ping(me, mode, relay, ask);
        }
        // THE REFEREE CLOCK PING, 2 Hz, arena relay mode only (plan 4.2).
        if (g_arena && mode == kModeRelay && state == kHubLive &&
            now - last_ref_ping >= 500u) {
            last_ref_ping = now;
            send_ref_ping(relay, me);
        }

        if (g_bo_n) {   // REJOIN1 test knob: say when a blackout starts and ends
            const LONG on = blackout_now(now) ? 1 : 0;
            if (on != g_bo_on) {
                g_bo_on = on;
                std::fprintf(stderr, "[hub] test: blackout %s at %u ms after the install (%ld sends dropped so far)\n",
                             on ? "STARTS" : "ENDS", now - g_install_ms, (long)g_bo_dropped);
            }
        }
        fd_set rd;
        FD_ZERO(&rd);
        FD_SET(g_sock, &rd);
        timeval tv = { 0, 5000 };   // 5 ms: keeps the delay knob honest
        const int r = WS.select(0, &rd, nullptr, nullptr, &tv);
        if (r > 0) {
            for (;;) {
                sockaddr_in from = {};
                int fl = (int)sizeof from;
                const int n = WS.recvfrom(g_sock, (char *)buf, (int)sizeof buf,
                                          0, (sockaddr *)&from, &fl);
                if (n <= 0) {
                    if (n < 0 && WS.WSAGetLastError() == WSAEMSGSIZE) {
                        Lock l;           // bigger than any kind: refuse it
                        ++g_st.drop_cap;
                        continue;
                    }
                    break;                // WSAEWOULDBLOCK: drained
                }
                const unsigned t = now_ms();
                if (n > kHubMaxDatagram) {
                    Lock l;
                    ++g_st.drop_cap;
                    continue;
                }
                if (g_bo_n && blackout_now(t)) {   // REJOIN1 test knob: the cable is out
                    Lock l;
                    ++g_st.drop_test;
                    continue;
                }
                if (g_loss_pct > 0) {
                    g_rng = g_rng * 1664525u + 1013904223u;
                    if ((int)((g_rng >> 8) % 100u) < g_loss_pct) {
                        Lock l;
                        ++g_st.drop_test;
                        continue;
                    }
                }
                if (g_delay_ms > 0) {
                    if (g_held_count == kHeldRing) {
                        Lock l;
                        ++g_st.drop_test;
                        continue;
                    }
                    Held &h = g_held[(g_held_head + g_held_count) % kHeldRing];
                    h.due = t + (unsigned)g_delay_ms;
                    h.len = n;
                    h.from = from;
                    std::memcpy(h.b, buf, (size_t)n);
                    ++g_held_count;
                    continue;
                }
                process(buf, n, from, t);
            }
        }
        while (g_held_count > 0) {
            Held &h = g_held[g_held_head];
            const unsigned t = now_ms();
            if ((int)(t - h.due) < 0) break;
            process(h.b, h.len, h.from, t);
            g_held_head = (g_held_head + 1) % kHeldRing;
            --g_held_count;
        }
    }
}

bool open_socket_mesh(int prefer) {
    SOCKET s = WS.socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET) return false;
    BOOL excl = TRUE;   // two consoles can never share a seat's port
    WS.setsockopt(s, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char *)&excl,
                  (int)sizeof excl);
    // A PINNED SEAT FIRST (arena: the seat is the body, so a test that names
    // its seats gets them, not whichever port came free first).
    const int seats = (g_room == kHubRoomWorld) ? kHubMaxSeats : kHubPartySeats;
    if (prefer >= 0 && prefer < seats) {
        sockaddr_in a = mesh_addr(prefer);
        if (WS.bind(s, (const sockaddr *)&a, (int)sizeof a) == 0) {
            g_sock = s;
            g_seat = prefer;
            return true;
        }
        std::fprintf(stderr, "[hub] mesh: pinned seat %d (port %d) is taken; "
                     "taking the first free seat\n", prefer,
                     g_mesh_base + prefer);
    }
    for (int k = 0; k < seats; ++k) {
        sockaddr_in a = mesh_addr(k);
        if (WS.bind(s, (const sockaddr *)&a, (int)sizeof a) == 0) {
            g_sock = s;
            g_seat = k;
            return true;
        }
    }
    WS.closesocket(s);
    std::fprintf(stderr, "[hub] mesh: every seat port %d..%d is taken\n",
                 g_mesh_base, g_mesh_base + seats - 1);
    return false;
}

// SEAMJOIN1: THE SEAT OF A SESSION LEFT IN PLACE ENDS AT ONCE ON THE NEXT JOIN.
// The relay has no goodbye message: a seat ends when its holder is unheard for
// the hub idle time (10 s), or when the SAME address says hello again, which
// the relay answers by detaching the old endpoint first ("leave ... (rejoin)",
// tools/relay/relay.py) and then seating the hello as new. A join after an
// in-place leave opens a new socket, so it binds the local port the last one
// had (comms_hub_teardown keeps it here): to the relay the new hello comes
// from the old address, the old seat is retired on the spot, and the join gets
// a clean seat instead of racing the idle timer. A port no longer free falls
// back to any port, which is the old behaviour (the old seat then idles out).
unsigned short g_last_local_port = 0;   // network order; 0 = none kept

bool open_socket_relay() {
    SOCKET s = WS.socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET) return false;
    sockaddr_in a = {};
    a.sin_family = AF_INET;   // any address, any port: the NAT picks
    if (g_last_local_port) {
        a.sin_port = g_last_local_port;
        if (WS.bind(s, (const sockaddr *)&a, (int)sizeof a) == 0) {
            std::fprintf(stderr, "[seamless] the hub socket took the last session's local port again: the relay retires "
                         "the old seat when this hello arrives\n");
            g_last_local_port = 0;
            g_sock = s;
            return true;
        }
        std::fprintf(stderr, "[seamless] the last session's local port is taken; the hub socket takes any port (the old "
                     "seat idles out)\n");
        g_last_local_port = 0;
        a.sin_port = 0;
    }
    if (WS.bind(s, (const sockaddr *)&a, (int)sizeof a) != 0) {
        WS.closesocket(s);
        return false;
    }
    g_sock = s;
    return true;
}

void finish_socket() {
    u_long nb = 1;
    WS.ioctlsocket(g_sock, FIONBIO, &nb);
    // An ICMP "port unreachable" (a mesh seat nobody holds, a relay restart)
    // must not surface as WSAECONNRESET on the next receive.
    BOOL off = FALSE;
    DWORD got = 0;
    WS.WSAIoctl(g_sock, SIO_UDP_CONNRESET, &off, (DWORD)sizeof off, nullptr,
                0, &got, nullptr, nullptr);
    int big = 256 * 1024;   // 16 seats of voice arrive in bursts
    WS.setsockopt(g_sock, SOL_SOCKET, SO_RCVBUF, (const char *)&big,
                  (int)sizeof big);
}

// AT PROCESS EXIT THE I/O THREAD STOPS FIRST. exit() runs this, then tears
// down stdio, and only then does ExitProcess end the other threads. A hub
// thread still printing "seat k left" into that window read a closed stderr:
// 2 of 16 probes in the first 16-seat mesh rung died 0xC0000005 AFTER their
// last line. So: raise the flag, wait for the thread (it checks every 5 ms,
// bounded at 500 ms), then print the exit line from here.
void report_at_exit() {
    ::InterlockedExchange(&g_stop, 1);
    if (g_thread) ::WaitForSingleObject(g_thread, 500);
    comms_hub_report("exit");
}

// THE OPEN, shared by the adventure install and the arena install (lane
// ARENACORE1 split it out of comms_hub_install_from_env, unchanged for
// adventure). From here on the hub owns the session even if it fails to open:
// a failed hub must not fall through to the VS carrier and seat a DS role
// (adventure), and a failed arena must not turn itself into a lockstep match
// (arena: lockstep is chosen by the go plan, never as a silent fallback).
bool install_open(bool want_relay, const char *relay, const char *code,
                  int want_seat) {
    const char *what = g_arena ? "arena online" : "adventure online";
    g_installed = true;
    g_state = kHubFailed;
    // THE ROOM KIND (lane WIRE1): SM64DS_ROOM_KIND = world | party | coop,
    // set by the launcher for a hub room; anything else (or nothing) is the
    // old world hub. An arena is never a party or coop room.
    g_room = kHubRoomWorld;
    if (!g_arena) {
        const char *rk = online_session_get(kSessRoomKind);   /* SEAMHUB1: the session record */
        if (rk && *rk) {
            if (!std::strcmp(rk, "party")) g_room = kHubRoomParty;
            else if (!std::strcmp(rk, "coop")) g_room = kHubRoomCoop;
            else if (std::strcmp(rk, "world"))
                std::fprintf(stderr, "[hub] SM64DS_ROOM_KIND=%s is not world, "
                             "party or coop; treated as world\n", rk);
        }
    }
    if (g_room != kHubRoomWorld && want_seat < 0xFF &&
        want_seat >= kHubPartySeats) {
        std::fprintf(stderr, "[hub] a %s room has seats 0..3; seat %d is "
                     "not one, asking for any free seat\n",
                     g_room == kHubRoomParty ? "party" : "coop", want_seat);
        want_seat = 0xFF;
    }
    g_delay_ms = env_int("SM64DS_HUB_DELAY_MS", 0, 0, 2000);
    g_loss_pct = env_int("SM64DS_HUB_LOSS_PCT", 0, 0, 90);
    g_report_s = env_int("SM64DS_HUB_REPORT_S", 10, 0, 3600);
    g_install_ms = now_ms();
    blackout_parse();
    g_rng ^= (unsigned)::GetCurrentProcessId() * 2654435761u;

    if (!ws_up()) return true;

    if (want_relay) {
        g_mode = kModeRelay;
        const size_t cl = code ? std::strlen(code) : 0;
        bool code_ok = cl >= 1 && cl <= 8;
        for (size_t i = 0; code_ok && i < cl; ++i)
            if ((unsigned char)code[i] < 0x21 || (unsigned char)code[i] > 0x7E)
                code_ok = false;
        if (!code_ok) {
            std::fprintf(stderr, "[hub] SM64DS_COMMS_CODE must be 1..8 "
                         "printable ASCII characters; %s is off\n", what);
            return true;
        }
        std::memcpy(g_code, code, cl);
        if (!parse_host_port(relay, 41234, &g_relay)) {
            std::fprintf(stderr, "[hub] could not resolve the relay; "
                         "%s is off\n", what);
            return true;
        }
        g_want_seat = want_seat;
        if (g_want_seat >= kHubMaxSeats) g_want_seat = 0xFF;
        if (!open_socket_relay()) {
            std::fprintf(stderr, "[hub] no UDP socket; %s is off\n", what);
            return true;
        }
        g_state = kHubPairing;
    } else {
        g_mode = kModeMesh;
        g_mesh_base = sess_int(kSessHubPort, 0, 1024, 65535 - kHubMaxSeats);
        if (!g_mesh_base) return true;
        if (g_mesh_base <= 51765 && 51765 < g_mesh_base + kHubMaxSeats)
            std::fprintf(stderr, "[hub] WARNING: mesh ports %d..%d include "
                         "51765\n", g_mesh_base,
                         g_mesh_base + kHubMaxSeats - 1);
        if (!open_socket_mesh(g_arena && want_seat < kHubMaxSeats ? want_seat
                                                                  : -1))
            return true;
        g_state = kHubLive;
    }
    finish_socket();

    HANDLE th = ::CreateThread(nullptr, 64 * 1024, hub_thread, nullptr, 0,
                               nullptr);
    if (!th) {
        std::fprintf(stderr, "[hub] could not start the I/O thread; %s is "
                     "off\n", what);
        WS.closesocket(g_sock);
        g_sock = INVALID_SOCKET;
        g_state = kHubFailed;
        return true;
    }
    g_thread = th;   // kept open: report_at_exit waits on it
    /* SEAMJOIN1: an in-place join installs a second time in one process; the exit hook goes in once */
    static bool exit_hooked = false;
    if (!exit_hooked) {
        exit_hooked = true;
        std::atexit(report_at_exit);
    }
    if (g_arena) {
        char cs[9] = {};
        std::memcpy(cs, g_code, 8);
        if (g_mode == kModeRelay)
            std::fprintf(stderr, "[hub] installed: ARENA through the relay "
                         "(address withheld), code %s, role 3, want seat %d. No DS session "
                         "and no rounds; the referee is the relay.\n",
                         cs, g_want_seat);
        else
            std::fprintf(stderr, "[hub] installed: ARENA on the loopback mesh "
                         "base %d, seat %d (tests only: no referee here). No "
                         "DS session and no rounds.\n", g_mesh_base, g_seat);
    } else if (g_mode == kModeRelay) {
        char cs[9] = {};
        std::memcpy(cs, g_code, 8);
        std::fprintf(stderr, "[hub] installed: adventure hub through the "
                     "relay (address withheld), code %s, want seat %d%s. No DS session is "
                     "seated; this console plays its own game.\n", cs,
                     g_want_seat,
                     g_room == kHubRoomParty ? ", PARTY room (role 4)"
                     : g_room == kHubRoomCoop ? ", COOP room (role 5)" : "");
    } else {
        std::fprintf(stderr, "[hub] installed: adventure hub, loopback mesh "
                     "base %d, seat %d%s. No DS session is seated; this "
                     "console plays its own game.\n", g_mesh_base, g_seat,
                     g_room == kHubRoomParty ? ", PARTY room"
                     : g_room == kHubRoomCoop ? ", COOP room" : "");
    }
    return true;
}

}  // namespace

// ===========================================================================
// THE SESSION RECORD (lane SEAMHUB1; see hal/comms_hub.h)
// ===========================================================================
namespace {
const char *const k_sess_names[kSessFields] = {
    "SM64DS_ADVENTURE", "SM64DS_COMMS_RELAY", "SM64DS_COMMS_CODE", "SM64DS_ROOM_KIND", "SM64DS_VOICE_ALLOWED",
    "SM64DS_HUB_SEAT", "SM64DS_PLAYER_NAME", "SM64DS_PLAYER_COLOR", "SM64DS_PLAYER_SHOES", "SM64DS_WORLD_NO",
    "SM64DS_HUB_PORT" };
OnlineSession g_sess;

void sess_fill() {
    if (g_sess.filled) return;
    g_sess.filled = true;
    if (g_sess.cleared) return;
    for (int i = 0; i < kSessFields; ++i) {
        const char *e = std::getenv(k_sess_names[i]);
        if (!e) continue;
        g_sess.set[i] = true;
        std::strncpy(g_sess.value[i], e, sizeof g_sess.value[i] - 1);
        g_sess.value[i][sizeof g_sess.value[i] - 1] = 0;
    }
}
}  // namespace

const char *online_session_get(OnlineSessionField f) {
    if (f < 0 || f >= kSessFields) return nullptr;
    sess_fill();
    return g_sess.set[f] ? g_sess.value[f] : nullptr;
}

// SEAMCLEAN1: host_settings.cpp reads the Adventure flag through the record when this file is linked in
extern "C" const char *(*port_session_adventure_hook)(void);
static const char *session_adventure_now() { return online_session_get(kSessAdventure); }
static struct SessionAdventureSeat { SessionAdventureSeat() { port_session_adventure_hook = session_adventure_now; } } g_session_adventure_seat;

const char *online_session_name(OnlineSessionField f) {
    return (f >= 0 && f < kSessFields) ? k_sess_names[f] : "?";
}

const OnlineSession &online_session() {
    sess_fill();
    return g_sess;
}

// SEAMJOIN1: the in-place join writes the record from the plan
void online_session_begin() {
    g_sess.filled = true;
    g_sess.cleared = false;
    for (int i = 0; i < kSessFields; ++i) {
        g_sess.set[i] = false;
        g_sess.value[i][0] = 0;
    }
}

void online_session_set(OnlineSessionField f, const char *v) {
    if (f < 0 || f >= kSessFields) return;
    if (!v || !*v) {
        g_sess.set[f] = false;
        g_sess.value[f][0] = 0;
        return;
    }
    g_sess.set[f] = true;
    std::strncpy(g_sess.value[f], v, sizeof g_sess.value[f] - 1);
    g_sess.value[f][sizeof g_sess.value[f] - 1] = 0;
}

int online_session_clear() {
    sess_fill();
    int n = 0;
    for (int i = 0; i < kSessFields; ++i) {
        if (g_sess.set[i]) ++n;
        g_sess.set[i] = false;
        g_sess.value[i][0] = 0;
    }
    g_sess.cleared = true;
    return n;
}

// ===========================================================================
// PUBLIC
// ===========================================================================
// SEAMHUB1: the install's answer lives here, not in the function, so the
// in-place leave (comms_hub_teardown) can forget it.
namespace {
bool g_inst_tried = false;
bool g_inst_owned = false;   // the answer: true = skip the VS carrier
// SEAMVS1: the arena install's latch, here for the same reason: a VS match
// that ends in place tears the hub down, and the next match handed to this
// process installs the arena again.
bool g_arena_tried = false;
bool g_arena_owned = false;
}  // namespace

bool comms_hub_install_from_env() {
    bool &tried = g_inst_tried;
    bool &owned = g_inst_owned;
    if (tried) return owned;
    tried = true;

    if (!adventure_ghost_mode()) return false;
    // A VS MATCH IS NEVER THE HUB'S, whatever the adventure flag says. The
    // launcher names the VS map for every VS match and never for adventure,
    // so SM64DS_VS_MAP is the one knob that cannot be a stale setting: a
    // hand-edited settings.json with AdventureGhosts on must not let the hub
    // take a VS match's relay code and keep the VS carrier from installing.
    // Declined silently; the VS carrier logs its own install.
    if (const char *vs = std::getenv("SM64DS_VS_MAP"))
        if (*vs) return false;

    /* SEAMHUB1: the address, the code and the seat come from the session record (hal/comms_hub.h), which the in-place leave clears */
    const char *relay = online_session_get(kSessRelay);
    const char *code = online_session_get(kSessCode);
    const char *mesh = online_session_get(kSessHubPort);
    const bool want_relay = relay && *relay;
    const bool want_mesh = !want_relay && mesh && *mesh;
    if (!want_relay && !want_mesh) {
        // ADVENTURE NEVER SEATS A DS ROLE, and that holds on this path too.
        // An adventure console handed the VS carrier's direct env (a role and
        // a port, no relay, no hub port) used to open that carrier and seat
        // roles 1 and 2: the old 0.5.3 adventure transport, joiner without a
        // save included. It plays solo instead, and says why once.
        if (const char *role = std::getenv("SM64DS_COMMS_ROLE")) {
            std::fprintf(stderr, "[hub] adventure mode with SM64DS_COMMS_ROLE="
                         "%s but no hub address (SM64DS_COMMS_RELAY or "
                         "SM64DS_HUB_PORT): the VS carrier is not opened for "
                         "adventure, so this console plays solo\n", role);
            owned = true;
            return true;
        }
        return false;   // adventure, offline: nothing to open, nothing to skip
    }
    owned = true;

    if (const char *role = std::getenv("SM64DS_COMMS_ROLE"))
        std::fprintf(stderr, "[hub] SM64DS_COMMS_ROLE=%s is ignored: an "
                     "adventure console opens the hub, never the VS "
                     "carrier\n", role);

    return install_open(want_relay, relay, code,
                        sess_int(kSessHubSeat, 0xFF, 0, 0xFF));
}

// THE ARENA INSTALL (lane ARENACORE1; hal/vs_arena.cpp is the only caller).
// The caller already decided this is an arena console: no adventure gate and
// no VS refusal here. Only the address is read from the env, the same two
// knobs the adventure hub uses.
bool comms_hub_install_arena(int want_seat) {
    bool &tried = g_arena_tried;
    bool &owned = g_arena_owned;
    if (tried) return owned;
    tried = true;
    const char *relay = online_session_get(kSessRelay);   /* SEAMHUB1: the session record */
    const char *code = online_session_get(kSessCode);
    const char *mesh = online_session_get(kSessHubPort);
    const bool want_relay = relay && *relay;
    const bool want_mesh = !want_relay && mesh && *mesh;
    if (!want_relay && !want_mesh) return false;
    owned = true;
    g_arena = true;
    return install_open(want_relay, relay, code,
                        want_seat >= 0 && want_seat < kHubMaxSeats ? want_seat
                                                                   : 0xFF);
}

bool comms_hub_arena() {
    Lock l;
    return g_installed && g_arena;
}

HubRoomKind comms_hub_room_kind() {
    Lock l;
    return g_installed ? (HubRoomKind)g_room : kHubRoomWorld;
}

bool hub_on_kind(HubKind kind, HubKindFn fn, void *user) {
    if (kind < kHubPresence || kind >= kHubKinds) return false;
    Lock l;
    g_handler[kind].fn = fn;
    g_handler[kind].user = fn ? user : nullptr;
    return true;
}

// The game thread's drain: handlers run here, with no lock held.
int comms_hub_pump() {
    HITCH_SCOPE(HK_HUBPUMP);
    int delivered = 0;
    {   // proof knob SM64DS_HUB_STALL=<start_ms>:<len_ms> (ms after the first pump): the game thread does not drain for a while, as in a level load
        static int st = -1; static unsigned t0 = 0, from = 0, len = 0;
        if (st < 0) {
            st = 0;
            if (const char *e = std::getenv("SM64DS_HUB_STALL")) { unsigned a = 0, b = 0; if (std::sscanf(e, "%u:%u", &a, &b) == 2) { from = a; len = b; st = 1; t0 = comms_hub_now_ms(); } }
        }
        if (st == 1) {
            const unsigned dt = comms_hub_now_ms() - t0;
            static int phase = 0;
            if (dt >= from && dt < from + len) {
                if (phase == 0) { phase = 1; std::fprintf(stderr, "[hub-knob] stall begins t=%u wall=%llu\n", dt, (unsigned long long)GetTickCount64()); }
                return 0;
            }
            if (phase == 1 && dt >= from + len) { phase = 2; std::fprintf(stderr, "[hub-knob] stall ends t=%u wall=%llu\n", dt, (unsigned long long)GetTickCount64()); }
        }
    }
    unsigned char buf[kHubMaxDatagram];
    for (int kind = kHubPresence; kind < kHubKinds; ++kind) {
        HubKindFn fn;
        void *user;
        {
            Lock l;
            fn = g_handler[kind].fn;
            user = g_handler[kind].user;
        }
        if (!fn) continue;
        for (int i = 0; i < 64; ++i) {
            int seat = -1;
            const int n = comms_hub_recv((HubKind)kind, buf, (int)sizeof buf,
                                         &seat);
            if (n <= 0) break;
            fn(seat, buf, n, user);
            ++delivered;
            Lock l;
            ++g_dispatched;
        }
    }
    return delivered;
}

unsigned comms_hub_now_ms() { return now_ms(); }

bool comms_hub_referee_clock(unsigned *ref_now_ms, int *rtt_ms, int *cap_ms) {
    Lock l;
    int best = -1;
    for (int i = 0; i < kRefSamples; ++i)
        if (g_ref[i].used && (best < 0 || g_ref[i].rtt < g_ref[best].rtt))
            best = i;
    if (best < 0) return false;
    const RefSample &r = g_ref[best];
    // At r.recv the referee's clock read about t_ref + rtt/2.
    const unsigned now = now_ms();
    if (ref_now_ms)
        *ref_now_ms = r.t_ref + (unsigned)(r.rtt / 2) + (now - r.recv);
    if (rtt_ms) *rtt_ms = r.rtt;
    if (cap_ms) *cap_ms = g_ref_cap_ms;
    return true;
}

bool comms_hub_active() {
    Lock l;
    return g_installed && g_sock != INVALID_SOCKET;
}

int comms_hub_state() {
    Lock l;
    return g_installed ? g_state : kHubOff;
}

// REJOIN1: how long since the relay last answered a HELLO (relay mode, after the first pairing); 0 otherwise. A guest
// whose own line is out stops hearing the relay too, which is how it tells "my connection" from "the host's".
unsigned comms_hub_relay_quiet_ms() {
    Lock l;
    if (g_mode != kModeRelay || !g_ever_paired) return 0;
    return now_ms() - g_last_ack;
}
bool comms_hub_coop_rejoin() { return coop_rejoin_on(); }

int comms_hub_my_seat() {
    Lock l;
    return (g_state == kHubLive) ? g_seat : -1;
}

bool comms_hub_send(const void *datagram, int len) {
    if (!datagram || len <= 0 || len > kHubMaxDatagram) {
        Lock l;
        ++g_st.send_refused;
        return false;
    }
    unsigned char p[kHubMaxDatagram];
    std::memcpy(p, datagram, (size_t)len);
    int kind;
    int to_seat = -1;
    int mode, me;
    sockaddr_in relay;
    {
        Lock l;
        if (!g_installed || g_state != kHubLive ||
            classify(p, len, &kind) != kOk || kind >= kHubKinds ||
            !shape_ok_send(kind, g_arena, g_room)) {
            ++g_st.send_refused;
            return false;
        }
        p[5] = (unsigned char)g_seat;
        // a party hit goes to ONE seat: the victim, byte 6 (the relay routes
        // it by that byte; the mesh is sent to that seat's port only). Never
        // to myself.
        if (kind == kHubPartyHit) {
            to_seat = p[6];
            if (to_seat == g_seat || to_seat >= kHubPartySeats) {
                ++g_st.send_refused;
                return false;
            }
        }
        mode = g_mode; me = g_seat; relay = g_relay;
        ++g_st.sent[kind];
    }
    send_out(p, len, to_seat, mode, me, relay);
    return true;
}

int comms_hub_recv(HubKind kind, void *buf, int cap, int *seat) {
    return comms_hub_recv_t(kind, buf, cap, seat, nullptr);
}

int comms_hub_recv_t(HubKind kind, void *buf, int cap, int *seat,
                     unsigned *t_ms) {
    if (kind < 0 || kind >= kHubKinds || !buf) return 0;
    Lock l;
    // MOVERS9: the coop world kind drains its one-shot events first (in order), then the mover batches
    for (int pass = 0; pass < (kind == kHubCoopWorld ? 2 : 1); ++pass) {
        Fifo &f = pass == 0 ? g_fifo[kind] : g_mfifo;
        const int depth = pass == 0 ? kFifoDepth[kind] : (int)kMoverFifoDepth;   // (the SPLIT=0 control only ever fills 32 of the 128 slots)
        while (f.count > 0) {
            Msg &m = f.slot[f.head];
            f.head = (f.head + 1) % depth;
            --f.count;
            if (m.len > cap) { ++g_st.drop_cap; continue; }
            std::memcpy(buf, m.b, (size_t)m.len);
            if (seat) *seat = m.seat;
            if (t_ms) *t_ms = m.t;
            return m.len;
        }
    }
    return 0;
}

unsigned long long comms_hub_queue_drops(int kind) {
    Lock l;
    return (kind >= 0 && kind <= kHubKinds) ? g_qdrop[kind] : 0;
}

int comms_hub_latest(HubKind kind, int seat, void *buf, int cap,
                     unsigned *age_ms) {
    if (kind < 0 || kind >= kHubKinds || seat < 0 || seat >= kHubMaxSeats ||
        !buf) return 0;
    Lock l;
    const Seat &z = g_seats[seat];
    const int len = z.latest_len[kind];
    if (!z.present || len <= 0 || len > cap) return 0;
    std::memcpy(buf, z.latest[kind], (size_t)len);
    if (age_ms) *age_ms = now_ms() - z.latest_t[kind];
    return len;
}

bool comms_hub_seat_info(int seat, HubSeatInfo *out) {
    if (!out || seat < 0 || seat >= kHubMaxSeats) return false;
    Lock l;
    if (!g_installed) return false;
    const Seat &z = g_seats[seat];
    const unsigned now = now_ms();
    std::memset(out, 0, sizeof *out);
    out->present = z.present;
    out->join_epoch = z.join_epoch;
    out->heard_ms_ago = z.present ? now - z.heard : ~0u;
    out->rtt_ms = z.present ? z.rtt_ms : -1;
    out->has_ghost = z.present && z.has_ghost;
    out->ghost_ms_ago = out->has_ghost ? now - z.ghost_t : ~0u;
    if (out->has_ghost) {
        HubGhostFrame g;
        std::memcpy(&g, z.latest[kHubGhost], sizeof g);
        out->flags = g.flags;
        out->level_id = g.level_id;
        out->area = g.area;
        out->x = g.x; out->y = g.y; out->z = g.z;
    }
    return true;
}

unsigned comms_hub_seat_mask() {
    Lock l;
    unsigned m = 0;
    for (int s = 0; s < kHubMaxSeats; ++s)
        if (g_seats[s].present) m |= 1u << s;
    return m;
}

unsigned comms_hub_roster_version() {
    Lock l;
    return g_roster_version;
}

CommsHubStats comms_hub_stats() {
    Lock l;
    CommsHubStats s = g_st;
    s.installed = g_installed;
    s.mode = g_mode;
    s.state = g_installed ? g_state : kHubOff;
    s.seat = g_seat;
    s.seat_mask = 0;
    for (int k = 0; k < kHubMaxSeats; ++k)
        if (g_seats[k].present) s.seat_mask |= 1u << k;
    s.drop_stale = g_drop_stale;
    s.arena = g_installed && g_arena;
    s.room = g_installed ? g_room : kHubRoomWorld;
    s.drop_field = g_drop_field;
    s.dispatched = g_dispatched;
    return s;
}

void comms_hub_report(const char *tag) {
    Lock l;
    if (!g_installed) return;
    report_locked(tag ? tag : "report");
}

// SEAMHUB1: no more periodic report lines from the I/O thread (the in-place leave asks for this at the quit, so the fade and the
// title print no "[hub]" line before the teardown).
void comms_hub_reports_off() {
    Lock l;
    g_report_s = 0;
}

// ===========================================================================
// THE TEARDOWN (run after093, lane SEAMHUB1; the one-process plan's H-a).
// Until now the hub lived as long as the process: installed once in main(),
// its I/O thread stopped only by report_at_exit. Leaving an Adventure Online
// world to the title in place needs it gone while the process carries on as a
// solo game, so this stops the thread the way report_at_exit does (the flag,
// then a wait; the thread looks at the flag every 5 ms), and only once it has
// returned closes the socket and puts every piece of state back to what a
// process that never installed the hub has. The atexit hook stays registered
// (it cannot be removed) and has nothing left to do: no thread to wait for,
// and comms_hub_report says nothing with the hub not installed. Winsock stays
// started, as it always has. Game thread only.
// ===========================================================================
bool comms_hub_teardown(const char *why) {
    HANDLE th;
    {
        Lock l;
        th = g_thread;
    }
    unsigned long long sent_g = 0, recvd_g = 0;
    int seat = -1, peers = 0;
    {
        Lock l;
        sent_g = g_st.sent[0];
        recvd_g = g_st.recvd[0];
        seat = g_seat;
        for (int s = 0; s < kHubMaxSeats; ++s)
            if (g_seats[s].present && s != g_seat) ++peers;
    }
    if (th) {
        ::InterlockedExchange(&g_stop, 1);
        const DWORD w = ::WaitForSingleObject(th, 2000);
        if (w != WAIT_OBJECT_0) {
            ::InterlockedExchange(&g_stop, 0);
            std::fprintf(stderr, "[seamless] the hub's I/O thread did not stop in 2 s "
                         "(wait %lu); the hub is left as it was (%s)\n",
                         (unsigned long)w, why ? why : "?");
            std::fflush(stderr);
            return false;
        }
        ::CloseHandle(th);
    }
    const bool had_socket = g_sock != INVALID_SOCKET;
    /* SEAMJOIN1: keep the relay socket's local port for the next join in this process (open_socket_relay) */
    if (had_socket && g_mode == kModeRelay && WS.getsockname) {
        sockaddr_in me = {};
        int len = (int)sizeof me;
        if (WS.getsockname(g_sock, (sockaddr *)&me, &len) == 0) g_last_local_port = me.sin_port;
    }
    if (had_socket && WS.closesocket) WS.closesocket(g_sock);
    {
        Lock l;
        g_installed = false;
        g_arena = false;
        g_room = kHubRoomWorld;
        for (int k = 0; k < kHubKinds; ++k) g_handler[k] = KindHandler();
        g_mode = kModeNone;
        g_state = kHubOff;
        g_seat = -1;
        g_sock = INVALID_SOCKET;
        g_relay = sockaddr_in();
        g_mesh_base = 0;
        std::memset(g_code, 0, sizeof g_code);
        g_want_seat = 0xFF;
        g_seat_taken_since = 0;
        g_ever_paired = false;
        g_exit_full = false;
        g_thread = nullptr;
        g_last_ack = 0;
        ++g_roster_version;            // every roster reader sees the change
        g_ping_rr = 0;
        std::memset(g_fifo, 0, sizeof g_fifo);
        std::memset(&g_mfifo, 0, sizeof g_mfifo);
        std::memset(g_qdrop, 0, sizeof g_qdrop);
        std::memset(g_seats, 0, sizeof g_seats);
        g_st = CommsHubStats();
        g_drop_stale = 0;
        g_drop_field = 0;
        g_dispatched = 0;
        std::memset(g_ref, 0, sizeof g_ref);
        g_ref_next = 0;
        g_ref_last_t_ref = 0;
        g_ref_last_recv = 0;
        g_ref_have = false;
        g_ref_cap_ms = -1;
        g_delay_ms = 0;
        g_loss_pct = 0;
        g_report_s = 10;
        g_held_head = 0;
        g_held_count = 0;
    }
    ::InterlockedExchange(&g_stop, 0);
    g_inst_tried = false;
    g_inst_owned = false;
    g_arena_tried = false;   // SEAMVS1
    g_arena_owned = false;
    std::fprintf(stderr, "[seamless] hub torn down (%s): I/O thread %s, socket %s; "
                 "it was seat %d with %d other seat(s) present, %llu ghost frame(s) "
                 "sent, %llu received\n", why ? why : "?",
                 th ? "stopped and joined" : "never started",
                 had_socket ? "closed" : "never opened", seat, peers, sent_g, recvd_g);
    std::fflush(stderr);
    return true;
}

// ===========================================================================
// THE INSTALL IN A LIVE PROCESS (run after093, lane SEAMJOIN1; the one-process
// plan's M4b). Joining an online world from the title used to start the game
// again so that main() could make this install with the join's names in the
// environment. The in-place join fills the session record instead and asks
// for the same install here, at the title, before any level of the session is
// loaded: comms_hub_install_from_env with its latch forgotten, reading the
// record, opening the socket and starting the I/O thread exactly as at boot.
// A hub still up (a previous session not torn down) or a thread handle still
// held refuses; so does an install that did not come up (no socket, a bad
// address or code), which is torn down again so the caller can restart as
// before with nothing of it left in this process. Game thread only.
// ===========================================================================
bool comms_hub_install_in_place(const char **why) {
    static char buf[160];
    *why = nullptr;
    {
        Lock l;
        if (g_installed || g_thread) {
            *why = "a hub of an earlier session is still installed in this process";
            return false;
        }
    }
    g_inst_tried = false;
    g_inst_owned = false;
    const bool owned = comms_hub_install_from_env();
    const int st = comms_hub_state();
    if (owned && comms_hub_active() && st != kHubFailed && g_thread) {
        std::fprintf(stderr, "[seamless] hub installed in this process (state %d, want seat %d)\n",
                     st, g_want_seat);
        std::fflush(stderr);
        return true;
    }
    std::snprintf(buf, sizeof buf, "the hub install did not come up (owned %d, socket %s, state %d)",
                  owned ? 1 : 0, comms_hub_active() ? "open" : "none", st);
    *why = buf;
    comms_hub_teardown("the in-place install did not come up");
    return false;
}

}  // namespace port
