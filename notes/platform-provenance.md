# Platform family — provenance

Where the `dBgActor_c` family's headers came from, and the arguments that settled
each layout. This file exists so the headers themselves can read the way the
original authors would have written them: source keeps only comments a game
author would plausibly write, plus the short load-bearing warnings that a
maintainer must not delete. Everything longer lives here.

Companion notes: `notes/actor-vtables.md`, `notes/dtor-migration.md`,
`notes/rtti-reconciliation.md`. The family-wide vtable census (183 slots, 64
classes) is in the project memory note `platform-family-census`.

---

## `include/dBgActor_c.h`

`dBgActor_c` is the base of the level-object family — ArmedRotatingPlatform,
PyramidTop, daObjKm3_Kaitendai_c, ShutterBob, ShutterHmc, SlidingIce and ~130
others. Seeded by `tools/gen_header.py` from matched-function evidence, then
given its real base and real member types by hand.

### Sub-objects, each checked twice

Once by that class's own size assertion, once by closing exactly on the next
named field:

| sub-object | span | closes on |
|---|---|---|
| `dActor_c`   | 0x000 + 0x0d0 = 0x0d0 | `pad_0d0` |
| `Model`      | 0x0d4 + 0x050 = 0x124 | `mMeshCollider` |
| `dBgW_KcMbg` | 0x124 + 0x1c8 = 0x2ec | padding, then the clsn matrix |

`mModel` and `mMeshCollider` are named by the class's own destructor calling
`Model`'s D1 at +0x0d4 and `dBgW_KcMbg`'s at +0x124 — relocations the ROM build
checks.

**Four of the generated header's fields were the `Model`'s own insides** and are
gone: `unk_0f0` (0x0d4 + 0x1c) and `unk_114` / `unk_118` / `unk_11c`
(0x0d4 + 0x40 / +0x44 / +0x48). They had been declared as siblings of a
`u8 mModel` marker whose pad stopped short of the real object.

### `mClsnMat` at 0x2ec

The generated header's `unk_310` / `unk_314` / `unk_318` were this matrix's
translation row — `m[9]`, `m[10]`, `m[11]`, at +0x24/+0x28/+0x2c.
`UpdateClsnPosAndRot` copies `mModel.mat4x3` in here and then overwrites exactly
those three words with the actor's position, which is what a
position-and-rotation matrix update *is*. Naming them separately described the
same bytes twice.

### `sizeof` is 0x320, and it takes more than one derived class to see why

`BowserFireSeaArena` starts its own `Model` at 0x324, which alone reads like the
class ending there. But four classes derive from `dBgActor_c` **directly** — one
non-`dBgActor_c` vtable store each, so no intermediate — and each places a
4-byte-aligned class member at 0x320, impossible if this class occupied
0x320..0x323:

| header name | RTTI name | member at 0x320 |
|---|---|---|
| `PyramidTop`    | `daObjDlPyramid_c` | `Model` |
| `BasementWater` | `daObjC0Water_c`   | `TextureTransformer` |
| `TowerStep`     | `daObjBk_Lift_c`   | `ShadowModel` |
| `WallSign`      | `daObjKanban_c`    | `dCcAcPos_c` |

Each is read straight off that class's destructor, which destroys its own member
at 0x320 before storing `_ZTV10dBgActor_c` and running the base.

One layout satisfies all five: **data ends 0x31e, `sizeof` 0x320.** The four
above align up from 0x31e to 0x320; `BowserFireSeaArena` puts its own three `s16`
at 0x31e/0x320/0x322 so its `Model` lands at 0x324; `ArmedRotatingPlatform` puts
a single `s16` at 0x31e, in this class's tail padding, and its `Behavior` reads
`this+0x31e` and reproduces. Ending at 0x324 satisfies `BowserFireSeaArena` and
contradicts the other four.

### `#include "common.h"` must come first — LOAD-BEARING

`Matrix4x3` has two guarded spellings that share the same 0x30 bytes: `common.h`'s
flat `s32 m[12]`, and `math/Matrix.h`'s `{ Matrix3x3 r; Vector3 t; }`. Whichever a
translation unit sees first stands. `Model.h` pulls in the second one, so without
the explicit `common.h` line ahead of it `dBgActor_c`'s TUs would get `.r`/`.t`.

