# System-class field provenance

Evidence tables for the classes that are *not* actors: the particle engine's
tracker, the minimap, the level scene, and the BMD model file. An actor is
documented by its own `InitResources` / `Behavior` / `Render`; these four are
driven from call sites scattered across the whole tree, so each row below cites
the file that proves it rather than a single owning source.

Every name here is byte-neutral: it renames a field, it does not change a type
that the compiler can see a width for. Where the evidence proves only a *kind*
and not a *role*, the field keeps its offset in the name (`mCallback_7d4`) so
the name does not overclaim.

---

## Particle::SysTracker — `include/Particle.h`, and the copy inside `include/Stage.h`

The struct is spelled `Particle` in `include/Particle.h` for historical
reasons; the ROM's mangled names call it `Particle::SysTracker`. There is one
instance and its address lives in the global `data_0209ee74`:
`_ZN8Particle10SysTrackerC1Ev` writes it, `_ZN8Particle10SysTrackerD1Ev`
clears it, and every free function in the namespace reaches the object through
it. That is what makes this a system class: the evidence is spread over eleven
files, none of which is a method of the class.

### The head of the object

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x000 | `mResourceFile` | `_ZN8Particle10SysTracker10InitialiseEv` stores either `data_02075f14` (the file, used in place) or a heap buffer it fills with `DecompressLZ16`, then hands it to the manager via `func_0204a17c`. `_ZN8Particle10SysTrackerD1Ev` frees it through `Memory::Deallocate` *only* when it is not `data_02075f14` — i.e. only when the tracker owns the decompressed copy. Was `pad_000[0x4]`. |
| 0x004 | `mManager` | The `Particle::Manager`. `Initialise` allocates it with `func_0204a4c8(...)` and pokes `0x8000` into its `+0x30`; `_ZN8Particle9RenderAllEv` and `_ZN8Particle10SysTracker6UpdateEv` pass it straight to the render/update entry points; `_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_` and `src/func_02021d1c.cpp` call `Particle::Manager::AddSystem` on it. Was `unk_004` typed `s32`, now `void *`. |
| 0x008 | `mContents` | A real nested class, not a guess: `_ZN8Particle6System12FromUniqueIDEj` calls `_ZNK8Particle10SysTracker8Contents8FindDataEj(this + 8, uniqueID)` — the mangled callee names `Particle::SysTracker::Contents` and puts it at `+8`. The constructor and destructor run `func_02021c90` / `func_02021b98` on the same address, and `SysTracker::Update` runs `func_02021bec`. Was `unk_008`. |

### The callback bank, 0x750 upward

`Particle::System::New(uniqueID, effectID, x, y, z, dir, callback)` is called
throughout the tree in one shape:

```c
*(u32*)(base + N) = System::New(*(u32*)(base + N), effect, x, y, z, dir, base + N + 4);
```

so slot `N` is a *last system unique ID* and `N + 4` is a `Particle::Callback`
subobject. Three call sites give both halves a role:

| ID slot | Callback | Effect | Proving file |
| --- | --- | --- | --- |
| 0x750 `mRunningSlidingDustSystemID` | 0x754 `mRunningSlidingDustCallback` | 0xda | `src/_ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_.c` |
| 0x768 `mBigSplashSystemID` | 0x76c `mBigSplashCallback` | 0xdd | `src/_ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_.c` |
| 0x78c `mRippleSystemID` | 0x790 `mRippleCallback` | 0x109 | `src/_ZN8Particle6System9NewRippleE5Fix12IiES2_S2_.c` |

Two more callbacks are named without an ID slot, because their `New*` wrapper
takes the unique ID from the caller instead:

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x810 | `mWeatherCallback` | `_ZN8Particle6System10NewWeatherEjj5Fix12IiES2_S2_PK11Vector3_16fj` builds `data_0209ee74 + 0x810` and passes it as the callback. |
| 0x814 | `mWeatherCallbackCount` | The same function writes its `numWeatherEffectsNow` argument to `callback + 4`, which is this byte; the constructor seeds it `1`. It is a member of the 0x810 object, not an independent field. |
| 0x818 | `mCallback_818` | `_ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f` passes it. Nothing names the effect, so the field does not claim one. |

