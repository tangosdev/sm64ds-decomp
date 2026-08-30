# C→C++ conversion queue

Which `.c` files the ROM proves are C++ translation units, and the staged queue for
migrating them. Ground truth and counts: `notes/tu-cpp-census-2026-08.md`. Per-file
evidence tiers: `notes/data/c-cpp-classification.tsv`. Source form: the
`decomp-cpp-class-form` skill. Prior plan this inherits from: `notes/plan-cpp-language-mode.md`.

**Destructors are out of scope** as evidence and as work, per the scope call. The 37
by-value-class-param files are permanently unconvertible — excluded, not backlogged.

Planned 2026-08-25. No builds were run during planning.

## 0. Corrections to the census, found in the tree

| census said | tree says | impact |
|---|---|---|
| 19 of the 330 already compile as C++ → **311** un-migrated | **15** active, 1 inert, 314 bare → **315** un-migrated. The 15 are all `func_ov006_*.c` files under `src/`; 19 `.c` files tree-wide carry an active marker but 4 are tier P3/P4, outside the 330 | stage sizing |
| 3 inert-marker violators "in the positive pool" | only **1** (`src/_ZN7dWipe_c15SetBackwardTimeEj.c`) is in the 1,167. `src/func_0204322c.c` is WEAK-refs-only, `src/func_ov075_0211b1cc.c` is PURE-C | two are not this workstream's problem |
| — | **262 of the 315** un-migrated direct files are in `build/eligible-names.txt`; **53 are not** | the 53 have no per-file byte gate → TU-work or nothing |
| — | of the 55 safe-pool TUs containing a provably-C++ `.c`, **all 55 are direct-seeded. Zero purely-transitive safe TUs exist.** | the 837 transitive files split 203 (inside direct-seeded safe TUs) / 634 (blocked TUs). There is no "transitive-only merge" to schedule |
| — | `config/rombuild-versions.txt` holds exactly **one** override (`_ZN11dScMgCard_c13InitResourcesEv → 1.2/base`) | `match.py`'s default `CANONICAL` == the build's pin for every file in this plan except that one. Grep before each batch |

Two mechanical facts that shape every stage, both verified:

1. **`build_pin.verify` calls `M.extract_func(obj, func)`** — the emitted symbol must
   still be spelled exactly as the file stem. A bare `//cpp` on
   `int func_ov027_02111680(char*)` yields `_Z18func_ov027_02111680Pc`, and the gate
   reports *"not in the object"*, not a byte diff. So the mechanical flip is
   **`//cpp` + an `extern "C"` wrapper**, which is exactly what the 15 landed files do.
2. **`rombuild.py:415` honours `//cpp` on a `.c` file** — the extension is never
   consulted (`src.suffix == ".cpp" or src.read_text().startswith("//cpp")`). So every
   stage below is a **content-only edit, no `git mv`**. That satisfies
   `notes/plan-cpp-language-mode.md` §6 ("no commit both moves and rewrites a file") and
   keeps every path stable for the merge workstream's manifests. Renaming to `.cpp` is a
   separate, later, mechanical commit — not in this plan.

## 1. The staged queue

The population splits on two axes that are **almost disjoint**, and that drives the ordering:

```
                      cheap to convert          valuable to the merge
  P2 (func_<addr>)    YES  no name decision     mostly NO  (blocked TUs: ov006 51, ov004 13)
  P1 (mangled name)   NO   double-mangles       YES  (79 of the 293 safe-pool files)
  transitive (837)    n/a  no individual proof  203 safe / 634 blocked
```

