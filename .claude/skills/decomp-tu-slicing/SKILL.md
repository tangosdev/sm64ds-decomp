---
name: decomp-tu-slicing
description: Find which classes shared an original .cpp translation unit in sm64ds-decomp, so a C++ conversion PR ships the right set of classes together instead of splitting a file the ROM says is one. Use before slicing a per-class migration PR, when deciding whether two classes belong in one header, when eligible.py rejects a file for extra sections or a multi-function TU, when asked which functions belong to a class in an unnamed overlay, or when reading build/tu_map.json. To then execute the merge and byte-verify it, use decomp-tu-build.
---

# Translation-unit slicing

The linker erased the original `.cpp` boundaries; `tools/tu_map.py` reads them back
out. Full derivation and limits: `notes/tu-boundaries.md`. **To execute a merge and
verify it, use `decomp-tu-build`** — this skill is about reading the map, not acting
on it.

## 0. Regenerate the map, in this order

```sh
python tools/rtti_extract.py     # -> build/rtti.json
python tools/rtti_vtables.py     # -> build/rtti_vtables.json
python tools/tu_map.py           # -> build/tu_map.json
```

`build/` is gitignored, so a fresh worktree has none of this.

**Nothing enforces that order, and skipping it fails silently.** `vtable_labels()`
returns `{}` when `build/rtti_vtables.json` is absent, and `tu_map.py` then writes a
complete, self-consistent, **wrong** map and exits **0**. Measured on `main` from an
empty `build/`:

| | full chain | `rtti_vtables.json` absent |
|---|---|---|
| whole-ROM TUs | 532 | 516 |
| boundaries | `{low: 68, medium: 110, high: 280}` | `{low: 72, medium: 124, high: 246}` |
| TUs with a class | 400/532 | 370/516 |
| under-segmented | ov007, main, ov075, ov084 | ov007, main, **ov005**, ov084 |
| ov080 | 5 TUs, 4 classed | **6 TUs**, 4 classed |
| `--check` | all gates PASS, exit 0 | **all gates PASS**, exit 0 |

The known-answer gate asserts ov080's *classed* TU count, which is 4 either way, so it
passes for the wrong reason. The under-segmented list is itself a casualty, so you
cannot use it to detect the problem either. The one visible tell is the negative-control
table, which collapses to `blind 1 / blind-classed 0`. There is no exit code to lean on:
check the prerequisites yourself.

`--blind` is **not** an opt-out — it is the negative control, dropping the mangled-name
signal to score what RTTI alone recovers. The whole flag set is `--module`, `--verbose`,
`--check`, `--blind`, `--split-swallowers [K]`, `--out`; nothing suppresses the RTTI
labels while keeping the names. `tubuild.py` regenerates the map only when it is
*entirely absent* — a stale-but-present map is reported as a note and used as-is.

Figures move whenever the map changes, so run the command rather than quoting these.
Measured on `main` at `343eab070`: **74 modules, 11,091 functions, 532 TUs, 305 sinits**,
boundaries `{low: 68, medium: 110, high: 280}`, 400/532 carrying a class.

## The one rule that matters

**A class is not a file.** Where two classes shared a `.cpp`, mwcc emitted their
methods interleaved in source order, so they look like many alternating blocks and are
in fact one TU. Never slice on "the class name changed."

```sh
python tools/tu_map.py --module ov080
ov080: 86 functions -> 5 TUs (4 with a class), 3 sinits / 3 ctor entries [ok]
  0x2123740-0x2124a20    26 fn  MontyMole, MontyMoleRock, daChoropu_c   <- ONE TU
  0x2124a20-0x2125404    21 fn  CrazedCrate, daBttBk_c
  0x2125404-0x2126fbc    29 fn  Painting
  0x2126fbc-0x212714c     6 fn  daObjMaruta_c, daPicGate_c
  0x212714c-0x212766c     4 fn  (unattributed)
  boundaries: {'high': 3, 'medium': 1}
```

MontyMole and MontyMoleRock ship together or not at all. Splitting them into two PRs
invents a structure the ROM contradicts, and any later consolidation has to undo it.

