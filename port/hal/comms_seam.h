// THE RADIO SEAM. Run mg15, lane MP1, stage one of the multiplayer plan.
//
// ###########################################################################
// #  PROVISIONAL UNTIL MP2. THE ARCHITECTURE IS SETTLED; THE API IS NOT.    #
// ###########################################################################
//
// Reviewer MP1RV's freeze judgment, and it is the right call: an API should be
// frozen when a REAL CONSUMER HAS RUN AGAINST IT, and this one has not. No
// face below has ever been called by its real caller -- src/func_0203ea5c.c,
// the ROM's own lockstep, is in no slice. Everything MP1 proved was driven
// either by the hosted faces directly or by a lockstep skeleton written for
// the probe. The first time the ROM drives these eight is MP2, and that is
// exactly when signature and ordering mistakes surface.
//
// So: WRITE A TRANSPORT AGAINST THIS, BUT DO NOT PUBLISH A MOD THAT DEPENDS ON
// IT until the contract is frozen at the end of MP2, when a loopback transport
// has actually driven it.
//
// WHAT WILL NOT MOVE: the architecture. The radio seam is wired faithfully in
// the port core so every line of game code above it stays the DS's and stays
// portable back to hardware; the network ships only ever as a launcher mod;
// the default is the ROM's own solo fallback. That is settled.
//
// THE FOUR KNOWN GAPS, so a would-be mod author sees them rather than
// discovering them in the first hour:
//
//   1. exchange()'s `status` OUT-PARAM has no useful meaning yet. It is
//      specified below as "whatever the ROM would have read back out of
//      data_020a0f80[1]", which cannot be understood without reading
//      func_0203ea5c. In practice the ROM stores it in a stack local (sp2e)
//      and never reads it. WRITE 0. That is the whole contract, and MP2 should
//      either say so in the field's own comment or give the parameter a real
//      job.
//   2. NO LIFECYCLE ORDERING IS SPECIFIED. Must open() precede
//      become_parent()? What does state() return before open()? Is a double
//      close() legal? A transport author has to guess today. MP2 pins it,
//      because MP2 is what first calls them in a fixed order.
//   3. NO ERROR CHANNEL. open() returns void. The intended answer is "leave
//      state() at idle and let the ROM's own bound fire", which is the right
//      shape -- the ROM already handles a radio that never comes up -- but it
//      is nowhere stated as the contract, and a transport that wants to report
//      WHY has no way to.
//   4. THE WM STATUS WORD IS NOT IN THE CONTRACT. func_02040a5c /
//      func_02040a84 are hosted in comms_seam.cpp, which keeps the word
//      itself, so a real transport cannot supply it. It is documented in the
//      .cpp and absent from CommsTransport. Decide in MP2 whether it becomes
//      an entry or stays seam-owned.
//
// AND ONE PRE-CONDITION ON MP2 ITSELF, not a gap in this file: func_02040a5c
// carries character-for-character the arity defect MP1 closed for
// func_0203dabc / func_0203dae4. src/func_02040a5c.c:11 takes a u32;
// src/func_0203ea5c.c:33 declares it (void) and :359 calls it with nothing.
// Correct on ARM, a read of an unwritten stack slot on x86. It is a RIDDEN row
// in port/tools/aritycheck_plainfunc_baseline.txt, so no ratchet fires, and
// func_0203ea5c is in no slice so nothing is broken today. THE DAY MP2 LINKS
// func_0203ea5c TO RUN THE LOCKSTEP, g_wm_status starts taking stack garbage.
// Fix it the way MP1 fixed the other two: give the declaration its parameter,
// give the call site its argument, and re-gate the caller at 2004/b56.
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
// frame, staged at data_020a1020 by src/func_0203ea5c.c:177-186 and unpacked
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
//                        src/func_0203db64.c:73 TESTS it and :79 seats
//                        data_020a0f04 = 2 -- an automatic wireless CHILD,
//                        followed by a 0x4b0-bounded spin that calls the
//                        sleep veneer -- and src/func_020408b0.c:33 picks its
//                        init branch on it. It is a BOOT fact, not a role
//                        request. (:72 is the `data_020a0f04 = 0` above the
//                        test; an earlier draft of this note cited it.)
//   data_020a0f10 != 0   "my comms slot is not the parent's", i.e. I am a
//                        guest in a session that already exists.
//
// port/ntr/io.cpp used to write 2 at 0x027FFC40 to skip an ARM7 sound
// handshake. That answer is now given where the question is asked --
// hal/star_flow.cpp's hosted func_0203d974, a plain `return 1` -- and the boot
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
//
// *** THE TWO SENTENCES ABOVE ABOUT `poll` ARE NOT TRUE TODAY. run mg16 lane
// MP2 wrote the first real transport against this contract and found that
// NOTHING IN THE SEAM EVER CALLS poll: `->poll` appears exactly once in the
// whole tree, in the null check at comms_seam.cpp:52, which REFUSES a
// transport for omitting an entry it then never drives. See HOLE 1 in the
// contract-holes write-up at the bottom of hal/comms_loopback.cpp for the
// measurement and the two proposed fixes. Annotation only -- changing the
// contract belongs to the freeze lane, not to a comment. ***
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
    // complete, 0 = keep waiting.
    //
    // `status`: WRITE 0. PROVISIONAL, gap 1 in the banner at the top of this
    // file. It nominally receives what the ROM reads back out of
    // data_020a0f80[1], but the ROM's own caller stores that in a stack local
    // and never reads it, so 0 is the entire contract until MP2 either gives
    // the parameter a real job or documents it away.
    int (*exchange)(const void *my_block, uint16_t *status);

    // Player `aid`'s block for the round exchange() just completed, or null
    // if that slot is not live. The pointer must stay valid until the next
    // exchange() call.
    const void *(*peer_block)(int aid);

    // Called once per pump turn while the game is blocked. A socket transport
    // does its recv here.
    //
    // NOT DRIVEN. Required at install and never called -- see the starred note
    // above and HOLE 1 in hal/comms_loopback.cpp. A transport that does its
    // recv ONLY here receives nothing; the loopback carrier services itself
    // from every entry it owns instead.
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

