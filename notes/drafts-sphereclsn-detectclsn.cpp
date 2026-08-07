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
    u8         pad_071[0x8b];
    s32        unk_100;      /* 0x100 - the best floor normal.y so far */
};

s32 MeshCollider::DetectClsn(SphereClsn &sphere)
{
    s32 loX, hiX;
    s32 loY, hiY;
    s32 loZ, hiZ;
    s32 rawX, rawY, rawZ;
    s32 r;
    s32 rsc;
    s64 rsq;
    u32 x, y, z;
    s32 stepX, stepY, stepZ;
    u32 one = 1;
    s32 passArg = 0;
    /* Slot order is declaration order on this compiler, and these fourteen words
       are the ROM's zeroed block, in order.

       The first six are three running min/max pairs -- the accumulated
       penetration extent, written by `if (v > hi) hi = v; else if (v < lo) lo = v`
       at every hit and handed to func_02037a6c at the end as
       (lo.x, lo.y, lo.z, hi.x, hi.y, hi.z). Six scalars, not two Vector3s by
       value: a by-value class parameter would have homed r0-r3 to the stack (the
       runbook section 7 dead end) and no homing happens here. */
    s32 loPX, hiPX;             /* sp+0x28 / sp+0x2c */
    s32 loPY, hiPY;             /* sp+0x30 / sp+0x34 */
    s32 loPZ, hiPZ;             /* sp+0x38 / sp+0x3c */
    s32 hitFlags;               /* sp+0x40 - a bitmask (|= 4 seen); RETURNED */
    s32 hitFlags2;              /* sp+0x44 - second flag, ORed into the hit test */
    /* ...and the last six of the fourteen are the two leaf triples: */
    u16 *prev1, *prev2, *prev3;
    u16 *p1, *p2, *p3;
    s32 s1, s2, s3;
    u16 *leaf;

    KCL_File *f = kclFile;
    const Vector3 *c = &sphere.centre;
    const Vector3 *origin = &f->origin;

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
                u32 shift = f->coordShift;
                u32 *node;
                u32 idx;
                s32 v;
                s32 size, mask, cy, cz;
                u32 lv;

                idx = (z >> shift) << f->zShift
                    | (y >> shift) << f->yShift;
                idx |= x >> shift;
                node = (u32 *)f->unk_0c;
                v = node[idx];

                while (v >= 0) {
                    node = (u32 *)((u8 *)node + v);
                    shift--;
                    v = node[((z >> shift) & 1) << 2
                           | ((y >> shift) & 1) << 1
                           | ((x >> shift) & 1)];
                }

                leaf = (u16 *)((u8 *)node + (v & ~0x80000000));

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
                    while ((lv = *++leaf) != 0) {
                        KCL_Tri *tri = &f->tris[lv];
                        s32 *vtx = f->positions[tri->posIdx];
                        s16 *en;
                        s16 *fn;
                        s32 dx, dy, dz;
                        s32 faceDot;
                        s32 nrm[3];
                        s32 depth;
                        s16 triID;
                        s32 cls;
                        s32 v;
                        s32 dot1, dot2, dot3;
                        Vector3 sn;

                        dx = rawX - vtx[0];
                        dy = rawY - vtx[1];
                        dz = rawZ - vtx[2];

                        en = f->normals[tri->edgeNormal1Idx];
                        dot1 = dx * en[0] + dy * en[1] + dz * en[2];
                        if (dot1 >= rsc) continue;
                        en = f->normals[tri->edgeNormal2Idx];
                        dot2 = dx * en[0] + dy * en[1] + dz * en[2];
                        if (dot2 >= rsc) continue;
                        en = f->normals[tri->edgeNormal3Idx];
                        dot3 = dx * en[0] + dy * en[1] + dz * en[2] - tri->length;
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
                           query object. */
                        if (_ZN4BgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b(
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
                        if (dot1 > dot2) {
                            if (dot1 > dot3) { /* edge 1 */ }
                            else             { /* edge 3 */ }
                        } else {
                            if (dot2 > dot3) { /* edge 2 */ }
                            else             { /* edge 3 */ }
                        }

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
