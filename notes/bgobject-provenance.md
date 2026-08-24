# `dBgActor_c`-derived interactive objects — field provenance

Every name below was read off a **matched** body: the citation names the file and the
thing the code actually does with the offset. Nothing here is inferred from a sibling
class, from a name in another decomp, or from what a field "ought" to be. Offsets that
survive as `unk_NNN` are listed too, with the reason the evidence was not enough.

Convention: instance members `mFoo`, statics `sFoo`, constants `cFoo`.

Renames are byte-neutral by construction. Every class in this file was re-verified with
`build_pin.verify` (`tools/build_pin.py`) against the ROM-pinned compiler (2004/b56) after the rename,
and again after each raw-offset collapse.

---

## StarSwitch (`include/StarSwitch.h`, ov002, size 0x354)

Bodies read: `src/_ZN10StarSwitch13InitResourcesEv.cpp`,
`src/_ZN10StarSwitch8BehaviorEv.cpp`, `src/_ZN10StarSwitch6RenderEv.cpp`,
`src/_ZN10StarSwitch16CleanupResourcesEv.cpp`,
`src/_ZN10StarSwitch15OnGroundPoundedER8dActor_c.cpp`, `src/StarSwitch_Spawn.c`.

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x320 | `mDrawScaleX` | `InitResources` writes `0x1000` (Fix12 1.0) to 0x320/0x324/0x328; `Render` passes `&mDrawScaleX` to the `Model` at 0xd4 through vtable slot 5. `BlueCoinSwitch::Render` calls the same slot with a literal `0` — so the argument is an optional pointer, and three consecutive Fix12 1.0s behind it are a scale vector. |
| 0x324 | `mDrawScaleY` | as above |
| 0x328 | `mDrawScaleZ` | as above |
| 0x334 | `mMusicVolume` | `Behavior` reads it into `v`, tests `v == 0x40` / `v == 0x7f`, then passes `v` as the first argument of `Sound::ChangeMusicVolume(u32, Fix12i)`. Every other caller in the tree (`Message::PrepareTalk`, `Message::EndTalk`, `Player::St_Talk_Main`, `func_0201f32c`) passes the literals `0x40` / `0x7f` there, so that parameter is a target volume level, not an id. |
| 0x338 | `mTimer` (`u16`) | `Behavior` sets it to `1` when the switch's home area starts showing, and increments it once a frame while the pause bit `0x4000000` of `data_0209b454` is clear. The ROM proves the type: the increment only reproduces as a `u16` increment — writing `mTimer++` against the old `s16` declaration cost 1 word, and re-declaring the field `u16` made `mTimer++` byte-exact. |
| 0x33a | `mTimeLimit` | `InitResources` takes `(param1 >> 8) & 0xff`; `0xff` or `0` becomes `0x190` (400), any other value is multiplied by `0xa`. A frame budget with a spawn-parameter override and a 400-frame default. |
| 0x33c | `mSwitchType` | `InitResources` sets `2` for `actorID == 0xc` (the silver-star variant) and `param1 & 3` otherwise; `Behavior` gates the music fade on `mSwitchType == 2`. |
| 0x344 | `mTargetActorID` | pre-existing name; `Behavior` passes it to `dActor_c::FindWithID`. |
| 0x348 | `mTargetActor` | zeroed in `InitResources`; `Behavior` stores the `dActor_c *` that `dActor_c::FindWithID(mTargetActorID)` returned. Declared type left `s32` — the store is still a cast, because changing the declared type is not needed to make the name true. |
| 0x34c | `mResourceIdx` | `InitResources` sets `1` for the silver-star variant and `0` otherwise, then uses it as `idx` in `*(int *)(table + idx * 0xc)` for both the BMD table (`data_ov002_021098e8`) and the KCL tables (`…8ec`, `…8f0`); `CleanupResources` releases the same two rows by the same index. |
| 0x34d | `mMusicFadeDone` | `InitResources` sets it to `1`; `Behavior` only calls `Sound::ChangeMusicVolume` when it is `0`, and stores the call's return value back into it. A "fade finished" latch, not a counter. |
| 0x34e | `mEventBit` | `InitResources` sets `(param1 >> 3) & 0xf` and passes it straight to `Event::ClearBit(u32)`. |
| 0x353 | `mHomeAreaId` | `InitResources` copies `mAreaId` into it; `Behavior` uses it as the argument to `IsAreaShowing` and restores `mAreaId` from it. The area the switch belongs to, kept across the area-id shuffling `Behavior` does. |

Left `unk_`:

* `0x340` — `OnGroundPounded` does `if (unk_340 != 0) return;` and nothing else in any
  matched body touches it. That says it gates the ground-pound, not what it holds.
* `0x34f` — written `5` once in `InitResources`, never read in a matched body.
* `0x350` — cleared at the end of every `Behavior`, never read in a matched body.
* `0x351` — `InitResources` sets `param1 & 0xff`, mapping `0xff` to `0`, only on the
  silver-star variant. Its consumer is in un-decompiled code.
* `0x060`…`0x0c4` in the `#else` C twin — those are `dActor_c`'s fields restated flat,
  not `StarSwitch`'s. `include/dActor_c.h` is out of scope for this pass.

Raw-offset collapses, each re-verified byte-exact:

