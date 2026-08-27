# Scene / stage family — provenance

Derivation notes for the `dScene_c` branch of the class tree. These paragraphs used to
live as multi-paragraph block comments at the top of the headers and source files
themselves. They are evidence, not authoring — the ROM's own programmers did not write
them — so they were moved here when the family was made readable
(`cpp/readable-scene`). Nothing in this file is load-bearing for codegen; every comment
that *was* load-bearing stayed in source, condensed, with its warning intact.

Companion note: `notes/dscene-c-siblings-census.md`.

---

## dScene_c (`include/dScene_c.h`)

The scene root: `fBase_c -> dBase_c -> dScene_c`. Its own code lives at
`0x0202e140..0x0202ec9c`. (An earlier revision also claimed the four GraphCallback
defaults at `0x02018ea0..0x02018ec0`; those are `dGraph_c::callback_c`'s virtual slot
defaults, not `dScene_c`'s — see `dGraph_c.h` — and `dScene_c`'s own vtable holds none
of them.)

**Ten** classes derive from it directly, not the two an earlier draft claimed. From the
ROM's type graph (`tools/rtti_extract.py`), every record whose single base is
`dScene_c`, with its vtable:

| class | vtable | class | vtable |
| --- | --- | --- | --- |
| `dScBoot_c` | 0x02091528 | `dScGameOver_c` | 0x020b179c |
| `dScStage_c` | 0x020921c0 | `dScMgBase_c` | 0x020bc0c0 |
| `dScMB_c` | 0x020943c4 | `dScMiniGm_c` | 0x020c2490 |
| `dScTitle_c` | 0x020b1650 | `dScDSMT_c` | 0x021032e8 |
| `dScStarSel_c` | 0x020b1704 | `dScEntry_c` | 0x0211d304 |

Four already have headers under the ROM's own names (`dScBoot_c.h`, `dScMgBase_c.h`,
`dScMiniGm_c.h`, `dScEntry_c.h`). One more has one under a coinage, `dScStage_c` as
`include/Stage.h` — and `dScBoot_c` is also known by the coinage `BootScene`, from its
destructor symbols. The remaining five — `dScMB_c`, `dScTitle_c`, `dScStarSel_c`,
`dScGameOver_c`, `dScDSMT_c` — the tree does not describe at all. 4 + 1 + 5 = 10.
Naming a subset is fine; naming a subset as though it were the set is the mistake this
paragraph exists to stop.

The header this replaced described `dScene_c` as a flat 0x14-byte struct with one
field. That was a shadow of `fBase_c`'s first twenty bytes, not a class: it named no
base, so `dScene_c` and `fBase_c` were unrelated types, and the two translation units
that included it could only reach `this` as an `int *`.

### The chain — four readings

1. `_ZTV8dScene_c` (0x02092680) is 18 slots. **Eight** still point at `fBase_c`
   implementations — slots 0, 3, 6, 9, 12, 13, 14, 15 — and the other **ten** are
   `dScene_c`'s own: 1, 2, 4, 5, 7, 8, 10, 11, and the destructor pair at 16/17. (An
   earlier revision of this comment had those two numbers the wrong way round.)
2. Slot 2 points at `dScene_c`'s own `AfterInitResources`, whose body is a tail call to
   `_ZN7dBase_c18AfterInitResourcesEj` — `dBase_c`'s only *functional* override, though
   it also carries its own D1/D0 at 16/17. This one *suggests* the base and does not
   force it: an `extern "C"` call reproduces the same three words, which is exactly
   what the `.c` file this slice replaced did, and it byte-matched.
3. `dScene_c::~dScene_c` says it from the other side: it writes `_ZTV8dScene_c`, then
   `_ZTV7dBase_c` (0x0208e4b8), and only then calls `fBase_c::~fBase_c`. That is the
   vptr sequence of a three-deep chain with the middle destructor inlined.
4. The decisive one. `dScene_c`'s `__si_class_type_info` record is at 0x020914d4 under
   the ROM's real name for the class, `dScene_c`; its vtable field is 0x02092680 =
   `_ZTV8dScene_c`, and its single base pointer walks to `dBase_c` (vtable 0x0208e4b8)
   whose own base is `fBase_c` (vtable 0x02099edc). `si` means single inheritance:
   exactly one base, no ambiguity.

