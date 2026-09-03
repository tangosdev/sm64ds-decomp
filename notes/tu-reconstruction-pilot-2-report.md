# TU reconstruction pilot #2 — `ov002/LevelObjects`

**What this is:** the second read-only experiment described in
[`translation-unit-reconstruction-plan.md`](translation-unit-reconstruction-plan.md)
§18. Pilot #1 ([`tu-reconstruction-pilot-report.md`](tu-reconstruction-pilot-report.md))
reconstructed a *key-function* TU — a class with a virtual destructor — and closed
by recommending that the next one deliberately avoid that shape, so the first
whole-range link is not confounded by the destructor-emission-order problem it
found. This is that candidate.

**What it is not:** a build change. `src_tu/stage/LevelObjects.cpp` is not
enrolled, `config/**/delinks.txt` is untouched, and the seventeen one-function
sources under `src/` remain the sole enrolled owners of
`0x020fe190..0x020fea4c`. No `rombuild.py`, no `eligible.py`, no link.

---

## 1. Headline result

> **17 of 17 already-matched functions remain byte-identical when compiled
> together in their candidate original TU context — and this time the merged
> object emits no vtable, no RTTI, and no destructor of any class it defines.**

```python
TU ov002/LevelObjects

MATCH  Stage::LoadClsnAndObjects   0x020fe190  size 0x1ac  relocs 19
MATCH  LoadObjects                 0x020fe33c  size 0x090  relocs  2
MATCH  LoadStarCameraObjects       0x020fe3cc  size 0x018  relocs  1
MATCH  LoadUnusedType13Objects     0x020fe3e4  size 0x014  relocs  1
MATCH  LoadMinimapScaleObjects     0x020fe3f8  size 0x014  relocs  1
MATCH  LoadMinimapTileObjects      0x020fe40c  size 0x014  relocs  1
MATCH  LoadExitObjects             0x020fe420  size 0x0d0  relocs  1
MATCH  LoadDoorObjects             0x020fe4f0  size 0x0dc  relocs  3
MATCH  LoadFogObjects              0x020fe5cc  size 0x014  relocs  1
MATCH  LoadTeleportDestObjects     0x020fe5e0  size 0x014  relocs  1
MATCH  LoadTeleportSourceObjects   0x020fe5f4  size 0x09c  relocs  1
MATCH  LoadViewObjects             0x020fe690  size 0x014  relocs  1
MATCH  LoadPathObjects             0x020fe6a4  size 0x014  relocs  1
MATCH  LoadPathNodeObjects         0x020fe6b8  size 0x010  relocs  1
MATCH  LoadEntranceObjects         0x020fe6c8  size 0x1e4  relocs 15
MATCH  LoadStandardObjects         0x020fe8ac  size 0x0b4  relocs  3
MATCH  LoadSimpleObjects           0x020fe960  size 0x0ec  relocs  4

objisolate.plan  : error=None on all seventeen
reloc destinations: 57 relocations, 57 OK, 0 WRONG-DEST, 0 missing
emitted section order: exact ROM address order, ordinals 0..16, no exceptions

Result: 17/17 functions reproduce in merged-TU context
```

That was the state of the **first** compile of the merged file; no byte-chasing
iteration was needed. Manifest status is `text-verified`.

Three gates were run, not two. Pilot #1 ran `match.compare` and
`objisolate.plan` and recorded the relocation-**destination** check as NOT RUN.
It is run here (`tools/reloc_audit.check_destinations`, the same code path
`match.py --strict-relocs` uses), because pilot #1's own headline lesson was that
a relocated word is a wildcard to the byte gate.

### Baseline control

Before and after, the seventeen untouched legacy files were re-verified with
`build_pin.verify` under their pinned compiler, so a green merged result cannot
be an environment artefact. All seventeen: `OK ... 2004/b56`.
`config/rombuild-versions.txt` carries no override for any of the seventeen
stems, so `2004/b56` is the pin for all of them.

---

## 2. Selection, and what the search itself established

The brief asked for a candidate with **zero destructor and zero vtable/RTTI
emission**, as a literal non-waived bar, via either shape (a) a non-polymorphic
class or shape (b) a real TU of a polymorphic class that is not its key-function
TU. Getting there produced four findings that are worth more than the candidate.

### 2.1 `tu_map.json`'s unit records are not guaranteed contiguous

