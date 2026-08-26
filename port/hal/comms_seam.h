// THE RADIO SEAM. Run mg15, lane MP1, stage one of the multiplayer plan.
//
// =========================== THE OWNER'S RULE ==============================
//
// The seam is wired FAITHFULLY in the port core so that every line of game
// code above it stays exactly the DS's and stays portable back to real
// hardware. The NETWORK TRANSPORT is not in the port core and is not in this
// file: it ships later as a launcher mod, the way the palette picker did.
// What lives here is the boundary, its honest solo answers, and the contract
// a transport has to satisfy.
//
// =========================== WHERE THE CUT IS ==============================
//
// SM64DS's multiplayer stack is decompiled end to end. Below a certain line
// it stops being a game and starts being a radio: it hands a command block to
// the ARM7 and the 802.11 work happens in arm7.bin, which this repo does not
// decompile and a PC port does not want. That line is these eight addresses,
// and nothing above them ever has to change:
//
//   func_020408b0(u16 mode)  bring the transport up
//   func_02040820()          become PARENT (the host)
//   func_02040790()          become CHILD  (a guest)
//   func_02040724()          leave
//   func_02040714() -> int   LINK STATE: 0 idle, 2 coming up,
//                            3 parent-connected, 4 child-connected
//   func_02040704() -> int   MY SLOT (aid). 0 is the parent.
//   func_020406b4(const void *block, u16 *status) -> int
//                            publish my 0x20-byte block for this frame and
//                            answer 1 when every live peer's block for this
//                            frame has arrived
//   func_0204068c(int aid) -> const void *
//                            player `aid`'s 0x20-byte block this round, or 0
//
// Two supports go with them: func_02040a5c/func_02040a84 (a status word the
// ROM keeps in the WM buffer) and func_02040c34, which on the DS starts the
// wireless thread with two callbacks.
//
// THE WIRE FORMAT IS THE ROM'S AND IT IS FIXED. 0x20 bytes per player per
// frame, staged at data_020a1020 by src/func_0203ea5c.c:176-187 and unpacked
// at the same offsets on receive (lines 277-290):
//
//   +0x00 (2)  flag word      0x8000 live, 0x4000 round complete, 0x2000 ...,
//                             0x1000 ..., 0x0001 info-mode
//   +0x02 (4)  frame counter  the parent is the clock
//   +0x06 (2)  key word       ((KEYINPUT | SHARED_PAD) ^ 0x2fff) & 0x2fff
//   +0x08 (1)  stylus x
//   +0x09 (1)  stylus y
//   +0x0A (1)  touch
//   +0x0B (2)  heading
//   +0x0D (1)  player count   honoured only from slot 0
//   +0x0E (1)  reserved       honoured only from slot 0
//   +0x0F (17) game payload   the shared per-player state channel
//
// A transport MUST NOT reinterpret those bytes. It moves them and nothing
// else. There is no netcode model to reproduce: the ROM is frame-locked
// lockstep with no prediction and no rollback (src/func_0203ea5c.c:157), the
// parent's frame counter is the clock, and when the wait bound runs out the
// ROM drops the session back to solo by itself.
//
// =========================== THE DEFAULT IS SOLO ===========================
//
// With no transport installed every answer here is the one the ROM gives a
// console with no partners:
//
//   connected  -> no   (link state 0)
//   slot       -> 0
//   players    -> 1
//   exchange   -> 0, forever; the ROM's own bound expires and its own solo
//                  fallback runs
//   peer block -> null
//
// And because the role byte data_020a0f04 never leaves 0 without something
// asking for a wireless mode, src/func_0203df40.c's switch keeps dispatching
// to func_0203e0ac, the solo cascade the port already runs. Today's
// single-player behaviour is unchanged, and that is proven by BMP rather
// than asserted.
//
// =========================== THE ROLE IS NOT THE FLAG ======================
//
// Two different questions the ROM asks with two different words, and the port
// used to answer both with one lie:
//
//   0x027FFC40 == 2      "this console was DOWNLOAD-PLAYED".
//                        src/func_0203db64.c:72 turns that into
//                        data_020a0f04 = 2 -- an automatic wireless CHILD --
//                        and src/func_020408b0.c:33 picks its init branch on
//                        it. It is a BOOT fact, not a role request.
//   data_020a0f10 != 0   "my comms slot is not the parent's", i.e. I am a
//                        guest in a session that already exists.
//
// port/ntr/io.cpp used to write 2 at 0x027FFC40 to skip an ARM7 sound
// handshake. That answer is now given where the question is asked (see
// comms_role_semantics() and hal/star_flow.cpp's func_0203d974) and the boot
// indicator carries the truth: a cartridge boot, host-capable, no role
// claimed. comms_set_boot_indicator exists so a launcher mod can simulate a
// downloaded child on purpose, which is the only way that value should ever
// become 2 here.

