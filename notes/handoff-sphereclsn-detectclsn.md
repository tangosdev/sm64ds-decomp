# Handoff: `MeshCollider::DetectClsn(SphereClsn&)`

Everything needed to land ITCM `0x01ffb830`, size `0x1bc8`. The discovery work is done; what
is left is transcribing three symmetric arithmetic blocks against a fully specified interface.

Read `notes/collision-query-classes.md` for the derivations. This file is the operating
summary.

---

## 0. The one thing that will waste your day if you skip it

**Build at `2004/b56`. Not 1.2/sp2p3.**

The previous floor for the sibling `DetectClsn(RaycastLine&)` was pinned to 1.2/sp2p3 and
every lever in it was measured on the wrong compiler. Of **16 installed mwccarm builds,
exactly one reproduces the ROM's size** for these functions:

| builds | size for RaycastLine |
|---|---|
| 1.2/base, 1.2/sp2, 1.2/sp2p3 | 0x73c |
| 1.2/sp3, 1.2/sp4 | 0x728 |
| all ten 2.0/* | 0x724 |
| **2004/b56** | **0x734 — exact** |

An exact size is evidence about the *build*, not only about the source. Derive the build from
the twin that already matched (`DetectClsn(RaycastGround&)` matched on 2004/b56), and
re-derive it from size when a function is unmatched.

```
python tools/fdiff.py --c <draft>.cpp \
  --name _ZN12MeshCollider10DetectClsnER10SphereClsn \
  --module itcm --addr 0x01ffb830 --size 0x1bc8 --version 2004/b56
```

`--module itcm`, never `arm9/itcm` — see `notes/itcm.md`.

## 1. Where the draft is

`notes/drafts-sphereclsn-detectclsn.cpp`, currently **0x8cc** against `0x1bc8`.

Written and shape-verified against the ROM: entry, three-axis AABB, radius square, the
three-axis march, octree descent, per-cell step, the leaf caches, the sorted top-3 insert,
the reject chain, `depth`, the classify, the pass-through filter, the record, the accumulate,
the epilogue.

**Missing: step 5 only** — the Voronoi edge/vertex distance blocks.

## 2. Per-prism body, in ROM order

```
1  three edge-normal rejects (tri+0x8, +0xa, +0xc), then the face reject (tri+0x6)
2  depth = rsc - faceDot
3  triID via func_020396dc; GetSurfaceInfo (REAL virtual call); CopyNormalTo; classify
4  BgCh::ShouldPassThroughImpl -> reject
5  Voronoi dispatch: face case, or edge/vertex distance vs the squared radius   <-- TODO
6  record into the class slot, set the class bit, accumulate depth x normal
```

## 3. The classify, derived three independent ways that agree

`func_02039794` is matched source:

```c
if (normalY >  0x600) return 0;   /* FLOOR */
if (normalY > -0xccc) return 1;   /* WALL  */
return 2;                          /* UND   */
```

| class | result slot | flag bit | recorder (all matched, all 0x10 bytes) |
|---|---|---|---|
| 0 floor | 0x74 | 4 | `func_020379f4` -> `func_02037fd4(c+0x74, ...)` |
| 1 wall | 0x9c | 8 | `func_020379c0` -> `func_02037fd4(c+0x9c, ...)` |
| 2 und | 0xc4 | 0x10 | `func_0203798c` -> `func_02037fd4(c+0xc4, ...)` |

Confirmed separately by the two matched wrappers, by these thresholds, and by the recorders.

**A floor accumulates only its Y component** and separately keeps the most upward-facing
normal (`func_0203794c`, matched: `d[63..65] = s[0..2]`). Walls and undersides accumulate all
three axes. Each record is guarded by `!(flags & bit)` so the *first* hit of a category wins,
but *every* hit contributes to the extent.

## 4. What is left, precisely

Three symmetric blocks, ~300 words each, one per edge. Each one:

1. forms `nn = dot(en_i, en_j) >> 10` — the cosine between two edge normals — and compares
   `(nn * dot_i) >> 10` against `dot_j`, against **both** neighbours, to pick edge vs vertex;
2. computes the squared distance for whichever region won;
3. compares it against the squared radius parked at `sp+0x60` / `sp+0x64`.

Inputs (`dot1/dot2/dot3` and the three edge normals), the comparison, and the output
(`depth`, feeding the accumulate that is already written) are all known.

**Do not let the 64-bit intermediates collapse to 32-bit.** `asr r2,r8,#0x1f` sign-extends
`dot1` into `sp+0xd8`, `sp+0xf0` holds another high word; the products stay 64-bit through the
compare. This is the most likely place a plausible-looking draft diverges.

