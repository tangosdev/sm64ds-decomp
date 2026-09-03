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
#include "host_settings.h"   // kRollbackMaxPlayers, beside the NetMode parse it guards

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

extern "C" int host_setting_net_mode(void);   /* hal/host_settings.cpp */

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

// VERSION 2 SINCE run rel0215 LANE vslag, and the bump IS the fix rather than
// bookkeeping that goes with one.
//
// Version 1 carried no input delay, so a version-1 build defaults to 0 and a
// version-2 build defaults to 5 on a relay. Pair the two and NOTHING
// COMPLAINS: the relay pairs them, the handshake completes, both logs report a
// healthy session of two players with nothing dropped, 600 rounds exchanged --
// and the two consoles quietly simulate different matches, because a
// MISMATCHED INPUT DELAY IS A DESYNC (the mechanism is written out over the
// ACCEPT's delay field below). Measured old-against-new on the public relay
// before this bump existed: paired, healthy, diverging.
//
// A silent desync that reads healthy in every log is the worst failure this
// carrier can produce, it survives into a crash report as nothing at all, and
// it was reachable with no knob touched by anybody. So the two generations
// must not form a session: the version byte is the cheapest way to say so, the
// check already existed and only had to become LOUD, and nothing relay-shaped
// has shipped in any release yet, so refusing every version-1 peer costs
// exactly nobody.
// AND THE SECOND BUMP, run vs16, for the same reason and with the same rule:
// a wide session's datagram is a different length with a different live mask,
// and a peer that speaks only version 2 must not half-understand it. The
// difference from the bump above is that BOTH versions stay supported here,
// because they describe different sessions rather than different generations
// of the same one -- see THE TWO WIRES over `struct Packet`. Four or fewer
// players is still version 2, on the byte, forever.
enum : unsigned char { kWireVersionNarrow = 2 };
enum : unsigned char { kWireVersionWide   = 3 };

enum : unsigned char {
    kTypeJoin   = 1,   // child -> parent: I have bound slot N, let me in
    kTypeAccept = 2,   // parent -> child: you are in, here is the live mask
                       //                  and the round to start from
    kTypeBlocks = 3,   // child -> parent: my block for round R
                       // parent -> child: every slot's block for round R
    kTypeBye    = 4,   // either way: I am leaving
};

// ===========================================================================
// THE TWO WIRES. Run vs16.
//
// This build speaks version 2 and version 3, and WHICH ONE A SESSION SPEAKS IS
// DECIDED BY HOW MANY PEOPLE ARE IN IT, once, before the first datagram:
//
//   four or fewer -> VERSION 2, 0x90 bytes on the wire, byte-for-byte what
//                    this port has always sent. The `live_wide` word below is
//                    NOT transmitted and the datagram ends at the fourth
//                    block. A 2P or 4P match is unchanged by this file's
//                    growth, and that is a property of the LENGTH, not of a
//                    branch anyone has to remember to take.
//   five or more  -> VERSION 3, 0x214 bytes, sixteen blocks and a 32-bit live
//                    mask in the tail word. This is the MOD.
//
// The header is IDENTICAL in both, on purpose: magic, version and type sit at
// the same three offsets in every datagram this carrier has ever sent, so a
// receiver can classify before it assumes a layout. `live` keeps its v2
// meaning (slots 0..7) in both, and a wide session ALSO writes the full mask
// into `live_wide`. Everything downstream of dispatch() reads `live_wide`,
// which dispatch fills in from `live` for a narrow datagram -- one
// normalisation point, so no reader has to know which wire it came off.
//
// WHY THE WIDE BITS ARE AT THE END AND NOT IN `have`. `have` is not spare: an
// ACCEPT carries 0x80000000|slot in it (the parent's slot assignment) and the
// input delay in bits 8..15. Stealing bits from it would have put the slot
// assignment and the live mask in one word, which is how a broadcast accept
// collapses two players onto one seat.
// ===========================================================================
struct Packet {
    unsigned char magic[4];      // 0x00  'M','P','2','L'
    unsigned char version;       // 0x04
    unsigned char type;          // 0x05
    unsigned char slot;          // 0x06  the sender's slot
    unsigned char live;          // 0x07  live-slot bitmask, slots 0..7; only
                                 //       slot 0's is authoritative, which is
                                 //       the contract's own "+0x0D player
                                 //       count honoured only from slot 0" rule
                                 //       kept at the header
    unsigned int  round;         // 0x08  the transport's round, not the ROM's
    unsigned int  have;          // 0x0C  which payloads are valid (32 bits,
                                 //       so it already named sixteen slots
                                 //       before this port did)
    unsigned char blocks[kCommsMaxPlayers][kCommsBlockBytes];   // 0x10 .. 0x210
    unsigned int  live_wide;     // 0x210 all sixteen live bits. WIDE ONLY --
                                 //       a narrow datagram stops at 0x90 and
                                 //       never carries this word.
};

// 0x10 + 4 * 0x20. THE FROZEN v2 LENGTH: this number is on the wire of every
// shipped build and must never move.
enum : int { kPacketNarrowBytes = 0x10 + kCommsNarrowPlayers * kCommsBlockBytes };
// 0x10 + 16 * 0x20 + 4 = 0x214.
enum : int { kPacketWideBytes   = 0x10 + kCommsMaxPlayers * kCommsBlockBytes + 4 };

static_assert(kPacketNarrowBytes == 0x90,
              "wire version 2 is 0x90 bytes on every shipped build; this "
              "number is not ours to change");
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
// Lane VOICE: proximity voice chat. Same tag space, same rule -- 'SYN*', four
// bytes, classified and never read past. Its payload is compressed microphone
// audio and nothing else; hal/voice_chat.cpp owns the format.
const unsigned kAuxVoiceLE = 0x564e5953u;   // 'S','Y','N','V'

static_assert(sizeof(Packet) == kPacketWideBytes,
              "the loopback wire packet grew padding; the length check is the "
              "frame check and it must stay exact");
// AND THE HALF THAT MATTERS MORE, because the narrow wire is the one already
// in the field: the first 0x90 bytes of this struct must still be exactly what
// version 2 puts on the wire. Padding anywhere before `blocks`, or a stride
// other than kCommsBlockBytes, would silently break every shipped build.
static_assert(offsetof(Packet, blocks) == 0x10,
              "the v2 header is four fixed offsets and this is one of them");
static_assert(offsetof(Packet, live) == 0x07,
              "the v2 live byte moved; every shipped build reads it here");
static_assert(offsetof(Packet, live_wide) == 0x210,
              "the wide live word is the tail of the wide datagram and its "
              "offset IS kPacketWideBytes - 4");

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

// The relay caps a forwarded payload at 700 bytes. Every one of this carrier's
// kinds is inside that -- 0x90 narrow lockstep, 0x214 WIDE lockstep, 256 aux
// -- and the assert is here so a wire change cannot quietly cross the line and
// get truncated by a service that has no way to tell the game about it.
// (kAuxMaxBytes gets the same assert where it is defined, below.)
//
// run vs16 measured the wide one rather than trusting it: 0x214 is 532 bytes
// against a 700-byte cap, 168 to spare. That is the whole reason sixteen was
// reachable without also changing the relay's own contract.
enum : int { kRelayMaxPayload = 700 };
static_assert(kPacketNarrowBytes <= kRelayMaxPayload,
              "the lockstep datagram no longer fits the relay's payload cap");
static_assert(kPacketWideBytes <= kRelayMaxPayload,
              "the WIDE lockstep datagram no longer fits the relay's payload "
              "cap; sixteen players would be silently truncated");

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
static_assert(kPacketNarrowBytes != kRelayMsgBytes,
              "the lockstep datagram is now the relay's HELLO length; it would "
              "have to differ in its first four bytes from \"SMRC\", which is "
              "true today but is no longer guaranteed by construction");
static_assert(kPacketWideBytes != kRelayMsgBytes,
              "the WIDE lockstep datagram is now the relay's HELLO length");

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
// Lane VOICE raised this from 256. A voice datagram is a 12-byte header and
// two 164-byte IMA ADPCM frame blocks = 340 bytes, so 256 would have refused
// every one of them at lb_send_aux's one-datagram check. 384 leaves the sync
// snapshot (which is far smaller) untouched and stays a long way under the
// relay's 700-byte cap, asserted just below.
enum : int { kAuxMaxBytes = 384 };
// The socket carries three kinds and the read buffer must hold the largest.
// run vs16 made that the WIDE lockstep datagram, not the aux message.
enum : int { kRawMaxBytes =
    kAuxMaxBytes > kPacketWideBytes ? kAuxMaxBytes : kPacketWideBytes };
static_assert(kRawMaxBytes >= kPacketWideBytes,
              "the read buffer cannot hold a wide lockstep datagram; every "
              "sixteen-player packet would be lost to WSAEMSGSIZE");
static_assert(kAuxMaxBytes <= kRelayMaxPayload,
              "an aux datagram no longer fits the relay's payload cap");
enum : int { kAuxKinds = 4 };        // 0 = 'SYN1' state, 1 = 'SYNP' ping,
                                     // 2 = 'SYNQ' pong, 3 = 'SYNV' voice --
                                     // see the tag note
// Lane VOICE: the voice kind is served by lb_recv_voice and SKIPPED by
// lb_recv_aux, so the sync layer's drain-until-zero pump cannot swallow audio
// and the voice pump cannot swallow a snapshot. Two consumers, one socket, no
// shared queue -- the alternative was one of them eating the other's messages,
// which is exactly the failure the per-(sender, kind) split was made to stop.
enum : int { kAuxKindVoice = 3 };
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

// ---------------------------------------------------------------------------
// THE SESSION'S WIDTH, and the ONE place the two wires are chosen between.
// Run vs16.
//
// g_want_players is what the session was opened for -- the conductor's clamped
// SM64DS_VS_PLAYERS, handed down by comms_set_session_width() before open().
// Everything else here is derived from it and nothing else, so there is no
// second opinion about which wire a datagram belongs to.
//
// DEFAULT 0 MEANS NARROW. A build that never calls the setter -- every solo
// boot, every local-play boot, every test that predates this -- speaks
// version 2 at 0x90 bytes, exactly as before.
// ---------------------------------------------------------------------------
int g_want_players = 0;

inline bool wire_wide() { return g_want_players > kCommsNarrowPlayers; }
// THE SLOT CAP FOLLOWS THE WIRE. A narrow datagram carries four blocks and an
// eight-bit live byte, so a narrow session must not admit, bind or pin a slot
// past 3: an admitted slot 4 would be counted live, have its `have` bit set,
// and have its block truncated off every 0x90-byte datagram -- a peer that
// pairs, seats a body, and never moves, with nothing in either log saying
// why. Every admission site below asks this instead of kCommsMaxPlayers.
inline int slot_cap() { return wire_wide() ? (int)kCommsMaxPlayers : (int)kCommsNarrowPlayers; }
inline unsigned char wire_version() {
    return wire_wide() ? kWireVersionWide : kWireVersionNarrow;
}
inline int packet_bytes() {
    return wire_wide() ? kPacketWideBytes : kPacketNarrowBytes;
}
// A received datagram's wire, decided by its LENGTH -- which is the frame
// check this carrier has always used, so the classification costs nothing new.
inline bool len_is_wide(int n)   { return n == kPacketWideBytes; }
inline bool len_is_ours(int n)   { return n == kPacketNarrowBytes || n == kPacketWideBytes; }
inline unsigned char len_version(int n) {
    return len_is_wide(n) ? kWireVersionWide : kWireVersionNarrow;
}

// WRITE THE LIVE MASK IN BOTH PLACES, ALWAYS. `live` keeps its version-2
// meaning on every wire (slots 0..7) so a narrow datagram is byte-identical to
// what this port has always sent, and `live_wide` carries the whole mask --
// transmitted only when the datagram is the wide length, but always correct in
// memory, which is what lets dispatch() normalise a received narrow packet into
// the same field and give every reader downstream ONE field to read.
inline void set_live(Packet &p, unsigned mask) {
    p.live      = (unsigned char)(mask & 0xffu);
    p.live_wide = mask;
}
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
// Whether that value came from the env rather than the mode default. Only
// used to make the child's "the parent overruled you" line say WHY the number
// it was asked for did not survive, which is the difference between a knob
// that looks broken and one that is documented.
bool g_delay_from_env = false;
bool delay_from_env() { return g_delay_from_env; }
enum : int { kPipeDepth = 64 };        // rounds in flight; ~2 s at 30 Hz
// RAISED FROM 8 TO 15 (run rel0215 lane LAGDELAY). 8 frames is 133 ms of
// cover at the 16.67 ms this seam's rounds are paced to, and the field logs
// this lane was given carry handshake round trips of 94..172 ms -- so the old
// ceiling could not cover the worst path the relay actually sees, and a
// session on that path starves on most frames no matter what anyone sets. 15
// covers 250 ms.
//
// NOTHING SIZED BY IT NEEDS TO GROW. The ring is kPipeDepth=64 rounds and the
// only requirement on it is that a round stays in the ring for the whole time
// it is in flight -- exchange() reads round R-N while the newest opened round
// is R, so the live span is N+1 entries, and the static_assert below is that
// statement rather than a comment about it. The wire carries the number in
// ACCEPT bits 8..15, which held 255 before this file existed.
enum : int { kInputDelayMax = 15 };
static_assert(kInputDelayMax + 1 < kPipeDepth,
              "the pipeline ring must hold every round in flight: frame R "
              "reads round R-N while round R is open, so N+1 entries are "
              "live at once and the ring has to be strictly larger");
// THE ROUND PERIOD, in hundredths of a millisecond. The seam's rounds are
// paced at 60 Hz, which is what makes the shipped relay default of 5 frames
// the 83 ms it was measured to be worth (p50 lockstep round trip 90.6 ms).
// The 33 ms in the pipelining banner above is the older 30 fps framing and is
// left standing because the argument it makes does not depend on the number.
enum : int { kRoundPeriodMs100 = 1667 };
struct PipeRound {
    unsigned      round;
    unsigned char blocks[kCommsMaxPlayers][kCommsBlockBytes];
    unsigned      mask;
    bool          valid;
    bool          sent;                // parent: this aggregate went out
    // ---- ROLLBACK LEDGER (port/rollback). Untouched with the mode off. ----
    unsigned      live;        // the live mask the PARENT declares for this
                               // round: set at the parent's exchange(), on a
                               // child copied off the aggregate that carried it
    bool          live_known;  // child: an aggregate for this round landed
    bool          consumed;    // exchange() handed this round to the game
    unsigned      served_live; // the live mask the game was handed
    unsigned      pred_mask;   // slots the game was handed a PREDICTION for
    unsigned      sent_mask;   // parent: the mask of the last aggregate sent
    unsigned      last_agg_ms; // parent: when the last aggregate for it left
    bool          live_set;    // parent: s.live was declared at staging
    unsigned char pred[kCommsMaxPlayers][kCommsBlockBytes];
};
PipeRound g_pipe[kPipeDepth];

