// POSE9 (run mp1, coop9c wave 2): the Yoshi tongue on a remote body.
//
// The tongue is the HEAD model's animation, not the body's: the ROM's
// func_ov002_020d71ec sets the body anim (two ids from a table in the overlay,
// and 0x6c for the pull back) and the head model's own animation together, and
// nothing else ever selects it. A body whose Behavior is skipped (a VS puppet,
// an adventure / party / coop / world ghost) is posed from the wire, so when the
// wire says a tongue anim the follower makes the ROM's call once and then
// follows the body frame with the head frame. Leaving the anims puts the head
// back (func_ov002_020d71a0). The VS puppet (vs_arena.cpp) used to own this
// block; both layers call the one copy here.
#ifndef PORT_HAL_GHOST_TONGUE_H
#define PORT_HAL_GHOST_TONGUE_H

namespace port {
namespace ghost_tongue {

struct State {
    int arg = -1;                       // 0 / 1 / 2 while the head plays the tongue, else -1
    unsigned long long starts = 0;      // ROM calls made
    unsigned long long frames = 0;      // head frames set
    unsigned long long stops = 0;       // head put back
};

// The ids and the head files the ROM call reads exist here (checked once).
bool ready();
// 0 / 1 / 2 for the two tongue anims and the pull back, -1 for anything else.
int arg_for(unsigned anim);
// Start / stop the head animation to match `anim`. Returns the new arg; sets
// *started when the ROM call was made on this call. `body` must be a Yoshi.
int select(State &st, char *body, unsigned anim, bool is_yoshi, bool *started);
// After the body frame is set: put the head on the same frame (wrapped by its
// own length). Returns the head frame, -1 when nothing was posed.
int follow(State &st, char *body, int want_f, int *head_frames);
// The pull-out body anim id (the tongue ROM table entry, 0x6c); for the proof lab.
unsigned retract_anim();
// The body is not a Yoshi head any more / was rebuilt: forget the state
// without touching the model.
void forget(State &st);

// POSE9 item 20: THE INVINCIBILITY BLINK ON A REMOTE BODY. The ROM arms
// mInvincibleTimer (Player+0x6a0) = 0x24 for a hurt player and counts it down in
// Player::Behavior (src/_ZN6Player8BehaviorEv.cpp:124); Player::Render hides the
// body on frames the timer is odd (src/_ZN6Player6RenderEv.cpp:50-53). A ghost's
// Behavior is skipped, so nothing arms or counts it. The wire has no blink flag:
// the watcher starts a local 36-frame blink when the ghost's animation turns to
// one of the hurt animations (the ROM's own table, Player::St_Hurt_Init's
// data_ov002_0210a6d4[mStateStep & 7]).
struct Blink {
    unsigned prev_anim = 0xffffffffu;
    bool     prev_hurt = false;
    int      left = 0;                  // frames of blink still to show
    unsigned long long starts = 0, shown = 0, ends = 0;
};
bool is_hurt_anim(unsigned anim);
// One call per follower pass. Returns the value to write into Player+0x6a0
// (0 = no blink); *started is set on the pass that arms a new blink.
// FRIENDLOOK1: the count is armed again (36) when the hurt animation ends, as the
// ROM arms it then for a hurt that took health; end_rearm = false skips that for a
// hurt known to carry none.
unsigned blink_step(Blink &b, unsigned anim, bool *started, bool end_rearm = true);
void blink_forget(Blink &b);

}  // namespace ghost_tongue
}  // namespace port

#endif
