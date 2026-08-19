# The collision system: reference and plan

Written 2026-08-19. Built from primary evidence — ROM bytes, `config/**/symbols.txt`,
`config/**/delinks.txt`, `config/**/relocs.txt`, `build/rtti.json`, enrolled source —
with the pre-existing notes treated as leads to re-verify rather than as facts. Claims
are marked **[P]** proven from primary evidence, **[I]** inferred, **[N✓]** taken from a
note and re-verified.

Collision was last worked on **2026-08-06** (PR #1197); 586 commits have landed since.
Everything written before that date predates the class-rename campaign and one resolved
open question. Read §6 before trusting any of it.

---

## 1. Read this first: three facts that reorder the work

**1. The ROM carries the real name of every collision class, and we use coined names for
all of them.** The subsystem was compiled with RTTI on. A `_ZTS` string is not metadata
*about* a class — its bytes **are** the mangled class name, so a class named
`dBgCh_SphCrr` cannot produce the bytes `12dBgCh_SphCrr`. Until the family is renamed, no
collision class can become a key-function TU. See §2.

**2. The best draft of the biggest unmatched function is not the one the notes point
at.** Both notes send you to `notes/drafts-sphereclsn-detectclsn.cpp`. A materially
better draft is already banked in `nearmiss/db.jsonl`:

| | `notes/drafts-…cpp` | `nearmiss/db.jsonl` |
|---|---|---|
| size vs 0x1bc8 | 0x1af4 — **53 instructions short** | 0x1b58 — **28 short** |
| divergences | 1412 | **1213** |
| exactly-equal (`--align`) | 366 | **565** |
| ratio | 0.209 | **0.320** |

Same symbol, same `srcPath` field: the better attempt overwrote the draft, was banked,
and was never committed back to `notes/`. Nothing is lost — it is 27,216 chars of C++
sitting in `nearmiss/db.jsonl` today. **[P]**

**3. There is a second, independent collision implementation on an unlanded branch.**
`origin/port-mount-noseat-cluster` (1,143 commits ahead of `main`, last touched
2026-08-17) carries `port/unmatched/MeshCollider_DetectClsn_Sphere.cpp` — a 682-line host
transcription of the same 0x1bc8 function, read block-by-block off the ITCM disassembly
with every block annotated by its ROM address. It is **not** byte-verified, but it *runs*:
port gate 8 executes it against the real castle-grounds KCL. The two lines diverged
2026-08-04 and neither contains the other's work. See §5.

---

## 2. The naming wall

`build/rtti.json` gives the cartridge's own class names and inheritance edges. The
pairing to our coined names is settled by **vtable-address identity**, not inference. **[P]**

| cartridge name | former coined name | vtable | vtable symbol today |
|---|---|---|---|
| `dBgCh` | `dBgCh` | 0x020991d8 | `_ZTV5dBgCh` |
| `dBgCh_Actr` | `dBgCh_Actr` | 0x02099204 | `_ZTV10dBgCh_Actr` |
| `dBgCh_Gnd` | `dBgCh_Gnd` | 0x02099264 | `_ZTV9dBgCh_Gnd` |
| `dBgCh_Lin` | `dBgCh_Lin` | 0x020992a4 | `_ZTV9dBgCh_Lin` |
| `dBgCh_SphCrr` | `dBgCh_SphCrr` | 0x02099338 | `_ZTV12dBgCh_SphCrr` |
| `dBgPi` | `dBgPi` | 0x02099368 | `_ZTV5dBgPi` |
| `dBgPc` | *none — no header* | — | base of `dBgPi` at +0x04 |
| `dM3dGLin` | `dBgCh_Lin::Line` | — | non-polymorphic |
| `dM3dGSph` | *none* | 0x020994cc | `_ZTV8dM3dGSph` |
| `dBgW` | `dBgW` | 0x02099388 | `_ZTV4dBgW` |
| `dBgW_Kc` | `dBgW_Kc` | 0x020993dc | `_ZTV7dBgW_Kc` |
| `dBgW_KcMbg` | `dBgW_KcMbg` | 0x02099434 | `_ZTV10dBgW_KcMbg` |
| `dBgW_KcMbgSclY` | `dBgW_KcMbgSclY` | 0x02099490 | `_ZTV14dBgW_KcMbgSclY` |
| `dCc_c` | `CylinderClsn` | 0x0208e6ec | `_ZTV5dCc_c` |
| `dCcPos_c` | `CylinderClsnWithPos` | 0x0208e6bc | `_ZTV8dCcPos_c` |
| `dCcAc_c` | `MovingCylinderClsn` | 0x0208e6d4 | `_ZTV7dCcAc_c` |
| `dCcAcPos_c` | `MovingCylinderClsnWithPos` | 0x0208e704 | `_ZTV10dCcAcPos_c` |

The `_ZTS` symbols are **already spelled with the cartridge names in `symbols.txt`**
while the `_ZTV` beside them carries a coined one — a contradiction inside a single
symbol table. Read straight out of the cartridge (`extracted/arm9_dec.bin`, load base
**0x02004000**, confirmed three independent ways):

```
file 0x952f8  "12dBgCh_SphCrr\0"      file 0x95291  "9dBgCh_Lin\0"
file 0x951ee  "10dBgCh_Actr\0"        file 0x95225  "9dBgCh_Gnd\0"
file 0x95476  "14dBgW_KcMbgSclY\0"    file 0x8a669  "7dCcAc_c\0"
```

Itanium length prefixes, exact. `_ZTV12dBgCh_SphCrr` sits 0x40 bytes from a string that
says the class is `dBgCh_SphCrr`. **[P]**

Two coined names are not merely arbitrary but **misleading**:

- `dCcAc_c` is *actor*-attached (`Ac`), not *moving*. `MovingCylinderClsn` returns
  `owner->pos` and `owner->uniqueID`; the "moving" reading has no support.
- `dBgW_KcMbgSclY` is KCL + moving-bg + **scale-Y**. `dBgW_KcMbgSclY` extends by
  scaling in Y, and the cartridge name says which axis.

The tree already adopted this convention on the actor side (`Platform` → `dBgActor_c`,
#1575). Collision is the remaining holdout.

---

## 3. Architecture

### 3.1 Three families

```
dBgW      dBgW   the collidable world  ─┐ registered in a
 └ dBgW_Kc      dBgW_Kc       static KCL mesh  │ 24-slot global table
    └ dBgW_KcMbg   dBgW_KcMbg  + transform  │ data_020a0c80
       └ dBgW_KcMbgSclY  dBgW_KcMbgSclY     ─┘

dBgCh     dBgCh               a query against it    ─┐ each embeds a
 ├ dBgCh_Gnd    dBgCh_Gnd   "what is under me"  │ dBgPi (dBgPi)
 ├ dBgCh_Lin    dBgCh_Lin     swept segment       │ at +0x10
 ├ dBgCh_SphCrr dBgCh_SphCrr      penetration + push  │
 └ dBgCh_Actr   dBgCh_Actr    an actor's response ─┘ (no dBgPi; owns a
                                                        dBgCh_SphCrr + dBgCh_Lin)

dCc_c     CylinderClsn       actor-vs-actor volume ─┐ intrusive list head
 ├ dCcPos_c    CylinderClsnWithPos                  │ data_0209cee8, swept
 ├ dCcAc_c     MovingCylinderClsn (actor-attached)  │ once per frame from
 └ dCcAcPos_c  MovingCylinderClsnWithPos           ─┘ Stage::Render
```

**The query family is multiple inheritance, and the old note says it is not.**
`notes/collision-query-classes.md:21` concludes "an embedded polymorphic member, **not**
inheritance depth". The RTTI records are `__vmi_class_type_info` with public non-virtual
bases: **[P]**

```
dBgCh_Gnd    -> dBgCh @0, dBgPi @0x10
dBgCh_Lin    -> dBgCh @0, dBgPi @0x10, dM3dGLin @0x38
dBgCh_SphCrr -> dBgCh @0, dBgPi @0x10, dM3dGSph @0x38
dBgPi        -> dBgPc  @0        (single inheritance)
dBgW chain, dCc_c chain          (single inheritance throughout)
```

Corroborated by the secondary vtables' offset-to-top words (`0xfffffff0` = −0x10,
`0xffffffc8` = −0x38) and by eight `this`-adjusting thunks. The note's own open question
— "`func_020380ec` is *not* `_ZN5dBgPiD1Ev` … do not rename it" — was answered by
**#1206**, which landed after it: 0x020380ec is `_ZN5dBgPiD2Ev`, the **base-object**
destructor variant, which is exactly what a base subobject gets. The evidence the note
read as ruling inheritance out is in fact its signature.

Shared prefix, proven three ways (RTTI offsets, the three destructors, the thunk
adjustments):

```
0x00  dBgCh   0x10 B   vptr, u8 detectMask@0x04, s32 ownerID@0x08, dActor_c* owner@0x0c
0x10  dBgPi   0x28 B   the dBgPi the hit is written into — SECOND vptr
0x38  dM3dGLin (0x18) / dM3dGSph (0x14)  — THIRD vptr, SphCrr only
```

### 3.2 Sizes

| class | size | basis |
|---|---|---|
| `dBgCh` | 0x10 **[P]** | ctor writes 0x00–0x0c; `dBgPi` starts at 0x10 in all three |
| `dBgPi` | 0x28 **[P]** | three at 0x74/0x9c/0xc4 in SphCrr, stride 0x28 |
| `dM3dGLin` | 0x18 **[P]** | `Line::Set` writes two `Vector3` |
| `dM3dGSph` | 0x14 **[P]** | vptr, centre +0x04, radius +0x10 |
| `dBgCh_Gnd` | 0x50 **[P]** | header spans it, gate-clean |
| `dBgCh_Lin` | **0x88** **[I]** | header says **0x65 — wrong**, see §6 |
| `dBgCh_SphCrr` | **0x114** **[I]** | header spans 0x10c; `dBgCh_Actr` 0x134 − 0x20 |
| `dBgCh_Actr` | 0x1bc **[I]** | ~40 embedding actor headers put the next field at +0x1bc |
| `dBgW` / `_Kc` / `_KcMbg` / `_KcMbgSclY` | 0x20 / 0x50 / 0x1c8 / 0x1d0 **[P]** | asserted, gate-clean |
| `dCc_c` / `Pos` / `Ac` / `AcPos` | 0x30 / 0x3c / 0x34 / 0x40 **[P]** | asserted, gate-clean |

### 3.3 The mesh side

`KCL_File` (0x38) and `KCL_Tri` (0x10) are typed and documented in
`include/dBgW_Kc.h`, gate-clean. **[N✓]** Load path, all matched:

1. `Stage::LoadClsnAndObjects` (ov002 0x020fe190) — the level path.
2. `dBgW_Kc::LoadFile(SharedFilePtr&)` (0x02017afc) — per-actor; rebases only on the
   first reference.
3. `UpdateFileOffsets` (0x02039760) — turns the four header words 0x00–0x0c from file
   offsets into pointers. Nothing past 0x0c is a pointer.
4. `SetFile(KCL_File*, CLPS_Block&)` (0x020396f0) — stores the file, takes a CLPS
   reference, seeds the edge/vertex contact policy at `unk_28..unk_4d`.

Octree index per descent level:
`(z>>coordShift)<<zShift | (y>>coordShift)<<yShift | (x>>coordShift)`, with `coordShift`
decremented each level. `KCL_Tri::attribute` is used as a **raw, unmasked CLPS index** —
in this game the KCL attribute word *is* the CLPS index. **[P]**

**Unit basis.** World Fix12i → KCL file units is a **plain `>>6`**, the same six bits for
every collider in the game (`0x01ffb870 asr r1,r5,#6`; `DetectClsn(dBgCh_Lin&)` does the
same at 0x01ffb110). Routing the conversion through the collider's own `dBgW_Kc+0x2c`
instead reduces correctly for the level and for nothing else — `dBgW_KcMbg::SetFile`
leaves those words at 1.0, so every actor-owned collider runs its walk in world units
against a file-unit mesh and answers no to everything. Found on the port line, where the
symptom was that no ray could find the moat. **[P, port]**

A CLPS entry is 8 bytes; the bit layout is fully decoded from the matched accessors —
`w0 & 0x1f`, `(w0>>6)&0x3f`, `(w0>>12)&7`, `(w0>>15)&0xf`, `(w0>>19)&0x1f` (**surface
type**), `w0 & 0x20` (water), single bits at 0x1000000 / 0x2000000 / 0x4000000, and
`w1 & 0xff`. **[P]**

### 3.4 The query side

All three overloads are `dBgW_Kc` vtable slots 6/7/8, all in **ITCM**:

| overload | addr | size | status |
|---|---|---|---|
| `DetectClsn(dBgCh_Gnd&)` | 0x01ffd3f8 | 1,176 | **matched, enrolled** |
| `DetectClsn(dBgCh_Lin&)` | 0x01ffb0fc | 1,844 | unrecovered — draft at 203 divergences |
| `DetectClsn(dBgCh_SphCrr&)` | 0x01ffb830 | 7,112 | unrecovered — **largest unmatched function in the ROM** |

The drivers `dBgCh_Gnd::DetectClsn()`, `dBgCh_Lin::DetectClsn()` and
`dBgCh_SphCrr::DetectClsn()` are all matched and enrolled. Each walks the 24-entry table
`data_020a0c80`: slot 0 is the stage's static mesh and skips broad phase; slots 1–23 are
culled by `func_02035354` (self-collision — the query's `owner`/`ownerID` against the
collider's) and then by range. `dBgCh_Lin` culls on its own **`dM3dGSph` bounding sphere
at +0x64** — centre +0x68, radius +0x74. **[P]**

Table lifecycle: `dBgW::Enable(dActor_c*)` (0x02039184, 52 call sites) claims a
slot, `Disable()` (0x02039140, 154 sites) frees it, `Stage::ResetMeshColliders()`
(0x020391f4) zeroes all 24.

`dBgCh::ShouldPassThroughImpl` (0x02039488, matched, **static**) is the per-triangle filter:
CLPS bits against the query's `detectMask` at `dBgCh+0x04` — water, toxic, ordinary, and
surface types 0x11 and 0x14.

The floor/wall/underside classifier is `func_02039794`: `y > 0x600` floor, `y > -0xccc`
wall, else underside. **[P]**

### 3.5 The shape side

`dCc_c::Clear()` (237 call sites) and `Update()` (189) are called by actors from
their own behaviour; `Update` links the object at the head of `data_0209cee8`, so the
active set is rebuilt each frame by whoever chooses to register. `Process()` (0x02014aa8)
is O(n²) over that list and has **exactly one caller: `Stage::Render()` at 0x0202ba14** —
verified directly, `src/_ZN5Stage6RenderEv.cpp` is the only file in `src/` that names it.
**[P]**

*(Call-site counts in this section and §3.6 are reloc-table derived — they count sites,
not files, so they run higher than a `grep -l` over `src/`, which corroborates them at
179 / 136 files respectively.)*

`GetPos`/`GetOwnerID` are pure in the root. `MovingCylinderClsnWithPos` slot 3 is a
**linker interworking veneer**, not an override — raw words `e59fc000 e12fff1c 0201493c`.
**[P]**

### 3.6 What drives collision per frame

**There is no central mesh-collision sweep.** Every actor calls it from its own
`Behavior()`. The veneers `dBgCh_Actr_UpdateContinuous_Veneer` (44 call sites) and
`…UpdateDiscreteNoLava_veneer` (19) are spread across ov002/030/070/072/085/098/100;
`dBgCh_Gnd::DetectClsn` has 105 call sites and `dBgCh_Lin::DetectClsn` 57. The only
engine-driven collision call per frame is `dCc_c::Process()` inside
`Stage::Render()`. **[P]**

`dBgActor_c` is the other half: `UpdateClsnPosAndRot()` copies the model matrix, replaces
the translation row with the actor position and calls `dBgW_KcMbg::Transform`;
`IsClsnInRange()` enables/disables the collider by distance to the closest player, which is
the LOD gate on the 24-slot table.

---

## 4. Status

Membership rule: five contiguous address blocks holding collision code, plus every function
elsewhere whose symbol names a collision class. Enrollment read from
`config/**/delinks.txt` — a file is compiled into the ROM only if its entry carries
`complete`; otherwise dsd serves the range from the cartridge.

| | count | bytes |
|---|---|---|
| core collision functions | **330** | **35,840** |
| enrolled, byte-matching real C/C++ | **320 (97.0%)** | 26,756 (74.7%) |
| pure dsd passthrough, no source at all | **2** | **8,956 (25.0%)** |
| unenrolled 16-byte thunks (source exists, 4 bannered + 4 not) | 8 | 128 |
| `asm` / HAND-ASM bodies | **0** | 0 |

The two functions with no source are the two unrecovered ITCM `DetectClsn` overloads —
0x734 (1,844 B) and 0x1bc8 (7,112 B), read from `config/arm9/itcm/symbols.txt`.
**A quarter of the subsystem by size is two functions.**

Per class: dBgW_Kc 16 fn / 10,840 B · unnamed `func_*` 182 / 8,220 · dBgCh_Actr
27 / 5,860 · dBgW_KcMbg 15 / 2,824 · dBgW_KcMbgSclY 10 / 1,548 ·
dCc_c 8 / 1,428 · dBgCh_SphCrr 5 / 1,172 · dBgCh_Lin 6 / 900 · dBgW
19 / 764 · dBgCh_Gnd 4 / 676 · dBgCh 5 / 452 · dBgPi 6 / 304 · the three
`MovingCylinder*` families 20 / 712 · remainder small.

No collision function appears in `bytegate-known-failures.txt`, `rombuild-exclude.txt`,
`header-offset-known-issues.txt`, `layout-known-issues.txt`, or
`rombuild-versions.txt`. **[P]**

### A progress-accounting defect

Four thunk `.cpp` files — `func_020375b0`, `func_0203780c`, `func_02037d84`,
`func_02037da4` — carry **no banner**, so `tools/progress.py::synced_from_src` counts them
as matched (+4 functions, +64 bytes), yet `build_pin.verify` shows each emits
`_ZThn16_N7DerivedD0Ev` from a synthetic placeholder hierarchy rather than the `func_…`
symbol its filename claims. They are permanently unenrollable as written. Their four D1
twins *are* correctly bannered. Banner all eight or fix all eight. **[P]**

---

## 5. The port line, and why it matters here

`origin/port-mount-noseat-cluster` runs collision on the host. Port **gate 8**
(`port/slice_gate8.txt`, `port/tests/smoke_clsn.cpp`) loads the castle-grounds KCL through
the real card seam, wires the CLPS block, and casts vertical rays through the mesh,
asserting that floor hits come back with upward normals and hit points inside the probe
column. **Gate 9** puts a real actor's `dBgW_KcMbg` through the same path.

This is a **behavioural oracle the matching tree does not have**. A byte gate proves a
function reproduces the ROM; it cannot tell you an unmatched draft is *logically* right.
Gate 8 can, and did — the unit-basis bug in §3.3 was caught by Mario failing to swim in
the moat, not by any byte comparison.

Three things on that branch are harvestable into `main` with no byte risk:

1. **A fully typed `dBgCh_SphCrr` layout**, every offset pinned to the matched function that
   writes it and backed by an `offsetof` assert: `resolvedPush` 0x4c, `pushMin` 0x58,
   `pushMax` 0x64, `wallHeight` 0xec, `bestFloorNormal` 0xfc, `maxNormalY` 0x108. `main`
   still carries all six as `unk_*`. It also derives three more flag bits (gate, no-slope,
   no-corner) from `func_02037b5c`, a function `main`'s header never consulted.
   *(That file's own claim that `main` has "neither the centre nor the radius" is stale —
   `main` has both at 0x3c/0x48.)*
2. **The unit-basis rule** (§3.3).
3. **A vtable-shape constraint.** `CylinderClsn` declares its destructor first, so MSVC
   collapses the Itanium D1/D0 pair into one slot and every virtual past it lands one slot
   low — `GetPos` indexes what is `D0` in a ROM-shaped table. The ROM shape is
   `[0] D1, [1] D0, [2] GetPos, [3] GetOwnerID`, and the matched grab test
   `func_ov002_020caf98` calls slot 2 for `GetPos`, confirming it. Any host or portable
   build must keep the ROM shape and dispatch by explicit slot.

The 1,143-commit gap is mostly the class-rename campaign, which the port branch predates —
it still spells `Actor`, `Platform`, `Enemy`. Reconciling the whole branch is a separate
project. Harvesting the three items above is not.

---

## 6. Documentation that is wrong, and what to distrust

Audited against the ROM. The mechanism-level content of the collision notes is unusually
good — around thirty claims re-verified against the disassembly held up. The defects are
concentrated in structure, status, and numbers.

| where | claim | verdict |
|---|---|---|
| `collision-query-classes.md:21` | "embedded polymorphic member, **not** inheritance depth" | **WRONG** — RTTI says three public bases (§3.1) |
| `collision-query-classes.md:43` | "do not rename `func_020380ec` to `dBgPi`" | **SUPERSEDED** by #1206 — it is `_ZN5dBgPiD2Ev` |
| `collision-query-classes.md:56` | dBgCh_Lin's 0x38 member "is overlay-resident" | dtor **is** in ov002 (4-byte `bx lr`) but the *type* is not — `dM3dGLin`'s RTTI is in arm9 |
| `collision-query-classes.md` (0x5c) | "0x5c = `lineEnd.z`" | **TRUE** — retracted 2026-08-19; see the dual-role note below |
| `collision-query-classes.md:882` | wall slab is "**symmetric**", half-width `unk_ec + radius` | **WRONG** — asymmetric `[-(ec+rad), ec-rad]`; the note quoted four instructions and missed the `sub` |
| `collision-query-classes.md:886` | a wall "wholly **outside** the slab is not a real contact" | **INVERTED** — all three vertices *inside* ⇒ reject |
| `collision-query-classes.md:332` | `sp+0x40`'s `orr r0,r0,#4` is the floor-hit bit | **WRONG**, retracted only half — it is the und path |
| `handoff-sphereclsn-detectclsn.md:41` | "0x1b40, 34 instructions short, 0.5968 / 1051" | **STALE** — re-measured 0x1af4, 53 short, 0.5983 / 1048 |
| `handoff-sphereclsn-detectclsn.md:63` | "**Every mechanism is written.** Nothing structural is missing." | **WRONG** — its companion note calls the wall block "the largest unwritten region" in the same PR |
| `handoff-sphereclsn-detectclsn.md:18` | "of **16** installed mwccarm builds" | **STALE** — 25 installed |
| `handoff-…md:14`, several headers | "build at 2004/b56, not 1.2/sp2p3" | **redundant** — #1619 dropped all pins; it is the tree-wide default |
| `drafts-sphereclsn-detectclsn.cpp:2` | "first draft — head/AABB only, the walk and prism tests are stubs" | **WRONG** — 618 lines, every mechanism written |
| `include/dBgCh_Lin.h` | spans 0x65 | **understated** — ≥0x78 (0x88 inferred); the `dM3dGSph` at +0x64 is missing entirely. The field naming is NOT wrong — see below |
| `include/dBgW.h:40` | "the symbol names are on the wrong bodies" | **WRONG and self-contradicting** — its own bullets say what `symbols.txt` says; #1203 settled it. Delete the paragraph |
| `include/dBgCh.h:41` | `ShouldPassThroughImpl` is static "which is what the mangled name spells" | **WRONG reasoning** — Itanium never encodes `this`; the declared signature also has one arg too many for the ROM's 4-register call |
| `itcm.md:65` | 6 unnamed dBgW_Kc funcs, "88 bytes" | **WRONG** — 0x54 = 84 |
| `itcm.md:384` | "the full 13-slot map is in `include/dBgW_Kc.h`" | **WRONG pointer** — it is `dBgW.h:11` |
| `itcm.md:533` | "the **only** read of `KCL_Prism::length`" | **WRONG** — `DetectClsn(dBgCh_Gnd&).cpp:172` reads it too; and the type is `KCL_Tri` |
| `dtor-variant-audit.md` | "enrolled destructor migrations in this tree: **zero**" | **WRONG** — `src/_ZN7dBgW_KcD1Ev.cpp` is one |
| `docs/class-reference.html` | `dCc_c` has "2 live slots" | **WRONG** — 4. **FIXED 2026-08-19**; the same defect also hid `dFader_c` (2→10) and `mHeap::Heap_t` (2→16) |
| `docs/class-hierarchy.html` §6 | dBgCh_Gnd and dBgCh_Actr "have no `type_info` record" | **WRONG** — both do; §4 of the same page contradicts it |
| `docs/class-reference.html` | built 2026-08-07, pre-rename | **regenerate** — stale paths, `unknown_class` joins its own data disproves |
| `archive/func_ov079_02124008-floor.md` | "near-miss banked" | **STALE** — that function matched and is enrolled |

#### Retraction: `dBgCh_Lin` 0x54 is not misnamed

A first pass of this survey reported that 0x54 should be `clsnPos`, not `lineEnd`. **That was
wrong, and it rested on an inverted premise** — that `func_ov002_020fea4c` is `GetStart`. Read
the body: `a[0..2] = b[3..5]` reads offset 0x44, the `dM3dGLin`'s *second* `Vector3`, so it is
**GetEnd**; `func_ov002_020fea68` (`b[0..2]`) is GetStart.

So `func_02037608` seeds 0x54 from the line **end**, and `dBgW_KcMbg::DetectClsn`
depends on that, transforming 0x38 and 0x54 as the two endpoints of the scratch segment. On a
hit, `func_020375ec` does `d[21..23] = s[0..2]` — `d[21]` is 0x54 — so the collision point
overwrites the end point, which is why `GetClsnPos` reads it back under that name.

**0x54 is dual-role: line end in, collision point out.** Both names are right at different
times; `lineEnd` is the correct declaration name because it is what callers pass in. Recorded
in the header. The lesson generalises — do not rename a field on one accessor's naming without
reading the function that *writes* it.

**Trust ratings.** `rtti-reconciliation.md` and `handoff-class-skeleton.md` have the right
answer on this hierarchy and are the notes the others should defer to.
`collision-query-classes.md` is a good *disassembly gazetteer* and a bad *class model* —
use it for ROM mechanism, never for structure. `handoff-sphereclsn-detectclsn.md` needs its
top half rewritten. The mesh and cylinder headers are in good shape; the four query-side
headers are auto-generated placeholders with no bases, no virtuals and no size assert.

**Gaps nothing covers:** the cartridge names (§2); multiple inheritance as a codegen
constraint (what the C++ must look like to emit the observed thunks); `dBgW`
itself; `dBgCh_Lin+0x64`; `dBgCh_Actr` and the whole cylinder family, which have no
note at all; and KCL as an asset — `notes/assets.md` has no KCL section.

---

## 7. The plan

Ordered by unblocking value over risk. Phases 0–2 are prerequisites for 3 and 4; within a
phase, slices are independent.

### Phase 0 — Correct the record — **DONE 2026-08-19** (branch `clsn/phase0`)

The cheapest work in the list, and it stops the next person re-deriving wrong things.
Verified end to end: **rombuild 106/106 exact, 100.000000%, 0 mismatching**; eligibility
bracketed against HEAD with an **identical name list** (11061 both sides, zero lost, zero
gained) and the denominator down exactly 4 by design; `check_header_offsets` 0 mismatched /
0 unparsed; four consumers of the edited headers byte-verified at 2004/b56.

- **0a.** **DONE.** Both bodies re-scored on one worktree at 2004/b56 before swapping:
  old `cand=1725 equal=366 ratio=0.2090` (53 short) → new `cand=1750 equal=565
  ratio=0.3203` (28 short). **+199 exactly-equal instructions.** Provenance and the
  swept-lever list are now in the file header.
- **0b.** Fix the wrong header prose: `dBgW.h:40` (the conclusion contradicts
  its own bullets), `dBgCh.h:41` (the mangling claim), and document `dBgCh_Lin.h`'s
  dual-role field. **DONE 2026-08-19**, byte-verified on four consumers.
- **0c.** **DONE.** Both notes gained a defect table at the top, and the three most
  dangerous spots are marked inline. The wall block was re-read from the ROM to confirm:
  `01ffcfa8 add r1,r6,r3` then `01ffcfb8 sub r1,r6,r3` — `r1` is redefined, so the slab is
  `[-(ec+rad), ec-rad]`, asymmetric; and the final `01ffcfe0 ble → 01ffd314` is the
  per-prism `continue`, so three vertices INSIDE rejects.
- **0d.** **DONE.** Root cause was `read_slots` in `tools/rtti_vtables.py` popping trailing
  nulls unconditionally. Right when the walk merely ran out of road; wrong when the next
  record's offset-to-top has *proven* the table end — then those nulls are real
  pure-virtual slots. Gated the pop on a `bounded` flag. Blast radius across all 426
  classes: **3 changed, all increases, zero decreases, 24 slots recovered** — `dCc_c` 2→4,
  `dFader_c` 2→10, `mHeap::Heap_t` 2→16. `dFader_c` landing on exactly ten independently
  matches the ten-slot fader hierarchy from #1259.
  **Two traps:** `rtti_vtables.py` rewrites `build/rtti_vtables.json` on *every* run, so a
  comparison run with the old code leaves a stale intermediate and the page regenerates
  self-consistently wrong — regenerate `rtti_extract` → `rtti_vtables` →
  `evidence_hierarchy` → `rtti_reconcile` → pages. And `rtti_reference.py` needs
  `build/eligible-names.txt` present or the page silently reports **0 enrolled**.
- **0e.** **DONE — bannered all eight.** The four `.c` D1 twins were already bannered and
  their text already diagnosed the defect; the four `.cpp` D0 twins were not. `build_pin`
  confirms each: "func_… is not in the object 2004/b56 produced". Candidates 11189 → 11185,
  numerator unchanged at 11061. Decoded each thunk from the cartridge while bannering:
  three adjust −0x10 and one −0x38, matching the RTTI base offsets `dBgPi`@0x10 and
  `dM3dGSph`@0x38, and both dBgCh_SphCrr thunks tail-branch to the same D0 at 0x02037c40 —
  one class, two bases.

### Phase 1 — Adopt the cartridge names — **DONE 2026-08-19** (branch `clsn/phase0`)

All four slices landed, one `git mv` + content pair each:

| slice | classes | symbols |
|---|---|---|
| `dCc*` cylinders | 4 | 35 |
| `dBgW*` mesh spine | 4 | 59 |
| support types (`dBgPi`, `dM3dGLin`) | 2 | 9 |
| `dBgCh*` query family | 5 | 51 |

**The last five vtables are deliberately NOT named.** `dBgCh`, `dBgCh_Actr`, `dBgCh_Gnd`,
`dBgPi` and `dM3dGSph` have `_ZTS` and `_ZTI` but no `_ZTV`, so their triples are still
incomplete. Naming them was tried and backed out: it raises
`codegen_hacks.extern_vtable` **347 → 374** and `extern_vtable_classes` **188 → 193**,
failing the langmode ratchet. Nothing gets worse when you name them — those 27 sites
always stored a vptr through an extern array instead of a real polymorphic class, and
the metric is *textual*, so hiding behind `data_020991d8` was the only reason it did not
count them. **The naming and the 27 storage sites have to move together**, which is the
same Phase 2c/3 work that converts them; doing it then drops the count honestly instead
of needing the root `langmode-baseline.json` override the tree spent six cycles closing
(#1632, deleted in #1615).

Byte-neutral throughout: rombuild **106/106 exact, 100.000000%, 0 mismatching** after every
slice, `check_references` 45 vs baseline 45, `port_refcheck` 393/0 before and after, and
**154 renamed with credit intact, 0 changed, 0 lost**.

Two measurable side effects, both the point of the exercise: rombuild's "unnamed by config"
fell **844 → 784**, so 60 data symbols became attributable purely from the spelling agreeing
with the ROM's own `_ZTS` bytes; and the `SphereClsn` draft's symbol moved twice yet still
scores `cand=1750 equal=565 ratio=0.3203`, so Phase 0's gain survived intact.

#### The recipe, as it actually works

Two commits per family — `git mv` only, then content. That is what keeps
`prepush_attribution` reporting *renamed, credit intact* instead of lost.

Substitution order inside the content pass is load-bearing:

1. **Digit-prefixed mangled encodings first, unbounded** — `12MeshCollider` → `7dBgW_Kc`.
   Doing plain names first turns `_ZN12MeshCollider4InitE` into `_ZN12dBgW_Kc4InitE`, a
   length prefix that no longer matches its spelling (the defect #1580 cleaned up).
2. **Include guards**, `DECL_` variants before the bare ones.
3. **Plain class names LAST, and word-bounded** (a regex word boundary at each end).
   Unbounded, `MeshCollider`
   rewrote `Stage::ResetMeshColliders` into `ResetdBgW_Kcs`. `layout_check.py` caught it —
   delinks named a path with no file behind it, which would have let the function fall back
   to ROM bytes silently. Bounding cut 489 corruptions out of 2,755 substitutions, including
   309 `mMeshCollider` members.
4. **`decl_<Class>` needs its own explicit rule.** `\b` does not fire across an underscore,
   so 90 `#include "decl_MovingMeshCollider.h"` lines survived a bounded pass while the
   header had already been renamed. Sweep for unresolvable includes afterwards.

Order the families easiest-first; within a family everything moves at once, because the
class name appears inside *other* classes' mangled symbols wherever it is a parameter type.

Rename the 17 classes in §2 to their ROM names. This is what lets any collision class
become a key-function TU, and it is a prerequisite for Phase 4, not a cosmetic follow-up.
It also lets the six currently-`data_*` vtables be named.

Sequencing, from the tree's own scar tissue:

- One class family per PR, one commit pair per chain step — the attribution permutation bug
  is real and this family is large (`dBgCh_Actr` alone has 27 symbols and 63 external
  veneer call sites).
- Grep **other branches** for each old name before landing; a rename has broken three
  sibling PRs before.
- Bracket `eligible.py` around every shared-header edit and diff the name list, not just
  the count.
- Check `config/rombuild-versions.txt` before renaming (currently one unrelated pin, so
  this should be clear).
- `port_refcheck.py` is blind to mangled names — run it, but do not treat it as coverage.
- Expect and accept `CREDIT LOST` on files that become key-function TUs.

Suggested order, easiest first: `dCc*` (4 classes, self-contained, headers already good) →
`dBgW*` (4, single-inheritance chain, headers good) → `dBgPi`/`dBgPc`/`dM3dG*` (support
types) → `dBgCh*` (5, the multiple-inheritance family, largest blast radius, do last).

### Phase 2 — Complete the headers *(no byte risk if done as declarations only)*

- **2a.** Give `SurfaceInfo` a real header. It is 0x14 bytes,
  `{ CLPS clps /*0x00, 8B*/; Vector3 normal /*0x08*/; }`, and today **11 `src/` files
  define it ad-hoc with six mutually contradictory bodies**. Highest-risk item in the
  subsystem: six definitions of one type is how a wrong one silently wins.
- **2b.** Type `CLPS` and `CLPS_Block`; the bit layout is already decoded (§3.3). Give
  `data_020a0c78`, `data_020a0c80` and `data_0209cee8` typed declarations.
- **2c.** Promote the four query headers to real classes with their bases, virtuals and
  size asserts (§3.1–3.2). Harvest the `dBgCh_SphCrr` field names from the port branch (§5).
- **2d.** Correct `dBgCh_Lin.h` to its real span and add the `dM3dGSph` member at 0x64.

Every header edit here needs the `eligible.py` bracket — a shared-header change reaches
neighbours, and `check_header_offsets` is blinded by span-form padding.

### Phase 3 — The two ITCM bodies *(the 25%)*

- **3a. `DetectClsn(dBgCh_Lin&)`**, 1,844 B, draft at 203 divergences. Do this **first**:
  it shares the octree walk verbatim with the already-matched `dBgCh_Gnd` twin, so it
  is the calibration run for 3b. Its recorded floor is a `this` register allocation
  (r7 vs r8) at 2004/b56.
- **3b. `DetectClsn(dBgCh_SphCrr&)`**, 7,112 B, best draft 28 instructions short after 0a.

  What is already **swept and dead** — do not re-walk: nineteen declaration-level variants
  across five classes of lever (hoist, permutation, re-scoping, folding, moving `en1`/`en2`),
  all byte-neutral with positive controls; frame-size chasing (the surplus is register
  pressure from structural difference, not a frame problem); loop rotation (mwccarm rotates
  this loop unprompted and always did). The lore that "declaration order IS the stack
  layout" holds for the 0x498 twin and **not** for this 0x1bc8 function.

  What is left is source-shape change that reduces simultaneous liveness across the prism
  body — the draft reloads edge-normal pointers from the frame (`ldr [sp,#0xc4]` three
  times in ten instructions) where the ROM keeps `en1` in r5 and `en2` in r4 and spills
  only `en3`.

  Score with plain `--align` now that the work is codegen; `--align-shape` normalises away
  stack offsets and will read flat across real gains. `mismatches=N/M` is frozen at 999
  until the sizes match.

  ```
  python tools/fdiff.py --c notes/drafts-sphereclsn-detectclsn.cpp \
    --name _ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr \
    --module itcm --addr 0x01ffb830 --size 0x1bc8 --version 2004/b56 --align
  ```
  `--module itcm`, never `arm9/itcm`.

- **3c.** Cross-read the port transcription (§5) against the draft. Two independent readings
  of one function is a rare asset; where they disagree, one of them is wrong about the ROM,
  and gate 8 can adjudicate the semantics that byte-scoring cannot.

### Phase 4 — Naming debt *(mechanical, after Phase 1)*

182 of the 330 core functions still carry `func_*` names (8,220 B). All build and match, so
this is pure naming. Many are already derivable from the vtables: `func_020354d0/e0` and
`func_02035504` are `dBgCh` D2/D0/D1, `func_02035514` its ctor, `func_020373b8`
`dBgCh_Actr` D0, `func_020374f0` `dBgCh_Gnd` D0, `func_02037710` `dBgCh_Lin` D0,
`func_02037c40` `dBgCh_SphCrr` D0, `func_02038114`/`func_0203819c` `dBgPi` D0/C2,
`func_0203ac2c/50/60/70` `dM3dGSph` D0/D1/C1/C2, plus the eight `_ZThn…` thunks. Largest
unnamed: `func_02036acc` (1,048 B, a `dBgCh_Actr::Update*` sibling), `func_02038824`
(532), `func_0203842c` (368), `func_02038a38` (320).

Naming a method can emit a vtable — check `eligible-names.txt` after adding any virtual
declaration.

### Free-standing quick win — measured 2026-08-19

`dEnemyBase_c::UpdateYoshiEat(dBgCh_Actr&)`, ov002 0x020ade78, 0x3cc. **Two instructions
from matching:** `align target=243 cand=243 ratio=0.9877, equal=240, replace=1, insert=2,
delete=2`.

One of the two reported divergences is not real — `fdiff` labels `+0x3c0` **`reloc
(wildcard)`** itself: the ROM word `14220802` is little-endian `0x02082214`, the address of
`data_02082214`, which an isolated compile leaves as zero. The whole residual is a single
scheduling swap of two independent instructions:

```
ROM   ldrh r7,[r0,#0x8e]   then   add r1,r6,#0x5c
ours  add r1,r6,#0x5c      then   ldrh r7,[r0,#0x8e]
```

Four levers swept against it, all inert or worse, so do not re-walk them: re-spelling the
RMW load side plainly (**worse** — drops an instruction, so the
`((long long)(int)x) & 0xFFFF...` laundering on *both* sides is load-bearing); moving the
`ang`/`idx` declarations ahead of `rnd`; moving `rnd` between the load and `idx`; and
binding the store address to a `int *pz` temporary declared after the load. mwcc schedules
the address materialisation first regardless of source order.

**The draft was also unusable until now.** Its banked entry was keyed to
`_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn` — a symbol two rename campaigns out of date —
and its body still defined that name. Re-keyed and re-spelled; it compiles and scores again.

---

## 8. Definition of done, per phase

- **0** — the draft in `notes/` scores 1213, not 1412; the four wrong prose claims are gone;
  `class-reference.html` reports 4 slots for `dCc_c`; all eight thunks agree on banner
  policy.
- **1** — **MET for the names, PARTIAL for the triples (2026-08-19).** Every collision
  `_ZTV`/`_ZTI`/`_ZTS` that exists spells the cartridge name, but five classes still
  have no `_ZTV` at all — see Phase 1 above for why naming them is coupled to Phase 2c; zero `_ZTS<coined>` anywhere in `config/`; the `eligible.py` name-list
  delta is exactly the renamed symbols each time; ROM build still 106/106 exact.
  *Checking against this wording is what caught the five missing `_ZTV` symbols — "no
  coined names left" and "every triple complete" are not the same claim, and only the
  second one is the goal.*
- **2** — `SurfaceInfo` defined once; `check_header_offsets` clean with the query classes
  actually spanned (not skipped as polymorphic); no `src/` file carries a private
  redefinition of a collision type.
- **3** — both ITCM overloads enrolled with `complete`; collision passthrough drops from
  8,956 bytes to 0; subsystem goes 74.7% → 99.6% of bytes (the last 128 is Phase 0e's
  thunks).
- **4** — zero `func_<addr>` symbols in the five collision address blocks.

## 9. Risks

- **The rename is the whole risk surface.** Everything in Phase 1 touches shared headers
  with wide blast radius. Mitigations are in §7 Phase 1 and are all things this tree has
  been burned by before.
- **`SurfaceInfo`'s six definitions** (2a) may not all be layout-compatible; one of them is
  `{char dummy;}`. Fix the definition before assuming any consumer is correct.
- **Phase 3b may not close.** It has resisted five sweeps. The plan deliberately puts 3a
  first so that effort produces a match either way, and 0a means 3b starts 25 instructions
  ahead of where the notes suggest.
- **The port branch is 1,143 commits of divergence.** §5 recommends harvesting three
  specific artefacts, not merging. Do not let this become a branch-reconciliation project
  by accident.

## 10. Reproducing every number here

```bash
# cartridge class names and inheritance edges (dict: records + edges)
python -c "import json;d=json.load(open('build/rtti.json'));print(list(d))"
# RTTI strings straight from the cartridge; the LENGTH-PREFIXED form is the mangled name
python -c "import pathlib;b=pathlib.Path('extracted/arm9_dec.bin').read_bytes();i=b.find(b'12dBgCh_SphCrr\0');print(hex(i),hex(0x02004000+i))"
# enrollment: a file is compiled only if its delinks entry says complete
grep -c complete config/arm9/delinks.txt
# the banked drafts
python -c "import json;[print(json.loads(l)['name'],json.loads(l)['divergences']) for l in open('nearmiss/db.jsonl',encoding='utf-8')]"
grep -n dBgCh_SphCrr config/match_attempts.jsonl
# score a draft (module itcm, never arm9/itcm)
python tools/fdiff.py --c notes/drafts-sphereclsn-detectclsn.cpp \
  --name _ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr \
  --module itcm --addr 0x01ffb830 --size 0x1bc8 --version 2004/b56 --align
# the port line
git show origin/port-mount-noseat-cluster:port/unmatched/MeshCollider_DetectClsn_Sphere.cpp | head -60
git show origin/port-mount-noseat-cluster:port/slice_gate8.txt
```

Do not run `rombuild.py` concurrently with `eligible.py` — they share `build/` and invent
link errors.
