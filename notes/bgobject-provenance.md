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

Bodies read: `src/game/actors/StarSwitch/_ZN10StarSwitch13InitResourcesEv.cpp`,
`src/game/actors/StarSwitch/_ZN10StarSwitch8BehaviorEv.cpp`, `src/game/actors/StarSwitch/_ZN10StarSwitch6RenderEv.cpp`,
`src/game/actors/StarSwitch/_ZN10StarSwitch16CleanupResourcesEv.cpp`,
`src/game/actors/StarSwitch/_ZN10StarSwitch15OnGroundPoundedER8dActor_c.cpp`, `src/game/actors/StarSwitch/StarSwitch_Spawn.c`.

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

Bodies read: `src/game/actors/Crate/_ZN5Crate13InitResourcesEv.cpp`, `src/game/actors/Crate/_ZN5Crate8BehaviorEv.cpp`,
`src/game/actors/Crate/_ZN5Crate6RenderEv.cpp`, `src/game/actors/Crate/_ZN5Crate16CleanupResourcesEv.cpp`,
`src/game/actors/Crate/_ZN5Crate4KillEv.cpp`, `src/game/actors/Crate/_ZN5Crate13OnTurnIntoEggER6Player.cpp`,
`src/game/actors/Crate/_ZN5Crate13OnYoshiTryEatEv.cpp`,
`src/game/actors/Crate/_ZN5Crate15OnGroundPoundedER8dActor_c.cpp`, `src/game/objects/Crate_SetState.cpp`.

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

A swinging platform. Bodies read: `src/game/actors/daDgr_c/_ZN7daDgr_c13InitResourcesEv.cpp`,
`src/game/actors/daDgr_c/_ZN7daDgr_c8BehaviorEv.cpp`, `src/game/actors/daDgr_c/_ZN7daDgr_c6RenderEv.cpp`,
`src/game/actors/daDgr_c/_ZN7daDgr_c16CleanupResourcesEv.cpp`, `src/unnamed/ov025/0211/func_ov025_02111898.c` (the factory).

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

`src/game/actors/daDgr_c/_ZN7daDgr_c13InitResourcesEv.cpp` was an `extern "C"` free function over a raw
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

Bodies read: `src/game/actors/SignPost/_ZN8SignPost13InitResourcesEv.cpp`,
`src/game/actors/SignPost/_ZN8SignPost8BehaviorEv.cpp`, `src/game/actors/SignPost/_ZN8SignPost6RenderEv.cpp`,
`src/game/actors/SignPost/_ZN8SignPost16CleanupResourcesEv.cpp`, `src/game/actors/SignPost/_ZN8SignPost4KillEv.cpp`,
`src/game/actors/SignPost/_ZN8SignPost15OnGroundPoundedER8dActor_c.cpp`,
`src/game/actors/SignPost/_ZN8SignPost11OnAttacked1ER8dActor_c.cpp`,
`src/game/actors/SignPost/_ZN8SignPost15OnHitByMegaCharER6Player.cpp`.

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

`src/game/actors/SignPost/_ZN8SignPost8BehaviorEv.cpp` was an `extern "C"` free function over a raw
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

Both `src/game/actors/daPgDfdr_c/_ZN10daPgDfdr_c13InitResourcesEv.cpp` and
`src/game/actors/daPgDfdr_c/_ZN10daPgDfdr_c8BehaviorEv.cpp` were `extern "C"` free functions over a raw
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

Bodies read: `src/game/actors/Eyerok/_ZN6Eyerok13InitResourcesEv.cpp`, `src/game/actors/Eyerok/_ZN6Eyerok8BehaviorEv.cpp`,
`src/game/actors/Eyerok/_ZN6Eyerok6RenderEv.cpp`, `src/game/actors/Eyerok/_ZN6Eyerok16CleanupResourcesEv.cpp`,
`src/game/actors/Eyerok/_ZN6Eyerok16OnAimedAtWithEggEv.cpp`, `src/game/actors/Eyerok/Eyerok_Spawn.cpp`.

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

Bodies read: `src/game/actors/PushBlock/_ZN9PushBlock13InitResourcesEv.cpp`,
`src/game/actors/PushBlock/_ZN9PushBlock8BehaviorEv.c`, `src/game/actors/PushBlock/_ZN9PushBlock6RenderEv.cpp`,
`src/game/actors/PushBlock/_ZN9PushBlock8OnPushedER8dActor_c.cpp`, `src/game/actors/PushBlock/_ZN9PushBlock4KillEv.cpp`,
`src/game/actors/PushBlock/_ZN9PushBlock15OnHitByMegaCharER6Player.cpp`.

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

