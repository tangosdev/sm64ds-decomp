# dActor_c leaf classes -- field-naming provenance

Evidence for every `unk_NNN` renamed on a class whose immediate base is
`dActor_c` (excluding `Player`, `Bowser` and `dBgActor_c`, which have their own
notes). Naming is byte-neutral: nothing here changes a compiled byte, and every
row was taken with `reproducing: 11,059`, `mismatching: 0`,
`module fidelity: 106/106 exact` on both sides of the edit.

A row exists only where an enrolled, byte-matching body says what the field is.
Where a field is only ever written and never read back, it keeps its `unk_`
name; those cases are listed under "deliberately left `unk_`" per class, because
"we looked and there was nothing" is itself worth writing down.

Name collisions with the ancestor chain are the standing hazard here -- every
name below was checked against `include/dActor_c.h`, `include/dBase_c.h` and
`include/fBase_c.h` first. A derived field that reuses a base spelling compiles
cleanly and silently rebinds every unqualified use across the tree.

## Ukiki -- include/Ukiki.h

| offset | new name | evidence |
| --- | --- | --- |
| 0x38c | `mSpawnPosX` | `InitResources` copies `mPosX` in; never written again. |
| 0x390 | `mSpawnPosY` | same, `mPosY`. |
| 0x394 | `mSpawnPosZ` | same, `mPosZ`. |
| 0x3ac | `mCapUniqueID` | set to the `fBase_c::uniqueID` (`+0x4`) of the actor `dActor_c::Spawn(0x10d, ...)` just returned. |
| 0x3b0 | `mCapPlayerNo` | read out of `ClosestPlayer()->param1` (`+0x8`), guarded `< 3`, then shifted into the spawn parameter as `(mCapPlayerNo << 8) | 2`. |
| 0x3c8 | `mHasSpawnedCap` | tested `== 0` before the spawn block and latched to 1 inside it, so the cap is spawned at most once. |

Both `src/_ZN5Ukiki13InitResourcesEv.cpp` and `src/_ZN5Ukiki8BehaviorEv.cpp`
run the same block; only the first names its fields, the second still reaches
them as raw `c + 0xNN` offsets.

Deliberately left `unk_`: 0x380/0x384/0x388 (a second position triple, seeded
from `mPos` with `0x64000` added to Y right after -- no enrolled body reads it
back, so what it is for is unevidenced); 0x3a8 (zeroed, never read); 0x3cb (set
to 0x96, never read).

## Door -- include/Door.h

| offset | new name | evidence |
| --- | --- | --- |
| 0x138 | `mKeyModel` | `new Model` + `ModelBase::SetFile` in `src/_ZN4Door13InitResourcesEv.c`; `Virtual10(mModel.data.transforms)` then `Render(0)` in `src/_ZN4Door6RenderEv.cpp`, where the local holding it is already called `key`; `delete key` through Model's vtable slot 1 in `src/_ZN4Door16CleanupResourcesEv.cpp`. Owned by the Door. |
| 0x13c | `mKeyFile` | handed to `Model::LoadFile` and `Release()`d as a `SharedFilePtr`. Three sources in `InitResources`: [data_ov002_0211094c](../config/arm9/overlays/ov002/symbols.txt), `func_02132894[mKeyModelIdx + 1]` for the keyed-door `param1` range, else [data_ov089_02132c50](../config/arm9/overlays/ov089/symbols.txt). |
| 0x140 | `mCallbackNode` | `src/_ZN4Door8BehaviorEv.cpp` casts it to a node whose `+0x8` is a `void (Door::*)(int)` and calls it on this Door. |
| 0x144 | `mKeyModelIdx` | `param1 - 8` for `param1` in 9..0xd, re-zeroed for `param1 == 0xc`; indexes `LoadKeyModels` and `func_02132894`. The header already carried "key-model index" as a comment. |

Not touched: `unk_0a4` / `unk_0ac` reached through `self->base` are
`dActor_c`'s own fields, and `include/dActor_c.h` is frozen for this pass --
the existing note in `src/_ZN4Door13InitResourcesEv.c` about that triple stands.