`tu_map.absorb_unlabelled` attaches a stray function to a cluster on call-graph
evidence **without extending that cluster's `[start,end)`**. So a unit's function
list can be scattered. `ov006` unit #64 is the proof: ten functions labelled
`dScMgMemory_c`, nominally `0x20f523c..0x20f5504`, actually spread over
`0x20f38f0..0x20f5504` with three other units sitting inside the gaps. Any
candidate search that reads the unit record as "a contiguous linker run" is
wrong. Contiguity has to be re-derived from `symbols.txt`: functions abutting
exactly, and no other function of the module inside the resulting span.
Enforcing that dropped the destructor-free 5–15-function candidate pool from
36 units to 30.

### 2.2 Destructor-freedom and sinit corroboration are mutually exclusive here

**Of the 36 sinit-corroborated modules in this ROM (`sinits == tus`), not one
contains a single destructor-free TU.** Every TU of every one of them carries a
D1/D0 pair. That is not a coincidence to work around; it is what the corpus is.
A corroborated module is an actor or scene overlay, and each of its TUs is one
polymorphic class whose destructor anchors it.

So §14's "independent corroboration where possible" and this pilot's
"zero destructor emission" cannot both be satisfied by the sinit gate, ever. A
TU with no class is not constructed by a static initialiser, so the sinit count
has nothing to say about it. The corroboration for this candidate had to come
from somewhere else, and §2.4 is where it came from.

### 2.3 Destructors are frequently unnamed, and the vtable knows

A first pass filtering on mangled `D[012]Ev` names produced a list of
"destructor-free" class-labelled TUs that were nothing of the sort: in
RTTI-labelled overlays the destructor pair is named `func_ovNNN_<addr>`. The
reliable test is **vtable slots 16 and 17** from `build/rtti_vtables.json`, which
is the D1/D0 pair for every class in this ROM's `fBase_c` hierarchy (verified
against `fBase_c`, `dBase_c`, `dScene_c`, `dScMgBase_c`, `dScMgD3DBase_c`,
`daObjFloatBoard_c` and others). Applying that killed every one of the
first pass's actor candidates.

### 2.4 Shape (b) is rarer than it looks, and ITCM is not TU-shaped

Two whole families of apparent shape-(b) candidates were rejected:

* **The `ov006` minigame scenes.** `dScMgMemory_c`, `dScMgMemory2_c`,
  `dScMgBSC_c`, `dScMgMCarlo_c` and `dScMgSingle3DBase_c` each present a clean
  contiguous run of vtable-slot methods with the class's destructors nowhere in
  it. They are not separate TUs: the destructors sit at a *lower* address in the
  same object, with unlabelled helpers between, and `tu_map` splits them only
  because the mangled-name span and the RTTI span do not overlap. For
  `dScMgSingle3DBase_c` the destructor pair abuts the run exactly
  (`0x210a4b0..0x210a534`, run starts `0x210a534`) — same object, cut in half by
  the labelling.
* **ITCM.** `itcm` offers two clean-looking runs, including an eight-function
  `MeshCollider` block. But ITCM is a *section collection*: the linker gathers
  each object's ITCM contribution, so an ITCM run is a TU's ITCM **fragment**,
  not a TU. `MeshCollider`'s own destructors and `MeshColliderBase`,
  `MovingMeshCollider` and `ExtendingMeshCollider` all live in `arm9` at
  `0x0203xxxx`, and the `DMAStartTransfer` block at `0x01ffde00` is called by
  `DMASyncHalfTransfer`/`DMASyncWordTransfer`/`DMASyncFillTransfer` in `arm9` at
  `0x0205a1xx`. Reconstructing an ITCM run as a standalone `.cpp` would be
  historically false by construction. **ITCM and DTCM should be excluded from
  this workstream's candidate pool the same way `main` is** — for a different
  reason, but just as firmly.
  (`MeshCollider` is separately disqualified: `CLAIMS.md` carries an active
  "Collision chain" conversion claim, and `MeshCollider::DetectClsn(SphereClsn&)`
  is unmatched.)

A direct search that bypassed `tu_map` entirely — cut every module's
address-sorted function list wherever the mangled class label changes — found
**171** single-class contiguous destructor-free runs of 5–15 functions. Every
one outside `main` is the *named tail* of an ordinary actor TU whose destructor
pair sits at its head with unnamed helpers between (`ov062/Chuckya` is the
canonical example: a "7-function Chuckya run" that is the last seven of a
36-function object). And every run belonging to a class with **no destructor
anywhere in the ROM** — `IRQ`, `GX`, `GXS`, `CP15`, `SaveData`, `Memory`,
`Message`, `ExpandingHeapAllocator`, `SolidHeapAllocator`, `NestedHeapIterator` —
is in `main`, which §14 excludes.