Scales, all cross-checked: positions and the octree origin are raw 1/64 Fix12i units, face and
edge normals are `1.0 == 0x400`, `rsc = radius << 4` is `raw_radius * 0x400` so both sides of
every reject compare match, and `>>10` is scale-preserving for a normal-normal product
(`0x400 * 0x400 >> 10 == 0x400`).

## 5. The frame

Declaration order **is** the frame on this compiler — the matched `RaycastGround` twin's first
lever. The fourteen zeroed words, in order, are already placed in the draft:

| slot | | slot | |
|---|---|---|---|
| 0x28/0x2c | lo.x / hi.x | 0x40 | result bitmask, **returned** |
| 0x30/0x34 | lo.y / hi.y | 0x44 | second flag |
| 0x38/0x3c | lo.z / hi.z | 0x48–0x50 | `prev1..3` visited leaves |
| | | 0x54–0x5c | `p1..3` top-3 leaf pointers |

and elsewhere: `0x0c` file, `0x10`–`0x24` the AABB pairs, `0x64` squared-radius high word,
`0x6c`/`0x70` stepY/stepZ, `0x74`–`0x7c` the top-3 scores, `0x80`/`0x84` y/x, `0x88` leaf,
`0x9c` depth, `0xa0` triID, `0xc4` `&centre`, `0xc8`–`0xd0` rawX/Y/Z, `0x104` `radius << 4`,
`0x180`–`0x188` the surface normal.

## 6. Reading a flat size correctly

The draft only grows when code has an observable effect. With no geometry, mwccarm stripped
the entire prism loop as dead and the size *fell* (0x320 -> 0x310) when accumulators were
added that nothing wrote yet. Progression across the geometry passes: 0x230, 0x328, 0x4f8,
0x58c, 0x8a8, 0x8cc. A flat or falling number is not necessarily a lack of progress — check
whether the new code is reachable and has side effects before diagnosing.

## 7. Method notes, earned the hard way this session

* **Grep narrows where to look; it does not establish absence.** "Never writes through `fp`"
  was wrong because the writes go through `add r4, fp, #0x70` then `strb [r4]`. Confirm a
  negative by reading the sites or counting the complement.
* **Symbols come off the call site.** The wall recorder was written as `func_02037a24`,
  inferred from a neighbour's address; the actual call is `bl #0x20379c0`.
* **Check whether it is already matched before decoding it.** Nine helpers, the classifier,
  the AABB expander and both sibling wrappers were all already matched source. Reading them
  was faster and more reliable than disassembly, and `DotVec3` was already named in
  `symbols.txt`.
* **The virtual call must be a real one.** `GetSurfaceInfo` through `[vtable+0xc]`: mwccarm's
  own dispatch reads `this` from r0, a hand-rolled `(*(fn**)this)[3](...)` reads it from the
  callee-saved copy. That one word was the entire residual on the twin.

## 8. Fields this function gave a purpose to

`MeshCollider::unk_34`, `unk_35`, `unk_38`, `unk_44` and `unk_4c` are all consumed here as a
normal filter on the face test — a face-angle cutoff, a preferred-direction test against a
stored axis, and the gate on edge/vertex handling. The set/clear accessors for `0x34`/`0x35`
and the writer for the `0x38` vector were among the **original eleven ITCM matches** and had
no known purpose until now. `include/MeshCollider.h` can name them.
