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
    // ADDRESS MODE ONLY. Both are looked up with the rest of the table, and
    // both are allowed to be missing: a build of Windows without them still
    // runs the loopback carrier, which is the default and must not acquire a
    // new way to fail. resolve() checks the pointer before it calls.
    unsigned long (WSAAPI *inet_addr)(const char *);
    struct hostent *(WSAAPI *gethostbyname)(const char *);
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
    // OPTIONAL ENTRIES, looked up WITHOUT the g_ws_loaded refusal above: these
    // two are needed only to turn a typed address into bytes, which only the
    // address modes ever ask for. A missing one must not take the loopback
    // carrier down with it.
    *(FARPROC *)&WS.inet_addr      = ::GetProcAddress(m, "inet_addr");
    *(FARPROC *)&WS.gethostbyname  = ::GetProcAddress(m, "gethostbyname");
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
// run mg16 lane MP4: the aux channel's tags as they sit on the wire, distinct
// from kMagic so one socket can carry every kind. The carrier CLASSIFIES on
// these four bytes and never reads past them -- classification is not
// reinterpretation. 'SYN1' is the sync layer's state snapshot; 'SYNP' and
// 'SYNQ' (mp-sync-coopdx item 6) are its RTT probe and the echo.
//
// PING AND PONG ARE SEPARATE KINDS, and that is measured rather than tidy:
// with one shared tag, a peer's echo of OUR probe and that peer's own next
// probe landed in the same one-deep slot, and at delay 0 the two are
// phase-locked to the frame boundary -- the readout lost roughly half its
// RTT samples to newest-wins (37 of 80 one way, 52 of 80 the other, measured
// on the first rig session). Distinct kinds cannot supersede each other.
const unsigned kAuxMagicLE = 0x314e5953u;   // 'S','Y','N','1'
const unsigned kAuxPingLE  = 0x504e5953u;   // 'S','Y','N','P'
const unsigned kAuxPongLE  = 0x514e5953u;   // 'S','Y','N','Q'

static_assert(sizeof(Packet) == kPacketBytes,
              "the loopback wire packet grew padding; the length check is the "
              "frame check and it must stay exact");

const unsigned char kMagic[4] = { 'M', 'P', '2', 'L' };

// ===========================================================================
// ADDRESS MODES: the same carrier, pointed somewhere other than 127.0.0.1.
// Run vsdec, lane NET.
//
// THE ONE THING that stood between this file and the internet was never the
// socket -- it was that PEER IDENTITY WAS DERIVED FROM THE SOURCE PORT. Three
// places did it: slot_addr() built every destination out of `base + slot`,
// on_parent_packet/on_child_packet refused a datagram whose claimed slot
// disagreed with the port it came from, and drain()'s aux classifier named its
// sender `ntoh16(from.sin_port) - g_port_base`. That rule is EXACTLY RIGHT on
// loopback (four processes on one machine, ports they each bound, no spoofing
// surface worth the name) and impossible anywhere else, because a peer across
// the internet binds whatever port it likes and a NAT rewrites it anyway.
//
// So the rule becomes MODE-DEPENDENT, and the loopback mode keeps the old
// arithmetic UNCHANGED -- that is the whole of the "nothing that works today
// moves" promise on this side:
//
//   kNetLoopback  the default, and what runs when none of the new env is set.
//                 Destinations are 127.0.0.1:base+k; the sender is the source
//                 port. Byte-for-byte the pre-NET behaviour.
//   kNetDirect    SM64DS_COMMS_HOST=<ip[:port]>. A child sends to that address
//                 instead of loopback; a parent LEARNS each child's address
//                 from the JOIN that arrives. The sender is the SOURCE ADDRESS
//                 matched against the learned table.
//   kNetRelay     SM64DS_COMMS_RELAY=<host[:port]> + SM64DS_COMMS_CODE. Every
//                 datagram goes to the relay, and every datagram arrives FROM
//                 the relay, so the source address identifies nobody. The
//                 sender is the packet's own `slot` header field, which the
//                 wire has carried since MP2 and which nothing needed until
//                 now.
// ===========================================================================
enum : int { kNetLoopback = 0, kNetDirect = 1, kNetRelay = 2 };
int g_net_mode = kNetLoopback;
bool g_bind_any = false;         // SM64DS_COMMS_BIND_ANY, plus the cases below

// The default relay port, matching the staging service the RELAY lane deploys.
enum : int { kRelayDefaultPort = 41234 };

// ---------------------------------------------------------------------------
// THE RELAY HANDSHAKE -- A FROZEN WIRE CONTRACT, implemented exactly.
//
// The relay is a rendezvous, not a peer: it learns which public address each
// end is really coming from (which neither end can know behind a NAT) and
// forwards between them. It is the no-port-forwarding path, and it is the only
// one that works for two ordinary home connections.
//
//   HELLO      16 bytes, client -> relay. "SMRC", version 1, role (0 parent,
//              1 child), 2 reserved zero bytes, then 8 bytes of session code,
//              ASCII, right-padded with NUL.
//   HELLO-ACK  16 bytes, relay -> client. "SMRA", version 1, status (0 ok,
//              1 session full, 2 bad request), 2 reserved, the code echoed.
//
// The client repeats HELLO ONCE PER SECOND UNTIL ACKED, which is also what
// punches and then holds the NAT mapping open before there is any game traffic
// to do it. After a parent and at least one child share a code the relay
// forwards every later datagram VERBATIM -- parent to all children, child to
// parent only. That star is the DS's own parent/child radio model, which is
// why the game's packets need no envelope and no rewriting: the shape the ROM
// already assumes is the shape the relay already implements.
//
// Endpoints expire after 90 s idle and a re-HELLO is explicitly allowed, so
// this carrier keeps sending one every kRelayKeepaliveMs forever rather than
// stopping at the first ACK. Handled idempotently on the game side: an ACK
// that arrives when already paired is counted and otherwise ignored.
// ---------------------------------------------------------------------------
enum : int { kRelayMsgBytes = 16, kRelayCodeBytes = 8 };
enum : unsigned char { kRelayVersion = 1 };
enum : unsigned char { kRelayRoleParent = 0, kRelayRoleChild = 1 };
enum : unsigned char {
    kRelayStatusOk       = 0,
    kRelayStatusFull     = 1,
    kRelayStatusBadReq   = 2,
};

const unsigned char kRelayHelloMagic[4] = { 'S', 'M', 'R', 'C' };
const unsigned char kRelayAckMagic[4]   = { 'S', 'M', 'R', 'A' };

#pragma pack(push, 1)
struct RelayHello {
    unsigned char magic[4];                 // "SMRC" out, "SMRA" back
    unsigned char version;                  // 1
    unsigned char role_or_status;           // role on the way out, status back
    unsigned char reserved[2];              // zero
    unsigned char code[kRelayCodeBytes];    // ASCII, NUL right-padded
};
#pragma pack(pop)
static_assert(sizeof(RelayHello) == kRelayMsgBytes,
              "the relay handshake datagram is a frozen 16 bytes");

// The relay caps a forwarded payload at 700 bytes. Both of this carrier's
// kinds are far inside that (0x90 lockstep, 256 aux), and the assert is here
// so a later wire change cannot quietly cross the line and get truncated by a
// service that has no way to tell the game about it.
// (kAuxMaxBytes gets the same assert where it is defined, below.)
enum : int { kRelayMaxPayload = 700 };
static_assert(kPacketBytes <= kRelayMaxPayload,
              "the lockstep datagram no longer fits the relay's payload cap");

// AND THE OTHER HALF OF THE RELAY'S CONTRACT, which is a rule about what the
// GAME may send rather than about what the relay does: a forwarded datagram
// must never be exactly 16 bytes beginning with "SMRC", because the relay
// would read it as a HELLO and swallow it instead of forwarding it.
//
// Satisfied here without any padding, and worth writing down so it stays
// satisfied. The lockstep packet is 144 bytes, so it cannot collide on length.
// The aux channel CAN produce a 16-byte datagram -- a 'SYN1' snapshot carrying
// zero player entries is exactly its 16-byte header -- but every aux kind
// begins with its own four-byte tag ('SYN1', 'SYNP', 'SYNQ'), none of which is
// "SMRC". The collision needs BOTH conditions and no shape in this file meets
// both. A NEW aux kind whose tag is picked carelessly could, so: the tag
// space is 'SYN*' and must stay there.
static_assert(kPacketBytes != kRelayMsgBytes,
              "the lockstep datagram is now the relay's HELLO length; it would "
              "have to differ in its first four bytes from \"SMRC\", which is "
              "true today but is no longer guaranteed by construction");

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

