// ARENA MODE, the core half. Run mp1, lane ARENACORE1 (plan A0 of
// runs/mp1/out/VSSYNC1/plan.txt, "coopdx-style VS").
//
// WHAT ARENA MODE IS. Every console runs its own VS arena world, with one
// Player body per seat, exactly as a solo VS boot with SM64DS_VS_PLAYERS=n
// already does (VSSYNC1 M4). There is NO DS transport and NO round exchange:
// nobody waits for anybody. My own body reads my own pad with zero added
// delay; every other body is a PUPPET drawn from that console's own state
// stream ('SYNS', below). Everything two players can disagree about (stars,
// hits, score, the clock, the winner) belongs to the REFEREE in the relay
// (port/tools/relay, lane REFEREE1; its wire is arena_wire.h there).
//
// THIS HEADER IS THE CONTRACT the other arena lanes build against:
//   A1 PUPPET1   the puppet half of vs_arena.cpp (reads the sample rings
//                below, holds and poses the other seats' bodies);
//   A3 STARREF1  hal/vs_arena_ref.cpp, the game-side referee client (sends
//                'SYNE' claims through comms_hub_send, reads 'SYNR' with
//                comms_hub_recv(kHubArenaRef), stamps claims with
//                vs_arena_referee_now(), feeds vs_arena_set_ack());
//   A5 EAT1      the held / eaten_by fields and the mouth sanction;
//   A4 / A6      the env below (the launcher's go plan) and the ladder rows.
// A field or a call changes only by a note in out/ARENACORE1/contract.md and
// the lanes that read it agreeing.
//
// ---------------------------------------------------------------------------
// SELECTED ONLY BY ENV (the launcher's go plan, lane ARENALOBBY1):
//   SM64DS_VS_NETMODE=arena   arena mode. Absent, empty or "lockstep" = the
//                             lockstep VS of 0.5.x, untouched (the hidden
//                             fallback for one release). Anything else is
//                             refused loudly and treated as lockstep.
//   SM64DS_VS_MAP=<0..3>      required (a VS boot); arena is never adventure.
//   SM64DS_VS_PLAYERS=<2..4>  the bodies the arena boots with = the room's
//                             capacity; body k belongs to seat k. PHASE A IS
//                             2..4: the ROM fills four slot records, so a seat
//                             of 4 or more never gets its pad. More than 4
//                             players, or a seat pin of 4 or more, prints one
//                             "[vsarena] ERROR ..." line and arena mode is NOT
//                             set up (the process plays as if the env were
//                             absent). B1 widens this to 16.
//   Address, the hub's own knobs (hal/comms_hub.h):
//     SM64DS_COMMS_RELAY + SM64DS_COMMS_CODE   the relay (the real path), or
//     SM64DS_HUB_PORT                          the loopback mesh (tests: no
//                                              referee there).
//   Seat pin, first one present wins: SM64DS_HUB_SEAT, SM64DS_COMMS_SLOT,
//   SM64DS_COMMS_ROLE=parent (seat 0); none = any free seat. So the go plan
//   the launcher already builds (role, slot) pins the seat with no new knob.
//   SM64DS_COMMS_ROLE never opens the VS carrier in arena mode.
//   SM64DS_ADVENTURE is forced to 0 for the process: an arena console is
//   never an adventure console, whatever the player's settings file says.
//
// Lab knobs (off by default): SM64DS_ARENA_LOG=1 one line per SYNS sent and
// received; SM64DS_ARENA_LATENCY=1 the press-to-response readout (works in
// solo, lockstep and arena VS so all three can be compared);
// SM64DS_ARENA_PAIR_MS=<n> how long the level boot waits for a relay seat
// (default 8000).
//
// PORT_HOST_ABI: host plumbing. Nothing in src/ knows this file exists.

#ifndef PORT_HAL_VS_ARENA_H
#define PORT_HAL_VS_ARENA_H

#include <cstddef>

