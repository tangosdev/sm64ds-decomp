// OWNER-AUTHORITATIVE STATE SYNC. Run mg16 lane MP4; authority reshaped by
// the mp-sync-coopdx lane (item 1).
//
// THE DESIGN IS runs/mg16/status/MP4-DESIGN.md and this file implements it,
// with one deliberate amendment that document predates: authority is PER
// BODY, not the host's. Each console publishes its OWN body -- the one it
// simulates from its own input with zero latency -- and applies every peer's
// view of that peer's body. The design doc's host-only rule corrected in one
// direction only; the host's copy of every client measured recvd=0 applied=0
// forever. Read the doc first for everything else: what this does, what it
// deliberately does NOT do, and what the correction constants are guesses
// about.
//
// WHAT THIS FILE IS NOW: the whole layer -- the enable decision, the contract
// check, the refusal, the message, the send, the apply, the readout. It was
// built in that order on purpose, with SY4 and SY0 green before any code
// existed that could move a remote body.
//
// ##########################################################################
// #  THE CORRECTION CONSTANTS ARE STILL GUESSES, BUT THE TOOL EXISTS NOW.  #
// ##########################################################################
//
// The paragraph that used to stand here named the missing tool: every loopback
// session reported lerps=0 and a worst error of about 1.26 units against the
// 2.0-unit ignore threshold, so the entire 2..60-unit LERP band -- the path
// that WILL run over the internet, and the only reason this layer exists --
// had never executed, and neither had the snap path. The SY rungs proved
// PLUMBING (built, sent, received, applied to the right body, never the wrong
// one, survives 20% loss, does not tax the lockstep) and nothing about the
// numbers.
//
// THE TOOL IS THE RIG (mp-sync-coopdx item 6), and it is exactly the one the
// old banner asked for: SM64DS_SYNC_DELAY_MS holds every received aux message
// in a FIFO for N ms, receive-side, so the correction band stretches the way
// real latency stretches it -- and a 'SYNP' ping/pong probe feeds a MEASURED
// round trip into sync_report (rtt_last/rtt_avg), so a rigged run states its
// own conditions. Tune the constants against the rig at 0/50/150 ms before
// trusting them in front of a player; the numbers below are still the
// design's starting guesses until someone does.
//
// ============================ WHERE THIS SITS ==============================
//
// OUTSIDE THE ROM-FAITHFUL PATH, entirely. Nothing in src/ knows this exists.
// The DS input-lockstep -- src/func_0203df40.c, src/func_0203ea5c.c and the
// seam under them -- runs exactly as it does today whether this is on or off,
// and remains the SHIPPED DEFAULT. This layer only ever corrects where a REMOTE
// body ended up, after the ROM has already decided it.
//
// That separation is the owner's condition and it is also what keeps a DS
// back-port possible: everything this file does is port-side, so a build for
// real hardware simply does not include it.
//
// ========================= WHAT IT IS NOT ==================================
//
// NO ROLLBACK. NO RESIMULATION. NO INPUT PREDICTION. No history buffer. Named
// here because they are what everyone reaches for next and they are a different
// architecture with a different cost. This layer sends where bodies ARE and
// nudges remote copies toward it.
//
// AND IT NEVER CORRECTS THE LOCAL PLAYER. Your own character is simulated from
// your own input with no latency and is already right; correcting it is what
// makes a netcode feel like rubber. rung SY2 asserts it.

#include "comms_seam.h"
#include "player_fields.h"

#include <windows.h>   // GetTickCount, for the delay rig and the RTT probe --
                       // kernel32 only, which every object here already
                       // imports; the static-DLL trap is about NEW libraries.

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
// SetAnim IS DECLARED void HERE, WHICH IS ITS ACTUAL RETURN TYPE.
// src/_ZN6Player7SetAnimEji5Fix12IiEj.cpp:14 defines it `void`; 74 matched
// sources declare it `extern int`. Those 74 are BYTE-VERIFIED ROM TUs and are
// not mine to edit -- and their callers all discard the result, so the lie
// costs them nothing. This declaration is the port's own, so it gets to be
// correct, and being correct here is what makes it impossible for anything in
// this file to start consuming a return value that is stack garbage.
//
// Link-compatible either way: the name is already Itanium-mangled and used as
// a plain identifier inside extern "C", so no C++ mangling participates and the
// linker sees one symbol. The earlier note here argued for matching the wrong
// declaration to avoid competing spellings; that reasoning kept a copy of
// exactly the declaration defect the first commit of this lane fixed three of.
void _ZN6Player7SetAnimEji5Fix12IiEj(void *, unsigned int, int, int, unsigned int);
void _ZN6Player11ChangeStateERNS_5StateE(char *, void *);

// The per-slot Player pointers, the count, and which slot this console is.
extern void *data_0209f394[];
extern unsigned char data_0209f21c;
extern unsigned char data_0209f250;
}