// ===========================================================================
// ROLLBACK NETCODE (port/rollback). ON BY DEFAULT since the owner's decision
// of 2026-09-03: g_rollback is true unless settings.json says NetMode
// "lockstep", SM64DS_NETMODE=lockstep, or the seated session is wider than
// kRollbackMaxPlayers (the width it is proven to; the install block says so
// on one line). With it false every line below is dead and the stop-and-wait
// / pipelined paths run byte-for-byte as before; a solo run never installs
// the transport at all, so it is never reached there.
// status/ROLLBACK.md (the spike) has the numbers
// and the hook-point argument; status/ROLLBACK_SHIP.md has what shipped.
//
// THE IDEA, in one paragraph. The pipelined ring above already files every
// slot's block by round number. Rollback mode consumes round R the frame it
// is opened, at input delay ZERO: this console's own block is real, and any
// live slot whose block for R has not arrived is handed a PREDICTION (its
// last confirmed block, heading extrapolated). What was predicted is kept in
// the ledger; when the real block lands and differs, the frame loop
// (hal/rollback.cpp) restores the snapshot it took before that round, rewinds
// this clock (comms_rb_rewind) and replays: exchange() then serves the ring
// -- confirmed blocks where they exist, fresh predictions elsewhere -- and
// publishes nothing, because this console's own blocks for those rounds are
// already on the wire and must not change.
//
// THE WINDOW. A slot may run at most kRbWindow rounds ahead of its last
// confirmed block. Past that exchange() STALLS (returns 0, the ROM sleeps
// through the pump exactly as a starve does today) for kRbGraceMs; then the
// parent DROPS the slot through the same retire path a Bye takes, and a
// child whose PARENT went silent leaves the session the way a parent's Bye
// makes it leave. A child never drops another child on its own: the parent
// is the clock and its revised aggregates carry the verdict.
//
// A DROP IS ROUND-EXACT. The slot leaves the session at round D = its last
// confirmed round + 1 -- the first round anybody had to guess for it -- and
// the parent revises every round from D on: the per-round live mask loses the
// slot and the aggregates go out again. Every console then finds its served
// live mask for D disagreeing with the declared one, rolls back to D, and
// re-runs with the slot absent, which the ROM's own unpack loop turns into a
// zeroed record (src/func_0203ea5c.c:290, func_0205a588 on a null block).
// That is the state the ROM's own records take when a DS peer stops sending.
// ===========================================================================
bool g_rollback = false;
enum : unsigned { kRbWindow = 8 };       // rounds predicted past the last confirmed
enum : unsigned { kRbGraceMs = 1000 };   // stall this long, then drop
bool     g_rb_replaying = false;
unsigned g_rb_replay_end = 0;            // live round while replaying
unsigned g_rb_scan_low = 0;              // oldest round not yet fully confirmed
unsigned g_rb_live_round = 0;            // child: the round g_live was read from
unsigned g_rb_stall_start_ms = 0;        // 0 = not stalling
unsigned g_rb_stall_mask = 0;            // slots currently past the window
unsigned long long g_rb_predicted = 0;   // slot-rounds served as a prediction
unsigned long long g_rb_confirmed_ok = 0;// of those, confirmed equal
unsigned long long g_rb_mispredicted = 0;// of those, confirmed different
unsigned long long g_rb_stalled = 0;     // exchange() calls that stalled
unsigned long long g_rb_stall_events = 0;// stall episodes
unsigned long long g_rb_drops = 0;       // slots retired by the grace rule
unsigned long long g_rb_rewinds = 0;     // comms_rb_rewind calls
unsigned long long g_rb_replayed = 0;    // rounds served in replay
unsigned long long g_rb_xcalls = 0, g_rb_xzero = 0;   // live exchange calls, zero returns
unsigned g_rb_live_peak = 0;             // every slot that was ever live this session
unsigned g_rb_dropped = 0;               // parent: slots the grace rule retired; their
                                         // late datagrams are ignored for the session
unsigned long long g_rb_rxcalls = 0, g_rb_rxzero = 0; // the same while replaying
struct RbSlotPred {
    bool          have;
    unsigned      round;                  // last confirmed round
    unsigned char block[kCommsBlockBytes];
    int           head_delta;             // heading change per round, last seen
};
RbSlotPred g_rb_pred[kCommsMaxPlayers];
enum : unsigned { kRbAggMs = 8 };        // parent: min gap between growth aggregates
bool     g_rb_det_reuse = false;         // replay serves the ledger's LAST guess
bool     g_rb_flush_wait = false;        // area change: stall until nothing is a guess
unsigned g_rb_unrecoverable = 0;         // rollbacks the ring could not honour
unsigned char g_rb_my_served[kCommsBlockBytes];
unsigned g_rb_served_round = ~0u;

inline bool pipelined() { return g_input_delay > 0 || g_rollback; }
inline unsigned rb_live_round() {
    return g_rb_replaying ? g_rb_replay_end : g_round;
}
inline short rb_heading(const unsigned char *b) {
    return (short)((unsigned)b[0x0B] | ((unsigned)b[0x0C] << 8));
}
// A real block for slot k, round r, landed (own blocks included). Keeps the
// per-slot prediction source at the NEWEST confirmed round; an older round
// arriving late (a resend) changes nothing.
void rb_note_block(int k, unsigned r, const unsigned char *b) {
    if (!g_rollback) return;
    RbSlotPred &p = g_rb_pred[k];
    if (p.have && (int)(r - p.round) <= 0) return;
    if (p.have) {
        const int d = (short)(rb_heading(b) - rb_heading(p.block));
        const int span = (int)(r - p.round);
        p.head_delta = span > 0 ? d / span : 0;
    }
    p.have  = true;
    p.round = r;
    std::memcpy(p.block, b, kCommsBlockBytes);
}
// The guess for slot k at round r: the last confirmed block, the heading
// carried forward at its last per-round delta (the remote camera's yaw
// moves smoothly while it turns; a flat repeat mispredicts every frame of a
// turn), and the frame counter at +2 overwritten with THIS console's so the
// ROM's clock check at src/func_0203ea5c.c:418 agrees.
void rb_predict(int k, unsigned r, const unsigned char *mine,
                unsigned char *out) {
    const RbSlotPred &p = g_rb_pred[k];
    std::memcpy(out, p.block, kCommsBlockBytes);
    const int span = (int)(r - p.round);
    const short h = (short)(rb_heading(p.block) + p.head_delta * span);
    out[0x0B] = (unsigned char)(h & 0xFF);
    out[0x0C] = (unsigned char)((h >> 8) & 0xFF);
    std::memcpy(out + 2, mine + 2, 4);
}
// Parent: the lowest round not yet broadcast. Child: the lowest round whose
// aggregate has not come back, and therefore the one to republish.
unsigned  g_pipe_low = 0;
unsigned long long g_pipe_starved = 0;  // exchange() had to return 0 anyway

// ===========================================================================
// STARVATION ATTRIBUTION (run rel0215 lane LAGDELAY).
//
// `starved` says a session's input delay is too shallow for its path. It does
// not say WHOSE path, and in a session of seven that is most of the question:
// one peer on a bad connection starves every frame for all six others, and the
// close line as it stood named nobody. These counters are the answer -- one
// per slot, incremented for every slot whose record was missing from the round
// the frame wanted, so the summary at close reads "starvedby=3:1974" and the
// field log names the laggy peer instead of implying the session was.
//
// A starve is a PER-FRAME event on a bad path (1976 of them in 3032 rounds, in
// the logs this lane was given), so the per-event line is rate limited to one
// a second. The counters are not: they are counters.
// ===========================================================================
unsigned long long g_starve_by_slot[kCommsMaxPlayers] = { 0 };
unsigned  g_starve_last_round   = 0;
unsigned  g_starve_last_log_ms  = 0;
bool      g_starve_ever         = false;
enum : unsigned { kStarveLogMs = 1000 };

// ===========================================================================
// ADAPTIVE INPUT DELAY (run rel0215 lane LAGDELAY). PARENT SIDE.
//
// THE NUMBER WAS A CONSTANT AND THE PATH IS NOT. The relay default of 5 frames
// was measured on ONE path (p50 90.6 ms, p95 98.8 ms) and shipped as the whole
// world's answer. The field logs carry handshake round trips of 94, 118, 141
// and 172 ms in the same week, and 5 frames is 83 ms of cover -- so on the
// slower halves of that spread the pipeline is short by design and exchange()
// starves on the majority of frames. That is what starved=1976 in 3032 rounds
// is: not a defect, an under-sized constant.
//
// SO THE PARENT SIZES IT FROM WHAT IT MEASURED. Every child reports the round
// trip it measured across its own handshake; the parent takes the WORST of
// them, because one number has to cover every peer, and turns it into frames:
//
//     N = ceil(worst_rtt_ms * safety / 16.67) + jitter_margin
//
// clamped to [floor, kInputDelayMax]. safety (125 percent) is headroom for a
// path that is worse under load than it was at handshake; jitter_margin (1
// frame) is for the variance a single handshake sample cannot see; the floor
// (5) keeps a session that measured suspiciously well from shipping less cover
// than the constant it replaced.
//
// EVERY PEER STILL RUNS THE SAME NUMBER. That invariant is not weakened here
// and could not be: it is published through the ACCEPT field exactly as
// before, and the three rules below are what keep it true while the number is
// allowed to move at all.
//
//   1. IT ONLY MOVES BEFORE FRAME 0. g_delay_frozen goes true the first time
//      the parent enters the pipelined path in exchange(), which is the frame
//      the ROM's wait loop first asks for a round. Every join happens in the
//      lobby, before that. After the freeze the number is a constant again.
//   2. A CHILD ONLY ADOPTS A CHANGE BEFORE ITS OWN FRAME 0. Same reason,
//      stated locally: a peer that has handed the game no round cannot have
//      handed it a round under the old N, so there is no frame index whose N
//      disagrees. A change arriving after that is REFUSED, loudly.
//   3. THE PARENT WILL NOT SERVE ROUND 0 UNTIL EVERY CHILD IT RETOLD HAS SAID
//      BACK WHAT IT IS RUNNING. This is the one that makes it sound rather
//      than merely likely. A child cannot produce frame 0 without the
//      parent's aggregate, so withholding the aggregate withholds frame 0
//      from the whole session -- and the parent's own exchange() starves in
//      the meantime, which is an already-defined, already-counted, harmless
//      state. Only children whose told number CHANGED after their accept are
//      gated, so a session whose delay never moved behaves exactly as before.
//
// AND IT ONLY ENGAGES WHEN EVERY LIVE CHILD HAS REPORTED. A peer of an older
// generation never sends a report, so it would never adopt a raised number and
// never ack one -- the mixed-build desync. Requiring a report from everybody
// means the sizing is off in exactly the sessions where it would be unsafe,
// and those sessions keep the shipped constant they always had.
// ===========================================================================
bool g_adaptive_delay  = false;         // sizing is in effect on this end
bool g_delay_frozen    = false;         // parent: frame 0 has been asked for
bool g_frames_produced = false;         // this end handed the game a round
// THE THREE TERMS OF THE FORMULA, NAMED. The floor is the mode default every
// shipped build already runs on a relay; the margin is one frame of jitter
// cover on top of the measured path; the safety is the 125 percent the field
// numbers were worked against (94 ms -> 9, 172 ms -> 14 in the banner).
enum : int { kDelayFloorDefault = 5 };       // frames
enum : int { kDelayMarginDefault = 1 };      // frames
enum : int { kDelaySafetyPctDefault = 125 }; // percent of the worst round trip
int  g_delay_floor       = kDelayFloorDefault;      // SM64DS_COMMS_DELAY_FLOOR
int  g_delay_margin      = kDelayMarginDefault;     // SM64DS_COMMS_DELAY_MARGIN, frames
int  g_delay_safety_pct  = kDelaySafetyPctDefault;  // SM64DS_COMMS_DELAY_SAFETY, percent
// ARMED AT INSTALL, kept for the life of the process. g_adaptive_delay is the
// live flag and a stand-down clears it for the session that stood down; this
// one is what delay_state_reset() re-arms from, so the next session in the
// same launch (a rematch) sizes again instead of inheriting the stand-down.
bool g_adaptive_armed  = false;
// THE ROSTER AT THE FREEZE. After the freeze the depth cannot move, so a
// joiner seated after it is handed the frozen number in its own accept (or
// refused) and has nothing to confirm; the round-0 ack gate only ever waits
// on the children that were in the session when the number could still
// change. Without this a post-freeze joiner whose ack had not landed held
// every other console's next round.
unsigned g_frozen_live = 0;
// Parent, per child slot. rtt is the worst of the child's own handshake sample
// and the parent's own accept-to-report measurement of the same path.
int  g_child_rtt_ms[kCommsMaxPlayers];
int  g_child_told_delay[kCommsMaxPlayers];   // last number sent to this slot
int  g_child_ack_delay[kCommsMaxPlayers];    // last number it said it runs
// THE VALUE THE SESSION WOULD HAVE RUN IF NOTHING HAD BEEN SIZED -- the mode
// default, captured before the first recompute. It is the value to fall BACK
// to, and it is the reason a fallback is safe: every build that has ever
// shipped can express it, because it is the number they all already used.
int  g_delay_presize = -1;
// AND THE SAME VALUE, KEPT ACROSS THE PER-SESSION RESET. The install step
// captured g_delay_presize and lb_open() then ran delay_state_reset(), which
// wiped it to -1 -- so the withdrawal in recompute_adaptive_delay() and the
// one in the grace stand-down were both guarded by a value that was never
// there, and no session in this tree's logs ever printed a WITHDRAWN line.
// The stand-down then reported "the mode default of 11" while leaving 11 in
// force, and an old build handed 11 in its first accept dropped it and kept
// its own number: two depths in one session. The reset restores the mode
// default from here instead of forgetting it.
int  g_delay_mode_default = -1;
// THE DEPTH EVERY SHIPPED BUILD CAN EXPRESS. 0.3.2 and earlier clamp an
// adopted delay at 8 (`parent_delay <= kInputDelayMax` against their own
// smaller cap), so a published 11 does not raise them -- it is DROPPED, in
// silence, and they keep the number they had. Anything at or under this is
// adoptable by every peer alive; anything over it may only be published to a
// session in which every live child has PROVED it is a new build by reporting.
enum : int { kLegacyInputDelayMax = 8 };
unsigned g_accept_sent_ms[kCommsMaxPlayers]; // when the last unicast went out
unsigned g_sizing_wait_since_ms = 0;         // 0 = not waiting yet
unsigned long long g_sizing_holds = 0;
enum : unsigned { kSizingGraceMs = 400 };
unsigned g_delay_gate_since_ms = 0;          // 0 = the gate is open
unsigned g_delay_gate_last_log_ms = 0;
// AND ONE OF ITS OWN FOR THE LATE-JOIN REFUSAL. It shared the hold line's
// stamp at first, so a refusal inside 3 s of a hold line printed nothing --
// the one line a refused player's host most needs, swallowed by a line about
// something else.
unsigned g_refuse_join_last_log_ms = 0;
unsigned long long g_delay_gate_holds = 0;
enum : unsigned { kDelayGateLogMs = 3000 };
// Child. The report is a JOIN with bit 30 set in `have`; it is repeated at the
// republish interval until the parent's next unicast ACCEPT acknowledges it,
// because a lost report leaves the parent sizing off one fewer peer.
unsigned g_adopt_refuse_last_ms = 0;     // child: rate limit on the refusal
bool     g_legacy_peer_sim = false;      // SM64DS_COMMS_LEGACY_PEER
bool     g_report_acked   = false;
unsigned g_last_report_ms = 0;
int      g_report_tries   = 0;
enum : int { kMaxReportTries = 12 };
// The floor under the report's retry interval: an ack cannot come back faster
// than the path, so retrying at loopback's 4 ms republish rate would send
// every retry before the first ack could possibly land.
enum : unsigned { kReportRetryFloorMs = 50 };
// JOIN `have` has been zero on the wire since MP2, which makes it the one free
// word in the handshake. Bit 30 tags a REPORT; an older parent reads `have` on
// a JOIN nowhere at all and answers a report exactly as it answers a re-knock.
enum : unsigned { kJoinReportBit = 0x40000000u };
// ACCEPT `have`: bit 31 is the slot assignment and 8..15 the delay, both
// frozen. Bit 16 is new and says "your report was recorded".
enum : unsigned { kAcceptRttAckBit = 0x00010000u };
// Bit 17 (port/rollback): the parent runs NetMode rollback. Carried in the
// accept for the same reason the delay is: one end predicting while the
// other waits is two different timelines. An older parent never sets it.
enum : unsigned { kAcceptRollbackBit = 0x00020000u };

