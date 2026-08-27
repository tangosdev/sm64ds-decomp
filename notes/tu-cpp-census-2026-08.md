# Reconciled ground truth — 2026-08-25

Two censuses ran read-only over worktree `C:\Users\andre\source\tangosdev\sm64ds-decomp` (branch `fix/1607-restack2`).

Data (checked in under `notes/data/`):
- `c-cpp-classification.tsv` — 7,108 `.c` files, one row each, tiered by C++ evidence
- `tu-merge-candidates.json` — 524 TUs from a freshly regenerated tu_map chain
- `provably-cpp-c-files.json` — 1,167 `.c` files provably C++ (direct + transitive)
- `strict-rom-tus.json` — 87 high-confidence TUs seeded by ROM-proven C++ members

## 1. The TU map on disk was INVALID before today

`rtti_vtables.json` (Aug 16) was older than `rtti.json` (Aug 18) — the staleness condition `tu_map.py` refuses. Regenerated in order today: 74 modules, 11,096 fn, **524 TUs** (was 608), boundaries `{low 70, medium 102, high 278}` (was `{97,160,277}`). 104 boundaries vanished, 18 appeared.

**ov006 went 178 -> 87 TUs**: the stale map invented ~91 boundaries there. DISCARD any ov006 plan predating today.

All 8 `--check` gates pass; negative control did not collapse.

- NOTE: this build of `tu_map.py` has NO V4 partition gate and no `meta.partition_defects` (the skill documents V4 as present; it is not in this tree). Do not rely on it.
- NOTE: ov080's known answer is now 4; `notes/tu-boundaries.md` and the slicing skill still say 3. Notes are stale.

## 2. How many .c files are provably C++ (destructors EXCLUDED as evidence)

Direct ROM evidence (file defines a function occupying a NON-destructor slot of an RTTI-attested ROM vtable): **330**
- P1 vtable+mangled 177 | P2 vtable-only (`func_<addr>`, no name at all) 153
- Validation: all 389 vtable-slot hits land on an exact function start in `config/**/symbols.txt`.
- **15** of the 330 already compile as C++ via an active first-bytes `//cpp` marker -> **315 un-migrated**. (19 .c files tree-wide carry an active marker, but 4 are tier P3/P4, outside the 330. Verified 2026-08-25.)

**TRANSITIVE PROOF** (the reconciliation finding; neither agent produced it): mwcc compiles a TU with ONE language mode. So every member of a TU containing a ROM-proven C++ function was compiled by the C++ front end — including members with no C++ signal of their own.

Restricted to TUs with both ends hard, not a swallower, not in an under-segmented module:
- **87** such TUs, seeded by 148 of the 330, additionally contain
- **837** `.c` files tiered PURE-C / WEAK-refs-only / S1 -> transitively proven C++
- (+56 dtor-only `.c` files in the same TUs)

**UNION: 1,167 `.c` files provably C++**, of which **293** sit inside the mergeable safe pool.
Top modules by transitively promoted files: ov006 489, ov002 38, ov100 35, ov071 28, ov072 23.

If you drop the confidence filter and trust every boundary incl. swallowers the number is 4,010 — **DO NOT use that number**; the swallowers (main 2,927 files, ov002 Player 613) dominate it.

