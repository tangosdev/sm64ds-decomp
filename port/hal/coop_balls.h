// COOP: THE ROLLING IRON BALLS A SPAWNER MAKES, SHARED. Run coopfix1, lane BOBSYNC1 (Tango's bug 15).
//
// A ball spawner (daIbl_c kind 0, class 220, e.g. the two at the top of Bob-omb Battlefield's hill) makes a new ball every
// 0x7f frames while the closest player is in its band. The balls it makes are runtime actors (death-table slot -1), so the
// mover sync (hal/coop_actors.cpp) never saw them: every console rolled its own balls from its own timer, and a pause, a
// late level load or a slow frame left the screens seconds apart for good.
//
// The rule now: the console that owns the SPAWNER (the mover owner of its placed slot, the nearest player's console) is the
// only one whose spawner makes balls. Each ball it makes gets a virtual mover slot (kBallSlot0 + spawner index * 6 + ring
// index), so the mover sync publishes its pose like any placed enemy. A friend's spawner is held (its timer never reaches 0);
// when the owner's first record of a new ball arrives (still at the spawner), the friend spawns its own copy there the same
// way the spawner does (same param, place, facing, area, the spawner's ball count) and the pin keeps it on the owner's ball.
//
// MIXED ROOMS: a console that does not share (an older build) never sends a record for these slots. A held spawner whose
// owner has sent none while the spawn test passed twice here stops being held for that owner, so the friend rolls its own
// balls as before. An older build ignores the records for these slots (no actor of its own sits there).
//
// INERT outside a coop room. SM64DS_COOP_BALLS=0 is the control (the old behaviour: every console rolls its own).
// SM64DS_COOP_BALLS_LOG=1 prints [cball] lines. SM64DS_COOP_BALLS_SAMPLE=N prints a [ballpos] line every N frames with every
// rolling ball and every Bob-omb on this console (proof rows compare two consoles' lines by wall time).

#ifndef PORT_HAL_COOP_BALLS_H
#define PORT_HAL_COOP_BALLS_H

namespace port {

// The virtual mover slot of a spawned ball, or -1 (not shared: a placed actor, another class, or a ball no owner announced).
int coop_ball_virtual_slot(const void *actor);
// Who steers the ball in virtual slot `slot`: the seat that owns its spawner; -1 when `slot` is not a ball slot.
int coop_ball_claim_seat(int slot);
// Once a frame, from the mover sync's frame (after its records were applied).
void coop_balls_frame(unsigned now_ms);

}  // namespace port

#endif  // PORT_HAL_COOP_BALLS_H
