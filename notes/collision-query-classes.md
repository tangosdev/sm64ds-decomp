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
