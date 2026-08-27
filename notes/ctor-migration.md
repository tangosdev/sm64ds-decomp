# Migrating constructors to real C++

**Written 2026-08-23**, branch `worktree-ctor-frontier`. Companion to
`dtor-migration.md`, which this mirrors. Before this week the tree recorded
**0 constructors migrated, ever** (`notes/dtor-variant-audit.md`,
`plan-cpp-language-mode.md` Phase 5); it now records **nine**, all verified
under 2004/b56 with the full ROM rebuilding 106/106 exact:
`plan-cpp-language-mode.md` Phase 5); the table below is the first nine, all
verified under 2004/b56 with the full ROM rebuilding 106/106 exact. The
waves that followed through 2026-08-24 are recorded in §8 — **twenty** now:
the dBgW family, the Animation-family leaves, Clipper, and the first
typed-subobject constructor (dBgCh_Actr).

| file | function | ROM |
|---|---|---|
| `src/_ZN9ModelBaseC2Ev.cpp` | `ModelBase::ModelBase()` | 0x02017150, 0x18 |
| `src/_ZN5ModelC1Ev.cpp` | `Model::Model()` | 0x02016d58, 0x50 |
| `src/_ZN11CommonModelC1Ev.cpp` | `CommonModel::CommonModel()` | 0x02016204, 0x50 |
| `src/_ZN11ShadowModelC1Ev.cpp` | `ShadowModel::ShadowModel()` | 0x02016068, 0x34 |
| `src/_ZN7PathPtrC1Ev.cpp` | `PathPtr::PathPtr()` | 0x0203ad74, 0x10 |
| `src/_ZN8dM3dGSphC1Ev.cpp` | `dM3dGSph::dM3dGSph()` | 0x0203ac60, 0x10 |
| `src/_ZN9dBgCh_LinC1Ev.cpp` | `dBgCh_Lin::dBgCh_Lin()` | 0x020377b0, 0x5c |
| `src/_ZN12dBgCh_SphCrrC1Ev.cpp` | `dBgCh_SphCrr::dBgCh_SphCrr()` | 0x02037d18, 0x6c |
| `src/_ZN9dBgCh_GndC1Ev.cpp` | `dBgCh_Gnd::dBgCh_Gnd()` | 0x02037570, 0x40 |

The seventh is the tree's **first multiple-inheritance constructor**: its
header declares `dBgCh_Lin : dBgCh, dBgPi, dM3dGLin` straight out of the
ROM's own `__vmi_class_type_info`, and the compiler synthesises everything
the hand-written form spelled by hand — both base steps, **both vptr
stores** (primary to `_ZTV9dBgCh_Lin`, secondary landing on the separately
named `VTable_dBgPi_dBgCh_LinThunk`), and the `mBoundSphere` member step.
What made that possible mechanically was an objisolate extension: a
constructor-only TU references the class vtable UNDEF (the key function is
the destructor, defined nowhere), and the secondary store arrives as UNDEF
`_ZTV9dBgCh_Lin` with **addend 24** — 8 preamble + 0x10 into the secondary
block. The UNDEF path used to refuse any addend but 8; ModelAnim's already-
enrolled externalise case (44 − 8 = +0x24 =
`VTable_Animation_ModelAnimThunk`) is the verification the original refusal
asked for, so the same subtraction is now accepted there, with a probe test
(`test_corrects_ctor_only_mi_secondary_vptr`) pinning the shape. Two local
helpers came out of the same compile — `_ZN7Vector3D1Ev` (Vector3's inline
empty destructor, emitted because the TU odr-uses it) and the trivial
`_ZN8dM3dGLinD2Ev` — both stripped like the C2 sibling.

The body itself needed one non-obvious spelling. The ROM zeroes `lineEnd`
through a load-store chain (`[z]=0`, `[y]=[z]`, `[x]=[y]`) instead of three
immediate stores, and plain constant propagation would have folded that —
except Vector3 carries a user-declared (empty) destructor, making it
non-POD, and across those member accesses mwcc refuses to forward the
constant. Writing the chain literally — `lineEnd.z = 0;
lineEnd.y = lineEnd.z; lineEnd.x = lineEnd.y; clsnDist = 0;` — reproduced
the schedule byte-for-byte, including the scheduler parking
`add r0,r4,#0x64` between the two vptr stores, on the first attempt.

