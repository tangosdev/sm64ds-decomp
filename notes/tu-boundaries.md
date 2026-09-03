# Recovering translation-unit boundaries

`tools/tu_map.py` → `build/tu_map.json`. What it knows, how it knows it, and where
it stops knowing.

## Why

`src/` holds 11,122 one-function files. The original build had nothing of the sort:
it had `.cpp` files, and the linker emitted each one's `.text` as a single contiguous
run. That structure is still legible in the ROM, and three workstreams want it:

* **The ctor/dtor endgame is blocked on it.** `tools/eligible.py:135` rejects a file
  with `"N .text sections (multi-function TU)"`, and per `notes/plan-cpp-language-mode.md`
  all 72 compiling D1 destructors and 3 D0s are rejected that way — **zero enrolled**,
  ~517 ctor/dtor functions stranded. Enrolling them needs per-TU objects carrying
  per-TU `.data` (the vtables), which needs to know what a TU is.
* **Per-class PR slicing is wrong-grained where classes co-reside.** MontyMole and
  MontyMoleRock share a file; shipping them in separate PRs invents a structure the
  ROM contradicts.
* **`static` decisions during langmode conversion** want the file-locality evidence.

This is *not* the file-consolidation question that was measured and declined on
2026-08-02 (see `[[decomp-file-consolidation]]`). That was about build time. This is
about structure, and the map does not merge anything.

## The trap that makes the naive detector look correct

Cut wherever the class name changes, and ov062 scores 5/5. Chuckya, Koopa,
KoopaTheQuick, daRFlag_c and Klepto really do sit in five contiguous runs.

ov062 is the lucky case. Where two classes share a file, mwcc emits their methods
**interleaved in source order**, not grouped by class:

```sh
_ZN9MontyMoleD1Ev                 _ZN9MontyMoleD0Ev
_ZN13MontyMoleRockD1Ev            _ZN13MontyMoleRockD0Ev
_ZN9MontyMole16CleanupResourcesEv _ZN13MontyMoleRock16CleanupResourcesEv
_ZN9MontyMole6RenderEv            _ZN13MontyMoleRock6RenderEv
_ZN9MontyMole8BehaviorEv          _ZN13MontyMoleRock8BehaviorEv
MontyMoleRock_Spawn               MontyMole_Spawn
```

A name-change detector turns ov080's three TUs into thirteen. ov020
(BookShot/BookShotSpawner) and ov063 (Boo/BooCage/BigBooIcon) interleave the same way.

## The rule that works

Treat a class as an **interval**, not a block:

> span(A) overlaps span(B) ⟹ A and B share a translation unit

Forced by the linker, not a heuristic: a TU is contiguous, so if two classes were
separate objects one object's run would have to sit inside the other's. Union-find
over that relation collapses MontyMole+MontyMoleRock automatically and leaves Chuckya
and Koopa apart, with no "how interleaved is too interleaved" threshold to tune.

## Two label sources, and why they are not equal

* **Mangled names** via `srcpath.class_of` (which already encodes the outer-component
  and free-function traps). Direct evidence about the function carrying the name.
* **Vtable slots** via `build/rtti_vtables.json`. RTTI carries EAD's real class names
  (`daChoropu_c` = Monty Mole, `daJango_c` = Klepto), so it labels code in overlays
  where no symbol was ever named — ov006 has 1866 functions and 38 named ones.
  `notes/dtor-variant-audit.md` records that mwcc emits the vtable group into the TU
  defining the destructor, so a vtable also anchors its TU in `.data`.

**RTTI may extend or create a cluster; it may never bridge two symbol clusters.**
Gate V2b found this rather than foresight. ov081:

```sh
MrBlizzard    0x2124090-0x2125f14
Snowball      0x2125f14-0x2126504   abuts exactly — two TUs
daSnowman_c   0x2125eb8-0x21261d4   RTTI span straddles the boundary
```

The naive union merged them. The sinit count adjudicates: ov081 has five named classes
and five sinits, so five TUs is right. ov090 fails identically (`daMenbo_c` bridging
MantaRay and Skeeter; four classes, four sinits). An RTTI span is *inferred* from a
slot list, and a slot can point at inherited or shared code in a neighbouring object;
a mangled name cannot be wrong about its own function. Hence the asymmetry.

## Sinits are a count, not a cut

`.ctor` entry count == `__sinit_*` symbol count across **all 106 modules, zero
exceptions**, with ctor targets always ascending and inside `.init`. But a sinit lives
in `.init`, nowhere near the `.text` it belongs to, so it cannot place a cut. It is
used as an independent witness (`corroborated: sinits == tus`) and as gate V2b, never
as a signal — which is what let it catch the RTTI bridge instead of rationalising it.

Only 67 of the 104 `.ctor` sections are non-empty and only 52 modules have ≥2 sinits,
so a third of code-bearing modules get no witness at all.

## What it produces

74 modules with `.text` (the other 31 overlays are data-only — no TU problem exists
there), 11,088 functions → **501 TUs**. 388 carry a class. Boundaries: **270 high,
107 medium, 50 low**. **30 of 74 modules corroborated exactly** by sinit count.

## Where it stops

Under-segmentation is the characteristic failure and it is *silent*: a module with no
class labels collapses into a few enormous contiguous runs and reports a TU count as
confidently as ov062 does. `meta.under_segmented` names them — **main** (3066 functions
in 25 runs; the real answer is certainly in the hundreds), **ov007** (548 functions,
*zero* sinits, so no witness is even possible), **ov006**, ov075, ov004, ov084. For
those the TU count is a lower bound and should be read as one.

The flag deliberately requires *both* coarseness and a missing witness: ov063 packs
141 functions into 4 TUs and is right, because its 4 sinits corroborate exactly.

Boundaries marked `low` rest on contiguity alone, which cannot distinguish two classes
in one file from two files that linked adjacently. That is the fundamental limit, not
an implementation gap.

## The negative control

Every gate above scores on overlays whose symbols are named, but the map is worth most
where they are not. `--check` re-runs the known answers with mangled names stripped
(`--blind`), leaving only RTTI:

| module | known | sighted | blind | blind-classed |
|---|---|---|---|---|
| ov062 | 5 | 5 | 10 | **5** |
| ov063 | 4 | 4 | 9 | **4** |
| ov080 | 3 | 4 | 6 | **3** |
| ov020 | 2 | 2 | 4 | **2** |

Read it as: **RTTI alone recovers the class clusters perfectly** — blind-classed hits
the known answer in all four. What it loses is the non-polymorphic remainder, which
fragments into orphan runs and roughly doubles the raw TU count. So in an anonymous
overlay, trust a TU that carries a class name and treat the unattributed runs between
them as unresolved rather than as TUs.

## Usage

```sh
python tools/tu_map.py                    # all modules -> build/tu_map.json
python tools/tu_map.py --module ov062     # one module, printed
python tools/tu_map.py --module ov080 --verbose  # with per-function detail
python tools/tu_map.py --check            # gates V1-V3 + the negative control
```

## Not done here

Merging files. A merged TU must emit its functions in exactly the ROM's order, and
`notes/mwccarm-codegen.md` records that TUs carrying `#pragma opt_*` cannot be merged
as-is. `rombuild.py` is the verdict on any such change, not this map.
