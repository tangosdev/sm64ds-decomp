# VS16HOST - hosting the ROM's exchange-and-unpack loop

Lane: the VS16 follow-up. The VS16 delivery ended on one sentence: "sixteen
players get into one match together ... four of them play", and named the
wall - src/func_0203ea5c.c, the ROM's own wireless exchange and unpack loop,
nine hard-coded `< 4` bounds at :310, :326, :347, :352, :367, :403, :428,
:471, :499. Byte-matched ROM code, so it cannot be edited. This lane HOSTS
it, and with it the wall is down: slots 4..15 enter world lockstep, and a
sixteen-player match runs to MATCH OVER.

Worktree `C:\tmp\vs16host`, branch `port/vs16-hostloop`, base `6c3d0a7a0`
(the VS16 delivery tip). Own TEMP `C:\tmp\vs16hostbld`, evidence
`C:\tmp\vs16host-out`. `git diff --stat 6c3d0a7a0 -- src/` is empty. Nothing
pushed; the cons gate merges after review.

---

## 2026-09-01 -- DELIVERY

### The design: the ROM TU stays, and the wide copy runs only wide

**The dispatch.** `port/CMakeLists.txt` compiles src/func_0203ea5c.c under
the per-source rename `func_0203ea5c=func_0203ea5c_narrow` - the same R1/R2
mechanism a dozen actor TUs already use, and the TU's bytes are untouched.
`port/hal/comms_conductor_wide.cpp` owns the symbol: a two-line gate that
runs the ROM TU itself for every session of four or fewer, and the wide copy
for five or more. So the narrow path is not a transcription of the
cartridge's conductor - it IS the cartridge's conductor, the one already
proven by every existing 2P and 4P run.

**The gate reads the number that picks the wire.**
`port::comms_session_players()` (new, comms_loopback.cpp) returns the same
`g_want_players` the carrier derives wire v2/v3 from, so the record bounds
and the datagram layout cannot disagree. No transport installed answers 0,
which is narrow, which is solo: unchanged.

**The wide copy is a transcription, not a rewrite.** Same locals, same
staging, same switch, same gotos, same store order. SEVEN of the nine bounds
walk the per-slot record run and now walk sixteen (:310 the unpack, :326 /
:367 / :403 the ready scans, :428 the game-id scan, :471 the version scan,
:499 the go-bit scan). Sixteen is the ARRAY's width, mirroring the ROM's own
walk-the-whole-array shape; presence is still the record's 0x8000 bit.
`port/tools/hostloop_transcription_check.py` proves the fidelity claim
mechanically: normalize the wide copy back to the ROM shape (kRecs -> 4, two
C++ casts, the two flagged additions) and it is **line-identical to
src/func_0203ea5c.c - 406 lines - with all 60 extern declarations verbatim**.
The reviewer runs it from the worktree root.

**The two bounds that do not move** (:347, :352) are the DS lobby's
info-exchange merge. Their storage is the cartridge's four-slot name bands
(conductor bands B/C/D), the path cannot arm in the port's flow (the trigger
needs every live peer advertising record bit 0, which only the DS wireless
lobby's countdown sets, and the port's names travel as SM64DS_VS_NAMES), and
if it ever does arm in a wide session it says so out loud once. Zero such
lines in every run of this lane's evidence.

**What had to exist underneath:**

- The record run at data_020a1154 grows 4 -> 16 records of 0x24
  (hal/camera_bridges.cpp), by its TAIL member only - every interior offset
  the layout check asserts is untouched, and the check now also asserts the
  0x240 span. This is load-bearing beyond the conductor: the ROM's own
  slot-indexed readers (func_0203d950(i), func_0203dabc(i),
  GetAngleToCamera(i)) index these records with no bound of their own, so
  records 4..15 had to exist contiguously at the ROM's stride - a side array
  would have satisfied the conductor and left every one of those readers
  reading garbage.
- The seam's open face (hal/comms_seam.cpp, func_020408b0) zeroes records
  4..15 at session-arm, mirroring src/func_0203db64.c:64's 0x90 clear of
  0..3 - the face runs exactly once per session-arm off the conductor's own
  one-shot, so a wide session formed after an earlier one cannot wait on a
  dead session's live bits.

### The proofs

Build sha256 862fac1f... (C:\tmp\vs16host-out\exe.sha); all build guards
green (dsstate, gxband incl vsstar/ready, tailjump).

**The ladder, same rig as VS16** (port/tools/vs16_ladder.sh; sweeps by
C:\tmp\vs16host-out\dhsweep.sh - all pairings among 0..3, every slot 4+
against the parent, wide child-child pairs):