`src/game/actors/PushBlock/_ZN9PushBlock13InitResourcesEv.cpp` was an `extern "C"` free function over a raw
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
`src/game/actors/BlueCoinSwitch/_ZN14BlueCoinSwitch13InitResourcesEv.cpp`,
`src/game/actors/BlueCoinSwitch/_ZN14BlueCoinSwitch8BehaviorEv.cpp`, `src/game/actors/BlueCoinSwitch/_ZN14BlueCoinSwitch6RenderEv.cpp`,
`src/game/actors/BlueCoinSwitch/_ZN14BlueCoinSwitch16CleanupResourcesEv.cpp`.

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

## daObjCannonShutter_c (`include/daObjCannonShutter_c.h`, ov002, size 0x330)

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x320/0x324/0x328 | `mHomePosX/Y/Z` | `InitResources` copies `mPosX/mPosY/mPosZ`; `Behavior` restores the actor's position from them once the cannon is open, and passes `&mHomePosX` as a `Vector3 *` to `Vec3_Dist` and `Vec3_HorzDist` to measure how far the hatch has slid. |
| 0x32c | `mOpening` | gates the entire slide block in `Behavior`; nothing else reads it. |
| 0x32d | `mOpenPhase` | picks the direction of the slide — `0` moves the hatch down (`-0x1000` in Y), `1` moves it forward (`+0x2000` in Z) — and `Behavior` flips it to `1` once the hatch is more than `0xa000` from home. A two-step animation, not a flag. |
| 0x32e | `mCannonOpen` | `InitResources` sets it from `IsCannonOpenInCurLevel()`. `Render` refuses to draw while it is set, and `Behavior` parks the hatch at its home position with the collider disabled. `Behavior` also walks every actor of id `0xe` and sets THEIR copy when this hatch finishes sliding — so it is the level-wide "the cannon is open" fact, not a per-hatch animation flag. |

In the C twin, the `u8` marker at `0x2ec` became `mClsnMat`, the name
`include/dBgActor_c.h` gives that offset and what `InitResources` passes to
`dBgW_KcMbg::SetFile`.

The rename carried into `src_tu/actors/daObjCannonShutter_c.cpp`, along with the same
raw-offset collapses: `Render`'s whole-object `struct Obj { char pad[0xd4]; Sub sub; }`
shadow is gone in favour of `&mModel`, and `InitResources` reaches `mModel`,
`mMeshCollider` and `mClsnMat` by name.

---

## QuestionBlock (`include/QuestionBlock.h`, ov102, size 0x3f8)

Bodies read: `src/game/actors/QuestionBlock/_ZN13QuestionBlock13InitResourcesEv.cpp`,
`src/game/actors/QuestionBlock/_ZN13QuestionBlock8BehaviorEv.cpp`, `src/game/actors/QuestionBlock/_ZN13QuestionBlock6RenderEv.cpp`,
`src/game/actors/QuestionBlock/_ZN13QuestionBlock16CleanupResourcesEv.cpp`,
`src/game/actors/QuestionBlock/_ZN13QuestionBlock15OnGroundPoundedER8dActor_c.cpp`,
`src/game/actors/QuestionBlock/_ZN13QuestionBlock11OnAttacked1ER8dActor_c.cpp`,
`src/game/actors/QuestionBlock/_ZN13QuestionBlock8OnKickedER8dActor_c.cpp`,
`src/game/actors/QuestionBlock/_ZN13QuestionBlock15OnHitByMegaCharER6Player.cpp`,
`src/game/actors/QuestionBlock/_ZN13QuestionBlock19OnHitFromUnderneathER8dActor_c.cpp`.

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

Bodies read: `src/game/actors/FortressWall/_ZN12FortressWall13InitResourcesEv.cpp`,
`src/game/actors/FortressWall/_ZN12FortressWall8BehaviorEv.cpp`, `src/game/actors/FortressWall/_ZN12FortressWall6RenderEv.cpp`,
`src/game/actors/FortressWall/_ZN12FortressWall4KillEv.cpp`,
`src/game/actors/FortressWall/_ZN12FortressWall16CleanupResourcesEv.cpp`,
`src/game/actors/FortressWall/_ZN12FortressWall24OnHitByCannonBlastedCharER8dActor_c.cpp`,
`src/game/actors/FortressWall/FortressWall_Spawn.c`, `src/game/actors/FortressWallBreakable/FortressWallBreakable_Spawn.c`.