namespace port {
namespace {

// ---------------------------------------------------------------------------
// THE KNOBS. Defaults from MP4-DESIGN.md; the correction constants are
// STARTING GUESSES to be tuned against the owner's eyes, not mine.
// ---------------------------------------------------------------------------
struct SyncCfg {
    bool on;          // SM64DS_SYNC          default OFF
    int  hz;          // SM64DS_SYNC_HZ       default 30
    int  lerp_pct;    // SM64DS_SYNC_LERP     default 25
    int  snap_units;  // SM64DS_SYNC_SNAP     default 60
    bool report;      // SM64DS_SYNC_REPORT
    bool force_v1;    // SM64DS_SYNC_FORCE_V1 -- test scaffolding, see below
    int  drop_pct;    // SM64DS_SYNC_DROP -- deliberate aux loss, for rung SY5
    int  delay_ms;    // SM64DS_SYNC_DELAY_MS -- deliberate aux LATENCY; the
                      // tuning rig this file's own banner asked for
    bool no_events;   // SM64DS_SYNC_NO_EVENTS -- test scaffolding in the
                      // FORCE_V1/DROP class: disables item 3's event-triggered
                      // sends so one build can measure cadence-only ("before")
                      // against event-driven ("after") transition latency
    int  phase_frames;// SM64DS_SYNC_PHASE -- item 4's same-animation reseed
                      // threshold, in whole frames (default 8). The rig is
                      // what turns this from a guess into a number: measure
                      // phase_worst on a healthy pair at the target latency
                      // and keep the threshold above that noise floor.
};

SyncCfg g_cfg;
int      g_send_every = 2;    // frames between sends, from SM64DS_SYNC_HZ
unsigned g_frame = 0;         // this console's own tick of the sync clock
bool g_parsed = false;
bool g_enabled = false;        // the layer actually running
bool g_said_refusal = false;

SyncStats g_stats;

int env_int(const char *name, int dflt) {
    const char *s = std::getenv(name);
    if (!s || !*s) return dflt;
    const int v = std::atoi(s);
    return v;
}

void parse_cfg() {
    if (g_parsed) return;
    g_parsed = true;
    g_cfg.on         = env_int("SM64DS_SYNC", 0) != 0;
    g_cfg.hz         = env_int("SM64DS_SYNC_HZ", 30);
    g_cfg.lerp_pct   = env_int("SM64DS_SYNC_LERP", 25);
    g_cfg.snap_units = env_int("SM64DS_SYNC_SNAP", 60);
    g_cfg.report     = env_int("SM64DS_SYNC_REPORT", 0) != 0;
    g_cfg.force_v1   = env_int("SM64DS_SYNC_FORCE_V1", 0) != 0;
    /* DELIBERATE AUX LOSS, and it is test scaffolding like FORCE_V1. The aux
       channel is specified UNRELIABLE, so "it works on loopback" proves nothing
       about the internet, where loss is the normal condition rather than the
       exception. rung SY5 turns this up and requires the world to converge
       anyway. Applied on the RECEIVE side so the sender stays honest and the
       loss looks exactly like the wire eating datagrams. */
    g_cfg.drop_pct   = env_int("SM64DS_SYNC_DROP", 0);
    if (g_cfg.drop_pct < 0) g_cfg.drop_pct = 0;
    if (g_cfg.drop_pct > 99) g_cfg.drop_pct = 99;
    /* DELIBERATE AUX LATENCY -- the rig the banner at the top of this file
       spent a paragraph asking for. Loss (above) makes a remote body staler
       but leaves the error under the ignore threshold; DELAY is what actually
       stretches the correction band, because every received position is now
       describing where the sender was N milliseconds ago. Applied on the
       RECEIVE side, to every aux message alike, so a delayed pong inflates
       the measured RTT exactly the way a real network would -- the readout
       proves the rig from inside. The LOCKSTEP channel is deliberately not
       delayed: the ROM blocks on it, and stalling the game is not latency
       induction, it is a different experiment. */
    g_cfg.delay_ms   = env_int("SM64DS_SYNC_DELAY_MS", 0);
    if (g_cfg.delay_ms < 0) g_cfg.delay_ms = 0;
    if (g_cfg.delay_ms > 2000) g_cfg.delay_ms = 2000;
    g_cfg.no_events  = env_int("SM64DS_SYNC_NO_EVENTS", 0) != 0;
    g_cfg.phase_frames = env_int("SM64DS_SYNC_PHASE", 8);
    if (g_cfg.phase_frames < 1) g_cfg.phase_frames = 1;
    if (g_cfg.phase_frames > 120) g_cfg.phase_frames = 120;
    if (g_cfg.hz < 1) g_cfg.hz = 1;
    if (g_cfg.hz > 60) g_cfg.hz = 60;
    if (g_cfg.lerp_pct < 1) g_cfg.lerp_pct = 1;
    if (g_cfg.lerp_pct > 100) g_cfg.lerp_pct = 100;
    /* frames between sends at a 60 Hz frame loop; 30 Hz is every other frame */
    g_send_every = 60 / g_cfg.hz;
    if (g_send_every < 1) g_send_every = 1;
}

}  // namespace

// ---------------------------------------------------------------------------
// THE ENABLE DECISION, and the refusal that is rung SY0.
//
// THREE THINGS HAVE TO BE TRUE and each failure is silent-by-default except the
// one that is a surprise:
//
//   the knob is on          -- SM64DS_SYNC=1. Default off means an unset
//                              environment gets the DS path, which is what
//                              makes SY4 (off is byte-identical) meaningful.
//   a transport is installed-- solo has nobody to sync with.
//   THE TRANSPORT IS v2     -- it must actually be able to carry aux messages.
//
// THE THIRD ONE IS THE INTERESTING FAILURE and it gets a loud line, once. A
// transport that reports v1, or reports v2 and leaves an aux entry null, cannot
// carry sync, and the honest thing is to say so and fall back to the DS path
// rather than to half-enable and produce a session where remote bodies
// mysteriously never correct. The seam INSTALLS such a transport happily -- the
// aux entries are the optional half of the contract -- so this is the only
// place the difference is noticed.
// ---------------------------------------------------------------------------
bool sync_enabled() { return g_enabled; }

void sync_decide() {
    parse_cfg();
    g_enabled = false;

    if (!g_cfg.on) return;                        // the default, silently

    const CommsTransport *t = comms_transport();
    if (!t) {
        if (!g_said_refusal) {
            g_said_refusal = true;
            std::fprintf(stderr,
                         "[sync] SM64DS_SYNC=1 but no transport is installed; "
                         "nothing to synchronise. Running the ROM's own solo "
                         "path.\n");
        }
        return;
    }

    // A transport that predates the field leaves it 0, and 0 means v1 -- its
    // own zeroed storage describes it correctly, which is why the contract
    // spells that out rather than requiring every v1 transport to be edited.
    unsigned ver = t->contract_version ? t->contract_version : kCommsContractV1;
    if (g_cfg.force_v1) ver = kCommsContractV1;   // test scaffolding, below

    if (ver < kCommsContractV2 || !t->send_aux || !t->recv_aux) {
        if (!g_said_refusal) {
            g_said_refusal = true;
            std::fprintf(stderr,
                         "[sync] REFUSED: transport '%s' reports contract v%u "
                         "%s, so it cannot carry state sync. FALLING "
                         "BACK TO THE DS INPUT-LOCKSTEP PATH, "
                         "which is complete and is the shipped default -- this "
                         "is a downgrade, not a failure.\n",
                         t->name ? t->name : "(unnamed)", ver,
                         (ver < kCommsContractV2)
                             ? "which is below the v2 this layer needs"
                             : "and is missing an aux entry");
        }
        return;
    }

    if (!g_enabled) {
        g_enabled = true;
        std::fprintf(stderr,
                     "[sync] enabled on transport '%s' (contract v%u): %d Hz, "
                     "lerp %d%%/frame, snap over %d units. Each console is "
                     "authoritative for its OWN body and corrects only remote "
                     "ones; local input and the DS lockstep are unchanged.\n",
                     t->name ? t->name : "(unnamed)", ver, g_cfg.hz,
                     g_cfg.lerp_pct, g_cfg.snap_units);
        if (g_cfg.delay_ms > 0)
            std::fprintf(stderr,
                         "[sync] RIG: SM64DS_SYNC_DELAY_MS=%d -- every "
                         "received aux message is held %d ms before it is "
                         "processed. This session's corrections are measuring "
                         "induced latency, not the wire.\n",
                         g_cfg.delay_ms, g_cfg.delay_ms);
    }
}

// SM64DS_SYNC_FORCE_V1=1 makes this layer treat any transport as v1.
//
// IT IS TEST SCAFFOLDING AND IT IS SAID OUT LOUD. rung SY0 has to prove the
// refusal path, and the only transport this tree has is v2 -- so without a way
// to present a v1 transport, SY0 could only be written by shipping a second
// crippled carrier, which is more code and less honest. This knob makes the
// refusal reachable from the outside without a fake transport existing in the
// build.
bool sync_forced_v1() { parse_cfg(); return g_cfg.force_v1; }

// ===========================================================================
// THE MESSAGE -- v1, and it is SMALLER than MP4-DESIGN.md specced.
//
// The design's SyncPlayer carries anim_id, anim_frame and state_id. It does not
// here, because hal/player_fields.h has no confirmed offsets for them and the
// rule this lane keeps is that a named gap beats a guessed offset: a guessed
// offset produces plausible corruption, which is far worse than a body whose
// position is right and whose pose is briefly its own. Approved at the gate
// under the design's own versioning rule -- the pose fields arrive in v2 when
// the matched setters have named the storage.
//
// SIZE: 12 + 16 * players. Four players is 76 bytes, far under any MTU, so the
// contract's one-datagram rule is satisfied with room to spare.
// ===========================================================================
#pragma pack(push, 1)
struct SyncPlayerV1 {
    unsigned char  slot;      // 0..3
    unsigned char  flags;     // bit0 live, bit2 teleport (see kFlag*)
    short          yaw;       // facing, player_fields::facing
    int            x, y, z;   // Fix12, player_fields::pos_*

