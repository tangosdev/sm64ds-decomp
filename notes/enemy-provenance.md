# `dEnemyBase_c` family — field provenance (set A)

Every name below is taken from what a **matched** body actually does with the offset.
Where a body does nothing but store a constant that is never read back, the field is
left `unk_NNN` on purpose: a wrong name is a claim the next reader will trust.

Naming is byte-neutral by construction; every collapse of a raw `this`-offset poke onto
a named member in this note was re-verified with `tools/build_pin.py`'s `verify` under
2004/b56 before it was kept.

Convention: instance members `mFoo`.

---

## `Unagi` (`include/Unagi.h`, ov016)

| offset | new name | evidence |
| --- | --- | --- |
| 0x34c | `mState` | `src/_ZN5Unagi8BehaviorEv.cpp` loads it as a pointer to a descriptor and calls the pointer-to-member-function at `+8` through `this`; the same file compares it against `&data_ov016_02114dbc`, and `func_ov016_02111bf0(this, &data_ov016_02114d8c)` in `src/_ZN5Unagi13InitResourcesEv.cpp` is the setter. |
| 0x3f0/0x3f4/0x3f8 | `mHomePosX/Y/Z` | `InitResources` writes them from `mPosX/mPosY/mPosZ` at spawn, then the `mVariant == 2` branch subtracts `0x80000` from `mHomePosY` and writes it back into `mPosY`. Written from the position and read back into the position: a home/spawn point. |
| 0x40c | `mPathNodeCount` | `InitResources`: `mPathNodeCount = PathPtr::NumNodes(&path1)`. |
| 0x410 | `mPathNodeIndex` | `InitResources` sets it to 1, then to 8, and clamps it to 4 when `mPathNodeIndex >= mPathNodeCount`. Only meaning consistent with being bounded by the node count. |
| 0x414 | `mStarParam` | `InitResources`: `mStarParam = (param1 >> 0xc) & 0xf`. Its only two reads are `mStarParam \| 0x50` (`InitResources`) and `mStarParam \| 0x40` (`Behavior`), each the spawn parameter of actor `0xb2`, the star. |
| 0x418..0x426 | `s16 mSegmentAngle[8]` | `InitResources` zeroes indices 0..6 through the base of 0x418 and then element 7 at 0x426 explicitly; `src/_ZN5Unagi6RenderEv.cpp` reads elements 1..6, multiplies each by `data_ov016_02114908[i].angleScale` and adds the result into the bone's rotation word at `bone + 0x1e`. Eight `s16`s, driving one bone angle each. |
| 0x428/0x42a/0x42c | `mInitAngleX/Y/Z` | last three statements of `InitResources`: copied verbatim from `mAngleX/Y/Z`. Named for the capture, and `func_ov016_02111534` does read all three back, restoring them into `mPrevAngleX/Y/Z`. |
| 0x43c | `Vector3 mStarPos` | `Behavior` passes `&mStarPos` to `Vec3_Dist` against the closest player's `+0x5c` (a position), passes `mStarPos` by reference as the spawn position of actor `0xb2`, and every frame copies its three words into the tracked star's `+0x5c/+0x60/+0x64`. Three consecutive words used only as a world position, and only ever the star's. |
| 0x448 | `Vector3 mSegmentPos[7]` | already typed `Vector3[7]` — the ROM destroys it with `__destroy_arr(ptr, 7, 0xc, Vector3::~Vector3)`. `InitResources`' tail loop writes the actor's position into all seven. Renamed from `unk_448` to match `mSegmentAngle`; the loop's `char*` walker was replaced with `mSegmentPos[i].x/y/z`, byte-neutral. |

Left `unk_`:

- **0x3c0** — no matched body touches it.
- **0x416** — `Behavior` adds `0x1000` to an `s16` there once per frame while a tracked star
  exists. It sits inside `pad_415`, and one increment is not enough to say whether it is an
  angle, a phase or a counter, so the raw poke stays.

Byte-neutral source cleanups in this class: `mBlendModelAnim.speed = 0x1000;` replaced
`*(s32 *)((char *)&mBlendModelAnim + 0x5c) = 0x1000;` (offset 0x5c of `BlendModelAnim` is
`speed`, see `include/BlendModelAnim.h`); the `off43c`/`off` scratch integers in `Behavior`
were dropped in favour of `mStarPos`.

---

