#ifndef DAOBJRCCARPET_C_H
#define DAOBJRCCARPET_C_H

#include "types.h"

#ifdef __cplusplus

#include "PathLift.h"
#include "dExtAnmModel_c.h"

/* Rainbow Ride flying carpet (ov036 FLYING_CARPET 130). RTTI
 * _ZTI15daObjRcCarpet_c at ov036:0x02113f4c names this class;
 * the debug table at 0x0208fef4 names profile RC_CARPET. Direct
 * base is dPathLiftActor_c (_ZTI at ov002:0x0210af0c).
 *
 * SIZE 0x4c0, the literal daObjRcCarpet_c_classInit passes to
 * fBase_c::operator new. dPathLiftActor_c ends at 0x450; dExtAnmModel_c
 * at 0x450 is this class's only constructed member (D1 calls
 * _ZN14dExtAnmModel_cD1Ev once, then inlines the base teardown).
 *
 * mRideDipY (0x4bc) + 4 = 0x4c0. pad_4b4 is unwitnessed in this TU.
 */

struct daObjRcCarpet_c : dPathLiftActor_c {
    dExtAnmModel_c mAnmModel; /* 0x450 */
    u8 pad_4b4[0x8];
    /* How far the carpet sags under a rider: Behavior eases it toward
       -0x14000 (20 units down) at 0x5000 a frame while mAfterClsnRan is
       set and back toward 0 otherwise; UpdateModelTransform and
       UpdateClsnTransform add it to mPosY for the model and collision
       matrices. Was unk_4bc. */
    s32 mRideDipY; /* 0x4bc */

    /* Inline plus vtable instantiation is load-bearing: mwcc emits
       retail's D1 then D0 pair, with no homeless D2. */
    virtual ~daObjRcCarpet_c() {}

    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();

    /* Rotate the model by mAngleX/Y/Z and place it at mPos (dipped by
       mRideDipY), in model units. */
    void UpdateModelTransform();
    /* Copy the model matrix into mClsnMat at world scale and move the
       collision mesh with it. */
    void UpdateClsnTransform();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjRcCarpet_c_size_must_be_0x4c0[sizeof(daObjRcCarpet_c) == 0x4c0 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJRCCARPET_C_H */
