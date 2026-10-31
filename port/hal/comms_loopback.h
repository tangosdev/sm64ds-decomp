// THE LOOPBACK CARRIER for the radio seam. Run mg16, lane MP2.
//
// The first real transport written against port/hal/comms_seam.h, and
// therefore the thing that decides whether that contract is right. Two copies
// of the port on ONE machine, one as the DS PARENT (player 1) and one as a
// CHILD (player 2), exchanging the ROM's own 0x20-byte block once per frame
// over UDP on 127.0.0.1.
//
// It is HOST PLUMBING, exactly like the window layer. No line of game code
// above the seam changes and nothing here interprets the ROM's bytes; the
// transport moves them and answers the seam's questions. What the arm9 comms
// code sees -- join, slots, per-frame stylus and key exchange, sleep and wake
// -- has to satisfy comms_seam.h, and where that contract turned out to be
// wrong the divergence is written down in comms_loopback.cpp as a PROPOSED
// CONTRACT CHANGE rather than quietly reinterpreted. Freezing the contract is
// MP2's exit condition, so a hole found here is the deliverable, not a defect.
//
// SELECTED ONLY BY ENV, and only when the role env is present:
//
//   SM64DS_COMMS_ROLE=parent   bind the parent port, wait for children
//   SM64DS_COMMS_ROLE=child    claim the first free child port, join the parent
//   SM64DS_COMMS_ROLE=0 / =1   THE SAME TWO ROLES in the relay wire's own
//                              spelling (0 parent, 1 child), which is what
//                              port/tools/relay/README.md's datagram table
//                              teaches whoever is setting up a relay session.
//                              Accepted so the document a player reads and the
//                              knob he then sets agree. Nothing else is.
//   (unset)                    NOTHING IS INSTALLED. Every existing solo path
//                              runs the seam's own solo answers, untouched.
//
//   SM64DS_COMMS_PORT=<n>      base UDP port, default kCommsLoopbackPortBase.
//                              Slot k listens on <n>+k, so the parent is <n>.
//   SM64DS_COMMS_SLOT=<1..3>   a child may pin its slot instead of claiming
//                              the first free one. OUT OF RANGE WARNS AND FALLS
//                              BACK to the first free slot -- the PIN is
//                              refused, the session is not. Refusing to start
//                              would turn a mistyped knob into a game that
//                              never comes up, and the transport already has
//                              one honest way to fail (stay idle and let the
//                              ROM's bound drop to solo) which should mean a
//                              real transport failure, not a typo. Ignored
//                              entirely for the parent, which is always slot 0.
//
// It composes with the MP1 knobs rather than replacing them:
// SM64DS_COMMS_FANOUT=1 still runs the ROM's steps 0x16 and 0x17, and
// SM64DS_COMMS_REPORT=1 still prints the four slots. This file adds no knob
// that changes what those two do.
//
// ---------------------------------------------------------------------------
// THE ADDRESS MODES (run vsdec, lane NET). The same carrier, pointed somewhere
// other than 127.0.0.1. ALL OPT-IN: with none of these set the mode is
// loopback and the behaviour is what it was before they existed.
//
//   SM64DS_COMMS_HOST=<ip[:port]>   DIRECT MODE. A CHILD sends to this address
//                                   instead of loopback. Ignored for a parent,
//                                   which learns each child from its JOIN.
//                                   Needs the two ends to be able to reach
//                                   each other -- a LAN, or a forwarded port.
//   SM64DS_COMMS_BIND_ANY=1         bind 0.0.0.0 instead of 127.0.0.1. Needed
//                                   by a DIRECT-MODE PARENT, which otherwise
//                                   can send but never receive. Opt-in on
//                                   purpose: it is the difference between a
//                                   socket only this machine can reach and one
//                                   the whole network can.
//
//   SM64DS_COMMS_RELAY=<host[:port]>  RELAY MODE, the no-port-forwarding path.
//                                   Everything goes to a rendezvous service
//                                   which forwards parent->children and
//                                   child->parent. Default port 41234.
//   SM64DS_COMMS_CODE=<up to 8 ascii> the session code two players share.
//                                   REQUIRED with RELAY and has no default;
//                                   a default would put everyone who forgot to
//                                   set one into the same session.
//                                   Relay version 2 first (seats stamped by
//                                   the relay, voice / chat / pings reach
//                                   every seat); a version 1 relay answers
//                                   "bad request" and this end falls back to
//                                   version 1 on a fresh socket. A child asks
//                                   for SM64DS_COMMS_SLOT as its seat (any
//                                   free one without it) and plays as the
//                                   seat the relay's ACK names.
//   SM64DS_COMMS_RELAY_V=1          speak relay version 1 from the start
//                                   (escape hatch and test knob).
//
//   SM64DS_COMMS_DELAY_MS=<n>       TEST SCAFFOLDING: hold every received
//                                   datagram n ms. ONE WAY -- both ends run it,
//                                   so the round trip is 2n.
//   SM64DS_COMMS_JITTER_MS=<n>      uniform +/- spread on that hold, which is
//                                   also what makes reordering happen.
//   SM64DS_COMMS_DUP=<1..4>         send each lockstep datagram k times. The
//                                   in-flight redundancy lever, which is the
//                                   one the redundancy finding at the bottom
//                                   of comms_loopback.cpp did NOT refuse.
//   SM64DS_COMMS_RESEND_MS=<n>      pin the republish interval. Without it the
//                                   address modes start at 50 ms and retune to
//                                   the handshake's measured round trip; a
//                                   4 ms republish is right on loopback and is
//                                   flooding over the internet.
//
//   SM64DS_COMMS_INPUT_DELAY=<0..8> HOW MANY FRAMES AHEAD THE INPUT PIPELINE
//                                   RUNS, and the one knob in this block that
//                                   is NOT off by default. Frame R is handed
//                                   the records from round R-N, so rounds
//                                   overlap the wire instead of taking turns
//                                   with it. Without it a session is
//                                   STOP AND WAIT and the frame rate IS the
//                                   round trip -- 90 ms of relay is 11 fps.
//                                   Defaults: loopback 0, direct 2, relay 5;
//                                   the derivation is over the parse in
//                                   comms_loopback.cpp. Raise it on a worse
//                                   path (the signal is `starved` in the
//                                   report line: nonzero means frames are
//                                   still waiting on the wire), or set 0 to
//                                   get stop-and-wait back for a measurement.
//                                   SET IT ON THE PARENT. The parent's value
//                                   is the session's: it rides in the ACCEPT
//                                   and every child adopts it, saying so in
//                                   its log. A child's own setting LOSES, and
//                                   that is not tidiness -- THE TWO ENDS
//                                   RUNNING DIFFERENT DEPTHS IS A DESYNC (the
//                                   derivation, and the ROM's own compare at
//                                   src/func_0203ea5c.c:418, are over the
//                                   ACCEPT's delay field in the .cpp), and it
//                                   is a desync that reads HEALTHY in both
//                                   logs. The wire carries the number so this
//                                   cannot be got wrong by hand.
//                                   The SIMULATION IS UNTOUCHED at any depth
//                                   both ends share: both consoles consume the
//                                   same round sequence at the same frame
//                                   index, nothing is predicted and nothing is
//                                   rolled back.
//
// SLOT ASSIGNMENT OVER THE ADDRESS MODES: on loopback a child owns a slot by
// BINDING its port, and the parent verifies the two agree. Off this machine a
// peer binds whatever port it likes and a NAT rewrites it, so the child's slot
// becomes a PROPOSAL carried in its JOIN and THE PARENT ASSIGNS -- the answer
// comes back in the ACCEPT. The relay never assigns anything; it forwards.

