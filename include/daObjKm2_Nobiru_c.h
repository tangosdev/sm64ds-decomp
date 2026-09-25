#ifndef DAOBJKM2_NOBIRU_C_H
#define DAOBJKM2_NOBIRU_C_H

#include "dActor_c.h"
#include "Model.h"
#include "dBgW_KcMbgSclY.h"

/* daObjKm2_Nobiru_c: the stretching platform (registry profile KM2_NOBIRU).
 * Owns the pole-lift model and a mesh collider whose vertical scale grows and
 * shrinks.
 *
 * BASE: dActor_c, direct. _ZTI17daObjKm2_Nobiru_c at ov045 0x02112e3c reads
 * [__si_class_type_info, _ZTS17daObjKm2_Nobiru_c (0x02112e48), _ZTI8dActor_c],
 * the vtable at 0x02112e80 carries that _ZTI at its address point minus 4,
 * and it has exactly the same 31 slots as that base. The tree previously
 * called the class ExtendingPlatform (coined).
 *
 * SIZE 0x328: the reconstructed factory daObjKm2_Nobiru_c_classInit
 * (historical alias ExtendingPlatform_Spawn) allocates 0x328 and installs
 * this class's cartridge vtable. Layout evidence for the members:
 * notes/platform-provenance.md. */
struct daObjKm2_Nobiru_c : dActor_c {
    u8 mPad0d0[0x4];                    /* 0x0d0 */
    u8 mGrowing;                        /* 0x0d4 */
    u8 mPad0d5[0x3];                    /* 0x0d5 */
    Model mModel;                       /* 0x0d8 */
    Matrix4x3 mColliderTransform;       /* 0x128 */
    dBgW_KcMbgSclY mCollider;           /* 0x158 */

    /* Out of line in daObjKm2_Nobiru_c.cpp: the key function, so that TU
     * emits _ZTV/_ZTI/_ZTS17daObjKm2_Nobiru_c. */
    virtual ~daObjKm2_Nobiru_c();       /* slots 16, 17 */

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

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjKm2_Nobiru_c_size_must_be_0x328[
    sizeof(daObjKm2_Nobiru_c) == 0x328 ? 1 : -1];
#endif

#endif