The eighth (`dBgCh_SphCrr`, three polymorphic bases) added one more measured
rule, and it bit three times in one commit before the gates caught all three:
**an MI upcast spelled on a POINTER emits a null-checked adjustment; spelled
through a REFERENCE it is unconditional.** `(dM3dGSph *)self` for a base at
+0x38 compiles to `movs r0,self; addne r0,#0x38; ...` — the ABI must keep a
null derived pointer null — where the ROM has plain `add r0,#0x38`. The
byte-exact spellings are `&(dM3dGSph &)*self` (lvalue) or `&(dBgPi &)sphere`
(reference parameter): convert the object, then take the address. Probed
directly against 2004/b56 with a two-base struct; both forms emit the single
`add`. Two of the three sites kept their size (movs/addne reuse the slots),
which is exactly why eligible.py stayed green while rombuild's byte analysis
flagged the third — the per-word compare is the only gate that sees
condition codes. Also worth recording: `dBgW_Kc::DetectClsn(dBgCh_SphCrr&)`
and the KcMbg overload now reach interiors by real base names —
`sphere.centre` through dM3dGSph, `&(dBgPi &)sphere` for the dBgPi
sub-object — replacing the last flat `unk_010`/`unk_038`/`pos` spellings in
their C++ TUs.

The ninth (`dBgCh_Gnd`, two polymorphic bases) matched on the first compile —
`bl dBgChC2`, `bl dBgPiC2` at +0x10, both vptr stores (`_ZTV9dBgCh_Gnd` and
the newly named `VTable_dBgPi_dBgCh_GndThunk`; same pre-link symbol-path
mismatch as every MI ctor, resolved by the same objisolate rebinding), and
`unk_04c = 0x1f4000`. The ROM keeps no C2 sibling: nothing derives from it,
so this is the first landed ctor whose TU's C2 output has no ROM counterpart
at all. The slice's real content was §2's blast radius, now measured on a
class with ~55 consumers: declaring `dBgCh_Gnd();` flipped every TU holding
a typed local of the REAL class (five of them; the other fifty define their
own dumb shadow structs and never see it) from hand-managed lifecycle to
implicit synthesis. Two kept the moved declaration and are byte-exact as
synthesized (BowserFire, daObjPathLift_c — the latter already shaped right);
one tried the move and gave it back (Toad: the synthesized pair scheduled one
instruction differently at the tail); and two are truly interleaved
from the start. daTrs_c::Behavior has rc2 constructed only on some paths,
with gotos into the middle of its lifetime, so it keeps **named word arrays**
— `u32 rc1[sizeof(dBgCh_Gnd) / sizeof(u32)];` with every call site casting
`(dBgCh_Gnd *)rc1`, which costs exactly the sp-relative add the old POD local
spelled. The tempting alias form (`dBgCh_Gnd *const rc1 = (dBgCh_Gnd
*)&storage;`) is measured wrong: it perturbs register allocation and cost
Behavior +32 bytes. KnockDownPlank (+8) and Toad (+4) both showed that the
moved-declaration synthesis reproduces construction/destruction but not
always their exact scheduling; both restored hand C1/D1 calls over a `u32`
array. The old comment in
daObjPathLift_c that warned "adding constructors to its shared header would
silently add calls to every automatic consumer" described exactly this
hazard and is now obsolete by design.

The slice also produced the session's most expensive lesson, about the ROM
layout rather than C++ form: **overlay load addresses cascade**. Two small
growths (+32 Behavior, +8 Plank) inside one overlay shifted every later-loaded
overlay's base, and the byte analysis lit up **1,588 mismatching functions in
32 modules** — relocation and literal-pool bindings corrupting across dozens
of files that no one touched. The diagnosis path is worth recording because
the symptom lies: module-image byte comparison showed only ov063.bin grew,
and a word-level delta survey over 400 failures showed ±imm deltas pointing
at resolved retail targets — i.e. pure address shift, not codegen regressions.
A baseline worktree at the pre-slice commit rebuilding 106/106 exact proved
every failure belonged to the slice. Rule of thumb: when failures explode
across many modules at once, diff module SIZES first — one growing module
names the real offenders, and per-function spelunking is wasted motion.

The census this attacks (`tools/langmode_audit.py --by-class`, 2026-08-24):
**C1 32 (8 migrated), C2 14 (2 migrated), C3 2** — against 397 plain methods
and 65 D1s — though §5c
reclassifies the two "C3"s and at least two C1s (Camera settled; Minimap,
HUD pending shape-check) as
factories that no source form can express, so the true migratable backlog is
smaller than the raw census; §7 enumerates it symbol by symbol. D0 is out of
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
`IDENTITY_MATRIX4X3`. Under the structured spelling from `math/Matrix.h`
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

