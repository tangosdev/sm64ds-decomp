# LAGDELAY: the input delay is measured now, and a starve names a slot

Branch `port/lagdelay`, worktree `C:\tmp\lagdelay`, base `ad09cd9f5`
(port-mount-noseat-cluster). Own build dir `C:\tmp\lagdelay\build`, own TEMP
`C:\tmp\lagdelaybld`, evidence `C:\tmp\lagdelay-out`. Nothing under `src/` is
touched: `git diff ad09cd9f5 -- src/` is empty.

## The complaint and the arithmetic under it

Seven-player online matches freeze. The field logs say why without needing a
theory:

    [comms:loopback] accepted as slot 1 ... input delay 5 (handshake rtt 172 ms)
    [comms:loopback] closed after 3032 rounds; indelay=5 starved=1976 ...
    [comms:loopback] closed after 56 rounds; indelay=5 starved=1249 ...

The relay's input delay was the constant 5. This seam's rounds are paced at
16.67 ms, so 5 frames is **83 ms of cover**, and it was chosen against one
measurement (p50 lockstep round trip 90.6 ms, p95 98.8 ms) on one desk. The
handshake round trips in the logs run **94, 118, 141 and 172 ms**. On the
slower half of that spread the pipeline is short of the path by design, and
`lb_exchange` takes its starve branch on most frames.

That is not a defect in the pipelining. It is an under-sized constant, and the
fix is to stop using a constant.

## 1. Adaptive input delay

**The formula.** The parent sizes the session once, from the worst round trip
among its children:

    N = ceil(worst_rtt_ms * safety / 16.67) + jitter_margin
    clamped to [floor, kInputDelayMax]

    safety        125 percent   SM64DS_COMMS_DELAY_SAFETY (100..400)
    jitter_margin 1 frame       SM64DS_COMMS_DELAY_MARGIN (0..8)
    floor         5 frames      SM64DS_COMMS_DELAY_FLOOR
    cap           15 frames     kInputDelayMax, raised from 8

Worked against the field numbers: 94 ms gives 9, 118 ms gives 10, 141 ms gives
12, 172 ms gives 14. The old ceiling of 8 could not express three of those
four, which is why it moved. `SM64DS_COMMS_INPUT_DELAY` still wins outright and
disarms the sizing entirely, so every existing measurement rig keeps measuring
what it set. `SM64DS_COMMS_ADAPTIVE_DELAY=0` disarms it without pinning a
number.