// run mg16 lane MP4, reshaped by mp-sync-coopdx item 6: THE AUX QUEUE,
// one message deep PER (SENDER, KIND).
//
// ONE DEEP PER SLOT ON PURPOSE, for the same reason the original single slot
// was: aux carries a peer's LATEST view, so a backlog of one sender's one kind
// is worthless by definition -- the newer message supersedes the older
// completely, and queueing both would deliver a correction toward a position
// the sender has already left. Overwriting is the correct policy for state,
// and it is the opposite of what the input records need (every one of those
// matters, which is why they have a four-deep cache). The two channels share
// a socket and nothing else.
//
// WHY THE SLOT SPLIT, and both halves were measured needs rather than polish:
//
//   BY KIND: the sync layer now sends ping/pong probes ('SYNP') beside the
//   state snapshots ('SYN1'), and with a single slot a probe landing in the
//   same pump window as a snapshot silently ate it -- the RTT readout the
//   probes exist for would undercount in exact proportion to how busy the
//   channel is, which is when the number matters. A kind never supersedes
//   another kind.
//
//   BY SENDER: per-body owner authority (item 1 of the same lane) makes every
//   console a snapshot sender, so a receiver can hold one in-flight snapshot
//   PER PEER. With a single slot, peer A's body would freeze whenever peer B's
//   messages happened to arrive later in the pump -- newest-wins across
//   senders is starvation, not policy. The sender is identified by the source
//   port the datagram ACTUALLY arrived from (the slot-is-the-port rule), never
//   by reading the payload, so the bytes stay uninspected beyond the 4-byte
//   kind tag this file already classified on.
enum : int { kAuxMaxBytes = 256 };
static_assert(kAuxMaxBytes <= kRelayMaxPayload,
              "an aux datagram no longer fits the relay's payload cap");
enum : int { kAuxKinds = 3 };        // 0 = 'SYN1' state, 1 = 'SYNP' ping,
                                     // 2 = 'SYNQ' pong -- see the tag note
struct AuxSlot { int len; unsigned char buf[kAuxMaxBytes]; };
AuxSlot  g_aux[kCommsMaxPlayers][kAuxKinds];
int      g_aux_rr     = 0;           // recv_aux round-robin cursor, see below
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

// ===========================================================================
// INPUT-DELAY PIPELINING (run vsdec, lane NET). OFF BY DEFAULT.
//
// THE PROBLEM IS ARITHMETIC, NOT A DEFECT. The ROM's lockstep is STOP AND
// WAIT: a child publishes its block for round R and cannot leave R until the
// parent's aggregate for R comes back, and the parent cannot assemble R until
// every child's R is in. So ONE ROUND COSTS ONE ROUND TRIP. At the 30 fps the
// game is paced to, a frame has 33 ms; at an 80 ms round trip a round takes
// 80 ms and the game runs at roughly twelve frames a second. Nothing is
// broken -- that is what stop-and-wait means, and on the DS's own radio the
// round trip was small enough that it never showed.
//
// THE FIX IS THE STANDARD ONE and it is a HOST-LAYER fix: stop asking the
// wire to deliver round R inside frame R. With a delay of N, frame R hands the
// game the records from round R-N, which were sent N frames (N*33 ms) ago and
// have had that long to cross. Rounds then OVERLAP the wire instead of taking
// turns with it, and the condition for full speed is
//
//     N * 33 ms  >=  round trip
//
// so N=2 covers 66 ms, N=3 covers 100 ms, N=4 covers 133 ms. The cost is that
// the records are N frames old, which is input latency -- the same trade every
// fighting game on the internet makes, and the reason it is a knob rather than
// a decision made for the player.
//
// WHY THIS IS CONSISTENT AND NOT A DESYNC. Both consoles apply the SAME
// N to the SAME round indices, so at frame R both are looking at exactly the
// record set for round R-N. Nobody is guessing and nobody is rolling back;
// the timeline is the ROM's own, read at a fixed offset. That is what makes
// this safe to do under a decomp whose determinism is the contract -- and it
// is the reason the alternative (free-run on the freshest packet that
// happened to arrive) is NOT what is implemented here, however much simpler
// it would have been.
//
// WHAT IT NEEDED FROM THE PARENT: the aggregate has to go out the moment it is
// complete, not when the parent's own exchange() next comes round. The parent
// holds child R at wall (R + one way) and its own R at frame R, so the
// aggregate is ready at R + one way -- pipe_try_broadcast() is called from
// service(), which the pump runs constantly, rather than only from exchange().
//
// PAST-ROUND HISTORY STAYS REFUSED. The redundancy finding at the bottom of
// this file killed it because under stop-and-wait a datagram's main payload
// already IS the round its receiver is stuck on. Pipelining does not revive
// it: the ring below is a SEND-SIDE and RECEIVE-SIDE ledger of rounds in
// flight, addressed by round number, and no datagram carries a round other
// than its own.
// ===========================================================================
int g_input_delay = 0;                 // SM64DS_COMMS_INPUT_DELAY, 0 = off
enum : int { kPipeDepth = 64 };        // rounds in flight; ~2 s at 30 Hz
enum : int { kInputDelayMax = 8 };
struct PipeRound {
    unsigned      round;
    unsigned char blocks[kCommsMaxPlayers][kCommsBlockBytes];
    unsigned      mask;
    bool          valid;
    bool          sent;                // parent: this aggregate went out
};
PipeRound g_pipe[kPipeDepth];
// Parent: the lowest round not yet broadcast. Child: the lowest round whose
// aggregate has not come back, and therefore the one to republish.
unsigned  g_pipe_low = 0;
unsigned long long g_pipe_starved = 0;  // exchange() had to return 0 anyway

PipeRound *pipe_find(unsigned r) {
    PipeRound &s = g_pipe[r % kPipeDepth];
    return (s.valid && s.round == r) ? &s : 0;
}

PipeRound &pipe_open(unsigned r) {
    PipeRound &s = g_pipe[r % kPipeDepth];
    if (!s.valid || s.round != r) {
        std::memset(&s, 0, sizeof s);
        s.round = r;
        s.valid = true;
    }
    return s;
}

unsigned g_last_publish_ms = 0;
unsigned g_last_join_ms    = 0;

unsigned long long g_sent = 0, g_recvd = 0, g_dropped = 0;
unsigned long long g_resends = 0, g_stale_serves = 0;
// run mg16 lane MP3: how many rounds the game walked away from (HOLE 5).
unsigned long long g_abandons = 0;
// mp-sync-coopdx item 5: DELIBERATE LOCKSTEP LOSS, test scaffolding in the
// SM64DS_SYNC_DROP class. Applied on the receive side to LOCKSTEP packets
// only (aux has its own knob), so induced loss looks exactly like the wire
// eating datagrams. This is the instrument that measured whether loss stalls
// the lockstep at all on this carrier -- see the redundancy finding at the
// bottom of this file.
int g_test_drop_pct = 0;              // SM64DS_COMMS_DROP, 0..99
unsigned long long g_test_drops = 0;

// ---------------------------------------------------------------------------
// LATENCY INDUCTION (run vsdec, lane NET). The loss knob above could already
// make the wire lossy; nothing could make it SLOW, and slow is the entire
// difference between a LAN and the internet. Without this every latency claim
// about this carrier would be a guess.
//
// SM64DS_COMMS_DELAY_MS is ONE WAY. Both instances run it, so the round trip
// the game feels is 2*N -- RTT 80 ms is DELAY_MS=40 on each end. Said here
// because getting that factor of two wrong silently halves or doubles every
// measurement in the report.
//
// SM64DS_COMMS_JITTER_MS adds a uniform +/- spread on top, which is what turns
// a delay line into something resembling a real path: constant delay never
// REORDERS, and reordering is a distinct failure the protocol should be shown
// surviving rather than assumed to.
//
// Receive-side, exactly like the sync layer's own rig, so the sender stays
// honest and the hold is indistinguishable from wire time. Applied to EVERY
// datagram kind including the relay handshake -- a delay line that the
// handshake tunnelled under would make the handshake's own robustness
// untestable, which is most of what there is to test here.
// ---------------------------------------------------------------------------
int g_delay_ms  = 0;
int g_jitter_ms = 0;
enum : int { kDelayRingLen = 192 };   // ~2 s of a 30 Hz session plus resends
struct DelayedDatagram {
    unsigned    due;
    int         len;
    sockaddr_in from;
    unsigned char buf[kAuxMaxBytes];
};
DelayedDatagram g_dring[kDelayRingLen];
int g_dring_head = 0, g_dring_count = 0;
unsigned long long g_delay_overflow = 0;   // ring full: dropped, and COUNTED

// The republish interval, a variable now rather than the kPublishResendMs
// constant, because the constant is a loopback number. See the redundancy
// finding at the bottom: a 4 ms republish over the internet is flooding.
int g_resend_ms = kPublishResendMs;
// The join backoff. Loopback keeps the flat 50 ms it always had; the address
// modes start here and double to the cap, so handshake loss costs retries
// rather than a wedged session, and the ROM's own 20 s bound stays the thing
// that decides a session never formed.
int g_join_wait_ms = kJoinResendMs;
enum : int { kJoinBackoffStartMs = 200, kJoinBackoffCapMs = 1000 };
unsigned g_join_started_ms = 0;       // for the handshake RTT sample
int g_handshake_rtt_ms = -1;

bool g_wsa_up = false;