Two actors share this class: `FortressWallBreakable_Spawn` (actorID 0x30) and
`FortressWall_Spawn`. Every field is about telling those two apart.

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x31e | `mVariant` | `InitResources` sets 0 when `actorID == 0x30` and 1 otherwise, then uses it to index all three ov079 file tables — `data_ov079_02128058` (model), `data_ov079_0212805c` (KCL), `data_ov079_02128060` (CLPS block). `CleanupResources` indexes the first two again to `Release()` them. |
| 0x31f | `mStarId` | `param1 & 0xff`, with `0xff` read as 0. `Behavior` passes it as `dActor_c::Spawn(0xb2, mStarId \| 0x40, …)`; actor 0xb2 is the star and its spawn word is `starID \| (howToSpawnStar << 4)` (`src/game/actors/dActor_c/_ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h.cpp`), so the low nibble this contributes is a star index and the `0x40` is that call's spawn style. |
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

---

# Second batch — the rest of the `dBgActor_c` leaves

Same discipline as above: every name below is read off a **matched** body, and the
citation names the file and what the code does with the offset. Offsets that survive
as `unk_` are listed with the reason. Every class was re-verified with
`build_pin.verify` (`tools/build_pin.py`) against 2004/b56 after the rename, and the
tree was re-checked with `tools/rombuild.py --no-rom` (11,059 reproducing, 0
mismatching, 106/106 exact) and `tools/check_src_tu_compiles.py` (72/72).

---

## UkikiCage (`include/UkikiCage.h`, ov030, size 0x4e0)

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x4dc | `mStarActor` | `src/game/actors/UkikiCage/_ZN9UkikiCage13InitResourcesEv.cpp` stores what `dActor_c::Spawn(0xb2, (param1 & 0xf) or 0x50, ...)` returned; actor `0xb2` is the star (`src/game/actors/dActor_c/_ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h.cpp`). `src/game/actors/UkikiCage/_ZN9UkikiCage8BehaviorEv.cpp` writes that actor's `+0x5c/+0x60/+0x64` — `dActor_c::mPosX/Y/Z` — from the cage's own position plus `0x3c000` in Y on every falling frame. Declared type left `s32`; the store is still a cast. |

In the `#else` C twin, ten offsets already named at exactly those offsets in
`include/dActor_c.h` were repointed to those names: `mPosX/Y/Z` (0x05c),
`mScaleX/Y/Z` (0x080), `mAngleY` (0x08e), `mHorzSpeed`, `mVertAccel`,
`mTerminalVelocity` (0x098..0x0a0) and `mAreaId` (0x0cc).

---

## QuestionSwitch (`include/QuestionSwitch.h`, ov002, size 0x724)

Bodies read: `src/game/actors/QuestionSwitch/_ZN14QuestionSwitch13InitResourcesEv.cpp`,
`src/game/actors/QuestionSwitch/_ZN14QuestionSwitch8BehaviorEv.cpp`,
`src/game/actors/QuestionSwitch/_ZN14QuestionSwitch16CleanupResourcesEv.cpp`,
`src/game/actors/QuestionSwitch/_ZN14QuestionSwitch15OnGroundPoundedER8dActor_c.cpp`.

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x324 | `mStaticMeshCollider` | the first of the switch's two `dBgW_KcMbg`s. `InitResources` gives it CLPS block `data_ov002_0210d8b4` and points `mActiveMeshCollider` at it when the saved bit `data_0209caa0[1] & 0x80000000` is clear; `Behavior` re-points `mActiveMeshCollider` at `mMovingMeshCollider` on the frame the switch fires, and back at this one whenever that bit is clear. So this is the collider that is live before the switch has been used. |
| 0x718 | `mPressTimer` | `InitResources` sets `8`; `OnGroundPounded` sets `0`; `Behavior` runs `DecIfAbove0_Byte(&mPressTimer)` only while `mPressedThisFrame` is set, and the frame it reaches zero is the frame the switch fires. When `mPressedThisFrame` is clear it is re-armed to `8`. A countdown to activation, not a state. |
| 0x71a | `mPressedThisFrame` | gates that countdown, and the last statement of every `Behavior` clears it. No matched body sets it — the setter is in un-decompiled collision code — but "cleared once a frame and read only as a gate" is what a one-frame latch is. |
| 0x71b | `mTalking` | set to `1` on the frame the switch fires, immediately after the four `Particle::System::NewSimple` calls; gates the whole `Player::GetTalkState` / `Message::PrepareTalk` / `Player::ShowMessage` / `Message::EndTalk` block, and is cleared together with `mTalkingPlayer` when `Player::HasFinishedTalking` returns nonzero. |
| 0x71c | `mSoundDelay` | armed with `0x4b` on the frame the talk starts and run down by `DecIfAbove0_Short`; on the frame it reaches zero `Behavior` calls `Sound::PlaySub(0x20, 0x7f, 0, 0x8777, 0)`. |

