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
| 0x13c | `mKeyFile` | handed to `Model::LoadFile` and `Release()`d as a `SharedFilePtr`. Three sources in `InitResources`: `data_ov002_0211094c`, `func_02132894[mKeyModelIdx + 1]` for the keyed-door `param1` range, else `data_ov089_02132c50`. |
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
| 0x113 | `mDeathTimer` | counted down by `DecIfAbove0_Byte` at the top of `Behavior`; the frame it reaches 0 the coin runs `func_ov002_020f05f4` and marks itself for destruction. Zero means "not dying". |

Sources: `src/_ZN9daSCoin_c13InitResourcesEv.cpp`,
`src/_ZN9daSCoin_c8BehaviorEv.cpp`.

Deliberately left `unk_`: 0x10d (`param1 & 0xf`, written and never read);
0x112 (already documented as touched only by the class's unenrolled
`func_ov002_020f051c.c`).

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