// THE OTHER HALF OF "WINSOCK IS AVAILABLE", and it used to live inline in
// lb_open. Run rel0215 lane vsnet; this is what cost the 0.2.15 demo its
// evening.
//
// LoadLibrary + GetProcAddress make the ENTRY POINTS callable. They do not
// make them WORK: ws2_32 refuses every call out of a process that has not
// called WSAStartup, with WSANOTINITIALISED. lb_open did that call itself
// right under its own ws_load(), so the SOCKET path was always correct -- but
// the ADDRESS MODES resolve their target at INSTALL time, a whole game boot
// before any socket is opened, and the gethostbyname in parse_host_port was
// reaching a Winsock nothing had started.
//
// WHAT THAT LOOKS LIKE FROM OUTSIDE IS THE WORST POSSIBLE SHAPE, BECAUSE IT IS
// SELECTIVE. inet_addr is pure text arithmetic and needs no Winsock at all, so
// SM64DS_COMMS_RELAY=135.148.26.201:41234 installed and paired perfectly while
// SM64DS_COMMS_RELAY=tangos.dev:41234 said "could not resolve" and refused --
// the same knob, the same service, one of them spelled as a name. Every proof
// in this tree that ever reached a live relay named an ADDRESS, which is why
// nothing caught it; the first person to type the hostname was the owner, in
// front of two windows that then never paired.
//
// Factored out rather than duplicated so there is ONE place Winsock comes up.
// g_wsa_up keeps it a one-shot, so the loopback carrier's own sequence
// (ws_load, WSAStartup, socket) is exactly the sequence it always made, in the
// same order, with the same message on failure. And nothing calls either of
// these without a role in the environment, so a SOLO boot still never touches
// ws2_32 at all.
bool ws_start() {
    if (!ws_load()) return false;
    if (g_wsa_up) return true;
    WSADATA wsa;
    if (WS.WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::fprintf(stderr, "[comms:loopback] WSAStartup failed; staying "
                     "idle so the ROM's own bound fires and drops to solo\n");
        return false;
    }
    g_wsa_up = true;   // left up for the process lifetime on purpose: a
                       // WSACleanup here would tear down Winsock under any
                       // other user of it, and process exit does the job.
    return true;
}

unsigned now_ms() { return (unsigned)GetTickCount(); }

int popcount4(unsigned m) {
    int n = 0;
    for (int i = 0; i < kCommsMaxPlayers; ++i) if (m & (1u << i)) ++n;
    return n;
}

bool same_addr(const sockaddr_in &a, const sockaddr_in &b) {
    return a.sin_addr.s_addr == b.sin_addr.s_addr && a.sin_port == b.sin_port;
}

// A printable address, for the log lines that now have somewhere to point.
// Built by hand rather than through inet_ntoa: that is one more ws2_32 entry
// to look up, and it returns a shared static buffer that two arguments to one
// printf would silently alias.
const char *addr_text(const sockaddr_in &a, char *buf, int cap) {
    const unsigned long h = hton32(a.sin_addr.s_addr);   // back to host order
    std::snprintf(buf, (size_t)cap, "%lu.%lu.%lu.%lu:%u",
                  (h >> 24) & 0xFF, (h >> 16) & 0xFF, (h >> 8) & 0xFF, h & 0xFF,
                  (unsigned)ntoh16(a.sin_port));
    return buf;
}

// "1.2.3.4", "1.2.3.4:5678", "relay.example:41234", "relay.example" ->
// sockaddr_in. Returns false and says why on stderr; a knob that cannot be
// resolved must leave the carrier where it was rather than half-configure it.
//
// inet_addr and gethostbyname, not getaddrinfo, and the reason is the banner
// at the top of this file: every ws2_32 entry point here is looked up by hand
// because a static import of this DLL maps over the DS's fixed addresses and
// kills the port. Two hand-resolved entries is the cheapest thing that turns
// text into an address. IPv4 only, which is what the relay contract is shaped
// for anyway.
bool parse_host_port(const char *s, int default_port, sockaddr_in *out) {
    if (!s || !*s || !out) return false;
    // ws_start, not ws_load: the gethostbyname below is a WINSOCK call and
    // fails with WSANOTINITIALISED out of a process that has not started
    // Winsock. See ws_start's own banner -- this one line is the whole of the
    // "the relay works by IP and not by name" defect.
    if (!ws_start()) return false;

    char host[256];
    int  port = default_port;
    const char *colon = std::strrchr(s, ':');
    size_t hlen = colon ? (size_t)(colon - s) : std::strlen(s);
    if (hlen == 0 || hlen >= sizeof host) {
        std::fprintf(stderr, "[comms:loopback] '%s' is not a usable address\n", s);
        return false;
    }
    std::memcpy(host, s, hlen);
    host[hlen] = 0;
    if (colon) {
        port = std::atoi(colon + 1);
        if (port <= 0 || port > 65535) {
            std::fprintf(stderr, "[comms:loopback] port in '%s' is out of "
                         "range; using %d\n", s, default_port);
            port = default_port;
        }
    }

    std::memset(out, 0, sizeof *out);
    out->sin_family = AF_INET;
    out->sin_port   = hton16((unsigned short)port);

    if (WS.inet_addr) {
        const unsigned long v = WS.inet_addr(host);
        if (v != INADDR_NONE) { out->sin_addr.s_addr = v; return true; }
    }
    if (WS.gethostbyname) {
        struct hostent *he = WS.gethostbyname(host);
        if (he && he->h_addrtype == AF_INET && he->h_addr_list &&
            he->h_addr_list[0]) {
            std::memcpy(&out->sin_addr.s_addr, he->h_addr_list[0], 4);
            return true;
        }
    }
    std::fprintf(stderr, "[comms:loopback] could not resolve '%s'\n", host);
    return false;
}

// LOOPBACK'S ADDRESS RULE, unchanged and now explicitly named: slot k lives at
// 127.0.0.1:base+k. In the address modes this is only a fallback for a peer
// nothing has been learned about yet.
sockaddr_in loopback_slot_addr(int slot) {
    sockaddr_in a;
    std::memset(&a, 0, sizeof a);
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = hton32(INADDR_LOOPBACK);
    a.sin_port = hton16((unsigned short)(g_port_base + slot));
    return a;
}

// THE PEER TABLE. Loopback fills it from the arithmetic above and never
// changes it; direct mode learns a child's address from its JOIN and a child
// is told the parent's by env; relay mode leaves it empty because every
// destination is the relay.
sockaddr_in g_peer_addr[kCommsMaxPlayers];
bool        g_peer_known[kCommsMaxPlayers];
sockaddr_in g_relay_addr;            // relay mode: where everything goes
bool        g_relay_paired  = false; // an ACK with status 0 has landed
unsigned    g_last_hello_ms = 0;
unsigned long long g_relay_acks = 0;
unsigned char g_code[kRelayCodeBytes];

// Where a datagram addressed to `slot` actually goes.
sockaddr_in slot_addr(int slot) {
    if (g_net_mode == kNetRelay) return g_relay_addr;
    if (slot >= 0 && slot < kCommsMaxPlayers && g_peer_known[slot])
        return g_peer_addr[slot];
    return loopback_slot_addr(slot);
}

// SEND EACH LOCKSTEP DATAGRAM k TIMES. The redundancy finding at the bottom of
// this file named this lever and refused the other one: past-round history is
// structurally dead under stop-and-wait because the main payload already IS
// the round the receiver is stuck on, but SPATIAL redundancy of the in-flight
// round is not, and it becomes worth having exactly when the republish
// interval backs off toward the RTT (a 4 ms republish is fine on loopback and
// is flooding over the internet). 1 on loopback, so nothing moves there.
int g_dup = 1;
unsigned long long g_dup_sends = 0;

void send_raw(const void *buf, int len, const sockaddr_in &to) {
    if (g_sock == INVALID_SOCKET) return;
    const int n = WS.sendto(g_sock, (const char *)buf, len, 0,
                            (const sockaddr *)&to, (int)sizeof to);
    if (n == len) ++g_sent;
}

void send_to_slot(const Packet &p, int slot) {
    if (g_sock == INVALID_SOCKET) return;
    const sockaddr_in a = slot_addr(slot);
    send_raw(&p, kPacketBytes, a);
    for (int i = 1; i < g_dup; ++i) { send_raw(&p, kPacketBytes, a); ++g_dup_sends; }
}

