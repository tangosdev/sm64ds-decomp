# Static-initializer ownership: confirmed cases

Case log for `tools/sinit_owners.py`'s `high`-confidence ownership rows, each
independently confirmed by organic-compiler reproduction under pinned mwccarm
2004/b56. See `notes/sinit-ownership.md` for the evidence model, confidence
taxonomy, and promotion gate this log measures against -- not repeated here.

Consolidated 2026-09-06 from four prior files (`sinit-ownership-pilot-ov002.md`,
`sinit-ownership-wave-edges.md`, `sinit-ownership-wave-resource-handles.md`,
`sinit-wave-a-ownership.md`) that told one continuing case log four times with
near-verbatim boilerplate. Nothing here is new evidence.

**Confirming ownership is not promotion.** None of the nine rows below moves a
source, edits linker configuration, or licenses a shadow TU by itself -- see
`notes/sinit-ownership.md`'s promotion gate for what still has to happen after
a row is confirmed.

## Method, disclaimers, and gates common to every case

Each case below follows the same four-step method, stated once here instead of
up to four times per case:

1. **Ownership edges.** Trace every consumer of the initializer's globals via
   ROM relocations plus the source scanner, cross-checked against candidate TU
   boundaries (`tools/tu_map.py`/`tools/sinit_owners.py`). A global with no
   consumer outside the candidate TU, and no unmapped consumer, is an
   ownership edge for that TU.
2. **`.ctor` order.** Corroborate with the initializer's position among its
   overlay's `.ctor` entries and its immediate neighbours. Corroboration
   only, never proof by itself -- most overlays have far more candidate TUs
   than initializers (e.g. ov002: 26 initializers, 71 candidate TUs), so
   ordinal adjacency alone cannot attribute ownership.
3. **Organic compiler proof.** A compile-guarded probe (`notes/sinit-probes/
   *.cpp`, or an inline block behind `-DSINIT_*_PILOT`/`_PROBE`) declares
   ordinary C++ globals/PMF arrays with neutral stand-in types and names --
   never a hand-written `__sinit` function -- and is compiled under pinned
   mwccarm 2004/b56. The generated `.init` is compared against the ROM and
   against the committed hand transcription for raw bytes, total size, and
   every relocation offset/type/addend, in that order.
