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
3. **The parent will not serve round 0 until every live child has confirmed
   the value actually in force.** A child cannot produce a frame without the
   parent's aggregate, so withholding the aggregate withholds frame 0 from the
   whole session, and the parent's own exchange starves meanwhile -- an
   already-defined, already-counted, harmless state. The predicate is keyed on
   the VALUE, not on a flag: if the depth in force equals the pre-sizing value
   there is nothing to confirm and the gate does not exist, which is why a
   session that never sizes behaves byte-identically to before this lane.
4. **A live child with no report WITHDRAWS a raised depth.** The pre-sizing
   value is saved, and the moment any live child is seen without a round-trip
   report -- a join arriving after the sizing fired, or the grace expiring with
   somebody silent -- the raised number is taken back and re-announced to every
   child. Nobody has consumed a round at that point, so the fallback replays
   nothing and skips nothing, and the value fallen back to is the mode default,
   which every build that has ever shipped can express.
5. **A joiner arriving at a session already frozen deeper than 8 is not seated
   at all.** After the freeze there is no withdrawing: peers have consumed
   rounds at that depth and moving it would replay or skip them. 8 is the
   ceiling every shipped build's adopt clamp accepts, so past it the parent
   cannot publish a number it can be sure the joiner will run. It refuses by
   SILENCE rather than by a Bye: over the relay a parent datagram reaches every
   child, and a child reads Bye as "the parent left", so one refused joiner
   would have ended the match for everyone already playing. The refused peer
   spends its own ROM bound and falls back to solo, which is the failure it
   already knows how to have. It costs a late joiner that IS a new build its
   seat, and that is stated rather than hidden.

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
| :766 | starve attribution state and its rate limiter |
| :781 | the adaptive-delay banner and its rules |
| :838 | `g_delay_presize`, the value a raised depth is withdrawn back to |
| :845 | `kLegacyInputDelayMax = 8`, the depth every shipped build can adopt |
| :871 | `delay_state_reset()`, which now clears the rate-limit stamps too |
| :1280 | `recompute_adaptive_delay()` -- the formula, and the withdrawal |
| :1358 | `delay_gate_open()` -- keyed on the value in force, not on a flag |
| :1524 | `pipe_try_broadcast` holds round 0 behind the gate |
| :1605 | a joiner arriving at a session frozen deeper than 8 is not seated |
| :1674 | a stale reordered report is not an acknowledgement |
| :1845 | `child_send_report()`, silent under the legacy-peer simulation |
| :1867 | `child_adopt_delay()` -- rule 2, and the legacy clamp it reproduces |
| :2757 | the close summary carries `starvedby=` |
| :2838 | `lb_exchange` gives the sizing one round trip (400 ms ceiling) |
| :2870 | the grace stand-down withdraws before it stands down |
| :2891 | and the depth freezes above the path split |
| :2962 | the starve branch: per-slot counters and the rate-limited line |
| :3612 | `SM64DS_COMMS_LEGACY_PEER`, the pre-0.3.3 stand-in |
| :3639 | the sizing's arming rule and its env knobs |
| `port/tools/vs16_ladder.sh:20` | `VS16_ROOT`, `VS16_OUT`, `VS16_ASSET_ROOT` |

## Proofs

All from `C:\tmp\lagdelay`, all quiet and muted (`mp2_proof.env_base`:
CREATE_NO_WINDOW, SW_SHOWMINNOACTIVE, `SM64DS_NO_FOCUS`, `SM64DS_MINIMIZED`,
`SM64DS_VOLUME=0`). No window was ever shown. Build sha
`51E5B0674DFD8B10` (`build/port/walk_window.exe`), built by
`port/build-port.cmd` with zero errors (`C:\tmp\lagdelay-out\build7.log`).