| N | live mask | slots 4+ first hashed frame | dhdiff verdict |
|---|---|---|---|
| 2 | 0x3 | n/a | 1 pairing, NO DIVERGENCE |
| 4 | 0xf | n/a | 6 pairings, NO DIVERGENCE |
| 5 | 0x1f | **frame 0** (was: session's last round) | 7 pairings, NO DIVERGENCE |
| 8 | 0xff | frame 0 | 11 pairings, NO DIVERGENCE |
| 16 | **0xffff** | frame 0 | **23 pairings, NO DIVERGENCE** |

**The full match, the proof VS16 could not reach.** vs4_proof's rung-7 star
recipe at sixteen windows (C:\tmp\vs16host-out\match.sh): all sixteen print
the identical marker

    [vs] MATCH OVER f320 win=star-target scores=1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 players=16 total=1 winner=0

deterministic across five runs (f320 every time), sixteen scores latched
from a played match rather than a parse test.

**2P/4P regression.** Zero `WIDE SESSION` / `WIDE CONDUCTOR` lines in any
narrow log - the narrow path is the ROM TU and the logs show the v2 wire.
All pairings NO DIVERGENCE. And cross-BUILD: this build's rung4 children are
hash-identical to the DELIVERED VS16 build's own rung4 logs for all 600
common frames (p1..p3; p0 refuses on round alignment because the two runs'
join budgets differ - dhdiff refusing misaligned logs is the tool working).

**Suites.** battery **ALL GREEN**; linkage **9139 (80.7%) unchanged** (the
symbol still resolves and the matched TU is genuinely linked and executed -
both facts, not an accounting trick); ptr_audit **0**; shipping config
builds and selftests clean; abicheck **RC=0, no new extension slot**.

**Banner captures at sixteen - the deliverable HUD-OPTIONS.md was owed.**
All three modes captured from the real played match (child budget 360 puts
the selftest completion screenshot mid-banner; the DUMP_FROM knob lives in a
different harness loop and does not fire in this mode). Sent to the owner:
banner16-{basic,top4,all}-p6.png in C:\tmp\vs16host-out. The owner picks;
nothing recommended here.

### What is NOT proven, plainly

- **Nothing over the relay at sixteen.** All of the above is loopback, same
  as the VS16 delivery's own rungs.
- **The info-exchange path in a wide session.** Kept ROM-shaped and believed
  unreachable (argument above); it is guarded by a loud one-shot log, not by
  a proof of unreachability.
- **Sixteen palette rows** (yoshi_all_16p_pl) and **authored sixteen-player
  arena starts** stay owed exactly as the VS16 delivery recorded them.

---

## 2026-09-01 -- RELEASE 0.3.2: the review that made sixteen play

The owner's word: release. The VS16 review's reviewer session had died, so
the merged diff (VS16 + VS16HOST, 952b3c135..1e0a437ff) went through an
eight-angle review here (three correctness, reuse, simplification,
efficiency, altitude, conventions). It found what the lockstep proofs could
not: motionless bodies are perfectly deterministic.

### What the review found and what shipped for it

1. **Players 5..16 could not move.** Five per-player controller split
   arrays were still four wide (hal/actor_vtables.cpp), so the harness
   capped its per-frame fan-out at four, and the match-end hold loop wrote
   past those arrays after every wide match. Widened; cap gone.
2. **Under that, the ROM's own per-frame fan-out** (src/func_0203bb60.c and
   func_0203bc7c.c, comms records -> TouchInfo/PadData) loops to four.
   Hosted like the conductor: ROM TUs linked under a rename, sixteen-wide
   copies in hal/comms_fanout_wide.cpp chosen by the session's width.
3. **Under that, the ROM seats health for four** (src/SetPlayerGlobals.c),
   and the health table was four wide. A wide slot read dead the frame
   interaction came on and hung in St_Respawn (exit armed only for the
   local player): eleven of twelve wide bodies, frame 60, every run. Table
   widened; the port seats health and controller mode for the slots it
   spawns.
4. **The carrier admitted slots a narrow wire cannot carry.** Slot admission,
   binding and pinning now follow the wire (slot_cap()).
5. **The lobby let a host dial wide over a seated old client.** Members
   record their contract version; the dial and the start refuse with
   seated_client_too_old; unit test added; launcher shows a sentence for it
   and for needs_newer_client.
6. Level teardown, the editor channel's player guard, the player render
   gate and the seam probe's private band all reach sixteen.

Evidence of each, before and after, in C:\tmp\vs16host-out\release\ and
the cons-rung*/cons-match16 directories: pre-fix slot 4/9/15 had 4/25/26
distinct positions over 900 frames (slot 0: 2280); post-fix every slot at
16 windows has 901..2280 and ends in a walking state.

### Proofs on the shipped build (cons tip)

| rung | dhdiff | every slot moves | wide lines | live |
|---|---|---|---|---|
| 2 | 1/1 NO DIVERGENCE | min 805 positions | 0 | 0x3 |
| 4 | 6/6 | min 1516 | 0 | 0xf |
| 5 | 7/7 | min 1564 | 5 | 0x1f |
| 8 | 11/11 | min 884 | 8 | 0xff |
| 16 | 23/23 | min 901 | 16 | 0xffff |

Sixteen-player match: all 16 windows print the identical MATCH OVER marker
at f320 with sixteen scores. Seam probe mp_comms_seam: OK. Lobby
test_units 479/0, test_security 8/0; launcher LobbyUnit 203/0, ExtractFail
86/0, DpiLayout 6590/0, ReviewHarness 359/0 (its five Y-series
expectations predated the Multiplayer button and were brought up to date).

### Deployed with the release

- Relay (systemd sm64ds-relay on tangos): repo relay.py with
  SM64DS_RELAY_MAX_CHILDREN=15 as a unit drop-in; log says
  `seats=16 (1 parent + 15 children)`. Backup at ~/relay-backup-*.
- Lobby (~/sm64ds-lobby, docker compose): contract_v 3, dial_max 16.

### Debt, said plainly

- test_client.py's `selftest` against the deployed URL assumes a four-seat
  deployment (23 of 149 checks red on a sixteen-seat one); the unit suite
  and the `dial`/`negatives` wire suites are the honest gates until it is
  reshaped.
- Review cleanups not taken for the release (kept the battery-tested tree):
  vs_width.h's drift static_assert never compiles (include order); three
  parsers of SM64DS_VS_PLAYERS; the 3-or-15 grammar in five readers; the
  banner mode knob; 532-byte memsets on the narrow send path; the refusal
  log unthrottled; the palette row for slots 4+ follows the truncated slot
  (cosmetic, deterministic); ring spawn slot 14 overlaps record 1 on arena
  0 by geometry; data_0209fc5c[4..15] not cleared across sessions in one
  process (one session per process today).