// THE PARENT'S FAN-OUT, and the one place the star topology has to be spelled
// out rather than looped. On loopback and in direct mode the parent addresses
// each child in turn, because it holds each child's address. THROUGH THE RELAY
// IT MUST SEND EXACTLY ONE COPY: the relay is what fans a parent datagram out
// to every child, so a parent that also looped would multiply its own traffic
// by the number of children and hand each child that many duplicates.
void send_to_children(const Packet &p) {
    if (g_net_mode == kNetRelay) {
        send_to_slot(p, 1);          // any slot: relay mode ignores it
        return;
    }
    for (int k = 1; k < kCommsMaxPlayers; ++k)
        if (g_live & (1u << k)) send_to_slot(p, k);
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

// ===========================================================================
// WHO SENT THIS. The one rule that had to become mode-dependent, kept in ONE
// function so the three answers can be read side by side instead of being
// rediscovered at three call sites.
//
// Returns the sender's slot, or -1 for "not a peer of this session".
//
//   LOOPBACK  the slot IS the port, unchanged since MP2. A datagram whose
//             claimed slot disagrees with the port it arrived from is refused.
//             There is no weaker check available on one machine and no need
//             for one.
//   DIRECT    the slot is whichever table entry the SOURCE ADDRESS matches.
//             A parent that does not recognise the address will LEARN it, but
//             only from a JOIN (`learn_ok`) -- a session is joined, never
//             merely spoken into.
//   RELAY     every datagram arrives from the relay, so the address answers
//             nothing and the header's own `slot` field is the answer. It has
//             been on the wire since MP2 and this is the first thing to read
//             it. THE TRUST THIS COSTS IS REAL AND WORTH NAMING: anyone who
//             can reach the relay with the right session code can claim to be
//             slot 1. That is the security model of a 2-player friend-code
//             session over an unauthenticated relay, it is what the DS's own
//             radio offered, and the mitigation (a per-session shared secret)
//             belongs in the relay contract, not smuggled in here.
// ===========================================================================
int classify_sender(const sockaddr_in &from, int claimed, bool learn_ok) {
    if (g_net_mode == kNetRelay) {
        if (!same_addr(from, g_relay_addr)) return -1;
        if (claimed < 0 || claimed >= kCommsMaxPlayers) return -1;
        return claimed;
    }

    if (g_net_mode == kNetDirect) {
        for (int k = 0; k < kCommsMaxPlayers; ++k)
            if (g_peer_known[k] && same_addr(from, g_peer_addr[k])) return k;
        if (!learn_ok || g_role != kRoleParent) return -1;
        // A new child. It PROPOSES a slot and the parent is the authority:
        // take the proposal when it is free, otherwise hand out the lowest
        // free one. The assignment travels back in the ACCEPT.
        int want = (claimed >= 1 && claimed < kCommsMaxPlayers) ? claimed : 1;
        if (g_peer_known[want]) {
            want = -1;
            for (int k = 1; k < kCommsMaxPlayers; ++k)
                if (!g_peer_known[k]) { want = k; break; }
            if (want < 0) return -1;              // session full
        }
        g_peer_addr[want]  = from;
        g_peer_known[want] = true;
        char t[32];
        std::fprintf(stderr, "[comms:loopback] direct: learned slot %d at %s\n",
                     want, addr_text(from, t, sizeof t));
        return want;
    }

    // kNetLoopback: the slot is the port.
    if (claimed < 0 || claimed >= kCommsMaxPlayers) return -1;
    if (ntoh16(from.sin_port) != (unsigned short)(g_port_base + claimed))
        return -1;
    return claimed;
}

// ---------------------------------------------------------------------------
// THE RELAY HANDSHAKE, game side.
// ---------------------------------------------------------------------------
enum : unsigned { kRelayHelloMs = 1000, kRelayKeepaliveMs = 20000 };

void relay_send_hello() {
    RelayHello h;
    std::memset(&h, 0, sizeof h);
    std::memcpy(h.magic, kRelayHelloMagic, 4);
    h.version        = kRelayVersion;
    h.role_or_status = (g_role == kRoleParent) ? kRelayRoleParent
                                               : kRelayRoleChild;
    std::memcpy(h.code, g_code, kRelayCodeBytes);
    send_raw(&h, kRelayMsgBytes, g_relay_addr);
    g_last_hello_ms = now_ms();
}

// Idempotent BY CONTRACT: the relay expires an idle endpoint after 90 s and
// explicitly allows a re-HELLO, so acks arrive forever and only the first one
// means anything.
void relay_on_ack(const RelayHello &h) {
    ++g_relay_acks;
    if (h.version != kRelayVersion) {
        std::fprintf(stderr, "[comms:relay] ACK version %u, expected %u; the "
                     "relay and this build disagree. Staying unpaired.\n",
                     (unsigned)h.version, (unsigned)kRelayVersion);
        return;
    }
    if (std::memcmp(h.code, g_code, kRelayCodeBytes) != 0) {
        std::fprintf(stderr, "[comms:relay] ACK echoed a different session "
                     "code; ignored\n");
        return;
    }
    switch (h.role_or_status) {
    case kRelayStatusOk:
        if (!g_relay_paired) {
            g_relay_paired = true;
            std::fprintf(stderr, "[comms:relay] paired as %s on code '%.8s'; "
                         "the relay will forward from here\n",
                         g_role == kRoleParent ? "parent" : "child",
                         (const char *)g_code);
        }
        break;
    case kRelayStatusFull:
        std::fprintf(stderr, "[comms:relay] session '%.8s' is FULL; staying "
                     "unpaired so the ROM's own bound drops this to solo\n",
                     (const char *)g_code);
        break;
    case kRelayStatusBadReq:
        std::fprintf(stderr, "[comms:relay] the relay refused the HELLO as a "
                     "bad request (code '%.8s')\n", (const char *)g_code);
        break;
    default:
        std::fprintf(stderr, "[comms:relay] ACK status %u is not one this "
                     "build knows; ignored\n", (unsigned)h.role_or_status);
        break;
    }
}

// ---------------------------------------------------------------------------
// PIPELINED SEND HELPERS. Parent side only; a child publishes one slot and has
// nothing to assemble.
// ---------------------------------------------------------------------------
void pipe_send_aggregate(const PipeRound &s) {
    Packet b;
    fill_header(b, kTypeBlocks);
    b.round = s.round;               // NOT g_round: the parent is ahead of it
    b.have  = s.mask & g_live;
    for (int i = 0; i < kCommsMaxPlayers; ++i)
        if (b.have & (1u << i))
            std::memcpy(b.blocks[i], s.blocks[i], kCommsBlockBytes);
    send_to_children(b);
}

// Close and send every round that has become complete. Called from service(),
// so it runs on every pump turn rather than once per frame -- the whole point
// of pipelining is that the aggregate leaves the moment it is assemblable, and
// waiting for the parent's own next exchange() would put a frame of the
// parent's own pacing back into the path this exists to shorten.
void pipe_try_broadcast() {
    if (g_role != kRoleParent || g_input_delay <= 0) return;
    if (g_state != kCommsParentConnected) return;
    for (unsigned q = g_pipe_low; q <= g_round; ++q) {
        PipeRound *s = pipe_find(q);
        if (!s) break;                        // a hole: nothing past it is due
        if (!s->sent) {
            if ((s->mask & g_live) != g_live) break;   // still waiting on a peer
            pipe_send_aggregate(*s);
            s->sent = true;
        }
        if (q == g_pipe_low) ++g_pipe_low;
    }
}

void on_parent_packet(const Packet &p, const sockaddr_in &from, int k) {
    (void)from;
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
        // THE PARENT ASSIGNS THE SLOT AND THE RELAY DOES NOT. A child over the
        // internet cannot claim a slot by binding a port the way a loopback
        // child does, so it PROPOSES one in its JOIN and this is the answer.
        // Carried in `have`, which is meaningless on an ACCEPT and has been
        // zero on the wire since MP2 -- so an older build simply keeps the slot
        // it proposed, which is what it did before this field existed.
        a.have = 0x80000000u | (unsigned)k;
        send_to_slot(a, k);
        if (fresh)
            std::fprintf(stderr, "[comms:loopback] slot %d joined at round %u; "
                         "live mask 0x%x, players %d\n",
                         k, g_round, g_live, popcount4(g_live));
        break;
    }
    case kTypeBlocks:
        if (g_input_delay > 0) {
            // PIPELINED. Rounds are in flight rather than one at a time, so a
            // child's block is filed by ITS OWN round number and the "is this
            // the round we are on" test does not apply -- being behind the
            // parent's clock is the normal state here, not a stale packet.
            if (p.have & (1u << k)) {
                PipeRound &s = pipe_open(p.round);
                std::memcpy(s.blocks[k], p.blocks[k], kCommsBlockBytes);
                s.mask |= (1u << k);
            }
            // A republish of a round already closed means the child never saw
            // the aggregate. Re-serve it from the ledger, same job the
            // stop-and-wait cache does.
            PipeRound *done = pipe_find(p.round);
            if (done && done->sent) {
                pipe_send_aggregate(*done);
                ++g_stale_serves;
            }
            pipe_try_broadcast();
            break;
        }
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

void on_child_packet(const Packet &p, const sockaddr_in &from, int k) {
    (void)from;
    if (k != 0) { ++g_dropped; return; }   // only the parent talks to a child

    switch (p.type) {
    case kTypeAccept: {
        // THE PARENT'S SLOT ASSIGNMENT, in direct mode only. Over the relay an
        // ACCEPT is unavoidably a broadcast -- the relay fans every parent
        // datagram to every child, and the ACCEPT carries no field naming its
        // recipient -- so adopting an assignment there would let child 1's
        // accept move child 2. Direct mode's ACCEPT is a unicast to the address
        // the JOIN came from, so it is unambiguous and the assignment stands.
        //
        // THE LIMIT THIS LEAVES, stated rather than hidden: a RELAY session of
        // three or four keeps whatever slots the children were configured with
        // and the parent cannot resolve a collision. Two players -- the case
        // this lane exists for -- has exactly one child, which proposes slot 1,
        // and there is nothing to collide with. Fixing it properly wants a
        // recipient field in the ACCEPT, and that is a wire change.
        if (g_net_mode == kNetDirect && (p.have & 0x80000000u)) {
            const int assigned = (int)(p.have & 0xFF);
            if (assigned >= 1 && assigned < kCommsMaxPlayers &&
                assigned != g_slot) {
                std::fprintf(stderr, "[comms:loopback] the parent assigned slot "
                             "%d (proposed %d)\n", assigned, g_slot);
                g_slot = assigned;
            }
        }
        // AN ACCEPT THAT DOES NOT INCLUDE US IS NOT OUR ACCEPT. A no-op on
        // loopback and in direct mode, where the parent sets our live bit
        // before it sends and addresses us alone; it earns its keep over the
        // relay, where every child sees every accept and a child that adopted
        // one of those would stop knocking while the parent had never heard of
        // it -- a wedge that no timer recovers from, because both sides think
        // they are done.
        if ((p.live & (1u << g_slot)) == 0) { ++g_dropped; break; }
        g_live = p.live;
        if (g_state != kCommsChildConnected) {
            if (g_join_started_ms != 0) {
                g_handshake_rtt_ms = (int)(now_ms() - g_join_started_ms);
                if (g_handshake_rtt_ms < 0) g_handshake_rtt_ms = 0;
                // TUNE THE REPUBLISH TO THE MEASURED PATH. This is the first
                // and only round trip this carrier gets to see before the
                // lockstep starts, and it is enough: republishing faster than
                // the answer can possibly arrive is pure flooding, so the
                // floor is the round trip itself. The 20 ms floor keeps a LAN
                // from spinning; the 250 ms ceiling keeps one slow handshake
                // (a retransmitted JOIN inflates this sample) from crippling
                // recovery for the whole session. Only in the address modes:
                // loopback keeps its 4 ms, which is measured and correct there.
                //
                // ENV WINS. A run that set SM64DS_COMMS_RESEND_MS is measuring
                // something and must not be retuned underneath itself.
                if (g_net_mode != kNetLoopback &&
                    std::getenv("SM64DS_COMMS_RESEND_MS") == 0) {
                    int r = g_handshake_rtt_ms;
                    if (r < 20)  r = 20;
                    if (r > 250) r = 250;
                    g_resend_ms = r;
                    std::fprintf(stderr, "[comms:loopback] republish interval "
                                 "set to %d ms from the handshake round trip\n",
                                 g_resend_ms);
                }
            }
            g_round = p.round;          // adopt the parent's clock
            g_stage_mask = 0;
            g_round_done = false;
            g_state = kCommsChildConnected;
            std::fprintf(stderr, "[comms:loopback] accepted as slot %d at "
                         "round %u; live mask 0x%x, players %d"
                         " (handshake rtt %d ms)\n",
                         g_slot, g_round, g_live, popcount4(g_live),
                         g_handshake_rtt_ms);
        }
        break;
    }
    case kTypeBlocks:
        if (g_state != kCommsChildConnected) break;
        if (g_input_delay > 0) {
            // PIPELINED. File the aggregate under ITS OWN round. A child is
            // normally several rounds behind the parent's clock here -- that
            // is the mechanism, not lateness -- so the stop-and-wait test
            // "is this the round I am on" would throw away every packet.
            g_live = p.live;
            PipeRound &s = pipe_open(p.round);
            for (int i = 0; i < kCommsMaxPlayers; ++i) {
                if (p.have & (1u << i)) {
                    std::memcpy(s.blocks[i], p.blocks[i], kCommsBlockBytes);
                    s.mask |= (1u << i);
                }
            }
            // ADVANCE THE LOW-WATER MARK BY SCANNING, NOT BY JUMPING TO
            // p.round + 1. Jumping is the obvious version and it is wrong the
            // moment anything arrives out of order -- which jitter guarantees.
            // Aggregate 5 landing before aggregate 4 would push the mark to 6,
            // and round 4's hole would then never be republished: the child
            // needs 4 to advance, the parent already sent 4 and will not send
            // it again unasked, and the session stops for good with no error
            // anywhere. The mark is the lowest round NOT yet complete, so it
            // is found by walking up from where it was.
            while (true) {
                PipeRound *low = pipe_find(g_pipe_low);
                if (!low || (low->mask & g_live) != g_live) break;
                ++g_pipe_low;
            }
            break;
        }
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

// WHO SENT THIS AUX MESSAGE. Aux has no header of its own past its four-byte
// kind tag -- the carrier classifies on those bytes and has never read further,
// which is the property that keeps the payload opaque -- so the sender has to
// come from the transport.
//
//   LOOPBACK / DIRECT  exact: the source address names one peer.
//   RELAY              every datagram comes from the relay. What the STAR
//                      still tells us is exact for a CHILD (a child only ever
//                      receives parent traffic, so the sender is slot 0) and
//                      exact for a PARENT WITH ONE CHILD, which is the
//                      two-player session this lane exists for. A parent with
//                      several children cannot tell two snapshots apart, and
//                      rather than guess, those land in the lowest live
//                      child's lane and the ambiguity is COUNTED. The real fix
//                      is a sender byte in the sync payload -- a wire change,
//                      named as the next step, not smuggled in behind a
//                      measurement it would invalidate.
unsigned long long g_aux_ambiguous = 0;

int classify_aux_sender(const sockaddr_in &from) {
    if (g_net_mode == kNetRelay) {
        if (!same_addr(from, g_relay_addr)) return -1;
        if (g_role == kRoleChild) return 0;
        int found = -1;
        for (int k = 1; k < kCommsMaxPlayers; ++k) {
            if ((g_live & (1u << k)) == 0) continue;
            if (found < 0) found = k;
            else { ++g_aux_ambiguous; break; }
        }
        return found;
    }
    if (g_net_mode == kNetDirect) {
        for (int k = 0; k < kCommsMaxPlayers; ++k)
            if (g_peer_known[k] && same_addr(from, g_peer_addr[k])) return k;
        return -1;
    }
    /* The slot-is-the-port rule, unchanged. */
    return (int)ntoh16(from.sin_port) - g_port_base;
}

// One received datagram, classified and acted on. Split out of drain() so the
// delay line has something to call LATER: with induction on, a datagram is
// read off the socket at once (the OS buffer is not a delay line and letting
// it fill would model congestion, not latency) and dispatched when due.
void dispatch(const unsigned char *raw, int n, const sockaddr_in &from) {
    /* THE RELAY'S OWN TRAFFIC FIRST. It shares the socket with everything
       else -- one socket is one NAT mapping, which is the whole reason the
       relay path works at all -- and it is told apart the same way the aux
       kinds are, on four bytes. */
    if (n == kRelayMsgBytes && std::memcmp(raw, kRelayAckMagic, 4) == 0) {
        RelayHello h;
        std::memcpy(&h, raw, sizeof h);
        if (g_net_mode == kNetRelay && same_addr(from, g_relay_addr))
            relay_on_ack(h);
        else
            ++g_dropped;
        return;
    }

    int aux_kind = -1;
    if (n >= 4 && std::memcmp(raw, &kAuxMagicLE, 4) == 0) aux_kind = 0;
    else if (n >= 4 && std::memcmp(raw, &kAuxPingLE, 4) == 0) aux_kind = 1;
    else if (n >= 4 && std::memcmp(raw, &kAuxPongLE, 4) == 0) aux_kind = 2;
    if (aux_kind >= 0) {
        const int sender = classify_aux_sender(from);
        if (sender < 0 || sender >= kCommsMaxPlayers || sender == g_slot) {
            ++g_dropped;
            return;
        }
        /* Newest wins WITHIN one sender's one kind; see the queue's own
           note for why the slots are split. A superseded message is
           counted rather than silently forgotten, because "sync looks
           laggy" and "sync is being outrun by its own send rate" are
           different problems and the counter is what tells them apart. */
        AuxSlot &slot = g_aux[sender][aux_kind];
        if (slot.len != 0) ++g_aux_superseded;
        const int keep = n < kAuxMaxBytes ? n : kAuxMaxBytes;
        std::memcpy(slot.buf, raw, (size_t)keep);
        slot.len = keep;
        return;
    }

    Packet p;
    if (n != kPacketBytes) { ++g_dropped; return; }
    std::memcpy(&p, raw, sizeof p);
    if (std::memcmp(p.magic, kMagic, 4) != 0) { ++g_dropped; return; }
    if (p.version != kWireVersion)            { ++g_dropped; return; }
    if (g_test_drop_pct > 0) {
        /* Deterministic-per-run LCG, same discipline as the sync layer's
           drop knob: a proof that behaves differently every run is not a
           proof. */
        static unsigned r = 0x2468ace1u;
        r = r * 1664525u + 1013904223u;
        if ((int)((r >> 16) % 100u) < g_test_drop_pct) {
            ++g_test_drops;
            return;
        }
    }
    /* A JOIN is the only kind that may TEACH the parent a new address: a
       session is joined, never merely spoken into. */
    const int who = classify_sender(from, (int)p.slot, p.type == kTypeJoin);
    if (who < 0) { ++g_dropped; return; }
    /* Nobody may claim to be the parent to a parent. On loopback the port
       check already implied it; in the address modes it has to be said. */
    if (g_role == kRoleParent && who == 0) { ++g_dropped; return; }
    ++g_recvd;
    // The pump's radio IRQ: a session datagram landed, so a connected
    // wait turn may end now instead of at its VBlank. Counted for EVERY
    // accepted lockstep packet (blocks, joins, byes alike -- each one is
    // proof the peer's radio is alive); aux deliberately does not count,
    // because sync chatter must not hold the ROM's wait bound open. See
    // conductor_pump's banner in hal/comms_conductor.cpp.
    comms_note_wire_activity();
    if (g_role == kRoleParent) on_parent_packet(p, from, who);
    else                       on_child_packet(p, from, who);
}

// Hand every datagram whose hold has expired to dispatch(). A FIFO with a
// constant delay keeps due times sorted; jitter can invert two neighbours, and
// that REORDERING IS THE POINT -- a real path reorders and the protocol should
// be shown surviving it, so the ring is walked from the head and stops at the
// first entry that is not due rather than scanning for the earliest.
void pump_delayed() {
    const unsigned t = now_ms();
    while (g_dring_count > 0) {
        DelayedDatagram &d = g_dring[g_dring_head];
        if ((int)(t - d.due) < 0) break;
        dispatch(d.buf, d.len, d.from);
        g_dring_head = (g_dring_head + 1) % kDelayRingLen;
        --g_dring_count;
    }
}

void drain() {
    if (g_sock == INVALID_SOCKET) return;
    pump_delayed();
    for (;;) {
        /* ONE BUFFER, EVERY MESSAGE KIND. Run mg16 lane MP4: the aux channel
           shares this socket (one NAT mapping for internet play, ruled at the
           MP4 gate), so the read has to be big enough for any of them and the
           kinds are told apart by their first four bytes AFTER the read. Lane
           NET added a third kind to the same socket for the same reason -- the
           relay handshake, which would defeat its own purpose from a second
           port.

           THE OLD SHAPE WOULD HAVE MISCOUNTED THEM. This read asked for
           exactly kPacketBytes and charged anything else to g_dropped, so every
           aux message would have arrived, been discarded, and shown up in the
           carrier's drop counter -- a channel that works while its own
           instrument says the wire is failing. */
        unsigned char raw[kAuxMaxBytes];
        sockaddr_in from;
        int fromlen = (int)sizeof from;
        const int n = WS.recvfrom(g_sock, (char *)raw, (int)sizeof raw,
                                  0, (sockaddr *)&from, &fromlen);
        if (n < 0) break;                       // WSAEWOULDBLOCK, or nothing

        if (g_delay_ms <= 0 && g_jitter_ms <= 0) {
            dispatch(raw, n, from);
            continue;
        }

        /* INDUCTION ON. Read it off the socket now, act on it later. */
        if (g_dring_count >= kDelayRingLen) { ++g_delay_overflow; continue; }
        const int tail = (g_dring_head + g_dring_count) % kDelayRingLen;
        DelayedDatagram &d = g_dring[tail];
        int hold = g_delay_ms;
        if (g_jitter_ms > 0) {
            static unsigned jr = 0x13579bdfu;
            jr = jr * 1664525u + 1013904223u;
            hold += (int)((jr >> 16) % (unsigned)(2 * g_jitter_ms + 1))
                    - g_jitter_ms;
            if (hold < 0) hold = 0;
        }
        d.due  = now_ms() + (unsigned)hold;
        d.len  = n < kAuxMaxBytes ? n : kAuxMaxBytes;
        d.from = from;
        std::memcpy(d.buf, raw, (size_t)d.len);
        ++g_dring_count;
    }
    pump_delayed();
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

    // THE RELAY HANDSHAKE RUNS BEFORE ANY GAME TRAFFIC AND NEVER STOPS.
    // Once per second until paired, which is the frozen contract's rate and is
    // also what punches the NAT mapping open before there is anything else to
    // do it; then once per kRelayKeepaliveMs forever, because an endpoint the
    // relay has not heard from in 90 s is expired and re-HELLO is explicitly
    // allowed. A menu with no session in it can easily be quiet for 90 s.
    if (g_net_mode == kNetRelay) {
        const unsigned every = g_relay_paired ? kRelayKeepaliveMs
                                              : kRelayHelloMs;
        if ((unsigned)(t - g_last_hello_ms) >= every) relay_send_hello();
        // Game traffic before pairing is traffic the relay has nowhere to put.
        // Holding it back is not just tidiness: a child hammering JOINs at an
        // unpaired relay looks exactly like the flood the relay is entitled to
        // rate-limit, and the ROM's own bound is long enough to wait out a
        // handshake that takes a few seconds.
        if (!g_relay_paired) return;
    }

    // A child that has not been accepted keeps knocking. BACKOFF IN THE
    // ADDRESS MODES, flat 50 ms on loopback (where it always was and where
    // there is no wire to flood). A lost JOIN or a lost ACCEPT must cost a
    // retry, never a wedge -- so this keeps knocking for as long as the game
    // is willing to wait, and the ROM's own ~20 s bound stays the one thing
    // that decides a session did not form.
    if (g_role == kRoleChild && g_state == kCommsConnecting) {
        if ((unsigned)(t - g_last_join_ms) >= (unsigned)g_join_wait_ms) {
            if (g_join_started_ms == 0) g_join_started_ms = t;
            g_last_join_ms = t;
            Packet j;
            // fill_header puts g_slot in the header. Over loopback that is a
            // slot this child already OWNS, by having bound its port; in the
            // address modes the same byte is only a PROPOSAL, and the parent's
            // ACCEPT is the answer. Same byte, two meanings, decided by mode.
            fill_header(j, kTypeJoin);
            send_to_slot(j, 0);
            ++g_resends;
            if (g_net_mode != kNetLoopback) {
                g_join_wait_ms *= 2;
                if (g_join_wait_ms > kJoinBackoffCapMs)
                    g_join_wait_ms = kJoinBackoffCapMs;
            }
        }
    }

    // PIPELINED. The parent closes and ships any round that has become
    // complete, HERE rather than in its own exchange(): the pump runs this
    // constantly, and making the aggregate wait for the parent's next frame
    // would put 33 ms back into the path pipelining exists to shorten.
    if (g_input_delay > 0) {
        if (g_role == kRoleParent) {
            pipe_try_broadcast();
        } else if (g_state == kCommsChildConnected &&
                   (unsigned)(t - g_last_publish_ms) >= (unsigned)g_resend_ms) {
            // Republish the OLDEST round still unanswered. One per tick: a
            // hole is healed at the resend rate, and a child that dumped its
            // whole in-flight window on every tick would turn one lost
            // datagram into a burst exactly when the path is already unhappy.
            g_last_publish_ms = t;
            for (unsigned q = g_pipe_low; q < g_round; ++q) {
                PipeRound *s = pipe_find(q);
                if (!s || (s->mask & (1u << g_slot)) == 0) continue;
                Packet b;
                fill_header(b, kTypeBlocks);
                b.round = q;
                b.have  = (1u << g_slot);
                std::memcpy(b.blocks[g_slot], s->blocks[g_slot],
                            kCommsBlockBytes);
                send_to_slot(b, 0);
                ++g_resends;
                break;
            }
        }
        return;
    }

    // A child with an open round keeps republishing it until the parent's
    // answer lands. This is the whole of the retransmission story.
    if (g_role == kRoleChild && g_state == kCommsChildConnected &&
        !g_round_done && (g_stage_mask & (1u << g_slot)) != 0) {
        if ((unsigned)(t - g_last_publish_ms) >= (unsigned)g_resend_ms) {
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

    // ws_start is ws_load plus the one-shot WSAStartup this block used to do
    // inline; the sequence and the failure message are unchanged.
    if (!ws_start()) return;

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
        // THE BIND ADDRESS IS NOT THE PEER ADDRESS, and conflating them is why
        // this needed its own line. Loopback binds 127.0.0.1:base+k, which is
        // also where its peers are. An address-mode parent has to be reachable
        // from OFF this machine, so it binds 0.0.0.0 -- BUT ONLY WHEN ASKED.
        //
        // SM64DS_COMMS_BIND_ANY IS OPT-IN AND STAYS OPT-IN. Binding every
        // interface by default would silently put a UDP listener that accepts
        // session traffic on whatever network the machine is attached to, for
        // every player, including everyone who only ever plays alone. That is
        // a decision a player makes, not one a default makes for them.
        //
        // TWO CASES BIND ANY WITHOUT THE FLAG, because in them a loopback bind
        // is not "safe", it is BROKEN -- the socket could send but never
        // receive, which is the most confusing failure a transport has:
        //   * ANY relay role. Every packet arrives from the relay, which is
        //     off this machine by definition.
        //   * A DIRECT-MODE CHILD. It sends to one address it was given and
        //     the answers come back from off-machine too. Its exposure is not
        //     the parent's: it is not accepting a session from anyone, it is
        //     answering the one host it was pointed at.
        // The direct-mode PARENT is the one that genuinely opens a door -- it
        // accepts JOINs from strangers -- and that one still needs the flag.
        sockaddr_in a = loopback_slot_addr(k);
        const bool any = g_bind_any || g_net_mode == kNetRelay ||
                         (g_net_mode == kNetDirect && g_role == kRoleChild);
        if (any) a.sin_addr.s_addr = 0;             // INADDR_ANY
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
    g_dring_head = g_dring_count = 0;
    std::memset(g_pipe, 0, sizeof g_pipe);
    g_pipe_low = 0;

    // SEED THE PEER TABLE. In loopback mode every entry is known up front and
    // never changes -- that is the old arithmetic, written down as data. In
    // direct mode a CHILD is told the parent's address by env (already parsed)
    // and the parent knows nothing until a JOIN teaches it. Relay mode fills
    // nothing: slot_addr() short-circuits to the relay for every slot.
    if (g_net_mode == kNetLoopback) {
        for (int k = 0; k < kCommsMaxPlayers; ++k) {
            g_peer_addr[k]  = loopback_slot_addr(k);
            g_peer_known[k] = true;
        }
    }

    char t[32];
    if (g_net_mode == kNetRelay) {
        std::fprintf(stderr, "[comms:loopback] open(mode=%u) as %s, slot %d, "
                     "udp *:%d via RELAY %s, code '%.8s'\n",
                     mode, g_role == kRoleParent ? "parent" : "child",
                     g_slot, g_my_port, addr_text(g_relay_addr, t, sizeof t),
                     (const char *)g_code);
    } else if (g_net_mode == kNetDirect) {
        std::fprintf(stderr, "[comms:loopback] open(mode=%u) as %s, slot %d, "
                     "udp %s:%d DIRECT (peer %s)\n",
                     mode, g_role == kRoleParent ? "parent" : "child",
                     g_slot, g_bind_any || g_role == kRoleChild ? "*"
                                                                : "127.0.0.1",
                     g_my_port,
                     g_peer_known[0] ? addr_text(g_peer_addr[0], t, sizeof t)
                                     : "learned from the JOIN");
    } else {
        std::fprintf(stderr, "[comms:loopback] open(mode=%u) as %s, slot %d, "
                     "udp 127.0.0.1:%d (parent at :%d)\n",
                     mode, g_role == kRoleParent ? "parent" : "child",
                     g_slot, g_my_port, g_port_base);
    }
}

void lb_close() {
    if (!g_open) return;             // double close is legal and silent
    Packet b;
    fill_header(b, kTypeBye);
    if (g_role == kRoleParent) send_to_children(b);
    else                       send_to_slot(b, 0);
    WS.closesocket(g_sock);
    g_sock  = INVALID_SOCKET;
    g_open  = false;
    g_state = kCommsIdle;
    g_live  = 0;
    g_slot  = 0;
    g_parent_requested = false;
    g_latched_mask = 0;
    g_stage_mask   = 0;
    std::memset(g_aux, 0, sizeof g_aux);   // pending aux dies with the session
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
    // Knock IMMEDIATELY. Backed off the interval actually in use, not the
    // loopback constant: with the address modes' 200 ms start, subtracting 50
    // would have left the first knock waiting 150 ms for no reason.
    g_last_join_ms = now_ms() - (unsigned)g_join_wait_ms;
    g_join_started_ms = 0;
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

    // =======================================================================
    // THE PIPELINED PATH. See the banner over the ring for why it exists and
    // why it is consistent. Nothing below this block changes; with the knob at
    // 0 the stop-and-wait code underneath is what runs, unaltered.
    // =======================================================================
    if (g_input_delay > 0) {
        PipeRound &mine = pipe_open(g_round);
        // PUBLISH ONCE PER ROUND, NOT ONCE PER CALL, and the difference is not
        // cosmetic. The ROM's wait loop calls exchange() over and over inside a
        // single frame -- that is what the seam's exchanges-vs-rounds counters
        // have always shown -- so a send on every call is a send on every spin.
        // Measured on the live relay before this line existed: 1447 datagrams
        // for 600 rounds, about 2.4x the traffic the protocol needs, at exactly
        // the moment the path is already the bottleneck. The stop-and-wait path
        // beneath has always had this guard (`first_publish`); the pipelined
        // one needs its own because it stages into the ring instead.
        const bool first_publish = (mine.mask & (1u << g_slot)) == 0;
        std::memcpy(mine.blocks[g_slot], my_block, kCommsBlockBytes);
        mine.mask |= (1u << g_slot);

        if (g_role == kRoleChild) {
            if (first_publish) {
                Packet b;
                fill_header(b, kTypeBlocks);
                b.round = g_round;
                b.have  = (1u << g_slot);
                std::memcpy(b.blocks[g_slot], mine.blocks[g_slot],
                            kCommsBlockBytes);
                send_to_slot(b, 0);
                g_last_publish_ms = now_ms();
            }
        } else {
            pipe_try_broadcast();
        }
        service();                       // the answer may already be here

        // THE ROUND THE GAME ACTUALLY GETS. Frame R is handed round R-N.
        //
        // BEFORE THE PIPELINE HAS FILLED, every frame is handed round 0
        // instead of nothing. Handing back an EMPTY round would be worse than
        // slow: the ROM's own record loop clears each slot and only fills the
        // ones it got, so an empty round tells the game that no player -- not
        // even this console -- is live, and src/func_0203ea5c.c takes a very
        // different branch on that. Repeating round 0 for N frames is a held
        // first frame, which is what the start of a session already looks
        // like.
        const unsigned want = (g_round >= (unsigned)g_input_delay)
                                  ? g_round - (unsigned)g_input_delay
                                  : 0u;
        PipeRound *s = pipe_find(want);
        if (!s || (s->mask & g_live) != g_live) {
            // The wire is slower than N frames of pipeline. This is the honest
            // stall, and it is COUNTED rather than hidden: a session running
            // with a nonzero starve count is one whose input delay is set too
            // low for its path, and that is a tuning fact somebody needs.
            ++g_pipe_starved;
            return 0;
        }
        std::memcpy(g_latched, s->blocks, sizeof g_latched);
        g_latched_mask = s->mask & g_live;
        ++g_round;
        return 1;
    }

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
        send_to_children(b);
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

    /* THROUGH THE RELAY, ONE COPY. The relay fans a parent's datagram out to
       every child and sends a child's to the parent, so the loop below would
       hand each peer as many duplicates as there are peers. Same rule as
       send_to_children, and it has to be repeated here because aux does not go
       through that helper -- aux carries opaque bytes, not a Packet. */
    if (g_net_mode == kNetRelay) {
        if (!g_relay_paired) return 0;
        send_raw(buf, len, g_relay_addr);
        return len;
    }

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
    service();                      // drain; aux messages land in the slots
    /* ROUND-ROBIN over the (sender, kind) slots, resuming after the last one
       served, so no sender and no kind can starve another: a reader that
       always scanned from slot zero would hand the parent's messages out
       first every call and a busy parent could shadow a quiet child forever.
       One whole message per call, exactly as the frozen contract words it --
       a caller that wants everything pending loops until 0, which is what the
       sync layer's pump does. */
    const int total = kCommsMaxPlayers * kAuxKinds;
    for (int i = 0; i < total; ++i) {
        const int idx = (g_aux_rr + i) % total;
        AuxSlot &slot = g_aux[idx / kAuxKinds][idx % kAuxKinds];
        if (slot.len <= 0) continue;
        const int n = slot.len < cap ? slot.len : cap;
        std::memcpy(buf, slot.buf, (size_t)n);
        slot.len = 0;
        g_aux_rr = (idx + 1) % total;
        return n;
    }
    return 0;
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

    if (const char *d = std::getenv("SM64DS_COMMS_DROP")) {
        int v = std::atoi(d);
        if (v < 0) v = 0;
        if (v > 99) v = 99;
        g_test_drop_pct = v;
        if (v > 0)
            std::fprintf(stderr, "[comms:loopback] TEST SCAFFOLDING: dropping "
                         "%d%% of received lockstep packets "
                         "(SM64DS_COMMS_DROP)\n", v);
    }

    // =======================================================================
    // THE ADDRESS MODES. Run vsdec, lane NET.
    //
    // ORDER MATTERS AND RELAY WINS: a launcher that sets both was told two
    // different things and the relay is the one that works without port
    // forwarding, so it is the safer of the two to honour. Said out loud
    // because silently preferring one is the kind of thing that costs an hour
    // of somebody's evening.
    //
    // EVERY ONE OF THESE IS OPT-IN. With none of them set the mode stays
    // kNetLoopback and every line above and below behaves as it did before
    // this block existed.
    // =======================================================================
    const char *relay = std::getenv("SM64DS_COMMS_RELAY");
    const char *host  = std::getenv("SM64DS_COMMS_HOST");

    if (relay && *relay) {
        if (!parse_host_port(relay, kRelayDefaultPort, &g_relay_addr)) {
            std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_RELAY='%s' "
                         "could not be resolved; NOT installing, so the seam "
                         "keeps its solo answers rather than pretending to "
                         "have a session\n", relay);
            return false;
        }
        // The session code. Up to 8 ASCII, right-padded with NUL, and the
        // relay pairs a parent with the children that share it. No default: a
        // default code would put every player in the world who forgot to set
        // one into the same session, which is a worse failure than refusing.
        const char *code = std::getenv("SM64DS_COMMS_CODE");
        if (!code || !*code) {
            std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_RELAY needs "
                         "SM64DS_COMMS_CODE too (up to 8 ASCII). NOT "
                         "installing.\n");
            return false;
        }
        std::memset(g_code, 0, sizeof g_code);
        size_t cl = std::strlen(code);
        if (cl > kRelayCodeBytes) {
            std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_CODE '%s' is "
                         "longer than %d bytes; using the first %d\n",
                         code, kRelayCodeBytes, kRelayCodeBytes);
            cl = kRelayCodeBytes;
        }
        std::memcpy(g_code, code, cl);
        g_net_mode = kNetRelay;
        if (host && *host)
            std::fprintf(stderr, "[comms:loopback] both SM64DS_COMMS_RELAY and "
                         "SM64DS_COMMS_HOST are set; using the RELAY\n");
    } else if (host && *host) {
        // DIRECT MODE. The child is told where the parent is; the parent is
        // told nothing and learns each child from its JOIN.
        if (g_role == kRoleChild) {
            if (!parse_host_port(host, g_port_base, &g_peer_addr[0])) {
                std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_HOST='%s' "
                             "could not be resolved; NOT installing\n", host);
                return false;
            }
            g_peer_known[0] = true;
        } else {
            std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_HOST is a "
                         "CHILD knob; the parent learns each child's address "
                         "from its JOIN. Ignored.\n");
        }
        g_net_mode = kNetDirect;
    }

    if (const char *b = std::getenv("SM64DS_COMMS_BIND_ANY"))
        g_bind_any = (std::atoi(b) != 0);

    // BIND_ANY ON A PARENT IS ITSELF A DIRECT-MODE REQUEST, and this cost a
    // failed rung to find. SM64DS_COMMS_HOST is a CHILD knob -- a parent has
    // no peer address to be told, it learns each child from the JOIN -- so a
    // direct-mode parent has no HOST set and, without this, stayed in loopback
    // mode. What that looks like from outside is the nastiest kind of
    // half-working: the parent ACCEPTS the child (the child's source port
    // still satisfies loopback's port arithmetic) and then answers to
    // 127.0.0.1, which the child correctly refuses because it is not the
    // address it dialled. Parent says two players, child says none, and
    // neither log contains an error.
    //
    // Opening the socket to every interface IS the decision that this session
    // comes from off this machine. Nothing else needs to be said twice.
    if (g_bind_any && g_net_mode == kNetLoopback && g_role == kRoleParent) {
        g_net_mode = kNetDirect;
        std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_BIND_ANY on a "
                     "parent means DIRECT mode: children are learned from "
                     "their JOINs and answered at the address they came "
                     "from\n");
    }

    // The remaining combination that looks fine and cannot work: a parent that
    // was given SM64DS_COMMS_HOST (which does nothing for it) and no
    // BIND_ANY. It binds 127.0.0.1 and no child off this machine can reach it,
    // and nothing else in the system will say so -- the child knocks into the
    // void and both sides blame the network.
    if (g_net_mode == kNetDirect && g_role == kRoleParent && !g_bind_any) {
        std::fprintf(stderr,
            "[comms:loopback] DIRECT MODE PARENT WITHOUT SM64DS_COMMS_BIND_ANY=1. "
            "The socket will bind 127.0.0.1 and no child off this machine can "
            "reach it. Set SM64DS_COMMS_BIND_ANY=1 to accept a session on "
            "every interface.\n");
    }

    if (const char *d = std::getenv("SM64DS_COMMS_DELAY_MS")) {
        int v = std::atoi(d);
        if (v < 0) v = 0;
        if (v > 2000) v = 2000;
        g_delay_ms = v;
    }
    if (const char *j = std::getenv("SM64DS_COMMS_JITTER_MS")) {
        int v = std::atoi(j);
        if (v < 0) v = 0;
        if (v > 1000) v = 1000;
        g_jitter_ms = v;
    }
    if (g_delay_ms > 0 || g_jitter_ms > 0)
        std::fprintf(stderr, "[comms:loopback] TEST SCAFFOLDING: holding every "
                     "received datagram %d ms (+/- %d) before acting on it. "
                     "THIS IS ONE WAY -- with both ends running it the round "
                     "trip is %d ms.\n",
                     g_delay_ms, g_jitter_ms, 2 * g_delay_ms);

    if (const char *k = std::getenv("SM64DS_COMMS_DUP")) {
        int v = std::atoi(k);
        if (v < 1) v = 1;
        if (v > 4) v = 4;
        g_dup = v;
        if (v > 1)
            std::fprintf(stderr, "[comms:loopback] sending each lockstep "
                         "datagram %d times (SM64DS_COMMS_DUP)\n", v);
    }

    // THE REPUBLISH INTERVAL. 4 ms is a loopback number and the redundancy
    // finding at the bottom of this file already called it flooding over the
    // internet, so the address modes start at a wire-shaped default and the
    // handshake's own measured round trip tunes it from there.
    if (g_net_mode != kNetLoopback) g_resend_ms = 50;
    if (const char *r = std::getenv("SM64DS_COMMS_RESEND_MS")) {
        int v = std::atoi(r);
        if (v < 1) v = 1;
        if (v > 1000) v = 1000;
        g_resend_ms = v;
    }
    if (g_net_mode != kNetLoopback) g_join_wait_ms = kJoinBackoffStartMs;

    // INPUT-DELAY PIPELINING. A NUMBER OF FRAMES, and deliberately not an
    // "auto".
    //
    // Auto was written and taken back out, and the reason is worth keeping:
    // only the CHILD ever measures a round trip here (its JOIN to the parent's
    // ACCEPT). The parent has no equivalent sample, so an auto would have
    // resolved to one value on one end and a fallback on the other -- and
    // while mismatched delays are not a desync (both consoles consume the same
    // round sequence, one simply lags the other), it would mean a knob whose
    // effective value nobody could state. A number both launchers set is worth
    // more than an automation that is right on one side.
    //
    // THE FORMULA, so a caller is not guessing: N >= round_trip_ms / 33,
    // rounded up. N=2 covers 66 ms, N=3 covers 100 ms, N=4 covers 133 ms. Too
    // low costs stalls (counted as `starved`); too high costs input lag.
    //
    // Off by default, and off on loopback even when asked, because loopback's
    // round trip is microseconds: there is nothing there to hide and it would
    // only add frames of lag to a session that has none.
    if (const char *n = std::getenv("SM64DS_COMMS_INPUT_DELAY")) {
        int v = std::atoi(n);
        if (v < 0) v = 0;
        if (v > kInputDelayMax) v = kInputDelayMax;
        g_input_delay = v;
        // REFUSED ON A BARE LOOPBACK, ALLOWED WHEN THERE IS A ROUND TRIP TO
        // HIDE -- and the second half of that sentence was missing at first,
        // which broke the one rig that most needed it. The guard keyed off the
        // MODE, so an induced-latency run (loopback carrier, delay knob on,
        // which is the whole controlled experiment) had its input delay
        // silently thrown away and measured pipelining doing nothing. The
        // right question is not "which mode is this" but "is there any latency
        // here at all", and with the induction knobs on there certainly is.
        if (g_net_mode == kNetLoopback && g_input_delay > 0 &&
            g_delay_ms <= 0 && g_jitter_ms <= 0) {
            std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_INPUT_DELAY is "
                         "for a wire with a round trip on it; a bare loopback "
                         "has none and no delay is being induced. Ignored.\n");
            g_input_delay = 0;
        }
        if (g_input_delay > 0)
            std::fprintf(stderr, "[comms:loopback] input delay %d frame(s): "
                         "frame R is handed the records from round R-%d, so "
                         "rounds overlap the wire instead of taking turns with "
                         "it. Both ends should run the same number.\n",
                         g_input_delay, g_input_delay);
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
    s.net_mode        = g_net_mode;
    s.relay_paired    = g_relay_paired;
    s.handshake_rtt_ms= g_handshake_rtt_ms;
    s.resend_ms       = g_resend_ms;
    s.delay_ms        = g_delay_ms;
    s.jitter_ms       = g_jitter_ms;
    s.delay_overflow  = g_delay_overflow;
    s.input_delay     = g_input_delay;
    s.starved         = g_pipe_starved;
    return s;
}

