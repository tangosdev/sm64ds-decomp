# Player / Bowser / SpikeBomb field provenance

Why each `unk_NNN` in these headers got the name it has. Every row cites a matched
body in `src/` and what that body does with the slot; a slot with no row is still
`unk_` on purpose, and the last section says why.

Field NAMES cannot change codegen, so every rename here is byte-neutral by
construction -- but it is only *true* if the evidence says so, and a wrong name is
worse than `unk_`, because the next reader will believe it. Everything below was
read off a body that reproduces the cartridge.

## The shadowing trap, first, because it cost a whole build

`Player` derives from `dActor_c`, and `dActor_c` already owns `mPrevPosX/Y/Z`
(0x068) and `mPrevAngleX/Y/Z` (0x092). Naming Player's own 0x548 snapshot
`mPrevPosX` compiles cleanly and *silently rebinds every unqualified `mPrevPosX`
in every Player method to the derived member* -- 201 files include `Player.h`.
The measured cost of doing that: `reproducing` fell 11,059 -> 6,641,
`module fidelity` 106/106 -> 20/106, and the linked image grew 320 bytes.

So: before naming a Player field `mFoo`, grep `include/dActor_c.h`,
`include/dBase_c.h` and `include/fBase_c.h` for `mFoo`. The compiler will not warn
you. The names below were checked that way; that is why 0x548 is `mPreClsnPos*`
and not `mPrevPos*`.

## Player -- named this pass

`Player::Behavior` (`src/_ZN6Player8BehaviorEv.cpp`) is the single richest witness:
it is the per-tick driver, so it holds the snapshot writes and the whole timer
countdown block. Where a row cites "Behavior" that is the file meant.

