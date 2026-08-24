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
  **DONE -- see "The four arrays" below.** Six of the seven are gone; 0x0ec is
  not part of any of them and is still a scalar.

## The four arrays -- the structural half, and it IS byte-neutral

The previous pass handed this on as "the right fix, left for a pass that can
byte-verify each indexing site". Every site was verified individually. **All of
it is byte-neutral**, so the measurement to record is a positive one: on this
compiler, indexing a real member array of pointers emits the same code as
hand-written `*(T **)((char *)this + base + i * 4)`, in a loop and out of it.

Four runs, and each one's length is pinned by two independent witnesses -- the
`CleanupResources` walk and the padding that had to be deleted to make room:

| offset | declaration | length is pinned by |
| --- | --- | --- |
| 0x0dc | `ModelAnim *mBodyModels[4]` | `CleanupResources` `i < 4`; closes at 0x0ec, the standalone pointer that was already there |
| 0x154 | `ModelAnim *unk_154[8]` | `CleanupResources` `i < 4` **and** `i + 4`; closes exactly at `mModelAnim4` (0x174) |
| 0x27c | `s32 unk_27c[4]` | `i < 4`; closes exactly at 0x28c |
| 0x28c | `s32 unk_28c[8]` | `i < 4` **and** `i + 4`; closes exactly at `mShadowModel` (0x2ac) |

