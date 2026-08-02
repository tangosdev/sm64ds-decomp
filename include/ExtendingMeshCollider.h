#ifndef EXTENDINGMESHCOLLIDER_H
#define EXTENDINGMESHCOLLIDER_H

#include "types.h"
#include "MovingMeshCollider.h"

/* A moving mesh collider that stretches vertically (the Snowman's body,
 * lift chains), vtable _ZTV21ExtendingMeshCollider at 0x02099490. Derives
 * from MovingMeshCollider; its two fields sit at 0x1c8, which is what
 * pins the parent's size. GetTriangleOrigin stays on the parent's
 * implementation; GetNormal re-normalizes after scaling x and z by
 * scaleY.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- see
 * include/ModelBase.h. The structors stay C files.
 *
 * SetScaleY's mangled name carries Fix12<int> (wall 6az), and SetFile and
 * the DetectClsn triple are launder-heavy, so those definitions stay
 * extern-C free functions; the declarations here are the real ones.
 */

#ifdef __cplusplus

#include "math/Fix12.h"

struct ExtendingMeshCollider : MovingMeshCollider {
    Fix12i scaleY;             /* 0x1c8 */
    Fix12i invScaleY;          /* 0x1cc - func_02053200(scaleY) */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~ExtendingMeshCollider();                     /* slots 0/1 */
    virtual void Virtual08();                             /* slot 2 */
    virtual void GetNormal(s16 triID, Vector3 &res);      /* slot 4 */
    virtual int DetectClsn(RaycastGround &ray);           /* slot 6 - free def */
    virtual int DetectClsn(RaycastLine &ray);             /* slot 7 - free def */
    virtual int DetectClsn(SphereClsn &sphere);           /* slot 8 - free def */

    /* --- non-virtual --- */
    void SetFile(KCL_File *file, const Matrix4x3 &mat, Fix12<int> scale,
                 s16 angY, CLPS_Block &clps);             /* free def, wall 6az */
    void SetScaleY(Fix12<int> scaleY);                    /* free def, wall 6az */
};

typedef char ExtendingMeshCollider_size_must_be_0x1d0[sizeof(ExtendingMeshCollider) == 0x1d0 ? 1 : -1];

#endif /* __cplusplus */

#endif /* EXTENDINGMESHCOLLIDER_H */