| offset | name | evidence |
| --- | --- | --- |
| 0x35c | `mGrabbedByActor` | `St_Grabbed_Cleanup` reads it, checks the actor's id word for 0xbf and calls `Player::DropActor` **with that actor as `this`** -- so it is the actor holding the player, not `mHeldObj` (0x358) which is what the player holds. `St_Grabbed_Main` drops through the same slot; `St_Walk_Init` and `St_NoControl_Init` clear it. |
| 0x548 | `mPreClsnPosX` | Behavior stores `mPosX/Y/Z` into 0x548/0x54c/0x550 after the state's `mMain` has run and immediately before the collision calls `func_ov002_020bf36c(this, &mdCcAcPos_c)` / `func_ov002_020bf13c` -- i.e. the position after movement, before collision push-back. `St_Talk_Init` and `St_CeilingGrate_Main` copy all three back into `mPos*`, undoing the push-back. Two functions, opposite directions, the same three slots. |
| 0x54c | `mPreClsnPosY` | as above; also read by `St_Swim_Main` (`mPosY = mPreClsnPosY - 0x8000`). |
| 0x550 | `mPreClsnPosZ` | as above. |
| 0x628 | `mParticle1` | Passed as the *existing-handle* first argument of `Particle::System::New` and reassigned from its result, in `St_BurnFire_Main`, `St_BurnLava_Main`, `St_YoshiPower_Main` and `St_Electrocute_Main`. `ChangeState` copies `mParticle2` into it. Same shape as the already-named `mParticle2` at 0x62c. |
| 0x630 | `mParticle3` | Same `Particle::System::New` handle shape, in `St_Swim_Main` (via `func_02022d44`) and `St_Electrocute_Main`; `ChangeState` zeroes it and copies it back into `mParticle2`. Third of the three consecutive handles 0x628/0x62c/0x630. |
| 0x6a2 | `mPrevAreaId` | Behavior: `if (mPrevAreaId != mAreaId) { mPrevAreaId = mAreaId; mMouthHoldTimer = 0; }`. `InitResources` seeds it from `mAreaId`. Compare-then-store against a base field is a previous-value latch. |
| 0x6aa | `mPunchKickCooldown` | `St_PunchKick_Main` gates on `== 0` at the top and re-arms it to 0x10 on the way out; Behavior decrements it with `DecIfAbove0_Short`. Only that state touches it. |
| 0x6ae | `mPowerupTimer` | `InitMetalWario` and `InitVanishLuigi` both arm it to 0x258 (600 frames, the cap duration); Behavior counts it down and, on reaching 0, calls the three teardown helpers `func_ov002_020e032c` / `_020bdef0` / `_020bdd9c` -- the same three `CleanupResources` calls to drop the powerup. |
| 0x6b0 | `mCrouchTimer` | Armed to 0xa by `St_Crouch_Main` and by nothing else; Behavior decrements. |
| 0x6b4 | `mHoldHeavyTimer` | Armed to 0xa by `St_HoldHeavy_Main` and by nothing else; Behavior decrements. |
| 0x6b8 | `mWalkTimer` | Armed to 0x10 by `St_Walk_Init` and by nothing else; Behavior decrements. What it gates is not in matched code, so the name claims only ownership and shape, both of which are witnessed. |
| 0x6c6 | `mMouthHoldTimer` | Behavior: on reaching 0 **and** `mObjInMouth != 0` it calls `func_ov002_020d6790` (the same spit-out helper `St_YoshiPower_Cleanup` calls when the mouthful is not a player); the area-change latch above resets it; `St_YoshiPower_Main` arms it to 0x5a. Gated on `mObjInMouth`, so it is that object's hold time. |
| 0x6c8 | `mTeleportTimer` | Armed to 0x3c by `St_Teleport_Main`; Behavior clears the global `data_0209f284` when it reaches 1. Only the teleport state touches it. |
| 0x6d0 | `mMegaKillCount` | `Player::IncMegaKillCount` pre-increments it **as a `u16`** and passes the new value straight to `dActor_c::Spawn(0x14b, <it>, ...)` as the spawn's param -- the escalating mega-cap kill combo. Widened from the declared `u8`+pad by that increment. |
| 0x6d4 | `mPrevDesiredAngleY` | Behavior copies `mDesiredAngleY` here immediately before overwriting `mDesiredAngleY` from the controller table; `St_GrabBowserTail_Main` reads the difference `(mDesiredAngleY - mPrevDesiredAngleY)` as this frame's stick swing. |
| 0x6d6 | `mPreClsnAngleY` | Behavior stores `mAngleY` in the same pre-collision block as 0x548; `St_SwingPlayer_Init` computes `mAngleYSpeed = mAngleY - mPreClsnAngleY`. Not `mPrevAngleY` -- see the shadowing section. |
| 0x6db | `mBodyModelId` | `Render` passes it as the `a` argument of `Player::GetBodyModelID`, indexes the body-model pointer array at 0xdc with the result, indexes `mTexSeqBody[]` with it directly (`this + 0x1dc + it * 0x14`), and special-cases `it == 3`. |
| 0x6dc | `mPrevCharacter` | `SetNewHatCharacter` saves the incoming `param1` (the displayed character) here, sets `param1 = mHatCharacter` for the duration of `func_ov002_020e6350`, then restores `param1` from it. `InitMetalWario` runs `TurnOffToonShading` over both `mHatCharacter` and this slot; `CleanupResources` uses it as a model index. |
| 0x6e6 | `mStatePhase` | 36 sites, the most-referenced `unk_` in the header. Zeroed by ~12 `St_*_Init` bodies; `switch`ed over 0/1/2 by `St_Hurt_Main`, `St_ButtSlide_Main`, `St_StomachSlide_Main`; tested `!= 2` by `St_Squish_Main` and `== 0` by `St_PunchKick_Main` and `St_Wait_Main`. A sub-step *within* the current state, reset on entry -- distinct from `mStateStep` (0x6e3) and `mStateWork` (0x6e5), which `St_EndingFly_Init` copies between (`mStateWork = mStatePhase`). `St_InYoshiMouth_Main` reuses it as a plain counter (`+= d`, exits at `>= 0x1e`). |
| 0x6e8 | `mTeleportId` | `St_Teleport_Main`: `GetTeleportDestObj((u8)(mTeleportId - 1))`. Sole use, and it names itself. |
| 0x6fb | `mIsVanish` | `InitVanishLuigi` sets it to 1 and nothing else writes it; `St_Wait_Init` and `St_Wait_Main` test it in the same expression as `mIsMetal` (`if (mIsMetal != 0 \|\| mIsVanish != 0)`), i.e. "wearing a cap power". `daBakubaku_c::Behavior` skips its collider update when the closest player has it set -- the vanish cap phasing through. |
| 0x6fc | `mPlayerTexFrame` | `Render` writes `mTexSeqPlayer[0].currFrame = it << 12` and `mTexSeqPlayer[1].currFrame = it << 12`. An integer frame index for the already-named face/eye texture sequences. |
| 0x715 | `mUseFarCamera` | `InitResources`: `if (StartWithFarCamera() != 0) mUseFarCamera = 1;`. `St_CameraZoom_Cleanup` clears it. |
| 0x71a | `mHasNoCap` | `Player::HasNoCap()` returns `mHasNoCap != 0` as its final expression, after the save-data and state checks fall through. `InitMetalWario` and `St_NoControl_Main` clear it. Named after the accessor that exposes it. |
| 0x73c | `mCapFlags` | `IsCollectingCap` reads it as `u16` and answers `v != 0 && (v & 7) < 5` -- low three bits are a stage counter; `SetNewHatCharacter` ORs in 0x8000 when the new hat is not the real character; `InitMetalWario` and `SetRealCharacter` clear the whole word. A flags word, not a scalar. |