## `KingBobOmb` (`include/KingBobOmb.h`, ov078)

| offset | new name | evidence |
| --- | --- | --- |
| 0x420 | `void *mState` | `src/KingBobOmb_SetState.cpp` is exactly `c->pp = p; if (*c->pp) return (c->**c->pp)();` with `pp` at 0x420, and `src/_ZN10KingBobOmb8BehaviorEv.cpp` compares the same word against four ov078 state tables (`data_ov078_0212703c`, `_0212707c`, `_021270bc`, `_021270fc`) to pick its per-state path. Previously unnamed inside `pad_420`. |
| 0x4d4/0x4d8/0x4dc | `mArenaPosX/Y/Z` | `InitResources` stores the Fix12 triple `0xb1d000 / 0x1060000 / 0xfee15000` — a fixed world point. `Behavior`'s only read is `(mArenaPosY - 0x28000) > mPosY`, which forces `SetState(data_ov078_021270bc)`, the same state that switches position updates to `UpdatePosWithOnlySpeed`: a "fell below the arena floor" test. The ov078 handlers read the whole triple: `func_ov078_02123d3c` loads all three into a `Vector3`, and `func_ov078_021240a0`/`_021243c0` pass `&mArenaPosX` to `Vec3_Dist` and `Vec3_HorzAngle` against `mPos`. |
| 0x4e0/0x4e4/0x4e8 | `mHomePosX/Y/Z` | `InitResources` writes them from `mPosX/mPosY/mPosZ` at spawn. |
| 0x4f8 | `mInitAngleY` | `InitResources`' `*(short*)(this + 0x400 + 0xf8) = mAngleY;`, now spelled `mInitAngleY = mAngleY;`. Previously unnamed inside `pad_4ec`. |
| 0x4fc | `mAnimSpeed` | `Behavior`'s only read is `mBlendModelAnim.speed = mAnimSpeed << 0xc`, i.e. it is the animation speed in whole units, converted to Fix12 on the way in. `BlendModelAnim`'s `speed` is at +0x5c (`include/BlendModelAnim.h`), and 0x2cc + 0x5c = 0x328, the address the raw poke used. `InitResources` sets it to 1. |
| 0x504, 0x505 | `mTimer504`, `mTimer505` | `Behavior` passes both to `DecIfAbove0_Byte` once per frame. The offset stays in the name because the countdown is all the evidence says: nothing in a matched body reads either back. Previously unnamed inside `pad_504`. |
| 0x507 | `mStarTracked` | `InitResources`: `mStarTracked = dActor_c::TrackStar(this, mStarID, 2);`. |
| 0x509 | `mStarID` | `InitResources`: `mStarID = param1 & 0xf;` and it is the star id argument of `TrackStar`. |

Left `unk_`:

- **0x494** — a pointer, null-checked, and one field at `+0xc8` is tested (`Render`,
  `Behavior`). Neither body says what it points at.
- **0x498** — set to `0x1f` in `InitResources`, never read in a matched body.
- **0x499** — `Behavior` compares it against 1; nothing writes it in matched code.
- **0x4a0** — `InitResources` sets it to `((rand >> 0x1e) & 1) + 1`, so 1 or 2, and no
  matched body reads it.
- **0x500** — `mHealth`. Set to 3 in `InitResources`; `func_ov078_021243c0` decrements it
  by one in the same body that plays the stagger anim and applies the knockback speeds,
  and then latches `+0xb0` when it reaches 0; `func_ov078_021240a0` gates the whole
  chase-the-player branch on `<= 0`. Three throws, exactly as the fight plays.
- **0x424/0x428/0x42c/0x42d** — zeroed by a two-iteration loop in `InitResources` and
  otherwise untouched.

---

## `BowserFire` (`include/BowserFire.h`, ov060)

| offset | new name | evidence |
| --- | --- | --- |
| 0x35c | `mVariant` | `src/_ZN10BowserFire13InitResourcesEv.cpp`: `mVariant = param1 & 7`. It is then the index into both behaviour tables — `data_ov060_0211af74[mVariant]` (called once at init) and `data_ov060_0211afb4[mVariant].pmf` (called every frame in `Behavior`) — and `mVariant == 0` is what disables the collider by setting `mdCcAc_c.flags \|= 1`. |
| 0x364 | `mGroundY` | `InitResources` casts a `dBgCh_Gnd` ray down from the actor's position and stores `rc.clsnY` on a hit, `mPosY` on a miss. |
| 0x374 | `mFrameCount` (`u16`) | zeroed in `InitResources`, incremented by 1 at the top of every `Behavior`. Widened from `s16` to `u16` to match the `unsigned short` the ROM's read-modify-write used, which is now spelled `mFrameCount += 1;`. |

