# `dEnemyBase_c` family — field provenance

Every name below is taken from what a **matched** body actually does with the offset.
Where a body does nothing but store a constant that is never read back, the field is
left `unk_NNN` on purpose: a wrong name is a claim the next reader will trust.

Naming is byte-neutral by construction; every collapse of a raw `this`-offset poke onto
a named member in this note was re-verified with `tools/build_pin.py`'s `verify` under
2004/b56 before it was kept.

Convention: instance members `mFoo`.

Classes covered: Unagi, KingBobOmb, BowserFire, MrBlizzard, Shark, PiranhaPlant,
FirePiranhaPlantBig, dCapEnemy_c, daKrb_c, JetStream, Goomboss, daWanwan_c, BobOmb,
Whomp, RollingIronBall, KoopaShell, Klepto.

A recurring source of `unk_` in these headers is the `#else` C twin, which
restates the whole `fBase_c -> dBase_c -> dActor_c -> dEnemyBase_c` chain flat.
Those names are not independent claims: they are copied from the base headers that
own the offsets (`include/fBase_c.h`, `include/dActor_c.h`), and the `#ifdef
__cplusplus` branch of the same header inherits them properly. They are listed
once, here, rather than per class.

| offset | name | owner |
| --- | --- | --- |
| 0x008 | `param1` | `include/fBase_c.h` |
| 0x08c..0x096 | `mAngleX/Y/Z`, `mPrevAngleX/Y/Z` | `include/dActor_c.h` |
| 0x0b4 | `mClipOffsetY` | `include/dActor_c.h` |
| 0x0b8 | `mClipRadius` | `include/dActor_c.h` |
| 0x0bc | `mClipDistance` | `include/dActor_c.h` |
| 0x0c0 | `mFarDistance` | `include/dActor_c.h` |
| 0x0c4 | `mClipResult` | `include/dActor_c.h` |
| 0x0ce | `mDeathTableID` | `include/dActor_c.h` |

---

## `Unagi` (`include/Unagi.h`, [ov016](../config/arm9/overlays/ov016/symbols.txt))

| offset | new name | evidence |
| --- | --- | --- |
| 0x34c | `mState` | `src/_ZN5Unagi8BehaviorEv.cpp` loads it as a pointer to a descriptor and calls the pointer-to-member-function at `+8` through `this`; the same file compares it against &[data_ov016_02114dbc](../config/arm9/overlays/ov016/symbols.txt), and [func_ov016_02111bf0](../src/func_ov016_02111bf0.cpp)(this, &[data_ov016_02114d8c](../config/arm9/overlays/ov016/symbols.txt)) in `src/_ZN5Unagi13InitResourcesEv.cpp` is the setter. |
| 0x3f0/0x3f4/0x3f8 | `mHomePosX/Y/Z` | `InitResources` writes them from `mPosX/mPosY/mPosZ` at spawn, then the `mVariant == 2` branch subtracts `0x80000` from `mHomePosY` and writes it back into `mPosY`. Written from the position and read back into the position: a home/spawn point. |
| 0x40c | `mPathNodeCount` | `InitResources`: `mPathNodeCount = PathPtr::NumNodes(&path1)`. |
| 0x410 | `mPathNodeIndex` | `InitResources` sets it to 1, then to 8, and clamps it to 4 when `mPathNodeIndex >= mPathNodeCount`. Only meaning consistent with being bounded by the node count. |
| 0x414 | `mStarParam` | `InitResources`: `mStarParam = (param1 >> 0xc) & 0xf`. Its only two reads are `mStarParam \| 0x50` (`InitResources`) and `mStarParam \| 0x40` (`Behavior`), each the spawn parameter of actor `0xb2`, the star. |
| 0x418..0x426 | `s16 mSegmentAngle[8]` | `InitResources` zeroes indices 0..6 through the base of 0x418 and then element 7 at 0x426 explicitly; `src/_ZN5Unagi6RenderEv.cpp` reads elements 1..6, multiplies each by [data_ov016_02114908](../config/arm9/overlays/ov016/symbols.txt)`[i].angleScale` and adds the result into the bone's rotation word at `bone + 0x1e`. Eight `s16`s, driving one bone angle each. |
| 0x428/0x42a/0x42c | `mInitAngleX/Y/Z` | last three statements of `InitResources`: copied verbatim from `mAngleX/Y/Z`. Named for the capture, and [func_ov016_02111534](../src/func_ov016_02111534.c) does read all three back, restoring them into `mPrevAngleX/Y/Z`. |
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

## `KingBobOmb` (`include/KingBobOmb.h`, [ov078](../config/arm9/overlays/ov078/symbols.txt))

| offset | new name | evidence |
| --- | --- | --- |
| 0x420 | `void *mState` | `src/KingBobOmb_SetState.cpp` is exactly `c->pp = p; if (*c->pp) return (c->**c->pp)();` with `pp` at 0x420, and `src/_ZN10KingBobOmb8BehaviorEv.cpp` compares the same word against four [ov078](../config/arm9/overlays/ov078/symbols.txt) state tables ([data_ov078_0212703c](../config/arm9/overlays/ov078/symbols.txt), [_0212707c](../config/arm9/overlays/ov078/symbols.txt), [_021270bc](../config/arm9/overlays/ov078/symbols.txt), [_021270fc](../config/arm9/overlays/ov078/symbols.txt)) to pick its per-state path. Previously unnamed inside `pad_420`. |
| 0x4d4/0x4d8/0x4dc | `mArenaPosX/Y/Z` | `InitResources` stores the `Fix12` triple `0xb1d000 / 0x1060000 / 0xfee15000` — a fixed world point. `Behavior`'s only read is `(mArenaPosY - 0x28000) > mPosY`, which forces `SetState`([data_ov078_021270bc](../config/arm9/overlays/ov078/symbols.txt)), the same state that switches position updates to `UpdatePosWithOnlySpeed`: a "fell below the arena floor" test. The [ov078](../config/arm9/overlays/ov078/symbols.txt) handlers read the whole triple: [func_ov078_02123d3c](../src/func_ov078_02123d3c.c) loads all three into a `Vector3`, and [func_ov078_021240a0](../src/func_ov078_021240a0.c)/[_021243c0](../src/func_ov078_021243c0.cpp) pass `&mArenaPosX` to `Vec3_Dist` and `Vec3_HorzAngle` against `mPos`. |
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
- **0x500** — `mHealth`. Set to 3 in `InitResources`; [func_ov078_021243c0](../src/func_ov078_021243c0.cpp) decrements it
  by one in the same body that plays the stagger anim and applies the knockback speeds,
  and then latches `+0xb0` when it reaches 0; [func_ov078_021240a0](../src/func_ov078_021240a0.c) gates the whole
  chase-the-player branch on `<= 0`. Three throws, exactly as the fight plays.
