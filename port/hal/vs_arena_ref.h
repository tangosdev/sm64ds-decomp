// ARENA MODE, the referee client (the game side of the referee in the relay).
// Run mp1, lane STARREF1 (plan A3 of runs/mp1/out/VSSYNC1/plan.txt).
//
// The referee (port/tools/relay/referee.py, lane REFEREE1) owns everything two
// players can disagree about: which star wakes next, who broke a marker, who got
// a contested star, whether a hit landed, the score, the clock and the winner.
// This file is the console's half of that conversation. It
//   * sends READY, then claims (SYNE): marker break, grab, hit, hazard hurt, pit,
//     quit; resends each claim every 60 ms until a verdict names it;
//   * reads the referee's event stream (SYNR) exactly once and in order, acks it
//     through vs_arena_set_ack, and turns every event into what the ROM would
//     have done: the star order, the marker glass, the star that pops and is
//     taken, the dropped star, the victim's own Player::Hurt, the ledger in
//     data_0209f310, the match clock's start and the match's end;
//   * keeps the ROM's own star actors, markers, bodies and HUD as the picture,
//     and takes from them only what the referee must decide.
//
// WIRE: port/tools/relay/arena_wire.h (generated from arena_contract.json).
// CORE:  hal/vs_arena.h (the hub, SYNS, the referee clock, the seat).
//
// PORT_HOST_ABI: host plumbing; nothing in src/ knows this file exists. Every
// entry is inert unless arena mode is installed AND the hub is in relay mode
// (the loopback mesh has no referee: a mesh row plays the arena world without
// a referee exactly as ARENACORE1's rows do).
//
// WHO CALLS WHAT (all from hal/star_flow.cpp, the arena fences):
//   vs_ref_pre_walk(frame)    port_vs_match_end_hold, before the actor walk
//   vs_ref_post_walk(frame)   port_vs_match_end_poll, after the walk and before
//                             vs_arena_tick sends this frame's SYNS
//   vs_ref_countdown(...)     the countdown drive, port_luigi_countdown_drive's
//                             seat (the referee's T0 owns the READY/3/2/1/GO)
//   vs_ref_match_over(...)    the end poll: the only way an arena match ends
//
// FOR A5 (EAT1, the eat half of this file): vs_ref_set_event_sink() hands every
// applied stream event (EATEN / RELEASED and the rest) to a callback;
// vs_ref_claim() sends any claim kind and tracks its verdict.

#ifndef PORT_HAL_VS_ARENA_REF_H
#define PORT_HAL_VS_ARENA_REF_H

#include "../tools/relay/arena_wire.h"

namespace port {

// Arena mode installed, relay shape (a referee exists). Cheap; safe anywhere.
bool vs_ref_active();

// Once a frame (star_flow.cpp). `frame` is the host frame counter.
void vs_ref_pre_walk(int frame);
void vs_ref_post_walk(int frame);

// THE COUNTDOWN. True when the referee owns this frame's countdown: *count is
// data_0209f2bc (3, 2, 1, 0 = START), *f304 the word beside it, *beep is the
// ROM sound id to play now (0 none: 0x2b a count beep, 0x2a the GO chord),
// *music is set on the frame the arena's own track starts. False outside an
// arena relay match (the ROM's own countdown runs).
bool vs_ref_countdown(int *count, int *f304, int *beep, int *music);

// THE END. True once the referee ended the match; fills the verdict.
struct VsRefOver {
    int reason;              // arena_wire::Over: 1 time up, 2 first to N,
                             // 3 last standing, 4 everyone left
    int winners_mask;        // bit = seat
    int winners;             // how many
    int roster_mask;         // seats still in the match at the end
    int seen_mask;           // every seat that was ever in it
    signed char scores[16];  // the final ledger
    unsigned seq;            // the MATCH_OVER event's seq (identical everywhere)
    bool local_timeout;      // no verdict came: the console gave up (3 s)
    // LUIGI INFECTION only (0 everywhere else):
    int luigi_cancel;        // the match never started: 1 the relay does not run
                             // Luigi Infection, 2 the room's games disagree, 3 no
                             // START came in kLuigiStartGiveUpMs
    int luigi_left;          // reason 5 came because the last survivor left
};
bool vs_ref_match_over(VsRefOver *out);

// The match clock ran out locally (the ROM's own time-up flag): the poll tells
// this layer, which then waits for the referee's verdict up to 3 s.
void vs_ref_note_local_timeup(int frame);

// The latest ledger, per seat (the referee's score), and whether it is live.
bool vs_ref_ledger(signed char out[16]);

// A claim (any kind) to the referee. stamp == 0 means "now" (the touch is this
// frame's); my_frame == 0 means "my last completed frame". Returns the claim id
// (1.., never 0) or 0 when it could not be made (no referee clock yet, or the
// table is full). READY is made by this layer itself.
unsigned short vs_ref_claim(unsigned char kind, unsigned char target,
                            unsigned short aux, unsigned ref_frame,
                            unsigned stamp, unsigned my_frame);

// What came of a claim: 0 still pending, 1 the referee named it (accepted /
// taken / hit / eaten / released), -1 denied (*reason = arena_wire::Deny,
// *winner = the seat that beat us for DENY_LOST else 0xFF), -2 gave up after
// kVerdictTimeoutMs (treated as denied).
int vs_ref_claim_result(unsigned short claim_id, int *reason, int *winner);

// Every applied stream or directed event, in order, after this layer has
// applied its own effect. my_seat = this console's seat. One sink.
typedef void (*VsRefEventSink)(const arena_wire::RefEvent *ev, int my_seat,
                               bool directed);
void vs_ref_set_event_sink(VsRefEventSink sink);

// The referee clock stamp of a local comms_hub_now_ms() instant (0 = none).
bool vs_ref_stamp_for(unsigned local_ms, unsigned *stamp);

// Counters + the periodic "[vsarena-ref]" report line.
struct VsRefStats {
    bool active;
    bool started;            // START arrived
    bool over;
    unsigned applied_seq;
    unsigned long long frames_rx, events_applied, events_dup, gaps,
                       claims_sent, claims_resent, verdicts, denied,
                       stars_taken, hits_claimed, hits_landed, hits_denied,
                       spills, syno_tx, syno_rx,
                       frames_bad, events_bad, syno_bad;  // dropped by the field checks
    // FIELD4 F9-b / F9-c: claims never made because the referee would refuse them
    unsigned long long stars_shown;     // VSREF3 V4: stars kept over another seat's head
    unsigned long long shown_frames;    // ... and the frames they were drawn there
    // VSREF3 caps
    unsigned long long cap_claims;      // CAP_TAKE claims I made (my body's pickups)
    unsigned long long cap_lost;        // ... that lost and were undone
    unsigned long long cap_taken;       // CAP_TAKEN events applied
    unsigned long long cap_removed;     // ... that took a cap off my screen
    unsigned long long cap_dropped;     // CAP_DROPPED events applied
    unsigned long long cap_drop_claims; // CAP_DROP claims I made (my own cap popped)
    unsigned long long cap_copies;      // knocked-off caps I spawned for another seat
    unsigned long long cap_respawns;    // placed caps I put back
    unsigned long long cap_puppet_touches;  // puppet contacts on a cap cleared
    unsigned long long touches_gated;   // a star / marker touch while my body may not act
    unsigned long long hits_gated;      // an attack contact on a puppet that cannot be hit
};
VsRefStats vs_ref_stats();
void vs_ref_report(const char *tag);

}  // namespace port

#endif  // PORT_HAL_VS_ARENA_REF_H
