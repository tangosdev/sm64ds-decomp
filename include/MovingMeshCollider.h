#ifndef MOVINGMESHCOLLIDER_H
#define MOVINGMESHCOLLIDER_H

#include "types.h"
#include "MeshCollider.h"
#include "math/Matrix.h"

/* The moving-mesh collider, vtable _ZTV18MovingMeshCollider at 0x02099434.
 * Derives from MeshCollider (SetFile chains to MeshCollider::SetFile and
 * the fields start at exactly 0x50); ExtendingMeshCollider derives from
 * this in turn. Overrides every slot except BeforeClsn (slot 9) and
 * GetSurfaceInfo (slot 3), both of which it inherits unchanged.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- see
 * include/ModelBase.h. The structors stay C files.
 *
 * SetFile and Transform keep extern-C free definitions for now: their
 * bodies are dominated by flat 12-word Matrix4x3 copies (the
 * Model::Virtual10 lesson) and belong to a later pass with a shared
 * flat-copy helper. Their declarations here are the real signatures
 * (SetFile's carries Fix12<int>, wall 6az, so its definition could not
 * be a method yet anyway).
 */

#ifdef __cplusplus

#include "math/Fix12.h"

struct MovingMeshCollider : MeshCollider {
    Fix12i scale;              /* 0x50 */
    Matrix4x3 mat;             /* 0x54 */
    Matrix4x3 invRotMat;       /* 0x84 - rotation-only inverse */
    Matrix4x3 scaledMat;       /* 0xb4 */
    Matrix4x3 invMat;          /* 0xe4 - previous frame's inverse in Transform */
    s16 angY;                  /* 0x114 */
    s16 angVelY;               /* 0x116 */
    Vector3 pos;               /* 0x118 */
    Vector3 velocity;          /* 0x124 */
    u8 unk_130;                /* 0x130 */
    u8 pad_131[3];
    Matrix4x3 newScaledMat;    /* 0x134 */
    s32 unk_164;               /* 0x164 - func_02053200(scale) */
    Matrix4x3 invScaledMat;    /* 0x168 */
    Matrix4x3 prevInvScaledMat;/* 0x198 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~MovingMeshCollider();                        /* slots 0/1 */
    virtual void Virtual08();                             /* slot 2 */
    /* slot 3 GetSurfaceInfo is NOT overridden: this table's word is
       0x01ffd920, the same one MeshCollider holds, so it is inherited. */
    virtual void GetNormal(s16 triID, Vector3 &res);      /* slot 4 */
    virtual void GetTriangleOrigin(s16 triID, Vector3 &res); /* slot 5 */
    virtual int DetectClsn(RaycastGround &ray);           /* slot 6 - free def */
    virtual int DetectClsn(RaycastLine &ray);             /* slot 7 - free def */
    virtual int DetectClsn(SphereClsn &sphere);           /* slot 8 - free def */
    virtual int TransformPos(const Vector3 &pos, Vector3 &res); /* slot 10 */
    virtual s16 GetAngularVelY();                         /* slot 11 */
    virtual void GetVelocity(Vector3 &res);               /* slot 12 */

    /* --- non-virtual --- */
    void SetFile(KCL_File *file, const Matrix4x3 &mat, Fix12<int> scale,
                 s16 angY, CLPS_Block &clps);             /* free def, wall 6az */
    void Transform(const Matrix4x3 &mat, s16 angY);       /* free def for now */
};

typedef char MovingMeshCollider_size_must_be_0x1c8[sizeof(MovingMeshCollider) == 0x1c8 ? 1 : -1];

#endif /* __cplusplus */

#endif /* MOVINGMESHCOLLIDER_H */