## daSCoin_c -- include/daSCoin_c.h

Silver coins, actor 0x149. One coin elects itself leader of the set and stamps
the rest, so the fifth collection can destroy the whole set from one place.

| offset | new name | evidence |
| --- | --- | --- |
| 0x108 | `mLeaderUniqueID` | the leader writes its own `fBase_c::uniqueID` (`c + 4`) here and into every other 0x149 actor found through `dActor_c::FindWithActorID`. |
| 0x10e | `mGroupId` | `(param1 >> 8) & 0xf` in `InitResources`; the election in `Behavior` fires only for the values 0 and 0xf. |
| 0x10f | `mGroupRole` | 0 until the election, 1 on the coin that claims it, 2 written into every other coin. Both the election guard and the destroy test read it. |
| 0x110 | `mCollectedCount` | leader destroys itself at `mGroupRole == 1 && mCollectedCount == 5` -- the five silver coins of a mission. |
| 0x111 | `mClsnDisabled` | nonzero suppresses the `dCcAc_c` member's per-frame `Update()`; its `Clear()` runs either way. |
| 0x113 | `mDeathTimer` | counted down by `DecIfAbove0_Byte` at the top of `Behavior`; the frame it reaches 0 the coin runs [func_ov002_020f05f4](../config/arm9/overlays/ov002/symbols.txt) and marks itself for destruction. Zero means "not dying". |

Sources: `src/_ZN9daSCoin_c13InitResourcesEv.cpp`,
`src/_ZN9daSCoin_c8BehaviorEv.cpp`.

Deliberately left `unk_`: 0x10d (`param1 & 0xf`, written and never read);
0x112 (already documented as touched only by the class's unenrolled
[func_ov002_020f051c](../config/arm9/overlays/ov002/symbols.txt)).

## Pokey -- include/Pokey.h

A Pokey is two actors: the head (`actorID` 0xf0) and its body segments
(`actorID` 0xf1). `src/_ZN5Pokey13InitResourcesEv.cpp` branches on that all the
way through.

| offset | new name | evidence |
| --- | --- | --- |
| 0x33c | `mMatrix` | `*(Matrix4x3*)&unk_33c = IDENTITY_MATRIX4X3` in `InitResources`, and 0x33c..0x36b is exactly the 0x30 bytes a `Matrix4x3` occupies. Kept spelt `u8` + pad so the header need not pull in `math/Matrix.h`. |
| 0x36c | `mRootPosX` | head seeds it from its own `mPosX`; a segment copies it word for word out of the head object at the same 0x36c offset. |
| 0x370 | `mRootPosY` | same. |
| 0x374 | `mRootPosZ` | same. |
| 0x38c | `mState` | `src/_ZN5Pokey8BehaviorEv.cpp` skips its distance-to-player early-out when this is 2 or 5. |
| 0x390 | `mHead` | a segment stores `dActor_c::FindWithID(param1)` here; the head stores 0. A `dActor_c*` spelt `s32` and cast at every use. |
| 0x394 | `mNextSegment` | `src/_ZN5Pokey16OnPendingDestroyEv.cpp` (head only) walks `p = mNextSegment` and then `p->mNextSegment` at the same 0x394 offset, tearing down each segment in turn. |

Deliberately left `unk_`: 0x3a8 (set to 1 by the head after it loads the
blue-coin model, never read by an enrolled body).

## BabyPenguin -- include/BabyPenguin.h

| offset | new name | evidence |
| --- | --- | --- |
| 0x350 | `mSpawnPosX` | `src/_ZN11BabyPenguin13InitResourcesEv.cpp` copies `mPosX` in; never written again. |
| 0x354 | `mSpawnPosY` | same, `mPosY`. |
| 0x358 | `mSpawnPosZ` | same, `mPosZ`. |
| 0x364 | `mCachedActor` | `src/_ZN11BabyPenguin8BehaviorEv.cpp`: `if (mCachedActor == 0) mCachedActor = FindWithActorID(0x101, 0)`, lazily filled and never cleared. A `dActor_c*` spelt `s32`. |