### 2.5 The candidate, and why its boundary is better than `tu_map`'s

`ov002` `0x020fe190..0x020fea4c`: the level-object loader. Seventeen functions,
fifteen of which are `_Z`-mangled **free functions with no class component at
all** and one identical parameter list,
`(LVL_Overlay::ObjSubTable&, int, unsigned)`.

The decisive evidence is a `.data` table this TU owns:

```python
data_ov002_0210cbb8   fifteen function pointers
  [ 0] -> 0x020fe8ac  LoadStandardObjects        [ 8] -> 0x020fe5cc  LoadFogObjects
  [ 1] -> 0x020fe6c8  LoadEntranceObjects        [ 9] -> 0x020fe4f0  LoadDoorObjects
  [ 2] -> 0x020fe6b8  LoadPathNodeObjects        [10] -> 0x020fe420  LoadExitObjects
  [ 3] -> 0x020fe6a4  LoadPathObjects            [11] -> 0x020fe40c  LoadMinimapTileObjects
  [ 4] -> 0x020fe690  LoadViewObjects            [12] -> 0x020fe3f8  LoadMinimapScaleObjects
  [ 5] -> 0x020fe960  LoadSimpleObjects          [13] -> 0x020fe3e4  LoadUnusedType13Objects
  [ 6] -> 0x020fe5f4  LoadTeleportSourceObjects  [14] -> 0x020fe3cc  LoadStarCameraObjects
  [ 7] -> 0x020fe5e0  LoadTeleportDestObjects
```

Fifteen entries, fifteen targets, all inside the span, nothing else. **Correction
(adversarial review):** the claim that those fifteen relocations are the *only*
references into the span was checked against the full reloc sweep and is off by
three — there are eighteen: the fifteen table loads, two intra-span calls to
`LoadObjects` (both from inside `Stage::LoadClsnAndObjects`, so "one caller"
still holds), and one external call from arm9 `0x0202d2b8` into the TU's public
entry point, `Stage::LoadClsnAndObjects` itself. All three extra references land
*inside* the boundary already drawn or call *into* it from outside in the
expected direction — none of them contradicts the boundary, they support it —
but the "only fifteen" sentence as originally written was wrong and is corrected
here rather than left standing. `LoadObjects` — which indexes that table — is
referenced by exactly one function in the whole ROM: `Stage::LoadClsnAndObjects`,
immediately below it in address order.

**Both ends are hard.** `_ZN3HUDC1Ev` ends exactly at `0x020fe190` and is the
last function of the `HUD` class object (which carries its own D1/D0). At the
other end, `tu_map` cuts at `0x020fea84`; that cut is **wrong**, and this pilot
corrects it. The four functions at `0x020fea4c..0x020feabc` are a `RaycastLine`
group, not loaders:

| addr | function | what it does |
| --- | --- | --- |
| `0x020fea4c` | `func_ov002_020fea4c` | copies a `Vector3` out of offset `0x0c` |
| `0x020fea68` | `func_ov002_020fea68` | copies a `Vector3` out of offset `0x00` |
| `0x020fea84` | `RaycastLine::Line::Set` | writes a `Vector3` **at** `0x00` and another **at** `0x0c` |
| `0x020feab8` | `func_ov002_020feab8` | empty stub |

The two unnamed ones are the getters for exactly the two fields `Line::Set`
writes, and `config/arm9/relocs.txt` shows all four called from one `main`
function at `0x02037624..0x02037788`, with the empty stub also called from
`_ZN11RaycastLineD1Ev`. They belong to `RaycastLine`, not here.

### 2.6 Criteria, honestly scored

