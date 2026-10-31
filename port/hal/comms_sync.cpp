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
#include "vs_width.h"
#include "hitch_probe.h"     // HITCH9C
#include "comms_hub.h"       // run mp1 lane GHOSTWIRE1: the adventure hub carries
                             // 'SYNG' / 'SYNI' when it is installed
#include "voice_chat.h"      // run mp1 fold: the SYNG talking bit mirrors
                             // voice_talking() (VOICE1 exports it)
#include "player_fields.h"
#include "star_tally.h"      // SMALLS9: per-player star count in a coop room
#include "ghost_door.h"      // GHOSTDOOR1: door / area / hidden look of a hub ghost
#include "ghost_tongue.h"    // POSE9: a Yoshi ghost's tongue, the hurt blink
#include "party.h"           // PBODY1: solid party bodies (membership by seat)
#include "party_hits.h"
#include <cmath>      // PLAYINT1: the squash after a stomp
#include "host_settings.h"   // port::adventure_ghost_mode(): adventure runs this
                             // layer without SM64DS_SYNC

#include <windows.h>   // GetTickCount, for the delay rig and the RTT probe --
                       // kernel32 only, which every object here already
                       // imports; the static-DLL trap is about NEW libraries.

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "vs_palette_gen.h"  // GHOSTLOOK1: the VS colour arithmetic, for a hub
                             // ghost in its player's own colours

// The decoded-texture cache's generation (ntr/gx.h): it counts every time the
// cache is thrown away, which is when a cached decode of an old row stops
// being able to come back. See gw_colour_row.
namespace ntr { unsigned gx_texture_generation(); }

extern "C" {
// SetAnim IS DECLARED void HERE, WHICH IS ITS ACTUAL RETURN TYPE.
// src/game/player/Player.cpp:14 defines it `void`; 74 matched
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
// The per-slot "this slot is live" byte (16 wide), for the body probe.
extern unsigned char data_0209fc5c[];
// The running cutscene (kuppa) script, 0 = none (src/ResetKuppaScript.c).
extern int data_0209fc48;
// The ROM's player count and its setter (src/SetNumPlayers.c: data_0208a0e0 =
// n, then func_020308d0 writes the live bytes), for the hub seat repair.
extern unsigned char data_0208a0e0;
void SetNumPlayers(int n);
// The wireless session state Player::Behavior tests before it points
// data_020a0e40 at its own slot (hal/level_boot.cpp's a2 seat: 1 when more
// than one body is seated, 0 for one), for the hub's opening hold.
extern int data_0209fc68[];
// SEAMJOIN1: my comms slot (src/func_0203da9c.c: return data_020a0f10), for the a2 seat's bring-up made again in place
int func_0203da9c(void);
// The file's save block (FileSaveData); byte 8 bit 7 is the opening-seen bit,
// for the end of the hub opening's hand-over.
extern unsigned char data_0209caa0[];
// ADVENTURE co-op (M2): the level currently up on THIS console, the id
// hal/level_boot.cpp seats at every mount and hal/level_change.cpp rewrites on
// a warp. The send side stamps it into each snapshot so a peer can tell whether
// it shares this console's level; the presence gate compares a received level
// against it. signed char in the ROM (levels 0..51); read as int here.
extern signed char data_0209f2f8;
// GHOSTDOOR1: the area shown inside that level (-1 none): ChangeArea writes it
extern signed char data_02092120;

// GHOSTWIRE1. The ROM's ONE per-frame body pose (src/func_ov002_020e4768.cpp:
// UpdateBones from the animation, the head/neck look angles, then
// UpdateVertsUsingBones) -- the pass hal/player_bridges.cpp's render banner
// names as the only place a player body is posed. Re-run on a ghost after the
// follower has installed the wire's animation; see gw_follow.
void func_ov002_020e4768(char *self);
// The in-place character change every swap entry point shares
// (hal/player_bridges.cpp, the door path by default).
void port_player_set_character(void *player, unsigned ch);
int port_player_render_hidden(const void *player);   /* POSE9: the ROM Render gates, for the blink probe */
// Stage::Behavior's own call counter (hal/stage_frame.cpp): it does not move on
// a frame the game did not tick, which is what "paused" means here.
unsigned port_stage_behavior_calls(void);
// The save's star total, the number the HUD prints outside VS
// (src/_ZN3HUD15RenderStarCountEv.cpp:69).
unsigned char NumStars(void);
// The ROM's per-frame body placement, the half of Player::Behavior's draw
// preparation before the pose (src/func_ov002_020e444c.c): the body / head
// matrices from position and angles, and the shadow node
// (Actor::DropShadowRadHeight). Run by the follower for a hub ghost, whose own
// Behavior is skipped (hal/player_bridges.cpp, port_adventure_ghost_hold).
void func_ov002_020e444c(char *self);
// The ROM's live-actor walk (src/_ZN8dActor_c4NextEPKS_.cpp), for the
// targeting probe.
void *_ZN8dActor_c4NextEPKS_(const void *prev);
// GHOSTLOOK1: the head pick Player::Render makes (src/actors/Player.cpp,
// func_ov002_020becf4: the character, then with arg2 = 1 the render latches),
// for the look probe.
unsigned _ZN6Player19func_ov002_020becf4Eii(char *self, unsigned v, int arg2);
// Player::HasNoCap through the port's C face (hal/player_bridges.cpp), the
// value func_ov002_020e4bb8 latches into +0x701 for the render.
int _ZN6Player8HasNoCapEv(void *self);
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
    bool no_dr;       // SM64DS_SYNC_NO_DR -- test scaffolding in the
                      // NO_EVENTS class: corrections aim at the raw received
                      // position instead of the reckoned one, so one build
                      // can A/B item 2 by avg_err/snaps
};

// ONE ROM FRAME IS 33.3 ms ON THIS BASE (design.txt F1): the game loop runs
// 30 frames a second, not the 60 the first cut of this file assumed.
enum : int { kGameHz = 30 };

SyncCfg g_cfg;
int      g_send_every = 1;    // frames between sends, from SM64DS_SYNC_HZ
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
    /* ADVENTURE GHOSTS turn this layer on without SM64DS_SYNC. Adventure mode
       IS the state-broadcast session -- each console runs its own solo game and
       the only thing on the wire is where each body is and what it is doing,
       which is exactly what this layer carries. SM64DS_SYNC stays the explicit
       knob for a VS session; adventure mode is the second door to the same
       layer. Off by default either way: adventure mode defaults OFF, so an
       unset environment still gets the byte-identical DS path SY4 asserts. */
    g_cfg.on         = env_int("SM64DS_SYNC", 0) != 0 || adventure_ghost_mode();
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
    g_cfg.no_dr      = env_int("SM64DS_SYNC_NO_DR", 0) != 0;
    if (g_cfg.hz < 1) g_cfg.hz = 1;
    if (g_cfg.hz > 60) g_cfg.hz = 60;
    if (g_cfg.lerp_pct < 1) g_cfg.lerp_pct = 1;
    if (g_cfg.lerp_pct > 100) g_cfg.lerp_pct = 100;
    /* Frames between sends. THE GAME RUNS 30 FRAMES A SECOND ON THIS BASE, not
       60 (design.txt F1/F2: data_0208ee44 = 2, one ROM frame per round, 900
       frames in 30.3 s). This line used to divide 60 by the rate, so the
       default "30 Hz" sent every 2nd frame -- 15 Hz -- and every rate knob
       read half of what it said. */
    g_send_every = kGameHz / g_cfg.hz;
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
namespace { bool gw_decide(); }   // GHOSTWIRE1, defined with the hub layer

bool sync_enabled() { return g_enabled; }

void sync_decide() {
    parse_cfg();
    g_enabled = false;

    if (!g_cfg.on) return;                        // the default, silently

    /* run mp1 lane GHOSTWIRE1: an adventure console on the HUB has no DS
       transport by design (hal/comms_hub.h: the ROM stays single-player). The
       ghost layer runs on the hub's own send/receive instead of the aux
       entries below; nothing else in this function applies to it. */
    if (gw_decide()) return;

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
// SIZE, as of v4: a 16-byte header plus one 40-byte entry -- under owner
// authority each console sends only its own body, so a snapshot is 56 bytes
// regardless of player count (the level id added two). Far under any MTU; the
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

    // ---- v3, DEAD RECKONING (mp-sync-coopdx item 2) -------------------
    // The sender's own per-frame position delta, Fix12 per frame, derived by
    // differencing its body's position between consecutive frames -- no new
    // Player offsets involved (the named-gap rule stands; there may well be a
    // velocity field on the object, but nobody has evidenced one and the
    // delta is just as true). The receiver advances the correction target by
    // vel * age instead of chasing a point that is always one latency old.
    // Zeroed, and the teleport flag set, when one frame moved the body
    // further than the snap threshold -- extrapolating through a warp would
    // aim at a place nobody is.
    int            vx, vy, vz;

    // ---- v4, PRESENCE / LEVEL FILTER (ADVENTURE co-op, M2) ------------
    // The level this body's console is in right now (data_0209f2f8, the id
    // hal/level_boot.cpp seats and hal/level_change.cpp rewrites on a warp).
    // Adventure mode is a set of solo games sharing a wire, so a snapshot from
    // a peer three levels away is not a body this console should draw. The
    // receiver keeps the latest level per slot and draws a ghost only for a
    // peer in the SAME level; on a mismatch it despawns the ghost. VS ignores
    // it -- a versus match is one shared level -- but carries it so one wire
    // format serves both modes. u16 for room; ids are 0..51 today.
    unsigned short level_id;
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
// v3: dead reckoning added vx/vy/vz to the entry (item 2). v4: the level id
// (ADVENTURE co-op M2), so a receiver ghosts only peers in its own level. THE
// VERSION FIELD BUMPS, THE MAGIC NEVER DOES -- spec trap 8, paid for once
// already when a tag bump unframed the whole channel (247 sent, 0 received).
// Both sides ship together; a mismatched peer is recognised, counted, dropped
// loudly.
enum : unsigned { kSyncVersion = 4u };
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
static_assert((int)kPortBodySlots >= (int)kHubMaxSeats, "HUB32: a body slot per hub seat");
bool           g_arr_seen[kPortBodySlots];
unsigned short g_arr_anim[kPortBodySlots];

// Item 4's per-slot wire-side animation-length estimate: the high-water
// received cursor for the id currently playing on that slot. Reset on id
// change. See the wrap note in apply_pose.
unsigned short g_ph_id[kPortBodySlots];
int            g_ph_hw[kPortBodySlots];

// ADVENTURE GHOSTS: the per-slot follow target. In adventure mode apply_snapshot
// does NOT move the body -- it records the latest snapshot here, and
// port_adventure_ghost_follow eases the body toward it every render frame. That
// is what turns a body corrected only on ~30 Hz receive frames (and drifting on
// its own ROM physics in between) into a smooth pure follower.
struct GhostTarget { bool have; int x, y, z; short yaw; bool teleport;
                     int vx, vz; };
GhostTarget g_gtarget[kPortBodySlots];

// ADVENTURE GHOSTS, the ANIMATION half of the follower. The ROM keeps ticking a
// ghost's own Player::Behavior, which forces the puppet back into its spawn
// no-control pose and overwrites the wire's animation every frame -- so an
// animation applied only on the ~30 Hz receive frames flickers against the
// ROM's reset and never plays. The follower instead re-drives the wire animation
// EVERY render frame at a locally-advanced cursor, so the last write each frame
// is always the right animation at a smoothly climbing frame. g_ganim is the
// wire's current anim id, g_gcursor the playback cursor in 20.12 (advanced one
// frame per render frame, re-synced to the wire on every snapshot so it stays in
// phase with the sender's own cycle).
unsigned short g_ganim[kPortBodySlots];
int            g_gcursor[kPortBodySlots];
bool           g_ganim_have[kPortBodySlots];

// ADVENTURE co-op (M2), PRESENCE. The per-slot filter that turns "everyone on
// the wire" into "everyone in MY level". Filled from every received snapshot:
// the peer's level id, and the wall clock of the last snapshot for a liveness
// timeout. peer_visible() is the one predicate every ghost loop consults --
// draw, hold-follow, name tag -- so "spawn a ghost" and "despawn it when the
// peer leaves or changes level" are one decision made in one place. A peer is
// visible when it is present (a snapshot inside kPresenceTimeoutMs) AND its
// level id equals this console's data_0209f2f8. A peer that stops sending, or
// warps to another level, or that this console warps away from, drops out of
// visible on the next frame -- which is the despawn.
enum : unsigned { kPresenceTimeoutMs = 2000u };
int      g_peer_level[kPortBodySlots];
unsigned g_peer_seen_ms[kPortBodySlots];
bool     g_peer_present[kPortBodySlots];


// Item 2's sender-side velocity sample: the local body's position last frame,
// differenced each frame. Seeded on first sight so the first frame's
// "velocity" is zero rather than the distance from the origin to the spawn.
bool g_vel_seeded = false;
int  g_vel_prev[3];
int  g_vel[3];
// A warp is STICKY until a send ships it: the warp frame is not necessarily
// a send frame, and a warp whose flag never reached the wire would be counted
// by the receiver as a correction bug (a snap) instead of honoured as a warp.
bool g_warp_pending = false;
}  // namespace

// One DS unit is 4096 in Fix12. The thresholds are in units and converted here
// so the constants read the way the design note states them.
inline int units(int n) { return n * 4096; }

// ===========================================================================
// GHOSTWIRE (run mp1, lane GHOSTWIRE1): ADVENTURE GHOSTS OVER THE HUB.
//
// With the adventure hub installed (hal/comms_hub.h) there is no DS transport
// at all: comms_transport() is null, the ROM thinks it is single-player, and
// the whole 'SYN1' path above has nothing to send on. This section is the
// adventure layer for that carrier, the wire of design.txt section 6:
//
//   'SYNG' ghost frame, 44 bytes, 15 Hz from a wall-clock cadence (2 Hz with
//          kGhostPaused while the game is not ticking). seq is the sender's
//          15 Hz wall tick, so a receiver reads the sender's clock straight
//          off it: sender_ms = seq * 1000 / 15.
//   'SYNI' identity, name + colours + character + build, on the first live
//          frame, every 2 s, and when the roster changes.
//
// THE RECEIVER DRAWS EVERY SEAT 100 ms BEHIND REAL TIME. Per seat it keeps the
// last 8 frames and a lower-envelope estimate of (arrival - sender clock), so
// "100 ms ago" is a point on the sender's own timeline: interpolate between
// the two frames around it, dead-reckon past the newest for at most 250 ms,
// snap across a kGhostTeleport frame, and stand still on a paused one. A seat
// whose last frame is 3 s old is gone; a seat in another level is hidden.
//
// WHICH BODY. The ghost bodies are the ROM's own per-slot Player actors
// data_0209f394[] that level boot seats (vs_player_count, hal/comms_conductor
// .cpp); the local player is data_0209f250. Remote seats map onto the other
// bodies in seat order, skipping mine, so seat s drives one fixed body for as
// long as my seat does not change. A seat with no body (more peers than
// bodies) is simply not drawn.
//
// NOTHING HERE WRITES THE LOCAL BODY, and nothing here reads a payload for its
// sender: the seat is the one the relay (or the loopback mesh) stamped.
// Every datagram is checked for tag, version, exact length and field ranges
// before any field is used; failures are dropped and counted by reason.
// ===========================================================================
namespace {

/* THE DEATH PLANE: func_ov002_020c5d60, the pit test Player::Behavior runs
   every frame, returns while y >= 0xf8ad0000 (-29952 units) and otherwise
   puts the player in ST_DEAD_PIT. A ghost whose player is down there is
   falling out of the level: it is hidden, never drawn or placed there. */
const int kDeathPlaneY = (int)0xf8ad0000;

enum : int {
    kGwSendHz     = 15,      // design 6.1: the wall-clock rate of the old
                             // stream (SM64DS_GHOST_LEGACY=1 and old builds)
    /* GHOSTSMOOTH1: THE STAMPED FRAME. A SYNG with this flag bit is one frame
       per game TICK that moved (30 a second): seq counts the sender's ticks (the
       order, the duplicates) and reserved[0..2] carry the sender's own wall
       clock at that tick, in ms, low 24 bits (little endian). The receiver
       places each frame on the sender's real timeline, so a sender whose game
       hitches (a stalled tick) is a slow stretch of its path, not a shift of its
       whole clock, and no frame is taken at the wrong time the way the old wall
       tick (15 a second, the first game tick after each boundary) took half of
       them. Old builds read any flag outside 0x1f as a bad field and drop the
       frame (nothing is drawn rather than a wrong timeline); a frame without the
       bit is the old 15 Hz wall tick and is still taken, so an old sender still
       shows. */
    kGwFrameClock = 0x20,
    kGwFrameHz    = 30,
    kGwPausedMs   = 500,     // 2 Hz while paused / in a menu
    kGwIdentMs    = 2000,    // SYNI every 2 s
    kGwIdentGapMs = 250,     // an extra SYNI (roster change) no sooner than this
    kGwInterpMs   = 100,     // draw a remote seat this far behind real time
    kGwReckonMs   = 66,      // dead-reckon past the newest frame at most this
                             // (2 ticks: a late frame is covered, a stalled
                             // sender is held, not carried on and pulled back)
    kGwReckonLegacyMs = 250, // ...and in the old (legacy) follower
    kGwOffN       = 10,      // frames the clock offset's minimum looks at
    kGwStillEvery = 3,       // a standing body is sent every 3rd tick (10 Hz)
    kGwMinSendGapMs = 20,    // no two frames closer than this (a catch-up burst)
    /* A GHOST GOES ON THE AGE OF ITS OWN FRAMES, NOT ON HUB PRESENCE. The
       hub's presence is its I/O thread's pings, which keep answering while a
       console's game loop is hung, so a frozen game would otherwise leave a
       frozen ghost standing (review of HUB1, fold note 4). No SYNG for this
       long: the ghost fades out from kGwFadeMs and is gone at kGwDespawnMs,
       and comes back with the next frame. A paused player sends 2 Hz, so
       one lost paused frame (a 1 s gap) still does not blink it. */
    kGwFadeMs     = 700,
    kGwAppearMs   = 300,     // GHOSTLOOK1: a ghost that comes on show fades in
    /* GHOSTDOOR1: a ghost that leaves my level or my area (a door, a painting)
       fades out in place over this long instead of vanishing in a frame; a
       ghost whose newest frames have all been "paused" for kGwPauseFadeMs (a
       friend left in the pause menu or a star select) fades out over
       kGwPauseFadeLen and is gone, not left standing frozen. */
    kGwLeaveMs    = 300,
    kGwPauseFadeMs  = 2000,
    kGwPauseFadeLen = 300,
    kGwDespawnMs  = 1200,
    kGwRing       = 16,      // frames kept per seat (~0.5 s at 30 Hz)
    kGwRingLegacy = 8,       // ...and in the legacy follower
    /* THE PLAYER HAS 194 ANIMATIONS. Player::SetAnim indexes
       data_ov002_020ff480[id * 4 + character], and that table is 0xc20 bytes
       (0x020ff480..0x021000a0) = 194 ids x 4 characters: id 194 and up read
       the words after the table as file pointers, and the re-pose then walks
       garbage (REVGHOSTWIRE1: one frame with anim 194 crashed every console
       in the room, c0000005 in UpdateBones). */
    kPlayerAnimCount = 194,
    kGwPosLimit   = 0x8000 << 12,   // |x|, |y|, |z| beyond 32768 units: garbage
    kGwParkUp     = 200000 << 12,   // the park, above the local player (Fix12)
    kGwSwapNear   = 500,            // units: a shown seat this close keeps its body
    kGwClockJumpMs = 2000,   // a sender clock step this big is a restart
    /* GHOSTLOOK1: 'SYNI' byte 18 (flags, design 6.2) bit 0: the body and shoe
       colours at bytes 12..17 are the player's own pick (the launcher sent
       SM64DS_PLAYER_COLOR and SM64DS_PLAYER_SHOES). Without it they are this
       file's defaults, which name the tag but do not recolour the ghost. An
       identity from an older build carries flags 0 and is taken as before. */
    kGwIdentColours = 0x01,
};

// GHOSTLOOK1: TWO TEST KNOBS for the smoothness measurement, both default to
// the design's numbers. SM64DS_GHOST_SEND_HZ (5..30) is the SYNG rate AND the
// timebase a receiver turns seq into time with, so EVERY console in a room
// must run the same value: it is a lab knob, not a setting (a real rate change
// has to put the rate on the wire). SM64DS_GHOST_INTERP_MS (0..400) is how far
// behind real time a ghost is drawn.
int gw_send_hz() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_GHOST_SEND_HZ");
        v = e ? std::atoi(e) : (int)kGwSendHz;
        if (v < 5) v = 5;
        if (v > 30) v = 30;
    }
    return v;
}
// GHOSTSMOOTH1: SM64DS_GHOST_LEGACY=1 runs the build-4 ghost stream and
// follower exactly as they were (15 Hz wall-clock SYNG, GetTickCount stamps, a
// slow-rise clock envelope, 100 ms behind, 250 ms reckoning): the BEFORE arm
// of the smoothness rows on one exe. Every console in a room must agree.
// SM64DS_GHOST_SEND_HZ below 30 also selects the old wall-clock stream.
bool gw_legacy() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_GHOST_LEGACY");
        v = (e && std::atoi(e) != 0) || (std::getenv("SM64DS_GHOST_SEND_HZ") &&
                                         gw_send_hz() < kGwFrameHz) ? 1 : 0;
    }
    return v != 0;
}
// GHOSTSMOOTH1: HOW FAR A BODY MAY MOVE IN ONE TICK AND STILL BE A WALK. The
// shared SM64DS_SYNC_SNAP (60 units, the sum of the three axes' steps) calls a
// fast dive, a long jump or a fall at terminal speed a teleport, so the other
// side held the ghost and then snapped it (measured: 12 of 2300 drawn frames in a
// run / jump walk, 20 to 30 units off while held). A real warp (a door, a pipe,
// a level change) is hundreds of units; 250 keeps those as snaps and lets every
// walk, jump and fall interpolate. SM64DS_GHOST_SNAP=<units> sets it (60 = the
// old rule); the legacy stream keeps the shared value.
int gw_snap_units(int shared) {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_GHOST_SNAP");
        v = e ? std::atoi(e) : 250;
        if (v < 10) v = 10;
        if (v > 5000) v = 5000;
    }
    return gw_legacy() ? shared : v;
}
// SM64DS_GHOST_NO_PLAYOUT=1 draws at the offset-derived time with no rate
// limit (the first GHOSTSMOOTH1 follower), for the A/B row.
bool gw_playout() {
    static int v = -1;
    if (v < 0) v = std::getenv("SM64DS_GHOST_NO_PLAYOUT") ? 0 : 1;
    return v != 0;
}
// SM64DS_GHOST_INTERP_MS pins the draw delay (ms behind the sender's clock);
// -1 (unset) lets the follower size it from the link's own jitter.
int gw_interp_pin() {
    static int v = -2;
    if (v == -2) {
        const char *e = std::getenv("SM64DS_GHOST_INTERP_MS");
        v = e ? std::atoi(e) : -1;
        if (v < -1) v = -1;
        if (v > 400) v = 400;
    }
    return v;
}
int gw_interp_ms() {      // the legacy follower's fixed delay
    const int p = gw_interp_pin();
    return p >= 0 ? p : (int)kGwInterpMs;
}

struct GwFrame {
    unsigned      ext;       // seq extended to 32 bits
    double        ts;        // the sender's clock for this frame, ms
    unsigned      rt;        // comms_hub_now_ms() the hub's I/O thread took it
                             // off the socket (the legacy follower: unused)
    HubGhostFrame f;
};

struct GwSeat {
    bool      have_epoch;
    unsigned  epoch;         // comms_hub_seat_info().join_epoch last seen
    GwFrame   ring[kGwRing]; // ascending by ext
    int       n;
    bool      have_off;
    long long off;           // legacy: local arrival ms - sender ms, envelope
    int       hz;            // GHOSTSMOOTH1: this seat's frame clock, 30 or 15
    double    buf_ms;        // ...and how far behind it is drawn (the jitter
                             // buffer B, slewed 1 ms a pass)
    double    play;          // the draw time on the sender's clock (the playout
    unsigned  play_t;        // clock), the wall clock it was last advanced at
    bool      play_have;
    unsigned  last_ms;       // GetTickCount of the newest accepted frame
    bool      have_ident;
    char      name[kHubIdentNameMax + 1];
    unsigned char character, body[3], shoes[3];
    bool      colours;       // GHOSTLOOK1: body / shoes are the player's pick
    unsigned short pal_src[16];  // GHOSTLOOK1: the row 0 the colours were
    unsigned short pal_out[16];  // made from, and the row made from it with
    unsigned char  pal_body[3], pal_shoes[3];   // these colours
    bool      pal_have;
    unsigned  build_id;
    unsigned  char_try_ms;   // last character-swap attempt on its body
    unsigned  driven;        // frames the follower has driven its body
    const void *body_ptr;    // ...and which body that was (a level load
                             // seats new ones: the count starts again)
    unsigned  show_ms;       // GHOSTLOOK1: when it last came on show, and
    unsigned  show_n;        // the follower pass that last showed it
    // GHOSTDOOR1
    bool      paused_now;    // the newest frame is a paused one, and the run
    unsigned  paused_ms;     // of paused frames began at this GetTickCount
    unsigned  here_ms;       // the last GetTickCount it was really visible
    bool      leaving;       // visible only as a fade-out in place (gw_visible)
    int       leave_cut;     // 0..256 taken off its fade while it leaves
    bool      last_have;     // g_gw_last holds a sample drawn in my level / area
    int       last_lvl, last_area;
    unsigned  char_wait_ms;  // when its body first showed the wrong character
    // GHOSTCHAR1: how this seat's ghost was posed, per follower frame
    // (gw_sample), for the exit summary. A "hold" is a frame with no newer
    // data to move to; the streak is how many of them ran back to back.
    unsigned long long sm_n, sm_interp, sm_reckon;
    unsigned long long sm_hold_old, sm_hold_tele, sm_hold_still;
    unsigned  sm_streak, sm_streak_max;
    unsigned long long sm_swaps;   // character swaps made on its body
};

