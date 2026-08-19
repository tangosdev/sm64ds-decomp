//cpp
// NONMATCHING: size 0x1b58 vs 0x1bc8 (28 insn short), align equal=565 ratio=0.320
//
// PROVENANCE. Restored 2026-08-19 from nearmiss/db.jsonl, attempt
// 8273344dc1434a9e86882b88eebf7ffa (divergences 1213, parent 1332).
// This body was banked but never committed back to notes/, so the file that lived
// here was the WORSE, earlier draft. Both were re-scored on one worktree at
// 2004/b56 before the swap:
//
//     this body          cand=1750  equal=565  ratio=0.3203  delete=159  (28 insn short)
//     what it replaced   cand=1725  equal=366  ratio=0.2090  delete=182  (53 insn short)
//
// The replaced file's banner claimed "first draft -- head/AABB only, the walk and
// prism tests are stubs". That was false: it was 618 lines with every mechanism
// written. Do not reinstate it.
//
// Levers already swept and DEAD (#1197, positive controls on each): nineteen
// declaration-level variants across hoist / permutation / re-scoping / folding /
// en1-en2 placement, all byte-neutral; frame-size chasing (the surplus is register
// pressure from structural difference); loop rotation (mwccarm rotates this loop
// unprompted and always did). "Declaration order IS the stack layout" holds for the
// 0x498 dBgCh_Gnd twin and NOT for this 0x1bc8 function.
//
// What is left: source-shape change that reduces simultaneous liveness across the
// prism body. This draft reloads edge normals (ldr [sp,#0xc4] three times in ten
// instructions) where the ROM keeps en1 in r5, en2 in r4 and spills only en3.
//
// Score with plain --align; --align-shape normalises away stack offsets and reads
// flat across real gains. mismatches=N/M is frozen at 999 until the sizes match.
// Map and status: notes/collision-system.md.
//
// @symbol _ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr
/* dBgW_Kc::DetectClsn(dBgCh_SphCrr &) at 0x01ffb830 (ITCM), 0x1bc8 bytes. */
#include "dBgW_Kc.h"
#include "dBgCh_SphCrr.h"
#include "dBgPi.h"

extern "C" void func_02037a6c(dBgCh_SphCrr *self, s32 loX, s32 loY, s32 loZ,
                              s32 hiX, s32 hiY, s32 hiZ);
extern "C" s32 DotVec3(const s32 *a, const Vector3 *b);
extern "C" s16 func_020396dc(dBgW_Kc *self, KCL_Tri *tri);
extern "C" void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(SurfaceInfo *self, Vector3 *out);
extern "C" s32 func_02039794(s32 normalY);
extern SurfaceInfo data_020a0cec;
extern "C" void func_02037fd4(dBgPi *res, s16 triID, SurfaceInfo *info);
extern "C" void func_020379f4(dBgCh_SphCrr *self, s16 triID, SurfaceInfo *info);
extern "C" void func_020379c0(dBgCh_SphCrr *self, s16 triID, SurfaceInfo *info);
extern "C" void func_0203798c(dBgCh_SphCrr *self, s16 triID, SurfaceInfo *info);
extern "C" void func_0203794c(dBgCh_SphCrr *self, const Vector3 *n);
extern "C" int _ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b(void *self, SurfaceInfo *info,
                                                              dBgCh_SphCrr *q, int flag);
extern "C" int func_020397dc(int x);
extern "C" int func_02037e58(unsigned int *p);
extern "C" Fix12i _ZN4cstd4fdivEii(Fix12i a, Fix12i b);

/* The ROM inlines a RAW hardware sqrt at four sites -- NOT cstd::sqrt(u64)
   (0x0203d744), which pre-shifts `x << 2` and rounds its result `(r + 1) >> 1`.
   Neither the shift nor the rounding is present here, so this is a separate
   inline helper, and the `0` and `1` it writes are loaded from frame slots
   (sp+0x118, sp+0x10c) rather than immediates -- which is what four expansions
   of one inline function look like on this compiler. */