Nintendo EAD's own names for the three are therefore `fBase_c -> dBase_c -> dScene_c`.

### Slot order and the inline destructor

Slot order is `fBase_c`'s, unchanged — `dScene_c` adds no virtual of its own. It
overrides eight functionally, plus the destructor pair at 16/17.

`~dScene_c()` is **defined inline**, and per `include/dBase_c.h` the key-function worry
is moot — `objisolate` makes a key-function TU eligible regardless, by dropping the
vtable it emits and rebinding to the ROM's own `_ZTV`. What forced the inline move:
`Stage::~Stage` (and every one of `dScene_c`'s other nine direct children) *inlines*
`dScene_c`'s own D2 the same way `dScene_c` inlines `dBase_c`'s — the ROM's `Stage`
destructor stores `Stage`'s vptr, then `dScene_c`'s, then `dBase_c`'s, then calls
`fBase_c`'s D2 directly, with no call to a separate `dScene_c::~dScene_c()`. A merely
declared `virtual ~dScene_c();` cannot be inlined and emits `bl _ZN8dScene_cD2Ev` where
the ROM has none (measured on a `Stage` trial: 80 bytes with the call vs the ROM's 104
with none).

The cost is what `dBase_c` already pays: the D1 and D0 bodies can no longer *define*
`~dScene_c()`, and a bare include emits nothing, so a forcing call stands in. Both
variants now live in `src/actors/Scene.cpp`, the promoted translation unit that owns
the whole `arm9/Scene` range; see the note there.

### Layout — why dScene_c declares no fields

First, no `dScene_c` method reads or writes anything past `fBase_c`'s own members. The
only this-relative access in the entire class, across all 23 functions, is
`this->pauseFlags` (0x13) in `BeforeBehavior`; the destructors touch the vptr; everything else
either passes `this` straight through or never sees it.

Second — and this is the half that rules out a hidden field — `Stage` putting its first
own field at 0x050 is not on its own enough; that is consistent with a `dScene_c` field
at 0x50 that only `Stage`'s code happens to touch. What separates the two is that
`Stage` is not the only child: `dScBoot_c`'s generated header puts a **u16** at 0x050
where `Stage` has a `u8`. Two siblings disagreeing about the type at 0x050 means 0x050
is not inherited.

The size assertion in the header holds all three headers to that.

### Static vs non-static

Cannot be decided from a definition — both spellings mangle identically, and a method
that ignores `this` compiles the same either way. It is decided at the **call site**:

- For the ones that take arguments — `SetFaders`, `SetSceneToSpawn`, `StartSceneFade` —
  the call site puts the first declared argument in r0, leaving no register for a
  `this`. `SetAndStopColorFader` loads a `FaderColor` into r0 and `bl`s `SetFaders`;
  `Stage::Behavior` calls `SetSceneToSpawn` with `mov r0,#4; mov r1,#0`.
- For the ones that take none — `SetAndStopColorFader`, `SpawnIfNecessary`,
  `PrepareToSpawnBoot`, `Initialise3dGraphics`, `ResetHardwareRegisters` — nothing is
  placed in r0 at all; r0 still holds whatever the previous call left in it, and a
  member call would have had to overwrite it. `Initialise3dGraphics` reaches
  `ResetHardwareRegisters` with r0 carrying an unrelated return value.
- `ResetFadersAndSound` is the one that is **not** static: `BeforeInitResources` (itself
  a virtual, so r0 is a real `this`) calls it with a plain `bl` and r0 untouched, and
  the body consumes r0 — it stores it to 0x0209f5c0 and passes it on. Its mangled name
  takes no parameter, so a static could not receive that register.

This header has no C spelling and cannot be included from a C file, because `dBase_c.h`
has none either.

---

## Stage / dScStage_c (`include/Stage.h`)

The playable level: `fBase_c -> dBase_c -> dScene_c -> Stage`.

The generated header this replaced named no base and re-declared the whole of
`fBase_c` inline — `uniqueID` at 0x004, `actorID` at 0x00c, the three list nodes, a pad
to 0x050 — so `Stage` and `dScene_c` were unrelated types. Everything below 0x050 is
gone from the header now; it comes from the base chain, which owns it.

**Derivation.** `dScStage_c` at 0x02092158, vtable 0x020921c0 = `_ZTV5Stage`, single
base `dScene_c`. It is a leaf: no record in the image names `dScStage_c` as a base.

**Vtable.** `_ZTV5Stage` is 18 slots, the same shape `dScene_c` and `fBase_c` have, and
`Stage` adds no virtual of its own. It overrides six functionally — 0 `InitResources`,
1 `BeforeInitResources`, 3 `CleanupResources`, 6 `Behavior`, 9 `Render`,
12 `OnPendingDestroy` — plus the destructor pair at 16/17. The remaining ten still point
at `dScene_c`'s Before/After hooks or at `fBase_c`.

**Key function.** Slot 0 is `Stage::InitResources`, so declaration order matters in the
way `include/dActor_c.h` warns about: whichever non-inline virtual is declared first is
the key function, and CW 1.2 emits the vtable group into the TU that *defines* it —
colliding with the copy the module's gap object supplies from ROM data. The destructor
is declared first, which is free for a derived class (an override takes its base's slot
wherever it is written) and pins the role to `~Stage`. `tools/objisolate.py` makes that
TU eligible anyway, so `~Stage` is a real method, defined identically —
`Stage::~Stage() {}` — in both `src/_ZN5StageD1Ev.cpp` and `src/_ZN5StageD0Ev.cpp`.
Unlike `dScene_c`, `Stage` does not need to define it inline in the class body: `Stage`
is a leaf, so nothing derives from it that would need to inline *its* destructor in
turn.

**Size is deliberately not asserted for the observed-field span.** The last field the
header lists is the last one any matched function has been observed to touch, which is
not the same as the last field the object has. The trailing `pad_9c0[0x8]` and the
`0x9c8` assertion come from the ROM's own `new Stage` size literal
(`tools/opnew_sizes.py`), which *is* decisive. The offsets are checked:
`tools/check_header_offsets.py` walks them from `dScene_c`'s asserted 0x50.

**`Particle::SysTracker`, embedded at `Stage+0x50`.** Not its own header yet:
`include/Particle.h` and `include/Particle__SysTracker.h` are two *separate*
`gen_header.py` shadows of this same class. `src/_ZN8Particle10SysTrackerC1Ev.cpp` writes
fields through `struct Particle *self` up to `unk_818`, while
`src/_ZN8Particle10SysTracker10InitialiseEv.cpp` and `6UpdateEv.c` read `mManager`/`mContents`
through `struct Particle__SysTracker *self` — the same offsets `Particle.h` also
carries. Their union is what `Stage.h` declares locally: `Particle.h`'s full 34-field
layout, last field `unk_818` (1 byte, ends 0x819), padded to 0x81c for 4-byte
alignment — exactly the gap `Stage`'s own D1/D0 give this member (0x50..0x86c), a
second independent check on the same number. Declared locally rather than merging the
two real headers because neither `Stage` source file includes them and a merge has its
own blast radius across every file that already casts through one shadow or the other.

Its destructor is declared, never defined: `src/_ZN8Particle10SysTrackerD1Ev.cpp`
already supplies `_ZN8Particle10SysTrackerD1Ev` as an `extern "C"` free function; the
declaration only lets `Stage`'s implicit destructor find it by name. Not virtual —
`dtor_variant_audit.py` established `Particle::SysTracker` has no RTTI record and no
`_ZTV`, so it is not polymorphic and must not gain a vtable pointer.

**`mSkyboxModel` at 0x9bc** is typed, not guessed, and evidenced from both ends:
`LoadSkybox` `new`s 0x50 bytes, runs `Model::Model` on them and stores the result here;
`CleanupResources` loads it back and destroys it through its vtable. The generated
header called it `u8 unk_9bc`, so both files that use it had to cast a pointer out of a
byte.

**`GetSkyboxID`** is reached from `LoadSkybox` with r0 still holding `this` and no
argument set up, which is a member call. Its body reads the level record through a
global rather than through a field, so the bytes alone cannot confirm the `this`; the
call site is what decides it.

**`GraphCallback1`/`GraphCallback2`** are vtable slots 1/2 of
`dScStage_c::graphCallback_c` (RTTI si-child of `dGraph_c::callback_c`). Virtual and
nullary in the ROM: dispatchers call through the object's vptr with r0 = the callback
object and no other argument; `GraphCallback2` reads its fields (fixed-point matrix at
+0x4) through `this`. Members of `Stage` only as the family's legacy scope for
`dScStage_c`; non-static because slot 2 needs `this`, non-virtual to keep their TUs from
emitting a vtable the delink ranges do not own.

**`PS_Init` is deliberately not declared in the header**, even though
`src/_ZN5Stage7PS_InitEv.cpp` defines `Stage::PS_Init()` and is byte-verified and
enrolled. It uses its own local shadow `class Stage` (the same pattern
`src/_ZN5Stage8BehaviorEv.cpp` uses) on purpose: a real landmine lived at that exact
filename — an untracked, un-enrolled second `.cpp` for the symbol whose private
`struct G2x` declared `SetBlendBrightness`'s middle parameter as `int`, mangling to
`_ZN3G2x18SetBlendBrightnessEPVtis`, which resolves to nothing and would silently *not*
be what byte-matches. The surviving file never declares a `G2x` type at all — it calls
the correctly-mangled `_ZN3G2x18SetBlendBrightnessEPVtts` symbol directly, as the `.c`
file it replaced did — so the landmine cannot recur through it. Wiring `PS_Init` to the
header is a separate, low-risk change.

---

## BootScene / dScBoot_c (`include/BootScene.h`)

The boot/intro scene: `fBase_c -> dBase_c -> dScene_c -> BootScene`.

The generated header this replaced, `include/dScBoot_c.h`, named no base and
re-declared a 0x50-byte pad in place of the inherited chain. The class's *functions* are
attributed under the English coinage `BootScene`, because that is what its destructor
symbols (`_ZN9BootSceneD1Ev` / `_ZN9BootSceneD0Ev`) mangle to; this header follows
`include/Stage.h`'s precedent of naming after the coinage. `include/dScBoot_c.h` is left
in place for other includers; neither `BootScene` source needs it any more.

**Derivation.** `dScBoot_c` at 0x020914c8, vtable 0x02091528, single base `dScene_c`. A
leaf.

**Vtable.** `data_02091528` is 18 slots and `BootScene` overrides two functionally —
slot 0 `InitResources` (arm9 0x02005a58) and slot 6 `Behavior` (arm9 0x02005418) — plus
the destructor pair at 16/17. Confirmed against `config/arm9/relocs.txt`'s vtable words
at 0x02091528 and 0x02091540.

**Converted 2026-08-22.** Both overrides are now real `BootScene::` methods
(`src/_ZN9BootScene13InitResourcesEv.cpp`, `src/_ZN9BootScene8BehaviorEv.cpp`), both
byte-exact, and neither includes `include/dScBoot_c.h` any more. A previous revision of
the header said they were "NOT CONVERTED BY THIS PASS", which was later read as saying
they *could not* be. The key function is the first non-inline virtual *declared*, which
is `~BootScene`, defined out of line in `_ZN9BootSceneD1Ev.cpp`; where `InitResources`
is defined cannot move that. Bracketing `eligible.py` across the conversion returned a
byte-identical name list, which is the gate that would show a `_ZTV` appearing.

**Layout corrections over the generated header.** `dScBoot_c.h` put a `u16` at 0x050, a
`u8` at 0x052, an assumed pad at 0x053, then `u8`s at 0x054 and 0x055, ending the struct
at 0x056. Two corrections:

1. 0x053 is **not** padding. `Behavior` both reads and writes it as a real 0/1 value,
   and `func_02005348` — called from `Behavior` with the same `this` — compares it
   against a loop index. Two independent readers agreeing on a live value rules out
   coincidence.
2. There is a field at **0x056** the generated header never saw: `Behavior`'s case 4
   sets it to 0x78 and case 5 decrements and re-reads it — the same countdown shape
   0x054 and 0x055 have.

With 0x056 counted, the class's own fields run 0x050..0x056 — seven bytes, one short of
the 0x058 an eighth (padding) byte reaches. 0x058 is exactly what
`src/func_02023624.c`'s `operator new(0x58)` allocates. Since `dScBoot_c` is a leaf, no
subclass needs the object to extend further, so the assertion holds to it.

**`InitResources` in detail.** The very first thing the game draws. It hands the four
VRAM banks to BG/OBJ on both screens, brings up a text-mode main BG and three sub BGs,
decompresses the same 0x020918c4 tile set into both screens' character memory with the
0x020914e0 palette, starts the boot countdown at 0x3c frames, and kicks off a fade
through `func_0201a244`. The register writes stay as literal `volatile` stores to the
ARM7/9 I/O block: this tree has no register header, and the read-modify-write masks are
the ROM's own.

---

## dScEntry_c (`include/dScEntry_c.h`)

One of `dScene_c`'s ten direct children — the "entry" scene: whichever course/minigame
is currently loaded, keyed off the actor ID it was spawned with (`fBase_c::actorID`,
inherited at 0x0c).

**Base.** `dScene_c`, confirmed by the RTTI edge (`dScEntry_c` ov075:0x0211c8f8 ->
`dScene_c` arm9:0x020914d4) and independently by the vtable: reading
`data_ov075_0211d304` (18 slots) against `_ZTV8dScene_c` slot for slot, `dScEntry_c`
overrides exactly eight — 0, 1, 3, 6, 9, 12, 16, 17.

**Construction.** A real Spawn-style factory: `func_ov075_0211a740` (the complete-object
constructor) and `func_ov075_0211a854` (an identical base-object variant — the usual
C1/C2 pair) both open with `_ZN7fBase_cnwEj(0x288)`, i.e. `fBase_c::operator new(0x288)`.
Size 0x288 is read directly off the allocator call, not inferred from field span.

The same constructor writes the vtable chain, sets two spawn-flag bits at `fBase_c`'s
own 0x13, copies an 8-byte template struct into three slots 8 bytes apart at
0x54/0x5c/0x64, then calls a shared "construct N elements of size S" helper
(`func_020733a8`) twice:

- `func_020733a8(p+0x70,  9, 0x24, ...)` — 9 elements of 0x24 bytes, spanning 0x70..0x1b4
- `func_020733a8(p+0x1b4, 4, 0x2c, ...)` — 4 elements of 0x2c bytes, spanning 0x1b4..0x264

These line up with the two nested RTTI classes the ROM's type graph records under this
class — `dScEntry_c::icon_c` (base `dThIcon_c`) and `dScEntry_c::graphCallback_c` (base
`dGraph_c::callback_c`) — and the destructor (`func_ov075_02115ab8` / `_02115b28`) tears
down the same two ranges with a matching `__destroy_arr(ptr, count, stride, dtor)` before
calling `fBase_c`'s D2. Recovering those two nested classes' own layouts is a separate,
later pass; the header only holds the byte range they occupy, as opaque storage.

**Members below 0x50** are all inherited. The flat header this superseded had an
`unk_00c` that was actually `fBase_c::actorID` misread as this class's own, because a
flat struct has no base to attribute it to.

**Vtable order** follows `dScene_c`'s; `dScEntry_c` adds no new virtual. The destructor
pair is at slots 16/17, not 0/1 — the `fBase_c`/actor-family convention, not the Fader
family's.

Only `func_ov075_0211a410`/`InitResources` and `func_ov075_0211a2b8`/`Behavior` include
this header directly; everything else in the family uses its own local offset-based
struct, matching convention.

---

## dWipe_c (`include/dWipe_c.h`)

Hardware screen wipe: a `FaderColor` concrete implementation that drives the
capture/DMA-based wipe transition (CP15 cache flush + GX palette load + IRQ-driven
per-scanline capture), instead of a flat blend register (`FaderColor`'s own path, still
used when `type == 1`) or a 3D model (`FaderWipe`'s path — a different, unrelated class).

**Derivation.** The ROM's `__si_class_type_info` for `dWipe_c` names its single base
`dFdColor_c` (== `FaderColor`), confirmed independently by `dWipe_c::AdvanceFade`
(0x0202f428), which forwards to `_ZN10FaderColor11AdvanceFadeEv` when `type == 1`.

**Construction.** `dWipe_c` is not spawned; it is a single global static object,
`data_0209f61c`, placement-constructed by `__sinit_02074f80` (`func_0202fc40`) and
registered for atexit teardown via `func_020731dc(obj, D1, dso-handle)`. There is no
`operator new` call to read a size literal from.

**Size.** Two independent witnesses agree on 0x2c (44 bytes): the bss layout gap
(`data_0209f61c` to the next symbol `data_0209f648` is exactly 0x2c) and the field span
(the highest field written is a `u8` at 0x28, and 0x29 rounds up to 0x2c).

**Members.** `FaderColor`'s own dsize (vptr + currInterp + speed + color) is 0xe; its
`sizeof` of 0x10 leaves a 2-byte tail-padding gap at 0xe..0xf that the Itanium ABI lets
a derived class reuse, and `dWipe_c`'s constructor helper (`func_0202ed14`) does: it
writes single bytes at both 0xe and 0xf before touching anything past 0x10. Real,
newly-added storage starts at 0x10.

**Vtable.** `data_020926f0` (unnamed — like the rest of this family, only `FaderWipe`'s
table has a real `_ZTV` symbol in `config/arm9/symbols.txt`) is ten slots and overrides
*all* of them, even the ones `FaderColor` itself inherits unmodified from
`FaderBrightness`: every `dWipe_c` word differs from `FaderColor`'s own vtable at the
same slot. Two of the ten (`SetToEnd`, `SetToStart`) are pure tail-call veneers to
`FaderBrightness`'s implementation; the rest branch on `type == 1` to the same
`FaderBrightness`/`FaderColor` bodies and otherwise run the hardware-capture path.

**`SetBackwardTime` stays plain C — measured, do not "fix".** The full record:
`src/_ZN7dWipe_c15SetBackwardTimeEj.c` takes a third parameter that arrives in r2 and is
forwarded to the guard call with zero instructions, which keeps r2 live from entry to
the call and forces the cached `type` into r3 as in the ROM. As a real method the
mangled name fixes the arity at one (`Ej`), so the r2-holding parameter cannot exist,
and the miss is exactly three words: `ldr r3,[r4,#0x14]` / `cmp r3,#1` / `cmp r3,#0`
become r2. Measured 2026-08-22 over eleven spellings, all 3 words: declaration order
(both ways), separate vs combined decls, init-at-decl, `int` vs `s32` vs `u32`, a third
unused local, no `type` local at all, and reading `state` directly. `const s32 type`
does not compile (assigned later). The rest of the function is byte-identical in every
variant — only the colouring differs. Re-open only with a lever that makes r2 live at
*entry* without emitting an instruction; source spelling alone does not reach it.

---

## Stage's fog array (`include/Fog.h`, `Stage::LoadFog`)

`Stage.h` used to model 0x96c..0x9bc as one `Fog` (0x26 bytes) followed by five
loose `unk_` bytes at 0x994, 0x9b4, 0x9b5, 0x9b6 and 0x9b8. Those five are the
*second element of a two-element `Fog` array*, and `Stage::LoadFog` is what shows
it: its second loop starts at `this + 0x96c` and advances `dst += 0x28` once per
level fog record, handing each `dst` to `Fog::Init`. 0x994 = 0x96c + 0x28 is
`mFog[1]`'s density ramp; 0x9b4/0x9b5/0x9b6/0x9b8 are `mFog[1]`'s
`mEnabled`/`mShift`/`mOffset`/`mColor` at +0x20/0x21/0x22/0x24. The array ends at
0x9bc, exactly where `mSkyboxModel` begins.

So `Fog`'s size is 0x28, not the 0x26 its field span alone suggests — the stride
is the witness, and the old header's "independently, Stage.h places Fog at 0x96c
with its next real field at 0x994" was reading the next array element as a
neighbouring field. `Fog` is now `Fog mFog[2];` in `Stage.h`, and `LoadFog` reads
as two hand-written default ramps followed by the level's own records.

**Dead lever, recorded so it is not retried.** In `Fog::Init` the density ramp is
walked with a running `char *p`, not `self->mDensity[i]`. Swapping to the indexed
spelling (and dropping `p`) changes the function's size — `build_pin.verify`
returns `999 word(s) differ`, the size-mismatch signature. Reverted; the pointer
walk stays, with a short warning in the file.
