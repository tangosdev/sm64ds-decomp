# Enrollment audit: what the matched count contains that the cartridge does not settle

Measured at **code ref `7bbfa1ef3edf231df3d40cfb16e59884e82e9420`** (origin/main,
"notes: resolve two committed stash-conflict regions in mwccarm-codegen.md (#1531)").
Every number below was re-derived at that ref by this audit. Nothing is quoted forward
from an earlier measurement, because the counts move by tens of functions per day.

This is a **report, not a policy change**. `matched` is untouched, no flag is flipped,
and contributions.json is byte-identical before and after the code in this PR. What the
published count should be is Tango's call, made on this.

## 1. The shape of the gap

| | functions | bytes | share of code bytes |
| --- | --- | --- | --- |
| universe (config/**/symbols.txt) | 11,396 | 2,235,468 | |
| `matched` (a src/ file exists, unbannered, not a dcd transcription) | 11,221 | 2,093,152 | 93.63% |
| `verified` (a delinks range carries `complete`) | 10,970 | 2,017,640 | 90.26% |
| **gap** | **251** | **75,512** | 3.38pp |

The 251 split cleanly in two, and the halves are different problems:

| class | rows | bytes | meaning |
| --- | --- | --- | --- |
| `unenrolled` | 203 | 71,620 | a delinks entry names the source, with no `complete` |
| `no_block` | 48 | 3,892 | no delinks entry names the range at all |
| stale paths | **0** | 0 | every unenrolled block's src path exists on disk |

Zero stale paths is worth stating plainly: `layout_check` L1 is clean, so none of this
gap is a file that moved and left its delinks entry pointing at nothing.

## 2. The 47-vs-48 boundary, settled

Two independent derivations of the no-block set produced 47 and 48. The one record
between them is:

**`src/game/player/Player_ScaleByCharFactor.c`, ov002, 0x020bf30c.**

Both derivations are right about different questions. The address range 0x020bf30c to
0x020bf340 IS marked `complete` in `config/arm9/overlays/ov002/delinks.txt`, so a test
that asks "is this address covered by a complete range" answers yes and returns **47**.
But the entry that owns it is `mods/Player_ScaleByCharFactor.c`, the single intentional
ROM divergence in the tree (a 4.12 shift changed from 12 to 11, doubling the character
scale factor, documented in notes/rom-build.md as M3). `enrolled_addresses()` drops
`rel.startswith("mods/")` on purpose, and a test keyed on the src path answers no and
returns **48**.

For the question this audit is about, 48 is the right answer:
`src/game/player/Player_ScaleByCharFactor.c` is the faithful decomp, it is counted `matched`, and
it is compiled by nothing. The mod is built in its place. This is the only `mods/` entry
in all 106 delinks files, so the discrepancy cannot recur elsewhere without someone
adding a second mod.

## 3. The 48 no-block rows: policy, not error

Every one of the 48 was re-run through `enroll.candidates()`'s own gates, in order.
**Zero are unexplained.**

| enroll.py's own reason | rows |
| --- | --- |
| on the exclude list (`config/rombuild-exclude.txt`) | 22 |
| not 4-byte aligned (thumb stub) | 12 |
| thumb function | 9 |
| zero size | 4 |
| intentional mod, enrolled under the `mods/` path | 1 |

The 21 thumb rows are the SDK SWI stubs at the bottom of arm9 (`Div`, `IntrWait`,
`CpuSet`, `Sqrt`, `BitUnPack`, `VBlankIntrWait` and friends, 4 or 6 bytes each) plus
three thumb helpers at 0x0205xxxx. `enroll.py` documents why: dsd gives carved code
sections 4-byte alignment, so a 2-aligned thumb stub gets padded up by the linker and
shifts the whole module. These are `HAND-ASM PRIMITIVE` files (`asm void Div(void) { swi
0x09; bx lr }`), which the project's asm policy already accepts as matched.

