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

### The fourteen zeroed words, fully decoded (2026-08-06)

All fourteen now have meanings, which fixes declaration order for the whole function.

| slot | meaning |
|---|---|
| `0x28` / `0x2c` | `lo.x` / `hi.x` |
| `0x30` / `0x34` | `lo.y` / `hi.y` |
| `0x38` / `0x3c` | `lo.z` / `hi.z` |
| `0x40` | result bitmask — **the return value** |
| `0x44` | second flag, ORed into the hit test |
| `0x48` / `0x4c` / `0x50` | `prev1..3`, the visited-leaf triple |
| `0x54` / `0x58` / `0x5c` | `p1..3`, the top-3 leaf pointers |

**The first six are three running min/max pairs.** Every write site is the same shape:

```
cmp v,[sp,#0x2c] ; strgt v,[sp,#0x2c] ; bgt skip      if (v > hi) hi = v
cmp v,[sp,#0x28] ; strlt v,[sp,#0x28]                 else if (v < lo) lo = v
```

and the epilogue hands them to `func_02037a6c` as `r1=[0x28] r2=[0x30] r3=[0x38]` plus
stack `[0x2c] [0x34] [0x3c]` — i.e. `(lo.x, lo.y, lo.z, hi.x, hi.y, hi.z)`. So the function
does not stop at the first hit like its siblings: it accumulates a **penetration extent**
across every intersecting prism and reports the box once at the end. That is the real reason
it is 0x1bc8 against the Line overload's 0x734, more than the geometry being harder.

Six scalars, **not** two `Vector3` by value: a by-value class parameter makes mwccarm home
`r0-r3` to the stack (+0x14, the runbook section 7 dead end) and no homing happens here.
The slots are also interleaved per axis, which two contiguous `Vector3` locals could not
produce.

The accumulated value is a fixed-point product: `umull`, two `mla` sign-corrections, then
`>>14` / `<<18` / `>>2` — net `v = ((s64)a * b) >> 16`.

`func_02037a6c` (0x02037a6c, 0xb0) is still unnamed. Naming it would say what the extent
*means* and is cheap next work.

The epilogue also confirms the two loop tails already in the draft: `prev1..3 = p1..3` at row
end, then `z += stepZ while (z <= hiZ)`.

**Remaining:** the per-prism geometry only. Everything else — entry, AABB, radius square,
march, descent, step, leaf caches, sorted insert, accumulators, epilogue — is written. The
draft compiles to 0x310 because with no geometry the accumulators are never written, so
mwccarm folds them and dead-strips the epilogue call. Both reappear the moment a hit path
exists; do not read the shrink as a regression.

## SphereClsn is largely recoverable from its already-matched callers (2026-08-06)

`func_02037a6c` needed no analysis — `src/func_02037a6c.c` is **already matched and enrolled**,
and it is an AABB expand:

```c
void func_02037a6c(AABB *b, s32 x1,s32 y1,s32 z1, s32 x2,s32 y2,s32 z2)
{   /* min at 0x58/0x5c/0x60, max at 0x64/0x68/0x6c */
    if (b->minX > x1) b->minX = x1;   if (b->maxX < x1) b->maxX = x1;
    ... both corners, all three axes ...
}
```

An independent decode from the ROM bytes agreed with it exactly, which is a useful check on
the method. What it *does* tell us is that the accumulated extent lands in the SphereClsn
itself: **min Vector3 at 0x58, max Vector3 at 0x64.**

### The far better source: two matched siblings call our target

`ExtendingMeshCollider::DetectClsn(SphereClsn&)` and
`MovingMeshCollider::DetectClsn(SphereClsn&)` are both matched, and both are *wrappers around
the function we are trying to write*: transform the sphere into local space, call
`MeshCollider::DetectClsn(SphereClsn&)`, post-process. They name most of the object:

| offset | meaning | evidence |
|---|---|---|
| 0x10 | the shared `ClsnResult` | `ClsnResult::operator=(sphere+0x10, loc.result)` |
| 0x38 | shape sub-object | `func_0203abb0(sphere+0x38, v1)` extracts a Vector3 |
| 0x3c | centre | our entry's `add r0,fp,#0x3c` |
| 0x48 | radius | `FMUL(*(int*)(sphere+0x48), scale)` |
| 0x58 / 0x64 | accumulated AABB min / max | `func_02037a6c` above |
| **0x70** | **flags byte** | `\|= 1`, `\|= 4`, `\|= 8`, `\|= 0x10` |
| 0x74 | **floor** result | `SetFloorResult(sphere, ...)` under `loc.flags & 4` |
| 0x9c | **wall** result | under `loc.flags & 8` |
| 0xc4 | third result | under `loc.flags & 0x10` |
| 0xec | a SECOND radius | `FMUL(*(int*)(sphere+0xec), scale)` |
| 0xfc / 0x100 | a pair, compared then copied | `if (sphere->0x100 < loc.f_100) func_0203794c(...)` |

The generated header already had 0x74 / 0x9c / 0xc4 as `mClsnResult1..3` and 0xec as
`unk_0ec`; the wrapper says which is which — **floor, wall, and a third** — and adds 0x70,
0x58, 0x64 and the second radius, all of which currently sit inside `pad_039[0x3b]` or later
padding.

### What that fixes in our target

`sp+0x40`, the bitmask we return, **is the 0x70 flags word**: the `orr r0,r0,#4` seen at
0x01ffd260 is the *floor-hit* bit, the same bit the wrapper tests with `loc.flags & 4` before
calling `SetFloorResult`. So the three result slots and the flag bits are one mechanism, and
the geometry's job is to classify each hit as floor / wall / third and write the matching
`ClsnResult`, accumulating the extent box as it goes.

That also explains `sp+0x44`: the wrapper's `r` and `loc.flags` are distinct values — one is
returned, one drives the post-processing — which is exactly the two-flag shape at 0x40/0x44.

**Route the geometry through the wrappers, not the raw disassembly.** They are matched code
that already names the constants, and `MovingMeshCollider`'s version is a second, independent
reading of the same interface.

### The SphereClsn interface, named from both wrappers (2026-08-06)

`MovingMeshCollider`'s version confirms `ExtendingMeshCollider`'s field map independently --
same `LocSphere` layout, same post-processing -- and between them the accessors fall out.
Moving passes the result buffers directly where Extending goes through getters, so the pairs
identify each other:

| symbol | is | evidence |
|---|---|---|
| `func_02037938` | `GetFloorResult()` -> `&0x74` | Extending passes it where Moving passes `loc.floorRes` |
| `func_020378dc` | `GetWallResult()` -> `&0x9c` | same, against `loc.wallRes` |
| `func_02037880` | `GetUndResult()` -> `&0xc4` | same, against `loc.undRes` |
| `func_02037888` | `SetWallResult(const ClsnResult&)` | called under `flags & 8` |
| `func_0203782c` | `SetUndResult(const ClsnResult&)` | called under `flags & 0x10` |
| `func_02037a04` | `GetBounds(Vector3 &min, Vector3 &max)` | reads back what `func_02037a6c` accumulates |
| `func_02037a6c` | `ExpandBounds(...)` | already matched, see above |
| `func_02037940` | `SetFlags(u8)` | seeds `loc` from `sphere->0x70` before the call |
| `func_0203794c` | sets the `0xfc`/`0x100` pair | guarded by `if (sphere->0x100 < loc.f_100)` |

`Moving` also pins 0x3c directly: `func_02039e48(self, sphere + 0x3c, pos)` transforms it as a
Vector3, which is the same field our entry reaches with `add r0,fp,#0x3c`.

### Input vs output, settled

Our target **never writes through `fp`** — there is not one `str [fp, ...]` in all 1778 words.
It only *reads* `fp+0x70` (the flags byte, eight times) and `fp+0xec` (the second radius,
twice). So:

* `0x70` is an **input**: the wrapper seeds it with `func_02037940(&loc, sphere->0x70)` and our
  function reads it to decide what to do — the eight reads are almost certainly
  "has this category already been recorded?" guards.
* results reach the object through **helper calls**, not stores. At 0x01ffd240 the pattern is
  `mov r0, fp ; bl 0x203798c`, and immediately after, `ldr r0,[sp,#0x40] ; orr r0,r0,#4 ;
  str r0,[sp,#0x40]`.

So each hit does: classify, call the setter on the sphere, OR the category bit into the
returned mask. Bits are the wrapper's: 1 general, 4 floor, 8 wall, 0x10 third.

`func_0203798c` — the callee in that floor-hit pattern — is the next thing to name; it is the
floor recorder, and its two siblings for wall and third will sit beside it.

### CORRECTION: it does write the sphere, through a derived pointer (2026-08-06)

The claim above that our target "never writes through `fp` -- there is not one
`str [fp, ...]` in all 1778 words" is **wrong, and the method that produced it was wrong.**
Grepping for the `[fp, #imm]` addressing form cannot see a write that goes through a register
derived from `fp`, and that is exactly what this function does:

```
01FFCFFC  add  r0, fp, #0x10      &sphere->result
01FFD000  bl   0x2037fd4          func_02037fd4(&sphere->result, triID, info)
01FFD008  add  r4, fp, #0x70      r4 = &sphere->flags
01FFD010  ldrb r0, [r4]
01FFD014  orr  r0, r0, #1         flags |= 1   (general hit)
01FFD018  strb r0, [r4]           <-- a write to the object
```

Counted properly — every `str`/`strb` whose base is not `sp` — there are **eight**: four
`strb` through `r4`/`r1`, and four `str r2,[r3]` / `str r1,[r2,#4]` pairs writing eight bytes
at a time. Only the `[fp, #imm]` *form* is absent.

So the corrected model. The function **records directly into the sphere** as it goes:

* `func_02037fd4(&sphere->result /* 0x10 */, triID, info)` — the same recorder the Ground and
  Line overloads use.
* `func_0203798c(sphere, triID, info)`, which is matched source and is simply
  `func_02037fd4(c + 0xc4, h, src)` — the **third/und** result.
* the flags byte at `0x70` is read *and* written: `|= 1` for a general hit at 0x01ffd018, and
  `|= 0x10` alongside the `0xc4` record at 0x01ffd23c. Bit and slot agree, which is a check on
  the wrapper's bit map.

`func_02037940` corroborates the direction of travel: it is `p[0x70] = v & ~0x1c`, i.e. the
caller seeds the flags but **clears bits 4/8/0x10** so the callee sets the three category bits
fresh. That is only coherent if the callee writes them, which it does.

What survives from the earlier reading: `sp+0x40` is still a separate word from the object's
flags -- it takes `orr #4` at 0x01ffd260, four instructions after the `0x10` byte write, and
it is what the function returns. Two flag words, not one.

**Method note.** Two structural claims this session have now come from grepping a *form*
rather than the *semantics* -- this one, and the earlier `--module` footgun that was already
fixed on main. Grep narrows where to look; it does not establish absence. Confirm a negative
by reading the sites, or by counting the complement (here: every store whose base is not `sp`).

### The geometry, first half: the reject chain (2026-08-06)

The per-prism opening is the matched `DetectClsn(RaycastGround&)` twin's, with one
substitution — where the twin uses a fixed `0x20000` tolerance the sphere uses **its own
radius**:

```
ldrh r2,[r0,#2]!         *++leaf, the writeback walker
add  r0, r1, r2, lsl #4  tri = &file->tris[lv]
mul  r1, tri->posIdx, #0xc
sub  ip/sb/r3            d = raw - vtx      (raw 1/64 units)
```

| test | field | twin | sphere |
|---|---|---|---|
| edge 1 | `tri+0x8` | `> 0x20000` | `>= rsc` |
| edge 2 | `tri+0xa` | `> 0x20000` | `>= rsc` |
| edge 3 | `tri+0xc` | `< -0x20000`, `> length+0x20000` | `dot - tri->length >= rsc` |
| face | `tri+0x6` | normal[1] <= 0 rejects | `> rsc` rejects — **GT, not GE** |

The units work out: `d` is raw, normals are `1.0 == 0x400`, and `rsc = radius << 4` is
`raw_radius * 0x400`. Same scale on both sides of every compare, which is the check that the
`<< 4` is a units conversion and not a magic number.

### What `unk_34`, `unk_35` and `unk_38` are FOR

Straight after the face test:

```
ldrb r0,[sl,#0x34]        this->unk_34
  set   -> reject unless faceDot >= -0x50000
ldrb r0,[sl,#0x35]        this->unk_35
  set   -> scale fn << 2 into sp+0x174..0x17c, then bl 0x205380c with &this->unk_38
```

Those are the `MeshCollider` bytes at 0x34/0x35 and the `Vector3` at 0x38 — and their
set/clear accessors were among **the original eleven ITCM matches**: `func_01ffb098` /
`func_01ffb0a4` clear and set 0x35, `func_01ffb0b0` / `func_01ffb0bc` clear and set 0x34, and
`func_01ffb07c` writes the 0x38 vector, which `SetFile` seeds to the unit X vector.

Those five were matched a batch ago as bare one-line accessors with no known purpose.
**This is the purpose.** They are a normal filter on the face test — a face-angle cutoff
(`unk_34`) and a preferred-direction test against a stored axis (`unk_35` + `unk_38`) — which
is what a one-way or slope-limited collider needs. `include/MeshCollider.h` can now say so
instead of calling them `unk_`.

Draft is **0x4f8** against 0x1bc8. It only started growing once a call gave the prism loop a
side effect; before that mwccarm stripped the whole loop as dead, which is worth knowing
before reading a flat size as no progress.

