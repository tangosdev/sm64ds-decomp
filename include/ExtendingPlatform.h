#ifndef EXTENDINGPLATFORM_H
#define EXTENDINGPLATFORM_H

#include "dActor_c.h"
#include "Model.h"
#include "dBgW_KcMbgSclY.h"

/* The cartridge calls this class daObjKm2_Nobiru_c in RTTI. ExtendingPlatform
 * is the readable compatibility name already carried by its factory and every
 * known virtual. The __si_class_type_info record points directly at dActor_c,
 * and the vtable has exactly the same 31 slots as that base.
 *
 * The readable name intentionally does not claim that the compiler-generated
 * RTTI spelling is cartridge-authentic. Per-function object isolation keeps
 * the ROM's daObjKm2_Nobiru_c RTTI and vtable records authoritative. */
struct ExtendingPlatform : dActor_c {
    u8 mPad0d0[0x4];                    /* 0x0d0 */
    u8 mGrowing;                        /* 0x0d4 */
    u8 mPad0d5[0x3];                    /* 0x0d5 */
    Model mModel;                       /* 0x0d8 */
    Matrix4x3 mColliderTransform;       /* 0x128 */
    dBgW_KcMbgSclY mCollider;           /* 0x158 */

    virtual ~ExtendingPlatform();       /* slots 16, 17 */

    virtual int InitResources();        /* slot  0 */
    virtual int CleanupResources();     /* slot  3 */
    virtual int Behavior();             /* slot  6 */
    virtual int Render();               /* slot  9 */
    virtual void OnPendingDestroy();    /* slot 12 */

    /* Both helpers are called only by InitResources. Their writes stay wholly
     * inside this object's two owned transform records. */
    void UpdateColliderTransform();
    void UpdateModelTransform();
};

typedef char ExtendingPlatform_size_must_be_0x328[
    sizeof(ExtendingPlatform) == 0x328 ? 1 : -1];

#endif