GwSeat g_gw[kHubMaxSeats];
bool   g_hub = false;        // the ghost layer runs on the hub

struct GwStats {
    unsigned long long syng_sent, syng_paused, syni_sent, send_fail;
    unsigned long long syng_recvd, syni_recvd;
    unsigned long long drop_len, drop_tag, drop_ver, drop_seat, drop_field,
                       drop_old;
    unsigned long long resets, charswaps, reposes, recursor;
    unsigned long long parked, below, clamped, target_ghost, near_ghost;
    unsigned long long interp, reckon, hold, teleports;
    unsigned long long seat_fix, adopted;   // the hub seat repair
    unsigned long long cut_nosend;          // ghost frames not sent: cutscene
    unsigned long long cut_frames, cut_drawn;   // follower frames in one, and
                                                // of those, with a ghost shown
    unsigned long long shown_frames;            // follower frames, any ghost shown
    unsigned long long comebacks;   // frames that brought back a gone ghost
    unsigned long long pal_held;   // GHOSTLOOK1: frames a pick waited for a
                                   // cache drop (gw_row_drawable)
    unsigned long long pal_rows, pal_writes;   // GHOSTLOOK1: ghosts drawn in
                                               // their own colours; row writes
    unsigned long long door_snaps;   // GHOSTDOOR1: door sequences flagged as a
                                     // teleport on the first frame
    unsigned long long hid_frames;   // follower frames a ghost was hidden by its
                                     // sender's own state
    unsigned long long linger_frames;  // ...and drawn fading out in place
    unsigned long long char_holds;   // ...and held back for the wrong character
    unsigned long long drains;       // hub drains during my own star select
};
GwStats g_gws;

// The sender.
unsigned g_gw_t0 = 0;
unsigned g_gw_f0 = 0;              // GHOSTSMOOTH1: the tick count at the first frame
bool     g_gw_t0_set = false;
HubGhostFrame g_gw_sent;           // ...the last frame sent, to tell a change
bool     g_gw_sent_have = false;
unsigned g_gw_last_seq = 0xffffffffu;
unsigned g_gw_last_send_ms = 0;
unsigned g_gw_last_ident_ms = 0;
bool     g_gw_ident_sent = false;
unsigned g_gw_roster = 0;
unsigned g_gw_beh_last = 0;
bool     g_gw_beh_seeded = false;
int      g_gw_frozen = 0;          // frames the local body has not moved at all
int      g_gw_fz[6];               // x y z yaw anim cursor, last frame
bool     g_gw_prev_have = false;
int      g_gw_prev[3];
int      g_gw_prev_level = -1;
bool     g_gw_warp = false;
unsigned char g_gw_prev_dkind = 0;   // GHOSTDOOR1: the last tick's door kind
int      g_gw_v[3];
unsigned g_gw_last_report_ms = 0;
unsigned g_gw_frame = 0;
bool     g_gw_quiet = false;       // SEAMHUB1: no periodic report (an in-place leave is under way)

// Who I am on the wire. SM64DS_PLAYER_NAME (1..24 bytes, 0x20..0x7e),
// SM64DS_PLAYER_COLOR / SM64DS_PLAYER_SHOES (six hex digits, the lobby's colour
// grammar). The launcher exports them from the same nick/colours it sends the
// lobby; absent means an empty name (the tag then reads "PLAYER n").
struct GwMe {
    bool read;
    char name[kHubIdentNameMax + 1];
    int  name_len;
    unsigned char body[3], shoes[3];
    bool colours;            // GHOSTLOOK1: both colours came from the launcher
    unsigned build_id;
};
GwMe g_gw_me;

// The character a body renders as: the low byte of param1 (+0x08), which
// GetBodyModelID reads and the door swap writes (hal/player_bridges.cpp).
unsigned gw_char(const void *a) {
    const unsigned c = (unsigned)(*(const int *)((const char *)a + 8) & 0xff);
    return c <= 3 ? c : 0u;
}

int gw_hex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// true when the variable was there and was six hex digits (out is then it)
bool gw_rgb_env(const char *var, const char *e, unsigned char out[3],
                unsigned char r, unsigned char g, unsigned char b) {
    out[0] = r; out[1] = g; out[2] = b;
    if (!e) return false;
    if (std::strlen(e) != 6) {
        std::fprintf(stderr, "[ghost] %s ignored: not six hex digits\n", var);
        return false;
    }
    unsigned char t[3];
    for (int i = 0; i < 3; ++i) {
        const int hi = gw_hex(e[2 * i]), lo = gw_hex(e[2 * i + 1]);
        if (hi < 0 || lo < 0) {
            std::fprintf(stderr, "[ghost] %s ignored: not six hex digits\n",
                         var);
            return false;
        }
        t[i] = (unsigned char)(hi * 16 + lo);
    }
    out[0] = t[0]; out[1] = t[1]; out[2] = t[2];
    return true;
}

void gw_me_read() {
    if (g_gw_me.read) return;
    g_gw_me.read = true;
    g_gw_me.name[0] = 0;
    g_gw_me.name_len = 0;
    /* SEAMHUB1: the name and both colours come from the session record (hal/comms_hub.h), which the in-place leave clears */
    if (const char *e = online_session_get(kSessPlayerName)) {
        const size_t n = std::strlen(e);
        bool ok = n >= 1 && n <= (size_t)kHubIdentNameMax;
        for (size_t i = 0; ok && i < n; ++i)
            ok = (unsigned char)e[i] >= 0x20 && (unsigned char)e[i] <= 0x7e;
        if (ok) {
            std::memcpy(g_gw_me.name, e, n);
            g_gw_me.name[n] = 0;
            g_gw_me.name_len = (int)n;
        } else {
            std::fprintf(stderr, "[ghost] SM64DS_PLAYER_NAME ignored: 1..%d "
                         "bytes of 0x20..0x7e\n", (int)kHubIdentNameMax);
        }
    }
    /* GHOSTLOOK1: a pick is the PAIR, the launcher's both-or-neither rule
       (LobbyLaunch IdentityEnv): one without the other is no pick */
    const bool b_ok =
        gw_rgb_env("SM64DS_PLAYER_COLOR", online_session_get(kSessPlayerColor), g_gw_me.body, 0xff, 0xff, 0xff);
    const bool s_ok =
        gw_rgb_env("SM64DS_PLAYER_SHOES", online_session_get(kSessPlayerShoes), g_gw_me.shoes, 0x80, 0x40, 0x20);
    g_gw_me.colours = b_ok && s_ok;
    /* A build id for the identity record: FNV-1a of this TU's build stamp.
       Display / diagnosis only (design 6.2: never parsed for meaning). */
    unsigned h = 2166136261u;
    for (const char *p = __DATE__ " " __TIME__; *p; ++p) {
        h ^= (unsigned char)*p;
        h *= 16777619u;
    }
    g_gw_me.build_id = h;
}

// Remote seat <-> ghost body. THE BODIES ARE FEW: the ROM seats them at level
// boot (vs_player_count), and an adventure level with more than four Player
// bodies stops taking stars and saving (suspects.tsv S1: 2 and 4 bodies keep
// the star + save, 8 and 16 lose it). So a room of 16 cannot give every seat
// a body. gw_assign() hands the free bodies to the NEAREST visible seats (same
// level, frames fresh) once a frame, keeps a seat on its body while it stays
// visible, and moves a body to a much nearer seat only after it has shown its
// current one for 2 s, and never while the farthest shown seat is within
// kGwSwapNear units: players standing together (a shared entrance, a crowd at
// a wall) are all at a distance near 0, where "under half the distance" is
// true of a unit or two and the three drawn ghosts swapped every 2 s
// (REVGHOSTWIRE1-R3, crowd8: 10-16 swaps per console). A seat without a body
// is still on the wire (name, tag facts, voice, chat); it is simply not drawn.
int      g_body_seat[kPortBodySlots];     // seat driving body i, or -1
unsigned g_body_since[kPortBodySlots];    // GetTickCount of that assignment
bool     g_assign_init = false;
unsigned g_gw_myseat_seen = 0xffffffffu;

int gw_seat_for_body(int body) {
    if (!g_assign_init || body < 0 || body >= kPortBodySlots) return -1;
    if (body >= (int)data_0209f21c || body == (int)data_0209f250) return -1;
    return g_body_seat[body];
}

bool gw_visible(int seat);

long long gw_dist2(int seat, const int me_xyz[3]) {
    const GwSeat &z = g_gw[seat];
    const HubGhostFrame &f = z.ring[z.n - 1].f;
    const long long dx = (long long)(f.x - me_xyz[0]) >> 12;
    const long long dz = (long long)(f.z - me_xyz[2]) >> 12;
    const long long dy = (long long)(f.y - me_xyz[1]) >> 12;
    return dx * dx + dy * dy + dz * dz;
}

void gw_assign(unsigned now) {
    if (!g_assign_init) {
        for (int b = 0; b < kPortBodySlots; ++b) g_body_seat[b] = -1;
        g_assign_init = true;
    }
    const int my = comms_hub_my_seat();
    const int me = (int)data_0209f250;
    const int nb = (int)data_0209f21c < kPortBodySlots ? (int)data_0209f21c
                                                         : kPortBodySlots;
    if ((unsigned)my != g_gw_myseat_seen) {      // re-paired: start over
        g_gw_myseat_seen = (unsigned)my;
        for (int b = 0; b < kPortBodySlots; ++b) g_body_seat[b] = -1;
    }
    if (my < 0 || me < 0 || me >= kPortBodySlots || !data_0209f394[me]) return;
    void *mb = data_0209f394[me];
    const int me_xyz[3] = {*player::pos_x(mb), *player::pos_y(mb),
                           *player::pos_z(mb)};
    bool has[kHubMaxSeats] = {};
    for (int b = 0; b < kPortBodySlots; ++b) {
        const int s = g_body_seat[b];
        if (s < 0) continue;
        if (b >= nb || b == me || !gw_visible(s)) { g_body_seat[b] = -1; continue; }
        has[s] = true;
    }
    for (;;) {
        /* the nearest visible seat with no body */
        int cand = -1;
        long long cd = 0;
        for (int s = 0; s < kHubMaxSeats; ++s) {
            if (s == my || has[s] || !gw_visible(s)) continue;
            const long long d = gw_dist2(s, me_xyz);
            if (cand < 0 || d < cd) { cand = s; cd = d; }
        }
        if (cand < 0) return;
        int free_b = -1;
        for (int b = 0; b < nb; ++b)
            if (b != me && data_0209f394[b] && g_body_seat[b] < 0) { free_b = b; break; }
        if (free_b < 0) {
            /* no free body: take the one showing the farthest seat, if this
               one is under half its distance, it has had 2 s, and that seat
               is not itself close by */
            long long fd = -1;
            for (int b = 0; b < nb; ++b) {
                const int s = g_body_seat[b];
                if (b == me || s < 0) continue;
                if ((unsigned)(now - g_body_since[b]) < 2000u) continue;
                const long long d = gw_dist2(s, me_xyz);
                if (d > fd) { fd = d; free_b = b; }
            }
            if (free_b < 0 || cd * 4 >= fd ||
                fd <= (long long)kGwSwapNear * kGwSwapNear)
                return;
            has[g_body_seat[free_b]] = false;
        }
        g_body_seat[free_b] = cand;
        g_body_since[free_b] = now;
        has[cand] = true;
        g_gw[cand].driven = 0;
        g_gw[cand].body_ptr = 0;
    }
}

void gw_reset_seat(GwSeat &z, bool keep_ident) {
    const bool hi = z.have_ident;
    char name[kHubIdentNameMax + 1];
    std::memcpy(name, z.name, sizeof name);
    const unsigned char ch = z.character;
    unsigned char body[3], shoes[3];
    std::memcpy(body, z.body, 3);
    std::memcpy(shoes, z.shoes, 3);
    const unsigned bid = z.build_id;
    const bool he = z.have_epoch;
    const unsigned ep = z.epoch;
    // GHOSTCHAR1: the exit summary's per-seat pose counts run for the whole
    // session; a seat reset (a rejoin, a clock jump, a level reload) is not
    // the end of the ghost
    const unsigned long long sm_n = z.sm_n, sm_i = z.sm_interp,
        sm_r = z.sm_reckon, sm_ho = z.sm_hold_old, sm_ht = z.sm_hold_tele,
        sm_hs = z.sm_hold_still, sm_sw = z.sm_swaps;
    const unsigned sm_mx = z.sm_streak_max;
    std::memset(&z, 0, sizeof z);
    z.have_epoch = he;
    z.epoch = ep;
    z.sm_n = sm_n; z.sm_interp = sm_i; z.sm_reckon = sm_r;
    z.sm_hold_old = sm_ho; z.sm_hold_tele = sm_ht; z.sm_hold_still = sm_hs;
    z.sm_swaps = sm_sw; z.sm_streak_max = sm_mx;
    if (keep_ident && hi) {
        z.have_ident = true;
        std::memcpy(z.name, name, sizeof name);
        z.character = ch;
        std::memcpy(z.body, body, 3);
        std::memcpy(z.shoes, shoes, 3);
        z.build_id = bid;
    }
}

// A seat (re)joined since we last looked: forget everything about the old
// presence, its identity included.
void gw_epoch_check(int seat) {
    HubSeatInfo si;
    if (!comms_hub_seat_info(seat, &si)) return;
    GwSeat &z = g_gw[seat];
    if (!z.have_epoch || z.epoch != si.join_epoch) {
        if (z.have_epoch) ++g_gws.resets;
        gw_reset_seat(z, false);
        z.have_epoch = true;
        z.epoch = si.join_epoch;
    }
}

void gw_on_ghost(const unsigned char *buf, int n, int seat, unsigned t_arr) {
    if (n != (int)sizeof(HubGhostFrame)) { ++g_gws.drop_len; return; }
    HubGhostFrame f;
    std::memcpy(&f, buf, sizeof f);
    if (f.tag != kHubTagGhost) { ++g_gws.drop_tag; return; }
    if (f.version != 1) { ++g_gws.drop_ver; return; }
    if (seat < 0 || seat >= kHubMaxSeats || seat == comms_hub_my_seat()) {
        ++g_gws.drop_seat;
        return;
    }
    if ((f.flags & ~(0x1fu | (unsigned)kGwFrameClock |
                     (unsigned)ghostdoor::kFlagHidden)) || f.character > 3 ||
        ((f.flags & ghostdoor::kFlagHidden) && !(f.flags & kGwFrameClock)) ||
        f.anim_id >= kPlayerAnimCount || f.level_id > 0xff ||
        f.anim_frame < 0 || f.anim_frame > (2000 << 12) ||    /* MPSAFE1: a cursor past any real animation */
        (!(f.flags & kGwFrameClock) &&
         (f.reserved[0] || f.reserved[1] || f.reserved[2])) ||
        f.x > kGwPosLimit || f.x < -kGwPosLimit ||
        f.y > kGwPosLimit || f.y < -kGwPosLimit ||
        f.z > kGwPosLimit || f.z < -kGwPosLimit) {
        ++g_gws.drop_field;
        return;
    }
    /* GHOSTDOOR1: a build-6 frame names a door kind in the area byte's high
       nibble; a kind we do not know is a bad field */
    if ((f.flags & kGwFrameClock) &&
        (unsigned)(f.area >> ghostdoor::kDoorShift) >= (unsigned)ghostdoor::kDoorKinds) {
        ++g_gws.drop_field;
        return;
    }
    gw_epoch_check(seat);
    GwSeat &z = g_gw[seat];
    const unsigned now = GetTickCount();
    const bool legacy = gw_legacy();
    /* GHOSTSMOOTH1: the frame's clock. The frame-clock bit says seq counts the
       sender's game ticks (30 a second); without it seq is the old wall tick
       (15 a second). A seat that changes its mind (a restart on another
       build) starts its timeline again. */
    const int hz = legacy ? gw_send_hz()
                          : ((f.flags & kGwFrameClock) ? (int)kGwFrameHz
                                                       : (int)kGwSendHz);
    if (z.n > 0 && z.hz != hz) {
        ++g_gws.resets;
        gw_reset_seat(z, true);
    }
    z.hz = hz;
    unsigned ext;
    if (z.n == 0) {
        ext = 0x100000u + f.seq;     // headroom so a backward step stays > 0
    } else {
        const unsigned newest = z.ring[z.n - 1].ext;
        const int delta = (short)(unsigned short)(f.seq - (unsigned short)newest);
        if (delta <= 0) {
            /* HUB1 H1d's rule, receiver side: a sender that restarted counts
               from 0 again; a step back of more than 16 frames is that, not a
               reordered datagram. */
            if (delta >= -16) { ++g_gws.drop_old; return; }
            ++g_gws.resets;
            gw_reset_seat(z, true);
            ext = 0x100000u + f.seq;
        } else {
            ext = newest + (unsigned)delta;
        }
    }
    z.hz = hz;
    if (!legacy) {
        /* THE PUPPET METHOD (vs_arena.cpp, PUPPET1), for a hub ghost. The frame
           is kept with the hub I/O thread's own arrival stamp (not the game
           tick's GetTickCount, which is quantised to a tick and to 15 ms); the
           clock offset is read at sample time as the minimum over the newest
           kGwOffN frames (gw_sample_fc), so a sender that hitches does not
           leave a long memory behind. Here only: a step in the sender's clock
           bigger than kGwClockJumpMs is a restart. */
        double ts;
        const unsigned st = (unsigned)f.reserved[0] |
                            ((unsigned)f.reserved[1] << 8) |
                            ((unsigned)f.reserved[2] << 16);
        if (f.flags & kGwFrameClock) {
            /* the sender's wall clock, 24 bits, taken on from the last frame
               (a step is read as signed, so the wrap at 4.6 hours costs
               nothing); never backwards, so a lerp's span is always > 0 */
            if (z.n == 0) {
                ts = (double)st + 16777216.0;
            } else {
                const double last = z.ring[z.n - 1].ts;
                const int step = ((int)((st - ((unsigned)last & 0xffffffu)) << 8)) >> 8;
                ts = last + (double)step;
                if (ts <= last) ts = last + 1.0;
            }
        } else {
            ts = (double)ext * 1000.0 / (double)hz;
        }
        const double d = (double)(int)t_arr - ts;
        /* A GAP FIRST (OPENCOUNT1's comeback rule, below in the legacy half
           too): a sender's load screen stops its frame clock for as long as the
           load lasts, and that is a gap in the stream, not a restart. */
        if (z.n > 0 && (unsigned)(now - z.last_ms) > (unsigned)kGwDespawnMs) {
            z.n = 0;
            ++g_gws.comebacks;
        }
        if (z.n > 0) {
            double lo = 0;
            const int k0 = z.n > kGwOffN ? z.n - kGwOffN : 0;
            for (int k = z.n - 1; k >= k0; --k) {
                const double e = (double)(int)z.ring[k].rt - z.ring[k].ts;
                if (k == z.n - 1 || e < lo) lo = e;
            }
            if (d - lo > kGwClockJumpMs || lo - d > kGwClockJumpMs) {
                ++g_gws.resets;
                gw_reset_seat(z, true);
                z.hz = hz;
                if (f.flags & kGwFrameClock) ts = (double)st + 16777216.0;
            }
        }
        if (z.n == kGwRing) {
            std::memmove(&z.ring[0], &z.ring[1], sizeof(GwFrame) * (kGwRing - 1));
            --z.n;
        }
        z.ring[z.n].ext = ext;
        z.ring[z.n].ts = ts;
        z.ring[z.n].rt = t_arr;
        z.ring[z.n].f = f;
        ++z.n;
        z.last_ms = now;
        if (f.flags & kGhostPaused) {            // GHOSTDOOR1: a paused run
            if (!z.paused_now) { z.paused_now = true; z.paused_ms = now; }
        } else {
            z.paused_now = false;
        }
        ++g_gws.syng_recvd;
        return;
    }
    const long long ts = (long long)ext * 1000 / gw_send_hz();
    const long long d = (long long)now - ts;
    if (z.have_off && (d - z.off > kGwClockJumpMs || z.off - d > kGwClockJumpMs)) {
        /* the sender's clock stepped (a restart inside the u16 window, or a
           machine that stalled for seconds): start its timeline again */
        ++g_gws.resets;
        const GwFrame keep = { ext, (double)ts, 0u, f };
        gw_reset_seat(z, true);
        z.hz = hz;
        z.ring[0] = keep;
        z.n = 1;
        z.have_off = true;
        z.off = d;
        z.last_ms = now;
        ++g_gws.syng_recvd;
        return;
    }
    if (!z.have_off || d < z.off) {
        z.off = d;
        z.have_off = true;
    } else {
        z.off += (d - z.off) / 32;   // slow rise: drift and route changes
    }
    /* run mp1 lane OPENCOUNT1: A GHOST THAT WAS GONE COMES BACK AT ITS NEW
       FRAME. Past kGwDespawnMs with no frame the ghost has faded out and is not
       drawn (gw_visible), so the frames still in its ring describe a spot the
       player has since left: a cutscene during which the player moved (the
       star-get camera slides Mario on his momentum while his console sends
       nothing), or a gap on the link. Kept, gw_sample drew the newest of them
       for the 100 ms interpolation delay -- across a teleport frame it holds
       the frame before -- and then snapped to the real spot (REVGHOSTCUT1 N1:
       ~110 ms at the old spot, then a 105-unit jump). Nothing on screen
       continues from those frames, so the ring starts again with this one, and
       the sample holds it until the timeline reaches it. The clock estimate is
       kept: the sender's clock did not step. */
    if (z.n > 0 && (unsigned)(now - z.last_ms) > (unsigned)kGwDespawnMs) {
        z.n = 0;
        ++g_gws.comebacks;
    }
    if (z.n == kGwRingLegacy) {
        std::memmove(&z.ring[0], &z.ring[1], sizeof(GwFrame) * (kGwRingLegacy - 1));
        --z.n;
    }
    z.ring[z.n].ext = ext;
    z.ring[z.n].ts = (double)ts;
    z.ring[z.n].rt = 0;
    z.ring[z.n].f = f;
    ++z.n;
    z.last_ms = now;
    ++g_gws.syng_recvd;
}

// Printable ASCII passes through; every other code point (control
// characters, anything outside the tag font, broken UTF-8) becomes one '?'.
int gw_sanitize(const unsigned char *in, int n, char *out, int cap) {
    int o = 0, i = 0;
    while (i < n && o < cap) {
        const unsigned c = in[i];
        if (c >= 0x20 && c <= 0x7e) { out[o++] = (char)c; ++i; continue; }
        int len = (c >= 0xc2 && c <= 0xdf) ? 2
                : (c >= 0xe0 && c <= 0xef) ? 3
                : (c >= 0xf0 && c <= 0xf4) ? 4 : 1;
        int k = 1;
        while (k < len && i + k < n && (in[i + k] & 0xc0) == 0x80) ++k;
        if (k < len) len = 1;
        out[o++] = '?';
        i += len;
    }
    out[o] = 0;
    return o;
}

void gw_on_ident(const unsigned char *buf, int n, int seat) {
    if (n < 24 || n > 64 || (n & 3)) { ++g_gws.drop_len; return; }
    HubIdentHead h;
    std::memcpy(&h, buf, sizeof h);
    if (h.tag != kHubTagIdent) { ++g_gws.drop_tag; return; }
    if (h.version != 1) { ++g_gws.drop_ver; return; }
    if (seat < 0 || seat >= kHubMaxSeats || seat == comms_hub_my_seat()) {
        ++g_gws.drop_seat;
        return;
    }
    if (h.name_len > kHubIdentNameMax || 20 + (int)h.name_len > n ||
        h.character > 3) {
        ++g_gws.drop_field;
        return;
    }
    gw_epoch_check(seat);
    GwSeat &z = g_gw[seat];
    gw_sanitize(buf + 20, h.name_len, z.name, kHubIdentNameMax);
    z.character = h.character;
    std::memcpy(z.body, h.body_rgb, 3);
    std::memcpy(z.shoes, h.shoes_rgb, 3);
    /* GHOSTLOOK1: bit 0 only; the other flag bits stay unread (display data,
       design 6.2), so an identity from any build parses as it did */
    z.colours = (h.flags & kGwIdentColours) != 0;
    z.build_id = h.build_id;
    z.have_ident = true;
    ++g_gws.syni_recvd;
}

void gw_recv() {
    unsigned char buf[kHubMaxDatagram];
    int seat = -1, n;
    unsigned t_arr = 0;
    while ((n = comms_hub_recv_t(kHubGhost, buf, (int)sizeof buf, &seat,
                                 &t_arr)) > 0)
        gw_on_ghost(buf, n, seat, t_arr);
    while ((n = comms_hub_recv(kHubIdent, buf, (int)sizeof buf, &seat)) > 0)
        gw_on_ident(buf, n, seat);
}

