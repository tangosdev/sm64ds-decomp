// COOP RABBIT HOLD (run coopfix1, lane RABBIT2): a friend's held rabbit is held on my screen, his caught rabbit is caught
// here too. Design and evidence: coop_rabbit.cpp. INERT outside a coop room; SM64DS_COOP_RABBIT_HOLD=0 switches it off.
#ifndef PORT_HAL_COOP_RABBIT_H
#define PORT_HAL_COOP_RABBIT_H

namespace port {

// Once a frame, right after party_hits pinned the objects friends hold to their drawn hands (comms_sync's ghost pass).
void coop_rabbit_after_follow();

// coop_actors.cpp ownership: the seat whose hands hold this rabbit (mine when my body carries it), or -1 for the usual
// proximity rule. `a` is the local actor in coop sync slot `slot`; any class other than the rabbit (187) gives -1.
int coop_rabbit_owner(int slot, const char *a);

// party_hits' held message: true when the object in my hands is a rabbit the ROM already let go (Released, +0x45c clear)
// while my body's held pointer still names it for the put-down animation. Lets the friend's copy go the same frame mine
// does instead of about 13 frames later. False for anything else, outside a coop room, or with the kill switch.
bool coop_rabbit_let_go(const char *ob);

}  // namespace port

#endif  // PORT_HAL_COOP_RABBIT_H
