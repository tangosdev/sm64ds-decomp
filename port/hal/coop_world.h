// COOP WORLD CORE. Run mp1 wave 56, lane COOPWORLD1.
//
// Phase 1 of "a private world with full synced levels": consoles standing in
// the same (sublevel, act) form a group; the game's own "already killed or
// collected" list (the death table, data_0209f4f8) is kept equal across the
// group, live and for a late joiner. Wire layouts: hal/coop_wire.h. Group and
// owner rules: hal/coop_groups.h. Notes for the next phases:
// out of the repo, in the lane's PHASE2_NOTES.
//
// INERT unless SM64DS_COOP is set AND the hub is live in a coop room
// (SM64DS_ROOM_KIND=coop). A world, party or VS launch never reaches any of it.
//
// ENVIRONMENT (all optional, proof knobs are inert unless named):
//   SM64DS_COOP=1            the world core on
//   SM64DS_COOP_LOG=1        [coop] / [coopbits] lines
//   SM64DS_COOP_KILL=F:ID:N[;..]   at F frames after the group state is held,
//                            kill N placed actors of class ID the ROM's own way
//                            (KillAndTrackInDeathTable), lowest slots first
//   SM64DS_COOP_GOTO=F:ID    at frame F put the local player on the first live
//                            actor of class ID (so the ROM's own collect runs)
//   SM64DS_COOP_FORGE=F:name at frame F send one forged datagram (oob, group,
//                            count, snapbad, sub, trunc, snapgroup)
//   SM64DS_COOP_SYNC=0       must-fail control: apply nothing received

#ifndef PORT_HAL_COOP_WORLD_H
#define PORT_HAL_COOP_WORLD_H

namespace port {

struct CoopWorldStats {
    unsigned long long sent_hello, sent_enter, sent_snap, sent_ack, sent_owner,
                       sent_delta, sent_full;
    unsigned long long recv_hello, recv_enter, recv_snap, recv_ack, recv_owner,
                       recv_delta, recv_full;
    unsigned long long bits_applied, reconcile_kills, snaps_served,
                       snaps_applied, first_in, snap_timeouts, local_bits;
    unsigned long long drop_bad, drop_group, drop_target, drop_nonce,
                       drop_unknown_sub, drop_not_coop, drop_seat,
                       drop_sync_off, drop_trust;
    unsigned long long star_collects, local_clears, out_resends;
};
CoopWorldStats coop_world_stats();

// STEP-0 API for the phase-2 lanes (run mp1, COOPINT1). All inert (false / -1 / 0) outside a coop room.
// SEAMCOOP1: the room's state back to a process that never was in one (a coop room left or entered in place: hal/coop_boot.cpp).
void coop_world_session_reset();
bool coop_group_key(int *level, int *act);     // my current group's sublevel and act
bool coop_group_established();                 // I hold the group's state (snapshot taken, level live)
int  coop_group_owner();                       // owner seat, -1 none
bool coop_group_i_am_owner();
// AREACOIN1: a move from one part of a course to another part of the same course and act, where the cartridge keeps its
// death table, coin count and red coin count (dScStage_c::InitResources only clears them on a course change)
bool coop_world_course_move(int from_level, int from_act, int to_level, int to_act);
unsigned coop_group_member_mask();             // other consoles in my group, bit = seat
// Callback registries (up to 8 each; false when full, null or already added).
bool coop_on_frame(void (*fn)(unsigned now_ms));                    // each frame, after the presence refresh
bool coop_on_group_latched(void (*fn)(int level, int act));         // end of the level latch, before any object spawns; fires just BEFORE established
bool coop_on_group_established(void (*fn)(int level, int act));     // snapshot taken, level live
bool coop_on_snapshot_request(void (*fn)(int seat, unsigned nonce));// owner side: a LEVEL_ENTER arrived and was answered
bool coop_on_member_gone(void (*fn)(int seat));                     // a member left my group


}  // namespace port

// The two hook lines: once a frame (tests/walk_window.cpp) and once per level
// latch (hal/level_change.cpp, the end of port_level_latch: the death table has
// been cleared by the ROM's own rule, the level and act are the new ones, and no
// object has spawned yet).
extern "C" void port_coop_world_frame(void);
extern "C" void port_coop_world_level_latched(void);

#endif  // PORT_HAL_COOP_WORLD_H