// ---------------------------------------------------------------------------
// THE CONDUCTOR'S HOST SIDE. Run mg16 lane MP3, implemented in
// hal/comms_conductor.cpp. These exist because src/func_0203df40.c and
// src/func_0203ea5c.c are LINKED now and the ROM's own code has hardware
// expectations the port has to meet.
// ---------------------------------------------------------------------------

// Publish `held` into the DS key register the way the hardware carries it:
// ACTIVE LOW, complemented against 0x2fff. src/func_0203df40.c:31 reads
// KEYINPUT and un-complements it, so a port that leaves the register at 0 hands
// the game ALL FOURTEEN BUTTONS HELD rather than none.
//
// CALL IT UPSTREAM OF THE CONDUCTOR AND OF THE FAN-OUT, and pass the value in
// rather than letting this read the pad mirror: with the fan-out on,
// func_0203bc7c rewrites that mirror from the four comms records later in the
// same frame, so a self-read here would feed the wire back into itself.
void comms_publish_pad(unsigned held);

// Install the seam's pump, which calls the installed transport's poll() once
// per OS_SleepThread turn inside the ROM's own wait loop. This is HOLE 1's
// fix (a), and it is what makes this header's `poll` paragraph true.
void comms_install_pump();

// Seat the role byte data_020a0f04 and the open one-shot data_02099e1c, which
// on the DS are seated by the MULTIPLAYER MENU before the conductor first runs.
// `role` is kCommsRoleParent or kCommsRoleChild.
//
// Without this the linked src/func_0203df40.c switches to its solo arm forever
// and never asks the transport for anything -- the conductor's own
// become_parent/become_child live in the case-0 arm and are chosen off this
// same byte. A launcher mod that installs a transport must seat it; that is
// the mod standing in for the menu the port does not have on this path.
void comms_seat_session_request(int role);

// The DS touch-panel ring's write index, at data_020a80cc[6] as a halfword.
// hal/sub_screen.cpp's poll_touch writes one entry per frame and advances it;
// src/func_0203b9bc.c reads the four entries behind it. Nine entries.
int touch_ring_index();
void touch_ring_advance();

// SM64DS_COMMS_INJECT's stylus half, for hal/sub_screen.cpp's poll_touch to
// honour where it already honours SM64DS_TOUCH_PROBE. Returns false when no
// stylus injection is configured, leaving the real touch source alone.
bool comms_inject_touch(int *down, int *x, int *y);

// How many Player actors this level should boot with. ONE when no transport is
// installed -- which is what keeps every single-player baseline valid -- and
// otherwise the live count the ROM's conductor negotiated. hal/level_boot.cpp
// seats data_0209f21c from it. SM64DS_VS_PLAYERS overrides, for proofs.
int vs_player_count();

// SM64DS_VS_PROBE=1: one stderr line per frame per player slot, read out of
// data_0209f394 (the per-slot Player pointers), each actor's mPlayerNo, its
// position, and its body cylinder's "who did I touch this frame". This is the
// instrument rungs 9 to 11 read their verdicts off, and it deliberately reads
// the GAME'S OWN actor array rather than any wire counter.
void vs_probe(int frame);

}  // namespace port

extern "C" {
// Assert that the .dsstate$ymp3 bands came out of the linker contiguous and in
// ROM order. Returns nonzero when they did. Everything the conductor does with
// &data_020a1020 as a 0x20-byte block depends on it.
int port_comms_conductor_check_layout(void);
}

#endif  // PORT_HAL_COMMS_SEAM_H