- **0x424/0x428/0x42c/0x42d** — zeroed by a two-iteration loop in `InitResources` and
  otherwise untouched.

---

## `BowserFire` (`include/BowserFire.h`, [ov060](../config/arm9/overlays/ov060/symbols.txt))

| offset | new name | evidence |
| --- | --- | --- |
| 0x35c | `mVariant` | `src/_ZN10BowserFire13InitResourcesEv.cpp`: `mVariant = param1 & 7`. It is then the index into both behaviour tables — [data_ov060_0211af74](../config/arm9/overlays/ov060/symbols.txt)`[mVariant]` (called once at init) and [data_ov060_0211afb4](../config/arm9/overlays/ov060/symbols.txt)`[mVariant].pmf` (called every frame in `Behavior`) — and `mVariant == 0` is what disables the collider by setting `mdCcAc_c.flags \|= 1`. |
| 0x364 | `mGroundY` | `InitResources` casts a `dBgCh_Gnd` ray down from the actor's position and stores `rc.clsnY` on a hit, `mPosY` on a miss. |
| 0x374 | `mFrameCount` (`u16`) | zeroed in `InitResources`, incremented by 1 at the top of every `Behavior`. Widened from `s16` to `u16` to match the `unsigned short` the ROM's read-modify-write used, which is now spelled `mFrameCount += 1;`. |

Left `unk_`:

- **0x2cc, 0x37c, 0x380, 0x384, 0x388** — written to 0 (or copied from each other) in
  `InitResources` and never read in a matched body.
- **0x360** — `mShadowRadiusScale`. [func_ov060_02117624](../src/func_ov060_02117624.cpp) passes `*(0x368) * this`
  as the radius argument of `dActor_c::DropShadowRadHeight`, so the 0x2000 is a
  Fix12 2.0 multiplier on the shadow's base radius.
- **0x370** — an `s32` incremented once per `Behavior`, alongside `mFrameCount`. Two
  free-running counters in one class, and nothing reads either, so there is no evidence
  for which is which; the raw poke stays rather than pick a name at random.
- **0x378** — `(param1 >> 4) & 3`. Provenance without meaning: no matched body reads it.
- **0x379** — `mDropsShadow`. [func_ov060_02117624](../src/func_ov060_02117624.cpp) returns immediately when it is 0,
  before it drops the shadow at all, so `mVariant != 0` is deciding which variants
  cast one.

---

## `MrBlizzard` (`include/MrBlizzard.h`, [ov081](../config/arm9/overlays/ov081/symbols.txt))

| offset | new name | evidence |
| --- | --- | --- |
| 0x3f8 | `void *mState` | `src/_ZN10MrBlizzard8BehaviorEv.cpp`'s local shadow places `PMF *pp` at 0x3f8, calls the pointer-to-member-function at `pp[1]` through `this`, and compares the word against four [ov081](../config/arm9/overlays/ov081/symbols.txt) state tables ([data_ov081_02128e24](../config/arm9/overlays/ov081/symbols.txt) / [_02128e64](../config/arm9/overlays/ov081/symbols.txt) / [_02128e84](../config/arm9/overlays/ov081/symbols.txt) / [_02128e94](../config/arm9/overlays/ov081/symbols.txt)). [func_ov081_02125488](../src/func_ov081_02125488.cpp)`(this, ...)` in `InitResources` is the setter. Previously unnamed inside `pad_398`. |
| 0x400 | `mCapUniqueID` | `Behavior` spawns actor `0x10d` only when `SaveData::HasPlayerLostCap()` and this word is 0, then stores the spawned actor's unique id (`*(int*)(spawned + 4)`, the same `+4` `Unagi` uses for `mStarUniqueID`) into it. `src/_ZN10MrBlizzard6RenderEv.cpp` hides material 2 when it is non-zero — the head is bare once the cap actor exists. |
| 0x414 | `mInitAngleY` | `InitResources`, immediately after `mAngleY = mPrevAngleY`: `mInitAngleY = mAngleY`. |
| 0x420 | `mPathNodeCount` | `InitResources` (`mType == 0` branch): `= PathPtr::NumNodes()`. |
| 0x424 | `mPathNodeIndex` | passed to `PathPtr::GetNode(pos, index)` and then set to 1 in the same branch. |
| 0x44c | `Vector3 mHomePos` | `InitResources` writes `mPosX/Y/Z` into it at spawn; both it and `Behavior` then pass `&mHomePos` as the spawn position of actor `0xdf`. |

Left `unk_`:

- **0x3fc** — `mUniqueID_3fc`. Not a plain scalar: [func_ov081_02124134](../src/func_ov081_02124134.c) hands it to
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

## `Shark` (`include/Shark.h`, [ov090](../config/arm9/overlays/ov090/symbols.txt))

