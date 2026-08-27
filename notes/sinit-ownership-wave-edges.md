# Static-initializer ownership wave: multi-class and large-table edges

## Verdicts

| Initializer | Classifier owner | Ownership verdict | Regeneration verdict |
| --- | --- | --- | --- |
| `__sinit_ov002_02107370` | `ov002/CutsceneObject+daDemo_c` | **CONFIRMED HIGH for the combined original TU.** Do not narrow it to one class: the resource half is tied to `CutsceneObject`, while ten PMF tables and all 69 used PMF descriptors stay in the same 146-function multi-class TU. | **Structurally exact but not symbol-ready.** Ordinary global objects and PMF arrays generate the exact `0xc18` `.init` bytes and relocation shape, but stand-in type/member spellings are not licensed production symbols. |
| `__sinit_ov080_02127b2c` | `ov080/Painting` | **CONFIRMED HIGH and narrowable to `Painting`.** All 19 resource objects and the 12-entry PMF table are consumed within the 29-function Painting TU; all 12 descriptor targets are internal. | **Structurally exact but not symbol-ready.** Ordinary globals generate the exact `0x434` `.init` bytes and relocation shape; original resource-wrapper and unnamed member spellings remain to be recovered. |

Both committed C transcriptions independently pass strict byte and linked-ROM
verification under pinned mwccarm 2004/b56. This audit confirms ownership; it
does not authorize moving either initializer or enrolling the probe.

## `ov002/CutsceneObject+daDemo_c`

### TU boundary and consumers

The candidate is `ov002:60`, text span `0x020f1f70..0x020f8838`, with 146
functions and RTTI labels `CutsceneObject` and `daDemo_c`. Overlay 2 is not on
`tu_map`'s under-segmented list. The immediately surrounding candidate units are:

- `ov002:59`, `0x020f1bc4..0x020f1f70`, `MugenBgm`;
- `ov002:61`, `0x020f8838..0x020f8858`, the two `daDemo_c::anmModel_c`
  `this`-adjusting destructor thunks;
- `ov002:62`, `0x020f8858..0x020f934c`, `Fireball`.

The initializer creates 34 eight-byte resource objects (5 model, 14 animation,
15 texture-sequence objects), registers 34 twelve-byte destructor records, and
initializes ten non-const PMF arrays. The report's 44 directional ownership
globals are those 34 objects plus the ten PMF destinations.

The source scanner directly sees only 15 ownership-global consumers: five
resource objects and ten PMF arrays. Inverting the checked-in ROM relocations
recovers the missing indirection:

- `CutsceneObject::InitResources` loads `data_ov002_0210b600`,
  `data_ov002_0210b610`, `data_ov002_0210bcf0`, and
  `data_ov002_0210bd24` at `0x020f87d4`, `0x020f87dc`, `0x020f87e8`, and
  `0x020f87e0`. Those data records point to 28 of the resource objects.
- The same method directly loads five more resource objects at
  `0x020f87d8..0x020f87fc`.
- Therefore 33 of 34 resource objects have a ROM consumer in
  `CutsceneObject::InitResources`. Only `data_ov002_02110bc8` (constructed with
  ID `0x3f8`) has no relocation outside the initializer; it is an unused member
  of the otherwise TU-local resource run, not an external ownership edge.
- The ten PMF destination arrays are consumed by ten functions from
  `0x020f2dd4` through `0x020f5f0c`, all inside `ov002:60`. The matched consumer
  sources establish two signatures: `void (C::*)(int)` for eight arrays and
  `void (C::*)()` for two arrays.
- All 69 PMF descriptor records used by the initializer relocate to functions
  inside `0x020f1f70..0x020f8838`. Each record is an eight-byte
  `<R_ARM_ABS32 function + 0, this-adjustment 0>` descriptor. None points into
  another TU or module.

