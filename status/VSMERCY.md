# VSMERCY -- the VS star-collect mercy window

Lane worker: VSMERCY. Coordinator: sm64ds-decomp-6c.
Branch `port/vs-mercy` off `339db6bb3`. Worktree `C:\tmp\vsmercy`.
Evidence `C:\tmp\vsmercy-out`.

**DELIVERY. VERDICT: THE MERCY RULE IS AUTHENTIC, AND A SEPARATE DEFECT ON
THE SAME PATH IS FIXED.**

1. The literal question closes as authentic. The DS gives a star collector
   exactly the length of the star-get pose -- 42 frames -- and not one frame
   more; no timer, no post-animation grace. The port reproduces that exactly,
   from the same code and the same animation file.
2. Hunting it found a real online defect one layer over, at internet latency
   only, which the owner's live 0.3.0 session would have hit every time
   somebody grabbed a star: **on the console that is NOT collecting, the
   collector's body got stuck in the no-control state permanently** -- still
   walking and animating, but unable to be hit or to land a hit for the rest
   of the match. Root-caused, fixed in the port's own host layer, proven at
   three latencies. Section 3a.

---

## 1. THE ROM'S RULE

### The predicate

`func_ov002_020d82f0` (src/func_ov002_020d82f0.c, 0x020d82f0) is the "may he be
hurt right now" gate. It returns 0 -- refuse -- on any of:

| field | offset | name |
|---|---|---|
| `mInvincibleTimer` | +0x6a0 | the post-hit blink timer |
| `mIsMega` | +0x703 | |
| `mCapFlags` | +0x73c | |
| `mIsVanish` | +0x6fb | |
| `mIsTakingDamage` | +0x708 | |
| `mIsNoControl` | +0x709 | **the star-collect arm** |

Every damage entry asks it first: `Player::Hurt`
(`_ZN6Player4HurtERK7Vector3j5Fix12IiEjjj`:26), `Player::Shock`,
`Player::Burn`, and `func_ov002_020d8360` (0x020d8360), the player-vs-player
hit handler that `Player::Behavior` reaches every frame through
`func_ov002_020d869c` (0x020d869c). 020d869c refuses a second way as well:
its first line returns on `mIsBodyClsnEnabled` (+0x713) == 0, and its
jumped-on branch tests +0x709 directly.

