#ifndef DAOBJC1_TRAP_C_H
#define DAOBJC1_TRAP_C_H

#include "types.h"

/* The ROM's RTTI identifies daObjC1_Trap_c as a direct dBgActor_c subclass.
 * Its own storage starts at 0x320 with the rendered trap-door model; the
 * inherited dBgActor_c model and moving collider remain at 0x0d4 and 0x124.
 * daObjC1_Trap_c_classInit's 0x3b0 allocation fixes the complete size.
 *
 * SM64DS RTTI names the implementation daObjC1_Trap_c. The reconstructed factory
 * daObjC1_Trap_c_classInit (historical alias Trap_Spawn) installs this class's
 * cartridge vtable for the C1_TRAP registry profile.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "Model.h"

struct daObjC1_Trap_c : dBgActor_c {
    Model mDoorModel;                 /* 0x320 */
    Matrix4x3 mDoorMat;               /* 0x370 */
    s32 mState;                         /* 0x3a0 */
    s32 mPlayerDist;                    /* 0x3a4 */
    s16 mOpenSpeed;                     /* 0x3a8 */
    u8  mTrapActive;                    /* 0x3aa -- set when a player enters the trap */
    u8  mIsSpawner;                     /* 0x3ab -- invisible paired-door controller */
    s32 mSpawnerID;                     /* 0x3ac -- unique ID of the controller */

    /* --- vtable --- */
    /* Inline so InitResources can own the vtable and emit retail's D1 then D0
       pair without an extra D2. */
    virtual ~daObjC1_Trap_c() {}

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();

    void State0();
    void State1();
    void State2();
    void State3();
    void State4();
    daObjC1_Trap_c *GetSpawner();
    void UpdateCollisionTransform();
    void UpdateModelTransform();
    void OnCollision(dActor_c &other);
};

typedef char Trap_size_must_be_0x3b0[sizeof(daObjC1_Trap_c) == 0x3b0 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJC1_TRAP_C_H */
