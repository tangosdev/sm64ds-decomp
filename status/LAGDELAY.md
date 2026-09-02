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

PROOFS_PLACEHOLDER

## Honest remaining gaps

GAPS_PLACEHOLDER
