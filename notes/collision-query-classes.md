# The collision-query class family: RaycastLine, RaycastGround, SphereClsn

Recovered 2026-08-06 while sweeping `MeshCollider::DetectClsn(RaycastLine&)` (ITCM
0x01ffb0fc). The three types passed to the `DetectClsn` overloads are not independent
structs — they share a base and a sub-object, and the already-matched destructors prove it
without any new matching work.

## What the destructors prove

`_ZN11RaycastLineD1Ev` (0x02037764), `_ZN13RaycastGroundD1Ev` and `_ZN10SphereClsnD1Ev` are
matched, enrolled source. All three have the identical skeleton:

```c
((int *)self)[0] = VT0;                 /* own vptr at 0x00 */
*(int *)&self->unk_010 = VT1;           /* a SECOND vptr at 0x10 */
    /* ... class-specific member destructors ... */
func_020380ec(&self->unk_010);          /* destroy the 0x10 sub-object */
func_020354d0(self);                    /* destroy the shared base */
```

Two vptr stores at two different offsets is an embedded polymorphic member, not inheritance
depth. So, proven:

1. **A shared base at 0x00**, vptr at 0x00, destroyed by `func_020354d0` — identical call in
   all three.
2. **A polymorphic sub-object at 0x10**, its own vptr, destroyed by `func_020380ec` —
   identical in all three.
3. The three generated headers already agree on the prefix — `pad_000[0x10]`, `unk_010`,
   `pad_011[0x27]` — i.e. **0x00..0x37 is common layout across all three.**

## Why the prefix ends at exactly 0x38

`SphereClsn` carries three more of the same type at 0x74, 0x9c and 0xc4 — stride **0x28** —
and its destructor calls `_ZN10ClsnResultD1Ev` on each. So `ClsnResult` is 0x28 bytes, and
`0x10 + 0x28 = 0x38`: the sub-object at 0x10 exactly fills the shared prefix. That is an
independent derivation of the same boundary the three headers show, from a different
function.

`RaycastGround.h`'s hand-extended comment already called 0x10 "the 0x28-byte ClsnResult the
hit is written into"; this generalises it to the whole family and pins the width from the
stride rather than from one call site.

Caveat kept honest: `func_020380ec` is *not* `_ZN10ClsnResultD1Ev` — `SphereClsn` calls both,
for different members. So the 0x10 member is ClsnResult-*sized* and ClsnResult-*shaped*, but
either it is a derived type or `func_020380ec` is a wrapper. Do not rename it to `ClsnResult`
until that is settled.

## Where the three diverge, at 0x38

| class | 0x38 | evidence |
|---|---|---|
| `RaycastGround` | plain data — `Vector3 pos` | destructor destroys **nothing** at 0x38 |
| `RaycastLine` | a sub-object, dtor `func_ov002_020feab8` | destroyed before 0x10 |
| `SphereClsn` | a **polymorphic** sub-object, third vptr `VT2`, dtor `func_0203ac1c` | third vptr store |

`RaycastLine`'s 0x38 destructor lives in **overlay 2** (`func_ov002_020feab8`), so that member's
type is overlay-resident — worth knowing before anyone tries to type it from arm9 alone.

## The tension this exposes, and what NOT to do yet

The banked `DetectClsn(RaycastLine&)` tip models 0x38 as a bare `Vector3 lineStart`, and that
is what produces the correct `add r6,r1,#0x38` / `[r6]`, `[r6,#4]`, `[r6,#8]` loads. But the
destructor proves something *destructible* lives at 0x38. Both hold only if that sub-object
**begins with** a Vector3.

So do not promote `Vector3 lineStart;` at 0x38 into `include/RaycastLine.h`. It is right for
the three loads and wrong as a description of the member, which is exactly the trade
`notes/plan-cpp-language-mode.md` §3 warns against. Settle the 0x38 type from
`func_ov002_020feab8` first.

`include/RaycastLine.h` does carry two names that the ITCM evidence contradicts outright, and
those are safe to fix independently of 0x38:

| field | header says | ROM says |
|---|---|---|
| 0x5c | `mPosX` | `lineEnd.z` — `add r5,r1,#0x54` then `[r5,#8]` |
| 0x60 | `mPosY` | `clsnDist` — `ldr r1,[r0,#0x60]` on entry, `str r1,[r0,#0x60]` on exit |

0x50 is also unnamed in the header but is written `strb r0,[r1,#0x50]` with `r0 = 1` on the
hit path, i.e. a `hasClsn` byte, the same role `RaycastGround.h` already names at its 0x48.

## SphereClsn: the shape sub-object at 0x38 has contents (2026-08-06)

