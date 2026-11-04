// PARTY HITS (lane PLAYINT1): friends affect each other in party and coop rooms. Punch / kick / spin /
// dive = shove + stun; ground pound landing = bigger shove; jump on a friend's head = bounce + squash;
// Yoshi tongue = a short hold in the mouth and a spit. No health loss unless the room has damage on.
// The rules are in hal/party_hits_core.h (pure, host-testable); this file is the game side.
// Design, wire, hooks: cloud_tasks/PLAYINT1_DESIGN.md. Nothing in src/ knows this exists.
//
// Room toggles (the launcher exports them from the lobby plan):
//   SM64DS_ROOM_INTERACT=0/1  (default 1)  all of it
//   SM64DS_ROOM_DAMAGE=0/1    (default 0)  a believed hit also costs one health unit
// Lab knobs (off by default): SM64DS_PHIT_LOG=1|2, SM64DS_PHIT_LAB=forge:<melee|pound|stomp|eat>:<units>,
//   and the must-fail switches SM64DS_PHIT_NOREACH / NOLEVEL / NOCOOLDOWN / NOPOS / NOBUSY = 1.
#ifndef PORT_HAL_PARTY_HITS_H
#define PORT_HAL_PARTY_HITS_H

namespace port {
namespace party_hits {

// This console is in a party / coop room AND SM64DS_ROOM_INTERACT is on.
bool active();

// SYNB sub-type the coop transport uses. 0x40 is COOPTP's toggle (coop_tp.cpp), so this is 0x41; it is outside the
// ranges W56 claims (0x00-0x3f) and is registered with coop_ctl_register, which refuses an overlapping range.
enum : int { kCoopCtlSubtype = 0x41 };

// Coop transport: coop_world's SYNB dispatcher forwards a received SYNB with this sub-type here
// (`seat` = the relay-stamped sender, `d` = the payload after the 12-byte head, `len` its length).
void on_coop_ctl(int seat, const unsigned char *d, int len);

// Cosmetic: the y-scale (Fix12, 0x1000 = normal) the follower should give the friend's body on `seat` for
// the squash after a stomp I landed. Returns 0x1000 when none is running. Optional hook.
int squash_scale_for_seat(int seat);

// INTERACT9: the attach rule. While the friend on `victim_seat` is in somebody's mouth (kind 1) or hands (kind 2)
// the follower draws his body at the holder (comms_sync.cpp gw_follow). Returns the holder's seat, or -1.
int attached_holder(int victim_seat, int *kind);
// Units ahead of the holder's yaw and up from its feet for the attach kind.
void attach_offsets(int kind, int *ahead, int *up);
// TONGUE1: after the follower posed the ghosts, put the objects friends hold at their drawn hands / mouth (kHeldState).
void held_after_follow();

struct Stats {
    unsigned long long claims_sent, claims_recv, applied, refused, bounces, eats_began, eats_spat, eats_aborted;
    unsigned long long refused_by[16];
};
const Stats &stats();

}  // namespace party_hits
}  // namespace port

// Once a frame, right after port_party_stars_frame (hub queues just pumped, presences fresh).
extern "C" void port_party_hits_frame(void);

#endif  // PORT_HAL_PARTY_HITS_H
