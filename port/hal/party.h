// PARTY BODIES (run mp1 wave 3, lane PBODY1). In a party room the friends who are
// in my level AND my area are solid, opaque bodies that push me and that I push;
// everyone else stays what the adventure hub made them (a see-through ghost, or
// nothing). Nothing in src/ knows this exists (PORT_HOST_ABI).
//
// HOW IT RIDES THE HUB. A party member is a hub SEAT. The hub follower
// (hal/comms_sync.cpp, gw_follow) already poses, places and draws the body of
// every visible seat from the SYNG ring and parks the body between draws; for a
// party seat that is in my level and area it calls party::follow() after the pose,
// which (1) marks the body solid for the render pass (hal/player_bridges.cpp draws a
// solid body without the ghost layer) and (2) registers the body's collision
// cylinder, so the ROM's own pair pass pushes my body out of it. Nothing else of the
// friend's body ever runs on my console: its Behavior is skipped, it is parked 200000
// units up from the end of the draw to the next follower pass (so no enemy picks it,
// no trigger that scans bodies by position sees it), and the cylinder is in the
// collision list only for the pair pass of the frame it is drawn.
//
// THE CYLINDER AND WHY IT CANNOT COLLECT A COIN (hal/party_cyl.cpp, SPIKE.md):
// dCc_c::Process pairs two cylinders only when (A.flags & B.vuln) | (A.vuln &
// B.flags) is non-zero. The party cylinder carries flags 0 and vuln 0x400000 (the
// "player" bit), so it pairs with a body cylinder (flags 0x400008) and with nothing
// the world owns, and every cylinder it does pair with is handed hitFlags 0. A
// runtime audit walks the live collision list before each registration and drops
// the cylinder for that frame if any non-player node would pair with it.
#ifndef PORT_HAL_PARTY_H
#define PORT_HAL_PARTY_H

namespace port {
namespace party {

// SM64DS_PARTY set and non-zero: this console is in a party room.
bool active();

// Membership is by hub SEAT (0..15), never by body index: the body a seat is
// shown on changes (gw_assign), the seat does not. The local seat is never a
// party seat. SM64DS_PARTY_MEMBERS="1,2" (a lab knob) names the party SEATS and
// leaves the rest as adventure ghosts; unset, every other seat is a member.
bool seat_is_party(int seat);

// Whether the friend on `seat` may have a body on this console right now, from his
// newest SYNL against what this console is showing.
enum Gate : int {
    kGateOk = 0,      // same level, same area (or no word from him yet)
    kGateHide = 1,    // he is in a cutscene / has no level: no body at all
    kGateLevel = 2,   // another level
    kGateArea = 3,    // another area (room) of this level
    kGatePaused = 4,  // my level and area, but in a menu (the pause menu): his body stays where he paused,
                      // drawn but not solid. SM64DS_COOP_DRAWGATE=0 folds it back into kGateHide.
};
int gate(int seat);

// Run-time membership (in-world party invites): seat `seat` becomes a solid party body (on) or goes back to what the
// hub made it (off). Same per-seat rule as the SM64DS_PARTY_MEMBERS knob; the env path keeps working. active() is true
// while any seat is a member. Main thread only.
void set_member(int seat, bool on);
bool is_member(int seat);

// Before the follower places body `a` for a party seat: the things a body that stopped running at
// its level entry can still carry that would keep it from being drawn where its friend stands.
void prepare(void *a);
// Log-level 2 only: where the party body is and what the draw reads, once a second (the push line's twin).
void draw_diag(void *a);

// Per follower pass: forget that body `body` was solid last frame.
void clear(int body);

// The follower drew party seat `seat` on body `body` (`a`), fully shown and in my
// level and area. Marks it solid and registers its cylinder for this frame's pair
// pass. Returns false when the cylinder was withheld (the audit found a pairing with
// something that is not a player body); the body is still drawn solid.
bool follow(void *a, int body, int seat);

// Is body `body` a solid party body in this frame's draw (hal/player_bridges.cpp
// and tests/walk_window.cpp ask through port_party_member).
bool body_solid(int body);

struct Stats {
    unsigned long long solid_frames;    // bodies marked solid, summed over frames
    unsigned long long registers;       // cylinders put in the collision list
    unsigned long long guard_skips;     // frames a cylinder was withheld by the audit
    unsigned long long unstick;         // times my body stepped off a stack of friends (FIX9A)
    unsigned long long assists;         // frames my body took the second half of a push
    unsigned long long detached;        // bodies that were still attached to an actor (+0x35c) and were let go
    unsigned long long gate_hide, gate_level, gate_area;
    unsigned long long audit_nodes;     // list nodes seen when the follower runs (0: the pair pass already ran)
    unsigned long long pairs_body;      // frames the party cylinder was paired with a body cylinder
    unsigned long long pairs_other, shared_nodes;   // frames it was paired with anything else (must stay 0)
};
const Stats &stats();
Stats &stats_rw();            // for hal/party_cyl.cpp
int  log_level();             // SM64DS_PARTY_LOG

// --- hal/party_cyl.cpp ------------------------------------------------------
// PHIT1: extra vulnerability bits for the party cylinder (the attack kinds a shove can be claimed
// by). Zero until a lane that reads the party node's hit flags asks for more; the audit counts any
// pairing with a non-player cylinder that results.
void set_extra_vuln(unsigned bits);
// Put body `a`'s cylinder in the collision list for this frame's pair pass.
// Returns false when the audit withheld it.
bool cyl_register(void *a, int seat);
// SHOVE9: take body `a`'s cylinders out of the collision list for this frame (no push on my body from this friend).
void cyl_withhold(void *a);
// The other half of the push (my body takes what the friend's body would have), before the
// node is registered again.
void cyl_assist(void *a);
// The audit: what the last pair pass left on the party node (1 = it paired with
// a cylinder that is not a body's). Runs before each registration.
int cyl_audit(void *a);
// SM64DS_PARTY_LAB: the spike's sweep (moves the first solid body over every actor
// of the level in turn and reports what changed). Called by follow(); a no-op
// without the knob.
void lab_before_register(void *a, int body);
// The lab's per-pass clock (party::clear calls it): steps the sweep, samples the world.
void lab_tick(int body);

}  // namespace party
}  // namespace port

// The old per-slot face (hal/player_bridges.cpp, tests/walk_window.cpp): 1 when
// body `slot` is a solid party body in this frame's draw.
extern "C" int port_party_member(int slot);
extern "C" void party_set_member(int seat, bool on);
extern "C" bool party_is_member(int seat);
extern "C" bool party_active();

#endif  // PORT_HAL_PARTY_H