| §14 criterion | Verdict |
| --- | --- |
| high-confidence contiguous TU boundary | **PASS.** Contiguity re-derived from `symbols.txt`, both ends bounded by demonstrably different objects. |
| independent corroboration | **PASS, by a different witness.** Not sinits (§2.2 proves that is unavailable for this shape) but the owned dispatch table, the single-caller chain, and the source-order reading in §3.2 — none of which was used to pick the boundary. |
| not an under-segmented module | **PASS.** `ov002` is not in `tu_map.json`'s `under_segmented` list (1511 functions / 79 TUs). |
| approximately 5–15 functions | **MISS — 17.** Stated rather than fixed: splitting the run to hit the count would have been an invented cut. See §7. |
| all functions already individually matched | **PASS.** 17/17 under `build_pin.verify`, not merely `complete` in `delinks.txt`. |
| zero destructor (D0/D1/D2) symbols | **PASS for the bar as meant** — no destructor of any class this TU defines, because it defines none. **See §5.1 for what it does emit.** |
| zero `_ZTV`/`_ZTI`/`_ZTS` emission | **PASS, literally.** Zero. Zero `.data`/`.rodata`/`.bss`/`.init`/`.ctor` sections at all. |
| no `#pragma opt_*` | **PASS.** No `#pragma` of any kind in any of the seventeen legacy sources or in the consolidated file. |
| no unexpected `.bss`/`.data`/`.rodata` | **PASS.** Verified by section inventory (§4), not by inspection of sources. |
| no by-value class-parameter exclusions | **PASS at the definitions.** One *callee*, `LoadMinimapChangeObject`, takes a by-value `Fix12<int>`; it stays hand-spelled as an `extern "C"` symbol exactly as the legacy file had it, so the exclusion never reaches a definition here. |
| small header blast radius | **PASS, unusually so.** `tools/affected_src.py include/LVL_Overlay.h` returns exactly fifteen files, and all fifteen are inside this TU. No header was edited. |
| no active `CLAIMS.md` row | **PASS.** No row covers `ov002 0x020fe190..0x020fea4c`. (The active "Collision chain" row is what disqualified the `MeshCollider` alternative.) |

---

## 3. Section order

### 3.1 The rule, re-probed

Not inherited from pilot #1. Four trivial functions, this compiler, these flags:

```cpp
//cpp
extern "C" int probe_A(void) { return 0x11111111; }   /* ... B, C, D */
```

```python
[5] '.text' 0xc  <- probe_D   (LAST in source)
[6] '.text' 0xc  <- probe_C
[7] '.text' 0xc  <- probe_B
[8] '.text' 0xc  <- probe_A   (FIRST in source)
```

Strict reversal, third independent confirmation.

### 3.2 On the real TU — and the reversal is evidence, not just mechanics

The file is written with `LoadSimpleObjects` (highest ROM address) first and
`Stage::LoadClsnAndObjects` (lowest) last, and the emitted order comes out as
ordinals 0..16 in exact ROM address order, with **no exceptions**. Unlike pilot
#1 there is no destructor variant group to break the rule inside a definition,
and no vague-linkage class body trailing the object — the only two extras are
4-byte value-type destructors and they land mid-object, not at the end (§4).

The direction is worth stating precisely, because "write it in reverse" makes it
easy to get backwards:

```text
compiled section order = ROM address order = reverse of SOURCE order
  =>  the ORIGINAL file's reading order = reverse of the ROM's addresses
  =>  the original file read exactly the way src_tu/stage/LevelObjects.cpp
      reads top-to-bottom
```

Read that way the file is textbook define-before-use: the fifteen category
loaders, then `LoadObjects` which dispatches to all fifteen, then
`Stage::LoadClsnAndObjects`, `LoadObjects`' only caller. And after
`LoadSimpleObjects` the remaining fourteen fall in **ascending handler-table
index** — `0, 1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14` — with only index 5
hoisted to the front.

Nothing about that ordering was used to choose the boundary; it is what the
mechanical reversal produced. That makes it a genuinely independent corroboration
of both the boundary and the reversal rule, of a kind the workstream did not have
before: a *semantic* check on a purely mechanical claim.

---

## 4. Object inventory

41 sections: 19 `.text`, 17 `.rela.text`, `.comment`, `.symtab`, `.strtab`,
`.shstrtab`, and the null section.

**No `.data`. No `.rodata`. No `.bss`. No `.init`. No `.ctor`. No
`.exception`/`.exceptix`. Zero `_ZTV`, `_ZTI` or `_ZTS` symbols of any kind.**

That is the whole point of this pilot's selection, and it is measured, not
argued. Pilot #1's equivalent inventory carried twelve `.data` sections.

### `.text` — 19 sections, 17 licensed

| | bytes |
| --- | ---: |
| licensed (the seventeen manifest functions) | 2236 = `0x8bc` |
| unlicensed (`_ZN7Vector3D1Ev`, `_ZN8Vector3sD1Ev`, 4 each) | 8 |
| **total emitted** | **2244** |