    // ---- v2, THE POSE FIELDS ------------------------------------------
    // Offsets and semantics from runs/mg16/status/POSEFIELDS.md, which is the
    // authority and supersedes MP4-DESIGN.md's sketch where they disagree.
    unsigned short anim_id;    // player_fields::anim_id, already unscaled
    // The state's Main-function address: an OBSERVATION id, carried for
    // diagnosis and NOT applied. See player_fields.h's banner -- the port does
    // not preserve the ROM's contiguous state table, so there is no safe
    // decode back to a State *. u32 because it is an address, not an index.
    unsigned       state_id;
    // THE CURSOR IS 20.12 FIXED POINT AND s32, NOT the design's s16. A
    // 100-frame animation reaches 409600 in 20.12, which overflows a short by
    // two orders of magnitude -- the design's field would have wrapped every
    // animation past frame 8. Four bytes rather than lose the whole value.
    int            anim_frame;
};
struct SyncMsgV1 {
    unsigned       magic;     // kSyncMagic -- framing, never changes
    unsigned       version;   // kSyncVersion -- payload shape
    // THE SENDER'S OWN SEND COUNTER, not the session frame MP4-DESIGN.md's
    // sketch called for. Named `seq` because that is what it is: it increments
    // once per send on its sender and means nothing on any other console. The
    // design wanted a SESSION frame so a receiver could tell how stale a
    // message was in game-time; that needs the comms frame counter
    // (data_020a1040+0) and is a later field. Nothing reads this today -- it
    // is carried so the wire format has the slot when someone does.
    unsigned       seq;
    unsigned char  count;
    unsigned char  pad[3];
    // SyncPlayerV1 follows, `count` of them
};
#pragma pack(pop)

// THE FRAMING TAG IS STABLE AND THE VERSION IS A FIELD, and an earlier
// revision had them as the same four bytes. That was wrong in a way worth
// keeping: hal/comms_loopback.cpp's drain() recognises an aux datagram by this
// tag, so bumping the tag for a payload change silently UNFRAMED the channel --
// the host sent 247 messages and the child received 0, with every one of them
// charged to the carrier's drop counter. Transport framing and payload
// versioning are different jobs and coupling them breaks the transport on every
// bump.
//
// So: 'S','Y','N','1' means "this datagram is a sync message" FOREVER, and
// kSyncVersion says what shape it is. A peer running the other version is
// recognised, counted, and DROPPED at the version check -- no half-understood
// message reaches the apply path, which is what the unreliable channel needs.
enum : unsigned { kSyncMagic = 0x314e5953u };
enum : unsigned { kSyncVersion = 2u };
enum : unsigned char { kFlagLive = 1, kFlagGrounded = 2, kFlagTeleport = 4 };
enum : int { kSyncBufBytes = 256 };

// ===========================================================================
// THE TUNING RIG (mp-sync-coopdx item 6): latency induction + a measured RTT.
//
// The banner at the top of this file named the missing tool: nothing could
// stretch the 2..60-unit correction band on loopback, so the band had never
// executed and every constant in it was an unexercised guess. The rig is two
// small things:
//
//   SM64DS_SYNC_DELAY_MS   every aux message this layer receives is held in a
//                          FIFO for N ms after arrival before it is processed.
//                          Receive-side, so the sender stays honest and the
//                          hold looks exactly like wire latency. 0 (the
//                          default) short-circuits to today's behaviour.
//
//   'SYNP'/'SYNQ' probes   each console sends a 12-byte probe every ~500 ms;
//                          the peer echoes it back addressed to the pinger;
//                          the round trip lands in sync_report as rtt_last/
//                          rtt_avg. Under the delay rig the probes are delayed
//                          like everything else, so rtt reads ~2N ms -- the
//                          readout is the proof the rig is on.
//
// A SEPARATE MESSAGE KIND, NOT A SNAPSHOT FIELD, and the queue was fixed
// first: the carrier's aux queue was one message deep for ALL kinds, so a
// probe could be silently superseded by a snapshot in the same pump window.
// hal/comms_loopback.cpp's drain() now keeps one-deep slots PER (SENDER,
// KIND), which is what makes a second kind safe to bolt on at all. The tag is
// framing and never changes; a payload change here bumps kPingVersion-shaped
// fields, not the tag -- same discipline as kSyncMagic/kSyncVersion above.
//
// THE PONG IS ITS OWN TAG, not a kind byte under the ping's, and the first
// rig session is why: with one tag, a peer's echo of our probe shared a queue
// slot with that peer's own next probe, and at delay 0 the two are
// phase-locked to the frame boundary -- roughly half the RTT samples died to
// newest-wins (37 of 80 one way, 52 of 80 the other, measured). The kind byte
// in the struct stays authoritative for the payload; the tag exists so the
// carrier's slots cannot let one supersede the other.
// ===========================================================================
enum : unsigned { kPingMagic = 0x504e5953u };   // 'S','Y','N','P'
enum : unsigned { kPongMagic = 0x514e5953u };   // 'S','Y','N','Q'
enum : unsigned char { kPingKindPing = 0, kPingKindPong = 1 };

#pragma pack(push, 1)
struct SyncPingV1 {
    unsigned      magic;       // kPingMagic -- framing, never changes
    unsigned char kind;        // kPingKindPing or kPingKindPong
    unsigned char from_slot;   // the sender's comms slot
    unsigned char to_slot;     // pong: the pinger this echo is addressed to.
                               // send_aux fans a message to EVERY live peer,
                               // so a pong must name its pinger or a third
                               // console would compute an RTT from a probe it
                               // never sent. 0xff on a ping (broadcast).
    unsigned char pad;
    unsigned      t_send_ms;   // pinger's GetTickCount at send, echoed back
};
#pragma pack(pop)

enum : int { kDelayRing = 32 };   // ~1 s of aux at the default 30 Hz send rate
struct DelayedAux {
    unsigned      t_due;          // GetTickCount when this may be processed
    int           len;
    unsigned char buf[kSyncBufBytes];
};
namespace {
DelayedAux g_delayq[kDelayRing];  // FIFO: constant delay keeps due times sorted
int      g_dq_head = 0;
int      g_dq_count = 0;
unsigned g_last_ping_ms = 0;

// ---------------------------------------------------------------------------
// EVENT-TRIGGERED SENDS (mp-sync-coopdx item 3, idea from sm64coopdx's
// action-change trigger, idea only). A fixed 30 Hz cadence means a transition
// -- jump, land, sleep, wake -- waits up to a full cadence interval before
// the peers hear about it, on top of the wire. So a change of the local
// body's anim id or grounded flag publishes a snapshot THAT frame, and the
// cadence remains the floor underneath.
//
// BUDGETED, newest-change-wins-nothing: a token bucket of kEvBurst refilled
// one per 30 frames caps event sends at ~2/s sustained with a small burst
// allowance, so a flapping grounded bit (stairs, slopes) degrades to the
// cadence instead of flooding the channel. A change that finds the bucket
// empty is not lost -- the next cadence send carries it, at most two frames
// later at the default rate.
// ---------------------------------------------------------------------------
enum : int { kEvBurst = 4 };
int            g_ev_tokens = kEvBurst;
bool           g_ev_seeded = false;
unsigned short g_ev_anim = 0;
unsigned char  g_ev_ground = 0;

// Receiver-side transition arrivals, per slot, for the latency measurement:
// the anim-arrive line fires when a received entry's anim id differs from the
// last one received for that slot -- the moment the WIRE delivered the
// transition, whether or not the world needed correcting (on a healthy
// deterministic pair the peer's own sim has already made the change, so
// apply_pose no-ops and wire arrival is the only measurable edge).
bool           g_arr_seen[kCommsMaxPlayers];
unsigned short g_arr_anim[kCommsMaxPlayers];

// Item 4's per-slot wire-side animation-length estimate: the high-water
// received cursor for the id currently playing on that slot. Reset on id
// change. See the wrap note in apply_pose.
unsigned short g_ph_id[kCommsMaxPlayers];
int            g_ph_hw[kCommsMaxPlayers];
}  // namespace

// One DS unit is 4096 in Fix12. The thresholds are in units and converted here
// so the constants read the way the design note states them.
inline int units(int n) { return n * 4096; }

// ---------------------------------------------------------------------------
// THE POSE, APPLIED THROUGH THE ROM'S OWN FACES. Never by raw store.
//
// runs/mg16/status/POSEFIELDS.md settled this and the reasons are not stylistic:
//
//   Player::SetAnim does REFCOUNTED FILE I/O for the animation, forces the
//   cursor reset that makes the following ModelAnim::SetAnim take its slow
//   path, drives the eye TextureSequence, and sets a refusal flag
//   (mStateFlags |= 0x80) that Player::Behavior REACTS TO by forcing a state
//   change. A raw store to +0x63c gets none of that and leaves the object
//   describing an animation whose file was never loaded.
//
//   Player::ChangeState is a TRANSACTION: two veto paths, a silent target
//   substitution while the player is holding something, roughly twenty field
//   resets, and a camera side effect. A raw store to +0x370 skips all of it.
//
// AND THE CAMERA SIDE EFFECT IS THE SECOND LOAD-BEARING REASON FOR THE
// NEVER-CORRECT-LOCAL RULE. ChangeState's camera work is gated on the locally
// viewed player, so applying a state to the LOCAL body would yank the player's
// own camera once per packet. The rule already existed for feel; it now also
// exists for that, and apply_snapshot's `slot == me` skip is what enforces both.
// ---------------------------------------------------------------------------
void apply_pose(void *a, const SyncPlayerV1 *e) {
    // ---- STATE IS NOT APPLIED, and that is a measured decision rather than
    // caution. POSEFIELDS.md's 0..77 encoding needs the ROM's contiguous state
    // table; this port hosts those 78 records as separate objects and the
    // linker scatters them, so a received id cannot be range-checked back into
    // a State *. ChangeState on a wrong pointer is a call through a garbage
    // pointer-to-member -- an arbitrary jump. player_fields.h::state_from_id
    // refuses by construction, and the field is carried for diagnosis only.
    //
    // The animation below is what the owner actually sees, and it applies
    // safely because SetAnim takes an ID and validates it itself.

    // ---- ANIMATION. On an id CHANGE, seeded through SetAnim's own
    // startFrame; on a SAME-ID PHASE FORK past a threshold, RESEEDED through
    // the same face (mp-sync-coopdx item 4).
    //
    // THE CURSOR IS NEVER WRITTEN DIRECTLY. ModelAnim+0x58 is live machinery:
    // Animation::WillHitFrame tests whether [currFrame, currFrame + speed)
    // crosses a given frame, which is how the ROM fires footsteps, hitboxes
    // and animation-timed sounds -- a raw store can skip such an event or
    // fire it twice (spec trap 2). Player::SetAnim with a startFrame is the
    // legal road: it forces the cursor reset that makes the following
    // ModelAnim::SetAnim take its slow path, so the startFrame lands even for
    // the same file (POSEFIELDS.md; the same-file fast path only ignores
    // startFrame when the cursor reset is absent).
    //
    // The old note here left the same-id half undone because the threshold
    // for "large drift" had no measured value and the latency tool to measure
    // it did not exist. The tool exists now (item 6), the threshold is
    // SM64DS_SYNC_PHASE (default 8 frames), and the rig's phase_worst readout
    // is what keeps it above the healthy-pair noise floor.
    if (e->anim_id != player::anim_id(a)) {
        // Argument shape copied from a real call site rather than guessed:
        // src/_ZN6Player11St_Fly_InitEv.cpp:26 is
        // SetAnim(this, 0x49, 0, 0x1000, 0) -- id, flags, speed 1.0 in Fix12,
        // startFrame. The received cursor is 20.12, so its integer frame is
        // >> 12.
        const unsigned start = (unsigned)(e->anim_frame >> 12);
        _ZN6Player7SetAnimEji5Fix12IiEj(a, e->anim_id, 0, 0x1000, start);
        // Declared void above, so there is no return value to discard and no
        // way for a later edit here to start reading an unwritten slot.
        if (g_cfg.report)
            /* GetTickCount is machine-wide, so on a one-machine rig this line
               and the sender's anim-event line are on ONE clock: their delta
               is the measured transition latency. */
            std::fprintf(stderr, "[sync] anim-apply slot=%u id=%u t=%u\n",
                         (unsigned)e->slot, (unsigned)e->anim_id,
                         GetTickCount());
        return;
    }

    // ---- SAME ID: the phase check, two corrections deep before it dares
    // compare anything:
    //
    //   AGE: the received cursor is one-way-stale, so it is advanced at
    //   nominal speed 1.0 first -- without that, any latency past the
    //   threshold makes every snapshot look like a fork and the reseed loops,
    //   planting the cursor a latency behind each time. rtt/2 in ms becomes
    //   20.12 frames as ms * 4096 / (1000/60) ~= 246. Approximate (anims can
    //   play off-1.0 speeds), which a THRESHOLD absorbs.
    //
    //   WRAP: looping animations reset their cursor, so a plain delta reads
    //   ~one animation length once per loop and would reseed spuriously on a
    //   perfectly healthy pair. The wire itself supplies a length estimate --
    //   the HIGH-WATER received cursor for the current id -- and the delta is
    //   taken modulo that. No new Player offsets involved (the named-gap rule
    //   stands); the estimate undershoots until one full loop has been
    //   observed, which the threshold absorbs the same way.
    int *cursor = player::anim_frame_ptr(a);   // read-only use; see trap 2
    if (!cursor) return;
    /* No RTT sample yet means no age correction, and an uncorrected
       comparison at any real latency reads as a fork that is not there --
       measured: the only reseeds in a default-threshold rig session were in
       the first half-second, before the first pong landed. The probe is ~2 Hz
       so the blind window is short; a session-boot fork the window hides is
       caught by the first check after it closes. */
    if (g_stats.rtt_avg_ms == 0) return;
    const int slot = e->slot;
    if (g_ph_id[slot] != e->anim_id) {
        g_ph_id[slot] = e->anim_id;
        g_ph_hw[slot] = 0;
    }
    if (e->anim_frame > g_ph_hw[slot]) g_ph_hw[slot] = e->anim_frame;

    const int age_2012 = (g_stats.rtt_avg_ms / 2) * 246;
    int est = e->anim_frame + age_2012;
    const int hw = g_ph_hw[slot];
    if (hw > (2 << 12) && est > hw) est %= hw;
    int d = est - *cursor;
    if (d < 0) d = -d;
    if (hw > (2 << 12)) {
        const int ph = d % hw;
        d = ph < hw - ph ? ph : hw - ph;       // circular distance
    }
    if (d > g_stats.phase_worst) g_stats.phase_worst = d;
    /* THE THRESHOLD CARRIES THE AGE CORRECTION'S OWN UNCERTAINTY. The
       correction assumed speed 1.0 and real anims play off it -- a full-run
       cycle near 2.0 leaves a residual of about age * (speed - 1), which at
       360 ms simulated RTT measured 8-11 frames and tripped a fixed 8-frame
       threshold on a perfectly healthy pair (clustered in the first second
       of each new cycle, before the wrap estimate matures). So the slack
       scales with the thing that causes it: base + age. A real fork grows
       without bound and crosses any such line within a second; a latency
       artifact never leaves the band. */
    if (d > (g_cfg.phase_frames << 12) + age_2012) {
        /* Reseed at the age-corrected frame, through the ROM's own face. */
        _ZN6Player7SetAnimEji5Fix12IiEj(a, e->anim_id, 0, 0x1000,
                                        (unsigned)(est >> 12));
        ++g_stats.reseeds;
        if (g_cfg.report)
            std::fprintf(stderr,
                         "[sync] phase-reseed slot=%u id=%u drift=%d frames "
                         "t=%u\n",
                         (unsigned)e->slot, (unsigned)e->anim_id, d >> 12,
                         GetTickCount());
    }
}

// ---------------------------------------------------------------------------
// THE SEND SIDE -- every console, its OWN body only. mp-sync-coopdx item 1.
//
// AUTHORITY IS THE OWNER'S. The console whose player a body is simulates that
// body from its own input with zero latency, so its view of that one body is
// the best view that exists anywhere -- and it publishes exactly that one.
// Nobody ever asserts a view of a body it does not own, so there are never
// two authorities over one actor; the fight the old host-only rule guarded
// against cannot happen under this rule either, and this rule also closes the
// direction the old one left open. MEASURED, that direction: in every
// pre-item-1 session the host's counters read recvd=0 applied=0 -- on the
// host's screen a remote body got ZERO corrections, ever, because only the
// host sent and the host discarded. Half of "the two screens disagree" lived
// in exactly that half of the wire.
//
// (The idea is sm64coopdx's -- each client authoritative for its own Mario --
// taken as an idea only; their netcode is unlicensed and their whole
// architecture solves a determinism problem our lockstep does not have. The
// slice adopted here is the authority rule, nothing else.)
//
// ORDERING: this runs AFTER the conductor's exchange has returned for the
// frame, so the input record is always on the wire first. That is the
// contract's ordering rule and it is satisfied by call position rather than by
// a comment -- rung SY6 measures the consequence.
// ---------------------------------------------------------------------------
void sync_send_own() {
    if (!g_enabled) return;
    const CommsTransport *t = comms_transport();
    if (!t || !t->send_aux) return;

    /* MY body is the one at MY world slot -- data_0209f250, the same index
       apply_snapshot's never-local skip reads, so the sender's "mine" and the
       receiver's "not mine" can never disagree about which body that is. */
    const int me = (int)data_0209f250;
    if (me < 0 || me >= kCommsMaxPlayers) return;
    void *a = data_0209f394[me];
    if (!a) return;

    ++g_frame;
    if (g_frame % 30u == 0 && g_ev_tokens < kEvBurst) ++g_ev_tokens;

    /* Item 3: a transition of the local body publishes NOW, budget allowing;
       the cadence below stays the floor. Detection is edge-triggered off the
       last OBSERVED pair, seeded on the first frame so boot state is not
       itself an event. */
    const unsigned short anim_now = player::anim_id(a);
    const unsigned char  ground_now = player::on_ground(a);
    const bool changed = g_ev_seeded &&
                         (anim_now != g_ev_anim || ground_now != g_ev_ground);
    bool event = false;
    if (changed && !g_cfg.no_events && g_ev_tokens > 0) {
        --g_ev_tokens;
        event = true;
        ++g_stats.evsends;
    }
    if (changed && g_cfg.report)
        /* Logged on DETECTION, whether or not an event send follows, so a
           cadence-only arm (SM64DS_SYNC_NO_EVENTS) produces the same pairing
           line and the receiver's anim-apply minus this is the measured
           transition latency in both arms. One machine, one GetTickCount. */
        std::fprintf(stderr,
                     "[sync] anim-change slot=%d id=%u grounded=%d send=%s "
                     "t=%u\n",
                     me, anim_now, (int)ground_now,
                     event ? "now" : "cadence", GetTickCount());
    g_ev_seeded = true;
    g_ev_anim = anim_now;
    g_ev_ground = ground_now;

    const bool cadence = (g_frame % (unsigned)g_send_every) == 0;
    if (!cadence && !event) return;

    unsigned char buf[kSyncBufBytes];
    SyncMsgV1 *m = (SyncMsgV1 *)buf;
    m->magic = kSyncMagic;
    m->version = kSyncVersion;
    m->seq = g_frame;
    m->count = 1;
    m->pad[0] = m->pad[1] = m->pad[2] = 0;

    SyncPlayerV1 *e = (SyncPlayerV1 *)(buf + sizeof(SyncMsgV1));
    e->slot  = (unsigned char)me;
    e->flags = (unsigned char)(kFlagLive |
                               (player::on_ground(a) ? kFlagGrounded : 0));
    e->yaw   = *player::facing(a);
    e->x     = *player::pos_x(a);
    e->y     = *player::pos_y(a);
    e->z     = *player::pos_z(a);
    e->anim_id    = player::anim_id(a);
    e->state_id   = player::state_id(a);
    e->anim_frame = player::anim_frame(a);

    const int len = (int)sizeof(SyncMsgV1) + (int)sizeof(SyncPlayerV1);
    if (t->send_aux(buf, len) == len) ++g_stats.sent;
}

// ---------------------------------------------------------------------------
// THE APPLY SIDE -- every console, REMOTE bodies only. (Item 1: the host
// stopped being a special case; it applies its peers' own-body snapshots like
// anyone else.)
//
// THE LOCAL BODY IS NEVER TOUCHED, and that is the single most important line
// in this file. Your own character is simulated from your own input with no
// latency and is already right; correcting it against a message describing
// where the host thought you were N milliseconds ago is what makes a netcode
// feel like rubber. rung SY2 asserts it by injecting authority that disagrees
// with the client's own body and requiring the body not to move.
//
// THE POLICY, from MP4-DESIGN.md, constants tuneable and deliberately guesses:
//   under 2 units    ignore   -- below visible
//   2 .. snap        LERP     -- 25% of the error per frame, ~4 frames to land
//   over snap        SNAP     -- and COUNTED, because a snap is a bug report
//
// RESHAPED BY THE RIG (item 6): the recv, the deliberate loss, and the delay
// queue all live in sync_recv_pump below now; this function is handed one
// whole already-due 'SYN1' datagram and does only the checking and the
// applying. The one-message-per-tick note that used to sit here is obsolete
// -- the pump drains everything pending each tick, because the carrier's
// per-(sender, kind) slots can legitimately hold several messages at once.
// ---------------------------------------------------------------------------
void apply_snapshot(const unsigned char *buf, int n) {
    /* EVERY console applies now -- mp-sync-coopdx item 1 removed the
       `slot() == 0` early-out that made the host discard everything. What
       protects the local body is not that gate and never really was: it is
       the `slot == me` skip in the loop below, which survives this refactor
       and every future one (spec trap 3). */
    if (n < (int)sizeof(SyncMsgV1)) { ++g_stats.dropped; return; }
    const SyncMsgV1 *m = (const SyncMsgV1 *)buf;
    if (m->magic != kSyncMagic) { ++g_stats.dropped; return; }
    if (m->version != kSyncVersion) {
        /* A peer on a different message shape. Recognised as ours, counted, and
           dropped -- never parsed. Said once so a version skew is diagnosable
           from a playlog instead of looking like packet loss. */
        static bool said;
        if (!said) {
            said = true;
            std::fprintf(stderr,
                         "[sync] peer is sending message version %u and this "
                         "build speaks %u; dropping its state. Positions will "
                         "not correct. Both sides need the same build.\n",
                         m->version, kSyncVersion);
        }
        ++g_stats.dropped;
        return;
    }
    const int want = (int)sizeof(SyncMsgV1) + m->count * (int)sizeof(SyncPlayerV1);
    if (n < want) { ++g_stats.dropped; return; }
    ++g_stats.recvd;

    const int me = (int)data_0209f250;

    /* THE LOCAL-BODY WITNESS. Read the local player's fields before the entry
       loop and compare after: the frame loop is single-threaded, so a change
       across this window can only be this function writing the local body.
       rungSY2 asserts the counter stays 0 under live corrections, which is a
       mechanism-level probe -- its predecessor (sync-on vs sync-off trajectory
       equality) turned out to measure the SIM instead: the two bodies spawn
       overlapping, the pushback that separates them runs while early
       corrections move the remote copy, and a standing session came out 2.49
       units apart at frame 34 with zero input and zero local writes. */
    void *a_me = (me >= 0 && me < kCommsMaxPlayers) ? data_0209f394[me] : 0;
    int wx = 0, wy = 0, wz = 0;
    short wyaw = 0;
    if (a_me) {
        wx = *player::pos_x(a_me);
        wy = *player::pos_y(a_me);
        wz = *player::pos_z(a_me);
        wyaw = *player::facing(a_me);
    }

    const SyncPlayerV1 *e = (const SyncPlayerV1 *)(buf + sizeof(SyncMsgV1));
    for (int i = 0; i < (int)m->count; ++i, ++e) {
        const int slot = e->slot;
        if (slot < 0 || slot >= kCommsMaxPlayers) continue;
        if (slot == me) {
            /* NEVER the local body -- spec trap 3, the line every refactor
               must keep. Under owner authority nobody publishes another
               console's body, so an entry naming OUR slot is a peer claiming
               authority it does not have; counted so rungSY2 can assert the
               count stays zero rather than trusting the skip silently. */
            ++g_stats.own_claims;
            continue;
        }
        void *a = data_0209f394[slot];
        if (!a) continue;

        if (g_cfg.report) {
            if (g_arr_seen[slot] && e->anim_id != g_arr_anim[slot])
                std::fprintf(stderr,
                             "[sync] anim-arrive slot=%d id=%u t=%u\n",
                             slot, (unsigned)e->anim_id, GetTickCount());
            g_arr_seen[slot] = true;
            g_arr_anim[slot] = e->anim_id;
        }

        int *px = player::pos_x(a);
        int *py = player::pos_y(a);
        int *pz = player::pos_z(a);
        const int dx = e->x - *px, dy = e->y - *py, dz = e->z - *pz;
        int err = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy) + (dz < 0 ? -dz : dz);
        if (err > g_stats.worst_error) g_stats.worst_error = err;

        if (err < units(2)) {
            /* Below visible. Applying the POSITION would be jitter, not
               correction -- but the POSE still applies. Found by item 3's
               latency rig: this branch used to `continue` above apply_pose,
               so a body whose position matched while its pose had forked
               (which is the shape of the sleep-divergence complaint: both
               bodies standing still, one asleep) could NEVER have its
               animation corrected -- the safety net existed only for bodies
               that were also in the wrong place. apply_pose is id-change-
               gated, so on a healthy pair this is a comparison and nothing
               else. */
            ++g_stats.applied;
            *player::facing(a) = e->yaw;
            apply_pose(a, e);
            continue;
        }
        if (err > units(g_cfg.snap_units) || (e->flags & kFlagTeleport)) {
            *px = e->x; *py = e->y; *pz = e->z;
            /* A teleport is a real warp and is not a bug; anything else this
               far out is, and is counted so rung SY3 can insist on zero. */
            if (!(e->flags & kFlagTeleport)) ++g_stats.snaps;
        } else {
            *px += dx * g_cfg.lerp_pct / 100;
            *py += dy * g_cfg.lerp_pct / 100;
            *pz += dz * g_cfg.lerp_pct / 100;
            ++g_stats.lerps;
        }
        *player::facing(a) = e->yaw;
        apply_pose(a, e);
        ++g_stats.applied;
    }

