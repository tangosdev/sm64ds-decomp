# Migrating constructors to real C++

**Written 2026-08-23**, branch `worktree-ctor-frontier`. Companion to
`dtor-migration.md`, which this mirrors. Before this week the tree recorded
**0 constructors migrated, ever** (`notes/dtor-variant-audit.md`,
`plan-cpp-language-mode.md` Phase 5); it now records four, all in the model
family, all verified strict (bytes + reloc destinations) under 2004/b56 with
the full ROM rebuilding 106/106 exact:

| file | function | ROM |
|---|---|---|
| `src/_ZN9ModelBaseC2Ev.cpp` | `ModelBase::ModelBase()` | 0x02017150, 0x18 |
| `src/_ZN5ModelC1Ev.cpp` | `Model::Model()` | 0x02016d58, 0x50 |
| `src/_ZN11CommonModelC1Ev.cpp` | `CommonModel::CommonModel()` | 0x02016204, 0x50 |
| `src/_ZN11ShadowModelC1Ev.cpp` | `ShadowModel::ShadowModel()` | 0x02016068, 0x34 |
| `src/_ZN7PathPtrC1Ev.cpp` | `PathPtr::PathPtr()` | 0x0203ad74, 0x10 |
| `src/_ZN8dM3dGSphC1Ev.cpp` | `dM3dGSph::dM3dGSph()` | 0x0203ac60, 0x10 |

The census this attacks (`tools/langmode_audit.py --by-class`): **C1 41,
C2 10, C3 2 unmigrated**, against 397 plain methods and 65 D1s — though §5c
reclassifies the two "C3"s and at least three C1s (Camera, Minimap, HUD) as
factories that no source form can express, so the true migratable backlog is
smaller than the raw census. D0 is out of
backlog forever; constructors were the last symbol kind with no playbook.

PathPtr also supplied the first live sighting of §2's blast radius: the
moment `PathPtr();` appeared in its header, every typed local of that class
in every TU grew an implicit construction call. `Shark::InitResources`
holds two such locals and constructs them BY HAND at interleaved ROM
positions — each double-constructed, Shark went four words long, and
eligible.py caught it before anything linked. Its locals are now raw `u32`
storage with a comment explaining why they must stay dumb. When you declare
a constructor on a widely-embedded class, grep for typed locals of it
first; the full-ROM gate is the backstop, not the first line of defence.

## 1. One definition emits both C1 and C2

An out-of-line `X::X() { ... }` makes mwccarm emit `_ZNXC1Ev` AND `_ZNXC2Ev`
as two separate global FUNC symbols in one object (plus, when a member has a
nontrivial type, a local helper such as `_ZN9Matrix4x3D1Ev`). This is the
constructor mirror of the destructor finding: the variants are not chosen by
the writer, they are emitted around the body.

Consequences, same as the D1/D0 arrangement:

- The enrolled file keeps ONE symbol (`_ZN5ModelC1Ev.cpp` owns C1);
  `objisolate` strips the sibling variant and any local helpers, exactly as
  it strips a real destructor's extra sections.
- The sibling variant keeps its existing hand-written `.c` file
  (`_ZN5ModelC2Ev.c` still provides C2). Do not delete it and do not write
  the constructor twice — that is an ODR violation across TUs.
- Eligibility rules hold unchanged. All four landed files pass
  `tools/eligible.py` with isolation on.

## 2. The base constructor must be DECLARED, never defined

The single highest-leverage header change. Left undeclared, the compiler
synthesises the base construction inline — the ROM's out-of-line call never
appears and the candidate lands tens of bytes short. Declared-but-not-defined
in the class body is what makes every derived constructor emit
`bl _ZN…C…Ev`. This is already how `Model()` was declared in
`include/Model.h`; this wave added the same declaration to `ModelBase`,
`CommonModel`, and `ShadowModel`.

Risk to check when adding one: every TU that value-constructs the class or a
derived class changes codegen. In this family every derived constructor was
still a hand-written `.c` TU that never sees the header, so nothing moved —
but the gate for the check is the full `rombuild -j16`, not per-file verify.

## 3. Struct-copy spelling decides the mat4x3 block move

`Model::Model` and `CommonModel::CommonModel` copy a 0x30-byte matrix from
`data_02082128`. Under the structured spelling from `math/Matrix.h`
(`Matrix4x3 { Matrix3x3 r; Vector3 t; }`) the copy splits into a subobject
copy of `r` plus field-by-field stores of `t`: **+28 bytes over the ROM**.
Including `common.h` FIRST flips the whole TU to the flat spelling
(`Matrix4x3 { s32 m[12]; }`, guarded by `MATRIX4X3_DEFINED`, whichever a TU
sees first stands) and reproduces the ROM's three `ldm/stm` quads exactly.