`0x020fea4c - 0x020fe190 = 0x8bc`, so every licensed byte is accounted for and
the entire surplus is those two 4-byte bodies.

### Relocations and imports

57 relocations, every one in `.text`, all `RELA`. 39 undefined imports, every one
a real ROM symbol. The two vague-linkage destructors have no relocations at all
(each is a bare `bx lr`), which is why there are 17 `.rela.text` sections for 19
`.text` sections.

`objisolate.plan` returns `error=None` for all seventeen and reports no unnamed
section-symbol relocation into a dropped section.

---

## 5. What consolidation forced

Pilot #1 recorded: *"Nothing contradicted anything — all repeated signatures
agreed."* **That does not generalise.** This TU had four genuine contradictions,
one of which no cast could reconcile. All four resolutions were measured
byte-free — that is what the 17/17 above means, since each function reproduces
its ROM bytes with its resolution applied.

### 5.1 The destructor-free hypothesis: held for classes, **not** for value types

The merged object emits `_ZN7Vector3D1Ev` and `_ZN8Vector3sD1Ev`, 4 bytes each,
`STB_LOPROC`. `include/types.h` gives `Vector3` and `Vector3s` inline
destructors, and the loaders build `Vector3`/`Vector3s` temporaries, so mwcc
emits vague-linkage out-of-line copies.

This is a real qualification on the brief's bar and it should be recorded as one:
**"no class of its own" does not imply "no destructor emitted".** A TU that
merely *uses* a value type with an inline destructor emits one.

It is not, however, caused by consolidation, and that was checked rather than
assumed — each legacy object was compiled alone and inventoried:

```sh
LoadExitObjects            .text x3   _ZN7Vector3D1Ev, _ZN8Vector3sD1Ev
LoadTeleportSourceObjects  .text x2   _ZN7Vector3D1Ev
LoadEntranceObjects        .text x2   _ZN7Vector3D1Ev
LoadStandardObjects        .text x2   _ZN7Vector3D1Ev
LoadSimpleObjects          .text x2   _ZN7Vector3D1Ev
(the other twelve)         .text x1   none
```

Six copies across five objects today; **two** in the merged object. Consolidation
*reduces* the surplus. No legacy object emits any `.data`/`.rodata`/`.bss`
either, so the merged object's cleanliness there is inherited, not new.

Contrast with pilot #1, where the unlicensed output was `_ZN18daObjKm2_Ami_Bou_cD2Ev` plus
`Platform`'s two out-of-line destructors plus twelve RTTI/vtable `.data`
sections — all consequences of owning a class. None of that shape is present
here.

### 5.2 CONFLICT — `func_0203accc`'s **arity**. The one no cast could fix.

`decl_common.h` (line 2116) declares `extern void func_0203accc(int);`.
`src/_Z19LoadPathNodeObjects...cpp` declares
`extern "C" void func_0203accc(void *entries, int areaID, u32 param);` and calls
it with three arguments. `Stage::LoadClsnAndObjects` includes `decl_common.h` and
calls it with one. Two `extern "C"` declarations of one symbol with different
arity cannot coexist in a translation unit, and no cast reconciles them.

The ROM settles it. Disassembling both call sites:

```arm
Stage::LoadClsnAndObjects        LoadPathNodeObjects (0x10, the whole function)
  mov r0, #0                       ldr ip, [pc, #4]
  bl  func_0203accc                ldr r0, [r0, #4]
                                   bx  ip
                                   .word func_0203accc
```

`Stage` never sets `r1`/`r2`, so a three-argument declaration there costs two
extra instructions and breaks the byte match. `LoadPathNodeObjects` is a tail
call that passes `r1`/`r2` through **by accident of the ABI, not by intent** —
the same four instructions come out whether the callee is declared to take them
or not. So the one-argument declaration is the only one that can be true of both
call sites, and the three-argument spelling was an inference from the tail-call
shape that the other caller contradicts.

Resolved to `func_0203accc((int)tbl.entries)`. Byte-free — `LoadPathNodeObjects`
is still `0x10` and `Stage::LoadClsnAndObjects` still `0x1ac`.