**Still to decode:** the result of `func_0205380c`, then the squared-distance classify (the
`smull`/`>>16` chain feeding the min/max accumulators) and the three-way floor/wall/und
branch that drives `func_02037fd4`, `func_0203798c` and the `0x70` flag bits.

### The classify, and the whole hit model closes (2026-08-06)

Two of the three remaining unknowns were already named in the tree, not new work:

* `func_0205380c` is **`DotVec3`** (`config/arm9/symbols.txt`), so the `unk_35` branch is
  `DotVec3(faceNormal << 2, &this->unk_38)` — a dot against the collider's stored axis, which
  is exactly the preferred-direction reading.
* `func_02039794` is **matched source** and is three lines:

```c
int func_02039794(int normalY) {
    if (normalY >  0x600) return 0;   /* FLOOR   -- normal.y > 0.375 */
    if (normalY > -0xccc) return 1;   /* WALL    -- steep            */
    return 2;                          /* UND     -- ceiling/underside */
}
```

With 1.0 == 0x1000 those thresholds are +0.375 and -0.8. **That return value is the whole
classification**, and it lines up one-for-one with what the wrappers told us:

| class | result slot | flag bit | wrapper call |
|---|---|---|---|
| 0 floor | 0x74 | 4 | `SetFloorResult` |
| 1 wall | 0x9c | 8 | `func_02037888` |
| 2 und | 0xc4 | 0x10 | `func_0203782c` / `func_0203798c` |

The accept path, in order:

```
sub  r0, rsc, faceDot ; str [sp,#0x9c]     penetration depth along the face normal
bl   func_020396dc    ; str [sp,#0xa0]     triID
ldr  r3,[r3,#0xc] ; blx r3                 GetSurfaceInfo -- REAL virtual call, slot 3
bl   0x2037dcc                             SurfaceInfo::CopyNormalTo -> sp+0x180
ldr  r0,[sp,#0x184] ; bl 0x2039794         classify on normal.Y
```

`sp+0x9c` and `sp+0x184`/`0x188` are precisely the operands feeding the `smull` / `>>16`
chains that write the min/max accumulators, so the accumulated quantity is
**depth x faceNormal — the pushback vector**, min/maxed per axis and handed to
`func_02037a6c` at the end. That closes the model end to end: reject chain -> depth ->
classify -> record into the matching slot, set the matching flag bit, accumulate the pushback
extent.

The virtual call is the twin's documented lever again: slot 3 via `[vtable+0xc]`, and it has
to be a genuine C++ virtual call or `this` comes from the callee-saved copy instead of r0.

Draft is **0x58c** against 0x1bc8. Remaining is the per-class record and the min/max
accumulate — both now fully specified rather than guessed.

### Record and accumulate: the model confirmed from three directions (2026-08-06)

The three recorders are matched source and form an exact triple:

```c
func_020379f4(c, h, src) { func_02037fd4(c + 0x74, h, src); }   /* floor */
func_020379c0(c, h, src) { func_02037fd4(c + 0x9c, h, src); }   /* wall  */
func_0203798c(c, h, src) { func_02037fd4(c + 0xc4, h, src); }   /* und   */
```

So the whole classification is now derived **three independent ways that agree**: the
wrappers (which slot each flag bit guards), `func_02039794`'s normal-Y thresholds (which
return 0/1/2), and these three one-line recorders (which offset each writes). Class *n*
-> slot `0x74 + 0x28n` -> flag bit `4 << n`.

Per hit, in ROM order:

```
func_02037fd4(&sphere->result /*0x10*/, triID, info)    always, the shared result
sphere->flags |= 1
  cls 0 floor: if !(flags & 4) { func_020379f4(...); ret |= 1 }
               hitFlags2 = 1 ; flags |= 4
               accumulate (depth * normal.Y) >> 16 into loPY/hiPY  -- Y ONLY
               if (normal.Y > sphere->0x100) func_0203794c(sphere, &normal)
  cls 1 wall : if !(flags & 8)    { func_020379c0(...); ret |= 2 }  flags |= 8
  cls 2 und  : if !(flags & 0x10) { func_0203798c(...); ret |= 4 }  flags |= 0x10
               both accumulate all three axes
```

**A floor accumulates only its vertical component** and separately keeps the most
upward-facing normal seen (`func_0203794c`, which is matched and is just
`d[63..65] = s[0..2]`, i.e. the 0xfc/0x100/0x104 triple). Walls and undersides accumulate the
full XYZ pushback. That asymmetry is the design: standing on a slope should push you up, not
sideways.

Note the `!(flags & bit)` guard on each record -- only the **first** hit of each category is
recorded, but **every** hit contributes to the accumulated extent. That is why the object's
flags are an input as well as an output, and why the wrapper clears bits 4/8/0x10 with
`v & ~0x1c` before calling.

Draft is **0x8a8** against 0x1bc8, about half.

**One error corrected in passing:** an earlier version of this draft called the wall recorder
`func_02037a24`, which I had inferred rather than read. The actual call at 0x01ffd0d4 is
`bl #0x20379c0`. Symbols must come off the call site, never from the shape of a neighbour's
address.

### The bulk is a Voronoi-region test (2026-08-06)

The unexamined 0x1ffbe80..0x1ffcff0 -- about 1100 of the function's 1778 words -- has a
simple shape once you see the dispatch:

```
bl 0x2039488 ; cmp r0,#0 ; bne reject     ShouldPassThroughImpl(this, info, sphere, flag)
cmp r8,r7 / cmp r8,r6 / cmp r7,r6         which of the three EDGE dots is largest
cmp r8,#0  ; ble 0x1ffcaa4                largest <= 0 -> centre is INSIDE -> face case
ldrb r0,[sl,#0x4c]                        this->unk_4c gates the edge/vertex path
smull pairs, >>10                         the real distance against that edge
```

So the per-prism test is the standard sphere-triangle **Voronoi region** decision:

* All three edge dots `<= 0` -> the centre projects inside the triangle, and the face
  distance already in `depth` is the answer. That is the `0x1ffcaa4` case.
* Otherwise the centre is outside the edge with the largest dot, and the true distance is to
  that **edge or one of its endpoints** -- which is why the radius is squared once up front
  (`radius << 4` then `smull r0,r0,r0`, high word at `sp+0x64`): a point-to-segment test
  compares squared lengths, so nothing needs a square root.

Three symmetric branches, one per edge, each doing the edge/vertex distance and its squared
compare. Three copies of the same ~300-word block is most of the 1100.

`this->unk_4c` (init 1) enables the edge/vertex handling; with it clear, only the face case
can register a hit. That is the fifth `MeshCollider` field this function gives a purpose to,
after `unk_34`, `unk_35`, `unk_38` and `unk_44`.

