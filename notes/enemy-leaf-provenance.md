# dEnemyBase_c leaf classes, set B — field provenance

Every name below is taken from what the matched bodies in `src/` actually do with
the offset. Where the bodies only *write* a field and nothing in the tree reads it
back, the field stays `unk_NNN` and the reason is recorded — a name nobody can
check is a claim the next reader would trust for nothing.

Classes covered here: JetStream, Goomboss, ChainChomp, BobOmb, Whomp,
RollingIronBall, KoopaShell, Klepto, Rabbit, ChiefChilly.

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

## JetStream (`include/JetStream.h`, ov064)

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
  `(unsigned short*)&unk_100`; `func_ov064_0211987c(((C*)this))` to `(this)`.
* `InitResources` — `(dCcAc_c*)((char*)&(*(u8 *)&mdCcAc_c))` to `&mdCcAc_c`,
  `(dActor_c*)((char*)this)` to `(dActor_c*)this`.

---

## Goomboss (`include/Goomboss.h`, ov074)

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

## ChainChomp (`include/ChainChomp.h`, ov014)

Bodies read: `src/_ZN10ChainChomp8BehaviorEv.cpp`,
`src/_ZN10ChainChomp13InitResourcesEv.cpp`, `src/_ZN10ChainChomp6RenderEv.cpp`,
and the merged `src_tu/actors/ChainChomp.cpp`.

| offset | name | evidence |
| --- | --- | --- |
| 0x524 | `mLinkPos[7]` (`Vector3`) | `InitResources` walks a seven-iteration loop writing `mPosX/mPosY/mPosZ` into `self+0x524 + i*0xc`. Seven is the chain-link count this whole header is built around (`Model mLinkModels[7]`, `ShadowModel mLinkShadows[7]`). |
| 0x5ec | `mSpawnPosX` | `InitResources` copies `self+0x5c/0x60/0x64` into `self+0x5ec/0x5f0/0x5f4` and then adds `0xc8000` to each of the live position words, so these three are where the actor started. |
| 0x5f0 | `mSpawnPosY` | as above, and `Behavior` clamps `mPosY` up to `mSpawnPosY + 0xc8000` every frame — the rest height the chomp hangs at. |
| 0x5f4 | `mSpawnPosZ` | as above. |
| 0x608 | `mStumpUniqueID` | `InitResources` calls `dActor_c::Spawn(0x1b, 0x11, &mPosX, ...)` and stores `spawned + 4` here. `fBase_c + 0x04` is `uniqueID` (`include/fBase_c.h`). ACTOR_SPAWN_TABLE at 0x02090864, entry 0x1b, points at 0x02135298 = `Stump_SpawnInfo`; and the very next line writes `spawned + 0x320`, which `include/Stump.h` declares as `Stump::mBusy`. Two independent witnesses for the same class. |
| 0x60c | `mFenceUniqueID` | `Behavior` lazily fills it with `dActor_c::FindWithActorID(0x29, 0)->uniqueID`. ACTOR_SPAWN_TABLE entry 0x29 points at 0x0211488c = `ChainChompFence_SpawnInfo`, in this same overlay. |
| 0x61c | `mIsOnGround` | `Behavior` clears it at the top of the frame and sets it to 1 in exactly the branch that had to clamp `mPosY` up to the rest height. |
| 0x61d | `mWasOnGround` | last statement of that block is `mWasOnGround = mIsOnGround`, and `func_ov014_02111fb8` fires only when the clamp happens *and* `mWasOnGround == 0` — a rising-edge one-shot. |

Left `unk_`:

* **0x578** — a second `Vector3[7]` immediately after `mLinkPos`. The header's
  prose calls both "the per-link positions", but nothing matched reads or writes
  this one, so which role it plays (previous-position history is the obvious
  guess) is not evidenced. Naming it would be guessing.
* **0x5f8** — `InitResources` stores `0x50000` and nothing reads it back. Promoted
  out of padding so the offset is at least visible, but not named.
* **0x605** — gates three `Behavior` helper calls; no writer anywhere in the tree,
  so it is a mode flag of unknown meaning.

C twin: `unk_0b4..unk_0ce` were `dActor_c`'s clip volume and death-table index,
renamed to the base header's own spellings (see the shared table at the top).

Byte-neutral cleanups (each re-verified, `2004/b56`):

* `Behavior` — two `((char*)this) + 0x110` became `&mdCcAcPos_c`.
* `Render` — `((char *)this) + 0x1dc` became `(char *)mLinkModels`.

`src_tu/actors/ChainChomp.cpp` carries the same renames and still compiles
(`match.compile_c`, 2004/b56) — the merged TU is not in the default rombuild
profile, so a stale spelling there would not have been caught by any green gate.

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

## Whomp (`include/Whomp.h`, ov079)

Bodies read: `src/_ZN5Whomp13InitResourcesEv.cpp`,
`src/_ZN5Whomp8BehaviorEv.cpp`, `src/_ZN5Whomp6RenderEv.cpp`,
`src/_ZN5Whomp16OnAimedAtWithEggEv.cpp`,
`src/_ZN5Whomp15OnHitByMegaCharER6Player.cpp`.

| offset | name | evidence |
| --- | --- | --- |
| 0x3b0 | `mState` | `Behavior` uses it as the index into `data_ov079_02128280`, a table of pointer-to-member handlers, and calls the one it selects; the king's camera-target update is skipped while it reads 9. |
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

**For the `dEnemyBase_c` owner:** `Whomp::Behavior` increments `unk_100` (as `u16`)
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
