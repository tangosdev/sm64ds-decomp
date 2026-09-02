# Handoff: the class skeleton, and how much of it is proven

**Written 2026-08-07.** Branch `docs/class-reference-confidence`, five commits, pushed to
`origin`. Everything below is checkable; every number appears with the command that
produces it. Where an earlier answer was wrong, it is recorded rather than quietly fixed.

---

## 0. Read this first if you are picking it up cold

The question this answers is *"can this ROM be turned into matchable C++, or is that
impossible?"* The short version:

**The interface is solved. The fields are not.** Those are different problems with
different oracles, and conflating them is the mistake this work exists to prevent.

* **Fields are a weak oracle.** `notes/runbook-type-reconstruction.md` §2: a retype that
  nothing reads emits identical bytes. 71 retypes across 119 TUs came out byte-identical
  under `s32/s16` *and* `u32/u16`. The ROM cannot adjudicate most type questions.
* **The skeleton is a strong oracle.** Inheritance and virtual dispatch are not inferred
  from code -- the compiler emits them as **data**, and that data is in the ROM. So a
  C++ declaration can be compiled and byte-compared against retail.

```sh
python tools/rtti_extract.py --report --check    # -> build/rtti.json
python tools/rtti_vtables.py                     # -> build/rtti_vtables.json
python tools/rtti_reference.py                   # -> docs/class-reference.html
python tools/skeleton_conform.py --report        # -> build/skeleton_conform.json
```

## 1. What is proven, with numbers

`python tools/rtti_reference.py` prints these and writes the browsable page:

| claim | count | of | | what makes it true |
|---|---:|---:|---|---|
| class exists and is named | 429 | 429 | 100% | the ROM stores the name string |
| base class known | 413 | 413 | 100% | a pointer in the typeinfo record |
| vtable located | 426 | 429 | 99.3% | bound to the record |
| slot resolves to a named function | 10984 | 11021 | 99.7% | slot → `config/**/symbols.txt` |
| …and that function has a source file | 10973 | 11021 | 99.6% | `tools/srcpath.py` |
| …and that file is **enrolled** | 10324 | 11021 | 93.7% | linked; rebuilt ROM byte-identical to retail |

`python tools/skeleton_conform.py --report`:

```python
checked 423: conform=420, bases_only=3
skipped 6: no vtable slots (dBgPc, dM3dGLin, dMg3DHeyhoObjAdapter_c, daDemo_c::param_c)
```

**420 of 429 classes reproduce their vtable shape, offset-to-top, pure-virtual slots and
mangled name string from a compiled C++ declaration.** Zero mismatches.

### What "conform" does and does not claim

Compared **exactly**: the `_ZTS` name string, vtable offset-to-top, table length, which
slots are null, and for multiple inheritance the flags, base count and every base's
`offset_flags`. Those are literals.

**Wildcarded**: every pointer word. In an object file it is an addend against an
undefined symbol; in the ROM it is a resolved address. `tools/match.py` has the identical
problem for code and solves it the same way. Whether the pointer words hold the *right*
functions is answered by a different join -- the 99.7%/93.7% rows above.

Not claimed at all: field names, types, offsets, or method names. A named slot is not a
named method: where the symbol is a `func_*` placeholder the *position* is right and the
name is still unknown, and a vtable never carries a signature.

## 2. The multiple-inheritance payload -- the ROM's only direct `sizeof`

A `__vmi_class_type_info` record stores where each base **subobject** starts, as a
literal. Under Itanium a non-virtual base sits at the next available offset, so
consecutive starts state how much room the earlier base occupies. This is the one place
the ROM states layout outright.

