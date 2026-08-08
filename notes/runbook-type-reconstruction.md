# Runbook: reconstructing types, and migrating a function to real C++

**Audience:** an agent or contributor picking this up cold.
**Scope:** turning observed offsets into named, typed members, then rewriting a
function to use them.
**Not in scope:** which symbol a call targets -- see `runbook-reference-repair.md`.

---

## 1. What is actually wrong

The compiler erased types in 2004 and the ROM is its output. A register holding a
`Vector3*` and one holding an `int` are bit-identical; `ldr r1,[r0]` does not care what
`r0` "is". So the ROM records addresses, instructions and relocations -- and no types,
no signedness, no struct layouts.

Nothing in the build can object to a wrong type, because a wrong type usually emits
identical instructions. That is why `src/` currently carries **43,922 local `extern`
declaration lines across 8,948 files**, restating 13,368 distinct names, of which **27%
are declared inconsistently across files**. `ModelAnim::SetAnim` has 123 distinct
spellings; `ApproachLinear` is declared under three different mangled names at once
(`...Riii`, `...Rsss`, `...R7Vector3RKS_5Fix12IiE`). Those were degrees of freedom the
matcher was free to exploit, and nothing ever forced them to converge.

Reproduce those two figures before quoting them -- an earlier revision of this paragraph
claimed 18,497 and 8,523, which do not reproduce under any counting variant and were
already wrong when written:

```sh
git grep -h -cE "^\s*extern " -- 'src/*' | awk '{s+=$1} END{print s}'   # lines
git grep -lE  "^\s*extern " -- 'src/*' | wc -l                          # files
```

**The build is correct; the description is not.** Every *enrolled* function reproduces
the ROM byte-for-byte -- 10,669 at the time of writing, and
`json.load(open("build/rombuild-report.json"))["enrolledFiles"]` for the live number
(the tree matches more than it enrolls -- see the README for the matched total). The
types are wrong as *documentation*, not as *machinery*. This runbook is about making the
description true.

## 2. The three properties of a field, and how they differ

This distinction is the whole safety model:

| property | where it comes from | changing it |
|---|---|---|
| **offset / width** | *observed* from the accesses matched code happens to make | trustworthy, **not** authoritative -- see below |
| **name** | a human | **always free** -- cannot change codegen |
| **type** | a human | **never assume safe; byte-verify every time** |

### Equal width is necessary and NOT sufficient

The tempting rule is "same width, safe". It is false under mwccarm at `-O4,p`, and
measured on this toolchain:

| change | effect |
|---|---|
| `s16` -> `u16`, plain load | `ldrsh` (`e1d000f0`) vs `ldrh` (`e1d000b0`) |
| `s8` -> `u8`, plain load | `ldrsb` vs `ldrb` |
| `s32` -> `u32`, compared | `movgt/movle` vs `movhi/movls` |
| `s32` -> `u32`, `>>` | `asr` vs `lsr` |
| `s32` -> `u32`, `/2` | sign-fixup add disappears; **function shrinks 16 -> 12 bytes** |

**Signedness is part of the type at every sub-word load, comparison, right shift and
division.** Genuinely inert changes: renaming a field, a typedef alias (`Fix12i` is
`typedef s32`), 32-bit load/store with no arithmetic, and int-vs-pointer in a parameter.

### "Observed" is not "correct"

The generated width is the width of the access matched code happened to make, not the
field's real width. `include/ChainChomp.h:13` declares `u8 mScaleX; /* 0x080 */` where
`include/Actor.h:86` -- de-bannered, hand-reconstructed -- declares `s32 mScaleX` at the
same offset, and `src/_ZN10ChainChomp13InitResourcesEv.cpp` settles it by writing
`*(int *)(c + 0x80) = 0x1000;`. The derived header is the wrong one, and it is one of
**87** base-conflicts `tools/gen_header.py --report` still lists. Treat generated offsets
as strong evidence, then reconcile across the whole hierarchy -- which is what pass 2
below does for you.