### 5a. MI hierarchies are declared flat, so their ctors cannot go real — dBgCh_Lin is no longer one of them

`dBgCh_SphCrr : dBgCh @0, dBgPi @16, dM3dGSph @56` (the RTTI record states
this outright) still declares `struct dBgCh_SphCrr {` with pad bytes where
its bases belong, and its constructor's vptr stores at +0/+0x10/+0x38 are
spelled by hand — though the base steps themselves are named real symbols
(`_ZN5dBgChC2Ev`, `_ZN5dBgPiC2Ev`, `_ZN8dM3dGSphC1Ev`), so what remains for
it is purely header work. **dBgCh_Lin already crossed this line** (2026-08-23):
its header declares all three bases (`dBgPi` promoted to a polymorphic C++
branch, `dM3dGLin` rewritten to its true two-Vector3 shape after the
generated header had modelled `start` as padding), and its constructor is
the seventh landed above — see the MI notes under the table for the
objisolate secondary-vptr extension that made it linkable. The remaining
collision-family ctors (dBgCh_Gnd, dBgCh_SphCrr, dBgW_Kc*) are now straight
§6 applications of the same recipe.
### 5a. MI hierarchies are declared flat, so their ctors cannot go real — none of them are any more

~~`dBgCh_SphCrr ... still declares struct with pad bytes where its bases
belong`~~ **crossed 2026-08-24** (§8): promoted to the real three-base
declaration and its constructor re-emitted empty-body. So were **dBgCh_Lin**
(2026-08-23: all three bases declared — `dBgPi` promoted to a polymorphic
C++ branch, `dM3dGLin` rewritten to its true two-Vector3 shape after the
generated header had modelled `start` as padding) and every member of the
**dBgW family** through KcMbgSclY. What still stands from this section is
the method — name the base constructors first, promote the headers, let
synthesis do the rest — and exactly one flat wall: dBgPi's own pair (§5f).

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

### 5f. dBgPi's own pair: a base subobject at +0x04 the flat header cannot spell

The probe behind §2 and §6 (build/probe_dbgpi1.cpp) measured mwcc's
constructor emission order for a polymorphic class with an out-of-line ctor:
**base-subobject calls first, then the derived vptr store, then member
constructors, then body statements.** That order is what makes §2 work, and
it also dates ROM bodies: any call BEFORE the vptr store is base work;
everything after it is members or body.

dBgPi's ROM pair (C1 0x0203816c / C2 0x0203819c) reads:

```text
bl func_02037f18(this + 0x04)
vptr = _ZTV5dBgPi (data_02099368)
bl func_020380c0(this)
```

func_02037f18 stores five words at +0x04..+0x14 (`0xfc0, 0xff, 0, 0, 0`) and
no vtable. An ABI probe settles the shape: dBgPi introduces the virtual
destructor and derives from the non-polymorphic dBgPc. With no dynamic primary
base, the compiler places dBgPi's own vptr at +0x00 and its dBgPc base at +0x04.
The generated C2 sequence therefore constructs dBgPc at +0x04 before storing
dBgPi's vptr. C1 now lives in `src/_ZN5dBgPiC1Ev.cpp` as real C++; the separately
enrolled C2 ABI variant remains in `src/_ZN5dBgPiC2Ev.c`.

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

The compiler's own ordering, measured once and relied on everywhere
(build/probe_dbgpi1.cpp): **base-subobject calls → derived vptr store →
member constructors → body statements.** A call that precedes the vptr
store is base work; anything after it is members or body. That is why §2
works, why an empty-body derived ctor reproduces its ROM twin exactly, and
how a ROM ctor is dated from its `bl` order alone.

Verify with the strict bar, then the whole-ROM gate:

```sh
python tools/build_pin.py …            # or dtor_diff.py while shaping
python tools/eligible.py               # isolation on, expect verdict None
python tools/rombuild.py -j16          # 106/106 exact or it does not land
```

Delete the sibling `.c` only when it defined the SAME symbol you now define;
keep it when it defines the sibling variant (§1).

## 7. Worklist order from here