#ifndef PORT_HAL_COMMS_LOOPBACK_H
#define PORT_HAL_COMMS_LOOPBACK_H

namespace port {

// Slot k listens on kCommsLoopbackPortBase + k. Chosen in the IANA dynamic
// range (49152-65535) and away from the blocks Windows hands out for ephemeral
// client ports by default, so a parent bind that fails means another copy of
// the game is already parent rather than a random program holding the port.
enum : int { kCommsLoopbackPortBase = 51765 };

// Read SM64DS_COMMS_ROLE and, if it names a role, build the transport and hand
// it to comms_set_transport(). Returns true only when a transport was actually
// installed. With the env unset it installs nothing, touches nothing, and
// returns false -- which is the whole of the "solo stays byte-identical"
// promise on the code path side.
//
// Safe to call more than once; only the first call with a role installs.
bool comms_loopback_install_from_env();

// KNOWNNET1: the lockstep carrier's teardown for a VS match that ends in place at the title (hal/star_flow.cpp). The goodbye, the
// role byte back to 0 and the transport handed back; true when nothing is left installed. Safe when nothing was installed.
bool comms_loopback_leave_reset(const char *why);

// Run vs16, hosted-conductor follow-up: the number of players this session was
// opened for -- the clamped SM64DS_VS_PLAYERS the installer read, 0 when no
// session was installed. This is the SAME number that chooses between the two
// wires, exported so the conductor dispatch in hal/comms_conductor_wide.cpp
// and the wire can never disagree about whether the session is wide.
int comms_session_players();

// Lane VOICE: one whole 'SYNV' voice message into `buf`, at most `cap` bytes,
// with the SENDER'S SLOT written through `from_slot` (the slot the carrier
// itself classified, never a field read out of the payload). Returns the byte
// count or 0 when nothing is waiting; never blocks. The voice kind is served
// here and skipped by the seam's recv_aux, so the sync layer and the voice
// pump cannot eat each other's messages. hal/voice_chat.cpp is the only
// caller; its header carries the datagram format.
int comms_recv_voice(void *buf, int cap, int *from_slot);

// Multiplayer 1.0 (lane VSAUX1): THE GENERAL KIND READER. One whole aux
// datagram of the kind `tag` names (the first four bytes read as a little-
// endian word), with the SENDER'S SEAT through `from_seat` -- stamped by relay
// v2, classified by this carrier everywhere else, and never read out of the
// payload; byte 5 of the datagram carries the same seat. Returns the byte
// count or 0 when nothing is waiting; never blocks. A datagram longer than
// `cap` is dropped (counted), not cut short.
//
// Every aux datagram starts with the 8-byte envelope tag | version | seat | two
// kind bytes, and is checked against the carrier's kind table (tag + length)
// both ways: send_aux refuses, and the receive side drops, anything else.
//   'SYNT' chat      12..132 B   one FIFO for the room, oldest first
//   'SYNI' identity  24..64 B    newest per seat
//   'SYNV' voice     176/340 B   a FIFO per seat (comms_recv_voice is this)
// The sync layer's 'SYN1' / 'SYNP' / 'SYNQ' are served by the seam's recv_aux
// and the 'SYNK' clock report is the carrier's own, so this returns 0 for
// those. Sending is the seam's send_aux for every kind.
enum : unsigned {
    kCommsTagChat  = 0x544e5953u,   // 'S','Y','N','T'
    kCommsTagIdent = 0x494e5953u,   // 'S','Y','N','I'
    kCommsTagVoice = 0x564e5953u,   // 'S','Y','N','V'
};
int comms_recv_kind(unsigned tag, void *buf, int cap, int *from_seat);

// The carrier's own counters, for a log line and for the proof ladder. These
// are the transport's view; port::comms_readout() is the seam's.
struct CommsLoopbackStats {
    bool installed;
    int role;               // 0 none, 1 parent, 2 child
    int slot;               // this process's slot, 0 for the parent
    int port;               // the UDP port this process listens on
    int live_mask;          // bit k set when slot k is live
    unsigned long round;    // the round index exchange() is currently on
    unsigned long long sent;
    unsigned long long recvd;
    unsigned long long dropped;      // datagrams refused by the header check
    unsigned long long resends;      // duplicate publishes of an open round
    unsigned long long stale_serves; // cached rounds re-sent to a lagging peer
    // run vsdec, lane NET: the address modes.
    int net_mode;                    // 0 loopback, 1 direct, 2 relay
    bool relay_paired;               // relay mode: a status-0 ACK has landed
    int handshake_rtt_ms;            // child: JOIN -> ACCEPT, or -1
    int resend_ms;                   // the republish interval actually in use
    int delay_ms;                    // induced one-way hold, 0 when off
    int jitter_ms;
    unsigned long long delay_overflow;  // datagrams the delay ring could not
                                        // hold. Nonzero invalidates a run.
    int input_delay;                    // pipelining depth in frames, 0 = off
    unsigned long long starved;         // pipelined exchange() calls that still
                                        // had to return 0: the input delay is
                                        // set too low for the path
};
CommsLoopbackStats comms_loopback_stats();

// The host's view of joiner `slot`'s round trip in ms (what the joiner
// reported at join; the per-child list of the host's sizing line). -1 on a
// joiner, for slot 0, a slot that is not live or has not reported yet.
// Read-only.
int comms_loopback_child_rtt_ms(int slot);

// One line to stderr, the transport half of the MP1 report. Tagged the same
// way port::comms_report is so the two interleave readably in one log.
void comms_loopback_report(const char *tag);

// ---------------------------------------------------------------------------
// ROLLBACK (port/rollback). All of these are inert (false / ~0u / no-op)
// unless NetMode is rollback. hal/rollback.cpp is the caller; see its banner.
// ---------------------------------------------------------------------------
bool     comms_rb_mode();                 // NetMode rollback selected
bool     comms_rb_enabled();              // ... and a session is connected
bool     comms_rb_replaying();            // a rewind is being re-run
unsigned comms_rb_round();                // the round exchange() serves next
unsigned comms_rb_replay_end();           // the live round while replaying
unsigned comms_rb_scan();                 // oldest contradicted round, or ~0u
bool     comms_rb_rewind(unsigned to);    // start replaying from round `to`
void     comms_rb_flush(const char *why); // suspend guessing until confirmed
void     comms_rb_det_reuse(bool on);     // replay serves the exact old guess
const unsigned char *comms_rb_my_served(unsigned *round); // own block served
void     comms_rb_leave(const char *why); // leave the session, Bye sent
struct CommsRollbackStats {
    unsigned long long predicted, confirmed_ok, mispredicted, rewinds,
                       replayed, stalled, stall_events, drops;
    unsigned unrecoverable;
};
CommsRollbackStats comms_rb_stats();

}  // namespace port

// Multiplayer 1.0, lane LAGGARD1 (NETARCH1 design 4.2, the laggard rule).
// port_comms_rounds_ahead: for walk_window.cpp's frame pacer, which skips its
// sleep for the frame when the answer is above 2 (a child catching up after a
// hitch); 0 on every console that must not sprint (the parent, rollback,
// solo). port_vs_seat_live_mask: bit k set when the round the game is
// simulating carried slot k's block -- the same on every console at the same
// frame, so a seat refused at the door, quit or retired is clear from the
// round it left on; 0 when no VS carrier session is running.
extern "C" int      port_comms_rounds_ahead(void);
extern "C" unsigned port_vs_seat_live_mask(void);

#endif  // PORT_HAL_COMMS_LOOPBACK_H
