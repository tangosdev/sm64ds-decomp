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
    /* Slot order is declaration order on this compiler. The ROM's fourteen zeroed
       words end with these two triples: prev1..3 at sp+0x48/0x4c/0x50 and
       p1..3 at sp+0x54/0x58/0x5c; the scores s1..3 are sp+0x74/0x78/0x7c. */
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

                    /* TODO: the per-prism squared-distance tests go here. */
                }

                x += stepX;
            } while (x <= hiX);
            prev1 = p1; prev2 = p2; prev3 = p3;
            y += stepY;
        } while (y <= hiY);
        z += stepZ;
    } while (z <= hiZ);

    return (s32)rsq;
}
