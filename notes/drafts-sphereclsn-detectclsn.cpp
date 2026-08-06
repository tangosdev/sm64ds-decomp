//cpp
// NONMATCHING: first draft -- head/AABB only, the walk and prism tests are stubs.
// @symbol _ZN12MeshCollider10DetectClsnER10SphereClsn
/* MeshCollider::DetectClsn(SphereClsn &) at 0x01ffb830 (ITCM), 0x1bc8 bytes.
 * vtable slot 8. Same octree as the RaycastGround twin, but a sphere query needs
 * a full 3-axis AABB like DetectClsn(RaycastLine&), which is why the frame is
 * 0x1b4 rather than 0x4c.
 *
 * The AABB is centre +/- (radius + 0x40) in the file's 1/64 units: 0x40 raw is
 * one whole world unit, i.e. one octree cell of slack, exactly as the Line
 * overload pads its segment bounds.
 */
#include "MeshCollider.h"

struct ClsnResult { u8 raw[0x28]; };
struct SurfaceInfo { u8 clps[8]; Vector3 normal; };

extern "C" void func_02037a6c(SphereClsn *self, s32 loX, s32 loY, s32 loZ,
                              s32 hiX, s32 hiY, s32 hiZ);
extern "C" s32 DotVec3(const s32 *a, const Vector3 *b);
extern "C" s16 func_020396dc(MeshCollider *self, KCL_Tri *tri);
extern "C" void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(SurfaceInfo *self, Vector3 *out);
extern "C" s32 func_02039794(s32 normalY);
extern SurfaceInfo data_020a0cec;
extern "C" void func_02037fd4(ClsnResult *res, s16 triID, SurfaceInfo *info);
extern "C" void func_020379f4(SphereClsn *self, s16 triID, SurfaceInfo *info);
extern "C" void func_020379c0(SphereClsn *self, s16 triID, SurfaceInfo *info);
extern "C" void func_0203798c(SphereClsn *self, s16 triID, SurfaceInfo *info);
extern "C" void func_0203794c(SphereClsn *self, const Vector3 *n);
extern "C" int _ZN4BgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b(void *self, SurfaceInfo *info,
                                                              SphereClsn *q, int flag);
extern "C" int func_020397dc(int x);         /* |x| <= 8 -- a near-zero divisor guard */
extern "C" int func_02037e58(unsigned int *p);
extern "C" Fix12i _ZN4cstd4fdivEii(Fix12i a, Fix12i b);

/* The ROM inlines a RAW hardware sqrt at four sites -- NOT cstd::sqrt(u64)
   (0x0203d744), which pre-shifts `x << 2` and rounds its result `(r + 1) >> 1`.
   Neither the shift nor the rounding is present here, so this is a separate
   inline helper, and the `0` and `1` it writes are loaded from frame slots
   (sp+0x118, sp+0x10c) rather than immediates -- which is what four expansions
   of one inline function look like on this compiler. */
static inline s32 SqrtRaw(u64 x)
{
    volatile u16 *ime = (volatile u16 *)0x4000208;
    u16 saved = *ime;
    *ime = 0;
    *(volatile u16 *)0x40002b0 = 1;
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
    cd = MUL10(cr[0], en3[0]) + MUL10(cr[1], en3[1]) + MUL10(cr[2], en3[2]);  \
    if (func_020397dc(cd)) continue;                                          \
    ck = _ZN4cstd4fdivEii(tri->length, cd) >> 2;                              \
    (out)[0] = tp[0] + (s32)(((s64)cr[0] * ck) >> 14);                        \
    (out)[1] = tp[1] + (s32)(((s64)cr[1] * ck) >> 14);                        \
    (out)[2] = tp[2] + (s32)(((s64)cr[2] * ck) >> 14);

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
#define VERTEX_BLOCK(nn, den, ea, eb, dotI, dotJ)                                  \
    den = MUL10(nn, nn) - 0x400;                                              \
    if (func_020397dc(den)) continue;                                         \
    t = _ZN4cstd4fdivEii(MUL10(nn, dotJ) - (dotI), den) >> 2;                  \
    u = (dotJ) - MUL10(t, nn);                                                \
    vx = MUL10(t, (ea)[0]) + MUL10(u, (eb)[0]);                               \
    vy = MUL10(t, (ea)[1]) + MUL10(u, (eb)[1]);                               \
    vz = MUL10(t, (ea)[2]) + MUL10(u, (eb)[2]);                               \
    goto vtail;

/* The filter each edge block runs before its distance is taken, and the reason
   MeshCollider::unk_48, unk_4d and the 0x28 vector exist. unk_48 is a SHIFT
   COUNT, not a value: the test is "is the lateral distance outside this edge
   more than faceDot >> unk_48", i.e. a slope tolerance expressed as a fraction
   of the penetration. Only a floor (cls 0) that fails it gets the expensive
   path -- a real hypotenuse through the hardware sqrt, then the contact angle
   through cstd::fdiv, compared against the collider's stored axis at +0x28.
   func_020397dc guards the divisor: |x| <= 8 means near-zero, so bail. */
#define EDGE_FILTER(d)                                                        \
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
                          + (s64)(faceDot >> 4) * (faceDot >> 4)));           \
        if (func_020397dc(hyp)) continue;                                     \
        if (DotVec3((const s32 *)&sn, (const Vector3 *)&unk_28)               \
                > _ZN4cstd4fdivEii(faceDot >> 4, hyp)) continue;              \
    }