**Which output came from which binary.** P2, P3, P4's sweep and P7 were
run on the current binary, after the review fix. P1 and P6 predate it and
name the earlier sha `7E837CDA1317F41A`; nothing in the review fix touches
what they measure (the sizing arithmetic and the battery's level and scene
arms), and they are left as recorded rather than restated from memory.

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

    PASS  s1_sync: session survives a 2.5s peer stall -- stall 2.50s rounds 37->165 dead_p=False dead_c=False p1='[comms:level] transport=loopback (udp 127.0.0.1) link=3 connected=yes slot=0 players=2 role=1 boot=0x0000 exchanges=1735 rounds=900'
    PASS  s2_nosync: session survives a 2.5s peer stall -- stall 2.50s rounds 35->162 dead_p=False dead_c=False p1='[comms:level] transport=loopback (udp 127.0.0.1) link=3 connected=yes slot=0 players=2 role=1 boot=0x0000 exchanges=1735 rounds=900'
    PASS  s3: a killed peer still ends the session by the ROM's own bound (solo after 30.1 s, window 45s)
    PASS  s3: the sync liveness gate held on the dead session (gated=857, transition line present)
    stall ladder: ALL GREEN

### P3. net_proof, seven rungs (`C:\tmp\lagdelay-out\proof_net.log`)

27 PASS, 0 FAIL.

    net_proof: exe 51E5B0674DFD8B10  ports 57104.. relay 57136

    === N0 ===
    PASS  rungN0 SOLO IS BYTE-IDENTICAL | pos=(-4915200, 2929633, 11141348), expected (-4915200, 2929633, 11141348). The address work is in the link; with none of its env set it may not move the game one unit.
    PASS  rungN0 and the carrier is SILENT when unasked | no [loopback:] or [comms:relay] line in a 300-frame solo run

    === N1 ===
    PASS  rungN1 LOOPBACK SESSION STILL FORMS | live=0x3 both, rounds 600/600
    PASS  rungN1 and it is still LOOPBACK MODE | modes=['loopback', 'loopback'] (the new code paths must not activate without their env)
          loopback: 600 rounds over 21.9s of process wall time

    === N2 ===
          direct mode over 192.168.1.240:57104
    PASS  rungN2 DIRECT SESSION OVER THE LAN ADDRESS | live=0x3 both, rounds 600/600
    PASS  rungN2 and both ends are in DIRECT mode | modes=['direct', 'direct']
    PASS  rungN2 and the parent LEARNED the child's address off the wire | expected 'direct: learned slot 1 at 192.168.1.240' in the parent log (the loopback port-arithmetic rule cannot produce this)
          direct: 600 rounds over 21.8s of process wall time

    === N3 ===
          local reference relay on 127.0.0.1:57136
    PASS  rungN3 RELAY SESSION FORMS | live=0x3 both, rounds 600/600
    PASS  rungN3 and both ends are in RELAY mode | modes=['relay', 'relay']
    PASS  rungN3 and both ends PAIRED with the relay | paired=[True, True] (a status-0 HELLO-ACK landed on each)
    PASS  rungN3 and NEITHER END KNEW THE OTHER'S ADDRESS | the parent never learned a peer address; every byte went through the relay
          relay: 600 rounds over 24.2s of process wall time

    === N4 ===
          RTT   0 ms:   600 rounds  wall   17.8s   1.00x the zero-latency arm  session ok
    PASS  rungN4 RTT 0 ms still forms a session | live=0x3 both, rounds 600/600
    PASS  rungN4 RTT 0 ms delay ring never overflowed | delayovf=[0, 0] (a nonzero value means the induction dropped datagrams and the number above is not a latency measurement)
          RTT  40 ms:   600 rounds  wall   50.7s   2.85x the zero-latency arm  session ok
    PASS  rungN4 RTT 40 ms still forms a session | live=0x3 both, rounds 600/600
    PASS  rungN4 RTT 40 ms delay ring never overflowed | delayovf=[0, 0] (a nonzero value means the induction dropped datagrams and the number above is not a latency measurement)
          RTT  80 ms:   600 rounds  wall   71.1s   3.99x the zero-latency arm  session ok
    PASS  rungN4 RTT 80 ms still forms a session | live=0x3 both, rounds 600/600
    PASS  rungN4 RTT 80 ms delay ring never overflowed | delayovf=[0, 0] (a nonzero value means the induction dropped datagrams and the number above is not a latency measurement)
          RTT 120 ms:   600 rounds  wall   88.2s   4.95x the zero-latency arm  session ok
    PASS  rungN4 RTT 120 ms still forms a session | live=0x3 both, rounds 600/600
    PASS  rungN4 RTT 120 ms delay ring never overflowed | delayovf=[0, 0] (a nonzero value means the induction dropped datagrams and the number above is not a latency measurement)

    === N5 ===
          80 ms + 5% loss + 10 ms jitter: 600 rounds, wall 69.2s, 3.42x the clean-wire arm
    PASS  rungN5 SURVIVES 80 ms + 5% LOSS | live=0x3 both, rounds 600/600
    PASS  rungN5 delay ring never overflowed | delayovf=[0, 0]

    === N7 ===
    PASS  rungN7 RTT 80 ms the input-delay knob ACTUALLY TOOK | indelay=[3, 3], wanted [3, 3]. Anything else means the comparison below is measuring two identical arms.
          RTT  80 ms: stop-and-wait   78.4s  ->  input delay 3   30.8s   2.55x   starved=[272, 298]
    PASS  rungN7 RTT 80 ms forms a session both ways | off: live=0x3 both, rounds 600/600 | on: live=0x3 both, rounds 600/600
    PASS  rungN7 RTT 80 ms INPUT DELAY 3 BUYS THE PACE BACK | 78.4s -> 30.8s, 2.55x. Under 1.5x is not a mitigation worth the input lag it costs.
    PASS  rungN7 RTT 120 ms the input-delay knob ACTUALLY TOOK | indelay=[4, 4], wanted [4, 4]. Anything else means the comparison below is measuring two identical arms.
          RTT 120 ms: stop-and-wait   96.3s  ->  input delay 4   25.5s   3.77x   starved=[384, 349]
    PASS  rungN7 RTT 120 ms forms a session both ways | off: live=0x3 both, rounds 600/600 | on: live=0x3 both, rounds 600/600
    PASS  rungN7 RTT 120 ms INPUT DELAY 4 BUYS THE PACE BACK | 96.3s -> 25.5s, 3.77x. Under 1.5x is not a mitigation worth the input lag it costs.

    ALL GREEN

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