* `Behavior`: `(*(u8 *)&mAreaId) = mHomeAreaId;` → `mAreaId = mHomeAreaId;` (2 sites).
* `Behavior`: `*(void **)((char *)&unk_348) = a;` → `mTargetActor = (s32)a;`.
* `Render`: the local `struct C { char p1[0xd4]; Sub sub; }` shadow of the whole object
  is gone; the call now goes through `&mModel` directly, matching the shape
  `BlueCoinSwitch::Render` already used.
* `CleanupResources`: `((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))` →
  `((dBgW *)&mMeshCollider)`.

---

## Crate (`include/Crate.h`, ov098, size 0x608)

Bodies read: `src/_ZN5Crate13InitResourcesEv.cpp`, `src/_ZN5Crate8BehaviorEv.cpp`,
`src/_ZN5Crate6RenderEv.cpp`, `src/_ZN5Crate16CleanupResourcesEv.cpp`,
`src/_ZN5Crate4KillEv.cpp`, `src/_ZN5Crate13OnTurnIntoEggER6Player.cpp`,
`src/_ZN5Crate13OnYoshiTryEatEv.cpp`,
`src/_ZN5Crate15OnGroundPoundedER8dActor_c.cpp`, `src/Crate_SetState.cpp`.

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x4e8/0x4ec/0x4f0 | `mHomePosX/Y/Z` | `InitResources` copies `mPosX/mPosY/mPosZ` into them once; `Behavior` passes `&mHomePosX` as the second `Vector3 *` of `Vec3_HorzDist(&mPosX, …)`. A spawn position kept to measure drift. |
| 0x500/0x502/0x504 | `mHomeAngleX/Y/Z` | `InitResources` copies `mAngleX/mAngleY/mAngleZ` into them, in the same run as the position triple above. |
| 0x560 | `mState` | `Crate_SetState(c, i)` writes `i` to 0x560 and immediately dispatches `data_ov098_0213c878[i]`, a table of pointer-to-member functions — so 0x560 is the state index into that table. `Render` short-circuits on `mState == 6`, the value `Kill`, `OnTurnIntoEgg` and the two destruction paths in `Behavior` all park the crate at. |
| 0x5e4 | `mHoldingPlayer` | already typed `Player *`; `Behavior` calls `Player::DropActor()` on it when the pause bit `0x4000000` of `data_0209b454` is set and the crate's own `mFlags & 0x4000000` is set. Only a carrying player can be made to drop the actor. |
| 0x5f4 | `mClsnYOffset` | `Behavior`'s only use: the crate's own position minus this value in Y is what both `dCcAcPos_c` members' `pos` are set to each frame. A vertical offset from the model origin to the collision origin. |
| 0x5fc | `mParticleHandle1` | `Behavior` passes the field as the first argument of `Particle::System::New(u32, u32, …)` and stores the returned pointer straight back into it — a handle the particle system reads and rewrites. Effect id `0x13a`. |
| 0x600 | `mParticleHandle2` | same shape, through `Particle::System::NewUnkCallback818`, effect id `0x13b`. |
| 0x606 | `mBreakTimer` | `Behavior` runs the whole break sequence under `if (0x606 != 0)`: `DecIfAbove0_Byte(&0x606)` once a frame, spawning the two particles while it is still above zero, and on the frame it reaches zero poofing dust and setting state 6. `OnYoshiTryEat` returns 0 (inedible) while it is nonzero and 6 otherwise. A countdown to destruction, not a flag. |
| 0x607 | `mCoinsPaid` | `OnTurnIntoEgg` pays three coins only when it is not already `1`, then sets it to `1`, on both the `Player::IsCollectingCap` branch (`dActor_c::GivePlayerCoins(…, 3, 0)`) and the egg branch (`Player::RegisterEggCoinCount(player, 3, …)`). A once-only latch. |

In the `#else` C twin only, three offsets that are `dActor_c`'s and are already named
on the C++ side of the very same header were repointed to those names — the same
correction `include/CrazedCrate.h` documents having made: `0x09c` → `mVertAccel`
(`InitResources` writes `-0x2000`), `0x0a0` → `mTerminalVelocity` (`-0x3c000`),
`0x0b0` → `mFlags`.

Four more C-twin offsets are interior fields of members the C++ side already names:
`0x598/0x59c/0x5a0` are `mdCcAcPos_c1.pos.x/y/z` (`0x564 + 0x34`) and
`0x5d8/0x5dc/0x5e0` are `mdCcAcPos_c2.pos.x/y/z` (`0x5a4 + 0x34`) — `Behavior` writes
exactly those six through the C++ member spellings. They are now
`mdCcAcPos_c1_posX/Y/Z` and `mdCcAcPos_c2_posX/Y/Z`.

Left `unk_`: nothing in this class's own span. `include/Crate.h`'s `pad_` runs are
unevidenced and stay padding.

Raw-offset collapses, each re-verified byte-exact:

* `InitResources`: `((char *)this) + 0xd4` → `&mModel`, `+ 0x124` → `&mMeshCollider`,
  `+ 0x2ec` → `&mClsnMat`, `+ 0x320` → `&mWithMeshClsn`. The one remaining raw
  `this + 0xd0` write is deliberate and documented in the header: it is `dBgActor_c`'s
  generic pad, not a `Crate` field.
* `Behavior`: `*(void **)((char *)&unk_5e4)` → `mHoldingPlayer` (2 sites),
  `(u8 *)((char *)&unk_606)` → `&mBreakTimer`,
  `*(void **)((char *)&unk_5fc) = …` → `mParticleHandle1 = (u32)…` (and `…600`),
  and the two `(char *)&mPosX` / `(char *)&mHomePosX` double casts.
