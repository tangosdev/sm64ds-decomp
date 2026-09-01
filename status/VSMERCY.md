# VSMERCY -- the VS star-collect mercy window

Lane worker: VSMERCY. Coordinator: sm64ds-decomp-6c.
Branch `port/vs-mercy` off `339db6bb3`. Worktree `C:\tmp\vsmercy`.
Evidence `C:\tmp\vsmercy-out`.

**DELIVERY. VERDICT: AUTHENTIC.** The DS gives a star collector exactly the
length of the star-get pose and not one frame more, and the port reproduces
that exactly -- same code, same data, same frame count, on both consoles of a
live two-console session. Nothing to fix. There is one honest online caveat
and one one-sentence scope for a MOD-ruling option, both below.

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
kind, same pose. The pose-sync layer does **not** cut the star-get animation
short on the non-collecting console, and does not bypass the local immunity:
`comms_sync.cpp` carries position, yaw, anim id, anim cursor and velocity
only, applies the animation through `Player::SetAnim`, and never applies state
or damage. Evidence `C:\tmp\vsmercy-out\pass3.txt` and
`build/vsmercy/pair/p{0,1}/run.log`.

---

## 3. VERDICT AND THE HONEST CAVEAT

**AUTHENTIC.** "Attackable right out of the star collection animation" is what
the cartridge does. 42 frames -- 0.7 s -- of the collect dance, and then the
collector is fair game with no blink. The port arms the same three layers off
the same matched code, ends on the same animation, and behaves identically on
both consoles of a session.

**The one caveat, and it is architectural, not a defect in this path.** The
port's online VS is not the DS's lockstep: `comms_sync.cpp` syncs bodies and
each console computes collisions, damage and scores on its own copy. Each
console therefore runs the collect the frame ITS copy of the bodies overlaps
the star, so the two consoles' windows open and close a few frames apart --
roughly the one-way latency plus the corrector's lag. A hit thrown in those
boundary frames can be accepted on the attacker's console and refused on the
victim's, and the two would then disagree about a star. This is a REAL but
NARROW effect (a handful of frames at internet latency), it is not what the
report describes, and closing it means making damage authoritative on one
console rather than lengthening any window. **NAMED GAP, not measured**: the
fixture pins the collect to the same frame on both consoles, so this run
cannot show the offset, and the harness cannot drive a natural cylinder
overlap headlessly.

**The MOD-ruling option, scoped in one sentence, NOT built:** a lobby toggle
would, when the collect's no-control ends, write the ROM's own
`mInvincibleTimer` (Player+0x6a0) with a configurable frame count -- the same
field and the same gate the hurt states already use, so no new mechanism and
no matched code touched -- and carry that count on the aux channel so both
consoles agree on it.

---

## 4. WHAT THIS BRANCH CONTAINS

Diagnosis only. **No fix, because there is no defect.** Zero changes under
`src/`, `include/` or `config/`, so the byte gate is untouched by
construction; solo and adventure are unchanged because both probes return on
their first line when their env knob is unset.

* `port/hal/input_probe.cpp` -- `port_probe_vs_mercy` and
  `port_probe_mercy_hit`, plus `mercy_anim_clock`; read-only, env-gated.
* `port/tests/walk_window.cpp` -- two calls in the per-frame probe block,
  before the actor tick.
* `port/tools/vsmercy_probe.py` -- the three-pass driver.
* `status/VSMERCY.md` -- this file.

Proofs: battery, and abicheck over the rebuilt objects -- results recorded
below on completion. Evidence bundle `C:\tmp\vsmercy-out\`.