```sh
dBgCh_SphCrr (SphereClsn)   dBgCh@0   dBgPi@16   dM3dGSph@56
dBgCh_Lin    (RaycastLine)  dBgCh@0   dBgPi@16   dM3dGLin@56
dBgCh_Gnd                   dBgCh@0   dBgPi@16
dExtAnmModel_c              dExtSimpleModel_c@0   dExtFrameCtrl_c@80
daDemo_c::anmModel_c        dExtAnmModel_c@0      daDemo_c::param_c@100
daDemo_c::simpleModel_c     dExtSimpleModel_c@0   daDemo_c::param_c@80

implied:  sizeof(dBgCh) 16   dBgPi 40   dExtSimpleModel_c 80   dExtAnmModel_c 100
```

Each is corroborated by two or three independent records; none conflicts.

**Two confirmations that were not inputs.** `include/dBgCh.h`, hand-reconstructed from the
constructor's stores, says *"LAYOUT is 0x10 bytes"* -- 16, derived here from a different
structure entirely. And `include/dBgCh_SphCrr.h`, a generated flat header, declares fields
starting at `0x010` and `0x038`; the record puts dBgPi at 16 and dM3dGSph at 56, and
16 + 40 = 56. **Both subobject boundaries land exactly on a field that generator already
found**, knowing nothing about any of this.

> **If you are matching `DetectClsn(SphereClsn&)`:** everything below `0x38` in
> `include/dBgCh_SphCrr.h` belongs to a base, not to SphereClsn. The ROM says that class
> has three bases; `include/` models one. See `notes/rtti-reconciliation.md` §4.

**Honest about the circularity.** Taking offsets from the ROM and checking they come back
is close to vacuous. Two things make it not: the compiler gets a vote (declaration order,
alignment and the primary-base choice can each refuse), and the useful output is a
*restatement* -- "dBgPi starts at 16" becomes "sizeof(dBgCh) == 16", which is the form a
matcher can use.

## 3. Two defects this found in `build/rtti_vtables.json`

Both are fixed. Both were found by **using** the data rather than inspecting it, which is
the transferable part.

**Over-read, 62 classes, 1128 slots.** The end-of-table rule only fired on a null whose
successor is a *known typeinfo record*. Where a `{0, handler}` table follows instead, the
successor is a code pointer, nothing fires, and the walk reads that table as slots.
`daPeach_c` ran to 62 slots against a real 31.

The fix trims the longest strictly-alternating null/pointer suffix. Orientation is not the
signal -- `daPeach_c`'s tail reads `{fn, 0}` and `daObjSwdoor_c`'s reads `{0, fn}` -- the
*run* is. Both then land on their base's count exactly, which nobody put in by hand.