// A seat is DRAWN when its newest ghost frame is under 3 s old and names the
// level this console is in. Paused seats keep sending at 2 Hz and stay.
// GHOSTCUT1: THE LOCAL PLAYER IS IN A CUTSCENE. data_0209fc48 is the running
// cutscene (kuppa) script, non-zero from the script's start to its
// EndKuppaScript: the new-file opening, the star-get camera, the ending. While
// it runs Mario is not the player's to move, so nobody else's ghost is drawn on
// this console and this console's own body goes off the wire (its last frame
// fades out on the others the way a dropped player's does); both come back on
// the first frame the script is over. Stage::CheckInput keys its stick block off
// the same word (src/_ZN5Stage10CheckInputEv.cpp).
// SM64DS_GHOST_CUT_SHOW=1 leaves this out (the before arm, for the proof).
bool gw_cut() {
    static int show = -1;
    if (show < 0) show = std::getenv("SM64DS_GHOST_CUT_SHOW") ? 1 : 0;
    return !show && data_0209fc48 != 0;
}

// GHOSTDOOR1: 0 not here, 1 here, 2 in another level or another area of mine
// (a ghost that was here a moment ago fades out in place: see gw_visible).
int gw_here(const GwSeat &z) {
    if (z.n == 0) return 0;
    const unsigned now = GetTickCount();
    if ((unsigned)(now - z.last_ms) > (unsigned)kGwDespawnMs) return 0;
    const HubGhostFrame &nw = z.ring[z.n - 1].f;
    if (nw.y < kDeathPlaneY) return 0;                      // fell out
    /* a friend who has been in the pause menu / star select for 2 s fades
       out and is gone, he does not stand frozen for as long as it lasts */
    if (z.paused_now &&
        (unsigned)(now - z.paused_ms) >=
            (unsigned)(kGwPauseFadeMs + kGwPauseFadeLen))
        return 0;
    if ((int)nw.level_id != (int)data_0209f2f8) return 2;
    if (nw.flags & kGwFrameClock) {
        /* his area is not mine: HideArea hid that room's world for me, and a
           friend in it would float in a room I cannot see */
        const int ga = nw.area & ghostdoor::kAreaMask,
                  ma = ghostdoor::pack_area(data_02092120);
        if (ga >= 1 && ga <= 14 && ma >= 1 && ma <= 14 && ga != ma) return 2;
    }
    return 1;
}

bool gw_visible(int seat) {
    if (!g_hub || seat < 0 || seat >= kHubMaxSeats) return false;
    GwSeat &z = g_gw[seat];
    const unsigned now = GetTickCount();
    z.leaving = false;
    const int h = gw_here(z);
    if (h == 1) {
        z.here_ms = now;
        z.leave_cut = 0;
        return true;
    }
    /* LEAVING: here a moment ago, drawn in this level and area, and now his
       newest frame names another level or area: keep drawing his last
       sample, fading out over kGwLeaveMs, so a door or a painting takes him
       away instead of deleting him in a frame */
    if (h == 2 && z.last_have && z.last_lvl == (int)data_0209f2f8 &&
        z.last_area == (int)data_02092120 &&
        (unsigned)(now - z.here_ms) < (unsigned)kGwLeaveMs) {
        z.leaving = true;
        z.leave_cut = (int)(256u * (now - z.here_ms) / (unsigned)kGwLeaveMs);
        return true;
    }
    z.last_have = false;
    return false;
}

struct GwSample {
    int x, y, z;
    short yaw;
    unsigned short anim;
    int cursor;              // 20.12
    unsigned char character;
    bool teleport;
    bool grounded;
    char mode;               // GHOSTSMOOTH1 probe: i interp, r reckon, o older
                             // than the ring, t / h held (see gw_count)
    int  buf_ms;             // the draw delay behind the sender's clock
    unsigned char door_kind;   // GHOSTDOOR1: from the frame at the draw time
    unsigned char door_frame;
    bool hidden;               // ...and: his own state hides his body
};
GwSample g_gw_last[kHubMaxSeats];   // GHOSTDOOR1: the last sample drawn per seat

// GHOSTDOOR1: the door look the frame carries (ghost_door.h). Only a
// frame-clock frame (a build-6 sender) means it; the legacy 15 Hz stream's
// vy is a real velocity.
inline void gw_look_of(const HubGhostFrame &f, GwSample *o) {
    if (f.flags & kGwFrameClock) {
        o->door_kind = (unsigned char)(f.area >> ghostdoor::kDoorShift);
        o->door_frame = o->door_kind ? (unsigned char)(f.vy & 0xff) : 0;
        o->hidden = (f.flags & ghostdoor::kFlagHidden) != 0;
    } else {
        o->door_kind = 0;
        o->door_frame = 0;
        o->hidden = false;
    }
}

inline int gw_lerp(int a, int b, long long num, long long den) {
    return a + (int)(((long long)(b - a) * num) / den);
}

// GHOSTCHAR1: 0 interpolated, 1 reckoned, 2 held (nothing older than the
// oldest frame), 3 held (the next frame is a teleport), 4 held (paused or a
// teleport frame is the newest).
void gw_count(int seat, int kind) {
    GwSeat &m = g_gw[seat];
    ++m.sm_n;
    if (kind >= 2) {
        ++g_gws.hold;
        ++m.sm_streak;
        if (m.sm_streak > m.sm_streak_max) m.sm_streak_max = m.sm_streak;
        if (kind == 2) ++m.sm_hold_old;
        else if (kind == 3) ++m.sm_hold_tele;
        else ++m.sm_hold_still;
    } else {
        m.sm_streak = 0;
        if (kind == 0) { ++g_gws.interp; ++m.sm_interp; }
        else { ++g_gws.reckon; ++m.sm_reckon; }
    }
}

// The seat's body as it was kGwInterpMs ago on the sender's own clock (the
// legacy follower, SM64DS_GHOST_LEGACY=1).
bool gw_sample_legacy(int seat, unsigned now, GwSample *o) {
    const GwSeat &z = g_gw[seat];
    if (z.n == 0 || !z.have_off) return false;
    o->grounded = (z.ring[z.n - 1].f.flags & kGhostGrounded) != 0;
    const long long T = (long long)now - z.off - gw_interp_ms();
    const GwFrame &nw = z.ring[z.n - 1];
    const GwFrame *a = 0, *b = 0;
    for (int k = z.n - 1; k >= 0; --k)
        if (z.ring[k].ts <= (double)T) { a = &z.ring[k]; b = k + 1 < z.n ? &z.ring[k + 1] : 0; break; }
    const long long fms = 1000 / kGameHz;          // 33 ms, one game frame
    if (!a) {
        /* older than anything held: hold the oldest frame */
        const GwFrame &o0 = z.ring[0];
        o->x = o0.f.x; o->y = o0.f.y; o->z = o0.f.z; o->yaw = o0.f.yaw;
        o->anim = o0.f.anim_id; o->cursor = o0.f.anim_frame;
        o->character = o0.f.character;
        o->teleport = (o0.f.flags & kGhostTeleport) != 0;
        o->mode = 'o'; o->buf_ms = gw_interp_ms();
        gw_count(seat, 2);
        return true;
    }
    o->character = a->f.character;
    if (b && !(b->f.flags & kGhostTeleport)) {
        /* INTERPOLATE between the two frames around T */
        const long long num = T - (long long)a->ts,
                        den = (long long)b->ts - (long long)a->ts;
        o->x = gw_lerp(a->f.x, b->f.x, num, den);
        o->y = gw_lerp(a->f.y, b->f.y, num, den);
        o->z = gw_lerp(a->f.z, b->f.z, num, den);
        const short dy = (short)(b->f.yaw - a->f.yaw);   // the short way round
        o->yaw = (short)(a->f.yaw + (int)((long long)dy * num / den));
        o->teleport = false;
        if (a->f.anim_id == b->f.anim_id && b->f.anim_frame >= a->f.anim_frame) {
            o->anim = a->f.anim_id;                        // same cycle
            o->cursor = gw_lerp(a->f.anim_frame, b->f.anim_frame, num, den);
        } else {
            /* the cycle wrapped or the animation changed between the two:
               walk back from b at speed 1.0 while that stays inside b's
               animation, otherwise carry a forward at speed 1.0 */
            const long long back = (den - num) * 4096 / fms;
            if ((long long)b->f.anim_frame - back >= 0) {
                o->anim = b->f.anim_id;
                o->cursor = (int)(b->f.anim_frame - back);
            } else {
                o->anim = a->f.anim_id;
                o->cursor = (int)(a->f.anim_frame + num * 4096 / fms);
            }
        }
        o->mode = 'i'; o->buf_ms = gw_interp_ms();
        gw_count(seat, 0);
        return true;
    }
    if (b) {
        /* the next frame is a teleport: stay on a until T reaches it */
        o->x = a->f.x; o->y = a->f.y; o->z = a->f.z; o->yaw = a->f.yaw;
        o->anim = a->f.anim_id; o->cursor = a->f.anim_frame;
        o->teleport = false;
        o->mode = 't'; o->buf_ms = gw_interp_ms();
        gw_count(seat, 3);
        return true;
    }
    /* PAST THE NEWEST FRAME: dead-reckon on the ground axes for at most
       kGwReckonMs at the sender's own per-frame velocity (Y is a parabola
       under gravity and is held, the measured reason in apply_snapshot), and
       carry the cursor at speed 1.0. A paused or teleport frame holds. */
    long long ex = T - (long long)nw.ts;
    if (ex > kGwReckonLegacyMs) ex = kGwReckonLegacyMs;
    const bool still = (nw.f.flags & (kGhostPaused | kGhostTeleport)) != 0;
    o->x = nw.f.x; o->y = nw.f.y; o->z = nw.f.z; o->yaw = nw.f.yaw;
    o->anim = nw.f.anim_id; o->cursor = nw.f.anim_frame;
    o->character = nw.f.character;
    o->teleport = (nw.f.flags & kGhostTeleport) != 0;
    if (!still && ex > 0) {
        o->x += (int)((long long)nw.f.vx * ex / fms);
        o->z += (int)((long long)nw.f.vz * ex / fms);
        o->cursor += (int)(ex * 4096 / fms);
        o->mode = 'r'; o->buf_ms = gw_interp_ms();
        gw_count(seat, 1);
    } else {
        o->mode = 'h'; o->buf_ms = gw_interp_ms();
        gw_count(seat, 4);
    }
    return true;
}

// GHOSTSMOOTH1: THE FRAME-CLOCK FOLLOWER, the puppet follower (vs_arena.cpp
// pup_sample) for a hub ghost. The seat's body as its console had it at the
// draw time:
//   * the clock offset is the minimum of (arrival - sender wall clock stamp) over
//     the newest kGwOffN frames (the link's best recent delay, whatever the two
//     machines' clocks read);
//   * B, how far behind the newest frame the body is drawn, is 1.5 frame
//     intervals (50 ms at 30 Hz), raised to what interpolation needs when the
//     stream is not that regular: per frame, the gap the sender left before it
//     (a stalled tick, minus the ticks a standing body skips on purpose) plus how
//     much later the link brought it than its best recent delivery; the 95th
//     percentile of that over the ring, never above 120 ms; moved by at most
//     1 ms a pass so a change never shows as a jump;
//   * between two frames it interpolates; past the newest it dead-reckons the
//     ground axes at the sender's own per-tick step for at most kGwReckonMs,
//     then holds; a teleport frame is never smoothed across.
// The pose half (animation, cursor, bones) is gw_follow's, unchanged.
double gw_buf_target(const GwSeat &z, double lo_all, double fi) {
    const double lo_b = 1.5 * fi, hi_b = fi < 50.0 ? 120.0 : 200.0;
    if (z.n < 8) return lo_b;
    double ex[kGwRing];
    const int n = z.n - 1;                         // one need per frame after the first
    for (int i = 1; i <= n; ++i) {
        const GwFrame &a = z.ring[i - 1], &b = z.ring[i];
        double gap = (b.ts - a.ts) - (double)(int)(b.ext - a.ext - 1) * fi;
        if (gap < 0) gap = 0;
        double e = ((double)(int)b.rt - b.ts) - lo_all;
        if (e < 0) e = 0;
        ex[i - 1] = gap + e;
    }
    for (int i = 1; i < n; ++i) {                 // insertion sort, n <= 15
        const double v = ex[i];
        int j = i - 1;
        while (j >= 0 && ex[j] > v) { ex[j + 1] = ex[j]; --j; }
        ex[j + 1] = v;
    }
    double b = ex[(n - 1) * 95 / 100];
    if (b < lo_b) b = lo_b;
    if (b > hi_b) b = hi_b;
    return b;
}

inline int gw_lerpd(int a, int b, double num, double den) {
    return a + (int)((double)((long long)b - (long long)a) * num / den);
}

bool gw_sample_fc(int seat, unsigned tnow, GwSample *o) {
    GwSeat &z = g_gw[seat];
    if (z.n == 0 || z.hz <= 0) return false;
    const double fi = 1000.0 / (double)z.hz;           // one sent frame
    const double fms = 1000.0 / (double)kGameHz;       // one game tick
    double lo_n = 0, lo_all = 0;
    for (int k = z.n - 1; k >= 0; --k) {
        const double e = (double)(int)z.ring[k].rt - z.ring[k].ts;
        if (k == z.n - 1 || e < lo_all) lo_all = e;
        if (z.n - 1 - k < kGwOffN && (k == z.n - 1 || e < lo_n)) lo_n = e;
    }
    if (gw_interp_pin() >= 0) {
        z.buf_ms = (double)gw_interp_pin();
    } else {
        const double want = gw_buf_target(z, lo_all, fi);
        if (z.buf_ms <= 0.0) z.buf_ms = 1.5 * fi;
        if (z.buf_ms < want) z.buf_ms = z.buf_ms + 1.0 > want ? want : z.buf_ms + 1.0;
        else if (z.buf_ms > want) z.buf_ms = z.buf_ms - 1.0 < want ? want : z.buf_ms - 1.0;
    }
    o->buf_ms = (int)(z.buf_ms + 0.5);
    const GwFrame &nw = z.ring[z.n - 1];
    /* THE PLAYOUT CLOCK. The draw time the offset gives (target) jumps whenever
       the offset's minimum does: a sender that hitched leaves the newest ten
       frames' minimum stale for a third of a second, and the body is then drawn
       ahead (reckoned) and a moment later pulled back across the same ground
       (measured: 20 units forward, 8 back, on a 120 ms stall). The body is drawn
       at a clock of its own instead, run at the wall rate and steered to the
       target by at most 25 percent of that rate, so a step in the target is
       walked off over a few frames, never jumped; it never runs past the newest
       frame by more than the reckoning cap (a stalled sender is held, not
       carried on); a target more than 250 ms away is a restart and is taken. */
    const double target = (double)(int)tnow - z.buf_ms - lo_n;
    double T = target;
    if (gw_playout()) {
        if (!z.play_have) {
            z.play = target;
            z.play_have = true;
        } else {
            double dtw = (double)(int)(tnow - z.play_t);
            if (dtw < 0) dtw = 0;
            if (dtw > 250) dtw = 250;
            const double adv = z.play + dtw;          // where it stands after this pass
            const double err = target - adv;
            if (err > 250.0 || err < -250.0) {
                z.play = target;
            } else {
                double adj = 0.25 * err;
                const double lim = 0.25 * dtw;
                if (adj > lim) adj = lim;
                if (adj < -lim) adj = -lim;
                z.play = adv + adj;
            }
        }
        z.play_t = tnow;
        if (z.play > nw.ts + kGwReckonMs) z.play = nw.ts + kGwReckonMs;
        T = z.play;
    }
    o->grounded = (nw.f.flags & kGhostGrounded) != 0;
    const GwFrame *a = 0, *b = 0;
    for (int k = z.n - 1; k >= 0; --k)
        if (z.ring[k].ts <= T) { a = &z.ring[k]; b = k + 1 < z.n ? &z.ring[k + 1] : 0; break; }
    if (!a) {                                  // older than anything held
        const GwFrame &o0 = z.ring[0];
        o->x = o0.f.x; o->y = o0.f.y; o->z = o0.f.z; o->yaw = o0.f.yaw;
        o->anim = o0.f.anim_id; o->cursor = o0.f.anim_frame;
        o->character = o0.f.character;
        o->teleport = (o0.f.flags & kGhostTeleport) != 0;
        gw_look_of(o0.f, o);
        o->mode = 'o';
        gw_count(seat, 2);
        return true;
    }
    o->character = a->f.character;
    gw_look_of(a->f, o);
    if (b && !(b->f.flags & kGhostTeleport)) {
        const double num = T - a->ts, den = b->ts - a->ts;
        o->x = gw_lerpd(a->f.x, b->f.x, num, den);
        o->y = gw_lerpd(a->f.y, b->f.y, num, den);
        o->z = gw_lerpd(a->f.z, b->f.z, num, den);
        const short dy = (short)(b->f.yaw - a->f.yaw);   // the short way round
        o->yaw = (short)(a->f.yaw + (int)((double)dy * num / den));
        o->teleport = false;
        if (a->f.anim_id == b->f.anim_id && b->f.anim_frame >= a->f.anim_frame) {
            o->anim = a->f.anim_id;                        // same cycle
            o->cursor = gw_lerpd(a->f.anim_frame, b->f.anim_frame, num, den);
        } else {
            /* the cycle wrapped or the animation changed between the two:
               walk back from b at speed 1.0 while that stays inside b's
               animation, otherwise carry a forward at speed 1.0 */
            const int back = (int)((den - num) * 4096.0 / fms);
            if (b->f.anim_frame - back >= 0) {
                o->anim = b->f.anim_id;
                o->cursor = b->f.anim_frame - back;
            } else {
                o->anim = a->f.anim_id;
                o->cursor = a->f.anim_frame + (int)(num * 4096.0 / fms);
            }
        }
        o->mode = 'i';
        gw_count(seat, 0);
        return true;
    }
    if (b) {
        /* the next frame is a teleport: stay on a until T reaches it */
        o->x = a->f.x; o->y = a->f.y; o->z = a->f.z; o->yaw = a->f.yaw;
        o->anim = a->f.anim_id; o->cursor = a->f.anim_frame;
        o->teleport = false;
        o->mode = 't';
        gw_count(seat, 3);
        return true;
    }
    /* PAST THE NEWEST FRAME: dead-reckon the ground axes for at most
       kGwReckonMs at the sender's own per-tick step (Y is a parabola under
       gravity and is held), carry the cursor at speed 1.0; a paused or
       teleport frame holds. */
    double ex = T - nw.ts;
    if (ex > kGwReckonMs) ex = kGwReckonMs;
    const bool still = (nw.f.flags & (kGhostPaused | kGhostTeleport)) != 0;
    o->x = nw.f.x; o->y = nw.f.y; o->z = nw.f.z;
    o->yaw = nw.f.yaw;
    o->anim = nw.f.anim_id; o->cursor = nw.f.anim_frame;
    o->character = nw.f.character;
    o->teleport = (nw.f.flags & kGhostTeleport) != 0;
    gw_look_of(nw.f, o);
    if (!still && ex > 0.0) {
        o->x += (int)((double)nw.f.vx * ex / fms);
        o->z += (int)((double)nw.f.vz * ex / fms);
        o->cursor += (int)(ex * 4096.0 / fms);
        o->mode = 'r';
        gw_count(seat, 1);
    } else {
        o->mode = 'h';
        gw_count(seat, 4);
    }
    return true;
}

bool gw_sample(int seat, unsigned now, GwSample *o) {
    o->door_kind = 0;
    o->door_frame = 0;
    o->hidden = false;
    return gw_legacy() ? gw_sample_legacy(seat, now, o)
                       : gw_sample_fc(seat, now, o);
}

// FNV-1a over the body's posed bone transforms -- the matrices the draw
// reads (ModelComponents::transforms, one Matrix4x3 per bone). What the
// player sees, as a number: SM64DS_GHOST_POSE_PROBE's witness.
unsigned gw_pose_hash(void *a) {
    const char *ma = (const char *)player::body_model_anim(a);
    if (!ma) return 0;
    const char *comp = ma + 8;                               // ModelComponents
    const char *file = *(const char *const *)comp;           // BMD_File
    const int *xf = *(const int *const *)(comp + 0x0c);      // transforms
    if (!file || !xf) return 0;
    unsigned nb = *(const unsigned *)(file + 4);             // numBones
    if (nb > 128) nb = 128;
    unsigned h = 2166136261u;
    for (unsigned i = 0; i < nb * 12; ++i) {
        h ^= (unsigned)xf[i];
        h *= 16777619u;
    }
    return h;
}

int gw_pose_probe() {
    static int p = -1;
    if (p < 0) {
        const char *e = std::getenv("SM64DS_GHOST_POSE_PROBE");
        p = e ? std::atoi(e) : 0;
        if (p < 0) p = 0;
    }
    return p;
}

// GHOSTLOOK1: SM64DS_GHOST_LOOK_PROBE=<stride>. What a shown ghost body is
// drawn as, read the way Player::Render reads it: the body's character
// (param1, +0x08), the head the render picks (func_ov002_020becf4 with the
// render latches: +0x6db the latched character, +0x6fa / +0x6fe / +0x701), the
// palette row the body's materials name (+0x61c, in 16-byte units of the
// texture palette window) and the first two words of row 0 and of that row,
// the drawn position and the fade; and on the sending side the local body's
// own position on the same clock, so a watcher's drawn path can be laid over
// the source's real one. Reads only; inert unset.
int gw_look_probe() {
    static int p = -1;
    if (p < 0) {
        const char *e = std::getenv("SM64DS_GHOST_LOOK_PROBE");
        p = e ? std::atoi(e) : 0;
        if (p < 0) p = 0;
    }
    return p;
}

// GHOSTSMOOTH1: SM64DS_GHOST_ERR_LOG=1. The drawn-vs-truth rows' input, the
// hub twin of PUPPET1's [pup-truth] / [pup]: one line per game tick for my own
// body as it is when the frame is built ([gwtruth]) and one per shown ghost per
// follower pass ([gwdraw]), both on the hub's own clock (comms_hub_now_ms, the
// clock every console on one machine shares). Reads only; inert unset.
bool gw_err_log() {
    static int p = -1;
    if (p < 0) p = std::getenv("SM64DS_GHOST_ERR_LOG") ? 1 : 0;
    return p != 0;
}

// One sixteen-colour row of the texture palette slots, by the 16-byte unit a
// material's +0x20 names: mapped at the host address the raster decodes them
// from (ntr/gx.cpp PLTT_SLOT_BASE, 0x18000 bytes). 0 when out of the window.
unsigned short *gw_pltt_row(int base16) {
    if (base16 < 0 || base16 * 16 + 32 > 0x18000) return 0;
    return (unsigned short *)(0x06880000u + (unsigned)base16 * 16u);
}

// GHOSTLOOK1: A HUB GHOST IN ITS PLAYER'S OWN COLOURS, the way VS colours its
// Yoshis. VS tells its Yoshis apart by palette ROW: every body's materials name
// the row Player+0x61c holds (row 0 + mPlayerNo << 1 in 16-byte units, written
// by InitResources and repaired to the true slot by hal/level_boot.cpp), and
// Player::Render stamps that into each material before the draw
// (src/_ZN6Player6RenderEv.cpp, the data_0209f2d8 == 1 arm). A VS pick reaches
// the row through the served file (hal/fs_mods.cpp, vs_palette_row over the
// sixteen grown rows). A hub ghost's pick arrives on the wire after the level
// is up, so its row is made here, with the same vs_palette_row out of the same
// row 0 this console's file served, and written into the body's own row of
// the texture palette slots; hal/player_bridges.cpp stamps the row into the
// ghost's materials around its draw. Rows above 0 belong to VS seats, and the
// adventure hub never draws one otherwise; row 0 (this console's own player)
// is never written. Yoshi only, as in VS: the bros have no row per player.
// Returns the row to stamp, or -1 to leave the model's own.
//
// NO CACHE DROP HERE. The first version threw the decoded-texture cache away
// (ntr::gx_invalidate_textures) whenever it rewrote a row, the way the ROM's
// GX::EndLoadTexPltt does. The ROM does that at load time; this runs after the
// frame's render walk has bound its textures and before the raster draws
// them, and every triangle holds a pointer into a decode the drop frees: the
// watcher crashed in the raster on the first frame a coloured ghost showed.
// The drop is not needed. A bind's cache key is the palette base, the texture
// words AND the first two words of the row, and words 0..1 are the body
// family, so a new pick with a new body colour can never be served an old
// decode. The one case that could is a second pick with the SAME body colour
// and other shoes drawn on the same row before the cache is next dropped (a
// level load does it); this keeps, per body row, what has been drawn there
// since the last drop and leaves such a ghost in the model's own colours until
// the next one.
struct GwRowSeen {
    unsigned gen;            // ntr::gx_texture_generation() these were kept in
    int      n;
    unsigned cp[8];          // words 0..1 of a row drawn here (the cache key)
    unsigned sum[8];         // ...and all sixteen words, hashed
};
GwRowSeen g_row_seen[kPortBodySlots];
unsigned short g_row_last[kPortBodySlots][16];   // the last row written
bool g_row_last_have[kPortBodySlots];            // into each body's row

unsigned gw_row_sum(const unsigned short *r) {
    unsigned h = 2166136261u;
    for (int i = 0; i < 16; ++i) { h ^= r[i]; h *= 16777619u; }
    return h;
}

// true when `row` may be drawn on body row `no` (whose words are at `rp`)
// without a cached decode of another row with the same key being served for
// it (and then remembered)
bool gw_row_drawable(int no, const unsigned short *row,
                     const unsigned short *rp) {
    GwRowSeen &rs = g_row_seen[no];
    const unsigned gen = ntr::gx_texture_generation();
    if (rs.gen != gen) {
        /* dropped since: only what the row holds now can have been drawn
           (and cached) after the drop, and port_adventure_ghost_palette
           stamps the row only while it still holds the last row written.
           A level load uploads the file's rows again before anything is
           drawn, so after one nothing is seeded, and a pick held for its
           body colour (same words 0..1 as the row drawn before, other
           shoes) is written in this generation: the hold ends at the first
           drop, not when a different body colour happens to be written on
           the row. */
        rs.gen = gen;
        rs.n = 0;
        if (g_row_last_have[no] &&
            std::memcmp(rp, g_row_last[no], sizeof g_row_last[no]) == 0) {
            std::memcpy(&rs.cp[0], g_row_last[no], 4);
            rs.sum[0] = gw_row_sum(g_row_last[no]);
            rs.n = 1;
        }
    }
    unsigned cp;
    std::memcpy(&cp, row, 4);
    const unsigned sum = gw_row_sum(row);
    bool seen = false;
    for (int k = 0; k < rs.n; ++k) {
        if (rs.cp[k] != cp) continue;
        if (rs.sum[k] != sum) return false;   // same key, other colours
        seen = true;
    }
    if (seen) return true;
    if (rs.n >= 8) return false;
    rs.cp[rs.n] = cp;
    rs.sum[rs.n] = sum;
    ++rs.n;
    return true;
}