`mCachedActor` is named for what it holds and not for what it means on purpose:
nothing in the tree names actor 0x101, and no enrolled body reads the pointer
back, so a name like "mMotherPenguin" would be a claim the bytes do not make.

Deliberately left `unk_`: 0x360 (zeroed, never read); 0x36c (set to 0x384 every
frame the penguin is near the player, never read).

## Lakitu -- include/Lakitu.h

| offset | new name | evidence |
| --- | --- | --- |
| 0x3c0 | `mMatrix` | `*(Matrix4x3*)&unk_3c0 = IDENTITY_MATRIX4X3` in `src/_ZN6Lakitu13InitResourcesEv.cpp`. The pad after it runs to 0x3f3, four bytes past the matrix's own 0x30. |
| 0x3f4 | `mState` | `src/_ZN6Lakitu6RenderEv.cpp` draws the second `Model` only when this is 1. |
| 0x3f8 | `mSpawnPosX` | `InitResources` copies `this + 0x5c` (`mPosX`) in. |
| 0x3fc | `mSpawnPosY` | same, `this + 0x60` (`mPosY`). |
| 0x400 | `mSpawnPosZ` | same, `this + 0x64` (`mPosZ`); this is the only one of the three spelt as a member, the other two are raw `this + 0xNN` stores. |

Deliberately left `unk_`: 0x410 (zeroed, never read).

## Moneybag -- include/Moneybag.h

| offset | new name | evidence |
| --- | --- | --- |
| 0x3a0 | `mMatrix` | `*(Matrix4x3*)&unk_3a0 = IDENTITY_MATRIX4X3`; 0x3a0..0x3cf is exactly 0x30 bytes. |
| 0x3d0 | `mSpawnPosX` | `src/_ZN8Moneybag13InitResourcesEv.cpp` copies `mPosX` in. |
| 0x3d4 | `mSpawnPosY` | same, `mPosY`. |
| 0x3d8 | `mSpawnPosZ` | same, `mPosZ`. |
| 0x3f0 | `mState` | set to 1 by `InitResources`; `src/_ZN8Moneybag6RenderEv.cpp` draws the `ModelAnim` only above 1 and the `Model` only at or below 0x1f. |

## Coin -- include/Coin.h

| offset | new name | evidence |
| --- | --- | --- |
| 0x3ab | `mSpawnFilter` | `param1` bits 4..6 for a red (0x121) or blue (0x122) coin, 0xff otherwise. The same three-bit value `LoadObjects` compares against `data_0209f220` to decide whether an object belongs to the entrance the level was started from (`src/_Z11LoadObjectsRN11LVL_Overlay8ObjTableEij.cpp`). A red coin claims a star-marker slot only when it matches (or `SublevelToLevel` is 0x13); a blue coin whose filter is under 8 clears bit 0 of `mCoinFlags`. |
| 0x3ae | `mCoinFlags` | the flag byte `src/_ZN4Coin13InitResourcesEv.cpp` already documented as such: bit 0 gates `Render` outright, `Behavior` tests bits 0 and 1. Named `mCoinFlags` and NOT `mFlags`, which is `dActor_c`'s own field 0x0b0. |

The read-modify-write sites keep their raw `*(u8*)((int)c + 0x3ae)` spelling --
that launder is measured and per-site, and the existing note in
`src/_ZN4Coin13InitResourcesEv.cpp` explains why.

## PowerFlower -- include/PowerFlower.h

| offset | new name | evidence |
| --- | --- | --- |
| 0x3bc | `mGroundY` | `src/_ZN11PowerFlower13InitResourcesEv.cpp` raycasts a `dBgCh_Gnd` from `mPos` with Y + 0x14000 and stores the hit height (`ray + 0x44`), falling back to the probe's own Y when `DetectClsn` finds nothing. |
| 0x3c0 | `mState` | `src/_ZN11PowerFlower6RenderEv.cpp` switches on it: 0 draws `mModel1`, 1 and 2 draw `mModel2`. |
| 0x3ca | `mLifeTimer` | seeded 0xb4 (180 frames) in `InitResources`; `Render` skips drawing on odd values once it is below 0x2d, so the flower blinks through its last 45 frames. |

