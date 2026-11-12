// DOORKEY1 (run coopfix1, bug 12): a friend's key-door unlock as this screen shows it. See coop_doorkey.cpp.
#ifndef PORT_HAL_COOP_DOORKEY_H
#define PORT_HAL_COOP_DOORKEY_H

namespace port {

// A friend's key-door kick arrived (coop_switch: DOOR, state 2, a live message from `seat`, not a snapshot). `idx` is the door's
// table index (param1). Builds the key prop of the unlock on this screen and remembers where the ROM put the friend.
void coop_doorkey_friend_unlock(int seat, int idx);

// Once per coop group frame (coop_switch's frame, armed): proof knobs and the probe. `frames` is the group frame count.
void coop_doorkey_frame(unsigned frames);

// The follower asks: is seat `seat` in a key-door unlock here, and where did the ROM stand him? True with the spot (Fix12) and
// facing while the unlock lasts on this screen; the follower draws him there instead of the sampled place.
bool coop_doorkey_hold(int seat, int pos[3], short *yaw);

// DOORKEY1 kill switches, SM64DS_DOORKEY_OFF=<list>: "leaf" (the watcher drives both leaves of a double door, as 1.0.3),
// "key" (no key prop for a friend's unlock), "snap" (the friend is drawn where his frames say during the unlock).
bool coop_doorkey_off(const char *what);

}  // namespace port

#endif  // PORT_HAL_COOP_DOORKEY_H
