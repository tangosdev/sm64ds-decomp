// TP TO FRIENDS (coop only). Run mp1, lane COOPTP1.
//
// Tango, 10-05: "In coop mode add a 'TP to friends' toggle that lets you click
// those and then be tp'd into the start of that level synced with them. dont do
// that in party mode."
//
// You click a friend's name in the top-left player list (hal/player_list.h).
// This file marks every friend row that is in a level with a "TP" badge and,
// on a click, warps THIS console to the start of the friend's level and act
// with the ROM's own level change (LoadLevelNoReturn, entrance 0). The normal
// level latch then runs, COOPWORLD1's late-join snapshot makes the level equal
// the group's, and the group key becomes the friend's.
//
// HUB ONLY (COOPTP2): the badge shows and a click is taken only while MY player is in the castle (sublevels
// 1..5: grounds, 1F, courtyard, basement, 2F/top). In a course there is no badge and the click goes on to
// the game. The friend may be in any course and act. Landing is always the level's START (entrance 0).
//
// REFUSALS (one gray chat line each, nothing is queued):
//   * the friend is in a menu / star select / cutscene / not in a level, has
//     only just changed level (presence unsettled), or the level is not
//     mounted in this build;
//   * I am already in the friend's level and act;
//   * I am mid star-get / dialogue / cutscene / pause / fade / level change.
//
// THE TOGGLE "TP to friends". Default ON. The host switches it off with the env
// SM64DS_COOP_TP=0 (a launcher switch comes later and sets the same env); the
// host console sends its setting over SYNB sub-type 0x40 (4 B payload, every
// 3 s and on a change), so every console follows it. A console that never
// hears the host uses its own env. With the toggle off there is no badge and a
// click does nothing.
//
// KNOBS (env). SM64DS_COOP_TP_LOG=1 prints "[cotp]" lines.
// SM64DS_COOP_TP_FAKE=<seat>@<tick> clicks a row from inside the frame loop
// (a proof knob, compiled out of a PORT_ROM_CLEAN build).
#ifndef PORT_HAL_COOP_TP_H
#define PORT_HAL_COOP_TP_H

namespace port {

struct CoopTpStats {
    unsigned long long clicks, warps, refused, toggle_rx;
};
CoopTpStats coop_tp_stats();

// True while TP is on for this console (coop room, toggle on).
bool coop_tp_enabled();

// Where leaving a course puts the player: the castle sublevel (1..5) holding that course's painting and the
// entrance number inside it. False for a level no castle door leads to. (REJOIN1 reads this too.)
bool coop_castle_entrance_for_course(int course, int *castle_sublevel, int *return_entrance);

}  // namespace port

#endif