| Stage | Set | Count | Granularity | Owner |
|---|---|---|---|---|
| **S0** | inert `//cpp` marker in the positive pool | **1 file** | file, no build | report only |
| **S1** | **pilot**, then P2-vtable-only, zero-hazard, enrolled | **82 files** | file-level | cheap model, bulk |
| **S2** | triad (`InitResources`/`CleanupResources`/`OnPendingDestroy`) whose `include/<Class>.h` **already declares the method** under `#ifdef __cplusplus` | **36 files / 23 classes** (31 enrolled, 26 in the safe pool) | file-level, real migration | cheap model from template + spot-check |
| **S3** | same triad, header exists but does not declare the method | **91 files / ~59 classes** | header + file; blast radius | judgment |
| **S4** | remaining direct-proven enrolled files carrying a lexical hazard | **135 files** | file-level, bespoke | judgment |
| **S5** | 53 direct-proven files **not** enrolled | **53 files** | TU-level or nothing | deferred to merge |
| **S6** | 203 transitive `.c` members of the 55 direct-seeded safe TUs | **203 files** | **TU-level, unavoidably** | merge workstream |
| **S7** | 871 provably-C++ `.c` in blocked TUs (573 ov006) | **871 files** | parked | until pragmas clear |

Sum: 1,167 = 293 (safe pool: 90 direct + 203 transitive) + 871 (blocked) + 3 (single-file TUs).

S0–S4 are file-level, each independently byte-gated. **S6 is TU-level**: a transitive file
has no evidence of its own, so flipping it standalone is a bet on the TU boundary and its
payoff only arrives when the TU merges. **S5 is TU-level** because there is no per-file
gate at all — `eligible.py` already rejects those 53, so a standalone conversion cannot be
verified and must not be attempted.

The 293-in-safe-pool split lands as: S1 contributes 6, S2 contributes 26, S3/S4 the
remaining 58 direct, and S6 is the 203. **Per §7, all of these are sequenced behind the
merge** — see the interface section.

## 2. Per stage

**GATE-1 (per file, concurrency-safe).** `tools/match.py` compiles into a
`tempfile.TemporaryDirectory` and never touches `build/`. Safe to run while the merge
workstream builds.

```
python tools/match.py --c src/<FILE>.c --func <SYMBOL> \
    --addr <ADDR> --size <SIZE> --module <ovNNN>
```

`--strict-relocs` is on by default, so this delivers two of the three legs: byte
comparison and reloc-destination identity. Pass = a `MATCH` line under `2004/b56` **and**
no `WRONG-DEST`. Read `<ADDR>`/`<SIZE>` from `config/arm9[/overlays/ovNNN]/symbols.txt`.

**GATE-2 (per stage, serial).** The `eligible.py` bracket — before on a clean tree, after
on the stage's branch. Delivers the third leg (`objisolate` addend/isolation) and the
enrollment count. **This is the leg that catches the key-function trap: a file that starts
emitting `_ZTV` silently leaves the build while GATE-1 still says True.** Batch one
bracket per stage, not per file.

**GATE-3 (per stage, serial).** `python tools/rombuild.py` — `ROM-build analysis: PASS`,
`module fidelity: 106/106`.

Rollback is identical and cheap for every stage because no stage moves a file:
`git checkout -- <files>`. Nothing in S0–S4 edits a header except S3.

### S0 — the one inert marker · 1 file · no build

`src/_ZN7dWipe_c15SetBackwardTimeEj.c` has its `//cpp` below an `#include`, so it is inert.

**Do not move it.** The file is **not** in `build/eligible-names.txt`, so no per-file byte
gate exists for it, and moving the marker to byte 0 would change the language mode of a
file nothing can verify. S0 is a report, not an edit. *(A deliberate downgrade from
"report, do not tidy" — the tree shows tidying it is unverifiable, which is a stronger
reason than tidiness.)*

### S1 — P2 mechanical flip · 82 files

**Entry:** pilot (§3) passed.
**Set:** direct-proven, tier `P2-vtable-only` (`func_<addr>`, no mangled name anywhere),
enrolled, free of all seven lexical hazards. Modules: ov006 51, ov004 13, arm9 9, ov064 7,
ov002 5, ov022 5, ov045 5, ov071 4, ov070 4, ov072 3, tail. Six sit in the safe merge pool
(`func_ov081_021261b8`, `func_ov026_021122b0`, `func_ov026_021122cc`,
`func_ov027_02111680`, `func_ov064_0211755c`, `func_ov022_02112710`) — **per §7 those six
are ceded to the merge**, leaving 76 here.

The exact edit — two lines, no restructuring:

```c
//cpp
// @symbol func_ov027_02111680
...existing comments and #includes unchanged...
extern "C" int func_ov027_02111680(char* self);
int func_ov027_02111680(char* self){ ...body byte-for-byte unchanged... }
```

`//cpp` must be **byte 0**. Every pre-existing `extern` for an unmangled ROM symbol
(`func_*`, `data_*`) must also be wrapped in `extern "C"` or it mangles and GATE-1 reports
the callee undefined. The zero-hazard filter already excluded files with a
`bare_extern_mangled` line (100 of the 262), so this is a lexical transform on a small,
known shape.

**Failure modes, descending likelihood:**

1. `func … is not in the object` → the `extern "C"` wrapper is missing or does not cover
   the definition. Loud, immediate.
2. `N word(s) differ` → a real C-vs-C++ codegen divergence. The 90.6% census figure
   predicts ~8 of 82 land here. **Do not fight them in S1** — revert, move to S4.
3. Enrolled count falls in GATE-2 with all GATE-1s green → something started emitting a
   section. Bisect by module.
4. `999 word(s) differ` → sizes differ; the object gained a symbol. Same handling as (3).

**Honest statement of value:** S1 buys **codegen truth, not description truth.** An
`extern "C"`-wrapped `func_<addr>` still hand-carries its own name, and
`tools/langmode_audit.py` counts it as nothing either way (P2 files carry no mangled
symbol). What S1 actually buys is confounder removal for the merge: it settles "does this
member survive `-lang c++`" before the greedy admission loop has to ask.

### S2 — triad real migration, headers already ready · 36 files / 23 classes

**Entry:** S1 green.
129 `.c` files tree-wide are `_ZN<C><16OnPendingDestroy|16CleanupResources|13InitResources>Ev.c`
and provably C++; 84 distinct classes; **83 already have `include/<Class>.h`**. Of the 129,
**36** have a header that already declares the exact method inside an `#ifdef __cplusplus`
guard; 31 enrolled; **26 in safe-pool TUs** (ceded to the merge per §7).

`include/FallBlockBfs.h` is the exemplar and is already correct in the way that matters:

```cpp
struct FallBlockBfs : daObjFallBlock_c {
    virtual ~FallBlockBfs();       /* slots 16 (D1), 17 (D0) */
    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};
typedef char FallBlockBfs_size_must_be_0x34c[sizeof(FallBlockBfs) == 0x34c ? 1 : -1];
```

The two vtable-slot methods are declared **non-virtual on purpose**. Mangling is
unaffected by virtualness, and not declaring them `virtual` is what keeps this file from
becoming the key-function TU and emitting `_ZTV` — the exact trap the class-form skill
warns about. **Preserve that; do not "fix" it to `virtual`.**

**SINCE LANDED.** This pilot went in with #1684, essentially as written below; the
file is now `src/_ZN12FallBlockBfs13InitResourcesEv.cpp`. Kept because the reasoning
above it is what the rest of the queue rests on.

The exact edit (`src/_ZN12FallBlockBfs13InitResourcesEv.cpp`, `0x02111e10`, size `0x14`, ov045):

```cpp
//cpp
#include "FallBlockBfs.h"
extern "C" int func_ov098_0213a794(void *self, void *data);
extern "C" int data_ov045_021130ac[];
int FallBlockBfs::InitResources() { return func_ov098_0213a794(this, data_ov045_021130ac); }
```

This is a **real** migration: the compiler mangles the name. Verify that claim with the
oracle **before** the byte gate — it needs no ROM and no serialization:

```
python tools/mangle.py src/_ZN12FallBlockBfs13InitResourcesEv.cpp \
    --expect _ZN12FallBlockBfs13InitResourcesEv
```

**Failure modes:**

1. `sizeof(X) == 0xNNN` static-assert fires → the base-class header chain has a layout
   error. **Loud, and a genuine find** — but it is header work, so bounce that class to S3.
2. `this` is now typed `FallBlockBfs*` where the body assumed `void*`/`char*` → add an
   explicit cast. Codegen-neutral; the single most common edit.
3. Enrolled count falls → some class in the batch does have an out-of-line destructor in a
   sibling file and the include chain moved the anchor. Bisect by class; the offender goes
   to the merge workstream.