## Number -- include/Number.h

The floating score popup, actor 0x14a -- `dActor_c::SpawnNumber` is what puts
one up, and its parameter list is half the evidence here.

| offset | new name | evidence |
| --- | --- | --- |
| 0x138 | `mOwnerUniqueID` | `src/_ZN6Number8BehaviorEv.cpp` resolves it through `dActor_c::FindWithID` and, when the actor still exists, takes the popup's draw position from that actor's own `mPos` triple. 0 means "not following anything" -- the `owner` argument of `SpawnNumber`. |
| 0x13c | `mStartPosX` | copied from `mPosX` in `src/_ZN6Number13InitResourcesEv.cpp`, alongside the already-named `mStartPosY` at 0x140. |
| 0x144 | `mStartPosZ` | same, `mPosZ`. |
| 0x148 | `mFollowOffsetY` | added to `mPosY - mStartPosY` -- how far the popup has risen -- when the position is taken from the owner. |
| 0x14c | `mDelay` | the `delay` argument of `SpawnNumber`: while nonzero `Behavior` returns immediately and `Render` decrements it and draws nothing. |
| 0x14e | `mState` | the bounce. 0: rising, and the first frame back below `mStartPosY` snaps to that floor, sets `mVertSpeed` 0xf000 and advances to 1. 1: the next fall below the floor emits particle 0xd2 and marks the popup for destruction. |

## CrazedCrate, MontyMole, BowserShockwaves -- nothing named

Listed so the next reader does not repeat the search.

- `CrazedCrate` 0x374: zeroed in `src/_ZN11CrazedCrate13InitResourcesEv.cpp`, never read.
- `MontyMole` 0x17c/0x180..0x184/0x188 and the 16 bytes from 0x16c: all written
  in `src/_ZN9MontyMole13InitResourcesEv.cpp` -- 0x180 is `param1 & 0xf`, 0x182
  is `(param1 >> 4) & 0xf`, 0x181 is 1 when 0x180 is 0 and `(param1 >> 8) & 1`
  otherwise -- and none of them is read by any enrolled body. Which nibble means
  what is a guess until the class's other functions are enrolled.
- `BowserShockwaves` 0x214: zeroed in
  `src/_ZN16BowserShockwaves13InitResourcesEv.cpp`, never read.

## Three shapes that repeat across these leaves

Worth naming once here rather than thirteen times below.

- **The identity matrix.** `*(Matrix4x3*)&unk_NNN = IDENTITY_MATRIX4X3` in
  `InitResources`, over a slot whose pad runs 0x30 bytes. That is a `Matrix4x3`
  member and it is named `mMatrix`. It stays spelt `u8` + pad rather than given
  the real type, because typing it would drag `math/Matrix.h` into every
  includer -- a change with its own risk and no byte to gain.
- **The life timer.** A byte or halfword seeded with a round frame count in
  `InitResources`, run through `DecIfAbove0_Byte`/`DecIfAbove0_Short` once a
  frame, and destroying the actor at 0. Where `Render` additionally skips
  drawing on odd values below some threshold, the actor blinks before it goes --
  `PowerFlower` and `WingFeather` both do exactly that at 0xb4 down to 0x2d.
- **The particle handle.** `mParticle = Particle::System::New(mParticle, effect,
  pos...)` -- last frame's handle goes back in as the first argument and the
  result is stored again, so one effect is kept alive rather than a new one
  spawned each frame.

## SpikeBomb -- include/SpikeBomb.h

| offset | new name | evidence |
| --- | --- | --- |
| 0x180 | `mHomeHorzDist` | `Vec3_HorzLen` of the spawn position -- how far out in XZ the bomb starts from the world origin. |
| 0x184 | `mHomeYOffset` | the constant 0x2ee000; `mHomePosY` is raised by `mHomeYOffset >> 3` immediately after the home triple is copied. |

Source: `src/_ZN9SpikeBomb13InitResourcesEv.cpp`. Both branches of the header
carry the rename.