The ROM says it was the flat one here: `UpdateClsnPosAndRot` copies
`mModel.mat4x3` into `mClsnMat` as three `ldm`/`stm` pairs of four registers —
twelve flat words. The `{r, t}` spelling copies the two members separately, 9
words then 3, and the function comes out 0x74 against the ROM's 0x64. Both were
built.

### `virtual ~dBgActor_c() {}` is inline on purpose — LOAD-BEARING

Every subclass's destructor inlines this body rather than calling
`_ZN10dBgActor_cD1Ev` (which does exist, at ov002 0x020ee42c, for the times it is
called out of line). An out-of-line declaration here would make each subclass
emit a `bl` the ROM does not have. Being inline also keeps `dBgActor_c` without a
key function, so no translation unit that merely includes this header emits
`_ZTV10dBgActor_c`.

### `virtual void Kill();` — slot 31, the only new virtual this class adds

`dActor_c` ends at slot 30, so this one word is the whole difference between the
vtable this header emitted and the one in the cartridge.

`_ZTV10dBgActor_c` is 0x84 at ov002:0x0210ae38 — 33 words, one more than the 32 a
`dActor_c`-shaped table needs — and `_ZTV8PoleLift`, one of the 70 subclasses, is
0x84 as well. `rtti_vtables` agrees from the other side: `dActor_c` has 31 slots
and `dBgActor_c`'s own overrides are 16 (D1), 17 (D0) and 31, and 97 of its 101
RTTI children have exactly 32 slots. Without this declaration every one of those
tables came out a word short, which is why `daObjFallBlock_c::Kill` and its
siblings had to reach slot 31 through a hand-declared 32-slot shadow struct.

`Kill` is not an override — no ancestor declares it, so `virtual` here **creates**
the slot. It also makes `Kill` this class's key function (the destructor above is
inline on purpose), so the TU defining it emits `_ZTV10dBgActor_c`,
`_ZTI10dBgActor_c` and the destructor variants alongside. That is expected and
handled: `objisolate.py` reduces the object to the one function its delink entry
declares before `eligible.py` and `rombuild.py` judge it.

