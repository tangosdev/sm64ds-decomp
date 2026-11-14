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
 * include/dExtModel_c.h for the key-function rule and the objisolate exemption
 * to it. D0 stays a C file.
 *
 * The complete DetectClsn overload set is real C++. Slot 7 uses named line
 * members; slots 6 and 8 also use the reconstructed query classes and their
 * ordinary automatic C1/D1 lifetimes. All three call dBgW_Kc::DetectClsn with
 * a qualified base call so the override cannot dispatch back into itself.
 */

#ifdef __cplusplus


struct dBgW_KcMbgSclY : dBgW_KcMbg {
    fx32 scaleY;    /* 0x1c8 */
    fx32 invScaleY; /* 0x1cc - FX_Inv(scaleY) */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~dBgW_KcMbgSclY();                     /* slots 0/1 */
    virtual void Virtual08();                             /* slot 2 */
    virtual void GetNormal(s16 triID, Vector3 &res);      /* slot 4 */
    virtual int DetectClsn(dBgCh_Gnd &ray);               /* slot 6 */
    virtual int DetectClsn(dBgCh_Lin &ray);               /* slot 7 */
    virtual int DetectClsn(dBgCh_SphCrr &sphere);         /* slot 8 */

    /* Defined in src/engine/collision/d_bg_w_kc_mbg_scl_y.cpp, which owns C1;
       the C2 variant has no ROM counterpart because nothing derives
       from SclY (notes/ctor-migration.md section 2). */
    dBgW_KcMbgSclY();

    /* --- non-virtual --- */
    void SetFile(KCL_File *file, const Matrix4x3 &mat, fx32 scale, s16 angY, CLPS_Block &clps);
    void SetScaleY(fx32 scaleY);
    s32 GetScaleY();
    /* World to collider space, then collider back to world. */
    void ToColliderSpace(Vector3 *, Vector3 *);
    void ToWorldSpace(Vector3 *, Vector3 *);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dBgW_KcMbgSclY_size_must_be_0x1d0[sizeof(dBgW_KcMbgSclY) == 0x1d0 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DBGW_KCMBGSCLY_H */
