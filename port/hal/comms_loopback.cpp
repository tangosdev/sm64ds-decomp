// THE LOOPBACK CARRIER, implemented. Run mg16, lane MP2.
// The env knobs and the shape of the thing are in comms_loopback.h.
//
// PORT_HOST_ABI: this file is host plumbing. It is the only thing in the tree
// that opens a socket, it sits BELOW the eight ROM addresses hosted in
// hal/comms_seam.cpp, and no line of game code above the seam knows it exists.
//
// ===========================================================================
// WHY UDP, ARGUED FROM THE CONTRACT AND NOT FROM HABIT
// ===========================================================================
//
// comms_seam.h picks the datagram shape for us four times over:
//
//   1. "0x20 bytes per player per frame". A fixed-size message with a natural
//      message boundary, produced once per frame. That is a datagram. A stream
//      would need a length prefix and a reassembly buffer to rebuild a
//      boundary the medium already has.
//
//   2. exchange() is a POLL: "answer whether every live peer's block for THIS
//      frame has arrived. 1 = the round is complete, 0 = keep waiting", and
//      the contract says nothing here blocks. recvfrom on a non-blocking
//      socket answers exactly that question -- a whole message or nothing.
//      A stream read can return half a block, and half a block inside a call
//      that must not block means holding parse state across frames.
//
//   3. "when the wait bound runs out the ROM drops the session back to solo by
//      itself" (src/func_0203ea5c.c:487, bounds at :142-146). The ROM ALREADY
//      HANDLES LOSS. A reliable stream would fight that: TCP head-of-line
//      blocking stalls a round the ROM is prepared to abandon, and the only
//      retry lockstep wants is "send this frame's block again", which is one
//      sendto rather than a retransmission queue.
//
//   4. open() returns void and there is no error channel (gap 3 in the
//      contract's own banner). A UDP bind resolves synchronously and locally,
//      so the one failure that matters is known before open() returns. A TCP
//      connect completes asynchronously and would need the channel the
//      contract does not have.
//
// And a fifth, which is the north star rather than the contract: the DS's own
// radio is an unacknowledged per-frame broadcast. UDP keeps the semantics the
// arm9 sees closest to the hardware this port has to stay portable back to.
//
// ===========================================================================
// ADDRESSING: THE SLOT IS THE PORT
// ===========================================================================
//
// Slot k listens on kCommsLoopbackPortBase + k. That single rule removes every
// discovery mechanism a loopback pair would otherwise need:
//
//   * the parent is slot 0, so its address is a constant and a child needs no
//     rendezvous;
//   * a child claims a slot BY BINDING ITS PORT, so slot assignment is decided
//     by the operating system's bind and cannot race between two children;
//   * a second parent on one machine fails its bind instead of silently
//     stealing the session, which is why SO_REUSEADDR is deliberately NOT set;
//   * the parent can verify a sender's claimed slot against the source port it
//     actually arrived from.
//
// ===========================================================================
// TOPOLOGY: PARENT-MEDIATED, THE WAY WM IS
// ===========================================================================
//
// Loopback would let every peer talk to every other peer directly, and for two
// players a mesh and a star are indistinguishable. The star is still the right
// one: on the DS the children send to the parent and the parent broadcasts the
// assembled round, and the contract says "the parent's frame counter is the
// clock". So:
//
//   child  -> sends its own block to the parent, and its round completes when
//             the PARENT's packet for that round arrives.
//   parent -> collects every live child's block for the round, then broadcasts
//             one packet carrying ALL FOUR slots, then completes.
//
// One round trip per frame, the clock lives in exactly one place, and a child's
// completion condition is one packet rather than a quorum. It also means a
// child gets the other children's blocks the same way the DS gives them to it,
// through the parent, so peer_block() is fed from the same shape of message.
//
// ===========================================================================
// THE BYTES ARE NOT READ
// ===========================================================================
//
// "A transport MUST NOT reinterpret those bytes. It moves them and nothing
// else." Nothing below indexes into a block. The round id this file
// synchronises on is the TRANSPORT's own counter in its own header, not the
// ROM's frame counter at block offset +0x02, precisely so that no code here
// has to look inside. memcpy in, memcpy out.

#include "comms_loopback.h"

#include "comms_seam.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>    // SIO_UDP_CONNRESET; see the ioctl in lb_open()

// This SDK's mstcpip.h does not expose it (it is gated behind version macros
// that the port's 32-bit toolchain build does not set). The value is the
// documented one and has been stable since Windows 2000, so it is defined here
// rather than letting a header version decide whether the ioctl happens.
#ifndef SIO_UDP_CONNRESET
#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>

// ===========================================================================
// ws2_32 IS NOT LINKED. IT IS LoadLibrary'd ON FIRST OPEN, AND THAT IS NOT A
// PREFERENCE -- IT IS A BUG THIS LANE SHIPPED, MEASURED, AND HAD TO UNDO.
// ===========================================================================
//
// A PLAIN STATIC IMPORT OF ws2_32 BREAKS THE WHOLE PORT, on every path, with
// the transport switched off and no socket ever created.
//
// The port survives by claiming the DS's FIXED addresses -- 0x02000000 main
// RAM and 0x027ff000, the shared system block -- and it claims them from a TLS
// CALLBACK AT PROCESS START, BEFORE THE CRT, precisely because anything that
// maps low first wins and the game has no second choice of address. The
// WINDOWS LOADER RESOLVES STATIC IMPORTS BEFORE IT RUNS TLS CALLBACKS. So the
// moment ws2_32 became a static import, ws2_32 and its dependency chain were
// mapped before the port ever got to ask, and something in that chain landed
// across 0x02000000.
//
// MEASURED, not deduced. Before this file was linked, the level-1 selftest ran
// rc 0 twice. With ws2_32 statically imported it failed 6 times out of 6, from
// two different launchers, with:
//
//   [io] main RAM 02000000 NOT mapped
//   [io]   LOST 02000000..02400000  main memory
//   [io]   LOST 027ff000..02800000  the shared system block (required)
//   [io]   in 02000000: 02000000..02e61000 reserved -> a mapped file with no
//                       readable name
//
// The blocking region was about 14 MB and its size MOVED between runs, so it
// was never the 4.4 MB exe image; it is a pagefile-backed section belonging to
// the Winsock chain.
//
// LOADING IT BY HAND on the first open() puts the DLL well after io_init has
// taken the fixed ranges, and with SM64DS_COMMS_ROLE unset it is never loaded
// at all -- which is also the honest shape: a run with no multiplayer session
// should not be carrying a socket library.
//
// THE GENERAL RULE, worth more than this instance: ANY new static DLL import
// in this port is a hazard to the fixed-range reservation, because it runs
// ahead of the TLS callback by construction. The tree already reaches user32
// and friends through GetProcAddress (walk_window.cpp's winapi_load); this is
// the same lesson arriving through the linker instead.
// WHAT WAS TRIED AND DID NOT WORK, so nobody retries it: a plain
// #pragma comment(lib, "ws2_32.lib") is what broke the port, and adding
// #pragma comment(linker, "/DELAYLOAD:ws2_32.dll") beside it DID NOT FIX IT --
// dumpbin /dependents still listed WS2_32.dll as a normal import, and the
// selftest still failed with LOST 02000000. So there is no import library here
// at all. ws2_32 is reached the way port/tests/walk_window.cpp's winapi_load()
// reaches user32: LoadLibraryA plus GetProcAddress, on the first open(), which
// is the first moment a role has actually been asked for.
//
// The winsock2.h include above is kept for the TYPES and CONSTANTS -- sockaddr_in,
// SOCKET, AF_INET, FIONBIO. None of its function DECLARATIONS are used; every
// call below goes through the WS table.