int gw_colour_row(GwSeat &z, void *a) {
    if (!z.have_ident || !z.colours) return -1;
    if (gw_char(a) != 3u) return -1;
    const char *c = (const char *)a;
    const int no = (int)*(const unsigned char *)(c + 0x6d8);
    const int pal = *(const int *)(c + 0x61c);
    if (no <= 0 || no >= kPortBodySlots) return -1;
    const unsigned short *r0 = gw_pltt_row(pal - (no << 1));
    unsigned short *rp = gw_pltt_row(pal);
    if (!r0 || !rp) return -1;
    if (!z.pal_have || std::memcmp(z.pal_src, r0, sizeof z.pal_src) != 0 ||
        std::memcmp(z.pal_body, z.body, 3) != 0 ||
        std::memcmp(z.pal_shoes, z.shoes, 3) != 0) {
        std::memcpy(z.pal_src, r0, sizeof z.pal_src);
        std::memcpy(z.pal_body, z.body, 3);
        std::memcpy(z.pal_shoes, z.shoes, 3);
        vspal::vs_palette_row(z.pal_src, z.pal_body, z.pal_shoes, z.pal_out);
        z.pal_have = true;
    }
    if (!gw_row_drawable(no, z.pal_out, rp)) {
        ++g_gws.pal_held;
        return -1;
    }
    /* a level load uploads the file's rows again: write it back when the
       row no longer holds it */
    if (std::memcmp(rp, z.pal_out, sizeof z.pal_out) != 0) {
        std::memcpy(rp, z.pal_out, sizeof z.pal_out);
        ++g_gws.pal_writes;
    }
    std::memcpy(g_row_last[no], z.pal_out, sizeof z.pal_out);
    g_row_last_have[no] = true;
    ++g_gws.pal_rows;
    return pal;
}

void gw_send_ident(unsigned now) {
    gw_me_read();
    unsigned char buf[64];
    std::memset(buf, 0, sizeof buf);
    HubIdentHead h;
    h.tag = kHubTagIdent;
    h.version = 1;
    h.seat = 0xff;
    h.name_len = (unsigned char)g_gw_me.name_len;
    const int me = (int)data_0209f250;
    void *a = (me >= 0 && me < kPortBodySlots) ? data_0209f394[me] : 0;
    h.character = (unsigned char)(a ? gw_char(a) : 0u);
    h.build_id = g_gw_me.build_id;
    std::memcpy(h.body_rgb, g_gw_me.body, 3);
    std::memcpy(h.shoes_rgb, g_gw_me.shoes, 3);
    h.flags = g_gw_me.colours ? (unsigned char)kGwIdentColours : 0;
    h.reserved = 0;
    std::memcpy(buf, &h, sizeof h);
    std::memcpy(buf + 20, g_gw_me.name, (size_t)g_gw_me.name_len);
    int len = (20 + g_gw_me.name_len + 3) & ~3;
    if (len < 24) len = 24;
    if (comms_hub_send(buf, len)) {
        ++g_gws.syni_sent;
        g_gw_ident_sent = true;
        g_gw_last_ident_ms = now;
    } else {
        ++g_gws.send_fail;
    }
}

// FIX9B: the identity (name, colours, character) goes out on its own clock,
// not behind a body frame. It used to ride the tail of gw_send, which returns
// early while a cutscene runs (GHOSTCUT1) and whenever no frame is due, so a
// console in the opening sent no SYNI at all and the others read "Player n"
// in chat until the cutscene was over. SYNI needs no body.
void gw_ident_tick(unsigned now) {
    if (comms_hub_state() != kHubLive) return;
    const unsigned roster = comms_hub_roster_version();
    const bool roster_moved = roster != g_gw_roster;
    if (!g_gw_ident_sent ||
        (unsigned)(now - g_gw_last_ident_ms) >= (unsigned)kGwIdentMs ||
        (roster_moved &&
         (unsigned)(now - g_gw_last_ident_ms) >= (unsigned)kGwIdentGapMs)) {
        g_gw_roster = roster;
        gw_send_ident(now);
    }
}

// THE SEND SIDE: my own body only, once per 15 Hz wall tick.
void gw_send(unsigned now) {
    if (comms_hub_state() != kHubLive) return;
    const int me = (int)data_0209f250;
    if (me < 0 || me >= kPortBodySlots) return;
    void *a = data_0209f394[me];
    if (!a) return;

    /* PAUSED: the game is not advancing my body. Two ways to see it, both
       observations rather than a pause-state variable the port would have to
       name: Stage::Behavior did not run since the last tick (the host menu, a
       loading hold), or the body has not changed at all -- position, facing,
       animation AND its cursor -- for 8 frames. The ROM's own pause menu is
       the second kind: Stage::Behavior keeps running it, and the player's
       animation stops dead (measured: anim 72 cursor 11 on every frame from
       the START press on). A player standing idle still has a looping idle
       animation turning over, so standing is not paused. */
    const unsigned beh = port_stage_behavior_calls();
    const bool no_tick = g_gw_beh_seeded && beh == g_gw_beh_last;
    g_gw_beh_last = beh;
    g_gw_beh_seeded = true;

    const int cx = *player::pos_x(a), cy = *player::pos_y(a),
              cz = *player::pos_z(a);
    if (gw_err_log())
        std::fprintf(stderr, "[gwtruth] t=%u f=%u pos=%d,%d,%d yaw=%d anim=%u\n",
                     comms_hub_now_ms(), g_gw_frame, cx, cy, cz,
                     (int)*player::facing(a), (unsigned)player::anim_id(a));
    const int fz[6] = {cx, cy, cz, (int)*player::facing(a),
                       (int)player::anim_id(a), player::anim_frame(a)};
    if (std::memcmp(fz, g_gw_fz, sizeof fz) == 0) {
        if (g_gw_frozen < 1000) ++g_gw_frozen;
    } else {
        g_gw_frozen = 0;
        std::memcpy(g_gw_fz, fz, sizeof fz);
    }
    const bool paused = no_tick || g_gw_frozen >= 8;
    const int level = (int)data_0209f2f8;
    if (!paused) {
        if (g_gw_prev_have && level == g_gw_prev_level) {
            const int d[3] = {cx - g_gw_prev[0], cy - g_gw_prev[1],
                              cz - g_gw_prev[2]};
            const int step = (d[0] < 0 ? -d[0] : d[0]) +
                             (d[1] < 0 ? -d[1] : d[1]) +
                             (d[2] < 0 ? -d[2] : d[2]);
            if (step > units(gw_snap_units(g_cfg.snap_units))) {
                g_gw_warp = true;
                g_gw_v[0] = g_gw_v[1] = g_gw_v[2] = 0;
            } else {
                for (int k = 0; k < 3; ++k) g_gw_v[k] = d[k];
            }
        } else {
            if (g_gw_prev_have) g_gw_warp = true;     // a level change
            g_gw_v[0] = g_gw_v[1] = g_gw_v[2] = 0;
        }
        g_gw_prev[0] = cx; g_gw_prev[1] = cy; g_gw_prev[2] = cz;
        g_gw_prev_have = true;
        g_gw_prev_level = level;
    }

    /* GHOSTDOOR1 (A3): THE DOOR TOUCH IS A TELEPORT. func_ov100_021451c4 ->
       func_ov100_02145170 puts the player 100 units in front of (or behind)
       the door and turns him to the door's heading in the SAME tick the door
       takes him (CanEnterDoor -> the no-control state), up to ~175 units and
       45 degrees from where he stood: under gw_snap_units, so it went out as
       a walk and the others slid and spun him. The first tick of a door
       sequence is flagged as a teleport, so they hold him and snap. */
    const unsigned char dkind = gw_legacy() ? 0 : ghostdoor::sender_kind(a);
    if (!gw_legacy()) ghostdoor::sender_probe(a, g_gw_frame, dkind);
    if (dkind && !g_gw_prev_dkind) {
        g_gw_warp = true;
        g_gw_v[0] = g_gw_v[1] = g_gw_v[2] = 0;
        ++g_gws.door_snaps;
    }
    g_gw_prev_dkind = dkind;

    if (gw_pose_probe() > 0 && (g_gw_frame % (unsigned)gw_pose_probe()) == 0)
        std::fprintf(stderr, "[gwpose] t=%u f=%u LOCAL seat=%d anim=%u cur=%d "
                     "hash=%08x\n", now, g_gw_frame, comms_hub_my_seat(),
                     (unsigned)player::anim_id(a), player::anim_frame(a) >> 12,
                     gw_pose_hash(a));
    if (gw_look_probe() > 0 && (g_gw_frame % (unsigned)gw_look_probe()) == 0)
        std::fprintf(stderr, "[gwlook] t=%u f=%u LOCAL seat=%d pos=(%d,%d,%d) "
                     "anim=%u\n", now, g_gw_frame, comms_hub_my_seat(),
                     cx >> 12, cy >> 12, cz >> 12,
                     (unsigned)player::anim_id(a));

    /* GHOSTSMOOTH1: THE FRAME CLOCK. seq is this console's own game tick count
       (30 a second) since its first frame, and a frame goes out on every tick
       whose body differs from the last frame sent (position, facing, animation,
       character, ground / talking bits, level) and every 3rd tick otherwise (a
       standing body, 10 Hz); 2 Hz while paused. The receiver then reads the
       sender's own game time off the number, where the old wall tick (15 a
       second, taken at the first tick after each boundary) put half the
       frames at the wrong time. SM64DS_GHOST_LEGACY=1 sends the old stream. */
    const bool fc = !gw_legacy();
    unsigned seq;
    if (fc) {
        if (!g_gw_t0_set) { g_gw_f0 = g_gw_frame; g_gw_t0_set = true; }
        seq = (g_gw_frame - g_gw_f0) & 0xffffu;
        if (paused && (unsigned)(now - g_gw_last_send_ms) < (unsigned)kGwPausedMs)
            return;
    } else {
        if (!g_gw_t0_set) { g_gw_t0 = now; g_gw_t0_set = true; }
        seq = (unsigned)((unsigned long long)(now - g_gw_t0) *
                         (unsigned)gw_send_hz() / 1000u);
        if (seq == g_gw_last_seq) return;
        if (paused && (unsigned)(now - g_gw_last_send_ms) < (unsigned)kGwPausedMs)
            return;
    }
    if (gw_cut()) {
        /* GHOSTCUT1: in a cutscene: no ghost frame. The first frame after it
           is a teleport, so the others snap the ghost back in rather than
           slide it from where it was before the cutscene. */
        ++g_gws.cut_nosend;
        g_gw_last_seq = seq;
        g_gw_warp = true;
        return;
    }

    HubGhostFrame f;
    std::memset(&f, 0, sizeof f);
    f.tag = kHubTagGhost;
    f.version = 1;
    f.seat = 0xff;
    f.flags = (unsigned char)(kGhostLive |
                              (player::on_ground(a) ? kGhostGrounded : 0) |
                              (g_gw_warp ? kGhostTeleport : 0) |
                              (paused ? kGhostPaused : 0) |
                              (voice_talking() ? kGhostTalking : 0));
    f.character = (unsigned char)gw_char(a);
    f.seq = (unsigned short)seq;
    f.level_id = (unsigned short)(level & 0xff);
    f.x = cx; f.y = cy; f.z = cz;
    /* per-frame Fix12 delta, clamped to the s16 the frame carries (design
       6.1): about 8 units a frame, so a dash reckons short -- the 100 ms
       interpolation, not the reckoning, is what normally draws the body */
    short vv[3];
    for (int k = 0; k < 3; ++k) {
        int v = g_gw_warp ? 0 : g_gw_v[k];
        if (v > 32767) v = 32767; else if (v < -32767) v = -32767;
        vv[k] = (short)v;
    }
    f.vx = vv[0]; f.vy = vv[1]; f.vz = vv[2];
    f.yaw = *player::facing(a);
    f.anim_id = player::anim_id(a);
    f.anim_frame = player::anim_frame(a);
    /* SMALLS9: in a coop room the player's OWN star count this session (hal/star_tally.h);
       everywhere else the save total, as before */
    f.stars = (unsigned short)star_tally_wire_value((int)NumStars());
    f.area = 0;
    if (fc) {
        f.flags |= (unsigned char)kGwFrameClock;
        /* GHOSTDOOR1: the door kind and the door's own frame (ghost_door.h);
           vy is a velocity the follower never reads */
        f.area = (unsigned char)((dkind << ghostdoor::kDoorShift) |
                                 ghostdoor::pack_area(data_02092120));
        if (ghostdoor::sender_hidden(a)) f.flags |= ghostdoor::kFlagHidden;

        f.vy = dkind ? (short)ghostdoor::sender_frame(a) : (short)0;
        {
            const unsigned st = comms_hub_now_ms() & 0xffffffu;
            f.reserved[0] = (unsigned char)(st & 0xff);
            f.reserved[1] = (unsigned char)((st >> 8) & 0xff);
            f.reserved[2] = (unsigned char)((st >> 16) & 0xff);
        }
        const unsigned char look = (unsigned char)(kGhostGrounded | kGhostTalking |
                                                   kGhostPaused |
                                                   ghostdoor::kFlagHidden);
        const bool moved = !g_gw_sent_have || g_gw_warp ||
            f.x != g_gw_sent.x || f.y != g_gw_sent.y || f.z != g_gw_sent.z ||
            f.yaw != g_gw_sent.yaw || f.anim_id != g_gw_sent.anim_id ||
            f.character != g_gw_sent.character ||
            f.level_id != g_gw_sent.level_id ||
            f.stars != g_gw_sent.stars ||
            f.area != g_gw_sent.area || f.vy != g_gw_sent.vy ||
            (f.flags & look) != (g_gw_sent.flags & look);
        const bool still_due = !g_gw_sent_have ||
            ((seq - g_gw_last_seq) & 0xffffu) >= (unsigned)kGwStillEvery;
        if (!moved && !still_due) return;
        /* two ticks never closer than kGwMinSendGapMs: a console that catches
           up after a stall runs several ticks back to back, and the relay
           holds a seat to 35 SYNG a second; a teleport still goes at once */
        if (!g_gw_warp && g_gw_sent_have &&
            (unsigned)(now - g_gw_last_send_ms) < (unsigned)kGwMinSendGapMs)
            return;
    }
    if (comms_hub_send(&f, (int)sizeof f)) {
        ++g_gws.syng_sent;
        if (paused) ++g_gws.syng_paused;
        g_gw_last_seq = seq;
        g_gw_sent = f;
        g_gw_sent_have = true;
        g_gw_last_send_ms = now;
        g_gw_warp = false;                 // the flag reached the wire
    } else {
        ++g_gws.send_fail;
    }
}

void gw_report(const char *tag) {
    unsigned vis = 0, drawn = 0;
    for (int s = 0; s < kHubMaxSeats; ++s)
        if (gw_visible(s)) vis |= 1u << s;
    for (int b = 0; b < kPortBodySlots; ++b) {
        const int s = gw_seat_for_body(b);
        if (s >= 0) drawn |= 1u << s;
    }
    char names[kHubMaxSeats * (kHubIdentNameMax + 6) + 1];
    int w = 0;
    names[0] = 0;
    for (int s = 0; s < kHubMaxSeats; ++s) {
        if (!g_gw[s].have_ident) continue;
        const int r = std::snprintf(names + w, sizeof names - (size_t)w,
                                    "%s%d:%s", w ? "," : "", s, g_gw[s].name);
        if (r < 0 || w + r >= (int)sizeof names) break;
        w += r;
    }
    std::fprintf(stderr,
                 "[ghost:%s] hub=1 seat=%d level=%d syng_sent=%llu "
                 "syng_paused=%llu syni_sent=%llu send_fail=%llu "
                 "syng_recvd=%llu syni_recvd=%llu drop_len=%llu drop_tag=%llu "
                 "drop_ver=%llu drop_seat=%llu drop_field=%llu drop_old=%llu "
                 "resets=%llu vis=0x%04x drawn=0x%04x interp=%llu reckon=%llu hold=%llu "
                 "reposes=%llu recursor=%llu charswaps=%llu parked=%llu below=%llu "
                 "clamped=%llu target_ghost=%llu near_ghost=%llu seat_fix=%llu "
                 "adopted=%llu cut_nosend=%llu cut_frames=%llu cut_drawn=%llu "
                 "shown_frames=%llu comebacks=%llu pal_rows=%llu "
                 "pal_writes=%llu pal_held=%llu door_snaps=%llu cut=%d names=[%s]\n",
                 tag ? tag : "-", comms_hub_my_seat(), (int)data_0209f2f8,
                 g_gws.syng_sent, g_gws.syng_paused, g_gws.syni_sent,
                 g_gws.send_fail, g_gws.syng_recvd, g_gws.syni_recvd,
                 g_gws.drop_len, g_gws.drop_tag, g_gws.drop_ver,
                 g_gws.drop_seat, g_gws.drop_field, g_gws.drop_old,
                 g_gws.resets, vis, drawn, g_gws.interp, g_gws.reckon, g_gws.hold,
                 g_gws.reposes, g_gws.recursor, g_gws.charswaps, g_gws.parked,
                 g_gws.below, g_gws.clamped, g_gws.target_ghost,
                 g_gws.near_ghost, g_gws.seat_fix, g_gws.adopted,
                 g_gws.cut_nosend, g_gws.cut_frames, g_gws.cut_drawn,
                 g_gws.shown_frames, g_gws.comebacks, g_gws.pal_rows,
                 g_gws.pal_writes, g_gws.pal_held, g_gws.door_snaps,
                 data_0209fc48 != 0 ? 1 : 0, names);
}

// sync_decide's hub arm: an adventure console on the HUB has no DS transport
// by design (hal/comms_hub.h: the ROM stays single-player), so the ghost layer
// runs on the hub's own send/receive instead of the transport's aux entries.
bool gw_decide() {
    if (!(adventure_ghost_mode() && comms_hub_active())) return false;
    g_hub = true;
    g_enabled = true;
    gw_me_read();
    if (gw_legacy())
        std::fprintf(stderr,
                     "[ghost] adventure ghosts on the hub (LEGACY stream): SYNG "
                     "%d Hz wall clock (2 Hz paused), SYNI every %d s, drawn %d "
                     "ms behind, reckon <= %d ms, fades from %d ms and gone at "
                     "%d ms without a frame; name \"%s\"\n",
                     gw_send_hz(), (int)kGwIdentMs / 1000, gw_interp_ms(),
                     (int)kGwReckonLegacyMs, (int)kGwFadeMs, (int)kGwDespawnMs,
                     g_gw_me.name);
    else
        std::fprintf(stderr,
                     "[ghost] adventure ghosts on the hub: SYNG on the game "
                     "tick clock (30 Hz moving, 10 Hz standing, 2 Hz paused), "
                     "SYNI every %d s, drawn %s behind, reckon <= %d ms, "
                     "fades from %d ms and gone at %d ms without a frame; "
                     "name \"%s\"\n",
                     (int)kGwIdentMs / 1000,
                     gw_interp_pin() >= 0 ? "a pinned delay" : "50..100 ms",
                     (int)kGwReckonMs, (int)kGwFadeMs, (int)kGwDespawnMs,
                     g_gw_me.name);
    return true;
}

void gw_target_probe();   // with the follower, below

// KNOWNNET1: A SEAT THAT LEFT IS FORGOTTEN WHEN IT LEAVES. The identity (name, character, colours) of a seat was only dropped when
// the seat's join epoch changed, i.e. when somebody took the seat again (gw_epoch_check), so a player who left stayed in this
// console's names for as long as the seat stood empty. The hub's roster says when a seat goes (the relay's goodbye or 3 s unheard,
// hal/comms_hub.cpp seat_leave), the way hal/player_list.cpp already reads it: the ghost layer now forgets the seat on that frame.
void gw_forget_left() {
    const int my = comms_hub_my_seat();
    for (int s = 0; s < kHubMaxSeats; ++s) {
        GwSeat &z = g_gw[s];
        if (s == my || (!z.have_ident && z.n == 0)) continue;
        HubSeatInfo si;
        if (comms_hub_seat_info(s, &si) && si.present) continue;
        std::fprintf(stderr, "[ghost] seat %d (\"%s\") left the hub: its name and ghost are forgotten\n", s,
                     z.have_ident ? z.name : "");
        gw_reset_seat(z, false);
    }
}

