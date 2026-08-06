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

    return 0;
}