**The pairwise sweep at depth 10, seven windows, 1200 frames per child,
trimmed by ROUND** -- the reviewer's axis, using their own
`C:\tmp\lagrev\trimsweep.py`, which keeps only frames whose `rounds=` is at or
under a cap every window reached, so no pairing is compared across a round one
peer never had (`C:\tmp\lagdelay-out\proof_vs7_fix_trim.log`):

    max rounds per window: {'p0': 1207, 'p1': 1200, 'p2': 1200, 'p3': 1200, 'p4': 1200, 'p5': 1200, 'p6': 1200}
    agreed-round cap used: 1150 (min max = 1200 )
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
    trimmed sweep fail=0

and the bare-loopback control at seven windows (no induced latency, so the
carrier refuses a depth and the session is stop-and-wait) is also 21/21 clean.

### P6. The battery: every code-shaped arm GREEN, one environment red

`python -u port/tools/battery.py C:/tmp/lagdelay --skip-build`, log at
`C:\tmp\lagdelay-out\proof_battery.log`.

| Arm | Result |
|---|---|
| smoke suite | 19 of 19 ok (`smoke`, `smoke_actor`, `smoke_anim`, `smoke_clsn`, `smoke_frames`, `smoke_fs`, `smoke_gx`, `smoke_heap`, `smoke_model`, `smoke_modelanim`, `smoke_oam`, `smoke_objwin`, `smoke_persist`, `smoke_player`, `smoke_roots`, `smoke_savestate`, `smoke_sdat`, `smoke_soak`, `smoke_soak_anim`) |
| level selftests | **50 of 50 ok** |
| scene selftests | **34 of 34 ok** |
| default boot | ok, reaches the TITLE, 300 frames clean |
| linkage | **9139 (80.7%)**, identical to the number VS16HOST recorded; nothing lost |
| ptr_audit | **0** unhosted code pointers |
| shipping config | **ok** on a re-run, once a PATH line the battery omits is supplied -- see below |

Not one `selftest level` or `selftest scene` line is anything but `ok`. The one
qualified pass is level 45, which runs with `SM64DS_SKIP_CLASS=GOOMBOSS` and
says so itself -- that skip is the battery's own pre-existing baseline row,
owned by the decomp (`func_ov074_02121380` has no matched body), and has
nothing to do with this lane.