The remaining slots are named only as far as the evidence goes. Two structural
facts carry them:

* **The constructor zeroes exactly ten `s32`s in the tail** — 0x750, 0x75c,
  0x768, 0x774, 0x780, 0x78c, 0x798, 0x7a4, 0x7b0, 0x7c0 — and no others. That
  is precisely the set of words sitting four bytes below a callback subobject,
  and it includes all three ID slots proven above. Those ten are the
  `mSystemID_*` fields. `0x7fc` and `0x804` are *not* in that set: the
  constructor seeds them `0x3000` and (finally) `0x4b000`, Fix12 3.0 and 75.0,
  so they are parameters of the callbacks at 0x7f8 and 0x800, and they are
  named `mCallbackParam_*`.
* **Every callback subobject is constructed by storing the base
  `dPa_c::level_c::callback_c` vtable at 0x0208f3b4 and then overwriting it with a
  derived vtable**, which is exactly what the real
  `dPa_c::level_c::simpleCallback_c` C1/C2 pair does. That is the "this is a Callback"
  evidence for the `mCallback_*` fields. The derived vtables also pair objects
  up: 0x76c and 0x778 both take `data_0208f3e4`, and 0x7f8 and 0x800 both take
  `data_0208f454`, so each pair is two instances of one callback class — which
  is why 0x7fc and 0x804 (their `+4` parameters) exist in the same shape.

Constructed by helper, so the class is known but not the role:
`_ZN5dPa_c7level_c16simpleCallback_cC1Ev` builds the complete objects at
0x754, 0x760, 0x79c, 0x7a8; its C2 sibling at 0x020226a4 builds the base
subobjects at 0x76c, 0x778, 0x784, 0x790, 0x808; the RTTI-backed
`dPa_c::level_c::scaleCallback_c` C1 builds 0x7b4, 0x7c4, 0x7d4, 0x7e4.

Outside `SysTracker`, ROM RTTI and the vtable slots identify the constructor at
0x02022298 as `dPa_c::level_c::edStarKiraCallback_c` C1. Its 64 tracking
records begin at 0x8: compiling byte-aligned storage instead places them in the
base class's 0x6 tail padding, so the ROM's address proves the original record
type had at least four-byte alignment. The record initializer remains unnamed
until separate evidence supports its historical type name.

### Shadow structs collapsed onto the header

`data_0209ee74` was declared six different ways across the tree — `char*`,
`void*`, and three private one-off structs (`ParticleSysTracker` twice, with
different fields, and `SomeGlobal`). All but one now declare it
`struct Particle *` and reach members by name; every one was re-verified
byte-identical. The exception is
`src/_ZN8Particle14SimpleCallback14SpawnParticlesERNS_6SystemE.cpp`, which
opens `namespace Particle { ... }` — including `Particle.h` there makes the
struct name and the namespace name collide, so it keeps its local shadow.

The old hand-written constructor was typed `struct Particle *self`
and reached the callback's own `s16` as `&self->unk_004`. That was never a
`SysTracker`: ROM RTTI names it `dPa_c::level_c::simpleCallback_c`, and the two
objects only agreed because both start with a pointer-sized word. Its two ROM
constructor variants are now generated from the real nested C++ class.

### The third shadow — collapsed onto the real names

`include/Particle__SysTracker.h` is a *third* declaration of this class, and it
is what `src/_ZN8Particle10SysTracker10InitialiseEv.cpp` and
`src/_ZN8Particle10SysTracker6UpdateEv.cpp` include. A later pass that owned the
file finished it: its three fields now read `mResourceFile` / `mManager` /
`mContents`, with the same types `include/Particle.h` carries, so all three
declarations of the class spell the head identically.

Only the head is declared there, and that is deliberate — the two including
functions touch 0x000, 0x004 and 0x008 and nothing else, so a 0x747-byte tail
would be a claim neither of them backs.