    if (a_me && (wx != *player::pos_x(a_me) || wy != *player::pos_y(a_me) ||
                 wz != *player::pos_z(a_me) || wyaw != *player::facing(a_me))) {
        ++g_stats.local_writes;
        static bool said;
        if (!said) {
            said = true;
            std::fprintf(stderr,
                         "[sync] BUG: the local body (slot %d) changed across "
                         "apply_snapshot. The layer wrote the local player, "
                         "which it must never do. Counted in local_writes; "
                         "said once.\n", me);
        }
    }
}

// ---------------------------------------------------------------------------
// THE PING HANDLER. A ping is answered with a pong echoing the pinger's own
// timestamp, addressed to the pinger's slot; a pong addressed to us closes the
// loop and lands in the stats. Clock arithmetic is all in the PINGER's
// GetTickCount domain -- the echo is opaque to the peer -- so nothing here
// assumes the two consoles share a clock, even though on loopback they do.
// ---------------------------------------------------------------------------
void handle_ping(const unsigned char *buf, int n) {
    if (n < (int)sizeof(SyncPingV1)) { ++g_stats.dropped; return; }
    const SyncPingV1 *p = (const SyncPingV1 *)buf;
    const CommsTransport *t = comms_transport();
    if (!t || !t->send_aux) return;

    if (p->kind == kPingKindPing) {
        SyncPingV1 r;
        r.magic     = kPongMagic;
        r.kind      = kPingKindPong;
        r.from_slot = (unsigned char)t->slot();
        r.to_slot   = p->from_slot;
        r.pad       = 0;
        r.t_send_ms = p->t_send_ms;             // echoed, never interpreted
        t->send_aux(&r, (int)sizeof r);
        return;
    }
    if (p->kind == kPingKindPong) {
        if (p->to_slot != (unsigned char)t->slot()) return;  // not our echo
        const int rtt = (int)(GetTickCount() - p->t_send_ms);
        g_stats.rtt_last_ms = rtt;
        g_stats.rtt_avg_ms  = g_stats.rtt_avg_ms
                                  ? (g_stats.rtt_avg_ms * 7 + rtt) / 8
                                  : rtt;
        ++g_stats.pongs;
    }
}

