// THE RADIO SEAM. Run mg15 lane MP1; frozen by run mg16 lane MP3.
//
// ###########################################################################
// #  FROZEN. The ROM's own caller has run against this contract.            #
// ###########################################################################
//
// Reviewer MP1RV set the condition and it has now been met: an API should be
// frozen when a REAL CONSUMER HAS RUN AGAINST IT. src/func_0203ea5c.c -- the
// ROM's own lockstep -- and src/func_0203df40.c, the dispatcher that owns the
// only close() call site, are LINKED as of MP3 (port/slice_mp3.txt) and drive
// these faces themselves. MP1 drove them from a probe, MP2 from a faithful
// transcription; neither could settle a question the real caller answers by
// existing. A transport written against what follows can be published.
//
// WHAT DID NOT MOVE: the architecture. The seam is wired faithfully in the port
// core so every line of game code above it stays the DS's and stays portable
// back to hardware; the network ships only as a launcher mod; the default is
// the ROM's own solo fallback.
//
// ======================= THE FIVE HOLES, AND HOW EACH CLOSED ===============
//
// MP2 wrote these up at the bottom of hal/comms_loopback.cpp after building the
// first real transport. Each is settled here, and where the real caller
// contradicted the proposal that is said rather than quietly corrected.
//
//   HOLE 1  poll() WAS REQUIRED AND NEVER CALLED.  CLOSED, by fix (a): the
//           seam grows the pump the prose always claimed. Linking the real
//           conductor is what made that cheap -- the ROM's wait does not spin,
//           it SLEEPS (src/func_0203ea5c.c:417 -> func_02042778 ->
//           func_0201a4d0 -> OS_SleepThread), and hal/os_thread.h's pump hook
//           was written for exactly that call chain. hal/comms_conductor.cpp
//           installs a pump that calls poll() once per turn of the ROM's own
//           wait. The sentence in this file is now true.
//           A TRAP FOR THE NEXT PUMP AUTHOR, paid for once: a pump that returns
//           "keep going" burns the whole pump limit on every sleep, because
//           nothing clears the sleep queue word on this path. 600 turns times
//           the ROM's 1200-turn bound is 720,000 pump calls inside one frame,
//           and the game appears to hang. The pump services the transport and
//           gives the turn straight back.
//
//   HOLE 2  THE LIFECYCLE.  CLOSED as MP2 proposed; the real caller agreed with
//           every line, so the proposal is simply promoted:
//             open() precedes become_parent()/become_child(). state() is
//             kCommsIdle before open(). A become_* before open() is REFUSED and
//             leaves state idle. A second open() while open is a NO-OP that
//             keeps the live socket, because the ROM's init path can reach
//             func_020408b0 twice and a reopen would drop a live session.
//             close() when never opened is legal and silent; double close is
//             legal. slot() is 0 and player_count() is 1 before a link exists.
//             exchange() before connected returns 0 and writes 0 through
//             status. peer_block() before any completed round returns null.
//
//   HOLE 3  THE ROLE IS DECIDED TWICE AND NOBODY SAID WHO WINS.  CLOSED, and
//           the real caller REFRAMED THE QUESTION rather than answering it.
//           "Who wins" never arises, because the ROM does not ask for a role at
//           all until something seats data_020a0f04: src/func_0203df40.c
//           switches on that byte, and the become_parent/become_child calls
//           live in the case-0 arm chosen off the same byte. At 0 the ROM takes
//           its solo arm forever. On the DS the MULTIPLAYER MENU seats it. So:
//             a transport MAY be pre-configured with a role, and must be if the
//             role decides how it binds; become_parent()/become_child() are a
//             REQUEST it may refuse, and refusal is expressed by leaving state
//             idle; and WHATEVER INSTALLS A TRANSPORT MUST ALSO SEAT THE
//             SESSION REQUEST (port::comms_seat_session_request), because it is
//             standing in for the menu. A mod that installs a transport and
//             nothing else will connect to nothing and report role 0 forever.
//
//   HOLE 4  "THIS FRAME" IN exchange() HAS NO IDENTIFIER.  CLOSED as proposed,
//           and now measured against the real caller rather than a
//           transcription of it: exchange() is polled with the same logical
//           frame until it returns 1, and the caller does not begin a new
//           frame's exchange before the previous one completed or the ROM's own
//           wait bound expired. A transport may treat the transition to 1 as
//           the round boundary. The bound half of that sentence is what HOLE 5
//           is about.
//
//   HOLE 5  NOTHING TOLD A TRANSPORT THE SESSION WAS ABANDONED.  CLOSED by
//           adding the entry MP2 proposed: abandon(). It is called where the
//           ROM drops to solo, and it means "forget the open round".
//           IT HAS MORE TEETH THAN MP2 KNEW. The write-up expected a drifting
//           round counter that a four-deep cache would usually heal. What the
//           real caller does is worse and simpler: the drop is PERMANENT unless
//           something re-requests the session, because data_020a0f04 goes to 0
//           and nothing on the port side was going to set it again. A child
//           that starts before its parent gives up and never joins. So the seam
//           also RE-SEATS the request when the ROM drops a session whose
//           transport is still connected, which is the other half of what the
//           menu would do.
//
//   GAP 1 (status out-param): NO CHANGE, as proposed. Write 0. The field's own
//           comment now says so instead of pointing at a ROM read nobody does.
//   GAP 3 (no error channel): CLOSED as "leave state() at idle". The ROM
//           already handles a radio that never comes up, and MP3 exercised
//           exactly that path -- a child knocking at an unbound port -- so this
//           is measured rather than intended.
//   GAP 4 (the WM status word): CLOSED as STAYS SEAM-OWNED. A transport has no
//           opinion about that word. func_02040a5c/func_02040a84 keep it.
//
// THE ARITY DEFECT THIS BANNER USED TO WARN ABOUT IS FIXED. func_02040a5c is
// declared with its u32 and called as func_02040a5c(func_02040a84()) -- the
// ROM's own r0 hand-off written so a stack ABI carries it too -- and
// src/func_0203ea5c.c still compiles to the ROM's bytes at 2004/b56. Two more
// like it were found and fixed at the same time: func_0203ea5c declared `int`
// by its dispatcher against a `void` definition, and func_0203f604 declared
// `void (void)` against `void *(int, unsigned, void *)`. The third is the one
// that mattered, because it is ADDRESS-TAKEN and no call-site edit could have
// reached it.
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
// `poll` IS DRIVEN as of MP3 -- hal/comms_conductor.cpp installs a pump on
// hal/os_thread.h's hook and the ROM's own wait sleeps through it, so "once per
// pump turn" is once per turn of src/func_0203ea5c.c's real wait loop. MP2
// measured that nothing called it and annotated this paragraph as false; the
// code changed rather than the claim. See HOLE 1 in the banner at the top.
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
    // `status`: WRITE 0. That is the whole contract and it is now FROZEN as
    // such. The ROM's caller stores what comes back in a stack local (sp2e at
    // src/func_0203ea5c.c:95) and never reads it, which the linked TU makes
    // checkable rather than a claim about code nobody had run.
    int (*exchange)(const void *my_block, uint16_t *status);

    // Player `aid`'s block for the round exchange() just completed, or null
    // if that slot is not live. The pointer must stay valid until the next
    // exchange() call.
    const void *(*peer_block)(int aid);

    // Called once per pump turn while the game is blocked. A socket transport
    // does its recv here.
    //
    // DRIVEN, as of run mg16 lane MP3. hal/comms_conductor.cpp installs a pump
    // on hal/os_thread.h's hook, and the ROM's own wait sleeps through it
    // (src/func_0203ea5c.c:417 -> func_02042778 -> func_0201a4d0 ->
    // OS_SleepThread), so this is called once per turn of the ROM's real wait
    // loop. That is exactly what the paragraph at the top of this struct always
    // claimed and what MP2 measured was not happening. HOLE 1 is CLOSED.
    void (*poll)();

    // THE SESSION WAS ABANDONED. Called when the ROM gives up waiting and drops
    // back to solo -- src/func_0203ea5c.c:487, which sets the role byte to 0 --
    // meaning "forget the round you have open; the game has walked away from
    // it".
    //
    // ADDED IN MP3, and it is HOLE 5's fix. Before it the drop was invisible
    // from the transport's side: no close(), no state change, no notification,
    // and the carrier sat holding a round the game would never finish while its
    // round counter drifted one ahead of its peer's for good. A transport that
    // has nothing to reset may leave this empty, but it may not be null --
    // every entry here is required, and a null one is refused at install.
    void (*abandon)();

    // =======================================================================
    // CONTRACT v2. Run mg16 lane MP4.
    //
    // THE FREEZE IS NOT BROKEN, IT IS SUPERSEDED ON A DECLARED BOUNDARY. v1
    // above is exactly what it was and a v1 transport still works: both entries
    // below may be NULL and comms_set_transport accepts null FOR THESE TWO
    // SPECIFICALLY, where it refuses null for every v1 entry.
    //
    // WHY AN EXTENSION AND NOT A REINTERPRETATION. The obvious move is to carry
    // sync state inside the existing exchange(): it already moves bytes both
    // ways. It cannot. exchange() moves EXACTLY kCommsBlockBytes and this
    // header's own wire-format section forbids a transport from reinterpreting
    // any of them; the only slack is the 17-byte game payload at +0x0F, which
    // is the ROM's shared per-player flag channel (GetPlayerFlagByte reads it)
    // and is far too small regardless. Widening the block would change the
    // ROM's own wire format, which is the one thing the seam exists to keep.
    //
    // THE AUX CHANNEL IS UNRELIABLE AND UNORDERED. It is UDP under the loopback
    // carrier today and UDP over the internet later. It carries WHOLE messages,
    // never fragments -- a sender that cannot fit a message in one datagram
    // must not send it. There is no ack, no retransmit, and no ordering
    // guarantee; a lost aux message is a slightly staler remote body and
    // nothing else. Anything that needs reliability does not belong here.
    //
    // ONE SOCKET, MULTIPLEXED BY MESSAGE KIND, for loopback and internet alike.
    // Ruled at the MP4 gate and the reasoning belongs with the contract:
    // internet play means NAT traversal, one socket is one NAT mapping, and a
    // second port per instance multiplies the hole-punching problem. Head-of-
    // line blocking is not a real risk at these sizes -- a four-player sync
    // message is 136 bytes, nothing fragments, and UDP datagrams are
    // independent on the wire.
    //
    // THE ORDERING RULE THAT COMES WITH IT, and it is a requirement on the
    // TRANSPORT, not advice: the input record is sent FIRST on every pump and
    // aux after it, always. The lockstep is what the game blocks on; aux is
    // what makes it look right. rung SY6 asserts the consequence by measuring
    // that input round times with sync on match sync off within noise.
    // =======================================================================

    // Send one whole aux message. Returns the bytes accepted, or 0 if the
    // message could not be sent as a single datagram. NULL if unsupported.
    int (*send_aux)(const void *buf, int len);

    // Receive one whole aux message into `buf`, at most `cap` bytes. Returns
    // the byte count, or 0 when nothing is waiting. Never blocks. NULL if
    // unsupported.
    int (*recv_aux)(void *buf, int cap);

    // 1 or 2. A transport that leaves this 0 is read as 1, so a v1 transport
    // written before this field existed is still correctly described by its
    // own zeroed storage.
    unsigned contract_version;
};