The complete measured backlog (2026-08-24 census over `config/**/symbols.txt`:
every `_ZN…C[123]Ev` function symbol cross-checked against `src/`). Ten real
constructors are landed (nine above, plus `dCapEnemy_c::dCapEnemy_c`, which
arrived from main with #1614); everything below is still a hand-mangled shell,
asm transcription, or absent.

1. ~~**fBase_c / dActor_c chain**~~ DONE as a naming step (2026-08-23):
   `_ZN7fBase_cC1Ev` renamed C2 per §4b, 17 callers audited, ROM green.
2. ~~**dBgCh_Gnd**~~ DONE (ninth, above).
3. **Collision family, continued** — the natural next slice; each is one §6
   application once its header un-flattens:
   `dBgCh_C2Ev` 0x02035514 · `dBgCh_ActrC1Ev` 0x02037430 · `dBgPiC1/C2Ev`
   0x0203816c/9c · `dBgWC2Ev` 0x0203969c · `dBgW_KcC1Ev` 0x02039894 ·
   `dBgW_KcMbgC1Ev` 0x0203a494 · `dBgW_KcMbgSclYC1Ev` 0x0203ab8c. The Kc*
   trio inherits through dBgW/dBgCh like Gnd does; dBgPi's own pair becoming
   real is what lets every child TU spell base steps by name.
4. **Leaf singles, config module**: `ClipperC1Ev` 0x02015730 ·
   `MaterialChangerC1Ev` 0x02015850 · `TextureTransformerC1Ev` 0x02015950 ·
   `TextureSequenceC1Ev` 0x02015a50 · `AnimationC1/C2Ev` 0x02015cf8/18 ·
   `Particle14SimpleCallbackC2Ev` 0x02022680 ·
3. ~~**Collision family, continued**~~ mostly DONE (§8: dBgW C2, Kc, KcMbg,
   KcMbgSclY, Actr). Still open: `dBgCh_C2Ev` 0x02035514 (base step, small)
   and `dBgPiC1/C2Ev` 0x0203816c/9c — the pair §5f walls until its +0x04
   base is probed.
4. ~~**Leaf singles, config module**: Clipper, MaterialChanger,
   TextureTransformer, TextureSequence, Animation C1/C2~~ DONE (§8).
   Remaining: `Particle14SimpleCallbackC2Ev` 0x02022680 ·
   `Particle10SysTrackerC1Ev` 0x02023204 (0x1d0 — the biggest body on the
   list; expect member-ctor synthesis work). Shape-check before attempting:
   if the disassembly starts `operator new` → null-check, it is a factory
   (§5c/§5d) and no source form exists. `Camera` is settled that way;
   FaderWipe C1 0x02017480 has no source at all and needs the same check
   before anyone promotes its header.
5. **Anim/model family**: `ModelAnimC1/C2Ev` 0x02016958/98 ·
   `ModelAnim2C1Ev` 0x020163a0 · `BlendModelAnimC1Ev` 0x020166d4.
   (`Model::C2` stays hand-written until someone wants it real; nothing
   calls it as a complete object.)
6. ~~**dCc response family**~~ DONE (2026-08-24): `dCcPos_cC1Ev`,
   `dCcAc_cC1/C2Ev` and `dCcAcPos_cC1Ev` are real C++ -- empty bodies over
   declared base steps, `dCcAc_c`'s `owner(0)` in the init list. `dCc_cC2Ev`
   stays hand-written extern "C" (its vtable data symbol is spelt
   `data_0208e6ec`), and declaring `dCc_c()` in the header is what lets the
   derived ones emit the base step by name. `dBgPcC2Ev` landed the same day:
   byte-identical to its C1 sibling, same five stores.
7. **Hierarchy-rooted, hardest last**: `fBase_c9SceneNodeC1Ev` 0x0203b4c4
   (nested class, independent of the chain — try early if 3–6 stall);
   `fBase_cC2Ev` 0x02043dec is a 0x160 NONMATCHING asm transcription today —
   reproducing it from real C++ needs the `dBase_c` intermediate declared
   (§5b/§5e facts) and is the root of everything below it:
   `dActor_cC1/C2Ev` 0x020113c0/0x0201150c · `dEnemyBase_cC2Ev` 0x020aed98 ·
   ~~`dBgActor_cC2Ev` 0x020eea50~~ DONE (2026-08-24): real and empty-bodied
   once `dActor_c()` was DECLARED in include/dActor_c.h -- the declaration
   points at the hand-spelt extern "C" C1/C2 pair (not the key function, so
   no TU coins a vtable), and the derived constructor emits
   `bl _ZN8dActor_cC2Ev` instead of inlining the base. The remaining two are
   rich bodies that still need that treatment one level down.
8. **Overlays**: `PlayerC1Ev` 0x020e68f4 · `MinimapC1Ev` 0x020fb8bc ·
   `HUDC1Ev` 0x020fe154. Minimap and HUD looked like genuine receive-`this`
   ctors during the §4b caller sweep — shape-check, then §6.
9. **Settled, do not retry** (the §5c factory wall): `PlayerC3Ev`,
   `StageC3Ev`, `CameraC1Ev`.

## 8. Waves two through four — the dBgW family, the leaves, and the first typed-subobject ctor (2026-08-24)

Eleven more constructors landed after this note was written, taking the tree
from nine to twenty: **dBgW C2, dBgW_Kc C1, dBgW_KcMbg C1,
dBgW_KcMbgSclY C1** (the Kc trio inherits through dBgW/dBgCh exactly like
Gnd); the five config-module leaves **Animation C1/C2, MaterialChanger,
TextureTransformer, TextureSequence**; **Clipper**; and **dBgCh_Actr**,
the first constructor whose class holds typed sub-objects. Three lessons
from those waves that §1–7 do not already carry:

**Typed members are what make a derived ctor free.** dBgCh_Actr's ROM
constructor is four synthesized steps (`bl dBgChC2`, vptr store, `bl
dBgCh_SphCrrC1` at +0x20, `bl dBgCh_LinC1` at +0x134) and its source is now
literally `dBgCh_Actr::dBgCh_Actr() {}`. That only compiles once the header
declares the two members AS THEMSELVES (`dBgCh_SphCrr mSphereClsn;
dBgCh_Lin mRaycastLine;`) instead of flat byte blobs — an empty body
synthesizes base step, vptr store and every member construction in the ROM's
order (§6's measured sequence) whenever the member classes have declared
out-of-line ctors. The cost is not the .cpp; it is the blast radius: a dozen
already-matched consumers reached into those interiors by absolute offset
and had to be rewritten to named paths (`mSphereClsn.disp`, `.flags`,
`.unk_108`, `.unk_10c`) before anything matched again. Budget the consumers,
not the constructor.

**Size-pin both language branches before promoting a header.** The two
sub-object sizes were settled by embedding evidence, not standalone
footprints: `sizeof(dBgCh_SphCrr)` = **0x110** (KcMbg::DetectClsn gives its
local query an exact 0x110 stack slot; compiling against 0x110 vs 0x10c
differs by a word), and `sizeof(dBgCh_Lin)` = **0x84** (Actr embeds it at
0x134 with Actr's own next word at 0x1b8). Standalone stack-slot footprints
(0x78/0x7c for Lin locals) are UNRELIABLE — mwcc lifetime-shares stack slots
across unrelated locals, so a footprint measures the frame, not the type.
The asserts are spelled `typedef char X_size_must_be_0xNNN[sizeof(struct X)
== 0xNNN ? 1 : -1];` AFTER the `#endif /* __cplusplus */`, struct-tag style,
so one line pins BOTH branches — and each branch must independently have the
bytes to reach the pinned size, or the .c TUs break first.

**Local shadow structs collide with promoted headers.** Consumer TUs that
predate a promotion often declare dumb local shadows of OTHER classes
(`typedef struct { char pad[0x28]; } dBgPi;`-shaped things). Once the real
header enters the include chain these are redefinitions, and "fixing" them by
using the real type silently changes codegen — a real-typed local auto-invokes
the newly declared ctor/dtor. The working fix is RENAME, not retype:
`dBgPiLoc` / `dBgCh_LinLoc` keep the bytes identical while clearing the name.
Grep the consumer's own file for shadow definitions of every class your
promoted header pulls in BEFORE compiling.

**The strict-reloc gate needed one more MI extension.** After the Lin/SphCrr
promotions, per-file `match.py --strict-relocs` began reporting "bytes match
but 1 reloc destination(s) WRONG" on their constructors — including on
pristine HEAD sources, so not a regression but a gate blind spot exposed by
the third `_ZTV` block. `reloc_audit.object_reloc_dests` resolved every
relocation by SYMBOL NAME alone and ignored addends entirely; a synthesized
secondary-vptr store names the SAME `_ZTV<C>` symbol as the primary store and
distinguishes blocks only by RELA addend (8 primary, +0x10 per secondary).
The fix makes `_ZTV`-named data relocs addend-aware — destination =
sym + addend − 8 for raw objects, sym + addend post-objisolate — selected by
an explicit `vt_form="raw"|"isolated"` parameter threaded through
check_destinations/gate_wrong_dests, because the two forms carry different
addends and no local test can tell them apart. Branch relocs keep name-only
resolution (their −8 is PC bias, not addressing). Measured on all four shapes
in-tree: three-block SphCrr, two-block Lin, single-inheritance Clipper,
no-base Animation, plus ModelAnim2 D0 (the documented 44-addend case) — all
MATCH under strict relocs now, where the MI ones could never pass before.