**This is the interesting class of finding for the workstream.** Per-function
compilation lets two files hold mutually exclusive beliefs about a ROM symbol
indefinitely, and no existing gate can see it: both files byte-match, both are
enrolled, and the link binds by name so the disagreement never surfaces. TU
consolidation is the only thing in this tree that forces the question.

### 5.3 CONFLICT — `Actor::Spawn`'s spelling, four inconsistent copies

Five of the seventeen call it. Four spelled the mangled symbol by hand as an
`extern "C"` free function; `LoadEntranceObjects` called the real
`Actor::Spawn` static method from `include/Actor.h`. One TU can only have one,
because the real method mangles to precisely the name the alias declares.

The four hand-spelled copies also disagreed *with each other*: `const Vector3 *`
vs `const Vector3 &` for the position, `Vector3s` vs a locally re-declared
`Vector3_16` for the rotation.

The hand-spelled form wins, and `LoadStandardObjects`' own comment records why:
`Actor::Spawn`'s real header gives the last two arguments as `s8`/`s16`, and
calling it as a method inserts truncates that make that function four bytes too
long, because `areaID` arrives as a full-word `int`. So `LoadEntranceObjects`
moves *off* the real method onto the alias — measured byte-free at `0x1e4`.

The same collision exists for `ActorDerived::Spawn` and resolves the other way:
`include/ActorDerived.h` arrives transitively through `Stage.h` → `Scene.h`, and
the real static method costs nothing, so both call sites use it and the
hand-spelled alias in `Stage`'s legacy file is dropped.

### 5.4 CONFLICT — `data_0209caa0`'s element type

`Stage::LoadClsnAndObjects` declared `extern int data_0209caa0[]` and reads
`data_0209caa0[2] & 0x80`; `LoadEntranceObjects` declared `extern u8
data_0209caa0[]` and reads `data_0209caa0[0x41]`. Both cannot be the element
type. The byte view wins because it is the finer one — index `0x41` is not
expressible as an `int` index — and `Stage`'s word read becomes an explicit
`((int *)data_0209caa0)[2]`, which is what the wider declaration was doing
implicitly. Byte-free on both.

### 5.5 CONFLICT — `data_0209f5c0`'s type

`ActorBase *` (Stage) vs `void *` with a cast back to `ActorBase *`
(`LoadEntranceObjects`). `ActorBase *` is the real type — it is the parent handed
to `ActorDerived::Spawn` — so the cast disappears. Byte-free.

### 5.6 Taken, measured free: shadow declarations retired

`LoadDoorObjects` was the outlier of the seventeen. It carried its own
`Vector3`, `Vector3_16`, `DoorEntry`, **and a third private spelling of
`class LVL_Overlay` with its own `ObjSubTable`** (`u8 b0; u8 count; DoorEntry
*entries;`) — layout-compatible with `include/LVL_Overlay.h`'s but a separate
type, and a globally-visible name that cannot survive in a real TU. Reconciled
onto the shared header; `0xdc`/`0xdc`, MATCH. Its header comment also claimed a
match under "mwccarm 1.2/sp2p3"; that is stale — `build_pin` pins it to
`2004/b56` like every other file here, and it verifies there.

`LoadStandardObjects` and `LoadSimpleObjects` each re-declared
`struct Vector3_16` locally, shadowing `include/common.h`'s. Dropped; both MATCH.

Two shadow declarations were **kept** deliberately, and neither is a duplicate of
an existing type:

* `DoorEntry` — `include/LVL_Overlay.h` has no record type for the door category
  yet. Its own banner says record types get added as their loader is migrated;
  adding one is an `include/` edit with a blast radius over the fifteen enrolled
  siblings, which this read-only pilot may not make.
* `LVL_Overlay_Layout` — `LVL_Overlay` in the header has **no data members at
  all**, only nested table and record types (that nesting is what makes the
  compiler emit `N11LVL_Overlay8ObjTableE`). The outer object's runtime shape is
  still unrecovered, and only `Stage::LoadClsnAndObjects` reads it.

### 5.7 Reconciled with no code change