Other buckets: dtor-only 259 (out of scope by user's instruction) | suggestive S1 (`extern _ZTV` + hand vptr store, defines nothing) 103 | weak, calls a mangled callee only 2,036 | pure C 4,266.

Permanently excluded: **37** files take a class BY VALUE (mostly `Fix12<int>`/`Vector3`) — mwccarm homes r0-r3 (+0x14), so they can never get a real method body. They are proof the file is C++ AND unconvertible. Not backlog.

## 3. Evidence-strength caveat that constrains claims

A retail NDS cart has NO symbol table. Every `_ZN...` name in this tree is a placeholder, a community import (>=1,349 verbatim from `reference/DynamicAllocationDecomp/symbols.x`), or an assertion — only the ADDRESS was verified on import. So a hand-spelled mangled name is a LABEL, not ROM evidence. That is why tier P3 (112 non-virtual mangled members) + P4 (2 free fns) are NOT in the 330.

Corollary already observed: coined names disagree with ROM RTTI in places — `_ZN10FlameChomp16CleanupResourcesEv.c` occupies slot 3 of `daKrpa_c`'s vtable; Scuttlebug sits in `daSpd_c`. The ROM name wins.

Also confirmed: NO `.c` file in `src/` defines `_ZTV`/`_ZTI`/`_ZTS` data or owns a `.ctor` delink section. Every `_ZTV` in a `.c` file is an extern declaration.

## 4. REVIEW CORRECTION to the C++ census

It claimed "320 clean files collapse into roughly 171 TU-merge units." 171 is its own count of DISTINCT CLASSES, reused as a TU count. That is the class-as-file error the TU map explicitly forbids. The map says the 330 proven files sit in **141** TUs (**87** under the strict filter). Use 87/141.

Every positive file contains exactly one function, so "multi-function TU" blocks nothing — the tree is strictly file-per-function. The real cost is the merge, not the rename.

## 5. Merge pool (from the fresh map)

Licensed pool: **173 TUs / 2,005 files -> 173 files**, net **-1,832** one-function files (16.8% of `src/`), 53 modules.

Gates every one passed: >=2 members; both boundaries high or module-edge; carries a class label; module not under-segmented; every member `complete` in delinks; no `#pragma opt_*`/`optimize_for_size`; no inline asm; no member with an extra section; zero NONMATCHING; zero missing files.

- **Tier 1 (2-9 members): 100 TUs, 667 files, net -567** <- realistic near-term batch
- **Tier 2 (>=10 members): 73 TUs, 1,338 files, net -1,265** <- campaign; needs greedy per-member admission

EMPIRICAL SIZE CLIFF: 80% of 1-4-member TUs compiled; **0 of 159 with >=10 members did**. All-or-nothing.

19 of the 173 are MULTI-CLASS (MegaMushroom+OneUpMushroom 38 files; BowserPuzzleManager+Piece; Boo family; CccArena+CccBigIce+CccSmallIce). Ship each as ONE file or not at all.

Composition: 171/173 are mixed `.c`+`.cpp`, 2 all-`.cpp`, 0 all-`.c`. 811 `.c` + 1,194 `.cpp` members.
- 155/173 contain a member wrapped in an `extern "C" { }` block — `tubuild.py create` REFUSES those ("scanned to end of file without finding a function body"); hand-assemble via `tubuild.build_manifest_entry`.
- 106/173 have file-scope local struct definitions in MORE THAN ONE member (collision risk).
- 5 of the 173 already have `src_tu/` entries at text-/link-verified but UNPROMOTED (PoleLift, FallBlockBfs, LevelObjects, Platform, ov002/Enemy) — dedupe against `config/tu_manifest.d/` first.

Blocked pool: 214 TUs. Traps, in descending attractiveness:
- `main` @0x20049f0 (2,927 files) — NOT a TU, it is the un-segmented remainder of arm9. Do not touch.
- ov002 @0x20bd828 Player (613) — medium/high, 4 incomplete, `opt_common_subs` in 13 members.
- ov007 @0x20b72a0 (389) / @0x20ad660 (123) — no class label, low boundaries, ZERO sinits so no witness is constructible. ov007 is where the map knows least and looks most confident.
- ov006 `dScMg*` family — newly attractive after regeneration, but carries `#pragma opt_*` on up to 35 members in one TU. TUs carrying `#pragma opt_*` CANNOT be merged as-is.
- ov004 @0x20b42c0 (107) — unattributed, low/low, swallower.
- ov063 Boo family (94) / ov060 Bowser (80) — correct groupings, DEFERRED not wrong: clear the pragmas and the incomplete members and they promote into the safe pool.

## 6. Breakage modes for a merge (detect BEFORE committing)

- **SOURCE ORDER**: mwccarm 2004/b56 emits one `.text` section per function in REVERSE source order — the highest-address ROM function is written FIRST. Wrong order = address permutation, NOT a compile error; every member can match individually while the TU lands wrong. Read order from the ELF SECTION INDEX, never `st_value` (`st_value` is 0 for every symbol here). `tubuild verify` catches it; per-member `build_pin` does not.
- **`#pragma opt_*` / `optimize_for_size` are FILE-GLOBAL, LAST-WINS.** Position irrelevant. One stray pragma silently recompiles the whole TU. Precedent: ov062/001 went 14 match/24 differ from one stray `optimize_for_size on`; removing it gave 36/2. Manifests as a silent byte diff on UNRELATED members. Static pre-merge grep removes ~92 TUs from the pool.
- **>=10-member all-or-nothing**: greedy per-member admission, one at a time, keep only if the TU still compiles; never trade a MATCH for a DIFFER. Quadratic past ~200 members.
- **DECLARATION HOISTING regresses, always**: 5 of 5 experiments (72->62, 72->46). Files carry real ordering dependencies (some declare `typedef int s32;` inline). Reconcile IN PLACE, keep the first occurrence, delete later duplicates. NEVER build a preamble.
- **`extern "C"` double-mangling**: C++ silently swallows conflicting declarations as OVERLOADS, so the TU "compiles" having proven nothing and the wrong symbol links. Use `extern "C"` as a diagnostic: give every definition C linkage first, then read the errors.
- **LOCAL STRUCT COLLISION**: two members' private structs merge into one scope -> redefinition error (loud) or one member silently compiling against the other's layout (silent byte diff). Keep the MOST COMPLETE observation (`extern Mtx43 data_x` over `extern char`; `void f(void*)` over implicit `int f()`).
- **VTABLE / VAGUE LINKAGE**: mwcc anchors `_ZTV`/`_ZTI` to the TU that defines the destructor OUT OF LINE. Inline it and the anchor disappears; wrong TU = undefined or multiply-defined `_ZTV` link error. objisolate addend rule: a `_ZTV*` relocation's addend must LOSE 8 on rebinding — get it wrong and it links clean and corrupts 34 modules. Use `tubuild.py linkcheck`.
- **SINIT ORDERING**: `.ctor` entry count == `__sinit_*` count across all 106 modules, zero exceptions, targets always ascending. Two merged TUs that each had a sinit must produce ONE. `tu_map --check` V2a/V2b; 37/74 modules are `corroborated:true` and will catch a miscount.
- **"999 word(s) differ"** means SIZES differ (D0/D1/D2 variant collapse), not a type error.
- **A byte MATCH that calls the WRONG function**: `match.compare` WILDCARDS every relocated word. ov077 `func_ov077_02124118` called `ApproachLinear` where the ROM calls `ApproachLinear2` -> reported MATCH, cost a day. Require all three on the verify line: `byte comparison N/N MATCH`, `objisolate check: clean`, AND reloc-destinations clean.
- **DATA-ONLY OBJECTS are invisible to the map** (built from `.text` runs). 260 delink entries tree-wide have no `.text`; zero mix `.text` with another section. Attribute non-`.text` only by "object k's contribution to every section precedes object k+1's"; discard any rule whose per-TU intervals are not ascending and disjoint. NEVER "TU k's code loads this address, so TU k owns it."
- **`include/decl_common.h` is sometimes ACTIVELY WRONG** (declares a TU's own functions as data -> silent mismatch). Drop it and restate the 3-17 lines. Median distinct includes per safe TU is 7.

## 7. Verification ladder

`shadow -> text-verified -> link-verified -> data-verified -> promoted`
`tubuild.py inspect|create|compile|verify|partial|linkcheck|promote` (promote is `--dry-run` only)

Wall-clock ESTIMATES ONLY (no builds were run this pass): Tier-1 create+compile seconds-to-minutes, but 15-60 min of reconcile iteration per TU is the real cost; verify ~a minute; linkcheck is the expensive step, minutes per TU, and where vtable-anchor failures surface. Tier-2 is ~N compiles for N members — budget hours for the 35-member MrBlizzard.

`rombuild.py` is the FINAL verdict on any merge, not `build_pin`. Run it SERIALLY — concurrent builds against the shared `build/` produce fake failures.

## 8. Sequencing (the two workstreams are coupled)

171/173 safe TUs are mixed `.c`+`.cpp`, and 0 of their 811 `.c` members carry `//cpp`. A merged file has exactly ONE language mode, so **for a mixed TU the merge and the C->C++ conversion are THE SAME EDIT.** Flipping `.c` members to C++ is byte-affecting: bool widening, struct-copy scalarization, `(a==b)` widening as `int` in C vs `bool` in C++.

- **MERGE FIRST, convert inside the merge**, for mixed TUs with no census verdict: converting standalone costs two byte-gated passes and is strictly harder (a one-function `.cpp` must carry the vtable/RTTI anchor alone; in the merged TU the destructor-defining member owns it naturally). Do the language flip inside the greedy admission loop so a member that only matches as C99 can be LEFT OUT rather than sinking the TU.
- **CONVERT FIRST** for the 293 census-proven files inside the safe pool: the language variable is already settled, so the merge reduces to ordering + reconcile.
- **NEVER merge before settling the destructor member's D0/D1/D2 form** for a class needing an out-of-line destructor — the anchor depends on it, and the linkcheck failure will look like a merge bug when it is a class-form bug.

## 9. Known inert-marker violators (report, do NOT tidy)

`//cpp` must be the FIRST BYTES; an `#include` above it makes it inert. In the positive pool: `src/game/actors/dWipe_c/_ZN7dWipe_c15SetBackwardTimeEj.c`, `src/unnamed/arm9/0204/func_0204322c.c`, `src/unnamed/ov075/0211/func_ov075_0211b1cc.c`.

Also: 113 `.c` files tree-wide are absent from `delinks.txt` and therefore INVISIBLE to the byte gates. 190 `.cpp` files still hand-spell their own mangled symbol (separate debt, not part of this).

## 10. Corrections found during planning (2026-08-25, after this doc was first written)

Both plans re-measured this brief against the tree. Five of its claims were wrong; the
plans are authoritative where they disagree. Full detail in
`notes/plan-tu-merge-queue.md` §0 and `notes/plan-cpp-conversion-queue.md` §0.

* **§5's "155/173 TUs blocked by `extern "C"`" is wrong — it is 64/173, 16 in Tier 1.**
  `externc_block_files` counts files *containing* a block; `create` only refuses when the
  *definition* is inside one. 84 of 100 Tier-1 TUs run `create` unmodified.
* **The pragma gate had a hole: `#pragma long_calls on` sits in 15 safe-pool TUs**
  (12 Tier 1) and was never screened. It is file-global and last-wins exactly like
  `opt_*`, and `assemble_shadow_source` does not carry pragmas into the output at all.
* **§5's "5 TUs already in the manifest" is 2** in the safe pool; the census joined on the
  wrong key. The real key is `entries[*].functions[*].legacy_source`.
* **§2's "19 of the 330 already compile as C++" is 15** → 315 un-migrated, not 311.
  19 `.c` files tree-wide carry an active marker, but 4 are tier P3/P4, outside the 330.
* **`n_incomplete` / `n_missing_files` under-report** because the census's `files` list
  silently drops functions with no legacy source. All 100 Tier-1 TUs are genuinely clean;
  3 Tier-2 TUs and both of §5's deferred families hide blockers.

**And one finding that outranks all of them:** `ov002/LevelObjects` is `link-verified` over
a range the regenerated map cuts in two *and* extends past the verified end. In the one
place ground truth exists to check it, a **medium/medium** boundary is wrong in both
directions — empirical support for the high/high + module-edge gate.

## 11. How the two workstreams were sequenced

Both plans claimed the ~90 direct-proven files inside the safe pool. They are **not
substitutes**: the merge's flip buys *codegen* truth (the TU compiles as C++) but
`extern "C"` preserves the hand-spelled symbol — precisely what #821 meant by "your cute
little `//cpp` changes nothing". The conversion's S2 buys *description* truth, where the
compiler mangles the name.

Resolved as a **sequence, not a split**: the merge goes first and owns the language mode
of every safe-pool TU; real name migration then happens *inside the merged `.cpp`*, where
the destructor-defining member owns the vtable anchor naturally instead of a lone
one-function file carrying it. The conversion workstream's independent throughput comes
from the blocked pool, where the merge will never contend.