4. `mangle.py` reports the expected symbol **and** a stray `_ZTV<Class>` → stop; that file
   is now the vtable anchor. Revert and hand it to the merge with the class-form skill's
   D1-inline rule attached.

### S3 — triad, header needs one declaration · 91 files / ~59 classes

**Entry:** S2 green, and the S2 bracket showed no enrollment loss.
Add one method declaration to `include/<Class>.h` inside the existing `#ifdef __cplusplus`
block, then do the S2 edit. Representative classes: `DonutBlock`, `FlameChomp`,
`Scuttlebug`, `dScEntry_c`, `dScTitle_c`, `BabyPenguin`. Two classes have no header at all
(`RecRoomCupboard` + one more) — drop those files; that is header-reconstruction work.

**Why a separate stage:** a header edit has blast radius through every includer, and
`notes/plan-cpp-language-mode.md` §7 names "header retype silently un-matches a
non-enrolled includer" as the top risk. Per class:

```
python tools/affected_src.py include/<Class>.h      # what is in range
```

then the GATE-2 bracket per **class batch**, not per stage — a header regression is
attributable at class granularity and is not at 91-file granularity.

**Specific caution:** `include/decl_common.h` is sometimes *actively wrong* (declaring a
TU's own functions as data). Several triad files include it. When a class's real header
arrives, **drop `decl_common.h` from that file and restate the 3–17 lines it actually
supplied** rather than keeping both.

**Rollback:** revert the header and the files together; never leave a declared-but-undefined
method in a header across a commit boundary.

### S4 — hazardous direct-proven files · 135 files

This is the residue, and where the judgment is. Hazard census over the 262 enrolled
un-migrated direct files (a file may carry several):

| hazard | files | why it costs |
|---|---|---|
| bare `extern` on an already-mangled `_Z…` name | **100** | double-mangles under `//cpp`. Mechanical to fix (`extern "C"`), but each one is an admission the callee is unmigrated |
| local `struct`/`union` definition | 38 | the merge's local-struct collision hazard, in advance. Reconcile in place, keep the most complete observation |
| whole-struct assignment | 38 | C++ scalarizes; the fix is the array-wrapper struct. ~12 bytes short is the signature |
| `this` used as an identifier | 26 | keyword in C++. Rename to `self` — but that is a source change on a file the merge may also want; coordinate |
| `new`/`delete`/`class`/`operator`/… as identifiers | 11 | same |
| empty-paren prototype `f();` | 8 | "unspecified args" in C, "no args" in C++ — silent call-site change |
| `long long` | 4 | usually the `LAUNDER` codegen hack; bespoke |
| `enum { false, true }` | 1 | delete it |

**Rule:** one file, one gate, one revert. Timebox each. A file that will not match after
20 minutes is not a C++-conversion problem, it is a source-recovery problem — log it and
move on. **Do not batch S4.**

### S5 — 53 non-enrolled direct files · deferred, not backlog

`eligible.py` already rejects these (extra sections, or the range is not a clean
single-`.text` drop-in). There is no per-file byte gate, so **any conversion here is
unverifiable and must not be committed**. 48 of the 53 are P1. They become tractable only
as members of a merged TU, where `tubuild verify` gates the whole range. Hand the list to
the merge workstream; do not attempt them.

### S6 — 203 transitive members of the 55 safe TUs · owned by the merge

Files with no evidence of their own. For a mixed TU the merge and the conversion are the
same edit. **This workstream does not touch them.** What it supplies instead: for each of
the 203, a recorded GATE-1 verdict from a *throwaway* flip, so the merge's greedy
admission loop starts with the language question already answered.

### S7 — 871 in blocked TUs · parked

573 are ov006. Dominant blocker signatures by file count: `1 member not complete` (96);
`1 not complete + opt pragma in 7 members` (68); `2 not complete + opt pragma in 35
members` (62); `1 not complete + opt_strength_reduction` (59); `optimize_for_size in 1
member` (51).

These do **not** block a **file-level** S1/S2 conversion of an individual member — 101 of
the 135 zero-hazard files live here and are perfectly convertible. They block only the
merge. So S1 legitimately harvests blocked-pool files; S6 does not.

## 3. The pilot — run first, serially

**`src/_ZN10ChillBully14UpdateRunStateEv.cpp`**, chosen deliberately over the smaller candidates.

Why not the 4-byte `src/_ZN9dThIcon_c6RenderEv.cpp` (an empty `bx lr`): a stub is
codegen-identical in both languages by construction and would validate nothing. This file
is the smallest candidate that puts **real code** through the C→C++ front end while
holding every other variable fixed:

* 0x70 bytes / 28 instructions, with branching, `unsigned short` comparisons against
  0xa/9/0x23, and pointer arithmetic — exactly the integer-promotion and
  comparison-widening territory where a `.c`→`.cpp` rename historically cost 17 words.
* `P2-vtable-only`: no mangled name to preserve, **no naming decision at all**.
* Zero lexical hazards, so a failure is unambiguously codegen, not hygiene.
* Enrolled; no entry in `config/rombuild-versions.txt`, so `match.py`'s default
  `CANONICAL` **is** the build's pin.
* Sits in a **safe-pool** TU (ov027 @`0x21115c4`, 7 members, `ChillBully`+`daIDonketu_c`),
  so a green result immediately feeds the merge workstream.
* Includes `decl_common.h`, so it also exercises the one include known to be sometimes wrong.

The edit (three added tokens, body untouched):

```c
//cpp
// @symbol func_ov027_02111680
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* daIDonketu_c::Kill - recovered from vtable slot identity */
extern "C" int func_ov064_02116110(void *self, int arg);
extern "C" int func_ov027_02111680(char* self);
int func_ov027_02111680(char* self){ /* …unchanged… */ }
```

The `func_ov064_02116110` declaration currently arrives via `decl_common.h`; if that
header is not `extern "C"`-guarded, the local `extern "C"` line above is required. Check
the header first — that is the one judgment call in the pilot.

**Verification, exactly this, serially:**

```
python tools/match.py --c src/_ZN10ChillBully14UpdateRunStateEv.cpp \
    --func _ZN10ChillBully14UpdateRunStateEv --addr 0x02111680 --size 0x70 --module ov027
```

**PASS (all three, no partial credit):**

1. `TARGET func_ov027_02111680 @ 0x02111680 size 0x70` followed by **MATCH under `2004/b56`**;
2. **no** `(reloc-destination check unavailable: …)` line — if it appears the run is "not
   verified", never "probably fine";
3. **zero** `WRONG-DEST` verdicts.

**FAIL:**

* `func … is not in the object` → the `extern "C"` wrapping is wrong. Fix and rerun; not a
  signal about the approach.
* `N word(s) differ`, N small (1–4) → C++ widened a comparison. **Does not kill the plan**
  — it moves this file from S1 to S4 and says the S1 hit rate is nearer 90% than 100%.
* `N word(s) differ`, N large, or it will not compile despite the hazard filter saying it
  was clean → **this kills the mechanical S1 stage.** The filter would be proven not
  predictive and the queue collapses back to per-file judgment (S1 and S2 dissolve into
  S4). Report that loudly; it changes the effort estimate by roughly an order of magnitude.

**Follow-on, only if the pilot passes** — validates the *naming* half that S2 depends on
and the pilot deliberately excludes:

```
python tools/mangle.py src/_ZN12FallBlockBfs13InitResourcesEv.cpp --expect _ZN12FallBlockBfs13InitResourcesEv
python tools/match.py --c src/_ZN12FallBlockBfs13InitResourcesEv.cpp \
    --func _ZN12FallBlockBfs13InitResourcesEv --addr 0x02111e10 --size 0x14 --module ov045
```

Pass = `mangle.py` reports the expected symbol **and no `_ZTV12FallBlockBfs`**, plus a
clean `match.py`. If `_ZTV` appears, S2's premise (that the non-virtual declaration keeps
the anchor elsewhere) is false and S2/S3 must be re-scoped as merge work.

