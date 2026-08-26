#ifndef HAUNTEDCHAIR_H
#define HAUNTEDCHAIR_H

#include "dActor_c.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

/* The cartridge RTTI names this class daChair_c. HauntedChair is the readable
 * compatibility spelling already carried by every matched virtual. The ROM's
 * __si_class_type_info record gives it one direct base, dActor_c at offset
 * zero, and its 31-slot vtable has exactly the same extent as that base. Slots
 * 0, 3, 6, 9, 16 and 17 are the only overrides. symbols.txt carries
 * `_ZTV12HauntedChair` and `_ZTV9daChair_c` at one address, but only the latter
 * name is derived from cartridge RTTI. The old hand-written D0 selected that
 * alias explicitly; the real destructor now lets the compiler reference its
 * own `_ZTV12HauntedChair` spelling and objisolate rebinds it to the same ROM
 * slot array without retaining a source-level alias.
 *
 * HauntedChair_Spawn allocates 0x3a8 bytes, constructs dActor_c, then the four
 * owned objects below. Both destructor variants destroy those objects in
 * reverse order at these exact offsets, independently pinning the layout.
 */
struct HauntedChair : dActor_c {
    u8 mPad0d0[0x4];                    /* 0x0d0 */
    Model mModel;                       /* 0x0d4 */
    ShadowModel mShadowModel;           /* 0x124 */
    Matrix4x3 mShadowMat;               /* 0x14c */
    dCcAcPos_c mCylinder;               /* 0x17c */
    dBgCh_Actr mWithMeshClsn;           /* 0x1bc */

    s32 mState;                         /* 0x378 */
    u32 mTargetID;                      /* 0x37c */
    Vector3 mHomePos;                   /* 0x380 */
    Vector3 mClsnOffset;                /* 0x38c */
    s16 mStateValue0;                   /* 0x398 */
    s16 mStateValue1;                   /* 0x39a */
    s16 mStateValue2;                   /* 0x39c */
    u16 mStateTimer;                    /* 0x39e */
    u16 mActionTimer;                   /* 0x3a0 */
    s16 mTargetAngle;                   /* 0x3a2 */
    s16 *mTrackedAngle;                 /* 0x3a4 */

    virtual ~HauntedChair();            /* slots 16, 17 */

    virtual s32 InitResources();        /* slot  0 */
    virtual s32 CleanupResources();     /* slot  3 */
    virtual s32 Behavior();             /* slot  6 */
    virtual s32 Render();               /* slot  9 */

    /* The four Behavior switch targets. Their descriptive original names are
     * absent from the image, so the ROM-evidenced state indices are kept. */
    void State0();
    void State1();
    void State2();
    void State3();

    int ApproachStateValue(s16 *value, s16 *velocity, s32 target,
                           s16 threshold, s32 acceleration, s16 multiplier);
    void Break();
    void UpdateModel();
};

typedef char HauntedChair_size_must_be_0x3a8[
    sizeof(HauntedChair) == 0x3a8 ? 1 : -1];

#endif