This is the array-member-struct lesson from `mwccarm-codegen.md` applied in
reverse: the ROM's original sources used the flat spelling here, and the
guard mechanism exists precisely so both spellings can coexist.

## 4. The base-object variant label: check which C the callers mean

`ModelBase`'s constructor was labelled `_ZN9ModelBaseC1Ev` since the initial
release, and every derived constructor called it by that name through
hand-spelled externs. It is a **C2**. The evidence, all four callers:

| caller | what it is |
|---|---|
| `_ZN11ShadowModelC1Ev` | derived ctor, base-subobject step |
| `_ZN11CommonModelC1Ev` | derived ctor, base-subobject step |
| `_ZN5ModelC1Ev` (+8) | derived ctor, base-subobject step |
| `_ZN5ModelC2Ev` (+8) | derived ctor, base-subobject step |

Under the Itanium ABI mwccarm names the base-subobject call **C2**
(measured: a real `Model::Model()` references `_ZN9ModelBaseC2Ev`), and
nothing anywhere constructs a standalone `ModelBase`, so no C1 caller exists.
The linker's dead stripping kept only the referenced variant, which is why
the image holds one function and why the sibling label was never
contradicted until something tried to migrate a derived constructor for
real — objisolate could not resolve `_ZN9ModelBaseC2Ev` against a table that
only knew C1.

The rename touched: `config/arm9/symbols.txt`, the file itself (git mv +
its own definition), `config/arm9/delinks.txt` (enrolment path), the three
caller `.c` files' hand-spelled externs, the header prose, and
`port/slice_gate4b.txt`. Expect the same list for any future variant-label
correction, and expect `layout_check` to catch a missed delinks rename.

Related rename, same PR family: the vtable object at 0x0208e87c carried the
data name `data_0208e87c`; naming it `_ZTV9ModelBase` is what lets
objisolate rebind the real constructor's vptr store (it already handles
`_ZTV5Model` et al). Its addend loses 8 on rebinding, per the standing
vtable-preamble rule.

### 4b. Same treatment applied to `_ZN7fBase_cC1Ev` — also a C2

The §4 procedure scales to the whole actor chain: `_ZN7fBase_cC1Ev`
(0x02043dec, 0x160) had **17 callers**, and every one constructs fBase_c as
a **base subobject** of a derived class:

| callers | what they are |
|---|---|
| `_ZN6CameraC1Ev`, `_ZN8dActor_cC1Ev`, `_ZN8dActor_cC2Ev` | derived ctors, base-subobject step |
| `_ZN3HUDC1Ev`, `_ZN7MinimapC1Ev` | derived ctors, base-subobject step |
| `_ZN5StageC3Ev` | allocating ctor folding a base-subobject step (§5c) |
| `StarSelect_Spawn`, `UnknownVsEntry_Spawn`, `func_02023624`, `func_020352b4` | unnamed C3s: `operator new` → null check → base step → double vptr store |
| 7 overlay `func_*` placeholders | same Spawn shape in ov003/004/005/007/075 |

Nothing anywhere constructs a standalone `fBase_c` — it is pure machinery
with no complete-object instantiation site — so the C1 label was never
contradicted until derived constructors were migrated for real. Renamed to
`_ZN7fBase_cC2Ev` across symbols.txt, the file itself (git mv), delinks,
17 caller files' externs, `include/fBase_c.h`, `tu_manifest.json`, and
`port/hal/actor_vtables.cpp`. Eligible 11064 → 11065; ROM 106/106 exact.

Status caveat preserved by the rename: the function itself is still
**NONMATCHING hand-written asm** — its file header says so and asks for
reproduction from real C++ (`fBase_c::fBase_c()`; inheritance chain and
callees are known). The rename changes only the label, but it is what lets
derived-constructor candidates reference the base step by its real variant
name instead of a placeholder extern.

## 5. What does NOT generalise yet — the measured walls

### 5a. MI hierarchies are declared flat, so their ctors cannot go real