/* The sphere shape sub-object at 0x38. The destructor stores a third vtable
   here (VT2) and destroys it with func_0203ac1c, so 0x38 is a polymorphic
   member, not loose fields -- see notes/collision-query-classes.md. The entry
   code pins its contents: `add r0,fp,#0x3c` then [r0], [r0,#4], [r0,#8] is a
   Vector3 centre, and `ldr r0,[fp,#0x48]` is the radius. */
struct SphereClsn {
    u8         head[0x10];
    ClsnResult result;       /* 0x10 - the shared 0x28 sub-object */
    u8         pad_038[0x4]; /* 0x38 - the shape sub-object's vptr */
    Vector3    centre;       /* 0x3c */
    Fix12i     radius;       /* 0x48 */
    u8         pad_04c[0x24];
    u8         flags;        /* 0x70 - 1 hit, 4 floor, 8 wall, 0x10 und */
    /* 0x71 + 0x8b lands unk_100 at 0xfc, not 0x100 -- an off-by-4 that was
       harmless only while nothing below 0x100 was referenced. The wall block
       reads 0xec, so the padding is now spelled out per field. */
    u8         pad_071[0x7b];
    s32        unk_ec;       /* 0xec  - slab half-width, and the wall block's
                                        own enable: <= 0 skips it entirely */
    u8         pad_0f0[0x10];
    s32        unk_100;      /* 0x100 - the best floor normal.y so far */
    u8         pad_104[0x4];
    s32        unk_108;      /* 0x108 - a normal.y floor the hit must clear */
};

