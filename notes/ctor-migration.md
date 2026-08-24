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

The census this attacks (`tools/langmode_audit.py --by-class`): **C1 41,
C2 10, C3 2 unmigrated**, against 397 plain methods and 65 D1s. D0 is out of
backlog forever; constructors were the last symbol kind with no playbook.

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

## 5. What does NOT generalise yet — the measured walls

### 5a. MI hierarchies are declared flat, so their ctors cannot go real

`dBgCh_SphCrr : dBgCh @0, dBgPi @16, dM3dGSph @56` (the RTTI record states
this outright) still declares `struct dBgCh_SphCrr {` with pad bytes where
its bases belong, and its constructor's three vptr stores at +0/+0x10/+0x38
are spelled by hand against placeholder-named callees
(`func_02035514` is dBgCh's C2 per the dtor-variant audit's constructor
population). A real C++ ctor would need the compiler to synthesise those
base steps from declarations that do not exist yet.

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

### 5c. C3, the allocating constructor: not writable, but reachable

A TU that says `new Stage()` does NOT emit `_ZN5StageC3Ev` — the allocation
and construction fold straight into the calling function (measured:
`probe_spawn` came out 0x88 bytes holding everything, plus local D2 helpers
for the base chain). The ROM's two C3s (`_ZN5StageC3Ev`, 0x0202e088, 0x90
bytes; `_ZN6PlayerC3Ev`) are out-of-line because their construction sequences
large enough that mwcc kept them as functions — `Stage`'s folds an operator
new, three vptr stores, flag bits, and a `Model::C1` member construction.

They cannot be written as source under their own symbol: C3 has no source
form. If they are ever enrolled it will be from a TU whose shape makes the
compiler re-emit the folded sequence out-of-line, isolated down to the C3 —
unproven, and blocked behind the same hierarchy declarations as 5a/5b
(`Stage` embeds a `Model` and sits on the dScene chain).

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

1. **fBase_c / dActor_c chain**: apply §4 to `_ZN7fBase_cC1Ev` (who calls
   it, with which variant semantics?), then declare the chain for real.
   Unblocks the biggest per-class backlog (dActor_c, 11 files).
2. **dBgCh/dBgPi/dM3dG collision family**: name the base ctors the audit
   already identified, un-flatten the MI headers, then dBgCh_Gnd /
   dBgCh_SphCrr / dBgW_Kc* ctors follow.
3. **Leaf singles** (`Camera`, `Clipper`, `Minimap`, `PathPtr`, `HUD`,
   `TextureSequence`, …): each is one §6 application once its header
   declares its ctor; these are the bulk C1 population.
4. ModelAnim family dtor-style follow-ups (`Model::C2` stays hand-written
   until someone wants it real; nothing calls it as a complete object).