## 4. Deliberately not touched

| excluded | count | reason |
|---|---|---|
| by-value class param | 37 | mwccarm homes r0–r3 (+0x14) on all 25 sweep versions at every opt level. Permanently unconvertible; excluded, not backlogged. They remain evidence the file is C++ |
| destructors — D0/D1/D2 | 259 dtor-only + every `~X()` | out of scope by instruction. Every stage is constructed to avoid becoming a key-function TU, the only route by which destructor form could leak in. S2's non-virtual triad declarations exist precisely to keep that door shut |
| constructors | 54 | zero ever migrated in this tree. A research spike, not throughput |
| `main` @0x20049f0 | 2,927 | not a TU; the un-segmented remainder of arm9 |
| ov002 Player @0x20bd828 | 613 | swallower; 4 incomplete members; `opt_common_subs` in 13 |
| ov007 @0x20b72a0 / @0x20ad660 | 389 + 123 | no class label, low boundaries, **zero sinits so no witness is constructible**. Where the map knows least and looks most confident |
| ov004 @0x20b42c0 | 107 | unattributed, low/low, swallower. *13 S1 files are ov004 but from other, non-swallower ranges — S1 harvests files, not TUs, so this exclusion does not cost them* |
| `#pragma opt_*` TUs | ~92 TUs | file-global, last-wins. **But a pragma blocks the *merge*, not a file-level conversion** — a per-file gate is unaffected by a sibling's pragma, so S1/S2 deliberately do convert individual members of pragma-blocked TUs |
| 190 hand-spelled `.cpp` | 190 | separate debt (`langmode_audit`'s second population). Renaming an extension is not a migration |
| 113 `.c` absent from delinks | 113 | invisible to every byte gate; any edit is unverifiable. Same rule that made S0 a report and S5 a deferral |
| `.c` → `.cpp` renames | all | deferred entirely. `rombuild` honours `//cpp` on a `.c`; renaming adds attribution churn and collides with the merge's manifest paths for no byte-level gain |

## 5. Effort shape

**Bulk-delegable to a cheap fast model (~118 files, less whatever §7 cedes):**

* **S1.** Insert `//cpp` at byte 0, add one `extern "C"` declaration for the file's own
  symbol, wrap any pre-existing unmangled `extern` in `extern "C"`. Zero semantic
  decisions. Give the model the file, the expected symbol, and the `match.py` command;
  require it to paste the MATCH line. Reject anything else.
* **S2.** One template, 23 instantiations. The only per-file variation is the `this` cast.
  The header already carries the declaration and the size assertion, so the model has an
  oracle (`mangle.py --expect`) and a compile-time layout check before it reaches the byte gate.

**Needs real judgment (~279 files), unevenly distributed:**

* **S3.** The judgment is *"is this header's layout actually right?"* — which the `sizeof`
  assert asks, but only for classes that already have one. Adding a declaration is
  trivial; deciding whether the base chain is correct is header-reconstruction work with
  tree-wide blast radius, and it is the one place a mistake propagates to files nobody touched.
* **S4.** Genuinely bespoke. The 100 `bare_extern_mangled` files look mechanical but are
  not: each is a decision to wrap the callee in `extern "C"` (cheap, preserves the lie) or
  migrate the callee too (expensive, correct). That decision is the whole of #821 and
  should be made per class, not per file.
* **S5.** The judgment is "don't."

**Where the judgment is *not*, despite looking like it is:** deciding which files are C++.
That is settled — the 330 are ROM-proven by their own vtable slot, and no stage here asks
anyone to re-litigate it. Spend no time on evidence.

**Estimates** (no builds run): S1 ≈ 82 × 3 min model time + one serial bracket.
S2 ≈ 36 × 10 min + one bracket per class batch. S3 ≈ 59 classes × 30–60 min.
S4 ≈ 135 × 20 min timeboxed, expected ~60% completion. **The serial-gate windows, not the
edits, are the schedule.**

## 6. Interface with the TU-merge workstream

See `notes/plan-tu-merge-queue.md`. Sequencing rules: §8 of `notes/tu-cpp-census-2026-08.md`.

**Resolution of the one collision.** Both plans claimed the ~90 direct-proven files inside
the safe pool. They are **not substitutes**: the merge's flip buys *codegen* truth (the TU
compiles as C++) but `extern "C"` preserves the hand-spelled symbol — precisely what #821
meant by "your cute little `//cpp` changes nothing". S2 buys *description* truth, where the
compiler mangles the name.