void delay_state_reset() {
    for (int i = 0; i < kCommsMaxPlayers; ++i) {
        g_child_rtt_ms[i]      = -1;
        g_child_told_delay[i]  = -1;
        g_child_ack_delay[i]   = -1;
        g_accept_sent_ms[i]    = 0;
        g_starve_by_slot[i]    = 0;
    }
    // THE RATE-LIMIT STAMPS ARE PER SESSION, NOT PER PROCESS. Left standing,
    // a second session in one launch inherits the first one's clock and
    // swallows its own first starve line -- the one line a field log most
    // needs, dropped precisely because an earlier session already said it.
    g_starve_last_log_ms     = 0;
    g_delay_gate_last_log_ms = 0;
    g_refuse_join_last_log_ms = 0;
    g_adopt_refuse_last_ms   = 0;
    g_delay_gate_holds       = 0;
    g_sizing_holds           = 0;
    // The fallback value survives the reset (see g_delay_mode_default), and
    // a session that starts while the sizing is armed starts from it, not
    // from whatever an earlier session in this launch was sized to.
    g_delay_presize          = g_delay_mode_default;
    // A stand-down clears g_adaptive_delay for ITS session only. The next
    // session in this launch starts armed again if install armed it, which
    // is what makes "the next match sizes for them" true (section 3(c) of
    // status/LAGDELAY.md) rather than a claim.
    if (g_adaptive_armed) g_adaptive_delay = true;
    if (g_adaptive_delay && g_delay_mode_default >= 0)
        g_input_delay = g_delay_mode_default;
    g_delay_frozen        = false;
    g_frozen_live         = 0;
    g_frames_produced     = false;
    g_report_acked        = false;
    g_last_report_ms      = 0;
    g_report_tries        = 0;
    g_delay_gate_since_ms = 0;
    g_sizing_wait_since_ms = 0;
    g_starve_ever         = false;
}

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
// ===========================================================================
// THE ROSTER ANNOUNCE (run vs4p). PARENT ONLY, and it exists because a session
// of THREE OR FOUR cannot form without it.
//
// A child stops knocking the moment it is accepted: service()'s JOIN retry is
// gated on kCommsConnecting. The parent answers each JOIN with a UNICAST
// ACCEPT to the joiner alone. So slot 1, accepted while it was the only child,
// is told live mask 0x3 and is then never told anything again until the
// lockstep starts carrying blocks -- and the lockstep does not start until
// every console has seated a world, which is precisely what they are all
// waiting to be allowed to do.
//
// MEASURED, four windows on one loopback session at 577b48832:
//
//     slot 1 joined at round 0;   live mask 0x3, players 2
//     [a2] VS: 2 players       <- slot 0 and slot 1 seated a TWO-player world
//     slot 2 joined at round 62;  live mask 0x7, players 3
//     slot 3 joined at round 109; live mask 0xf, players 4
//
// Slots 2 and 3 reached the wire and never reached the world. So when the
// roster GROWS, the parent tells everybody, and a child that is already
// connected updates its live mask from it -- on_child_packet's kTypeAccept arm
// already does `g_live = p.live` unconditionally for an accept that names its
// own slot, so nothing on the child side has to change.
//
// REPEATED FOR A WINDOW rather than sent once, because a lost announce would
// strand exactly the console the announce exists to inform, and there is no
// other traffic on this path to heal it -- nobody is exchanging anything yet.
//
// TWO SECONDS, AND A 50 ms FLOOR UNDER THE RATE. The rate wants to follow the
// republish interval, which is already tuned per mode, but that interval is
// 4 ms on loopback and up to 250 ms over the relay, and taking it neat gives
// 500 announces in the window at one end and three at the other -- spam at one
// end and thin cover at the other. The floor cuts loopback to 40 and the
// window gives the relay 8. Either way it is a handful of 144-byte datagrams
// per join and nothing at all once the session is formed.
//
// EVERY FRESH JOIN RE-ARMS THE WINDOW, so a launcher that spaces its consoles
// out by more than the window is still covered: the announce that matters is
// the one after the LAST join, and that one is always sent.
enum : unsigned { kRosterAnnounceMs = 2000, kRosterAnnounceEveryMs = 50 };
unsigned g_roster_until   = 0;    // announce while now_ms() is below this
unsigned g_last_roster_ms = 0;
// JOINs this child has sent while connecting, for the "nobody is answering,
// and one reason is a build mismatch" hint in service(). Six is past the
// backoff's first second and well short of the ROM's twenty, so the hint
// lands while a player is still watching and long before the session is
// declared dead.
unsigned g_joins_sent = 0;
enum : unsigned { kJoinsBeforeHint = 6 };

unsigned long long g_sent = 0, g_recvd = 0, g_dropped = 0;
// run vs16: how many narrow (version 2) joiners this wide session turned away.
// Counted so "nobody could join my sixteen-player room" is a number in the
// readout rather than an inference from silence.
unsigned long long g_refused_narrow = 0;
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
    // kRawMaxBytes, not kAuxMaxBytes: the induction ring holds whatever came
    // off the socket, and since run vs16 that can be a wide lockstep datagram.
    // Holding it at the aux size would have made induced latency silently drop
    // every sixteen-player packet -- and induced latency is exactly the rig the
    // desync proofs run under, so the bug would have hidden inside its own test.
    unsigned char buf[kRawMaxBytes];
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
bool ieq_word(const char *a, const char *b) {
    for (; *a && *b; ++a, ++b)
        if ((*a | 0x20) != (*b | 0x20)) return false;
    return *a == 0 && *b == 0;
}

int popcount_live(unsigned m) {
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
    const int len = packet_bytes();
    send_raw(&p, len, a);
    for (int i = 1; i < g_dup; ++i) { send_raw(&p, len, a); ++g_dup_sends; }
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
    p.version = wire_version();
    p.type    = type;
    p.slot    = (unsigned char)g_slot;
    set_live(p, g_live);
    p.round   = g_round;
}

// THE ROSTER, SENT TO EVERYONE. See the banner over kRosterAnnounceMs for why
// a three- or four-player session cannot form without this.
//
// It is an ACCEPT because that is the packet a child already reads its live
// mask out of, and reusing it means NO WIRE CHANGE and no new type for an
// older build to choke on -- an ACCEPT is what a peer of this generation
// already expects to see.
//
// THE ASSIGNMENT BIT IS CLEARED, and that is the one thing this must not get
// wrong. A JOIN's own accept carries 0x80000000 | slot, and a DIRECT-MODE
// child adopts that slot whenever it differs from its own. Broadcast with the
// bit set, the accept that welcomes slot 3 would move slot 1 to slot 3 and
// collapse two players onto one seat. So `have` carries the input delay and
// nothing else: the delay is one number the parent applies to the whole
// session, every child may safely read it, and an already-connected child
// ignores it anyway (the adoption sits inside the not-yet-connected arm).
void announce_roster() {
    if (g_role != kRoleParent) return;
    if ((g_live & ~1u) == 0) return;          // no children to tell
    Packet a;
    fill_header(a, kTypeAccept);
    set_live(a, g_live);
    a.have = ((unsigned)(g_input_delay & 0xFF) << 8) |
             (g_rollback ? kAcceptRollbackBit : 0u);   // port/rollback: the mode too
    send_to_children(a);
    // WHAT EVERY LIVE CHILD HAS NOW BEEN TOLD. Recorded here and not only in
    // the unicast arm, because rule 3 asks "has this slot confirmed the value
    // in force", and a value published by a roster broadcast is in force just
    // as much as one published by an accept.
    for (int k = 1; k < kCommsMaxPlayers; ++k)
        if (g_live & (1u << k)) g_child_told_delay[k] = g_input_delay;
}

// SIZE THE SESSION'S INPUT DELAY FROM WHAT THE PATHS MEASURED. See the banner
// over g_adaptive_delay for the rules that keep this from breaking the
// same-number-everywhere invariant; this function is only the arithmetic and
// the publish.
void recompute_adaptive_delay(const char *why) {
    if (g_role != kRoleParent) return;
    if (!g_adaptive_delay || g_delay_frozen) return;

    // EVERY LIVE CHILD, OR FALL BACK -- and "or nothing" was the bug. This
    // guard used to just `return` on an unreported child, which is right the
    // first time it runs and WRONG every time after, because by then a number
    // may already be published and adopted. The sequence that broke it:
    //
    //   new children join and report   -> sized to 11, published, adopted
    //   an OLD build joins afterwards  -> its ACCEPT carries 11, its own adopt
    //                                     clamp is 8, so 11 is DROPPED and it
    //                                     keeps 5 while everyone else runs 11
    //
    // Same round, two depths, which is exactly the desync this whole field
    // exists to prevent. Returning early left the raised value standing; the
    // answer is to TAKE IT BACK. Every peer is still before frame 0 (the
    // parent withholds round 0 until the gate below opens), so a fallback here
    // replays nothing and skips nothing, and the value fallen back to is the
    // mode default, which every build that has ever shipped can express.
    int worst = -1, worst_slot = -1;
    bool all_reported = true;
    for (int k = 1; k < kCommsMaxPlayers; ++k) {
        if ((g_live & (1u << k)) == 0) continue;
        if (g_child_rtt_ms[k] < 0) { all_reported = false; continue; }
        if (g_child_rtt_ms[k] > worst) { worst = g_child_rtt_ms[k]; worst_slot = k; }
    }
    if (!all_reported) {
        if (g_delay_presize >= 0 && g_input_delay != g_delay_presize) {
            std::fprintf(stderr, "[comms:loopback] a live peer has not "
                         "reported a round trip, so the sized depth of %d is "
                         "WITHDRAWN and this session falls back to %d. A peer "
                         "that does not report may be a build whose adopt "
                         "clamp is %d, which would DROP a larger number in "
                         "silence and leave one console at a depth of its own "
                         "-- and the fallback value is the one every build "
                         "already runs. Nobody has consumed a round yet.\n",
                         g_input_delay, g_delay_presize, kLegacyInputDelayMax);
            g_input_delay = g_delay_presize;
            announce_roster();
        }
        return;
    }
    if (worst < 0) return;                       // no children yet

    // ceil(worst * safety_pct/100 / (kRoundPeriodMs100/100)) in integers.
    const long long num = (long long)worst * g_delay_safety_pct * 100;
    const long long den = 100LL * (long long)kRoundPeriodMs100;
    int frames = (int)((num + den - 1) / den) + g_delay_margin;
    if (frames < g_delay_floor)   frames = g_delay_floor;
    if (frames > kInputDelayMax)  frames = kInputDelayMax;
    if (frames == g_input_delay) return;

    // ONE LINE, AND IT CARRIES THE EVIDENCE. A playlog that says only "delay
    // 14" leaves the next reader unable to tell a measured 14 from a typo, so
    // the round trips it was derived from go on the same line.
    char rtts[256];
    int  w = 0;
    rtts[0] = 0;
    for (int k = 1; k < kCommsMaxPlayers && w < (int)sizeof rtts - 16; ++k) {
        if ((g_live & (1u << k)) == 0) continue;
        w += std::snprintf(rtts + w, sizeof rtts - (size_t)w, "%ss%d=%dms",
                           w ? " " : "", k, g_child_rtt_ms[k]);
    }
    std::fprintf(stderr, "[comms:loopback] adaptive input delay %d frame(s) "
                 "(was %d): ceil(%d ms worst * %d%% / %d.%02d ms) + %d, floor "
                 "%d cap %d; worst is slot %d; per-child rtt %s [%s]\n",
                 frames, g_input_delay, worst, g_delay_safety_pct,
                 kRoundPeriodMs100 / 100, kRoundPeriodMs100 % 100,
                 g_delay_margin, g_delay_floor, kInputDelayMax, worst_slot,
                 rtts, why ? why : "-");
    g_input_delay = frames;
    announce_roster();
}