namespace port {

// ---------------------------------------------------------------------------
// 'SYNS' VS STATE v1, 48 bytes, fixed length (VSSYNC1 plan 2.1). One console
// sends it about its OWN body; the relay forwards it to every other seat and
// keeps it as the referee's lag-compensation history. Little endian, packed.
// ---------------------------------------------------------------------------
enum : unsigned { kArenaTagState = 0x534e5953u };   // 'S','Y','N','S'

enum : unsigned char {          // ArenaState.flags
    kArenaLive      = 0x01,     // my body's live byte (data_0209fc5c[seat])
    kArenaGrounded  = 0x02,     // standing on something (mIsAirborne == 0)
    kArenaTeleport  = 0x04,     // a jump no state can make in one frame
                                // (respawn after a pit, a spit): snap, do not
                                // interpolate; vx/vy/vz are 0
    kArenaPaused    = 0x08,     // reserved; v1 senders write 0
    kArenaTalking   = 0x10,     // mic gate open (voice_talking())
    kArenaInHurt    = 0x20,     // St_Hurt / St_HurtWater, or mIsTakingDamage
    kArenaNoControl = 0x40,     // mIsNoControl
    kArenaStarLock  = 0x80,     // in the star collect sequence (set by A3)
};
enum : unsigned char {          // ArenaState.power
    kArenaFire   = 0x01,        // +0x6ce & 0x1000 (InitFireYoshi)
    kArenaWings  = 0x02,        // mHasWings  +0x6ff
    kArenaMetal  = 0x04,        // mIsMetal   +0x6f9
    kArenaVanish = 0x08,        // mIsVanish  +0x6fb
    kArenaMega   = 0x10,        // mIsMega    +0x703
    kArenaNoCap  = 0x20,        // mHasNoCap  +0x71a
    // 0x40, 0x80 reserved (0)
};
enum : int {
    kArenaStateBytes = 48,
    kArenaStates     = 78,      // Player::State objects (state_idx < 78)
    kArenaAnimLimit  = 194,     // anim_id < 194 (the ghost layer's guard)
    kArenaPosLimit   = 32768,   // |x|,|y|,|z| <= 32768 units (Fix12 << 12)
    kArenaSeats      = 16,
    kArenaHeldObject = 0x20,    // held >= 0x20: a non-player object
    kArenaInvulnMax  = 36,      // invuln is clamped here (the referee drops
                                // a SYNS with more)
};

#pragma pack(push, 1)
struct ArenaState {
    unsigned       tag;         // +0  kArenaTagState
    unsigned char  version;     // +4  1
    unsigned char  seat;        // +5  RELAY/CARRIER-STAMPED sender seat
    unsigned char  flags;       // +6  kArena* flags
    unsigned char  character;   // +7  Player+0x6d9 & 7 (3 = Yoshi; a cap
                                //     changes it to the cap's character)
    unsigned       frame;       // +8  the sender's match frame (30 Hz),
                                //     monotonic from its arena start
    int            x, y, z;     // +12 Fix12 (Player+0x5c..)
    short          vx, vy, vz;  // +24 this frame's step in 1/64 unit
                                //     (clamped +-32767 = +-512 units);
                                //     0 with kArenaTeleport
    short          yaw;         // +30 Player+0x8e
    unsigned short anim_id;     // +32 Player+0x63c >> 2, < kArenaAnimLimit
    unsigned short anim_cursor; // +34 animation frame in 1/16 frame (the
                                //     20.12 cursor >> 8)
    unsigned char  state_idx;   // +36 0..77, the Player::State object in ov002
                                //     bss order (0x0210ffec + idx*0x18): see
                                //     out/ARENACORE1/state_table.json.
                                //     Senders never write 0xFF (the referee
                                //     drops it): a body with no state object
                                //     repeats its last. Receivers accept
                                //     0xFF. OBSERVATION ONLY: never applied.
    unsigned char  state_step;  // +37 mStateStep +0x6e3
    unsigned char  invuln;      // +38 mInvincibleTimer +0x6a0, clamped 36
    unsigned char  power;       // +39 kArena* power bits
    unsigned char  held;        // +40 0 none; 1..16 = seat+1 in my mouth;
                                //     kArenaHeldObject = something else
    unsigned char  eaten_by;    // +41 0 free; 1..16 = seat+1 whose mouth
    unsigned short ack;         // +42 newest referee event seq applied (A3)
    unsigned char  sfx[2];      // +44 sound ids my body started this frame
                                //     (0 = none; filled by A1's hook)
    unsigned char  reserved[2]; // +46 0
};
#pragma pack(pop)
static_assert(sizeof(ArenaState) == kArenaStateBytes, "SYNS v1 is 48 bytes");
static_assert(offsetof(ArenaState, frame) == 8, "SYNS frame at 8");
static_assert(offsetof(ArenaState, x) == 12, "SYNS x at 12");
static_assert(offsetof(ArenaState, vx) == 24, "SYNS vx at 24");
static_assert(offsetof(ArenaState, yaw) == 30, "SYNS yaw at 30");
static_assert(offsetof(ArenaState, anim_id) == 32, "SYNS anim at 32");
static_assert(offsetof(ArenaState, state_idx) == 36, "SYNS state at 36");
static_assert(offsetof(ArenaState, held) == 40, "SYNS held at 40");
static_assert(offsetof(ArenaState, ack) == 42, "SYNS ack at 42");
static_assert(offsetof(ArenaState, sfx) == 44, "SYNS sfx at 44");

// The receiver's check of one SYNS (version, ranges). True = keep it. Every
// field a puppet reads is inside its range when this says yes.
bool arena_state_valid(const ArenaState &s);

// SEND RATE (plan 2.2): every frame (30 Hz) while anything a puppet reads
// changed (position, yaw, anim, state, flags, power, held, eaten_by), at once
// on a state or anim change, and every third frame (10 Hz) while unchanged.

// ---------------------------------------------------------------------------
// LIFECYCLE (tests/walk_window.cpp main, hal/level_boot.cpp)
// ---------------------------------------------------------------------------

// The env asks for arena mode (SM64DS_VS_NETMODE=arena with SM64DS_VS_MAP).
bool vs_arena_requested();

// main(), FIRST of the three installs:
//   if (!vs_arena_install_from_env() && !comms_hub_install_from_env())
//       comms_loopback_install_from_env();
// True when arena mode owns the session: the hub was opened in its arena
// shape (or failed to open: arena never falls back to lockstep by itself).
// False for every lockstep VS, adventure and solo run, which then take the
// installs they always took.
bool vs_arena_install_from_env();

// True once installed.
bool vs_arena_active();

// hal/level_boot.cpp, right before the body count is read: waits (bounded,
// SM64DS_ARENA_PAIR_MS) for the relay to name my seat and seats the ROM's
// "my comms slot" (data_020a0f10) with it, so Stage::InitResources' own
// data_0209f250 = func_0203da9c() makes body <seat> mine. No-op outside arena.
void vs_arena_seat_world();

// My seat (= my body's slot), or -1 before vs_arena_seat_world.
int vs_arena_my_seat();

// tests/walk_window.cpp, right before the ROM's PadData builder
// (func_0203bc7c): my pad reaches my slot only, every other slot reads 0.
// The ROM's solo record copy (func_0203e0ac) put my key word in all four.
void vs_arena_route_pads();

// tests/walk_window.cpp, right before the actor walk (phase 4). The puppet
// hold (A1) hangs off this. No-op outside arena.
void vs_arena_before_walk(int frame);

// tests/walk_window.cpp, after the actor tick and the sync layer: sample my
// body and send SYNS, drain received SYNS into the per-seat rings, roster.
void vs_arena_tick(int frame);

// My match frame: the SYNS frame of the last state I sent (0 before any).
unsigned vs_arena_frame();

// ---------------------------------------------------------------------------
// THE RECEIVE SIDE, for the puppets (A1) and the referee client (A3)
// ---------------------------------------------------------------------------
enum : int { kArenaRing = 32 };   // samples kept per seat

struct ArenaSample {
    ArenaState s;          // as received, seat byte = the stamped sender
    unsigned   recv_ms;    // comms_hub_now_ms() when the I/O thread took it
                           // off the socket (after any SM64DS_HUB_DELAY_MS)
};

// Copy up to `cap` samples of `seat`, NEWEST FIRST (strictly decreasing
// frame). Returns how many. 0 for my own seat, a bad seat, or none yet.
int vs_arena_samples(int seat, ArenaSample *out, int cap);

struct ArenaSeatView {
    bool     present;        // the hub hears the seat (any kind, 3 s rule)
    bool     has_state;      // at least one valid SYNS this presence
    unsigned join_epoch;     // the hub's: bumps on every (re)join
    int      rtt_ms;         // peer RTT (hub SYNP), -1 unknown
    unsigned newest_frame;   // newest SYNS frame
    unsigned newest_age_ms;  // how long ago it arrived
    int      clock_offset_ms;// lower envelope of (recv_ms - frame*1000/30)
                             // over the last 8 s: sender frame f is due at
                             // f*1000/30 + offset in comms_hub_now_ms() time
                             // at the fastest delivery seen. A1 draws behind
                             // that. 0 when !has_state.
    unsigned samples;        // valid SYNS taken this presence
    unsigned rejected;       // SYNS dropped by arena_state_valid / order
    bool     body_held;      // its body is parked out of play (absent seat)
};
bool vs_arena_seat_view(int seat, ArenaSeatView *out);

// Bit k = seat k is present (never mine). Bumps on every join or leave.
unsigned vs_arena_roster_mask();
unsigned vs_arena_roster_version();

// ---------------------------------------------------------------------------
// SENDER HOOKS (what other lanes write into my next SYNS)
// ---------------------------------------------------------------------------
void vs_arena_note_sfx(unsigned id);        // A1: a sound my body started
void vs_arena_set_ack(unsigned short seq);  // A3: newest referee event applied
void vs_arena_set_star_lock(bool on);       // A3: kArenaStarLock
void vs_arena_send_now();                   // send on the next tick whatever
                                            // the rate rule says

// The referee clock (plan 4.2): my estimate of the referee's ms right now,
// from the hub's 'SYNP' v2 pings to seat 0xFE (min-RTT sample of the 8
// newest). False until the first referee pong (mesh mode: never).
bool vs_arena_referee_now(unsigned *ref_ms, int *rtt_ms);

// The 0..77 state index of a Player body (0xFF: not one of the 78).
int vs_arena_state_index(const void *body);

// ---------------------------------------------------------------------------
// PUPPETS (A1 PUPPET1). The puppet half lives in vs_arena.cpp's PUPPET section
// and runs from vs_arena_before_walk; the one call another file makes is the
// sound seam: hal/sdat/sound_abi.cpp's flat Sound::Play hands every request to
// port_vs_arena_sound (extern "C"), which lands here. A sound whose vector is
// my own body's Player+0x74 goes out in my next SYNS (sfx[0] = a bank 0
// effect id, sfx[1] = a bank 1 character voice id); the puppets replay them.
// Lab knobs: SM64DS_PUPPET=0 (off), SM64DS_PUPPET_LOG=1, SM64DS_PUPPET_BUF_MS=n.
// ---------------------------------------------------------------------------
void vs_arena_sound_request(unsigned kind, unsigned id, const void *v);

// VSPUP2 (V5): remote character sounds are heard near, fading with distance.
// The distance is measured from MY body (not the camera) with the ROM's own
// metric (x and z as they are, y doubled), in world units; both positions are
// world 20.12 {x, y, z}. Gain is 1.0 up to the near range, 0.0 from the far
// range on, linear between (defaults 90 and 450; SM64DS_PUPPET_SFX_RANGE=
// <near>,<far> or "off"). Party sounds (PSND1) reuse these.
double vs_puppet_sfx_distance(const int me[3], const int src[3]);
float vs_puppet_sfx_gain(const int me[3], const int src[3]);
// MPSOUND1: the rule itself (true = the near..far range is on; near / far in
// units) and the camera-vector length the ROM's own fade needs for a sound
// `dist` units from my body (hal/coop_sound.cpp plays party / coop sounds with it).
bool vs_puppet_sfx_rule(double *near_out, double *far_out);
double vs_puppet_sfx_romdist(double dist);

// ---------------------------------------------------------------------------
// BODIES OF ABSENT SEATS. A seat with no console is taken out of play the way
// the cartridge takes a lost player out (live byte 0, its "lost" bit) and
// parked far from the arena (DROPOUT1's spot rule), so it is nobody's nearest
// player and not on the map. When the seat's console comes back the body is
// put back where it stood and the live byte returns. Who calls what: the
// roster pass in vs_arena_tick does both on its own; A3 reads it for leave
// spills (the stars are the referee's business, not this).
// ---------------------------------------------------------------------------
bool vs_arena_body_held(int seat);

// ---------------------------------------------------------------------------
// COUNTERS + REPORT
// ---------------------------------------------------------------------------
struct ArenaStats {
    bool installed;
    int  seat;
    int  bodies;
    unsigned frame;
    unsigned long long sent, send_refused, recvd, rejected, out_of_order;
    unsigned long long holds, releases;
};
ArenaStats vs_arena_stats();
void vs_arena_report(const char *tag);   // one "[vsarena] ..." line to stderr

}  // namespace port

#endif  // PORT_HAL_VS_ARENA_H
