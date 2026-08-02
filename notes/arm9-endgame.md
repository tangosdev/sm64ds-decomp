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

## STATUS 2026-08-02: most of this file is out of date -- re-measured

Everything below was written on 2026-07-28 and has not been re-measured since. It has, so
here is what every function in the three groups does today, compiled against **all 25
installed mwccarm builds** (the sweep used to cover only 12 -- see `tools/match.py`).

**Four functions of the twenty still do not reproduce.** Not twenty, and not the group
split below.

| group | note said | reproduces today |
|---|---|---|
| A -- "believed impossible, do not spend" | 6 impossible | **3 of 6 match** |
| B -- "mid-divergence" | 7 open | **7 of 7 match** |
| C -- "structurally wrong, needs real decompilation" | 7 open | **6 of 7 match** |

Still unmatched across all 25 builds, and these are the real arm9 endgame:

```
_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii     _ZN5Stage13InitResourcesEv
func_0202ffec                                   func_02009e70
```

Four of the matches are **2004/b56 only**: `_ZN12WithMeshClsn20UpdateExtraContinousEv`,
`func_020412f0`, `func_0206f46c`, `func_0206e4a4`. The rest match on the 1.2 trio and b56.

Two consequences worth acting on:

* **Group C's advice is wrong now.** "Permuting these is wasted budget; they need someone
  reading the ROM disassembly and writing correct C" -- six of the seven were matched
  between 2026-07-28 and 2026-08-01, mostly by the readable-conversion passes. Do not
  route work by this file without re-measuring first.
* **Group A's "do not spend" is now better evidenced for the three that remain**, since a
  25-build sweep failing is a stronger negative than the 12-build sweep the original
  measurement had. `_ZN5Stage13InitResourcesEv` in particular keeps its worked example
  below, which is still the best description of that wall in the tree.

The rest of the file is left as written -- its mechanisms, worked examples and gotchas are
still the most useful part, and only the status is stale.

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

Nine functions, none floored, each with a draft in the near-miss DB. Verify the draft
compiles before trusting its number: one entry (`func_0203e0ac`) stored a scratch-file
path instead of C and could not reproduce anything until it was repaired. These are
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
- 2004/b56 across the whole no-src set (0 wins) and across all 42 NONMATCHING files.

---

# Part 2: the arm9 NONMATCHING files (measured, not assumed)

Two counts have circulated, 21 and 43. Both were wrong, and so was the first version of
this section. Measuring rather than reading headers gives a much smaller number.

As of 2026-07-28, 42 arm9 files carry a NONMATCHING marker. Compiling every one of them
against 1.2/sp2p3 and 2004/b56 splits them:

| | count | meaning |
|---|---|---|
| hand-asm primitives | 27 | reproduce; SDK assembly, count as matched by policy |
| **stale marker** | 9 | reproduce, are not asm primitives, header is simply out of date |
| genuinely do not reproduce | 6 | the real work |

So the NONMATCHING set is **6 functions of real work**, not 43. And one of those six is a
duplicate-file artifact (below), which makes it **5**.

## The 9 stale markers — reproduce today, just mislabelled

```
func_02059d8c  func_020610fc  func_0206470c  func_02068398  func_02071644
func_020729e8  func_020732e8  func_0207335c  func_02073584
```

All nine match under both 1.2/sp2p3 and 2004/b56. Four of them (`func_020729e8`,
`func_020732e8`, `func_0207335c`, `func_02073584`) are the exit-stub shapes described
below and are not C-expressible, so their marker is arguably correct in spirit; the other
five simply reproduce and should lose the banner.

## The 6 that genuinely do not reproduce

| function | size | note |
|---|---|---|
| `_ZN8CapEnemy11GetCapStateEv` | 180 | div 4 under b56, see below |
| `_ZN5Model27LoadCompressedTextureToVramEPcjPc` | 184 | div 13 both compilers |
| `_ZN2GX7LoadTexEPKvjj` | 332 | div 34 both compilers |
| `_ZN5Stage25PS_UpdateOkAndBackButtonsEb` | 340 | size mismatch both |
| `func_02038824` | 532 | div 25 sp2p3, **div 20 under b56** |
| `_ZN7Message30DisplayCourseNameForStarSelectEj` | 656 | **already matched** as `.cpp`; the `.c` is a stale duplicate |

`_ZN8CapEnemy11GetCapStateEv` is worked in detail below and is a b56 case, not the
"missing logic" its header claimed.

## Duplicate source files (a link hazard, and why the counts kept moving)

Nine symbols have BOTH a `.c` and a `.cpp` in src/. The repo convention is one file per
symbol, so each pair is a duplicate definition and each also double-counts in any scan.

| symbol | .c | .cpp | action |
|---|---|---|---|
| `_ZN7Message30DisplayCourseNameForStarSelectEj` | no | MATCH | delete `.c` |
| `func_ov070_0211f6e0` | MATCH | no | delete `.cpp` |
| `_ZN15TtcRotatingGear8BehaviorEv` | MATCH | MATCH | pick one |
| `_ZN5Stage7PS_InitEv` | MATCH | MATCH | pick one |
| `func_ov006_020f46ec` | MATCH | MATCH | pick one |
| `func_ov065_02116364` | MATCH | MATCH | pick one |
| `func_ov075_02116f40` | MATCH | MATCH | pick one |
| `func_ov084_021298d0` | MATCH | MATCH | pick one |
| `func_ov084_0212d564` | MATCH | MATCH | pick one |

Three pairs were created by the recent b56 batches (`e8350ed1` / #783 added `.cpp` files
while stale NONMATCHING `.c` siblings from `c31a2a63` stayed). Nothing is broken today,
since every pair has at least one reproducing side, but this should be cleaned up before
it bites at link time.

Reproduce with `scratchpad/dups.py` and `scratchpad/stale.py`, or rerun the same two
checks: compile each file and compare, do not read the header.

## Bucket detail (for the ones that are not work)

**SDK assembly primitives, 27 files.** Functions Nintendo shipped as assembly: block copy,
byte fill, CP15, lock IDs, context switch. No original C exists, so writing C invents a
source that never existed. Several self-declare `Counts as matched (asm-primitive policy)`
while still carrying the blanket "does NOT count" banner on line 1, which is the main
reason this set reads as pending work. `func_0206a928` alone is 4,960 B.

**Not C-expressible.** `func_020729e8` is a bare epilogue (`add sp,#0xac; ldmia sp!,{...};
bx lr`). `func_020732e8`, `func_02073584` and `func_0207335c` start mid-frame, using `fp`
without ever establishing it. These are shared exit stubs the symbol table split out.

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

Part 1 claims 21 with no src file. That number moves as others land matches; it was 22
when Part 1 was written and 20 by the time Part 2 was measured. Check it, do not quote it.

The complete arm9 picture, measured on 2026-07-28:

| | count |
|---|---|
| reproducing C | 3,029 |
| reproduce but marked NONMATCHING (27 asm primitives + 9 stale markers) | 36 |
| genuinely not reproducing, with a src file | 5 |
| no src file at all | 20 |

So **25 functions of genuine work**, not 43 and not 33. The 5 with drafts are far cheaper
than the 20 without, and two of them (`func_02038824` at div 20, `_ZN8CapEnemy11GetCapStateEv`
at div 4) only reach those numbers under 2004/b56.
