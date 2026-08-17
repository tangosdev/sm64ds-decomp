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

```
python tools/rtti_extract.py     # -> build/rtti.json
python tools/rtti_vtables.py     # -> build/rtti_vtables.json
python tools/tu_map.py           # -> build/tu_map.json
```

`build/` is gitignored, so a fresh worktree has none of this.

**This is now enforced.** `tu_map.py` fails closed (exit 1) when `rtti_vtables.json` is
missing, older than `rtti.json`, or inconsistent with it (class/vtable-address/module
cross-check, which catches a file built from a different commit where mtime looks fine).
`--no-rtti` is the only way past; it is loud, recorded as `meta.rtti_labels`, and
**refused under `--check`**. `tubuild.py` inherits this, since it shells out to
`tu_map.py` when the map is absent.

It is worth knowing what that gate prevents, because the failure was invisible:
`vtable_labels()` used to return `{}` silently, producing a complete, self-consistent,
**wrong** map — 516 TUs instead of 532 on the pre-fix code, with **all gates passing**
(ov080 read 6 TUs instead of 5 and still had 4 classed, so the known-answer gate passed
for the wrong reason). The under-segmented list was itself a casualty, so you could not
use it to detect the problem either.

Current whole-ROM figures: **74 modules, 11,083 functions, 520 TUs, 305 sinits**,
boundaries `{low: 56, medium: 110, high: 280}`, 400/520 carrying a class.

## The one rule that matters

**A class is not a file.** Where two classes shared a `.cpp`, mwcc emitted their
methods interleaved in source order, so they look like many alternating blocks and are
in fact one TU. Never slice on "the class name changed."

```
python tools/tu_map.py --module ov080
  0x2123740-0x2124a20  26 fn  MontyMole, MontyMoleRock, daChoropu_c   <- ONE TU
  0x2124a20-0x2125404  21 fn  CrazedCrate, daBttBk_c
  0x2125404-0x212714c  35 fn  Painting, daPicGate_c
  0x212714c-0x212766c   4 fn  (unattributed)
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

In an unnamed overlay, a TU carrying a class name is trustworthy — the negative
control shows RTTI alone recovers class clusters exactly — but the unattributed runs
between them are *unresolved*, not TUs. Do not report a raw TU count from an
under-segmented module as if it were the file count.

## Gotchas that have already bitten

* **A module can contain an object with NO `.text` at all.** ov045 has a seventh,
  data-only object — 2,128 bytes at the head of `.data`, read by three TUs'
  `InitResources`. `tu_map` is built from `.text` runs and **structurally cannot see
  it**, so "N TUs" is a count of code-bearing objects, not of original files. It was
  found only because attributing `.data` by "whose code loads it" produced a TU
  ordering the linker's layout rule forbids. Distinct from the 31 whole modules that
  are data-only.
* **A size-0 function symbol is an ALIAS, not a function.** All 8 in the corpus are a
  second `kind:function` symbol at an address a *sized* one already occupies — the ARM
  runtime exporting both its mwcc and AEABI name from one entry point
  (`__aeabi_uidiv`/`_u32_div_f`, `_dadd`, `_ll_sdiv`, …) plus `__destroy_arr`/
  `__cxa_vec_cleanup`. They are not unrecorded sizes: no size-0 symbol is alone at its
  address and no address carries two sized ones, so the extent is never ambiguous.
  **Fixed** — `functions()` now collapses aliases (one entry per address, survivor is
  the sized symbol). Before the fix these fragmented real runs into overlapping 4-byte
  units and `main`/`itcm` were not partitions. If you inventory symbols yourself, collapse
  by address or you will reintroduce it.
* **RTTI spans can bridge two real TUs.** A vtable slot may point at inherited or
  shared code in the neighbouring object. `tu_map.py` already suppresses this
  (mangled names win over RTTI spans), but if you compute spans yourself, don't
  re-introduce it — ov081's `daSnowman_c` and ov090's `daMenbo_c` are the cases.
* **Inventory functions from `symbols.txt`, not `delinks.txt`.** delinks can leave a
  hole: ov062 has no entry covering `0x02117724-0x02117994` yet
  `func_ov062_02117724` sits right there inside the Koopa run.
* **Key by (module, addr), never addr alone.** 52 overlays share base `0x021111A0`.
* **Join files to TUs by address -> symbol -> `units[].functions`**, never by file stem
  (renames) nor by TU interval — 212 functions sit outside their own TU's start/end.
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
vtable and RTTI -- can link. 99 of 105 `.cpp` destructor files are source-built.
Read `notes/objisolate.md` before touching it; in particular a `_ZTV*` relocation's
addend must lose 8 on rebinding, because mwcc's vtable symbol addresses the object's
start while symbols.txt's addresses the slot array. Getting that wrong links clean
and corrupts 34 modules.

**mwcc anchors the vtable to the TU that DEFINES the destructor out of line** — which
is why a vtable is also a `.data` anchor for its TU, and why moving that definition
inline removes the anchor entirely. The full source-form rules are in
`decomp-cpp-class-form`.

## After changing tu_map.py

`python tools/tu_map.py --check` must stay green — and it now **exits non-zero** on
failure, which it previously did not (it only printed). The gates are
ov062/ov063/ov080/ov020 known answers, `KoopaSmall_Spawn` landing inside the Koopa TU,
sinit==ctor counts, no module with more sinits than TUs, every function in exactly one
TU, and **V4: units partition their module — no overlap, no zero-length or empty unit**
(also recorded as `meta.partition_defects` for consumers reading the JSON off disk).
V2b earns its keep — it caught the RTTI-bridge bug. V4 earns its keep — applied to the
pre-fix map it reports 8 defects.

V4 deliberately does **not** assert that every function lies inside its unit's range:
109 functions across 10 modules violate that on purpose, because `absorb_unlabelled`
attaches call-graph-proven file-local functions without widening the cluster span. Nor
does it forbid gaps — 85 exist legitimately.

The map does **not** merge files. Consolidation is a separate byte-gated change:
a merged TU must emit functions in exactly the ROM's order (mwccarm emits one `.text`
section per function in the REVERSE of source order), `notes/mwccarm-codegen.md`
records that `#pragma opt_*` TUs cannot be merged as-is, and `rombuild.py` is the
verdict — not `build_pin`.