* `Render`: `(Sub *)((char *)&mModel)` → `(Sub *)&mModel`.

`Crate_SetState.cpp` keeps its local `struct C` shadow on purpose: it calls through a
pointer-to-member function, and that representation is not the same for a polymorphic
class as for the flat shadow the ROM's own code is built against.

---

## daDgr_c (`include/daDgr_c.h`, ov025, size 0x334)

A swinging platform. Bodies read: `src/_ZN7daDgr_c13InitResourcesEv.cpp`,
`src/_ZN7daDgr_c8BehaviorEv.cpp`, `src/_ZN7daDgr_c6RenderEv.cpp`,
`src/_ZN7daDgr_c16CleanupResourcesEv.cpp`, `src/func_ov025_02111898.c` (the factory).

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x320 | `mBasePosY` | `InitResources` copies `mPosY` into it once; `Behavior` never writes it again and computes `mPosY = mBasePosY + |sine * 23|` from `data_02082214` every swing frame, and puts the dust particles at `mBasePosY - 0xb9000`. The resting height the swing is measured from. |
| 0x324 | `mAngleXSpeed` | `Behavior` sets it to `±(0x400 / n)` and then adds it to `mAngleX` — it is only ever the per-frame angular step. It is also the guard on the swing sound: the sample only plays when it is nonzero. |
| 0x326 | `mPhaseTimer` | incremented once per `Behavior` call on every path, and compared against `0x20`, `n + 8`, `m` and `m - 1`; zeroed whenever one of those thresholds is hit. A frame counter within the current stage. |
| 0x328 | `mSwingStage` | counts `0 … 0x14` and then goes to the `-1` "hold at the end" sentinel; `Behavior` reads it as `10 - mSwingStage` and folds that to a magnitude, which is what makes the platform ease in and out. |
| 0x329 | `mSwingDir` | flipped with `^ 1` exactly when a swing finishes (`mSwingStage == 0x14`), and the only thing it decides is the sign of the Z step (`±0x14000 / n`) and of `mAngleXSpeed`. A direction bit, not a counter. |
| 0x32c | `mDustParticle1` | passed as the first argument of `Particle::System::New(u32, u32, …)` and overwritten with that call's result — a recycled handle. Effect `0x2d`, placed 100 units along the platform's facing (`data_02082214[(mAngleY >> 4) * 2 + 1]`). |
| 0x330 | `mDustParticle2` | identical shape, placed 100 units the other way. The two ends of the platform. |

In the `#else` C twin only, eight offsets that are `dActor_c`'s and are already named
in `include/dActor_c.h` at exactly those offsets were repointed to those names:
`0x074` → `mCamSpacePosX` (the twin already named `mCamSpacePosY/Z` right after it),
`0x098` → `mHorzSpeed`, `0x09c` → `mVertAccel`, `0x0a0` → `mTerminalVelocity`,
`0x0a8` → `mVertSpeed`, `0x0b0` → `mFlags`, `0x0b4` → `mClipOffsetY`,
`0x0b8` → `mClipRadius`.

Left `unk_`: `0x31c` / `0x31d` in the C twin — those are `dBgActor_c`'s own two
trailing bytes and are `unk_` in `include/dBgActor_c.h` too, which this pass does not
own.

`src/_ZN7daDgr_c13InitResourcesEv.cpp` was an `extern "C"` free function over a raw
`char *`, with four local one-word shadow structs (`Model`, `ModelBase`, `dBgW_Kc`,
`dBgW_KcMbg`) and every field reached by literal offset — `*(int *)(c + 0x320) =
*(int *)(c + 0x60);`. It is now a real `s32 daDgr_c::InitResources()` over the shared
header, with `&mModel`, `&mMeshCollider`, `mClsnMat`, `mAngleY`, `mPosY` and the six
named swing fields. Byte-exact under 2004/b56. The file's old banner claimed there was
"nothing to gain by converting the body and a real risk of a codegen-driven byte miss"
— the first half was wrong and the second did not happen; the banner now records what
was measured instead.

---

## SignPost (`include/SignPost.h`, ov002, size 0x5a4)