See also the plain-English write-up in the project memory note
`platform-slot31-tldr` (PRs #1555 / #1558).

### `UpdateKillByMegaChar`'s by-value `Fix12<int>` is measured — LOAD-BEARING

The cartridge's RTTI carries class names only, so every parameter type in a
mangled symbol is this project's reconstruction until some function's bytes test
it. This one is tested: the body at ov002 0x020ee674 reproduces with a
single-int-by-value fourth parameter, which is exactly what `math/Fix12.h`'s
`{ T val; }` is.

Contrast the two `IsClsnInRange*` symbols, whose names make the same `Fix12<int>`
claim and whose bytes **refuse** it — see the note in `include/dActor_c.h` about
CW homing class-typed by-value parameters to the stack, and the header comment in
`src_tu/actors/Platform.cpp`. `UpdateKillByMegaChar` is non-virtual, so the
declaration adds no slot and no field; the 0x320 assertion is unaffected.

---

## Why so many headers carry a flat `#else` half

Several of these classes still have a compiler-generated D0 destructor living in a
C translation unit, which reads the fields by offset. Those TUs cannot be
migrated, so the header keeps a flat C spelling of the same object behind
`#else`. Same arrangement as `include/ShadowModel.h`.

## The standard derived-class argument

For a class whose destructor stores its own vptr and then `dBgActor_c`'s
(inlined), and destroys the `dBgW_KcMbg` at 0x124 and the `Model` at 0xd4 before
chaining to `dActor_c`: all three of those are `dBgActor_c`'s own. Everything such
a header used to restate below 0x31e was `dActor_c`'s and `dBgActor_c`'s, and is
inherited. This applies to `SlidingPlatformWf`, `FloatOnLavaPlatform`,
`ArmedRotatingPlatform`, `RotatingUpDownPlatformUtm` and their siblings.

Where a size assertion is only the observed field span rounded up, it guards the
declaration and is **not** independent evidence about the ROM. Where a factory's
`operator new` literal is available, that is the real size and it wins — see
`SlidingPlatformWf` below.

---

## `include/SlidingPlatformWf.h`

The field span stops short of the real size. `SlidingPlatformBdw_Spawn` and
`SlidingPlatformBfsRectangle_Spawn` call `fBase_c::operator new(0x330)`, read off
the retail instruction. **A span is only a lower bound**, so the header carries
explicit tail padding from 0x324 to 0x330.

Field roles, from `InitResources` (ov091) and `Behavior`:

| offset | name | evidence |
|---|---|---|
| 0x31e | `mPauseTimer` | `DecIfAbove0_Byte`; set to 0xf at each turnaround, and movement only runs once it reaches 0 |
| 0x324 | `mBasePosX` | `InitResources` copies the actor's position into 0x324/0x328/0x32c, which is also what closes the header on the factories' 0x330 |
| 0x328 | `mBasePosY` | as above |
| 0x32c | `mBasePosZ` | as above |
| 0x320 | `mMoveTimer`  | `DecIfAbove0_Short`; reloaded from `data_ov091_02134504[mVariant]` and the yaw flips by 0x8000 when it expires |
| 0x322 | `mVariant`    | set 0..6 by a switch on `actorID`; indexes the model, collider and CLPS tables (stride 0xc) and the two `data_ov091_021345xx` tables |

## `include/RotatingUpDownPlatform.h`

Still a flat generated struct. Own fields start at 0x320.

| offset | name | evidence |
|---|---|---|
| 0x320 | `mState` | `Behavior` dispatches `data_ov091_021354e0[mState]` as a pointer-to-member; `func_ov091_02132000` sets it to 1 |
| 0x324 | `mNodeCount` | `= PathPtr::NumNodes()` |
| 0x328 | `mNodeIndex` | `= 0`, passed to `PathPtr::GetNode(…, idx)`, incremented when the first node equals the start position |
| 0x32c | `mBasePosX` | `= mPosX` in `InitResources`; `Vec3_Equal(this+0x338, this+0x32c)` reads 0x32c as a `Vector3` |
| 0x330 | `mBasePosY` | `= mPosY` |
| 0x334 | `mBasePosZ` | `= mPosZ` |
| 0x338 | `mTargetPosX` | `PathPtr::GetNode` writes a `Vector3` over 0x338/0x33c/0x340; `func_ov091_02132000` reads all three back as one |
| 0x33c | `mTargetPosY` | as above |
| 0x340 | `mTargetPosZ` | as above |
| 0x344 | `mPathPtr` | `PathPtr::FromID(this+0x344, param & 0xf)` |
| 0x34c | `mSinkOffsetY` | `ApproachLinear(&this[0x34c], mIsPressed ? 0x1e000 : 0, 0x5000)`, then subtracted from `mPosY` |
| 0x350 | `mBaseAngleY` | `= mAngleY` in `InitResources` |
| 0x352 | `mVariant` | `= (param1 >> 8) & 0xff`; indexes the model / collider / CLPS tables |
| 0x354 | `mStateTimer` | incremented every `Behavior`, zeroed on a state change; `func_ov091_02132000` gates on `<= 0x14` |
| 0x356 | `mIsPressed` | cleared at the end of every `Behavior`; `func_ov091_02132360` sets it from a collision callback when the toucher's actorID is 0xbf; when set, `mSinkOffsetY` approaches 0x1e000 and `func_ov091_02132000` advances the state |

`mIsPressed` names the *observed role* (something is bearing on the platform), not
the identity of actor 0xbf, which is not settled here.

Left as `unk_`: nothing below 0x320 was touched — those are `fBase_c`'s and
`dActor_c`'s fields restated by `gen_header.py`, and belong to those headers.

`Behavior`'s `IsClsnInRange` declaration is deliberate — see the comment in the
source. Declared `int IsClsnInRange(int, int)` it mangles
`_ZN10dBgActor_c13IsClsnInRangeEii`, which nothing defines; the ROM's symbol takes
two `Fix12<int>`, and that type is an aggregate with no converting constructor
from `int`, so materialising a zero one costs stack traffic the ROM does not have.
The pair goes in registers exactly as two ints either way.

## `include/RotatingUpDownPlatformUtm.h`

Base and size from the factory (`src/RotatingUpDownPlatformUtm_Spawn.c`):
`fBase_c::operator new(936)` — 0x3a8 — then `dBgActor_c::dBgActor_c()`, then stores
`_ZTV25RotatingUpDownPlatformUtm`. No intermediate base: one non-base vtable
store, matching the D1 destructor.

`dBgActor_c` ends at 0x320. Members below that used to be restated here under
placeholder names by the auto-generated flat header; they are `dActor_c`'s and
`dBgActor_c`'s own storage now (`mSpawnParam` → `fBase_c::param1`, `mActorID` →
`fBase_c::actorID`, `unk_074` → `dActor_c::mCamSpacePosX`, `unk_0b0` →
`dActor_c::mFlags`), confirmed against `include/dActor_c.h` and
`include/fBase_c.h` and updated at every call site.