In the C twin, `unk_124` became `mMeshCollider`, the name `include/dBgActor_c.h`
gives that offset.

---

## CccArena (`include/CccArena.h`, ov073, size 0x33c)

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x320 | `mState` | `src/game/actors/CccArena/_ZN8CccArena8BehaviorEv.cpp` reads the WORD here as a pointer, tests `*(p + 8)` and calls the pointer-to-member pair at `p + 8` through `this`. The same shape `Eyerok`'s own 0x48c has. `InitResources` installs it through `func_ov073_021223a4(this, &data_ov073_021234b0)`. Declared type left `u8`. |
| 0x32c | `mVariant` | `InitResources` sets `0`/`1`/`2` for actorID `0xaa`/`0xab`/`0xac` and then uses it as the row index into all three 0xc-stride ov073 tables — `data_ov073_021231bc` (model), `...1c0` (KCL), `...1c4` (CLPS). `CleanupResources` indexes the first two again to `Release()` them. Was inside `pad_321`. |
| 0x32d | `mSpawnIndex` | `InitResources` copies the value of a per-variant global counter (`data_ov073_02123424` for `0xab`, `...3420` for `0xac`) and then increments that counter — a serial number among the arena pieces of this variant. Was inside `pad_321`. |

Left `unk_`: `0x330` (set to `3` on all three paths, never read in a matched body),
`0x334` and `0x338` (both zeroed by `InitResources`, never read).

Raw-offset collapses, each re-verified byte-exact: the six
`((char *)this)[0x32c]` / `[0x32d]` reads and writes in `InitResources`, the two in
`CleanupResources`, and `Behavior`'s `*(void**)((char*)&unk_320)`.

---

## BowserFireSeaArena (`include/BowserFireSeaArena.h`, ov060, size 0x570)

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x31e | `mAngleXSpeed` | `src/game/actors/BowserFireSeaArena/_ZN18BowserFireSeaArena8BehaviorEv.cpp` adds it to `dActor_c::mAngleX` every frame and does nothing else with it. |
| 0x320 | `mAngleYSpeed` | the same, into `mAngleY`. |
| 0x322 | `mAngleZSpeed` | the same, into `mAngleZ`. |

`InitResources` zeroes all three, so their only evidenced role is the one `Behavior`
gives them. `0x56c` is zeroed and never read in a matched body; it stays `unk_`.

**Byte-rejected lever.** `Behavior` reaches all three angles through
`short *a = (short *)(((int)this) + 0x8c);` and friends. Spelling those three
statements as `mAngleX = mAngleX + mAngleXSpeed;` costs the function its size
(`999 word(s) differ`) — the same "integer-first address materialisation is the
ROM's own" result `Eyerok`'s `mDustCounter` produced. The raw form stays.

In the C twin, `unk_08e` was repointed to `mAngleY`.

---

## TtcRotatingGear (`include/TtcRotatingGear.h`, ov065, size 0x330)

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x320/0x324/0x328 | `mHomePosX/Y/Z` | `InitResources` copies `mPosX/mPosY/mPosZ` into them; `Behavior` clamps `mPosY` into `[mHomePosY, mHomePosY + 0x14a000]` on every timer expiry, and pins the gear at `mHomePosY + 0x14a000` when the clock-hand setting is 3. |
| 0x32c | `mMoveTimer` | `DecIfAbove0_Short`'d once a frame; on expiry the gear flips `mMoveDir` and reloads both this and `mVertSpeed` from the two 16-byte-stride per-setting tables `data_ov065_0211c0d4` / `...c0d0`, or from a random multiple of `0x14` under setting 2. |

In the C twin, `0x0a0` becomes `mTerminalVelocity` and `0x0a8` becomes `mVertSpeed`.

---

