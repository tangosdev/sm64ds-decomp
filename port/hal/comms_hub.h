// THE ADVENTURE HUB CARRIER. Run mp1, lane HUB1.
//
// The transport for adventure online ("ghosts"): every console plays its OWN
// solo game from its own save, and the wire carries only presentation -- where
// my body is and what it looks like, who I am, what I said, my voice. There
// are NO ROUNDS here: no input exchange, no lockstep, nobody waits for anybody,
// people join and leave whenever they like, and a frozen console freezes only
// its own ghost on everyone else's screen.
//
// WHAT THIS IS NOT. It is not a DS transport. It never calls
// comms_set_transport(), never seats the DS multiplayer role (data_020a0f04),
// never arms the "open the radio" one-shot, and the conductor never waits on
// it. As far as the ROM can tell the game is single-player, which is exactly
// what an adventure console is. comms_loopback.cpp stays the VS carrier.
//
// PORT_HOST_ABI: host plumbing below the game, like comms_loopback.cpp. Nothing
// in src/ knows it exists. ws2_32 is LoadLibrary'd on install (the rule in
// comms_loopback.cpp's banner: a static ws2_32 import loses the DS fixed
// ranges), so a solo run never loads a socket library.
//
// ---------------------------------------------------------------------------
// SELECTED ONLY BY ENV. comms_hub_install_from_env() installs the hub when the
// adventure mode is on (SM64DS_ADVENTURE, or the AdventureGhosts setting) AND
// one of the two address modes below is named. Otherwise it installs nothing
// and returns false, and the caller goes on to comms_loopback_install_from_env()
// exactly as before (VS, and every solo path, unchanged). A VS match
// (SM64DS_VS_MAP set) is never the hub's, even with the adventure flag on, and
// an adventure console is never handed to the VS carrier; see
// comms_hub_install_from_env below.
//
//   RELAY MODE (what the launcher sets; design.txt section 3, relay v2):
//     SM64DS_COMMS_RELAY=<host[:port]>  the relay, default port 41234
//     SM64DS_COMMS_CODE=<1..8 ascii>    the room's hub code (lobby-minted)
//     SM64DS_HUB_SEAT=<0..15>           OPTIONAL pin; default 0xFF = any free
//                                       seat (the lobby plan's want_seat 255)
//   The hub sends HELLO v2 role 2 (member) and re-sends it every 1 s as the
//   keepalive; the ACK v2 names the seat the relay will stamp on everything
//   this console sends. Received datagrams carry the SENDER's seat at byte 5,
//   written by the relay; a payload can never claim to be another seat.
//
//   LOOPBACK MESH (tests only; no relay needed):
//     SM64DS_HUB_PORT=<n>   seat k binds 127.0.0.1:<n>+k, first free k in
//                           0..15; every datagram goes to the other 15 ports.
//                           The receiver stamps byte 5 from the SOURCE PORT.
//                           NO DEFAULT on purpose (derive it from your PID in a
//                           harness; never 51765, the live desk pair's port).
//
//   With the hub installed, SM64DS_COMMS_ROLE is IGNORED (logged): an
//   adventure console must never also open the VS carrier.
//
//   Test knobs (all off by default):
//     SM64DS_HUB_DELAY_MS=<n>   hold every received datagram n ms (one way)
//     SM64DS_HUB_LOSS_PCT=<n>   drop n% of received datagrams (0..90)
//     SM64DS_HUB_REPORT_S=<n>   print the report line every n s (default 10;
//                               0 = only at exit)
//
// Exit code 4: the relay answered the FIRST pairing with status 1 (room full).
// The launcher then asks the lobby for another world (design.txt section 5).
// A "full" on a later re-pair (relay restart mid-game) never exits: the player
// keeps playing solo and the report line says so.
//
// ---------------------------------------------------------------------------
// THE WIRE (design.txt section 6). Every datagram starts with the 8-byte
// envelope:  0 tag[4] | 4 version u8 | 5 seat u8 | 6 u8 | 7 u8   (little endian)
// Byte 5 is ALWAYS overwritten by the carrier/relay; senders may leave it 0xFF.
// comms_hub_send() refuses anything that is not in this table, by tag and
// exact/min/max length, so a caller can never send what the relay would drop:
//
//   kind   tag     length        version  queue at the receiver
//   ghost  'SYNG'  44            1        FIFO 64 + newest-by-seq per seat
//   ident  'SYNI'  24..64, %4    1        FIFO 32 + newest per seat
//          (length = max(24, 20 + name_len rounded up to 4); 20+name_len <= len)
//   chat   'SYNT'  12..132       1        FIFO 32 (C1 dedupes on seat,msg_id)
//   voice  'SYNV'  176 or 340    1        FIFO 32 (hal/voice_chat.h format)
//   ping   'SYNP'  12            -        hub-internal (RTT per seat, 2 Hz)
//   pong   'SYNQ'  12            -        hub-internal (byte 6 = pinger seat)
// PARTY AND COOP ROOMS (run mp1 wave 56, lane WIRE1; layouts in
// hal/hub_presence.h, the room gates are the relay's and are mirrored here):
//   presence 'SYNL' 20          1        FIFO 16; world, party, coop rooms
//   pstar    'SYNX' 12          1        FIFO 16; party rooms only
//   phit     'SYNH' 24          1        FIFO 16; party only, to ONE seat (the
//                                         victim, byte 6; the relay routes it)
//   cworld   'SYNW' 16..700     1        FIFO 32; coop rooms only
//   cctl     'SYNB' 16..700     1        FIFO 32; coop rooms only
// A party or coop room holds seats 0..3 (the ROM seats four bodies); the room
// kind is SM64DS_ROOM_KIND = world | party | coop (default world) and picks the
// HELLO role (2 / 4 / 5). hub_on_kind() below plugs a consumer into a kind.
// Anything else received is dropped and counted by reason.
//
// THE ARENA SHAPE (run mp1, lane ARENACORE1; hal/vs_arena.h). A VS console in
// arena mode opens this same carrier through comms_hub_install_arena(): HELLO
// v2 role 3 (arena member), the relay's "arena" session, the REFEREE in the
// relay at seat mark 0xFE (port/tools/relay/arena_wire.h, lane REFEREE1).
// The kinds then differ by shape: 'SYNG' is adventure only, and these four
// plus two referee-clock sizes are arena only (everything else as above):
//   state  'SYNS'  48            1        FIFO 64 + newest per seat (A0)
//   object 'SYNO'  32            1        FIFO 64 (a star in flight, A3)
//   ref    'SYNR'  272           1        FIFO 32; ONLY from the relay with
//                                         byte 5 == 0xFE (the referee)
//   claim  'SYNE'  24            1        send only (to the referee)
//   rping  'SYNP'  20            2        hub-internal, byte 6 = 0xFE, 2 Hz
//   rpong  'SYNQ'  20            2        hub-internal, ONLY from 0xFE: the
//                                         referee clock (comms_hub_referee_clock)