The pass-through filter is the twin's again: `BgCh::ShouldPassThroughImpl(collider, surface,
query, flag)`, sitting between the classify and the record so that a surface the query is
allowed to pass through never reaches the accumulators.

**Order of the per-prism body, settled:**

```
1  three edge-normal rejects, then the face-normal reject
2  depth = rsc - faceDot
3  triID, GetSurfaceInfo (real virtual call), CopyNormalTo, classify on normal.Y
4  ShouldPassThroughImpl -> reject
5  Voronoi dispatch: face, or edge/vertex distance vs the squared radius   <-- the bulk
6  record into the class slot, set the class bit, accumulate depth x normal
```

Steps 1-4 and 6 are written. Step 5 is the remaining work, and it is now three instances of
one known shape rather than an unexplored region.

Draft is **0x8cc** against 0x1bc8.

### The edge/vertex discriminator (2026-08-06)

Inside each of the three symmetric branches, before any distance is computed, there is a
test that decides **edge contact vs vertex contact**. For the branch where edge 1's dot is
largest:

```
smull en1.z, en2.z ; >>10        three component products of the two EDGE NORMALS,
smull en1.x, en2.x ; >>10        summed:
smull en1.y, en2.y ; >>10          nn = dot(en1, en2) >> 10
smull r1,r0, r6,r8 ; >>10          (nn * dot1) >> 10
cmp  r1, r7                        vs dot2
ble 0x1ffc63c  /  b 0x1ffc1ec      -> two different regions
```

and immediately below it the same computation against `en3` (`sp+0x94`) instead of `en2`, so
each branch tests its edge against **both** its neighbours.

`nn` is the cosine between two edge normals: the `>>10` is scale-preserving at the normals'
`1.0 == 0x400` scale (`0x400 * 0x400 >> 10 == 0x400`), which is the check that it is a
normalised dot and not an arbitrary shift. Comparing `dot1 * cos` against `dot2` is the
standard way to decide whether the closest point lies along the shared edge or has passed the
vertex where the two edges meet.

So each branch is: pick the region against neighbour A, pick again against neighbour B, then
compute the squared distance for whichever of edge/vertex won and compare it to the squared
radius parked at `sp+0x60`/`sp+0x64`. Three copies of that -- one per edge -- is the ~1100
words.

The 64-bit intermediates are kept deliberately: `asr r2, r8, #0x1f` sign-extends `dot1` into
`sp+0xd8` and `sp+0xf0` holds another high word, so the products stay 64-bit through the
compare. Any draft that lets these collapse to 32-bit will diverge here.

**This is the last unmapped mechanism.** Everything in the function now has a description:
entry, AABB, radius square, march, descent, step, leaf caches, sorted insert, reject chain,
depth, classify, pass-through filter, Voronoi dispatch, edge/vertex discriminator, record,
accumulate, epilogue. What remains is transcribing three symmetric blocks of fixed-point
arithmetic whose inputs, comparison and output are all known.

### Step 5 is six shared blocks, not three symmetric ones (2026-08-06)

The previous section's model -- "three symmetric branches, each doing the edge/vertex
distance, three copies of the same ~300-word block" -- is **wrong in a way that matters for
how the source is spelled**. Transcribing it as three self-contained branches cannot
reproduce the ROM's control flow.

What is actually there:

```
0x1ffbea8   dispatch: which of dot1/dot2/dot3 is largest      -> 3 blocks
0x1ffc1ec   E1   closest point is on edge 1                   \
0x1ffc35c   E2   closest point is on edge 2                    > 3 EDGE blocks
0x1ffc4cc   E3   closest point is on edge 3                   /
0x1ffc63c   V12  closest point is the vertex of edges 1 and 2 \
0x1ffc750   V23  closest point is the vertex of edges 2 and 3  > 3 VERTEX blocks
0x1ffc89c   V31  closest point is the vertex of edges 3 and 1 /
0x1ffca30   the shared tail: sqrt, depth, sign check
0x1ffcaa4   the face case joins here
0x1ffd314   the reject (`continue`)
```

**The distance blocks are shared, and that is why the ROM branches to labels instead of
falling through.** Each edge's dispatch tests its edge against *both* neighbours; failing
either test means the closest point has passed the vertex the two edges share, so edge 1's
en2 test and edge 2's en1 test both land on V12. Likewise the edge-3 dispatch at 0x1ffc0d0
is entered from both halves of the top-level comparison. Six labels, nine predecessors.

Confirmed exhaustively -- every dispatch target, both arms:

| largest | neighbour test | ble (vertex) | else (edge) |
|---|---|---|---|
| dot1 | vs en2 | V12 `0x1ffc63c` | E1 `0x1ffc1ec` |
| dot1 | vs en3 | V31 `0x1ffc89c` | E1 `0x1ffc1ec` |
| dot2 | vs en3 | V23 `0x1ffc750` | E2 `0x1ffc35c` |
| dot2 | vs en1 | V12 `0x1ffc63c` | E2 `0x1ffc35c` |
| dot3 | vs en1 | V31 `0x1ffc89c` | E3 `0x1ffc4cc` |
| dot3 | vs en2 | V23 `0x1ffc750` | E3 `0x1ffc4cc` |

### The depth formula is not `rsc - faceDot` outside the face case (2026-08-06)

`rsc - faceDot`, stored to `sp+0x9c` at `0x1ffbe3c` before the dispatch, is **only the face
case's answer**. Every edge and vertex region overwrites it in the shared tail at
`0x1ffca80`:

```
depth = SqrtRaw(rsq - d*d) - faceDot;      /* d = the winning edge dot */
if (depth < 0) continue;                    /* `bmi 0x1ffd314` */
```

so `sp+0x9c` is one variable assigned twice, and the 64-bit `rsq - d*d` computed by each
block (`umull`/`mla`/`mla` -- the full signed 64x64 square, using the sign-extension word
each dispatch parked at `sp+0xd8`/`0xe0`/`0xe8`) exists to be square-rooted, not compared.
The earlier note's "compares it against the squared radius" is the wrong operation.

### The inlined square root is NOT cstd::sqrt(u64) (2026-08-06)

Four sites (`0x1ffc2d8`, `0x1ffc448`, `0x1ffc5b8`, `0x1ffca78`) inline the DS hardware sqrt:
save IME, `SQRTCNT = 1`, 64-bit `SQRT_PARAM`, restore IME, spin on `SQRTCNT & 0x8000`, read
`SQRT_RESULT`. But `cstd::sqrt(u64)` at `0x0203d744` (matched, `src/_ZN4cstd4sqrtEy.cpp`)
pre-shifts `x << 2` and rounds its result `(r + 1) >> 1`. **Neither appears here.** So this
is a separate raw inline helper, not that function -- and the `0` and `1` it writes come from
frame slots `sp+0x118` and `sp+0x10c` rather than immediates, which is what four expansions
of one `inline` function look like on this compiler.

### What the edge blocks actually do, and five more named fields (2026-08-06)

Each edge block runs a filter before its distance is taken:

```c
if (sphere.flags & 2)          { cls==1 ? (d > faceDot) : (d > faceDot >> unk_48) -> reject }
else if (cls == 1)             { func_02037e58(&surface)==1 || unk_4d
                                   ? (d > faceDot >> unk_48) : (d > faceDot) -> reject }
else if (d > (faceDot >> unk_48)) {
    if (cls != 0) reject;                       /* only a floor gets the slow path */
    if (sphere.flags & 0x20) reject;
    hyp = SqrtRaw((d>>4)^2 + (faceDot>>4)^2);   /* a real hypotenuse */
    if (func_020397dc(hyp)) reject;             /* |hyp| <= 8: divisor about to vanish */
    if (DotVec3(&surfaceNormal, &this->unk_28) > cstd::fdiv(faceDot >> 4, hyp)) reject;
}
```

* **`MeshCollider::unk_48` is a SHIFT COUNT, not a value** (init 2). The test is "is the
  lateral distance outside this edge more than `faceDot >> unk_48`" -- a slope tolerance
  expressed as a fraction of the penetration.
* **`unk_4d`** (init 0) selects the tolerant form of that test for walls.
* **`unk_28`/`unk_2c`/`unk_30` are one `Vector3`**, not three scalars: `DotVec3` is handed
  `sl+0x28` as a vector. `include/MeshCollider.h` types them as separate `Fix12i`/`s32`.
* **`SphereClsn` flags bit 2 and bit 0x20** gate the filter and the slow path.
* **`SphereClsn+0x108`** is a `normal.y` floor the hit must clear, checked at the face label.

Call census for the whole function: `cstd::fdiv` x8, `func_020397dc` x8, `DotVec3` x4,
`func_02037e58` x3. `func_020397dc(x)` is `|x| <= 8` -- a near-zero divisor guard, and it
precedes every `fdiv`.

### Register model, for reading any of these blocks (2026-08-06)

| | | | |
|---|---|---|---|
| `r8` `r7` `r6` | dot1 dot2 dot3 | `r5` `r4` `[sp+0x94]` | en1 en2 en3 |
| `sb` | **faceDot**, from `0x1ffbd74` on | `[sp+0x98]` | face normal |
| `ip` `sb` `r3` | dx dy dz, but only until `0x1ffbd74` | `[sp+0xa4]` | cls |
| `[sp+0x104]` | rsc | `[sp+0x9c]` | depth |
| `[sp+0x60/0x64]` | rsq, 64-bit | `[sp+0x180]` | surface normal |

`sb` holding dy and then faceDot is the trap: the dispatch's `cmp r8, sb, asr r0` is
*dot1 vs faceDot >> unk_48*, not anything to do with dy.

### V12's prologue, decoded (2026-08-06)

The vertex blocks are the remaining work. V12 (`0x1ffc63c`) begins:

```c
if (func_020397dc(MUL10(nn, nn) - 0x400)) continue;   /* edges within 8 of parallel */
t = cstd::fdiv(MUL10(nn, dot2) - dot1, MUL10(nn, nn) - 0x400) >> 2;
```

then builds the offset to the vertex from `t`, `en1` and `en2` (`smull`/`>>10` pairs against
each component of both normals, and `dot2 - MUL10(t, nn)` as the second coefficient) before
joining the shared tail at `0x1ffc9cc`. The vector half is not transcribed yet.

### Tooling: `mismatches=N/M` is frozen while the sizes differ (2026-08-06)

`fdiff.py` reports `mismatches=999/1778` for *every* draft of this function, before and after
a change that added 513 instructions -- `match.compare` does not produce a meaningful count
until the candidate is the target's size. Score the intermediate drafts with the alignment
ratio instead, and note that `--align-shape` is a *modifier*: without `--align` it prints
nothing at all and you get a silent no-op.

```
python tools/fdiff.py --c <draft> --name _ZN12MeshCollider10DetectClsnER10SphereClsn \
  --module itcm --addr 0x01ffb830 --size 0x1bc8 --version 2004/b56 \
  --align --align-shape --align-changes 0 --quiet
```

Draft progress on that metric: **0x8cc / ratio 0.3494 / 409 shape-equal -> 0x10d0 / ratio
0.5011 / 715 shape-equal**, with the dispatch, the three edge blocks, the filter, the raw
sqrt and the shared tail written and the three vertex blocks still open.

### There is a ~336-word wall block after the face test, and it was never in the map (2026-08-06)

With step 5 written, the alignment still reports `delete: 815` — target instructions with no
counterpart at all. Most of them are one region the handoff's inventory does not mention:
**`0x1ffcaa4`..`0x1ffcfe4`, about 336 words**, entered right after the face test and rejoining
at `0x1ffcfe4`. Section 1 of the handoff lists "the classify, the pass-through filter, the
record, the accumulate, the epilogue" as written; this sits between them and is not any of
them.

It is gated four ways, all of which must pass:

```
sphere.unk_108 >= surfaceNormal.y      0x1ffcaa4   (already in the draft)
sphere.unk_ec  >  0                    0x1ffcab4
cls == 1                               0x1ffcac0   walls only
(tri->length & 0xf0000000) == 0        0x1ffcacc   no flags in the high nibble
```

and what it then does is **reconstruct the triangle's real geometry from the KCL prism**:

* `[sp+0x90]` (the vertex position) is re-read and each component taken `<< 6` into
  `sp+0x18c`/`0x190`/`0x194` — the position at full Fix12i scale rather than the 1/64 units
  the walk uses;
* then a **cross product**, `faceNormal x en2`, componentwise as `MUL10` pairs subtracted and
  stored as three `s16` into a 6-byte vector at `sp+0x16c` (`0x1ffcb0c`..`0x1ffcbb0`), and
  further cross/dot work against `en3` (`sp+0x94`) after it.

That is the standard way to recover a KCL triangle's edges and vertices from
(position, face normal, three edge normals, length), so this is a precise wall contact test
that only runs once the cheap tests have already accepted the prism.

**Do not treat this as part of step 5.** It is a separate mechanism with its own gate, and it
is now the single largest unwritten region in the function.

### The wall block, mapped end to end (2026-08-06)

`0x1ffcaa4`..`0x1ffcfe4`. Gate, in ROM order — the first is a reject, the other three skip the
block and rejoin at `0x1ffcfe4`:

```c
if (sphere.unk_108 < surfaceNormal.y) continue;      /* 0x1ffcaa4, already drafted */
if (sphere.unk_ec > 0 && cls == 1 && !(tri->length & 0xf0000000)) {
    ... the block ...
}
```

`[sp+0x8c]` is the `tri` pointer and `[r0]` off it is `tri->length`, so the last gate is a
high-nibble flag test on the length word.

**Step 1 — reconstruct the triangle's vertices from the KCL prism.** A KCL prism stores a
position, a face normal, three edge normals and a length; the actual vertices come back as
`pos + cross(fn, en_i) * (length / dot(cross(fn, en_i), en3))`. The ROM does this three
times, reusing one 6-byte `s16` scratch vector at `sp+0x16c` for the cross each round:

```c
cr[0] = MUL10(fn[1], ea[2]) - MUL10(fn[2], ea[1]);      /* 0x1ffcb0c, 0x1ffcc8c, ... */
cr[1] = MUL10(fn[2], ea[0]) - MUL10(fn[0], ea[2]);      /* stored as s16 to sp+0x16c */
cr[2] = MUL10(fn[0], ea[1]) - MUL10(fn[1], ea[0]);
cd = MUL10(cr[0], en3[0]) + MUL10(cr[1], en3[1]) + MUL10(cr[2], en3[2]);
if (func_020397dc(cd)) continue;                         /* degenerate prism */
ck = cstd::fdiv(tri->length, cd) >> 2;
v[i] = triPos[i] + (s32)(((s64)cr[i] * ck) >> 14);       /* lsr #0xe + orr lsl #18 */
```

`triPos` is `[sp+0x90]` re-read with each component `<< 6` into `sp+0x18c`/`0x190`/`0x194` —
the position at full Fix12i scale, not the 1/64 units the octree walk uses. The three
reconstructed vertices land at `sp+0x198`/`0x19c`/`0x1a0`, `sp+0x1a4`/`0x1a8`/`0x1ac` and
back into `sp+0x18c`/`0x190`/`0x194`. `ea` is `en2` for the first, `en1` for the second.

**Step 2 — a slab test along the collider's axis.** Each vertex has the sphere centre
(`[sp+0xc4]`) subtracted from it, and the result is dotted with the `Vector3` at
`MeshCollider+0x28` using the *rounded* Fix12 multiply, `(a*b + 0x800) >> 12`
(`smull` / `adds #0x800` / `adc` / `lsr #0xc` / `orr lsl #20`) — note this is a different
multiply from the `>> 10` used everywhere else in the function, because the axis is Fix12i
and the edge normals are not.

The three dots are then compared against `+/- (sphere.unk_ec + sphere.radius)`
(`0x1ffcf9c`: `ldr r6,[fp,#0xec]`, `ldr r3,[fp,#0x48]`, `add r1,r6,r3`, `rsb r2,r1,#0`) —
i.e. a symmetric slab of half-width `unk_ec + radius` about the sphere centre, along the
collider's preferred axis. A wall whose reconstructed triangle lies wholly outside that slab
is not a real contact.

This closes the last unmapped mechanism in the function. Everything from entry to epilogue now
has a description; what remains is transcription, and the two multiplies (`>> 10` unrounded
for normals, `+0x800 >> 12` rounded for the axis) must not be conflated.

### `SphereClsn` fields this block names (2026-08-06)

| offset | meaning |
|---|---|
| `0x48` | radius (already known) |
| `0xec` | slab half-width tolerance, and the block's own enable — `<= 0` skips it |
| `0x108` | a `normal.y` floor the hit must clear |
| flags `0x40` | disables the vertex regions |
| flags `2`, `0x20` | gate the edge filter and its slow path |

### The pass-through filter's last argument is `cls == 1` (2026-08-06)

Not a constant. `0x1ffbe78` builds it straight off the classify with
`cmp r0,#1 / ldreq r3,[sp+0x10c] / movne r3,r0` — 1 for a wall, 0 otherwise — so
`BgCh::ShouldPassThroughImpl(collider, surface, query, isWall)`. The draft passed a literal
zero, which read as correct only because the block around it had never been exercised. Worth
a look at the RaycastGround and RaycastLine twins, which pass the same argument.

### The twin read: four levers tried, all four inert (2026-08-06)

`src/_ZN12MeshCollider10DetectClsnER13RaycastGround.cpp` re-verified `match=True 0/294` on
`2004/b56`, and its header documents four load-bearing levers found by bisection. Applied to
the SphereClsn draft, **all four are byte-neutral** — 1744 instructions, ratio 0.5968, 1051
shape-equal, before and after each:

| lever from the twin | result here |
|---|---|
| every local in one C89 block at function top, none nested in loops | inert |
| `cstd::fdiv` declared `Fix12i` rather than `s32` | inert — `typedef s32 Fix12i` |
| leaf terminator spelled `word & 0x7fffffff` not `& ~0x80000000` | inert — same constant |
| `while (*++leaf)` re-reading rather than caching the index | inert — CSE'd |

**The harness was proven live before any of that was believed.** Positive control
`#pragma optimization_level 1` moved the draft 1744 -> 1648 and the ratio 0.5968 -> 0.5382,
so the compile is real and these nulls are real. Per the standing rule — treat a lever as
dead until a byte delta proves it live *for this function* — none of these four is worth
re-running.

**The hoist is kept anyway**, for two reasons that are not codegen: it matches the matched
twin's shape, and *declaration ORDER cannot be experimented with until the declarations are
in one block*. That is the distinction the twin's note actually draws — the lever is the
order, and the hoist preserved relative order, which is exactly why it changed nothing.

**So the next lever to try is a reorder, not another idiom.** Handoff section 5 has the frame
map (`0x28`-`0x3c` the penetration pairs, `0x40`/`0x44` the flags, `0x48`-`0x5c` the two leaf
triples, `0x60`/`0x64` rsq, `0x74`-`0x7c` the top-3 scores, `0x9c` depth, `0xa0` triID,
`0xa4` cls, `0xa8` contact kind, `0xc4` `&centre`, `0xc8`-`0xd0` rawX/Y/Z, `0x104` rsc) and
the block should be permuted to match it. That is a targeted permutation against a known
answer, not a blind sweep.


### CORRECTION: `--align-shape` cannot see the frame, and I scored four levers with it (2026-08-06)

The previous section concluded "four levers tried, all four inert". **The measurement was
wrong for the one that mattered.** `--align-shape` normalises away register names *and stack
offsets* — that is its documented purpose — so it is structurally incapable of detecting a
change to the frame layout, which is exactly what a declaration reorder is. Scoring decl-order
work with it reports 1051/0.5968 no matter what the frame does.

Measured again with plain `--align` (operand-sensitive), on the same three files:

| draft | `--align` ratio | exactly-equal |
|---|---|---|
| before the hoist | 0.1658 | 292 |
| after the hoist | 0.1658 | 292 |
| after the frame-order permutation | **0.2084** | **367** |

So the hoist really is inert on both metrics — that conclusion survives. But **the reorder is
not inert; it is the largest single gain of the session**, +75 exactly-matching instructions,
and the earlier note called it dead.

**Use `--align` for frame and register work; `--align-shape` only for structure.** Shape was
the right metric while blocks were still missing and the wrong one the moment the work turned
into codegen. Section 1 of the handoff has been corrected.

### What the frame comparison actually shows (2026-08-06)

```
ROM        sub sp, sp, #0x1b4     f -> sp+0x0c      &centre -> sp+0xc4
candidate  sub sp, sp, #0x1dc     f -> sp+0x0c      &centre -> sp+0x10
```