## SeesawBob (`include/SeesawBob.h`, ov095, size 0x328)

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x31e | `mVariant` | `InitResources` sets `0`..`6` from actorID (`0x1c`, `0x27`, `0x85`, `0x8f`, `0x95`, `0x96`, `0x80`) and uses it as the row index into all three 0xc-stride ov095 tables `data_ov095_021374a0/a4/a8`; `CleanupResources` indexes the first two again. Was inside `pad_31e`. |
| 0x320 | `mTiltSound` | `Behavior` passes it as the first argument of `Sound::PlayLong(u32, 3, 0x8b, ...)` and stores the result back — a recycled long-sound handle, replayed only while the tilt speed's magnitude is above `0xa`. |
| 0x324 | `mAngleXSpeed` | `Behavior` passes `&mAngleX` and `&mAngleXSpeed` as consecutive arguments of `func_ov095_021358cc(this, ..., 0, 6, 3, 3)`, the spring step, and then gates the creak sound on this field's magnitude. |
| 0x326 | `mPoundedThisFrame` | `OnGroundPounded` sets it to `1`; `Behavior` runs the spring step only when it is `0` and clears it with its last statement. |

Raw-offset collapses, each re-verified byte-exact: the nine `((char *)this)[0x31e]`
sites across `InitResources` and `CleanupResources`, and `a[0x326] = 1;` in
`OnGroundPounded`. In the C twin, `actorID` (0x00c), `mAngleX`/`mAngleY` (0x08c) and
`mFlags` (0x0b0) were repointed to the names their own headers give them.

---

## RotatingCogSmall (`include/RotatingCogSmall.h`, ov035, size 0x330)

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x31e | `mStepTimer` | `DecIfAbove0_Short`'d in `Behavior`, and reloaded from `data_ov035_02111ef4[mRotationState][setting]` — the same table `InitResources` seeds it from. |
| 0x320 | `mDirTimer` | a second `DecIfAbove0_Short` countdown, reached only under clock setting 2, and the only thing it gates is the random re-roll of the step's sign. |
| 0x322 | `mTargetAngleY` | the `to` argument of `ApproachLinear(&mAngleY, mTargetAngleY, 0xc8)`, and the only field the step is added to. |
| 0x324 | `mAngleYStep` | added to `mTargetAngleY` once per expiry; its magnitude is `data_ov035_02111ef0[mRotationState]` and its sign is what the setting-2 re-roll picks. |

Left `unk_`: `0x326`, split out of `pad_326`. `InitResources` writes it the same
`data_ov035_02111ef0[mRotationState]` value it gives `mAngleYStep`, and no matched
body reads it — a write alone does not say what a field is *for*.

Raw-offset collapse, re-verified byte-exact: `InitResources`' running
`char *r = ((char *)this) + 0x300;` with `*(s16*)(r + 0x1e)`, `+ 0x24` and `+ 0x26`
is three named member stores now.

---

## IceBlock (`include/IceBlock.h`, ov081, size 0x368)

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x354 | `mMeltTimer` | `Behavior` sets `0x1e` on the frame the collider reports hit bit `0x40000`; while it is nonzero it recomputes `mScale = cstd::fdiv(mMeltTimer << 12, 0x1e000)` — a 30-frame ramp from 1.0 to 0 — and on the frame `DecIfAbove0_Byte` returns 0 it calls `fBase_c::MarkForDestruction`. |
| 0x35c | `mParticleHandle1` | passed as the first argument of `Particle::System::New(..., 0x77, ...)` and overwritten with that call's result; the handle is then looked up through `Particle::System::FromUniqueID` to scale the effect by `mScale * 0xf`. |
| 0x360 | `mParticleHandle2` | the same, effect `0x78`. |
| 0x364 | `mContainedActor` | `Behavior` loads the WORD as an actor pointer, reads `+0xc` (`dActor_c::actorID`) off it and compares against `0xb2` — the star — then clears that actor's `+0x49f`. `Kill`'s `func_ov081_02127be0` is the same handoff. |

In the C twin, `0x340`/`0x344` were repointed to `mdCcAc_c_hitFlags` /
`mdCcAc_c_otherOwner` — `0x320 + 0x20` and `+ 0x24`, which `include/dCc_c.h` names
`hitFlags` and `otherOwner`. On the C++ side `Behavior`'s two
`*(s32 *)((char *)&mdCcAc_c + 0x20)` reads are `mdCcAc_c.hitFlags` and
`mdCcAc_c.otherOwner` now, re-verified byte-exact.

---