Left `unk_`:

- **0x2cc, 0x37c, 0x380, 0x384, 0x388** — written to 0 (or copied from each other) in
  `InitResources` and never read in a matched body.
- **0x360** — `mShadowRadiusScale`. `func_ov060_02117624` passes `*(0x368) * this`
  as the radius argument of `dActor_c::DropShadowRadHeight`, so the 0x2000 is a
  Fix12 2.0 multiplier on the shadow's base radius.
- **0x370** — an `s32` incremented once per `Behavior`, alongside `mFrameCount`. Two
  free-running counters in one class, and nothing reads either, so there is no evidence
  for which is which; the raw poke stays rather than pick a name at random.
- **0x378** — `(param1 >> 4) & 3`. Provenance without meaning: no matched body reads it.
- **0x379** — `mDropsShadow`. `func_ov060_02117624` returns immediately when it is 0,
  before it drops the shadow at all, so `mVariant != 0` is deciding which variants
  cast one.

---

## `MrBlizzard` (`include/MrBlizzard.h`, ov081)

| offset | new name | evidence |
| --- | --- | --- |
| 0x3f8 | `void *mState` | `src/_ZN10MrBlizzard8BehaviorEv.cpp`'s local shadow places `PMF *pp` at 0x3f8, calls the pointer-to-member-function at `pp[1]` through `this`, and compares the word against four ov081 state tables (`data_ov081_02128e24 / _02128e64 / _02128e84 / _02128e94`). `func_ov081_02125488(this, ...)` in `InitResources` is the setter. Previously unnamed inside `pad_398`. |
| 0x400 | `mCapUniqueID` | `Behavior` spawns actor `0x10d` only when `SaveData::HasPlayerLostCap()` and this word is 0, then stores the spawned actor's unique id (`*(int*)(spawned + 4)`, the same `+4` `Unagi` uses for `mStarUniqueID`) into it. `src/_ZN10MrBlizzard6RenderEv.cpp` hides material 2 when it is non-zero — the head is bare once the cap actor exists. |
| 0x414 | `mInitAngleY` | `InitResources`, immediately after `mAngleY = mPrevAngleY`: `mInitAngleY = mAngleY`. |
| 0x420 | `mPathNodeCount` | `InitResources` (`mType == 0` branch): `= PathPtr::NumNodes()`. |
| 0x424 | `mPathNodeIndex` | passed to `PathPtr::GetNode(pos, index)` and then set to 1 in the same branch. |
| 0x44c | `Vector3 mHomePos` | `InitResources` writes `mPosX/Y/Z` into it at spawn; both it and `Behavior` then pass `&mHomePos` as the spawn position of actor `0xdf`. |

Left `unk_`:

- **0x3fc** — `mUniqueID_3fc`. Not a plain scalar: `func_ov081_02124134` hands it to
  `dActor_c::FindWithID`, shoves the actor it finds (`+0x9c = -0x2000`,
  `+0xa0 = -0x28000`) and then clears it -- exactly what it does one field later with
  `mCapUniqueID`. Which actor it tracks is not evidenced, so the offset stands in for
  the role while the type does not.
- **0x468** — a byte guarding one `func_02012694(0x166, &mCamSpacePos)` call in the death
  path, cleared afterwards. Nothing writes it in a matched body.
- **0x469** — a 0/1/2 machine in the `mType == 3` branch of `Behavior`, driven entirely by
  `HasPlayerLostCap()`, whose only effect is to flip `mType` to 2. Clearly cap-related and
  clearly a state, but three values with no name attached is not enough to pick one.

Byte-neutral source cleanups: `mType`, `mCapUniqueID`, `&mHomePos` and
`mModelAnim.speed = 0x1000` (offset 0x30c + 0x5c = 0x368, `ModelAnim::speed`) replaced the
corresponding raw `c + 0xNNN` pokes in `Behavior`.

---

## `Shark` (`include/Shark.h`, ov090)