| offset | new name | evidence |
| --- | --- | --- |
| 0x370 | `mState` | `src/_ZN5Shark8BehaviorEv.cpp` reads it as a `SharkBehaviorState *` and calls the pointer-to-member-function at `+8` through `this`; [func_ov090_021338b4](../src/func_ov090_021338b4.cpp)(this, [data_ov090_021345cc](../config/arm9/overlays/ov090/symbols.txt)) at the end of `InitResources` is the setter. |
| 0x374 | `Vector3 mClsnOffset` | `InitResources` zeroes all three words and then passes `&mClsnOffset` as the `const Vector3 &` argument of `dCcAcPos_c::Init` — the collider's offset from the actor. |
| 0x38c | `mPathNodeCount` | `InitResources`: `= PathPtr::NumNodes()`. `Behavior` wraps `mPathNodeIdx` to 0 when it reaches this value. |

---

## `PiranhaPlant` (`include/PiranhaPlant.h`, [ov084](../config/arm9/overlays/ov084/symbols.txt))

| offset | new name | evidence |
| --- | --- | --- |
| 0x428 | `Vector3 mPipeScale` | `src/_ZN12PiranhaPlant6RenderEv.cpp` passes `&mPipeScale` as the `Vector3 *` argument of `mModel.Render` — the same slot `mModelAnim.Render` gets `&mScaleX` in. Twelve bytes of what used to be `u8 unk_428` plus `pad_429[0xb]`. |
| 0x434 | `Vector3 mFirePos` | `InitResources`' tail computes it: `0xe0` along the facing angle out of the shared sin/cos table `data_02082214`, plus `0x37800` above the spawn Y. The file's own header comment already called it "where the plant's fire comes from". |
| 0x440 | `Vector3 mSpawnPos` | `InitResources` writes `mPosX/Y/Z` into it; `Behavior` passes `&mSpawnPos` to `dCcAcPos_c::SetPosRelativeToActor`. |
| 0x44c | `Vector3 mHomePos` | copied wholesale from `mSpawnPos` in `InitResources`; `Behavior` restores `mPosX/Y/Z` from it when `UpdateKillByInvincibleChar` returns 2 (the plant zeroes its scale and goes home). Two separate copies of the spawn point, distinguished by which one is read back into the position. |
| 0x45c | `mClsnEnabled` | zeroed in `InitResources`; its only read in `Behavior` gates `dCc_c::Update` on all three colliders (the `Clear` calls above it are unconditional). |
| 0x468 | `mInitAngleY` | `InitResources`: `= mPrevAngleY`. |

Left `unk_`:

- **0x470** — `mParticleHandle`. [func_ov084_0212f460](../src/func_ov084_0212f460.cpp) stores `Particle::System::New`'s
  return in it and reads it straight back as the `slot` argument of the next call.
- **0x45d** (set to 1), **0x460** (0), **0x464** (`0x7fffffff`), **0x46c** (0),
  **0x474** (0) — written in `InitResources`, never read in a matched body, including
  in the [ov084](../config/arm9/overlays/ov084/symbols.txt) handlers.
- **0x478** — zeroed in `InitResources` and again whenever `mState` changes, alongside the
  `dEnemyBase_c` counter at 0x100. A per-state something, but nothing reads it.

Byte-neutral source cleanups: `&mWithMeshClsn`, `&mModelAnim`, `&mdCcAc_c1`,
`&mdCcAcPos_c` and `&mSpawnPos` replaced the corresponding `((char*)this) + 0xNNN`
arguments in `Behavior`. `InitResources`' `Vector3 *home = (Vector3 *)((int)this + 0x440)`
stays as it is — the file already documents that the add must sit inside the integer cast.

---

## `FirePiranhaPlantBig` (`include/FirePiranhaPlantBig.h`, [ov084](../config/arm9/overlays/ov084/symbols.txt))

| offset | new name | evidence |
| --- | --- | --- |
| 0x1f8 | `Vector3 mClsnOffset` | `Behavior` passes `&mClsnOffset` as the `const Vector3 &` of `dCcAcPos_c::SetPosRelativeToActor`. Twelve bytes of what used to be `pad_1f8[0xc]`. |
| 0x208 | `mClsnRadiusFactor` | `Behavior`: `mdCcAc_c.radius = mScale * mClsnRadiusFactor`. `InitResources` sets it per variant (0x3c for actor 0xfc, 0x28 otherwise). |
| 0x20c | `mClsnHeightFactor` | `Behavior`: `mdCcAc_c.height = mScale * mClsnHeightFactor`. |
| 0x210 | `mMaxScale` | `Behavior` enables the position collider only once `mScale == mMaxScale`, i.e. the plant is fully grown. Its per-variant values 0x800 / 0x1000 / 0x2000 are Fix12 0.5 / 1.0 / 2.0. |
| 0x220 | `mAlive` | `InitResources`, free-standing variant only: `mAlive = (GetBitInDeathTable() == 0)`. |

Left `unk_`:

- **0x1e8** — `mRespawnMode`. [func_ov084_0212e010](../src/func_ov084_0212e010.cpp) kills the plant outright unless this
  is 1, in which case it calls `TrackInDeathTable` and sets `mState` to 4 instead.
- **0x1f0** — `mGroupLeaderID`. Handed straight to `dActor_c::FindWithID`; the actor it
  returns carries the group's two tallies (0x21a/0x21b) that this plant's death updates.
- **0x1f4, 0x224, 0x228** — zeroed in `InitResources`; still no reader, including in the
  [ov084](../config/arm9/overlays/ov084/symbols.txt) handlers.
- **0x214** — `mScaleRate`. [func_ov084_0212e010](../src/func_ov084_0212e010.cpp) passes it as the step to
  `ApproachLinear(&mScale, 0, rate)`, so the per-variant 0x52 / 0xa4 / 0x147 are how fast
  each size of plant shrinks away.