static inline s32 SqrtRaw(u64 x, s32 zval, s32 one)
{
    volatile u16 *ime = (volatile u16 *)0x4000208;
    u16 saved = *ime;
    *ime = (u16)zval;
    *(volatile u16 *)0x40002b0 = (u16)one;
    *(volatile u64 *)0x40002b8 = x;
    *ime;
    *ime = saved;
    while (*(volatile u16 *)0x40002b0 & 0x8000)
        ;
    return *(volatile s32 *)0x40002b4;
}

/* The scale-preserving product for the normals' 1.0 == 0x400 scale. Both
   operands stay 64-bit into the shift; letting this collapse to 32 bits is the
   documented way this function diverges. */
#define MUL10(a, b) ((s32)(((s64)(a) * (b)) >> 10))

/* nn = cos between two edge normals, at the same 0x400 scale. */
#define EDGENORMAL_DOT(a, b) \
    (MUL10((a)[0], (b)[0]) + MUL10((a)[1], (b)[1]) + MUL10((a)[2], (b)[2]))

/* The ROUNDED Fix12 multiply -- `smull` / `adds #0x800` / `adc` / `lsr #0xc` /
   `orr lsl #20`. The wall block uses this and nothing else does: the collider's
   axis is Fix12i where the edge normals are 0x400-scale, so MUL10 is wrong there
   and this is wrong everywhere else. */
#define FX12(a, b) ((s32)((((s64)(a) * (b)) + 0x800) >> 12))

/* One vertex of the triangle, recovered from the KCL prism: the prism stores a
   position, a face normal, three edge normals and a length, and the vertex is
   `pos + cross(fn, ea) * (length / dot(cross(fn, ea), en3))`. The cross goes
   through a 6-byte s16 scratch (sp+0x16c, reused by both rounds), so it really
   does truncate to 16 bits. The quotient's `>> 2` and the product's `>> 14` are
   fdiv's Fix12 result reconciled with the 0x400 normal scale. */
#define KCL_VERTEX(out, ea)                                                   \
    cr[0] = (s16)(MUL10(fn[1], (ea)[2]) - MUL10(fn[2], (ea)[1]));             \
    cr[1] = (s16)(MUL10(fn[2], (ea)[0]) - MUL10(fn[0], (ea)[2]));             \
    cr[2] = (s16)(MUL10(fn[0], (ea)[1]) - MUL10(fn[1], (ea)[0]));             \
    t = MUL10(cr[0], en3[0]) + MUL10(cr[1], en3[1]) + MUL10(cr[2], en3[2]);   \
    if (func_020397dc(t)) continue;                                           \
    u = _ZN4cstd4fdivEii(tri->length, t) >> 2;                                \
    (out)[0] = tp[0] + (s32)(((s64)cr[0] * u) >> 14);                         \
    (out)[1] = tp[1] + (s32)(((s64)cr[1] * u) >> 14);                         \
    (out)[2] = tp[2] + (s32)(((s64)cr[2] * u) >> 14);

/* That vertex's offset from the sphere centre, projected on the collider axis. */
#define AXIS_DOT(v) (FX12((v)[0] - c->x, unk_28)                              \
                   + FX12((v)[1] - c->y, unk_2c)                              \
                   + FX12((v)[2] - c->z, unk_30))

/* A vertex region: the closest point is where edge normals `ea` and `eb` meet.
   `nn` is their cosine, already formed by the dispatch. Solve the 2x2 for the
   point's coefficients in that basis, then hand the offset to the vertex tail.

   `func_020397dc(den)` is the divisor guard: |den| <= 8 means the two edge
   normals are within a hair of parallel and the 2x2 is singular. The `>> 2` on
   the quotient is cstd::fdiv's Fix12 result brought back to the 0x400 scale. */
#define VERTEX_BLOCK(nn, nnh, den, ea, eb, dotI, dotJ)                        \
    den = MUL10(nn, nn) - 0x400;                                              \
    den += (nnh) - (nnh);                                                     \
    if (func_020397dc(den)) continue;                                         \
    t = _ZN4cstd4fdivEii(MUL10(nn, dotJ) - (dotI), den) >> 2;                  \
    u = (dotJ) - MUL10(t, nn);                                                \
    vx = MUL10(t, (ea)[0]) + MUL10(u, (eb)[0]);                               \
    vy = MUL10(t, (ea)[1]) + MUL10(u, (eb)[1]);                               \
    vz = MUL10(t, (ea)[2]) + MUL10(u, (eb)[2]);                               \
    goto vtail;