| offset | new name | evidence |
| --- | --- | --- |
| 0x370 | `mState` | `src/_ZN5Shark8BehaviorEv.cpp` reads it as a `SharkBehaviorState *` and calls the pointer-to-member-function at `+8` through `this`; `func_ov090_021338b4(this, data_ov090_021345cc)` at the end of `InitResources` is the setter. |
| 0x374 | `Vector3 mClsnOffset` | `InitResources` zeroes all three words and then passes `&mClsnOffset` as the `const Vector3 &` argument of `dCcAcPos_c::Init` — the collider's offset from the actor. |
| 0x38c | `mPathNodeCount` | `InitResources`: `= PathPtr::NumNodes()`. `Behavior` wraps `mPathNodeIdx` to 0 when it reaches this value. |

---

## `PiranhaPlant` (`include/PiranhaPlant.h`, ov084)

| offset | new name | evidence |
| --- | --- | --- |
| 0x428 | `Vector3 mPipeScale` | `src/_ZN12PiranhaPlant6RenderEv.cpp` passes `&mPipeScale` as the `Vector3 *` argument of `mModel.Render` — the same slot `mModelAnim.Render` gets `&mScaleX` in. Twelve bytes of what used to be `u8 unk_428` plus `pad_429[0xb]`. |
| 0x434 | `Vector3 mFirePos` | `InitResources`' tail computes it: `0xe0` along the facing angle out of the shared sin/cos table `data_02082214`, plus `0x37800` above the spawn Y. The file's own header comment already called it "where the plant's fire comes from". |
| 0x440 | `Vector3 mSpawnPos` | `InitResources` writes `mPosX/Y/Z` into it; `Behavior` passes `&mSpawnPos` to `dCcAcPos_c::SetPosRelativeToActor`. |
| 0x44c | `Vector3 mHomePos` | copied wholesale from `mSpawnPos` in `InitResources`; `Behavior` restores `mPosX/Y/Z` from it when `UpdateKillByInvincibleChar` returns 2 (the plant zeroes its scale and goes home). Two separate copies of the spawn point, distinguished by which one is read back into the position. |
| 0x45c | `mClsnEnabled` | zeroed in `InitResources`; its only read in `Behavior` gates `dCc_c::Update` on all three colliders (the `Clear` calls above it are unconditional). |
| 0x468 | `mInitAngleY` | `InitResources`: `= mPrevAngleY`. |

Left `unk_`:

- **0x470** — `mParticleHandle`. `func_ov084_0212f460` stores `Particle::System::New`'s
  return in it and reads it straight back as the `slot` argument of the next call.
- **0x45d** (set to 1), **0x460** (0), **0x464** (`0x7fffffff`), **0x46c** (0),
  **0x474** (0) — written in `InitResources`, never read in a matched body, including
  in the ov084 handlers.
- **0x478** — zeroed in `InitResources` and again whenever `mState` changes, alongside the
  `dEnemyBase_c` counter at 0x100. A per-state something, but nothing reads it.

Byte-neutral source cleanups: `&mWithMeshClsn`, `&mModelAnim`, `&mdCcAc_c1`,
`&mdCcAcPos_c` and `&mSpawnPos` replaced the corresponding `((char*)this) + 0xNNN`
arguments in `Behavior`. `InitResources`' `Vector3 *home = (Vector3 *)((int)this + 0x440)`
stays as it is — the file already documents that the add must sit inside the integer cast.

---

## `FirePiranhaPlantBig` (`include/FirePiranhaPlantBig.h`, ov084)

| offset | new name | evidence |
| --- | --- | --- |
| 0x1f8 | `Vector3 mClsnOffset` | `Behavior` passes `&mClsnOffset` as the `const Vector3 &` of `dCcAcPos_c::SetPosRelativeToActor`. Twelve bytes of what used to be `pad_1f8[0xc]`. |
| 0x208 | `mClsnRadiusFactor` | `Behavior`: `mdCcAc_c.radius = mScale * mClsnRadiusFactor`. `InitResources` sets it per variant (0x3c for actor 0xfc, 0x28 otherwise). |
| 0x20c | `mClsnHeightFactor` | `Behavior`: `mdCcAc_c.height = mScale * mClsnHeightFactor`. |
| 0x210 | `mMaxScale` | `Behavior` enables the position collider only once `mScale == mMaxScale`, i.e. the plant is fully grown. Its per-variant values 0x800 / 0x1000 / 0x2000 are Fix12 0.5 / 1.0 / 2.0. |
| 0x220 | `mAlive` | `InitResources`, free-standing variant only: `mAlive = (GetBitInDeathTable() == 0)`. |