#ifndef PORT_HAL_COMMS_SEAM_H
#define PORT_HAL_COMMS_SEAM_H

#include <stdint.h>

namespace port {

// The ROM's own link-state values, as func_02040714 reports them.
enum CommsLinkState {
    kCommsIdle = 0,
    kCommsConnecting = 2,
    kCommsParentConnected = 3,
    kCommsChildConnected = 4,
};

// The ROM's own role byte, data_020a0f04, as src/func_0203df40.c switches on it.
enum CommsRole {
    kCommsRoleSolo = 0,
    kCommsRoleParent = 1,
    kCommsRoleChild = 2,
    kCommsRoleDownloadHost = 3,
};

enum : int { kCommsMaxPlayers = 4 };
enum : int { kCommsBlockBytes = 0x20 };

// ---------------------------------------------------------------------------
// THE MOD-FACING CONTRACT.
//
// A transport implements this and calls comms_set_transport(). Every entry is
// required; a null entry is refused at install time rather than crashed on
// later. Nothing here allocates, blocks, or throws. `poll` is the only entry
// called while the game is inside the ROM's lockstep wait, and it is called
// once per pump turn -- that is, once per DS frame the game is stalled.
// ---------------------------------------------------------------------------
struct CommsTransport {
    // Human-readable, for the debug readout and the crash log. Not parsed.
    const char *name;

    // func_020408b0's argument reaches `mode` unchanged (the ROM passes 2).
    void (*open)(unsigned mode);
    void (*close)();

    // Asynchronous requests. The answer appears in state(), never here.
    void (*become_parent)();
    void (*become_child)();

    // One of CommsLinkState.
    int (*state)();

    // My slot index. 0 is the parent. Only meaningful once state() is 3 or 4.
    int (*slot)();

    // Live slots, 1..kCommsMaxPlayers.
    int (*player_count)();

    // Publish my kCommsBlockBytes for this frame and answer whether every
    // live peer's block for THIS frame has arrived. 1 = the round is
    // complete, 0 = keep waiting. `status` receives whatever the ROM would
    // have read back out of data_020a0f80[1]; write 0 if there is nothing
    // meaningful to report.
    int (*exchange)(const void *my_block, uint16_t *status);

    // Player `aid`'s block for the round exchange() just completed, or null
    // if that slot is not live. The pointer must stay valid until the next
    // exchange() call.
    const void *(*peer_block)(int aid);

    // Called once per pump turn while the game is blocked. A socket transport
    // does its recv here.
    void (*poll)();
};

// Install a transport, or null for solo. Returns false and installs nothing
// if any entry is null.
bool comms_set_transport(const CommsTransport *t);
const CommsTransport *comms_transport();

// ---------------------------------------------------------------------------
// THE BOOT INDICATOR, honestly.
// ---------------------------------------------------------------------------

// The halfword at 0x027FFC40. 0 is a cartridge boot; 2 is "download-played",
// which makes src/func_0203db64.c seat the console as a wireless CHILD with
// no menu. The port comes up 0. Only a launcher mod deliberately simulating a
// downloaded copy should ever pass 2.
void comms_set_boot_indicator(uint16_t value);
uint16_t comms_boot_indicator();

// ---------------------------------------------------------------------------
// THE READOUT. What the seam is answering right now, for a log line or a
// debug overlay. Nothing here has side effects.
// ---------------------------------------------------------------------------
struct CommsReadout {
    const char *transport;      // "solo (no transport)" when none is installed
    int link_state;             // func_02040714
    int slot;                   // func_02040704
    int players;                // live count
    int role;                   // data_020a0f04
    bool connected;             // link_state is 3 or 4
    uint16_t boot_indicator;    // 0x027FFC40
    uint64_t exchanges;         // func_020406b4 calls since process start
    uint64_t rounds;            // of those, the ones that completed a round
};
CommsReadout comms_readout();

// ---------------------------------------------------------------------------
// THE ROM'S OWN FAN-OUT, steps 0x16 and 0x17 of the main loop
// (src/func_020197b8.c). func_0203bb60 turns the four comms records into
// TouchInfo[4] at data_020a0de8; func_0203bc7c turns them into PadData[4] at
// data_020a0e58 plus the per-player edge masks. Both are the ROM's, linked
// through port/slice_comms.txt.
//
// NOT CALLED FROM ANY FRAME LOOP YET. The port still writes TouchInfo and
// PadData directly, and replacing that is MP2's change because that is the
// one with a regression surface. This entry point exists so the closure can
// be exercised and proven now.
// ---------------------------------------------------------------------------
void comms_fanout();

// One line per player: the four 0x24-byte records at data_020a1154 as the
// game will actually read them, plus the local record at data_020a1040.
// Written to stderr. This is the instrument step 5(c) reads its verdict off.
void comms_report(const char *tag);

}  // namespace port

#endif  // PORT_HAL_COMMS_SEAM_H