Bodies read: `src/_ZN8SignPost13InitResourcesEv.cpp`,
`src/_ZN8SignPost8BehaviorEv.cpp`, `src/_ZN8SignPost6RenderEv.cpp`,
`src/_ZN8SignPost16CleanupResourcesEv.cpp`, `src/_ZN8SignPost4KillEv.cpp`,
`src/_ZN8SignPost15OnGroundPoundedER8dActor_c.cpp`,
`src/_ZN8SignPost11OnAttacked1ER8dActor_c.cpp`,
`src/_ZN8SignPost15OnHitByMegaCharER6Player.cpp`.

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x354 | `mClsnResult` | `Behavior` reads this word immediately after `dCc_c::Clear` and `dCc_c::Update` on `mdCcAc_c` (which ends at exactly 0x354) and branches on `== 3` and `<= 1`. The collider's result code. Was inside `pad_354`. |
| 0x380 | `mShadowMat` | `Behavior` passes it as the `Matrix4x3 &` argument of `dActor_c::DropShadowScaleXYZ(ShadowModel &, Matrix4x3 &, …)`, with `mShadowModel` as the argument before it. A `Matrix4x3` is 0x30 bytes and closes exactly on 0x3b0, the next evidenced field. Was `pad_380[0x30]`. |
| 0x3b0/0x3b4/0x3b8 | `mHomePosX/Y/Z` | `InitResources` copies `mPosX/mPosY/mPosZ` into them once, after the ground-raycast has already settled `mPosY`; `Behavior` restores `mPosY = mHomePosY` when the sign respawns. |
| 0x3bc/0x3be/0x3c0 | `mHomeAngleX/Y/Z` | `InitResources` copies `mAngleX/mAngleY/mAngleZ` in the same run. |
| 0x584 | `mParticleHandle1` | `Behavior` passes it as the first argument of `Particle::System::New` and stores the result back — a recycled handle. Effect `0x13a`. Was inside `pad_584`. |
| 0x588 | `mParticleHandle2` | same shape through `Particle::System::NewUnkCallback818`, effect `0x13b`. Was inside `pad_584`. |
| 0x58c | `mBreakTimer` | `Behavior` runs the whole break sequence under `if (0x58c != 0)`: disable the mesh collider, `DecIfAbove0_Byte` once a frame trailing the two particles, and on the frame it hits zero poof the dust and hand off to the class's reset routine `func_ov002_020bae9c`. The same shape `Crate` uses at its own 0x606. Was inside `pad_584`. |
| 0x58e | `mPoundsLeft` | `InitResources` sets `2`. `OnGroundPounded` either sinks the sign by `(mPoundsLeft * 0x2d) << 12` and zeroes it (a hard pound), or by one `0x2d000` step and decrements it. `Behavior` respawns the sign when it is `0`, setting it back to `2`, and only drops the shadow while it is still `2`. A remaining-steps count, not a state. |
| 0x58f | `mPoundCooldown` | `OnGroundPounded` sets `0xf` on the soft-pound branch and returns early whenever it is nonzero; `Behavior` runs it down with `DecIfAbove0_Byte` once a frame. The gap between two successive pounds. |
| 0x590 | `mHidden` | `Render` returns without drawing while it is nonzero; `Behavior` skips the collision-range check while it is nonzero and clears it under the same "player is far away" condition that respawns a pounded-in sign. |
| 0x591 | `mRespawnDelay` | `OnGroundPounded` sets `0x1e` on both branches; `Behavior` requires `DecIfAbove0_Byte(&mRespawnDelay) == 0` **and** `DistToCPlayer > 0x7d0000` before it will restore the sign or clear `mHidden`. |
| 0x59c | `mHoldingPlayer` | already typed `Player *`; `Behavior` calls `Player::DropActor()` on it through a pause, and `Behavior`/`Render` both read `player + 0xc8` through it. Same shape as `Crate`'s 0x5e4. |

In the `#else` C twin only: `0x09c` → `mVertAccel` and `0x0a0` →
`mTerminalVelocity` (`InitResources` writes `-0x2000` / `-0x3c000` to them through the
inherited names), `0x0b0` → `mFlags`, and the `u8` marker at `0x2ec` → `mClsnMat`,
which is what `include/dBgActor_c.h` calls that offset and what `InitResources` passes
to `dBgW_KcMbg::SetFile`. The marker stays `u8`, the idiom the twin already uses for
`mdCcAc_c` and `mShadowModel`.

`src/_ZN8SignPost8BehaviorEv.cpp` was an `extern "C"` free function over a raw
`char *c` with 30 literal offsets and a local one-word `Vector3`; it is now a real
`int SignPost::Behavior()`. That conversion is what turned 0x354, 0x380, 0x584, 0x588
and 0x58c from padding into evidenced fields. It needed one declaration added to the
header — `int Behavior();`, placed **after** `virtual void Kill()` so that Kill stays
this class's key function. `tools/eligible.py` gains exactly one name,
`_ZN8SignPost8BehaviorEv`, and loses none.

Raw-offset collapses, each re-verified byte-exact: `Render`'s
`*(void **)((char *)&unk_59c)` and `(Sub041 *)((char *)&mModel)`,
`CleanupResources`'s `((dBgW *)((char *)&mMeshCollider))`, and `InitResources`'
`(char *)&mWithMeshClsn` plus the `((dBgCh_Actr *)((char *)&mWithMeshClsn))->` cast,
which is now just `mWithMeshClsn.StartDetectingWater()`.

---

## daPgDfdr_c (`include/daPgDfdr_c.h`, ov027, size 0x3dc)

Only two of this class's own six trailing fields are named. The other four are
described in the header from a read of the un-decompiled `func_ov027_*` state
machine, and "a flag" / "an index" / "a table index" does not say what a field holds,
so they stay `unk_`.

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x3cc | `mStateTable` | `func_ov027_02111d70` writes `&data_ov027_02113ce4[idx]` into it and `func_ov027_02111d38` / `02111cfc` read `*(p)` and `*(p + 1)` as function pointers and call them through `this`. Left a raw `void *` — the callee's exact function-pointer type is not proven. |
| 0x3d8 | `mTimer` | `func_ov027_02111ca8` sets `0x14`; `func_ov027_02111c48` runs it down through `DecIfAbove0_Byte`. |

Left `unk_`: `0x3d0` (set to 0 or 1, no reader identified), `0x3d4` (an index into
`data_ov027_02113a1c` keyed by `0x3d9`), `0x3d9` (0..9, reset above 9). In the C twin,
`0x31c` / `0x31d` are `dBgActor_c`'s and stay `unk_` there too.

