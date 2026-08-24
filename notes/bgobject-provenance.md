# `dBgActor_c`-derived interactive objects — field provenance

Every name below was read off a **matched** body: the citation names the file and the
thing the code actually does with the offset. Nothing here is inferred from a sibling
class, from a name in another decomp, or from what a field "ought" to be. Offsets that
survive as `unk_NNN` are listed too, with the reason the evidence was not enough.

Convention: instance members `mFoo`, statics `sFoo`, constants `cFoo`.

Renames are byte-neutral by construction. Every class in this file was re-verified with
`tools/build_pin.verify` against the ROM-pinned compiler (2004/b56) after the rename,
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