- **0x21a** — `mGroupAliveCount`, and **0x21b** — `mGroupDefeatedCount`: a dying plant
  decrements 0x21a and increments 0x21b *on the group leader*, and when the leader's
  0x21b reaches 5 the star spawns and both actors are killed.
- **0x21c, 0x21d** — still no reader.
- **0x21e** — `mSuppressDeathReward`. Non-zero returns early from both death paths,
  before the group tally, the coin drop and the star.
- **0x21f** — `mStarID`. [func_ov084_0212e010](../src/func_ov084_0212e010.cpp) passes it to `IsStarCollectedInCurLevel`
  and ORs it with 0x40 as the spawn parameter of actor 0xb2, the star.

Byte-neutral source cleanups: `(char *)&mdCcAc_c`, `(char *)&mdCcAcPos_c` and
`(char *)&mClsnOffset` replaced the `((char *)this) + 0xNNN` arguments in `Behavior`.
`InitResources`' `*(int *)((int)this + 0x190) |= 0x8000` stays as it is — the file already
documents that the add must sit inside the integer cast.

---

## `dCapEnemy_c` (`include/dCapEnemy_c.h`, [arm9](../config/arm9/symbols.txt))

The shared base for the cap-wearing enemies (`daKrb_c`, `daTrs_c`). All three of its
remaining `unk_` fields resolve; the header's own prose already described two of them.

| offset | new name | evidence |
| --- | --- | --- |
| 0x110 | `mCapBank` | `src/_ZN11dCapEnemy_c6AddCapEj.cpp` sets it to 1 when the six-way cap selector is `>= 3` and to 0 otherwise. `src/_ZN11dCapEnemy_c10ReleaseCapERK7Vector3.cpp` picks marker bit 3 vs bit 7 of `mCapId` by it, and `src/_ZN11dCapEnemy_c16GetCapEatenOffItERK7Vector3.cpp` re-binds the model only for bank 0. |
| 0x111 | `mIsDormant` | `src/_ZN11dCapEnemy_c15RespawnIfHasCapEv.cpp` sets it to 1 on the *replacement* actor it spawns; `AddCap` clears it. `src/_ZN11dCapEnemy_c11GetCapStateEv.cpp` returns 2 while it is clear (the ordinary enemy path), and while it is set returns 0 — or, once the cap's release bit is up, clears it, restores `mFlags` from field 0xf4, and returns 1. `src/_ZN7daKrb_c6RenderEv.cpp` draws nothing while it is set, and `daKrb_c::Behavior` treats `GetCapState() == 1` as the wake-up (poof dust + flag). |
| 0x112 | `mHadBank1Cap` | `AddCap` latches it to 1 the first time it selects bank 1 and never clears it; it is passed as the fourth argument of [func_ov001_020ab228](../src/func_ov001_020ab228.c), the cap-icon setup. |

---

## `daKrb_c` (`include/daKrb_c.h`, [ov084](../config/arm9/overlays/ov084/symbols.txt) — derives from `dCapEnemy_c`, not `dEnemyBase_c`)

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

- **0x468** — `mSoundLatchFlags`. [func_ov084_0212934c](../src/func_ov084_0212934c.c) reads bit 1 of it through a
  two-bit bitfield, plays sound 0xd0 only when that bit is clear, sets it, and clears
  it again as soon as the surface type underfoot leaves the matching range -- a latch
  that stops the sound retriggering every frame.
- **0x43c** — `mTargetUniqueID`. `daKrb_c::CleanupResources` already read it as an
  `unsigned int id`, and [func_ov084_0212af74](../src/func_ov084_0212af74.c) hands the same word to
  `dActor_c::FindWithID` and bails to state 1 when it is 0.
- **0x450** — `mHeadingHoldTimer`. [func_ov084_0212abd4](../src/func_ov084_0212abd4.c) seeds it (0x19 when the
  player is out of range, 0x64 after a fresh heading) and counts it down; only at 0
  does it pick a new target heading into 0x45c.
- **0x454** — `mWanderRerollTimer`. [func_ov084_0212af74](../src/func_ov084_0212af74.c) runs it through
  `DecIfAbove0_Short` and, at 0, reseeds it to `(rand >> 0x1b) + 0x1e` -- 30 to 61
  frames -- while writing a fresh random heading into 0x45a.
- **0x440** — `mDistToPlayer`. [func_ov084_0212abd4](../src/func_ov084_0212abd4.c) tests it against 0x61a8000, the
  sentinel the same overlay stores when there is no reachable player.
- **0x438, 0x467** — still no reader. Both are only ASSIGNED in the ov084 handlers
  that mention them, which is not the same as being read.
- **0x440** — set to `0x7fffffff` and never read.
- **0x444** — `InitResources` loads [data_ov084_02130228](../config/arm9/overlays/ov084/symbols.txt)`[mGoombaType]` into it, and
  `Behavior`'s only read tests whether it is *still* that same table entry, choosing
  `UpdateWMClsn` flag 3 over flag 2. Naming it would mean naming the table, and nothing
  in a matched body says what the table holds.

Note: `src/_ZN7daKrb_c13InitResourcesEv.cpp` is a NONMATCHING `extern "C"` free function
working raw `char *c` offsets (916 bytes against the ROM's 912 — a pre-existing four-byte
near-miss, documented at the top of the file). It names no fields, so the renames above do
not reach it; every citation to it above is a citation to a raw offset in that file.

---

## JetStream (`include/JetStream.h`, [ov064](../config/arm9/overlays/ov064/symbols.txt))

| offset | name | evidence |
| --- | --- | --- |
| 0x300 | `mState` (`State *`) | `src/_ZN9JetStream8BehaviorEv.cpp` loads the pointer word at 0x300 every frame, tests the word at `+0x08` of what it points at, and if non-zero calls it as a pointer-to-member on `this`. That is the same object `Bullet::State` describes (`include/Bullet.h`, handler at +0x08) and the same `mState` spelling `Chuckya`, `ChiefChilly` and `daBakubaku_c` already use for it. Only `+0x08` is evidenced; the first two words stay padding. |