## daObjHeart_c -- include/daObjHeart_c.h

| offset | new name | evidence |
| --- | --- | --- |
| 0x16c | `mAnimSpeed` | seeded 0x1000 (1.0), eased toward 0x8000 while the heal cooldown is above 0x2d and back toward 0x1000 otherwise, then copied into `mModelAnim.speed`. The heart spins slowly while idle and fast just after it heals. |
| 0x171 | `mWasTouched` | edge detector on the collider: latched to 1 (and `mHealTimer` zeroed, so the heal fires that frame) the first frame the `dCcAc_c` reports an occupant, cleared the frame it reports none. |

Sources: `src/actors/d_a_obj_heart.cpp`, the single translation unit the
class's per-function sources were promoted into.

## WingFeather -- include/WingFeather.h

| offset | new name | evidence |
| --- | --- | --- |
| 0x378 | `mDriftSpeed` | eased toward 0x10000 every frame; multiplied by a `data_02082214` sine entry to produce `mHorzSpeed`. |
| 0x37c | `mSwayAngle` | advances 0x400 per airborne frame, eased back to 0 on the ground; `(mSwayAngle >> 4) * 2` indexes the same sine table for `mAngleX` and `mAngleZ`. |
| 0x380 | `mParticle` | the particle-handle shape above, effect 0x4a. |
| 0x384 | `mLifeTimer` | 0xb4 at init, counted down only while on the ground, destroys at 0, blinks below 0x2d. |

Sources: `src/game/actors/WingFeather/_ZN11WingFeather13InitResourcesEv.cpp`,
`src/game/actors/WingFeather/_ZN11WingFeather8BehaviorEv.cpp`,
`src/game/actors/WingFeather/_ZN11WingFeather6RenderEv.cpp`,
`src_tu/actors/WingFeather.cpp`.

## daObjAbuku_c -- include/daObjAbuku_c.h

The bubble runs the same float as `WingFeather`, one class earlier in the
overlay.

| offset | new name | evidence |
| --- | --- | --- |
| 0x108 | `mDriftSpeed` | eased toward 0x6000; times a `data_02082214` sine entry is `mHorzSpeed`. |
| 0x10c | `mSwayAngle` | `+= 0x400` a frame, `(mSwayAngle >> 4) * 2 + 1` indexes the sine table. |
| 0x10e | `mLifeTimer` | 0x12c (300 frames) at init, `DecIfAbove0_Short`, pops at 0. |
| 0x110 | `mParticle` | particle-handle shape, effect 1. |

Source: `src/actors/daObjAbuku_c.cpp`.

## daEyBm_c -- include/daEyBm_c.h

| offset | new name | evidence |
| --- | --- | --- |
| 0x2f8 | `mMatrix` | identity-matrix shape; this one was already TYPED `Matrix4x3`, only the name was missing. |
| 0x328 | `mParticle1` | particle-handle shape, effect 0x46, re-issued every `Render`. |
| 0x32c | `mParticle2` | same, effect 0x47. |
| 0x330 | `mLifeTimer` | 0x96 (150 frames) at init, `DecIfAbove0_Short` in `Behavior`. |

Source: `src/actors/daEyBm_c.cpp`.

## daKpFr_c -- include/daKpFr_c.h

| offset | new name | evidence |
| --- | --- | --- |
| 0x2ec | `mMatrix` | identity-matrix shape. |
| 0x324 | `mParticle1` | particle-handle shape, effect 0x7f, emitted at `mPosY + 0x4b000`. |
| 0x328 | `mParticle2` | same, effect 0x80. |

Source: `src/actors/daKpFr_c.cpp`.

## daKrpa_c -- include/daKrpa_c.h

| offset | new name | evidence |
| --- | --- | --- |
| 0x35c | `mMatrix` | identity-matrix shape. |
| 0x3a8 | `mHeightAboveGnd` | `InitResources` raycasts a `dBgCh_Gnd` down from `mPos` and stores `(mPosY - hit height) + 0x1e000`, or the constant 0x1f4000 when nothing is hit. |