This is a genuine shared initializer for one multi-class translation unit. The
resource graph can be narrowed to `CutsceneObject`; the PMF graph cannot yet be
given a safe recovered class name because its consumers and targets remain
mostly `func_ov002_*`. The initializer itself must retain the combined
`CutsceneObject+daDemo_c` owner until the original source partition or names give
stronger evidence.

### `.ctor` order

Overlay 2 has 26 `__sinit` functions and 26 `.ctor` entries. This initializer is
ordinal 22; `.p__sinit_ov002_02107370` at `0x02108128` has a relocation to
`0x02107370`. Its neighbors are:

- ordinal 21: `__sinit_ov002_02107304`, high-confidence `BlueCoinSwitch`
  (`ov002:56`);
- ordinal 23: `__sinit_ov002_02107f88`, high-confidence `SoundObject`
  (`ov002:63`).

The `56 -> 60 -> 63` text-unit ordering agrees with `.ctor` order. It is useful
corroboration, but overlay 2 has more candidate TUs than initializers, so order
alone is not the ownership proof.

## `ov080/Painting`

### TU boundary and consumers

The candidate is `ov080:2`, text span `0x02125404..0x02126fbc`, with 29
functions and class label `Painting`. Its neighbors are `CrazedCrate`
(`0x02124a20..0x02125404`) and `daObjMaruta_c`
(`0x02126fbc..0x0212714c`).

The initializer creates 19 eight-byte resource objects, registers 19
twelve-byte destructor records, and initializes one 12-entry non-const PMF
array at `data_ov080_02128628`.

- Data table `data_ov080_0212775c` contains 19 consecutive relocations, one to
  every resource object. `func_ov080_02125630`'s closing literal-pool
  relocation at `0x021256f4` points to that table. Both the function and the
  table belong to the Painting text/data cluster.
- `Painting::InitResources` loads the PMF destination at `0x02126f7c` and
  dispatches through it.
- The twelve source descriptors at `0x02128214..0x0212826c` all relocate to
  functions inside the Painting span. Every descriptor has addend 0 and
  `this` adjustment 0.
- There are no source-scanned external consumers, unmapped consumers, or raw
  ROM relocation edges to another candidate TU.

### `.ctor` order

Overlay 80 has three initializers and three `.ctor` entries. The text units and
initializer order agree exactly:

1. `MontyMole+MontyMoleRock` / `__sinit_ov080_021278c0`;
2. `CrazedCrate` / `__sinit_ov080_02127a60`;
3. `Painting` / `__sinit_ov080_02127b2c`.

The Painting entry is `.p__sinit_ov080_02127b2c` at `0x02127f68`, relocating to
`0x02127b2c`. This order is corroboration in addition to the direct global,
data-table, and PMF edges.

## Organic CodeWarrior generation

`notes/sinit-ownership-wave-edges-probe.cpp` is excluded from every normal build.
Defining exactly one of `SINIT_EDGE_OV002` or `SINIT_EDGE_OV080` presents ordinary
C++ global resource objects and non-const PMF arrays to CodeWarrior. It contains
no `__sinit` body.

Pinned mwccarm 2004/b56 produces these results:

| Probe | `.init` size | `.init` relocs | Relocation sequence | Non-relocation differences from matched C object |
| --- | ---: | ---: | --- | ---: |
| `SINIT_EDGE_OV002` | `0xc18` | 218 | 150 `R_ARM_ABS32` addend 0; 68 `R_ARM_PC24` addend -8 | 0 |
| `SINIT_EDGE_OV080` | `0x434` | 90 | 52 `R_ARM_ABS32` addend 0; 38 `R_ARM_PC24` addend -8 | 0 |

For both probes, all of the following are identical to the corresponding
matched C object's `.init`: total bytes, every non-relocation word, every
relocation offset, and every relocation type/addend in order. Each probe also
emits one four-byte `.ctor` entry with `R_ARM_ABS32`, addend 0, to the generated
initializer.

The surrounding compiler output has the expected original shape:

- ov002: 34 separate 8-byte resource BSS objects, 34 separate 12-byte destructor
  records, 69 separate 8-byte PMF descriptor data sections, and ten PMF-array
  BSS sections with the recovered flattened sizes;
- ov080: 19 separate 8-byte resource BSS objects, 19 separate 12-byte destructor
  records, twelve separate 8-byte PMF descriptor data sections, and one 0x60-byte
  PMF-array BSS section.

The probe is intentionally not relocation-symbol identical. Its inline
constructors preserve the 34/19 real constructor-call symbols, but compiler
private BSS/data names and stand-in PMF methods differ. CodeWarrior also spells
the runtime registration call `__register_global_object`, while the current
config names the same ROM routine `func_020731dc`. Recovering the original
resource wrapper types, member names, and symbol aliases is required before a
TU can be linked or promoted. Structural generation is not production readiness.

## Classifier lesson

The current report sentence says that all `N ownership global(s) map through
consumers` whenever the *union* of observed consumer units contains one TU. It
does not count how many individual globals actually had a source-scanned
consumer. Here the sentence overstates direct coverage:

- ov002 says 44, while source scanning directly maps 15; raw data-table
  inversion raises real coverage to 43 of 44;
- ov080 says 20, while source scanning directly maps one; raw data-table
  inversion raises real coverage to 20 of 20.

The two verdicts survive, but the wording should report `mapped/total` and the
classifier should eventually follow data-pointer tables. Until then, `high`
means one non-conflicting directional consumer cluster, not that every listed
global was independently traced. A multi-class label should survive only when
it denotes one credible TU and all audited branches remain internal, as they do
for this ov002 case.

## Commands and observed gates

```powershell
python tools/rtti_extract.py --check
python tools/rtti_vtables.py
python tools/tu_map.py
python tools/sinit_owners.py --json build/sinit-owners.json --tsv build/sinit-owners.tsv
```

Observed: 429 RTTI records, 0 unresolved; 454 candidate TUs; the ownership report
contains 305 initializers, including both rows as high with no blocker.

```powershell
python tools/match.py --c src/__sinit_ov002_02107370.c `
  --func __sinit_ov002_02107370 --addr 0x02107370 --size 0xc18 `
  --version 2004/b56 --module ov002 --strict-relocs --brief
python tools/linkcheck.py --name __sinit_ov002_02107370 `
  --c src/__sinit_ov002_02107370.c --addr 0x02107370 --size 0xc18 --module ov002

python tools/match.py --c src/__sinit_ov080_02127b2c.c `
  --func __sinit_ov080_02127b2c --addr 0x02127b2c --size 0x434 `
  --version 2004/b56 --module ov080 --strict-relocs --brief
python tools/linkcheck.py --name __sinit_ov080_02127b2c `
  --c src/__sinit_ov080_02127b2c.c --addr 0x02127b2c --size 0x434 --module ov080
```

Observed for both: `MATCH`; linkcheck `VERIFIED`, `blind: 0`.

The probe was compiled twice with the normal pinned C++ flags plus respectively
`-DSINIT_EDGE_OV002` and `-DSINIT_EDGE_OV080`. A pyelftools comparison against
the two matched C objects produced the exact size, relocation, and difference
counts in the table above.

Additional scoped gates:

```text
python -m unittest tools.test_sinit_owners       8 tests, OK
python tools/port_refcheck.py                    407/407 references resolve
python tools/langmode_audit.py --check langmode-baseline.json
                                                   ratchet PASS
python tools/eligible.py -j 16                   11069/11189 eligible
python tools/check_references.py                 44 unresolved vs baseline 45;
                                                   no new unresolved references
git diff --check                                clean
```

The one-reference improvement predates this notes-only lane, so the baseline was
not updated.

No production `src/`, delinks, symbols, manifests, claims, or attribution files
were changed. `tools/claims_key.txt` is absent; this lane therefore remained a
read-only ownership audit and did not claim or modify any function span.
