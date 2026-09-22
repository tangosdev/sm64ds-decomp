# Recovering translation-unit boundaries

`tools/tu_map.py` → `build/tu_map.json`. What it knows, how it knows it, and where
it stops knowing.

This note preserves the boundary method and its historical measurements. Current
execution and completion rules live in [the promotion workflow](tu-promotion-conventions.md).
Refresh the relevant map and tool behavior; older counts and blockers below are
not a live work queue or instructions to repeat staging phases.

## Why

`src/` holds 11,122 one-function files. The original build had nothing of the sort:
it had `.cpp` files, and the linker emitted each one's `.text` as a single contiguous
run. That structure is still legible in the ROM, and three workstreams want it:

* **Lifecycle ownership needs TU evidence.** Early per-function enrollment could
  not retain some multi-function destructor output. Production TU support now
  exists; measure the candidate's emitted code/data and current tool restrictions
  instead of treating the old zero-enrollment census as a current blocker.
* **A production TU can contain several classes.** MontyMole and MontyMoleRock
  provide a co-residence example. Preserve the evidenced boundary for promotion;
  an explicitly scoped method conversion or fix can still ship independently.
* **`static` decisions during langmode conversion** want the file-locality evidence.

This is *not* the file-consolidation question that was measured and declined on
2026-08-02 (see `[[decomp-file-consolidation]]`). That was about build time. This is
about structure, and the map does not merge anything.

## The trap that makes the naive detector look correct

Cut wherever the class name changes, and [ov062](../config/arm9/overlays/ov062/symbols.txt) scores 5/5. daHolhei_c, Koopa,
daRNk_c, KoopaFlag and Klepto really do sit in five contiguous runs.

[ov062](../config/arm9/overlays/ov062/symbols.txt) is the lucky case. Where two classes share a file, mwcc emits their methods
**interleaved in source order**, not grouped by class:

```sh
_ZN9MontyMoleD1Ev                 _ZN9MontyMoleD0Ev
_ZN13MontyMoleRockD1Ev            _ZN13MontyMoleRockD0Ev
_ZN9MontyMole16CleanupResourcesEv _ZN13MontyMoleRock16CleanupResourcesEv
_ZN9MontyMole6RenderEv            _ZN13MontyMoleRock6RenderEv
_ZN9MontyMole8BehaviorEv          _ZN13MontyMoleRock8BehaviorEv
MontyMoleRock_Spawn               MontyMole_Spawn
```

A name-change detector turns [ov080](../config/arm9/overlays/ov080/symbols.txt)'s three TUs into thirteen. [ov020](../config/arm9/overlays/ov020/symbols.txt)
(BookShot/BookShotSpawner) and [ov063](../config/arm9/overlays/ov063/symbols.txt) (Boo/BooCage/BigBooIcon) interleave the same way.

## The rule that works

Treat a class as an **interval**, not a block:

> span(A) overlaps span(B) ⟹ A and B share a translation unit

Forced by the linker, not a heuristic: a TU is contiguous, so if two classes were
separate objects one object's run would have to sit inside the other's. Union-find
over that relation collapses MontyMole+MontyMoleRock automatically and leaves daHolhei_c
and Koopa apart, with no "how interleaved is too interleaved" threshold to tune.

## Two label sources, and why they are not equal

* **Mangled names** via `srcpath.class_of` (which already encodes the outer-component
  and free-function traps). Direct evidence about the function carrying the name.
* **Vtable slots** via `build/rtti_vtables.json`. RTTI carries EAD's real class names
  (`daChoropu_c` = Monty Mole, `daJango_c` = Klepto), so it labels code in overlays
  where no symbol was ever named — [ov006](../config/arm9/overlays/ov006/symbols.txt) has 1866 functions and 38 named ones.
  `notes/dtor-variant-audit.md` records that mwcc emits the vtable group into the TU
  defining the destructor, so a vtable also anchors its TU in `.data`.

**RTTI may extend or create a cluster; it may never bridge two symbol clusters.**
Gate V2b found this rather than foresight. [ov081](../config/arm9/overlays/ov081/symbols.txt):
```sh
MrBlizzard    0x2124090-0x2125f14
Snowball      0x2125f14-0x2126504   abuts exactly — two TUs
daSnowman_c   0x2125eb8-0x21261d4   RTTI span straddles the boundary
```