## Player -- deliberately left `unk_`

- **0x69e** -- an `s16` used as an angle by three unrelated states (`St_Swim_Main`
  stores `mAngleZ` into it, `St_Balloon_Init` seeds 0x100, `TryEnterStarDoor`
  stores its `kind` argument and then passes it as the rotation to
  `Vec3_RotateYAndTranslate`). Three lifetimes, no common meaning; a single name
  would be a claim none of the three supports.
- **0x716, 0x717, 0x726** -- every matched site *writes* them (`ChangeState`
  clears, various `St_*_Init` set). Nothing in matched code reads them, so there
  is no evidence of what they gate.
- **0x6ac, 0x6b2, 0x6b6, 0x6ba, 0x6bc, 0x6c2, 0x6c4** -- countdown timers whose
  only matched site is Behavior's `DecIfAbove0_Short` block. Their shape is
  witnessed (`u16`, decrement-to-zero); their owner is not, because nothing
  matched arms them. `0x6be` and `0x6c2` additionally fire one unnamed helper at
  zero, which is not enough to name either.
- **0x744..0x74c and 0x750..0x758** -- two `Vector3`s, already documented in the
  header. 0x750 is the anchor and 0x744 the point `Vec3_RotateYAndTranslate`
  derives from it (`TryEnterStarDoor`, `ShowMessage2`, `JumpIntoBooCage`), but
  what the pair *means* is not settled, so the widths stay and the names do not.
- **0x760, 0x762, 0x764, 0x766** -- four `s16`s that `ApproachLinear2` decays
  toward 0 (`St_YoshiPower_Main`, `St_Talk_Main`) and that `St_Shell_Main` and
  `St_Land_Main` set to 0x1000 / 0x4000. Almost certainly a render tilt/squash
  group, but no matched body consumes them, so "almost certainly" is where it
  stops.
- **0x64c, 0x650** -- both seeded to the sentinel 0x80000000 by `InitResources`
  and compared against `mPosY` with different slacks (`- 0xc8000` in
  `St_Swim_Main`, `- 0x64000` in `St_Wait_Init`). Y thresholds of some kind;
  which surface they record is not witnessed.
- **0x578, 0x57c** -- `InitResources` allocates a 0x32-element container with a
  0xc stride and `Vector3::~Vector3` as the element destructor at 0x578, and a
  bare 0x32-byte block at 0x57c; `CleanupResources` frees both. Fifty `Vector3`s
  plus fifty bytes is suggestive of a position trail, but nothing matched reads
  either back.
- **0x6f4, 0x6ee, 0x742, 0x722..0x728, 0x743** -- state-local scratch in
  `St_YoshiPower_*`, `St_Talk_*` and `St_Wait_Cleanup` with writes but no
  witnessed consumer.
- **0x0e0, 0x0ec, 0x154, 0x158, 0x160, 0x27c, 0x28c** -- these are not scalars at
  all. `CleanupResources` walks them as arrays (`this + i*4 + 0xdc`,
  `+ 0x154`, `+ 0x27c`, `+ 0x28c` for `i < 4`, plus `i + 4` for the 0x154 and
  0x28c runs) and `Render` and `TurnOffToonShading` index the same runs. Turning
  them into real arrays is the right fix and is a structural change, not a
  rename, so it is left for a pass that can byte-verify each indexing site.