extern "C" void func_02040724(void);   /* the seam's close() face */

namespace port {
namespace {

// ---------------------------------------------------------------------------
// WS2_32, LOADED BY HAND. See the banner above the includes for why there is no
// import library: a static import of this DLL loads it BEFORE the port's TLS
// callback can claim the DS fixed addresses, and the whole port dies.
//
// Byte order is done here rather than through ws2_32's htons/htonl, which are
// three more entry points to look up for two lines of shifting. The port is
// little-endian x86 and the wire fields these build are the SOCKET's, not the
// game's, so the swap is unconditional and correct.
// ---------------------------------------------------------------------------
inline unsigned short hton16(unsigned short v) {
    return (unsigned short)((v >> 8) | (v << 8));
}
inline unsigned short ntoh16(unsigned short v) { return hton16(v); }
inline unsigned long hton32(unsigned long v) {
    return ((v & 0x000000FFul) << 24) | ((v & 0x0000FF00ul) << 8) |
           ((v & 0x00FF0000ul) >> 8)  | ((v & 0xFF000000ul) >> 24);
}

struct WsTable {
    int    (WSAAPI *WSAStartup)(WORD, LPWSADATA);
    SOCKET (WSAAPI *socket)(int, int, int);
    int    (WSAAPI *ioctlsocket)(SOCKET, long, u_long *);
    int    (WSAAPI *WSAIoctl)(SOCKET, DWORD, LPVOID, DWORD, LPVOID, DWORD,
                              LPDWORD, LPWSAOVERLAPPED,
                              LPWSAOVERLAPPED_COMPLETION_ROUTINE);
    int    (WSAAPI *bind)(SOCKET, const struct sockaddr *, int);
    int    (WSAAPI *sendto)(SOCKET, const char *, int, int,
                            const struct sockaddr *, int);
    int    (WSAAPI *recvfrom)(SOCKET, char *, int, int, struct sockaddr *,
                              int *);
    int    (WSAAPI *closesocket)(SOCKET);
    int    (WSAAPI *WSAGetLastError)(void);
};
WsTable WS;
int g_ws_loaded = 0;        // 0 not tried, 1 loaded, -1 refused

bool ws_load() {
    if (g_ws_loaded) return g_ws_loaded > 0;
    HMODULE m = ::LoadLibraryA("ws2_32.dll");
    if (!m) {
        g_ws_loaded = -1;
        std::fprintf(stderr, "[comms:loopback] ws2_32.dll would not load; "
                     "staying idle so the ROM's own bound fires\n");
        return false;
    }
#define WS_GET(n) \
    *(FARPROC *)&WS.n = ::GetProcAddress(m, #n); \
    if (!WS.n) { g_ws_loaded = -1; }
    WS_GET(WSAStartup); WS_GET(socket);   WS_GET(ioctlsocket);
    WS_GET(WSAIoctl);   WS_GET(bind);     WS_GET(sendto);
    WS_GET(recvfrom);   WS_GET(closesocket); WS_GET(WSAGetLastError);
#undef WS_GET
    if (g_ws_loaded < 0) {
        std::fprintf(stderr, "[comms:loopback] ws2_32.dll is missing an entry "
                     "point; staying idle\n");
        return false;
    }
    g_ws_loaded = 1;
    return true;
}

// ---------------------------------------------------------------------------
// THE WIRE
// ---------------------------------------------------------------------------
//
// One fixed 0x90-byte datagram in both directions. Fixed rather than variable
// because a fixed size makes the length check the entire frame check: a
// datagram that is not exactly kPacketBytes long is not ours, and there is no
// second parse that can disagree with the first.
//
// Every field is naturally aligned inside the struct, so there is no padding
// and no #pragma pack. The static assert below is what keeps that true.
//
// The layout is little-endian by construction. Both ends of a loopback pair
// are the same 32-bit MSVC binary on the same machine, so no byte swapping is
// correct here and adding it would be a fiction. THE DAY THIS CARRIER GROWS A
// REAL NETWORK it needs explicit LE accessors on the six header words; the
// 0x80 payload bytes are opaque and never need swapping at all.

enum : unsigned char { kWireVersion = 1 };

enum : unsigned char {
    kTypeJoin   = 1,   // child -> parent: I have bound slot N, let me in
    kTypeAccept = 2,   // parent -> child: you are in, here is the live mask
                       //                  and the round to start from
    kTypeBlocks = 3,   // child -> parent: my block for round R
                       // parent -> child: every slot's block for round R
    kTypeBye    = 4,   // either way: I am leaving
};

struct Packet {
    unsigned char magic[4];      // 0x00  'M','P','2','L'
    unsigned char version;       // 0x04
    unsigned char type;          // 0x05
    unsigned char slot;          // 0x06  the sender's slot
    unsigned char live;          // 0x07  live-slot bitmask; only slot 0's is
                                 //       authoritative, which is the contract's
                                 //       own "+0x0D player count honoured only
                                 //       from slot 0" rule kept at the header
    unsigned int  round;         // 0x08  the transport's round, not the ROM's
    unsigned int  have;          // 0x0C  which of the four payloads are valid
    unsigned char blocks[kCommsMaxPlayers][kCommsBlockBytes];   // 0x10 .. 0x90
};

enum : int { kPacketBytes = 0x90 };
// run mg16 lane MP4: the aux channel's tag, 'S','Y','N','1' as it sits on
// the wire. Distinct from kMagic so one socket can carry both kinds.
const unsigned kAuxMagicLE = 0x314e5953u;

static_assert(sizeof(Packet) == kPacketBytes,
              "the loopback wire packet grew padding; the length check is the "
              "frame check and it must stay exact");

const unsigned char kMagic[4] = { 'M', 'P', '2', 'L' };

// ---------------------------------------------------------------------------
// TIMERS
//
// Both are resend intervals, not timeouts. Nothing here times a session out;
// the ROM's own bound does that and drops to solo, which is the behaviour the
// contract asks a transport to leave alone.
// ---------------------------------------------------------------------------
enum : unsigned {
    kPublishResendMs = 4,    // republish an open round. exchange() may be
                             // called in a tight spin, so the wire rate is
                             // pinned here rather than to the call rate.
    kJoinResendMs    = 50,   // a child re-knocking on the parent's door
};

enum : int { kCacheDepth = 4 };   // parent's last four broadcasts

enum { kRoleNone = 0, kRoleParent = 1, kRoleChild = 2 };

// ---------------------------------------------------------------------------
// STATE
// ---------------------------------------------------------------------------
int      g_role       = kRoleNone;   // what the env asked for
bool     g_installed  = false;
bool     g_open       = false;
SOCKET   g_sock       = INVALID_SOCKET;
int      g_slot       = 0;

// run mg16 lane MP4: THE AUX QUEUE, one message deep.
//
// ONE DEEP ON PURPOSE. Aux carries the HOST's latest view of the world, so a
// backlog is worthless by definition -- if two arrive before the game reads
// one, the older is stale and the newer supersedes it completely. Queueing
// them would deliver a correction toward a position the host has already left.
// Overwriting is the correct policy for state, and it is the opposite of what
// the input records need (every one of those matters, which is why they have a
// four-deep cache). The two channels share a socket and nothing else.
enum : int { kAuxMaxBytes = 256 };
unsigned char g_aux[kAuxMaxBytes];
int      g_aux_len    = 0;
unsigned long long g_aux_superseded = 0;
int      g_port_base  = kCommsLoopbackPortBase;
int      g_my_port    = 0;
int      g_pinned     = -1;          // SM64DS_COMMS_SLOT, or -1
// Has the GAME asked to be parent yet? The socket is bound in open() and the
// role is requested a few statements later, in the same first tick, with a
// drain() in between -- so a JOIN can be handled while this is still false.
bool     g_parent_requested = false;
int      g_state      = kCommsIdle;
unsigned g_live       = 0;           // bit k set when slot k is live
unsigned g_round      = 0;

// The round being assembled, and the round already handed to the game.
unsigned char g_stage[kCommsMaxPlayers][kCommsBlockBytes];
unsigned      g_stage_mask   = 0;
unsigned char g_latched[kCommsMaxPlayers][kCommsBlockBytes];
unsigned      g_latched_mask = 0;
bool          g_round_done   = false;   // child: the parent's packet landed

// Parent only: the last kCacheDepth broadcasts, so a child whose copy of a
// round was lost can be re-served instead of deadlocking against a clock that
// has already moved on.
Packet g_cache[kCacheDepth];
bool   g_cache_valid[kCacheDepth];

unsigned g_last_publish_ms = 0;
unsigned g_last_join_ms    = 0;

unsigned long long g_sent = 0, g_recvd = 0, g_dropped = 0;
unsigned long long g_resends = 0, g_stale_serves = 0;
// run mg16 lane MP3: how many rounds the game walked away from (HOLE 5).
unsigned long long g_abandons = 0;

bool g_wsa_up = false;

unsigned now_ms() { return (unsigned)GetTickCount(); }

int popcount4(unsigned m) {
    int n = 0;
    for (int i = 0; i < kCommsMaxPlayers; ++i) if (m & (1u << i)) ++n;
    return n;
}

sockaddr_in slot_addr(int slot) {
    sockaddr_in a;
    std::memset(&a, 0, sizeof a);
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = hton32(INADDR_LOOPBACK);
    a.sin_port = hton16((unsigned short)(g_port_base + slot));
    return a;
}

void send_to_slot(const Packet &p, int slot) {
    if (g_sock == INVALID_SOCKET) return;
    const sockaddr_in a = slot_addr(slot);
    const int n = WS.sendto(g_sock, (const char *)&p, kPacketBytes, 0,
                            (const sockaddr *)&a, (int)sizeof a);
    if (n == kPacketBytes) ++g_sent;
}

void fill_header(Packet &p, unsigned char type) {
    std::memset(&p, 0, sizeof p);
    std::memcpy(p.magic, kMagic, 4);
    p.version = kWireVersion;
    p.type    = type;
    p.slot    = (unsigned char)g_slot;
    p.live    = (unsigned char)g_live;
    p.round   = g_round;
}

// ---------------------------------------------------------------------------
// RECEIVE
// ---------------------------------------------------------------------------

void on_parent_packet(const Packet &p, const sockaddr_in &from) {
    const int k = p.slot;
    if (k <= 0 || k >= kCommsMaxPlayers) { ++g_dropped; return; }

    // The slot is the port. A sender whose claimed slot disagrees with the
    // port it actually arrived from is not a peer of this session.
    if (ntoh16(from.sin_port) != (unsigned short)(g_port_base + k)) {
        ++g_dropped;
        return;
    }

    switch (p.type) {
    case kTypeJoin: {
        const bool fresh = (g_live & (1u << k)) == 0;
        g_live |= (1u << k);
        // ACCEPT IT EVEN IF THE GAME HAS NOT ASKED TO BE PARENT YET, but do not
        // fake the state. open() binds the socket and become_parent() is a few
        // statements later in the SAME first tick, with the drain() that
        // handles this packet in between -- so a child that reached its frame
        // loop first can land a JOIN in that window. Dropping it would be safe
        // (the child re-knocks every 50 ms) but it would also be a join thrown
        // away for no reason; recording it and letting become_parent()
        // reconcile loses nothing. What must NOT happen is advancing to
        // parent-connected before the game asked, which would report a session
        // the ROM never requested.
        if (g_parent_requested && g_state != kCommsParentConnected)
            g_state = kCommsParentConnected;
        // The accept carries the CURRENT round, so a child that joins mid
        // session adopts the parent's clock instead of starting at 0 and
        // asking for a round the cache retired long ago.
        Packet a;
        fill_header(a, kTypeAccept);
        a.live = (unsigned char)g_live;
        send_to_slot(a, k);
        if (fresh)
            std::fprintf(stderr, "[comms:loopback] slot %d joined at round %u; "
                         "live mask 0x%x, players %d\n",
                         k, g_round, g_live, popcount4(g_live));
        break;
    }
    case kTypeBlocks:
        if (p.round == g_round) {
            if (p.have & (1u << k)) {
                std::memcpy(g_stage[k], p.blocks[k], kCommsBlockBytes);
                g_stage_mask |= (1u << k);
            }
        } else if (p.round < g_round) {
            // The child never saw our broadcast for that round. Re-serve it
            // from the cache rather than let two clocks stare at each other.
            const int i = (int)(p.round % kCacheDepth);
            if (g_cache_valid[i] && g_cache[i].round == p.round) {
                send_to_slot(g_cache[i], k);
                ++g_stale_serves;
            }
        } else {
            // A child ahead of the parent cannot happen while the parent is
            // the clock: it needs our round-R broadcast to leave round R.
            ++g_dropped;
        }
        break;
    case kTypeBye:
        g_live &= ~(1u << k);
        g_stage_mask &= ~(1u << k);
        if (popcount4(g_live) <= 1) g_state = kCommsConnecting;
        std::fprintf(stderr, "[comms:loopback] slot %d left; live mask 0x%x\n",
                     k, g_live);
        break;
    default:
        ++g_dropped;
        break;
    }
}

void on_child_packet(const Packet &p, const sockaddr_in &from) {
    if (p.slot != 0) { ++g_dropped; return; }
    if (ntoh16(from.sin_port) != (unsigned short)g_port_base) {
        ++g_dropped;
        return;
    }

    switch (p.type) {
    case kTypeAccept:
        g_live = p.live;
        if (g_state != kCommsChildConnected) {
            g_round = p.round;          // adopt the parent's clock
            g_stage_mask = 0;
            g_round_done = false;
            g_state = kCommsChildConnected;
            std::fprintf(stderr, "[comms:loopback] accepted as slot %d at "
                         "round %u; live mask 0x%x, players %d\n",
                         g_slot, g_round, g_live, popcount4(g_live));
        }
        break;
    case kTypeBlocks:
        if (g_state != kCommsChildConnected) break;
        if (p.round != g_round) break;   // a round we already finished
        g_live = p.live;
        for (int i = 0; i < kCommsMaxPlayers; ++i) {
            if (p.have & (1u << i)) {
                std::memcpy(g_stage[i], p.blocks[i], kCommsBlockBytes);
                g_stage_mask |= (1u << i);
            }
        }
        g_round_done = true;
        break;
    case kTypeBye:
        std::fprintf(stderr, "[comms:loopback] the parent left; back to idle\n");
        g_state = kCommsIdle;
        g_live = 0;
        break;
    default:
        ++g_dropped;
        break;
    }
}

void drain() {
    if (g_sock == INVALID_SOCKET) return;
    for (;;) {
        /* ONE BUFFER, TWO MESSAGE KINDS. Run mg16 lane MP4: the aux channel
           shares this socket (one NAT mapping for internet play, ruled at the
           MP4 gate), so the read has to be big enough for either and the kinds
           are told apart by their first four bytes AFTER the read.

           THE OLD SHAPE WOULD HAVE MISCOUNTED THEM. This read asked for
           exactly kPacketBytes and charged anything else to g_dropped, so every
           aux message would have arrived, been discarded, and shown up in the
           carrier's drop counter -- a channel that works while its own
           instrument says the wire is failing. */
        union { Packet p; unsigned char raw[kAuxMaxBytes]; } msg;
        sockaddr_in from;
        int fromlen = (int)sizeof from;
        const int n = WS.recvfrom(g_sock, (char *)msg.raw, (int)sizeof msg.raw,
                                  0, (sockaddr *)&from, &fromlen);
        if (n < 0) break;                       // WSAEWOULDBLOCK, or nothing

        if (n >= 4 && std::memcmp(msg.raw, &kAuxMagicLE, 4) == 0) {
            /* Newest wins; see the queue's own note. A superseded message is
               counted rather than silently forgotten, because "sync looks
               laggy" and "sync is being outrun by its own send rate" are
               different problems and the counter is what tells them apart. */
            if (g_aux_len != 0) ++g_aux_superseded;
            const int keep = n < kAuxMaxBytes ? n : kAuxMaxBytes;
            std::memcpy(g_aux, msg.raw, (size_t)keep);
            g_aux_len = keep;
            continue;
        }

        Packet &p = msg.p;
        if (n != kPacketBytes) { ++g_dropped; continue; }
        if (std::memcmp(p.magic, kMagic, 4) != 0) { ++g_dropped; continue; }
        if (p.version != kWireVersion)          { ++g_dropped; continue; }
        ++g_recvd;
        if (g_role == kRoleParent) on_parent_packet(p, from);
        else                       on_child_packet(p, from);
    }
}

// ---------------------------------------------------------------------------
// SERVICE
//
// HOLE 1 WAS FOUND HERE AND IS NOW CLOSED. comms_seam.h says poll() is "called
// once per pump turn while the game is blocked" and that "a socket transport
// does its recv here". When this file was written NOTHING IN THE SEAM CALLED
// IT -- comms_set_transport refused a null poll at install and that was the
// only mention of the entry in the tree -- so a transport that took the
// sentence at its word would have received nothing, ever.
//
// Run mg16 lane MP3 closed it by fix (a): the seam grows the pump, because
// linking src/func_0203ea5c.c showed the ROM's wait SLEEPS rather than spins
// and hal/os_thread.h's hook was written for that exact call chain. poll() is
// now called once per turn of the ROM's own wait. See the frozen contract at
// the top of comms_seam.h.
//
// THIS CARRIER STILL SERVICES ITSELF FROM EVERY ENTRY, and that is deliberate
// rather than leftover: a transport that only works when someone remembers to
// pump it is fragile, and the servicing below is what makes the connect phase
// work before any wait loop is running to pump it.
//
// So this carrier services itself from EVERY entry it owns -- state(), slot(),
// player_count(), exchange() and poll() all land here first -- and is correct
// whether or not poll() is ever called. That is a workaround, not a fix; the
// fix is a contract decision and it is written up at the bottom of this file.
// ---------------------------------------------------------------------------
void service() {
    if (!g_open) return;
    drain();

    const unsigned t = now_ms();

    // A child that has not been accepted keeps knocking.
    if (g_role == kRoleChild && g_state == kCommsConnecting) {
        if ((unsigned)(t - g_last_join_ms) >= kJoinResendMs) {
            g_last_join_ms = t;
            Packet j;
            fill_header(j, kTypeJoin);
            send_to_slot(j, 0);
        }
    }

    // A child with an open round keeps republishing it until the parent's
    // answer lands. This is the whole of the retransmission story.
    if (g_role == kRoleChild && g_state == kCommsChildConnected &&
        !g_round_done && (g_stage_mask & (1u << g_slot)) != 0) {
        if ((unsigned)(t - g_last_publish_ms) >= kPublishResendMs) {
            g_last_publish_ms = t;
            Packet b;
            fill_header(b, kTypeBlocks);
            b.have = (1u << g_slot);
            std::memcpy(b.blocks[g_slot], g_stage[g_slot], kCommsBlockBytes);
            send_to_slot(b, 0);
            ++g_resends;
        }
    }
}

// ---------------------------------------------------------------------------
// THE TRANSPORT FACES
//
// PROPOSED CONTRACT CHANGE 2 is the lifecycle, which comms_seam.h gap 2 says
// is unspecified and that MP2 has to pin. What this carrier implements, and
// what the write-up at the bottom proposes freezing:
//
//   * state() before open() is kCommsIdle.
//   * become_parent() / become_child() BEFORE open() are refused and leave
//     state at idle. That is gap 3's intended answer applied to gap 2: the ROM
//     already handles a radio that never comes up.
//   * a second open() while open is a no-op that keeps the live socket.
//   * close() when not open is legal and does nothing. Double close is legal.
//   * slot() and player_count() answer 0 and 1 before a link exists, which is
//     what the seam's own solo path answers.
//   * exchange() before connected returns 0 and writes 0 through `status`.
//   * peer_block() before a completed round returns null.
// ---------------------------------------------------------------------------

void lb_open(unsigned mode) {
    if (g_open) {
        // Documented no-op rather than a reopen: the ROM's own init path can
        // reach func_020408b0 more than once and a reopen would drop a live
        // session on the floor.
        std::fprintf(stderr, "[comms:loopback] open(mode=%u) while already "
                     "open on port %d; keeping the live socket\n",
                     mode, g_my_port);
        return;
    }

    if (!ws_load()) return;

    if (!g_wsa_up) {
        WSADATA wsa;
        if (WS.WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            std::fprintf(stderr, "[comms:loopback] WSAStartup failed; staying "
                         "idle so the ROM's own bound fires and drops to "
                         "solo\n");
            return;
        }
        g_wsa_up = true;   // left up for the process lifetime on purpose: a
                           // WSACleanup here would tear down Winsock under any
                           // other user of it, and process exit does the job.
    }

    g_sock = WS.socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (g_sock == INVALID_SOCKET) {
        std::fprintf(stderr, "[comms:loopback] socket() failed (%d); staying "
                     "idle\n", WS.WSAGetLastError());
        return;
    }

    u_long nb = 1;
    WS.ioctlsocket(g_sock, FIONBIO, &nb);

    // Windows turns an ICMP port-unreachable from a peer that is not up yet
    // into WSAECONNRESET on the NEXT recvfrom of an unconnected UDP socket,
    // which would show up as a phantom receive error every time a child starts
    // before its parent. SIO_UDP_CONNRESET off is the documented way to make
    // the socket behave the way the rest of the world's UDP does.
    {
        DWORD off = 0, ret = 0;
        WS.WSAIoctl(g_sock, SIO_UDP_CONNRESET, &off, sizeof off, 0, 0, &ret,
                    0, 0);
    }

    // Bind. NO SO_REUSEADDR: a bind that fails is the session telling us
    // someone already holds that slot, and that answer is load-bearing.
    int first = 0, last = 0;
    if (g_role == kRoleParent)      { first = 0; last = 0; }
    else if (g_pinned >= 0)         { first = g_pinned; last = g_pinned; }
    else                            { first = 1; last = kCommsMaxPlayers - 1; }

    int bound = -1;
    for (int k = first; k <= last; ++k) {
        sockaddr_in a = slot_addr(k);
        if (WS.bind(g_sock, (const sockaddr *)&a, (int)sizeof a) == 0) {
            bound = k;
            break;
        }
    }

    if (bound < 0) {
        const int e = WS.WSAGetLastError();
        std::fprintf(stderr,
            "[comms:loopback] could not bind %s port%s %d..%d (winsock %d). "
            "%s Staying idle, which leaves the ROM's own wait bound to expire "
            "and its own solo fallback to run.\n",
            g_role == kRoleParent ? "parent" : "child",
            first == last ? "" : "s", g_port_base + first, g_port_base + last,
            e,
            g_role == kRoleParent
                ? "Another copy of the game is probably already the parent."
                : "Every child slot is probably already taken.");
        WS.closesocket(g_sock);
        g_sock = INVALID_SOCKET;
        return;
    }

    g_slot    = bound;
    g_my_port = g_port_base + bound;
    g_open    = true;
    g_parent_requested = false;
    g_state   = kCommsIdle;      // open() does not connect anything
    g_live    = 0;
    g_round   = 0;
    g_stage_mask = 0;
    g_latched_mask = 0;
    g_round_done = false;
    std::memset(g_cache_valid, 0, sizeof g_cache_valid);

    std::fprintf(stderr, "[comms:loopback] open(mode=%u) as %s, slot %d, "
                 "udp 127.0.0.1:%d (parent at :%d)\n",
                 mode, g_role == kRoleParent ? "parent" : "child",
                 g_slot, g_my_port, g_port_base);
}

void lb_close() {
    if (!g_open) return;             // double close is legal and silent
    Packet b;
    fill_header(b, kTypeBye);
    if (g_role == kRoleParent) {
        for (int k = 1; k < kCommsMaxPlayers; ++k)
            if (g_live & (1u << k)) send_to_slot(b, k);
    } else {
        send_to_slot(b, 0);
    }
    WS.closesocket(g_sock);
    g_sock  = INVALID_SOCKET;
    g_open  = false;
    g_state = kCommsIdle;
    g_live  = 0;
    g_slot  = 0;
    g_parent_requested = false;
    g_latched_mask = 0;
    g_stage_mask   = 0;
    std::fprintf(stderr, "[comms:loopback] closed after %u rounds\n", g_round);
}

void lb_become_parent() {
    if (!g_open) {
        std::fprintf(stderr, "[comms:loopback] become_parent() before open(); "
                     "refused, state stays idle\n");
        return;
    }
    if (g_role != kRoleParent) {
        // The env decided which port this process bound, so it decided the
        // role. A ROM that asks for the other one is a mismatch the launcher
        // made, and the honest answer is the contract's: stay idle.
        std::fprintf(stderr, "[comms:loopback] the game asked to be PARENT but "
                     "SM64DS_COMMS_ROLE=child bound a child port; refused, "
                     "state stays idle\n");
        return;
    }
    g_slot  = 0;
    g_parent_requested = true;
    // |= NOT =. A plain assignment here WIPES a child that already joined in
    // the window described at kTypeJoin, and the failure it produces is
    // permanent and asymmetric: the child has its accept, believes it is in,
    // and stops knocking, while the parent sits at link 2 with players=1 and
    // role never leaving 0. Reconcile instead: keep whoever is already in, and
    // if that is anybody, this session is already up.
    g_live |= 1u;                       // the parent is always live
    g_state = (g_live & ~1u) ? kCommsParentConnected : kCommsConnecting;
}

void lb_become_child() {
    if (!g_open) {
        std::fprintf(stderr, "[comms:loopback] become_child() before open(); "
                     "refused, state stays idle\n");
        return;
    }
    if (g_role != kRoleChild) {
        std::fprintf(stderr, "[comms:loopback] the game asked to be CHILD but "
                     "SM64DS_COMMS_ROLE=parent bound the parent port; refused, "
                     "state stays idle\n");
        return;
    }
    g_state = kCommsConnecting;
    g_last_join_ms = now_ms() - kJoinResendMs;   // knock immediately
    service();
}

int lb_state() { service(); return g_state; }

int lb_slot() {
    service();
    return (g_state == kCommsParentConnected || g_state == kCommsChildConnected)
               ? g_slot : 0;
}

int lb_player_count() {
    service();
    const int n = popcount4(g_live);
    return n < 1 ? 1 : n;
}

int lb_exchange(const void *my_block, uint16_t *status) {
    // GAP 1, honoured literally. comms_seam.h: "`status`: WRITE 0." The ROM's
    // own caller stores what it reads back in a stack local (sp2e) and never
    // reads it, so 0 is the entire contract.
    if (status) *status = 0;

    service();

    if (g_state != kCommsParentConnected && g_state != kCommsChildConnected)
        return 0;
    if (!my_block) return 0;

    // Stage my own block for the open round. Copied, never inspected.
    std::memcpy(g_stage[g_slot], my_block, kCommsBlockBytes);
    const bool first_publish = (g_stage_mask & (1u << g_slot)) == 0;
    g_stage_mask |= (1u << g_slot);

    if (g_role == kRoleChild) {
        if (first_publish) {
            Packet b;
            fill_header(b, kTypeBlocks);
            b.have = (1u << g_slot);
            std::memcpy(b.blocks[g_slot], g_stage[g_slot], kCommsBlockBytes);
            send_to_slot(b, 0);
            g_last_publish_ms = now_ms();
        }
        service();                       // the answer may already be here
        if (!g_round_done) return 0;
    } else {
        // The parent completes when every live slot's block for this round is
        // in. Its own is in by definition, one line above.
        if ((g_stage_mask & g_live) != g_live) return 0;

        // Broadcast the assembled round, and keep a copy so a child whose
        // datagram was lost can be re-served.
        Packet b;
        fill_header(b, kTypeBlocks);
        b.have = g_stage_mask & g_live;
        for (int i = 0; i < kCommsMaxPlayers; ++i)
            if (b.have & (1u << i))
                std::memcpy(b.blocks[i], g_stage[i], kCommsBlockBytes);
        for (int k = 1; k < kCommsMaxPlayers; ++k)
            if (g_live & (1u << k)) send_to_slot(b, k);
        const int ci = (int)(g_round % kCacheDepth);
        g_cache[ci] = b;
        g_cache_valid[ci] = true;
    }

    // The round is in. Latch it where peer_block() can read it, and only here:
    // the contract says a peer pointer "must stay valid until the next
    // exchange() call", and latching on completion alone is stricter than that
    // -- an incomplete exchange() never disturbs the last good round.
    std::memcpy(g_latched, g_stage, sizeof g_latched);
    g_latched_mask = g_stage_mask & g_live;

    ++g_round;
    g_stage_mask = 0;
    g_round_done = false;
    return 1;
}

const void *lb_peer_block(int aid) {
    if (aid < 0 || aid >= kCommsMaxPlayers) return 0;
    if ((g_latched_mask & (1u << aid)) == 0) return 0;
    return g_latched[aid];
}

void lb_poll() { service(); }

// THE GAME WALKED AWAY FROM THE OPEN ROUND. Run mg16 lane MP3; HOLE 5's entry.
//
// src/func_0203ea5c.c:487 drops the session back to solo when the ROM's wait
// bound runs out, and until this entry existed that was invisible here: the
// carrier kept holding a round nobody would ever complete, and its round
// counter stayed permanently one ahead of the peer's. The write-up at the
// bottom of this file predicted a four-deep broadcast cache would heal a single
// abandoned round and that repeated ones would walk the clocks apart faster
// than the cache could close. Rung 7 is what that looks like from outside: a
// child that starts before the parent spends its whole wait bound knocking at
// a port nobody has bound yet, gives up, and never joins.
//
// The reset is deliberately small. The SESSION is not being torn down -- the
// socket stays bound, the live mask stays as the peers last reported it, and
// the role does not change -- because the game has not left, it has stopped
// waiting for this one frame. What goes is the ROUND: the latched blocks and
// the mask that says which of them are fresh, so the next exchange() starts
// clean instead of trying to finish a frame the game has forgotten.
// ===========================================================================
// THE AUX CHANNEL -- contract v2. Run mg16 lane MP4.
//
// ONE SOCKET, MULTIPLEXED BY MESSAGE KIND, ruled at the MP4 gate. The input
// records and the aux messages share g_sock and are told apart by their first
// four bytes. The reasoning is internet play: one socket is one NAT mapping,
// and a second port per instance multiplies hole-punching. Head-of-line
// blocking is not a real risk at these sizes -- a four-player sync message is
// 136 bytes, nothing fragments, and UDP datagrams are independent.
//
// THE ORDERING RULE IS A REQUIREMENT, NOT ADVICE: the input record goes out
// FIRST on every pump and aux after it, always. The lockstep is what the game
// blocks on. Enforced by construction here -- lb_send_aux is only ever called
// from the sync layer, which runs after the conductor's exchange has returned
// -- and asserted from outside by rung SY6, which measures that input round
// times with sync on match sync off within noise.
//
// A ONE-DATAGRAM RULE, and it is why this returns 0 rather than fragmenting: a
// message that does not fit is a bug in the caller, and silently splitting it
// would turn an unreliable-but-whole channel into an unreliable-and-partial
// one, which is much harder to reason about.
// ===========================================================================
/* RETURNS len IF AT LEAST ONE LIVE PEER TOOK IT, else 0.
 *
 * The contract says "the bytes accepted", which is unambiguous for a
 * point-to-point transport and needs saying for this one: a loopback carrier
 * fans a message to every live peer, so there is no single "accepted" count.
 * Reporting len on a partial fan-out is the honest choice for an UNRELIABLE
 * channel -- a peer that missed this message is exactly the case the channel
 * already tolerates, and returning 0 there would make the caller treat a
 * mostly-delivered broadcast as a total failure. */
int lb_send_aux(const void *buf, int len) {
    if (!buf || len <= 0) return 0;
    if (g_sock == INVALID_SOCKET) return 0;
    if (len > kAuxMaxBytes) return 0;          // one datagram or nothing

    int sent = 0;
    for (int i = 0; i < kCommsMaxPlayers; ++i) {
        if (i == g_slot) continue;
        if ((g_live & (1u << i)) == 0) continue;
        /* slot_addr, not a hand-built sockaddr: this file reaches ws2_32
           through the WS wrapper and its own hton helpers ON PURPOSE. A STATIC
           ws2_32 import breaks the port's fixed-range reservation, because the
           loader resolves imports before the TLS callback that claims
           0x02000000 -- banked in MP2's traps, and htonl/htons are imports like
           any other. Reusing the existing helper keeps that property. */
        sockaddr_in to = slot_addr(i);
        const int n = WS.sendto(g_sock, (const char *)buf, len, 0,
                                (sockaddr *)&to, (int)sizeof to);
        if (n == len) sent = len;
    }
    return sent;
}

int lb_recv_aux(void *buf, int cap) {
    if (!buf || cap <= 0) return 0;
    service();                      // drain; aux messages land in the queue
    if (g_aux_len <= 0) return 0;
    const int n = g_aux_len < cap ? g_aux_len : cap;
    std::memcpy(buf, g_aux, (size_t)n);
    g_aux_len = 0;
    return n;
}

void lb_abandon() {
    if (g_latched_mask == 0 && g_stage_mask == 0) return;   // nothing open
    std::fprintf(stderr,
                 "[comms:loopback] the game abandoned round %lu (the ROM's "
                 "wait bound expired and src/func_0203ea5c.c:487 dropped it to "
                 "solo); clearing the open round, keeping the session\n",
                 (unsigned long)g_round);
    g_latched_mask = 0;
    g_stage_mask = 0;
    ++g_abandons;
}

const CommsTransport kLoopback = {
    "loopback (udp 127.0.0.1)",
    lb_open,
    lb_close,
    lb_become_parent,
    lb_become_child,
    lb_state,
    lb_slot,
    lb_player_count,
    lb_exchange,
    lb_peer_block,
    lb_poll,
    lb_abandon,
    lb_send_aux,
    lb_recv_aux,
    kCommsContractV2,
};

}  // namespace

// ---------------------------------------------------------------------------
// INSTALL
// ---------------------------------------------------------------------------

bool comms_loopback_install_from_env() {
    if (g_installed) return true;

    const char *role = std::getenv("SM64DS_COMMS_ROLE");
    if (!role || !*role) return false;      // nothing installed, nothing moved

    if (std::strcmp(role, "parent") == 0)      g_role = kRoleParent;
    else if (std::strcmp(role, "child") == 0)  g_role = kRoleChild;
    else {
        std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_ROLE='%s' is not "
                     "'parent' or 'child'; nothing installed, the seam keeps "
                     "its solo answers\n", role);
        return false;
    }