### How a VS collect arms it

    func_ov002_020e930c    the star's touch gate; refuses a player already
    (0x020e930c)           in no-control (`*(u8*)(b + 0x709) != 0`)
      -> func_ov002_020e8ef0  the collect handler
         (0x020e8ef0)         VS branch (data_0209f2d8 == 1) calls
                              Player::SetNoControlState(kind, -1, 0)
                              with kind = func_ov002_020e73ac(star), which
                              returns 2 in VS (the mov r0,#2 at 0x020e740c)
      -> Player::SetNoControlState (0x020ca1b8)
           kind 2 <= 3, so it REFUSES if already in no-control;
           otherwise mNoCtrlKind = 2 and ChangeState(data_ov002_0211022c)
      -> Player::St_NoControl_Init (0x020c976c)
           mIsControlDisabled = 1
           Player_DisableInteraction (0x020c9e40):
               mIsNoControl = 1  AND  body-collider flags +0x2ec |= 4
           then the per-kind entry from the PMF table data_ov002_02110884;
           row 2 is {0x020c9de4, 0} (read out of the ROM at 0x0210a244)
      -> func_ov002_020c9de4 -> func_ov002_020c9d68: mStateStep = 0
      -> Player::St_NoControl_Main (0x020c816c) step 0
           -> func_ov002_020c965c -> func_ov002_020c7ff8 (0x020c7ff8):
               SetAnim(0x19) -- the star-get pose
               mStateStep = 2
               mIsBodyClsnEnabled (+0x713) = 0

So **three** layers arm on the same frame: the gate's own flag, the collider's
category bit, and the collider being taken out of the collision system
entirely.

### When it ends

`St_NoControl_Main` step 2 -> `func_ov002_020c92fc` (0x020c92fc).
`SetNoControlState` stored -1 in +0x688 for a VS collect, so the
`WillHitFrame` early arm is skipped and the ONLY exit is
`Player::FinishedAnim` (0x020bed98) -> `ChangeState(data_ov002_0211013c)`,
which is `St_Walk`. `Player::ChangeState` (0x020e30a0) puts everything back in
the same call: `mIsBodyClsnEnabled = 1` at :76 and `func_ov002_020c9e18`
(0x020c9e18) at :108, which clears `mIsNoControl` and the +0x2ec bit.

`Animation::Finished` is `currFrame >= (numFramesAndFlags & 0x3fffffff) - 1`,
and `Animation::SetAnimation` stores both as 20.12 (`flags | (numFrames <<
12)`). So the window is **the animation's own frame count, out of the
cartridge's file**, with no constant anywhere in the path.

### THE RULE IN ONE LINE

> A VS star collector is un-hurtable for exactly as long as the star-get
> animation plays, and hurtable again the very next frame. There is no timer
> and no post-animation grace.

`mInvincibleTimer`'s 0x24 (36 frames) belongs to the HURT states --
`St_Hurt_Main` (0x020d966c):86, `St_Electrocute_Init`:16 / `_Main`:58,69,
`St_HurtWater_Main`:38. A collect never touches it. That asymmetry is the
whole feel of the report: get hit and you blink for a second; grab a star and
the protection stops dead with the pose.

---

## 2. THE PORT, MEASURED

Instrument: `port/hal/input_probe.cpp`, `SM64DS_VS_MERCY=every` (a per-player,
per-frame read-only line carrying the gate's verdict, all six of its inputs,
the pose and the animation clock) and `SM64DS_MERCY_HIT=<frames>` (asks the
REAL `Player::Hurt` to land a hit and prints what it returned next to the
gate's answer). Driver: `port/tools/vsmercy_probe.py`. Both are env-gated and
inert when unset.

Fixture: arena map 1 (level 51), `SM64DS_VS_PLAYERS=2`,
`SM64DS_VS_BREAKALL=120`, `SM64DS_STAR_TRIGGER=240`. Quiet and muted through
`mp2_proof.env_base`; port bases derived from the harness PID.

### 2a. One instance -- the window, and it is the ROM's own number

    [mercy] f240 p0 canhurt=1 noctl=0 kind=0 bodyclsn=1 cflag4=0 inv=0 anim=72 step=0 state=020d38c0 animlen=30 animcur=14
    [mercy] f241 p0 canhurt=0 noctl=1 kind=2 bodyclsn=0 cflag4=1 inv=0 anim=25 step=2 state=020c816c animlen=42 animcur=1
    ...
    [mercy] f281 p0 canhurt=0 noctl=1 kind=2 bodyclsn=0 cflag4=1 inv=0 anim=25 step=2 state=020c816c animlen=42 animcur=41
    [mercy] f282 p0 canhurt=0 noctl=1 kind=2 bodyclsn=0 cflag4=1 inv=0 anim=25 step=2 state=020c816c animlen=42 animcur=41
    [mercy] f283 p0 canhurt=1 noctl=0 kind=2 bodyclsn=1 cflag4=0 inv=0 anim=72 step=0 state=020d38c0 animlen=30 animcur=11

* the collect lands at f240 and the window opens at f241 with **all three ROM
  layers armed at once**: `noctl=1`, `cflag4=1`, `bodyclsn=0`;
* `kind=2` -- the VS kind, exactly what `func_ov002_020e73ac` returns;
* `anim=25` = 0x19, the star-get pose, in `state=020c816c` =
  `St_NoControl_Main`, `step=2`;
* **`animlen=42`** -- the pose is 42 frames long in the cartridge's own
  animation file, read back off the live object;
* `canhurt=0` on **42 of 42** frames inside the window;
* it closes at f283 into `state=020d38c0` = `St_Walk_Main` with
  `inv=0`. **No invincibility timer is left behind**, which is the ROM.

**Measured window = 42 frames = the animation's own length.** Not a port
constant: the number the probe prints as `animlen` and the number of armed
frames are the same number, and it came out of the ROM's file through the
ROM's matched `Animation::Advance` / `Animation::Finished`. That is why this
lane did not need the emulator -- there is nothing left for a DS run to
disagree with. Evidence `C:\tmp\vsmercy-out\pass12b.txt` and
`build/vsmercy/solo/run.log`.

### 2b. One instance -- damage really is refused, then really is allowed

`SM64DS_MERCY_HIT` calling the real `Player::Hurt`:

    [mercyhit] f2  (INSIDE) gate=0 Hurt=0 noctl=1 anim=71
    [mercyhit] f61 (INSIDE) gate=0 Hurt=0 noctl=1 anim=71
    [mercyhit] f64 (AFTER)  gate=1 Hurt=1 noctl=0 anim=105
    [mercyhit] f66 (AFTER)  gate=0 Hurt=0 noctl=0 anim=105

(that window is the level-entry no-control, which the same mechanism arms).
The f64 hit LANDS, and the follow-up two frames later is refused -- the port
also reproduces the ROM's post-hit mercy: hurt state from f65, then
`mInvincibleTimer` counting 0x24 down (`inv=25` at f100 ... `inv=14` at f111)
and the player hurtable again at f125. **60 frames after a hit, 42 after a
collect** -- both the ROM's.

### 2c. Two consoles, one live session -- the wire does not shorten it

Parent + child over the loopback carrier, `SM64DS_SYNC=1`, both seated
(`session up ... link=3 slot=0 players=2` / `link=4 slot=1 players=2`),
30 Hz pose sync live.

    window 0  transitions after f200: (241, 0->1, kind 2, anim 25), (283, 1->0)
    window 1  transitions after f200: (241, 0->1, kind 2, anim 25), (283, 1->0)

Both consoles open and close the collector's window on the same frames, same
kind, same pose. And the sync layer was genuinely live across it, not idle --
its own end-of-level counters:

    p0  sent=353 recvd=350 applied=350 lerps=22 snaps=0 reseeds=0 phase_worst=6468 rtt_avg=15
    p1  sent=351 recvd=353 applied=353 lerps=15 snaps=0 reseeds=0 phase_worst=6468 rtt_avg=13

350+ snapshots applied per side, anim events crossing, and **reseeds=0** with
`phase_worst=6468` in 20.12 = 1.58 frames, far under the 8-frame reseed
threshold. The sync layer does not bypass the local immunity either:
`comms_sync.cpp` carries position, yaw, anim id, anim cursor and velocity
only, applies the animation through `Player::SetAnim`, and never applies state
or damage -- grepped: no damage, score or `data_0209f310` field exists
anywhere in `comms_sync.cpp`, `comms_seam.h` or `comms_conductor.cpp`.
Evidence `C:\tmp\vsmercy-out\pass3.txt` and
`build/vsmercy/pair/p{0,1}/run.log`.

**AND THIS IS WHERE A CLEAN RUN WOULD HAVE LIED.** Loopback is ~15 ms, and at
15 ms the wire's animation is never stale long enough to matter. Re-running
the same fixture with `SM64DS_SYNC_DELAY_MS` -- the latency rig the
mp-sync-coopdx lane built for exactly this -- broke it at the first setting
tried. Section 3a.

### 2d. The kInvincibleTimer history the brief pointed at

`port/hal/player_fields.h` `kInvincibleTimer = 0x6a0`, `u16`, and its comment
carries the 2026-08-28 fix: the port had been reading **0x6a6**, which is
`mStateWaitTimer` -- a different field that ticks constantly -- and
`hal/player_bridges.cpp`'s body-draw gate was culling a player's body on bit 0
of it. Six bytes off, and the shadow path kept drawing: a shadow with no body,
from the fix meant to stop shadows without bodies. The offset is right in this
tree, `include/Player.h`:297 agrees, and the probe reads the same word: the
`inv=` column above counts 0x24 down cleanly after a hit and stays 0 across
the whole collect, which is exactly what the ROM does.

---

## 3. VERDICT

**THE RULE ITSELF IS AUTHENTIC.** "Attackable right out of the star collection
animation" is what the cartridge does. 42 frames -- 0.7 s -- of the collect
dance, and then the collector is fair game with no blink. The port arms the
same three layers off the same matched code and ends on the same animation.
The contrast the owner is feeling is real and it is the DS's: **60 frames of
mercy after being hit, 42 after collecting a star, and none at all once the
dance ends**, because `mInvincibleTimer` belongs to the hurt states and a
collect never touches it. If he wants more, that is a product choice, not a
bug -- see the one-sentence MOD scope in 3b.

**BUT the run that proved it also found a real defect on the same path**, and
that one is a bug, latency-only, and now fixed. Section 3a. Note the direction:
before the fix, at internet latency a collector became PERMANENTLY un-hittable
on the other player's screen -- so what the owner remembers as inconsistent
combat around star collection is more likely to have been this than the mercy
window being short.

### 3a. THE DEFECT THIS LANE FOUND AND FIXED

**MEASURED, at ordinary internet latency, on the two-console fixture with
`SM64DS_SYNC_DELAY_MS`. Before the fix:**

| induced delay | measured RTT | collector's own console | the OTHER console |
|---|---|---|---|
| 0 (loopback) | 15 ms | f241..f282, 42 frames | f241..f282, 42 frames |
| 20 ms | 76 ms | f241..f282, 42 frames | **opens f241, NEVER CLOSES** |
| 50 ms | 135 ms | f241..f282, 42 frames | **opens f241, NEVER CLOSES** |
| 120 ms | 264 ms | f241..f282, 42 frames | **opens f241, NEVER CLOSES** |

At f699 -- 458 frames after the collect -- the non-collecting console still
had `noctl=1 bodyclsn=0 canhurt=0 step=2 state=020c816c` on the collector's
body. So after ANY star collect, on the other player's screen that player
becomes a body that walks and animates but can neither be hit nor land a hit,
for the rest of the match. Both flags the ROM uses are stuck.

**Root cause.** A no-control state does not merely play a pose, it ENDS ON
one: `func_ov002_020c92fc` leaves the collect only when
`Player::FinishedAnim` says the star-get animation has run out, and
`Player::ChangeState` is what then clears `mIsNoControl` and puts the collider
back. `comms_sync.cpp`'s `apply_pose` was overwriting that animation with the
wire's, which at real latency is one animation behind. The frame-by-frame
trace at 264 ms (`build/vsmercy/latpair120/p1/run.log`):

    f241  anim=25 animlen=42 animcur=1     the collect starts, correctly
    f242  anim=72 animlen=30 animcur=10    the wire's STALE walk yanks the pose
    f250  anim=25 animlen=42 animcur=1     the wire's own 0x19 RESTARTS it
    f290  anim=25 animlen=42 animcur=41    it finally reaches the end...
    f291  anim=72 animlen=30 animcur=11    ...and the wire has already moved on

Anim 72 is a LOOPING animation, and `Animation::Finished` is
`currFrame >= (numFrames << 12) - 1` while `Animation::Advance`'s loop branch
wraps modulo the length -- so the cursor never lands on that value and
`FinishedAnim` is never true again. The state machine has no other exit.

**The fix** (commit `e62b179a4`, `port/hal/comms_sync.cpp`): `apply_pose`
holds the pose while the receiving body is in a no-control state, and counts
the holds as `pose_held` on the `[sync:level]` line. Position and facing still
apply; only the pose is left to the state machine that owns it. It re-arms by
itself on the next `ChangeState`. This is also the more faithful answer -- the
receiving console is running the ROM's own collect on its own copy, the same
42 frames, so its local pose is better information than a latency-old remote
one. `player_fields.h` gains `kIsNoControl = 0x709` with its evidence (two
writers found by scanning ov002 for a byte store at that displacement:
`Player_DisableInteraction` 0x020c9e40 sets, `func_ov002_020c9e18` clears).

**After the fix, every latency agrees with the ROM:**

| induced delay | measured RTT | console 0 | console 1 | pose_held |
|---|---|---|---|---|
| 0 (loopback) | 12-14 ms | f241..f282 | f241..f282 | 31 / 54 |
| 20 ms | 76 ms | f241..f282 | f241..f282 | 28 / 52 |
| 50 ms | 135-141 ms | f241..f282 | f241..f282 | 28 / 50 |
| 120 ms | 257-263 ms | f241..f282 | f241..f282 | 26 / 48 |

42 frames on both consoles at every latency, with `animlen=42` at both
transitions -- the star-get pose runs to its own end on the remote copy now.
Evidence: the evidence bundle's `latpair{20,50,120}.txt` (before) and
`fix_latpair{20,50,120}.txt` (after).

**No regression.** Solo and the loopback pair are unchanged after the fix:
same f241..f282 collect window, same 63-frame level-entry window, same hit
verdicts (`gate=0 Hurt=0` inside, `gate=1 Hurt=1` after, refused again two
frames later) -- the bundle's `fix_pass123.txt`. `SM64DS_SYNC` is off
by default, so single-player and adventure never reach the changed line at
all.

---

### 3b. WHAT IS STILL OPEN

**The one caveat, and it is the only thing here that could still be worth
acting on.** The port's online VS is not the DS's lockstep. `comms_sync.cpp`
syncs bodies; each console computes collisions, damage and scores on its own
copy, and nothing about damage or score ever crosses the wire. So each console
enters the collect the frame ITS OWN copy of the bodies overlaps the star, and
the remote copy of a moving player is one-way-latency behind. Concretely, on
the attacker's console the collector's proxy reaches the star LATER than the
collector's own console does, and inside that gap:

* the collector's console has already armed the window and refuses everything;
* the attacker's console still sees an ordinary body, accepts the punch, runs
  `Player::Hurt` on the proxy and takes a star off the collector **locally**.

The attacker sees the hit land on someone who is starting the star animation;
the collector sees nothing; the two consoles then disagree about the score
permanently, because scores are never reconciled. That is asymmetric in
exactly the way that produces a "we could hit each other during the star" feel
even though both consoles are running faithful ROM code.

What THIS lane proved about it: the full collect chain **does** run on a
remote proxy -- console 1's copy of player 0 armed all 42 frames, kind 2 and
all -- so the mechanism is not missing on the remote side. What is NOT
measured is the size of the detection-frame offset at internet latency: the
fixture pins the collect to the same frame on both consoles, and the harness
cannot drive a natural cylinder overlap headlessly. The way to measure it is a
star at a FIXED world position with the players walked into it by held
directions, run under `SM64DS_SYNC_DELAY_MS`; the loopback pair here was at
rtt_avg 15 ms with `avg_err` 4508 and `worst_err` 33793 in 20.12 (1.1 and 8.25
units), which is the number that would grow. At 120 ms induced delay the same
figures were `avg_err` 24275 / 67689 and `worst_err` 256914 / 383729, so the
error the offset is made of grows roughly with latency as expected. Closing it
means making damage authoritative on one console -- not lengthening any
window, and it is a bigger change than this lane. **NOT a blocker for 0.3.1:**
the 3a fix removes the large, permanent divergence; what is left is a
boundary-frame disagreement measured in single frames.

**The MOD-ruling option, scoped in one sentence, NOT built:** a lobby toggle
would, when the collect's no-control ends, write the ROM's own
`mInvincibleTimer` (Player+0x6a0) with a configurable frame count -- the same
field and the same gate the hurt states already use, so no new mechanism and
no matched code touched -- and carry that count on the aux channel so both
consoles agree on it.

---

## 4. WHAT THIS BRANCH CONTAINS

Two commits. **Zero changes under `src/`, `include/` or `config/`**, so the
byte gate is untouched by construction and the DS side is bit-for-bit the base
commit's (`git diff --stat 339db6bb3 HEAD -- src/ include/ config/ symbols/`
is empty). Solo and adventure are unaffected: both probes return on their
first line when their env knob is unset, and `SM64DS_SYNC` is off by default
so the changed line in `apply_pose` is never reached outside an online
session.

`b7f5c0468` -- the readout and the measurement:

* `port/hal/input_probe.cpp` -- `port_probe_vs_mercy` and
  `port_probe_mercy_hit`, plus `mercy_anim_clock`; read-only, env-gated.
* `port/tests/walk_window.cpp` -- two calls in the per-frame probe block,
  before the actor tick.
* `port/tools/vsmercy_probe.py` -- the three-pass driver.

`e62b179a4` -- the fix:

* `port/hal/comms_sync.cpp` -- `apply_pose` holds the pose while the receiving
  body is in a no-control state.
* `port/hal/comms_seam.h` -- `SyncStats::pose_held`, appended after `gated` so
  the `[sync:level]` prefix the SY rungs parse is unchanged.
* `port/hal/player_fields.h` -- `kIsNoControl = 0x709`, with its two ROM
  writers cited.

Branch tip `e62b179a4` on `port/vs-mercy`.

### Proofs

| gate | result |
|---|---|
| battery, before the fix | **ALL GREEN** (50 levels, 34 scenes, default boot to title, linkage 9126 = 80.6%, ptr_audit 0, shipcfg build + selftest ok) |
| abicheck, before the fix | 6417 OK, **EXTENSION RATCHET PASSED**, 0 new unjudged slots |
| battery, after the fix | **ALL GREEN** (same 50 levels, 34 scenes, default boot, linkage 9126 = 80.6%, ptr_audit 0, shipcfg build + selftest ok) |
| abicheck, after the fix | 6417 OK, **EXTENSION RATCHET PASSED**, 92 baselined / 92 live / 0 NEW / 0 retired |
| solo collect window | f241..f282, 42 frames, `canhurt=0` on 42/42, unchanged by the fix |
| real `Player::Hurt` | refused inside (`gate=0 Hurt=0`), lands after (`gate=1 Hurt=1`), refused again 2 frames later |
| DS side | no diff under `src/`, `include/`, `config/`, `symbols/` |

Evidence bundle `C:\tmp\vsmercy-out\`: `pass12.txt`, `pass12b.txt`,
`pass3.txt`, `latpair{20,50,120}.txt`, `fix_latpair{20,50,120}.txt`,
`fix_pass123.txt`, `battery.log`, `battery_fix.log`, `abicheck_base.txt`,
`abicheck_fix.txt`, plus every run log under
`C:\tmp\vsmercy\build\vsmercy\`.

2026-09-01 - INDEPENDENT REVIEW (tip `9731728ac`, fix `e62b179a4`): **PASS - rides 0.3.1.** Re-derived on my own build; the battery in flight when my session was killed was discarded unread and re-run clean. **PART 1 AUTHENTICITY: CONFIRMED**, chain verified end to end in ROM source (SetNoControlState -> St_NoControl_Init -> Player_DisableInteraction writes +0x709 and collider |=4; func_ov002_020d82f0 refuses on +0x709; exit via func_ov002_020c92fc -> FinishedAnim -> ChangeState, whose line 105 calls func_ov002_020c9e18 to clear both). animlen is read off the live ModelAnim (+0x54, the .bca's own count), not a port constant. The 36-frame mInvincibleTimer has exactly two writers in the tree, St_Hurt_Main and St_Electrocute_Main - none at collect. My own solo run reproduces f241 open / f282 last armed / f283 closed, 42 rows noctl=1, canhurt=0 on 42/42, inv=0 throughout. **And I measured the contrast the record only asserts**: a real hit at f64 puts the body in St_Hurt for 24 frames and then arms inv=36 counting down - same field, same probe, armed by the hurt path and left at zero by the collect path. **PART 2 THE FIX: sound.** I proved its causal story arithmetically rather than taking the banner: Advance's looping branch steps currFrame by 0x1000 modulo len so it tops out at len-0x1000, while Finished() needs cur >= len-1, making Finished() mathematically unreachable on a loop; the one-shot branch clamps exactly onto len-1. apply_pose is animation-only, the guard is its first statement, and both call sites apply position and facing before it - so "position and facing still apply" is accurate. (a) All three latency arms re-run, both consoles open f241 and close f283 at rtt_avg 86/87, 148/150, 284/301 - my RTTs ran HIGHER than the lane's because another lane had two windows live, making my arms the harder test. (b) **Never-exits attack, beyond the brief**: I added 300ms and 500ms arms (664 and 1016 ms RTT, ~4x the lane's worst) and both consoles still close at f283; decisively, pose_held stays in a 24-51 band from 86ms to 1018ms and does NOT scale with latency, which is the signature of a hold bounded by the local state rather than the wire. Structurally the fix REMOVES a foreign write, which cannot invent a new way for a locally-driven state machine to fail to exit; and the named failure mode (receiver never entered no-control) degrades gracefully to exact pre-fix behaviour. (c) battery ALL GREEN rc 0 (linkage 9126, ptr_audit 0, shipcfg ok), abicheck rc 0 92/92 0 NEW, loopback pair open-differs-by-0 length-differs-by-0. (d) zero delta under src/ include/ config/ symbols/ - verified empty. (e) Open item honestly recorded; **no escalation needed to hold 0.3.1** (pre-existing, and closing it means damage authority) but it should be carried as a tracked item into that work - one precision note, "a boundary-frame disagreement measured in single frames" undersells its own paragraph, since the trigger is single-frame but the consequence is a permanently divergent score. MOD scoping sentence accurate on every checkable claim, and correctly labelled a MOD precisely because part 1 shows the ROM grants no such window. Four non-blocking items, first two worth the gate's attention: **walk_window.cpp is NOT comment-only** - it adds two per-frame calls to the main loop in a file other lanes are live in (inert when unset: getenv-then-return, and the one ROM function the probe calls is a pure predicate); and **the probe's window() returns the entrance cutscene, not the collect**, so re-running vsmercy_probe.py prints "f0..f62, 63 frames, kind=0" and a reader will think the record is wrong - the record's f241..f282 is right but comes from the raw rows, and pass 1/2 should filter after f200 the way the latency rig already does. Worktree on 9731728ac, 0 tracked diffs, nothing pushed; another lane's walk_window processes were identified by executable path and left untouched. Full report: `C:\tmp\vsmercyrev-out\REVIEW.md`.