`MeshCollider::DetectClsn(SphereClsn&)` (ITCM 0x01ffb830, 0x1bc8) reads two fields that
`include/SphereClsn.h` does not have — both fall inside its `pad_039[0x3b]`, and both land
inside the polymorphic sub-object at 0x38 identified above. Straight off the entry code:

```
add r0, fp, #0x3c     then [r0], [r0,#4], [r0,#8]   -> a Vector3 CENTRE at 0x3c
ldr r0, [fp, #0x48]                                 -> the RADIUS at 0x48
```

So the 0x38 member is a sphere shape: vptr at 0x38 (the `VT2` store the destructor makes),
centre at 0x3c..0x47, radius at 0x48. That is a coherent object and it explains why
`SphereClsn` alone carries a third vtable where `RaycastGround` has plain data at 0x38.

Not promoted into the header yet, for the same reason `RaycastLine`'s 0x38 was not: the
sub-object's *type* is still unrecovered (its destructor is `func_0203ac1c`), so naming the
fields on `SphereClsn` directly would flatten a real member into loose fields. Recover the
type first, then the centre/radius belong to it, not to `SphereClsn`.

### Entry facts, for whoever picks the function up

| fact | value | vs the RaycastLine overload |
|---|---|---|
| frame | `sub sp,sp,#0x1b4` | 0xfc |
| `this` | **sl** (r10) | r7 |
| the query object | **fp** (r11) | kept in `[sp]` |
| AABB slack | centre ± `((radius >> 6) + 0x40)` | segment bounds ± 0x40 |

`0x40` raw is one whole world unit, i.e. one octree cell of slack — the same padding the
Line overload applies to its segment bounds, so the sphere is bounded by an AABB inflated by
its radius plus one cell.

A first draft carrying the head and the three-axis AABB is banked at
`notes/drafts-sphereclsn-detectclsn.cpp`. It is a **stub** — the walk and the prism tests are
not written — so it cannot match; what it establishes is that the AABB arithmetic is right,
emitting the ROM's own sequence (`sub` origin, `sub`/`add` the slack, `asrs` + `movmi #0` low
clamp, `mvn` for `~mask`, `movgt` high clamp, `cmp` and early-out) per axis. Build it at
**2004/b56** — the twin `DetectClsn(RaycastGround&)` matched there and so does the Line
overload's size, and `notes/itcm.md` records what mis-pinning that costs.

### The frame map, decoded from the entry (2026-08-06)

`DetectClsn(RaycastGround&)`'s first matching lever was *"declaration order IS the stack
frame"* — mwccarm hands out spill slots in declaration order. So for a 0x1b4 frame the slot
map is the single most useful thing to have before writing the body, and this much is read
straight off the ROM:

| slot | holds |
|---|---|
| `sp+0x0c` | `file` (`this->kclFile`) |
| `sp+0x10` / `sp+0x14` | `loX` / `hiX` |
| `sp+0x18` / `sp+0x1c` | `loY` / `hiY` — mask from `file+0x24` |
| `sp+0x20` / `sp+0x24` | `loZ` / `hiZ` — mask from `file+0x28` |
| `sp+0x28` … `sp+0x5c` | **fourteen words, all zeroed** before the walk |
| `sp+0x64` | high word of the squared radius |
| `sp+0xc4` | `&sphere.centre` (`sphere + 0x3c`) |
| `sp+0xc8` / `sp+0xcc` / `sp+0xd0` | `rawX` / `rawY` / `rawZ` |
| `sp+0x104` | `radius << 4` |
| `sp+0x10c` | seeded to `1` |
| `sp+0x114` | seeded to `0` |

Two structural reads from that:

**The test is squared-distance.** After the three AABB early-outs the radius is scaled
`<< 4` and squared into 64 bits with a single `smull r2,r1,r0,r0`, high word parked at
`sp+0x64`. Neither sibling does this — the Line overload solves a plane and compares along
the segment, the Ground overload compares heights. A sphere query has to compare against
face, edge and vertex distances, which is most of why this function is 0x1bc8 against the
Line's 0x734.

**Fourteen zeroed words is an accumulator block**, not scalars spilled one at a time — the
Line overload's equivalent is its nine un-SROA'd `Vector3` locals, and the `DVec` lever
(a user-declared destructor blocking SROA) is what kept those as aggregates. Expect the same
shape to be needed here.

Order confirmed: X, then Y, then Z, each `lo = (raw - origin - slack) >> 6` clamped at 0 and
`hi = (raw - origin + slack) >> 6` clamped at `~mask`, with `if (lo >= hi) return 0` between
them. The banked draft reproduces that arithmetic; what it does not yet have is the walk.