    if (const char *p = std::getenv("SM64DS_COMMS_PORT")) {
        const int v = std::atoi(p);
        // Leave room for kCommsMaxPlayers consecutive ports below 65536.
        if (v > 1024 && v < 65536 - kCommsMaxPlayers) g_port_base = v;
        else std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_PORT=%s out "
                          "of range; using %d\n", p, g_port_base);
    }

    if (const char *s = std::getenv("SM64DS_COMMS_SLOT")) {
        const int v = std::atoi(s);
        if (g_role != kRoleChild)
            std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_SLOT is a "
                         "child knob; ignored for the parent, which is always "
                         "slot 0\n");
        else if (v >= 1 && v < kCommsMaxPlayers) g_pinned = v;
        else std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_SLOT=%s out "
                          "of range 1..%d; claiming the first free slot "
                          "instead\n", s, kCommsMaxPlayers - 1);
    }

    if (!comms_set_transport(&kLoopback)) return false;
    g_installed = true;

    // run mg16 lane MP3: SEAT THE SESSION REQUEST AND INSTALL THE PUMP.
    //
    // Installing a transport used to be the whole of it, because MP2's
    // transcription drove the seam faces itself. The ROM's own conductor is
    // linked now and it dispatches on data_020a0f04, which nothing here was
    // setting -- so two carriers would come up, connect to nothing, and report
    // role=0 forever. On the DS that byte is seated by the multiplayer menu;
    // this launcher-side install is what stands in for it. See HOLE 3 in the
    // frozen contract.
    //
    // The pump goes in here too rather than at seam-install time, because a
    // transport is the only thing that gives poll() anything to do.
    comms_seat_session_request(g_role == kRoleParent ? kCommsRoleParent
                                                     : kCommsRoleChild);
    comms_install_pump();

    // SAY GOODBYE ON THE WAY OUT.
    //
    // On the DS the seam's close() face has exactly one caller,
    // src/func_0203df40.c:70, on the tick after the role byte drops to 0. That
    // TU is in no slice, so in this port NOTHING calls func_02040724 and a
    // process that simply exits leaves its peers holding it in their live mask
    // until their own wait bound expires -- every remaining peer stalls for a
    // full bound, once, for no reason.
    //
    // atexit is the honest hook for it here: it is the port's own shutdown, it
    // runs before the CRT tears down, and closesocket is safe there. It routes
    // through the SEAM FACE rather than lb_close directly so the call is the
    // same one the ROM would make.
    //
    // This is also what makes the departed-player path reachable at all: the
    // Bye is what clears the leaver out of the parent's live mask, which lets
    // the parent complete another round, which is when the ROM's per-record
    // clear at src/func_0203ea5c.c:275 wipes the leaver's stale live bit.
    std::atexit([]() { func_02040724(); });
    std::fprintf(stderr, "[comms:loopback] installed as %s, port base %d\n",
                 g_role == kRoleParent ? "parent" : "child", g_port_base);
    return true;
}