The 22 exclude-list rows are two documented sub-blocks: 12 in the 0x0206d9dc secondary
entry point cluster, which cannot be carved because a local dsd label cannot be
referenced across objects, and 10 that dsd rejected on relink.

**Verdict: policy-done, count them.** With one flagged sub-case and one defect, below.

### 3a. Flagged: the 8 "produced the wrong bytes" exclusions

`config/rombuild-exclude.txt` puts 8 symbols under "linked cleanly but produced the
wrong bytes": `_ZN11MirrorLuigiD1Ev`, `_ZN15RecRoomCupboardD1Ev`,
`_ZN15RecRoomCupboardD0Ev`, `_ZN14BlendModelAnimD1Ev`, `_ZN5ModelD2Ev`,
`_ZN5ModelD1Ev`, `CleanCommonModelDataArr`, `func_020313d8`.

`tools/linkcheck.py` returns **BENIGN** for all 8: the only word that differs resolves
to a sibling destructor variant, which linkcheck classifies as a benign reloc
resolution. That agrees exactly with the exclude file's own note ("a deleting/complete
variant resolves differently in a real relink than it does when linkcheck verifies the
match in isolation"). The C is right; which of D0/D1/D2 the linker picks is the
disagreement. This is the same failure surface as the known Actor/ActorBase D1/D2
address swap. Counting them is defensible, but they are the weakest 8 rows in the
no-block set and a reader should know they exist.

### 3b. Defect: 4 zero-size itcm aliases double-count in one direction and undercount in the other

| alias (counted matched) | size | primary at the same address | size | primary counted |
| --- | --- | --- | --- | --- |
| `_dmul` | 0 | `func_01ff8708` | 1,776 | **not matched** |
| `_ll_sdiv` | 0 | `func_01ffaa34` | 432 | **not matched** |
| `_s32_div_f` | 0 | `__aeabi_idiv` | 524 | **not matched** |
| `_u32_div_f` | 0 | `__aeabi_uidiv` | 484 | **not matched** |

`config/arm9/itcm/symbols.txt` declares each of these twice, once as a sized function
and once as a zero-size alias at the identical address. `srcpath` resolves
`src/runtime/math/_dmul.c`, which carries a `HAND-ASM PRIMITIVE` banner and is a real accepted
match, onto the **zero-size** record. So the same body is simultaneously counted as
matched (as the alias, worth 0 bytes) and unmatched (as the primary, worth its real
size).

Net effect at this ref: the function count is 4 too high, the byte count is 3,216 too
low, and because itcm carries a 10x coin weight these four zero-byte records are worth
40 MC. Four more zero-size aliases exist in the same file (`__cxa_vec_cleanup`, `_dadd`,
`_ll_udiv`, `_ull_mod`) and are currently unmatched, so the same trap is waiting.

The fix is a naming fix, not a policy fix: point the alias records at the sized primary
symbol, or teach the universe reader to drop zero-size symbols that duplicate an
address. It is out of scope for this PR because it moves `matched`.

## 4. The 203 unenrolled rows: byte-gated, not eyeballed

The brief asked for drafts, complete-but-unenrolled, and stale paths. Comment scanning
answers this badly (only 1 of the 203 carries any draft language at all), so each row
was put through the repository's own byte gate instead: `tools/linkcheck.py` recompiles
the committed source, writes the resolved target into every relocation slot with the
real ARM encoding, and compares the linked function to the cartridge.

| linkcheck verdict | rows | reading |
| --- | --- | --- |
| VERIFIED | 103 | every reloc resolved, linked bytes identical to the ROM |
| BLIND-n | 82 | identical except for n reloc slots that cannot be resolved to an address |
| NO-SYM | 18 | does not compile, or compiles to the wrong length |
| **WRONG** | **0** | no false match anywhere in the set |

**185 of 203 reproduce the cartridge.** Zero are proven wrong.

So "unenrolled" is overwhelmingly **not** "unmatched". Cross-tabulating against
`tools/eligible.py`, which answers the different question of whether the compiled object
is a drop-in replacement for the ROM's bytes, shows why they are still unenrolled:

| eligible.py reason | rows | VERIFIED | BLIND | NO-SYM |
| --- | --- | --- | --- | --- |
| unresolvable reference (rule 5) | 96 | 28 | 68 | 0 |
| .text section larger than the declared range | 80 | 74 | 6 | 0 |
| compile failed under the pinned version | 22 | 0 | 4 | 18 |
| extra sections: .data | 4 | 0 | 4 | 0 |
| defines a differently named symbol | 1 | 1 | 0 | 0 |

The two large classes are placement problems, not matching problems:

* **80 rows overshoot their declared range** by 4 to 32 bytes (a trailing literal pool
  or alignment pad the ROM's own layout absorbs elsewhere). 74 of them byte-match
  perfectly. Dropping the object in would shift every later address in the module, so
  the build correctly refuses, and the source is correct anyway.
* **96 rows name a symbol `config/**/symbols.txt` does not define**: invented
  placeholders like `G`, `@30`, `base_dtor_Clipper`, `overlay_64`, `Actor_TrackStar`.
  Rule 5 rejects those because a gap object imports carved symbols weakly and the
  reference would silently link to 0. This is symbol-coverage debt. It is also what
  makes 82 rows BLIND: 305 reloc slots across those 82 rows cannot be resolved, so those
  words are not compared.

### 4a. The 18 that do not compile

These are the real overstatements in the unenrolled half. All 18 were compiled by hand
at this ref with the ROM build's own flags and every version in the sweep. All 18 fail.

| root cause | rows |
| --- | --- |
| `illegal function overloading`: the file's `extern "C"` declaration of a symbol clashes with a C++-linkage declaration of the same symbol in `include/decl_common.h` | 16 |
| `template argument list expected`: `Fix12` used bare where the real type is `Fix12<int>` | 1 |
| broken draft: three struct members the file's own structs do not declare | 1 |

The overloading class is a single mechanical defect. Example:
`src/unnamed/ov002/020c/func_ov002_020cfbdc.cpp:24` defines `extern "C" int func_ov002_020cfbdc(char *self)`
while `include/decl_common.h:1414` declares `extern int func_ov002_020cfbdc(void*);`
with C++ linkage and a different parameter type. 12 of the 16 clash on their own symbol,
4 on a callee they redeclare (for instance `func_ov064_02119ecc`, declared `(void*, void*)`
in the source and `(char*, void*)` at `decl_common.h:2744`). These files are plausibly
correct decompilations that stopped compiling when `decl_common.h` grew; the byte gate
cannot say, because nothing gets as far as codegen.

The one broken draft is `src/unnamed/ov007/020b/func_ov007_020ba05c.c`, which is the file lane PC1 named.
Independently confirmed here: it fails with `'array28' is not a member of class 'struct
StructObj20'`, plus `array24` and `f2C`, and it carries the comment
`bHolder->b->f2C = 0; // or bHolder->f2C? wait`. It has never been compiled by any gate
and it is counted `matched: true` today.

## 5. Sample verification

At least 10 rows per class were opened and read, beyond the byte gate.

* **unenrolled VERIFIED** (10 of 103 sampled, seed 60): `func_02029408.c`,
  `func_02070c68.c`, `func_ov004_020adbc0.c`, `func_ov004_020b0b1c.c`,
  `func_ov006_020d5fd8.c`, `func_ov006_020d672c.c`, `func_ov006_020dbaf0.cpp`,
  `func_ov006_02103cbc.c`, `_ZN12dScMgSlot1_cD0Ev.cpp`, `func_ov007_020c5dec.cpp`.
  All ordinary decompiled C with no draft language. Confirmed by reading:
  `func_02029408.c` is a two-line indexed dispatch, `func_ov004_020adbc0.c` a null-checked
  field load.
* **unenrolled BLIND** (10 of 82 sampled): `_ZN7ClipperD0Ev.c`, `func_0201a694.c`,
  `func_02034fbc.c`, `func_02037d84.cpp`, `func_02037db4.c`, `func_02044b30.c`,
  `func_02055454.c`, `func_0205f650.c`, `func_ov007_020cc2cc.c`,
  `_ZN13RacingPenguin13InitResourcesEv.cpp`. Every blind slot traced to an invented
  extern name, never to a suspicious body. `func_02055454.c` is
  `extern int G[]; void func_02055454(int v) { G[0] = v; }`: correct code, placeholder
  global. `func_02037db4.c` is a documented this-adjusting virtual-destructor thunk that
  needs C++ flags its `.c` extension does not request.
* **unenrolled NO-SYM**: all 18 opened and compiled, listed in the appendix.
* **no_block**: all 48 attributed to a gate; `Div.c` (thumb `swi 0x09` primitive),
  `_ZN5ModelD1Ev.c` (documented D1 with the D2 codegen note), `func_0206d9cc.c`
  (FIQ-disable primitive with a shared body label), the four itcm aliases and
  `mods/Player_ScaleByCharFactor.c` versus `src/game/player/Player_ScaleByCharFactor.c` read in full.

## 6. What the count becomes under each candidate policy

Baseline is 11,221 matched / 2,093,152 bytes / 93.63% of code bytes.

| policy | matched | delta | bytes | share | coins lost, by contributor |
| --- | --- | --- | --- | --- | --- |
| **A** status quo (a src/ file exists) | 11,221 | 0 | 2,093,152 | 93.63% | none |
| **B** enrolled only (`complete` in delinks) | 10,970 | -251 | 2,017,640 | 90.26% | tangosdev -146, ruspecial -79, andrewboudreau -36, lunavyqo -19, lplaat -4, aitddlabs -2 |
| **C** drop the 203 unenrolled, keep the policy set | 11,018 | -203 | 2,021,532 | 90.43% | tangosdev -127, andrewboudreau -32, lunavyqo -19, ruspecial -18, lplaat -4, aitddlabs -2 |
| **D** byte-gated (drop what linkcheck cannot reproduce) | 11,199 | -22 | 2,085,244 | 93.28% | ruspecial -43, tangosdev -8, lunavyqo -4, andrewboudreau -2, mitch030504 -1 |
| **E** narrow fix (drop only what will not compile) | 11,203 | -18 | 2,085,244 | 93.28% | tangosdev -8, lunavyqo -4, ruspecial -3, andrewboudreau -2, mitch030504 -1 |

Coin figures assume `COIN_WEIGHTS` unchanged (itcm at 10x, which is why ruspecial's
D-column loss of 43 is dominated by the four zero-byte itcm aliases at 10 MC each).
Any of B through E requires bumping `COIN_FORMULA` so the backend rebases instead of
paying or clawing back a delta.

### Recommendation

**Adopt D, the byte gate, and keep `verified` published beside it.**

Reasoning:

1. **B is wrong on the evidence.** It would delete 251 matches of which 229 provably
   reproduce the cartridge. Enrollment is a *placement* test: 80 of the 203 are excluded
   purely because the compiled section runs a few bytes past its declared range, and
   96 because a global has a placeholder name. Neither says anything about whether the
   decompilation is right. Publishing 90.26% as the matched figure would understate the
   project by more than three points and take 146 coins off Tango for work that is
   correct.
2. **C is B's mistake, smaller.** It still deletes 185 reproducing matches.
3. **D removes exactly what cannot be defended**: 18 files that no compiler in the sweep
   will build, and 4 zero-size alias records that are double-counting a body their sized
   twin already reports as unmatched. Nothing that reproduces the ROM is lost. The
   headline moves 93.63% to 93.28%, which is a rounding-level correction, and the largest
   individual coin change is a 43 MC reduction that is mostly the alias defect rather
   than anyone's real work being revoked.
4. **E is D without the alias fix.** It is the smaller edit, but it leaves a record that
   is matched at 0 bytes sitting on top of a record that is unmatched at 1,776 bytes,
   which is the harder thing to explain to anybody reading the site.
5. **Keep `verified` published either way.** #1530 already ships it. The honest sentence
   for the site is that the cartridge settles 90.26% of code bytes today and 93.28% have
   a source that reproduces them, and both numbers should be visible with their
   definitions attached. Collapsing to one number is what created this gap in the first
   place.

Whichever is chosen, two follow-ups are independent of the policy and should land
regardless:

* Fix the 16 `decl_common.h` signature clashes. They are mechanical, and they would
  move 16 files from "counted but never compiled" to "counted and byte-checked".
* Fix the 4 (soon 8) zero-size itcm alias records in `config/arm9/itcm/symbols.txt`.

## 7. Unproven

Stated so nobody reads more into this than it carries.

* **BLIND is not VERIFIED.** 85 rows across both classes have 311 relocation slots that
  cannot be resolved to an address, so those words were not compared. A wrong callee
  hiding in a blind slot would pass. 12 rows have more than 5 blind slots;
  `_ZN13RacingPenguin13InitResourcesEv.cpp` has 13 and two rows have 33 and 35. These
  should be treated as "reproducing, modulo symbol coverage", not proven.
* **The 8 BENIGN rows are contested.** linkcheck (in isolation) says benign;
  `config/rombuild-exclude.txt` says a real relink produced the wrong bytes. The relink
  is the stronger evidence and this audit did not reproduce it.
* **linkcheck sweeps 12 compiler versions; the ROM build pins one per file.** A row that
  reproduces only under 1.2/base is a match by the project's own definition and still
  not buildable at the pinned version. This audit did not record which version won per
  row, so the D column may include rows that would need a
  `config/rombuild-versions.txt` entry before they could ever be enrolled.
* **The 10,970 enrolled rows were not re-verified here.** They are taken as reproducing
  because the ROM build compiles and byte-compares them. No full `rombuild.py` was run
  for this audit.
* **Whether the 16 clashing files are correct decompilations is unknown.** They do not
  compile, so no byte evidence exists in either direction. They are classified as
  "counted without evidence", not as "wrong".
* **Coin deltas are computed from the current attribution** (overrides, aliases and
  finisher credit as of this ref) and will drift as attribution.json changes.
* **The 48 no-block rows were attributed by replaying `enroll.candidates()`'s gates in
  source order**, not by instrumenting the tool. The reasons agree with
  `config/rombuild-exclude.txt` and with `enroll.py`'s own skip counter, but a gate
  reordering would change which reason a row is credited to (never whether it is
  skipped).

## 8. How to reproduce

```sh
python tools/chaos_db_ci.py --out chaos-db.json      # emits the enrollment field
python tools/linkcheck.py --module ov006 -j 10       # the byte gate, per module
python tools/eligible.py -j 16                       # drop-in eligibility, per file
python tools/layout_check.py --json                  # L1 stale paths, L5 unenrolled
```

The `enrollment` field added in this PR is what makes the 203 / 48 split queryable
without re-deriving it. It is purely additive: generating chaos-db.json before and after
at this ref differs by exactly one inserted key per record, with every existing field
holding both its value and its position, an identical `stats` block, and a byte-identical
contributions.json.

## Appendix: the 22 rows that do not reproduce

Every one compiled by hand at this ref with `tools/mwccarm/2004/b56` and the ROM build's
own flags, then swept across all 12 versions by linkcheck. Line numbers are from the
file at this ref.

| module | addr | size | file | credited to | first error |
| --- | --- | --- | --- | --- | --- |
| arm9 | 0x02022f40 | 428 | `src/game/actors/Particle/_ZN8Particle10SysTracker10InitialiseEv.cpp` | tangosdev | line 20: illegal function overloading |
| itcm | 0x01ff8708 | 0 | `src/runtime/math/_dmul.c` | ruspecial | zero-size alias of `func_01ff8708` (1,776 bytes, unmatched) |
| itcm | 0x01ffaa34 | 0 | `src/runtime/math/_ll_sdiv.c` | ruspecial | zero-size alias of `func_01ffaa34` (432 bytes, unmatched) |
| itcm | 0x01ffabe4 | 0 | `src/runtime/math/_s32_div_f.c` | ruspecial | zero-size alias of `__aeabi_idiv` (524 bytes, unmatched) |
| itcm | 0x01ffadf0 | 0 | `src/runtime/math/_u32_div_f.c` | ruspecial | zero-size alias of `__aeabi_uidiv` (484 bytes, unmatched) |
| ov002 | 0x020cfbdc | 424 | `src/unnamed/ov002/020c/func_ov002_020cfbdc.cpp` | ruspecial | line 24: illegal function overloading |
| ov002 | 0x020e3e00 | 400 | `src/unnamed/ov002/020e/func_ov002_020e3e00.cpp` | ruspecial | line 14: illegal function overloading |
| ov004 | 0x020aeed8 | 372 | `src/unnamed/ov004/020a/func_ov004_020aeed8.cpp` | tangosdev | line 50: illegal function overloading |
| ov004 | 0x020af094 | 488 | `src/unnamed/ov004/020a/func_ov004_020af094.cpp` | andrewboudreau | line 56: illegal function overloading |
| ov006 | 0x020e6e78 | 232 | `src/unnamed/ov006/020e/func_ov006_020e6e78.cpp` | tangosdev | line 14: illegal function overloading |
| ov007 | 0x020ba05c | 644 | `src/unnamed/ov007/020b/func_ov007_020ba05c.c` | mitch030504 | line 114: `'array28' is not a member of class 'struct StructObj20'` |
| ov064 | 0x02119afc | 356 | `src/unnamed/ov064/0211/func_ov064_02119afc.cpp` | tangosdev | line 15: illegal function overloading |
| ov065 | 0x02119c38 | 644 | `src/game/actors/TtcRotatingCube/_ZN15TtcRotatingCube13InitResourcesEv.cpp` | tangosdev | line 20: illegal function overloading |
| ov071 | 0x02121ba4 | 200 | `src/unnamed/ov071/0212/func_ov071_02121ba4.cpp` | tangosdev | line 12: illegal function overloading |
| ov078 | 0x02124cf4 | 424 | `src/unnamed/ov078/0212/func_ov078_02124cf4.cpp` | lunavyqo | line 14: illegal function overloading |
| ov080 | 0x02124088 | 384 | `src/unnamed/ov080/0212/func_ov080_02124088.cpp` | andrewboudreau | line 20: illegal function overloading |
| ov081 | 0x02123910 | 528 | `src/unnamed/ov081/0212/func_ov081_02123910.cpp` | lunavyqo | line 31: illegal function overloading |
| ov084 | 0x0212bc30 | 912 | `src/_ZN6Goomba13InitResourcesEv.cpp` | tangosdev | line 17: template argument list expected |
| ov085 | 0x02129dbc | 256 | `src/func_ov085_02129dbc.cpp` | ruspecial | line 17: illegal function overloading |
| ov096 | 0x02137088 | 568 | `src/func_ov096_02137088.cpp` | lunavyqo | line 22: illegal function overloading |
| ov098 | 0x0213a794 | 332 | `src/unnamed/ov098/0213/func_ov098_0213a794.cpp` | tangosdev | line 22: illegal function overloading |
| ov102 | 0x0214b248 | 316 | `src/unnamed/ov102/0214/func_ov102_0214b248.cpp` | lunavyqo | line 18: illegal function overloading |

`src/_ZN6Goomba13InitResourcesEv.cpp` carries its `//cpp` sentinel on line 2, behind an
`#include`. `eligible.classify` and `reloc_audit.winning_object` both test
`text.startswith("//cpp")`, so the file is offered to the compiler as C99 first. Moving
the sentinel to line 1 does not fix it (the bare `Fix12` on line 17 is a genuine error),
but the sentinel placement is worth knowing about as a trap for other files.
