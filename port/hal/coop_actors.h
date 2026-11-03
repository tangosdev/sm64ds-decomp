// COOP PHASE 2: LIVE ENEMIES AND MOVING PLATFORMS KEPT IN STEP. Run mp1, lane COOPMOVE1.
//
// Phase 1 (coop_world.*) keeps the death table equal. This file keeps the things
// that MOVE equal: path lifts, rotators, swingers, and the enemies that walk
// around. The design is out\COOPSCOUT2\PLAN.md sections 1, 3 and 7; the short
// version is below.
//
// WHAT IS MEASURED. Every placed actor is bit-identical on two consoles at the same
// LEVEL AGE; at the same wall time a late joiner is 300 to 1000 units off because
// each mover counts its own ticks from its own init. So the cure is a PHASE write
// (the mover's own counters plus its position) and a slow nudge, not a simulation.
//
// HOW IT WORKS (all per placed actor, keyed by its death-table slot, the same on
// every console for one (level, act), cross-checked with the class id):
//   * Every console runs every actor's own Behavior. After the actor frame the
//     console that OWNS an actor (the group member whose player is closest to it,
//     1.5x hysteresis, lower seat on a tie) publishes {pos, angles, class fields}.
//   * The others compare: inside the class's band they do nothing; inside its snap
//     radius they ease 25 percent per packet; past it (and for classes with phase
//     fields, on every record) they write the owner's values, so the next Behavior
//     integrates from the owner's state.
//   * A late joiner asks the group for a snapshot after its objects spawned
//     (SYNB MOVER_SNAP_REQ); the members that own actors answer with the same records
//     flagged "snap", which are applied unconditionally within a few frames.
//
// WIRE (PORT_HOST_ABI; layouts below, ranges fixed by PLAN 7.0):
//   SYNW 0x10 MOVER_BATCH   records, cap 20 pps per seat from this file
//   SYNB 0x06 MOVER_SNAP_REQ  key + nonce
// Every received byte is range-checked; an unknown class, a wrong field count, a
// slot out of range, a position far outside the level, a short or long datagram or
// an unknown profile version is dropped and counted, never applied.
//
// INERT unless SM64DS_COOP is set AND the hub is live in a coop room.
//
// ENVIRONMENT (proof knobs are inert unless named):
//   SM64DS_COOP_ACT=0        MUST-FAIL control: phase 2 off (publish nothing, apply nothing)
//   SM64DS_COOP_MOVE_SNAP=0  control: no snapshot request, snapshot records ignored
//   SM64DS_COOP_MOVE_HYST=0  control: owner switches at once (no 1.5x margin, no dwell)
//   SM64DS_COOP_MOVE_STALE=0 control: a silent owner is never replaced
//   SM64DS_COOP_MOVE_LOG=1   [coopmove] lines
//   SM64DS_COOP_MOVE_SAMPLE=N  every N ticks one [mvs] line: wall ms + every synced actor pose
//   SM64DS_COOP_MOVE_DUMP=a,b  at ticks a,b after the objects spawned dump every synced actor's
//                            words 0xd0..0x480 ([mvd] lines; the phase-field discovery)
//   SM64DS_COOP_MOVE_PROFILE=<file>  extra / overriding class rows (see coop_actors.cpp)
//   SM64DS_COOP_MOVE_FORGE=F:name  at tick F send one forged datagram
//                            (cls slot far short long nf pver key bits)

#ifndef PORT_HAL_COOP_ACTORS_H
#define PORT_HAL_COOP_ACTORS_H

#include "coop_wire.h"

#include <cstdint>