The naive union merged them. The sinit count adjudicates: [ov081](../config/arm9/overlays/ov081/symbols.txt) has five named classes
and five sinits, so five TUs is right. [ov090](../config/arm9/overlays/ov090/symbols.txt) fails identically (`daMenbo_c` bridging
daManta_c and daMenbo_c; four classes, four sinits). An RTTI span is *inferred* from a
slot list, and a slot can point at inherited or shared code in a neighbouring object;
configured class labels are a different input from slot reachability. Those
labels may themselves be inferred; preserve their provenance rather than treating
a coined mangling as original ROM text.

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
confidently as [ov062](../config/arm9/overlays/ov062/symbols.txt) does. `meta.under_segmented` names them — **main** (3066 functions
in 25 runs; the real answer is certainly in the hundreds), **[ov007](../config/arm9/overlays/ov007/symbols.txt)** (548 functions,
*zero* sinits, so no witness is even possible), **[ov006](../config/arm9/overlays/ov006/symbols.txt)**, [ov075](../config/arm9/overlays/ov075/symbols.txt), [ov004](../config/arm9/overlays/ov004/symbols.txt), [ov084](../config/arm9/overlays/ov084/symbols.txt). For
those the TU count is a lower bound and should be read as one.

The flag deliberately requires *both* coarseness and a missing witness: [ov063](../config/arm9/overlays/ov063/symbols.txt) packs
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
| [ov062](../config/arm9/overlays/ov062/symbols.txt) | 5 | 5 | 10 | **5** |
| [ov063](../config/arm9/overlays/ov063/symbols.txt) | 4 | 4 | 9 | **4** |
| [ov080](../config/arm9/overlays/ov080/symbols.txt) | 3 | 4 | 6 | **3** |
| [ov020](../config/arm9/overlays/ov020/symbols.txt) | 2 | 2 | 4 | **2** |

In that snapshot, the blind-classed counts agreed with the recorded known answers.
A later map used four classed TUs for ov080 and recovered only three without names.
The negative control is calibration, not a guarantee: unnamed runs and incomplete
labels remain unresolved, and raw map counts need their measured confidence.

## Usage and map provenance

When generating or refreshing a map, prepare its inputs in order:

```sh
python tools/rtti_extract.py
python tools/rtti_vtables.py
python tools/tu_map.py                    # all modules -> build/tu_map.json
python tools/tu_map.py --module ov062     # one module, printed
python tools/tu_map.py --module ov080 --verbose
python tools/tu_map.py --check            # diagnostics and negative control
```

Current `tu_map.py` rejects missing or stale vtable inputs. Verify provenance when
adopting an existing map; it can outlive the inputs it describes. `--check` does
not regenerate the JSON and printed failures do not produce a failing exit status.
Read the diagnostics, including the negative control, before relying on a boundary.

`corroborated` means the sinit count independently supports the cut count; it does
not certify every class label or function assignment. A low-confidence boundary
needs further evidence. Reconcile co-resident classes with the task's reservations
before editing: inferred TU extent does not expand the assigned scope by itself.

## Inventory and ownership pitfalls

These observations incorporate the technical material formerly called
`decomp-tu-slicing`; older manifest/fact notes retain that historical name.
Use the current map and symbols to remeasure a candidate rather than copying counts.

- A text-based map cannot see a data-only object, even inside a module that also
  contains code. A claimed 2,128-byte data-only band in ov045 was an unconfirmed
  investigation lead, not a license to assign those bytes to a neighboring code TU.
- Some zero-size function symbols are aliases at a sized function's address.
  Historical `main`/`itcm` maps counted eight such aliases as overlapping runs.
  Inspect aliases by `(module, address)` and retain the real sized contribution;
  do not invent another four-byte function or assume every zero size is an alias.
- Read the function inventory from `symbols.txt`. `delinks.txt` describes current
  build ownership and can be repartitioned; it is not the original file boundary.
- Key addresses by module. Overlays can share load addresses, and this map names
  ARM9 `main`, not `arm9`.
- Join files through address, symbol and `units[].functions`, not their filename
  or containment in a TU interval. `absorb_unlabelled` can attach proven file-local
  helpers without widening the anchor span. An earlier map had 109 such functions
  outside their units' start/end, alongside eight alias overlaps and 85 gaps.
  Check disjoint actual contributions; do not impose interval containment or
  forbid every gap as a shortcut.
