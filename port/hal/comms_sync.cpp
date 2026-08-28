// HOST-AUTHORITATIVE STATE SYNC. Run mg16 lane MP4.
//
// THE DESIGN IS runs/mg16/status/MP4-DESIGN.md and this file implements it.
// Read that first: it says what this does, what it deliberately does NOT do,
// and what the correction constants are guesses about.
//
// WHAT THIS STAGE IS. The gate ruled the build order: SY4 and SY0 before any
// sync sends. So this file currently implements the LAYER -- the enable
// decision, the contract check, the refusal, the readout -- and NOT the sending
// or applying of state. That is deliberate and it is the checkpoint: prove that
// OFF is unchanged and that a v1 transport is refused cleanly, before any code
// exists that could move a remote body.
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

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
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
                         "and %s an aux channel, so it cannot carry state "
                         "sync. FALLING BACK TO THE DS INPUT-LOCKSTEP PATH, "
                         "which is complete and is the shipped default -- this "
                         "is a downgrade, not a failure.\n",
                         t->name ? t->name : "(unnamed)", ver,
                         (t->send_aux && t->recv_aux) ? "has" : "lacks");
        }
        return;
    }

    if (!g_enabled) {
        g_enabled = true;
        std::fprintf(stderr,
                     "[sync] enabled on transport '%s' (contract v%u): %d Hz, "
                     "lerp %d%%/frame, snap over %d units. The host is "
                     "authoritative for REMOTE bodies only; local input and the "
                     "DS lockstep are unchanged.\n",
                     t->name ? t->name : "(unnamed)", ver, g_cfg.hz,
                     g_cfg.lerp_pct, g_cfg.snap_units);
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
};
struct SyncMsgV1 {
    unsigned       magic;     // kSyncMagic
    unsigned       frame;     // the session frame this state is FROM
    unsigned char  count;
    unsigned char  pad[3];
    // SyncPlayerV1 follows, `count` of them
};
#pragma pack(pop)

enum : unsigned { kSyncMagic = 0x314e5953u };   // 'S','Y','N','1'
enum : unsigned char { kFlagLive = 1, kFlagTeleport = 4 };
enum : int { kSyncBufBytes = 256 };

// One DS unit is 4096 in Fix12. The thresholds are in units and converted here
// so the constants read the way the design note states them.
inline int units(int n) { return n * 4096; }

// ---------------------------------------------------------------------------
// THE SEND SIDE -- host only.
//
// AUTHORITY IS THE HOST'S, so only the host sends. A client that sent would be
// asserting its own view of a body the host owns, which is how two authorities
// and a fight over one actor happen.
//
// ORDERING: this runs AFTER the conductor's exchange has returned for the
// frame, so the input record is always on the wire first. That is the
// contract's ordering rule and it is satisfied by call position rather than by
// a comment -- rung SY6 measures the consequence.
// ---------------------------------------------------------------------------
void sync_send_if_host() {
    if (!g_enabled) return;
    const CommsTransport *t = comms_transport();
    if (!t || !t->send_aux) return;
    if (t->slot() != 0) return;                 // host only
    if (++g_frame % (unsigned)g_send_every) return;

    unsigned char buf[kSyncBufBytes];
    SyncMsgV1 *m = (SyncMsgV1 *)buf;
    m->magic = kSyncMagic;
    m->frame = g_frame;
    m->count = 0;
    m->pad[0] = m->pad[1] = m->pad[2] = 0;

    SyncPlayerV1 *e = (SyncPlayerV1 *)(buf + sizeof(SyncMsgV1));
    const int n = (int)data_0209f21c;
    for (int i = 0; i < n && i < kCommsMaxPlayers; ++i) {
        void *a = data_0209f394[i];
        if (!a) continue;
        e->slot  = (unsigned char)i;
        e->flags = kFlagLive;
        e->yaw   = *player::facing(a);
        e->x     = *player::pos_x(a);
        e->y     = *player::pos_y(a);
        e->z     = *player::pos_z(a);
        ++e;
        ++m->count;
    }
    if (m->count == 0) return;

    const int len = (int)sizeof(SyncMsgV1) + m->count * (int)sizeof(SyncPlayerV1);
    if (t->send_aux(buf, len) == len) ++g_stats.sent;
}

// ---------------------------------------------------------------------------
// THE APPLY SIDE -- clients only, REMOTE bodies only.
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
// ---------------------------------------------------------------------------
void sync_apply() {
    if (!g_enabled) return;
    const CommsTransport *t = comms_transport();
    if (!t || !t->recv_aux) return;
    if (t->slot() == 0) return;                 // the host has nothing to apply

    unsigned char buf[kSyncBufBytes];
    const int n = t->recv_aux(buf, sizeof buf);
    if (n < (int)sizeof(SyncMsgV1)) return;
    if (g_cfg.drop_pct > 0) {
        /* A cheap deterministic-per-run LCG, not rand(): a proof that behaves
           differently every run is not a proof. */
        static unsigned r = 0x12345678u;
        r = r * 1664525u + 1013904223u;
        if ((int)((r >> 16) % 100u) < g_cfg.drop_pct) { ++g_stats.dropped; return; }
    }
    const SyncMsgV1 *m = (const SyncMsgV1 *)buf;
    if (m->magic != kSyncMagic) { ++g_stats.dropped; return; }
    const int want = (int)sizeof(SyncMsgV1) + m->count * (int)sizeof(SyncPlayerV1);
    if (n < want) { ++g_stats.dropped; return; }
    ++g_stats.recvd;

    const int me = (int)data_0209f250;
    const SyncPlayerV1 *e = (const SyncPlayerV1 *)(buf + sizeof(SyncMsgV1));
    for (int i = 0; i < (int)m->count; ++i, ++e) {
        const int slot = e->slot;
        if (slot < 0 || slot >= kCommsMaxPlayers) continue;
        if (slot == me) continue;               // NEVER the local body
        void *a = data_0209f394[slot];
        if (!a) continue;

        int *px = player::pos_x(a);
        int *py = player::pos_y(a);
        int *pz = player::pos_z(a);
        const int dx = e->x - *px, dy = e->y - *py, dz = e->z - *pz;
        int err = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy) + (dz < 0 ? -dz : dz);
        if (err > g_stats.worst_error) g_stats.worst_error = err;

        if (err < units(2)) {
            /* Below visible. Applying it would be jitter, not correction. */
            ++g_stats.applied;
            *player::facing(a) = e->yaw;
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
        ++g_stats.applied;
    }
}

void sync_tick() {
    if (!g_enabled) return;
    sync_apply();          // take the host's view first
    sync_send_if_host();   // then publish ours, after the input record
}


SyncStats sync_stats() { return g_stats; }

void sync_report(const char *tag) {
    parse_cfg();
    std::fprintf(stderr,
                 "[sync:%s] enabled=%s sent=%llu recvd=%llu dropped=%llu "
                 "applied=%llu lerps=%llu snaps=%llu worst_err=%d\n",
                 tag ? tag : "-", g_enabled ? "yes" : "no",
                 (unsigned long long)g_stats.sent,
                 (unsigned long long)g_stats.recvd,
                 (unsigned long long)g_stats.dropped,
                 (unsigned long long)g_stats.applied,
                 (unsigned long long)g_stats.lerps,
                 (unsigned long long)g_stats.snaps,
                 g_stats.worst_error);
}

}  // namespace port