The same change collapsed the raw pokes in both bodies. `Initialise` reached
the manager eleven times as `*(char**)((char*)&self->unk_004)` and the resource
file seven times as `*(char**)((char*)self)`; both are now plain
`self->mManager` / `self->mResourceFile`, and `Update`'s two accesses likewise.
Both functions still reproduce byte-exact under 2004/b56 (checked per function
with `build_pin`'s `verify`, from `tools/build_pin.py`).

What is still NOT merged is the three *declarations* into one. `include/Stage.h`
must keep its own copy because `Stage` embeds the object by value and needs the
declared-never-defined destructor, and
`src/_ZN8Particle14SimpleCallback14SpawnParticlesERNS_6SystemE.cpp` opens
`namespace Particle { ... }`, so a header declaring a struct of that name
collides there. Those two reasons are unchanged by this pass; only the names
converged. `include/Stage.h`'s own header comment still describes the third
shadow as spelling `unk_`, which is now out of date — that file was frozen for
the pass that made this change.

### Deliberately left alone

* Seven of the ten `mSystemID_*` slots and eleven of the `mCallback_*`
  subobjects keep their offset in the name. The evidence proves what they
  *are*; nothing surviving in the tree says which effect each one drives.

---

## Minimap — `include/Minimap.h`

`Minimap` derives from `dBase_c` and there is one of it. Its own six methods
are the evidence here, but the class is still a system class in the sense that
matters: the fields it computes are consumed by hardware registers and by
`OAM::Render`, not by anything that looks like actor behaviour, and its two
biggest readers carried private shadows of the class rather than including the
header.

### Two 2x2 affine matrices

Both blocks are four consecutive `s32` filled in the same shape — `A` and `B`
from the sine/cosine table `data_02082214` indexed by `angle >> 4` and scaled,
then `C = -B` and `D = A`. That is a rotate-and-scale matrix, and
`OAM::Render`'s overload taking a `Matrix2x2 *` (`_ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2`)
names the type in its own mangled signature.

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x050-0x05c | `mBgMatrixA` … `mBgMatrixD` | `Minimap::Behavior` fills them from `data_02082214` scaled by `mInvScale`, sets `C = -B` and `D = A`, and passes `&mBgMatrixA` to `UpdateMinimap` as a four-word block. `Minimap::InitResources` seeds 0x1000, 0, 0, 0x1000 — the Fix12 identity. 0x05c was called `mPosX`; `mPosX = mBgMatrixA` in `Behavior` is what makes that name untenable. |
| 0x200-0x20c | `mArrowMatrixA` … `mArrowMatrixD` | `Minimap::Render` fills them from the same table scaled by `mArrowScale` (0x210), in both the VS and single-player branches, immediately before drawing `OAM::MM_ARROW`. Seeded to the same Fix12 identity. |

### Where the map is centred

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x1e0-0x1e8 | `mMapOriginX/Y/Z` | A per-level constant world position. `InitResources` writes 0x258000/0/0x64000 for one level group, -0x2bc000/0/-0x2bc000 for another and zero otherwise; nothing else assigns it. |
| 0x1f4-0x1fc | `mMapCenterWorldX/Y/Z` | The centre actually used this frame. `Behavior` copies `mMapOrigin*` into it, projects the player through `Minimap::GetPosOnMinimap`, clamps the icon to the visible window, unprojects with `Minimap::GetPosFromMinimapPos` and adds the difference back — so the map scrolls only as far as it must to keep the player on screen. Every icon on the map is projected relative to it, and `mMapCenterX/Y` are recomputed from its x and z. |

### Two fields the header did not reach

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x22e | `mStarIconAnimFrame[12]` | Was `pad_22e[0xc]`. `Minimap::Render` advances entry `i` by `data_0208ee44`, wraps it at 12, and uses `% 12 > 4` to choose between the two frames of a star marker; it resets the entry to 0 for any star not on the current map. |
| 0x256 | `mStarKeyBlinkTimer` | Was past the end of the declared struct entirely. `Render` increments it, wraps it at 5, and draws the first of the two `OAM::MM_STAR_KEYS` frames while it is below 2. The size assert is unchanged: 0x257 still rounds to 0x258. |

### Shadow structs collapsed onto the header

`Minimap::Render` carried a 33-field private redeclaration of the class and
`Minimap::UpdateLevelSpecific` a one-method stub; both now include
`include/Minimap.h`, which needed `int Render();` and
`static void UpdateLevelSpecific();` added to it. `build/eligible-names.txt` is
byte-identical across that header change.

Collapsing `Render` onto the real header cost one thing: the shadow spelled
0x21c `s16` and the header spells it `u16` (`Behavior` casts every read to
`(u16)`, which is why). `Render` reads it *without* a cast, so the ROM does an
`ldrsh` there and the two call sites now say `(s16)this->mAngle`. Without that
the function is two words off — the only two words in this whole pass that a
rename could have silently changed.

Two raw pokes also collapsed onto members and stayed byte-identical:
`*(u8*)((int)((char*)this + i) + 0x22e) += …` is `mStarIconAnimFrame[i] += …`,
and `*(u8*)((int)this + 0x256) += 1` is `mStarKeyBlinkTimer += 1`.

### Deliberately left `unk_`

0x090, 0x094, 0x098, 0x09c and 0x1ec are each written once, to zero, by
`InitResources`, and no matched function reads any of them. 0x090 sits between
the player-icon arrays and the star-icon arrays and 0x1ec between the two
Vector3s, so a shape can be guessed for both — but a guess is what it would be.

---

## BMD_File — `include/BMD_File.h`

A file-format header, so the evidence is a different shape: nothing has methods
or state, and a field is named by what a loader *reads at a fixed offset and
then does with the result*. Two behaviours carry almost everything here.

**A count that bounds a loop.** `Model::UpdateFileOffsets` walks the header
with `for (i = 0; i < file.X; i++)` over `file.Y + i`, which is what pins each
count/array pair — `numBones`/`bones`, `numTextures`/`textures`,
`numPalettes`/`palettes`, `numMaterials`/`materials`, and now
`numDisplayListGroups`/`displayListGroups`. It also fixes the stride, because
`file.Y + i` is pointer arithmetic on a struct the ROM's own indexing sizes.

**An offset added to a base pointer.** Every field the same function rewrites
as `field += (int)&file` is a file-relative offset, i.e. a pointer, and every
field it leaves alone is not. That is the entire "is a pointer" evidence in
this header, including for the slots still named `unk_*`.

### The display-list chain, 0x0c/0x10

`UpdateFileOffsets` walks it three levels deep, and the shape falls straight
out of the walk: a count at 0x0c, a table at 0x10 of 8-byte records
`{count, pointer}`, and under each of those a run of 0x10-byte records whose
`+0x04` and `+0x0c` are both rebased. Those are now two real types,
`BMD_DisplayListGroup` and `BMD_DisplayList`, and the function reaches them by
name instead of with `*(int *)(m + 4)`-style pokes. Byte-identical.

The host port supplies the role: `port/ntr/bmd.cpp` reads `+0x08` of the
0x10-byte record as a byte size and hands the bytes at `+0x0c` straight to the
geometry engine, which is why they are `size` and `data`. `+0x00` and `+0x04`
stay `unk_`.

### Three names that are not from matched ARM

`BMD_Material::name` / `textureId` / `paletteId`, and the matching
`BMD_Texture::name` and `BMD_Palette::name`, are *empirical*, and the
distinction is worth keeping straight. No matched function in the tree reads
any of them — the BMD parse itself is still `func_02046564` / `func_020462d0`.
What matched code proves is only that the word at `+0x00` of each record is a
file-relative pointer, because `UpdateFileOffsets` rebases it.

The role comes from `port/ntr/bmd.cpp`, which parses these offsets and renders
Mario correctly out of them: it reads `+0x00` as a name string and gets
`mat_bm_body` / `mat_bm_eye` / `mat_bm_head` / `mat_bm_head_c`, and indexes
`textures` with `+0x04` and `palettes` with `+0x08` to get `mario_body` /
`mario_eye_1` / `mario_head`. Rendering the right pixels out of the real
cartridge data is good evidence, but it is a different kind of evidence from
reading the ROM's instructions, and a later reader should know which one they
have. `textureId` and `paletteId` were flat `pad_04[0x20]` before.

`flags` at 0x24 stays as it is. `Model::HideMaterial` ORs bit 31 in and
`ShowMaterial` clears it, so the field is at least partly a flag word; the port
reads the same word as the DS specular/emission material colour. Both can be
true of one register-shaped word, and neither reading is complete, so the
conservative name stands.

### Deliberately left `unk_`

| Field | What is actually known |
| --- | --- |
| `BMD_File::unk_2c` | Rebased unconditionally. A pointer, to nothing named. |
| `BMD_File::unk_30` | Guards the block below: `if (file.unk_30 == 0) return;`. A count or a flag; the code never distinguishes. |
| `BMD_File::unk_34` | Rebased, and then its first four words are each rebased. A block of exactly four pointers — nothing says to what. |
| `BMD_Bone::unk_00`, `unk_3c` | Never rebased and never read by a matched function. Not pointers; that is all. |
| `BMD_Bone::unk_04`, `unk_38` | Rebased, so pointers. Only `materialIds` at 0x34 has a reader (`Model::HideMaterial` / `ShowMaterial`). |
| `BMD_Texture::unk_0c` | Not rebased, not read. |
| `BMD_DisplayList::unk_00`, `unk_04` | `unk_04` is rebased, so a pointer; `unk_00` is not. |

---

## Stage — `include/Stage.h`

`Stage` is the playable level scene, and it had no `unk_` fields of its own:
the thirty this pass was pointed at all belonged to the `Particle::SysTracker`
embedded at 0x50, which is the same class as `include/Particle.h` and is
covered by the table at the top of this file. The two copies are kept identical
by hand. (`include/Particle__SysTracker.h` is a third copy; it spelled `unk_`
until a later pass that owned it collapsed its three fields onto these same
names -- see "The third shadow" above.)

What was left here was *padding* rather than `unk_`, and two runs of it turned
out to be real structure.

### 0x8bc — eight texture-animation slots

Was `pad_8bc[0x60]`, and four functions were walking it by hand with a 0xc
stride. It is an array of eight `StageTexAnimSlot`, one per entry of the
level's texture-animation table (`data_0209f340`):

| Offset | Name | Evidence |
| --- | --- | --- |
| 0x00 | `mTransformer` | `Stage::LoadTextureTransformers` news a 0x14-byte `TextureTransformer`, constructs it, stores it here and calls `SetFile` on it; `Stage::RenderModel` passes it to `TextureTransformer::Update`; `Stage::CleanupResources` destroys it through its vtable. |
| 0x04 | `mActive` | Gates the slot in three places: `Stage::Render` only advances the animation when it is set, and `Stage::RenderModel` and `Stage::RenderModelTransparent` both use it to decide whether that part's materials get the transparent bit or lose it. |
| 0x08 | `mBlockList` | `Stage::CleanupResources` walks it as a singly linked list, freeing each block and following the next pointer at the block's `+0x0c` — which is inside the *block*, not the slot; the slot's own stride is 0xc. |

Three private redeclarations of this record existed —
`struct Slot` in `Stage::RenderModel`, `struct AnimSlot` in `Stage::Render`,
and raw `char *` arithmetic in `Stage::CleanupResources`,
`Stage::RenderModelTransparent` and `Stage::LoadTextureTransformers`. All but
`Stage::Render`'s (which carries its own full shadow of `Stage` and is a larger
change) now use the header type. Every one re-verified byte-identical.

### 0x9c4 — the two-phase load latch

Was inside `pad_9c0[0x8]`, reached as `*(s32*)((char*)thiz + 0x9c4)`.
`Stage::InitResources` runs its entire first block only while it is zero, sets
it to 1 at the end of that block when `data_0209fc68` says a wait is needed,
and then — on this call and every later one — returns -1 while
`func_020308a8()` reports the load unfinished. That is a "call me again" latch,
so the field is `mWaitingForLoad`. `Stage::InitResources` itself carries a
local shadow of the class and still spells it as a raw poke; the header now
names the field for anything that includes it.

`skyboxModel` became `mSkyboxModel`, for the `m`-prefix convention the rest of
the class already follows.

### A side effect worth recording

`build/eligible-names.txt` gained one entry across this pass and lost none:
`_ZN5Model17UpdateFileOffsetsER8BMD_File` became eligible once the raw
`*(int *)(m + 4)` pokes in it were replaced by the `BMD_DisplayList` /
`BMD_DisplayListGroup` field accesses. Collapsing a shadow onto a real type is
not only a readability change; it can move a file across the eligibility gate.