s32 MeshCollider::DetectClsn(SphereClsn &sphere)
{
    /* The ROM's first slot: `f` is at sp+0x0c, so it is declared FIRST -- ahead of
       the whole C89 block, exactly as the matched RaycastGround twin declares
       `file` and `pos` before its own block. `&centre` at 0xc4 comes with it. */
    KCL_File *f = kclFile;
    const Vector3 *c = &sphere.centre;
    const Vector3 *origin = &f->origin;
    /* DECLARATION ORDER IS THE FRAME. This block is permuted to the ROM's slot
       map (handoff section 5 plus what the step-5 work pinned down), one slot per
       declaration from sp+0x10 upward, rather than to anything the source would
       naturally read as. The run 0x10..0xa8 is contiguous and fully attested;
       everything after it is unplaced and simply follows. */
    s32 loX, hiX;               /* 0x10 0x14 */
    s32 loY, hiY;               /* 0x18 0x1c */
    s32 loZ, hiZ;               /* 0x20 0x24 */
    /* The three running min/max pairs -- the accumulated penetration extent,
       handed to func_02037a6c at the end as (lo.x, lo.y, lo.z, hi.x, hi.y, hi.z).
       Six scalars, not two Vector3s by value: a by-value class parameter would
       have homed r0-r3 to the stack and no homing happens here. */
    s32 loPX, hiPX;             /* 0x28 0x2c */
    s32 loPY, hiPY;             /* 0x30 0x34 */
    s32 loPZ, hiPZ;             /* 0x38 0x3c */
    s32 hitFlags;               /* 0x40 - a bitmask (|= 4 seen); RETURNED */
    s32 hitFlags2;              /* 0x44 - second flag, ORed into the hit test */
    u16 *prev1, *prev2, *prev3; /* 0x48 0x4c 0x50 */
    u16 *p1, *p2, *p3;          /* 0x54 0x58 0x5c */
    s64 rsq;                    /* 0x60 0x64 - squared radius, 64-bit */
    s32 stepY, stepZ;           /* 0x6c 0x70 */
    s32 s1, s2, s3;             /* 0x74 0x78 0x7c - the top-3 scores */
    u32 y, x;                   /* 0x80 0x84 - y before x */
    u16 *leaf;                  /* 0x88 */
    KCL_Tri *tri;               /* 0x8c */
    s32 *vtx;                   /* 0x90 */
    s16 *en3;                   /* 0x94 - only en3 spills; en1/en2 stay in r5/r4 */
    s16 *fn;                    /* 0x98 */
    s32 depth;                  /* 0x9c */
    s16 triID;                  /* 0xa0 */
    s32 cls;                    /* 0xa4 */
    s32 contactKind;            /* 0xa8 */
    s32 den12, den23, den31;    /* 0xb8 0xbc 0xc0 - one per vertex pair */
    s32 rawX, rawY, rawZ;       /* 0xc8 0xcc 0xd0 */
    s32 nn12, nn23, nn31;       /* 0xf0 / 0xf4 0xf8 / 0xfc 0x100 */
    s32 rsc;                    /* 0x104 */
    /* No observed slot -- these follow the attested run. */
    u32 z;
    s32 stepX;
    s32 r;
    u32 one = 1;
    s16 *en1, *en2;
    u32 shift;
    u32 *node;
    u32 idx;
    s32 word;
    s32 size, mask, cy, cz;
    s64 dsq;
    s32 t, u;
    s32 vx, vy, vz;
    s64 lensq;
    s32 dx, dy, dz;
    s32 faceDot;
    s32 nrm[3];
    s32 v;
    s32 dot1, dot2, dot3;
    Vector3 sn;
    s16 cr[3];
    s32 tp[3], vb[3], vc[3];
    s32 cd, ck, lo, hi;


    rawX = c->x >> 6;
    rawY = c->y >> 6;
    rawZ = c->z >> 6;
    r = (sphere.radius >> 6) + 0x40;

    loX = (rawX - origin->x - r) >> 6;
    if (loX < 0) loX = 0;
    hiX = (rawX - origin->x + r) >> 6;
    if (hiX > (s32)~f->xMask) hiX = ~f->xMask;
    if (loX >= hiX) return 0;

    loY = (rawY - origin->y - r) >> 6;
    if (loY < 0) loY = 0;
    hiY = (rawY - origin->y + r) >> 6;
    if (hiY > (s32)~f->yMask) hiY = ~f->yMask;
    if (loY >= hiY) return 0;

    loZ = (rawZ - origin->z - r) >> 6;
    if (loZ < 0) loZ = 0;
    hiZ = (rawZ - origin->z + r) >> 6;
    if (hiZ > (s32)~f->zMask) hiZ = ~f->zMask;
    if (loZ >= hiZ) return 0;

    /* The sphere test is a squared-distance compare, so the radius is scaled up
       (<< 4) and squared into 64 bits once, before the walk. */
    rsc = sphere.radius << 4;
    rsq = (s64)rsc * rsc;

    loPX = hiPX = 0;
    loPY = hiPY = 0;
    loPZ = hiPZ = 0;
    hitFlags = hitFlags2 = 0;
    prev1 = prev2 = prev3 = 0;
    p1 = p2 = p3 = 0;
    s1 = s2 = s3 = 0;

    z = loZ;
    do {
        stepZ = 1000000;
        y = loY;
        do {
            stepY = 1000000;
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

                size = one << shift;
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
                       DetectClsn(RaycastGround&) twin, but the sphere's tolerance
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
                        contactKind = 0;

                        dx = rawX - vtx[0];
                        dy = rawY - vtx[1];
                        dz = rawZ - vtx[2];

                        en1 = f->normals[tri->edgeNormal1Idx];
                        dot1 = dx * en1[0] + dy * en1[1] + dz * en1[2];
                        if (dot1 >= rsc) continue;
                        en2 = f->normals[tri->edgeNormal2Idx];
                        dot2 = dx * en2[0] + dy * en2[1] + dz * en2[2];
                        if (dot2 >= rsc) continue;
                        en3 = f->normals[tri->edgeNormal3Idx];
                        dot3 = dx * en3[0] + dy * en3[1] + dz * en3[2] - tri->length;
                        if (dot3 >= rsc) continue;

                        /* Face normal. Note GT, not the GE the three edges use. */
                        fn = f->normals[tri->normalIdx];
                        faceDot = dx * fn[0] + dy * fn[1] + dz * fn[2];
                        if (faceDot > rsc) continue;

                        /* this->unk_34 / unk_35 gate the face test. These are the
                           MeshCollider bytes whose set/clear accessors were among
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
                           MeshCollider::GetSurfaceInfo and the RaycastGround twin:
                           mwccarm's dispatch reads `this` from r0, a hand-rolled
                           one reads it from the callee-saved copy. */
                        GetSurfaceInfo(triID, data_020a0cec);
                        _ZNK11SurfaceInfo12CopyNormalToER7Vector3(&data_020a0cec, &sn);

                        /* 0 = floor, 1 = wall, 2 = underside -- and those select
                           the 0x74 / 0x9c / 0xc4 result slots and the 4 / 8 / 0x10
                           flag bits respectively. */
                        cls = func_02039794(sn.y);

                        /* Same one-way/pass-through filter the RaycastGround twin
                           applies, and it takes the collider, the surface and the
                           query object.

                           The last argument is NOT a constant: 0x01ffbe78 sets it
                           from the classify with `cmp r0,#1 / ldreq r3,[sp+0x10c]
                           / movne r3,r0`, i.e. `cls == 1` -- a wall tells the
                           filter it is a wall. The draft passed a zero here, which
                           only looked right because the surrounding block was
                           never exercised. */
                        if (_ZN4BgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b(
                                this, &data_020a0cec, &sphere, cls == 1))
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
                                if (MUL10(nn12, dot1) <= dot2) goto v12;
                            } else {
                                nn31 = EDGENORMAL_DOT(en1, en3);
                                if (MUL10(nn31, dot1) <= dot3) goto v31;
                            }
                            /* --- E1: closest point is on edge 1 --- */
                            EDGE_FILTER(dot1)
                            dsq = rsq - (s64)dot1 * dot1;
                            goto tail;
                        }
                        if (dot2 <= dot3) goto edge3;
                        if (dot2 <= 0) goto face;
                        if (!unk_4c) continue;
                        if (dot3 > dot1) {
                            nn23 = EDGENORMAL_DOT(en2, en3);
                            if (MUL10(nn23, dot2) <= dot3) goto v23;
                        } else {
                            nn12 = EDGENORMAL_DOT(en2, en1);
                            if (MUL10(nn12, dot2) <= dot1) goto v12;
                        }
                        /* --- E2 --- */
                        EDGE_FILTER(dot2)
                        dsq = rsq - (s64)dot2 * dot2;
                        goto tail;

                    edge3:
                        if (dot3 <= 0) goto face;
                        if (!unk_4c) continue;
                        if (dot1 > dot2) {
                            nn31 = EDGENORMAL_DOT(en3, en1);
                            if (MUL10(nn31, dot3) <= dot1) goto v31;
                        } else {
                            nn23 = EDGENORMAL_DOT(en3, en2);
                            if (MUL10(nn23, dot3) <= dot2) goto v23;
                        }
                        /* --- E3 --- */
                        EDGE_FILTER(dot3)
                        dsq = rsq - (s64)dot3 * dot3;
                        goto tail;

                    v12:  VERTEX_BLOCK(nn12, den12, en1, en2, dot1, dot2)   /* 0x01ffc63c */
                    v23:  VERTEX_BLOCK(nn23, den23, en2, en3, dot2, dot3)   /* 0x01ffc750 */
                    v31:  VERTEX_BLOCK(nn31, den31, en3, en1, dot3, dot1)   /* 0x01ffc89c */

                    vtail:
                        /* Shared by all three vertex regions (0x01ffc9cc). The
                           `ldreq [sp+0x10c] / ldrne [sp+0x154]` pair here is not a
                           second variable -- both slots are hoisted CONSTANTS, 1
                           and 0, materialised at 0x01ffba18 / 0x01ffba30 (the same
                           trick the inlined sqrt uses for its 0 and 1). So the
                           gate is a plain bit test. */
                        if (sphere.flags & 0x40) continue;
                        lensq = (s64)vx * vx + (s64)vy * vy + (s64)vz * vz;
                        if (faceDot < 0) continue;
                        if ((s64)faceDot * faceDot < lensq) continue;
                        dsq = rsq - lensq;
                        if (dsq <= 0) continue;
                        contactKind = 3;
                        goto tail;

                    tail:
                        /* Every edge/vertex region lands here: the distance is a
                           real square root of the 64-bit remainder, and THAT is
                           the depth -- the `rsc - faceDot` computed above is only
                           the face case's answer. */
                        depth = SqrtRaw((u64)dsq) - faceDot;
                        if (depth < 0) continue;
                        /* 0x01ffca94: the contact kind defaults to 2 here and is
                           set to 3 by the vertex tail, so the face case (which
                           jumps straight past this) keeps whatever it came in
                           with. sp+0x158 and sp+0x160 are the hoisted 3 and 2. */
                        if (!contactKind) contactKind = 2;

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
                        if (sphere.unk_ec > 0 && cls == 1
                                && !(tri->length & 0xf0000000)) {

                            tp[0] = vtx[0] << 6;   /* full Fix12i, not 1/64 */
                            tp[1] = vtx[1] << 6;
                            tp[2] = vtx[2] << 6;

                            KCL_VERTEX(vb, en2)
                            KCL_VERTEX(vc, en1)

                            lo = -(sphere.unk_ec + sphere.radius);
                            hi =   sphere.unk_ec - sphere.radius;

                            if (AXIS_DOT(tp) >= lo && AXIS_DOT(tp) <= hi
                             && AXIS_DOT(vb) >= lo && AXIS_DOT(vb) <= hi
                             && AXIS_DOT(vc) >= lo && AXIS_DOT(vc) <= hi)
                                continue;
                        }
                        /* 0x01ffcfe4: a face contact is kind 1, where the sqrt
                           tail defaulted the edge case to 2 and the vertex tail
                           set 3. */
                        if (!contactKind) contactKind = 1;

                        func_02037fd4(&sphere.result, triID, &data_020a0cec);
                        sphere.flags |= 1;

                        if (cls == 0) {
                            /* Floor: only the vertical component is accumulated. */
                            if (!(sphere.flags & 4)) {
                                func_020379f4(&sphere, triID, &data_020a0cec);
                                hitFlags |= 1;
                            }
                            hitFlags2 = one;
                            sphere.flags |= 4;
                            v = (s32)(((s64)depth * sn.y) >> 16);
                            if (v > hiPY) hiPY = v; else if (v < loPY) loPY = v;
                            /* Keep the most upward-facing floor seen. */
                            if (sn.y > sphere.unk_100) func_0203794c(&sphere, &sn);
                        } else if (cls == 1) {
                            if (!(sphere.flags & 8)) {
                                func_020379c0(&sphere, triID, &data_020a0cec);
                                hitFlags |= 2;
                            }
                            sphere.flags |= 8;
                            v = (s32)(((s64)depth * sn.x) >> 16);
                            if (v > hiPX) hiPX = v; else if (v < loPX) loPX = v;
                            v = (s32)(((s64)depth * sn.y) >> 16);
                            if (v > hiPY) hiPY = v; else if (v < loPY) loPY = v;
                            v = (s32)(((s64)depth * sn.z) >> 16);
                            if (v > hiPZ) hiPZ = v; else if (v < loPZ) loPZ = v;
                        } else {
                            if (!(sphere.flags & 0x10)) {
                                func_0203798c(&sphere, triID, &data_020a0cec);
                                hitFlags |= 4;
                            }
                            sphere.flags |= 0x10;
                            v = (s32)(((s64)depth * sn.x) >> 16);
                            if (v > hiPX) hiPX = v; else if (v < loPX) loPX = v;
                            v = (s32)(((s64)depth * sn.y) >> 16);
                            if (v > hiPY) hiPY = v; else if (v < loPY) loPY = v;
                            v = (s32)(((s64)depth * sn.z) >> 16);
                            if (v > hiPZ) hiPZ = v; else if (v < loPZ) loPZ = v;
                        }
                    }
                }

                x += stepX;
            } while (x <= hiX);
            prev1 = p1; prev2 = p2; prev3 = p3;
            y += stepY;
        } while (y <= hiY);
        z += stepZ;
    } while (z <= hiZ);

    /* The accumulated extent goes back as two corners; the flags word is the
       return value. func_02037a6c (0x02037a6c, 0xb0) is still unnamed. */
    if (!hitFlags && !hitFlags2) return 0;
    func_02037a6c(&sphere, loPX, loPY, loPZ, hiPX, hiPY, hiPZ);
    return hitFlags;
}