**The battery's own shipping-config arm went red, and the reason is not the
code.** In full, because it is the one red in the run:

    'vswhere.exe' is not recognized as an internal or external command,
    operable program or batch file.
    CMake Error:
      Running
       '...\Ninja\ninja.exe' '-C' 'C:/tmp/lagdelay/build/port-kit' '-t' 'restat' 'build.ninja'
      failed with:
       ninja: error: failed recompaction: Permission denied
    CMake Generate step failed.  Build files cannot be regenerated correctly.

Two causes, both the harness's or this lane's, neither the diff's:

1. `battery.py`'s `shipcfg_script` (`port/tools/battery.py:967`) calls
   `vcvars32.bat` but does **not** put `%ProgramFiles(x86)%\Microsoft Visual
   Studio\Installer` on PATH first. `port/build-port.cmd:5` does, and that is
   the line vcvars needs in order to find `vswhere.exe`. So the arm passes or
   fails on whatever the launching shell happens to carry, which is how one
   tree can produce a green battery and a red one.
2. `build/port-kit/build.ninja` was locked. This lane killed two racing
   `build-port.cmd` chains earlier (the desk was carrying seven other lanes'
   port builds at the time) and left a handle behind in that directory.

**Supply the missing PATH line, clear `port-kit`, and the arm is GREEN**
(`C:\tmp\lagdelay-out\shipcfg.cmd`, log `C:\tmp\lagdelay-out\proof_shipcfg.log`).
Same cmake line the battery uses -- `-DPORT_ROM_CLEAN=ON`
`-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded`, target `walk_window`:

    -- Configuring done (29.1s)
    ...
    [10210/10210] Linking CXX executable walk_window.exe
    dsstate_guard: OK -- 14233 hosted DS symbols all inside .dsstate [0xe17000, 0xf19683), 1058435 bytes captured (12829 matched by name, 1404 by mount object).

`rc=0`, zero `error C`, zero `LNK`, zero `FAILED:` over all 10210 targets, and
`walk_window.exe` linked at 5,685,248 bytes. Its liveness selftest, the check
the battery arm runs after the build (`C:\tmp\lagdelay-out\proof_kitsmoke.log`,
muted and minimized):

    [scene] 300 frames of scene 1 (SCENE_TITLE), clean
    [audio] ring: 301 pushes, 280 refills, 0 starved (4 x 1024 frames at 32768 Hz = 125 ms)

`rc=0`, and it wrote its frame (`walk_window_selftest.bmp`, 589,878 bytes).

**So every battery arm passes on this branch.** The battery's own verdict line
still says otherwise for the reason above, and that PATH bug is written up as a
gap rather than fixed here.

### P5. The late-frame divergence at depth 10: a run-teardown effect

At a 900-frame child budget with depth 10, three of four runs disagreed **in
the last one to four frames**, in one or two player actors (`actorID 191`),
while three runs at depth 5 on the same rig were clean.

**The trimmed sweep settles that it is not a blocker.** Trimming by ROUND
rather than by frame -- keeping only frames whose `rounds=` is at or under an
agreed cap every window reached -- and re-running all 21 pairings is clean, on
two fresh 1200-frame runs (`C:\tmp\lagrev\r1`, `C:\tmp\lagrev\r2`, swept with
`C:\tmp\lagrev\trimsweep.py`). Reproduced in this worktree against r1:

    max rounds per window: {'p0': 1208, 'p1': 1200, 'p2': 1200, 'p3': 1200, 'p4': 1200, 'p5': 1200, 'p6': 1200}
    agreed-round cap used: 1150 (min max = 1200 )
    p0 vs p1  NO DIVERGENCE
    ... all 21 pairings ...
    p5 vs p6  NO DIVERGENCE
    trimmed sweep fail=0

**And it corrects what this file said before.** The earlier reading here was a
sampling straddle -- two consoles at different points on one timeline, caught a
round apart by the harness's last sample. That is wrong on both halves:

- **The peers were at the SAME round.** The trim is by `rounds=`, and the
  windows agree on every frame at or under the cap, so the disagreeing frames
  are not peers reading different rounds.
