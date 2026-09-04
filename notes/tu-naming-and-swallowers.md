# TU naming from RTTI, and why `main` collapsed into one unit

**Status:** analysis. Two tools and one opt-in flag; no build change, no enrollment,
`build/tu_map.json` byte-identical by default.

Companion to [`translation-unit-reconstruction-plan.md`](translation-unit-reconstruction-plan.md).
That plan asks for TU boundaries and a manifest. This note adds two things it does
not have: a candidate *name* for each TU, and a diagnosis of the single largest
defect in the current map.

---

## 1. The ROM's class names encode a filename convention

EAD's class names carry a lowercase prefix naming the layer. Measured over the 429
`type_info` records:

| prefix | count | reading | examples |
| --- | --- | --- | --- |
| `da` | 282 | actor | `daKrb_c`, `daObjKaitendai_c` |
| `d` | 101 | game layer | `dActor_c`, `dBgActor_c`, `dScMgBase_c` |
| `c` | 12 | common/library | `cMgSmartball_ball_c` |
| `f` | 1 | framework root | `fBase_c` |
| `m` | 3 | memory | `mHeap::ExpHeap_t` |

`_c` closes **408 of 429** names (class); `_t` closes 3 (all `mHeap::*`, plain
structs); `_info` closes the 4 ABI records.

In the GameCube Zelda codebases that share this convention the prefix *is* the path:
`daKrb_c` lives in `d_a_krb.cpp`. Later NSMBW source independently maps
`dScBoot_c` to `d_s_boot.cpp` and `daObjFruitTree_c` to
`d_a_obj_fruit_tree.cpp`. `tools/tu_names.py` applies that lineage convention to
every RTTI class.

### This part is a hypothesis, and the ROM cannot confirm it

There is no `__FILE__` evidence to check against. A sweep of every printable string
in all 104 modules finds exactly one source filename ROM-wide — `isdoverlay.c` at
`0x020868fc`, Nintendo's IS-Debugger SDK, with the
`ASSERTION FAILED FILE=[%s] LINE=[%d] EXP=[%s]` format above it at `0x020868a0`.
Game code was built with asserts stripped. So the derived filenames are a naming
*proposal*; nothing writes them where the build can see them.

The prefix mapping does not settle acronym or numeric word boundaries. Candidates
such as `d_s_mg_m_carlo.cpp` and `d_s_mg3_d_esp.cpp` remain mechanical hypotheses
until separate TU evidence supports their exact spelling.

## 2. What IS testable: the grouping the convention implies

`tu_map.py` derives boundaries from address intervals and never sees a name. So the
names are an independent signal, and `tu_names.py --check` runs two predictions.

**Prediction 1 — classes sharing a derived stem share a TU.** Zero failures. Six
stems are claimed by more than one class; every one of those groups lands in a single
TU (`daDemo_c` with its three nested helpers, `dScMgBase_c` with its
`graphCallback_c`, the three `mHeap::*_t`). Four more are unplaced because their
functions carry no label at all. No group is split across TUs.

*(Nested names resolve against the OUTER component. `dScMgBase_c::graphCallback_c` is
a helper declared inside its outer class, not its own TU — deriving from the leaf
gives `graph_callback` for five different scene classes and collides them.)*

**Prediction 2 — a multi-class TU carries related classes.** 20 of 21 pass. And the
relatedness is far more visible under the original names than the tree's coined
English ones:

| module | ROM names | tree names |
| --- | --- | --- |
| ov080 | `daChoropu_c` + `daChoro_Rock_c` | MontyMole + MontyMoleRock |
| ov002 | `daStar_c` + `daStarBase_c` | PowerStar + PowerStarBase |
| ov060 | `daKpa_c` + `daKpaTail_c` | Bowser + BowserTail |
| ov065 | `daDossy_c` + `daDossyCap_c` | Dorrie + DorrieCap |
| ov020 | `daBook_c` + `daBookGen_c` | BookShot + BookShotSpawner |
| ov026 | `daWater_Suikomi_c` + `daWater_Tatumaki_c` | — |
| ov064 | `daObjFl_Coin_c` + `daObjFl_Puzzle_c` | — |

`daChoropu_c`/`daChoro_Rock_c` is `tu_map.py`'s own canonical interleave case,
recovered here from name evidence the clustering never used.

The one failure is `main`, which is section 3.

## 3. Why `main` collapsed: labels that are not single-TU entities

Baseline `main`: **2,984 of 3,067 functions and 73 class labels in ONE unit**
(`0x20049f0..0x20707a4`), out of 26 total. Its boundary confidence was
`{low: 23, medium: 2}` — not one high-confidence cut in the whole module.

The overlap rule — *span(A) overlaps span(B) ⟹ same TU* — is forced by the linker
only if each label occupies exactly one object. Two kinds of label break that, and
`main` is full of both:

- **namespaces used as class labels.** `_ZN4CP15...` is direct evidence the function
  belongs to `CP15`, but `CP15` is a namespace spanning dozens of TUs. Its 16
  functions stretch across `0x690bc` bytes and strictly contain the complete spans of
  **71** other labels. Same shape: `IRQ`, `cstd`, `GX`, `Sound`, `SaveData`,
  `Message`, `Particle`.
- **genuinely multi-TU classes** — the case the plan predicts in section 2 ("A large
  class can have methods defined across several TUs"). `Model`, `Scene`, `Stage`,
  `Animation`, `MeshCollider`, `TextureSequence` all *do* have `type_info` records
  and still swallow.

Because that second group exists, **"has RTTI" does not separate the two kinds** —
it gets 8 of 14 and must not be used as the discriminator. This was tested and
rejected.

### The rule that works needs no list

If A's span strictly contains the COMPLETE spans of *k* or more other labels, A is
not one contiguous object with all of them. Drop A's span; keep the split. Its
functions fall through to `absorb_unlabelled` and attach by call graph. This is the
same treatment a bridging *RTTI* span already gets in `cluster()` — the asymmetry
that docstring describes (symbols trusted, RTTI not) is right about which class
**owns** a function and wrong about TU **co-membership**.

`python tools/tu_map.py --split-swallowers [K]`, default `K=8`, off unless asked:

| | baseline | `--split-swallowers` |
| --- | --- | --- |
| `main` TUs | 26 | **164** |
| `main` boundaries | `low 23, medium 2` | `low 119, medium 32, **high 12**` |
| `main` TUs with a class | 1 | 28 |
| tree-wide TUs | 608 | 746 |
| tree-wide high-confidence boundaries | 277 | **289** |
| modules flagged under-segmented | ov007, **main**, ov075, ov084 | ov007, ov075, ov084 |

At `k >= 6` **no module other than `main` changes at all**, and the result is flat
across `k = 6..12` — it is reading a real structural feature, not a tuned cutoff.
`k = 3` is too aggressive: it splits ov063, whose Boo/BooCage/BigBooIcon interleave is
one of the cases the tool exists to get right.

`main` is still under-segmented afterwards, but at subsystem granularity rather than
one blob — the largest remaining units are coherent: `Heap`/`Memory`/`HeapAllocator`
(607), `Scene`/`Event`/`Fog`/`OAM` (530), the collision family
`BgCh`/`SphereClsn`/`RaycastGround`/`MeshColliderBase` (223), `Camera`/`Vector3`
(157), the GX register API `G2`/`G2S`/`G3X`/`GXS` (122).

## 4. Two gate findings

**`sinit_vs_tu` is structurally blind to this defect.** The gate is
`ok if len(sinits) <= len(tus)`. `main` has 23 sinits and 26 TUs, so it passed while
97% of the module sat in one unit. It can only fire when under-segmentation is severe
enough to push the TU count below the count of TUs *with static initialisers*. Not
changed here — a fix needs a different signal, not a different constant.

**`V1 ov080 classed-TU count == 3` was failing on `origin/main`, with no flag.
Corrected to 4.** Reproduced on a clean baseline before touching it, so it was
pre-existing rather than caused by anything else in this note, and the map was never
wrong. ov080 really does have 5 TUs, of which 4 carry a class label:

```sh
0x2123740  MontyMole, MontyMoleRock, daChoropu_c
0x2124a20  CrazedCrate, daBttBk_c
0x2125404  Painting
0x2126fbc  daPicGate_c        <- labelled by RTTI; no mangled name ever named it
```

The constant predated the RTTI label source. The 4th classed TU is new *labelling*,
not a new *boundary* — the total is 5 either way, and 3 sinits still fits under it.
`--check` is now 8/8 green both with and without `--split-swallowers`.

Note this is a different count from the module docstring's "ov080's three TUs shatter
into thirteen", which describes the *mangled-name* view alone; `daPicGate_c` has no
mangled name and is not part of that illustration, so that passage stands.

## 5. Pilot cross-check

Against the three `config/tu_manifest.d/` entries:

| entry | status | result |
| --- | --- | --- |
| `ov045/PoleLift` | text-verified | `0x211150c..0x2111840` — **exact match** |
| `ov045/daObjKm2_Fall_Block_c` | text-verified | `0x2111d48..0x2111e60` — **exact match** |
| `ov002/LevelObjects` | link-verified | manifest `0x20fe190..0x20fea4c`, map `0x20fe190..0x20fe33c` — **start agrees, end short by 0x710** |

The two text-verified pilots reproduce exactly. The one **link-verified** pilot — the
strongest evidence in the manifest — is a case where the map cuts *early*: same start,
2 functions instead of the verified range. Worth treating as a known-good counterexample
when tuning end-cut logic, since link verification outranks the map.

## 6. Reproducing

```powershell
python tools/rtti_extract.py --check     # 429 records, 413 edges, GATE OK
python tools/rtti_vtables.py
python tools/evidence_hierarchy.py
python tools/rtti_reconcile.py
python tools/tu_map.py                   # baseline, 608 TUs
python tools/tu_map.py --split-swallowers --out build/tu_map_split.json
python tools/tu_names.py --check
```