- A code load from an address does not establish which TU owns that data. For an
  evidenced linker object order, check ordered, disjoint section contributions
  and follow contained pointers through vtable, typeinfo and type-name records.
  Some ownership will remain unresolved. The earlier ov045 experiment left all
  840 `.rodata` bytes unattributed; that is a limit, not proof of absent data.

RTTI slots can point to inherited or shared code in another TU, as the bridging
examples above show. For lifecycle output, see [class-form observations](cpp-class-form.md)
and [object isolation](objisolate.md): isolation can make a function eligible while
discarding unreviewed vtable/RTTI data. Eligibility, default enrollment and complete
emitted-output proof remain distinct.

## Not done here

Merging files. Whole-object placement needs the emitted contributions in the
required ROM order; a verified derived-object partition can restore linker order
from a single compiler input. Neither follows from this map. File-global pragmas
can also change codegen when sources combine; see [the codegen notes](mwccarm-codegen.md).
The [promotion workflow](tu-promotion-conventions.md) distinguishes experimental
partitioning from supported default-build enrollment and defines the final proof.

## Worked example: a class is not a TU (ov065, `daDossyCap_c`)

`daDossyCap_c` looks like a clean fold candidate from outside the map: 6 loose shards in
[ov065](../config/arm9/overlays/ov065/symbols.txt), its own `include/daDossyCap_c.h`, a full
RTTI triple homed in its own overlay (`_ZTI12daDossyCap_c` 0x0211cd4c, `_ZTS12daDossyCap_c`
0x0211cd58, `_ZTV12daDossyCap_c` 0x0211cdc4), and a vtable that claims exactly those 6 and no
more. `tubuild` refuses it — `no candidate TU 'ov065/daDossyCap_c' in build/tu_map.json` — and
the refusal is correct.

The committed delinks settle it with no tool at all:

| address | function |
|---|---|
| 0x02118d80 | `_ZN9daDossy_c6RenderEv` |
| 0x02118da8 | `_ZN12daDossyCap_c6RenderEv` |
| 0x02118df0 | `_ZN9daDossy_c8BehaviorEv` |
| 0x021190a8 | `_ZN12daDossyCap_c8BehaviorEv` |
| 0x02119228 | `_ZN9daDossy_c13InitResourcesEv` |
| 0x021194e8 | `_ZN12daDossyCap_c13InitResourcesEv` |

Every `daDossyCap_c` member sits strictly between two `daDossy_c` members, with no gap on either
side. For `daDossyCap_c` to be its own object the linker would have had to split `daDossy_c`'s
`.text` in half around it — the one thing the contiguity rule this whole map rests on says
never happens. The two `classInit` bodies interleave the same way (`daDossyCap_c_classInit`
0x021195ec, then `daDossy_c_classInit` 0x02119634, then `_ZN14DorriePlatformC1Ev` 0x021196bc),
so `daDossyCap_c_classInit` is inside the unit but so is a second class's.

The unit is 0x02117f40..0x021196d8 — 29 functions, 3 classes — which is what
`notes/data/tu-promotion-queue.tsv` already calls `daDossy_c+DorriePlatform+daDossyCap_c`.

Two blockers on that unit, independent of each other:

* **`DorriePlatform` is still a coined name.** The ROM's own RTTI spells the main class
  `daDossy_c` (`_ZTS9daDossy_c` 0x0211cd34, `_ZTI9daDossy_c` 0x0211cd40, vtable 0x0211ce48),
  and that rename has since landed: the overlay's symbols and
  `notes/data/tu-promotion-queue.tsv` both spell it `daDossy_c` now, so it is no longer a
  prerequisite. `DorriePlatform` has no RTTI at all, so there is no ROM spelling to rename
  it to, and it still blocks the fold.
* **The generated shadow does not compile.** `tubuild create` on the real candidate emits 17
  human-review items — 16 conflicting `extern` declarations plus one body it cannot split at
  all, because `src/func_ov065_021182e4.cpp` defines it inside an `extern "C"` block — and the
  result fails under 2004/b56 on a redefined `dBgW` and a redeclared `data_ov065_0211c080`.
  Five `#pragma` directives survive in the legacy sources on top of that.

The general lesson: a complete, correctly-homed RTTI triple says the *name* is real. It says
nothing about where the *file* boundary falls. Check the span for overlap before scoping a fold.