So they are **sequenced, not split**:

* The **merge goes first** and owns the language mode of every safe-pool TU. S1's 6, S2's
  26 and S3/S4's ~58 safe-pool files are ceded.
* **Real name migration then happens inside the merged `.cpp`**, not on the one-function
  files — still byte-gated by `tubuild verify`, but now the destructor-defining member
  owns the vtable anchor naturally instead of a lone one-function file carrying it.
* This workstream's independent throughput comes from the **blocked pool**, where the
  merge will never contend: 101 of the 135 zero-hazard S1 files already live there, and
  the merge has explicitly excluded ov006 (51) and ov004 (13).

**OWES the merge:**

1. **A per-member language verdict for all 811 `.c` members of the 173 safe TUs.** GATE-1
   is concurrency-safe, so this can be produced as a standing artifact without competing
   for the build. It removes the largest confounder from the greedy admission loop: when a
   merged TU differs, the merge no longer has to ask whether it was the language flip or
   the ordering. Format: `{path, symbol, addr, size, module, cpp_ok: bool, ndiff: int}`.
2. **Path stability.** No `git mv` in any stage, so `delinks.txt` entries and any `src_tu/`
   manifest stay valid across this workstream's commits.
3. **No destructor form settled or disturbed.** S2 preserves the non-virtual triad
   declarations specifically so that no conversion here can become the `_ZTV` anchor and
   pre-empt the merge's D0/D1/D2 decision.