Left `unk_`:

- **0x1e8** — `mRespawnMode`. `func_ov084_0212e010` kills the plant outright unless this
  is 1, in which case it calls `TrackInDeathTable` and sets `mState` to 4 instead.
- **0x1f0** — `mGroupLeaderID`. Handed straight to `dActor_c::FindWithID`; the actor it
  returns carries the group's two tallies (0x21a/0x21b) that this plant's death updates.
- **0x1f4, 0x224, 0x228** — zeroed in `InitResources`; still no reader, including in the
  ov084 handlers.
- **0x214** — `mScaleRate`. `func_ov084_0212e010` passes it as the step to
  `ApproachLinear(&mScale, 0, rate)`, so the per-variant 0x52 / 0xa4 / 0x147 are how fast
  each size of plant shrinks away.
- **0x21a** — `mGroupAliveCount`, and **0x21b** — `mGroupDefeatedCount`: a dying plant
  decrements 0x21a and increments 0x21b *on the group leader*, and when the leader's
  0x21b reaches 5 the star spawns and both actors are killed.
- **0x21c, 0x21d** — still no reader.
- **0x21e** — `mSuppressDeathReward`. Non-zero returns early from both death paths,
  before the group tally, the coin drop and the star.
- **0x21f** — `mStarID`. `func_ov084_0212e010` passes it to `IsStarCollectedInCurLevel`
  and ORs it with 0x40 as the spawn parameter of actor 0xb2, the star.

Byte-neutral source cleanups: `(char *)&mdCcAc_c`, `(char *)&mdCcAcPos_c` and
`(char *)&mClsnOffset` replaced the `((char *)this) + 0xNNN` arguments in `Behavior`.
`InitResources`' `*(int *)((int)this + 0x190) |= 0x8000` stays as it is — the file already
documents that the add must sit inside the integer cast.

---

## `dCapEnemy_c` (`include/dCapEnemy_c.h`, arm9)

The shared base for the cap-wearing enemies (`daKrb_c`, `daTrs_c`). All three of its
remaining `unk_` fields resolve; the header's own prose already described two of them.

| offset | new name | evidence |
| --- | --- | --- |
| 0x110 | `mCapBank` | `src/_ZN11dCapEnemy_c6AddCapEj.cpp` sets it to 1 when the six-way cap selector is `>= 3` and to 0 otherwise. `src/_ZN11dCapEnemy_c10ReleaseCapERK7Vector3.cpp` picks marker bit 3 vs bit 7 of `mCapId` by it, and `src/_ZN11dCapEnemy_c16GetCapEatenOffItERK7Vector3.cpp` re-binds the model only for bank 0. |
| 0x111 | `mIsDormant` | `src/_ZN11dCapEnemy_c15RespawnIfHasCapEv.cpp` sets it to 1 on the *replacement* actor it spawns; `AddCap` clears it. `src/_ZN11dCapEnemy_c11GetCapStateEv.cpp` returns 2 while it is clear (the ordinary enemy path), and while it is set returns 0 — or, once the cap's release bit is up, clears it, restores `mFlags` from field 0xf4, and returns 1. `src/_ZN7daKrb_c6RenderEv.cpp` draws nothing while it is set, and `daKrb_c::Behavior` treats `GetCapState() == 1` as the wake-up (poof dust + flag). |
| 0x112 | `mHadBank1Cap` | `AddCap` latches it to 1 the first time it selects bank 1 and never clears it; it is passed as the fourth argument of `func_ov001_020ab228`, the cap-icon setup. |

---

## `daKrb_c` (`include/daKrb_c.h`, ov084 — derives from `dCapEnemy_c`, not `dEnemyBase_c`)