**Under-read, 124 classes.** "Is this word code" was asked of the vtable's own module plus
arm9. Too narrow twice: ITCM is its own module and was never loaded (`dBgW_Kc` slots 3-7
are `0x01ffd920`…), and a slot may point into a **different overlay** (`dScMgAmida_c`'s table is in [ov006](../config/arm9/overlays/ov006/symbols.txt) while slot 1 is [ov004](../config/arm9/overlays/ov004/symbols.txt)'s code). Asking *every* module fails the other way -- `dScStage_c` ran to 70 slots against a base of 18. The bound is residency (`overlay_residency.conflict`, rule E2, the game's own `LoadOverlay` panic), applied
**one-directionally**: arm9 conflicts with nothing, so an arm9-hosted table gets no cross-overlay allowance or the rule bounds nothing.

**The invariant is now a gate.** A derived table is never shorter than its base's -- every
base slot is inherited or overridden and neither removes one. `rtti_vtables.py` fails the
run if it is violated. That invariant is what made both bugs findable.

### The correction that followed

`include/daObjSwdoor_c.h` declared four fields read from twelve "own" methods. Nine of the
twelve were the over-read tail. §8's attribution argument -- *a method can reach its own
members and its ancestors', never a descendant's* -- only licenses a field if the method
really is the class's, so **all four fields fell**: 3 own methods, 0 own fields. The header
is retracted, and `--emit-headers` now deletes a header it previously wrote for a class
with no remaining evidence.

**The other four headers regenerate byte-identically** (4, 6, 4, 4 methods; same offsets,
same widths). A trim eating real data would have moved them too. That is the reason to
believe the fix and not merely the finding.

## 4. What is still open, in the order I would take it

1. **The 37 unnamed slots** are addresses hosted by two co-resident modules. The join
   reports a genuine tie as unresolved rather than picking one. `tools/overlay_residency.py`
   `settle()` may narrow some; the rest want a human. Low value, listed for completeness.
2. **`daDemo_c::anmModel_c` / `simpleModel_c`** report `bases_only`: they declare their
   offset-100 / offset-80 base first and the primary second, which happens when the
   first-declared base is not the one the compiler makes primary. A probe needs the
   empty-base and primary-base rules modelled. Their offsets are already recorded.
3. **`dBgCh_Lin` needs `dM3dGLin`'s vtable.** It is one of three records with no vtable
   bound (`dBgPc`, `dM3dGLin`, `daDemo_c::param_c`); `rtti_vtables.py` refuses to guess.
   Settling any of them unblocks a probe and would confirm `dBgPi`'s size a third time.
4. **Surface conformance on `docs/class-reference.html`.** The data is in
   `build/skeleton_conform.json` and the page does not read it yet. Cheap; it would let
   the page state both "the code is byte-verified" and "the declaration is byte-verified".
5. **The real blocker for migration is unchanged and is not a codegen problem.**
   `eligible.py` rejects any object with a `.data` section (85 files) and any with
   multiple `.text` sections (13). A `~Class()` definition always emits D2/D0/D1, so
   **enrolled destructor migrations in this tree: zero.** The prerequisite is a delink
   model that can bind one source file to a multi-function address range. mwccarm emits
   the group in D2, D0, D1 order, which is exactly the ROM's per-class layout -- one TU
   already reproduces the contiguous range; nothing can bind it. See
   `notes/dtor-variant-audit.md` §7.

## 5. Wrong answers recorded, so they are not re-derived

* **"9351 / 9351 slots named, 100.0%"** was an artifact, not an achievement. The walk
  stopped early at precisely the slots it could not classify, so they were never in the
  denominator. The corrected figure is 10984 / 11021 -- a lower percentage over 1670 more
  slots. **A suspiciously perfect number is a reason to check the denominator.**
* **Trimming the over-read tail** was tried before refusing it, and the two orientations
  disagree about which slot is last. Refusing was what dropped mismatches 43 → 0, which is
  itself the evidence that the mismatches were bad slot data rather than a bad model.
* **`content-visibility: auto`** on the class page makes the browser guess card heights,
  so a jump from the tree lands in blank space and the card never paints. Removed. The
  weight it was there to fix is handled by building slot tables lazily (9351 rows as
  markup is 2.8 MB and froze the renderer) and shipping groups collapsed (69,000px → 8,201px).
* **`skeleton_conform` reported "the probe emitted no `_ZTS`" for 57 classes** when the
  probe was fine and its own ROM reader was the broken half -- a fixed 256-byte read
  overran the end of the module. Always say *which side* of a comparison is missing.
* **Symbol lookups by prefix/suffix.** A probe object defines every ancestor's RTTI
  symbols too, so a scan can return an ancestor's record and compare it against the leaf's
  ROM bytes. Index by exact mangled name.

## 6. Caveats on the artifacts

`docs/class-reference.html` is generated and committed. It was generated from a working
tree on `d2/renames-small`, so its enrolled counts reflect that branch's `config/` and
`src/`, not `main`'s. Regenerate after merging; `--check` refuses to emit if its inputs
are stale, and warns if any slot resolves to no symbol.

`build/*.json` are gitignored artifacts. `tools/skeleton_conform.py` needs
`tools/mwccarm/**` and `extracted/**`, neither of which a fresh clone has -- see
`notes/rom-build.md`.

No `include/`, `config/` or `src/` file is touched by any of this except the retraction of
`include/daObjSwdoor_c.h`, which had zero consumers, so codegen cannot move.
