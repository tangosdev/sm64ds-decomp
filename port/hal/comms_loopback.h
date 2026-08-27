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
//   (unset)                    NOTHING IS INSTALLED. Every existing solo path
//                              runs the seam's own solo answers, untouched.
//
//   SM64DS_COMMS_PORT=<n>      base UDP port, default kCommsLoopbackPortBase.
//                              Slot k listens on <n>+k, so the parent is <n>.
//   SM64DS_COMMS_SLOT=<1..3>   a child may pin its slot instead of claiming
//                              the first free one. Out of range is refused.
//
// It composes with the MP1 knobs rather than replacing them:
// SM64DS_COMMS_FANOUT=1 still runs the ROM's steps 0x16 and 0x17, and
// SM64DS_COMMS_REPORT=1 still prints the four slots. This file adds no knob
// that changes what those two do.

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
};
CommsLoopbackStats comms_loopback_stats();

// One line to stderr, the transport half of the MP1 report. Tagged the same
// way port::comms_report is so the two interleave readably in one log.
void comms_loopback_report(const char *tag);

}  // namespace port

#endif  // PORT_HAL_COMMS_LOOPBACK_H
