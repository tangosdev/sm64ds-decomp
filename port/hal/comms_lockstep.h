// THE LOCKSTEP DRIVER. Run mg16, lane MP2.
//
// WHAT THIS IS, SAID PLAINLY FIRST: it is a TRANSCRIPTION of the ROM's own
// lockstep, not the ROM's lockstep. src/func_0203ea5c.c -- the real thing --
// is still in no slice, so this file drives the eight seam faces in the order
// that TU drives them and moves the same bytes to the same offsets, with every
// step carrying the line it came from. It exists because the alternative was
// to invent an order, and an invented order proves nothing about a contract.
//
// WHY THE REAL TU IS NOT LINKED, measured rather than assumed:
//
//   src/func_0203ea5c.c is 489 lines and calls EIGHT non-seam functions.
//   func_0204271c, func_02042748 and func_0203fd64 have no slice row and no
//   host face anywhere in the tree; func_0203f604 and func_0203f644 are
//   address-taken (passed to func_02040c34) so they must still resolve at
//   link, and today they exist only as prose. src/func_0203df40.c, the
//   dispatcher that owns the ONLY call site of the seam's close() face
//   (func_0203df40.c:70), is also in no slice and pulls func_0203b9bc.
//
//   And TWO signature defects go live the day it links, not the one the seam
//   header names. comms_seam.h calls out the func_02040a5c arity. The second
//   is this lane's: src/func_0203df40.c:21 declares func_0203ea5c as `int`
//   and src/func_0203ea5c.c:82 defines it `void`.
//
// So linking it is a lane of its own with an unbounded closure, and MP2 says
// so out loud rather than half-doing it. THE CONSEQUENCE IS HONEST AND IT IS
// THE HEADLINE OF THIS LANE'S REPORT: the seam contract still has not been
// driven by its real caller, so it CANNOT be fully frozen at the end of MP2.
// What it can be, and what comms_loopback.cpp writes up, is frozen against a
// transport that ran the ROM's real ORDER over a real socket.
//
// WHERE IT SITS. Exactly where src/func_0203df40.c's switch sits: this is the
// role-1/role-2 arm, and func_0203e0ac (already linked, slice_gate13) is the
// role-0 arm the port already runs. comms_lockstep_tick() returns false when
// the caller should run func_0203e0ac() instead, which is the switch.

#ifndef PORT_HAL_COMMS_LOCKSTEP_H
#define PORT_HAL_COMMS_LOCKSTEP_H

namespace port {

// One frame of the ROM's lockstep.
//
//   true  -- the round ran. data_020a1154's four records now hold this frame's
//            per-player state and the caller should NOT run func_0203e0ac.
//   false -- no transport, not connected yet, or the wait bound expired and the
//            session dropped to solo the way src/func_0203ea5c.c:487 drops it.
//            The caller runs func_0203e0ac(), the ROM's own role-0 arm.
//
// Safe to call with no transport installed: it answers false immediately and
// touches nothing, which is what keeps every knob-off path untouched.
bool comms_lockstep_tick();

// Counters for the proof ladder. Distinct from port::comms_readout()'s
// exchanges/rounds, which count seam CALLS: these count lockstep FRAMES.
struct CommsLockstepStats {
    unsigned long long ticks;        // comms_lockstep_tick() calls that ran
    unsigned long long rounds;       // of those, the ones that completed
    unsigned long long timeouts;     // bound expiries, i.e. drops to solo
    unsigned long long spins;        // total turns of the :157 wait loop
    unsigned long long peer_updates; // per-slot record writes past the 0x8000
                                     // live-bit gate at :278
};
CommsLockstepStats comms_lockstep_stats();
void comms_lockstep_report(const char *tag);

}  // namespace port

#endif  // PORT_HAL_COMMS_LOCKSTEP_H