// RULE 3, ONE PREDICATE. False means round 0 is held back because a child that
// was retold the delay has not yet said back what it is running -- and a child
// running the wrong N is a divergence, where a held round is a stall.
bool delay_gate_open() {
    if (g_role != kRoleParent) return true;
    // NOT KEYED ON g_adaptive_delay, and that was the second half of the same
    // bug: the stand-down path sets that flag false, which made this predicate
    // answer "open" unconditionally at the exact moment a raised value was
    // still in force with an unconfirmed peer in the session.
    //
    // The honest key is the VALUE. If what is in force is the value every
    // build would have run anyway, there is nothing to confirm and this gate
    // does not exist -- which is why a session that never sized behaves
    // byte-identically to before this lane. If the value in force is anything
    // else, EVERY live child must have said back that exact number, including
    // a child being told for the first time. The old `moved` test skipped
    // those, which is how a late joiner walked straight through.
    if (g_delay_presize < 0 || g_input_delay == g_delay_presize) return true;
    // AFTER THE FREEZE, ONLY THE ROSTER THAT WAS THERE FOR IT. The number
    // cannot move past the freeze, so a joiner seated later is handed it in
    // its first accept (or refused, if it is deeper than 8) and cannot have
    // produced a frame under any other value; waiting on its ack would hold
    // everyone else's next round for a confirmation that proves nothing.
    // Before the freeze the whole live roster must confirm, because that is
    // the window in which the number is allowed to change under a child.
    // Not scoped to !g_delay_frozen outright: the freeze and the first serve
    // of round 0 happen in the same exchange, and the P1 hold spans the
    // FROZEN line (held 156 ms, confirmed after it), so a gate that opened
    // at the freeze would serve round 0 to a child still on the old number.
    const unsigned must = g_delay_frozen ? (g_live & g_frozen_live) : g_live;
    for (int k = 1; k < kCommsMaxPlayers; ++k) {
        if ((must & (1u << k)) == 0) continue;
        if (g_child_ack_delay[k] != g_input_delay) return false;
    }
    return true;
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
        if (claimed < 0 || claimed >= slot_cap()) return -1;
        return claimed;
    }

    if (g_net_mode == kNetDirect) {
        for (int k = 0; k < kCommsMaxPlayers; ++k)
            if (g_peer_known[k] && same_addr(from, g_peer_addr[k])) return k;
        if (!learn_ok || g_role != kRoleParent) return -1;
        // A new child. It PROPOSES a slot and the parent is the authority:
        // take the proposal when it is free, otherwise hand out the lowest
        // free one. The assignment travels back in the ACCEPT.
        int want = (claimed >= 1 && claimed < slot_cap()) ? claimed : 1;
        if (g_peer_known[want]) {
            want = -1;
            for (int k = 1; k < slot_cap(); ++k)
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
    if (claimed < 0 || claimed >= slot_cap()) return -1;
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
void pipe_send_aggregate_rb(PipeRound &s) {
    // The ROLLBACK aggregate: sent whenever the round's real mask GREW (so a
    // child confirms the parent's own block one hop after the parent staged
    // it, without waiting for the slowest child), and again when a retire
    // revised the round's live mask. It carries the PER-ROUND live mask, not
    // the parent's current one, which is what makes a drop round-exact on
    // every console.
    Packet b;
    fill_header(b, kTypeBlocks);
    b.round = s.round;
    set_live(b, s.live);
    b.have  = s.mask & s.live;
    for (int i = 0; i < kCommsMaxPlayers; ++i)
        if (b.have & (1u << i))
            std::memcpy(b.blocks[i], s.blocks[i], kCommsBlockBytes);
    send_to_children(b);
    s.sent_mask = b.have;
    if (b.have == s.live) s.sent = true;
}

void pipe_try_broadcast() {
    if (g_role != kRoleParent || !pipelined()) return;
    if (g_state != kCommsParentConnected) return;
    if (g_rollback) {
        const unsigned end = rb_live_round();
        bool contiguous = true;
        for (unsigned q = g_pipe_low; (int)(q - end) < 0; ++q) {
            PipeRound *s = pipe_find(q);
            if (!s) { contiguous = false; continue; }
            if ((s->mask & s->live) != s->sent_mask) {
                // Every growth would be O(players) datagrams per round per
                // child. Send on the first (the parent's own block, so a
                // child confirms it one hop later), on completion, on a
                // revision, and otherwise at most every kRbAggMs.
                const unsigned have = s->mask & s->live;
                const bool complete = have == s->live;
                const bool forced   = s->sent_mask == 0 || s->sent_mask == ~0u;
                if (complete || forced ||
                    (unsigned)(now_ms() - s->last_agg_ms) >= kRbAggMs) {
                    pipe_send_aggregate_rb(*s);
                    s->last_agg_ms = now_ms();
                }
            }
            if (contiguous && s->sent && q == g_pipe_low) ++g_pipe_low;
            else contiguous = false;
        }
        return;
    }
    // RULE 3. A child that was retold the delay and has not said back what it
    // is running may still be on the old number, and serving it round 0 would
    // start it simulating at a depth nobody else is at. Hold instead: a child
    // cannot produce a frame without an aggregate, so holding holds the whole
    // session, and the parent's own exchange() starves in the meantime, which
    // is a state this file already defines and counts.
    if (!delay_gate_open()) {
        ++g_delay_gate_holds;
        const unsigned t = now_ms();
        if (g_delay_gate_since_ms == 0) g_delay_gate_since_ms = t ? t : 1;
        if (g_delay_gate_last_log_ms == 0 ||
            (unsigned)(t - g_delay_gate_last_log_ms) >= kDelayGateLogMs) {
            g_delay_gate_last_log_ms = t ? t : 1;
            char who[128];
            int w = 0;
            who[0] = 0;
            for (int q = 1; q < kCommsMaxPlayers && w < (int)sizeof who - 12; ++q) {
                if ((g_live & (1u << q)) == 0) continue;
                if (g_child_ack_delay[q] == g_input_delay) continue;
                w += std::snprintf(who + w, sizeof who - (size_t)w, "%s%d(runs %d)",
                                   w ? "," : "", q, g_child_ack_delay[q]);
            }
            std::fprintf(stderr, "[comms:loopback] holding round %u: input "
                         "delay is %d and slot(s) %s have not confirmed it. "
                         "The roster announce repeats until every child "
                         "seated before the freeze says the number back, "
                         "normally one round trip; a child that never does "
                         "holds the session until the ROM's own bound ends "
                         "it. Holding is a stall where serving would be a "
                         "desync. Held %ums.\n",
                         g_pipe_low, g_input_delay, who,
                         (unsigned)(t - g_delay_gate_since_ms));
        }
        // Keep telling them. announce_roster() is the same packet the accept
        // arm sends and is what a child adopts from.
        if ((unsigned)(t - g_last_roster_ms) >= (unsigned)g_resend_ms) {
            g_last_roster_ms = t;
            announce_roster();
        }
        return;
    }
    if (g_delay_gate_since_ms != 0) {
        std::fprintf(stderr, "[comms:loopback] every peer has confirmed input "
                     "delay %d; the session runs (held %ums, %llu turns)\n",
                     g_input_delay,
                     (unsigned)(now_ms() - g_delay_gate_since_ms),
                     g_delay_gate_holds);
        g_delay_gate_since_ms = 0;
    }
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

// ROLLBACK: slot k leaves the session at round D = last confirmed + 1. Every
// round from D on is revised (live mask without k) and goes out again; the
// boundary scan on every console -- this one included -- then sees the served
// live mask disagree with the declared one and rolls back to D. See the
// ledger banner. Used by the parent's Bye handling AND the grace-rule drop, so
// a peer that quit and a peer that fell off the wire leave the same records.
void rb_retire_slot(int k, const char *why) {
    g_live &= ~(1u << k);
    g_stage_mask &= ~(1u << k);
    const unsigned end = rb_live_round();
    // A slot that never confirmed a block in this incarnation was never
    // served live (exchange() waits on a first block), so it leaves as of
    // now with nothing to revise. Otherwise the ring bounds the revision:
    // the ledger cannot re-serve a round it no longer holds.
    unsigned D = g_rb_pred[k].have ? g_rb_pred[k].round + 1 : end;
    if ((int)(D - end) > 0) D = end;
    if ((int)(D - g_pipe_low) < 0) D = g_pipe_low;
    if ((int)(D - (end - (kPipeDepth - 2))) < 0) D = end - (kPipeDepth - 2);
    if (why[0] == 'g') g_rb_dropped |= (1u << k);
    unsigned revised = 0;
    for (unsigned q = D; (int)(q - end) < 0; ++q) {
        PipeRound *s = pipe_find(q);
        if (!s) continue;
        if (s->live & (1u << k)) {
            s->live &= ~(1u << k);
            s->sent = false;
            s->sent_mask = ~0u;          // force a resend with the new mask
            ++revised;
        }
    }
    if ((int)(D - g_pipe_low) < 0) g_pipe_low = D;
    g_rb_pred[k].have = false;
    std::fprintf(stderr, "[comms:loopback] ROLLBACK: slot %d retired (%s) as of "
                 "round %u; %u round(s) revised, live mask now 0x%x\n",
                 k, why, D, revised, g_live);
    if (why[0] == 'g') {
        // The grace rule dropped it: tell it so, with the same Bye a parent
        // that quits sends, so it leaves the way a parent's Bye makes a child
        // leave (idle, then the ROM's own wait bound runs out and it goes
        // solo with data_020a0f04 = 0, its own records for a lost session).
        Packet r;
        fill_header(r, kTypeBye);
        send_to_slot(r, k);
        ++g_rb_drops;
    }
    if (popcount_live(g_live) <= 1) g_state = kCommsConnecting;
    pipe_try_broadcast();
}

void on_parent_packet(const Packet &p, const sockaddr_in &from, int k) {
    (void)from;
    if (g_rollback && (g_rb_dropped & (1u << k)) && p.type != kTypeBye) {
        // A slot the grace rule retired is gone the way a DS that stopped
        // sending is gone: its late keepalives and blocks must not seat it
        // again mid-match, which would be a fresh join the ROM never saw.
        static unsigned told = 0;
        if (!(told & (1u << k))) {
            told |= (1u << k);
            std::fprintf(stderr, "[comms:loopback] ROLLBACK: slot %d was dropped; "
                         "its later datagrams are ignored this session\n", k);
        }
        return;
    }
    switch (p.type) {
    case kTypeJoin: {
        const bool fresh = (g_live & (1u << k)) == 0;
        // A DEPTH AN OLD BUILD CANNOT EXPRESS IS NOT SOMETHING TO SEAT A NEW
        // PEER INTO, once the session is past the point where it could be
        // taken back. Before frame 0 an unreported peer makes the parent
        // WITHDRAW the sized depth (see recompute_adaptive_delay) and nobody
        // is harmed. After the freeze there is no withdrawing it: peers have
        // consumed rounds at that depth and moving it would replay or skip
        // them. So a joiner arriving into a frozen session deeper than every
        // shipped build's clamp is REFUSED, out loud, because the alternative
        // is seating a console that may silently run a different number.
        //
        // It costs a late joiner that IS a new build its seat, and that is
        // accepted rather than hidden: the parent cannot tell the two apart at
        // JOIN time (the report is a round trip away) and only a path bad
        // enough to need more than %d frames can reach this at all.
        if (fresh && g_delay_frozen && g_input_delay > kLegacyInputDelayMax) {
            // NOT SEATED, AND NOT SENT A BYE EITHER. A Bye would be the tidy
            // answer on a unicast wire and is the wrong one here: over the
            // relay send_to_slot() fans a parent datagram out to EVERY child,
            // and a child reads Bye as "the parent left" and goes idle. One
            // refused joiner would end the match for the six people already
            // playing. Saying nothing costs the joiner its own ROM bound,
            // which is the same ~20 s it would spend on a parent that never
            // answered -- the failure it already knows how to have.
            const unsigned t = now_ms();
            if (g_refuse_join_last_log_ms == 0 ||
                (unsigned)(t - g_refuse_join_last_log_ms) >= kDelayGateLogMs) {
                g_refuse_join_last_log_ms = t ? t : 1;
                std::fprintf(stderr, "[comms:loopback] REFUSED a late join "
                             "from slot %d: this session is frozen at input "
                             "delay %d, past the %d that every shipped build "
                             "can adopt, so a peer that silently kept its own "
                             "number would simulate a different match. The "
                             "depth cannot be lowered now -- rounds have been "
                             "consumed at it.\n",
                             k, g_input_delay, kLegacyInputDelayMax);
            }
            ++g_dropped;
            break;
        }
        g_live |= (1u << k);
        // A REPORT RATHER THAN A KNOCK. Bit 30 of a JOIN's `have` means the
        // child is telling the parent what its path measured and what number
        // it is currently running. Everything below this block treats it as
        // an ordinary re-knock, which is exactly right: a report gets the same
        // unicast ACCEPT a re-knock gets, and that ACCEPT is what acks it.
        bool report_now = false;
        if (p.have & kJoinReportBit) {
            const int child_rtt   = (int)(p.have & 0xFFFFu);
            const int child_delay = (int)((p.have >> 16) & 0xFFu);
            // THE PARENT'S OWN INTERVAL IS A CROSS CHECK, NOT A SAMPLE, and
            // that distinction was learned from a run rather than argued. It
            // looks like a free second measurement of the same path: the report
            // is sent the moment the ACCEPT lands, so accept-out to report-in
            // ought to be one round trip on this end's own clock. It is not,
            // because the parent stamps EVERY accept it sends and the accept
            // being answered is not necessarily the last one stamped. The first
            // accept goes out while the child is still booting its world, so
            // the interval measured is the child's BOOT TIME -- 2500 ms in this
            // lane's A/B rig -- and a repeat report answers an accept sent
            // milliseconds earlier and measures 0. Taking the worse of the two
            // sized that session at the cap off a number that was never a round
            // trip, published it, and published a corrected one a moment later:
            // two adoptions where one was owed.
            //
            // The child's own handshake sample IS a round trip: it timed its
            // JOIN against the ACCEPT that answered it, on one clock, over this
            // path. That is the number. The interval is printed beside it
            // because a wild disagreement is worth seeing in a log, and it
            // decides nothing.
            int measured = -1;
            if (g_accept_sent_ms[k] != 0) {
                measured = (int)(now_ms() - g_accept_sent_ms[k]);
                if (measured < 0) measured = -1;
            }
            int rtt = child_rtt;
            if (rtt < 0) rtt = 0;
            if (rtt != g_child_rtt_ms[k]) {
                std::fprintf(stderr, "[comms:loopback] slot %d reports "
                             "handshake rtt %d ms (this end saw %d ms since its "
                             "last accept to that slot, a cross check and not "
                             "the sample) and runs input delay %d\n",
                             k, child_rtt, measured, child_delay);
            }
            g_child_rtt_ms[k]    = rtt;
            // ONLY AN ANSWER TO WHAT THIS SLOT WAS LAST TOLD COUNTS AS AN ACK.
            // The child reports once at accept (carrying the old N) and again
            // after adopting (carrying the new one), and UDP is free to
            // deliver those out of order. The stale one landing second used to
            // overwrite the ack with the old number and re-close the gate --
            // while the unicast ACCEPT that answered it still set bit 16, so
            // the child marked itself acknowledged and stopped retrying. Round
            // 0 then sat held until the ROM's own 20 s bound killed the
            // session, with nothing in the log admitting why. The round trip
            // in a stale report is still perfectly good and is kept; only the
            // ack is conditional.
            if (child_delay == g_child_told_delay[k])
                g_child_ack_delay[k] = child_delay;
            else
                std::fprintf(stderr, "[comms:loopback] ignoring slot %d's "
                             "report of delay %d as an acknowledgement: that "
                             "slot was last told %d, so this is a stale report "
                             "that overtook a newer one. Its round trip is "
                             "kept.\n", k, child_delay, g_child_told_delay[k]);
            report_now = true;
        }
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
        set_live(a, g_live);
        // THE PARENT ASSIGNS THE SLOT AND THE RELAY DOES NOT. A child over the
        // internet cannot claim a slot by binding a port the way a loopback
        // child does, so it PROPOSES one in its JOIN and this is the answer.
        // Carried in `have`, which is meaningless on an ACCEPT and has been
        // zero on the wire since MP2 -- so an older build simply keeps the slot
        // it proposed, which is what it did before this field existed.
        //
        // AND THE PARENT'S INPUT DELAY, IN BITS 8..15, BECAUSE THE TWO ENDS
        // MUST RUN THE SAME ONE OR THE MATCH IS NOT THE SAME MATCH.
        //
        // THE MECHANISM, because "both ends should run the same number" was
        // once written here as advice and that was wrong -- it is an
        // INVARIANT. exchange() hands simulation frame k the records from
        // round k-N, and it holds round 0 for the first N+1 frames while the
        // pipeline fills. So the sequence of rounds a console consumes,
        // indexed by its own frame counter, is:
        //
        //   N=0   0, 1, 2, 3, 4, 5, 6, ...
        //   N=5   0, 0, 0, 0, 0, 0, 1, 2, 3, ...
        //
        // Both consoles advance their frame counter by one per completed
        // exchange, so frame k on this console is frame k on that one. Feed
        // frame k different rounds on the two machines and the two are
        // simulating different inputs at the same instant. There is no wall
        // clock offset that makes them agree again, which is what the old
        // "mismatched delays are not a desync, one simply lags the other"
        // reading got wrong: what lags is not the frame index, it is only the
        // wall time, and the ROM indexes by frame.
        //
        // THE ROM AGREES AND SAYS SO. src/func_0203ea5c.c:418 walks the four
        // player records and compares each live one's frame counter against
        // the local player's (`var_r1->unk0 != data_020a1154[..].unk0`),
        // setting error bit 2 in data_020a0f1c when they differ. That compare
        // is the DS's own statement of this invariant, and a mismatched delay
        // is precisely what trips it.
        //
        // SO THE PARENT DECIDES AND THE CHILD ADOPTS. Not a negotiation: one
        // authority, carried in the reply the child is already waiting for,
        // applied before the child has consumed a single round. A child that
        // was told a different number by its own environment loses, loudly.
        // SIZE BEFORE THE ACCEPT GOES OUT, and this ordering was wrong the
        // other way round. The accept used to be sent first and the session
        // sized afterwards, so a joiner arriving while a raised depth was in
        // force was handed THAT number -- and the withdrawal that follows one
        // instruction later never reached it, because a pre-0.3.3 child adopts
        // only on its FIRST accept. It dropped the raised value (past its
        // clamp), kept whatever it already had, and the gate then waved the
        // session through: after the withdrawal the depth equals
        // g_delay_presize, which is the "nothing to confirm" case.
        //
        // That was survivable only while a legacy peer's own value happened to
        // equal the parent's mode default, and SM64DS_COMMS_INPUT_DELAY on the
        // child breaks exactly that assumption. Sizing first costs nothing:
        // the joiner is already in g_live above, which is the only thing the
        // old ordering comment claimed to be waiting for.
        if (report_now || fresh) recompute_adaptive_delay(report_now ? "report"
                                                                    : "join");
        if (fresh) {
            std::fprintf(stderr, "[comms:loopback] slot %d joined at round %u; "
                         "live mask 0x%x, players %d\n",
                         k, g_round, g_live, popcount_live(g_live));
            // THE ROSTER GREW, SO EVERYBODY HEARS ABOUT IT. Only on a FRESH
            // join: a re-knock from a slot already live changes nothing and
            // must not put a burst on the wire. See kRosterAnnounceMs.
            const unsigned now = now_ms();
            g_roster_until   = now + kRosterAnnounceMs;
            // 0 is this field's "no window open" sentinel, and the clock wraps
            // once every 49 days, so the one deadline that would land on it is
            // nudged by a millisecond rather than silently skipping a window.
            if (!g_roster_until) g_roster_until = 1;
            g_last_roster_ms = now;
            announce_roster();
        }
        // AND THE ACCEPT CARRIES WHATEVER THE SIZING SETTLED ON, which is now
        // the last word rather than a value one statement out of date.
        a.have = 0x80000000u | (unsigned)k |
                 ((unsigned)(g_input_delay & 0xFF) << 8) |
                 (g_rollback ? kAcceptRollbackBit : 0u);   // port/rollback: the mode too
        // BIT 16: "your report was recorded". Only the unicast accept can say
        // this, because only the unicast accept has one recipient -- the
        // roster broadcast reaches every child and an ack in it would tell
        // six peers something true of one.
        if (g_child_rtt_ms[k] >= 0) a.have |= kAcceptRttAckBit;
        g_child_told_delay[k] = g_input_delay;
        g_accept_sent_ms[k]   = now_ms();
        send_to_slot(a, k);
        break;
    }
    case kTypeBlocks:
        if (pipelined()) {
            // PIPELINED. Rounds are in flight rather than one at a time, so a
            // child's block is filed by ITS OWN round number and the "is this
            // the round we are on" test does not apply -- being behind the
            // parent's clock is the normal state here, not a stale packet.
            if (p.have & (1u << k)) {
                // ROLLBACK: a child may run ahead of the parent's clock by
                // the window, so its block can name a round this end has
                // not opened yet. Filing it into a fresh ring entry is
                // right; the parent's own exchange() finds it there. But a
                // round older than the ring can hold must not evict a live
                // one: pipe_open would reset a newer round's entry.
                PipeRound *s = pipe_find(p.round);
                if (!s && (!g_rollback ||
                           ((int)(p.round - g_pipe_low) >= 0 &&
                            (int)(p.round - (rb_live_round() + kPipeDepth / 2)) < 0)))
                    s = &pipe_open(p.round);
                if (s) {
                    std::memcpy(s->blocks[k], p.blocks[k], kCommsBlockBytes);
                    s->mask |= (1u << k);
                    rb_note_block(k, p.round, p.blocks[k]);
                }
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
        if (g_rollback && (g_live & (1u << k))) rb_retire_slot(k, "Bye");
        g_live &= ~(1u << k);
        g_stage_mask &= ~(1u << k);
        if (popcount_live(g_live) <= 1) g_state = kCommsConnecting;
        std::fprintf(stderr, "[comms:loopback] slot %d left; live mask 0x%x\n",
                     k, g_live);
        break;
    default:
        ++g_dropped;
        break;
    }
}

// THE CHILD'S RTT REPORT. A JOIN with bit 30 set in `have`, carrying the round
// trip this end measured across its own handshake and the input delay it is
// currently running. It is a JOIN and not a new type on purpose: an older
// parent answers any JOIN from a live slot with a unicast ACCEPT and changes
// nothing else, so this costs a build that predates it exactly one datagram.
void child_send_report() {
    if (g_role != kRoleChild) return;
    if (g_legacy_peer_sim) return;      // an old build has no report to send
    int r = g_handshake_rtt_ms;
    if (r < 0)      r = 0;
    if (r > 0xFFFF) r = 0xFFFF;
    Packet j;
    fill_header(j, kTypeJoin);
    j.have = kJoinReportBit | (unsigned)r |
             ((unsigned)(g_input_delay & 0xFF) << 16);
    send_to_slot(j, 0);
    g_last_report_ms = now_ms();
    ++g_report_tries;
}

// ADOPT A DELAY THE PARENT PUBLISHED. Split out of the accept arm because the
// parent may now publish a SECOND number -- it sizes the session from every
// child's path, and the last child to join can move it -- so an
// already-connected child has to read the field too. The bound that keeps this
// safe is not "the first accept" any more, it is "before this end has handed
// the game a round", which is the property the first-accept bound was standing
// in for. See rule 2 in the banner over g_adaptive_delay.
void child_adopt_delay(int parent_delay, bool at_accept) {
    // THE CEILING AN OLD BUILD APPLIES, reproduced exactly: a value past it is
    // not clamped DOWN, it is dropped, and the peer keeps whatever it had.
    // That silence is the whole blocker.
    const int cap = g_legacy_peer_sim ? kLegacyInputDelayMax : kInputDelayMax;
    if (parent_delay < 0 || parent_delay > cap) return;
    // AND AN OLD BUILD READS THE FIELD ON ITS FIRST ACCEPT ONLY. That is the
    // third behaviour of a pre-0.3.3 peer, and the one the stand-in was
    // missing: its adopt sat inside the branch that sets kCommsChildConnected
    // ("ON THE FIRST ACCEPT ONLY, which is what this branch already is"), and
    // every later accept and every roster announce was a no-op. Without this
    // line the stand-in adopted a WITHDRAWAL off the roster broadcast, so a
    // late-join proof passed on either ordering of the accept and the sizing
    // -- the ordering commit 99c86bba2 exists to fix. With it, a joiner handed
    // a raised number in its first accept keeps its own forever, which is
    // what a real old build does, and the proof can tell the orderings apart.
    if (g_legacy_peer_sim && !at_accept) return;
    if (parent_delay == g_input_delay) return;

    if (g_frames_produced) {
        // REFUSED, AND LOUD. Moving N under a frame counter that only
        // increments makes `want` (g_round - N) jump, so the console replays or
        // skips rounds -- the divergence this whole field exists to prevent,
        // arriving by a different door. Rate limited: if the parent is going to
        // say this it will say it every roster announce.
        const unsigned t = now_ms();
        if (g_adopt_refuse_last_ms == 0 ||
            (unsigned)(t - g_adopt_refuse_last_ms) >= kDelayGateLogMs) {
            g_adopt_refuse_last_ms = t ? t : 1;
            std::fprintf(stderr, "[comms:loopback] the parent published input "
                         "delay %d and this end is running %d, but this end "
                         "has already handed the game %u round(s). REFUSED: "
                         "changing the depth now would replay or skip rounds, "
                         "which is the desync the shared depth exists to "
                         "prevent. The parent freezes the number before round "
                         "0, so seeing this means a peer joined late.\n",
                         parent_delay, g_input_delay, g_round);
        }
        return;
    }

    std::fprintf(stderr, "[comms:loopback] the parent runs input delay %d and "
                 "this end had %d; ADOPTING %d%s. The parent is authoritative "
                 "because the two ends must run the same depth -- frame k "
                 "reads round k-N on both consoles or they are not simulating "
                 "the same match%s\n",
                 parent_delay, g_input_delay, parent_delay,
                 at_accept ? "" : " (the parent re-sized the session)",
                 delay_from_env()
                     ? ". SM64DS_COMMS_INPUT_DELAY was set on this end and "
                       "LOST; set it on the parent, which is the end whose "
                       "knob decides."
                     : ".");
    g_input_delay = parent_delay;
    // SAY IT BACK. The parent will not serve round 0 until every child it
    // retold has acked, so this is not politeness, it is the thing that lets
    // the session start.
    g_report_acked  = false;
    g_report_tries  = 0;
    child_send_report();
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
        if ((p.live_wide & (1u << g_slot)) == 0) { ++g_dropped; break; }
        g_live = p.live_wide;
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
            // ADOPT THE PARENT'S INPUT DELAY. Every mode, unlike the slot
            // assignment above: a relayed ACCEPT is a broadcast and adopting
            // a SLOT from it would let one child's accept move another, but
            // the delay is one number the parent applies to the whole
            // session, so every child adopting it from any of its accepts is
            // exactly right.
            //
            // ON THE FIRST ACCEPT ONLY, which is what this branch already is,
            // and that bound is load-bearing. Changing N mid-session moves
            // `want` (g_round - N) backwards or forwards under a frame
            // counter that only ever increments, so the console would replay
            // or skip rounds -- which is the same divergence this field
            // exists to prevent, arriving by a different door. Before any
            // round has been consumed there is nothing to replay: exchange()
            // returns 0 until this line sets kCommsChildConnected.
            child_adopt_delay((int)((p.have >> 8) & 0xFF), true);
            // ROLLBACK IS THE PARENT'S CALL TOO, for the same reason the
            // delay is: one end predicting while the other waits is two
            // different timelines. Bit 17 of the accept carries it (bit 16 is the
            // adaptive delay's report ack).
            // Read off the accept that seats this end (bit 31, the slot
            // assignment) and off the roster announce, which carries it too;
            // a datagram from an older parent has neither bit and reads as
            // lockstep, which is what that parent runs.
            {
                const bool parent_rb = (p.have & kAcceptRollbackBit) != 0;
                if (parent_rb != g_rollback) {
                    std::fprintf(stderr, "[comms:loopback] the parent runs "
                                 "NetMode %s and this end had %s; ADOPTING "
                                 "the parent's\n",
                                 parent_rb ? "rollback" : "lockstep",
                                 g_rollback ? "rollback" : "lockstep");
                    g_rollback = parent_rb;
                    if (g_rollback) g_input_delay = 0;
                }
            }
            g_round = p.round;          // adopt the parent's clock
            if (g_rollback) {           // port/rollback: the ledger starts here
                g_rb_scan_low = p.round;
                g_pipe_low = p.round;
            }
            g_stage_mask = 0;
            g_round_done = false;
            g_state = kCommsChildConnected;
            std::fprintf(stderr, "[comms:loopback] accepted as slot %d at "
                         "round %u; live mask 0x%x, players %d, input delay %d"
                         " (handshake rtt %d ms)\n",
                         g_slot, g_round, g_live, popcount_live(g_live),
                         g_input_delay, g_handshake_rtt_ms);
            // TELL THE PARENT WHAT THIS PATH MEASURED. It sizes the session's
            // delay off the worst round trip among its children and cannot
            // measure a child's path from its own side until the child answers
            // one of its datagrams, so this is the sample.
            g_report_acked = false;
            g_report_tries = 0;
            child_send_report();
            break;
        }
        // ALREADY CONNECTED. Two things can still be in this accept: the ack
        // for this end's report, and a delay the parent re-sized after some
        // later child joined.
        // AND THE ACK IS ONLY OURS IF THE ACCEPT IS. Over the relay a
        // parent datagram is fanned out to every child, so the accept that
        // answers slot 3's report is also delivered to slots 1, 2 and 4. Bit
        // 31 marks an accept aimed at one joiner and bits 0..7 name that
        // joiner -- the slot-assignment field, which has carried the recipient
        // since the parent started assigning slots -- so this is a recipient
        // test with no new wire bits. Taking a stranger's ack would leave this
        // end believing the parent has its round trip when the parent has
        // nothing, and the parent would then hold round 0 for a confirmation
        // that had already been thrown away.
        if ((p.have & kAcceptRttAckBit) && (p.have & 0x80000000u) &&
            (int)(p.have & 0xFFu) == g_slot)
            g_report_acked = true;
        child_adopt_delay((int)((p.have >> 8) & 0xFF), false);
        break;
    }
    case kTypeBlocks:
        if (g_state != kCommsChildConnected) break;
        if (g_rollback) {
            // ROLLBACK. The aggregate names ITS round's live mask (the
            // parent's per-round declaration), so it is filed per round and
            // g_live follows the NEWEST round heard, never an older resend.
            if ((int)(p.round - g_pipe_low) < 0 &&
                (int)(p.round - (rb_live_round() - kPipeDepth + 2)) < 0)
                break;                   // older than the ring can hold
            if ((int)(p.round - (rb_live_round() + kPipeDepth / 2)) >= 0)
                break;                   // so far ahead (a parent that ran on
                                         // while this end slept) that filing
                                         // it would evict rounds still in play
            if ((int)(p.round - g_rb_live_round) >= 0) {
                g_live = p.live_wide;
                g_rb_live_round = p.round;
            }
            PipeRound &s = pipe_open(p.round);
            s.live = p.live_wide;
            s.live_known = true;
            for (int i = 0; i < kCommsMaxPlayers; ++i) {
                if (p.have & (1u << i)) {
                    std::memcpy(s.blocks[i], p.blocks[i], kCommsBlockBytes);
                    s.mask |= (1u << i);
                    if (i != g_slot) rb_note_block(i, p.round, p.blocks[i]);
                }
            }
            while (true) {
                PipeRound *low = pipe_find(g_pipe_low);
                if (!low || !low->live_known ||
                    (low->mask & low->live) != low->live) break;
                ++g_pipe_low;
            }
            break;
        }
        if (g_input_delay > 0) {
            // PIPELINED. File the aggregate under ITS OWN round. A child is
            // normally several rounds behind the parent's clock here -- that
            // is the mechanism, not lateness -- so the stop-and-wait test
            // "is this the round I am on" would throw away every packet.
            g_live = p.live_wide;
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
        g_live = p.live_wide;
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
    else if (n >= 4 && std::memcmp(raw, &kAuxVoiceLE, 4) == 0)
        aux_kind = kAuxKindVoice;
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
    // THE LENGTH IS STILL THE FRAME CHECK, it just names TWO frames now. A
    // datagram is ours iff it is exactly the narrow length or exactly the wide
    // one, and which one it is decides the wire it came off before a single
    // field past the magic is believed.
    if (!len_is_ours(n)) { ++g_dropped; return; }
    const bool rx_wide = len_is_wide(n);
    std::memset(&p, 0, sizeof p);
    std::memcpy(&p, raw, (size_t)n);
    if (std::memcmp(p.magic, kMagic, 4) != 0) { ++g_dropped; return; }
    // NORMALISE THE LIVE MASK ONCE, HERE. A narrow datagram never carried
    // live_wide, so fill it in from the byte it did carry. Past this line no
    // reader in the file has to know which wire the packet came off, which is
    // the whole reason there is exactly one of these lines.
    if (!rx_wide) p.live_wide = p.live;
    // A WRONG VERSION IS SAID OUT LOUD, ONCE. It used to be counted and
    // dropped like a corrupt datagram, which is the right handling and the
    // wrong report: the two cases a version mismatch actually arises from are
    // a stale copy of the game on one desk and a half-finished update, and
    // both look from the outside like "we paired and then nothing happened".
    // One line naming both numbers turns a silent twenty-second wait into a
    // diagnosis. Once, because a peer that keeps knocking would otherwise
    // write this every 200 ms for the whole of the ROM's wait bound.
    //
    // TWO CHECKS NOW, AND THEY ARE DIFFERENT QUESTIONS. Run vs16:
    //   (a) does the version byte agree with the LENGTH the datagram arrived
    //       at? A version-3 byte in a 0x90 datagram is not a wide packet, it
    //       is the deliberate refusal a wide parent sends back to a narrow
    //       joiner (see the kTypeJoin arm) -- and on THIS side of that
    //       exchange it means we are the narrow one being turned away.
    //   (b) is it the wire THIS session speaks? A four-player build and a
    //       sixteen-player session must not form a session together for the
    //       same reason version 1 and version 2 must not: the ends would
    //       simulate different matches and every log would read healthy.
    if (p.version != len_version(n) || p.version != wire_version()) {
        static bool said = false;
        if (!said) {
            said = true;
            std::fprintf(stderr, "[comms:loopback] REFUSING a peer speaking "
                         "wire version %u; this session speaks %u. The two are "
                         "not compatible and MUST NOT play together. Version 1 "
                         "has no input-delay field, so the ends would run "
                         "different delays; version 2 carries four player "
                         "slots and version 3 carries sixteen, so the ends "
                         "would not even agree who is in the match. Either way "
                         "it is a DESYNC that reads healthy in both logs. Make "
                         "both copies of the game the same build, and make the "
                         "room the same size. No session will form until you "
                         "do.\n",
                         (unsigned)p.version, (unsigned)wire_version());
        }
        // AND SAY IT TO THE PEER, NOT ONLY TO OUR OWN LOG. This is the whole
        // "design the refusal cleanly" half of the wide wire, and it costs
        // four lines because the shipped narrow build ALREADY knows how to
        // read it.
        //
        // A narrow build cannot receive a wide datagram at all -- its recv
        // buffer is 256 bytes and a 532-byte datagram fails at the socket, so
        // it would sit in silence for the whole of the ROM's twenty-second
        // wait and report nothing. So a wide parent answers a narrow JOIN with
        // a NARROW-LENGTH packet carrying OUR version byte. The narrow build's
        // own version check -- the one that has been in this file since the
        // version-1 bump, unchanged and already loud -- then fires on its side
        // and prints the whole diagnosis in the log of the person who is
        // actually holding the stale build.
        //
        // It is deliberately NOT a new packet type: a new type is a thing an
        // older build has to already know about, and this one does not have to
        // know about anything.
        if (g_role == kRoleParent && p.type == kTypeJoin &&
            n == kPacketNarrowBytes && p.version < wire_version()) {
            Packet r;
            std::memset(&r, 0, sizeof r);
            std::memcpy(r.magic, kMagic, 4);
            r.version = wire_version();
            r.type    = kTypeBye;
            r.slot    = 0;
            send_raw(&r, kPacketNarrowBytes, from);
            ++g_refused_narrow;
            std::fprintf(stderr, "[comms:loopback] refused a four-player peer "
                         "on port %u: it spoke wire version %u into a "
                         "%d-player session\n",
                         (unsigned)ntoh16(from.sin_port), (unsigned)p.version,
                         g_want_players);
        }
        ++g_dropped;
        return;
    }
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
        // BIG ENOUGH FOR THE WIDEST KIND ON THIS SOCKET, which since run
        // vs16 is the wide lockstep datagram (0x214) rather than an aux
        // message (256). recvfrom into a buffer smaller than the datagram is
        // not a truncation on Windows, it is WSAEMSGSIZE and a LOST packet,
        // so this number is a correctness bound and not a convenience.
        unsigned char raw[kRawMaxBytes];
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
        d.len  = n < kRawMaxBytes ? n : kRawMaxBytes;
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
    // AN UNACKED REPORT IS A PEER THE PARENT IS SIZING WITHOUT. The report is
    // one datagram on a lossy path and losing it silently costs the session
    // the very measurement the delay is derived from, so it repeats at the
    // republish interval until the parent's unicast ACCEPT says it landed --
    // and it STOPS after kMaxReportTries, because a parent that never acks is
    // an older build and this must not become a permanent trickle.
    // NOT AT THE REPUBLISH INTERVAL FLAT. Loopback republishes every 4 ms,
    // which is correct for a round the receiver is already stuck on and wrong
    // here: the answer to a report cannot come back faster than the path, so
    // twelve retries at 4 ms would all leave before the first ack could
    // possibly arrive and every one of them past the first would be a
    // duplicate by construction. 50 ms is the floor.
    const unsigned report_every = (unsigned)(g_resend_ms > (int)kReportRetryFloorMs
                                             ? g_resend_ms : (int)kReportRetryFloorMs);
    if (g_role == kRoleChild && g_state == kCommsChildConnected &&
        !g_report_acked && g_report_tries < kMaxReportTries &&
        (unsigned)(t - g_last_report_ms) >= report_every) {
        child_send_report();
    }

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
            ++g_joins_sent;
            // THE OTHER HALF OF THE VERSION REFUSAL, and it exists because the
            // loud line above can only fire on the end that RECEIVES the wrong
            // version. Pair an OLD parent with a NEW child and the old one
            // drops our JOIN in silence -- it is the generation that cannot
            // say anything, and no change here can teach a binary that already
            // shipped. So the new end infers it instead: many JOINs sent, not
            // one datagram back, while the transport otherwise looks fine
            // (relay paired, socket open). That is either nobody there or a
            // peer that cannot understand us, and both are worth saying once,
            // because from the outside they are the same twenty seconds of
            // nothing happening.
            if (g_joins_sent == kJoinsBeforeHint && g_recvd == 0) {
                std::fprintf(stderr, "[comms:loopback] %d JOINs sent and NOT "
                             "ONE datagram back from the other player. Either "
                             "nothing is listening on that session code, or "
                             "the other copy of the game is a DIFFERENT BUILD "
                             "-- this one speaks wire version %u and refuses "
                             "anything else, which is deliberate: two builds "
                             "that disagree would run different input delays "
                             "and silently simulate different matches. Check "
                             "the code, then check both are on the same "
                             "build.\n",
                             kJoinsBeforeHint, (unsigned)wire_version());
            }
            if (g_net_mode != kNetLoopback) {
                g_join_wait_ms *= 2;
                if (g_join_wait_ms > kJoinBackoffCapMs)
                    g_join_wait_ms = kJoinBackoffCapMs;
            }
        }
    }

    // THE ROSTER ANNOUNCE'S REPEAT, and it sits ABOVE the pipelining branch on
    // purpose: a session forming is a session that has not started exchanging
    // anything yet, so it must run in both the pipelined and the stop-and-wait
    // arms. Bounded by wall clock, paced at the republish interval, and dead
    // the instant the window closes -- a formed session pays nothing.
    if (g_role == kRoleParent && g_roster_until != 0) {
        unsigned every = (unsigned)g_resend_ms;
        if (every < kRosterAnnounceEveryMs) every = kRosterAnnounceEveryMs;
        if ((int)(t - g_roster_until) >= 0) {
            g_roster_until = 0;
        } else if ((unsigned)(t - g_last_roster_ms) >= every) {
            g_last_roster_ms = t;
            announce_roster();
        }
    }

    // PIPELINED. The parent closes and ships any round that has become
    // complete, HERE rather than in its own exchange(): the pump runs this
    // constantly, and making the aggregate wait for the parent's next frame
    // would put 33 ms back into the path pipelining exists to shorten.
    if (pipelined()) {
        if (g_role == kRoleParent) {
            pipe_try_broadcast();
        } else if (g_state == kCommsChildConnected &&
                   (unsigned)(t - g_last_publish_ms) >= (unsigned)g_resend_ms) {
            // Republish the OLDEST round still unanswered. One per tick: a
            // hole is healed at the resend rate, and a child that dumped its
            // whole in-flight window on every tick would turn one lost
            // datagram into a burst exactly when the path is already unhappy.
            g_last_publish_ms = t;
            for (unsigned q = g_pipe_low; (int)(q - rb_live_round()) < 0; ++q) {
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

    // Every per-session adaptive-delay and starve-attribution counter starts
    // at the session, not at the process: a second session in one launch must
    // not inherit the first one's round trips or its blame.
    delay_state_reset();

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
    else                            { first = 1; last = slot_cap() - 1; }

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
    g_joins_sent = 0;
    g_rb_replaying = false;
    g_rb_replay_end = 0;
    g_rb_scan_low = 0;
    g_rb_live_round = 0;
    g_rb_stall_start_ms = 0;
    g_rb_stall_mask = 0;
    g_rb_live_peak = 0;
    g_rb_dropped = 0;
    std::memset(g_rb_pred, 0, sizeof g_rb_pred);

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
    // THE PACING VERDICT, ONCE, ON EVERY SESSION. Run rel0215 lane vslag.
    //
    // `starved` is what tells a pair their input delay is too shallow for
    // their path, and the relay README now tells players to read it -- but the
    // only thing that printed it was comms_loopback_report, which walk_window
    // calls from inside the SM64DS_COMMS_FANOUT block. So the number the
    // documentation sends a player to look for did not exist in an ordinary
    // playlog, and the two knobs that produce it are per-frame work nobody
    // should run while playing. One line at close costs a session nothing and
    // lands in playlog/play_*.log where a bug report will carry it.
    // WHOSE PATH WAS SHORT. `starved` was always the tuning fact; without the
    // per-slot breakdown a seven-player session's field log said the session
    // was slow and named nobody, which is the difference between a bug report
    // that can be acted on and one that cannot. Omitted entirely when nothing
    // starved, so a healthy session's line is byte-identical to what it was.
    char by[192];
    by[0] = 0;
    if (g_starve_ever) {
        int w = std::snprintf(by, sizeof by, " starvedby=");
        bool any = false;
        for (int i = 0; i < kCommsMaxPlayers && w < (int)sizeof by - 24; ++i) {
            if (!g_starve_by_slot[i]) continue;
            w += std::snprintf(by + w, sizeof by - (size_t)w, "%s%d:%llu",
                               any ? "," : "", i, g_starve_by_slot[i]);
            any = true;
        }
        if (!any) by[0] = 0;
        else std::snprintf(by + w, sizeof by - (size_t)w, " lastround=%u",
                           g_starve_last_round);
    }
    std::fprintf(stderr, "[comms:loopback] closed after %u rounds; indelay=%d "
                 "starved=%llu sent=%llu recvd=%llu resends=%llu%s\n",
                 rb_live_round(), g_input_delay, g_pipe_starved, g_sent, g_recvd,
                 g_resends, by);
    if (g_rollback)
        std::fprintf(stderr, "[comms:loopback] rollback: live=0x%x peak=0x%x predicted=%llu ok=%llu "
                     "mispredicted=%llu rewinds=%llu replayed=%llu "
                     "stalled=%llu stallevents=%llu drops=%llu "
                     "xcalls=%llu xzero=%llu rxcalls=%llu rxzero=%llu\n",
                     g_live, g_rb_live_peak, g_rb_predicted, g_rb_confirmed_ok, g_rb_mispredicted,
                     g_rb_rewinds, g_rb_replayed, g_rb_stalled,
                     g_rb_stall_events, g_rb_drops,
                     g_rb_xcalls, g_rb_xzero, g_rb_rxcalls, g_rb_rxzero);
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
    delay_state_reset();
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
    const int n = popcount_live(g_live);
    return n < 1 ? 1 : n;
}

// ===========================================================================
// ROLLBACK: the exchange. Round R = g_round is served THIS call, at input
// delay zero. This console's block is real; a live slot whose block for R is
// filed is real; any other live slot is a guess from its last confirmed block
// -- unless the guess would reach past kRbWindow, in which case this stalls
// (returns 0, the ROM sleeps a turn and asks again) for kRbGraceMs and then
// the parent drops the slot. In REPLAY nothing is published and nothing is
// staged: the ring already holds this console's block for every round being
// re-run, and the round is served from confirmed blocks where they exist and
// a fresh guess (or, under comms_rb_det_reuse, the exact previous guess)
// where they do not.
// ===========================================================================
bool rb_same_block(const unsigned char *a, const unsigned char *b) {
    // +2..+5 is the frame counter, which a guess overwrites with this
    // console's own and which the ROM's clock check polices itself.
    return std::memcmp(a, b, 2) == 0 &&
           std::memcmp(a + 6, b + 6, kCommsBlockBytes - 6) == 0;
}

bool rb_all_confirmed(unsigned end) {
    for (unsigned q = g_rb_scan_low; (int)(q - end) < 0; ++q) {
        PipeRound *s = pipe_find(q);
        if (!s || !s->consumed) continue;
        if (!s->live_known) return false;
        if ((s->mask & s->live) != s->live) return false;
    }
    return true;
}

void rb_leave(const char *why) {
    std::fprintf(stderr, "[comms:loopback] ROLLBACK: leaving the session at "
                 "round %u (%s); the ROM's own wait bound now runs out and "
                 "it goes solo the way its own drop does\n", g_round, why);
    Packet r;
    fill_header(r, kTypeBye);
    if (g_role == kRoleChild) send_to_slot(r, 0); else send_to_children(r);
    g_state = kCommsIdle;
    g_live = 0;
    g_rb_replaying = false;
    g_rb_stall_start_ms = 0;
}

int rb_exchange_body(const void *my_block);
int rb_exchange(const void *my_block) {
    const bool rp = g_rb_replaying;
    const int r = rb_exchange_body(my_block);
    g_rb_live_peak |= g_live;
    if (rp) { ++g_rb_rxcalls; if (!r) ++g_rb_rxzero; }
    else    { ++g_rb_xcalls;  if (!r) ++g_rb_xzero; }
    return r;
}
int rb_exchange_body(const void *my_block) {
    const unsigned R = g_round;
    PipeRound *s = pipe_find(R);
    if (g_rb_replaying) {
        if (!s || (s->mask & (1u << g_slot)) == 0) {
            ++g_rb_unrecoverable;
            rb_leave("replay found its own block missing from the ring");
            return 0;
        }
    } else {
        if (!s) s = &pipe_open(R);
        if (g_role == kRoleParent && !s->live_set) {
            s->live = g_live;
            s->live_known = true;
            s->live_set = true;
        }
        if ((s->mask & (1u << g_slot)) == 0) {
            std::memcpy(s->blocks[g_slot], my_block, kCommsBlockBytes);
            s->mask |= (1u << g_slot);
            if (g_role == kRoleChild) {
                Packet b;
                fill_header(b, kTypeBlocks);
                b.round = R;
                b.have  = (1u << g_slot);
                std::memcpy(b.blocks[g_slot], my_block, kCommsBlockBytes);
                send_to_slot(b, 0);
                g_last_publish_ms = now_ms();
            } else {
                pipe_try_broadcast();
            }
        }
    }
    service();                           // the answer may already be here
    if (g_state != kCommsParentConnected && g_state != kCommsChildConnected)
        return 0;
    s = pipe_find(R);
    if (!s) return 0;

    unsigned live = s->live_known ? s->live : g_live;
    if (g_rb_replaying && (!s->live_known || g_rb_det_reuse)) live = s->served_live;

    const unsigned now = now_ms();
    unsigned waiting = 0, stalled = 0;
    if (!g_rb_replaying && g_rb_flush_wait) {
        if (rb_all_confirmed(R)) g_rb_flush_wait = false;
        else waiting |= 0x80000000u;
    }
    for (int k = 0; k < kCommsMaxPlayers; ++k) {
        const unsigned bit = 1u << k;
        if (!(live & bit) || k == g_slot) continue;
        if (s->mask & bit) continue;                      // real
        const RbSlotPred &p = g_rb_pred[k];
        if (!p.have) { waiting |= bit; continue; }        // the first round
        if (!g_rb_replaying && (int)(R - p.round) > (int)kRbWindow) {
            // A CHILD NEVER WINDOW-DROPS THE PARENT. The parent is the clock,
            // and while it grace-drops a stalled peer it stops opening new
            // rounds for up to kRbGraceMs -- so a child running ahead of it on
            // prediction will overshoot the parent's own last block by the
            // window. That is the parent being briefly behind, not gone: the
            // child WAITS for it (starves) rather than deciding a drop it has
            // no authority to decide. A parent that is truly gone stops even
            // resending, and the ROM's own ~20 s wait bound ends the session
            // the way it always has. Without this the child leaves and
            // re-knocks in a loop every time the parent stalls a sleeper.
            if (g_role == kRoleChild && k == 0)
                waiting |= bit;
            else
                stalled |= bit;
        }
    }
    if (waiting || stalled) {
        if (stalled) {
            if (!g_rb_stall_start_ms) {
                g_rb_stall_start_ms = now ? now : 1;
                g_rb_stall_mask = stalled;
                ++g_rb_stall_events;
                std::fprintf(stderr, "[comms:loopback] ROLLBACK: round %u is "
                             "%u past the last confirmed block of slot(s) 0x%x; "
                             "stalling (grace %u ms)\n", R, kRbWindow + 1,
                             stalled, kRbGraceMs);
            }
            ++g_rb_stalled;
            const unsigned held = now - g_rb_stall_start_ms;
            if (g_role == kRoleParent && held >= kRbGraceMs) {
                for (int k = 0; k < kCommsMaxPlayers; ++k)
                    if (stalled & (1u << k)) rb_retire_slot(k, "grace");
                g_rb_stall_start_ms = 0;
                g_rb_stall_mask = 0;
            }
            // A child does not leave on a stall: it cannot drop a peer (only
            // the parent revises the roster), and it never stalls on the
            // parent (see the loop above), so a stall on a child is always a
            // wait for the parent's verdict on some other slot. It holds
            // until that verdict arrives or the ROM's own wait bound ends the
            // session.
        } else {
            ++g_pipe_starved;
        }
        return 0;
    }
    if (g_rb_stall_start_ms) {
        std::fprintf(stderr, "[comms:loopback] ROLLBACK: stall over after "
                     "%u ms at round %u\n", now - g_rb_stall_start_ms, R);
        g_rb_stall_start_ms = 0;
        g_rb_stall_mask = 0;
    }

    // Serve it.
    const unsigned char *mine = s->blocks[g_slot];
    unsigned pred_mask = 0;
    for (int k = 0; k < kCommsMaxPlayers; ++k) {
        const unsigned bit = 1u << k;
        if (!(live & bit)) continue;
        // The determinism check (comms_rb_det_reuse) re-serves the guess the
        // straight run was handed even when the real block has landed since
        // and disagrees: the question it asks is whether restore + re-tick
        // reproduces the SAME inputs byte for byte, and the contradiction is
        // the ordinary scan's business the moment the check is over.
        const bool reuse = g_rb_replaying && g_rb_det_reuse && (s->pred_mask & bit);
        if (!reuse && (s->mask & bit)) {
            std::memcpy(g_latched[k], s->blocks[k], kCommsBlockBytes);
            continue;
        }
        if (!reuse)
            rb_predict(k, R, mine, s->pred[k]);
        std::memcpy(g_latched[k], s->pred[k], kCommsBlockBytes);
        pred_mask |= bit;
        ++g_rb_predicted;
    }
    g_latched_mask = live;
    s->pred_mask   = pred_mask;
    s->served_live = live;
    s->consumed    = true;
    std::memcpy(g_rb_my_served, mine, kCommsBlockBytes);
    g_rb_served_round = R;
    ++g_round;
    if (g_rb_replaying) {
        ++g_rb_replayed;
        if (g_round == g_rb_replay_end) g_rb_replaying = false;
    }
    return 1;
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

    // WAIT FOR THE MEASUREMENTS BEFORE FREEZING, BRIEFLY AND ONCE. The order
    // here is otherwise fatal to the whole point: the parent becomes connected
    // the instant a child's JOIN lands, its very next exchange() would freeze
    // the depth, and that child's round-trip report is still one round trip
    // away -- so the number would freeze at the mode default every time and the
    // sizing would never fire on a real path.
    //
    // RETURNING 0 IS NOT A NEW STATE. It is what exchange() already returns on
    // an incomplete round, the ROM's own wait loop is built to spin on it
    // (src/func_0203ea5c.c, 1200 turns), and nothing has committed: no frame
    // has been produced and no aggregate has gone out, so there is no history
    // for a later number to be inconsistent with.
    //
    // THE GRACE IS A CEILING, NOT A DELAY. The moment every live child has
    // reported this falls through, which on any real path is one round trip.
    // If it expires with somebody still silent, that peer is an older build
    // that is never going to answer, and the session keeps the mode default it
    // would have had before this lane existed -- which is the same answer the
    // "every live child, or nothing" rule gives everywhere else, and it is the
    // one that cannot desync a mixed session.
    if (g_role == kRoleParent && g_adaptive_delay && !g_delay_frozen) {
        const unsigned t = now_ms();
        if (g_sizing_wait_since_ms == 0) g_sizing_wait_since_ms = t ? t : 1;
        bool all_reported = true;
        for (int k = 1; k < kCommsMaxPlayers; ++k)
            if ((g_live & (1u << k)) && g_child_rtt_ms[k] < 0)
                all_reported = false;
        if (!all_reported) {
            if ((unsigned)(t - g_sizing_wait_since_ms) < kSizingGraceMs) {
                ++g_sizing_holds;
                return 0;
            }
            // WITHDRAW FIRST, THEN STAND DOWN. The line below printed "the
            // mode default of %d" while passing the RAISED number and left
            // it in force, so the one line meant to say "nothing was sized
            // here" was itself the evidence that something had been -- and
            // the session went on at a depth an unreported peer may not
            // have been able to adopt at all.
            if (g_delay_presize >= 0 && g_input_delay != g_delay_presize) {
                g_input_delay = g_delay_presize;
                announce_roster();
            }
            std::fprintf(stderr, "[comms:loopback] not every peer reported a "
                         "round trip within %ums, so the adaptive sizing "
                         "stands down and this session runs the mode default "
                         "of %d. A peer that never reports is a peer that "
                         "would never adopt a re-sized depth either, and a "
                         "session where one console runs a different depth is "
                         "a desync.\n", kSizingGraceMs, g_input_delay);
            g_adaptive_delay = false;
        }
    }

    // FREEZE, AND BEFORE THE PATH SPLIT RATHER THAN INSIDE ONE ARM. The ROM's
    // wait loop is asking for a round, so frame 0 is imminent and the number
    // stops moving here. It sits above the `g_input_delay > 0` test so the
    // freeze is recorded on every path, pipelined or not: the sizing is armed
    // only on a relay or direct carrier and never on loopback (see the arming
    // rule in comms_loopback_install_from_env), but the late-join refusal and
    // the withdrawal are keyed on this flag and it must be true from the
    // first exchange regardless of which arm the session runs. Every join
    // happens in the lobby, upstream of this call.
    if (g_role == kRoleParent && !g_delay_frozen) {
        g_delay_frozen = true;
        g_frozen_live  = g_live;    // the roster the ack gate waits on from here
        if (g_adaptive_delay)
            std::fprintf(stderr, "[comms:loopback] input delay FROZEN at %d "
                         "for the rest of this session; the ROM asked for its "
                         "first round\n", g_input_delay);
    }
    if (g_rollback) return rb_exchange(my_block);   // port/rollback

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
            //
            // AND IT IS COUNTED PER SLOT. `starved` alone says the session is
            // short of cover; it does not say whose path is short, and in a
            // seven-player match that is the whole question. A round known to
            // the ring is missing whatever `mask` does not hold; a round the
            // ring does not hold at all is missing everyone but this end,
            // which is the honest reading of "nothing for that round arrived".
            ++g_pipe_starved;
            g_starve_ever = true;
            g_starve_last_round = want;
            const unsigned have_mask = s ? (s->mask & g_live) : (1u << g_slot);
            const unsigned missing   = g_live & ~have_mask;
            for (int i = 0; i < kCommsMaxPlayers; ++i)
                if (missing & (1u << i)) ++g_starve_by_slot[i];
            // RATE LIMITED TO ONE LINE A SECOND. On a path this delay does not
            // cover, a starve is a per-frame event -- 1976 of them in 3032
            // rounds in the field logs -- and a line each would be the flood,
            // not the diagnosis. The counters below carry the totals.
            const unsigned tnow = now_ms();
            if (g_starve_last_log_ms == 0 ||
                (unsigned)(tnow - g_starve_last_log_ms) >= kStarveLogMs) {
                g_starve_last_log_ms = tnow ? tnow : 1;
                char who[128];
                int  w = 0;
                who[0] = 0;
                for (int i = 0; i < kCommsMaxPlayers && w < (int)sizeof who - 8; ++i)
                    if (missing & (1u << i))
                        w += std::snprintf(who + w, sizeof who - (size_t)w,
                                           "%s%d", w ? "," : "", i);
                std::fprintf(stderr, "[comms:loopback] starved at round %u "
                             "(indelay=%d): no record yet from slot(s) %s\n",
                             want, g_input_delay, w ? who : "-");
            }
            return 0;
        }
        std::memcpy(g_latched, s->blocks, sizeof g_latched);
        g_latched_mask = s->mask & g_live;
        ++g_round;
        // A ROUND HAS REACHED THE GAME. Rule 2's bound: from here the depth is
        // part of this end's history and cannot move.
        g_frames_produced = true;
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
    // Rule 2 bound on the stop-and-wait path too. A session whose depth was
    // raised from zero by the adaptive sizing spends its first exchanges here,
    // and a bound that only existed on the pipelined arm would leave this end
    // believing it had produced no frames when it had produced several.
    g_frames_produced = true;
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
        if ((idx % kAuxKinds) == kAuxKindVoice) continue;   // lane VOICE: not ours
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

// ---------------------------------------------------------------------------
// THE VOICE READER. Lane VOICE.
//
// The same round-robin over the same slots, restricted to the voice kind, with
// its OWN cursor so a busy voice channel cannot move the sync layer's cursor
// and vice versa. `from` is the sender slot the carrier already classified --
// by source port on loopback, by learned address in direct mode, by the
// packet's own slot field on the relay -- so the audio payload never has to be
// trusted to say who sent it.
//
// Returns the byte count, or 0 when nothing is waiting. Never blocks. It
// drains the socket first, exactly as lb_recv_aux does, because voice has to
// work with the sync layer switched off and that layer's pump is the only
// other thing that would have drained it. service() is idempotent -- a
// nonblocking read loop that stops on WSAEWOULDBLOCK -- so a second call in
// the same frame costs one failed recvfrom.
int g_voice_rr = 0;

int lb_recv_voice(void *buf, int cap, int *from) {
    if (!buf || cap <= 0) return 0;
    service();
    for (int i = 0; i < kCommsMaxPlayers; ++i) {
        const int s = (g_voice_rr + i) % kCommsMaxPlayers;
        AuxSlot &slot = g_aux[s][kAuxKindVoice];
        if (slot.len <= 0) continue;
        const int n = slot.len < cap ? slot.len : cap;
        std::memcpy(buf, slot.buf, (size_t)n);
        slot.len = 0;
        g_voice_rr = (s + 1) % kCommsMaxPlayers;
        if (from) *from = s;
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
// THE SESSION'S WIDTH, exported. Run vs16, hosted-conductor follow-up.
//
// One caller: hal/comms_conductor_wide.cpp's dispatcher, which decides whether
// src/func_0203ea5c.c (the DS-faithful four-record conductor) or the wide host
// copy runs this session. It gates on the SAME g_want_players that picks the
// wire above, so the record bounds and the datagram layout cannot disagree --
// there is no second opinion to get out of step. 0 means no setter ran, which
// is every solo boot, every local-play boot, and every narrow deployment.
// ---------------------------------------------------------------------------
int comms_session_players() { return g_want_players; }

// ---------------------------------------------------------------------------
// THE VOICE READER, exported. Lane VOICE.
//
// One caller: hal/voice_chat.cpp's receive pump. The CommsTransport struct is
// deliberately NOT widened for this. That struct is a frozen contract with a
// declared v2 boundary, and a proximity chat channel is a port feature rather
// than a term of the seam. So voice SENDS through the contract's own send_aux
// (an opaque whole message carrying its own 'SYNV' tag, which is exactly what
// that entry is for) and READS through this one carrier-specific entry,
// because the read side is the only half that needs the sender's identity and
// a queue of its own. A build with some other transport installed simply has
// no voice: the pump asks comms_transport() first and stays silent when the
// carrier is not this one.
int comms_recv_voice(void *buf, int cap, int *from_slot) {
    return lb_recv_voice(buf, cap, from_slot);
}

// ---------------------------------------------------------------------------
// INSTALL
// ---------------------------------------------------------------------------

bool comms_loopback_install_from_env() {
    if (g_installed) return true;

    const char *role = std::getenv("SM64DS_COMMS_ROLE");
    if (!role || !*role) return false;      // nothing installed, nothing moved

    // TWO SPELLINGS, AND BOTH OF THEM ARE THIS TREE'S OWN. Run rel0215, lane
    // vsnet.
    //
    // 'parent'/'child' is the seam's vocabulary and has been since MP2. '0'/'1'
    // is the RELAY WIRE'S vocabulary -- the role byte at offset 5 of the HELLO,
    // frozen in the handshake contract above and written down as
    // "role: 0 parent, 1 child" in port/tools/relay/README.md's own datagram
    // table. Somebody who has read the relay's documentation and is setting up
    // a relay session types the number, because the number is what that
    // document taught him, and the 0.2.15 demo is what happens next: the role
    // check refuses, nothing installs, and two windows sit in the same arena
    // with no idea the other exists.
    //
    // THIS IS NOT A SECOND IDIOM, it is one knob that accepts the two
    // vocabularies the tree already publishes for the same fact. The numbers
    // mean what the wire says they mean and nothing else is accepted -- a
    // 'host', a 'p1', a '2' still refuse, loudly, with both spellings named in
    // the refusal so the next person does not have to read this file to find
    // out what to type.
    if (std::strcmp(role, "parent") == 0)      g_role = kRoleParent;
    else if (std::strcmp(role, "child") == 0)  g_role = kRoleChild;
    else if (std::strcmp(role, "0") == 0)      g_role = kRoleParent;
    else if (std::strcmp(role, "1") == 0)      g_role = kRoleChild;
    else {
        std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_ROLE='%s' is not "
                     "'parent'/'child' (the seam's spelling) or '0'/'1' (the "
                     "relay wire's, 0 parent 1 child); nothing installed, the "
                     "seam keeps its solo answers\n", role);
        return false;
    }

    // ---------------------------------------------------------------------
    // THE SESSION'S WIDTH, read here and nowhere else. Run vs16.
    //
    // SM64DS_VS_PLAYERS is the conductor's knob and the lobby's plan already
    // exports it on every seat, byte-identical across the session -- which is
    // what makes it safe to derive the WIRE from it: every peer in a room
    // reads the same number, so every peer picks the same wire, with nothing
    // negotiated and nothing to get out of step.
    //
    // ABSENT OR <= 4 IS NARROW, and that is the default in every existing
    // deployment. A 2P or 4P session speaks version 2 at 0x90 bytes exactly as
    // it always has; this block simply does not fire.
    //
    // OUT OF RANGE IS IGNORED AND SAID OUT LOUD, matching the conductor's own
    // handling of the same knob, because a mistyped width would otherwise
    // become a session that never forms and never says why.
    if (const char *w = std::getenv("SM64DS_VS_PLAYERS")) {
        const int v = std::atoi(w);
        if (v >= 1 && v <= kCommsMaxPlayers) {
            g_want_players = v;
        } else {
            std::fprintf(stderr, "[comms:loopback] SM64DS_VS_PLAYERS=%s out of "
                         "range 1..%d; this session stays on the four-player "
                         "wire\n", w, (int)kCommsMaxPlayers);
        }
    }
    if (wire_wide())
        std::fprintf(stderr, "[comms:loopback] WIDE SESSION: %d players, wire "
                     "version %u, %d-byte datagram. This is a MOD of the "
                     "cartridge's four-player protocol and a peer speaking the "
                     "four-player wire will be refused.\n",
                     g_want_players, (unsigned)wire_version(), packet_bytes());

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
    // resolved to one value on one end and a fallback on the other.
    //
    // THAT SENTENCE USED TO END "-- and while mismatched delays are not a
    // desync (both consoles consume the same round sequence, one simply lags
    // the other), it would mean a knob whose effective value nobody could
    // state". THE PARENTHESIS WAS FALSE, and it is quoted here rather than
    // deleted because it is the exact wrong idea that nearly shipped a silent
    // desync. A MISMATCH IS A DESYNC. Both consoles do consume the same round
    // SEQUENCE, but not at the same FRAME INDEX, and the ROM indexes by frame;
    // the derivation and the ROM's own compare at src/func_0203ea5c.c:418 are
    // written out over the ACCEPT's delay field. Measured on the public relay:
    // 5-against-0, 0-against-5, 5-against-6 and 5-against-4 all diverge from
    // about frame 65, positions widening, no constant shift explaining them,
    // while both ends report a perfectly healthy paired session.
    //
    // So the value is no longer left to two launchers agreeing about it. THE
    // PARENT CARRIES IT IN THE ACCEPT AND THE CHILD ADOPTS IT, which makes
    // "both ends run the same depth" a property of the wire rather than a note
    // in a README, and the version byte at the top of this file refuses the
    // generation that cannot do it. What follows picks the number the PARENT
    // will use; a child's own answer here is provisional until its ACCEPT.
    //
    // THE FORMULA, so a caller is not guessing: N >= round_trip_ms / 33,
    // rounded up. N=2 covers 66 ms, N=3 covers 100 ms, N=4 covers 133 ms. Too
    // low costs stalls (counted as `starved`); too high costs input lag.
    //
    // Off on loopback even when asked, because loopback's round trip is
    // microseconds: there is nothing there to hide and it would only add
    // frames of lag to a session that has none.
    //
    // ---- AND IT IS ON BY DEFAULT OFF THIS MACHINE. Run rel0215 lane vslag,
    // and "there is a knob for it" was not good enough:
    //
    //   The owner played the two-window VS demo through the live relay and
    //   reported it "extremely laggy, very slow and low framerate". Nothing
    //   was broken and nothing was mistuned. The knob was simply unset, so the
    //   session ran STOP AND WAIT, and under stop-and-wait a frame cannot
    //   finish until the peer's block for that same frame has crossed the wire
    //   and come back: THE FRAME RATE IS THE ROUND TRIP. Measured on that
    //   exact path -- port/tools/relay/test_client.py remote-check against the
    //   live relay with both endpoints on one desk, which is the demo's own
    //   shape -- the lockstep round trip is p50 90.6 ms, p95 98.8 ms. 1000/90.6
    //   is about 11 frames a second against the VS scene's 30.
    //
    //   Zero is the right default for the wire this file was born on and the
    //   wrong one for every wire it has grown to reach. So THE DEFAULT NOW
    //   FOLLOWS THE MODE, which is the one thing both ends derive identically
    //   without measuring anything and without a wire change: a relay session
    //   is a relay session on both consoles, so both compute the same N. The
    //   "a knob whose effective value nobody could state" objection that
    //   killed auto does not reach a constant, and the report line states it.
    //
    //   RELAY 5, and the 5 was MEASURED rather than derived. The formula off
    //   a good hour gives 4 (ceil(98.8 / 33.3) plus a frame of headroom), and
    //   4 is enough while the path stays that good -- 4 to 8 starved frames
    //   in 360. But the same desk an hour later measured p50 131 ms, and the
    //   three depths run back to back on THAT path separate cleanly
    //   (port/tools/vs_pace.py --arms live --delay N, 360 frames each):
    //
    //     N=4   starved 44   29.87 fps   frame p95 51.45 ms
    //     N=5   starved  5   30.02 fps   frame p95 46.16 ms
    //     N=6   starved  5   30.05 fps   frame p95 48.31 ms
    //
    //   5 is the knee ON A PATH SHAPED LIKE THIS ONE: it takes an order of
    //   magnitude off the stalls 4 leaves on an ordinary bad hour, and 6 buys
    //   nothing more for the extra frame of input lag it charges. An internet
    //   path is not one number, and a default tuned to its best hour is a
    //   default that fails in its worst.
    //
    //   THE KNEE IS PATH-SHAPED, NOT UNIVERSAL, and the review that scoped
    //   this measured the counter-case: under induced jitter of +/-45 ms the
    //   arms keep improving past 5 (23.25 / 26.46 / 29.09 fps, monotone), so
    //   on a path with that much spread 6 is still buying. A latency that is
    //   mostly CONSTANT is covered by a depth; a latency that is mostly SPREAD
    //   needs depth in proportion to the spread, and no one default is right
    //   for both. 5 is the default for the path this relay actually presents.
    //   The knob is for the path a given pair actually has, and the paragraph
    //   below is why no constant can be enough on its own.
    //
    //   NO FIXED N COVERS THE WHOLE TAIL, and pretending otherwise would be
    //   the dishonest version of this. That same measurement's p95 round trip
    //   was 410 ms, which is 13 frames -- past kInputDelayMax and far past
    //   what anyone would accept as input lag. The design answer is that a
    //   starve is CHEAP under pipelining: the frame waits only for the part of
    //   the round trip that ran past the budget, not for a whole one, which is
    //   why 44 starved frames still measured 29.87 fps.
    //
    //   DIRECT 2, which covers 66 ms, AND IT IS NOT MEASURED -- said plainly
    //   because the relay number above is. Direct mode is two different
    //   things: a LAN (microseconds, where two frames costs almost nothing)
    //   and a forwarded port between two houses (tens of ms). 2 is the
    //   smallest depth that covers the second without charging the first five
    //   frames of lag it has no use for. A direct pair that sees `starved`
    //   climb should raise it, and if that mode is ever measured the way relay
    //   was, this number should move to whatever the knee turns out to be.
    //   LOOPBACK 0, unchanged.
    //
    //   IT IS STILL A KNOB, in both directions. SM64DS_COMMS_INPUT_DELAY=0
    //   restores stop-and-wait exactly, which is how this lane measured its
    //   own before and after out of one binary; a pair on a worse path than
    //   this desk should raise it. THE SIGNAL TO RAISE IT IS `starved` IN THE
    //   REPORT LINE -- nonzero means frames are still waiting on the wire, and
    //   every one of those is a stall.
    //
    //   WHAT IT DOES NOT TOUCH: the simulation. The ROM's lockstep is
    //   untouched, both ends still consume the SAME round sequence in the same
    //   order, and nothing is predicted, rolled back or discarded. Only the
    //   offset between the frame being drawn and the round being read moves.
    if (g_net_mode == kNetRelay)       g_input_delay = 5;
    else if (g_net_mode == kNetDirect) g_input_delay = 2;

    if (const char *n = std::getenv("SM64DS_COMMS_INPUT_DELAY")) {
        int v = std::atoi(n);
        if (v < 0) v = 0;
        if (v > kInputDelayMax) v = kInputDelayMax;
        g_input_delay = v;
        g_delay_from_env = true;
    }

    // THE ADAPTIVE SIZING'S OWN KNOBS. All three are tuning, none of them is a
    // decision a player makes, and the defaults are the shipped behaviour's
    // own numbers: floor 5 is the relay constant this replaces, so a session
    // whose paths all measure well keeps exactly the cover it had.
    if (const char *v = std::getenv("SM64DS_COMMS_DELAY_SAFETY")) {
        const int q = std::atoi(v);
        if (q >= 100 && q <= 400) g_delay_safety_pct = q;
        else std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_DELAY_SAFETY="
                          "%s is outside 100..400 percent; keeping %d\n",
                          v, g_delay_safety_pct);
    }
    if (const char *v = std::getenv("SM64DS_COMMS_DELAY_MARGIN")) {
        const int q = std::atoi(v);
        if (q >= 0 && q <= 8) g_delay_margin = q;
        else std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_DELAY_MARGIN="
                          "%s is outside 0..8 frames; keeping %d\n",
                          v, g_delay_margin);
    }
    if (const char *v = std::getenv("SM64DS_COMMS_DELAY_FLOOR")) {
        const int q = std::atoi(v);
        if (q >= 0 && q <= kInputDelayMax) g_delay_floor = q;
        else std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_DELAY_FLOOR="
                          "%s is outside 0..%d frames; keeping %d\n",
                          v, kInputDelayMax, g_delay_floor);
    }
    // STAND IN FOR AN OLD BUILD, ON PURPOSE. The blocker this knob exists to
    // prove is a MIXED-GENERATION one, and proving it needs a peer that
    // behaves like 0.3.2: never sends a round-trip report, and DROPS an
    // adopted depth past that generation's own ceiling rather than clamping
    // to it. Keeping a real old binary as a fixture would rot; reproducing
    // its two relevant behaviours behind one env does not. A test knob, and
    // it says so on the line it prints.
    if (std::getenv("SM64DS_COMMS_LEGACY_PEER")) {
        g_legacy_peer_sim = true;
        std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_LEGACY_PEER: this "
                     "end will behave like a pre-0.3.3 peer -- no round-trip "
                     "report, and an adopted delay past %d dropped rather than "
                     "clamped\n", kLegacyInputDelayMax);
    }

    // NetMode (port/rollback): settings.json "NetMode": "lockstep" | "rollback",
    // SM64DS_NETMODE overriding it. ROLLBACK IS THE DEFAULT (owner's decision,
    // 2026-09-03; hal/host_settings.cpp reads an absent or unparseable file as
    // rollback). With lockstep g_rollback stays false, which leaves every path
    // above and below this block byte-for-byte what it was. Rollback runs at
    // input delay 0 by construction (the delay is what it replaces), and the
    // parent's choice travels in the ACCEPT so a child that asked for the
    // other mode adopts.
    //
    // THE WIDTH GUARD, applied after the env so it cannot be talked past:
    // determinism and cost are proven up to kRollbackMaxPlayers seated
    // players (the DET and COST rungs of port/tools/rollback_proof.py; past
    // eight the wide lane's Minimap::Render faults in lockstep too), so a
    // session opened wider than that runs lockstep and says why. Every peer
    // reads the same SM64DS_VS_PLAYERS, so every peer lands on the same side
    // of this line, and the parent's bit 17 carries it anyway.
    {
        int mode = host_setting_net_mode();
        if (const char *e = std::getenv("SM64DS_NETMODE")) {
            if (ieq_word(e, "rollback")) mode = 1;
            else if (ieq_word(e, "lockstep")) mode = 0;
            else std::fprintf(stderr, "[comms:loopback] SM64DS_NETMODE=%s is "
                              "neither lockstep nor rollback; ignored\n", e);
        }
        if (mode == 1 && g_want_players > kRollbackMaxPlayers) {
            std::fprintf(stderr, "[comms:loopback] NetMode rollback asked for "
                         "a %d-player session, but rollback is proven "
                         "(determinism and cost) only up to %d players; this "
                         "session runs LOCKSTEP instead\n",
                         g_want_players, (int)kRollbackMaxPlayers);
            mode = 0;
        }
        g_rollback = mode == 1;
        if (g_rollback) {
            if (g_input_delay > 0)
                std::fprintf(stderr, "[comms:loopback] NetMode rollback: the "
                             "input delay of %d is not used (rollback runs at "
                             "delay 0 and predicts instead)\n", g_input_delay);
            g_input_delay = 0;
            std::fprintf(stderr, "[comms:loopback] NetMode ROLLBACK: rounds are "
                         "served the frame they open, missing blocks are "
                         "predicted up to %u rounds ahead, a misprediction "
                         "rewinds and replays (hal/rollback.cpp); past the "
                         "window this stalls %u ms and then drops the slot\n",
                         kRbWindow, kRbGraceMs);
        }
    }
    // ARMED ONLY WHERE IT CAN HELP AND ONLY WHERE IT IS ALLOWED TO DECIDE.
    //   THE PARENT, because the delay is published parent to child and a child
    //     that sized its own would be sizing a number it is about to lose.
    //   NOT UNDER THE ENV OVERRIDE: a run that set SM64DS_COMMS_INPUT_DELAY is
    //     measuring something, and retuning it underneath is how a controlled
    //     experiment stops being one. The override still wins, as promised.
    //   NOT WITH THE PIPELINE OFF, which is stop-and-wait by request and has
    //     no depth to size.
    //   NOT ON A LOOPBACK CARRIER, EVEN AN INDUCED-LATENCY ONE, and this was
    //     tried the other way first. An induced-latency loopback is the
    //     controlled experiment for pipelining and net_proof rungs N4, N5 and
    //     N7 are built on it: N7 in particular runs one arm with NO input-delay
    //     env as its STOP-AND-WAIT baseline and compares a pipelined arm
    //     against it. Sizing that baseline arm turns the comparison into two
    //     identical arms and the rung measures nothing while still passing its
    //     session checks -- the exact failure its own banner warns about. The
    //     mode default of 0 on loopback is the rig contract, so the sizing
    //     leaves it alone and is measured over a RELAY instead, which is the
    //     mode that actually has the problem.
    g_adaptive_delay = (g_role == kRoleParent) && !g_delay_from_env &&
                       g_net_mode != kNetLoopback && g_input_delay > 0;
    if (const char *v = std::getenv("SM64DS_COMMS_ADAPTIVE_DELAY"))
        if (std::atoi(v) == 0) g_adaptive_delay = false;
    // THE VALUE TO FALL BACK TO, captured before anything can raise it. It is
    // the mode default, which is the number every build that has ever shipped
    // already runs, and that is precisely what makes a fallback safe.
    if (g_adaptive_delay) g_delay_presize = g_delay_mode_default = g_input_delay;
    if (g_adaptive_delay)
        std::fprintf(stderr, "[comms:loopback] adaptive input delay ARMED: "
                     "this end is the parent and will size the session's depth "
                     "from the worst round trip its children report "
                     "(safety %d%%, margin %d, floor %d, cap %d). It is frozen "
                     "before the first round and published in every ACCEPT, so "
                     "every peer still runs one number.\n",
                     g_delay_safety_pct, g_delay_margin, g_delay_floor,
                     kInputDelayMax);
    // REFUSED ON A BARE LOOPBACK, ALLOWED WHEN THERE IS A ROUND TRIP TO
    // HIDE -- and the second half of that sentence was missing at first,
    // which broke the one rig that most needed it. The guard keyed off the
    // MODE, so an induced-latency run (loopback carrier, delay knob on,
    // which is the whole controlled experiment) had its input delay
    // silently thrown away and measured pipelining doing nothing. The
    // right question is not "which mode is this" but "is there any latency
    // here at all", and with the induction knobs on there certainly is.
    //
    // Only ever reached from the env now: the mode-derived default above is
    // already 0 on loopback, so a bare loopback run prints nothing.
    if (g_net_mode == kNetLoopback && g_input_delay > 0 &&
        g_delay_ms <= 0 && g_jitter_ms <= 0) {
        std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_INPUT_DELAY is "
                     "for a wire with a round trip on it; a bare loopback "
                     "has none and no delay is being induced. Ignored.\n");
        g_input_delay = 0;
        g_adaptive_delay = false;   // nothing left to size
    }
    g_adaptive_armed = g_adaptive_delay;
    // A PINNED DEPTH PAST 8 ON THE PARENT IS A MIXED-BUILD DESYNC WAITING.
    // The sizing withdraws a raised number when an unreported peer is live,
    // but SM64DS_COMMS_INPUT_DELAY disarms the sizing, so nothing withdraws
    // a pinned 10: an old build present from the start reads it off its
    // first accept, DROPS it past its own clamp of 8, and keeps its own
    // number in silence. Say so at install, where the person who set it is.
    if (g_role == kRoleParent && g_delay_from_env &&
        g_input_delay > kLegacyInputDelayMax)
        std::fprintf(stderr, "[comms:loopback] WARNING: SM64DS_COMMS_INPUT_DELAY"
                     "=%d is past the %d that every shipped build can adopt. "
                     "A pre-0.3.3 peer in this session will drop it and keep "
                     "its own number in silence, which is two depths in one "
                     "match. Only use a depth past %d when every peer is a "
                     "current build.\n",
                     g_input_delay, kLegacyInputDelayMax, kLegacyInputDelayMax);
    if (g_input_delay > 0)
        std::fprintf(stderr, "[comms:loopback] input delay %d frame(s) (%s): "
                     "frame R is handed the records from round R-%d, so "
                     "rounds overlap the wire instead of taking turns with "
                     "it. %s\n",
                     g_input_delay,
                     g_delay_from_env ? "SM64DS_COMMS_INPUT_DELAY"
                                      : "the default for this net mode",
                     g_input_delay,
                     g_role == kRoleParent
                         ? "This end is the PARENT, so this is the number the "
                           "whole session runs at: it goes out in every ACCEPT "
                           "and every child adopts it."
                         : "This end is a CHILD, so this is PROVISIONAL -- the "
                           "parent's ACCEPT carries the number the session "
                           "actually runs at, and it is adopted here.");
    else if (g_delay_from_env && g_net_mode != kNetLoopback)
        std::fprintf(stderr, "[comms:loopback] input delay 0 by request: this "
                     "session is STOP AND WAIT, so a frame costs a whole round "
                     "trip and the frame rate is the round trip.\n");

    if (const char *s = std::getenv("SM64DS_COMMS_SLOT")) {
        const int v = std::atoi(s);
        if (g_role != kRoleChild)
            std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_SLOT is a "
                         "child knob; ignored for the parent, which is always "
                         "slot 0\n");
        else if (v >= 1 && v < slot_cap()) g_pinned = v;
        else std::fprintf(stderr, "[comms:loopback] SM64DS_COMMS_SLOT=%s out "
                          "of range 1..%d; claiming the first free slot "
                          "instead\n", s, slot_cap() - 1);
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

// ---------------------------------------------------------------------------
// ROLLBACK (port/rollback): what hal/rollback.cpp's frame-boundary step asks.
// ---------------------------------------------------------------------------
bool comms_rb_enabled() {
    return g_rollback && g_open &&
           (g_state == kCommsParentConnected || g_state == kCommsChildConnected);
}
bool comms_rb_mode() { return g_rollback; }
bool comms_rb_replaying() {
    // a session that collapsed mid-replay (the last peer left) has nothing
    // left to replay against; the frame loop's own check reads this
    return g_rb_replaying &&
           (g_state == kCommsParentConnected || g_state == kCommsChildConnected);
}
unsigned comms_rb_round() { return g_round; }
unsigned comms_rb_replay_end() { return g_rb_replay_end; }

// The oldest consumed round whose served blocks the wire has since
// contradicted -- a guessed block confirmed different, or a served live mask
// the parent has revised -- or ~0u when every consumed round stands.
unsigned comms_rb_scan() {
    if (!g_rollback || g_rb_replaying) return ~0u;
    const unsigned end = g_round;
    unsigned low = g_rb_scan_low, target = ~0u;
    for (unsigned q = g_rb_scan_low; (int)(q - end) < 0; ++q) {
        PipeRound *s = pipe_find(q);
        if (!s || !s->consumed) break;
        bool bad = false;
        if (s->live_known && s->served_live != s->live) bad = true;
        const unsigned confirmed = s->pred_mask & s->mask;
        for (int k = 0; k < kCommsMaxPlayers && confirmed; ++k) {
            if (!(confirmed & (1u << k))) continue;
            if (rb_same_block(s->pred[k], s->blocks[k])) {
                ++g_rb_confirmed_ok;
                s->pred_mask &= ~(1u << k);
            } else {
                bad = true;
            }
        }
        if (bad) { ++g_rb_mispredicted; target = q; break; }
        const bool done = s->live_known && (s->mask & s->live) == s->live &&
                          s->pred_mask == 0;
        if (done && q == low) low = q + 1;
    }
    g_rb_scan_low = low;
    return target;
}

bool comms_rb_rewind(unsigned to) {
    if (!g_rollback || g_rb_replaying) return false;
    if ((int)(to - g_round) >= 0) return false;
    for (unsigned q = to; (int)(q - g_round) < 0; ++q) {
        PipeRound *s = pipe_find(q);
        if (!s || (s->mask & (1u << g_slot)) == 0) {
            ++g_rb_unrecoverable;
            rb_leave("a rollback reached a round the ring no longer holds");
            return false;
        }
    }
    g_rb_replay_end = g_round;
    g_rb_replaying  = true;
    g_round = to;
    ++g_rb_rewinds;
    for (unsigned q = to; (int)(q - g_rb_replay_end) < 0; ++q)
        if (PipeRound *s = pipe_find(q)) s->consumed = false;
    return true;
}

void comms_rb_flush(const char *why) {
    if (!g_rollback) return;
    g_rb_flush_wait = true;
    std::fprintf(stderr, "[comms:loopback] ROLLBACK: %s at round %u; guessing "
                 "is suspended until every open round is confirmed\n",
                 why ? why : "flush", g_round);
}

void comms_rb_det_reuse(bool on) { g_rb_det_reuse = on; }

const unsigned char *comms_rb_my_served(unsigned *round) {
    if (round) *round = g_rb_served_round;
    return g_rb_served_round == ~0u ? 0 : g_rb_my_served;
}

void comms_rb_leave(const char *why) {
    if (!g_rollback || !g_open) return;
    if (g_state != kCommsParentConnected && g_state != kCommsChildConnected)
        return;
    rb_leave(why ? why : "asked to");
}

CommsRollbackStats comms_rb_stats() {
    CommsRollbackStats r;
    r.predicted     = g_rb_predicted;
    r.confirmed_ok  = g_rb_confirmed_ok;
    r.mispredicted  = g_rb_mispredicted;
    r.rewinds       = g_rb_rewinds;
    r.replayed      = g_rb_replayed;
    r.stalled       = g_rb_stalled;
    r.stall_events  = g_rb_stall_events;
    r.drops         = g_rb_drops;
    r.unrecoverable = g_rb_unrecoverable;
    return r;
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