The element type of the two model runs is `ModelAnim`, and the matched bodies pin
every part of that claim rather than just the size: they reach `+0x08` (Model's
`ModelComponents`, handed to `TextureSequence::Update`), `+0x14` (`Model::mat4x3`),
`+0x50` (the `Animation` base, handed to `Animation::Advance`) and `+0x58` (that
base's `currFrame`); they call `Model::SetPolygonMode` and `Model::Render` on the
same pointer; and they call vtable slots `+0x14` and `+0x18`, which in
`_ZTV9ModelAnim` are `Render` and `Virtual18`. `CleanupResources` calls slot 1 --
the deleting destructor -- on each element.

The two `s32` runs are parallel to the model runs, one word per model, and that
pairing is witnessed rather than inferred: `TurnOffToonShading` passes
`unk_27c[j]` with `mBodyModels[...]`, `unk_28c[j]` with `unk_154[j]` and
`unk_28c[j + 4]` with `unk_154[j + 4]`, all three to `func_ov002_020e6b74`, which
walks the model's material records writing one word of the array into each
record's `+0x1c`. So each element is an allocated per-material word buffer owned
by the model at the same index -- `CleanupResources` frees them with
`func_0203cbc0` over the same `i` / `i + 4` pattern. **They are still `unk_`**:
nothing matched allocates or fills either run, so what the word means is unread.
Naming the shape was possible; naming the meaning was not.

`unk_154`'s index range has one wrinkle worth writing down before someone
"fixes" it. `Render` indexes it with `func_ov002_020becf4(mBodyModelId, 1)`, and
that helper can return 8 or 9 -- the "no model" sentinels. `Render` loads the slot
*first* and only then tests `i != 9 && i != 8`, so the ROM itself reads one or two
words past the array, into `mModelAnim4`, and discards the result. That is
reproduced as written; it is what the cartridge does.

Verified byte-for-byte with `build_pin`, one call per function, all
`(True, '2004/b56')`:

```
_ZN6Player16CleanupResourcesEv    _ZN6Player6RenderEv     _ZN6Player18TurnOffToonShadingEj
_ZN6Player18St_YoshiPower_MainEv  _ZN6Player15St_DeadHit_MainEv
_ZN6Player13St_Crawl_MainEv       _ZN6Player17St_NoControl_MainEv
_ZN6Player16St_BurnFire_MainEv    _ZN6Player18St_CameraZoom_MainEv
```

Two sites were deliberately NOT collapsed:

- `St_CameraZoom_Main` reads the run through `*(volatile int *)((char *)&mBodyModels
  + id * 4)`. The `volatile` is load-bearing there (it is the round-trip lever, see
  the "When a rename stops being byte-neutral" section) and `&mBodyModels` still
  compiles unchanged against the array, so it was left exactly as it was.
- 0x0ec and 0x1d8 stay scalars. `CleanupResources` destroys them in their own
  block, outside the `i < 4` loop, and they sit one on each side of a `ModelAnim`
  member (`mModelAnim3` at 0x0f0, `mModelAnim4` at 0x174) rather than inside a run.
  Both are `ModelAnim *` by the same slot-1/slot-5 evidence, but neither is part
  of an array and neither was retyped this pass.

`build/eligible-names.txt` is byte-identical across the whole change (11,065
lines both sides), `check_header_offsets` reports `0 mismatched, 0 unparsed,
struct spans 0x768` and the full `rombuild` stays at 11,059 reproducing / 0
mismatching / 106-106 exact.

One number does move and it is expected: `check_header_offsets` counts 173
commented fields in `Player.h` where it counted 176. Three of the fields it used
to check -- `unk_0e0`, `unk_158`, `unk_160` -- are now interior elements of
`mBodyModels` and `unk_154` and have no offset comment of their own. The file did
not fall out of the gate: the struct still spans 0x768 and nothing is unparsed.

## The six shadow-class files

Six Player method sources did not include `Player.h` at all. Each carried its
own one-method stand-in:

```
class Player {
public:
    int St_Climb_Init();
    int IsAnim(unsigned int anim);
    void SetAnim(unsigned int animID, int a, int b, unsigned int c);
};
```

so every field access in them had to be spelt as a raw offset off a
`char *self = (char *)this`, and none of the naming work above reached them.
All six now include the real header, and 94 of their offset pokes are named
member accesses.

The reason they had stand-ins is real and is preserved: the ROM's
`Player::SetAnim` takes a by-value `Fix12<int>`, which mwccarm passes
differently from an `int`, so declaring the true signature breaks the byte
match (notes/mwccarm-codegen.md 6az). That is why `SetAnim` is still called
through `extern "C" _ZN6Player7SetAnimEji5Fix12IiEj` and is still not declared
in `Player.h`. What the stand-ins did NOT need was to hide the fields.

Six method declarations were added to `Player.h` for this -- `St_Climb_Init`,
`St_Fly_Main`, `St_Land_Init`, `St_WindCarry_Main`, `St_CrazedCrate_Main` and
`St_DizzyStars_Cleanup`, each the missing half of a pair the header already
declared. All six are non-virtual, so none of them can become the key function
(the destructor is declared first and holds that role); `build/eligible-names.txt`
is byte-identical across the change.

## When a rename stops being byte-neutral

Five sites in this pass had to be reverted, all the same shape:

```
*(u8 *)(((int)((char *)this) + 0x6e3)) = *(u8 *)(((int)((char *)this) + 0x6e3)) + 1;
```

Written as `mStateStep = mStateStep + 1;` mwcc common-subexpressions the field
address across the load and the store, drops an instruction, and the function
changes size -- `build_pin` reports `999 word(s) differ`, which is its way of
saying the sizes do not match. It is per-site, not per-function: the same file
keeps every other collapsed access.

The five: `St_LedgeHang_Main` and `St_SlideKick_Main` (0x6e3), 
`St_StuckInGround_Main` (0x6e5), and `St_Fly_Main` twice (`mHorzSpeed += 0xe00`
at 0x098 and `mStateArg + 1` at 0x70c).

So the rule for this lever is not "naming is free". Naming is free; *deleting a
cast* is not, and collapsing a poke deletes casts on both sides of a
read-modify-write.

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

### mTargetPlayer is a pointer, and typing it is byte-neutral

`mTargetPlayer` (0x3a0) was declared `s32` and every read of it was spelt
`*(dActor_c **)((char *)&mTargetPlayer)` -- three of those in `Behavior`, on top of
an `(int)` cast on the store. It is now `dActor_c *`, and
`_ZN6Bowser8BehaviorEv` still reproduces byte-for-byte under 2004/b56, as does
`_ZN6Bowser13InitResourcesEv`, which zeroes it. So a pointer-typed member does
*not* change how mwcc loads it here; the four casts were pure noise and are gone,
and `Vec3_HorzAngle` / `Vec3_HorzDist` now take `(Vector3 *)&mTargetPlayer->mPosX`
instead of `(char *) ... + 0x5c`.

Two details that are not free and are worth copying:

- It is `dActor_c *`, not `Player *`, even though `dActor_c::ClosestPlayer()`
  returns `Player *`. `Bowser.h` cannot include `Player.h`, so `Player` is only
  forward-declared, and mwcc rejects the implicit `Player *` -> `dActor_c *`
  conversion outright ("illegal implicit conversion") because it cannot see the
  derivation. The store therefore keeps ONE cast, `(dActor_c *)ClosestPlayer()`,
  which is exact: `dActor_c` is the base at offset 0, so no adjustment is
  involved.
- The `#else` C twin of `Bowser.h` has no `dActor_c` declaration in scope, so
  there the slot is `void *` -- same width, same offset, and the two halves still
  agree.

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

## Player -- named in the structural pass

Three more, all found while chasing the arrays.

| offset | name | evidence |
| --- | --- | --- |
| 0x560/0x564/0x568 | `mWallNormalX/Y/Z` | The exact counterpart of `mFloorNormal*` three words earlier, and written the same way: `func_ov002_020c25a8` calls `SurfaceInfo::CopyNormalTo(dBgCh_Actr::GetWallResult(&mMeshClsn) + 4, &wn)` and stores `wn.x/y/z` into the three slots, then pushes the actor back out along it (`mPosX -= mWallNormalX * 2`, `mPosZ -= mWallNormalZ * 2`). Seven bodies read the X/Z pair back as `cstd::atan2(mWallNormalX, mWallNormalZ)` to recover the wall's facing -- `St_Shell_Main`, `St_OnWall_Main` (twice), `St_Balloon_Main`, `St_CrazedCrate_Main`, `func_ov002_020c2138`, `func_ov002_020dd2f4`, `func_ov002_020e28d4`. 0x564 was declared padding until that middle store was disassembled, which is exactly how 0x554 and 0x55c got here. |
| 0x719 | `mKeyModelId` | `CleanupResources` passes it to `UnloadKeyModels(i)` under `mLoadedResourceFlags & 0x10`, and that function (`src/UnloadKeyModels.cpp`) indexes two eight-entry `SharedFilePtr` tables with it and releases both. `St_LevelEnter_Init` seeds it with -1, which `UnloadKeyModels`'s `if (i >= 8) return` treats as "nothing loaded". The same argument slot is `mState` in `Key::CleanupResources` and `v - 7` in `Door::CleanupResources`, so it selects WHICH key model, not how many. |
| 0x6f7 | `mSwimMusicPushed` | A latch on a music push. `St_Swim_Main` sets it to 1 immediately after `func_ov002_020bd928(this, 0x33)` and clears it immediately after `func_ov002_020bd8c0(this, 0x33)`; `St_Swim_Cleanup` does nothing unless it is set, and then clears it and calls `func_ov002_020bd8c0(this, 0x33)`. The two helpers are `Sound::SetMusic` / `Sound::EndMusic` wrappers around the track words at 0x678/0x67c/0x680, so what is latched is "this state has a temporary track pushed and still owes the pop". Only the Swim states touch it. |

## IceSlideManager

Fourteen `unk_` down to two, and the split is worth stating plainly because it is
two different kinds of naming.

`0x000..0x0d4` is `dActor_c`'s layout written out FLAT -- `IceSlideManager` does
not derive from `dActor_c`, it mirrors it. So `mPosX/Y/Z`, `mPrevPosX/Y/Z`,
`mClipOffsetY`, `mClipRadius`, `mClipDistance`, `mFarDistance`, `mClipResult` and
`mDeathTableID` are **copied from `include/dActor_c.h` at the matching offset**,
not independently evidenced here. That is a weaker claim than the rest of this
file and the header now says so. It is also why none of them can shadow anything:
there is no base class to shadow.

Only two slots are the actor's own, and both are witnessed:

| offset | name | evidence |
| --- | --- | --- |
| 0x0d4 | `mKillTimer` | `InitResources` seeds it with 0x78 (120 frames) and nothing else arms it; `Behavior`'s state 1 runs it down with `DecIfAbove0_Short` and, at zero, plays one more sound and calls `dActor_c::KillAndTrackInDeathTable`. Armed once, expires once, and its expiry IS the kill. |
| 0x0d6 | `mState` | `Behavior` switches on it over exactly `{0, 1}`. State 0 waits for `DistToCPlayer() < 0x180000`, plays a sound and increments it; state 1 is the countdown above. A two-state machine, not a flag -- and the increment is spelt through a byte pointer at `this + 0xd6`, which is why the slot is `u8`. |

The rename carried into `src_tu/actors/IceSlideManager.cpp`, which keeps its own
TU-local shadow `dActor_c`; `check_src_tu_compiles` is green on all 72.

## UpDownLiftBbh

The same shape: `0x000..0x0d4` is the flat `fBase_c -> dBase_c -> dActor_c`
layout, so `pauseFlags` came from `include/fBase_c.h` and `mPrevPosX/Y/Z`,
`mClipOffsetY`, `mClipRadius`, `mClipDistance`, `mFarDistance`, `mClipResult` and
`mDeathTableID` came from `include/dActor_c.h`, at the matching offset. Ten
names, no independent evidence, and the header says so.

Four slots inside that range stay `unk_`, and one of the reasons is not the usual
one:

- **0x010, 0x011, 0x012** -- `fBase_c` does not name them either. Copying across
  would be inventing a name, not importing one.
- **0x092, 0x096** -- these are `dActor_c`'s `mPrevAngleX` and `mPrevAngleZ`
  slots, and `InitResources` reads them as UNSIGNED shaft measurements:
  `mBottomY = mTopY - (unk_092 << 12)` and, on the one variant,
  `mTopY = mPosY + (unk_096 << 12)`. Nothing matched WRITES either slot, so
  whether they hold the base's angle snapshot or an actor-specific reuse of the
  same words is not settled by anything that reproduces the cartridge. Naming
  them either way picks a side on no evidence.

Two of the actor's own stay `unk_` for the ordinary reason (write-only in matched
code), but **0x349 carries an observation that should not be lost**:
`InitResources` stores only 0 or 1 into it -- 1 for `actorID == 0x83` -- and then,
four statements later, tests it for `== 2`. The branch that raises `mTopY` by
`unk_096` is unreachable in the shipped ROM. It is reproduced as written because
the cartridge contains it, and the header now records that so nobody "fixes" the
comparison.