## DonutBlock (`include/DonutBlock.h`, ov036, size 0x4ec)

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x4dc/0x4e0/0x4e4 | `mHomePosX/Y/Z` | `InitResources` copies `mPosX/mPosY/mPosZ` into them once; `Behavior`'s state 1 — the frame the block has fallen out of range — writes all three straight back into `mPosX/mPosY/mPosZ` before parking the block in state 2. |

In the C twin, `0x09c` becomes `mVertAccel`, `0x0a0` `mTerminalVelocity`,
`0x0a8` `mVertSpeed`.

---

## daObjCtMecha05_c (`include/daObjCtMecha05_c.h`, ov065, size 0x394)

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x320/0x324/0x328 | `mHomePosX/Y/Z` | `InitResources` copies `mPosX/mPosY/mPosZ`; under clock setting 3 `Behavior` computes `mPosX = mHomePosX + sin * 0xfa000` and `mPosZ = mHomePosZ + cos * 0xfa000` from `data_02082214`. |
| 0x32c | `mTravel` | `Behavior` adds `mHorzSpeed` to it every tick and compares it against `0xfa000` — the same magnitude the home offset uses — and against `0`, to decide when the arm has reached the end of its run. |
| 0x330 | `mPrevTravel` | assigned `mTravel` at the top of every tick, and the SIGN of `(0xfa000 - mTravel) * (0xfa000 - mPrevTravel)` is the "did we cross the end this frame" test. |
| 0x334 | `mStateTimer` | `DecIfAbove0_Short`'d in states 0, 1 and 3; seeded from `data_ov065_0211c0c8[setting]` and re-armed with `0x1e` and with random values under setting 2. |
| 0x336 | `mState` | the switch key, `0`..`3`, only ever incremented in place. |
| 0x338 | `mGroundY` | `InitResources` probes `0xa000` below the actor with a `dBgCh_Gnd` and stores the hit height here, falling back to the probe point. |

---

## daObjCtMecha03_c (`include/daObjCtMecha03_c.h`, ov065, size 0x388)

A pendulum, in four fields `Behavior` integrates. Bodies read:
`src/game/actors/daObjCtMecha03_c/_ZN16daObjCtMecha03_c13InitResourcesEv.cpp`,
`src/game/actors/daObjCtMecha03_c/_ZN16daObjCtMecha03_c8BehaviorEv.cpp`.

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x31e | `mSwingDir` | `1` at `InitResources`. `Behavior` negates it exactly when it agrees in sign with `mSwingAngle` — a restoring force, which is what makes this a pendulum rather than a spinner. |
| 0x320 | `mSwingAccel` | `data_ov065_0211c0b0[setting]` at `InitResources`, re-rolled to `0xd` or `0x2a` under setting 2; the only use is `mSwingSpeed += mSwingAccel * mSwingDir`. |
| 0x322 | `mSwingAngle` | `0x1964` at `InitResources`, which immediately copies it into `mAngleZ`; `Behavior` adds `mSwingSpeed` to it and copies it into `mAngleZ` again on every tick. |
| 0x324 | `mSwingSpeed` | accumulates `mSwingAccel * mSwingDir`; when it is `0` the class re-rolls the setting-2 parameters and reloads `mSoundTimer`. |
| 0x326 | `mSoundTimer` | `DecIfAbove0_Short`'d while nonzero; on the frame it reaches zero `Behavior` calls `Sound::PlayBank3(0x38, &mCamSpacePosX)`. Reloaded as `mPauseTimer + 0xf`. |
| 0x328 | `mPauseTimer` | `DecIfAbove0_Short`'d once a frame and gates the WHOLE swing update; re-armed to `((u32)rand >> 0x1b) + 3` under setting 2. |

---

## RotatingBridge (`include/RotatingBridge.h`, ov015, size 0x324)

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x31e | `mPauseTimer` | `InitResources` sets `0x3c`; `Behavior` turns the bridge only on the frame `DecIfAbove0_Byte` returns 0, and re-arms it to `0x3c` whenever `mPrevAngleY & 0x7fff` comes out zero — i.e. at each quarter turn. |
| 0x320 | `mTurnSound` | passed as the first argument of `Sound::PlayLong(mTurnSound, 3, 0x88, &mCamSpacePosX, 0)` and overwritten with the result. |

In the C twin, `0x074` becomes `mCamSpacePosX`, `0x08e` `mAngleY`, `0x094`
`mPrevAngleY`.

---

