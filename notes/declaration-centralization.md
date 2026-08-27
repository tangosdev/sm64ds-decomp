# Centralizing declarations

## What "a declaration" is here, and why there are so many

Every function in this tree lives in its own file, and any file that *calls* something
outside itself has to tell the compiler that the thing exists. That statement is a
**declaration**:

```c
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
    u32 id, u32 param, const Vector3 *pos, const Vector3_16 *rot, s32 area, s32 unk);
```

It says nothing about what the function *does*. It says only: this name exists, it
takes these types, it returns this type. The compiler needs that to emit a call; the
linker later fills in the address.

There is no rule saying two files must agree about it. Each file carries its own copy,
written by whoever recovered that file. So the tree contains:

```text
5,768  files carrying local `extern` declarations
8,523  distinct names declared
18,497 declaration lines
```

The same fact restated 18,497 times for 8,523 facts. `data_020a0eac` is declared
independently in **339 files**.

## Why they disagree, which is the actual problem

They are not just duplicated. **27% of them contradict each other** — 3,592 of the
13,279 locally-declared names that exist in `symbols.txt` are declared inconsistently
across files. `ModelAnim::SetAnim` has **124 distinct spellings**:

```c
extern "C" int          @(ModelAnim*, BCA_File*, int, int, unsigned int)
extern "C" int          @(char*, struct BCA_File*, int, int, unsigned int)
extern "C" int          @(void*, void*, int, int, int)
extern "C" unsigned int @(ModelAnim* thiz, BCA_File* f, int a, Fix12i b, unsigned int c)
```

`Actor::Spawn` has 117. `ApproachLinear` has 104.

This is not carelessness, and that matters for how it gets fixed. **The byte gate could
never object.** `match.py` compares a compiled function to the ROM word by word, but
every relocated word is a wildcard, because our object holds a placeholder where the
ROM holds a final address. A wrong parameter type usually does not change the emitted
instruction at all — a pointer and an `int` are both one register — so a file could
mistype every external it referenced and still match perfectly.

The per-file declarations were **degrees of freedom the matcher was free to exploit**.
Nothing ever forced them to converge, so they didn't.

## What it costs

Every naming problem becomes a tree-wide sweep, because the fact being fixed is stored
in hundreds of places. This session ran five of them:

| problem | scale | how it was fixed |
|---|---|---|
| C++ files re-mangling ROM names | 882 refs | 44 shared headers given `extern "C"` |
| `G0`/`VT1`/`HEAP` placeholders | 250 files | resolved from relocation data |
| one fake vtable name | **196 files, 196 real symbols** | same |
| mis-recovered signatures | 106 refs | pending |
| wrong callees | 3 functions | found only when enrollment forced a link |

The instructive one is the first. Fixing **44 shared headers** enrolled three times as
many functions as a **255-file** per-file sweep. Centralized facts are cheap to fix;
scattered ones are not.

## What centralizing means

Instead of each file writing its own declaration, one **generated** header states each
fact once, and files include it:

```c
/* include/decl_Actor.h -- generated; do not edit */
#ifdef __cplusplus
extern "C" {
#endif
void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
    u32, u32, const Vector3 *, const Vector3_16 *, s32, s32);
#ifdef __cplusplus
}
#endif
```

Generated, not hand-written — a hand-maintained central header is the same unvalidated
cache with a larger blast radius. The generator's inputs already exist and are already
authoritative:

- `config/**/symbols.txt` — every name and its address, per module
- `config/**/relocs.txt` — every cross-reference dsd found in the real ROM
- the **definition** in `src/` — the one place a signature is not a guess

Then a wrong type is fixed once, and a *new* wrong type has nowhere to live.

44 of these headers already exist (`include/decl_*.h`) and 1,780 files already include
`decl_common.h`. This is extending something proven, not a new bet.

## Why not do it all at once

Byte-testing 55 real currently-matching files, with local declarations stripped and a
generated header included instead:

| group | result |
|---|---|
| 20 `.c`, declarations agreeing with the header | 14 match, 6 compile-fail |
| 20 `.c`, non-majority declarations | 10 match, 9 conflicts surfaced, **1 real codegen change** |
| 15 `.cpp` | **0 match** |

Three things fall out, and each one shapes the plan.

**About 40% need individual reconciliation.** The compile failures are not the tool
breaking — they are the disagreements finally being forced into the open. Every one is
a real question that has to be answered by a person: does `func_ov007_020b7948` return
`void` as its definition says, or `int` as its callers say?

**Declaration form can change codegen.** `src/unnamed/ov002/020f/func_ov002_020f2aec.c` only matches with
`int` as the last parameter of `_ZN3G2x13SetBlendAlphaEPVttttt`; the header's `u16`
forces a truncation at the call site and the function grows `0x108 -> 0x110`. Rare —
1 in 25 — but real. Only the byte gate catches it, so every file must be byte-verified
individually.

**The `.cpp` half cannot take a shared header yet.** 0 of 15. C-harvested declarations
are not C++-clean: an unknown type in a parameter list is a warning in C and a hard
error in C++. That half needs its own approach.

And one rule with evidence behind it: **never generate a signature from what the
callers say.** 6 of the cleanest 20 failed exactly that way — the callers' majority
spelling disagreed with the definition. The definition is the only non-guess.

## Plan

**Phase 0 — stop the bleeding first.** Land the reference ratchet (#1071) before any
of this. Migrating 5,768 files while new inconsistencies are still landing is
pointless.

**Phase 1 — generate, do not migrate.** Write the generator; emit headers to a scratch
directory; report how many names it can state confidently (definition available, one
consistent signature) versus how many need a human. Nothing in `src/` changes. This
turns "27% disagree" from a statistic into a worklist.

**Phase 2 — migrate the agreeing names, `.c` only.** Strip local declarations where the
generated header already says the same thing, in byte-verified batches under the
validator's 200-file cap. Expect ~70% of files to pass mechanically. Reverts are
information, not failure.

**Phase 3 — reconcile the conflicts, one at a time.** Each is a genuine correctness
question. Some will change bytes, and the gate will say so. This is the slow part and
it does not parallelize well.

**Phase 4 — extend the ratchet.** Once a name is centrally declared, forbid re-declaring
it locally. That is a grep, and it is what makes the migration permanent instead of
something that silently rots back.

**Phase 5 — `.cpp`, separately, and only afterwards.** Different failure mode, different
solution, no reason to couple it to the above.

## What not to do

- **No big-bang strip.** ~40% of files need individual attention; one enormous change
  destroys reviewability, the 200-file validator cap, and attribution lineage, for no
  extra correctness.
- **No hand-maintained central header.** Generator output or nothing.
- **Never generate from caller majority.** Definitions only.
- **Do not relax the byte gate to ease migration.** It is the only thing that catches
  the 1-in-25 silent codegen shift, and it is what made this cheap to measure at all.
- **Do not start porting to real C++ classes first.** Real methods re-mangle and
  reopen the war the 44 `extern "C"` headers just ended. Readability is the last pass,
  after the byte war is over.

## The one-sentence version

The tree stores 18,497 copies of 8,523 facts, a quarter of the copies disagree, and
nothing in the build could ever notice — so every fix costs a tree-wide sweep until the
facts live in one place.