* **Include sets.** Six different sets across seventeen files reduced to four
  headers: `decl_common.h`, `LVL_Overlay.h`, `Stage.h`, `MeshColliderBase.h`.
  The `Matrix4x3` ordering hazard (`common.h` first, flat `s32 m[12]` vs
  `math/Matrix.h`'s `{r, t}`) is not triggered: `decl_common.h` pulls `common.h`
  first and `Stage.h` → `Model.h` → `math/Matrix.h` stands down on the guard.
* **Language mode.** All seventeen were already `//cpp`. Unlike pilot #1 there is
  no `.c` → C++ transition to measure here.
* **`_Z11LoadObjects...` by hand.** `Stage::LoadClsnAndObjects` reached
  `LoadObjects` through the hand-spelled mangled name because that was the only
  way to reach another file's symbol. In one TU the `extern "C"` declaration of
  that literal name and the C++ definition that mangles to it are the same
  linker symbol, so the alias had to go and the call is now by name.
* **`Stage::LoadClsnAndObjects` was defined inside `extern "C" { }`** in the
  legacy file — inert for a member function, but stated wrongly. Now outside.

---

## 6. What did **not** get verified, and is not claimed

**Not run:** any link; `dsd check symbols --fail`; module byte comparison; ROM
build; symbol-address checks. §12's layers 4 through 7 are untouched. A
`text-verified` status means layers 2 and 3 only — plus, this round, the
relocation-destination check that sits between them.

**Not claimed:** that `data_ov002_0210cbb8` and the four small tables the loaders
index are byte-correct, or that they belong to this TU in the linker's sense.
Their contents were *read* (§2.5) as boundary evidence; they were not
reconstructed or compared.

**Not refuted:** pilot #1's vtable-addend bug (a TU that both defines a class's
vtable and names `_ZTV<Class>` by hand in a `<Class>_Spawn` factory stores the
vptr eight bytes low, invisibly to the byte gate). This TU defines no vtable and
contains no `_Spawn` factory, so the bug's precondition is absent. Nothing here
confirms or weakens it.

**One inference, flagged rather than buried:** the membership of
`Stage::LoadClsnAndObjects`. It rests on contiguity, on being `LoadObjects`' only
caller, and on the define-before-use reading — but it is externally linked, so
nothing *forces* it into this TU the way internal linkage would force a static
helper. The alternative reading is a one-function TU sandwiched between `HUD`'s
object and the loader's. That is possible and is not refuted here. Dropping it
would leave the other sixteen unchanged, and the manifest records this as a
caveat rather than as evidence.

---

## 7. Negative controls

Plan §12 lists these and pilot #1 did not run them. Three were run, on copies in
a scratch directory — `src_tu/` was never mutated:

| control | result |
| --- | --- |
| swap two ordinals (`LoadFogObjects` ↔ `LoadTeleportDestObjects`) | **DETECTED** — emitted section order diverges at ordinal 8 |
| add an unexpected helper | **DETECTED** — an extra `_Z30LevelObjects_unexpected_helperi` `.text` section appears |
| omit a declared function | **DETECTED** — `LoadViewObjects` absent from the emitted set |

The order control matters more here than it looks: per-function comparison
cannot see an ordinal swap at all (each function still reproduces its own bytes),
so it is exactly the failure a whole-range link would hit and a per-function gate
would pass. The first control also re-proves §3's reversal rule is load-bearing
rather than incidental.

A fourth observation from running the baseline through the same gate: an
inventory rule spelled as plan §7.5 item 1 — *"the defined function set equals
the manifest function set"* — **fails on this TU**, on the two vague-linkage
`Vector3`/`Vector3s` destructors. That rule as written cannot be satisfied by any
TU that uses a value type with an inline destructor, which is most of them. It
needs to become "equals the manifest set, plus deadstrippable vague-linkage
bodies, each of which must be `STB_LOPROC` and absent from the ROM's symbol
table" before it can gate anything.

---

## 8. What stands between `text-verified` and `link-verified`

For this candidate specifically, and the list is **shorter than pilot #1's** —
which was the point of choosing it:

1. **The two 4-byte vague-linkage destructors have nowhere to go.**
   `rombuild.py` links `-nodead`, so a deadstrippable body cannot be dropped the
   way the original link dropped it. Same shape as pilot #1's items 1 and 3, one
   sixth the size, and equally pre-existing: five of the seventeen legacy
   objects emit these today and the current per-function link already discards
   them.
2. **Nothing else.** Pilot #1's item 2 — the D2/D0/D1 emission order that no
   source arrangement can fix, and which it identified as *the* genuinely new
   obstacle consolidation introduces — **does not arise here at all**. The
   seventeen sections come out in exact ROM order. Its item 4 (vtable, RTTI,
   `.data`, `.bss`) does not arise either; there is none.

So the whole-range link this pilot exists to unblock has exactly one open
question against it, and that question is 8 bytes of `STB_LOPROC` `.text` that
the current build already handles.