## PyramidTop (`include/PyramidTop.h`, ov024, size 0x3b8)

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x370 | `mClsnMat2` | `InitResources` passes `this + 0x370` as the `const Matrix4x3 &` argument of `dBgW_KcMbg::SetFile` — dBgActor_c's own `mClsnMat` at 0x2ec is a different object. A `Matrix4x3` is 0x30 bytes and `0x370 + 0x30 = 0x3a0`, the next evidenced field. Was `pad_370[0x30]`; left a `u8` marker, the idiom the family's C twins use. |
| 0x3a0/0x3a4/0x3a8 | `mHomePosX/Y/Z` | `InitResources` copies `mPosX/mPosY/mPosZ` into them once, in the same run that zeroes `mAngVelY`, `mNumTagsTriggered`, `mState`, `mSpinParticleID` and `mSoundTimer`. |

In the C twin, `0x074` becomes `mCamSpacePosX`.

---

## KnockDownPlank (`include/KnockDownPlank.h`, ov015, size 0x39c)

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x378/0x37c/0x380 | `mFrontPosX/Y/Z` | `InitResources` builds `(0, 0, 0x32000)`, rotates it by `mAngleY` through `data_020a0e68` with `Matrix4x3_FromRotationY` + `MulVec3Mat4x3`, adds the plank's own position, and stores the result here. The block just above does the same thing to derive `mFrontFloorY`, the ground height under that point. |

---

## BigBrickBlock (`include/BigBrickBlock.h`, ov002, size 0x330)

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x31e | `mBroken` | `Kill()` sets it to `1` on the switch-activated variant *instead of* destroying the block. `Render` draws nothing and `Behavior` takes the broken path while it is set, and `Behavior` clears it on the frame `Event::GetBit(mEventID)` disagrees with `mPrevEventBit`. |
| 0x31f | `mPrevEventBit` | `InitResources` stores `Event::GetBit(mEventID)` here; `Behavior`'s only use is that comparison. |
| 0x328 | `mLinkedActor` | already typed `dActor_c *`. `func_ov002_020b363c` loads this word, reads `actorID` at `+0xc` off it and writes `+0x3b0` or `+0xd6`; `Kill` only tests it against null. |

---

## ArrowSignRight (`include/ArrowSignRight.h`, ov098, size 0x380)

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x348 | `mShadowMat` | `Behavior` passes `&mShadowMat` as the `Matrix4x3 &` argument of `dActor_c::DropShadowScaleXYZ(ShadowModel &, Matrix4x3 &, ...)`, with `mShadowModel` as the argument before it. `0x348 + 0x30 = 0x378`. The same shape `SignPost` and `QuestionBlock` already carry. |
| 0x37c | `mVariant` | `InitResources` sets `0`/`1` from actorID and uses it as the row index into all three ov098 resource columns `data_ov098_0213c380/384/388`. |

The rename carried into `src_tu/actors/ArrowSignRight.cpp` as well as `src/`.
In the C twin, `0x00c` becomes `actorID` and `0x08e` `mAngleY`.

---

## The one-and-two-field leaves

| Class | Offset | Name | Evidence |
| --- | --- | --- | --- |
| `daObjEmmLog_c` (ov052) | 0x320 | `mBasePosY` | `InitResources` copies `mPosY`; `Behavior` computes `mPosY = mBasePosY + (sine * mBobAmplitude >> 12)` from `data_02082214`. |
| | 0x324 | `mBobAmplitude` | `InitResources` sets `0x64000`, or the spawn byte times `0xa000`; it is the multiplicand of that sine. |
| `TTC_MovingBeam` (ov065) | 0x330 | `mGroundY` | `InitResources` stores the probe point's Y, then overwrites it with the `dBgCh_Gnd` hit height when `DetectClsn` returns nonzero. |
| `TTC_MovingBar` (ov065) | 0x31e | `mVariant` | `InitResources` sets `0`/`1` and uses it as the row index into `data_ov065_0211d35c` / `...d360`; `CleanupResources` indexes both again. |
| | 0x320 | `mGroundY` | the same raycast shape as `TTC_MovingBeam`'s: the probe Y, replaced by `raycast + 0x44` on a hit. |
| `SlidingIce` (ov027) | 0x31e | `mDelayTimer` | `DecIfAbove0_Short`'d at the top of both variants of `Behavior`, which do nothing at all until it reaches 0; seeded `0x64` or `mNumToBigIce * 0x14` and re-armed to `(mNumToBigIce + 1) * 0x14` after each spawn. |
| `PyramidStep` (ov025) | 0x374 | `mClsnMat2` | `InitResources` passes `&mClsnMat2` as the `const Matrix4x3 &` of `dBgW_KcMbg::SetFile`, and `0x374 + 0x30 = 0x3a4` — the factory's own `operator new` literal. The header's `pad_378[0x2c]` "tail padding" WAS this matrix; it is gone and the size assert now closes on a field span. |
| `PathLift` (ov002) | 0x42a | `mAfterClsnRan` | set to `1` by the last statement of `AfterClsn`, cleared by the last statement of `BaseBehavior`. |
| | 0x42b | `mTriggerDelay` | `AfterClsn` fires `func_ov002_020efa54(this, 1)` only when `DecIfAbove0_Byte(&mTriggerDelay)` returns 0 and `mState == 0`. |
| `WDW_Water` (ov029) | 0x338 | `mUseSpawnPosY` | `InitResources` sets `param1 & 1`, and when it is clear — and only then — overrides `mPosY` from `data_ov029_02112b2c[clock setting]` before snapshotting `mTargetPosY`. |
| `ChainChompFence` (ov060) | 0x31e | `mDisabled` | both `Behavior` and `Render` return immediately while it is nonzero, and nothing else in a matched body touches it. |
| `LavaPlank` (ov022) | 0x324 | `mPhaseAngle` | `InitResources` seeds it from `mAngleX`; `Behavior` adds `0x400` per frame and uses `(u16)mPhaseAngle >> 4` as the sine-table index. |