`mShadowModel` at 0x320 is named by the class's own destructor calling
`ShadowModel`'s D1 at +0x320 — a relocation the ROM build checks.

Field roles, from `InitResources`, `Behavior`, `Kill` and `Render` (ov091):

| offset | name | evidence |
|---|---|---|
| 0x378 | `mSoundHandle` | `= Sound::PlayLong(mSoundHandle, 3, 0x8d, …)` — the previous handle is fed straight back in |
| 0x37c | `mGroundY` | seeded with `mPosY - 0x14000`, then overwritten with `dBgCh_Gnd`'s result at +0x44 when `DetectClsn` hits |
| 0x380 | `mSpawnAngleX` | `= mAngleX` at the end of `InitResources` |
| 0x382 | `mSpawnAngleY` | `= mAngleY` |
| 0x384 | `mSpawnAngleZ` | `= mAngleZ` |
| 0x388 | `mBasePosX` | `= mPosX`; `Vec3_Add(…, (Vector3 *)&mBasePosX, …)` reads the triple as a `Vector3` |
| 0x38c | `mBasePosY` | `= mPosY` |
| 0x390 | `mBasePosZ` | `= mPosZ` |
| 0x394 | `mWaypointIndex` | `= param1 & 0xf`; indexes `data_ov091_02134cdc + mVariant*0x78` at stride 0xc, wraps at 10 |
| 0x395 | `mVariant` | 0 / 1 / 2 from `actorID == 0x1e` and `data_0209f2f8 == 7`; the 0x78 stride selector |
| 0x398 | `mPlatform0` | `dActor_c::FindWithActorID(0x1d, …)` neighbour within 0xa0000 |
| 0x39c | `mPlatform1` | the second such neighbour |
| 0x3a0 | `mIsDead` | set to 1 by `Kill`; `Behavior` and `Render` both return early when it is set, and neighbours test `*(u8 *)(other + 0x3a0)` |
| 0x3a2 | `mPitchBase` | `mAngleX = mPitchBase + ((mPitchStep * progress + 0x800) >> 12)`; `mPitchBase += mPitchStep` on reaching a waypoint. Reached in the source as `*(s16 *)(this + 0x300 + 0xa2)` |
| 0x3a4 | `mPitchStep` | set to 0x2000 while `mWaypointIndex` is 4..7, else 0; the multiplier in the same expression. Reached as `*(s16 *)(this + 0x300 + 0xa4)` |

`mPitchBase` / `mPitchStep` are declared `u8` + pad by the generated header but
every access is a 16-bit one; the widths were left alone because changing them is
a layout claim this pass did not need to make.

`Kill` is slot 31, `dBgActor_c`'s own new virtual, attributed by the vtable:
`config/arm9/overlays/ov091/relocs.txt` has 0x02134cd8 → 0x02131070, and
`_ZTV25RotatingUpDownPlatformUtm + 4*31 = 0x02134cd8`. It is **not** the key
function: the destructor is declared out of line and defined identically in both
`D1Ev.cpp` and `D0Ev.cpp`, so those two TUs keep emitting the vtable — checked
with `objisolate`, not assumed.