// What a transport must report to carry the sync layer.
enum : unsigned { kCommsContractV1 = 1, kCommsContractV2 = 2 };

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

// Hold the world seat until the session joins, bringing the link up the way the
// DS's multiplayer menu would (open, then request the seated role). Returns
// true if the session came up. BOUNDED: on expiry it returns false and the
// caller seats a single-player world, which is a normal boot and not a hang.
//
// It exists because data_020a0f10 -- my comms slot -- is not written until a
// round completes, so a world seated before the join has every console
// believing it is player 0.
bool comms_wait_for_session(int frames);

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

// ---------------------------------------------------------------------------
// HOST-AUTHORITATIVE STATE SYNC (run mg16 lane MP4, hal/comms_sync.cpp).
//
// A PORT LAYER, NOT A ROM ONE. It sits above the seam and outside src/
// entirely: the DS input-lockstep remains the shipped default and runs
// unchanged whether this is on or off. Design and its honest limits are in
// runs/mg16/status/MP4-DESIGN.md.
// ---------------------------------------------------------------------------
struct SyncStats {
    unsigned long long sent, recvd, dropped, applied, lerps, snaps;
    int worst_error;          // Fix12, the largest correction ever needed
};

// Decide whether the layer runs this session. Requires SM64DS_SYNC=1, an
// installed transport, and a transport reporting contract v2 WITH both aux
// entries. Anything else falls back to the DS path, loudly for the v2 case.
void sync_decide();
bool sync_enabled();
bool sync_forced_v1();
SyncStats sync_stats();
void sync_report(const char *tag);

}  // namespace port

extern "C" {
// Assert that the .dsstate$ymp3 bands came out of the linker contiguous and in
// ROM order. Returns nonzero when they did. Everything the conductor does with
// &data_020a1020 as a 0x20-byte block depends on it.
int port_comms_conductor_check_layout(void);
}

#endif  // PORT_HAL_COMMS_SEAM_H