// One frame of the hub ghost layer, from sync_tick.
void gw_tick() {
    const unsigned now = GetTickCount();
    ++g_gw_frame;
    gw_target_probe();
    gw_recv();
    gw_forget_left();
    gw_assign(now);
    gw_ident_tick(now);
    gw_send(now);
    static int every = -1;
    if (every < 0) {
        const char *e = std::getenv("SM64DS_GHOST_REPORT_S");
        every = e ? std::atoi(e)
                  : (g_cfg.report ? 5 : 0);
        if (every < 0) every = 0;
    }
    if (every > 0 && !g_gw_quiet) {   /* SEAMHUB1: the in-place leave stops the report at the quit */
        if (!g_gw_last_report_ms) g_gw_last_report_ms = now;
        if ((unsigned)(now - g_gw_last_report_ms) >= (unsigned)every * 1000u) {
            g_gw_last_report_ms = now;
            gw_report("tick");
        }
    }
}

}  // namespace

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

    // ---- BUT NOT OVER A SCRIPTED SEQUENCE. Lane VSMERCY, MEASURED.
    //
    // A no-control state does not merely play a pose, it ENDS ON ONE. A VS
    // star collect parks the body in St_NoControl step 2 and
    // func_ov002_020c92fc leaves it only when Player::FinishedAnim says the
    // star-get animation (0x19, 42 frames in the cartridge's own file) has
    // run out. Player::ChangeState is what then clears mIsNoControl and puts
    // the body collider back, so that animation IS the mercy window the ROM
    // gives a collector.
    //
    // Applying the wire's animation on top of it breaks the exit condition,
    // and at any real latency it breaks it PERMANENTLY. Measured on the
    // two-console fixture (port/tools/vsmercy_probe.py, evidence in
    // C:\tmp\vsmercy-out): the non-collecting console enters the collect on
    // the right frame, the id-change branch below then yanks the pose back to
    // the stale walk animation on the very next frame, the wire's own 0x19
    // arrives a latency later and RESTARTS the pose, and by the time it
    // finishes the wire is already saying "walk" again -- so SetAnim installs
    // a LOOPING animation, whose Animation::Finished
    // (`currFrame >= (numFrames << 12) - 1`) is never true because Advance
    // wraps modulo the length and the cursor never lands on that value. The
    // state machine never leaves. At 264 ms round trip the collector's body
    // was still in no-control 458 frames later, and it reproduced at 132 ms
    // and at 71 ms; only the ~15 ms loopback pair was clean. What the other
    // player sees is a body that walks and animates but can neither be hit
    // nor land a hit for the rest of the match, because mIsNoControl and
    // mIsBodyClsnEnabled both stay stuck.
    //
    // THE REFUSAL IS THE FIX AND IT IS ALSO THE MORE FAITHFUL ANSWER. This
    // console is running the ROM's own collect on its own copy -- the same
    // matched state machine, the same animation file, the same 42 frames --
    // so its LOCAL pose is better information than a latency-old remote one.
    // Position and facing are still applied by the caller; only the pose is
    // left to the state machine that owns it. It re-arms by itself: the next
    // ChangeState clears mIsNoControl and the id-change branch takes over
    // again on the following snapshot.
    // ---- EXCEPT IN ADVENTURE-GHOST MODE, where the no-control flag is OURS.
    // The pose-hold above protects a LOCALLY-SIMULATED body running the ROM's
    // own scripted sequence (a VS star collect) from a stale wire animation.
    // A ghost is not that: it is a puppet this console does not simulate, and
    // its mIsNoControl is set by the ghost hold every frame to keep it
    // non-colliding (hal/player_bridges.cpp), not by a real state machine. So
    // the only true information about a ghost's pose is the wire's, and the
    // hold's flag must not suppress it. In adventure mode every apply target is
    // a remote puppet (the slot==me skip in apply_snapshot stands), so the
    // bypass is exactly the ghost bodies and never the local one.
    if (!adventure_ghost_mode() &&
        *(const unsigned char *)((const char *)a + player::kIsNoControl)) {
        ++g_stats.pose_held;
        return;
    }

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
    if (me < 0 || me >= kPortBodySlots) return;
    void *a = data_0209f394[me];
    if (!a) return;

    ++g_frame;
    if (g_frame % 30u == 0 && g_ev_tokens < kEvBurst) ++g_ev_tokens;

    /* Item 2: the per-frame velocity sample, taken EVERY frame whether or not
       this frame sends, so a cadence send never ships a delta spanning
       several frames as if it were one. A single frame further than the snap
       threshold is a warp: velocity is zeroed and the entry will carry the
       teleport flag, because extrapolating through a warp aims at a place
       nobody is. */
    const int cx = *player::pos_x(a), cy = *player::pos_y(a),
              cz = *player::pos_z(a);
    bool warped = false;
    if (g_vel_seeded) {
        g_vel[0] = cx - g_vel_prev[0];
        g_vel[1] = cy - g_vel_prev[1];
        g_vel[2] = cz - g_vel_prev[2];
        const int step = (g_vel[0] < 0 ? -g_vel[0] : g_vel[0]) +
                         (g_vel[1] < 0 ? -g_vel[1] : g_vel[1]) +
                         (g_vel[2] < 0 ? -g_vel[2] : g_vel[2]);
        if (step > units(g_cfg.snap_units)) {
            g_warp_pending = true;
            g_vel[0] = g_vel[1] = g_vel[2] = 0;
        }
    } else {
        g_vel[0] = g_vel[1] = g_vel[2] = 0;
    }
    g_vel_seeded = true;
    g_vel_prev[0] = cx; g_vel_prev[1] = cy; g_vel_prev[2] = cz;
    warped = g_warp_pending;

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
                               (player::on_ground(a) ? kFlagGrounded : 0) |
                               (warped ? kFlagTeleport : 0));
    e->yaw   = *player::facing(a);
    e->x     = cx;
    e->y     = cy;
    e->z     = cz;
    e->anim_id    = player::anim_id(a);
    e->state_id   = player::state_id(a);
    e->anim_frame = player::anim_frame(a);
    e->vx = g_vel[0];
    e->vy = g_vel[1];
    e->vz = g_vel[2];
    /* v4: the level this console is in, so a receiver ghosts only peers in its
       own level. data_0209f2f8 is signed (levels 0..51); widen through int so a
       hypothetical negative id does not sign-extend into the u16. */
    e->level_id = (unsigned short)(int)data_0209f2f8;

    const int len = (int)sizeof(SyncMsgV1) + (int)sizeof(SyncPlayerV1);
    if (t->send_aux(buf, len) == len) {
        ++g_stats.sent;
        g_warp_pending = false;        // the flag reached the wire
    }
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
    void *a_me = (me >= 0 && me < kPortBodySlots) ? data_0209f394[me] : 0;
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
        if (slot < 0 || slot >= kPortBodySlots) continue;
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

        /* ADVENTURE GHOSTS: a ghost is a PURE FOLLOWER. Record this snapshot as
           the slot's follow target and apply the pose, but do NOT move the body
           here -- port_adventure_ghost_follow eases it toward the target every
           render frame, which is what makes it smooth instead of stepping on the
           receive frames and drifting on ROM physics between them. This slot is
           never the local one (the slot==me skip above stands), so local_writes
           stays 0. VS mode falls through to the correction logic below unchanged. */
        if (adventure_ghost_mode()) {
            /* PRESENCE (M2): record this peer's level and mark it seen NOW, so
               peer_visible() can tell whether this slot shares our level and
               whether it is still live. Recorded for EVERY received snapshot,
               same-level or not -- the level compare is peer_visible()'s job, and
               a body only drops out of visible when its level stops matching or
               its snapshots stop arriving. */
            g_peer_level[slot] = (int)e->level_id;
            g_peer_seen_ms[slot] = GetTickCount();
            g_peer_present[slot] = true;
            g_gtarget[slot].have = true;
            g_gtarget[slot].x = e->x;
            g_gtarget[slot].y = e->y;
            g_gtarget[slot].z = e->z;
            g_gtarget[slot].yaw = e->yaw;
            g_gtarget[slot].teleport = (e->flags & kFlagTeleport) != 0;
            /* the sender's own per-frame velocity, so the follower can advance
               the target continuously between the ~30 Hz snapshots instead of
               letting it step. */
            g_gtarget[slot].vx = e->vx;
            g_gtarget[slot].vz = e->vz;
            /* the animation is not applied here -- the follower drives it every
               render frame so the ROM's per-frame reset cannot flicker it. Just
               record the wire's id and re-sync the playback cursor to the
               sender's own so the ghost stays in phase with its walk cycle. */
            g_ganim[slot] = e->anim_id;
            g_gcursor[slot] = e->anim_frame;
            g_ganim_have[slot] = true;
            ++g_stats.applied;
            continue;
        }

        int *px = player::pos_x(a);
        int *py = player::pos_y(a);
        int *pz = player::pos_z(a);

        /* Item 2, DEAD RECKONING: the correction aims at where the sender IS
           NOW (estimated), not where it was one latency ago. The received
           position is advanced by the sender's own per-frame delta times the
           message age in frames -- before this, the lerp chased a point that
           was always ~one latency stale, so a walking body's error grew
           between snapshots and released as the visible slide-then-snap. Age
           is rtt/2 off the rig's probe; with no RTT sample yet the age is 0
           and this degenerates to exactly the old behaviour. A teleport
           carries zero velocity and is never extrapolated. */
        /* GROUND AXES ONLY, VELOCITY CLAMPED -- both measured, not cautious.
           The first cut extrapolated all three axes with the raw delta and
           made the session WORSE than no reckoning at all (worst error 442
           units against the un-reckoned 98): a legitimate high-speed frame --
           a fall, a knockback -- passes the warp guard at up to 60 units and
           times ten frames of age it aims the target across the map. And
           vertical motion is a parabola under gravity, so extrapolating it
           linearly is wrong by construction, not just by magnitude. So Y
           takes the received value untouched, and X/Z velocity is clamped to
           3 units/frame -- above any locomotion, below the spikes. */
        int tx = e->x, ty = e->y, tz = e->z;
        if (!(e->flags & kFlagTeleport) && !g_cfg.no_dr) {
            const int age_frames = (g_stats.rtt_avg_ms / 2) * 60 / 1000;
            int vx = e->vx, vz = e->vz;
            const int vcap = units(3);
            if (vx > vcap) vx = vcap; else if (vx < -vcap) vx = -vcap;
            if (vz > vcap) vz = vcap; else if (vz < -vcap) vz = -vcap;
            tx += vx * age_frames;
            tz += vz * age_frames;
        }
        const int dx = tx - *px, dy = ty - *py, dz = tz - *pz;
        int err = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy) + (dz < 0 ? -dz : dz);
        if (err > g_stats.worst_error) g_stats.worst_error = err;
        g_stats.err_sum += err;
        ++g_stats.err_n;

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
            *px = tx; *py = ty; *pz = tz;
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

// ---------------------------------------------------------------------------
// TEST HOOK for the adventure-ghost probe (port/tests/smoke_player.cpp). Builds
// a REAL v3 snapshot naming `slot` at the given pose and runs it through the
// ACTUAL apply path, so a headless smoke can prove the ghost drive -- a wire
// message moving the REMOTE body and never the local one -- without a second
// instance on the loopback carrier. It uses the wire structs defined above, not
// a copy of them, and the real apply_snapshot, so the probe cannot drift from
// what the network path does. anim_id is taken from the target body so
// apply_pose sees no id change; the teleport flag makes the position land
// exactly for a clean assert. Inert unless called.
extern "C" void port_adventure_probe_apply_lvl(int slot, int x, int y, int z,
                                               short yaw, int level) {
    if (slot < 0 || slot >= kPortBodySlots) return;
    void *a = data_0209f394[slot];
    unsigned char buf[kSyncBufBytes];
    SyncMsgV1 *m = (SyncMsgV1 *)buf;
    m->magic = kSyncMagic;
    m->version = kSyncVersion;
    m->seq = 0;
    m->count = 1;
    m->pad[0] = m->pad[1] = m->pad[2] = 0;
    SyncPlayerV1 *e = (SyncPlayerV1 *)(buf + sizeof(SyncMsgV1));
    e->slot = (unsigned char)slot;
    e->flags = (unsigned char)(kFlagLive | kFlagTeleport);
    e->yaw = yaw;
    e->x = x;
    e->y = y;
    e->z = z;
    e->anim_id = a ? player::anim_id(a) : 0;
    e->state_id = 0;
    e->anim_frame = a ? player::anim_frame(a) : 0;
    e->vx = e->vy = e->vz = 0;
    /* v4: the injected peer's level. The M2 presence proof passes a level that
       is NOT this console's to prove the filter despawns it; the M1 proof (via
       port_adventure_probe_apply below) passes our own level so the ghost is
       same-level and visible, exactly as the M1 assert expects. */
    e->level_id = (unsigned short)level;
    apply_snapshot(buf, (int)(sizeof(SyncMsgV1) + sizeof(SyncPlayerV1)));
}

// The M1 face: inject at THIS console's own level, so the ghost is same-level
// and visible. Unchanged signature, so the M1 proof and its callers are
// untouched by the v4 level field.
extern "C" void port_adventure_probe_apply(int slot, int x, int y, int z,
                                           short yaw) {
    port_adventure_probe_apply_lvl(slot, x, y, z, yaw, (int)data_0209f2f8);
}

// ---------------------------------------------------------------------------
// ADVENTURE co-op (M2): the presence predicate. The single question every
// ghost loop asks -- render, hold-follow, name tag -- so "is this remote body a
// ghost I should draw right now" is answered in one place and cannot drift
// between the four sites. True when:
//   - adventure mode is on (VS and solo never ghost),
//   - the slot is a real remote slot (not out of range, not the local body),
//   - the peer is PRESENT: a snapshot arrived inside kPresenceTimeoutMs, so a
//     peer that quit or dropped its link despawns after the timeout, and
//   - the peer is in the SAME level: its last reported level id equals this
//     console's data_0209f2f8. A peer that warps away, or that we warp away
//     from, stops matching and despawns on the very next frame.
// This is the whole of the spawn/despawn mechanism: the Player bodies are
// seated once at level boot (vs_player_count), and which of them is DRAWN and
// DRIVEN as a ghost is this predicate, re-evaluated every frame.
// ---------------------------------------------------------------------------
namespace {
bool peer_visible(int slot) {
    if (!adventure_ghost_mode()) return false;
    if (slot < 0 || slot >= kPortBodySlots) return false;
    if (slot == (int)data_0209f250) return false;
    /* GHOSTWIRE1: on the hub the body's seat decides, off its own frames */
    if (g_hub) return !gw_cut() && gw_visible(gw_seat_for_body(slot));
    if (!g_peer_present[slot]) return false;
    if ((unsigned)(GetTickCount() - g_peer_seen_ms[slot]) > kPresenceTimeoutMs)
        return false;
    if (g_peer_level[slot] != (int)data_0209f2f8) return false;
    return true;
}
}  // namespace

// The extern "C" face the other host TUs gate on (walk_window's render loop,
// player_bridges' hold, nametag.h's tag). Same predicate, one authority.
extern "C" int port_adventure_peer_visible(int slot) {
    return peer_visible(slot) ? 1 : 0;
}

// GHOSTWIRE1: what the name tag (tests/nametag.h) shows over a hub ghost.
// The seat's SYNI name (sanitised to the tag font's ASCII, '?' for the rest),
// its body colour, and the star count its newest SYNG carries. -1 / null / 0
// when the body is not a hub ghost, so the tag falls back to the VS facts.
extern "C" int port_adventure_ghost_seat(int slot) {
    if (!g_hub) return -1;
    return gw_seat_for_body(slot);
}
extern "C" const char *port_adventure_ghost_name(int slot) {
    const int s = port_adventure_ghost_seat(slot);
    if (s < 0 || !g_gw[s].have_ident || !g_gw[s].name[0]) return 0;
    return g_gw[s].name;
}
extern "C" int port_adventure_ghost_stars(int slot) {
    const int s = port_adventure_ghost_seat(slot);
    if (s < 0 || g_gw[s].n == 0) return -1;
    return (int)g_gw[s].ring[g_gw[s].n - 1].f.stars;
}
// 0..256: how much of a hub ghost to draw, off the age of its newest frame
// (full to kGwFadeMs, then down to nothing at kGwDespawnMs). 256 for any
// body that is not a hub ghost, so VS, solo and the old path are untouched.
namespace {
// SM64DS_GHOST_DOORTAG_OFF=1: the control arm of the door-tag rows (the tag and
// body keep full strength through a door sequence, as in 97642ec0cd).
bool gw_door_tag_fade() {
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_GHOST_DOORTAG_OFF");
        v = (e && std::atoi(e) != 0) ? 0 : 1;
    }
    return v != 0;
}
enum : unsigned { kGwDoorGoneFrom = 10, kGwDoorGoneTo = 15 };
int gw_fade_out(int s) {
    const GwSeat &z = g_gw[s];
    const unsigned now = GetTickCount();
    const unsigned age = now - z.last_ms;
    int f;
    if (age <= (unsigned)kGwFadeMs) f = 256;
    else if (age >= (unsigned)kGwDespawnMs) return 0;
    else
        f = (int)(256u * ((unsigned)kGwDespawnMs - age) /
                  (unsigned)(kGwDespawnMs - kGwFadeMs));
    /* GHOSTDOOR1: leaving my level / area, and a long pause */
    if (z.leaving) f = f * (256 - z.leave_cut) / 256;
    /* GHOSTDOOR1: through the door. His door sequence goes on (the area, the
       level and the hidden state stay as they were) while his body is past the
       door leaf and behind the wall: the body and the name tag over it fade
       out together over frames kGwDoorGoneFrom..kGwDoorGoneTo of the door's
       own animation, instead of the tag floating on at full strength with
       nothing under it. The sample is the one the follower drew this frame. */
    if (gw_door_tag_fade()) {
        const GwSample &ls = g_gw_last[s];
        if (z.last_have && ls.door_kind && ls.door_frame != ghostdoor::kDoorFrameNone &&
            ls.door_frame >= kGwDoorGoneFrom &&
            ls.door_frame < ghostdoor::kGateBase) {   // 128+ is a star gate's slide, not a door frame
            const int d = ls.door_frame >= kGwDoorGoneTo ? 0 :
                (int)(kGwDoorGoneTo - ls.door_frame);
            f = f * d / (int)(kGwDoorGoneTo - kGwDoorGoneFrom);
        }
    }
    if (z.paused_now) {
        const unsigned pe = now - z.paused_ms;
        if (pe >= (unsigned)kGwPauseFadeMs) {
            const unsigned into = pe - (unsigned)kGwPauseFadeMs;
            if (into >= (unsigned)kGwPauseFadeLen) return 0;
            f = (int)((unsigned)f * ((unsigned)kGwPauseFadeLen - into) /
                      (unsigned)kGwPauseFadeLen);
        }
    }
    return f;
}
// GHOSTLOOK1: and UP from nothing over kGwAppearMs once it comes on show (a
// level entry, a player walking in from another level, a comeback after a
// despawn). A ghost is drawn solid now, and a solid body that pops into the
// world in one frame is the jolt the old see-through look hid. The follower
// stamps show_ms when a seat is shown after a pass that did not show it.
int gw_fade_in(int s) {
    const unsigned in = GetTickCount() - g_gw[s].show_ms;
    if (in >= (unsigned)kGwAppearMs) return 256;
    return (int)(256u * in / (unsigned)kGwAppearMs);
}
}  // namespace
extern "C" int port_adventure_ghost_fade(int slot) {
    const int s = port_adventure_ghost_seat(slot);
    if (s < 0 || g_gw[s].n == 0) return 256;
    const int o = gw_fade_out(s), i = gw_fade_in(s);
    return o < i ? o : i;
}

extern "C" unsigned port_adventure_ghost_rgb(int slot) {
    const int s = port_adventure_ghost_seat(slot);
    if (s < 0 || !g_gw[s].have_ident) return 0;
    return 0xff000000u | ((unsigned)g_gw[s].body[0] << 16) |
           ((unsigned)g_gw[s].body[1] << 8) | (unsigned)g_gw[s].body[2];
}

// ---------------------------------------------------------------------------
// PARTY (run mp1 lane PBODY1): membership is by SEAT and lives in hal/party.cpp.
// A party body is a hub ghost body that is shown solid and collides: the follower
// below hands it to party::follow() and the render pass asks port_party_member()
// (defined in hal/party.cpp) whether body `slot` is solid this frame. Nothing here
// is a membership flag any more: with SM64DS_PARTY unset every call below is a
// no-op and a ghost is exactly what it was.
// ---------------------------------------------------------------------------
namespace {
bool party_member(int body) { return party::body_solid(body); }
}  // namespace