4. **A hazard map, in advance,** of the two failure modes the merge will hit anyway: 38
   files with local struct definitions, 100 with bare mangled externs.
5. **The S5 (53) and S6 (203) lists**, handed over rather than attempted.

**NEEDS from the merge:**

1. **A TU claim protocol.** The unit of collision is the TU range `(module, start)`, not
   the file. Once the merge admits a TU, this workstream stops editing its members.
   `tools/agentlock.py` is present but untracked; if that is the mechanism, key it on
   `(module, start)`.
2. **A serial gate calendar.** GATE-2 and GATE-3 are the collision-prone resources — about
   one bracket per stage, five or six windows total, which is a small ask against the
   merge's per-TU `linkcheck` cadence. Everything else here runs through
   `match.py`/`mangle.py`, which are safe.
3. **Ownership of every `_ZTV`/`_ZTI` question.** If a file starts emitting a vtable it
   leaves this queue and enters the merge's. Detect it here (`mangle.py` symbol list, then
   the enrollment count); do not resolve it here.
4. **Notice when a TU is dropped as unmergeable.** Its transitive members (S6) then become
   permanently unverifiable-in-isolation, and that must be said rather than quietly attempted.

**Where the plans could collide and do not:** S1 harvests 101 of its files from
*blocked-pool* TUs the merge has explicitly excluded — throughput the merge will never
contend for. Conversely the merge's largest prizes (the 73 Tier-2 TUs, 1,338 files) contain
218 provably-C++ files that this plan schedules none of, because the size cliff makes that
pool all-or-nothing merge work, not conversion work.

## 7. Tooling (resolved)

Both plans want new files in `tools/` (`tu_create.py`, `tu_preflight.py`). This used
to be a caveat: the validator **restored all of `tools/` from base**, so no PR could
exercise its own tool change, and a batch whose correctness depended on a new tool
was validated with that tool deleted.

That is no longer true. The validator runs the **committed test merge** whole,
`tools/` included, so a batch ships its tools and is validated by them. A tools-only
PR is now a full base-vs-merge ROM comparison instead of a green no-op, which is
exactly the evidence a byte-neutral `tu_create.py` change wants.

Two things a batch still may not put in `tools/`: anything under `tools/mwccarm/`
(the compiler is the operator's, and a commit there is refused outright), and a
`config/rombuild-versions.txt` pin naming a compiler build the validator does not
have installed (validated, refused with a reason, never silently replaced).