/* The filter each edge block runs before its distance is taken, and the reason
   dBgW_Kc::unk_48, unk_4d and the 0x28 vector exist. unk_48 is a SHIFT
   COUNT, not a value: the test is "is the lateral distance outside this edge
   more than faceDot >> unk_48", i.e. a slope tolerance expressed as a fraction
   of the penetration. Only a floor (cls 0) that fails it gets the expensive
   path -- a real hypotenuse through the hardware sqrt, then the contact angle
   through cstd::fdiv, compared against the collider's stored axis at +0x28.
   func_020397dc guards the divisor: |x| <= 8 means near-zero, so bail. */
#define EDGE_FILTER(d, zval)                                                  \
    if (sphere.flags & 2) {                                                   \
        if (cls == 1) { if ((d) > faceDot) continue; }                        \
        else if ((d) > (faceDot >> unk_48)) continue;                         \
    } else if (cls == 1) {                                                    \
        if (func_02037e58((unsigned int *)&data_020a0cec) == 1) {             \
            if ((d) > (faceDot >> unk_48)) continue;                          \
        } else if (unk_4d) {                                                  \
            if ((d) > (faceDot >> unk_48)) continue;                          \
        } else {                                                              \
            if ((d) > faceDot) continue;                                      \
        }                                                                     \
    } else if ((d) > (faceDot >> unk_48)) {                                   \
        s32 hyp;                                                              \
        if (cls != 0) continue;                                               \
        if (sphere.flags & 0x20) continue;                                    \
        hyp = SqrtRaw((u64)((s64)((d) >> 4) * ((d) >> 4)                       \
                          + (s64)(faceDot >> 4) * (faceDot >> 4)), zval, k1); \
        if (func_020397dc(hyp)) continue;                                     \
        if (DotVec3((const s32 *)&sn, (const Vector3 *)&unk_28)               \
                > _ZN4cstd4fdivEii(faceDot >> 4, hyp)) continue;              \
    }