### The march and descent are the twins' verbatim; the leaf handling is not (2026-08-06)

**Ported and verified.** The three-axis march and the octree descent are the same code as
`DetectClsn(RaycastLine&)` and the matched `DetectClsn(RaycastGround&)`, including all three
of the twin's documented levers, and the draft now reproduces their shape:

| ROM | draft |
|---|---|
| `orr r3, r4, r3, lsl r2` | `orr r1, sl, r2, lsl r1` — `zterm \| yterm`, its own statement |
| `orr r2, r3, r2, lsr r1` | `orr r1, r1, r8, lsr ip` — `\|= x >> shift`, split out |
| `ldr r0, [r0, #0xc]` late | same — octree base loaded after the index |
| child index emitted x-bit, z<<2, y<<1 | same |

The child index is written **z-first in source** and mwccarm emits it **x-first**, which is
exactly the twin's lever 3. Loop vars live at `sp+0x84` (x), `sp+0x80` (y), `sp+0x20` (z,
reusing the `loZ` slot); `sp+0x6c`/`sp+0x70` are `stepY`/`stepZ`, both seeded from a
`1000000` literal, and the per-cell step is the twins' `size - (coord & (size-1))`.

The draft keeps x/y/z in registers where the ROM spills them. That is pressure the prism
loop supplies, not a source defect -- do not chase it before the body exists.

**Where the sphere diverges, and it is the thing to design around.** After the descent:

```
ldr r2,[sp,#0x88]                     the leaf just found
cmp r2,[sp,#0x48] -> beq skip         THREE previously-visited leaves
cmp r2,[sp,#0x4c] -> beq skip
cmp r2,[sp,#0x50] -> beq skip
ldrh r1,[r1,#2] ; beq skip            empty-leaf check
cmp r0,[sp,#0x7c] / [sp,#0x78] / [sp,#0x74]
   ... shuffles 0x74/0x78/0x7c against 0x54/0x58/0x5c ...
```

That is a **3-entry visited-leaf cache** (`0x48/0x4c/0x50`) and a **sorted top-3 list** of
(score, leaf) pairs (`0x74/0x78/0x7c` keyed, `0x54/0x58/0x5c` the pointers), with the
shuffle being an insertion into the sorted three. `RaycastLine` keeps exactly one of each --
`prevLeaf` and `rowLeaf`/`rowStep`. The sphere keeps three, which accounts for six of the
fourteen zeroed words and is why its dedup cannot be lifted from the Line tip.

Next increment: model those two triples as function-scope locals in slot order (declaration
order IS the frame) and write the insertion, before any per-prism geometry. The geometry is
squared-distance -- see the `radius << 4` then `smull r0,r0,r0` above -- and is the genuinely
novel part.

### The triples and the insertion are in, and verified (2026-08-06)

The score is `cy`, the cell's remaining Y extent — the same key the Line overload uses for
its single `rowStep`/`rowLeaf`, kept three deep and sorted descending:

```
cmp cy,[0x7c]  ble skip     s3 -- below all three, no insert
cmp cy,[0x78]  ble ins3
cmp cy,[0x74]  ble ins2
               fall -> ins1  shifts s2->s3, s1->s2 and p2->p3, p1->p2
```

The three visited-checks `beq` **past** the prism loop while the insert branches land just
before it, so the prism loop sits inside the not-visited guard — the same single-tail shape
as the Line overload's `if (lp != prevLeaf) { ... }`, which is what stops the `continue`
bug that cost the Line tip a pass. At row end the row's top three become the next row's
visited set (`prev1..3 = p1..3`).

Ported and checked instruction-for-instruction against the ROM: leaf pointer (`bic
0x80000000` then `add`), `size`/`mask`, then `stepX`, `cz`, `cy`, then the `stepZ` and
`stepY` minimum updates, then the three visited compares, then the empty-leaf check — same
operations in the same order. The ROM spends one extra `mov` before its `ldrh`, which is
allocation noise at the draft's current size.

Draft is now **0x320** against 0x1bc8. Everything above the per-prism geometry is in place.

**What is left is the geometry, and it is all of the remaining ~0x18a0.** It is a
squared-distance test — `radius << 4`, then `smull r0,r0,r0` into 64 bits with the high word
at `sp+0x64` — which means face, edge and vertex distances rather than the Line overload's
single plane solve. The first eight of the fourteen zeroed words (`sp+0x28`..`sp+0x44`) are
still unidentified and are the natural next thing to decode: they are almost certainly the
accumulator set the geometry writes into, and knowing them fixes the declaration order for
the rest of the function.
