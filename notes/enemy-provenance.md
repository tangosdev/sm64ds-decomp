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