Left `unk_`:

* **0x314, 0x318** — `src/_ZN9JetStream13InitResourcesEv.cpp` decodes both out of
  the spawn word: `unk_314 = (param1 >> 12) & 0xf`, `unk_318 = param1 & 1` and then
  `if ((param1 & 0xf) > 1) unk_318 = 0`. Nothing in the tree reads either back, so
  the bytes say how they are *built* but nothing about what they *select*. A name
  here would be invention.

Byte-neutral cleanups made in the same pass (each re-verified with
`build_pin.verify`, `2004/b56`):

* `Behavior` — dropped the local `struct C { char pad[0x300]; Obj* obj; }`
  stand-in in favour of the declared `mState`; collapsed
  `*(short*)((char*)&mAngleX) = *(short*)((char*)&mPrevAngleX)` (and Y, Z) to plain
  member assignments; `(unsigned short*)((char*)&(*(u8 *)&unk_100))` to
  `(unsigned short*)&unk_100`; [func_ov064_0211987c](../src/func_ov064_0211987c.c)`(((C*)this))` to `(this)`.
* `InitResources` — `(dCcAc_c*)((char*)&(*(u8 *)&mdCcAc_c))` to `&mdCcAc_c`,
  `(dActor_c*)((char*)this)` to `(dActor_c*)this`.

---

## Goomboss (`include/Goomboss.h`, [ov074](../config/arm9/overlays/ov074/symbols.txt))

| offset | name | evidence |
| --- | --- | --- |
| 0x5d0 | `mSpawnPosX` | `src/_ZN8Goomboss13InitResourcesEv.cpp` copies `self+0x5c` (mPosX) into `self+0x5d0` once, immediately after the collision cylinders are sized, and nothing writes it again. |
| 0x5d4 | `mSpawnPosY` | same, from `self+0x60` (mPosY). |
| 0x5d8 | `mSpawnPosZ` | same, from `self+0x64` (mPosZ). |

C twin only (the `#else` branch), which restated things the C++ branch already
spells properly:

| offset | name | evidence |
| --- | --- | --- |
| 0x09c | `mVertAccel` | `dActor_c`'s field; `InitResources` sets it to `-0xa000`, i.e. gravity. |
| 0x0a0 | `mTerminalVelocity` | `dActor_c`'s field; `InitResources` sets it to `-0x3c000`. |
| 0x3ac | `mCylClsnPos[9]` | The header's own note already said `unk_3b8..unk_3cc` were elements 1 and 2 of the C++ branch's `Vector3 mCylClsnPos[3]`. The twin now spells the whole array flat instead of six of its nine scalars. `Behavior` reads elements 1 and 2 as the two footfall positions. |
| 0x3d8 | `mMaterialChangerFrame` | `mMaterialChanger` sits at 0x3d0 and its `Animation` base puts the 20.12 playback cursor at +0x08 (`include/Animation.h`). `InitResources` stores `data_ov074_02122e04[mSizeIndex] << 12` there — a frame number in 20.12. |
| 0x3f0 | `mTextureSequenceSpeed` | `mTextureSequence` sits at 0x3e4 and `Animation` puts playback speed at +0x0c. `InitResources` calls `SetFile(..., 0x1000, 0)` and then zeroes this word, freezing the sequence. |

Byte-neutral cleanups (each re-verified, `2004/b56`):

* `Render` — `((char*)this)+0x3e4 / +0x3d0 / +0x3f8` became `&mTextureSequence`,
  `&mMaterialChanger`, `&mTextureTransformer`; `((char*)this)+0x218` became
  `&mModelAnim.data` (`Model::data` is at +0x08, `include/Model.h`).
* `Behavior` — `((char*)this)+0x40c` became `&mWithMeshClsn`, `+0x110` became
  `(char *)mdCc_cs`, `+0x74` became `&mCamSpacePosX`.

`src/_ZN8Goomboss13InitResourcesEv.cpp` is left alone: it is an `extern "C"`
function over a bare `char *self` that never includes the header at all, so its
offsets are not member accesses to collapse.

---

## daWanwan_c (`include/daWanwan_c.h`, [ov014](../config/arm9/overlays/ov014/symbols.txt))

Bodies read: the `Behavior`, `InitResources` and `Render` members, then in
one-function files, now all consolidated into the promoted TU
`src/game/actors/d_a_wanwan.cpp`.

| offset | name | evidence |
| --- | --- | --- |
| 0x524 | `mLinkPos[7]` (`Vector3`) | `InitResources` walks a seven-iteration loop writing `mPosX/mPosY/mPosZ` into `self+0x524 + i*0xc`. Seven is the chain-link count this whole header is built around (`Model mLinkModels[7]`, `ShadowModel mLinkShadows[7]`). |
| 0x5ec | `mSpawnPosX` | `InitResources` copies `self+0x5c/0x60/0x64` into `self+0x5ec/0x5f0/0x5f4` and then adds `0xc8000` to each of the live position words, so these three are where the actor started. |
| 0x5f0 | `mSpawnPosY` | as above, and `Behavior` clamps `mPosY` up to `mSpawnPosY + 0xc8000` every frame — the rest height the chomp hangs at. |
| 0x5f4 | `mSpawnPosZ` | as above. |
| 0x608 | `mStumpUniqueID` | `InitResources` calls `dActor_c::Spawn(0x1b, 0x11, &mPosX, ...)` and stores `spawned + 4` here. `fBase_c + 0x04` is `uniqueID` (`include/fBase_c.h`). ACTOR_SPAWN_TABLE at 0x02090864, entry 0x1b, points at 0x02135298 = `g_profile_PILE`; and the very next line writes `spawned + 0x320`, which `include/Stump.h` declares as `Stump::mBusy`. Two independent witnesses for the same class. |
| 0x60c | `mFenceUniqueID` | `Behavior` lazily fills it with `dActor_c::FindWithActorID(0x29, 0)->uniqueID`. ACTOR_SPAWN_TABLE entry 0x29 points at 0x0211488c = `g_profile_WANWAN_SHUTTER` (historical alias `ChainChompFence_SpawnInfo`), in this same overlay. |
| 0x61c | `mIsOnGround` | `Behavior` clears it at the top of the frame and sets it to 1 in exactly the branch that had to clamp `mPosY` up to the rest height. |
| 0x61d | `mWasOnGround` | last statement of that block is `mWasOnGround = mIsOnGround`, and [func_ov014_02111fb8](../src/game/actors/d_a_wanwan.cpp) fires only when the clamp happens *and* `mWasOnGround == 0` — a rising-edge one-shot. |