CommsLoopbackStats comms_loopback_stats() {
    CommsLoopbackStats s;
    s.installed    = g_installed;
    s.role         = g_role;
    s.slot         = g_slot;
    s.port         = g_my_port;
    s.live_mask    = (int)g_live;
    s.round        = g_round;
    s.sent         = g_sent;
    s.recvd        = g_recvd;
    s.dropped      = g_dropped;
    s.resends      = g_resends;
    s.stale_serves = g_stale_serves;
    return s;
}

void comms_loopback_report(const char *tag) {
    const CommsLoopbackStats s = comms_loopback_stats();
    std::fprintf(stderr,
        "[loopback:%s] role=%s slot=%d port=%d live=0x%x round=%lu "
        "sent=%llu recvd=%llu dropped=%llu resends=%llu stale=%llu\n",
        tag ? tag : "-",
        s.role == kRoleParent ? "parent" : (s.role == kRoleChild ? "child"
                                                                 : "none"),
        s.slot, s.port, s.live_mask, s.round,
        s.sent, s.recvd, s.dropped, s.resends, s.stale_serves);
}

}  // namespace port

// ===========================================================================
// ###########################################################################
// #  HISTORICAL. EVERY PROPOSAL BELOW HAS BEEN DECIDED.                     #
// #                                                                         #
// #  The contract froze in run mg16 lane MP3, once src/func_0203ea5c.c was  #
// #  linked and the ROM's own caller drove the seam. THE AUTHORITY IS THE   #
// #  BANNER AT THE TOP OF hal/comms_seam.h -- read that, not this. What     #
// #  follows is kept because it is the measurement each decision rests on,  #
// #  and a frozen contract with no record of why is worse than none.        #
// #                                                                         #
// #  Outcomes, so nobody has to reconstruct them:                           #
// #    HOLE 1 poll never called   CLOSED by fix (a), the seam grew the pump #
// #    HOLE 2 lifecycle           CLOSED exactly as proposed                #
// #    HOLE 3 role decided twice  CLOSED, but the real caller REFRAMED it   #
// #    HOLE 4 no round id         CLOSED as proposed                        #
// #    HOLE 5 abandon()           CLOSED, entry added; it had more teeth    #
// #    GAP 1 status out-param     no change, write 0                        #
// #    GAP 4 WM status word       stays seam-owned                          #
// #                                                                         #
// #  TWO DID NOT GO AS PROPOSED. HOLE 3's "who wins" turned out not to be   #
// #  the question -- the ROM asks for no role until something seats         #
// #  data_020a0f04 -- and HOLE 5's drop turned out to be PERMANENT rather   #
// #  than merely desynchronising, so the seam re-seats as well as telling   #
// #  the transport. Both are written up at the freeze.                      #
// ###########################################################################
//
// WHAT WRITING THIS FOUND: THE PROPOSALS, AS THEY WERE PUT (now all decided)
//
// comms_seam.h says the contract freezes at the end of MP2, when a loopback
// transport has actually driven it. This is that transport, and these are the
// places the PROVISIONAL contract turned out to be wrong or silent. Each one
// is a proposal for the freeze, not a change made here.
//
// ---------------------------------------------------------------------------
// HOLE 1 (NEW). poll() IS REQUIRED AND NEVER CALLED.
//
//   The header: "`poll` is the only entry called while the game is inside the
//   ROM's lockstep wait, and it is called once per pump turn"; and on the entry
//   itself, "A socket transport does its recv here." comms_seam.cpp:226 says
//   the same in prose: "it is polled from the seam's own pump".
//
//   MEASURED: there is no pump in the seam. `->poll` appears exactly once in
//   the whole tree, in the null check at comms_seam.cpp:52. comms_set_transport
//   REFUSES a transport whose poll is null and then never calls it. A transport
//   author who followed the sentence would ship a carrier that never receives.
//
//   This file works around it by servicing from every entry it owns. That is
//   defensible on its own -- a transport that only works when someone else
//   remembers to pump it is fragile -- but the contract should not require an
//   entry it does not drive.
//
//   PROPOSE, one of:
//     (a) the seam grows the pump the prose already claims. hal/os_thread.h
//         already has exactly the right hook: port::thread_set_pump installs a
//         ThreadPump called once per turn while OS_SleepThread blocks, which IS
//         the "pump turn" the header names. The seam would install a pump that
//         calls t->poll() and chains whatever pump was there. This is the
//         option that makes the sentence true.
//     (b) poll() becomes OPTIONAL -- droppable from the null check -- and the
//         contract says plainly that a transport must be able to service itself
//         from its other entries. Smaller change, weaker guarantee, and it
//         makes exchange() the de facto pump.
//   Either is fine. Requiring an entry nothing calls is not.
//
// ---------------------------------------------------------------------------
// HOLE 2 (gap 2 in the header, now answerable). THE LIFECYCLE.
//
//   The header asks: must open() precede become_parent()? What does state()
//   return before open()? Is a double close() legal?
//
//   PROPOSE freezing what this file implements, because each answer falls out
//   of the ROM's own behaviour rather than out of convenience:
//
//     open() before either become_*(). state() is kCommsIdle before open().
//     become_parent()/become_child() before open() are REFUSED and leave state
//     idle -- gap 3's answer, which the header already calls the right shape:
//     the ROM handles a radio that never comes up, so "leave it idle and let
//     the ROM's bound fire" is a complete answer to every lifecycle misuse.
//     A second open() while open is a NO-OP that keeps the live socket, not a
//     reopen, because the ROM's init path can reach func_020408b0 twice and a
//     reopen would drop a live session.
//     close() when never opened is legal and silent. Double close is legal.
//     slot() is 0 and player_count() is 1 before a link exists.
//     exchange() before connected returns 0 and writes 0 through status.
//     peer_block() before any completed round returns null.
//
// ---------------------------------------------------------------------------
// HOLE 3 (NEW). THE ROLE IS DECIDED TWICE AND THE CONTRACT DOES NOT SAY WHO
// WINS.
//
//   A transport binds a socket in open(), and for a loopback carrier the ROLE
//   decides which port it binds -- so the transport must know parent-or-child
//   BEFORE become_parent()/become_child() tell it. Here the env decides and the
//   ROM's request must agree; a mismatch is refused and left idle.
//
//   PROPOSE the contract say explicitly that a transport MAY be pre-configured
//   with a role, that become_parent()/become_child() are a REQUEST the
//   transport may refuse, and that refusal is expressed the only way the
//   contract has: state() stays idle. Alternatively give open()'s `mode` a
//   defined role meaning -- but the ROM passes 2 unconditionally, so that
//   would be inventing a value the game does not send.
//
// ---------------------------------------------------------------------------
// HOLE 4 (NEW). "THIS FRAME" IN exchange() HAS NO IDENTIFIER.
//
//   "answer whether every live peer's block for THIS frame has arrived." The
//   transport is given no round id, and it must not read one out of the block
//   (+0x02 is the ROM's frame counter, and reading it would be reinterpreting
//   the bytes the contract forbids touching). So every transport has to invent
//   its own round counter and infer round boundaries from the return value:
//   the round advances when, and only when, exchange() returns 1.
//
//   That inference is sound only if exchange() is called repeatedly for ONE
//   round until it returns 1, and never called for a NEW round while the old
//   one is still open. That is what the ROM does, but the contract does not say
//   it, and it is the single assumption every transport will bet on.
//
//   PROPOSE the contract state it directly: "exchange() is polled with the same
//   logical frame until it returns 1. The transport may treat the transition to
//   1 as the round boundary; the caller must not begin a new frame's exchange
//   before the previous one has completed or the ROM's own wait bound has
//   expired." The bound half matters: when the ROM gives up and drops to solo,
//   the transport's round and the ROM's frame have silently diverged, and
//   nothing today tells the transport that happened.
//
// ---------------------------------------------------------------------------
// HOLE 5 (NEW, and the one with teeth). NOTHING TELLS A TRANSPORT THE SESSION
// WAS ABANDONED.
//
//   src/func_0203ea5c.c:487 drops the session back to solo when the wait bound
//   at :142-146 runs out. From the transport's side that is invisible: no
//   close(), no state change requested, no notification. The carrier is still
//   holding an open round the game has walked away from, and its round counter
//   is now permanently one ahead of the peer's.
//
//   This carrier survives it by accident rather than by design -- the parent's
//   four-deep broadcast cache re-serves a lagging child, so a single abandoned
//   round heals -- but a bound that expires repeatedly walks the two clocks
//   apart faster than a four-deep cache can close.
//
//   PROPOSE a single new entry, `void (*abandon)()`, called by the seam where
//   the ROM drops to solo, meaning "forget the open round". It is the smallest
//   possible fix and it is the difference between a carrier that resynchronises
//   and one that needs a session restart. If a new entry is unwelcome, the same
//   information can ride on close() being called on the drop -- but close()
//   currently means "leave", and overloading it would make a reconnect
//   impossible to distinguish from a fresh join.
//
// ---------------------------------------------------------------------------
// GAPS 1 AND 4 FROM THE HEADER, as this transport found them:
//
//   GAP 1 (status out-param): NO CHANGE NEEDED. Writing 0 cost nothing and
//   wanted nothing. Propose the header keep the parameter and say in the
//   field's own comment that it is write-0, rather than giving it a job.
//
//   GAP 4 (the WM status word): LEAVE IT SEAM-OWNED. func_02040a5c/
//   func_02040a84 never came up while writing this carrier -- a transport has
//   no opinion about that word, and moving it into CommsTransport would add a
//   required entry with no implementation to give it. Propose closing gap 4 as
//   "stays seam-owned", which is a decision rather than a change.
// ===========================================================================