`PathLift::mAfterClsnRan` also carried into `src/game/actors/daObjRcCarpet_c/_ZN15daObjRcCarpet_c8BehaviorEv.cpp`,
a subclass that reads the inherited field — the kind of cross-file breakage a header
rename in this family causes, and which `tools/rombuild.py` catches while
`build_pin.verify` on the renamed class alone does not.

---

## C-twin repointing, no new names

These classes had no unnamed field of their own left; the `unk_` that remained in
their headers were `dActor_c`'s / `dBgActor_c`'s / `fBase_c`'s own storage restated
flat by the C twin, and were repointed to the names those headers already give the
same offsets:

* `include/ShipWater.h` — `mAngleY` (0x08e), `mClipOffsetY`, `mClipRadius`,
  `mClipDistance`, `mFarDistance` (0x0b4..0x0c0), `mClipResult` (0x0c4),
  `mDeathTableID` (0x0ce).
* `include/Trap.h` — `mPosX/Y/Z` (0x05c), `mAngleY` (0x08e), `mAreaId` (0x0cc).
  The same five carried into the TU-local `struct TrapFlat` in
  `src_tu/actors/Trap.cpp`, which `tools/check_src_tu_compiles.py` proves.
* `include/TowerStep.h` — `mHorzSpeed`, `mTerminalVelocity`, `mVertSpeed`.
* `include/MetalNet.h` — `param1` (0x008), `mAngleY`, `mClsnMat` (0x2ec).
* `include/PoleLift.h` — `param1`, `mAngleY`.
* `include/IceSheet.h` and `include/RotatingFirebar.h` — `mAngleY`, and `mFlags`
  (0x0b0).
* `include/Squasher.h` — `mCamSpacePosX` (0x074), `mClsnMat` (0x2ec).
* `include/FortressTower.h` — `actorID` (0x00c).
* `include/BasementWater.h` — `mCamSpacePosX`.
* `include/TTC_MovingBeam.h` — `mTerminalVelocity`, `mVertSpeed`, `mClsnMat`.
* `include/SlidingIce.h` — `mHorzSpeed`.
* `include/PyramidStep.h` — `param1`, `mAngleY`, `mVertSpeed`.
* `include/PathLift.h` — `actorID`.
* `include/daObjEmmLog_c.h` — `mPosY`.
* `include/RotatingCogSmall.h` — `actorID`, `mAngleY`.

## Left `unk_` across this batch, and why

* `CccArena` 0x330 / 0x334 / 0x338 — written once each by `InitResources`, never read.
* `RotatingCogSmall` 0x326 — written the same table value as `mAngleYStep`, never read.
* `BowserFireSeaArena` 0x56c — zeroed, never read.
* `RotatingUpDownPlatformUtm` 0x300 in the C twin — that offset is *interior* to
  `dBgActor_c::mClsnMat` (0x2ec + 0x14), and naming a matrix element from a single
  `s16` read would be an invention.
* The classes with no fields of their own — `MetalNet`, `IceSheet`,
  `FortressTower`, `HugeWater` — have nothing left to name.