Left `unk_`:

* **0x578** — a second `Vector3[7]` immediately after `mLinkPos`. The header's
  prose calls both "the per-link positions", but nothing matched reads or writes
  this one, so which role it plays (previous-position history is the obvious
  guess) is not evidenced. Naming it would be guessing.
* **0x5f8** — `mChainExtension`. [func_ov014_02111fe0](../src/game/actors/d_a_wanwan.cpp) builds the leash limit as
  `this * 7 + 0xc8000` and pulls the chomp back onto that radius around `mSpawnPos`
  whenever it strays outside; a second branch zeroes both speeds once it reaches
  `0x64000`. So the stored `0x50000` is the chain's slack, and 0x64000 its maximum.
* **0x605** — gates three `Behavior` helper calls; no writer anywhere in the tree,
  so it is a mode flag of unknown meaning.

C twin: `unk_0b4..unk_0ce` were `dActor_c`'s clip volume and death-table index,
renamed to the base header's own spellings (see the shared table at the top).

Byte-neutral cleanups (each re-verified, `2004/b56`):

* `Behavior` — two `((char*)this) + 0x110` became `&mdCcAcPos_c`.
* `Render` — `((char *)this) + 0x1dc` became `(char *)mLinkModels`.

The promoted TU `src/game/actors/d_a_wanwan.cpp` carries the same renames. rombuild
cannot see a stale spelling — `(char *)mLinkModels` and `((char *)this) + 0x1dc`
compile to the same bytes — so what caught the one the reconstruction still
carried was the readability ratchet (`tools/tiers_ratchet.py --check`), which
scores the source text and read the raw-offset cast as a backslide.

---

## BobOmb (`include/BobOmb.h`, ov102)

Bodies read: `src/_ZN6BobOmb13InitResourcesEv.cpp`,
`src/_ZN6BobOmb8BehaviorEv.cpp`, `src/_ZN6BobOmb6RenderEv.cpp`,
`src/_ZN6BobOmb13OnYoshiTryEatEv.cpp`,
`src/_ZN6BobOmb13OnTurnIntoEggER6Player.cpp`.

