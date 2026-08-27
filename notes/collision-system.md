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

**2. SUPERSEDED -- the biggest unmatched function is unmatched no more.**
`DetectClsn(dBgCh_SphCrr&)` byte-matches as of 2026-08-21 and lives at
`src/game/actors/dBgW_Kc/_ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr.cpp`, enrolled complete. The
notes draft it pointed at is deleted; its history is this file's Phase 3b
sections and git. (The original fact, kept for the record: the notes draft
and the nearmiss bank disagreed, and the bank was better -- check both before
starting work on any function.)
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

```sh
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

```sh
dBgW      dBgW   # the collidable world  ─┐ registered in a
 └ dBgW_Kc      dBgW_Kc       static KCL mesh # │ 24-slot global table
    └ dBgW_KcMbg   dBgW_KcMbg  + transform # │ data_020a0c80
       └ dBgW_KcMbgSclY  dBgW_KcMbgSclY    # ─┘

dBgCh     dBgCh              # a query against it    ─┐ each embeds a
 ├ dBgCh_Gnd    dBgCh_Gnd  # "what is under me"  │ dBgPi (dBgPi)
 ├ dBgCh_Lin    dBgCh_Lin    # swept segment       │ at +0x10
 ├ dBgCh_SphCrr dBgCh_SphCrr     # penetration + push  │
 └ dBgCh_Actr   dBgCh_Actr    # an actor's response ─┘ (no dBgPi; owns a
                                                        dBgCh_SphCrr + dBgCh_Lin)

dCc_c     CylinderClsn       # actor-vs-actor volume ─┐ intrusive list head
 ├ dCcPos_c    CylinderClsnWithPos                 # │ data_0209cee8, swept
 ├ dCcAc_c     MovingCylinderClsn (actor-attached) # │ once per frame from
 └ dCcAcPos_c  MovingCylinderClsnWithPos          # ─┘ Stage::Render
```

**The query family is multiple inheritance, and the old note says it is not.**
`notes/collision-query-classes.md:21` concludes "an embedded polymorphic member, **not**
inheritance depth". The RTTI records are `__vmi_class_type_info` with public non-virtual
bases: **[P]**

```text
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