Source: `src/actors/daKrpa_c.cpp`.

## Spiny -- include/Spiny.h

| offset | new name | evidence |
| --- | --- | --- |
| 0x3a0 | `mMatrix` | identity-matrix shape. |
| 0x3d8 | `mState` | `Render` draws the still `Model` in states 0 and 4 and the `ModelAnim` otherwise; `Behavior` treats 1 (once on the ground), 4 and 5 as states that must keep running whatever the distance to the player. |
| 0x3e9 | `mDespawnTimer` | 0x2c (44 frames) at init, counted down ONLY on the frames Spiny is too far from the player to behave, destroys at 0. Not a plain life timer -- being near the player refills nothing but stops the count. |

Sources: `src/_ZN5Spiny13InitResourcesEv.cpp`, `src/_ZN5Spiny6RenderEv.cpp`,
`src/_ZN5Spiny8BehaviorEv.cpp`, `src_tu/actors/Spiny.cpp`.

## EnemySwitchTag -- include/EnemySwitchTag.h

The tag sets an `Event` bit while something stands in its collider.

| offset | new name | evidence |
| --- | --- | --- |
| 0x108 | `mHoldDuration` | the spawn rotation Z word, or 0x96 (150 frames) when that is not positive. |
| 0x10a | `mHoldTimer` | counts `mHoldDuration` down; at 0 it clears the collider flag and `Event::ClearBit(mEventID)`. |
| 0x10c | `mIsReusable` | bit 5 of `param1`. Set: the tag re-arms by reloading `mHoldTimer` from `mHoldDuration`. Clear: it marks itself for destruction after firing once. |

Sources: `src/_ZN14EnemySwitchTag13InitResourcesEv.cpp`,
`src/_ZN14EnemySwitchTag8BehaviorEv.cpp`,
`src_tu/actors/EnemySpawner+EnemySwitchTag.cpp`.

## daDossyCap_c -- include/daDossyCap_c.h

| offset | new name | evidence |
| --- | --- | --- |
| 0x174 | `mCarrier` | the actor the cap is riding, a pointer spelt `s32`. `Behavior` returns immediately when it is null; otherwise it copies the cap position out of that actor 0x0d8 triple and its two angles out of that actor +0xe4 and `dActor_c::mAngleY`. |

Source: `src/_ZN12daDossyCap_c8BehaviorEv.cpp`.

## Scuttlebug -- include/Scuttlebug.h

| offset | new name | evidence |
| --- | --- | --- |
| 0x3a8 | `mTimer` | counted down once per frame at the top of `Behavior` with `DecIfAbove0_Short` -- as a HALFWORD, though the generated header typed the slot `u8`. The result is discarded there, so the name says "a timer" and no more. |

Source: `src/_ZN10Scuttlebug8BehaviorEv.cpp`.

## BowserTail -- include/BowserTail.h

| offset | new name | evidence |
| --- | --- | --- |
| 0x108 | `mBowserUniqueID` | `Behavior` resolves it with `dActor_c::FindWithID` and parks the tail 0x8c units out from Bowser's position along his facing angle. The file already said so in prose. |

Source: `src/_ZN10BowserTail8BehaviorEv.cpp`.

## More leaves searched, nothing named

- `BobOmbBuddy` 0x198: zeroed in `src/_ZN11BobOmbBuddy13InitResourcesEv.cpp`,
  never read.
- `daCamTag_c` 0x0d0 and `daBgSnwmn_c` 0x0d0: four opaque bytes each, touched by
  no enrolled body. `daBgSnwmn_c` already carries a note saying its 0x0cc read
  is the inherited `mAreaId`, not a field of its own.
- `daSoundObj_c` 0x0e0: filled from [data_ov002_0210c08a](../config/arm9/overlays/ov002/symbols.txt) + param1 * 0xc`, the
  same twelve-byte table row that fills `mLevelID`, `mTimerThreshold` and
  `mTimerReset` -- so it is the fourth column of the sound table, but nothing
  enrolled reads it, and which column is which is not something the table
  itself says. [`src/actors/d_a_sound_obj.cpp`]