| offset | name | evidence |
| --- | --- | --- |
| 0x394 | `mMatrix[12]` | `InitResources` does `*(Matrix4x3 *)unk_394 = IDENTITY_MATRIX4X3;` — a 0x30-byte copy of the identity matrix. Kept as twelve words rather than typed `Matrix4x3`: several includers of this header do not pull `common.h`. |
| 0x3dc | `mState` | `Behavior` branches on it three times and only on equality — `!= 5` guards the whole main body, `== 4` selects the egg/Chuckya hand-off, `== 0` allows the wall bounce. |
| 0x3f0 | `mHomeAngleY` | `InitResources`' last statement, `unk_3f0 = mAngleY`, sitting beside the `mHomePosX/Y/Z = mPos*` snapshot a few lines up. |
| 0x3f3 | `mShouldRender` | `Render` is `if (unk_3f3 != 0) { ...draw... }` and nothing else; `InitResources` sets it to 1. Same role and same spelling as `Goomboss::mShouldRender`. |
| 0x3f5 | `mVariant` | `InitResources` sets it to `param1 & 7` and immediately switches on it: 2 starts inert (sets the collision volume's hit bit, clears `mFlags` bit 0), 4 starts clear, anything else starts live. This header's own prose already called it the variant. |

Left `unk_`:

* **0x390, 0x3e0, 0x3e8, 0x3ea, 0x3ec, 0x3f2** — written once by `InitResources`
  (0, 2, 0, 0, 0x2000, 0) and never read anywhere in the tree.
* **0x3ee, 0x3f4** — no reader and no writer in any matched body.
* **0x3f6** — a latch: while non-zero `Behavior` calls `func_ov102_0214ae1c` and
  returns, doing nothing else. `InitResources` clears it and nothing matched sets
  it, so the diversion is evidenced but its meaning is not.

Byte-neutral cleanup (re-verified, `2004/b56`):

* `OnYoshiTryEat` — `unsigned char *p = (unsigned char *)this; return p[263] == 0;`
  became `return unk_107 == 0;` (263 == 0x107, `dEnemyBase_c`'s own byte).

**For the `dEnemyBase_c` owner:** `BobOmb::OnYoshiTryEat` reads `unk_107` as the
"can Yoshi eat me" gate, and `BobOmb::OnTurnIntoEgg` reads `unk_108 == 1` as the
"reward the player" gate; `BobOmb::InitResources` sets `unk_108` to 0 for the two
inert variants and 1 otherwise. That reads like an "active / can be interacted
with" flag rather than a counter.

---

## Whomp (`include/Whomp.h`, [ov079](../config/arm9/overlays/ov079/symbols.txt))

Bodies read: `src/_ZN5Whomp13InitResourcesEv.cpp`,
`src/_ZN5Whomp8BehaviorEv.cpp`, `src/_ZN5Whomp6RenderEv.cpp`,
`src/_ZN5Whomp16OnAimedAtWithEggEv.cpp`,
`src/_ZN5Whomp15OnHitByMegaCharER6Player.cpp`.

| offset | name | evidence |
| --- | --- | --- |
| 0x3b0 | `mState` | `Behavior` uses it as the index into [data_ov079_02128280](../config/arm9/overlays/ov079/symbols.txt), a table of pointer-to-member handlers, and calls the one it selects; the king's camera-target update is skipped while it reads 9. |
| 0x3bc | `mSpawnPosX` | `InitResources` copies `mPosX/mPosY/mPosZ` into 0x3bc/0x3c0/0x3c4 once. |
| 0x3c0 | `mSpawnPosY` | as above. |
| 0x3c4 | `mSpawnPosZ` | as above. |
| 0x3d4 | `mSafePosX` | `Behavior` writes `mPos*` into 0x3d4/0x3d8/0x3dc every frame, EXCEPT when `dEnemyBase_c::IsGoingOffCliff` returns true — then it restores `mPos*` FROM them. A last-known-safe ground position, and the only thing that keeps a walking Whomp on its ledge. |
| 0x3d8 | `mSafePosY` | as above. |
| 0x3dc | `mSafePosZ` | as above. |
| 0x404 | `mShouldRender` | `Render` is `if (unk_404 == 0) return 1;` before anything is drawn; `InitResources` sets it to 1. |
| 0x409 | `mStarID` | `InitResources` (king branch only) sets it to `param1 & 0xf` and passes it straight to `dActor_c::TrackStar(mStarID, 2)`. |

Left `unk_`:

* **0x39c[4]** — `InitResources` zeroes four words and then refills up to
  `data_0209f21c` of them from `data_0209f394`. Promoted out of padding so the
  `struct WithArr { char pad[0x39c]; s32 arr[8]; }` stand-in could be deleted, but
  what the globals hold is not evidenced here.
* **0x401, 0x406** — both are set to one value for the king and another for the
  plain Whomp (3/1 and 4/6) and read nowhere in the tree.
* **0x402, 0x407, 0x40b, 0x410** — written once as 0, no reader.
* **0x403** — cleared by `InitResources` *and* by the last statement of every
  `Behavior` frame, so it is a one-frame event flag that the per-state handlers
  set; which event is not evidenced.
* **0x408** — holds `dActor_c::TrackStar(mStarID, 2)`'s return value; nothing
  reads it back.
* **0x40c** — cleared on init and again whenever the state index changes, so it is
  per-state scratch, but no handler in this branch reads it.

Byte-neutral cleanups (each re-verified, `2004/b56`):

* `Behavior` — the whole body worked a `char *c` through raw offsets; 0x414,
  0x3b0, 0x98, 0x110, 0x5c/0x60/0x64, 0x3d4/0x3d8/0x3dc, 0x100, 0x40c and 0x403
  are all named fields now and the `char *c` is gone.
* `InitResources` — dropped the `struct Sub18` and `struct WithArr` stand-ins;
  seven `((char *)this) + 0xNNN` member addresses collapsed.
* `Render` — `+0x330` / `+0x2d4` became `&mTextureSequence` / `&mModelAnim.data`.
* `OnAimedAtWithEgg` — `*(unsigned char*)(c+0x414)` and `*(unsigned short*)(c+0x8c)`
  became `mIsKing` and `(unsigned short)mAngleX`.

**Landed as `dEnemyBase_c::mStateTimer`.** `Whomp::Behavior` increments it (as `u16`)
once per frame and resets it to 0 the moment the state handler changes `mState`.
That is a state-elapsed frame counter, which fits the 28 subclasses that declare
the offset better than `unk_100` does.

---

## RollingIronBall (`include/RollingIronBall.h`, ov100)

Bodies read: `src/_ZN15RollingIronBall13InitResourcesEv.cpp`,
`src/_ZN15RollingIronBall8BehaviorEv.cpp`,
`src/_ZN15RollingIronBall6RenderEv.cpp`.

| offset | name | evidence |
| --- | --- | --- |
| 0x3ac | `mDrawScaleX` | `Render` passes `&unk_3ac` as the scale argument of the model's render slot — the same call every sibling in this family makes with `&mScaleX`. `InitResources` writes 1.0 (0x1000) to all three, or 0.5 (0x800) in the one level that uses the small ball. |
| 0x3b0 | `mDrawScaleY` | as above. |
| 0x3b4 | `mDrawScaleZ` | as above. |
| 0x3d0 | `mVariant` | `InitResources` sets it to `param1 & 0xf`, shifts `param1` down by four (so the next nibble is the path ID) and switches on it: 0 the static ball, 1 the free-rolling one, 2 and 4 the path followers. `Behavior` indexes its handler table with it and `Render` skips 0. |
| 0x3d4 | `mNumPathNodes` | assigned `PathPtr::NumNodes()`. |
| 0x3d8 | `mPathNodeIndex` | zeroed, then passed as the index argument to `PathPtr::GetNode`, then incremented and passed again. |
| 0x3dc | `mSpawnPosX` | `InitResources` copies `mPosX/mPosY/mPosZ` into 0x3dc/0x3e0/0x3e4 once, on the path-follower paths only. |
| 0x3e0 | `mSpawnPosY` | as above. |
| 0x3e4 | `mSpawnPosZ` | as above. |
| 0x3e8 | `mNextNodePosX` | the out-parameter `PathPtr::GetNode` writes the node into; `Vec3_Equal(&mPosX, &mNextNodePosX)` then decides whether to skip ahead a node. Promoted out of `pad_3e8[0xc]`. |
| 0x3ec | `mNextNodePosY` | as above. |
| 0x3f0 | `mNextNodePosZ` | as above. |

Left `unk_`:

* **0x3a8, 0x3cc, 0x3d2** — zeroed by `InitResources`, no reader.
* **0x3ba** — an `s16` that `func_ov100_0214233c` fills and `InitResources`
  immediately copies into `mPrevAngleY`. It is an angle, but which one (heading to
  the next node is the obvious guess) the matched code does not say.
* **0x3c0, 0x3c4, 0x3c8** — per-level constants selected by `data_0209f2f8`
  (the level ID). 0x3c0/0x3c4 look like a near/far distance pair and 0x3c8 is
  seeded from `data_02092138`, but nothing in the tree reads any of them back.

Byte-neutral cleanups: six `((char *)this) + 0xNNN` member addresses in
`InitResources` collapsed to `&mModel`, `&mPathPtr`, `&mdCcAc_c`,
`&mWithMeshClsn`, `&mPosX` and `&mNextNodePosX`; `Behavior`'s
`*(unsigned char*)((char*)&unk_3d0)` became `mVariant`.

**Tooling note.** `build_pin.verify` reports `999 word(s) differ` for
`_ZN15RollingIronBall13InitResourcesEv` — *including on the untouched `HEAD`
version of the file*, checked by restoring both the header and the body from
`HEAD` and re-running. `match.extract_func` returns a 2-byte body for that
symbol, so the isolated-compile path cannot see this function at all; it is the
pre-existing false negative `notes/` already records, not a regression. The
whole-ROM build is the authority here and it is green.

---

## KoopaShell (`include/KoopaShell.h`, ov102)

This header was already largely named. One correction and one collapse:

| offset | name | evidence |
| --- | --- | --- |
| 0x09c | `mVertAccel` (C twin) | The twin called it `mSpeed`, which contradicts `include/dActor_c.h` and the C++ branch above it. `InitResources` writes -0x2000 there and -0x32000 at 0x0a0, the gravity/terminal-velocity pair every sibling writes. |
| 0x0a0 | `mTerminalVelocity` (C twin) | as above. |

Left `unk_`:

* **0x3c0** — zeroed by `InitResources`, no reader.
* **0x3c5** — `(param1 >> 4) & 1`, stored beside `mModelIndex` (`param1 & 1`).
  Nothing in the tree reads it, so calling it a variant selector would be an
  inference from the spawn-word decode alone, not from a use. Left alone.
* **0x3c8, 0x3cc, 0x3d0, 0x3d4** — four consecutive words zeroed together in one
  chained assignment at the end of `InitResources` and read nowhere.

Byte-neutral cleanup: `Behavior`'s
`*(u32 *)(int)((char *)&mFlags) &= ~0x80000;` became `mFlags &= ~0x80000u;`.

---

## Klepto (`include/Klepto.h`, [ov062](../config/arm9/overlays/ov062/symbols.txt))

Bodies read: `src/_ZN6Klepto13InitResourcesEv.cpp`,
`src/_ZN6Klepto8BehaviorEv.cpp`, `src/_ZN6Klepto6RenderEv.cpp`.

| offset | name | evidence |
| --- | --- | --- |
| 0x42c | `mState` (`void *`) | `Behavior` reads the word as a pointer, calls the pointer-to-member at its `+0x08` on `this` every frame, and compares it against the file-scope records [data_ov062_0211e14c](../config/arm9/overlays/ov062/symbols.txt) / [data_ov062_0211e15c](../config/arm9/overlays/ov062/symbols.txt) / [data_ov062_0211e17c](../config/arm9/overlays/ov062/symbols.txt) **by address**. [func_ov062_0211c658](../src/func_ov062_0211c658.cpp)(this, record) is the setter. |
| 0x430 | `mPathNodePosX` | `PathPtr::GetNode(path, &unk_430, mPathNodeIndex)` writes the node here, and the star-carrying branch then does `mPosX = unk_430; mPosY = unk_434; mPosZ = unk_438;`. |
| 0x434 | `mPathNodePosY` | as above. |
| 0x438 | `mPathNodePosZ` | as above. |
| 0x444 | `mTimer` | `DecIfAbove0_Short` on it at the top of every frame, and reloaded with 0x1e the moment the carried actor is lost — in the same block as the state switch back to [data_ov062_0211e17c](../config/arm9/overlays/ov062/symbols.txt). (`Behavior` reached it as `((char *)this) + 0x400 + 0x44`, which is this field.) |
| 0x450 | `mHeldPosX` | `Behavior` writes these three into the held actor's own `mPosX/mPosY/mPosZ` (`p + 0x5c/0x60/0x64`) every frame. |
| 0x454 | `mHeldPosY` | as above. |
| 0x458 | `mHeldPosZ` | as above. |
| 0x46c | `mHeldItemParam` | `(param1 >> 12) & 0xf`, used for exactly one thing: OR-ed with 0x50 to build the spawn parameter of the actor Klepto carries. |
| 0x474 | `mPathNodeIndex` | the index argument of `PathPtr::GetNode`. |
| 0x484 | `mSpawnPosX` | `InitResources` copies `mPosX/mPosY/mPosZ` here once, before any movement. |
| 0x488 | `mSpawnPosY` | as above. |
| 0x48c | `mSpawnPosZ` | as above. |

Left `unk_`:

* **0x448** — set to 2 when the spawn word's item field reads 2 (which then forces
  `mCarriedItem` to 1), and read back only as `!= 2`. It marks *something* about
  that variant, but the two reads only pick between two spawn calls.
* **0x44a** — `Vec3_HorzAngle(&mPosX, &mSpawnPosX)`, taken at a point where those
  two are still the same position, and read nowhere in the tree.
* **0x470** — `InitResources` stores 4; no reader.

Byte-neutral cleanups (each re-verified, `2004/b56`): the `struct Derived { char
pad[0x334]; Base base; }` stand-in in `Render` collapsed onto `&mBlendModelAnim`;
seventeen `((char *)this) + 0xNNN` and `(T *)((char *)&member)` launders across
`Behavior` and `InitResources` collapsed to plain member references.