namespace port {

// SYNB / SYNW sub-types (PLAN 7.0: SYNB 0x06-0x0b and SYNW 0x10-0x1f are this lane's).
enum : uint8_t {
    kCtlMoverSnapReq  = 0x06,
    kWorldMoverBatch  = 0x10,
};

enum : int {
    kMoverMaxFields   = 12,
    kMoverMaxRecs     = 40,
    kMoverProfileVer  = 1,
    kMoverMaxUnits    = 40000,    // |coordinate| in world units
};

#pragma pack(push, 1)
struct MoverBatchHead {        // 8 B
    CoopKeyWire key;
    uint8_t flags;             // bit0 = snapshot reply
    uint8_t count;             // 1..kMoverMaxRecs
    uint8_t pver;              // kMoverProfileVer
    uint8_t reserved;          // 0
};
struct MoverRec {              // 26 B, then nf * u32
    uint16_t slot;             // death-table slot 0..511
    uint16_t cls;              // actor class id (cross-check)
    uint16_t cd;               // the sender's distance to its nearest player, world units (ownership claim)
    int32_t  x, y, z;          // Fix12 raw
    int16_t  ax, ay, az;
    uint8_t  rflags;           // bit0 = the sender's copy is frozen / culled
    uint8_t  nf;               // must equal the class profile's field count
};
struct MoverSnapReq {          // 8 B
    CoopKeyWire key;
    uint32_t nonce;
};
#pragma pack(pop)

static_assert(sizeof(MoverBatchHead) == 8, "mover head is 8 bytes");
static_assert(sizeof(MoverRec) == 26, "mover record is 26 bytes");
static_assert(sizeof(MoverSnapReq) == 8, "snap request is 8 bytes");

struct CoopMoveStats {
    unsigned long long ticks, ents, batches_sent, recs_sent, snap_batches_sent, snap_recs_sent, req_sent, req_served;
    unsigned long long batches_rx, recs_rx, applied_write, applied_ease, in_band, skipped_state, cls_mismatch,
                       snap_applied, no_ent, owner_changes, owner_stale, throttled, slaved;
    unsigned long long drop_bad, drop_group, drop_member, drop_ver, drop_far, drop_off, drop_seat;
    unsigned long long drop_val, rows_dropped, write_refused, size_mismatch, overrun;   // size / value guard (COOPMOVE1_FIX)
};
CoopMoveStats coop_move_stats();

// MOVERS9: per-class sync numbers, one row per class that was seen. pin_* = how far (world units) the slave pin had to move the
// local copy at each pin: the per-class desync number (0 = the local Behavior agrees with the owner). Written to the log by
// coop_stats_log.cpp.
struct CoopClassStat {
    unsigned id; const char *name;
    unsigned ents_max, pub, applied, slaved, owner_ch, pin_n, pin_big, held_pins;
    double pin_avg, pin_max;
};
int coop_move_class_stats(CoopClassStat *out, int max);
// TONGUE1: true when the actor in spawn slot `slot` of class `cls` is synced here and THIS console owns it (it simulates
// and publishes its pose). A friend's held-object pin must leave such an actor alone.
bool coop_move_i_own(int slot, unsigned cls);
// TONGUE1: the held-object pin moved my (not owned) copy in `slot` this tick: it is not published this tick or the next,
// so a flip of ownership to me never sends the pinned / parked pose. `stay`: the hold ended with my copy parked out of
// sight, so it stays unpublished until an owner's pose lands on it. coop_move_pin_skips counts the publishes held back.
void coop_move_pinned(int slot, bool stay = false);
// TONGUEFIX2: the held-object pin put my parked copy in `slot` back on a real pose: coop sync may publish it again at once.
void coop_move_unpin(int slot);
// TONGUEFIX2: true while my copy in `slot` is still the parked copy a finished hold left (no owner's pose has landed on it).
bool coop_move_pin_stays(int slot);
unsigned coop_move_pin_skips();

// Validators (payload = after the 12-byte head). Exposed for the forged-input rows.
bool coop_mover_batch_validate(const void *payload, int len);
bool coop_mover_req_validate(const void *payload, int len);

}  // namespace port

#endif  // PORT_HAL_COOP_ACTORS_H
