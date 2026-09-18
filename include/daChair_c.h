#ifndef DACHAIR_C_H
#define DACHAIR_C_H

#include "dActor_c.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

/* Big Boo's Haunt haunted chair. ROM RTTI ov020:0x021149d8 names the class
 * daChair_c; the debug table names CHAIR (326). One direct base, dActor_c at
 * offset zero. Slots 0, 3, 6, 9, 16 and 17 are the only overrides.
 *
 * daChair_c_classInit allocates 0x3a8 bytes, constructs dActor_c, then the
 * four owned objects below. Both destructor variants destroy those objects
 * in reverse order at these exact offsets.
 *
 * classInit is reconstructed (RTTI daChair_c, CHAIR registry). Historical
 * alias HauntedChair_Spawn. Retail does not store that spelling.
 */
struct daChair_c : dActor_c {
    u8 mPad0d0[0x4];                    /* 0x0d0 */
    Model mModel;                       /* 0x0d4 */
    ShadowModel mShadowModel;           /* 0x124 */
    Matrix4x3 mShadowMat;               /* 0x14c */
    dCcAcPos_c mCylinder;               /* 0x17c */
    dBgCh_Actr mWithMeshClsn;           /* 0x1bc */

    s32 mState;                         /* 0x378 */
    u32 mTargetID;                      /* 0x37c -- uniqueID of the nearby PIANO */
    Vector3 mHomePos;                   /* 0x380 */
    Vector3 mClsnOffset;                /* 0x38c */
    s16 mStateValue0;                   /* 0x398 */
    s16 mStateValue1;                   /* 0x39a */
    s16 mStateValue2;                   /* 0x39c */
    u16 mStateTimer;                    /* 0x39e */
    u16 mActionTimer;                   /* 0x3a0 */
    s16 mTargetAngle;                   /* 0x3a2 */
    s16 *mTrackedAngle;                 /* 0x3a4 */

    /* INLINE, AND DECLARED FIRST. The cartridge puts D1 at 0x02112938 below
       D0 at 0x02112980 and carries no D2, which is what mwccarm 2004/b56
       emits for an in-class destructor. InitResources is then the first
       out-of-line virtual -- the key function -- so this TU emits the
       vtable. */
    virtual ~daChair_c() {}

    virtual s32 InitResources();        /* slot  0 -- key function */
    virtual s32 CleanupResources();     /* slot  3 */
    virtual s32 Behavior();             /* slot  6 */
    virtual s32 Render();               /* slot  9 */

    /* The four Behavior switch targets. Their descriptive original names are
     * absent from the image, so the ROM-evidenced state indices are kept. */
    void State0();
    void State1();
    void State2();
    void State3();

    /* Coined. The mangled suffix EPsS0_isis claims two s16* then int, s16,
     * int, s16. A pointer and a reference generate identical ARM for this
     * body, as do short and int in some of these slots, so the bytes cannot
     * prove the exact original types. */
    int ApproachStateValue(s16 *value, s16 *velocity, s32 target,
                           s16 threshold, s32 acceleration, s16 multiplier);
    void Break();
    void UpdateModel();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daChair_c_size_must_be_0x3a8[
    sizeof(daChair_c) == 0x3a8 ? 1 : -1];
#endif

#endif
