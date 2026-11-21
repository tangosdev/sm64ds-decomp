/* The cartridge's RTTI names this Tick Tock Clock moving-bar class
 * daObjCtMecha05_c. Profile CT_MECHA05 (113) is TTC_MOVING_BAR in
 * overlay_actors.md. ov065 is TTC.
 *
 * The factory constructs dBgActor_c, then dExtShadowModel_c at 0x33c, and
 * installs this class vptr. The allocation literal is 0x394. */
#ifndef DAOBJCTMECHA05_C_H
#define DAOBJCTMECHA05_C_H
#include "types.h"

#ifdef __cplusplus
#include "dBgActor_c.h"
#include "dExtShadowModel_c.h"

struct daObjCtMecha05_c : dBgActor_c {
    Vector3 mHomePos; /* 0x320 -- snapshot of mPos at InitResources */
    s32 mTravel;     /* 0x32c -- += mHorzSpeed each tick; compared to 0xfa000 */
    s32 mPrevTravel; /* 0x330 -- previous tick's mTravel */
    u16 mStateTimer; /* 0x334 -- DecIfAbove0_Short countdown */
    u8 mState;       /* 0x336 -- Behavior switch key, 0..3 */
    u8 pad_337[0x1];
    s32 mGroundY;                   /* 0x338 -- dBgCh_Gnd hit height */
    dExtShadowModel_c mShadowModel; /* 0x33c */
    Matrix4x3 mShadowMatrix;        /* 0x364 */

    int CleanupResources();
    int InitResources();
    int Behavior();
    int Render();

    /* Behavior's shadow drop, ahead of the bar along its yaw. */
    void UpdateShadow();
    /* Back to state 0 at the home position, with the clock setting's wait. */
    void ResetToHome();

    /* Declared last and inline so class instantiation emits the retail D1/D0
     * pair in cartridge order without a separate leaf D2 body. */
    virtual ~daObjCtMecha05_c() {}
};

typedef char daObjCtMecha05_c_size_must_be_0x394[sizeof(daObjCtMecha05_c) == 0x394 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJCTMECHA05_C_H */
