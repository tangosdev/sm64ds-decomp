#ifndef DBGW_KCMBG_H
#define DBGW_KCMBG_H

#include "types.h"
#include "dBgW_Kc.h"
#include "math/Matrix.h"

/* The moving-mesh collider, vtable _ZTV10dBgW_KcMbg at 0x02099434.
 * Derives from dBgW_Kc (SetFile chains to dBgW_Kc::SetFile and
 * the fields start at exactly 0x50); dBgW_KcMbgSclY derives from
 * this in turn. Overrides every slot except BeforeClsn (slot 9) and
 * GetSurfaceInfo (slot 3), both of which it inherits unchanged.
 *
 * THE DESTRUCTOR IS DECLARED FIRST -- see include/dExtModel_c.h for the
 * key-function rule. src/engine/collision/d_bg_w_kc_mbg.cpp defines it once
 * and the TU keeps all five structor variants.
 *
 * SetFile takes the scale as fx32 (a scalar; it mangles as i).
 * SetFile and Transform copy matrices through a flat 12-word
 * view (the dExtSimpleModel_c::Virtual10 lesson).
 */

#include "math/Fix12.h"

struct dBgW_KcMbg : dBgW_Kc {
    fx32 scale;          /* 0x50 */
    Matrix4x3 mat;       /* 0x54 */
    Matrix4x3 invRotMat; /* 0x84 - rotation-only inverse */
    Matrix4x3 scaledMat; /* 0xb4 */
    Matrix4x3 invMat;    /* 0xe4 - previous frame's inverse in Transform */
    s16 angY;            /* 0x114 */
    s16 angVelY;         /* 0x116 */
    Vector3 pos;         /* 0x118 */
    Vector3 velocity;    /* 0x124 */
    /* Set by SetFilterByMoveDir. While set, Transform points dBgW_Kc's
       depth policy (shallowOnly/depthByDir/depthDir/depthDirMinDot) along the
       collider's horizontal velocity every frame. SetFile clears it. */
    bool filterByMoveDir; /* 0x130 */
    u8 pad_131[3];
    Matrix4x3 newScaledMat;     /* 0x134 */
    s32 invScale;               /* 0x164 - FX_Inv(scale) */
    Matrix4x3 invScaledMat;     /* 0x168 */
    Matrix4x3 prevInvScaledMat; /* 0x198 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~dBgW_KcMbg();                        /* slots 0/1 */
    virtual void Virtual08();                             /* slot 2 */
    /* slot 3 GetSurfaceInfo is NOT overridden: this table's word is
       0x01ffd920, the same one dBgW_Kc holds, so it is inherited. */
    virtual void GetNormal(s16 triID, Vector3 &res);      /* slot 4 */
    virtual void GetTriangleOrigin(s16 triID, Vector3 &res); /* slot 5 */
    virtual int DetectClsn(dBgCh_Gnd &ray);                  /* slot 6 */
    virtual int DetectClsn(dBgCh_Lin &ray);                  /* slot 7 */
    virtual int DetectClsn(dBgCh_SphCrr &sphere);            /* slot 8 */
    virtual int TransformPos(const Vector3 &pos, Vector3 &res); /* slot 10 */
    virtual s16 GetAngularVelY();                         /* slot 11 */
    virtual void GetVelocity(Vector3 &res);               /* slot 12 */

    /* DECLARED, never defined as a method here -- src/engine/collision/d_bg_w_kc_mbg.cpp
       owns C1 and the base-subobject variant
       (notes/ctor-migration.md section 2). */
    dBgW_KcMbg();

    /* --- non-virtual --- */
    void SetFile(KCL_File *file, const Matrix4x3 &mat, fx32 scale, s16 angY, CLPS_Block &clps);
    void Transform(const Matrix4x3 &mat, s16 angY);

    /* Current-frame coordinate conversions. Position transforms include the
       uniform scale; direction conversion uses rotation only. */
    void WorldToLocalPosition(const Vector3 &, Vector3 &);
    void LocalToWorldPosition(const Vector3 &, Vector3 &);
    void WorldToLocalDirection(const Vector3 &, Vector3 &);
    /* Inferred name: the mirror of WorldToLocalDirection, rotating by `mat`
       with its translation dropped. Both GetNormal overrides use it. */
    void LocalToWorldDirection(const Vector3 &, Vector3 &);

    /* Sets filterByMoveDir (0x130); the actor calls it once the mesh moves. */
    void SetFilterByMoveDir(); /* 0x020398fc */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dBgW_KcMbg_size_must_be_0x1c8[sizeof(dBgW_KcMbg) == 0x1c8 ? 1 : -1];
#endif

#endif /* DBGW_KCMBG_H */