// One whole aux message, already past the delay rig, told apart by its tag.
void process_aux(const unsigned char *buf, int n) {
    unsigned tag = 0;
    if (n >= 4) std::memcpy(&tag, buf, 4);
    if (tag == (unsigned)kPingMagic || tag == (unsigned)kPongMagic) {
        handle_ping(buf, n);
        return;
    }
    apply_snapshot(buf, n);
}

// ---------------------------------------------------------------------------
// THE RECEIVE PUMP -- both roles. Drains EVERYTHING the transport has pending
// (the carrier's per-(sender, kind) slots can hold several messages), applies
// the deliberate-loss knob, and either processes each message now (delay 0,
// the default -- byte-identical behaviour to the pre-rig layer) or holds it
// in the FIFO until its due time. The host runs this too: pongs come back on
// aux, and per-body authority (item 1) makes the host a snapshot receiver.
// ---------------------------------------------------------------------------
void sync_recv_pump() {
    const CommsTransport *t = comms_transport();
    if (!t || !t->recv_aux) return;
    const unsigned now = GetTickCount();

    unsigned char buf[kSyncBufBytes];
    int n;
    while ((n = t->recv_aux(buf, sizeof buf)) > 0) {
        if (g_cfg.drop_pct > 0) {
            /* A cheap deterministic-per-run LCG, not rand(): a proof that
               behaves differently every run is not a proof. */
            static unsigned r = 0x12345678u;
            r = r * 1664525u + 1013904223u;
            if ((int)((r >> 16) % 100u) < g_cfg.drop_pct) {
                ++g_stats.dropped;
                continue;
            }
        }
        if (g_cfg.delay_ms <= 0) {
            process_aux(buf, n);
            continue;
        }
        if (g_dq_count == kDelayRing) {
            /* Full: the OLDEST entry is the one closest to stale anyway.
               Counted as dropped, because a rig that silently sheds load
               would tune constants against traffic that never arrived. */
            g_dq_head = (g_dq_head + 1) % kDelayRing;
            --g_dq_count;
            ++g_stats.dropped;
        }
        DelayedAux &d = g_delayq[(g_dq_head + g_dq_count) % kDelayRing];
        d.t_due = now + (unsigned)g_cfg.delay_ms;
        d.len = n;
        std::memcpy(d.buf, buf, (size_t)n);
        ++g_dq_count;
    }

    /* Release everything that has served its sentence. Unconditional, so a
       queue drained after the knob is lowered still empties. */
    while (g_dq_count > 0) {
        DelayedAux &d = g_delayq[g_dq_head];
        if ((int)(now - d.t_due) < 0) break;    // FIFO: later entries due later
        process_aux(d.buf, d.len);
        g_dq_head = (g_dq_head + 1) % kDelayRing;
        --g_dq_count;
    }
}