**Where the round trips come from.** The child already measured one
(`g_handshake_rtt_ms`, its JOIN to the parent's ACCEPT) and kept it to itself.
It now reports it: a JOIN with bit 30 set in `have`, carrying the round trip in
bits 0..15 and the delay the child is currently running in bits 16..23. `have`
on a JOIN has been zero on the wire since MP2, so this is not a wire change and
not a new packet type -- an older parent answers a report exactly as it answers
a re-knock, with a unicast ACCEPT, and is none the wiser.

The parent takes the **worse** of two samples of the same path: the number the
child reported, and its own accept-out to report-in measurement, which owes
nothing to the child's clock. One number has to cover the path and neither
sample is entitled to be the small one.

**Every peer still runs the same number.** That invariant is not weakened. It
is published through the ACCEPT field exactly as before (bits 8..15), and three
rules keep it true while the number is allowed to move at all:

1. **It only moves before frame 0.** `g_delay_frozen` goes true the first time
   the parent enters `lb_exchange` past its state checks, which is the frame the
   ROM's wait loop first asks for a round.
2. **A child only adopts a change before its own frame 0.** A peer that has
   handed the game no round cannot have handed it a round under the old N, so
   there is no frame index whose N disagrees. A change after that is REFUSED,
   loudly and rate-limited. This replaces the old "on the first accept only"
   bound, which was standing in for exactly this property.
3. **The parent will not serve round 0 until every child it retold has said
   back what it is running.** A child cannot produce a frame without the
   parent's aggregate, so withholding the aggregate withholds frame 0 from the
   whole session, and the parent's own exchange starves meanwhile -- an
   already-defined, already-counted, harmless state. Only children whose told
   number CHANGED after their accept are gated, so a session whose delay never
   moved behaves byte-identically to before.

**And it only engages when every live child has reported.** A peer of an older
generation never sends a report, so it would never adopt a raised number and
never ack one. Requiring a report from everybody turns the sizing off in
exactly the sessions where it would be unsafe, and those keep the constant they
always had.

**The one line in the playlog.**

    [comms:loopback] adaptive input delay 14 frame(s) (was 5): ceil(172 ms
    worst * 125% / 16.67 ms) + 1, floor 5 cap 15; worst is slot 3; per-child
    rtt s1=94ms s2=118ms s3=172ms [report]

## 2. Starvation attribution

`starved` said the session was short of cover. It did not say whose path was
short, and in a seven-player match that is the whole question: one peer starves
every frame for all six others and the close line named nobody.

`g_starve_by_slot[]` now counts, per slot, every round the frame wanted and
that slot's record was missing from. The close summary carries it:

    [comms:loopback] closed after 3032 rounds; indelay=5 starved=1976 sent=...
    resends=... starvedby=3:1974,5:12 lastround=3027

Omitted entirely when nothing starved, so a healthy session's line is
unchanged. The per-event line naming the missing slots is rate limited to one a
second, because on a path the delay does not cover a starve is a per-frame
event and a line each would be the flood rather than the diagnosis.

## 3. Late-peer mercy: proposed, NOT implemented, and why

**(a) Mid-session renegotiation at a safe boundary. Refused, and the reason is
the ROM.** A scheduled switch ("at round R, N becomes M") published in advance
does keep the number identical on every peer at every round -- the schedule is
the same everywhere -- so it passes the test as written. It still must not
ship. Raising N from 5 to 10 at frame k moves `want` from k-5 to k-10, so the
console re-consumes rounds it has already simulated; lowering it skips rounds
outright. Both peers do the same thing, so it is not a divergence between them,
but each block carries the ROM's own per-player frame counter and
`src/func_0203ea5c.c:418` compares every live player's against the local one's,
setting error bit 2 when they differ. Re-serving an old block re-serves an old
counter. The safe boundary is not a round boundary at all: it is a point where
the ROM's record stream restarts, and this seam does not have one.

**(b) Letting a consistently late peer run a deeper local delay.** That is the
invariant break stated directly. Refused.

**(c) What is actually cheap, and is proposed for the lobby lane rather than
this file.** The parent now knows, per slot, who is starving the session. A
peer whose starve share dominates can be surfaced in the lobby so the humans
decide -- and a rematch is already a fresh sizing, because the freeze is per
session and the next match re-derives N from that peer's current path. The
mercy for a late peer today is "the next match sizes for them", which costs
nothing and is already true as of this change.

## What changed, file and line

All in `port/hal/comms_loopback.cpp` unless noted. Line numbers are at
`port/lagdelay` tip.

| Where | What |
|---|---|
| :727 | `kInputDelayMax` 8 -> 15, with a `static_assert` tying it to `kPipeDepth` |
| :737 | `kRoundPeriodMs100 = 1667`, the 16.67 ms the formula divides by |
| :766 | starve attribution state: `g_starve_by_slot[]`, last round, rate limiter |
| :781 | the adaptive-delay banner and its three rules |
| :823 | its state: per-child rtt, told, ack, moved, frozen, frames-produced |
| :858 | `delay_state_reset()`, called from `lb_open` and `lb_become_child` |
| :1246 | `announce_roster` records that every live child was retold |
| :1263 | `recompute_adaptive_delay()` -- the formula, the one log line, the publish |
| :1311 | `delay_gate_open()` -- rule 3 as one predicate |
| :1465 | `pipe_try_broadcast` holds round 0 behind the gate, re-announces, logs |
| :1533 | the parent's JOIN arm reads a report and measures the path itself |
| :1625 | the unicast ACCEPT carries the rtt ack (bit 16) and stamps the send |
| :1651 | the sizing runs after the accept went out |
| :1717 | `child_send_report()` -- the report JOIN |
| :1738 | `child_adopt_delay()` -- rule 2, adopt before frame 0 or refuse loudly |
| :1883 | the connected child reads the ack and a re-sized delay from an ACCEPT |
| :2287 | `service()` repeats an unacked report, floored at 50 ms, capped at 12 |
| :2604 | the close summary carries `starvedby=` |
| :2705 | `lb_exchange` gives the sizing one round trip (400 ms ceiling) |
| :2748 | and freezes the depth above the path split |
| :2819 | the starve branch: per-slot counters and the rate-limited line |
| :2858, :2911 | `g_frames_produced` on both success paths |
| :3437 | the sizing's env knobs |
| :3481 | its arming rule (parent, not env-pinned, not loopback, pipeline on) |
| `port/tools/vs16_ladder.sh:20` | `VS16_ROOT`, `VS16_OUT`, `VS16_ASSET_ROOT` so a lane can run the ladder against its own tree |

## Proofs

All from `C:\tmp\lagdelay`, all quiet and muted (`mp2_proof.env_base`:
CREATE_NO_WINDOW, SW_SHOWMINNOACTIVE, `SM64DS_NO_FOCUS`, `SM64DS_MINIMIZED`,
`SM64DS_VOLUME=0`). No window was ever shown. Build sha
`7E837CDA1317F41A` (`build/port/walk_window.exe`), built by
`port/build-port.cmd` with zero errors (`C:\tmp\lagdelay-out\build5.log`).

### P1. The sizing works and it drops starves (`C:\tmp\lagdelay-out\proof_ab.log`)

RELAY mode over a local reference relay with 50 ms induced one way (round trip
about 100 ms) -- the mode with the problem, not a loopback. Two arms of one rig,
900 frames each. Arm A pins the shipped constant, arm B lets the parent size it.

    exe   C:/tmp/lagdelay\build\port\walk_window.exe  sha 7E837CDA1317F41A
    rig   RELAY via local reference relay on 127.0.0.1:59172, one-way 50 ms induced (round trip ~100 ms), 900 frames
    ==========================================================================
    ARM A_baseline_delay5   rc parent/child = (0, 0)
      parent [comms:loopback] closed after 905 rounds; indelay=5 starved=537 sent=969 recvd=945 resends=0 starvedby=1:537 lastround=898
      child  [comms:loopback] closed after 900 rounds; indelay=5 starved=485 sent=947 recvd=960 resends=40 starvedby=0:485 lastround=891
    ==========================================================================
    ARM B_adaptive   rc parent/child = (0, 0)
      parent [comms:loopback] closed after 903 rounds; indelay=11 starved=57 sent=944 recvd=921 resends=0 starvedby=1:57 lastround=856
      child  [comms:loopback] closed after 900 rounds; indelay=11 starved=79 sent=923 recvd=934 resends=16 starvedby=0:79 lastround=875
    ==========================================================================
    VERDICT  A baseline  indelay=(5, 5)  starved=(537, 485)
             B adaptive  indelay=(11, 11)  starved=(57, 79)
             PASS: the sized depth is larger than the constant, identical on both peers, and starves less

**Starves fall from 537/485 to 57/79, about 89 percent, on the same wire.** The
whole mechanism is visible in arm B's parent log, in order:

    [comms:loopback] adaptive input delay ARMED: this end is the parent and will size the session's depth from the worst round trip its children report (safety 125%, margin 1, floor 5, cap 15). It is frozen before the first round and published in every ACCEPT, so every peer still runs one number.
    [comms:loopback] slot 1 reports handshake rtt 125 ms (this end saw 688 ms since its last accept to that slot, a cross check and not the sample) and runs input delay 5
    [comms:loopback] adaptive input delay 11 frame(s) (was 5): ceil(125 ms worst * 125% / 16.67 ms) + 1, floor 5 cap 15; worst is slot 1; per-child rtt s1=125ms [report]
    [comms:loopback] holding round 0: input delay is 11 and slot(s) 1(runs 5) have not confirmed it. The roster announce repeats; this clears itself in one round trip, and holding is a stall where serving would be a desync. Held 0ms.
    [comms:loopback] input delay FROZEN at 11 for the rest of this session; the ROM asked for its first round
    [comms:loopback] every peer has confirmed input delay 11; the session runs (held 156ms, 111 turns)

and the child's side of the same three seconds:

    [comms:loopback] accepted as slot 1 at round 0; live mask 0x3, players 2, input delay 5 (handshake rtt 125 ms)
    [comms:loopback] the parent runs input delay 11 and this end had 5; ADOPTING 11 (the parent re-sized the session). The parent is authoritative because the two ends must run the same depth -- frame k reads round k-N on both consoles or they are not simulating the same match.

Round 0 was held for 156 ms while the gate waited for the child to confirm, and
`starvedby=` names the peer on both sides.

### P2. The stall ladder (`C:\tmp\lagdelay-out\proof_stall.log`)

    PASS  s1_sync: session survives a 2.5s peer stall -- stall 2.51s rounds 31->154 dead_p=False dead_c=False p1='[comms:level] transport=loopback (udp 127.0.0.1) link=3 connected=yes slot=0 players=2 role=1 boot=0x0000 exchanges=2496 rounds=900'
    PASS  s2_nosync: session survives a 2.5s peer stall -- stall 2.58s rounds 31->152 dead_p=False dead_c=False p1='[comms:level] transport=loopback (udp 127.0.0.1) link=3 connected=yes slot=0 players=2 role=1 boot=0x0000 exchanges=1923 rounds=900'
    PASS  s3: a killed peer still ends the session by the ROM's own bound (solo after 30.6 s, window 45s)
    PASS  s3: the sync liveness gate held on the dead session (gated=866, transition line present)
    stall ladder: ALL GREEN

### P3. net_proof, seven rungs (`C:\tmp\lagdelay-out\proof_net.log`)

    net_proof: exe 7E837CDA1317F41A  ports 59408.. relay 59440

    === N0 ===
    PASS  rungN0 SOLO IS BYTE-IDENTICAL | pos=(-4915200, 2929633, 11141348), expected (-4915200, 2929633, 11141348). The address work is in the link; with none of its env set it may not move the game one unit.
    PASS  rungN0 and the carrier is SILENT when unasked | no [loopback:] or [comms:relay] line in a 300-frame solo run

    === N1 ===
    PASS  rungN1 LOOPBACK SESSION STILL FORMS | live=0x3 both, rounds 600/600
    PASS  rungN1 and it is still LOOPBACK MODE | modes=['loopback', 'loopback'] (the new code paths must not activate without their env)
          loopback: 600 rounds over 25.5s of process wall time

    === N2 ===
          direct mode over 192.168.1.240:59408
    PASS  rungN2 DIRECT SESSION OVER THE LAN ADDRESS | live=0x3 both, rounds 600/600
    PASS  rungN2 and both ends are in DIRECT mode | modes=['direct', 'direct']
    PASS  rungN2 and the parent LEARNED the child's address off the wire | expected 'direct: learned slot 1 at 192.168.1.240' in the parent log (the loopback port-arithmetic rule cannot produce this)
          direct: 600 rounds over 19.8s of process wall time

    === N3 ===
          local reference relay on 127.0.0.1:59440
    PASS  rungN3 RELAY SESSION FORMS | live=0x3 both, rounds 600/600
    PASS  rungN3 and both ends are in RELAY mode | modes=['relay', 'relay']
    PASS  rungN3 and both ends PAIRED with the relay | paired=[True, True] (a status-0 HELLO-ACK landed on each)
    PASS  rungN3 and NEITHER END KNEW THE OTHER'S ADDRESS | the parent never learned a peer address; every byte went through the relay
          relay: 600 rounds over 22.4s of process wall time

    === N4 ===
          RTT   0 ms:   600 rounds  wall   41.2s   1.00x the zero-latency arm  session ok
    PASS  rungN4 RTT 0 ms still forms a session | live=0x3 both, rounds 600/600
    PASS  rungN4 RTT 0 ms delay ring never overflowed | delayovf=[0, 0] (a nonzero value means the induction dropped datagrams and the number above is not a latency measurement)
          RTT  40 ms:   600 rounds  wall   78.0s   1.89x the zero-latency arm  session ok
    PASS  rungN4 RTT 40 ms still forms a session | live=0x3 both, rounds 600/600
    PASS  rungN4 RTT 40 ms delay ring never overflowed | delayovf=[0, 0] (a nonzero value means the induction dropped datagrams and the number above is not a latency measurement)
          RTT  80 ms:   600 rounds  wall   96.6s   2.34x the zero-latency arm  session ok
    PASS  rungN4 RTT 80 ms still forms a session | live=0x3 both, rounds 600/600
    PASS  rungN4 RTT 80 ms delay ring never overflowed | delayovf=[0, 0] (a nonzero value means the induction dropped datagrams and the number above is not a latency measurement)
          RTT 120 ms:   600 rounds  wall  115.7s   2.81x the zero-latency arm  session ok
    PASS  rungN4 RTT 120 ms still forms a session | live=0x3 both, rounds 600/600
    PASS  rungN4 RTT 120 ms delay ring never overflowed | delayovf=[0, 0] (a nonzero value means the induction dropped datagrams and the number above is not a latency measurement)

    === N5 ===
          80 ms + 5% loss + 10 ms jitter: 600 rounds, wall 104.0s, 2.26x the clean-wire arm
    PASS  rungN5 SURVIVES 80 ms + 5% LOSS | live=0x3 both, rounds 600/600
    PASS  rungN5 delay ring never overflowed | delayovf=[0, 0]

    === N7 ===
    PASS  rungN7 RTT 80 ms the input-delay knob ACTUALLY TOOK | indelay=[3, 3], wanted [3, 3]. Anything else means the comparison below is measuring two identical arms.
          RTT  80 ms: stop-and-wait  112.5s  ->  input delay 3   59.9s   1.88x   starved=[300, 352]
    PASS  rungN7 RTT 80 ms forms a session both ways | off: live=0x3 both, rounds 600/600 | on: live=0x3 both, rounds 600/600
    PASS  rungN7 RTT 80 ms INPUT DELAY 3 BUYS THE PACE BACK | 112.5s -> 59.9s, 1.88x. Under 1.5x is not a mitigation worth the input lag it costs.
    PASS  rungN7 RTT 120 ms the input-delay knob ACTUALLY TOOK | indelay=[4, 4], wanted [4, 4]. Anything else means the comparison below is measuring two identical arms.
          RTT 120 ms: stop-and-wait  139.9s  ->  input delay 4   33.2s   4.21x   starved=[307, 304]
    PASS  rungN7 RTT 120 ms forms a session both ways | off: live=0x3 both, rounds 600/600 | on: live=0x3 both, rounds 600/600
    PASS  rungN7 RTT 120 ms INPUT DELAY 4 BUYS THE PACE BACK | 139.9s -> 33.2s, 4.21x. Under 1.5x is not a mitigation worth the input lag it costs.

    ALL GREEN

N7 is the rung that made this lane revert its first arming rule. Its
stop-and-wait baseline arm is an induced-latency LOOPBACK with no input-delay
env, and an earlier draft sized that arm -- which would have turned the
comparison into two identical arms while every assertion still passed.

### P4. The wide session at a depth the old cap could not express

`port/tools/vs16_ladder.sh` at seven windows, one arena, with the depth forced
to **10** (`SM64DS_COMMS_INPUT_DELAY=10`), past the old ceiling of 8, and 15 ms
of induced one way so the carrier accepts a depth at all.

Every window ran it, which is the cap-raise proof:

    p0 [comms:loopback] closed after 909 rounds; indelay=10 starved=101 sent=57711 recvd=15402 resends=0 starvedby=1:4,2:10,3:42,4:3,5:39,6:17 lastround=720
    p1 [comms:loopback] closed after 900 rounds; indelay=10 starved=89 ... starvedby=0:89,2:89,3:89,4:89,5:89,6:89 lastround=858
    p2 [comms:loopback] closed after 900 rounds; indelay=10 starved=97 ... starvedby=0:97,1:97,3:97,4:97,5:97,6:97 lastround=885
    p3 [comms:loopback] closed after 900 rounds; indelay=10 starved=55 ... starvedby=0:55,1:55,2:55,4:55,5:55,6:55 lastround=699
    p4 [comms:loopback] closed after 900 rounds; indelay=10 starved=89 ... starvedby=0:89,1:89,2:89,3:89,5:89,6:89 lastround=888
    p5 [comms:loopback] closed after 900 rounds; indelay=10 starved=68 ... starvedby=0:68,1:68,2:68,3:68,4:68,6:68 lastround=851
    p6 [comms:loopback] closed after 900 rounds; indelay=10 starved=114 ... starvedby=0:114,1:114,2:114,3:114,4:114,5:114 lastround=883

The parent's line is the diagnostic one: `starvedby=1:4,2:10,3:42,4:3,5:39,6:17`
names slots 3 and 5 as the peers that cost this session most of its stalls. A
CHILD's counts are necessarily flat (`0:89,2:89,...`) because a child receives
the whole aggregate or nothing, so it can only ever say "everyone but me". That
is a real limit of the attribution and it is written down rather than dressed up.

**The pairwise world-hash sweep at depth 10, 1500 frames per child, all 21
pairings** (`C:\tmp\lagdelay-out\proof_vs7_d10b.log`, sweep run with
`C:\tmp\lagdelay-out\dhsweep.sh`):

    pairwise dhdiff over 7 windows in C:/tmp/lagdelay-out/vs16d10b/rung7
    p0 vs p1  NO DIVERGENCE
    p0 vs p2  NO DIVERGENCE
    p0 vs p3  NO DIVERGENCE
    p0 vs p4  NO DIVERGENCE
    p0 vs p5  NO DIVERGENCE
    p0 vs p6  NO DIVERGENCE
    p1 vs p2  NO DIVERGENCE
    p1 vs p3  NO DIVERGENCE
    p1 vs p4  NO DIVERGENCE
    p1 vs p5  NO DIVERGENCE
    p1 vs p6  NO DIVERGENCE
    p2 vs p3  NO DIVERGENCE
    p2 vs p4  NO DIVERGENCE
    p2 vs p5  NO DIVERGENCE
    p2 vs p6  NO DIVERGENCE
    p3 vs p4  NO DIVERGENCE
    p3 vs p5  NO DIVERGENCE
    p3 vs p6  NO DIVERGENCE
    p4 vs p5  NO DIVERGENCE
    p4 vs p6  NO DIVERGENCE
    p5 vs p6  NO DIVERGENCE
    sweep fail=0

and the bare-loopback control at seven windows (no induced latency, so the
carrier refuses a depth and the session is stop-and-wait) is also 21/21 clean.

### P6. The battery: GREEN AS FAR AS IT GOT, NOT FINISHED

`python port/tools/battery.py C:/tmp/lagdelay --skip-build`, log at
`C:\tmp\lagdelay-out\proof_battery.log`. Every smoke binary green:

    smoke.exe: ok  smoke: all checks passed (math, Timer, Fader on host)
    smoke_actor.exe: ok  smoke_actor: all checks passed (an actor spawned, initialized, behaved and rendered throug
    smoke_anim.exe: ok  smoke_anim: all checks passed (the game posed and rendered the piano via its own recursive
    smoke_clsn.exe: ok  smoke_clsn: all checks passed (the game's octree walk answers ground queries over real KCL
    smoke_frames.exe: ok  smoke_frames: all checks passed (24 game-shaped frames through the fiber runtime, all with
    smoke_fs.exe: ok  smoke_fs: all checks passed (raw + LZ77 assets loaded through SharedFilePtr on the game he
    smoke_gx.exe: ok  smoke_gx: all checks passed (game DL pump byte-equals the harness path, 492 triangles, 186
    smoke_heap.exe: ok  smoke_heap: all checks passed (2504 allocs, 2496 frees, peak 63 live, 0 full-arena rejecti
    smoke_model.exe: ok  smoke_model: all checks passed (the game loaded, rebased, uploaded and rendered its own mo
    smoke_modelanim.exe: ok  smoke_modelanim: all checks passed (the game advanced, wrapped and re-posed its own animat
    smoke_oam.exe: ok  smoke_oam: all checks passed (the game's sprite engine emits, uploads and scans out on hos
    smoke_objwin.exe: ok  smoke_objwin: ok
    smoke_persist.exe: ok  smoke_persist: all checks passed (wrote disk state, a second process loaded it byte-exact;
    smoke_player.exe: ok  smoke_player: Mario walks on the castle grounds (gates 10+11 GREEN)
    smoke_roots.exe: ok  smoke_roots: all checks passed (root heap up, 1511 allocs, 1489 frees through Memory::Allo
    smoke_savestate.exe: ok  smoke_savestate: all checks passed (world evolved, saved, diverged, restored byte-exact, a
    smoke_sdat.exe: ok  OK
    smoke_soak.exe: ok  soak: 455 models, 455 rendered (89840 tris total), 0 empty, 0 load-fail, 0 faulted
    smoke_soak_anim.exe: ok  anim soak: 473 compatible pairs, 473 animated+rendered, 0 empty, 0 load-fail, 0 faulted
    levels: 50 mounted, from hal/level_boot.cpp
    selftest level 0: ok
    selftest level 1: ok
    selftest level 2: ok

**IT IS NOT AN ALL GREEN AND MUST NOT BE READ AS ONE.** Levels 3..49, every
scene selftest, the default-boot arm, linkage, ptr_audit and the shipping
configuration have not run. The desk was carrying seven other lanes' port
builds while this ran and the level arm was moving at about seven minutes a
level, which is roughly six hours for the rest. The run was left going rather
than killed; whoever picks this up should read the log's own verdict line and
not this section.

Nothing in this change is level-shaped or scene-shaped -- the diff is one
transport file plus a shell script -- but that is an argument, and the battery
is the thing that would actually settle it.

### P5. THE ONE THING THAT DID NOT COME OUT CLEAN

At a **900**-frame child budget with depth 10 and 15 ms induced one way, three
of four runs showed a divergence **in the last one to four frames**, in one or
two player actors (`actorID 191`), while three runs of the same rig at depth 5
were clean. Every observation:

| run | depth | frames | result |
|---|---|---|---|
| vs16d10  | 10 | 900  | first divergence f896, uid 3 and 6 |
| vs16d10c | 10 | 900  | first divergence f899, uid 7 |
| vs16d10d | 10 | 900  | first divergence f896, uid 3 |
| vs16d10b | 10 | 1500 | 21/21 NO DIVERGENCE, all 1500 frames |
| vs16d5, d5r1, d5r2 | 5 | 900 | 21/21 NO DIVERGENCE, three times |

Two facts narrow it down. First, the disagreeing peers hold **each other's**
hash values rather than novel ones -- `p1=b0bbf610 p2=b2366ed3` in one pairing
and exactly those two swapped in another -- which is two consoles at different
points on the SAME timeline, not two consoles simulating different inputs.
Second, deleting the last ten frames from the very logs that failed and
re-running the identical sweep is clean:

    pairwise dhdiff over 7 windows in C:/tmp/lagdelay-out/trim
    p0 vs p1  NO DIVERGENCE
    ... (all 21) ...
    sweep fail=0

So the disagreement is confined to the frames where the harness takes its last
sample while peers sit a round apart, and a deeper pipeline makes that straddle
likelier because it puts more distance between the frame being drawn and the
round being read. **That is the reading, not a proof.** It is not demonstrated
that no simulation-visible divergence hides underneath, and this should be
closed -- with a run whose windows are stopped at an agreed round rather than an
independent frame budget each -- before anything ships at a raised depth.

## Honest remaining gaps

1. **The final-frame divergence at depth 10 is explained, not proven.** P5 has
   the whole record. The reading is a harness sampling straddle; it is not
   demonstrated that nothing simulation-visible hides under it. Close it with a
   wide run whose windows stop at an agreed ROUND rather than at an independent
   per-window frame budget.

2. **One handshake sample per child, and it never updates.** The round trip the
   depth is sized from is measured once, during the join, and the number is
   frozen before frame 0. A path that is fine at join and bad ten minutes later
   is not re-sized -- it starves, and now says whose path it was. There is no
   running RTT estimate on this carrier (the aux layer has a SYNP/SYNQ probe
   pair, unused here).

3. **The cap is 15 frames, which is 250 ms.** A path worse than that still
   starves at the cap and always will; the log says so plainly rather than
   pretending. Raising it further trades input lag a player can feel for stalls
   a player can feel, and that is an owner's call, not this lane's.

4. **A child's starve attribution is flat by construction.** A child receives
   the parent's whole aggregate or nothing, so its per-slot counts are all the
   same number and it can only ever say "everyone but me". The PARENT's line is
   the diagnostic one. A field report should quote the parent.

5. **Twelve lost reports in a row wedges the start.** If a child's rtt report is
   lost `kMaxReportTries` times AND the parent had already retold that child a
   changed depth, the ack gate never opens and the parent holds round 0 until
   the ROM's own ~20 s bound ends the session. Bounded and loud, but it ends the
   session rather than degrading. It needs a loss rate that would ruin the match
   anyway, and it has not been reproduced.

6. **Nothing here ran over the live relay.** P1 uses the local reference relay
   on this desk; net_proof N6 (the deployed VPS) was not run. The induced delay
   is a carrier-side send delay, not a real internet path.

7. **The sizing is off for any session with a peer of an older generation**, by
   design (rule "every live child, or nothing"). Those sessions keep the shipped
   constant and the whole complaint with it. There is no version handshake that
   would let the parent tell "old build" from "still joining" faster than the
   400 ms grace.

8. **Late joiners after the freeze get the frozen number.** In real play the
   lobby gates the start so everyone is in first, but `vs16_ladder`-style
   staggered starts can seat a two-player world and freeze before slots 3..6
   arrive. Those peers are told the frozen depth and cannot change it, which is
   correct and is also not sized for them.

9. **The mid-session renegotiation in section 3 is analysis, not code.** The
   argument against it rests on reading `src/func_0203ea5c.c:418`, not on a run
   that tried it and broke.
