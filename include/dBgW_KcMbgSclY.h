#ifndef DBGW_KCMBGSCLY_H
#define DBGW_KCMBGSCLY_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* A moving mesh collider that stretches vertically (the Snowman's body,
 * lift chains), vtable _ZTV14dBgW_KcMbgSclY at 0x02099490. Derives
 * from dBgW_KcMbg; its two fields sit at 0x1c8, which is what
 * pins the parent's size. GetTriangleOrigin stays on the parent's
 * implementation; GetNormal re-normalizes after scaling x and z by
 * scaleY.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND D1 IS A REAL METHOD -- see
 * include/ModelBase.h for the key-function rule and the objisolate exemption
 * to it. D0 stays a C file.
 *
 * SetScaleY's mangled name carries Fix12<int>, and SetFile's does too (wall
 * 6az), so those two definitions stay extern-C free functions; the declarations
 * here are the real ones.
 *
 * The complete DetectClsn overload set is real C++. Slot 7 uses named line
 * members; slots 6 and 8 also use the reconstructed query classes and their
 * ordinary automatic C1/D1 lifetimes. All three call dBgW_Kc::DetectClsn with
 * a qualified base call so the override cannot dispatch back into itself.
 */

#ifdef __cplusplus

#include "math/Fix12.h"

struct dBgW_KcMbgSclY : dBgW_KcMbg {
    Fix12i scaleY;             /* 0x1c8 */
    Fix12i invScaleY;          /* 0x1cc - func_02053200(scaleY) */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~dBgW_KcMbgSclY();                     /* slots 0/1 */
    virtual void Virtual08();                             /* slot 2 */
    virtual void GetNormal(s16 triID, Vector3 &res);      /* slot 4 */
    virtual int DetectClsn(dBgCh_Gnd &ray);               /* slot 6 */
    virtual int DetectClsn(dBgCh_Lin &ray);               /* slot 7 */
    virtual int DetectClsn(dBgCh_SphCrr &sphere);         /* slot 8 */

    /* DECLARED, never defined as a method here -- src/_ZN14dBgW_KcMbgSclYC1Ev.cpp
       owns C1; the C2 variant has no ROM counterpart because nothing derives
       from SclY (notes/ctor-migration.md section 2). */
    dBgW_KcMbgSclY();

    /* --- non-virtual --- */
    void SetFile(KCL_File *file, const Matrix4x3 &mat, Fix12<int> scale,
                 s16 angY, CLPS_Block &clps);             /* free def, wall 6az */
    void SetScaleY(Fix12<int> scaleY);                    /* free def, wall 6az */
};

typedef char dBgW_KcMbgSclY_size_must_be_0x1d0[sizeof(dBgW_KcMbgSclY) == 0x1d0 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DBGW_KCMBGSCLY_H */