// ---------------------------------------------------------------------------
// GHOSTWIRE1: THE HUB FOLLOWER, and the G4 fix (the walking pose).
//
// Every visible seat's body is put exactly where the sender was 100 ms ago
// (gw_sample: interpolated, reckoned or held), facing the same way, playing
// the sender's animation at the sender's cursor.
//
// THE POSE IS RE-RUN AFTER THE ANIMATION IS INSTALLED, and that is the whole
// of the walking-pose bug. A player body is posed ONCE a frame, inside
// Player::Behavior (func_ov002_020e4768: UpdateBones from the animation, the
// look angles, UpdateVertsUsingBones); the draw poses nothing
// (hal/player_bridges.cpp, "NO RE-POSE AT DRAW TIME"). The ghost's own
// Behavior runs first and its no-control state puts the standing entry
// animation (71) back every frame, so by the time the follower installed the
// wire's walk (72) the frame's pose was already the standing one, and the
// draw showed that: GHOSTSCOUT T7, anim 71 with the cursor pinned at 1 while
// the source walked on 72. The 09-02 fix worked while the draw still re-posed
// the body; HEAD3 took that re-pose out for good reasons. So the follower now
// runs the ROM's own pose routine once more on the ghost, after SetAnim, and
// the draw reads the wire's animation. SM64DS_GHOST_NO_REPOSE=1 leaves it out
// (the before arm, for the proof).
// ---------------------------------------------------------------------------
namespace {
// Where a ghost body waits between draws: straight above the local player, far
// out of reach. dActor_c::ClosestPlayer walks every seated body
// (src/_ZN8dActor_c13ClosestPlayerEv.cpp), so a ghost standing where its
// player stands would be chased by the watcher's enemies; parked here it is
// never the nearest, and the one FarthestPlayer caller
// (func_ov002_020e88a8) still faces the local player's heading because the
// park shares its x and z. The body's own Behavior is skipped (its pauseFlags
// bit 2, hal/player_bridges.cpp), so the height runs no pit or fall logic.
// A parked body is also VANISHED (mIsVanish, +0x6fb): with the local player
// vanished (Luigi's power flower, Player::InitVanishLuigi),
// dActor_c::ClosestNonVanishPlayer skips the local body, and an unvanished
// parked ghost would be the only one left, where solo answers null. Boo,
// Pokey, Fly Guy, Scuttlebug, Ukiki and friends then turned to face the
// invisible Luigi. The follower clears it again on a ghost it shows.
int g_gw_nopark = -1;
unsigned g_gw_follow_n = 1;   // GHOSTLOOK1: follower passes, for show_n

/* GHOSTCHAR1: THE GHOST SMOOTHNESS EXIT SUMMARY, for field tests. One header
   line and one line per ghost seen (the four with the most frames; the rest
   are counted in the header), printed once when the game exits normally:
   how many follower frames each ghost was posed by interpolation between two
   received frames (smooth), by dead reckoning past the newest frame (a guess,
   fine for 250 ms), or held (no newer data to move to: the visible stutter),
   and the longest run of held frames back to back. */
static void gw_exit_pct(char *out, size_t cap, unsigned long long part,
                        unsigned long long whole) {
    const unsigned long long t = whole ? (part * 1000ull + whole / 2) / whole : 0;
    std::snprintf(out, cap, "%llu.%llu%%", t / 10, t % 10);
}

static void gw_exit_summary() {
    if (!g_hub) return;
    unsigned long long tot = 0, t_int = 0, t_rec = 0, t_hold = 0, t_sw = 0;
    int order[kHubMaxSeats], n = 0;
    for (int s = 0; s < kHubMaxSeats; ++s) {
        if (!g_gw[s].sm_n) continue;
        tot += g_gw[s].sm_n;
        t_int += g_gw[s].sm_interp;
        t_rec += g_gw[s].sm_reckon;
        t_hold += g_gw[s].sm_hold_old + g_gw[s].sm_hold_tele +
                  g_gw[s].sm_hold_still;
        t_sw += g_gw[s].sm_swaps;
        int k = n++;
        while (k > 0 && g_gw[order[k - 1]].sm_n < g_gw[s].sm_n) {
            order[k] = order[k - 1];
            --k;
        }
        order[k] = s;
    }
    char a[16], b[16], c[16];
    gw_exit_pct(a, sizeof a, t_int, tot);
    gw_exit_pct(b, sizeof b, t_rec, tot);
    gw_exit_pct(c, sizeof c, t_hold, tot);
    std::fprintf(stderr,
                 "[ghost:exit] smoothness: %d ghost%s posed over %llu frames "
                 "(follower passes %u): interpolated %s, reckoned %s, held %s;"
                 " %llu char swaps; showing the %d busiest\n",
                 n, n == 1 ? "" : "s", tot, g_gw_follow_n, a, b, c,
                 t_sw, n < 4 ? n : 4);
    for (int k = 0; k < n && k < 4; ++k) {
        const GwSeat &z = g_gw[order[k]];
        char pi[16], pr[16], ph[16];
        gw_exit_pct(pi, sizeof pi, z.sm_interp, z.sm_n);
        gw_exit_pct(pr, sizeof pr, z.sm_reckon, z.sm_n);
        gw_exit_pct(ph, sizeof ph,
                    z.sm_hold_old + z.sm_hold_tele + z.sm_hold_still, z.sm_n);
        std::fprintf(stderr,
                     "[ghost:exit] seat %d \"%s\": %llu frames, interp %llu "
                     "(%s), reckon %llu (%s), hold %llu (%s: oldest %llu, "
                     "teleport %llu, paused %llu), longest hold run %u, char "
                     "swaps %llu, clock %d Hz, last delay %.0f ms\n",
                     order[k], z.have_ident ? z.name : "", z.sm_n, z.sm_interp,
                     pi, z.sm_reckon, pr,
                     z.sm_hold_old + z.sm_hold_tele + z.sm_hold_still, ph,
                     z.sm_hold_old, z.sm_hold_tele, z.sm_hold_still,
                     z.sm_streak_max, z.sm_swaps, z.hz, z.buf_ms);
    }
    {
        const ghostdoor::Stats &d = ghostdoor::stats();
        std::fprintf(stderr, "[ghost:exit] doors: %llu sender door starts "
                     "flagged as teleports; watcher door frames applied %llu, "
                     "given back %llu, no door near %llu, local door left "
                     "alone %llu\n", g_gws.door_snaps, d.applied, d.released,
                     d.no_door, d.yielded);
        std::fprintf(stderr, "[ghost:exit] doors put back: %llu given back to "
                     "their own state, %llu by the idle sweep\n", d.restored, d.swept);
        std::fprintf(stderr, "[ghost:exit] look: hidden by his own state %llu "
                     "frames, faded out in place (door / painting / room) "
                     "%llu, held back for the wrong character %llu, hub "
                     "drains during my star select %llu\n", g_gws.hid_frames,
                     g_gws.linger_frames, g_gws.char_holds, g_gws.drains);
    }
    std::fflush(stderr);
}
                              // (from 1: a seat's show_n starts at 0)
int g_drawpos_have[kPortBodySlots];
int g_gw_pal[kPortBodySlots];   // GHOSTLOOK1: the row each body draws with
const void *g_gw_pal_body[kPortBodySlots];   // ...the body it was made for
int g_drawpos[kPortBodySlots][3];

void gw_park_body(void *a) {
    if (g_gw_nopark < 0) g_gw_nopark = std::getenv("SM64DS_GHOST_NO_PARK") ? 1 : 0;
    if (g_gw_nopark || !a) return;
    const int me = (int)data_0209f250;
    void *mb = (me >= 0 && me < kPortBodySlots) ? data_0209f394[me] : 0;
    if (!mb || mb == a) return;
    *player::pos_x(a) = *player::pos_x(mb);
    *player::pos_y(a) = *player::pos_y(mb) + kGwParkUp;
    *player::pos_z(a) = *player::pos_z(mb);
    *(int *)((char *)a + 0x98) = 0;
    *(int *)((char *)a + 0xa8) = 0;
    *((unsigned char *)a + 0x6fb) = 1;
    ++g_gws.parked;
}

/* POSE9: per ghost body, the Yoshi tongue (head animation) and the hurt blink. */
ghost_tongue::State g_gtongue[kPortBodySlots];
ghost_tongue::Blink g_gblink[kPortBodySlots];

/* INTERACT9: THE ATTACH RULE. While seat V is in somebody's mouth or hands (party_hits.cpp keeps the
   book from the victim's own broadcast), every console that draws V places V's body at the holder's
   mouth / hand instead of where V's own frames say: this console's idea of the holder (my own body when
   the holder is me, else the holder's ghost at its draw time), offset along the holder's yaw. */
bool gw_attach_target(int holder_seat, unsigned tnow, int *x, int *y, int *z, int *yaw) {
    const int me = (int)data_0209f250;
    /* ROMSTATE9: `me` is my player SLOT (0 on every console in a party / coop room), holder_seat a HUB SEAT: comparing the two
       made seat 0 (the host) look like "me" on every other console, so the carried / eaten ghost was drawn at the bystander's own
       body instead of at the holder. Seats only. */
    if (holder_seat == comms_hub_my_seat()) {
        char *mb = (char *)data_0209f394[me];
        if (!mb) return false;
        *x = *player::pos_x(mb); *y = *player::pos_y(mb); *z = *player::pos_z(mb);
        *yaw = (int)*player::facing(mb);
        return true;
    }
    if (holder_seat < 0 || holder_seat >= kHubMaxSeats || !gw_visible(holder_seat)) return false;
    GwSample h;
    if (!gw_sample(holder_seat, tnow, &h)) return false;
    *x = h.x; *y = h.y; *z = h.z; *yaw = (int)h.yaw;
    return true;
}

void gw_follow() {
    HITCH_SCOPE(HK_GHOSTFOLLOW);
    static int no_repose = -1, no_swap = -1, no_tongue = -1, no_blink = -1;
    if (no_tongue < 0) { const char *e = std::getenv("SM64DS_GHOST_TONGUE"); no_tongue = (e && e[0] == '0' && !e[1]) ? 1 : 0; }
    if (no_blink < 0) { const char *e = std::getenv("SM64DS_GHOST_BLINK"); no_blink = (e && e[0] == '0' && !e[1]) ? 1 : 0; }
    if (no_repose < 0) no_repose = std::getenv("SM64DS_GHOST_NO_REPOSE") ? 1 : 0;
    if (no_swap < 0) no_swap = std::getenv("SM64DS_GHOST_NO_CHARSWAP") ? 1 : 0;
    /* GHOSTSMOOTH1: THE RING AT THIS INSTANT. sync_tick drained the hub at the
       top of the tick; a frame that arrived since is in the hub's queue and the
       draw time (which counts from the arrival stamp) would sit ahead of every
       frame the ring holds. Drain again first; an empty queue is a no-op. */
    if (!gw_legacy()) gw_recv();
    const unsigned now = GetTickCount();
    const unsigned tnow = gw_legacy() ? now : comms_hub_now_ms();
    const int me = (int)data_0209f250;
    const int nb = (int)data_0209f21c < kPortBodySlots ? (int)data_0209f21c
                                                         : kPortBodySlots;
    const bool in_cut = data_0209fc48 != 0;
    bool shown_any = false;
    ++g_gw_follow_n;
    static bool exit_hooked = false;
    if (!exit_hooked) {
        exit_hooked = true;
        std::atexit(gw_exit_summary);
    }
    for (int i = 0; i < nb; ++i) {
        if (i == me) continue;
        void *a = data_0209f394[i];
        if (!a) continue;
        g_drawpos_have[i] = 0;
        g_gw_pal[i] = -1;
        party::clear(i);
        const int s = gw_seat_for_body(i);
        GwSample sm;
        bool got = s >= 0 && gw_visible(s) && !gw_cut();
        /* PBODY1: a party seat in a menu / star select / cutscene (SYNL), or whose
           newest ghost frame says his game is paused (the pause menu, a star select
           and a loading hold send those at 2 Hz), has no body at all here; one
           in another level or area is drawn the way a ghost is (it fades out
           as it leaves) but is never solid (party::follow below) */
        const bool pseat = got && party::seat_is_party(s);
        if (pseat && (party::gate(s) == party::kGateHide || g_gw[s].paused_now)) got = false;
        if (got) {
            if (g_gw[s].leaving) {         // GHOSTDOOR1: fading out in place
                sm = g_gw_last[s];
                ++g_gws.linger_frames;
            } else {
                got = gw_sample(s, tnow, &sm);
            }
        }
        if (!got) {
            if (s >= 0) ghostdoor::watcher_release(s);
            gw_park_body(a);               // undriven: parked, not drawn
            continue;
        }
        if (sm.y < kDeathPlaneY) {         // falling out of the level: hidden
            ++g_gws.below;
            ghostdoor::watcher_release(s);
            gw_park_body(a);
            continue;
        }
        if (sm.hidden && !g_gw[s].leaving) {
            /* GHOSTDOOR1: his own state hides his body (a cannon, a pipe, a
               warp): it is hidden here too, not left hovering at the mouth */
            ++g_gws.hid_frames;
            ghostdoor::watcher_release(s);
            gw_park_body(a);
            continue;
        }
        GwSeat &z = g_gw[s];
        if (!z.leaving) {
            g_gw_last[s] = sm;
            z.last_have = true;
            z.last_lvl = (int)data_0209f2f8;
            z.last_area = (int)data_02092120;
        }
        if (z.body_ptr != a) { z.body_ptr = a; z.driven = 0; }
        /* GHOSTLOOK1: on show from this pass; not shown on the last one
           means it is coming on show now, and fades in (gw_fade_in) */
        if (z.show_n + 1 != g_gw_follow_n) z.show_ms = now;
        z.show_n = g_gw_follow_n;
        if (sm.teleport) ++g_gws.teleports;
        int akind = 0;       /* INTERACT9 attach (see gw_attach_target) */
        const int holder = port::party_hits::attached_holder(s, &akind);
        {
            if (holder >= 0) {
                int hx, hy, hz, hyaw;
                if (gw_attach_target(holder, tnow, &hx, &hy, &hz, &hyaw)) {
                    int ahead = 0, up = 0;
                    port::party_hits::attach_offsets(akind, &ahead, &up);
                    const double ang = (double)((unsigned)hyaw & 0xFFFFu) * 3.14159265358979323846 / 32768.0;
                    sm.x = hx + (int)std::lround(std::sin(ang) * ahead) * 0x1000;
                    sm.z = hz + (int)std::lround(std::cos(ang) * ahead) * 0x1000;
                    sm.y = hy + up * 0x1000;
                    sm.yaw = (short)(hyaw + 0x8000);       /* faces the holder */
                    sm.teleport = false;
                    static unsigned alog;
                    if (alog++ % 30 == 0)
                        std::fprintf(stderr, "[attach] f=%u seat=%d kind=%d held by seat %d at (%d,%d,%d) holder (%d,%d,%d)\n",
                                     g_gw_frame, s, akind, holder, sm.x >> 12, sm.y >> 12, sm.z >> 12, hx >> 12, hy >> 12, hz >> 12);
                }
            }
        }
        if (gw_err_log())
            std::fprintf(stderr, "[gwdraw] t=%u f=%u seat=%d mode=%c pos=%d,%d,%d "
                         "yaw=%d anim=%u buf=%d dk=%u df=%u tp=%d lv=%d "
                         "ch=%u\n", comms_hub_now_ms(),
                         g_gw_frame, s, sm.mode, sm.x, sm.y, sm.z, (int)sm.yaw,
                         (unsigned)sm.anim, sm.buf_ms, (unsigned)sm.door_kind,
                         (unsigned)sm.door_frame, sm.teleport ? 1 : 0,
                         g_gw[s].leaving ? 1 : 0, (unsigned)sm.character);
        *player::pos_x(a) = sm.x;
        *player::pos_y(a) = sm.y;
        *player::pos_z(a) = sm.z;
        *player::facing(a) = sm.yaw;
        g_drawpos[i][0] = sm.x; g_drawpos[i][1] = sm.y; g_drawpos[i][2] = sm.z;
        g_drawpos_have[i] = 1;
        /* GHOSTDOOR1 (A2): the door he is using opens on this screen too, by
           its model animation alone (ghost_door.cpp says why only that) */
        if (!gw_legacy())
            ghostdoor::watcher_apply(s, sm.door_kind, sm.door_frame,
                                     g_drawpos[i]);
        *(int *)((char *)a + 0x98) = 0;
        *(int *)((char *)a + 0xa8) = 0;
        /* THE CHARACTER, through the game's own in-place swap (the door
           path). GHOSTDOOR1: AT ONCE, on the first pass that wants it, and
           the body is not drawn until it shows the right one (for at most
           1.5 s, then it is drawn as it is). It used to wait for the body to
           have been driven 30 frames and swapped once every 2 s, so after
           every level entry each friend showed as MY character for about a
           second. A swap that did not take is tried again after 250 ms. */
        if (!no_swap && gw_char(a) != (unsigned)sm.character) {
            if (!z.char_wait_ms) z.char_wait_ms = now ? now : 1u;
            if ((int)(z.char_try_ms - z.char_wait_ms) < 0 ||
                (unsigned)(now - z.char_try_ms) >= 250u) {
                z.char_try_ms = now;
                port_player_set_character(a, sm.character);
                ghost_tongue::forget(g_gtongue[i]);     // POSE9: the model was rebuilt
                ++g_gws.charswaps;
                ++z.sm_swaps;
                std::fprintf(stderr, "[ghost] seat %d body %d character -> %u\n",
                             s, i, (unsigned)sm.character);
            }
            if (gw_char(a) != (unsigned)sm.character &&
                (unsigned)(now - z.char_wait_ms) < 1500u) {
                ++g_gws.char_holds;
                ghostdoor::watcher_release(s);
                gw_park_body(a);
                continue;
            }
        } else {
            z.char_wait_ms = 0;
        }
        /* THE CURSOR. Player::SetAnim only lands its start frame when the
           animation CHANGES: that arm clears the ModelAnim's file word (+0x60,
           src/actors/Player.cpp, "*(int *)(modelPtr + 0x60) = 0") so
           ModelAnim::SetAnim takes its slow path; with the same id it keeps
           the file and ignores the start frame, so the ghost's own Behavior
           ran the cycle at its own pace -- measured: a paused source's frozen
           cursor 11 on the wire while its ghost walked on in place. When the
           id is the same and the frame differs, make the ROM's own reset
           first, then SetAnim lands the wire's frame. */
        int want_f = sm.cursor >> 12;
        if (want_f < 0) want_f = 0;
        /* LAB (POSE9 pictures): SM64DS_GHOST_TONGUE_FREEZE=<frame> holds the first ghost
           that has driven a tongue on the pull-out pose at that body frame, so a picture
           taken whenever shows the tongue out. Never set outside a proof. */
        {
            static int frz = -2, frz_body = -1;
            if (frz == -2) { const char *e = std::getenv("SM64DS_GHOST_TONGUE_FREEZE"); frz = e && *e ? std::atoi(e) : -1; }
            if (frz >= 0) {
                if (frz_body < 0 && g_gtongue[i].frames > 0) frz_body = i;
                if (frz_body == i) { sm.anim = ghost_tongue::retract_anim(); want_f = frz; sm.cursor = frz << 12; }
            }
        }
        {
            int *cur = player::anim_frame_ptr(a);
            if (player::anim_id(a) == sm.anim && cur && (*cur >> 12) != want_f) {
                if (void *m = player::body_model_anim(a))
                    *(int *)((char *)m + 0x60) = 0;
                ++g_gws.recursor;
            }
        }
        _ZN6Player7SetAnimEji5Fix12IiEj(a, sm.anim, 0, 0x1000,
                                        (unsigned)(want_f & 0xffff));
        /* THE FRAME, BOUNDED BY THE ANIMATION'S OWN LENGTH. The id is checked
           on receipt; the frame can only be checked now that SetAnim has the
           file: Animation (ModelAnim +0x50) holds numFrames << 12 in +0x04
           (src/_ZN9Animation12SetAnimationEti5Fix12IiEt.cpp) and the cursor in
           +0x08. A frame past the end would have UpdateBones read past the
           animation's channel tables. The follower installs every animation
           looping (flags 0), so an overrun wraps, the way Animation::Advance
           wraps a looping one. No length: nothing to pose. The ghost's own
           Behavior is skipped, so no animation event reads this cursor. */
        int frames = 0;
        if (char *m = (char *)player::body_model_anim(a)) {
            frames = (*(int *)(m + 0x54) & ~(int)0xc0000000) >> 12;
            if (frames > 0) {
                const int ok_f = want_f % frames;
                if (ok_f != want_f) ++g_gws.clamped;
                *(int *)(m + 0x58) = ok_f << 12;
            }
        }
        if (frames <= 0) { gw_park_body(a); continue; }
        /* POSE9 THE TONGUE. Yoshi's tongue is the head model's own animation
           (ghost_tongue.h): when the wire's body anim is a tongue anim make the
           ROM's call once, then put the head on the body's frame. */
        if (!no_tongue) {
            bool started = false;
            const int targ = ghost_tongue::select(g_gtongue[i], (char *)a, sm.anim,
                                                  gw_char(a) == 3u, &started);
            if (started)
                std::fprintf(stderr, "[ghost-tongue] f=%u seat=%d body=%d tongue %d (anim %u)\n",
                             g_gw_frame, s, i, targ, (unsigned)sm.anim);
            if (targ >= 0) ghost_tongue::follow(g_gtongue[i], (char *)a, want_f, nullptr);
        }
        /* PLACE, THEN POSE, in Player::Behavior's own order: the body matrix
           and the shadow node from the position (func_ov002_020e444c), then
           the bones (func_ov002_020e4768), which read that matrix. A ghost
           that is fading out gets neither: it keeps its last matrix and casts
           no shadow while it goes. The floor height the shadow measures from
           (+0x644) is the ghost's own feet whenever its player is grounded. */
        /* FULLY OPAQUE to the ROM's own gates: mOpacity (+0x6f5, 0..0x1f)
           starts at 0 at a level entrance and only the body's own entry state
           raises it (src/actors/Player.cpp), which a ghost never runs; at 0
           the draw gate (port_player_render_hidden) and the shadow skip it.
           The see-through look is the render's ghost alpha, not this. */
        *((unsigned char *)a + 0x6f5) = 0x1f;
        if (pseat) party::prepare(a);       /* PBODY1: nothing attached to it, so the placement below reads its own position */
        /* ...and not culled. dActor_c::BeforeBehavior writes the camera cull
           bits 0x38 of Actor mFlags (+0xb0) every frame an actor ticks
           (src/_ZN8dActor_c14BeforeBehaviorEv.cpp); a ghost's tick is
           skipped, so whatever they held at its last tick would stand for
           ever, and bit 0x10 is one the draw gate honours. The follower
           decides a ghost is on show, so it clears them. */
        *(unsigned *)((char *)a + 0xb0) &= ~0x38u;
        /* the park's vanish (+0x6fb) is off for every drawn ghost, fading or not:
           the render latch copies it and Luigi's texture sequence turns it
           into the vanish-cap stipple */
        *((unsigned char *)a + 0x6fb) = 0;
        /* fading OUT only: a ghost fading in is placed like a shown one */
        if (gw_fade_out(s) >= 256) {
            if (sm.grounded) *(int *)((char *)a + 0x644) = sm.y;
            *((unsigned char *)a + 0x717) = 0;
            func_ov002_020e444c((char *)a);
        }
        if (!no_repose) {
            func_ov002_020e4768((char *)a);
            ++g_gws.reposes;
        }
        /* GHOSTLOOK1: THE RENDER LATCHES, the last block of the same routine
           (src/func_ov002_020e4bb8.c, after the place and the pose this
           follower already runs). Player::Render does not read the live
           fields; it reads what the end of the body's Behavior copied for it:
           +0x6db the character whose head it picks (_ZN6Player19func_ov002_020becf4Eii with
           arg2 = 1, and the eye-blink sequence), +0x6fe / +0x6fa the balloon
           and no-head latches, +0x6fc the vanish latch Luigi's texture
           sequence reads, +0x700 the wings, +0x701 HasNoCap. A ghost's own
           Behavior is skipped from the frame its body is seated, so nothing
           ever wrote them: every byte stayed 0, and 0 in +0x6db is MARIO. A
           Yoshi ghost then asked for head index 0 (Mario's cap head, drawn on
           Yoshi's neck bone, nowhere to be seen) instead of 3, its own: the
           headless Yoshi of the field report. A Mario ghost looked right only
           because Mario is 0. The ROM's six statements, as it writes them. */
        {
            char *c = (char *)a;
            *(unsigned char *)(c + 0x6db) = (unsigned char)*(int *)(c + 8);
            *(unsigned char *)(c + 0x6fe) = *(unsigned char *)(c + 0x6fd);
            *(unsigned char *)(c + 0x6fa) = *(unsigned char *)(c + 0x6f9);
            *(unsigned char *)(c + 0x6fc) = *(unsigned char *)(c + 0x6fb);
            *(unsigned char *)(c + 0x700) = *(unsigned char *)(c + 0x6ff);
            *(unsigned char *)(c + 0x701) =
                (unsigned char)_ZN6Player8HasNoCapEv(c);
        }
        g_gw_pal[i] = gw_colour_row(z, a);
        g_gw_pal_body[i] = a;
        ++z.driven;
        shown_any = true;
        /* PBODY1: a party friend in my level and area, fully shown, is a solid body */
        /* INTERACT9: a body in somebody's mouth or hands is not solid (its cylinder would shove the holder) */
        if (pseat && !z.leaving && !sm.door_kind && gw_fade_out(s) >= 256 &&
            party::gate(s) == party::kGateOk && holder < 0)
            party::follow(a, i, s);
        if (pseat) {
            /* PLAYINT1 hook 4: the squash after a stomp I landed (+0x84 is the Y scale).
               Written only while a squash runs and put back once when it ends. */
            static bool squashed[kPortBodySlots];
            const int sy = port::party_hits::squash_scale_for_seat(s);
            if (sy != 0x1000) {
                *(int *)((char *)a + 0x84) = sy;
                squashed[i] = true;
            } else if (squashed[i]) {
                *(int *)((char *)a + 0x84) = 0x1000;
                squashed[i] = false;
            }
        }
        /* POSE9 THE BLINK (item 20): after party::follow, which clears the timer
           for a solid party body. The ROM's Render hides a body on odd timer
           frames; nothing counts a ghost's timer down, so this is the count. */
        if (!no_blink) {
            bool started = false;
            unsigned v = ghost_tongue::blink_step(g_gblink[i], sm.anim, &started);
            /* LAB (POSE9 pictures): SM64DS_GHOST_BLINK_PIN=1 / 2 holds a ghost that has
               started a blink on the hidden (1) or the drawn (2) frame for good. */
            {
                static int pin = -2;
                if (pin == -2) { const char *e = std::getenv("SM64DS_GHOST_BLINK_PIN"); pin = e && *e ? std::atoi(e) : 0; }
                static bool pinned[kPortBodySlots];
                if (pin > 0 && (started || pinned[i])) { pinned[i] = true; v = (unsigned)(pin & 3); g_gblink[i].left = 1; }
            }
            *(unsigned short *)((char *)a + player::kInvincibleTimer) = (unsigned short)v;
            if (started)
                std::fprintf(stderr, "[ghost-blink] f=%u seat=%d body=%d hurt anim %u: 36-frame blink\n",
                             g_gw_frame, s, i, (unsigned)sm.anim);
            static int blog = -1;       // LAB: SM64DS_GHOST_BLINK_LOG=1, a line per blink frame with the ROM's own draw gate
            if (blog < 0) blog = std::getenv("SM64DS_GHOST_BLINK_LOG") ? 1 : 0;
            if (v && blog)
                std::fprintf(stderr, "[ghost-blink] f=%u seat=%d timer=%u render_hidden=%d\n",
                             g_gw_frame, s, v, port_player_render_hidden(a));
        }
        if (gw_err_log())
            std::fprintf(stderr, "[gwshown] t=%u f=%u seat=%d body=%d bc=%u "
                         "want=%u fade=%d\n", comms_hub_now_ms(), g_gw_frame,
                         s, i, gw_char(a), (unsigned)sm.character,
                         port_adventure_ghost_fade(i));
        if (gw_look_probe() > 0 &&
            (g_gw_frame % (unsigned)gw_look_probe()) == 0) {
            char *c = (char *)a;
            const int pal = *(const int *)(c + 0x61c);
            const int row0 =
                pal - ((int)*(const unsigned char *)(c + 0x6d8) << 1);
            const unsigned short *r0 = gw_pltt_row(row0);
            const unsigned short *rp = gw_pltt_row(pal);
            const unsigned char latch = *(const unsigned char *)(c + 0x6db);
            std::fprintf(stderr, "[gwlook] t=%u f=%u GHOST seat=%d body=%d "
                         "ch=%u latch=%u hid=%d l6fa=%u l6fe=%u l701=%u "
                         "pal=%d row0=%04x,%04x rowpal=%04x,%04x "
                         "pos=(%d,%d,%d) fade=%d ident=%d\n",
                         now, g_gw_frame, s, i, gw_char(a), (unsigned)latch,
                         (int)_ZN6Player19func_ov002_020becf4Eii(c, latch, 1),
                         (unsigned)*(const unsigned char *)(c + 0x6fa),
                         (unsigned)*(const unsigned char *)(c + 0x6fe),
                         (unsigned)*(const unsigned char *)(c + 0x701),
                         pal, r0 ? r0[0] : 0u, r0 ? r0[1] : 0u,
                         rp ? rp[0] : 0u, rp ? rp[1] : 0u,
                         sm.x >> 12, sm.y >> 12, sm.z >> 12,
                         port_adventure_ghost_fade(i), z.have_ident ? 1 : 0);
        }
        if (gw_pose_probe() > 0 &&
            (g_gw_frame % (unsigned)gw_pose_probe()) == 0)
            std::fprintf(stderr, "[gwpose] t=%u f=%u GHOST seat=%d body=%d "
                         "anim=%u cur=%d hash=%08x wire_anim=%u wire_cur=%d "
                         "y=%d\n",
                         now, g_gw_frame, s, i,
                         (unsigned)player::anim_id(a),
                         player::anim_frame(a) >> 12, gw_pose_hash(a),
                         (unsigned)sm.anim, sm.cursor >> 12, sm.y >> 12);
    }
    ghostdoor::watcher_end_pass();      // DOORS9C: a seat that was not driven lately gives its door back
    if (shown_any) ++g_gws.shown_frames;
    if (in_cut) {
        ++g_gws.cut_frames;
        if (shown_any) ++g_gws.cut_drawn;
    }
}
}  // namespace


// SM64DS_GHOST_TARGET_PROBE=1: what the enemies saw. Runs at sync_tick, after
// this frame's actor phase and before the follower, so every body is where it
// was while the actors ran. For each non-player actor it makes
// dActor_c::ClosestPlayer's own choice (every seated body, strict <, index
// order: src/_ZN8dActor_c13ClosestPlayerEv.cpp) and counts target_ghost when
// that choice is not the local player; near_ghost counts actors standing
// within 600 units of where a ghost was DRAWN last frame -- the ones that could
// have been tempted. Reads only.
namespace {
void gw_target_probe() {
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_GHOST_TARGET_PROBE") ? 1 : 0;
    if (!on) return;
    const int me = (int)data_0209f250;
    const int nb = (int)data_0209f21c < kPortBodySlots ? (int)data_0209f21c
                                                         : kPortBodySlots;
    for (const char *a = (const char *)_ZN8dActor_c4NextEPKS_(0); a;
         a = (const char *)_ZN8dActor_c4NextEPKS_(a)) {
        bool is_player = false;
        for (int i = 0; i < nb; ++i)
            if (data_0209f394[i] == a) is_player = true;
        if (is_player) continue;
        const long long ax = *(const int *)(a + 0x5c),
                        ay = *(const int *)(a + 0x60),
                        az = *(const int *)(a + 0x64);
        int best = -1;
        long long bd = 0;
        for (int i = 0; i < nb; ++i) {
            const char *p = (const char *)data_0209f394[i];
            if (!p) continue;
            const long long dx = (*(const int *)(p + 0x5c) - ax) >> 4,
                            dy = (*(const int *)(p + 0x60) - ay) >> 4,
                            dz = (*(const int *)(p + 0x64) - az) >> 4;
            const long long d = dx * dx + dy * dy + dz * dz;
            if (best < 0 || d < bd) { best = i; bd = d; }
        }
        if (best >= 0 && best != me) {
            ++g_gws.target_ghost;
            static int said = 0;
            if (said < 8) {
                ++said;
                std::fprintf(stderr, "[gwtarget] frame %u: actor id %u picked "
                             "body %d (a ghost)\n", g_gw_frame,
                             (unsigned)*(const unsigned short *)(a + 0x0c), best);
            }
        }
        for (int i = 0; i < nb; ++i) {
            if (i == me || !g_drawpos_have[i]) continue;
            const long long dx = (g_drawpos[i][0] - ax) >> 4,
                            dy = (g_drawpos[i][1] - ay) >> 4,
                            dz = (g_drawpos[i][2] - az) >> 4;
            const long long lim = (600LL << 12) >> 4;
            if (dx * dx + dy * dy + dz * dz < lim * lim) {
                ++g_gws.near_ghost;
                break;
            }
        }
    }
}
}  // namespace

// GHOSTDOOR1 (queue drain): THE HUB IS DRAINED DURING MY OWN STAR SELECT.
// A painting runs the star select as an interlude inside one level frame
// (hal/level_change.cpp), where sync_tick never runs: the hub's 64-slot ghost
// FIFO filled in about a second and the rest of the select's 15 to 20 s of
// frames were dropped at the I/O thread (queue=284 on a field log), so every
// friend jumped to a stale spot when the course began. Called once per
// interlude frame (tests/walk_window.cpp port_interlude_frame), this keeps
// each seat's ring on its newest frames instead; nothing is drawn or sent.
extern "C" void port_adventure_ghost_interlude() {
    if (!g_hub || gw_legacy()) return;
    gw_recv();
    ++g_gws.drains;
}

// hal/player_bridges.cpp's two faces: is this body a ghost body on the hub,
// and park it (after its draw).
extern "C" int port_adventure_hub_body(int slot) {
    /* the hub itself, not g_hub: sync_decide runs at the first pad publish,
       and a level can seat and tick its bodies before that */
    return (adventure_ghost_mode() && comms_hub_active() &&
            slot >= 0 && slot < kPortBodySlots &&
            slot != (int)data_0209f250 && slot < (int)data_0209f21c) ? 1 : 0;
}
extern "C" void port_adventure_ghost_park(void *body) { gw_park_body(body); }

