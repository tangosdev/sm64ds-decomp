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
identical instructions. That is why the tree currently holds **18,497 declarations for
8,523 names, 27% of which contradict each other**. `ModelAnim::SetAnim` has 124
distinct spellings; `ApproachLinear` has 104. Those were degrees of freedom the matcher
was free to exploit, and nothing ever forced them to converge.

**The build is correct; the description is not.** All 10,532 *enrolled* functions
reproduce the ROM byte-for-byte (the tree matches more than it enrolls -- see the README
for the matched total). The types are wrong as *documentation*, not as *machinery*. This
runbook is about making the description true.

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
field's real width. In this very family, `include/FaderWipe.h` declares
`u8 mFadeAmount; /* 0x004 */` where `include/FaderColor.h` declares
`s32 unk_004; /* 0x004 */` -- the same inherited field. Treat generated offsets as
strong evidence, then reconcile across the whole hierarchy.

`include/BrickBlock.h` carries the banner of the generator that produced it. **Note
that generator is no longer in the tree** -- `tools/gen_header.py` does not exist, so
these headers cannot currently be regenerated and must be edited by hand:

```c
/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BrickBlock: 5 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
```

So `s32 unk_008` -> `Fix12i currInterp` is inert (`Fix12i` is a `typedef` of `s32`;
renaming and aliasing change nothing). `u16 unk_00c` -> `int` is not: it changes
truncation at every use. There is a real instance in the tree --
`func_ov002_020f2aec` only matches with `int` as the last parameter of
`_ZN3G2x13SetBlendAlphaEPVttttt`, and declaring it `u16` forces a truncation at the
call site that grows the function `0x108 -> 0x110`. **The narrower declaration is the
broken one**, which is the opposite of the intuition that narrowing is conservative.

Rule: **rename freely, treat every retype as codegen-affecting, and byte-verify.**

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

1. **Pick a class with several matched functions.** `gen_header.py` needs evidence;
   a class with one function yields a header of padding.
2. **Read the existing generated header.** `tools/gen_header.py` is gone, so there is
   nothing to regenerate -- the committed skeleton is the evidence you have. Cross-check
   its offsets against every sibling header in the hierarchy before trusting a width.
3. **Name and type the fields.** Same width unless you intend a codegen change. Write
   the *why* in a comment -- range, encoding, what selects each branch.
4. **Migrate one function.** Rung 2 -> rung 3. Keep the `// @symbol` line.
5. **Byte-verify that function**, then the whole build, because the header is shared:

```sh
# --addr and --size come from config/**/symbols.txt for that symbol
python tools/match.py --c src/<file> --func <symbol> --addr 0x... --size 0x... --module <mod>
python tools/rombuild.py
python tools/rombuild.py
```

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
- **Never derive a signature from what callers say.** They disagree 27% of the time.
  The definition is the only non-guess.
- **Do not migrate before the types are right.** A real `struct` built on guessed member
  types is a lie that every later file inherits.