`dBgCh_SphCrr : dBgCh @0, dBgPi @16, dM3dGSph @56` (the RTTI record states
this outright) still declares `struct dBgCh_SphCrr {` with pad bytes where
its bases belong, and its constructor's vptr stores at +0/+0x10/+0x38 are
spelled by hand — though the base steps themselves are now named real
symbols (`_ZN5dBgChC2Ev`, `_ZN5dBgPiC2Ev`, `_ZN8dM3dGSphC1Ev`, all migrated
or relabelled 2026-08-23), so what remains is purely the header work:
un-flatten `dBgCh_SphCrr` and `dBgCh_Lin` into real `: dBgCh, dBgPi`
declarations and their constructors become §6 applications. A real C++
ctor needs the compiler to synthesise those base steps from declarations,
and until the headers state them it cannot.

Progress within this item: **dM3dGSph is fully promoted** (polymorphic
header with declared dtor-first/declared ctor, size assert; its vtable
named `_ZTV8dM3dGSph` so objisolate rebinds the vptr store) and its
constructor is the sixth landed above. Its header promotion touched no
other TU — only `include/dBgCh_Lin.h` includes it, and nothing includes
that yet — which is why the §2 blast-radius question had a one-file answer
here. It will not be that quiet for dBgCh itself.

**The ordering this imposes: name the base constructors first** (the audit's
C2-vs-D2 discrimination method applies directly), declare them in real
hierarchy headers, then the derived ctors follow the model-family recipe.
This is the same dependency direction as everything else: the compiler only
generates what the declarations state.

### 5b. Rich bodies are fine, but only after 5a

`dActor_c`'s pair (0x148 each, the largest backlog class at 11 files) links
itself into a global list via `func_0203b244`, reads spawn data through
three globals, and calls its own `SetRanges`. None of that is a wall by
itself — bodies stay hand-written either way — but the synthesized part must
already match, and today the hand-written form stores TWO different vtables
to `+0` in sequence (`data_0208e4b8` then `data_0208e3a4`), which reads as
an undeclared intermediate base. Settle the fBase_c chain's declarations
first; note it also calls `_ZN7fBase_cC1Ev`, whose complete-vs-base variant
choice deserves the §4 treatment before anything builds on it.

### 5c. There are no allocating constructors in this ROM — the "C3"s are factories

Rewritten after the 2026-08-23 census. A TU that says `new X()` does NOT
emit `_ZNXC3Ev` — the allocation and construction fold straight into the
calling function (measured: `probe_spawn` came out 0x88 bytes holding
everything). The follow-up question was whether the ROM's two C3-labelled
functions were that fold kept out-of-line by size. They are not ctors at all.
Disassembled, all three allocating-labelled functions share one shape:

| label | prologue |
|---|---|
| `_ZN5StageC3Ev` 0x0202e088 | `ldr r0, [pc]` → `bl fBase_c::op new` → `movs r4,r0; beq ret` → construct |
| `_ZN6PlayerC3Ev` 0x020e6c0c | same, then `bl` Player's own ctor |
| `_ZN6CameraC1Ev` 0x0200e444 | same, construction fully inline |

Three tells, each individually fatal to the constructor reading:

* **The incoming `this` is ignored** — r0 is clobbered before use. No ABI
  variant discards its storage parameter.
* **The size is a hardcoded literal**, not a parameter. A true C3 receives
  the size from its caller.
* **Dispatch is indirect through data tables** (`Camera_SpawnInfo` holds
  `&CameraC1Ev` as an entry). You never call a constructor through a spawn
  table; you call a *maker*. These are plain factory functions into which
  `new X()` folded at -O4,p — the same fold §6 of the skill describes, just
  kept out-of-line because something took the function's address.

Corroboration already in-tree:
`config/name-roundtrip-baseline.json` records that a TU claiming
`_ZN6PlayerC3Ev` emitted `['_ZN6PlayerC1Ev', '_ZN6PlayerC2Ev']` — ask the
pinned compiler for a C3 and it hands you the ordinary pair, because **C3
has no source form under 2004/b56** and never did here. The labels were
project-assigned guesses about intent, and they guessed "constructor" for
what is really a spawn helper.

### 5d. The factory wall, measured four ways

Writing a factory as real C++ needs the compiler to synthesise a full
construction sequence into raw memory. Every route is rejected by the pinned
compiler (all four probed against 2004/b56):

```text
static void* operator new(u32);   -> illegal 'operator' declaration
void* operator new(u32);          -> illegal 'operator' declaration
void* operator new(u32, void*);   -> illegal 'operator' declaration (global placement)
new (raw) T();                    -> no matching operator new (no builtin placement)
p->T::T();                        -> illegal use of type-name
```

So a folded factory is **not writable as real C++ under this toolchain**,
period. `Camera_Spawn`, `Stage`'s and `Player`'s factories stay hand-written
`.c` files — like fBase_c's constructor, a permanent asm-or-C transcription,
with the reason recorded here so nobody retries the recipe hoping for a
different answer. What IS recoverable from them is knowledge, not code:

### 5e. What the Camera factory taught us about the hierarchy

RTTI names read straight off the vtables (typeinfo name strings):

| object | RTTI name | note |
|---|---|
| `data_0208e4b8` | `7dBase_c` | the unnamed intermediate base §5b predicted; vtable already carries the class's 18 slots |
| `_ZTV4View` | `7dView_c` | overrides slot 9 (Render) + D1/D0 only; adds no new virtuals |
| `_ZTV6Camera` | `9dCamera_c` | the project's `Camera` |

Chain: **`dCamera_c : dView_c : dBase_c : fBase_c`**. The factory's three
vptr stores walk it bottom-up (`dBase_c` store, `dView_c` store, then body +
final `dCamera_c` store), and `dActor_c`'s double store in §5b starts with
the same `data_0208e4b8` — everything sits on `dBase_c`, as the actor-vtables
note already said.

Layout consequence: `dView_c` owns the 48-byte matrix at +0x50 (constructed
with `Matrix4x3_LoadIdentity(&mat)` mid-fold), making it 0x80 tall — which is
exactly why auto-generated `Camera.h` shows `pad_000[0x80]` before `lookAt`:
those bytes are base subobjects, not padding. Reference-graph facts: only
`Camera` derives from `View` (five `_ZTV4View` references total: two inside
Camera's D1/D0 teardown, two inside dView_c's own dtor pair, one in the
factory), and `View` has no out-of-line constructor anywhere — its
construction exists only inside the folded factory, so its ctor must have
been defined inline in the original header.

None of this unblocks the factory itself (§5d), but it names two more links
of the hierarchy for every FUTURE constructor migration on the chain, and it
corrects the leaf-single plan below.

## 6. The recipe, condensed

```cpp
//cpp
// @symbol _ZNX<C>N…C<12>Ev…
/* recovery notes */
#include "common.h"        // FIRST, if the body copies a Matrix4x3 (§3)
#include "<Class>.h"       // real header; bases declare their ctors (§2)

extern Matrix4x3 data_……;  // data the body needs, canonical names

Class::Class() : member(0)      // init list = every store the ROM makes
{                                // beyond base+vptr, in ROM order
    mat4x3 = data_……;
}
```

Verify with the strict bar, then the whole-ROM gate:

```sh
python tools/build_pin.py …            # or dtor_diff.py while shaping
python tools/eligible.py               # isolation on, expect verdict None
python tools/rombuild.py -j16          # 106/106 exact or it does not land
```

Delete the sibling `.c` only when it defined the SAME symbol you now define;
keep it when it defines the sibling variant (§1).

## 7. Worklist order from here

1. ~~**fBase_c / dActor_c chain**~~ DONE (2026-08-23): `_ZN7fBase_cC1Ev`
   renamed C2 per §4b, 17 callers audited, ROM green. Remaining for the full
   chain: declare `dView_c` and correct `Camera`'s header to real
   inheritance (§5e facts), then dActor_c's pair becomes a §6 application —
   its own ctor is still nonmatching asm until someone reproduces it from
   real C++.
2. **dBgCh/dBgPi/dM3dG collision family**: base ctors are NAMED
   (`_ZN5dBgChC2Ev`, `_ZN5dBgPiC2Ev` C1+C2 pair, `_ZN8dM3dGSphC1Ev` — the
   sixth landed ctor) and dM3dGSph's header is promoted. Remaining:
   declare `dBgCh();`/`dBgPi();` in their headers, promote `dBgCh_Lin`
   (first MI test — objisolate's secondary-vptr rebinding is unproven),
   then dBgCh_Gnd / dBgCh_SphCrr / dBgW_Kc* follow.
3. **Leaf singles** (`Minimap`, `HUD`, `PathPtr`, `Clipper`,
   `TextureSequence`, …) — with one new gate learned the hard way:
   **shape-check before attempting**. Disassemble the candidate first; if it
   starts `operator new` → null-check, it is a factory (§5c/§5d), not a
   constructor, and no source form exists. Genuine receive-`this` ctors
   (Minimap and HUD looked like it during the §4b caller sweep) remain one
   §6 application each once their headers declare the ctor. `Camera` is
   settled: factory, blocked by §5d, hierarchy knowledge banked in §5e.
4. ModelAnim family dtor-style follow-ups (`Model::C2` stays hand-written
   until someone wants it real; nothing calls it as a complete object).