```text
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
verified directly, `src/game/stages/Stage/_ZN5Stage6RenderEv.cpp` is the only file in `src/` that names it.
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
| `dtor-variant-audit.md` | "enrolled destructor migrations in this tree: **zero**" | **WRONG** — `src/game/actors/dBgW_Kc/_ZN7dBgW_KcD1Ev.cpp` is one |
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
- **3b. `DetectClsn(dBgCh_SphCrr&)`**, 7,112 B. **MATCHED 2026-08-21 --
  0/1778 under the build flags, enrolled at
  `src/game/actors/dBgW_Kc/_ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr.cpp`.** The dated sections
  below are the full campaign record (1493 -> 978 -> 253 -> 125 -> 36 -> 31 ->
  15 -> 0); read them before touching the file, and note two flagged untrue
  constructs remain as open readability work. It began the campaign at
  1213 / 565 / 0.3203.

  Progression: 565 -> 601 (block layout) -> 816 (declaration order) -> 825 (edge
  blocks) -> 855 (min/max duplicate arms) -> 1047 (a fresh-context pass) -> **1304**
  (the permuter session below).

  ### The whole remaining defect turned out to be one thing

  Every gain in the 1047 -> 1304 run is the **same mechanism**: mwccarm common-
  subexpression-eliminates a load that the ROM re-issues. Each fix re-reads a value
  the candidate already had in a register. **None of them changes what the function
  computes** - every one assigns a variable the value it already holds, or spells an
  identical read differently.

  | gain | edit |
  |---|---|
  | **+153** | `en3 = f->normals[tri->edgeNormal3Idx];` between the two `KCL_VERTEX` rounds. mwcc was sharing the three `en3[i]` loads across both rounds. **One line.** It took the alignment's `delete` count from 100 to 21 |
  | **+56** | the sphere centre re-bound per *component* in the slab block, **together with** reading `cr` through a `(volatile s16 *)` view. Separately these are **-27** and **-64**; only the pair pays |
  | **+6** | six branch-polarity flips in the Voronoi dispatch - `if (MUL10(nn, dotA) > dotB) goto edgeN; goto vXX;`. The ROM branches *into* the vertex block. Exactly +1 each, additive, all 64 subsets swept |
  | **+42** | the prism origin read through a pointer alias `tpv` at five of the six vertex-tail sites. Found by the permuter, then swept over all 64 subsets |
  | **+31** | two declaration moves, `rawX/rawY/rawZ` and `den12/den23/den31`. Not the same mechanism - this one is the frame. Found by a greedy sweep over **every declaration line x every position**, 3,135 compiles a round, converged in three |
  | **+36** | the `tpv` alias **re-swept after the frame fix** - round 1 now reads the array directly, only round 2's `[0]`/`[1]` go through the pointer. The slot map found it: the ROM's three `tp` slots are `(2,2,0)` and ours were `(1,2,5)`, so the ROM never takes `tp`'s address at all. Sixth time a swept lever came back after a structural change; this is what took `cand` to exactly 1778 |
  | **+29** | **the frame lever.** `tp`/`vb`/`vc` retyped as `Vtx3` - an aggregate with a user-declared empty destructor - because mwccarm was scalarizing `vb` and `vc` where the ROM keeps them as arrays. Frame `0x1bc -> 0x1b4`, slots `105 -> 102`, both exactly the ROM's, and the load deficit reaches zero. **This one is +29 on the anchored count and only +3 on `equal`** |

  After these, the structure is essentially finished: **27 instructions of drift
  summed over all 35 call-gap regions** (was 39, and before that a single region was
  +168 on its own), and the load deficit that drove all of the above is down to 2.

  ### Believe `equal` only with a second opinion

  `equal` comes out of a difflib alignment, and an alignment can **re-anchor**: a
  change that deletes instructions can raise `equal` by letting a long downstream
  block line up again, without anything actually getting closer. The first candidate
  the permuter produced did exactly that - it scored +136 by deleting a dot product
  it was not allowed to delete.

  So every step was also scored on an **alignment-free** metric: the 34 call sites
  are anchors both streams agree on, and inside each gap instruction *i* is compared
  to instruction *i* with no slack at all (`anchored.py`).

  | draft | `equal` | anchored exact | cand |
  |---|---|---|---|
  | session start | 1047 | 496 / 1778 (0.2790) | 1734 |
  | after +153 | 1200 | 558 / 1778 (0.3138) | 1745 |
  | after +56 / +6 | 1262 | 574 / 1778 (0.3228) | 1752 |
  | after +42 | 1304 | 629 / 1778 (0.3538) | 1777 |
  | after +31 | 1335 | 656 / 1778 (0.3690) | 1777 |
  | after the frame | 1338 | 685 / 1778 (0.3853) | 1780 |
  | now | **1374** | **765 / 1778 (0.4303)** | **1778** |

  That last row is the case for keeping the anchored metric. The frame fix moved
  `equal` by **three** and the anchored count by **twenty-nine**; judged on `equal`
  alone it would have read as noise and been thrown away.

  Both metrics move together at every step. **Rebuild the anchored check before
  believing any future gain on this function.**

  ### The permuter: how it actually earned its keep

  `tools/permuter/` was pointed at this function for the first time. It needed three
  things:

  1. **Re-cloning `vendor/decomp-permuter`** - gitignored, so it was not in the
     worktree and not on the machine at all. The four Windows-compat patches in
     `tools/permuter/README.md` had to be re-applied; all four are still correct.
  2. **A C99 stand-in for the C++ draft.** The randomizer is pycparser-based and
     cannot see a member-function definition, a reference parameter, a namespace-
     qualified call or a virtual call. `mkbase.py` mechanically flattens all of them
     (headers to flat structs at the same offsets, `extern "C"` dropped, `cstd::fdiv`
     to `_ZN4cstd4fdivEii`, the slot-3 dispatch hand-rolled). That is legitimate only
     because `check.py` proves the two compile to the same bytes: **9 differing words
     out of 1,777**, both accounted for - a four-slot permutation in the prologue's
     zero-init, and a five-instruction reordering of the virtual dispatch, which
     mwcc's C++ frontend emits as `mov r0,this` then `ldr r3,[r0]` where C emits
     `ldr r3,[sl]` and moves later. One trap worth recording:
     `(*(void (**)(...))p)(...)` costs a whole extra instruction over
     `((void (*)(...))p)(...)` - two casts, two loads.
  3. **Re-scoring every output with fdiff.** The permuter's Scorer and our `equal`
     are poorly correlated here; its own best-scoring candidate was not in the top
     ten by `equal`.

  What it produced, per run:

  | run | base | wall | best on our metric | what it was worth |
  |---|---|---|---|---|
  | 1 | 1045 | 2 min | 1181 | **the hint that unlocked +153** - semantically invalid (it read `cr` across a rewrite of `cr`), but a +136 from deleting arithmetic said the ROM does not compute what we compute there |
  | 2 | 1198 | 35 min | 1207 | +9. Not worth the wall clock |
  | 3 | 1256, **`--stack-diffs`** | 30 min | 1283 | the `tpv` pointer-alias idea, which swept out to **+42** |

  **`--stack-diffs` is not optional here.** Without it the scorer ignores stack
  offsets, and stack offsets are the single largest remaining defect. Runs 1 and 2
  were without it and returned +9; run 3 with it returned the alias and the first
  candidates ever to reach the exact 1,778 instruction count.

  Treat it as a **hypothesis generator, not an optimiser**: two of the four gains
  above started as permuter output and neither was taken as written. Its
  transformations are not always semantics-preserving - it narrowed a parameter to
  `char`, and it moved reads across writes of a `volatile` array - so **every output
  must be read before it is believed**, then re-swept by hand, because it samples one
  arbitrary subset of a lever the byte gate can enumerate exhaustively.

  ### Where the remaining 443 divergences are

  `544` of the alignment's `replace`s, and **340 of those are one thing: the stack
  frame.** `str r0,[sp,#0xc4]` against `str r0,[sp,#0x104]` - the same instruction on
  a different slot.

  **Declaration order is that lever, and it is now exhausted.** The greedy sweep
  converged after two accepted moves (+25, +6) with a third round finding nothing;
  the recorded verdict it overturned was *"377 compiles, ZERO improving moves"*, which
  had been measured before the four CSE edits changed the allocator's input. Fifth
  time the re-test rule has paid here. **The next lever on the frame is not a source
  reorder** - and the frame is what is left.

  Also outstanding: 85 mnemonic-class replaces (down from 134; the six branch flips
  were the largest family and are closed) and 27 pure register renames (down from 51).

  Re-running the other three axes on the post-reorder structure moved nothing - 186
  compiles: all 64 `tpv` site subsets, all 64 polarity subsets, all 58 re-read
  insertion sites. Each is at its optimum for this structure.

  ### The frame SIZE is solved; the frame ASSIGNMENT is not, and not from source

  102 slots on both sides, at the same total, and **78 of the 102 offsets now carry
  identical traffic**. The other 24 hold the wrong variable, and nothing expressible
  in the source moves them. Everything below was measured on the current structure,
  after the frame fix, and is a negative:

  | axis | candidates | result |
  |---|---|---|
  | declaration order, one line x every position | 8,464 (twice) | zero improving moves |
  | order of the ten hoisted constant initialisations | 90 | inert |
  | **block scope** | probe | hoisting the existing inner declarations to the top is **byte-identical** - mwccarm does not key slot assignment on lexical scope at all, so the axis closes in one measurement |
  | redundant re-reads, 7 pointers x every use site | 58 | nothing |
  | `cr` read spelling, dot product x vertex tail | 48 | every spelling byte-identical |
  | slab block: rebind granularity x interleave x chain split | 12 | the draft is the optimum |

  Two specific residuals are named and **not** source-reachable:

  * The ROM's busiest slot is `[sp,#0x94]`: a `s16 *` into the normals array, stored
    once and read **21 times**. It is `en3` - the draft's own note says the ROM keeps
    `en1` in r5, `en2` in r4 and spills `en3`. Ours carries **20** loads. The one
    missing read is inside an expression, not between statements: every statement that
    mentions `en3` has had a redundant re-read inserted before it and all are inert.
  * `cr` sits at the same offset on both sides, but the ROM materialises its address
    **15** times to our **12**. All 48 spellings of those reads are byte-identical.

  So the remaining slot difference is a consequence of live ranges the source cannot
  reach, not of an ordering. The productive direction is the other 88 mnemonic-class
  and 72 register-class replaces, concentrated in call-gaps 28 (79), 0 (62) and 6 (56).

  ### The tree has two definitions of "the C++ flags", and the gates use the wrong one

  Worth knowing before quoting any `//cpp` byte score:

  ```
  swarm.CPP_FLAGS         = match.DEFAULT_FLAGS + -lang c++   ->  -w illpragmas
  mangle / name_roundtrip = rombuild.CFLAGS     + -lang c++   ->  -Cpp_exceptions off
  ```

  `-w illpragmas` is a warning switch. **`-Cpp_exceptions off` is codegen.** The build
  that produces the ROM uses the second (`rombuild.py:411-414` swaps the language on
  the `//cpp` marker); every tool that *verifies* -- `fdiff`, `bytegate`, `abverify`,
  `reverify_corpus`, `nearmiss_db`, `reloc_audit` -- uses the first.

  **Measured blast radius: nil.** 550 enrolled `//cpp` files compiled under both and
  compared byte for byte -- a 300-file spread sample of all 4,235, plus 250 of the 536
  that contain a destructor, which is the population `-Cpp_exceptions` should actually
  affect. **Zero differences.** So this is not a live gate defect and the shared
  constant is deliberately left alone; changing it is tree-wide risk for no measured
  benefit.

  It is not nil on **this** draft, which is now the unusual case: a destructor on a
  stack local inside loops that `continue` out of its scope.

  ```
  fdiff / verify   (-w illpragmas)        equal 1374   cand 1778   frame 0x1b4
  rombuild / SHIPS (-Cpp_exceptions off)  equal 1375   cand 1778   frame 0x1b4
  ```

  One instruction, and **the shipping flags are the better one**, so nothing measured
  this session is invalidated -- the real build is one instruction closer than every
  number recorded above. Score this function with the build flags from here.

  ### The permuter path is now CLOSED on this function

  The SROA block needs a C++ destructor, and `mkbase.py`'s C99 stand-in cannot express
  one - the same aggregate without a destructor is byte-identical to the *pre-frame*
  draft, which is exactly the point. So there is no longer a C base that compiles to
  the same bytes, and a permuter run would be optimising a different function. It can
  come back if a C-side SROA blocker is ever found; dead `&x`, references and launders
  are already known not to work.

  ### Swept and dead ON THIS STRUCTURE

  Re-run these after any structural change - that rule has now paid four times.

  * **58 redundant re-read insertions** for `fn`/`en1`/`en2`/`en3`/`vtx`/`tri`/`c` at
    every use site: all byte-neutral or worse. The `en3` win is **not** one of a
    family; it is the only site where the CSE spanned two rounds.
  * a **32-way product** of centre-rebind granularity x `cr` spelling x collider-axis
    spelling: nothing beats what is in the draft.
  * all **36 non-degenerate (cross, denominator) pairings** of the KCL vertex formula.
    The draft's is the documented Mario Kart Wii form and stays. Several alternatives
    score higher - every one of them is either degenerate (`cross(n,X) . X == 0`, so
    the guard would always fire) or reuses a denominator. They win by removing
    arithmetic, not by being right. **Do not take them.**

  ### A correction carried in from the reloc table

  There **is** a square root - four of them. The ROM drives the DS hardware unit by MMIO
  (`0x040002b0` SQRTCNT x8, `0x040002b4` RESULT x4, `0x040002b8` PARAM x4, each inside an
  IME `0x04000208` critical section), so it never appears as a call and a reloc-table read
  says "no sqrt". Sites: `0x1ffc2a8`, `0x1ffc418`, `0x1ffc588`, `0x1ffca30` - the three
  edge gates and the shared `do_sqrt`. Both drafts already model it.

  ### Nothing here waits on the C++ conversion

  ITCM is an **autoload** (`config/arm9/config.yaml:11`) - a linker section memcpy'd at
  boot - so it constrains no source language; 15 of the 27 enrolled ITCM files are `.c`
  and 12 are `.cpp`, and the sibling `DetectClsn(dBgCh_Gnd&)` byte-matches today as
  `//cpp`. There is exactly **one** virtual dispatch in all 1,778 instructions
  (`0x1ffbe60 blx r3`, slot 3 on `this`, already-matched `GetSurfaceInfo`) and **zero**
  through the `dBgCh_SphCrr&` parameter, so promoting that class (Phase 2c) is a
  readability convenience here, not a prerequisite.

  ### Reproducing

  Score with plain `--align`; `--align-shape` normalises away stack offsets and will
  read flat across real gains. `mismatches=N/M` is frozen at 999 until the sizes match.

  ```
  python tools/fdiff.py --c src/game/actors/dBgW_Kc/_ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr.cpp \
    --name _ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr \
    --module itcm --addr 0x01ffb830 --size 0x1bc8 --version 2004/b56 --align
  ```
  `--module itcm`, never `arm9/itcm`. A compile+score round trip is **~0.45 s warm**,
  so a sweep here should be exhaustive over an axis, never a sample of it.

  ### Appendix: the 2026-08-19 analysis, SUPERSEDED

  Kept because a measured negative is still evidence, and deleted evidence gets
  re-derived. But **every verdict below was measured on a source structure that no
  longer exists** -- the +153 / +56 / +42 edits above each changed the register
  allocator's input, and that has invalidated a recorded dead-lever list four times
  on this function now. Read it for the *mechanisms* it names, never for its
  numbers, and re-run anything you intend to rely on.

  In particular, two of its conclusions are now known to be wrong:

  * *"region A may not be reachable from local source"* -- it was reached, twice.
  * *"the next productive lever is probably another global one"* -- the four gains
    since were all local, and all one mechanism (a CSE the ROM does not make).

  ### 2026-08-20, third session: 1493 mismatching words -> 978

  `python tools/fdiff.py --align` on the draft, before and after:

  ```
  before   ratio 0.7728   equal 1374   mismatches 1493/1778
  after    ratio 0.8121   equal 1444   mismatches  978/1778
  ```

  Both were taken with the flags that BUILD the ROM (see "two definitions of the
  C++ flags" below), and the bank entry is re-scored from the banked body, not from
  the notes body -- `divergences` 404 -> 334.

  Two of the findings generalise past this function.

  ### Later the same day: 978 -> 253, and the count was lying to us

  ```
  after round 1   ratio 0.8121   equal 1444   mismatches 978/1778
  end of day      ratio 0.8341   equal 1483   mismatches 253/1778
  ```

  Bank `divergences` 404 -> 213. Instruction count 1778, frame 0x1b4, and **every
  call-gap length** now exactly the ROM's. The whole-function instruction multiset
  is **surplus 3 / deficit 3**, one pair of which is literal-pool data that capstone
  renders as `andeq` -- so two real instructions are wrong function-wide and
  everything else is register and stack-slot allocation.

  ### The sqrt region was the SUM OPERAND ORDER

  mwccarm evaluates a two-term 64-bit sum RIGHT TO LEFT: the `smull` takes the right
  addend and the `smlal` accumulates the left. The ROM emits `d^2` then `faceDot^2`,
  so the source has to read `fh*fh + dh*dh`. Written the other way round, hoisting
  the two `>> 4` shifts makes `axisDot` spill instead of staying in a callee-saved
  register -- which is exactly the failure that got the hoist rejected **four**
  separate times and recorded here as a re-anchoring artifact. With the terms
  swapped, all six sqrt gaps (7/8, 11/12, 15/16) become the ROM's exact code.
  Control: swap them back, -95 anchored.

  Two related negatives from the same 2,922-candidate sweep: the ROM's `mov r2, r3`
  in the 64-bit store is EMERGENT once the shifts hoist, not a source construct (ten
  of twelve store spellings are byte-identical -- leave `SqrtRaw` alone); and the
  three per-block `hyp` copies really are three separate block-scope locals.

  ### `cand == 1778` WAS A COINCIDENCE, NOT AN INVARIANT

  This is the methodological lesson of the day. ROM gap 7 is 29 words and gap 8 is 26
  -- 55 per site -- where the draft had 27+27 = 54. **The sqrt region was three words
  short and gap 28 was three words long, and the two cancelled.** Treating the total
  as a correctness invariant nearly rejected the largest fix of the session.

  **Check per-gap lengths (`shapegap.py`), never the total.** Once the sqrt region was
  right, gap 28's three surplus words were the only gap-length mismatch left, and
  three surplus words displace everything after them: removing them took 978 -> 302
  in one step.

  ### The slab bounds are not locals

  The ROM materialises the upper bound LAZILY, between the first and second compare:

  ```
  add r1,r6,r3 / rsb r2,r1,#0 / cmp r0,r2 / blt reject / sub r1,r6,r3 / cmp r0,r1
  ```

  A named local cannot produce that split -- it forces both to be emitted together.
  Lazy split materialisation is the signature of a CSE'd repeated expression, so both
  bounds are now written out in full at all six comparison sites. The two tests
  recorded here previously (move `t`/`u` later; give them their own `lo`/`hi` locals)
  both used named locals and so could not have found it. Came from the independent
  port transcription and was ROM-verified before use.

  ### The axis dot: operand order AND term order

  `FX12(unk_28, (v).x) + FX12(unk_30, (v).z) + FX12(unk_2c, (v).y)` -- axis in Rm, and
  the terms x, z, y. Worth 302 -> 253 and took gap 28's norm-miss from 135 to 53. All
  144 operand x term x association orderings were enumerated. **This axis had been
  measured twice the same day at +1 and +0**; it only pays once gap 28's length is
  right, because until then the whole tail was displaced and the gate could not see it.

  ### A fourth invented construct retired

  The `s32 *tpv` pointer alias -- worth +42 and then +36 in earlier sessions -- is
  dead and byte-neutral to delete. The ROM has ZERO `add r,sp,#0x18c`; it reads the
  three prism-origin slots directly. With `Vtx3`, the volatile cross-product array and
  four of six no-op re-binds already gone, **the file contains no untrue construct.**

  ### End of day: 253 -> 125, and the instruction multiset is EXACT

  ```
  ratio 0.9325   equal 1658   mismatches 125/1778
  ```

  Bank `divergences` 404 -> 120. Count, frame, **every call-gap length AND the
  whole-function instruction multiset** are now the ROM's -- the only entry left in
  the multiset diff is an `andeq` pair, which is literal-pool DATA that capstone
  renders as an instruction. **Every instruction in the function is the right one.**
  All 125 remaining differences are scheduling and register/slot allocation.

  The final gain was an interaction: `AXIS_DOT0` back in NATURAL x, y, z order
  **together with** deleting both remaining `c = &sphere.pos;` re-binds. +67, and it
  erased the entire 64-instruction shape run in gap 28. Neither half works alone --
  the natural term order takes shape 1747 -> 1768 but costs one instruction, and
  deleting the re-binds saves that instruction but is -29 by itself.

  Note the trap in the earlier record: a sweep here was logged as "multiply operand
  order in AXIS_DOT0, all 8". That was operand order INSIDE `FX12`. **Term order --
  the order of the three addends -- is a different axis and had never been swept.**

  The rest is declaration order, re-run after that landed: `fn`/`depth` just after
  `rawX` (+25), `c` just after `den31` (+15), `rad6` before `origin` (+10), `cy`
  before `cz` (+8), `-(radius + unk_0ec)` (+3).

  ### 125 -> 15: ask the compiler for the frame map

  ```
  ratio 0.9916   15 mismatching words / 1778
  ```

  Bank `divergences` 120 -> 34. Every declared local now sits at the cartridge's own
  frame offset except two, and those two are exactly swapped.

  **The method is the result.** Two sessions were spent identifying one variable's
  stack slot by hand; it came out wrong twice, in opposite directions, once into a
  commit. mwccarm emits DWARF under `-g`, the flag is byte-neutral, and
  `DW_AT_location` gives every local's frame offset by name. And because the two
  instruction streams are position-aligned at 1778, the ROM's slot referenced at
  position *k* holds whatever ours holds there -- so naming our frame names the
  CARTRIDGE'S frame. That turned "109 slot-only mismatches" into five named variables
  in the wrong place. `scratchpad/perm/frame2.py` does it.

  Three gotchas, all of which bit: pyelftools needs
  `get_dwarf_info(relocate_dwarf_sections=False)`; a `DW_AT_location` that is a loclist
  comes back as an `int` and `bytes(<int>)` silently yields a zero buffer, dropping half
  the variables; and **absence of a DWARF entry is not proof a variable left the frame
  chain** -- ten "wins" in one sweep were byte-identical.

  ### The frame model, corrected

  The frame is `[outgoing args][chain][spill pool][aggregates]`. The **chain** holds the
  declared locals mwcc made memory-resident, in DECLARATION ORDER, contiguously -- so
  its length sets where the spill pool begins, and one surplus chain word displaces
  every pool slot. On this function that single 4-byte surplus was worth 44 references.

  **A SECOND DEFINITION moves a local out of the chain into the pool.** That is what
  `c = &sphere.pos;` does, and it is worth +24 here. The second definition has to be in
  a different block -- on the next line it is a dead store and mwcc folds it. A local
  assigned a compile-time constant is coalesced regardless, which is why `k0/k1/z108/…`
  live in the pool.

  **Callee-saved registers go to the six hottest locals, numbered r9 down to r4 in
  declaration order, and the seventh loses and takes a chain slot.** Moving a hot local
  earlier makes it win a register and pushes the loss onto whoever is now last; moving
  the whole group earlier rotates every callee-saved register (strict 1652 -> 1308).

  Those two rules look like they conflict when the cartridge wants the spilled local
  EARLY in the frame. They do not: the chain is ordered only over locals that HAVE
  slots, and the six register winners have none. So `en3` can be declared after en1/en2
  and still be the 34th slot. That is the declaration order the draft now uses.

  The previous version of this section said pool membership "is derived from USE" and
  that the only lever was reducing definition count. Both were wrong, and the second is
  backwards.

  ### Two retractions

  * "136 slot-only mismatches are NOT reachable from declaration order" was wrong -- and
    wrong because the sweep behind it was measuring the wrong variable. Nearly all of
    them are now fixed.
  * "the one lever that moves a variable between pools is reducing its DEFINITION
    COUNT" is backwards. A second definition moves a local OUT of the chain.

  ### What the re-bind actually does

  `c = &sphere.pos;` written a second time is worth +24, and the obvious reading --
  that it forces the cartridge's reload of `c` -- is **false**. Delete it and the
  prologue emits exactly the same two reloads. What it does is give `c` a SECOND
  DEFINITION, which moves it out of the declaration chain into the spill pool; the
  chain is then one word shorter and every spill slot lands on the ROM's address
  instead of four bytes high. Forty-four references from one word.

  The lever is fussy in a way that is itself informative. It moves `c`
  (`&sphere.pos`, one `add` from fp). It does NOT move `f` (`this->kclFile`, a load)
  or `rsc` (`sphere.radius << 4`, a load and a shift). **It needs a value that
  rematerialises without touching memory.**

  ### Count the relocations out

  The raw word diff against the cartridge is 65, and 34 of those are link-time
  wildcards: 33 `bl` targets, plus ONE DATA WORD at index 1046 (ROM `0x020a0cec`,
  ours `0x00000000`) which is the unrelocated `&data_020a0cec` the source already
  passes to `func_02037fd4`. An earlier note called that word "a literal-pool word,
  not an instruction" and carried it as an unfixable defect. It is a relocation. The
  honest remaining count is **31**, and it was 32 before this session's one win.

  ### The two remaining defects are independent

  Worth stating because it was assumed the other way for two sessions: compile with
  the re-bind dropped, so `c` is chained at 0xc4 where the cartridge has it, and the
  prologue schedule comes out instruction-for-instruction identical -- only the slot
  number moves. The fifteen scheduling words are **not** downstream of the sixteen
  slot words.

  ### 32 -> 31: decouple the load order from the operand order

  Index 1502 is `add r1, r6, r3` in the ROM and `add r1, r3, r6` in ours, where r6 is
  `sphere.unk_0ec` and r3 is `sphere.radius`. Flipping the source terms was measured
  twice and rejected twice: it re-orders the two LOADS at 1499/1500 instead and takes
  the `sub` at 1506 with them, three words for nothing.

  The reason is that one expression was carrying two orderings at once. Written as
  `-(sphere.radius + sphere.unk_0ec)`, the load order and the add's operand order are
  welded together and mwcc takes both from the source. Name the two reads and they
  come apart:

  ```c
  s32 ext = sphere.unk_0ec;
  s32 rad = sphere.radius;
  if (da >= -(ext + rad) && da <= ext - rad && ...)
  ```

  Loads in the cartridge's order, add in the cartridge's order, nothing else touched.
  This is the same shape as the earlier `s32 dX = rawX - origin->x;` win, and the
  general rule is worth keeping: **when a spelling is pinned by two orderings at
  once, give the operands names and the orderings separate.**

  ### 31 -> 15: the slot swap, solved by a zero-cost volatile round-trip

  The whole 16-word slot defect came down to one thing: the cartridge chains `c` at
  0xc4 and pools `rsc` at 0x104, and we had it the other way round. Pool membership is
  conserved, so the task was to demote `rsc` into the pool while leaving `c` chained.
  Roughly 500 compiles across eight mechanism families had failed to move it.

  Two edits:

  ```c
  /* 1. delete the re-bind -- c then chains at 0xc4 by itself */
  /* 2. after `rsc = sphere.radius << 4;` add: */
  rsc = (s32)(volatile s32)rsc;
  ```

  Casting a prvalue to a cv-qualified scalar type discards the qualifier, so **that
  statement generates no code at all** -- the count stays exactly 1778 -- but it marks
  the value volatile inside mwcc's front end and that demotes `rsc` out of the
  declaration chain into the coalesced-locals band of the temp pool, landing on 0x104.
  Frame afterwards: den31 0xc0, c 0xc4, rawX 0xc8, rawY 0xcc, rawZ 0xd0, rsc pooled.
  All 99 slots now agree with the cartridge on address and reference count.

  **The flavour of the demotion picks the landing zone**, which is why nothing else
  worked:

  | second definition | effect |
  |---|---|
  | `rsc + k0` (k0 == 0 at runtime) | folds and forgets -- no demotion |
  | `rsc * k1` (k1 == 1) | demotes only to the annex at 0xd4, right after the last chained local |
  | volatile round-trip | demotes into the pool's coalesced band at 0x104 |

  And a structural fact that explains why the 0xd4 "gap" was never usable: **the pool
  always begins at chain end + 8**, one structural pad word, so any chained placement
  of rsc -- 0xd4 included -- shifts every temp by four.

  This is a MATCHING HACK, not a reconstruction. No 2004 author wrote
  `(s32)(volatile s32)rsc`. What it establishes is the mechanism, plus one fact about
  the original: the cartridge's rsc is a **demoted named local, not an inline
  subexpression** -- spelled inline, mwcc recomputes rather than building the 0x104
  temp and the count goes to 1779. Finding the spelling the author actually used is
  open work.

  ### What is left: 15

  Fifteen words of prologue scheduling, indices 2-17, and nothing else anywhere in the
  function. Same 17 instructions, same registers, different order. The cartridge does
  both parameter moves first, then computes and stores `c`, and only then loads `f` --
  so `ldr r0,[sl,#0x20]` clobbers the add result and all three centre reads come from
  reloads, {y,z} off one and {x} off a second. We load f between the two moves, the add
  result survives, and it serves `c->x` directly.

  It is downstream of **one allocator decision**, not of statement order: whether the
  add result is forwarded to the first centre read. That invariant held across every
  size-preserving variant of ~350 compiles, because mwcc canonicalises these local
  rewrites to the same IR before allocation. Byte-neutral on this structure: both
  orders of the c and f assignments, either or both moved into the block, origin taken
  from kclFile, rad6 hoisted, a volatile view on the f load, empty `asm { }` blocks,
  `do {} while (0)` wrappers, goto/label splits, and hoisted extra definitions (all
  DCE'd -- which is why re-bind count and placement never mattered).

  Two spellings *do* flip the basin and produce an instantly ROM-shaped prologue --
  `const volatile Vector3 *volatile c`, and a volatile-home store through
  `*(const volatile Vector3 *volatile *)&c`. Both are dead: on the old structure they
  cost `c` its slot, and re-run on the new one they no longer preserve the instruction
  count at all (1772, 1766, 1772, 1771). Aiming the rsc round-trip at `c` demotes `c`
  as well and undoes the slot fix. **The prologue's lever and the slot fix want
  opposite things from `c`.**

  Note the read order became load-bearing only after the slot fix -- all six
  permutations were byte-neutral before it, and now XYZ is the only one that scores 15.
  That is `dead-levers-are-scoped-to-structure` in one line.

  One further probe, negative but informative. Dropping the `origin` pointer and reading
  `f->origin.x/y/z` at each use makes words 2,3,4,5 come RIGHT -- the parameter moves land
  in the cartridge's order and the c store follows them -- at a cost of 57 elsewhere. The
  cost is not allocation noise: without the pointer mwcc folds the offset into the load
  (`ldr r3,[r1,#0x14]`) where the cartridge materialises the address first
  (`add r2,r0,#0x14` then `ldr r4,[r2]`), and 59 of the resulting 68 words differ in
  SHAPE rather than in registers. **So the cartridge really does hold an `origin`
  pointer, and the draft is right to.** The head defect is not reachable by removing work
  from the block; the `add r2` has to exist and merely be scheduled later.

  ### 15 -> 0. MATCHED, and promoted to src/

  ```
  RESULT match=True mismatches=0/1778   build_pin.verify -> (True, '2004/b56')
  ```

  The prologue fell to a spelling outside the canonicalised set: **per-site
  `(const Vector3 *)` casts on the three centre reads, moved ABOVE the
  `rad6`/`origin` declarations.** The "one allocator decision" invariant above
  held across ~350 statement-level rewrites because mwcc canonicalises those to
  the same IR -- but a per-site cv-qualifier cast is its OWN CSE class, so it
  never reaches that IR: the `add r0,fp,#0x3c` result is not forwarded into the
  first read, x takes its own reload, and y/z share the second, exactly the
  cartridge's shape. And unlike the two basin-flipping spellings above, the
  casts want nothing from `c` -- it stays chained at 0xc4, so the prologue
  lever and the slot fix stop fighting. Win conditions from a 2,520-variant
  positional sweep (measured before the slot fix landed, on a split-def
  structure; they compose unchanged with the round-trip): reads in x,y,z
  statement order, above BOTH `rad6` and `origin`, `rad6` declared before
  `origin`, and at least two of the three reads cast-spelled.

  The two parallel efforts converged on the same residues independently -- the
  slot swap on one side, the prologue and the index-1502 add on the other (the
  1502 fix is the same name-the-tolerance edit in both trees) -- and the two
  halves composed to zero on the first compile.

  The file now lives at `src/game/actors/dBgW_Kc/_ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr.cpp`,
  enrolled complete in `config/arm9/itcm/delinks.txt` (0x01ffb830-0x01ffd3f8,
  butted against the matched Gnd twin). Two untrue constructs ship in it,
  both flagged in the file header: the volatile round-trip on rsc and the
  `const volatile` pointee on `c`. Finding the author's true spellings for
  both is open readability work; every other construct in the file is honest.

  Scoring note for whoever touches it next: the byte gate that matters is the
  BUILD-flag one (`-Cpp_exceptions off`, what rombuild/build_pin/CI compile
  with). `fdiff`'s default regime (`-w illpragmas`) reads this same file as
  4/1778 -- a literal-pool word and the order of the four hoisted zero-init
  stores are flag artifacts, which is also why the earlier bucket arithmetic
  said 36 where the build gate said 32.

  ### A metric defect that hid work

  `fdiff.norm` pushes operands through `swarm.squash`, which normalises immediates --
  so two instructions writing DIFFERENT stack slots compare EQUAL under it. Every
  sweep in the previous session was ranked on that metric, which means any lever whose
  whole effect is a changed slot number was invisible. `scratchpad/perm/gscore.py`
  compares operands verbatim. Re-running the earlier sweeps on it found no missed
  wins, but the metric had to be fixed before that could be known.


  ### A CV-QUALIFIER CHANGE is what re-issues a CSE'd load. Not volatile.

  This function's dominant defect has always been that mwccarm shares a load the ROM
  re-issues, and the standing workaround was `volatile` -- on a stack scratch array,
  on a pointer, wherever it was needed. That is a lie about the memory, and it was
  going to have to come out before any of this reached `src/`.

  It turns out volatile was never the mechanism. Sweeping eight spellings of one
  re-read (`sphere.flags & 0x20`, which the ROM issues and we did not):

  ```
  reaches the ROM's 8 reads          leaves the CSE in place
    (const dBgCh_SphCrr *)&sphere      (&sphere)->flags
    (const dBgCh_SphCrr &)sphere       (u8)sphere.flags
    (volatile u8 *)&sphere.flags       q = &sphere; q->flags
    (volatile u8 &)sphere.flags        sphere.flags            (control)
  ```

  All four that work produce **identical bytes**. What they have in common is that
  they change a cv-qualifier on the access; what the four failures have in common is
  that they do not. `const` is therefore always available where `volatile` was being
  used for this, and it asserts nothing untrue.

  One corollary that cost a wrong prediction before it was measured: `(const T *)p`
  where `p` is ALREADY `const T *` does nothing, because it changes no qualifier. The
  prologue needed `const volatile Vector3 *c` at the declaration instead.

  ### Score the SHAPE as well, or you cannot tell wrong code from bad allocation

  `fdiff.norm` keeps register names and stack offsets, so a region that is
  structurally perfect and allocated differently scores near zero on it.
  `fdiff.shape` erases both. Reported per call-gap, the two together say where effort
  belongs, and on this function the answer was not what the raw diff suggested:

  ```
  whole function    shape 1666/1778  -- the CODE was already 93.7% right
  146 wrong insns   89 of them in ONE call-gap
  ```

  The order-independent version is better still: take the multiset of shapes each
  side emits and difference it. That reads "surplus 10, deficit 10" out of 1778 --
  and each deficit entry is a specific missing instruction you can go and find. Two
  of this session's gains came straight out of it (`-2 ldreq r,[sp]`, `-2 b`).

  A fourth metric earns its place once the instruction counts agree: instruction `i`
  against instruction `i` across the whole function, no gaps and no slack. It is the
  one closest to a byte match, and it went **276 -> 809 of 1778**.

  The four disagree, in both directions, and every disagreement was informative:

  * the dispatch fix is +180 anchored and +2 `equal` -- difflib was already sliding
    to find those matches; what changed is that they moved to the right OFFSET.
  * the prologue fix is +290 anchored and MINUS 2 `equal`.
  * hoisting the sqrt shifts is +48 `equal` and -46 anchored: a pure re-anchoring
    artifact, rejected on that basis.
  * the tpv subset that wins on anchored costs 22 shape.

  ### The gains themselves

  * **+290** the prologue reads the sphere centre through a volatile POINTEE. Gap 0
    was short exactly one `ldr r,[sp]`: the ROM loads `c` twice for the three `raw`
    components and we loaded it once. That one instruction restored the exact 1778
    count, and because the length mismatch had been shifting the whole 364-instruction
    gap, it put 290 instructions back on their correct index.
  * **+180** the Voronoi dispatch is a real `if/else`. The ROM ends BOTH arms in a
    branch to their own body; the draft wrote the first arm as a block falling
    through into the second, so mwcc laid it out the other way and dropped two
    branches -- shifting a 218-instruction gap by two.
  * **+71** DELETING the `en3 = f->normals[tri->edgeNormal3Idx];` re-read that was
    worth +153 last session. See the interaction note below.
  * **+13** the `sphere.flags & 0x20` re-read, three sites.
  * **+5** two selects the ROM writes as if/else and the draft wrote as
    assign-then-override.

  ### A lever SIGN-FLIPPED another one

  The re-test rule on this function has been "a dead verdict is scoped to the
  structure it was measured against". This session it went further. Full 2^4
  factorial on the anchored count:

  ```
  cr volatile -> const, alone          +0     looks like pure readability
  drop the en3 recompute, alone      -210     looks like a catastrophe
  both together                       +71
  both + the flags re-read            +84
  ```

  So a change that measures as a no-op can be the enabler for a change that measures
  as a disaster. Neither would ever be found by accepting improvements one at a time,
  which is what every greedy sweep in this file does. The only defence is the
  factorial: when a readability change measures neutral, keep it and re-test the
  levers around it.

  ### Three invented constructs retired, all byte-neutral or better

  * `struct Vtx3 { s32 e[3]; ~Vtx3(){} }` was the draft's own type, invented to block
    scalarization. `include/types.h` already has `Vector3` -- same three words, same
    empty destructor, declared for the same reason. Byte-identical.
  * `volatile s16 cr[3]` read through a volatile view -> plain `s16 cr[3]` read
    through a `const s16 *` view. Within one instruction, and it is the enabler above.
  * four of the six no-op `c = &sphere.pos;` re-binds. All 64 subsets swept
    individually; the previous sweep had only ever tried all six against none.

  ### Still open, and now precisely named

  Gap 28 -- the slab block, second KCL_VERTEX round through the three axis dot
  products -- holds 127 of the 331 remaining divergences and 66 of the 91 remaining
  shape defects. Its instruction MIX is nearly exact (surplus 7, deficit 4 of 151),
  so what is wrong there is ORDER: mwccarm scheduling under register pressure. No
  statement reordering tried so far reaches it, and declaration order is now
  converged (8,464 candidates, zero improving moves, ranked on the strict metric).

  Three deficits are understood and unreached: the ROM computes both `>> 4` halves of
  the hypotenuse before the DotVec3 call; it writes the 64-bit sqrt parameter through
  a second, copied pointer (`mov r2,r3`); and it never folds `c` back to the object,
  where mwcc does so once (`ldr r8,[fp,#0x44]`).

  ### Two process corrections worth more than any single lever

  **A compile+score round trip is ~0.45 s, not 30-60 s.** I asserted the slower
  figure without measuring it and it shaped the whole approach: I ran ~40 scored
  variants where 430+ were affordable. A 377-compile greedy sweep costs under
  three minutes. **Measure the loop cost before choosing a search strategy.**

  **`fold_nn` was worth +130 after previously measuring -16.** Folding
  `nn12/nn23/nn31` + `n12h/n23h/n31h` into one `nn`/`nnh` pair was recorded dead
  on the old structure. Re-run on the new one it is the largest single lever of
  the whole day. Fourth and biggest confirmation of the re-test rule.

  ### Region A is CLOSED

  `call 27 -> 28`: ROM 608 bytes / 152 insns, candidate 560 / 140 — **-48 bytes,
  from +168**. Verified independently. The fix was an in-place centre
  subtraction (`tp[j] -= c->x;`) with the three dots bound to locals `da/db/dc`
  so all six bound-compares run after all three dots. Note the dead list had
  tried *fresh* bindings and loops but never **in-place** subtraction — the
  25 dead variants had a hole in them.

  ### One non-natural spelling, flagged for review before this ever enters src/

  `volatile s16 cr[3]` is worth +20 and is the only unnatural construct in the
  draft. It forces the cross scratch through memory (strh/ldrsh, `smull` instead
  of `umull`+`mla` widening), which is what the ROM does in both `KCL_VERTEX`
  rounds. Removing it on the final structure costs -20 equal and +51 insns. A
  natural spelling reproducing the memory residency has not been found: pointer
  views fold or perturb allocation (-19..-23), an inline `Cross` helper is
  byte-identical, and an untracked `crw = cr + z15c` reproduces the ROM's reload
  *traffic* but loses -36 to register churn. Harmless in `notes/` and the bank;
  **must be revisited if this becomes an enrolled `src/` file.**

  ### The remaining wall, named

  All residual drift (0:-4, 6:-2, 7/11/15:-3, 8/12/16:+1, 20:+3, 22:-3, 24:-13,
  26:-7, 28:-12) is one family: the ROM re-reads pointer locals (`c`, `tri`,
  `fn`, `en3`, `vtx`) from their stack homes once per *use*, and keeps
  **en1=r5, en2=r4** register-resident, where the candidate CSEs those loads and
  register-homes `{en2, en3}` instead. Mechanism proven, not guessed: the
  `crw = cr + z15c` probe reproduces the reload traffic almost exactly. It is
  allocation/homing, not width and not a pragma — `opt_common_subs off` collapses
  the candidate by 379 and `opt_lifetimes off` by 477, killing that hypothesis
  measurably. No source spelling found flips the en1<->en3 homing. Worth roughly
  the remaining ~35 insns of count drift, and the natural target for
  `tools/permuter/`.

  ### RE-TEST YOUR DEAD LEVERS AFTER ANY STRUCTURAL CHANGE

  The single most valuable thing learned here. #1197 concluded that declaration
  order was **byte-neutral** for this function — "the lore that declaration order
  IS the stack layout holds for the 0x498 twin and **not** for this 0x1bc8 one",
  measured with positive controls across nineteen variants. That was true *of the
  structure it was measured against*. Once the block layout was fixed, the same
  class of lever became the largest single win available:

  | lever | equal | vs baseline |
  |---|---|---|
  | baseline (post block-layout) | 601 | — |
  | **`en1`/`en2`/`en3` declared LAST in the block** | **810** | **+209** |
  | + greedy: `fn`, `den12/23/31`, `depth` also moved last | **816** | **+215** |

  Five declaration lines. Nothing else changed. **A "swept and dead" verdict is
  scoped to the structure it was measured against — re-run the sweep whenever the
  structure moves.**

  The win is specific, not a rule: `ptr_last`, `ptr_first`, `arr_last`,
  `consts_first`, `consts_last`, `reverse`, `reverse_tail_half`, `s64_last` were
  all tested as whole-block reorderings and every one was inert or much worse
  (`reverse` costs 407). It is the position of those particular declarations, not
  a sortable property.

  ### Verdicts that SURVIVED re-test

  - **Frame-size chasing is still dead.** The candidate frame stayed `sub sp,#0x1fc`
    against the ROM's `#0x1b4` across all twenty orderings tried, including the
    +215 win. Declaration order changes *allocation* without changing *frame size*,
    so the 72-byte surplus really is structural, exactly as #1197 said.
  - **Folding is still dead.** `den12/23/31` into one `den` (−1), `nn*`/`n*h` into
    one pair (−16), `d1h/d2h/d3h` into one `dh` (inert), all three together (−10).
  - Hoisting `t/u/vx/vy/vz` to the block tail: inert.

  **3c is done, and it moved 3b.** The cross-read (below) found the shortfall was *block
  layout*, not missing code. `nearmiss/db.jsonl` carried the improved body, as did
  `notes/drafts-sphereclsn-detectclsn.cpp` until #1655 matched the function and deleted it;
  the finished body is `src/game/actors/dBgW_Kc/_ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr.cpp`.

  ### What the cross-read established

  The skeleton was already right: ROM and candidate emit **34 calls, same callees, same
  order** (33 `bl` + one `blx r3`). Aligning them by offset localises every deficit.

  The ROM lays the Voronoi region out in three contiguous runs — all six arms
  (`0x01ffbea8..0x01ffc1e8`), then the three edge blocks (`0x01ffc1ec`, `0x01ffc35c`,
  `0x01ffc4cc`), then the three corner blocks (`0x01ffc63c`, `0x01ffc750`, `0x01ffc89c`).
  With `EDGE_FILTER` expanded inline at the tail of each feature block, mwcc emitted
  feat1+armA+armB+EDGE1 as one fallthrough run and **exiled feat2 to candidate `+0x8d4`
  and feat3 to `+0xb3c`**. Everything after the first exiled block then landed at the
  wrong index — which is why the whole `0x01ffbf20..0x01ffcf50` band scored 0-15% while
  the instructions in it were largely correct. Branching to labelled `edge1`/`edge2`/
  `edge3` blocks restores the ROM's three runs: the drift at the first four edge-block
  calls went **-552 / -468 / -556 / -560 to -36 / +48 / -40 / -44**, and `delete` in the
  alignment counts collapsed **159 to 25**.

  The two readings each held half the truth and neither had it whole: the port
  transcription had the corner **pairing** right (A=e1e2, B=e2e3, C=e3e1); the banked
  draft had the **two-entries-per-corner** sharing right. A branch-target census settles
  it — `0x1ffc63c`/`0x1ffc750`/`0x1ffc89c` have exactly two sources each.

  ### Newly swept and dead — do not re-walk

  - Binding `DotVec3` to a temp **above the divisor guard** in `EDGE_FILTER`: inert on
    layout (gap 6->7 stayed 208 vs the ROM's 120), +3 instructions, ratio 0.3404 -> 0.3401.
  - Binding it **above `SqrtRaw`** as well, which is the ROM's actual order: **exactly
    scoring-neutral** (0.3404 / 601 / 1753). mwcc reschedules both spellings back.

  ### The three edge blocks: FIXED

  They were the dominant repeat -- each ran +84 bytes long before `DotVec3` then
  -88 short after it, three times over. Hoisting `DotVec3` above `SqrtRaw` fixed
  all three at once:

  ```
  gap 6->7 (CLPS read -> DotVec3)   ROM 120, cand 204 -> 108   (+84 -> -12)
  gap 7->8 (DotVec3 -> guard)       ROM 108, cand  20 -> 112   (-88 -> +4)
  equal 816 -> 825,  ratio 0.4622 -> 0.4673,  divergences 962 -> 953
  ```

  That lever had been measured INERT twice earlier the same day, before the
  block-layout and declaration-order fixes. Third instance of the re-test lesson.

  Do NOT bind the two `>>4` shifts to temporaries first, even though that is
  literally what the ROM does (`asr r7,r8,#4` / `asr r6,sb,#4` straddling the
  call at 0x01ffc270..0x01ffc280): `rom_order` -1, `rom_order_d4_late` -4, both
  growing the candidate to 1759. The shifts must stay inline in the `SqrtRaw`
  argument; only the call moves. Binding them without hoisting is inert.

  ### The wall slab is NOT the post-accept deficit

  `call 27->28` runs +168 (ROM 608, cand 776) and the obvious suspect was the slab
  test naming `AXIS_DOT` twice per vertex -- six three-term dot products where the
  ROM's 12 `smull` allow four. It is not:

  | variant | equal | cand |
  |---|---|---|
  | baseline | 825 | 1753 |
  | bind all three dots up front | 820 (-5) | 1722 |
  | bind each lazily, nested | 825 (inert) | 1753 |
  | bind lazily with goto-out | 825 (inert) | 1753 |

  The lazy forms are byte-identical, so **mwcc already CSEs the repeated
  `AXIS_DOT` within the `&&` chain**. Binding all three up front forces eager
  evaluation, which is 31 instructions shorter but costs 5 matches and takes the
  candidate to 56 short instead of 25. Look elsewhere in the post-accept chain.

  ### Region B (calls 31->32) is EXACT: the ROM duplicates where the draft shares

  They were NOT one boundary defect. +168 and -128 looked adjacent and opposite,
  but fixing B left A at +168 unchanged. Two independent defects.

  B was the min/max accumulate. The ROM gates it on `contactKind` and emits TWO
  COMPLETE ARMS, duplicating the x and z clones into both; the draft shared them
  and wrapped only the y clone in `if (contactKind != 1)`.

  ```
  0x01ffd0d8  cmp contactKind,#1 ; hitFlags |= 2 ; bne 0x01ffd170
  0x01ffd0f0    clone [sp,#0x180] x        <- contactKind == 1 arm: x, z
  0x01ffd12c    clone [sp,#0x188] z
              b continue
  0x01ffd170  clone [sp,#0x180] x          <- second copy of x
  0x01ffd1ac  clone [sp,#0x184] y
  ```

  Spelling it as two duplicated arms: **equal 825 -> 855, ratio 0.4673 -> 0.4799,
  gap 31->32 -128 -> +0 (ROM 372, cand 372, exact)**. Candidate size 1753 -> 1785,
  so it is now 7 instructions OVER 1778 rather than 25 under.

  **Arm order is load-bearing**: `contactKind == 1` first, matching the ROM's
  fallthrough, is +30; `!= 1` first is only +15.

  Third instance of share-vs-duplicate in this function (block layout, edge
  blocks, now min/max). **When a region is short, suspect the ROM duplicates
  something the draft shares.**

  ### Dead ends in region B, all with cand unchanged (mwcc eliminated them)

  The `asr r0,r0,#0x1f` in the first clone looks like a dead sign-extension and
  is not -- it is `depth`'s high word, consumed two clones later by
  `mla r4, r0, r3, r4`, part of a **full signed 64x64** expansion
  (`umull` + two `mla`) where the draft emits a 32x32 `smull`. But:

  - adding `dpH = depth >> 31` before / after / with laundering: all inert, and
    `cand` never moved, so mwcc dead-code-eliminates it (nothing reads it).
  - casting both operands, `(s64)depth * (s64)sn.c`, for any subset of x/y/z: all
    inert, `cand` never moved -- mwcc narrows it straight back to `smull`.

  Forcing the 64x64 needs an operand mwcc cannot prove is 32-bit. Unresolved, and
  it did not block region B closing by other means.

  ### Region A (calls 27->28, +168) is an ORDERING defect, not a width one

  Eleven variants across five hypotheses, all dead. But the measurement narrowed
  it a lot, so do not start over.

  **First, a false lead worth knowing about.** A mnemonic histogram of region A
  alone reads as a multiply-width defect and it is NOT one:

  | mnemonic | ROM | cand | |
  |---|---|---|---|
  | smull | 12 | 3 | -9 |
  | umull | 0 | 9 | +9 |
  | mla | 0 | 18 | +18 |
  | ldr | 39 | 57 | +18 |

  That looks like nine 32x32 multiplies replaced by nine full 64x64 ones. It is an
  artefact of comparing two spans that do not hold the same source. The
  WHOLE-FUNCTION shift census is **identical** -- 9 `>>12` sites and 15 `>>14`
  sites in both ROM and candidate -- so nothing anywhere is computed at the wrong
  width. **Always census the whole function before believing a per-region
  histogram.**

  What is actually wrong is the ORDER. Region A holds the same work in both (9 of
  the `>>12` and 3 of the `>>14`):

  ```
  ROM   0x158c 0x15a0 0x15cc  >>14   KCL_VERTEX(vc) finishes FIRST
        0x1630 ... 0x175c     >>12   then all nine AXIS_DOT terms
  CAND  0x1580 0x15ac 0x15d0  >>12   one AXIS_DOT starts early
        0x15fc 0x161c 0x1644  >>14   KCL_VERTEX(vc) interleaved into it
        0x16b0 ... 0x17ac     >>12   the other two AXIS_DOTs
  ```

  mwcc interleaves the second `KCL_VERTEX` with the first `AXIS_DOT`. That raises
  simultaneous liveness, and the +18 `ldr` is the resulting spill traffic. The
  +42 instructions are spills and reloads, not extra arithmetic.

  ### Region A: swept and dead (11 variants)

  - **Wall slab, 3 variants.** Binding the three `AXIS_DOT`s up front: -5, though
    it does cut 31 instructions. Binding each lazily / with goto-out: byte
    identical to baseline, so mwcc already CSEs the repeated `AXIS_DOT` inside the
    `&&` chain.
  - **Difference binding, 3 variants.** The ROM stores the three
    `(v)[i] - c->i` differences to a stack Vector3 (`sp+0x198/0x19c/0x1a0`) before
    multiplying. Reproducing that in source is worse, and grows the candidate:
    `s32[3]` -13 (cand 1800), three scalars -13 (cand 1800), a real `Vector3`
    **-181** (cand 1864).
  - **`t`/`u` reuse, 2 variants.** `t` and `u` are `KCL_VERTEX` scratch and then
    the slab bounds; the false dependency looked like the interleaving cause.
    Giving the bounds their own names is **inert** (mwcc coalesces them back);
    also computing them before the `KCL_VERTEX` pair is **-48**.

  Untried: forcing `KCL_VERTEX(vc)` to complete via a sequence point mwcc cannot
  schedule across, or splitting the slab test into its own function.

  ### The compiler and its flags are PROVEN for this TU, not assumed

  Worth settling before anyone spends time on "maybe it is the wrong compiler".
  The ITCM `dBgW_Kc` block is one unbroken run with zero gaps between symbols:

  ```
  0x1ffb0d0  GetUnkOctreeY        0x1c    MATCHED, enrolled
  0x1ffb0ec  GetOctreeOriginY     0x10    MATCHED, enrolled
  0x1ffb0fc  DetectClsn(Lin)      0x734   unmatched
  0x1ffb830  DetectClsn(SphCrr)   0x1bc8  unmatched   <- ours
  0x1ffd3f8  DetectClsn(Gnd)      0x498   MATCHED, enrolled
  0x1ffd890  GetTriangleOrigin    0x48    MATCHED, enrolled
  0x1ffd8d8  GetNormal            0x48    MATCHED, enrolled
  ```

  Contiguous means **one translation unit**, and compiler flags are per-TU. Five
  of the seven byte-match at 2004/b56 under plain `-O4,p` with **no pragma**, and
  our target is sandwiched between matched functions on both sides. So the
  compiler, version and flags are settled for this exact TU.

  This was a live hypothesis, not a straw man: **441 files in this tree carry
  optimisation pragmas** (`opt_strength_reduction` x169, `opt_common_subs` x157,
  `long_calls` x92, `optimize_for_size` x26, `opt_lifetimes` x4), and
  `opt_lifetimes` bears directly on the register-liveness problem region A has.
  The sandwich rules it out here.

  ### "Write it the way a human would" -- sound reasoning, 7 more dead variants

  The draft is macro soup (`FX12`, `MUL10`, `AXIS_DOT`, `KCL_VERTEX`,
  `EDGE_FILTER`, `VERTEX_BLOCK`). Nobody writes a 7KB function that way, and a
  macro splices into the caller's expression tree where an inline function is
  parsed as a unit -- so "what a person would write" and "what stops the
  interleaving" looked like the same change. They are not:

  | variant | equal | cand |
  |---|---|---|
  | `AXIS_DOT` as a `static inline` fn, same `&&` chain | **855, BYTE-IDENTICAL** | 1785 |
  | inline fn holding a real `Vector3` difference | 855 (inert) | 1791 |
  | inline fn + `Vector3` + hand-unrolled nesting | 855 (inert) | 1791 |
  | inline fn + `Vector3` + a real loop over `verts[3]` | 830 (-25) | 1722 |
  | inline fn, no `Vector3`, + loop | 828 (-27) | 1715 |
  | one `s32 verts[3][3]`, unrolled reads | 846 (-9) | 1790 |
  | same, rebuild writing through a pointer into it | 847 (-8) | 1789 |

  **`static inline` is byte-identical to the macro** -- mwcc inlines it away, so
  there is no scheduling boundary to buy. The array-aliasing idea (three separate
  locals let mwcc prove `AXIS_DOT(tp)` independent of the code filling `vc`; one
  array should not) does not hold either.

  Ground truth on house style, from the matched sibling in the SAME TU
  (`src/game/actors/dBgW_Kc/_ZN7dBgW_Kc10DetectClsnER9dBgCh_Gnd.cpp`): it uses **no macros and no
  helpers at all** -- named scalar intermediates and fully written-out
  expressions, e.g.

  ```c
  dy = -(cstd::fdiv((s32)(((s64)dx * normal[0]) >> 10)
                  + (s32)(((s64)dz * normal[2]) >> 10), normal[1]) >> 2);
  ```

  ### The real conclusion: region A may not be reachable from local source

  At `-O4,p` mwcc normalises away nearly every source-level distinction we can
  express here -- macro vs inline (identical), array vs scalars (no dependency
  respected), both-operand casts (narrowed back), dead sign-extensions (DCE'd),
  fresh names for reused temporaries (coalesced back). **25 variants across 8
  hypotheses, all dead.**

  Note where the value actually came from: the biggest win of the session (+215)
  was a **global** change, declaration order. Region-local edits bought +9 and
  +30. So the next productive lever is probably another global one -- whole-
  function register allocation, e.g. the permuter (`tools/permuter/`, validated,
  scores 0 for a true match) -- rather than more region-A surgery.

  ### A free readability win, banked for later

  Since `static inline` is byte-identical to the macro, the whole draft could be
  de-macroed at **zero byte cost** whenever readability is the goal. Project goal
  order is accurate-C++-and-byte-match first, then portability, then readability,
  so this is noted, not done.

  ### Where the remaining gap is, measured

  | region | drift | note |
  |---|---|---|
  | calls 0-6 (head) | constant **-36** | 9 instructions short in the prologue/setup/octree descent, ROM 1456 bytes vs cand 1420. Constant, so it does not misalign anything downstream |
  | calls 27->28 | **+168** | post-accept chain too long; NOT the wall slab, see above. The one big region left |
  | calls 31->32 | **EXACT** | closed by the duplicate-arms fix above |
  | call 18 | **-40** | ROM 136 vs cand 96 |
  | calls 22 / 24 / 26 | -28 / -32 / +36 | corner blocks |

  The edge blocks and `31->32` are done. `27->28` (+168) is the one big region
  left; the "adjacent and opposite, so one boundary defect" reading was WRONG --
  closing B moved A by nothing at all.

  ### A correction carried in from the reloc table

  There **is** a square root — four of them. The ROM drives the DS hardware unit by MMIO
  (`0x040002b0` SQRTCNT x8, `0x040002b4` RESULT x4, `0x040002b8` PARAM x4, each inside an
  IME `0x04000208` critical section), so it never appears as a call and a reloc-table read
  says "no sqrt". Sites: `0x1ffc2a8`, `0x1ffc418`, `0x1ffc588`, `0x1ffca30` — the three
  edge gates and the shared `do_sqrt`. Both drafts already model it.

  ### Nothing here waits on the C++ conversion

  ITCM is an **autoload** (`config/arm9/config.yaml:11`) — a linker section memcpy'd at
  boot — so it constrains no source language; 15 of the 27 enrolled ITCM files are `.c`
  and 12 are `.cpp`, and the sibling `DetectClsn(dBgCh_Gnd&)` byte-matches today as
  `//cpp`. There is exactly **one** virtual dispatch in all 1,778 instructions
  (`0x1ffbe60 blx r3`, slot 3 on `this`, already-matched `GetSurfaceInfo`) and **zero**
  through the `dBgCh_SphCrr&` parameter, so promoting that class (Phase 2c) is a
  readability convenience here, not a prerequisite.

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

  ```sh
  python tools/fdiff.py --c src/game/actors/dBgW_Kc/_ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr.cpp \
    --name _ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr \
    --module itcm --addr 0x01ffb830 --size 0x1bc8 --version 2004/b56 --align
  ```
  `--module itcm`, never `arm9/itcm`.

- **3c. DONE** — cross-read of the port transcription (§5) against the banked draft; the
  result is folded into 3b above. It was worth the hour: neither reading was right about
  block layout, and the ROM's own branch-target census was the adjudicator, not either
  document.

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

```arm
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
python tools/fdiff.py --c src/game/actors/dBgW_Kc/_ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr.cpp \
  --name _ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr \
  --module itcm --addr 0x01ffb830 --size 0x1bc8 --version 2004/b56 --align
# the port line
git show origin/port-mount-noseat-cluster:port/unmatched/MeshCollider_DetectClsn_Sphere.cpp | head -60
git show origin/port-mount-noseat-cluster:port/slice_gate8.txt
```

Do not run `rombuild.py` concurrently with `eligible.py` — they share `build/` and invent
link errors.