## Before slicing a per-class PR

1. `python tools/tu_map.py --module <ovNNN>` — find the TU holding your class.
2. **Every class listed on that line is in scope.** Names like `daChoropu_c` are the
   RTTI (EAD internal) name for a class you already know by its English name — the
   same class, not an extra one. Two *different* English names on one line means two
   classes genuinely co-resided.
3. Check `corroborated` for the module in `build/tu_map.json`. `true` means the sinit
   count independently confirms the cut count — a witness that never informed the
   cuts. `false` means the boundaries rest on labels alone.
4. Check the boundary confidence. `low` boundaries cannot distinguish two classes in
   one file from two files that linked adjacently; do not slice across one without
   further evidence.

## Trust calibration

| situation | read |
|---|---|
| module `corroborated: true`, boundary `high` | slice on it |
| boundary `medium` (one side unattributed) | the classed side is sound; the run beside it is unresolved |
| boundary `low` | contiguity only — evidence, not an answer |
| module in `meta.under_segmented` | TU count is a **lower bound**: ov007, main, ov075, ov084 |

In an unnamed overlay, a TU carrying a class name is largely trustworthy — but the
unattributed runs between them are *unresolved*, not TUs. Do not report a raw TU count
from an under-segmented module as if it were the file count.

The negative control (`--check`, bottom table) is the calibration, and it is good but
not exact. Names stripped, RTTI alone recovers the known classed-TU count in 3 of the 4
known-answer modules — ov062 5/5, ov063 4/4, ov020 2/2 — and falls one short on ov080
(3 against 4). Its `blind` column is much larger than `known` (10, 9, 6, 4) because
without mangled names the unlabelled runs never get absorbed.

## Gotchas that have already bitten

* **A module can contain an object with NO `.text` at all.** `tu_map --module ov045`
  reports 6 TUs; ov045 has a seventh, data-only object — 2,128 bytes at the head of
  `.data`, read by three TUs' `InitResources`. `tu_map` is built from `.text` runs and
  **structurally cannot see it**, so "N TUs" is a count of code-bearing objects, not of
  original files. (The 6 is reproducible; the 2,128 is an analysis result not recorded
  anywhere in `notes/` — treat it as unconfirmed.) It was
  found only because attributing `.data` by "whose code loads it" produced a TU
  ordering the linker's layout rule forbids. Distinct from the 31 whole modules that
  are data-only.
* **A size-0 function symbol is an ALIAS, not a function.** All 8 in the corpus are a
  second `kind:function` symbol at an address a *sized* one already occupies — the ARM
  runtime exporting both its mwcc and AEABI name from one entry point (`_u32_div_f`,
  `_s32_div_f`, `_dadd`, `_dmul`, `_ll_udiv`, `_ll_sdiv`, `_ull_mod`) plus
  `__cxa_vec_cleanup`. They are not unrecorded sizes: no size-0 symbol is alone at its
  address and no address carries two sized ones, so the extent is never ambiguous.
  **This is NOT fixed on `main`.** `functions()` reads symbols.txt straight through with
  no collapse by address, so these 8 still fragment real runs into 4-byte units that
  overlap their neighbours — 8 overlapping unit pairs in today's map, all of them in
  `main` and `itcm`, which are therefore **not partitions**. If you inventory symbols
  yourself, collapse by address (one entry per address, survivor is the sized symbol) or
  you inherit the same defect.
* **RTTI spans can bridge two real TUs.** A vtable slot may point at inherited or
  shared code in the neighbouring object. `tu_map.py` already suppresses this
  (mangled names win over RTTI spans), but if you compute spans yourself, don't
  re-introduce it — ov081's `daSnowman_c` and ov090's `daMenbo_c` are the cases.
* **Inventory functions from `symbols.txt`, not `delinks.txt`.** That is what
  `tu_map.functions()` does, and it is the right rule — delinks is a build artefact and
  can be re-partitioned, symbols.txt is the inventory. Its docstring justifies the choice
  with an ov062 hole at `0x02117724-0x02117994`; **that hole no longer exists on `main`**
  (config/arm9/overlays/ov062/delinks.txt covers it, and a sweep of every module finds 0
  symbols.txt functions uncovered by a delinks `.text` entry). Keep the rule, don't cite
  the example as current.