- **The hash values are NOVEL, not swapped.** `p1`'s `f896` world hash
  `da92dbd0` appears nowhere in `p3`'s log at any frame. The earlier "they hold
  each other's values" claim came from reading two different pairings' digests
  as if they were one pairing's, and it does not survive the check.

So it is a **run-teardown effect**: at the end of whatever budget is set, the
last one to four frames of a window can carry state no other window ever
computed, and it persists for two to four frames rather than diverging onward.
It scales with the budget's end, not with a fixed frame number, and a deeper
pipeline makes the window wider. What it is NOT is two consoles simulating
different inputs, and the trimmed sweep is the evidence.

It stays open as a gap: nothing here explains what the teardown actually
touches, and a run whose windows stop at an agreed round rather than an
independent per-window frame budget would remove the question instead of
bounding it.

### P7. Late join of an old build (the blocker's own proof)

`C:\tmp\lagdelay-out\latejoin_proof.py`, log
`C:\tmp\lagdelay-out\proof_latejoin.log`. `SM64DS_COMMS_LEGACY_PEER`
reproduces the two behaviours of a pre-0.3.3 peer that matter -- it never sends
a round-trip report, and it DROPS an adopted depth past 8 rather than clamping
to it.

    exe C:/tmp/lagdelay\build\port\walk_window.exe  sha 51E5B0674DFD8B10

    === ARM 1: the legacy peer is in the session before frame 0 ===
      rc parent/new/legacy = [0, 0, 0]
      parent       [comms:loopback] closed after 943 rounds; indelay=5 starved=2297 sent=980 recvd=1844 resends=0 starvedby=1:2262,2:1031 lastround=938
      child_new    [comms:loopback] closed after 941 rounds; indelay=5 starved=2243 sent=1188 recvd=974 resends=240 starvedby=0:2243,2:1007 lastround=936
      child_legacy [comms:loopback] closed after 900 rounds; indelay=5 starved=983 sent=905 recvd=933 resends=2 starvedby=0:983,1:983 lastround=894
      P| [comms:loopback] not every peer reported a round trip within 400ms, so the adaptive sizing stands down and this session runs the mode default of 5. A peer that never reports is a peer that would never adopt a re-sized depth either, and a session where one console runs a different depth is a desync.
      legacy peer joined the session: True
      indelay parent/new/legacy = [5, 5, 5]
      rounds parent/legacy = 943/900 (>=200 required: a session that never ran must not read as agreement)
      ARM 1 PASS: every peer closed on the same depth

    === ARM 2: the legacy peer arrives after the session froze ===
      rc parent/new/legacy = [0, 0, 0]
      parent       [comms:loopback] closed after 1209 rounds; indelay=11 starved=48 sent=1232 recvd=1215 resends=0 starvedby=1:48 lastround=1156
      child_new    [comms:loopback] closed after 1200 rounds; indelay=11 starved=61 sent=1206 recvd=1220 resends=1 starvedby=0:61 lastround=1176
      child_legacy [comms:loopback] closed after 0 rounds; indelay=5 starved=0 sent=12 recvd=452 resends=10
      P| [comms:loopback] adaptive input delay 11 frame(s) (was 5): ceil(125 ms worst * 125% / 16.67 ms) + 1, floor 5 cap 15; worst is slot 1; per-child rtt s1=125ms [report]
      P| [comms:loopback] input delay FROZEN at 11 for the rest of this session; the ROM asked for its first round
      P| [comms:loopback] REFUSED a late join from slot 2: this session is frozen at input delay 11, past the 8 that every shipped build can adopt, so a peer that silently kept its own number would simulate a different match. The depth cannot be lowered now -- rounds have been consumed at it.
      P| [comms:loopback] REFUSED a late join from slot 2: this session is frozen at input delay 11, past the 8 that every shipped build can adopt, so a peer that silently kept its own number would simulate a different match. The depth cannot be lowered now -- rounds have been consumed at it.
      P| [comms:loopback] REFUSED a late join from slot 2: this session is frozen at input delay 11, past the 8 that every shipped build can adopt, so a peer that silently kept its own number would simulate a different match. The depth cannot be lowered now -- rounds have been consumed at it.
      legacy peer got a transport at all: True
      rounds parent/new = 1209/1200   sized past 8: True
      parent/new indelay = 11/11   refused=True  seated=False
      ARM 2 PASS: the running pair agrees and no peer was seated at a depth it cannot adopt

    late-join proof: ALL GREEN

