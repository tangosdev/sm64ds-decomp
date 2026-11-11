// COOP BOOT: the 'SYNB' sub-types 0x30-0x3f and the pure rules around them.
// Run mp1 wave 56, lane COOPBOOT1. No dependencies on the carrier or the game
// (only hub_presence.h's head), so a proof tool can compile and run it alone.
//
// The common head is hub_presence.h's HubCoopHead (12 bytes, tag 'SYNB', seat
// stamped by the relay). Every payload below is a fixed 4 bytes, so every
// datagram is 16 bytes. Any reserved byte that is not 0 drops the datagram.
//
//   0x30 READY    guest -> all. "I am at the file press." Resent at 4 Hz until
//                 the host's STATE says go. payload: 0 0 0 0
//   0x31 STATE    host -> all, 2 Hz from the host's boot (4 Hz while it
//                 counts down a start). payload:
//                   u8 go      0 = still gathering, 1 = GO (everybody in)
//                   u8 phase   0 gathering, 1 playing (GO given), 2 intro
//                              running (a new file's opening, until the
//                              host's opening-seen bit), 3 settled
//                   u8 closed  1 = the opening's end barrier has passed
//                   u8 0
//                 STATE is the whole truth, resent, so a lost GO or CLOSE
//                 costs one beat.
//   0x32 ARRIVED  guest -> all. "I reached the opening's closing level load."
//                 payload: 0 0 0 0
//   0x33 CLOSE    host -> all. The end barrier is open. payload: 0 0 0 0
//   0x34 HOST_LEFT host -> all, three times as the host process exits.
//                 payload: 0 0 0 0
//   0x35 BYE      guest -> all, three times as a guest quits the room on
//                 purpose (REJOIN1). Only words: the others say "<name> left."
//                 instead of "<name> lost connection." payload: 0 0 0 0
//
// WHO MAY SAY WHAT. READY / ARRIVED are only used by the host (from guests).
// STATE / CLOSE / HOST_LEFT are only used by a guest, and only from the seat
// that sent the first valid STATE it heard (the host seat); a later STATE from
// another seat is dropped and counted. A forged STATE can start or release a
// barrier early or end a room; it cannot grant a star, move a player or touch
// a save (COOPSAVE1 owns the save sub-types). The worst case is "go now".

#ifndef PORT_HAL_COOP_BOOT_WIRE_H
#define PORT_HAL_COOP_BOOT_WIRE_H

#include <stdint.h>
#include <string.h>

#include "hub_presence.h"

namespace port {

enum : int {
    kCoopBootSubReady    = 0x30,
    kCoopBootSubState    = 0x31,
    kCoopBootSubArrived  = 0x32,
    kCoopBootSubClose    = 0x33,
    kCoopBootSubHostLeft = 0x34,
    kCoopBootSubBye      = 0x35,   // REJOIN1
    kCoopBootSubLo       = 0x30,
    kCoopBootSubHi       = 0x3f,
    kCoopBootPayload     = 4,
    kCoopBootDatagram    = kHubCoopHeaderBytes + kCoopBootPayload,   // 16
};

enum : int {
    kCoopPhaseGather  = 0,
    kCoopPhasePlaying = 1,
    kCoopPhaseIntro   = 2,
    kCoopPhaseSettled = 3,
};

struct CoopBootState {
    int go;
    int phase;
    int closed;
};

// Is `sub` one of this lane's sub-types (known or not)?
inline bool coop_boot_sub_mine(int sub) {
    return sub >= kCoopBootSubLo && sub <= kCoopBootSubHi;
}

// Validates a whole SYNB datagram of this lane: head, sub-type, length 16, and
// the payload rules above. Returns the sub-type (0x30..0x35) or -1.
inline int coop_boot_validate(const unsigned char *d, int len) {
    if (!d || len != kCoopBootDatagram) return -1;
    if (!hub_coop_validate(kHubTagCoopCtl, d, len)) return -1;
    const int sub = d[6];
    if (sub < kCoopBootSubReady || sub > kCoopBootSubBye) return -1;
    const unsigned char *p = d + kHubCoopHeaderBytes;
    if (sub == kCoopBootSubState) {
        if (p[0] > 1 || p[1] > 3 || p[2] > 1 || p[3] != 0) return -1;
        return sub;
    }
    if (p[0] | p[1] | p[2] | p[3]) return -1;
    return sub;
}

inline bool coop_boot_decode_state(const unsigned char *d, int len,
                                   CoopBootState *out) {
    if (coop_boot_validate(d, len) != kCoopBootSubState || !out) return false;
    const unsigned char *p = d + kHubCoopHeaderBytes;
    out->go = p[0]; out->phase = p[1]; out->closed = p[2];
    return true;
}

inline int coop_boot_encode(int sub, unsigned seq, const CoopBootState *st,
                            void *out, int cap) {
    unsigned char pl[kCoopBootPayload] = {0, 0, 0, 0};
    if (sub == kCoopBootSubState) {
        if (!st) return 0;
        pl[0] = (unsigned char)st->go; pl[1] = (unsigned char)st->phase;
        pl[2] = (unsigned char)st->closed;
    } else if (sub < kCoopBootSubReady || sub > kCoopBootSubBye) {
        return 0;
    }
    return hub_coop_encode(kHubTagCoopCtl, sub, seq, pl, kCoopBootPayload, out,
                           cap);
}

// ---- the two barrier rules, pure so they can be unit-tested ----------------
// START: the host sends GO when every present member has said READY, or when
// `waited_ms` since the host itself got ready reaches 8000.
inline bool coop_boot_start_due(unsigned present_mask, unsigned ready_mask,
                                bool host_ready, unsigned waited_ms) {
    if (!host_ready) return false;
    if ((present_mask & ~ready_mask) == 0) return true;
    return waited_ms >= 8000u;
}
// END (the opening's closing load): CLOSE when every present member has
// ARRIVED (the host counts itself), or 5000 ms after the first arrival.
inline bool coop_boot_close_due(unsigned present_mask, unsigned arrived_mask,
                                bool any_arrived, unsigned waited_ms) {
    if (!any_arrived) return false;
    if ((present_mask & ~arrived_mask) == 0) return true;
    return waited_ms >= 5000u;
}

}  // namespace port

#endif  // PORT_HAL_COOP_BOOT_WIRE_H