| offset | new name | evidence |
| --- | --- | --- |
| 0x410 | `Vector3 mSafePos` | `src/_ZN7daKrb_c8BehaviorEv.cpp`: when `dEnemyBase_c::IsGoingOffCliff` reports true the actor's position is restored from it, and otherwise the current position is written into it. The last position known not to be over a ledge. |
| 0x41c | `Vector3 mHomePos` | written from `mPosX/Y/Z` in `InitResources`; `Behavior` restores `mPosX/Y/Z` from it on both respawn paths — after `UpdateKillByInvincibleChar` returns 2, and after the stuck timer expires — each immediately before `dCapEnemy_c::RespawnIfHasCap`. |
| 0x428 | `Vector3 mStuckCheckPos` | written from `mPosX/Y/Z` in `InitResources`; `Behavior` compares `Vec3_Dist(&mPosX, &mStuckCheckPos) < 0xa000` and, while the enemy stays inside that radius, ticks the already-named `mStuckTimer`; the moment it leaves, the timer is zeroed and this field is re-recorded from the current position. |
| 0x44c | `mSavedParam` | last statement of `InitResources`: a copy of `param1`, taken *after* the earlier `param1 &= 0xf0ff` masking. Named for what it holds; no matched body reads it back. |
| 0x458 | `mTimer458` | `InitResources` zeroes it; `Behavior` sets it to `0x5a` when `mStuckTimer` hits 0x1e on a capped goomba, and both the release path (`mStuckTimer >= 0x12c && mTimer458 == 0`) and the fall-through (`if (mTimer458 == 0) mStuckTimer = 0`) gate on it reaching 0. Nothing in a matched body decrements it, so "a timer" is the whole of the evidence and the offset stays in the name. |
| 0x45a | `mInitAngleY` | `InitResources`: `= mPrevAngleY`. Same shape as `Unagi`, `MrBlizzard`, `KingBobOmb` and `PiranhaPlant`. |
| 0x464 | `mRewardType` | `InitResources`: `= (param1 >> 4) & 0xf`. Value 1 calls `dActor_c::TrackStar` and loads the silver-star assets; value 2 loads the silver-star assets only; anything else does neither. It selects what this goomba is worth. |
| 0x465 | `mStarTracked` | `InitResources` presets it to -1 and, when `mRewardType == 1`, assigns `dActor_c::TrackStar(mStarID, 1)` into it. Same call and same role as `KingBobOmb`'s 0x507. |
| 0x466 | `mStarID` | `InitResources`: `= (param1 >> 0xc) & 0xf`, and it is the star-id argument of `TrackStar`. Same as `KingBobOmb`'s 0x509. |

Left `unk_`:

- **0x468** — `mSoundLatchFlags`. `func_ov084_0212934c` reads bit 1 of it through a
  two-bit bitfield, plays sound 0xd0 only when that bit is clear, sets it, and clears
  it again as soon as the surface type underfoot leaves the matching range -- a latch
  that stops the sound retriggering every frame.
- **0x43c** — `mTargetUniqueID`. `daKrb_c::CleanupResources` already read it as an
  `unsigned int id`, and `func_ov084_0212af74` hands the same word to
  `dActor_c::FindWithID` and bails to state 1 when it is 0.
- **0x450** — `mHeadingHoldTimer`. `func_ov084_0212abd4` seeds it (0x19 when the
  player is out of range, 0x64 after a fresh heading) and counts it down; only at 0
  does it pick a new target heading into 0x45c.
- **0x454** — `mWanderRerollTimer`. `func_ov084_0212af74` runs it through
  `DecIfAbove0_Short` and, at 0, reseeds it to `(rand >> 0x1b) + 0x1e` -- 30 to 61
  frames -- while writing a fresh random heading into 0x45a.
- **0x440** — `mDistToPlayer`. `func_ov084_0212abd4` tests it against 0x61a8000, the
  sentinel the same overlay stores when there is no reachable player.
- **0x438, 0x467** — still no reader. Both are only ASSIGNED in the ov084 handlers
  that mention them, which is not the same as being read.
- **0x440** — set to `0x7fffffff` and never read.
- **0x444** — `InitResources` loads `data_ov084_02130228[mGoombaType]` into it, and
  `Behavior`'s only read tests whether it is *still* that same table entry, choosing
  `UpdateWMClsn` flag 3 over flag 2. Naming it would mean naming the table, and nothing
  in a matched body says what the table holds.

Note: `src/_ZN7daKrb_c13InitResourcesEv.cpp` is a NONMATCHING `extern "C"` free function
working raw `char *c` offsets (916 bytes against the ROM's 912 — a pre-existing four-byte
near-miss, documented at the top of the file). It names no fields, so the renames above do
not reach it; every citation to it above is a citation to a raw offset in that file.