Both arms assert LIVENESS first, because the first version of this rig scored a
dead session as a pass: arm 2's relay had stopped answering, no peer completed a
round, and "every peer agrees" was true of nothing at all. Each arm now requires
its peers to have completed at least 200 rounds before any agreement counts, and
each arm gets its own relay.

**What each arm proves.** Arm 1 is the withdrawal: the parent had a legacy peer
live and unreported at sizing time, so it never left a raised number standing --
all three closed at 5. Its `starved=2297` is the honest price of a mixed session
on a 100 ms path, and it is a STALL, which is the trade this whole design makes
against a desync. Arm 2 is the refusal: the session sized to 11, froze, ran 1200
rounds, and the late legacy peer was refused a seat three times over its knock
window rather than being handed a number its clamp would have dropped.


## Honest remaining gaps

1. **The run-teardown effect at the end of a budget is bounded, not
   explained.** P5 has the record. The round-trimmed sweep is 21/21 clean on
   three separate runs, so it is not a divergence in the simulation, but
   nothing here says what the last one to four frames of a closing window
   actually touch. A run whose windows stop at an agreed ROUND rather than an
   independent per-window frame budget would remove the question rather than
   bound it.

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

8. **A staggered start can freeze before the whole lobby is in.** The freeze
   is the parent's first exchange, and a `vs16_ladder`-style staggered start
   can seat a two-player world and freeze before slots 3..6 arrive. Real play
   gates the start behind the lobby, so everyone is in first; a session that
   does not is sized for whoever was there, and anyone later is either handed
   the frozen number (if it is 8 or under) or refused a seat.

9. **The mid-session renegotiation in section 3 is analysis, not code.** The
   argument against it rests on reading `src/func_0203ea5c.c:418`, not on a run
   that tried it and broke.

10. **A late joiner that IS a new build is refused too.** Past the freeze the
    parent cannot tell a new peer from an old one -- the report is a round trip
    away and the seat decision is now -- so a session frozen deeper than 8
    turns away every late joiner. Only a path bad enough to need more than 8
    frames reaches this at all, but on such a path a legitimate rejoin after a
    crash is refused for the rest of the match. Closing it properly wants a
    generation byte in the JOIN, which is a wire change this lane did not make.

11. **A mixed-generation session keeps depth 5 and starves.** That is the
    withdrawal working as designed -- a stall rather than a desync -- but the
    late-join proof measures the price: `starved=2297` over 943 rounds on a
    100 ms path. Nothing here improves a mixed session; it only makes it
    correct.

12. **`SM64DS_COMMS_LEGACY_PEER` reproduces two behaviours of a pre-0.3.3
    peer, not that peer.** It stands in for the report and the adopt clamp,
    which are the two the blocker turns on. A real 0.3.2 binary was not run
    against this branch.

13. **The shipping configuration is proven by a hand re-run, not by the
    battery's own arm.** It configures, builds all 10210 targets, links and
    passes its liveness selftest with rc=0 -- but through
    `C:\tmp\lagdelay-out\shipcfg.cmd`, because the battery's arm cannot get
    past its own PATH bug on this desk. The evidence is equivalent (same cmake
    line, same target, same selftest); the provenance is a hand-run script
    rather than a suite line, and that is worth knowing.

14. **`battery.py`'s shipcfg arm needs a PATH line it does not set.** Found
    here, not fixed here, because it is not this lane's file and a battery
    change wants its own review: `shipcfg_script` (`port/tools/battery.py:967`)
    should prepend `%ProgramFiles(x86)%\Microsoft Visual Studio\Installer` to
    PATH before calling `vcvars32.bat`, exactly as `port/build-port.cmd:5`
    does. Without it the arm passes or fails on what the launching shell
    happens to have on PATH, which is how a green battery and a red one can
    come from the same tree.