4. **Verification.** The already-committed hand transcription (real ROM
   spellings, not the probe's stand-ins) is checked with `tools/match.py
   --strict-relocs --brief` and `tools/linkcheck.py` -- see the table below
   for the address/size/module/result of each of the nine.

**Ownership inputs**, regenerated per case with
`python tools/sinit_owners.py --json build/sinit-owners.json --tsv
build/sinit-owners.tsv`: 429 RTTI records, 0 unresolved, 454 candidate TUs,
305 initializers (174 `high`-confidence), unless a case notes otherwise.

**No production change, in every case.** Every case in this log is a
read-only ownership audit: no case moved a production `src/` file, edited
delinks/symbols/manifest/linker configuration, or licensed a shadow TU. Every
probe is excluded from the normal build unless its own macro is defined, and
none is listed by any manifest/delinks file. Each case ran in its own wired
worktree off a pinned base sha with `mwccarm 2004/b56` as the compiler
canary. `tools/claims_key.txt` (and `CLAIMS_API_KEY`) were absent in every
case, so none could announce through the claims service; `CLAIMS.md` had no
conflicting active span for any of the nine owners in any case either, so the
missing claims key blocked nothing.

**Cross-case gates**, run and green for every case, not repeated below:
`tools/port_refcheck.py` (407/407 references resolve), `tools/langmode_audit.py
--check langmode-baseline.json` (ratchet PASS), `tools/eligible.py` +
`tools/check_references.py` (11,069/11,189 eligible; 44 unresolved vs.
baseline 45, no new), `git diff --check` (clean), `tools/tu_map.py --check`
(invariants pass), and the relevant `unittest` module for each tool touched
(`tools.test_sinit_owners`, `tools.test_tu_config`: 17 tests total, pass).

## Status at a glance

| Initializer | Owner | Ownership verdict | Current status (`config/tu_manifest.d`) |
|---|---|---|---|
| `__sinit_ov002_02100938` | `ov002/dEnemyBase_c` | CONFIRMED | `text-verified` (`ov002/Enemy.json`) — still has the one unmatched hole (`UpdateYoshiEatER10dBgCh_Actr`) case 1 flagged |
| `__sinit_ov002_02107370` | `ov002/CutsceneObject+daDemo_c` (combined TU, not narrowable) | CONFIRMED HIGH | no manifest yet — single-function files under `src/` |
| `__sinit_ov080_02127b2c` | `ov080/Painting` | CONFIRMED HIGH, narrowable to Painting | no manifest yet — single-function files under `src/` |
| `__sinit_ov009_02112458` | `ov009/Bird` | CONFIRMED | **`text-verified`** (`ov009/Bird.json`) — the "not promotion-ready" framing in case 3 is now stale in the good direction |
| `__sinit_ov015_02112f9c` | `ov015/daObjBkBillboard_c` | CONFIRMED | **`promoted`** (`ov015/daObjBkBillboard_c.json`) — the "not promotion-ready" framing in case 3 is now stale in the good direction |
| `__sinit_ov002_02101064` | `ov002/daObjMarioCap_c` | CONFIRMED | no manifest yet — matches case 3's original claim (`InitResources` still fails the pin check) |
| `__sinit_ov002_02100adc` | `ov002/OneUpMushroom` | CONFIRMED | no manifest yet — its functions sit inside the already-promoted `ov002/Enemy.json` TU's address span, but no separate promotion exists for it |
| `__sinit_ov002_02100f84` | `ov002/daObjKurumajiku_c` | CONFIRMED, byte blocker (register scheduling, not ownership) | no manifest yet — single-function files under `src/` |
| `__sinit_ov063_0211e5fc` | `ov063/MadPiano` | CONFIRMED | no manifest yet — single-function files under `src/` |

Status measured 2026-09-06 against `config/tu_manifest.d/**`; re-derive with
`python tools/tubuild.py inspect <module>/<Class>` before quoting it further out —
this table is a snapshot, not a live query.

## Verification record: every committed transcription

| Initializer | Module | Addr | `.init` size | `match.py --strict-relocs` | `linkcheck.py` |
|---|---|---:|---:|---|---|
| `__sinit_ov002_02100938` (dEnemyBase_c) | ov002 | `0x02100938` | `0x1a4` | MATCH | via `tubuild verify ov002/Enemy`: 30/30 |
| `__sinit_ov002_02107370` (CutsceneObject+daDemo_c) | ov002 | `0x02107370` | `0xc18` | MATCH | VERIFIED, blind: 0 |
| `__sinit_ov080_02127b2c` (Painting) | ov080 | `0x02127b2c` | `0x434` | MATCH | VERIFIED, blind: 0 |
| `__sinit_ov009_02112458` (Bird) | ov009 | `0x02112458` | `0xcc` | MATCH | VERIFIED, blind: 0 |
| `__sinit_ov015_02112f9c` (daObjBkBillboard_c) | ov015 | `0x02112f9c` | `0x40` | MATCH | VERIFIED, blind: 0 |
| `__sinit_ov002_02101064` (daObjMarioCap_c) | ov002 | `0x02101064` | `0x414` | MATCH | VERIFIED, blind: 0 |
| `__sinit_ov002_02100adc` (OneUpMushroom) | ov002 | `0x02100adc` | `0x174` | MATCH | VERIFIED, blind: 0 |
| `__sinit_ov002_02100f84` (daObjKurumajiku_c) | ov002 | `0x02100f84` | `0xe0` | MATCH | VERIFIED, blind: 0 |
| `__sinit_ov063_0211e5fc` (MadPiano) | ov063 | `0x0211e5fc` | `0x100` | MATCH | VERIFIED, blind: 0 |

---

## 1. `ov002/dEnemyBase_c`

**Verdict: CONFIRMED.** `__sinit_ov002_02100938` belongs to the original
`dEnemyBase_c` TU (`0x020ad838..0x020aedbc`, 31 functions), generated by two
non-const, eight-element arrays of pointer-to-member functions (PMFs). Not
promotion-ready: sixteen member functions remain address-derived
`func_ov002_*` names, `_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr` at
`0x020ade78` is an unmatched hole, and the vtable/RTTI alias issue is
independently unlicensed.

### Ownership edges

The initializer has no calls or destructor registrations; its eighteen ROM
relocations load sixteen 8-byte descriptors and two 0x40-byte BSS
destinations. Both destinations have exactly one non-initializer consumer,
both inside the TU's `0x020ad838..0x020aedbc` span:

- `data_ov002_0210db80` -> `func_ov002_020aea30` only (`void
  (dEnemyBase_c::*)(int,int)`, loaded at `0x020aeab8`), pool offset `+0x164`.
- `data_ov002_0210dbc0` -> `dEnemyBase_c::UpdateDeath(dBgCh_Actr&)` only
  (`int (dEnemyBase_c::*)(dBgCh_Actr&)`, loaded at `0x020ae738`), pool offset
  `+0x19c`.

Every descriptor's function relocation also lands inside the TU span, each
`<R_ARM_ABS32 function + 0, 0>` (zero `this`-adjustment). The first table's
eight descriptors init in source order at pool offsets `0x15c..0x17c`,
targeting `func_ov002_020ae9f8, _ae954, _ae890, _ae87c, _ae844, _ae80c,
_ae73c, _aea2c` (ROM descriptors `0x021081b8, 150, 168, 160, 158, 170, 140,
1a0`, respectively). The second table's eight init in source order at
`0x180..0x19c`, with CodeWarrior scheduling index 3's descriptor literal
(target `func_ov002_020ae4cc`) after the destination literal -- the natural
probe reproduces this exactly.

### `.ctor` order and organic proof

Overlay 2 has 26 `__sinit` functions and 26 `.ctor` entries. This is ordinal 1
(`.p__sinit_ov002_02100938` at `0x021080d4`); the preceding entry is the
ambiguous shared-resource initializer `__sinit_ov002_02100560`, the following
is `__sinit_ov002_02100adc` (OneUpMushroom, case 4 below) -- no conflict.

A pilot block appended to `src_tu/actors/Enemy.cpp`, built only with
`-DSINIT_OWNERSHIP_PILOT`, declares two ordinary non-const PMF arrays and
contains no `__sinit` function. Pinned mwccarm 2004/b56 generates one `.init`
function (`0x1a4` bytes), one 4-byte `.ctor` entry, sixteen separate 8-byte
`.data` PMF descriptors, two 0x40-byte `SHT_NOBITS` tables, and eighteen
`.init` relocations at the same offsets as above (all addend 0). This
reproduces the ROM at `0x02100938` (`MATCH`, size `0x1a4`, zero
non-relocation divergences) and the hand transcription
`src/__sinit_ov002_02100938.c` (all 420 raw bytes and all eighteen relocation
offsets identical). With the pilot flag on, the full Enemy shadow object's
thirty admitted text functions and their relocation signatures stay
byte-for-byte identical to the flag-off build.

`tools/tubuild.py verify ov002/Enemy`: 30/30 text MATCH, `objisolate` and
relocation destinations clean. `tools/tubuild.py partial ov002/Enemy`: 29/30
-- the one gap is the pre-existing `dEnemyBase_cD2Ev` / `_ZTV12dEnemyBase_c`
co-address alias, unrelated to and unaltered by this pilot.

---

## 2. Multi-class and large-table edges: `ov002/CutsceneObject+daDemo_c` and `ov080/Painting`

| Initializer | Owner | Ownership verdict | Regeneration verdict |
|---|---|---|---|
| `__sinit_ov002_02107370` | `CutsceneObject+daDemo_c` | **CONFIRMED HIGH for the combined TU.** Do not narrow to one class: the resource half ties to `CutsceneObject`, but ten PMF tables and all 69 used PMF descriptors stay inside the same 146-function multi-class TU. | Structurally exact (`0xc18` `.init`, full relocation shape) but not symbol-ready -- stand-in type/member spellings are not licensed production symbols. |
| `__sinit_ov080_02127b2c` | `Painting` | **CONFIRMED HIGH, narrowable to `Painting`.** All 19 resource objects and the 12-entry PMF table are consumed within the 29-function Painting TU; all 12 descriptor targets are internal. | Structurally exact (`0x434` `.init`, full relocation shape) but not symbol-ready -- original resource-wrapper and unnamed member spellings remain to be recovered. |

### `ov002/CutsceneObject+daDemo_c`

TU `ov002:60`, `0x020f1f70..0x020f8838`, 146 functions, RTTI labels
`CutsceneObject`/`daDemo_c`; neighbours `ov002:59` `MugenBgm`
(`..0x020f1f70`), `ov002:61` the two `daDemo_c::anmModel_c` this-adjusting
thunks (`0x020f8838..0x020f8858`), `ov002:62` `Fireball` (`..0x020f934c`).

The initializer creates 34 eight-byte resource objects (5 model, 14
animation, 15 texture-sequence), registers 34 twelve-byte destructor records,
and initializes ten non-const PMF arrays -- 44 directional ownership globals
total. The source scanner directly sees only 15 consumers (five resource
objects, ten PMF arrays); inverting the ROM relocations recovers the rest:
`CutsceneObject::InitResources` loads four data records
(`data_ov002_0210b600/b610/bcf0/bd24`) that point to 28 more resource objects,
plus five more resource objects directly -- 33 of 34 resource objects
accounted for. The one exception, `data_ov002_02110bc8` (ID `0x3f8`), has no
relocation outside the initializer -- an unused TU-local member, not an
external edge. The ten PMF destination arrays are consumed by ten functions
at `0x020f2dd4..0x020f5f0c`, all inside the TU (`void(C::*)(int)` x8,
`void(C::*)()` x2). All 69 PMF descriptor records relocate to functions
inside `0x020f1f70..0x020f8838`, each `<R_ARM_ABS32 function+0, 0>`. The
resource graph narrows to `CutsceneObject`; the PMF graph cannot yet be
named because its consumers/targets remain mostly `func_ov002_*`, so the
initializer keeps the combined owner.

`.ctor`: ordinal 22 of 26 (`.p__sinit_ov002_02107370` at `0x02108128`).
Neighbours: ordinal 21 `__sinit_ov002_02107304` (high, `daObjBC_Switch_c`,
`ov002:56`), ordinal 23 `__sinit_ov002_02107f88` (high, `daSoundObj_c`,
`ov002:63`) -- text order `56 -> 60 -> 63` agrees with `.ctor` order
(corroboration; ov002 has far more candidate TUs than initializers).

### `ov080/Painting`

TU `ov080:2`, `0x02125404..0x02126fbc`, 29 functions; neighbours `CrazedCrate`
(`0x02124a20..`) and `daObjMaruta_c` (`..0x0212714c`). The initializer creates
19 eight-byte resource objects, 19 twelve-byte destructor records, and one
12-entry non-const PMF array at `data_ov080_02128628`. Table
`data_ov080_0212775c` has 19 consecutive relocations, one per resource
object, referenced by `func_ov080_02125630`'s closing pool relocation --
both inside the Painting cluster. `Painting::InitResources` loads and
dispatches the PMF destination at `0x02126f7c`. All twelve source descriptors
at `0x02128214..0x0212826c` relocate (addend 0, this-adjustment 0) to
functions inside the Painting span. No external or unmapped consumer found.

`.ctor`: overlay 80 has three initializers/three `.ctor` entries, in exact
text-unit agreement: `MontyMole+MontyMoleRock` (`__sinit_ov080_021278c0`),
`CrazedCrate` (`_02127a60`), `Painting` (`_02127b2c` at
`.p__sinit_ov080_02127f68`) -- corroboration alongside the direct edges.

### Organic proof (both)

`notes/sinit-ownership-wave-edges-probe.cpp`, excluded from every normal
build, presents ordinary C++ global resource objects and non-const PMF arrays
under exactly one of `SINIT_EDGE_OV002`/`SINIT_EDGE_OV080`, with no `__sinit`
body. Pinned mwccarm 2004/b56:

| Probe | `.init` size | relocs | sequence | non-reloc diffs from matched C object |
|---|---:|---:|---|---:|
| `SINIT_EDGE_OV002` | `0xc18` | 218 | 150 `R_ARM_ABS32`@0 + 68 `R_ARM_PC24`@-8 | 0 |
| `SINIT_EDGE_OV080` | `0x434` | 90 | 52 `R_ARM_ABS32`@0 + 38 `R_ARM_PC24`@-8 | 0 |

Both probes match the corresponding C object on total bytes, every
non-relocation word, and every relocation offset/type/addend in order, plus
one 4-byte zero-addend `.ctor` entry. Surrounding output has the expected
original shape (ov002: 34 resource BSS + 34 destructor records + 69 PMF
descriptors + ten PMF-array BSS sections; ov080: 19 + 19 + twelve descriptors
+ one 0x60-byte PMF-array section). The probes are intentionally not
relocation-symbol identical -- stand-in BSS/data names and PMF methods differ,
and CodeWarrior spells the registration call `__register_global_object` where
the current config names the same ROM routine `func_020731dc`. Recovering
original wrapper types/member names/symbol aliases is still required before
either TU can link or promote.

### Classifier lesson

`sinit_owners.py`'s report sentence ("all N ownership global(s) map through
consumers") fires whenever the *union* of observed consumer units contains
one TU -- it does not count how many individual globals had a
source-scanned consumer, so it overstates direct coverage here: ov002 says
44 while source scanning directly maps only 15 (raw data-table inversion
raises real coverage to 43/44); ov080 says 20 while source scanning directly
maps one (inversion raises it to 20/20). Both verdicts survive, but the
report should say `mapped/total`, and the classifier should eventually follow
data-pointer tables itself. Until then, `high` means one non-conflicting
directional consumer cluster, not that every listed global was independently
traced.

---

## 3. Resource-handle lane: `ov009/Bird`, `ov015/daObjBkBillboard_c`, `ov002/daObjMarioCap_c`

| Initializer | Owner | Ownership verdict | Regeneration readiness |
|---|---|---|---|
| `__sinit_ov009_02112458` | `Bird` | **CONFIRMED** | Not promotion-ready: resource-family symbol names, four original PMF member names, and unlicensed vtable/RTTI output remain. |
| `__sinit_ov015_02112f9c` | `daObjBkBillboard_c` | **CONFIRMED** | Structurally straightforward, but no shadow manifest exists and vtable/RTTI/data ranges are unlicensed. |
| `__sinit_ov002_02101064` | `daObjMarioCap_c` | **CONFIRMED** | Not promotion-ready: `InitResources` is not a matching source, PMF member names remain unknown, and ov002 cannot be ordinal-partitioned. |

All three probes reproduce the complete initializer instruction stream, size,
and relocation offsets/types/addends with no hand-written `__sinit` body,
using neutral stand-in class names. The one-to-one semantic mapping for the
shared resource idiom: probe model ctor/dtor -> `func_02017acc`/
`func_02017ab4`; probe animation ctor/dtor ->
`_ZN13SharedFilePtr9ConstructEj`/`SharedFilePtr_Destruct_Anim`; compiler
`__register_global_object` -> `func_020731dc`.

### `ov009/Bird`

TU `0x021111a0..0x02111a70`, 13 functions (high boundaries). Two ownership
globals, each with no consumer outside `Bird::InitResources`/
`CleanupResources`: `data_ov009_02113c20` (8-byte model handle),
`data_ov009_02113c28` (8-byte animation handle). Constructs asset 1080 via
`func_02017acc`, registers `func_02017ab4`/node `data_ov009_02113c30`; asset
1081 via `SharedFilePtr::Construct`, registers
`SharedFilePtr_Destruct_Anim`/node `data_ov009_02113c3c`. Also inits the
32-byte PMF table `data_ov009_02113c48` (four 8-byte descriptors at
`0x021138fc..0x0211391c`, targets `func_ov009_021116ec/1115d8/11145c/111234`,
all inside the Bird span). ROM BSS `0x02113c20..0x02113c68` matches exactly:
two handles + two nodes + one PMF table.

`.ctor`: ordinal 0 of 4 (`.p` at `0x02112b34`); next entry
`__sinit_ov009_02112524` independently owns daObjMcWater_c. Probe
(`notes/sinit-probes/bird.cpp`, `-DSINIT_OWNERSHIP_PROBE`): `.init` `0xcc`
bytes raw-identical; `.rela.init` all 15 offsets identical (4x type-1/@-8,
11x type-2/@0); one 4-byte `.ctor`; BSS `8+8+12+12+32` bytes; no
`.exceptix`. Existing Bird shadow TU: 13/13 text-matched,
contribution-equivalent; promotion still refused on pre-existing compiler-only
D2/helper and vtable/RTTI outputs, and one legacy entry lacking `complete`
despite its asm hatch matching.

### `ov015/daObjBkBillboard_c`

TU `0x021111a0..0x02111314`, 7 functions (high boundaries). One ownership
global, `data_ov015_02114960` (8-byte model handle), consumed only by
`InitResources`/`CleanupResources`. Constructs asset 1416 via
`func_02017acc`, registers `func_02017ab4`/node `data_ov015_02114968`; ROM
BSS `0x02114960..0x02114974` = handle + node exactly.

`.ctor`: ordinal 0 of 7 (`.p` at `0x02113410`); next entry
`__sinit_ov015_02112fdc` independently owns KnockDownPlank. Probe
(`notes/sinit-probes/pole_billboard.cpp`): `.init` `0x40` bytes, all 64 bytes
identical; 5 relocations identical (2x type-1/@-8, 3x type-2/@0); one 4-byte
`.ctor`; BSS one 8-byte object + one 12-byte node; no data/exception output.
All seven text functions verify under the pin; not yet a production claim --
no shadow TU/manifest exists.

### `ov002/daObjMarioCap_c`

TU `0x020b6f18..0x020b8bf0`, 31 functions, high boundaries both sides. 22
directional globals, all consumers internal, no unmapped/external consumer.
Thirteen 8-byte animation handles (asset IDs `0x8012,0x8013,0x480,0x47e,
0x47f,0x478,0x476,0x47c,0x479,0x477,0x47d,0x47b,0x47a` in source order), each
via `SharedFilePtr::Construct`/`func_020731dc`/`SharedFilePtr_Destruct_Anim`,
with matching 12-byte nodes at `data_ov002_0210dee0..0210ded4`. Extra
consumers beyond Init/Cleanup: `0x8012`->`OnTurnIntoEgg`+2 funcs, and
`0x8013`/`0x47b`/`0x47a`->`func_020b7330`/`_74d0`; the rest have none. Nine
16-byte two-entry PMF arrays at `data_ov002_0210df04..df84`, five with no
extra consumer and four consumed by `func_020b7f7c`/`OnTurnIntoEgg`/
`Render`/`func_020b6fcc`. All eighteen source PMF descriptors at
`0x0210951c..0x021095ac` relocate (addend 0) to functions inside the
MarioCap span. Target data/BSS is exact and contiguous: 18 descriptors
(144B) + 13 handles (104B) + 13 nodes (156B) + 9 PMF arrays (144B).

`.ctor`: ordinal 8 of 26, between `daObjKurumajiku_c` (case 4) and
`daObjPushblock_c` -- corroboration only (26 initializers, 71 candidate TUs).
Probe (`notes/sinit-probes/mario_cap.cpp`): `.init` `0x414` bytes, all 1,044
bytes identical; 80 relocations identical (26x type-1/@-8, 54x type-2/@0);
one 4-byte `.ctor`; `.data` 18x8B descriptors; `.bss` 13x8B + 13x12B + 9x16B
(404B); no exceptions. Strong ownership confirmation despite the ordinal gap
staying unavailable; not promotion-ready -- current `InitResources` source
fails the pin check (999 differing words, no `complete`), no merged shadow
manifest exists, eighteen PMF member names remain address-derived, and the
TU would emit unlicensed vtable/RTTI data.

### Verification detail

```
daObjBkBillboard_c  size 64/64     raw_equal=True  reloc_topology_equal=True  5 relocs
Bird                size 204/204   raw_equal=True  reloc_topology_equal=True 15 relocs
MarioCap            size 1044/1044 raw_equal=True  reloc_topology_equal=True 80 relocs
```

`tools/tu_config.py` gives Bird/daObjBkBillboard_c ordered full-section
intervals and correctly refuses ov002's invalid 26-to-71 ordinal partition.

---

## 4. Wave A: `ov002/OneUpMushroom`, `ov002/daObjKurumajiku_c`, `ov063/MadPiano`

| Initializer | Ownership | Regeneration readiness |
|---|---|---|
| `__sinit_ov002_02100adc` | **CONFIRMED: OneUpMushroom** | Exact organic proof (raw-identical `0x174` `.init`, matching 15 reloc offsets/types/addends, 14 8-byte descriptors, `0x70` BSS destination, one `.ctor`). Needs names/signatures for 14 anonymous state methods + TU link placement. |
| `__sinit_ov002_02100f84` | **CONFIRMED: daObjKurumajiku_c** | Partial organic proof (exact `0xe0` size, exact 13 reloc offsets/types/addends, four 12-byte objects+nodes, one dtor, one `.ctor`) but 16 non-relocation words differ (store/register scheduling) -- **not byte-ready**. |
| `__sinit_ov063_0211e5fc` | **CONFIRMED: MadPiano** | Exact organic proof (raw-identical `0x100` `.init`, matching 20 reloc offsets/types/addends, three 12-byte nodes, four 8-byte descriptors, 32-byte BSS table, one `.ctor`). Needs real special-member type declarations/names + TU link placement. |

Ownership verdicts stay confirmed even where regeneration is incomplete:
ownership is exclusive-consumer/TU-boundary evidence; source reconstruction
is a separate publication gate.

### OneUpMushroom (`ov002:3`)

`__sinit_ov002_02100adc` copies fourteen 8-byte PMF descriptors into
`data_ov002_0210dc00` (`0x70` bytes). Sole ROM consumer:
`0x020b01bc -> 0x0210dc00` in `OneUpMushroom::Behavior`
(`src/_ZN13OneUpMushroom8BehaviorEv.cpp`); no unmapped/external consumer. All
fourteen descriptor targets (`0x02108350->020aff10`, `...330->afe4c`,
`...348->afd10`, `...358->afc44`, `...338->afbb4`, `...310->afa98`,
`...328->afa6c`, `...320->af950`, `...308->af924`, `...340->af838`,
`...318->af7cc`, `...300->afa50`, `...368->af908`, `...360->af724`) fall
inside the inferred TU `0x020aee40..0x020b05d0` (38 functions, high
boundaries, containing D1/D0, vtable-related methods, resource methods,
Behavior, both spawn functions; no hole found).

`.ctor` word `.p__sinit_ov002_02100adc` at `0x021080d8`, between
`dEnemyBase_c` (case 1) and the medium-confidence Coin initializer --
neighbour check only (26 initializers, 71 inferred TUs). Probe: `.init`
`0x174` both sides, `raw_equal=True`; 15 identical relocation words
(`0x138..0x170`), all type 2 (`R_ARM_ABS32`)/addend 0, descriptors emitted as
individual 8-byte `.data` sections in source order matching the ROM order
above.

### daObjKurumajiku_c (`ov002:28`)

Constructs four contiguous 12-byte `Vector3` objects, each separately
registering the empty `Vector3::~Vector3` (`0x020072c0`):
`0x0210ddd0 (0,+0x190000,0x12c000)`, `0x0210dddc (0,-0x190000,0x12c000)`,
`0x0210dde8 (+0x190000,0,0x12c000)`, `0x0210ddf4 (-0x190000,0,0x12c000)` --
nodes `0x0210ddac/ddb8/ddc4/dda0` respectively. Four separate destructor
registrations (all -> `func_020731dc`) prove four separate objects, not an
array. Sole consumer: `0x020b6c4c -> 0x0210ddd0` in
`daObjKurumajiku_c::Behavior`, indexing the storage as four `Vector3`
values; no other consumer of any of the eight symbols. Inferred TU
`0x020b69e4..0x020b6c54` (six functions, medium boundaries: D0/D1, Behavior,
Render, two anonymous; no hole). `.ctor` word at `0x021080ec`, between
QuestionSwitch and daObjMarioCap_c (case 3).

An aggregate `Vector3 v = {...}` was rejected as a generator (places
components flat in `.data`, emits only a `0x78` initializer). Four separate
objects of a 12-byte probe type with an inline three-argument constructor
instead emit the correct runtime stores: `.init` `0xe0` both sides,
`raw_equal=False`; matching relocation words (`2c 58 80 ac` calls type
1/@-8, `bc c0 c4 c8 cc d0 d4 d8 dc` pool type 2/@0). Sixteen non-relocation
words differ (`38 3c 40 4c 50 60 64 68 74 78 8c 90 94 9c a0 a4`) -- register
choice/store scheduling in objects 1-3 only (object 0, all calls, all
literal positions, size, and section inventory agree). A compiler/spelling
blocker, not evidence against ownership: the shared `Vector3` type has the
destructor but no recovered three-component constructor spelling yet.

### MadPiano (`ov063:3`)

Constructs and registers three consecutive 8-byte resource handles: model
`0x0211ef80` (asset `0x40a`, ctor `0x02017acc`, dtor `0x02017ab4`, node
`0x0211ef98`), collision `0x0211ef88` (`0x40b`, `0x02017b4c`/`0x02017b34`,
node `0x0211efa4`), attack-animation `0x0211ef90` (`0x40c`,
`0x0201799c`/`0x02017984`, node `0x0211efb0`) -- constructor calls at
initializer offsets `0x10/0x2c/0x48`, registration at `0x20/0x3c/0x58`,
pool triples at `0xbc..0xe8` (type 2/@0). The parser doesn't classify the
three dtor veneers as C++ destructors, but their position as
`func_020731dc`'s second argument is direct registration evidence.

Second half copies four PMFs into `data_ov063_0211efbc`
(`0x0211ecd8->0211dd78`, `...ce8->1dbb8`, `...ce0->1dba4`,
`...cf0->1d8cc`), all targets inside MadPiano's TU; the two table consumers
(`0x0211ddf0`, `0x0211de38`) sit in
`src/unnamed/ov063/func_ov063_0211ddac.cpp`/`...0211ddf4.cpp`. All resource
consumers stay inside MadPiano too (CleanupResources: all three handles;
InitResources: model/animation/collision). Inferred TU
`0x0211d4b8..0x0211e1c0` (17 functions, high boundaries: D0/D1, all four PMF
targets, both dispatchers, Behavior, Render, resource methods, spawn; no
hole). ov063 has exactly four inferred TUs/initializers/`.ctor` entries --
order corroboration. `.ctor` word `0x0211e708`, after BigBooIcon+BooCage+
daTrs_c (`e6fc`), MansionSteps (`e700`), FallBlockBbh (`e704`).

Probe (`notes/sinit-probes/wave_a.cpp`, one `SINIT_PROBE_*` macro at a time):
`.init` `0x100` both sides, `raw_equal=True`; 20 identical relocation words
(`0x10 20 2c 3c 48 58 bc c4 c8 cc d4 d8 dc e4 e8 ec f0 f4 f8 fc`). Emits
three 8-byte BSS resource objects, three 12-byte nodes, one 32-byte PMF
table, four 8-byte `.data` descriptors, one 4-byte `.ctor` -- exactly
explains the ROM's contiguous `ef80..efdc` region. Production declarations
remain blocked: `SharedFilePtr.h` doesn't yet claim the 8-byte layout or
constructor/destructor special members, and the three ROM veneers have
distinct generic names.