// The RTT probe, ~2 Hz, wall-clock scheduled so it is independent of frame
// rate and of the snapshot cadence. Sent AFTER the snapshot in the tick so it
// can never delay state.
void sync_send_ping() {
    const CommsTransport *t = comms_transport();
    if (!t || !t->send_aux) return;
    const unsigned now = GetTickCount();
    if (g_last_ping_ms && (unsigned)(now - g_last_ping_ms) < 500u) return;
    g_last_ping_ms = now;
    SyncPingV1 p;
    p.magic     = kPingMagic;
    p.kind      = kPingKindPing;
    p.from_slot = (unsigned char)t->slot();
    p.to_slot   = 0xff;                          // a ping is a broadcast
    p.pad       = 0;
    p.t_send_ms = now;
    if (t->send_aux(&p, (int)sizeof p) == (int)sizeof p) ++g_stats.pings;
}

void sync_tick() {
    if (!g_enabled) return;
    sync_recv_pump();      // take what has arrived (and is due) first
    sync_send_own();       // then publish OUR body, after the input record
    sync_send_ping();      // and the rig's probe last, behind the state
}


SyncStats sync_stats() { return g_stats; }

void sync_report(const char *tag) {
    parse_cfg();
    /* The prefix through worst_err is parsed by the SY rungs
       (port/tools/mp3_play_proof.py) and stays byte-identical; the rig's
       fields append after it. */
    std::fprintf(stderr,
                 "[sync:%s] enabled=%s sent=%llu recvd=%llu dropped=%llu "
                 "applied=%llu lerps=%llu snaps=%llu worst_err=%d "
                 "delay=%d rtt_last=%d rtt_avg=%d pings=%llu pongs=%llu "
                 "own_claims=%llu local_writes=%llu evsends=%llu "
                 "reseeds=%llu phase_worst=%d\n",
                 tag ? tag : "-", g_enabled ? "yes" : "no",
                 (unsigned long long)g_stats.sent,
                 (unsigned long long)g_stats.recvd,
                 (unsigned long long)g_stats.dropped,
                 (unsigned long long)g_stats.applied,
                 (unsigned long long)g_stats.lerps,
                 (unsigned long long)g_stats.snaps,
                 g_stats.worst_error,
                 g_cfg.delay_ms, g_stats.rtt_last_ms, g_stats.rtt_avg_ms,
                 (unsigned long long)g_stats.pings,
                 (unsigned long long)g_stats.pongs,
                 (unsigned long long)g_stats.own_claims,
                 (unsigned long long)g_stats.local_writes,
                 (unsigned long long)g_stats.evsends,
                 (unsigned long long)g_stats.reseeds,
                 g_stats.phase_worst);
}

}  // namespace port