* **Key by (module, addr), never addr alone.** 44 of the 93 modules `tools/modules.py`
  lists — all of them overlays — load at base `0x021111A0`.
* **Join files to TUs by address -> symbol -> `units[].functions`**, never by file stem
  (renames) nor by TU interval — 109 functions sit outside their own TU's start/end.
* **`tu_map.py` calls arm9 `main`.** Keying on `arm9` silently drops 3,067 functions.

## Attributing non-`.text` sections

If you extend a TU claim to `.rodata`/`.data`/`.bss` (for a TU-granular `delinks.txt`),
the only sound rule is: **object *k*'s contribution to every section precedes object
*k+1*'s.** Discard any rule whose per-TU intervals are not ascending and disjoint.

Do **not** use "TU *k*'s code loads this address, so TU *k* owns it" — that direction
is what produced the impossible ordering above. Follow only pointers a run *contains*,
then close transitively over referrers (`_ZTV` -> `_ZTI` -> `_ZTS`). Expect to leave
some sections unattributed and say so: ov045's `.rodata` is 0/840 because its code
never references its own `.rodata` at all.

## C++ destructors and the vtable anchor

`tools/objisolate.py` reduces a compiled object to the one function its delink entry
declares, so a `.cpp` destructor -- which mwcc emits as D0/D1/D2 plus the class
vtable and RTTI -- can link. `notes/objisolate.md` measures 99 of 105 `.cpp` destructor
files **eligible** (up from 30) — read that column as eligible, not enrolled — and is
required reading before touching it; in particular a `_ZTV*` relocation's
addend must lose 8 on rebinding, because mwcc's vtable symbol addresses the object's
start while symbols.txt's addresses the slot array. Getting that wrong links clean
and corrupts 34 modules.

**mwcc anchors the vtable to the TU that DEFINES the destructor out of line** — which
is why a vtable is also a `.data` anchor for its TU, and why moving that definition
inline removes the anchor entirely. The full source-form rules are in
`decomp-cpp-class-form`.

## After changing tu_map.py

`python tools/tu_map.py --check` must stay green — but **read the output, never the exit
status.** On `main` it prints `[PASS]`/`[FAIL]` per gate and returns; the exit code is
**0 either way**. `--check --blind` fails two gates and still exits 0. `--check` also
does not rewrite `build/tu_map.json`.

The gates on `main` are V1 (known-answer classed-TU counts for ov062/ov063/ov080/ov020),
V1b (`daNknk_c_classInit_NOKONOKO_S` lands inside the Koopa TU), V2a (sinit count == `.ctor` entry
count), V2b (no module has more sinits than TUs) and V3 (every function in exactly one
TU), followed by the blind negative-control table. V2b earns its keep — it caught the
RTTI-bridge bug.

**There is no partition gate.** Nothing asserts that a module's units are disjoint, and
the JSON carries no `meta.partition_defects` — `meta` is exactly `modules`, `total_tus`,
`total_functions`, `total_sinits`, `boundaries`, `tus_with_class`, `under_segmented`,
`caveat`. Today's map has 8 overlapping unit pairs (the alias defect above) and 85 gaps,
and nothing reports either. If such a gate is ever added it must **not** assert that
every function lies inside its unit's range: 109 functions across 10 modules violate
that on purpose, because `absorb_unlabelled` attaches call-graph-proven file-local
functions without widening the cluster span. Nor should it forbid gaps — the 85 are
legitimate.

The map does **not** merge files. Consolidation is a separate byte-gated change:
a merged TU must emit functions in exactly the ROM's order (mwccarm emits one `.text`
section per function in the REVERSE of source order), `notes/mwccarm-codegen.md`
records that `#pragma opt_*` TUs cannot be merged as-is, and `rombuild.py` is the
verdict — not `build_pin`.