#ifndef PORT_HAL_COMMS_HUB_H
#define PORT_HAL_COMMS_HUB_H

#include <cstddef>

#include "hub_presence.h"

namespace port {

enum : int {
    kHubMaxSeats     = 32,       // WORLD32: 32 world seats (was 16); arena VS stays 16 on the relay
    kHubEnvelopeBytes = 8,
    kHubMaxDatagram  = 700,     // the largest kind (coop world / control);
                                // was 340 (voice) before wave 56
    kHubPresenceMs   = 3000,    // a seat unheard this long has left
};

// The four kinds a game-side layer sends and receives. Pings are the hub's own.
enum HubKind : int {
    kHubGhost = 0,   // 'SYNG'  W2 GHOSTWIRE1
    kHubIdent = 1,   // 'SYNI'  W2 GHOSTWIRE1
    kHubChat  = 2,   // 'SYNT'  C1 CHATUI1
    kHubVoice = 3,   // 'SYNV'  V1 VOICE1
    // ARENA ONLY (lane ARENACORE1; hal/vs_arena.h)
    kHubArenaState  = 4,   // 'SYNS'  A0 vs_arena.cpp (the only consumer)
    kHubArenaObject = 5,   // 'SYNO'  A3 vs_arena_ref.cpp
    kHubArenaRef    = 6,   // 'SYNR'  A3 vs_arena_ref.cpp (seat 0xFE)
    kHubArenaClaim  = 7,   // 'SYNE'  A3 sends; never received
    // PARTY / COOP (lane WIRE1; hal/hub_presence.h)
    kHubPresence  = 8,     // 'SYNL'  hub_presence.cpp
    kHubPartyStar = 9,     // 'SYNX'  party only
    kHubPartyHit  = 10,    // 'SYNH'  party only, to one seat
    kHubCoopWorld = 11,    // 'SYNW'  coop only
    kHubCoopCtl   = 12,    // 'SYNB'  coop only
    // HOST EVENTS (lane EVENTS1, prototype; hal/event_client.h). Receive only,
    // and only from the relay itself (byte 5 == 0xFD) in a world room.
    kHubEvent     = 13,    // 'SYNZ'  96 bytes, version 1
    kHubKinds = 14,
};
enum : int { kHubRefereeSeat = 0xFE };   // byte 5 of what the referee sends
enum : int { kHubEventSeat = 0xFD };     // byte 5 of what the relay sends as an event

// The tags as little-endian words (the first four bytes read as a u32).
enum : unsigned {
    kHubTagGhost = 0x474e5953u,   // 'S','Y','N','G'
    kHubTagIdent = 0x494e5953u,   // 'S','Y','N','I'
    kHubTagChat  = 0x544e5953u,   // 'S','Y','N','T'
    kHubTagVoice = 0x564e5953u,   // 'S','Y','N','V'
    kHubTagPing  = 0x504e5953u,   // 'S','Y','N','P'
    kHubTagPong  = 0x514e5953u,   // 'S','Y','N','Q'
    kHubTagArenaState  = 0x534e5953u,   // 'S','Y','N','S'
    kHubTagArenaObject = 0x4f4e5953u,   // 'S','Y','N','O'
    kHubTagArenaRef    = 0x524e5953u,   // 'S','Y','N','R'
    kHubTagArenaClaim  = 0x454e5953u,   // 'S','Y','N','E'
    kHubTagEvent       = 0x5a4e5953u,   // 'S','Y','N','Z'  (host events, relay only)
};

// ---------------------------------------------------------------------------
// THE LAYOUTS, one definition for every lane (design.txt 6.1 - 6.3).
// ---------------------------------------------------------------------------
#pragma pack(push, 1)
struct HubEnvelope {
    unsigned      tag;
    unsigned char version;
    unsigned char seat;        // carrier/relay-stamped sender seat
    unsigned char b6;
    unsigned char b7;
};

// 'SYNG' ghost frame v1, 44 bytes. 15 Hz while playing, 2 Hz with kGhostPaused.
enum : unsigned char {
    kGhostLive      = 0x01,
    kGhostGrounded  = 0x02,
    kGhostTeleport  = 0x04,    // snap, do not interpolate
    kGhostPaused    = 0x08,    // paused / in a menu
    kGhostTalking   = 0x10,    // mic gate open (V1 exports it)
};
// BUILD 6 (lane GHOSTDOOR1, hal/ghost_door.h): the area byte, the low byte of
// vy and flags bit 6 (0x40) carry the door / area / hidden look of a ghost.
// The hub itself only copies flags / level_id / area into HubSeatInfo.
struct HubGhostFrame {
    unsigned       tag;         // kHubTagGhost
    unsigned char  version;     // 1
    unsigned char  seat;
    unsigned char  flags;       // kGhost*
    unsigned char  character;   // 0 Mario, 1 Luigi, 2 Wario, 3 Yoshi
    unsigned short seq;         // wrap-compared, newest wins
    unsigned short level_id;    // data_0209f2f8
    int            x, y, z;     // Fix12
    short          vx, vy, vz;  // per-frame delta, clamped; 0 with teleport
    short          yaw;
    unsigned short anim_id;
    int            anim_frame;  // 20.12
    unsigned short stars;
    unsigned char  area;
    unsigned char  reserved[3]; // 0
};

// 'SYNI' identity v1, 20-byte header + name_len bytes of UTF-8, zero padded
// to a multiple of 4 and to at least 24 bytes (so a 0..4 byte name is 24). Sent on join, every 2 s, and when a new seat appears.
struct HubIdentHead {
    unsigned       tag;         // kHubTagIdent
    unsigned char  version;     // 1
    unsigned char  seat;
    unsigned char  name_len;    // 0..24
    unsigned char  character;
    unsigned       build_id;
    unsigned char  body_rgb[3];
    unsigned char  shoes_rgb[3];
    unsigned char  flags;
    unsigned char  reserved;
    // unsigned char name[name_len]; then zero pad to a multiple of 4
};

// 'SYNT' chat v1, 12-byte header + text_len bytes of UTF-8 (1..120).
struct HubChatHead {
    unsigned       tag;         // kHubTagChat
    unsigned char  version;     // 1
    unsigned char  seat;
    unsigned char  flags;       // 0
    unsigned char  text_len;    // 1..120
    unsigned short msg_id;
    unsigned short reserved;    // 0
    // unsigned char text[text_len];
};
#pragma pack(pop)
static_assert(sizeof(HubEnvelope) == 8, "envelope is 8 bytes");
static_assert(sizeof(HubGhostFrame) == 44, "SYNG v1 is 44 bytes");
static_assert(offsetof(HubGhostFrame, level_id) == 10, "SYNG level at 10");
static_assert(offsetof(HubGhostFrame, x) == 12, "SYNG x at 12");
static_assert(offsetof(HubGhostFrame, vx) == 24, "SYNG vx at 24");
static_assert(offsetof(HubGhostFrame, yaw) == 30, "SYNG yaw at 30");
static_assert(offsetof(HubGhostFrame, anim_frame) == 34, "SYNG anim at 34");
static_assert(offsetof(HubGhostFrame, area) == 40, "SYNG area at 40");
static_assert(sizeof(HubIdentHead) == 20, "SYNI head is 20 bytes");
static_assert(sizeof(HubChatHead) == 12, "SYNT head is 12 bytes");

enum : int {
    kHubIdentNameMax = 24,
    kHubChatTextMax  = 120,
};

// ---------------------------------------------------------------------------
// LIFECYCLE
// ---------------------------------------------------------------------------

// Called once from main() (tests/walk_window.cpp) BEFORE
// comms_loopback_install_from_env(). Returns true when this is an adventure
// session the hub owns, and the caller then skips the loopback install: the
// hub was installed (relay or mesh), or the console was handed the VS
// carrier's env (SM64DS_COMMS_ROLE) with no hub address and plays solo, since
// adventure never seats a DS role. Returns false for every VS match
// (SM64DS_VS_MAP set, even with the adventure flag on), every solo run, and
// offline adventure. Whether a socket is open: comms_hub_active(). Safe to
// call more than once.
bool comms_hub_install_from_env();

// THE ARENA SHAPE (hal/vs_arena.cpp calls it; nothing else should). Opens
// the hub for a VS arena console: relay (SM64DS_COMMS_RELAY + _CODE) with
// HELLO v2 role 3 asking for `want_seat` (0..15, or 0xFF any), or the test
// mesh (SM64DS_HUB_PORT, binding base+want_seat first when one is pinned).
// No adventure gate, no VS refusal: the caller decided. True when the hub owns
// the session (opened, or failed after deciding to); false only when no
// address is named at all. Safe to call more than once (first call decides).
bool comms_hub_install_arena(int want_seat);

// True when installed in the arena shape.
bool comms_hub_arena();

// True once installed (socket open), whether or not a peer has been heard.
bool comms_hub_active();

enum HubState : int {
    kHubOff = 0,          // not installed (solo or VS)
    kHubPairing = 1,      // relay: HELLO sent, no ACK yet
    kHubLive = 2,         // seat known; sending and receiving
    kHubRelayTooOld = 3,  // the relay answered as v1: adventure online needs v2
    kHubRefused = 4,      // relay status 3/5 (version mix / wrong session kind)
    kHubFull = 5,         // relay status 1 after the first pairing (solo on)
    kHubFailed = 6,       // no socket (ws2_32, bind, resolve)
};
int comms_hub_state();

// My seat, 0..15, or -1 until the relay's ACK (loopback: the bound port).
int comms_hub_my_seat();
// REJOIN1 (run coopfix1). Milliseconds since the relay last ACKed this console's HELLO (relay mode, once paired), else
// 0. And SM64DS_COOP_REJOIN (default on; 0 = the old drop rules, for the before rows): a coop hub that loses the relay
// asks for its old seat back, and a guest holds the room through a quiet host (hal/coop_boot.cpp).
// Test knob SM64DS_HUB_BLACKOUT=<at_ms>:<for_ms>[;...] (off by default): no datagram in or out in that window, counted
// from the install.
unsigned comms_hub_relay_quiet_ms();
bool comms_hub_coop_rejoin();

// ---------------------------------------------------------------------------
// SEND / RECEIVE. Thread-safe; never block. The hub runs its own I/O thread
// (keepalive, pings, receive), so presence and keepalive do not depend on the
// game's frame rate or on anyone calling in.
// ---------------------------------------------------------------------------

// Send one whole datagram (envelope included) to every other seat. Refused
// (false, counted) when the hub is not live or the tag/length/version is not
// in the table above. Byte 5 is stamped with my seat before it goes out.
bool comms_hub_send(const void *datagram, int len);

// Pop the OLDEST queued datagram of `kind`. Returns its length (the whole
// datagram, envelope included, byte 5 = the sender seat) or 0 when none is
// waiting; *seat gets the sender seat. A datagram longer than `cap` is
// dropped (counted) and the next one is tried. Each kind has exactly one
// consumer lane: ghost + ident W2, chat C1, voice V1.
int comms_hub_recv(HubKind kind, void *buf, int cap, int *seat);

// The same, plus *t_ms = the comms_hub_now_ms() at which the I/O thread took
// the datagram off the socket (after any SM64DS_HUB_DELAY_MS hold).
int comms_hub_recv_t(HubKind kind, void *buf, int cap, int *seat,
                     unsigned *t_ms);

// The hub's millisecond clock (performance counter), the one recv_t stamps.
unsigned comms_hub_now_ms();

// THE REFEREE CLOCK (arena, relay mode). The hub pings the referee ('SYNP' v2,
// byte 6 = 0xFE) at 2 Hz and keeps the 8 newest pongs; the estimate comes from
// the one with the smallest round trip (half the route asymmetry of error).
// *ref_now_ms = the referee's ms right now; *rtt_ms = that sample's RTT;
// *cap_ms (optional) = the referee's back-dating cap for my seat as it last
// said. False before the first pong (and always in the mesh).
bool comms_hub_referee_clock(unsigned *ref_now_ms, int *rtt_ms,
                             int *cap_ms);

// WHAT KIND OF ROOM THIS HUB IS IN (hal/hub_presence.h HubRoomKind): world
// (the default, the old hub), party or coop, from SM64DS_ROOM_KIND at install.
// Decides the HELLO role and which kinds this console may send and take.
// kHubRoomWorld when the hub is not installed.
HubRoomKind comms_hub_room_kind();

// PLUG A CONSUMER INTO A KIND (the five party / coop kinds only). The
// handler runs on the CALLER of comms_hub_pump(), never on the I/O thread, so
// a consumer's own state needs no lock. One handler per kind; registering
// again replaces it (false for any other kind). With no handler a kind just
// queues (comms_hub_recv works as for every kind), oldest dropped when full.
// `data` is the whole datagram, byte 5 = the sender seat, already validated
// by the hub (tag, version, fixed fields, room gate, and for 'SYNH' that I am
// the victim).
typedef void (*HubKindFn)(int seat, const unsigned char *data, int len,
                          void *user);
bool hub_on_kind(HubKind kind, HubKindFn fn, void *user = nullptr);

// Deliver everything queued for kinds that have a handler; returns how many
// datagrams were delivered. Call once a frame (hub_presence's frame hook
// does, port_hub_presence_frame). Bounded: at most 64 per kind per call.
int comms_hub_pump();

// The NEWEST datagram of `kind` from `seat` (ghost: newest by seq). Does not
// consume anything from the FIFO. Returns the length or 0; *age_ms (optional)
// gets how long ago it arrived.
int comms_hub_latest(HubKind kind, int seat, void *buf, int cap,
                     unsigned *age_ms);

// ---------------------------------------------------------------------------
// SEATS. A seat is PRESENT while any valid datagram (pings included, 2 Hz)
// from it arrived in the last kHubPresenceMs. Consumers detect joins and
// leaves by comparing join_epoch / roster_version with what they saw last, so
// any number of lanes can watch the roster without eating each other's events.
// ---------------------------------------------------------------------------
struct HubSeatInfo {
    bool     present;
    unsigned join_epoch;      // bumps every time this seat (re)joins
    unsigned heard_ms_ago;    // any kind
    unsigned ghost_ms_ago;    // newest SYNG; ~0u if none this presence
    int      rtt_ms;          // SYNP/SYNQ round trip, -1 until measured
    // From the newest SYNG (valid when has_ghost). The hub parses only the
    // fixed fields below; W2 owns everything else in the frame.
    bool     has_ghost;
    unsigned char flags;      // kGhost*
    int      level_id;
    int      area;
    int      x, y, z;         // Fix12
};
bool comms_hub_seat_info(int seat, HubSeatInfo *out);   // false: bad seat/off
unsigned comms_hub_seat_mask();        // bit k = seat k present (never mine)
unsigned comms_hub_roster_version();   // bumps on every join or leave

// ---------------------------------------------------------------------------
// COUNTERS + REPORT
// ---------------------------------------------------------------------------
struct CommsHubStats {
    bool installed;
    int  mode;                 // 0 none, 1 relay, 2 loopback mesh
    int  state;                // HubState
    int  seat;
    unsigned seat_mask;
    unsigned long long sent[kHubKinds], recvd[kHubKinds];
    unsigned long long pings_sent, pongs_sent, pongs_recvd;
    unsigned long long send_refused;      // not live, or not in the table
    unsigned long long drop_tag, drop_len, drop_version, drop_source,
                       drop_self, drop_seat, drop_queue, drop_cap, drop_test,
                       drop_stale;        // SYNG older than the newest seq
    unsigned long long hellos, acks;
    unsigned long long joins, leaves;
    // arena shape (lane ARENACORE1)
    bool arena;
    unsigned long long drop_shape;        // a kind the other shape owns
    unsigned long long ref_pings, ref_pongs;
    // party / coop rooms (lane WIRE1)
    int room;                             // HubRoomKind
    unsigned long long drop_field;        // a fixed field out of range
    unsigned long long dispatched;        // delivered to a hub_on_kind handler
};
CommsHubStats comms_hub_stats();
// MOVERS9: queue evictions of one kind; index kHubKinds = the coop mover-batch queue.
unsigned long long comms_hub_queue_drops(int kind);
void comms_hub_report(const char *tag);   // one "[hub] ..." line to stderr

// ---------------------------------------------------------------------------
// THE SESSION RECORD (run after093, lane SEAMHUB1; the one-process plan's
// M-f). An online game is told what it is through environment names: the
// launcher's own start sets them, and a join from the title sets them for the
// game it starts again (hal/online_join.cpp apply_plan). MSVC's getenv reads
// the CRT's copy of the environment, which SetEnvironmentVariableA does not
// change, so a name set or cleared after boot is never seen by a getenv in
// this process. Leaving an online world in place has to make the game stop
// reading them, so the hub, the ghost layer and the join plate read this
// record instead: it is filled from the environment on the first read (what
// this process was started with, as before), and the in-place leave clears it
// for good. The restart path is unchanged: it still hands the names to the
// game it starts through the environment block.
// ---------------------------------------------------------------------------
enum OnlineSessionField : int {
    kSessAdventure = 0,     // SM64DS_ADVENTURE
    kSessRelay,             // SM64DS_COMMS_RELAY
    kSessCode,              // SM64DS_COMMS_CODE
    kSessRoomKind,          // SM64DS_ROOM_KIND
    kSessVoiceAllowed,      // SM64DS_VOICE_ALLOWED
    kSessHubSeat,           // SM64DS_HUB_SEAT
    kSessPlayerName,        // SM64DS_PLAYER_NAME
    kSessPlayerColor,       // SM64DS_PLAYER_COLOR
    kSessPlayerShoes,       // SM64DS_PLAYER_SHOES
    kSessWorldNo,           // SM64DS_WORLD_NO
    kSessHubPort,           // SM64DS_HUB_PORT (the test mesh's base port)
    kSessFields
};
struct OnlineSession {
    bool filled;                       // read from the environment once
    bool cleared;                      // the in-place leave emptied it for good
    bool set[kSessFields];
    char value[kSessFields][256];
};
// The value of one field, or null when it is not set (or the record was
// cleared). Game thread.
const char *online_session_get(OnlineSessionField f);
// The environment name a field stands for ("SM64DS_COMMS_RELAY").
const char *online_session_name(OnlineSessionField f);
// The whole record (filled on first use).
const OnlineSession &online_session();
// Empty the record for the rest of this process (the in-place leave). Returns
// how many fields were set.
int online_session_clear();
// SEAMJOIN1: the in-place join (hal/online_join.cpp apply_plan) fills the record
// from the launcher's plan instead of the environment: begin empties it and
// marks it filled (so the environment is not read over it), set writes one
// field (null or "" = not set). Game thread.
void online_session_begin();
void online_session_set(OnlineSessionField f, const char *v);

// ---------------------------------------------------------------------------
// THE TEARDOWN (lane SEAMHUB1): the hub out of a live process, for the
// in-place leave. Stops and joins the I/O thread, closes the socket, empties
// every queue, seat and counter, forgets the install latch and every
// hub_on_kind handler, so comms_hub_active() is false and a later install
// starts from nothing. False (and nothing else changed) when the thread did
// not stop within the bound: the caller then restarts as before.
bool comms_hub_teardown(const char *why);
// No more periodic "[hub]" report lines (the leave asks for this at the quit).
void comms_hub_reports_off();
// SEAMJOIN1: the hub INTO a live process, for the in-place join: the install
// comms_hub_install_from_env makes in main(), made again from the session
// record now that the title has it filled. Refuses (false, *why says why,
// nothing left installed) when a hub is still up or its I/O thread still
// exists, or when the install itself fails (no socket, bad address or code).
bool comms_hub_install_in_place(const char **why);
// The same for the ghost layer's periodic "[ghost:tick]" line.
void comms_sync_reports_off();
// The other layers' halves, called by the leave after comms_hub_teardown:
// hal/comms_sync.cpp (the ghost layer, its file globals, the read-once
// identity, the own colour row and the ROM's body count / input gate words),
// hal/comms_conductor.cpp (the hub body count latch) and hal/hub_presence.cpp.
void comms_sync_leave_reset();
void comms_conductor_leave_reset();
void hub_presence_leave_reset();
// One line of what a fresh read of this console's identity gives now (name,
// both colours, the own colour row), for the leave's proof line.
void comms_sync_identity_text(char *out, int cap);
// SEAMJOIN1: the in-place join's halves. hal/comms_sync.cpp: the ghost layer
// reset and decided again on the hub just installed (true = on).
bool comms_sync_join_in_place();
// hal/comms_conductor.cpp: has main's level boot carved the game heap yet, and
// with what extra (*extra); and what the online world hub needs on top.
bool hub_game_heap_carved(unsigned *extra);
unsigned hub_heap_need();

}  // namespace port

#endif  // PORT_HAL_COMMS_HUB_H
