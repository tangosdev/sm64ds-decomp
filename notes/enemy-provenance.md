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
| 0x428/0x42a/0x42c | `mInitAngleX/Y/Z` | last three statements of `InitResources`: copied verbatim from `mAngleX/Y/Z`. Named for the capture, not for a use: no matched body reads them back. |
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
| 0x4d4/0x4d8/0x4dc | `mArenaPosX/Y/Z` | `InitResources` stores the Fix12 triple `0xb1d000 / 0x1060000 / 0xfee15000` — a fixed world point. `Behavior`'s only read is `(mArenaPosY - 0x28000) > mPosY`, which forces `SetState(data_ov078_021270bc)`, the same state that switches position updates to `UpdatePosWithOnlySpeed`: a "fell below the arena floor" test. X and Z are written and never read in matched code; they are named for the shape of the triple. |
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
- **0x500** — set to 3 in `InitResources` and never read. "3" next to a boss is suggestive
  of hit points, which is exactly why it is not being named on a suggestion.
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
- **0x360** — set to `0x2000` and never read.
- **0x370** — an `s32` incremented once per `Behavior`, alongside `mFrameCount`. Two
  free-running counters in one class, and nothing reads either, so there is no evidence
  for which is which; the raw poke stays rather than pick a name at random.
- **0x378** — `(param1 >> 4) & 3`. Provenance without meaning: no matched body reads it.
- **0x379** — set to `mVariant != 0`. Same: written, never read.

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

- **0x3fc** — zeroed in `InitResources`, never read.
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

- **0x45d** (set to 1), **0x460** (0), **0x464** (`0x7fffffff`), **0x46c** (0),
  **0x470**/**0x474** (0) — written in `InitResources`, never read in a matched body.
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

- **0x1e8, 0x1f0, 0x1f4, 0x224, 0x228** — zeroed in `InitResources`, never read.
- **0x214** — set per variant to 0x52 / 0xa4 / 0x147 and never read; it parallels the three
  collision factors above but nothing in a matched body says what it scales.
- **0x21a..0x21e** — five bytes set to 0/0/0/0/1, never read.
- **0x21f** — `param1 & 0xf`. Provenance without meaning: no matched body reads it.

Byte-neutral source cleanups: `(char *)&mdCcAc_c`, `(char *)&mdCcAcPos_c` and
`(char *)&mClsnOffset` replaced the `((char *)this) + 0xNNN` arguments in `Behavior`.
`InitResources`' `*(int *)((int)this + 0x190) |= 0x8000` stays as it is — the file already
documents that the add must sit inside the integer cast.
