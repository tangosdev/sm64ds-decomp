# arm9 endgame: the last 22 functions

arm9 is at **3,070 / 3,090 matched**. What remains is 20 functions and 30,820 bytes of
the module's 456,608. Average size of the residue is ~1.5 KB against ~148 B for arm9
overall, so this is not a tail of near-misses that slipped through, it is the large
functions everything else was matched around.

This file exists so the remaining work can be routed instead of rediscovered. Status is
as of 2026-07-28.

**Update, same day, two down.** `func_02072fcc` matched under 1.2/sp2p3 (#781) and
`func_0206de14` matched under 2004/b56 (#785). Both came out of group B, and both were
functions this file had recorded as merely having a stale draft. Read the gotchas at the
bottom before trusting a negative anywhere else in this file: two of the three have now
been shown to be measurement artifacts rather than properties of the functions.

## Route first, grind second

The 22 split into three groups that want completely different treatment. Sending the
wrong tool at a group wastes the budget, which is the main thing this file is trying to
prevent.

| group | count | bytes | what it needs |
|---|---|---|---|
| A. allocator-bound | 6 | 9,580 | nothing available today; do not spend |
| B. mid-divergence | 7 | 5,348 | ordinary lever work, best value per token |
| C. structurally wrong | 7 | 15,892 | real decompilation from the disassembly |

Group B was 9 / 6,132 before `func_0206de14` and `func_02072fcc` landed.

Group B is where a model tier should be pointed. Group C is where a careful human or a
long-context session should go. Group A should be left alone until something changes.

## Group A: believed impossible at present (do not spend)

Six functions where the C is already correct and only register allocation or instruction
ordering differs. Two attacks have failed on all six: hand-sweeps of the source-level
levers, and a permuter pass. Treat these as closed unless a new compiler build or a new
lever family appears.

Caveat added 2026-07-28: the permuter half of that evidence is weaker than it reads. The
pass was budget-limited, and re-running one group B target for 35 minutes moved a number
the earlier pass had called still. These six have not had a long run. "Do not spend" is a
priority call, not a proof.

| function | size | div | floor |
|---|---|---|---|
| `_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi` | 1,152 | 2 | instruction reorder |
| `_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii` | 1,680 | 2 | instruction reorder |
| `func_0202ffec` | 472 | 2 | regperm |
| `_ZN5Stage13InitResourcesEv` | 2,692 | 4 | register allocation |
| `_ZN12WithMeshClsn20UpdateExtraContinousEv` | 2,668 | 5 | register allocation |
| `func_0206cf98` | 916 | 6 | (unclassified, same shape) |

Why "believed impossible" rather than "hard": the residual is a physical register choice,
and no C expression names a register. The only source-level handle is declaration order,
which reranks the allocator's webs, and that space is small enough to sweep exhaustively.
It has been swept.

`_ZN5Stage13InitResourcesEv` is the worked example, and its evidence is the template for
judging the others. All four of its divergences sit in one five-instruction window:

```
ROM:  mov r2,#0x36 ... mul r2,r7,r1 / ldr r1,[pc] / ldrb r2,[r1,r2] / mov r1,r2
ours: mov r1,#0x36 ... mul r2,r7,r1 / ldr r1,[pc] / and r2,r2,#0xff / ldrb r1,[r1,r2]
```

`bank` is colored r1 by mwccarm and r2 by the ROM. The ROM loads into bank's own register
then copies to the argument register; we fold the load into the argument register and pay
a `u8` truncation instead. Same instruction count, so the register choice is the entire
residual, and the ROM's form is the *less* optimal of the two. Dropping the intermediate
assignment removes the truncation and takes the size 0xa84 -> 0xa80, proving the mask is
load-bearing and direct indexing cannot be the original shape.

~25 variants are inert on it: int vs u8 bank, direct vs two-step indexing, typed
intermediates, the 6aj uninit-decl split in six orders, naming the `data_0209f220` read in
five decl positions to rerank the webs, and 2004/b56 on every shape.

## Group B: mid-divergence, the best target for a model tier

Nine functions, none floored, all with a reproducing draft in the near-miss DB. These are
ordinary lever work: the C is roughly right and the divergence is small enough to reason
about. Best tokens-per-match on the board.

| function | size | div | note |
|---|---|---|---|
| ~~`func_0206de14`~~ | 256 | — | **MATCHED** #785, 2004/b56. Draft was the wrong shape, not stale |
| ~~`func_02072fcc`~~ | 528 | — | **MATCHED** #781, 1.2/sp2p3, permuter from the banked near-miss |
| `func_020316d8` | 460 | 28 | |
| `func_0203e0ac` | 352 | 30 | |
| `func_0201b100` | 648 | 16 | permuter pass did not move it |
| `func_020412f0` | 1,132 | 27 | |
| `func_02044534` | 856 | 47 | |
| `func_02031428` | 688 | 83 | |
| `func_0204a730` | 1,788 | 44 | |

Start with the smallest. `func_020316d8` and `func_0203e0ac` are under 500 bytes with
verified-reproducing drafts, which makes them the cheapest two functions in the entire
remaining tree.

## Group C: structurally wrong, needs real decompilation

Seven functions where the divergence exceeds any plausible lever count. The existing
draft is not a near-miss, it is a different function that happens to compile. Permuting
these is wasted budget; they need someone reading the ROM disassembly and writing correct
C.

| function | size | div |
|---|---|---|
| `func_02009e70` | 4,252 | 1,613 |
| `func_02047910` | 2,340 | 665 |
| `func_0206d32c` | 1,340 | 406 |
| `func_0206f46c` | 948 | 380 |
| `func_020690b0` | 2,152 | 316 |
| `func_02072168` | 2,176 | 283 |
| `func_0206e4a4` | 2,108 | 234 |

### Live lead on `func_0206f46c` (smallest of the group, start here)

It diverges at instruction **two**, and everything after follows from that:

```
ROM:  push {r4,r5,r6,r7,r8,sb,sl,lr}      sub sp,#0x48
ours: push {r4,r5,r6,r7,r8,sb,sl,fp,lr}   sub sp,#0x44
```

The draft spills a frame pointer the ROM does not, so every subsequent stack offset
shifts and all 380 divergences cascade from one wrong frame.

It is a varargs function: `push {r0,r1,r2,r3}` at entry is mwccarm's register-argument
spill. With that prologue the r0-r3 spill slots land at `[sp,#0x68]`, `0x6c`, `0x70`,
`0x74` and the first stacked argument at `0x78`. The ROM reads **bytes** at `[sp,#0x75]`,
`[sp,#0x77]` and `[sp,#0x79]`, i.e. inside the r3 slot and inside the first stacked
argument, which means the real signature takes byte-width parameters the draft models as
ints. Re-derive the parameter list from those offsets before touching the body.

## Gotchas that will otherwise cost a session

- ~~**2004/b56 does not help here.**~~ **WRONG, corrected 2026-07-28.** The sweep was run
  by feeding each stored draft to b56. For the two functions whose stored draft did not
  reproduce, that tested a draft which was the wrong size and, for `func_0206de14`, the
  wrong *signature* — so the real function was never put in front of b56 at all.
  `func_0206de14` matches b56 byte-for-byte once the arguments are modelled correctly
  (#785). A version sweep is only evidence about the source you feed it; re-derive a
  non-reproducing draft first, then sweep.
- **Two drafts did not reproduce, and "stale" was the wrong diagnosis.** Both are now
  matched. `func_02072fcc` fell to the permuter off its banked near-miss (#781).
  `func_0206de14` was not a drifted draft at all: it modelled five ints forwarded through
  `&a0` where the ROM passes two 8-byte structs by value, which is why it came back 12
  bytes short. When a draft misses on SIZE, suspect the shape before suspecting drift.
- **The permuter reports NOCHANGE on a base that failed to compile.** During the arm9
  pass, two targets logged `compile failed (1.2/sp2p3): Errors caused tool to abort`
  and still returned NOCHANGE. A NOCHANGE from `crunch.py` is therefore not by itself
  evidence that a function resists permutation — check whether the converted base built
  at all. `permutable_base()` rewrites a `//cpp` draft to plain C for the C-only parser,
  which is the likely source.
- **Seven minutes per function is too short** for this class. The cracks documented in
  this file's neighbours took thousands of iterations; floors were only trusted at
  800-iteration scores. Budget 25 minutes or more before believing a negative. Confirmed
  2026-07-28: a 35-minute `-j4` run moved `func_020316d8` off the 28 this file records,
  where the earlier pass had reported no movement.
- **Two divergence metrics are in play, and they disagree.** `nearmiss_db.evaluate` scores
  a difflib edit distance over disassembled text; `wallcrack.Target.div` counts words that
  differ positionally. The same source scores 20 by the first and 25 by the second. Every
  number in this file is the DB metric. Do not compare a wallcrack figure against a table
  row here and conclude you improved or regressed something.
- **`func_0203e0ac` had no draft at all.** Its `c_source` was a filesystem path into a
  session scratch directory rather than C, so the entry could not reproduce anything. The
  div-30 source has been restored inline. Worth spot-checking other entries for the same
  failure before trusting a "has a reproducing draft" claim.

## What has already been tried, so it is not repeated

- Full hand-sweep of declaration order and type levers on `_ZN5Stage13InitResourcesEv`
  (~25 variants), detailed above.
- A permuter pass over the twelve closest arm9 targets at 420s each, `-j4`. Groups A and
  the top of B: no banks, no divergence improvements.
- 2004/b56 across all 22.

---

# Part 2: the 43 arm9 NONMATCHING files

Counting "arm9 symbols with no src file" gives 21. Counting "src files marked
NONMATCHING" gives 43. Both numbers are real and neither is the answer on its own, so
the honest arm9 accounting is three lines:

| | count | bytes |
|---|---|---|
| reproducing C | 3,027 | — |
| byte-exact via hand-asm, marked NONMATCHING | 43 | 10,232 |
| no src file at all (Part 1 above) | 21 | 31,076 |

The 43 already reproduce the ROM. `linkcheck` returns VERIFIED with zero blind slots on
them. They are marked NONMATCHING by policy, because hand-written assembly is not a
decompilation. So "run the tools at them and get them matched" does not apply: compiling
them only proves the assembler works. Every one of them passes a full 12-version sweep
including 2004/b56, which is a tautology, not a result.

What matters is that only **12 of the 43 are actually work**.

## A. SDK assembly primitives — 27 files, 7,208 B — leave alone

Functions Nintendo shipped as assembly: block copy, byte fill, CP15, lock IDs, context
switch. There is no original C to recover, so writing C for them invents a source that
never existed. Several already self-declare `Counts as matched (asm-primitive policy)`.
The headers are inconsistent, though: every file also carries the blanket "does NOT count
as matched" banner on line 1, which is what makes the set look like 43 units of pending
work. **Worth fixing the headers so the count stops misleading people.**

`func_0206a928` alone is 4,960 B, more than half this bucket.

## B. Not C-expressible — 4 files, 136 B — leave alone

Symbol-table artifacts rather than functions. `func_020729e8` is a bare epilogue
(`add sp,#0xac; ldmia sp!,{...}; bx lr`). `func_020732e8`, `func_02073584` and
`func_0207335c` start mid-frame, using `fp` without ever establishing it. These are
shared exit stubs the symbol table split out; no C construct produces them standalone.

## C. Convertible hand-asm — 5 files, 424 B — real work

| function | size | div | note |
|---|---|---|---|
| `func_02059d8c` | 12 | — | `subs r0,r0,#4 / bhs / bx lr` delay loop |
| `func_020610fc` | 60 | — | |
| `func_02071644` | 80 | — | |
| `func_02068398` | 120 | 1 | already floored `cond_opt` |
| `func_0206470c` | 152 | — | |

## D. C drafts that do not reproduce — 7 files, 2,464 B — real work

These already contain C. They are ordinary near-misses and the best targets in the whole
set, because the divergences are small and the shape is already right.

| function | size | div |
|---|---|---|
| `_ZN8CapEnemy11GetCapStateEv` | 180 | 3 |
| `_ZN5Model27LoadCompressedTextureToVramEPcjPc` | 184 | 5 |
| `_ZN2GX7LoadTexEPKvjj` | 332 | 16 |
| `_ZN5Stage25PS_UpdateOkAndBackButtonsEb` | 340 | 18 |
| `func_0206062c` | 240 | 20 |
| `func_02038824` | 532 | 22 |
| `_ZN7Message30DisplayCourseNameForStarSelectEj` | 656 | — |

**Start here.** `_ZN8CapEnemy11GetCapStateEv` at div 3 and
`_ZN5Model27LoadCompressedTextureToVramEPcjPc` at div 5 are the two cheapest.

## Worked attempt: func_02057078 (bucket A, and why A is bucket A)

Recorded because it is the evidence that the primitives resist conversion, not just an
assertion. It is the NitroSDK `OS_ReleaseLockID` sibling of `func_02057020`.

```
ROM:  ldr r3,=tbl / cmp r0,#0x60 / addpl r3,r3,#4 / subpl r0,r0,#0x60
      submi r0,r0,#0x40 / mov r1,#0x80000000 / lsr r1,r1,r0
      ldr r2,[r3] / orr r2,r2,r1 / str r2,[r3] / bx lr      (11 insns + pool)
ours: ldr r3,[pc] / cmp r0,#0x60 / addge r3,r3,#4 / subge r0,r0,#0x60
      ldr r2,[r3] / sublt r0,r0,#0x40 / mov r1,#0x80000000
      orr r0,r2,r1,lsr r0 / str r0,[r3] / bx lr             (10 insns + pool)
```

Two differences and both are compiler-internal. mwccarm fuses the shift into the `orr`
(`orr r0,r2,r1,lsr r0`) where the ROM keeps `lsr` and `orr` separate, and it selects
GE/LT where the ROM has PL/MI. Six source spellings produce **byte-identical** output:
plain if/else, `lockID - 0x60 >= 0` sign test, separate `bit` temp, `volatile` temp,
compound `bit >>= lockID`, and the u64 whole-expression launder. The launder does not
break the fusion, which is notable given how reliably it works elsewhere (6h).

That is a peephole no source form reaches, which is why this was hatched. Expect the
same for the rest of bucket A.

## Correcting Part 1's scope

Part 1 above claims 21 remaining and reads as if that is all of arm9. It is not. The
complete picture is 21 with no source plus the 12 real ones here, so **33 functions of
genuine work**, of which the 7 in bucket D are the cheapest anywhere in arm9.