In the `#else` C twin only, eight `dActor_c` offsets already named at exactly those
offsets in `include/dActor_c.h` were repointed to those names: `mCamSpacePosX`,
`mHorzSpeed`, `mVertAccel`, `mTerminalVelocity`, `mVertSpeed`, `mFlags`,
`mClipOffsetY`, `mClipRadius`.

Both `src/_ZN10daPgDfdr_c13InitResourcesEv.cpp` and
`src/_ZN10daPgDfdr_c8BehaviorEv.cpp` were `extern "C"` free functions over a raw
`char *`; both are real methods now, byte-exact. The header already declared both
virtual, so `tools/eligible.py` is unchanged by this. The conversion made one thing
readable that the offsets hid: `Behavior`'s two `Animation::Advance` calls are the
same call on two different sub-objects — `this + 0x370` is the **Animation base of
mModelAnim** (0x320 + 0x50, the multiple-inheritance offset `include/ModelAnim.h`
documents) and `this + 0x384` is `mTextureSequence`, whose `Animation` base is at
offset 0. Both are spelled `static_cast<Animation *>(&member)` now.
`Render`'s `this + 0x328` was `mModelAnim.data`, `Model`'s own `ModelComponents`.

---

## Eyerok (`include/Eyerok.h`, ov066, size 0x874)

The largest recovery in this pass: reading `InitResources` and `Behavior` as named
members turned **every remaining `pad_` run in the class** into an evidenced field,
and the class now closes on 0x874 by field span instead of only by the factory's
`operator new` literal.

Bodies read: `src/_ZN6Eyerok13InitResourcesEv.cpp`, `src/_ZN6Eyerok8BehaviorEv.cpp`,
`src/_ZN6Eyerok6RenderEv.cpp`, `src/_ZN6Eyerok16CleanupResourcesEv.cpp`,
`src/_ZN6Eyerok16OnAimedAtWithEggEv.cpp`, `src/Eyerok_Spawn.cpp`.

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x48c | `mState` | `Behavior` loads the WORD here, tests `*(p + 8)`, and calls the pointer-to-member at `p + 8` through `this`; it also compares the word against `&data_ov066_0211b07c`. A pointer to the current state descriptor. It was declared `u8` plus 0xf bytes of padding. |
| 0x49c | `mPartIdx` | `InitResources` sets it from `param1 & 0xff` (`0xff` becomes 0, `> 2` becomes 0) and switches on 0/1/2 to pick the model, the BTP sequences and the KCL. Case 0 is the body: it spawns two more actors of the same actorID `0xb0` with params 1 and 2. Cases 1 and 2 are those two, mirrored `-/+0x31f000` in X. So the field is which part of the boss this actor is. |
| 0x4a4/0x4a8/0x4ac | `mRestPosX/Y/Z` | seeded from the actor position on every path, then offset — a hand gets `+/-0x31f000` in X and `-0x32000` in Z. `Behavior` re-derives `mRestPosY = mSpawnPosY + 0x8000` every frame. Was two named scalars inside padding. |
| 0x4b0/0x4b4/0x4b8 | `mSpawnPosX/Y/Z` | the unmoved position, snapshotted only on the two hands and never written again. |
| 0x4d0 | `mTimer1` | `Behavior`'s first statement is `DecIfAbove0_Short(&0x4d0)`, and that helper takes a `u16 *`. The header declared `u8` plus one byte of padding; the ROM says `u16`. |
| 0x4d2 | `mTimer2` | second `DecIfAbove0_Short`; `InitResources` seeds it with `0x64` on the body part only. |
| 0x4d4 | `mDustCounter` | gates the whole dust-cloud block, is incremented once a frame, is halved into the `mDustPos` slot index (`(v >> 1) * 0xc`), only emits a new point on even values, and the array is cleared when it passes `0x26`. |
| 0x4dc | `mDustPos[0x14]` | was `mUnkVectors`. `Behavior` fills each entry with the actor position plus a randomised offset and feeds it straight to `Particle::System::New`. The count and element size still come from the destructor's `__destroy_arr(ptr, 0x14, 0xc, _ZN7Vector3D1Ev)`. |
| 0x5cc | `mDustParticle1[0x14]` | `Behavior` walks all 0x14 slots and passes `[i]` as the first argument of `Particle::System::New(…, 0x13a, …)`, storing the result back. One recycled handle per dust point. Was inside `pad_5cc[0xa8]`. |
| 0x61c | `mDustParticle2[0x14]` | the same, effect `0x13b`. Was inside `pad_5cc[0xa8]`. |
| 0x672 | `mStarId` | `InitResources` sets `(param1 >> 12) & 0xf` and passes it as the star index of `dActor_c::TrackStar`. Was inside `pad_5cc[0xa8]`. |
| 0x673 | `mStarTracked` | holds what that `dActor_c::TrackStar(this, mStarId, 2)` call returned. Was inside `pad_5cc[0xa8]`. |
| 0x674 | `mMeshCollider2` | already evidenced as a second, class-owned `dBgW_KcMbg`; renamed off `unk_674` because `InitResources` gives it a KCL and enables it exactly the way `dBgActor_c` treats its own at 0x124. |
| 0x83c | `mClsnMat2` | `InitResources` passes `this + 0x83c` as the `const Matrix4x3 &` argument of `dBgW_KcMbg::SetFile` on all three paths. `0x83c + 0x30 = 0x86c`. Was the front of the header's declared "unused tail". |
| 0x86c | `mHandUniqueID1` | `InitResources` spawns actor `0xb0` param 1 and, if it succeeds, stores `*(s32 *)(spawned + 4)` here — and `+4` is `dActor_c::uniqueID` (`include/dActor_c.h`). |
| 0x870 | `mHandUniqueID2` | the same for param 2. `0x870 + 4 = 0x874`, the factory's literal. |