`f` at `0x0c` is the ROM's *first* slot, and it only lands there when `KCL_File *f = kclFile;`
is declared ahead of the whole C89 block — the same shape the matched RaycastGround twin uses
for its `file` and `pos`. Leaving it at the bottom of the block parked it at `0xb8` and was
why the first permutation attempt looked inert.

The frame is still **0x28 bytes too big** (`0x1dc` vs `0x1b4`), so there are about ten surplus
words. Declaring `c` late to chase `0xc4` was tried and is slightly *worse* (0.2084 -> 0.2039),
so the remaining gap is not simply that one slot.

**A concrete lead on the surplus:** the ROM has *three* separate `den` slots for the vertex
blocks (`sp+0xb8`, `0xbc`, `0xc0`) where this draft shares one variable across all three, and
similarly distinct nn spill slots. Each vertex block having its own locals — rather than the
draft's shared `den`/`t`/`u`/`vx`/`vy`/`vz` — is the most likely source of both the offset
mismatch and some of the surplus.

### The surplus-frame hypothesis is dead: splitting the vertex locals makes it worse (2026-08-06)

The previous section's lead — that the draft's shared `den` accounts for the surplus frame
words, because the ROM has three slots (`sp+0xb8`, `0xbc`, `0xc0`) — is **wrong**, and the
same goes for the `nn` spills (`0xf0`, `0xf4`/`0xf8`, `0xfc`/`0x100`).

| draft | frame | `--align` ratio | equal | size |
|---|---|---|---|---|
| shared `nn`/`den` | `0x1dc` | 0.2084 | 367 | 1744 |
| split, one pair per vertex block | `0x1e4` | 0.2095 | 367 | 1725 |

Splitting adds two slots and moves the frame **further** from the ROM's `0x1b4`, not closer.
Exactly-matching instructions do not move at all. The ratio ticks up only because the
`replace` bucket shrinks with the smaller candidate.

The inference that actually follows: the ROM fits *three* `den` variables into `0x1b4` while
this draft fits *one* into `0x1dc`, so there are roughly twelve surplus words that have
nothing to do with the vertex blocks. Look elsewhere — the `nrm[3]`, `tp[3]`, `vb[3]`,
`vc[3]`, `cr[3]` arrays and the `Vector3 sn` are the obvious suspects, since aggregates get
whole slots and the ROM may be holding those in registers or reusing one scratch.

**`t`/`u` were split too and it is byte-neutral** — they never leave registers in any of the
three blocks, so there is no evidence either way and the draft keeps them shared.

**`vx`/`vy`/`vz` must NOT be split.** All three vertex blocks branch to one shared tail at
`0x1ffc9cc` which consumes the vector out of r0/r2/r6, so they are genuinely one variable;
splitting them would force copies the ROM does not make.

The split is kept anyway, because three distinct stack offsets holding three distinct
quantities is direct evidence about the source even when the metric is flat — but it is not
progress on the frame and should not be reported as such.

### The 12 surplus frame words are SCALARS, and the aggregates are already exact (2026-08-06)

Hunted by enumerating every `[sp, #N]` in both disassemblies. The ROM touches **99** distinct
slots, this draft **112** — and the whole difference sits above `0x1ac`, which is the ROM's
highest slot.

Both frames decompose the same way, and that is what settles it:

| | scalars | aggregates | frame |
|---|---|---|---|
| ROM | `0x000`..`0x16c` = **91 words** | `0x16c`..`0x1b0` = 17 words | `0x1b4` = 109 |
| draft | `0x000`..`0x1a0` = **104 words** | 17 words | `0x1e4` = 121 |

**The aggregate region is exactly 17 words on both sides.** So the previous section's
suspicion — that `nrm[3]`, `tp[3]`, `vb[3]`, `vc[3]`, `cr[3]` and `Vector3 sn` were the
surplus — is wrong. They are the right size and the right count; they merely sit 0x58 higher
because 13 *scalar* words are wedged underneath them.

The ROM's aggregate block, read straight off the frame and now mirrored in the draft's
declaration order (byte-neutral, but it is the known-correct answer so there is no reason to
hold a different one):

```
0x16c  cr[3]   s16, 2 words   the cross scratch, reused by both KCL_VERTEX rounds
0x174  nrm[3]                 the DotVec3 argument in the unk_35 branch
0x180  sn                     the surface normal
0x18c  triPos[3]              the KCL position at << 6
0x198  vb[3]                  reconstructed vertex 2
0x1a4  vc[3]                  reconstructed vertex 3
```

`nrm` is proof the two frames really are the same shape here: the ROM's `0x1ffbdb0`..`0x1ffbddc`
and the draft's `+0x53c`..`+0x568` are instruction-for-instruction identical, differing only in
that one is `sp+0x174` and the other `sp+0x1cc`.

**So the target is 13 scalar locals that spill here and do not in the ROM.** Not a
declaration-order problem — reordering was already shown byte-neutral twice. The draft
carries scalars the ROM never materialises: `size`, `mask`, `cy`, `cz`, `one`, `r`, `stepX`,
`z`, `t`, `u`, `vx`, `vy`, `vz`, `cd`, `ck`, `lo`, `hi`, plus `dsq` and `lensq` at two words
each. Thirteen of those are spilling. The next move is to fold them into their uses one at a
time and watch `sub sp, sp, #N`, which is a direct readout and far cheaper than scoring each
attempt on the alignment ratio.

### Chasing the frame directly is a dead end — it is register pressure (2026-08-06)

Every declaration-level lever was swept against `sub sp, sp, #N` directly. Baseline `0x1e4`,
ROM `0x1b4`:

| attempt | frame | verdict |
|---|---|---|
| fold `one` into its two uses | `0x1e4` | inert |
| fold `mask` into its three uses | `0x1e4` | inert |
| fold `size` into its four uses | `0x1e4` | inert |
| fold `lensq` into `dsq` | `0x1e4` | inert |
| fold `stepX` into its use | `0x1ec` | **worse** |
| re-scope the wall-block locals into the wall block | `0x1e4` | inert |
| order the aggregates as the ROM does | `0x1e4` | inert |

Folding a declared local does not free a slot, so mwccarm is not allocating one per
declaration here — it allocates by need, and the conveniences were already register-resident.
Re-scoping does not free one either, so C scope is not driving slot reuse.

And the decisive measurement:

```
below 0x180 : ROM 87   cand 87
below 0x1a0 : ROM 95   cand 95
below 0x1b0 : ROM 99   cand 99      <- ROM's highest slot is 0x1ac
cand also holds 0x1b0 0x1b4 0x1b8 0x1bc 0x1c0 0x1c4 0x1c8 0x1cc 0x1d0 0x1d4 0x1d8 0x1dc 0x1e0
```

