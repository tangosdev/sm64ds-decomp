#ifndef DAOBJSWITCH_C_H
#define DAOBJSWITCH_C_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the dExtSimpleModel_c at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#include "dBgActor_c.h"

struct daObjSwitch_c : dBgActor_c {
    struct Resources {
        SharedFilePtr *model;
        SharedFilePtr *collision;
        CLPS_Block *clps;
    };

    typedef void (daObjSwitch_c::*State)();
    struct StateEntry { State init; State exec; };

    u8  pad_31e[0x2];
    Vector3 mDrawScale;              /* 0x320 */
    s32 mDisplacementY;              /* 0x32c */
    s32 mTickSoundHandle;            /* 0x330 */
    s32 mMusicVolume;                /* 0x334 */
    u16 mTimer;                      /* 0x338 */
    u16 mTimeLimit;                  /* 0x33a */
    s32 mSwitchType;                 /* 0x33c */
    s32 mState;                      /* 0x340 */
    u32 mTargetActorID;              /* 0x344 */
    struct dActor_c *mTargetActor;   /* 0x348 */
    u8 mResourceIdx;                 /* 0x34c */
    u8 mMusicFadeDone;               /* 0x34d */
    u8 mEventBit;                    /* 0x34e */
    u8 mPressTimer;                  /* 0x34f */
    u8 mPlayerNearby;                /* 0x350 */
    u8 mStarID;                      /* 0x351 */
    u8  pad_352[0x1];
    s8 mHomeAreaId;                  /* 0x353 */

    /* Field names above describe observed use; the retail RTTI supplies only
       daObjSwitch_c. The target star field at +0x438 is not named here. */

    /* --- vtable --- */
    virtual ~daObjSwitch_c() {}

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    void OnGroundPounded(dActor_c &other);

    /* State-table members: the sinit data records at 0x0210987c.. store
       {init, exec} pointer-to-member pairs built from these addresses. */
    int  func_ov002_020b9f00();
    void func_ov002_020b9f80();
    void func_ov002_020ba01c(int mask, Fix12i b, Fix12i base, Fix12i target);
    void func_ov002_020ba0bc();
    void func_ov002_020ba0f8();
    void func_ov002_020ba1ac();
    void func_ov002_020ba2ac();
    void func_ov002_020ba2d0();
    void func_ov002_020ba3a8();
    void func_ov002_020ba3fc();
    void func_ov002_020ba4c0();
    void func_ov002_020ba4d8(int nextState);
    void func_ov002_020ba520();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char StarSwitch_size_must_be_0x354[sizeof(daObjSwitch_c) == 0x354 ? 1 : -1];
#endif

#endif /* DAOBJSWITCH_C_H */