s32 dBgW_Kc::DetectClsn(dBgCh_SphCrr &sphere)
{
    KCL_File *f;
    s32 loX, hiX;
    s32 loY, hiY;
    s32 loZ, hiZ;
    s32 loPX, hiPX;
    s32 loPY, hiPY;
    s32 loPZ, hiPZ;
    s32 hitFlags;
    s32 hitFlags2;
    u16 *prev1, *prev2, *prev3;
    u16 *p1, *p2, *p3;
    s64 rsq;
    s32 stepX;
    s32 stepY, stepZ;
    s32 s1, s2, s3;
    u32 y, x;
    u16 *leaf;
    KCL_Tri *tri;
    s32 *vtx;
    s16 *en3;
    s16 *fn;
    s32 depth;
    s16 triID;
    s32 cls;
    s32 contactKind;
    s32 den12, den23, den31;
    const Vector3 *c;
    s32 rawX, rawY, rawZ;
    s32 d1h, d2h, d3h;
    s32 nn12, n12h;
    s32 nn23, n23h;
    s32 nn31, n31h;
    s32 rsc;
    s32 z108;
    s32 k1;
    s32 k0;
    s32 z118, z11c, z120, z154;
    s32 k3;
    s32 z15c;
    s32 k2;
    s32 tlen;
    s32 passArg;
    u32 z;
    s16 *en1, *en2;
    u32 shift;
    u32 *node;
    u32 idx;
    s32 word;
    s32 size, mask, cy, cz;
    s64 dsq;
    s32 t, u;
    s32 vx, vy, vz;
    s32 faceDot;
    s32 v;
    s32 dot1, dot2, dot3;
    s16 cr[3];
    s32 nrm[3];
    Vector3 sn;
    s32 tp[3], vb[3], vc[3];

    c = &sphere.pos;
    f = kclFile;
    {
        const Vector3 *origin = &f->origin;
        s32 slack = (sphere.radius >> 6) + 0x40;

        rawX = c->x >> 6;
        rawY = c->y >> 6;
        rawZ = c->z >> 6;

        loX = (rawX - origin->x - slack) >> 6;
        if (loX < 0) loX = 0;
        hiX = (rawX - origin->x + slack) >> 6;
        if (hiX > (s32)~f->xMask) hiX = ~f->xMask;
        if (loX >= hiX) return 0;

        loY = (rawY - origin->y - slack) >> 6;
        if (loY < 0) loY = 0;
        hiY = (rawY - origin->y + slack) >> 6;
        if (hiY > (s32)~f->yMask) hiY = ~f->yMask;
        if (loY >= hiY) return 0;

        loZ = (rawZ - origin->z - slack) >> 6;
        if (loZ < 0) loZ = 0;
        hiZ = (rawZ - origin->z + slack) >> 6;
        if (hiZ > (s32)~f->zMask) hiZ = ~f->zMask;
        if (loZ >= hiZ) return 0;
    }

    rsc = sphere.radius << 4;

    hiPX = loPX = 0;
    hiPY = loPY = 0;
    hiPZ = loPZ = 0;
    hitFlags2 = hitFlags = 0;
    prev3 = prev2 = prev1 = 0;
    p3 = p2 = p1 = 0;

    rsq = (s64)rsc * rsc;
    k0 = loPX;
    k1 = 1;
    z118 = loPX;
    z11c = loPX;
    z120 = loPX;
    z154 = loPX;
    k3 = 3;
    z15c = loPX;
    k2 = 2;
    z108 = loPX;

    z = loZ;
    do {
        stepZ = 1000000;
        y = loY;
        do {
            stepY = 1000000;
            s1 = s2 = s3 = z108;
            x = loX;
            do {
                shift = f->coordShift;

                idx = (z >> shift) << f->zShift
                    | (y >> shift) << f->yShift;
                idx |= x >> shift;
                node = (u32 *)f->unk_0c;
                word = node[idx];

                while (word >= 0) {
                    node = (u32 *)((u8 *)node + word);
                    shift--;
                    word = node[((z >> shift) & 1) << 2
                              | ((y >> shift) & 1) << 1
                              | ((x >> shift) & 1)];
                }

                leaf = (u16 *)((u8 *)node + (word & 0x7fffffff));

                size = k1 << shift;
                mask = size - 1;
                stepX = size - (x & mask);
                cz = size - (z & mask);
                cy = size - (y & mask);
                if (cz < stepZ) stepZ = cz;
                if (cy < stepY) stepY = cy;

                /* Three leaves already covered by the previous row, not one:
                   the whole cell is skipped when it repeats any of them. */
                if (leaf != prev1 && leaf != prev2 && leaf != prev3) {
                    /* Keep the three widest non-empty leaves of this row, sorted
                       descending on cy -- the generalisation of the Line
                       overload's single rowLeaf/rowStep pair. */
                    if (leaf[1] != 0 && cy > s3) {
                        if (cy > s2) {
                            if (cy > s1) {
                                s3 = s2; s2 = s1; s1 = cy;
                                p3 = p2; p2 = p1; p1 = leaf;
                            } else {
                                s3 = s2; s2 = cy;
                                p3 = p2; p2 = leaf;
                            }
                        } else {
                            s3 = cy; p3 = leaf;
                        }
                    }

                    /* Same three side-plane rejects as the matched
                       DetectClsn(dBgCh_Gnd&) twin, but the sphere's tolerance
                       is its own radius (raw units x 0x400, matching the normals'
                       1.0 == 0x400 scale) where the twin uses a fixed 0x20000. */
                    while (*++leaf) {
                        tri = &f->tris[*leaf];
                        vtx = f->positions[tri->posIdx];
                        /* Three separately live edge-normal pointers, not one
                           reused cursor: the ROM keeps en1 in r5, en2 in r4 and
                           en3 spilled at sp+0x94, and step 5 reads all three
                           again after the reject chain has finished with them. */
                        /* sp+0xa8, seeded per prism at 0x01ffbe8c, set to 3 by the
                           vertex tail and defaulted to 2 by the shared tail. The
                           seed is sp+0x114, which 0x01ffba10 loads from sp+0x28
                           one instruction after 0x01ffb9c8 zeroed it -- so it is
                           the hoisted constant 0, not a value from anywhere. */
                        {
                        s32 dx = rawX - vtx[0];
                        s32 dy = rawY - vtx[1];
                        s32 dz = rawZ - vtx[2];
                        en1 = f->normals[tri->edgeNormal1Idx];
                        dot1 = dx * en1[0] + dy * en1[1] + dz * en1[2];
                        if (dot1 >= rsc) continue;
                        en2 = f->normals[tri->edgeNormal2Idx];
                        dot2 = dx * en2[0] + dy * en2[1] + dz * en2[2];
                        if (dot2 >= rsc) continue;
                        en3 = f->normals[tri->edgeNormal3Idx];
                        tlen = tri->length;
                        dot3 = dx * en3[0] + dy * en3[1] + dz * en3[2] - tlen;
                        if (dot3 >= rsc) continue;

                        fn = f->normals[tri->normalIdx];
                        faceDot = dx * fn[0] + dy * fn[1] + dz * fn[2];
                        if (faceDot > rsc) continue;
                        }

                        /* this->unk_34 / unk_35 gate the face test. These are the
                           dBgW_Kc bytes whose set/clear accessors were among
                           the original eleven ITCM matches (func_01ffb098/0a4 for
                           0x35, func_01ffb0b0/0bc for 0x34); this is what they are
                           for. unk_38 is the Vector3 func_01ffb07c writes, seeded
                           by SetFile to the unit X vector. */
                        if (unk_34) {
                            if (faceDot < -0x50000) continue;
                        } else if (unk_35) {
                            nrm[0] = fn[0] << 2;
                            nrm[1] = fn[1] << 2;
                            nrm[2] = fn[2] << 2;
                            if (DotVec3(nrm, (const Vector3 *)&unk_38) > unk_44) {
                                if (faceDot < -f->unk_10) continue;
                            } else {
                                if (faceDot < -0xa0000) continue;
                            }
                        } else {
                            if (faceDot < -f->unk_10) continue;
                        }

                        /* Accepted. The overlap along the face normal is what gets
                           accumulated; depth * normal is the pushback vector. */
                        depth = rsc - faceDot;
                        triID = func_020396dc(this, tri);

                        /* A REAL virtual call -- slot 3. Same lever as
                           dBgW_Kc::GetSurfaceInfo and the dBgCh_Gnd twin:
                           mwccarm's dispatch reads `this` from r0, a hand-rolled
                           one reads it from the callee-saved copy. */
                        GetSurfaceInfo(triID, data_020a0cec);
                        _ZNK11SurfaceInfo12CopyNormalToER7Vector3(&data_020a0cec, &sn);

                        /* 0 = floor, 1 = wall, 2 = underside -- and those select
                           the 0x74 / 0x9c / 0xc4 result slots and the 4 / 8 / 0x10
                           flag bits respectively. */
                        cls = func_02039794(sn.y);
                        contactKind = k0;
                        passArg = k1;
                        if (cls != 1) passArg = k0;
                        if (_ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b(
                                this, &data_020a0cec, &sphere, passArg))
                            continue;

                        /* Voronoi region select: whichever edge the centre is
                           furthest outside decides whether this is a face, edge or
                           vertex contact. All three dots <= 0 means the centre
                           projects inside the triangle and the face distance --
                           already in `depth` -- is the answer. Otherwise the true
                           distance has to be computed against that edge or its
                           endpoints, which is what the squared radius at sp+0x60/64
                           is compared against.

                           TODO: the three symmetric edge/vertex branches. They are
                           gated by this->unk_4c and are ~1100 of the function's
                           1778 words. */
                        /* The dispatch picks the edge the centre is furthest
                           outside, then asks -- against BOTH of that edge's
                           neighbours -- whether the closest point is still on the
                           edge or has passed a vertex.

                           The six distance blocks are SHARED, which is the part
                           the earlier note got wrong: each vertex block is entered
                           from the two edges that meet at it (V12 from edge 1's
                           en2 test and edge 2's en1 test, and so on), and the
                           edge-3 dispatch is entered from both halves of the top
                           test. That sharing is why the ROM branches to labels
                           instead of falling through, and why gotos are the
                           faithful spelling here. */
                        if (dot1 > dot2) {
                            if (dot1 <= dot3) goto edge3;
                            if (dot1 <= 0) goto face;
                            if (!unk_4c) continue;
                            if (dot2 > dot3) {
                                nn12 = EDGENORMAL_DOT(en1, en2);
                                d1h = dot1 >> 31;
                                n12h = nn12 >> 31;
                                if (MUL10(nn12, dot1) <= dot2) goto v12;
                            } else {
                                nn31 = EDGENORMAL_DOT(en1, en3);
                                d1h = dot1 >> 31;
                                n31h = nn31 >> 31;
                                if (MUL10(nn31, dot1) <= dot3) goto v31;
                            }
                            EDGE_FILTER(dot1, z118)
                            d1h = dot1 >> 31;
                            dsq = rsq - (s64)dot1 * dot1;
                            goto tail;
                        }
                        if (dot2 <= dot3) goto edge3;
                        if (dot2 <= 0) goto face;
                        if (!unk_4c) continue;
                        if (dot3 > dot1) {
                            nn23 = EDGENORMAL_DOT(en2, en3);
                            d2h = dot2 >> 31;
                            n23h = nn23 >> 31;
                            if (MUL10(nn23, dot2) <= dot3) goto v23;
                        } else {
                            nn12 = EDGENORMAL_DOT(en2, en1);
                            d2h = dot2 >> 31;
                            n12h = nn12 >> 31;
                            if (MUL10(nn12, dot2) <= dot1) goto v12;
                        }
                        EDGE_FILTER(dot2, z11c)
                        d2h = dot2 >> 31;
                        dsq = rsq - (s64)dot2 * dot2;
                        goto tail;

                    edge3:
                        if (dot3 <= 0) goto face;
                        if (!unk_4c) continue;
                        if (dot1 > dot2) {
                            nn31 = EDGENORMAL_DOT(en3, en1);
                            d3h = dot3 >> 31;
                            n31h = nn31 >> 31;
                            if (MUL10(nn31, dot3) <= dot1) goto v31;
                        } else {
                            nn23 = EDGENORMAL_DOT(en3, en2);
                            d3h = dot3 >> 31;
                            n23h = nn23 >> 31;
                            if (MUL10(nn23, dot3) <= dot2) goto v23;
                        }
                        EDGE_FILTER(dot3, z120)
                        d3h = dot3 >> 31;
                        dsq = rsq - (s64)dot3 * dot3;
                        goto tail;

                    v12:  VERTEX_BLOCK(nn12, n12h, den12, en1, en2, dot1, dot2)
                    v23:  VERTEX_BLOCK(nn23, n23h, den23, en2, en3, dot2, dot3)
                    v31:  VERTEX_BLOCK(nn31, n31h, den31, en3, en1, dot3, dot1)

                    vtail:
                        v = k1;
                        if (sphere.flags & 0x40) v = z154;
                        if (!v) continue;
                        dsq = (s64)vx * vx + (s64)vy * vy + (s64)vz * vz;
                        if (faceDot < 0) continue;
                        if ((s64)faceDot * faceDot < dsq) continue;
                        dsq = rsq - dsq;
                        if (dsq <= 0) continue;
                        contactKind = k3;
                        goto tail;

                    tail:
                        depth = SqrtRaw((u64)dsq, z15c, k1) - faceDot;
                        if (depth < 0) continue;
                        if (!contactKind) contactKind = k2;

                    face:
                        if (sphere.unk_108 < sn.y) continue;

                        /* Walls only, and only once the cheap tests have accepted
                           the prism. Rebuild the triangle's three real vertices --
                           the stored position IS the first one, the other two come
                           off the two crosses -- and reject the contact when all
                           three sit inside a slab about the sphere centre measured
                           along the collider's axis.

                           The slab is NOT symmetric: the bounds are
                           -(unk_ec + radius) and (unk_ec - radius), built at
                           0x01ffcfa8 by `add r1,r6,r3` then `rsb r2,r1,#0` and
                           then re-formed by `sub r1,r6,r3`. And the sense is
                           wholly-INSIDE rejects -- the last compare is
                           `ble 0x1ffd314`, the reject -- so a triangle that
                           escapes the slab in any component keeps its hit. */
                        if (sphere.unk_0ec > 0 && cls == 1
                                && !(tri->length & 0xf0000000)) {

                            tp[0] = vtx[0] << 6;
                            tp[1] = vtx[1] << 6;
                            tp[2] = vtx[2] << 6;

                            KCL_VERTEX(vb, en2)
                            KCL_VERTEX(vc, en1)

                            t = -(sphere.unk_0ec + sphere.radius);
                            u =   sphere.unk_0ec - sphere.radius;

                            if (AXIS_DOT(tp) >= t && AXIS_DOT(tp) <= u
                             && AXIS_DOT(vb) >= t && AXIS_DOT(vb) <= u
                             && AXIS_DOT(vc) >= t && AXIS_DOT(vc) <= u)
                                continue;
                        }
                        if (!contactKind) contactKind = k1;

                        func_02037fd4((dBgPi *)&sphere.unk_010, triID, &data_020a0cec);
                        sphere.flags |= 1;

                        if (cls == 0) {
                            if (!(sphere.flags & 4)) {
                                func_020379f4(&sphere, triID, &data_020a0cec);
                                hitFlags |= 1;
                            }
                            hitFlags2 = k1;
                            sphere.flags |= 4;
                            v = (s32)(((s64)depth * sn.y) >> 14) >> 2;
                            if (v > hiPY) hiPY = v; else if (v < loPY) loPY = v;
                            if (sn.y > sphere.unk_100) func_0203794c(&sphere, &sn);
                        } else if (cls == 1) {
                            sphere.flags |= 8;
                            func_020379c0(&sphere, triID, &data_020a0cec);
                            hitFlags |= 2;
                            v = (s32)(((s64)depth * sn.x) >> 14) >> 2;
                            if (v > hiPX) hiPX = v; else if (v < loPX) loPX = v;
                            if (contactKind != 1) {
                                v = (s32)(((s64)depth * sn.y) >> 14) >> 2;
                                if (v > hiPY) hiPY = v; else if (v < loPY) loPY = v;
                            }
                            v = (s32)(((s64)depth * sn.z) >> 14) >> 2;
                            if (v > hiPZ) hiPZ = v; else if (v < loPZ) loPZ = v;
                        } else {
                            sphere.flags |= 0x10;
                            func_0203798c(&sphere, triID, &data_020a0cec);
                            hitFlags |= 4;
                            v = (s32)(((s64)depth * sn.x) >> 14) >> 2;
                            if (v > hiPX) hiPX = v; else if (v < loPX) loPX = v;
                            v = (s32)(((s64)depth * sn.y) >> 14) >> 2;
                            if (v > hiPY) hiPY = v; else if (v < loPY) loPY = v;
                            v = (s32)(((s64)depth * sn.z) >> 14) >> 2;
                            if (v > hiPZ) hiPZ = v; else if (v < loPZ) loPZ = v;
                        }
                    }
                }

                x += stepX;
            } while (x <= hiX);
            y += stepY;
            prev1 = p1; prev2 = p2; prev3 = p3;
        } while (y <= hiY);
        z += stepZ;
    } while (z <= hiZ);

    /* The accumulated extent goes back as two corners; the flags word is the
       return value. func_02037a6c (0x02037a6c, 0xb0) is still unnamed. */
    if (!hitFlags && !hitFlags2) return 0;
    func_02037a6c(&sphere, loPX, loPY, loPZ, hiPX, hiPY, hiPZ);
    return hitFlags;
}