In the `#else` C twin only: `unk_124` becomes `mMeshCollider` (`dBgActor_c`'s own name
for that member) and `0x354/0x358/0x35c` become `mdCcAcPos_c_posX/Y/Z`, which is what
`Behavior` writes through the C++ member spelling (`0x320 + 0x34`).

Left `unk_`: `0x45c..0x48c` (a 0x30 blob no matched body touches) and `0x4d8`, which
`InitResources` sets to `3` on the two hands and nothing matched reads.

Raw-offset collapses: about fifty in `InitResources` and thirty in `Behavior`, each
re-verified. Two were rejected by the ROM and are documented in place:

* the `mDustPos` clearing loop in `InitResources` must stay a running `char *` with
  three literal offsets — rewriting it as `Vector3 *p = mDustPos; p->x = 0; … p += 1;`
  changes the function's SIZE (`999 word(s) differ`).
* `Behavior` reaches `mDustCounter` a second time through `c + 0x400` and then
  `+ 0xd4`; that two-step materialisation is the ROM's own and spelling it away is not
  free.

---

## PushBlock (`include/PushBlock.h`, ov002, size 0x4f4)

Bodies read: `src/_ZN9PushBlock13InitResourcesEv.cpp`,
`src/_ZN9PushBlock8BehaviorEv.c`, `src/_ZN9PushBlock6RenderEv.cpp`,
`src/_ZN9PushBlock8OnPushedER8dActor_c.cpp`, `src/_ZN9PushBlock4KillEv.cpp`,
`src/_ZN9PushBlock15OnHitByMegaCharER6Player.cpp`.

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x4dc/0x4e0/0x4e4 | `mHomePosX/Y/Z` | `InitResources` copies `mPosX/mPosY/mPosZ` into them once; `Behavior` measures `Vec3_HorzDist(&mHomePosX, &mPosX) >= 0x12c000` to decide the block has been pushed far enough, and then writes the triple back into the linked actor's own position. |
| 0x4e8 | `mGroundY` | `InitResources` raycasts down from `mPosY + 0x14000` and stores the hit height here (falling back to the raised point). `Behavior` re-raycasts ahead of the block each frame and stops the slide the moment the hit height differs from this — a step, a ledge or a hole. |
| 0x4ec | `mSlideSound` | `Behavior` passes it as the first argument of `Sound::PlayLong(u32, 3, 0x97, …)` and stores the result back: a recycled long-sound handle for the sliding noise. |
| 0x4f0 | `mLinkedActor` | `Behavior` treats it as an actor pointer: it reads `+0xc` (`dActor_c::actorID`) and compares against `0x149`, then writes `+0x5c/+0x60/+0x64` (that actor's position) with the block's own home position and clears the field. |

In the `#else` C twin, eight offsets already named at exactly those offsets in
`include/dActor_c.h` were repointed to those names — `mPosX/Y/Z`, `mAngleY`,
`mPrevAngleY`, `mHorzSpeed`, `mVertAccel`, `mTerminalVelocity`. The file's own banner
already asserted two of them (`mPrevAngleY 0x94, mHorzSpeed 0x98`) from a migration
experiment. Six more were added to the twin out of the 0x438-byte pad the header
carried: `mPrevPosX/Y/Z` (0x068) and `mCamSpacePosX/Y/Z` (0x074), both of which
`Behavior` passes as `Vector3 *`, and `dBgActor_c`'s own `mMeshCollider` (0x124),
`mClsnMat` (0x2ec) and `mWithMeshClsn` (0x320) as `u8` markers, which is the idiom the
other twins in this family use.

`src/_ZN9PushBlock13InitResourcesEv.cpp` was an `extern "C"` free function over a raw
`char *self`; it is a real `int PushBlock::InitResources()` now, byte-exact, with all
fourteen hand offsets on named members. `tools/eligible.py` is unchanged by it.

`Behavior` stays a `.c` file for the reason its own banner records (the C++ form emits
two extra literal-pool words), but ten of its raw offsets now go through the named C
twin. One did not survive: the single `q = *(char **)(c + 0x4f0);` read costs the
function its size when spelled `(char *)self->mLinkedActor`, while the three sites
around it are free. The measurement is in a comment at the site.

---

## BlueCoinSwitch (`include/BlueCoinSwitch.h`, ov002, size 0x330)

This header already carried a full prose account of every offset; the names below just
make the code say what the prose said. Bodies read:
`src/_ZN14BlueCoinSwitch13InitResourcesEv.cpp`,
`src/_ZN14BlueCoinSwitch8BehaviorEv.cpp`, `src/_ZN14BlueCoinSwitch6RenderEv.cpp`,
`src/_ZN14BlueCoinSwitch16CleanupResourcesEv.cpp`.

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x320 | `mStopPosY` | `InitResources` sets `mPosY - 0x64000`; `Behavior` walks `mPosY` down 0x14000 a frame and snaps to it; `Render` draws only while `mPosY > mStopPosY`. |
| 0x324 | `mTickSound` | fed back into `func_02012310(mTickSound, 0x39 or 0x38, 0)` and overwritten with the result, and zeroed on the frame the count crosses 0x2d — a recycled sound handle whose id changes with 45 frames left. |
| 0x328 | `mCoinTimer` | armed from `mCoinTimerSeed` when the switch bottoms out, run down by `DecIfAbove0_Short`, and parked at `1` rather than `0` on expiry so it stays latched. |
| 0x32a | `mCoinTimerSeed` | `(param1 >> 8) & 0xff`, then `* 10`, or `0xfa` when that byte is `0` or `0xff`. The spawn word carries tenths. |
| 0x32c | `mPressed` | zeroed by `InitResources`; every branch of `Behavior` is under `mPressed == 1`. |
| 0x32d | `mEventBit` | `param1 & 0xf`, passed straight to `Event::SetBit(u32)`. |
| 0x32e | `mHomeAreaId` | `InitResources` copies `mAreaId` here before `Behavior` sets `mAreaId = -1`, and `Behavior` passes it to `IsAreaShowing`. |

The rename carried into `src_tu/actors/BlueCoinSwitch.cpp` as well as `src/` — the
shadow TU builds only under a `tuModules` profile, so a stale spelling there compiles
nowhere and no normal gate would have caught it. `tools/check_src_tu_compiles.py`
(72/72) and `tools/check_src_tu.py` were run after.

---

## MovingBar (`include/MovingBar.h`, ov015, size 0x338)

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x320/0x324/0x328 | `mHomePosX/Y/Z` | `InitResources` copies `mPosX/mPosY/mPosZ` into them as its last act before installing state 5. |

`mVariant` (0x32c) and `mState` (0x330) were already named. `mState` was missing from
the `#else` C twin entirely and is added, because `Behavior`'s state dispatch indexes
`data_ov015_021149ec` by exactly that word — the local `struct C { char pad[0x330];
int idx; }` shadow existed only to reach it, and the index now reads `mState`. The
pointer-to-member call itself still goes through that shadow: a PMF's representation
is not the same for a polymorphic class.

In the C twin, six `dActor_c` offsets were repointed to the names
`include/dActor_c.h` gives them: `mPosX/Y/Z` (0x05c) and `mPrevPosX/Y/Z` (0x068).

Raw-offset collapses in `InitResources` (the `*(int *)((char *)&member)` double
indirection on `mVariant`, `actorID` and the three home-position stores) and in
`Render`, whose `struct Derived { char pad[0xd4]; Base base; }` shadow of the whole
object is gone — the call goes through `&mModel`.

---

## CannonHatch (`include/CannonHatch.h`, ov002, size 0x330)

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x320/0x324/0x328 | `mHomePosX/Y/Z` | `InitResources` copies `mPosX/mPosY/mPosZ`; `Behavior` restores the actor's position from them once the cannon is open, and passes `&mHomePosX` as a `Vector3 *` to `Vec3_Dist` and `Vec3_HorzDist` to measure how far the hatch has slid. |
| 0x32c | `mOpening` | gates the entire slide block in `Behavior`; nothing else reads it. |
| 0x32d | `mOpenPhase` | picks the direction of the slide — `0` moves the hatch down (`-0x1000` in Y), `1` moves it forward (`+0x2000` in Z) — and `Behavior` flips it to `1` once the hatch is more than `0xa000` from home. A two-step animation, not a flag. |
| 0x32e | `mCannonOpen` | `InitResources` sets it from `IsCannonOpenInCurLevel()`. `Render` refuses to draw while it is set, and `Behavior` parks the hatch at its home position with the collider disabled. `Behavior` also walks every actor of id `0xe` and sets THEIR copy when this hatch finishes sliding — so it is the level-wide "the cannon is open" fact, not a per-hatch animation flag. |

In the C twin, the `u8` marker at `0x2ec` became `mClsnMat`, the name
`include/dBgActor_c.h` gives that offset and what `InitResources` passes to
`dBgW_KcMbg::SetFile`.

The rename carried into `src_tu/actors/CannonHatch.cpp`, along with the same
raw-offset collapses: `Render`'s whole-object `struct Obj { char pad[0xd4]; Sub sub; }`
shadow is gone in favour of `&mModel`, and `InitResources` reaches `mModel`,
`mMeshCollider` and `mClsnMat` by name.

---

## QuestionBlock (`include/QuestionBlock.h`, ov102, size 0x3f8)

Bodies read: `src/_ZN13QuestionBlock13InitResourcesEv.cpp`,
`src/_ZN13QuestionBlock8BehaviorEv.cpp`, `src/_ZN13QuestionBlock6RenderEv.cpp`,
`src/_ZN13QuestionBlock16CleanupResourcesEv.cpp`,
`src/_ZN13QuestionBlock15OnGroundPoundedER8dActor_c.cpp`,
`src/_ZN13QuestionBlock11OnAttacked1ER8dActor_c.cpp`,
`src/_ZN13QuestionBlock8OnKickedER8dActor_c.cpp`,
`src/_ZN13QuestionBlock15OnHitByMegaCharER6Player.cpp`,
`src/_ZN13QuestionBlock19OnHitFromUnderneathER8dActor_c.cpp`.

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x3ac | `mShadowMat` | `InitResources` copies the 0x30 bytes at `mModel + 0x1c` (`Model`'s own matrix) here, and 0x3ac is exactly where `mShadowModel` ends — the `ShadowModel` / `Matrix4x3` pairing `dActor_c::DropShadowScaleXYZ` takes. `0x3ac + 0x30 = 0x3dc`. It was `u8 unk_3ac` followed by `u8 pad_3ad[0x33]`. |
| 0x3e0 | `mHomePosY` | `InitResources` sets it from `mPosY`; `Behavior` walks the block back down and clamps `if (mPosY <= mHomePosY) mPosY = mHomePosY;`. |
| 0x3e8 | `mState` | every one of the five combat callbacks returns early on `mState == 1`, `Render` skips the model on `mState == 2`, and `Behavior` branches on `!= 2` then `!= 0`. It is also the key of the `data_ov102_0214e890` dispatch the callbacks jump into. |
| 0x3f0 | `mStarTracked` | holds what `dActor_c::TrackStar(this, mStarId, 2)` returned. |
| 0x3f1 | `mStarId` | `(param1 >> 8) & 0xff`, `0xff` read as 0, and passed as the star index of that `TrackStar` call — read only when `mContentType == 1`. |
| 0x3f2 | `mHitterParam` | written (truncated from the caller's word) by all five combat-callback overrides, always immediately before `func_ov102_02149da8(this, 1)`. Was `u8 pad_3f2[0x1]` in the C twin. |
| 0x3f3 | `mContentType` | `param1 & 0xff` with `0xff` read as 0; `InitResources` switches on it to pick what the block holds, and `CleanupResources` switches on it again to release the matching files. `1` is the star case, which is the only one that reads `mStarId`. |

In the `#else` C twin, `0x0a0` was repointed to `mTerminalVelocity`, the name
`include/dActor_c.h` gives that offset, and the twin gained `mShadowMat` and
`mHitterParam` at the offsets above.

Raw-offset collapses: `InitResources`'s shadow-matrix copy now spells both sides as
members (`&mShadowMat` and `&mModel + 0x1c`); `Behavior`'s two
`((char*)&(*(u8 *)&mMeshCollider))` casts are `&mMeshCollider`; `Render`'s two
whole-object shadow casts are `&mModelAnim` and `&mModel`.

---

## FortressWall (`include/FortressWall.h`, ov079, size 0x324)

Bodies read: `src/_ZN12FortressWall13InitResourcesEv.cpp`,
`src/_ZN12FortressWall8BehaviorEv.cpp`, `src/_ZN12FortressWall6RenderEv.cpp`,
`src/_ZN12FortressWall4KillEv.cpp`,
`src/_ZN12FortressWall16CleanupResourcesEv.cpp`,
`src/_ZN12FortressWall24OnHitByCannonBlastedCharER8dActor_c.cpp`,
`src/FortressWall_Spawn.c`, `src/FortressWallBreakable_Spawn.c`.

Two actors share this class: `FortressWallBreakable_Spawn` (actorID 0x30) and
`FortressWall_Spawn`. Every field is about telling those two apart.

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x31e | `mVariant` | `InitResources` sets 0 when `actorID == 0x30` and 1 otherwise, then uses it to index all three ov079 file tables — `data_ov079_02128058` (model), `data_ov079_0212805c` (KCL), `data_ov079_02128060` (CLPS block). `CleanupResources` indexes the first two again to `Release()` them. |
| 0x31f | `mStarId` | `param1 & 0xff`, with `0xff` read as 0. `Behavior` passes it as `dActor_c::Spawn(0xb2, mStarId \| 0x40, …)`; actor 0xb2 is the star and its spawn word is `starID \| (howToSpawnStar << 4)` (`src/_ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h.cpp`), so the low nibble this contributes is a star index and the `0x40` is that call's spawn style. |
| 0x321 | `mBroken` | `Kill()` sets it on actorID 0x30 *instead of* calling `MarkForDestruction` — the breakable wall survives its own Kill. `Render` draws nothing while it is set, and `Behavior` runs the break sequence (disable the collider, wait for the sound, spawn the star, destroy) only while it is set. |
| 0x322 | `mBreakSoundState` | `Behavior` passes `&mBreakSoundState` as the `u16 *state` argument of `Sound::PlaySecretSound(dActor_c *, u16 *)` and spawns the star on the frame that returns nonzero. **Declared `u16`, not the `u8` the header had**: the parameter type is `u16 *`, and the byte at 0x323 was the struct's tail padding, so the field span now lands exactly on the ROM's 0x324 instead of being rounded up to it. |

Nothing is left `unk_` in this class.

In the `#else` C twin, six offsets already named at exactly those offsets in
`include/dActor_c.h` were repointed to those names — `mPosX/Y/Z` (0x05c) and
`mPrevPosX/Y/Z` (0x068) — and `dBgActor_c`'s `mClsnMat` (0x2ec) was split out of
`pad_2ec` as a `u8` marker, the idiom the other twins in this family use.

Raw-offset collapses: `CleanupResources` reaches `mMeshCollider` directly and reads
`mVariant` by name instead of `((char *)this)[0x31e]`; `Render`'s whole-object
`struct Obj { char pad[0xd4]; Base sub; }` shadow is gone in favour of `&mModel`;
`Behavior`'s `(u16 *)&mBreakSoundState` cast is no longer needed now that the field
carries its real type.