// THE HUB'S SPARE BODIES ON THE TITLE PATH, which is the path every player
// takes (the launcher's adventure plan boots the title; SM64DS_LEVEL is the
// test harness's path, and every hub proof before this used it).
//
// The title's file pick runs the ROM's own StartFile, and StartFile calls
// SetNumPlayers(1) (src/StartFile.c): data_0208a0e0 = 1 and, through
// func_020308d0, the live bytes data_0209fc5c become {1,0,0,0}. The level
// boot that follows still spawns vs_player_count() bodies (4 on the hub), and
// LoadEntranceObjects stores a spawned body in the seat table only when its
// live byte is set (src/_Z19LoadEntranceObjectsRN11LVL_Overlay11ObjSubTableEij
// .cpp: data_0209f394[i] = data_0209fc5c[i] ? a : 0). So three Player bodies
// came up that no seat names. Everything that keeps a hub body out of the game
// (the Behavior skip, the park, the follower, the draw gate) finds its bodies
// through the seat table, so those three ran their own Player::Behavior with
// nobody holding them: measured alone in a room (no peer at all), they walked
// off, climbed, fell and drowned around the player, made their sounds, and in
// the new-file opening one of them died and restarted the level ("level 1 ->
// 1, entrance 13, reason 2", the death wipe, where solo ends the opening with
// entrance 0, reason 0). And with the count at 1 a later level load seated one
// body, so no peer's ghost had a body to be drawn on.
//
// The repair is the level path's own state, made the level path's own way:
//   - the count and the live bytes through the ROM's setter, SetNumPlayers
//     (what hal/level_boot.cpp does before every level-path spawn), so every
//     later level load seats all four bodies itself;
//   - a body this level's spawn already made and left unseated goes into its
//     seat. It is found exactly: LoadEntranceObjects spawns slot i right after
//     slot i-1, so its unique id is the local body's plus i, and its
//     mPlayerNo (+0x6d8) is i. The new-file opening's cast (Mario, Luigi and
//     Wario as Player actors, positioned by the cutscene script) are spawned
//     much later and keep their own ids, so they are never taken.
// Runs from the hold (hal/player_bridges.cpp), before the walk, so a repaired
// body is skipped and parked before its first Behavior tick. Hub only.
static void port_adventure_hub_opening_tick();   // the opening's hand-over, below
extern "C" void port_adventure_hub_seat() {
    if (!(adventure_ghost_mode() && comms_hub_active())) return;
    port_adventure_hub_opening_tick();
    const int count = (int)data_0209f21c;
    if (count <= 1 || count > kPortBodySlots) return;
    bool short_seat = (int)data_0208a0e0 != count;
    for (int i = 0; i < count; ++i)
        if (!data_0209fc5c[i]) short_seat = true;
    if (short_seat) {
        std::fprintf(stderr, "[ghost] hub seat: the ROM's count was %d with "
                     "live bytes %d%d%d%d for %d seated bodies (the title's "
                     "StartFile); SetNumPlayers(%d)\n", (int)data_0208a0e0,
                     (int)data_0209fc5c[0], (int)data_0209fc5c[1],
                     (int)data_0209fc5c[2], (int)data_0209fc5c[3], count, count);
        SetNumPlayers(count);
        ++g_gws.seat_fix;
    }
    const int me = (int)data_0209f250;
    const char *mb = (me >= 0 && me < count) ? (const char *)data_0209f394[me] : 0;
    if (!mb) return;
    bool missing = false;
    for (int i = 0; i < count; ++i)
        if (i != me && !data_0209f394[i]) missing = true;
    if (!missing) return;
    const void *vt = *(void *const *)mb;
    const unsigned base = *(const unsigned *)(mb + 0x4) - (unsigned)me;
    for (const char *a = (const char *)_ZN8dActor_c4NextEPKS_(0); a;
         a = (const char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (a == mb || *(void *const *)a != vt) continue;
        const int i = (int)*(const unsigned char *)(a + 0x6d8);
        if (i <= 0 || i >= count || i == me || data_0209f394[i]) continue;
        if (*(const unsigned *)(a + 0x4) != base + (unsigned)i) continue;
        bool seated = false;
        for (int j = 0; j < kPortBodySlots; ++j)
            if (data_0209f394[j] == a) seated = true;
        if (seated) continue;
        data_0209f394[i] = (void *)a;
        ++g_gws.adopted;
        std::fprintf(stderr, "[ghost] hub seat: slot %d's body (id %u) was "
                     "spawned unseated; seated\n", i, *(const unsigned *)(a + 0x4));
    }
}

// THE NEW-FILE OPENING PLAYS ON THE ROM'S OWN ONE-PLAYER SEAT, on the hub too.
//
// The hub seats four bodies, and on the title path the level boot after the
// file pick spawns them before the new-file opening starts. The opening is a
// one-player cutscene and the ROM reads the count all through it: the kuppa
// script numbers its cast from it (ResetKuppaScript: data_0209b270 =
// data_0209f21c), and LakituBro's hand-over looks for the player through
// ClosestPlayer. Measured with four (REVGHOSTCUT1 item 5): the cast came out
// numbered 0..2 instead of 1..3 and walked off its marks, LakituBro ran its
// hand-over during the opening and set the opening-seen bit at f930 (solo
// f3480, after the reload), the ~31 s hand-over after the reload never played,
// and the pause opened during the rest of the opening (the bit gates it). With
// SM64DS_VS_PLAYERS=1 the same console plays the opening byte-identical to solo.
//
// So the hub plays the opening on exactly the seat a one-player boot has, in
// three steps:
//   1. the opening (port_adventure_hub_opening_begin, from
//      tests/walk_window.cpp's title fall-through before the level boot
//      spawns): the count StartFile left (SetNumPlayers(1): data_0208a0e0 = 1,
//      live bytes 1000), data_0209f21c to match, and the session state
//      data_0209fc68 at 0, which is what the a2 seat writes for one player.
//      Nothing is seated for a ghost, so nothing is drawn or held (the seat
//      repair, the hold and the follower all walk data_0209f21c).
//   2. the opening's own closing level load (port_adventure_hub_level_seat,
//      from hal/level_change.cpp before it boots the level): the count goes
//      back to the hub's through the ROM's setter, so that load spawns and
//      seats all four bodies, and the spares are held the moment they spawn
//      (port_adventure_hub_spawned, below).
//   3. the hand-over after that load (LakituBro wakes the player and talks,
//      solo f2504..f3480) still runs with the session state at 0: with 1 the
//      ROM's wireless arms (actor culling, Stage::Behavior) cut it 31 frames
//      short and the opening-seen bit came at f3450. The state goes to the
//      hub's 1 when the hand-over sets the bit (port_adventure_hub_opening_tick,
//      from the per-frame seat repair), or on any level change before that.
// Every other boot (an existing file, SM64DS_LEVEL, VS, solo) never takes this.
static bool g_hub_opening = false;        // step 1 until the closing load
static bool g_hub_opening_spawn = false;  // step 2, until that load spawns
static bool g_hub_handover = false;       // step 3, until the bit
static int g_hub_handover_level = -1;

extern "C" int port_adventure_hub_opening_begin() {
    if (!(adventure_ghost_mode() && comms_hub_active())) return 0;
    std::fprintf(stderr, "[ghost] hub: the new-file opening will play; it "
                 "boots on the ROM's one-player seat (count %d, live bytes "
                 "%d%d%d%d; was %d bodies), the hub's bodies come with the "
                 "opening's closing level load\n", (int)data_0208a0e0,
                 (int)data_0209fc5c[0], (int)data_0209fc5c[1],
                 (int)data_0209fc5c[2], (int)data_0209fc5c[3],
                 (int)data_0209f21c);
    g_hub_opening = true;
    data_0209f21c = data_0208a0e0;
    data_0209fc68[0] = 0;
    return 1;
}

extern "C" int port_adventure_hub_opening_held() { return g_hub_opening ? 1 : 0; }

// Step 2. Called by hal/level_change.cpp before every level boot; does nothing
// unless the opening is on.
extern "C" void port_adventure_hub_level_seat() {
    if (!g_hub_opening) return;
    g_hub_opening = false;
    if (!(adventure_ghost_mode() && comms_hub_active())) return;
    const int want = vs_player_count();
    if (want <= 1 || want > kPortBodySlots) return;
    std::fprintf(stderr, "[ghost] hub: the opening's level load seats the "
                 "hub's %d bodies again (count was %d); SetNumPlayers(%d)\n",
                 want, (int)data_0209f21c, want);
    SetNumPlayers(want);
    data_0209f21c = (unsigned char)want;
    g_hub_opening_spawn = true;
    g_hub_handover = true;
    g_hub_handover_level = (int)data_0209f2f8;
}

// The same load's spawn: the moment hal/level_boot.cpp's entrance loader has
// spawned the four bodies, before anything else the load creates. The spares
// come up on the level's player records 1..3 beside the local body, and the
// hand-over's LakituBro picks the body it puts to sleep through ClosestPlayer
// while the load is still running (func_ov085_0212df84 ->
// func_ov002_020d228c): measured, it took slot 2's body on record 2, and the
// local body walked in through St_LevelEnter instead of waking up. So the hold
// takes the spares at once (Behavior skipped, parked), what it does to them on
// every later frame, and the only body left on a player record is the local
// one, as in a one-player load.
extern "C" void port_adventure_ghost_hold();   // hal/player_bridges.cpp
extern "C" void port_adventure_hub_spawned() {
    if (!g_hub_opening_spawn) return;
    g_hub_opening_spawn = false;
    port_adventure_ghost_hold();
}

// Step 3's end, every frame from port_adventure_hub_seat: the opening-seen bit
// (save byte 8 bit 7, set by func_ov085_0212d5dc when LakituBro is done) or a
// level change ends the hand-over, and the session state takes the hub's 1.
static void port_adventure_hub_opening_tick() {
    if (!g_hub_handover) return;
    const bool seen = (data_0209caa0[8] & 0x80) != 0;
    if (!seen && (int)data_0209f2f8 == g_hub_handover_level) return;
    g_hub_handover = false;
    data_0209fc68[0] = 1;
    std::fprintf(stderr, "[ghost] hub: the opening's hand-over is over (%s); "
                 "the session state goes to 1 like every hub boot\n",
                 seen ? "the opening-seen bit is set" : "the level changed");
}

// Where the follower put a hub ghost this frame (tests/nametag.h reads the tag
// position here: by the time the overlay draws, the body is parked again).
extern "C" int port_adventure_ghost_drawpos(int slot, int out[3]) {
    if (!g_hub || slot < 0 || slot >= kPortBodySlots || !g_drawpos_have[slot])
        return 0;
    out[0] = g_drawpos[slot][0];
    out[1] = g_drawpos[slot][1];
    out[2] = g_drawpos[slot][2];
    return 1;
}

// ---------------------------------------------------------------------------
// ADVENTURE GHOSTS: the pure-follower step. Called once per RENDER frame from
// the walk loop, AFTER sync_tick (so the target is this frame's freshest) and
// after the ROM actor tick (so it OVERRIDES the physics the ghost would
// otherwise drift under). For each remote slot with a target it eases the body
// toward the last received snapshot -- position and facing -- at a fixed
// per-frame fraction, and zeros the body's own speed so the ROM cannot keep
// integrating it. That is the whole of "a ghost is a smooth pure follower":
// the rendered position is this eased value every frame, not the ~30 Hz
// receive-frame correction with ROM drift in between. The local body is never
// touched (the i == me skip), so local_writes stays 0. SM64DS_ADVENTURE_FOLLOW
// tunes the ease fraction (percent per frame, default 40). A teleport snapshot
// lands exactly rather than sliding across the level.
static int g_follow_diag = -1;
static unsigned g_follow_fcount = 0;

// SM64DS_GHOST_BODY_PROBE=<stride>: what every Player body on this console is
// doing, every <stride> follower frames and on any frame the set of bodies
// changes. A Player body is any live actor sharing the local body's vtable, so
// a body the seat table (data_0209f394) does not name is still seen. For each:
// its seat-table index (-1 if none), unique id, spawn param, mPlayerNo,
// character, position, camera-space position (what its sounds are placed
// from), pauseFlags bit 2 (the skip), mOpacity, mIsVanish and its state word;
// the line also carries the count, the four live bytes and the cutscene word.
// Reads only; inert unset.
namespace {
void gw_body_probe() {
    static int stride = -1;
    if (stride < 0) {
        const char *e = std::getenv("SM64DS_GHOST_BODY_PROBE");
        stride = e ? std::atoi(e) : 0;
    }
    if (stride <= 0) return;
    const int me = (int)data_0209f250;
    const char *mb = (me >= 0 && me < kPortBodySlots)
                         ? (const char *)data_0209f394[me] : 0;
    if (!mb) return;
    const void *vt = *(void *const *)mb;
    char buf[1200];
    int w = 0, n = 0;
    unsigned sig = (unsigned)data_0209f21c;
    for (const char *a = (const char *)_ZN8dActor_c4NextEPKS_(0);
         a && w < 1000; a = (const char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (*(void *const *)a != vt) continue;
        int idx = -1;
        for (int i = 0; i < kPortBodySlots; ++i)
            if (data_0209f394[i] == a) idx = i;
        ++n;
        sig = sig * 31u + (unsigned)(idx + 2);
        const int r = std::snprintf(
            buf + w, sizeof buf - (size_t)w,
            " [%s%d id=%u prm=%x no=%u ch=%u pos=(%d,%d,%d) cam=(%d,%d,%d) "
            "skip=%d op=%u van=%u st=%08x]",
            a == mb ? "ME" : "b", idx,
            *(const unsigned *)(a + 0x4), *(const unsigned *)(a + 0x8),
            (unsigned)*((const unsigned char *)a + 0x6d8),
            (unsigned)*((const unsigned char *)a + 0x6d9),
            *(const int *)(a + 0x5c) >> 12, *(const int *)(a + 0x60) >> 12,
            *(const int *)(a + 0x64) >> 12,
            *(const int *)(a + 0x74) >> 12, *(const int *)(a + 0x78) >> 12,
            *(const int *)(a + 0x7c) >> 12,
            (*((const unsigned char *)a + 0x13) & 2) ? 1 : 0,
            (unsigned)*((const unsigned char *)a + 0x6f5),
            (unsigned)*((const unsigned char *)a + 0x6fb),
            *(const unsigned *)(a + 0x370));
        if (r < 0) break;
        w += r;
    }
    buf[w] = 0;
    static unsigned last_sig = 0;
    if ((g_follow_fcount % (unsigned)stride) == 0 || sig != last_sig)
        std::fprintf(stderr, "[gcbody] f=%u hub=%d count=%d live=%d%d%d%d "
                     "cut=%d me=%d bodies=%d%s\n",
                     g_follow_fcount, g_hub ? 1 : 0, (int)data_0209f21c,
                     (int)data_0209fc5c[0], (int)data_0209fc5c[1],
                     (int)data_0209fc5c[2], (int)data_0209fc5c[3],
                     (int)data_0209fc48, me, n, buf);
    last_sig = sig;
}
}  // namespace

// GHOSTLOOK1: the palette row the render stamps into a hub ghost body's
// materials for this frame's draw, or -1 (not a shown hub ghost, no pick, not
// Yoshi): hal/player_bridges.cpp's hal_render_player_world.
//
// Only while the row still holds what the follower wrote, for the body it was
// written for: a level load seats new bodies and uploads the file's rows again
// before the next follower pass, and that frame draws the model's own row.
extern "C" int port_adventure_ghost_palette(int slot) {
    if (!g_hub || slot < 0 || slot >= kPortBodySlots) return -1;
    const int pal = g_gw_pal[slot];
    if (pal < 0 || !g_row_last_have[slot]) return -1;
    if (g_gw_pal_body[slot] != data_0209f394[slot]) return -1;
    const unsigned short *rp = gw_pltt_row(pal);
    if (!rp || std::memcmp(rp, g_row_last[slot], sizeof g_row_last[slot]) != 0)
        return -1;
    return pal;
}

// OWNCOLOR1 (field test 4, F4): THE LOCAL PLAYER'S OWN YOSHI IN HIS OWN PICK, on
// his own screen. GHOSTLOOK1 colours the ghosts (his friends' bodies) and sends
// the pick, so everyone saw him in his colours but himself; his body draws with
// the file's row 0, which is the SOURCE every ghost row is made from and what
// the HUD's Yoshi icon reads, so it is never written. His pick goes into a
// spare row of the sixteen the Yoshi file is grown to (rows 4..15 are
// unreachable unless a body has a player number of 4 or more, and the hub
// seats at most four): the same vs_palette_row out of the same row 0, the same
// pair of env colours, written under the local body's own draw and stamped into
// its materials around it by hal/player_bridges.cpp, as a ghost's is. Hub only
// (g_hub), Yoshi only, and only with both colours set: solo, VS and a hub
// console with no pick are the file's own row, byte for byte.
//
// The pick is the same for the whole run, so the row is one fixed address with
// one fixed set of words: the soft raster's decode cache (keyed on the palette
// base, the texture words and the first two words of the row) can never be
// served another pick's decode for it, and nothing here needs the ghosts'
// same-key hold (gw_row_drawable). A level load uploads the file's rows again,
// so the row is written back whenever it no longer holds the pick.
//
// JOINWIN1: THE ROW MOVED TO 32. HUB32 seats 32 bodies (data_0209f21c 32 on
// every hub level) and grows the file to 32 rows, one per body number, so row
// 15 belongs to body 15 and the guard below refused the pick on every hub
// level: his own Yoshi stayed green on his screen while everyone else saw his
// colour. hal/fs_mods.cpp now grows one row more, row 32, which no body number
// names (0..31).
namespace {
enum { kOwnRowNo = 32 };
struct OwnRow {
    bool have;                       // out is the row for src + the pick
    unsigned short src[16], out[16];
    unsigned writes, draws;
};
OwnRow g_own;

int own_off() {
    static int off = -1;
    if (off < 0) off = std::getenv("SM64DS_OWN_COLOUR_OFF") ? 1 : 0;
    return off;
}
}  // namespace

extern "C" int port_adventure_local_palette(void *body) {
    if (!g_hub || !body || own_off()) return -1;
    gw_me_read();
    if (!g_gw_me.colours) return -1;
    if (gw_char(body) != 3u) return -1;
    const char *c = (const char *)body;
    const int me = (int)data_0209f250;
    if (me < 0 || me >= kPortBodySlots || data_0209f394[me] != body)
        return -1;
    if ((int)data_0209f21c > kOwnRowNo) {            // a body could draw the row
        static bool said;
        if (!said) {
            said = true;
            std::fprintf(stderr, "[own] my pick is not drawn on my own Yoshi: %d "
                         "bodies, and row %d belongs to one of them\n",
                         (int)data_0209f21c, (int)kOwnRowNo);
        }
        return -1;
    }
    const int no = (int)*(const unsigned char *)(c + 0x6d8);
    const int pal = *(const int *)(c + 0x61c);
    if (no != me) return -1;
    const int base = pal - (no << 1);                // row 0
    const int own = base + (kOwnRowNo << 1);
    const unsigned short *r0 = gw_pltt_row(base);
    unsigned short *rp = gw_pltt_row(own);
    if (!r0 || !rp) return -1;
    if (!g_own.have || std::memcmp(g_own.src, r0, sizeof g_own.src) != 0) {
        std::memcpy(g_own.src, r0, sizeof g_own.src);
        vspal::vs_palette_row(g_own.src, g_gw_me.body, g_gw_me.shoes,
                              g_own.out);
        g_own.have = true;
    }
    if (std::memcmp(rp, g_own.out, sizeof g_own.out) != 0) {
        std::memcpy(rp, g_own.out, sizeof g_own.out);
        ++g_own.writes;
    }
    if (!g_own.draws++)
        std::fprintf(stderr, "[own] my Yoshi draws in my pick %02x%02x%02x / "
                     "%02x%02x%02x from palette row %d\n", g_gw_me.body[0],
                     g_gw_me.body[1], g_gw_me.body[2], g_gw_me.shoes[0],
                     g_gw_me.shoes[1], g_gw_me.shoes[2], (int)kOwnRowNo);
    if (gw_look_probe() > 0 &&
        (g_own.draws % (unsigned)gw_look_probe()) == 0) {
        std::fprintf(stderr, "[gwlook] OWN body=%d pal=%d own=%d writes=%u "
                     "draws=%u pick=%02x%02x%02x/%02x%02x%02x row0=%04x,%04x "
                     "rowpal=%04x,%04x words", no, pal, own, g_own.writes,
                     g_own.draws, g_gw_me.body[0], g_gw_me.body[1],
                     g_gw_me.body[2], g_gw_me.shoes[0], g_gw_me.shoes[1],
                     g_gw_me.shoes[2], r0[0], r0[1], rp[0], rp[1]);
        for (int i = 0; i < 16; ++i) std::fprintf(stderr, " %04x", rp[i]);
        std::fprintf(stderr, " src");
        for (int i = 0; i < 16; ++i) std::fprintf(stderr, " %04x", r0[i]);
        std::fprintf(stderr, "\n");
    }
    return own;
}

extern "C" void port_adventure_ghost_follow() {
    if (!adventure_ghost_mode()) return;
    static int fpct = -1;
    if (fpct < 0) {
        const char *e = std::getenv("SM64DS_ADVENTURE_FOLLOW");
        fpct = e ? std::atoi(e) : 40;
        if (fpct < 1) fpct = 1;
        if (fpct > 100) fpct = 100;
    }
    if (g_follow_diag < 0) {
        const char *e = std::getenv("SM64DS_ADVENTURE_DIAG");
        g_follow_diag = e ? std::atoi(e) : 0;   /* value = log stride, 0 = off */
    }
    ++g_follow_fcount;
    gw_body_probe();
    if (g_hub) { gw_follow(); return; }
    const int me = (int)data_0209f250;
    for (int i = 0; i < kPortBodySlots; ++i) {
        if (i == me) continue;
        /* PRESENCE (M2): only DRIVE a ghost that is present in this console's
           level. A peer in another level (or gone quiet) is despawned -- its
           body is left where it was, undriven, and the render/name-tag loops
           skip it on the same predicate, so nothing about it shows. */
        if (!peer_visible(i)) continue;
        // PARTY: a party member is a shared-world body driven by the rollback
        // sim, not a broadcast ghost -- leave it to the sim, do NOT ease it
        // toward a snapshot here (that would fight the sim).
        if (party_member(i)) continue;
        if (!g_gtarget[i].have) continue;
        void *a = data_0209f394[i];
        if (!a) continue;
        GhostTarget &t = g_gtarget[i];
        int *px = player::pos_x(a), *py = player::pos_y(a), *pz = player::pos_z(a);
        if (t.teleport) {
            /* a warp snaps everything -- position and facing -- because there is
               no smooth path between a body's two sides of a teleport. */
            *px = t.x; *py = t.y; *pz = t.z;
            *player::facing(a) = t.yaw;
        } else {
            /* DEAD-RECKON THE TARGET one frame at the sender's own velocity, so
               the target moves continuously and the follower does not beat at the
               ~30 Hz snapshot cadence. The next snapshot overwrites x/z with the
               true value, correcting any extrapolation error. Y is not reckoned
               (it is a parabola under gravity); the body eases toward the last
               received Y. Velocity is clamped GENEROUSLY -- 64 units/frame,
               unlike apply_snapshot's 3-unit corrector ceiling -- because a
               follower target must move at the sender's REAL speed (a dash is
               ~18 units/frame, far over 3) or the target steps at the snapshot
               cadence and the ghost beats; 64 still guards a garbage delta. */
            int vx = t.vx, vz = t.vz;
            const int vcap = units(64);
            if (vx > vcap) vx = vcap; else if (vx < -vcap) vx = -vcap;
            if (vz > vcap) vz = vcap; else if (vz < -vcap) vz = -vcap;
            t.x += vx;
            t.z += vz;
            *px += (t.x - *px) * fpct / 100;
            *py += (t.y - *py) * fpct / 100;
            *pz += (t.z - *pz) * fpct / 100;
            /* facing eased the SHORT way: the signed 16-bit wrap of
               (target - cur) is the shortest angular delta, so a ghost turning
               past 0 does not spin the long way round. */
            const short cur = *player::facing(a);
            const short d = (short)(t.yaw - cur);
            *player::facing(a) = (short)(cur + (int)d * fpct / 100);
        }
        /* suppress the ROM physics drift on the puppet: zero the Actor's own
           horizontal (+0x98) and vertical (+0xa8) speed so nothing integrates
           the body away from the target between snapshots. */
        *(int *)((char *)a + 0x98) = 0;
        *(int *)((char *)a + 0xa8) = 0;
        /* DRIVE THE ANIMATION EVERY FRAME. The ROM's own Player::Behavior ran on
           this ghost earlier in the frame and reset its pose to the spawn
           no-control animation; re-issuing the wire animation here, after that,
           makes the follower's animation the last word each frame instead of a
           value that only lands on the ~30 Hz receive frames. The cursor is
           advanced one frame per render frame (speed 1.0 in 20.12) so the cycle
           plays, and re-synced to the sender's cursor on every snapshot above so
           it does not drift out of phase. SetAnim is the legal cursor road (it
           forces the slow-path reset that makes startFrame land); a raw +0x58
           store would fire the animation's WillHitFrame events twice. */
        if (g_ganim_have[i]) {
            g_gcursor[i] += 0x1000;
            _ZN6Player7SetAnimEji5Fix12IiEj(a, g_ganim[i], 0, 0x1000,
                                            (unsigned)(g_gcursor[i] >> 12));
        }
        if (g_follow_diag > 0 && (g_follow_fcount % (unsigned)g_follow_diag) == 0)
            std::fprintf(stderr, "[advfollow] slot%d render anim=%u cur=%d\n",
                         i, (unsigned)player::anim_id(a),
                         player::anim_frame(a) >> 12);
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

// ---------------------------------------------------------------------------
// THE LIVENESS GATE, and the field failure that ordered it (owner live,
// 2026-08-28 13:03, runs/mg16/out/MP2/two_windows, the play_20260828_1303*
// pair). The lockstep session died at round 83 -- both instances fell to solo,
// connected=no players=1 role=0 -- and this layer had no opinion about that:
// enabled once at sync_decide, it would keep ticking against whatever the
// carrier still had, correcting remote bodies between two sims that no longer
// share a wire. The carrier's own guards (live-mask fan-out, close() clearing
// the aux slots) happened to keep the traffic at zero in that session, but
// "the carrier happened to save us" is not a policy, and a future carrier --
// or a half-dead session where one side still holds a live mask -- would sync
// two solo worlds against each other.
//
// So the layer asks the SEAM the same question the [comms:level] report line
// prints -- comms_readout(), whose `connected` is link state 3 or 4 off the
// transport's own state() and whose `players` is the live count -- and goes
// QUIET when the answer is not a live session: no receive, no apply, no
// snapshot, no ping. Counted per frame in `gated` (on the report line, so a
// playlog shows the gate working), said once per transition in each
// direction. On (re)connect it resumes by itself: the gate re-reads the seam
// every frame and g_enabled was never touched. Held aux in the delay rig dies
// with the session -- a message describing a body from the dead session must
// not apply into the next one.
// ---------------------------------------------------------------------------
namespace {
bool g_gated_now = false;
}

// ===========================================================================
// THE DIVERGENCE DETECTOR (lane VCHOMP). DEAD BY DEFAULT.
// ===========================================================================
//
// WHY IT EXISTS. Two-window VS runs in INPUT LOCKSTEP: the windows trade
// controller records and each simulates the whole world from them. So the two
// worlds are supposed to be bit-identical every frame, and the FIRST frame on
// which they are not is the crime scene -- everything after it is that one
// difference compounding. Nothing in the port could name that frame, so every
// desync report ("the chomp drifted") had to be chased by reading code.
//
// This walks the ROM's own live-actor list once a frame and prints a hash of
// every actor's physical state. Run both windows with it on, diff the two
// logs, and the first differing frame plus the actor that owns the difference
// falls out of port/tools/dhdiff.py.
//
// PLACED IN sync_tick's FILE, CALLED FROM ITS FIRST LINE, on purpose:
// tests/walk_window.cpp already calls sync_tick() once per frame of the level
// loop, right after func_0203df40 has put this frame's input record on the
// wire and before the fan-out. That is the sampling point a lockstep detector
// wants -- a fixed spot in the frame, the same spot in both windows -- and
// using it means this instrument adds no call site to any file another lane
// owns and no source file to CMake. sync_tick's own body still early-returns
// on !g_enabled; the detector runs AHEAD of that gate because it must work
// with the sync layer OFF, which is the shipped configuration and the one
// every desync report comes from.
//
// THAT CALL IS NOT UNCONDITIONAL, and an earlier version of this banner said
// it was. It sits inside `if (real_camera)` (walk_window.cpp:10023; the flag
// is on unless SM64DS_OLD_CAMERA is set), inside the level loop. So the frame
// counter below counts FRAMES ON WHICH THIS RAN, which is not the same thing
// as the game's frame number and not guaranteed to mean the same thing in two
// windows: a session with menus and level loads can enter and leave that path
// at different wall-clock moments on the two consoles.
//
// WHICH IS WHY EVERY [dh] LINE CARRIES rounds=. That is the comms round
// counter, the exchanged one both consoles agree on, and it -- not the local
// frame index -- is the sound key for lining two logs up. dhdiff.py checks it
// before comparing anything and refuses rather than guess. Comparing two logs
// on a frame index that has silently drifted is the one way this instrument
// could report a FALSE divergence, and a false positive on the owner's single
// live capture is worse than no capture at all.
//
// COST WHEN OFF: one compare of an int that is resolved on the first call.
//
// KNOBS
//   SM64DS_VS_STATE_HASH=1        per-frame world hash line only. Names the
//                                 FRAME. ~60 bytes/frame.
//   SM64DS_VS_STATE_HASH=2        plus a compact one-line digest of every
//                                 actor's uid:hash. Names the frame AND THE
//                                 ACTOR. ~630 bytes/frame. THIS IS THE LEVEL
//                                 A LIVE CAPTURE SHOULD RUN AT -- see the
//                                 note at the digest for the arithmetic.
//   SM64DS_VS_STATE_HASH=3        plus a verbose line per actor per frame,
//                                 which additionally names the FIELD that
//                                 moved. ~3800 bytes/frame; the lab level.
//   SM64DS_VS_STATE_HASH_ID=<n>   verbose lines for this actorID (decimal) at
//                                 any level -- the cheap way to watch one
//                                 suspect class in full detail
//   SM64DS_VS_STATE_HASH_FROM=<n> start at frame n (skips boot churn)
//   SM64DS_VS_STATE_HASH_WIN=<off>:<len>
//                                 also hash, and print, <len> raw bytes at
//                                 <off> in the actor named by _ID. This is
//                                 what turns "the chomp diverged" into "the
//                                 chomp's path-node index diverged": subclass
//                                 fields live past Actor's 0xd0 and no generic
//                                 dump can know their names. REQUIRES _ID --
//                                 see the note at the read. Both numbers
//                                 accept 0x; len is capped at 64.
//
// THE WORLD HASH IS ORDER-INDEPENDENT (a sum of per-actor hashes, each seeded
// with that actor's uniqueID) and the ORDER hash beside it is not. Two numbers
// rather than one so that "the list came out in a different order" and "an
// actor's state differs" are distinguishable at a glance; a single
// order-dependent hash would report a reordering as a state change and send
// the next reader after the wrong thing.
//
// WHAT IT READS. Raw offsets off the real Actor object, per include/Actor.h:
// +0x04 uniqueID, +0x0c actorID, +0x0e aliveState, +0x5c..0x64 position,
// +0x8c..0x90 angles, +0x98 horz speed, +0xa8 vert speed, +0xb0 flags. Raw
// reads rather than the C++ header are the established convention in port/hal
// -- see the banner on hal/editor_channel.cpp, which walks this same list the
// same way. It never writes anything.
//
// Actor::Next is src/_ZN8dActor_c4NextEPKS_.cpp, a byte-matched ROM TU already
// linked into walk_window -- the same traversal the game's own Behavior loops
// use, so the set this reports is the set that is actually being ticked.
// ---------------------------------------------------------------------------
extern "C" void *_ZN8dActor_c4NextEPKS_(const void *prev);

namespace {

struct DhCfg {
    int level;        // 0 = off, -1 = not parsed yet
    int only_id;      // -1 = every actor
    int from_frame;
    int win_off;
    int win_len;
};
DhCfg g_dh = {-1, -1, 0, 0, 0};
int g_dh_frame = 0;

int dh_env_int(const char *name, int dflt) {
    const char *v = std::getenv(name);
    if (!v || !*v) return dflt;
    return (int)std::strtol(v, 0, 0);
}

void dh_init() {
    g_dh.level = dh_env_int("SM64DS_VS_STATE_HASH", 0);
    g_dh.only_id = dh_env_int("SM64DS_VS_STATE_HASH_ID", -1);
    g_dh.from_frame = dh_env_int("SM64DS_VS_STATE_HASH_FROM", 0);
    g_dh.win_off = 0;
    g_dh.win_len = 0;
    const char *w = std::getenv("SM64DS_VS_STATE_HASH_WIN");
    if (w && *w) {
        char *end = 0;
        long off = std::strtol(w, &end, 0);
        if (end && *end == ':') {
            long len = std::strtol(end + 1, 0, 0);
            if (off >= 0 && len > 0) {
                if (len > 64) len = 64;
                g_dh.win_off = (int)off;
                g_dh.win_len = (int)len;
            }
        }
    }
    if (g_dh.level > 0)
        std::fprintf(stderr,
                     "[dh] detector armed: level=%d only_id=%d from=%d "
                     "win=0x%x:%d\n",
                     g_dh.level, g_dh.only_id, g_dh.from_frame, g_dh.win_off,
                     g_dh.win_len);
}

inline void dh_mix(unsigned &h, unsigned v) {
    h ^= v;
    h *= 16777619u;   // FNV-1a
}

void dh_frame() {
    if (g_dh.level < 0) dh_init();
    if (g_dh.level <= 0) return;

    const int frame = g_dh_frame++;
    if (frame < g_dh.from_frame) return;

    unsigned world = 0;              // order-independent: a SUM
    unsigned order = 2166136261u;    // order-dependent: a chain
    int n = 0;
    // The level-2 digest, built as we walk. Sized for the biggest actor list
    // this port mounts with room to spare, and the append is bounds-checked
    // rather than trusted: a level with more actors than fit must truncate the
    // digest, never the stack.
    char dig[64 * 32];
    int dig_len = 0;
    dig[0] = 0;

    for (const char *a = (const char *)_ZN8dActor_c4NextEPKS_(0); a;
         a = (const char *)_ZN8dActor_c4NextEPKS_(a)) {
        const unsigned uid = *(const unsigned *)(a + 0x04);
        const unsigned id = *(const unsigned short *)(a + 0x0c);
        const unsigned alive = *(const unsigned char *)(a + 0x0e);
        const int px = *(const int *)(a + 0x5c);
        const int py = *(const int *)(a + 0x60);
        const int pz = *(const int *)(a + 0x64);
        const int ax = *(const short *)(a + 0x8c);
        const int ay = *(const short *)(a + 0x8e);
        const int az = *(const short *)(a + 0x90);
        const int sh = *(const int *)(a + 0x98);
        const int sv = *(const int *)(a + 0xa8);
        const unsigned fl = *(const unsigned *)(a + 0xb0);

        // THE RAW WINDOW IS ONLY EVER READ OFF THE NAMED CLASS. Actors are not
        // all the same size -- a plain Actor is 0xd0 bytes and the chomp is
        // 0x7a4 -- so a window aimed at one class's subclass fields is an
        // over-read on every smaller actor in the list, which would both risk
        // a fault and manufacture a divergence out of whatever heap bytes
        // happened to follow. Requiring SM64DS_VS_STATE_HASH_ID to name the
        // class makes the window exactly as wide as the thing that asked for
        // it. It costs nothing: a window over a class you have not named has
        // no meaning anyway.
        const bool named = (g_dh.only_id >= 0 && (int)id == g_dh.only_id);
        const int wlen = named ? g_dh.win_len : 0;

        unsigned h = 2166136261u;
        dh_mix(h, uid);
        dh_mix(h, id);
        dh_mix(h, alive);
        dh_mix(h, (unsigned)px);
        dh_mix(h, (unsigned)py);
        dh_mix(h, (unsigned)pz);
        dh_mix(h, (unsigned)(ax & 0xffff) | ((unsigned)(ay & 0xffff) << 16));
        dh_mix(h, (unsigned)(az & 0xffff));
        dh_mix(h, (unsigned)sh);
        dh_mix(h, (unsigned)sv);
        // THE CULL BITS ARE MASKED OUT OF THE HASH, AND THIS IS NOT A
        // CONVENIENCE. Actor::BeforeBehavior (src/_ZN8dActor_c14BeforeBehaviorEv
        // .cpp) transforms the actor by data_0209b3ec -- THE CAMERA'S view
        // matrix -- runs a Clipper distance test on the result and then writes
        // bits 0x08/0x10/0x20 of this very word from the answer (|= 0x38,
        // |= 0x18, |= 0x10, &= ~0x38). Every window has its own camera, so
        // those three bits are per-window BY DESIGN and differ constantly
        // between two correctly-synchronised consoles. Hashing them raw would
        // make every actor in the arena report a divergence on the first
        // frame and bury the one that matters.
        //
        // The rest of the word IS hashed, including bit 0x10000 -- the bit
        // that turns a cull into a SKIPPED Behavior, which is a real
        // divergence and must not be masked away with the cosmetic ones.
        dh_mix(h, fl & ~0x38u);
        for (int k = 0; k < wlen; ++k)
            dh_mix(h, *(const unsigned char *)(a + g_dh.win_off + k));

        world += h;
        dh_mix(order, uid);
        ++n;

        // LEVEL 2: the compact digest, accumulated here and emitted as ONE
        // line below. This is the level a LIVE capture should run at, and the
        // reason it exists is bytes. Level 3 prints a verbose line per actor
        // per frame, which measured 3787 bytes/frame in this arena -- on the
        // order of 400 MB per window over a session the length of the owner's,
        // against the ~179 bytes/frame the comms dump already writes. The
        // digest carries the same "which actor moved" answer in about 610
        // bytes/frame; only the FIELD that moved needs the verbose form, and
        // by then you have the frame and can re-run at level 3.
        //
        // THE actorID IS IN THE LINE, not just the uid, and it is not
        // optional. uid is a per-console allocation counter: it identifies
        // the object but names nothing. NOTHING ELSE IN A PLAYLOG MAPS A uid
        // TO A CLASS, so a digest of uid:hash alone would hand a live capture
        // "frame N, uid 5" and no way to turn that into "the chain chomp"
        // except by re-running a session that by definition did not
        // reproduce. The id costs about ten characters a row and it is the
        // difference between a capture that answers the question and one that
        // only proves there was a question.
        if (g_dh.level >= 2 && dig_len < (int)sizeof dig - 32)
            dig_len += std::sprintf(dig + dig_len, " %u:%u:%08x", uid, id, h);

        if (g_dh.level >= 3 || named) {
            char win[64 * 2 + 1];
            win[0] = 0;
            for (int k = 0; k < wlen; ++k)
                std::sprintf(win + k * 2, "%02x",
                             *(const unsigned char *)(a + g_dh.win_off + k));
            std::fprintf(stderr,
                         "[dh+] f%d uid=%u id=%u al=%u pos=%d,%d,%d "
                         "ang=%d,%d,%d spd=%d,%d fl=%08x h=%08x%s%s\n",
                         frame, uid, id, alive, px, py, pz, ax, ay, az, sh, sv,
                         fl, h, wlen ? " w=" : "", win);
        }
    }

    const CommsReadout rr = comms_readout();
    std::fprintf(stderr, "[dh] f%d n=%d w=%08x o=%08x rounds=%llu slot=%d\n",
                 frame, n, world, order, (unsigned long long)rr.rounds,
                 rr.slot);
    if (g_dh.level >= 2)
        std::fprintf(stderr, "[dh=] f%d%s\n", frame, dig);
}

}  // namespace

/* port/rollback: the detector numbers frames with its own counter, which a
   rewind must put back with the world or every hash line after the first
   rollback names a different frame than the other console's. */
extern "C" int  port_dh_frame_get(void) { return g_dh_frame; }
extern "C" void port_dh_frame_set(int f) { g_dh_frame = f; }

void sync_tick() {
    dh_frame();      // VCHOMP detector: ahead of the gate on purpose -- it has
                     // to work with the sync layer off. No-op unless armed.
    if (!g_enabled) return;
    if (g_hub) { gw_tick(); return; }   // GHOSTWIRE1: the hub, no session gate
    const CommsReadout r = comms_readout();
    if (!r.connected || r.players <= 1) {
        ++g_stats.gated;
        if (!g_gated_now) {
            g_gated_now = true;
            g_dq_head = 0;
            g_dq_count = 0;            // held aux dies with the session
            std::fprintf(stderr,
                         "[sync] gated: the lockstep session is not live "
                         "(link=%d players=%d); the sync layer goes quiet "
                         "until it is\n",
                         r.link_state, r.players);
        }
        return;
    }
    if (g_gated_now) {
        g_gated_now = false;
        std::fprintf(stderr,
                     "[sync] resumed: the lockstep session is live again "
                     "after %llu gated frames\n",
                     (unsigned long long)g_stats.gated);
    }
    sync_recv_pump();      // take what has arrived (and is due) first
    sync_send_own();       // then publish OUR body, after the input record
    sync_send_ping();      // and the rig's probe last, behind the state
}


SyncStats sync_stats() { return g_stats; }

void sync_report(const char *tag) {
    parse_cfg();
    if (g_hub) gw_report(tag);
    /* The prefix through worst_err is parsed by the SY rungs
       (port/tools/mp3_play_proof.py) and stays byte-identical; the rig's
       fields append after it. */
    std::fprintf(stderr,
                 "[sync:%s] enabled=%s sent=%llu recvd=%llu dropped=%llu "
                 "applied=%llu lerps=%llu snaps=%llu worst_err=%d "
                 "delay=%d rtt_last=%d rtt_avg=%d pings=%llu pongs=%llu "
                 "own_claims=%llu local_writes=%llu evsends=%llu "
                 "reseeds=%llu phase_worst=%d avg_err=%d gated=%llu "
                 "pose_held=%llu\n",
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
                 g_stats.phase_worst,
                 (int)(g_stats.err_n ? g_stats.err_sum /
                                           (long long)g_stats.err_n
                                     : 0),
                 (unsigned long long)g_stats.gated,
                 (unsigned long long)g_stats.pose_held);
}

/* INTERACT9: what the follower last drew for a seat (anim id, character), for party_hits' grab and eat gates. */
bool comms_sync_ghost_look(int seat, unsigned *anim, unsigned *character) {
    if (seat < 0 || seat >= kHubMaxSeats || !g_gw[seat].last_have) return false;
    if (anim) *anim = g_gw_last[seat].anim;
    if (character) *character = g_gw_last[seat].character;
    return true;
}

// ===========================================================================
// THE IN-PLACE LEAVE (run after093, lane SEAMHUB1; the one-process plan's M-e
// and H-a). Leaving an Adventure Online world to the title in this process:
// hal/online_join.cpp calls this at the title, once the Stage (and with it
// every ghost body) is gone and comms_hub_teardown has stopped the hub. Every
// file global of the sync and ghost layers goes back to the value a process
// that never joined starts with, so nothing of the session is drawn, sent or
// counted again: the layer is off (g_enabled, g_hub), the configuration is
// parsed again on the next decide, the read-once identity (g_gw_me: name and
// colours) is read again from the session record (empty after the leave), the
// own colour row (g_own) is forgotten, and the opening hold is off.
//
// THE ROM'S BODY WORDS. The hub seats 32 bodies through the ROM's own setter
// and opens the per-player input gate (data_0209fc68[0] = 1, what
// hal/level_boot.cpp does for more than one player). The title's StartFile
// puts the count back to 1 on the next file pick (src/StartFile.c), but the
// gate is the port's: it goes back to 0 here, and the count is put to the
// solo value through the same setter now, so the title runs with the words a
// solo boot has. Environment knobs latched in function statics (probe and
// tuning knobs) mean the same before and after and are kept.
// ===========================================================================
template <class T, size_t N> static void leave_zero(T (&a)[N]) {
    for (size_t i = 0; i < N; ++i) a[i] = T();
}

static void sync_globals_reset();

void comms_sync_leave_reset() {
    int bodies = 0;
    for (int i = 0; i < kPortBodySlots; ++i)
        if (data_0209f394[i]) ++bodies;
    const int was_count = (int)data_0209f21c, was_rom = (int)data_0208a0e0, was_gate = data_0209fc68[0];

    sync_globals_reset();

    /* the Stage's teardown destroyed every body, but the ROM's per-player actor table keeps the pointers (each Player seats itself
       there at spawn and nothing clears the slot on its way out); a solo title has none of them, and the hub's 31 ghost slots are
       never seated by a solo spawn, so they would point at freed actors for the rest of the process */
    for (int i = 0; i < kPortBodySlots; ++i) data_0209f394[i] = 0;
    data_0209fc68[0] = 0;
    SetNumPlayers(1);
    data_0209f21c = data_0208a0e0;
    std::fprintf(stderr, "[seamless] ghost layer reset: off, identity and own colour row forgotten; "
                 "body words: count %d -> %d (ROM %d -> %d), input gate %d -> %d, %d stale body pointer(s) cleared\n",
                 was_count, (int)data_0209f21c, was_rom, (int)data_0208a0e0, was_gate, data_0209fc68[0], bodies);
    std::fflush(stderr);
}

// SEAMJOIN1: every file global of the sync and ghost layers back to a process that never joined (the leave's half above, without
// the ROM's body words, which the join leaves to the level boot exactly as a restarted game does).
static void sync_globals_reset() {
    g_cfg = SyncCfg();
    g_send_every = 1;
    g_frame = 0;
    g_parsed = false;
    g_enabled = false;
    g_said_refusal = false;
    g_stats = SyncStats();
    leave_zero(g_delayq);
    g_dq_head = 0;
    g_dq_count = 0;
    g_last_ping_ms = 0;
    g_ev_tokens = kEvBurst;
    g_ev_seeded = false;
    g_ev_anim = 0;
    g_ev_ground = 0;
    leave_zero(g_arr_seen);
    leave_zero(g_arr_anim);
    leave_zero(g_ph_id);
    leave_zero(g_ph_hw);
    leave_zero(g_gtarget);
    leave_zero(g_ganim);
    leave_zero(g_gcursor);
    leave_zero(g_ganim_have);
    leave_zero(g_peer_level);
    leave_zero(g_peer_seen_ms);
    leave_zero(g_peer_present);
    g_vel_seeded = false;
    leave_zero(g_vel_prev);
    leave_zero(g_vel);
    g_warp_pending = false;

    leave_zero(g_gw);
    g_hub = false;
    g_gws = GwStats();
    g_gw_t0 = 0;
    g_gw_f0 = 0;
    g_gw_t0_set = false;
    g_gw_sent = HubGhostFrame();
    g_gw_sent_have = false;
    g_gw_last_seq = 0xffffffffu;
    g_gw_last_send_ms = 0;
    g_gw_last_ident_ms = 0;
    g_gw_ident_sent = false;
    g_gw_roster = 0;
    g_gw_beh_last = 0;
    g_gw_beh_seeded = false;
    g_gw_frozen = 0;
    leave_zero(g_gw_fz);
    g_gw_prev_have = false;
    leave_zero(g_gw_prev);
    g_gw_prev_level = -1;
    g_gw_warp = false;
    g_gw_prev_dkind = 0;
    leave_zero(g_gw_v);
    g_gw_last_report_ms = 0;
    g_gw_frame = 0;
    g_gw_quiet = false;
    g_gw_me = GwMe();
    leave_zero(g_body_seat);
    leave_zero(g_body_since);
    g_assign_init = false;
    g_gw_myseat_seen = 0xffffffffu;
    leave_zero(g_gw_last);
    leave_zero(g_row_seen);
    std::memset(g_row_last, 0, sizeof g_row_last);
    leave_zero(g_row_last_have);
    g_gw_follow_n = 1;
    leave_zero(g_drawpos_have);
    leave_zero(g_gw_pal);
    leave_zero(g_gw_pal_body);
    std::memset(g_drawpos, 0, sizeof g_drawpos);
    leave_zero(g_gtongue);
    leave_zero(g_gblink);
    g_hub_opening = false;
    g_hub_opening_spawn = false;
    g_hub_handover = false;
    g_hub_handover_level = -1;
    g_follow_diag = -1;
    g_follow_fcount = 0;
    g_own = OwnRow();
    g_gated_now = false;
}

// ===========================================================================
// THE IN-PLACE JOIN'S HALF (run after093, lane SEAMJOIN1; the one-process
// plan's M4b). A restarted join successor decides the ghost layer once, at its
// first pad publish (hal/comms_conductor.cpp comms_publish_pad -> sync_decide),
// with the hub already installed by main(). A process joining in place made
// that decision long ago as a solo game (the layer off), and nothing asks
// again. So, once hal/online_join.cpp has the hub installed from the session
// record: every global back to a process that never joined (the leave's
// reset), then the same decision, now on the hub's arm (gw_decide: g_hub,
// g_enabled, the identity read from the record). The body count needs nothing
// here: the level boot asks vs_player_count() (hub_body_count: 32) at the
// spawn, as it does in a restarted successor, and the own colour row is filled
// from g_gw_me the first time a body draws.
// ===========================================================================
bool comms_sync_join_in_place() {
    sync_globals_reset();
    sync_decide();
    /* THE BRING-UP THE A2 SEAT MAKES ONCE PER PROCESS (hal/level_boot.cpp port_a2_seat_body, at boot, before the title). In a successor
       it ran with the hub installed: the per-player input gate open, the ROM's count and live bytes at the hub's bodies through the ROM's
       own setter, the window copy of the count, and my comms slot. This process ran it as a solo game, so the same four words are made
       here, in the same order, with the hub now installed; the title's StartFile then takes the count to 1 and the hub seat repair puts
       it back at the level's spawn, as in a successor. */
    const int bodies = vs_player_count();
    const int was_gate = data_0209fc68[0], was_count = (int)data_0208a0e0;
    data_0209fc68[0] = (bodies > 1) ? 1 : 0;
    SetNumPlayers(bodies);
    data_0209f21c = (unsigned char)bodies;
    data_0209f250 = (unsigned char)func_0203da9c();
    std::fprintf(stderr, "[seamless] the a2 seat's player words made again for the hub: input gate %d -> %d, count %d -> %d, "
                 "I am slot %d\n", was_gate, data_0209fc68[0], was_count, (int)data_0208a0e0, (int)data_0209f250);
    std::fprintf(stderr, "[seamless] ghost layer decided again in this process: %s; identity name \"%s\", colours %s "
                 "%02x%02x%02x / %02x%02x%02x; the level boot seats %d bodies\n",
                 g_hub && g_enabled ? "ON on the hub" : "OFF", g_gw_me.name, g_gw_me.colours ? "picked" : "none (the file's own)",
                 g_gw_me.body[0], g_gw_me.body[1], g_gw_me.body[2], g_gw_me.shoes[0], g_gw_me.shoes[1], g_gw_me.shoes[2],
                 vs_player_count());
    std::fflush(stderr);
    return g_hub && g_enabled;
}

void comms_sync_reports_off() {
    g_gw_quiet = true;
}

void comms_sync_identity_text(char *out, int cap) {
    gw_me_read();
    std::snprintf(out, (size_t)cap, "name \"%s\" (%d byte(s)), colours %s %02x%02x%02x / %02x%02x%02x, own row %s",
                  g_gw_me.name, g_gw_me.name_len, g_gw_me.colours ? "picked" : "none (the file's own)",
                  g_gw_me.body[0], g_gw_me.body[1], g_gw_me.body[2], g_gw_me.shoes[0], g_gw_me.shoes[1],
                  g_gw_me.shoes[2], g_own.have ? "held" : "none");
}

}  // namespace port