void comms_loopback_report(const char *tag) {
    const CommsLoopbackStats s = comms_loopback_stats();
    static const char *const kModeName[3] = { "loopback", "direct", "relay" };
    std::fprintf(stderr,
        "[loopback:%s] role=%s slot=%d port=%d live=0x%x round=%lu "
        "sent=%llu recvd=%llu dropped=%llu resends=%llu stale=%llu "
        "testdrop=%llu mode=%s paired=%d hsrtt=%d resendms=%d "
        "delayms=%d jitterms=%d delayovf=%llu dup=%d auxambig=%llu "
        "indelay=%d starved=%llu\n",
        tag ? tag : "-",
        s.role == kRoleParent ? "parent" : (s.role == kRoleChild ? "child"
                                                                 : "none"),
        s.slot, s.port, s.live_mask, s.round,
        s.sent, s.recvd, s.dropped, s.resends, s.stale_serves,
        g_test_drops,
        kModeName[s.net_mode >= 0 && s.net_mode <= 2 ? s.net_mode : 0],
        s.relay_paired ? 1 : 0, s.handshake_rtt_ms, s.resend_ms,
        s.delay_ms, s.jitter_ms, s.delay_overflow, g_dup, g_aux_ambiguous,
        s.input_delay, s.starved);
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
//
// ===========================================================================
// THE REDUNDANCY FINDING (mp-sync-coopdx item 5): per-datagram round history
// was SPECIFIED, MEASURED AGAINST, AND REFUSED. Read before reintroducing it.
//
// The adoption list called for each lockstep datagram to carry the last ~3
// rounds' 0x20-byte blocks so a receiver could recover a missed round from
// the next datagram instead of waiting a resend round trip -- the GGPO
// family's input-redundancy trick, flagged in the spec's own honesty note as
// standard lockstep netcode rather than anything sm64coopdx does.
//
// IT CANNOT HELP THIS PROTOCOL, structurally: the ROM's lockstep is
// STOP-AND-WAIT. Nobody advances past an incomplete round -- the parent
// completes round R only when every live child's R block is in, a child
// leaves R only on the parent's R broadcast, and rollback (what lets GGPO
// pipeline past a hole) is an explicit non-adoption because the ROM is the
// contract. So every datagram this carrier sends carries, as its MAIN
// payload, exactly the round its receiver is stuck on: the child republishes
// its open round every kPublishResendMs, and the parent answers a stale
// round with the cached broadcast. A history block bolted onto those
// datagrams could never contain a round the main payload does not already
// carry. Dead machinery by construction, not by tuning.
//
// MEASURED, not just argued -- SM64DS_COMMS_DROP is the instrument (receive-
// side lockstep loss, deterministic, both instances), two-window sessions,
// 40 s arms:
//
//   loss  0%   1216 rounds   resends  58   stale serves  55
//   loss 10%   1212 rounds   resends 319   stale serves 189
//   loss 20%   1170 rounds   resends 673   stale serves 325
//
// Twenty percent loss costs 3.8% of the round rate; the 4 ms republish and
// the stale-serve cache heal every hole sub-frame, which is the "stall" the
// item targeted not existing on this carrier. WHERE THE IDEA DOES APPLY: a
// future internet carrier must back its republish off toward the RTT (a 4 ms
// republish over the internet is flooding), and once the resend interval is
// RTT-scaled, loss recovery costs a visible hitch -- the lever THERE is
// spatial redundancy of the IN-FLIGHT round (send each datagram k times),
// not history of past rounds, which stays structurally dead as long as the
// lockstep is stop-and-wait.
// ===========================================================================
