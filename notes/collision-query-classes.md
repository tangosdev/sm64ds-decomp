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