## Bowser

Five matched functions only, so the evidence is thin and most of the header is
still `unk_`. Both halves of `Bowser.h` carry every rename: the file has a
`#else` C twin that spells the whole layout flat, and a name changed in only one
half is a spelling that compiles nowhere and that no gate reports.

| offset | name | evidence |
| --- | --- | --- |
| 0x3b0 | `mHomePosX` | `Bowser::InitResources` copies `mPosX/Y/Z` into 0x3b0/0x3b4/0x3b8 once, in the same block that sets `mVertAccel` and `mTerminalVelocity`. Saved-at-spawn position. |
| 0x3b4 | `mHomePosY` | as above. |
| 0x3b8 | `mHomePosZ` | as above. |
| 0x3ec | `mDistToTarget` | `Behavior`: `mDistToTarget = Vec3_HorzDist(&mPosX, &target->mPosX)` when `ClosestPlayer()` returned something, and `~0x80000000` (INT_MAX, "infinitely far") when it did not. |
| 0x3f8 | `mAnimSpeed` | `InitResources` sets 0x1000 alongside the three unit scales; `Behavior` copies it into `mModelAnim.speed` every frame before `Advance()`. |
| 0x406 | `mAngleToTarget` | `Behavior`: `Vec3_HorzAngle(&mPosX, &target->mPosX)`, falling back to `mAngleY` when there is no target -- the same if/else arms as `mDistToTarget`. |

Left `unk_` in Bowser, with the reason:

- **0x3a8** -- `InitResources` stores word +4 of the actor it spawns with id
  0x116. Offset 4 of an `fBase_c` has no name in the tree either, so naming this
  would be naming the same unknown twice.
- **0x414, 0x416** -- `param1 & 3` and `(param1 >> 2) & 1`. Configuration bits
  read off the spawn parameter, but no matched body consumes either, so which
  configuration is a guess.
- **0x41c** -- `Bowser::Render` early-outs on `< 8`, the same guard SpikeBomb's
  `mOpacity` gets. That is suggestive and it is not evidence: nothing matched
  writes 0x41c, so unlike SpikeBomb there is no 0xff to confirm it.
- **0x42b** -- `Behavior` clears it once `dActor_c::FindWithActorID(0x10d, 0)`
  stops finding anything. A latch tied to actor 0x10d; what it gates is not in
  matched code.
- **0x423..0x42a, 0x444..0x450, 0x3fc, 0x40c** -- written once each by
  `InitResources` and read by nothing matched.

## SpikeBomb

| offset | name | evidence |
| --- | --- | --- |
| 0x170 | `mStateIndex` | `InitResources` zeroes it; `Behavior` uses it as `data_ov060_0211b1d8[mStateIndex * 2]` -- a stride-8 `{ptr, adj}` pointer-to-member table, dispatched with the same ARM/Itanium pmf sequence `Player::ChangeState` uses. An index into a state table, not a flag. `Render` also early-outs while it is non-zero. |
| 0x174 | `mHomePosX` | `InitResources` copies `mPosX/Y/Z` into 0x174/0x178/0x17c, then raises the Y copy by `unk_184 >> 3`. Saved-at-spawn position with a vertical offset. |
| 0x178 | `mHomePosY` | as above; it is the one the `>> 3` term is added to. |
| 0x17c | `mHomePosZ` | as above. |
| 0x1a8 | `mSlotIndex` | `InitResources`: `mSlotIndex = AddSpikeBomb(this)`, and `src/AddSpikeBomb.c` returns the index of the first free slot in the eight-entry global `data_0209f3a4` (or -1). `src/ClearSpikeBomb.c` takes that index back. |
| 0x1ae | `mOpacity` | `InitResources` sets 0xff; `SpikeBomb::Render` returns early on `< 8`. Full alpha at spawn plus a "too faint to bother drawing" guard is an opacity byte, and 0xff is not a plausible state id or counter. |

Left `unk_` in SpikeBomb: **0x180** (`Vec3_HorzLen` of the spawn position, i.e. a
distance from the world origin -- plausibly an orbit radius, but nothing matched
reads it back) and **0x184** (`0x2ee000`, whose only use is the `>> 3` term added
to `mHomePosY`).