The `.data` phase for `data_ov002_0210cbb8` remains, but it is a *later* phase
(plan §11 difficulty 5), not a blocker for a text-only link.

---

## 9. Answer to §18's open question

> How many already-matched functions remain matched when compiled together in
> their candidate original TU context?

**All of them, for this TU: 17/17, first compile, three gates green.**

Combined with pilot #1's 7/7 that is 24/24 across two TUs of deliberately
opposite shape. Of §10's twelve listed hazards, the ones that bit here were
**declaration and definition order** and **local shadow declarations replaced by
real headers** — four hard contradictions (§5.2–§5.5), none of which cost a
byte once resolved, and one of which (`func_0203accc`'s arity) was a real defect
in the recovered source that only consolidation could expose. Language mode,
pragma state, literal placement, static initialisation, vtable/key-function
emission, inlining and template instantiation all cost nothing, most of them
because this shape of TU cannot trigger them.

Three caveats on generalising:

* **This candidate is friendlier than average in one specific way and harder in
  another.** Friendlier: no class, so six of §10's twelve hazards are
  structurally unreachable. Harder: seventeen files rather than seven, written by
  more hands, with four real contradictions between them.
* **The bar "zero destructor emission" is not achievable in the strict sense.**
  §5.1: a TU with no class of its own still emits value-type destructors. The
  achievable bar is "no destructor *of a class this TU defines*", and the gate
  wording needs to say so.
* **Sinit corroboration is structurally unavailable for this shape** (§2.2), so
  the workstream needs the alternative witnesses this pilot used — owned dispatch
  tables, single-caller chains, and the source-order reading — written down as
  first-class evidence types rather than improvised per candidate.

On the evidence: **implement scratch linking against this TU first.** It is the
cleanest whole-range target in the tree — one contiguous range, exact section
order, no data, no vtable, and 8 bytes of surplus.

---

## 10. Repository invariants checked

* `git status src/ config/` reports only `config/tu_manifest.d/`. The seventeen
  legacy files are byte-identical to their committed state and still `complete`
  in `config/arm9/overlays/ov002/delinks.txt`.
* No file under `include/` was edited. `tools/affected_src.py include/LVL_Overlay.h`
  lists exactly the fifteen loader files, all of which are inside this TU.
* Nothing in the ROM-build path sees `src_tu/`: `grep -rn src_tu` over
  `enroll.py`, `eligible.py`, `rombuild.py`, `srcpath.py`, `progress.py` returns
  no hits. (`tools/tubuild.py` does read it — see §11.)
* `build/` is gitignored, so `build/tu/ov002-LevelObjects/` cannot be committed.

## 11. A concurrency note, because it affects reading this change set

While this pilot ran, **another session was working in the same worktree** and
added, as untracked files, `tools/tubuild.py` (a real implementation of plan §7),
`tools/test_tubuild.py`, and the ov045 falling-block shadow under `src_tu/actors/`
(then spelled FallBlockBfs.c, since renamed to the cartridge's own class name) —
and appended an `ov045/FallBlockBfs` entry to `config/tu_manifest.d/`.

This pilot's manifest entry was therefore **spliced in textually** ahead of the
closing bracket rather than written by re-serialising the file, so that entry and
the concurrent session's edit to the `ov045/daObjKm2_Ami_Bou_c` record are preserved
byte-for-byte. The result parses and carries three entries in order:
`ov045/daObjKm2_Ami_Bou_c`, `ov045/FallBlockBfs`, `ov002/LevelObjects`.

Nothing here was produced by or verified against `tools/tubuild.py`; every
measurement above was made by hand against `match.py`, `objisolate.py`,
`reloc_audit.py` and `build_pin.py` directly, exactly as pilot #1 was. Its
existence is recorded because `git status` will show it and because the two
efforts will need reconciling — a useful first test for it would be whether
`tubuild.py verify` reproduces the 17/17 above.

## 12. Change set

| Path | |
| --- | --- |
| `src_tu/stage/LevelObjects.cpp` | new — the shadow TU, not enrolled |
| `config/tu_manifest.d/` | modified — one entry appended, `ov002/LevelObjects`, status `text-verified` |
| `notes/tu-reconstruction-pilot-2-report.md` | new — this file |

Untracked build output at `build/tu/ov002-LevelObjects/` is gitignored, per §13
item 10.