**The two frames are identically dense everywhere the ROM has slots at all.** The draft simply
needs thirteen more live values spilled, stacked on top. That is register pressure from the
code being structurally different, not a frame or declaration problem — the surviving thirteen
are the three `den`s, `nrm[3]`, `sn`, a three-word wall-block group and one more, all of which
the ROM also has and fits underneath `0x1ac` by keeping fewer things live at once.

**So stop optimising the frame.** It is a symptom, not a cause; it will close when the code
converges, and no rearrangement of declarations will close it first. The sqrt is not implicated
either — four expansions on both sides, same shape, both loading their `0` and `1` from hoisted
frame slots.

Reusable harness for this kind of question: `scratchpad/sweep.py` applies a list of textual
folds independently and prints frame / ratio / equal per variant, which is far cheaper than
editing and scoring by hand.

### "Declaration order IS the frame" does not hold for this function (2026-08-06)

Four independent classes of declaration-level lever have now been swept against this draft,
each with a positive control proving the harness live, and **every one is byte-neutral**:

| lever | variants | result |
|---|---|---|
| hoist all locals into one C89 block | 1 | inert on both metrics |
| permute the block to the ROM's frame map | 2 | inert (the `f`-first move is the exception, below) |
| re-scope wall-block locals into their block | 1 | inert |
| fold conveniences into their uses | 5 | 4 inert, 1 worse |
| move `en1`/`en2` to steer callee-saved allocation | 6 | all six byte-identical |

The one declaration change that ever mattered was moving `KCL_File *f = kclFile;` ahead of the
block, which is not really an ordering effect — it is the difference between a pre-block
initialised pointer and a block member, and it moved `f` from `sp+0xb8` to `sp+0x0c`.

**So the matched RaycastGround twin's first matching note — "Declaration order IS the stack
layout... mwccarm hands out spill slots in declaration order" — is true for that function and
NOT true for this one.** The twin is `0x498`; this is `0x1bc8`. Whatever the allocator does
above some size or pressure threshold, it stops being steerable from the declaration block.
Do not spend another session permuting declarations here; five sweeps and nineteen variants
say it does nothing.

### Where the structural churn actually is (2026-08-06)

179 shape-alignment change ranges. The largest, by size:

| words | kind | ROM address | region |
|---|---|---|---|
| 96 | draft has extra | `0x01ffbeb8` | the dispatch |
| 89 | draft has extra | `0x01ffbfc8` | the dispatch |
| 77 | ROM has extra | `0x01ffc720` | V23 |
| 58 | draft has extra | `0x01ffbec8` | the dispatch |
| 47 | replace | `0x01ffc57c` | E3 |
| 45 | ROM has extra | `0x01ffcb1c` | the wall block |

The dispatch accounts for ~240 surplus words on its own, and the visible cause is that the
draft reloads the edge-normal pointers from the frame — `ldr r0,[sp,#0xc4]` three times inside
ten instructions — where the ROM holds `en1` in `r5` and `en2` in `r4` across the whole prism
body and only ever spills `en3`. That is the same register pressure the frame hunt landed on,
seen from the other side, and the `en1`/`en2` sweep above shows it cannot be steered by
declaration position.

The remaining levers are therefore genuine source-shape changes that reduce how many values
are live at once across the prism body — not anything in the declaration block.

### Loop control is already converged — it is not the pressure source (2026-08-06)

The theory was that the draft holds loop state in registers where the ROM spills it, starving
the prism body. **It does not.** The ROM's loop tails are pure memory traffic:

```
0x1ffd314  ldr r0,[sp,#0x88] ; ldrh r2,[r0,#2]! ; str r0,[sp,#0x88] ; cmp r2,#0 -> body
0x1ffd328  x += [sp+0x68] ; cmp against [sp+0x14]      (x at 0x84, stepX 0x68, hiX 0x14)
0x1ffd344  y += [sp+0x6c] ; cmp against [sp+0x1c]      (y at 0x80, stepY 0x6c, hiY 0x1c)
0x1ffd378  z += [sp+0x70] ; cmp against [sp+0x24]      (z at 0x20, stepZ 0x70, hiZ 0x24)
```

and the draft emits `ldr r0,[sp,#0x88] ; ldrh r2,[r0,#2]! ; str r0,[sp,#0x88] ; cmp r2,#0`
**instruction-identical and in the same slot**. `f` at `0x0c`, `tri` at `0x8c` and `vtx` at
`0x90` match the ROM exactly too. Nothing about loop control needs changing.

Two frame-map entries fall out of this and complete section 5 of the handoff:

* **`stepX` is `sp+0x68`** — the gap previously marked `?` between `rsq` (`0x60`/`0x64`) and
  `stepY`/`stepZ` (`0x6c`/`0x70`).
* **`z` reuses the `loZ` slot at `sp+0x20`** (`ldr r1,[sp,#0x20] ; add ; str r1,[sp,#0x20]`,
  compared against `hiZ` at `0x24`), where `x` and `y` get their own slots at `0x84`/`0x80`.
  So the AABB block really is `loX 0x10, hiX 0x14, loY 0x18, hiY 0x1c, loZ 0x20, hiZ 0x24`.

One real layout difference does remain, and it is not liveness: the ROM's leaf loop is
**rotated** — body first from `0x1ffbc30`, test last at `0x1ffd314` branching backwards, with
every `continue` in the prism body targeting that bottom test. The draft emits the test at the
top with a forward exit branch and the body after it. That offsets the whole body against the
target and is a plausible contributor to the large INSERT/DELETE ranges, but it is a loop-
rotation question, not a register-pressure one.

### CORRECTION: the leaf loop was never unrotated — I misread my own grep (2026-08-06)

The previous section claimed the draft emits the leaf-loop test at the top where the ROM
rotates it to the bottom, and offered that as a contributor to the large INSERT/DELETE ranges.
**That is wrong.** Rewriting the source as the explicitly rotated

```c
if (*++leaf) do { ... } while (*++leaf);
```

produces output **byte-identical** to `while (*++leaf) { ... }`. Both already contain *two*
copies of `ldrh r2,[r0,#2]!` — an entry test at `+0x3b8` branching to the x-step and a second
at `+0x1a18` branching back into the body — which is exactly the ROM's `0x1ffbc1c` /
`0x1ffbc30` / `0x1ffd314` shape. mwccarm rotates this loop on its own and always did.

The error was mine and it was a tooling slip, not a reasoning one: the grep that produced the
diagnosis ended in `| head -1`, so it showed the entry test and hid the bottom copy, and I
read "one test, at the top" off a command that could only ever print one. **When a structural
claim rests on a count, print the count.**

So loop rotation joins loop control, declaration order, frame layout and the inlined sqrt on
the list of things already converged. The draft and the ROM agree on every structural
question anyone has thought to ask so far; what is left is 179 small shape ranges that are
register allocation and scheduling, and the only lever proven to move those on this function
remains a matched sibling with the same shape.
