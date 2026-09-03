#ifndef DAOBJKM2_NOBIRU_C_H
#define DAOBJKM2_NOBIRU_C_H

#include "dActor_c.h"
#include "Model.h"
#include "dBgW_KcMbgSclY.h"

/* The cartridge calls this class daObjKm2_Nobiru_c in RTTI. daObjKm2_Nobiru_c
 * is the readable compatibility name carried by the project's historical
 * factory alias and every known virtual. The reconstructed factory
 * daObjKm2_Nobiru_c_classInit (historical alias ExtendingPlatform_Spawn)
 * allocates 0x328 and installs this class's cartridge vtable. The
 * __si_class_type_info record points directly at dActor_c, and the vtable has
 * exactly the same 31 slots as that base.
 *
 * The readable name intentionally does not claim that the compiler-generated
 * RTTI spelling is cartridge-authentic. Per-function object isolation keeps
 * the ROM's daObjKm2_Nobiru_c RTTI and vtable records authoritative. */
struct daObjKm2_Nobiru_c : dActor_c {
    u8 mPad0d0[0x4];                    /* 0x0d0 */
    u8 mGrowing;                        /* 0x0d4 */
    u8 mPad0d5[0x3];                    /* 0x0d5 */
    Model mModel;                       /* 0x0d8 */
    Matrix4x3 mColliderTransform;       /* 0x128 */
    dBgW_KcMbgSclY mCollider;           /* 0x158 */

    /* Inline, and declared FIRST. This TU defines every virtual the class has,
     * so it emits the vtable and RTTI whatever is declared first -- moving the
     * declaration last buys nothing. Out of line, mwccarm emits the D2/D1/D0
     * triple in D0-before-D1 order, but retail puts D1 (0x02111840) ABOVE D0
     * (0x02111878), and objisolate then refuses the whole TU for emitting out
     * of ROM address order. The inline body emits only the retail D1/D0 pair,
     * in retail order, and emits no D2. */
    virtual ~daObjKm2_Nobiru_c() {}     /* slots 16 (D1), 17 (D0) */

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
    sizeof(daObjKm2_Nobiru_c) == 0x328 ? 1 : -1];

#endif