`include/BrickBlock.h` carries a banner naming the generator that produced it. **That
banner was never true.** No `tools/gen_header.py` was ever committed; the 241 headers
carrying it were added in one commit (`5ddf7d2d`, PR #866) that added zero `tools/`
files, so nothing ever checked what they claim:

```c
/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BrickBlock: 5 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
```

A tool of that name exists now, but it is **not** a regenerator. `tools/gen_header.py
--report` emits a *differential* between what these headers declare and what three
independent evidence passes can prove, bucketed per field; it writes nothing to
`include/`. Headers are still edited by hand -- but now against a report instead of
against nothing. See `notes/plan-gen-header.md`.

**There is a second banner family, and it has no report.** 59 further headers -- the
`d*_c.h` set, `include/dWipe_c.h` among them -- carry a different fabricated attribution:

```c
/* AUTO-GENERATED by tools/deepen_rtti.py from 8 functions
 * class dWipe_c, recovered from ROM RTTI + vtable slot identity. */
```

`tools/deepen_rtti.py` has **never existed in any branch** (`git log --all
--diff-filter=A -- '*deepen_rtti*'` is empty), and these headers came from the same
commit `5ddf7d2d`, which added zero `tools/` files. So the count is:

| banner | headers | differential available |
|---|---:|---|
| `tools/gen_header.py` | 242 | `gen_header.py --report` |
| `tools/deepen_rtti.py` | **59** | **none** |

The naming convention those headers use is sound and independently settled
(`notes/rtti-reconciliation.md` §5/§8 emit ROM names for classes the tree never named).
It is their *field contents* that nothing has ever checked.

So `s32 unk_008` -> `Fix12i currInterp` is inert (`Fix12i` is a `typedef` of `s32`;
renaming and aliasing change nothing). `u16 unk_00c` -> `int` is not: it changes
truncation at every use. There is a real instance in the tree --
`func_ov002_020f2aec` only matches with `int` as the last parameter of
`_ZN3G2x13SetBlendAlphaEPVttttt`, and declaring it `u16` forces a truncation at the
call site that grows the function `0x108 -> 0x110`. **The narrower declaration is the
broken one**, which is the opposite of the intuition that narrowing is conservative.

Rule: **rename freely, treat every retype as codegen-affecting until measured, and
byte-verify.**

### The `#ifdef __cplusplus` block is dead text until something includes it from C++

A generated header's method block is only parsed when a `.cpp` file includes it. Most
headers are included exclusively by `.c` files, so for those the block is text nothing
has ever compiled -- and `gen_header.py` put things in it that are not C++ at all.
Three existed; all three are the same root cause -- *a token copied out of a source file
without checking it names anything visible from the header* -- in three disguises:

| header | declared | where the token came from |
|---|---|---|
| `ExpandingHeapAllocator.h` | `forwards void* Allocate(u32, int);` | a fragment of the parameter name spilled into the return type (#1211) |
| `SolidHeapAllocator.h` | `call ResetEnd void Reset(u32);` | scraped out of the comment on `Reset`'s body, *"bit 0: call ResetStart; bit 1: call ResetEnd"* (#1215) |
| `Scene.h` | `Bool BeforeInitResources();` | a `typedef int Bool;` that is file-local to `src/_ZN5Scene19BeforeInitResourcesEv.c` (#1215) |

**No gate found any of them.** Each surfaced by migrating a method, which compiles the
block for the first time -- one slice per defect. `tools/header_cpp_sweep.py` finds them
all in about a minute: it compiles every header standalone **as C and as C++**, because
that pair is what separates a broken block from a header that merely needs other headers
first (`fails C++, passes C` is the defect; `fails both` is not). Run it after any
`gen_header.py` change, and before starting a slice on a class whose header no `.cpp`
has ever included.

Swept clean at 382 headers, 292 of which carry a block. **Expect the count to stay 0 --
if it rises, a generator regressed.**

What the sweep does *not* catch: a declaration that compiles both ways but **means**
different things. `types.h` defines `typedef int bool;` under `#ifndef __cplusplus`, so
`bool` is 4 bytes in C and the 1-byte keyword in C++. Four headers declare `bool`
returns inside their block (`Animation.h`, `ExpandingHeap.h`, `Heap.h`, `SolidHeap.h`);
all four are already included by `.cpp` files without incident, so it is latent, and
`build_pin.verify` would reject the byte mismatch if a migration ever leaned on it.
Prefer `int` when correcting a block -- that is what the C spelling already resolved to.

### "Until measured": the A/B, and what it does not prove

"Until measured" is not a softening. Some retypes are *byte-unobservable*: if every
compiled reference to a field goes through an address cast
(`*(int*)((char*)&self->unk_018)`), the declaration is never consulted and no spelling of
it can change a byte. That is a fact about the **includers**, not about the type, so it
cannot be reasoned out -- it has to be measured:

1. `tools/affected_src.py include/<Class>.h` -- every TU the header reaches.
2. Apply spelling A, compile them all under their pinned versions, keep the objects.
3. Apply spelling B, recompile, byte-compare object for object.
4. **Add a positive control** -- a scratch TU doing `>> 4` and `/ 2` on the same fields.
   If the control does not diverge, the experiment proved nothing and the *method* is
   broken, not the fields.

Done for the 71 retypes in #1129: 119 includers, 117/117 byte-identical under `s32/s16`
and `u32/u16` alike, control diverging as expected.

**A/B proves harmlessness, not correctness.** It cannot confirm a width -- that rests on
the evidence passes -- and it cannot confirm signedness at all, because what it has just
shown is that nothing depends on it. "The gate proved the width" is the overclaim this
method invites, and `notes/plan-scalar-markers.md` §3 is the retraction of exactly that.
Write **byte-unobservable**, not *verified*.

## 3. The ladder, with the tree's own before/after

**Rung 0 -- generated skeleton** (`include/BrickBlock.h`, today):

```c
struct BrickBlock {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u16 unk_00c;            /* 0x00c */
    u8  pad_00e[0xc6];
    ...
};
```

**Rung 1 -- reconstructed header** (`include/Fader.h`, fields done, vtable NOT):

```c
/* currInterp=0x4 and speed=0x8, both 4 bytes.
 * FIXED POINT. currInterp runs 0..0x1000 -- 0.0..1.0 in 20.12. SetToEnd writes
 * 0x1000 and SetToStart writes 0; SetForwardTime derives speed as 1.0/frames,
 * which is why AdvanceInterp picks its target from the sign of speed. */
struct Fader {
    Fix12i currInterp;  /* 0x04 -- current fade level, 0..0x1000 */
    Fix12i speed;       /* 0x08 -- per-frame delta; sign selects the target */
```

Note what the comment carries: not just names but the *range*, the *encoding*, and
*why* the code branches the way it does. That is the deliverable. A renamed field with
no explanation is a smaller lie, not a truth.

**Two warnings this exemplar carries, both worth copying and one worth not copying.**

*Copy this:* the header spells the class twice. Under `#else` (the C side) it declares
an explicit `void* vtable; /* 0x00 */`, because a C translation unit gets no implicit
vptr. Migrate one function of a polymorphic class without that and every C-side
includer's offsets shift by 4.

*Do not copy this:* the vtable half of `Fader.h` is **known wrong**. Dumping the four
fader vtables out of `extracted/arm9_dec.bin` shows 10 slots each with Fader's slots
2-9 null -- an abstract base -- while the header declares 7 non-pure virtuals, and
`FaderBrightness.h` declares three of the missing methods non-virtual. Fields
reconstructed, hierarchy not. Treat "this header looks finished" as a hypothesis and
check the ROM's own vtable before building on it.

**Rung 2 -- unmigrated function** (`src/_ZN10BrickBlockD0Ev.c`, today):

```c
int *_ZN10BrickBlockD0Ev(int *t)
{
    t[0] = (int)_ZTV10BrickBlock;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
```

`this` is an `int*`, members are array indices, the symbol name is spelled by hand.

**Rung 3 -- migrated function** (`src/engine/fader/_ZN5Fader13AdvanceInterpEv.cpp`, done):

```c
//cpp
// @symbol _ZN5Fader13AdvanceInterpEv
#include "Fader.h"

extern "C" void _Z14ApproachLinearRiii(Fix12i* value, Fix12i target, Fix12i step);

/* Step currInterp one frame toward its target. A positive speed fades toward
   1.0, a negative one toward 0.0; the helper takes an unsigned step. */
void Fader::AdvanceInterp()
{
    Fix12i step = speed;
    Fix12i target = step >= 0 ? 0x1000 : 0;
    if (step < 0)
        step = -step;
    _Z14ApproachLinearRiii(&currInterp, target, step);
}
```

Real method, implicit `this`, named typed members. **The compiler now mangles the
symbol for you** -- `Fader::AdvanceInterp()` becomes `_ZN5Fader13AdvanceInterpEv`
without anyone spelling it.

Note the honest leftover: the helper is still called by its raw mangled name. **Migration
is per-reference, not only per-function.** That line becomes
`ApproachLinear(currInterp, target, step)` only once `ApproachLinear` has a proper
declaration.

## 4. How the migration touches files

Nothing merges and nothing accumulates. The binding is one line in a generated file:

```
src/engine/fader/_ZN5Fader13AdvanceInterpEv.cpp:
    complete
    .text start:0x020175e8 end:0x02017610
```

An **address range is bound to a path**. The file must emit that symbol with those
bytes; its contents are otherwise unconstrained. So migration is an **in-place rewrite**
of one file, optionally moved into a subdirectory. The old text is replaced, not kept --
git history is the only copy. Consolidating the ~11,100 one-function files into real TUs
was measured and declined (byte-safe, worth only 14-19%).

Files touched by one function's migration:

| file | change | risk |
|---|---|---|
| `include/<Class>.h` | rename/retype fields, add the method declaration | shared -- affects every includer |
| `src/<mangled>.cpp` | rewrite body; rename `.c` -> `.cpp` if needed | isolated |
| `config/**/delinks.txt` | only if the path changed | generated; regenerate, never hand-edit |
| `attribution.json` | credit follows the file | a move plus a rewrite in one commit breaks lineage |

**The header is the dangerous one.** Retyping a field changes codegen in *every* file
that touches it, not just the one being migrated. Always re-verify the whole build, not
the single function.

**One rule with teeth:** a commit may *rewrite* a file or *move* it, never both.
Attribution pairs same-commit delete+add by stem and cannot follow a file that changed
in the same commit it moved.

## 5. Procedure

**Cold start:** `tools/mwccarm/**` and `extracted/**` are git-ignored, so a fresh clone
has neither the compilers nor the ROM and step 1 fails immediately. See
`notes/rom-build.md` before anything below.

```sh
git status --porcelain                       # clean
python tools/eligible.py                     # baseline, ~5 min
python tools/check_references.py --update    # bank the starting point
```

1. **Pick a class with several matched functions.** The evidence passes need accesses to
   observe; a class with one function yields a report that is almost entirely `unbacked`.
2. **Run the differential for that class, then read the header.**

   ```sh
   python tools/evidence_history.py     # pass 1 -- offsets/widths from the pre-migration tree
   python tools/evidence_hierarchy.py   # pass 2 -- base-class reconciliation
   python tools/evidence_rom.py         # pass 3 -- disassembly; the only signedness authority
   python tools/gen_header.py --report --cls <Class>
   ```

   The passes write JSON under `build/`; `gen_header.py` reads whatever is present, states
   which ran, and buckets every declared field. It writes no headers. The sibling
   cross-check an earlier revision of this runbook told you to do by hand *is* pass 2.

   **Read the recall line before concluding anything from a zero.** A pass that could not
   see your class says so; `notes/plan-scalar-markers.md` §3a is what happens when that
   distinction is ignored -- a class the hierarchy could not place was reported as
   "no ancestor declares this offset", and it was wrong.
3. **Name and type the fields.** Same width unless you intend a codegen change. Write
   the *why* in a comment -- range, encoding, what selects each branch.
4. **Migrate one function.** Rung 2 -> rung 3. Keep the `// @symbol` line.

   If the member signature will not reproduce, suspect the *name* before you contort the
   body. Roughly 1,349 mangled names are verbatim imports whose parameter types were never
   checked by anything -- a caller cannot observe them, so nothing upstream could have
   caught them, and migrating the function is the first moment they become falsifiable.
   `notes/symbol-name-provenance.md` says which parts of a name to trust and what the
   divergent instruction is telling you (`ldrh` vs `ldr` = declared width; a `blx` with no
   preceding load = function pointer, not pointer-to-one).
5. **Byte-verify that function**, then the whole build, because the header is shared:

```sh
# --addr and --size come from config/**/symbols.txt for that symbol.
# match.py sweeps all 25 compilers and uses its own flags; the build uses ONE compiler
# and different flags. Iterate with it, never take it as the verdict -- see
# runbook-reference-repair.md 2a.
python tools/match.py --c src/<file> --func <symbol> --addr 0x... --size 0x... --module <mod>

# The verdict the build will honour: pinned version, build flags, fails closed.
python - <<'PY'
import sys; sys.path.insert(0, "tools"); import build_pin as BP
print(BP.verify("src/<file>", "<symbol>", 0x..., 0x..., "<mod>"))   # -> (True, '2004/b56')
PY

python tools/affected_src.py include/<Class>.h   # every TU the header reaches
python tools/check_header_offsets.py include/<Class>.h
python tools/rombuild.py                         # 106/106 exact, PASS
python tools/eligible.py                         # AFTER -- diff against the baseline above
```

`python tools/build_pin.py <file>` only **reports** the pin; it does not verify. The
verify path is the library call above, or `--audit`.

**`rombuild.py` alone is not sufficient for a header change.** It compiles only
*enrolled* files, and the tree matches more than it enrolls, so a retype can silently
un-match a non-enrolled includer while 106/106 still passes. Bracket every header edit
with `tools/eligible.py` on a clean tree, before and after, and diff. **That bracket is
the check the ROM build cannot report**, and it is why the baseline is step 0.

6. Repeat per function. Move to a subdirectory only in a **separate commit**.

## 6. Definition of done

- [ ] `ROM-build analysis: PASS`, module fidelity 106/106 exact
- [ ] enrolled count did not fall (`check_references.py`)
- [ ] attribution 0 changed, 0 lost
- [ ] every retyped field is same-width, or the width change is deliberate and stated
- [ ] each field comment says what the value *means*, not just its offset
- [ ] no commit both moves and rewrites a file

## 7. Known dead ends

- **Do not declare a method whose mangled name carries a by-value class parameter**
  (`5Fix12IiE`). mwccarm homes `r0-r3` to the stack for any by-value class parameter,
  costing +0x14. `notes/mwccarm-codegen.md` 6az records two compiler versions; it was
  re-measured in the session that wrote this runbook across **all 25 versions in
  `match.SWEEP` and every optimization level** (`-O0`..`-O4`, `,p` and `,s`), plus
  `-Cpp_exceptions off` / `-RTTI off`, with no combination avoiding it.
  Callers are affected too: `take_i(h, 0x800)` emits `mov r1,#0x800` while the
  `Fix12<int>` form emits `ldr` + `ldm`. Keep those as `extern "C"` free functions with
  scalar args. See `notes/mwccarm-codegen.md` 6az.
- **Do not define a destructor of a polymorphic class as a real method against its
  real header.** Defining it makes it the class's **key function**, so mwccarm emits
  the whole vtable group into that translation unit — `Fader::~Fader() {}` yields
  `_ZTV5Fader`, `_ZTI5Fader`, `_ZTS5Fader` and `_ZN5FaderD0Ev` alongside the
  destructor. Those already exist as delinked ROM data, so `eligible.py` rejects the
  file with **"extra sections: .data"** and the enrolled count falls.

  **`include/Actor.h` already states this rule, and the arrangement is deliberate:**
  *"The key function -- the first non-inline virtual declared -- must never be defined
  as a real method in any translation unit. Declaring the destructor first pins that
  role to TUs which by construction never will."* `include/MeshCollider.h` reaches the
  same end as a rule of thumb (*"The structors stay C files"*), and
  `include/ActorBase.h` differently again — it declares `InitResources` (slot 0)
  in-class but defines it as an `extern "C"` free function on purpose.

  **There is a second, independent blocker**, and it is the one that actually binds:
  a `~Class()` definition always emits **three** functions — D2, D0 and D1 — so the
  object has three `.text` sections and `eligible.py:132-136` rejects it as a
  multi-function TU *even when no vtable is emitted*. `ActorBase` declares its
  destructor last, so it is not the key function and emits no vtable; it is still
  ineligible. Tree-wide: **85** files rejected for `extra sections: .data`, **13** for
  multi-function TU.

  So this is not a codegen problem — the bytes are exact, D1 and D2 both — and vtable
  ownership is *not* the fix (not necessary for `ActorBase`, not sufficient for
  anyone). The prerequisite is a delink model that can bind one file to a
  **multi-function address range**. mwccarm emits the group in D2, D0, D1 order, which
  is exactly the ROM's per-class layout, so one TU already reproduces the contiguous
  range — nothing can bind it.

  **Corollary, and check this before quoting the plan:** the 72 `D1`s
  `notes/plan-cpp-language-mode.md` §2 calls *proven* are matched but **not enrolled** —
  every one is `extra sections: .data`, none is in `build/eligible-names.txt`. Enrolled
  destructor migrations in this tree: **zero**. Phase 3 is unaffected; defining a
  non-key virtual emits a single clean `.text`. Measured in
  `notes/dtor-variant-audit.md` §7.

  **Two escapes were measured and both fail. Do not re-derive them.**

  *Move the key-function role to another virtual.* `include/Actor.h` notes that an
  **override** takes its base's slot wherever it is declared, so for a *derived*
  class the declaration order is free and the role can be moved. For a **root**
  class it cannot: with no base to inherit slots from, vtable slot order **is**
  declaration order. Probed — a root declaring `~R()` first emits `[D1, D0, a]` at
  slots 0/1/2, so declaring it later moves the destructor to slots 2/3 and stops
  matching the ROM. Every root in the ROM puts D1/D0 at slots 0/1 (`dFader_c`,
  `dBgW`, `ModelBase`), so the flip is impossible for exactly the classes that
  motivate it. And for a derived class it buys nothing on its own — the freed
  destructor TU is still a three-`.text` object.

  *Own the vtable in a source file.* dsd selects sections **by name**, and a
  key-function TU emits `_ZTV`/`_ZTI`/`_ZTS` as three sections all called `.data`,
  so `File.o(.data)` would place them contiguously. The ROM does not lay them out
  that way: it groups by **kind**, not by class. The four fader vtables sit
  back-to-back at `0x0208ea9c`/`eacc`/`eafc`/`eb2c` while their typeinfo records
  cluster separately at `0x0208e9e0..0x0208ea24`, interleaved across classes.
  File-granular, name-based placement cannot reproduce that interleaving.

- **Never derive a signature from what callers say.** They disagree 27% of the time.
  The definition is the only non-guess.
- **Do not migrate before the types are right.** A real `struct` built on guessed member
  types is a lie that every later file inherits.