`OnHitByMegaChar` is slot 27, `dActor_c`'s combat callback, attributed the same
way: 0x02134cc8 → 0x021310fc.

In `Kill`, `Particle::System::NewSimple`'s signature is deliberately the local
`(unsigned, int, int, int)` one — the ROM name carries by-value class parameters
(`Fix12<int>`) which mwccarm passes differently at the call site, so declaring the
true types breaks the byte match. See `notes/mwccarm-codegen.md` 6az.

## `include/UpDownLiftBbh.h`

Still a flat generated struct (ov095). Own fields start at 0x320.

| offset | name | evidence |
|---|---|---|
| 0x320 | `mRider` | `func_ov095_02136764` stores the colliding actor here; `Behavior` passes it to `Player::IsInAir` and clears it when the rider leaves |
| 0x324 | `mClosestPlayer` | `= dActor_c::ClosestPlayer()` every frame |
| 0x328 | `mVariant` | 0 / 1 / 2 from `actorID` 0x20 / 0x21 / 0x83; indexes `data_ov095_02136f68` (model), `…f74` (collider) and `…021375a4` (CLPS) |
| 0x32c | `mState` | the index into the `data_ov095_02137910` pointer-to-member table `Behavior` dispatches |
| 0x330 | `mPlayerPosY` | `= mClosestPlayer->mPosY` (that actor's +0x60) |
| 0x334 | `mTopY` | `= mPosY`, or `mPosY + (spawn word << 12)` for the third variant |
| 0x338 | `mBottomY` | `= mTopY - (spawn word << 12)` |
| 0x33c | `mMiddleY` | `= (mTopY + mBottomY) / 2` |
| 0x340 | `mSoundHandle` | `= Sound::PlayLong(mSoundHandle, 3, 0x82, …)` in the state functions |
| 0x344 | `mStateTimer` | incremented every `Behavior`, zeroed on a state change |
| 0x347 | `mIsArmed` | 1 at init; `func_ov095_02136368` only starts the lift while it is 1 and clears it on trigger; `Behavior` re-arms it when the rider leaves |
| 0x348 | `mIsRidden` | set by the collider callback `func_ov095_02136764`, read once and cleared at the end of every `Behavior` |

Left as `unk_`, honestly:

- `unk_346` — a flag the state functions set and clear (`func_ov095_02136178`
  sets it, `…02136298` clears it, `…02136368` does both). No body in the tree
  shows what it means.
- `unk_349` — set to 0, and to 1 for the `actorID == 0x83` variant. Both
  `InitResources` and `func_ov095_02136298` then compare it against 2 and 0, and
  the `== 2` arm is unreachable from what the tree can see. It is a byte load in
  the ROM, not the `s32 mVariant` at 0x328 (those are different instructions), so
  it is genuinely its own field and its role is not settled. Do not "fix" the
  comparison — the bytes are the bytes.

Below 0x320 only three fields were named, and each from this class's own
bodies: `actorID` at 0x00c (switched on to pick the variant, and the same field
the sibling flat headers already call `actorID`), `mPosY` at 0x060 (copied to
`mTopY`) and `mAngleY` at 0x08e (handed to `dBgW_KcMbg::SetFile` as its yaw, and
sitting between the header's existing `mAngleX` and `mAngleZ`). Everything else
below 0x320 is `fBase_c`'s and `dActor_c`'s, restated by `gen_header.py`.

`mTopY` / `mBottomY` / `mMiddleY` / `mSoundHandle` were spelled `u8` plus three
bytes of padding by the generated header; every access in the ROM is 32-bit, so
they are `s32` / `u32` now. Same offsets, same size, byte-verified.

## `include/daObjSm_Lift_c.h` (SkiLift)

An RTTI-derived flat placeholder credited to a `deepen_rtti.py` that has
never existed in this repo (see notes/minigame-provenance.md), shared by
`SkiLift::InitResources` and `func_ov018_021122ec` (ov018).

| offset | name | evidence |
|---|---|---|
| 0x05c/0x060/0x064 | `mPosX` / `mPosY` / `mPosZ` | `dActor_c`'s position, copied to 0x324..0x32c |
| 0x08e | `mAngleY` | `-= 0x4000`, then handed to `dBgW_KcMbg::SetFile` as its yaw |
| 0x098 | `mHorzSpeed` | `dActor_c`'s; zeroed by `func_ov018_021122ec` |
| 0x324/0x328/0x32c | `mBasePosX` / `mBasePosY` / `mBasePosZ` | `= mPosX/mPosY/mPosZ` in `InitResources` |
| 0x334 | `mPathId` | `= param1 & 0xff`; `InitResources` bails when it is 0xff, otherwise passes it to `PathPtr::FromID` |
| 0x338 | `mNodeCount` | `= PathPtr::NumNodes()` |
| 0x33c | `mNodeIndex` | `+= mNodeStep` |
| 0x34c | `mNodeStep` | `= 1`; the increment applied to `mNodeIndex` |

Left as `unk_`: `unk_094`, `unk_130` (inside the `Model` at 0xd4, +0x5c — an
animation field, not this class's), `unk_320`, `unk_330`, `unk_331` (set to 0x3c,
so plausibly a one-second timer, but only ever written), `unk_350`, `unk_374`,
`unk_37c`.

---

## `include/TiltingPlatformLll.h` — retraction, kept for the trap

Lethal Lava Land's tilting slab. It does **not** derive from `dBgActor_c`; it
derives from `daObjGuragura_c`, which derives from `dBgActor_c`, and the
difference is in the bytes rather than only in the RTTI: its destructor stores
three vptrs — its own, `daObjGuragura_c`'s, then `dBgActor_c`'s. A one-level chain
emits two.

    _ZTI14daObjFl_Gura_c     ov064 0x0211bce8
    _ZTS14daObjFl_Gura_c     ov064 0x0211bcf4
    _ZTV18TiltingPlatformLll ov064 0x0211bd2c   (its record sits at V-4)
    kind  __si_class_type_info, ONE base, subobject offset 0
    base  daObjGuragura_c, ov002 0x0210905c

Size 0x350, from `TiltingPlatformLll_Spawn`'s literal 848, which
`daObjGuragura_c` fills. No fields of its own; it overrides slots 0 and 3, which
the base leaves null.

**The header was once built from the wrong factory.** It said `sizeof 0x368`,
"`MetalNetLift_Spawn` asks `fBase_c::operator new` for 872 bytes", and a
`PathPtr mPath` at 0x360 "named by `MetalNetLift_Spawn` calling `_ZN7PathPtrC1Ev`
at +0x360". `MetalNetLift` is a different class: its factory stores
`_ZTV12MetalNetLift`, ov064 0x0211bc68, and never mentions this one. This class's
factory is `TiltingPlatformLll_Spawn`, which allocates 848 = 0x350, stores
`_ZTV15daObjGuragura_c` and then `_ZTV18TiltingPlatformLll`, ov064 0x0211bd2c, and
constructs no `PathPtr`. The two vtables are 0xc4 apart in the same overlay, which
is presumably how they were crossed. Both relocation sets are in
`config/arm9/overlays/ov064/relocs.txt` and they do not overlap. Nothing consumed
the wrong fields: this class's two methods are cross-overlay veneers that pass
`this` and a table pointer and touch no member at all.

The general rule: **pair a class to its factory by vtable address, never by
filename.**

## `include/daObjKm3_Dorifu_c.h` — the crossed names

The Bowser-in-the-Sky drifting platform. ROM name `daObjKm3_Dorifu_c`. It does
not derive from `dBgActor_c`; it derives from `daObjDorifu_c`, which does. The
destructor stores three vptrs and destroys `daObjDorifu_c`'s `Model[5]` and
`dBgW_KcMbg[5]` in between, all of it from the base declaration.

    _ZTI17daObjKm3_Dorifu_c  ov047 0x021124cc
    _ZTS17daObjKm3_Dorifu_c  ov047 0x021124d8
    _ZTV17daObjKm3_Dorifu_c ov047 0x0211254c   (its record sits at V-4)
    kind  __si_class_type_info, ONE base, offset 0
    base  daObjDorifu_c, ov002 0x02108d70

**The tree's "Bs" names are crossed**, and the header does not try to fix it. The
factory that builds *this* class is `StairsBs_Spawn` (0xdcc, this class's
vtable), mirroring `StairsBdw_Spawn` in ov043. The function called
`daObjKm3_Dorifu_c_Spawn` builds something else — 800 = 0x320,
`daObjKuruma_c`'s vtable then ov047 0x0211244c, which is `daObjKm3_Kuruma_c`, a
class the tree has never named. Untangling the two names is a config change and
belongs in its own piece of work; the class modelled here is the one the vtable
symbol names.

No fields of its own: `StairsBs_Spawn` passes 0xdcc, which `daObjDorifu_c` fills.
It overrides slots 0 and 3, which the base leaves null.

## `include/SpinningPlatform.h`

`mClsnMat` at 0x2ec: `InitResources` hands `this+0x2ec` to `dBgW_KcMbg::SetFile`
as its `const Matrix4x3 &`. Was a `u8` marker plus its pad.

`mShadowMat` at 0x350: `ShadowModel` + `Matrix4x3` is the same pair
`HauntedChair` evidences by byte (a 48-byte identity block-copied over +0x14c,
which lands exactly on the next member), and 0x350 + 0x30 closes on the 0x380
`SpinningPlatform_Spawn` allocates.

## `include/ExtendingPlatform.h`

The `Model` marker's pad ran 0x30 bytes past the end of the object; that space is
not evidenced and stays explicit padding rather than being folded into the
member.

## `include/ArmedRotatingPlatform.h`

A donut lift: stands still until ridden, then shakes, falls and respawns.

`mAngVelY` sits at 0x31e, in `dBgActor_c`'s **tail padding**: `dBgActor_c`'s last
field ends at 0x31e and its size rounds to 0x320, and the Itanium ABI lets a
derived class place members in a non-POD base's tail padding. `Behavior` reads
`this+0x31e` and reproduces the ROM, which is what confirms the placement.

---

## Second pass: bodies

Four functions were re-spelled to use the new member names instead of raw offset
arithmetic. Each was checked with `build_pin.verify` against its delink range
before and after; all four still reproduce under 2004/b56, and the whole tree
still builds 106/106.

| function | module / range | what changed |
|---|---|---|
| `SlidingPlatformWf::InitResources` | ov091 0x021325d4 +0x214 | `*(u8 *)(c+0x322)` → `mVariant` throughout, `c+0x320` → `mMoveTimer`, `c+0x324..0x32c` → `mBasePos{X,Y,Z}` |
| `RotatingUpDownPlatform::Behavior` | ov091 0x02132108 +0x104 | `s+0x320` → `mState`, `s+0x354` → `mStateTimer`, `s+0x352` → `mVariant`, `s+0x356` → `mIsPressed`, `s+0x34c` → `mSinkOffsetY`; the two sink magic numbers become `cSinkDepth` / `cSinkRate` |
| `RotatingUpDownPlatform::InitResources` | ov091 0x0213220c +0x154 | `this+0x344` → `&mPathPtr`, `this+0x338` → `&mTargetPosX`, `this+0x32c` → `&mBasePosX` |
| `UpDownLiftBbh::InitResources` and `::Behavior` | ov095 0x021365d8 +0x18c, 0x021364d8 +0x100 | the `*((int *)((char *)&mTopY))` cast wrappers drop away now that the fields are `s32`; `this+0x344` and `(&unk_300)+0x44` both become `mStateTimer` |

One thing that did NOT hold: `*(int *)(s + 0x60) -= mSinkOffsetY;` in
`RotatingUpDownPlatform::Behavior` is followed by `mPosY = saved;`, so the
subtraction is dead as the tree spells it. That is what reproduces, and it was
left exactly as it is — do not "fix" it into something that reads better.

`build_pin.verify` reports a wrong `size` argument as `999 word(s) differ`, the
same string it uses for a real mismatch. Check the delink range before believing
a 999.
