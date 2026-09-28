#ifndef DAHOLHEI_C_H
#define DAHOLHEI_C_H

#include "types.h"
#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

/* Chuckya (HOLHEI 190) -- ov062/daHolhei_c.
 *
 * RTTI ov062:0x0211d9ac is the string 10daHolhei_c. The factory
 * daHolhei_c_classInit allocates 0x438 bytes. Member names below are from
 * this TU's own reads and writes. g_profile_HOLHEI stays outside the TU.
 */

struct daHolhei_c : dEnemyBase_c {
    dCcAc_c     mdCc_c;        /* 0x110 */
    dBgCh_Actr  mMeshClsn;     /* 0x144 */
    ModelAnim   mModel;        /* 0x300 */
    /* Behavior calls the PMF at *mState + 8. InitResources enters
       data_ov062_0211dee0 through daHolhei_c_ChangeState. */
    void       *mState;        /* 0x364 */
    ShadowModel mShadowModel;  /* 0x368 */
    /* func_ov062_02116dbc / 02116d28 copy identity here, write pos>>3 into
       the translation, and pass it to DropShadowRadHeight. */
    Matrix4x3   mShadowMtx;    /* 0x390 */
    s32         mHomePosX;     /* 0x3c0 -- InitResources copies mPos here */
    s32         mHomePosY;     /* 0x3c4 */
    s32         mHomePosZ;     /* 0x3c8 */
    /* func_ov062_021165e8 stores the closest player's pos when the charge starts. */
    s32         mChasePosX;    /* 0x3cc */
    s32         mChasePosY;    /* 0x3d0 */
    s32         mChasePosZ;    /* 0x3d4 */
    /* Not dActor_c::mPrevPos at 0x068. Behavior rewinds mPos from these on a
       cliff or level-fence trip, then republishes them. */
    s32         mPrevPosX;     /* 0x3d8 */
    s32         mPrevPosY;     /* 0x3dc */
    s32         mPrevPosZ;     /* 0x3e0 */
    /* 1 when IsGoingOffCliff or the level fence trips, else 0. */
    u8          mEdgeStop;     /* 0x3e4 */
    /* func_ov062_021164e8 sets this once it has started the grab animation. */
    u8          mGrabAnim;     /* 0x3e5 */
    /* Behavior counts this down. func_ov062_02116bf8 will not turn while it is set. */
    u16         mTurnWait;     /* 0x3e6 */
    /* Behavior counts this down. func_ov062_02116a08 will not re-engage the
       player while it is set; 021167c0 and 02116894 reload it with 0x1e. */
    u16         mChaseCooldown;/* 0x3e8 */
    u8          pad_3ea[2];    /* 0x3ea */
    /* Sound::PlayLong handle. func_ov062_021165e8 and 02116a08 pass id 0x18a. */
    u32         mMoveSound;    /* 0x3ec */
    /* 0 aim at the player, 1 run, 2 past the point saved in mChasePos. */
    s32         mChargeStep;   /* 0x3f0 */
    s16         mTargetAngY;   /* 0x3f4 */
    u8          pad_3f6[2];    /* 0x3f6 */
    /* Actor being held, or the player holding this. +0xc8 on that actor is
       still an unnamed dActor_c gap; Behavior and Render read it raw. */
    dActor_c   *mHeld;         /* 0x3f8 */
    /* func_ov062_02116e80 builds this from bone 3 and stores its address at
       this+0xc8, the same unnamed gap. */
    Matrix4x3   mHoldMtx;      /* 0x3fc */
    /* Behavior zeroes these. func_ov062_02116edc homes them toward the two
       Vector3s at data_ov062_0211df10 and hands them to UpdateCarry. */
    s32         mCarryOffsX;   /* 0x42c */
    s32         mCarryOffsY;   /* 0x430 */
    s32         mCarryOffsZ;   /* 0x434 */

    /* INLINE IS LOAD-BEARING. Out-of-line, mwccarm emits D0 before D1 plus
       a homeless D2; the cartridge has D1 at 0x02115ee0 then D0 at 0x02115f28
       and no D2. */
    virtual ~daHolhei_c() {}

    virtual s32 OnAimedAtWithEgg(); /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    int func_ov062_021164e8();
    int func_ov062_021165e8();
    int func_ov062_021167c0();
    int func_ov062_02116894();
    int func_ov062_02116980();
    int func_ov062_02116bf8();
    int func_ov062_02116c78();
    void func_ov062_02116d28();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daHolhei_c_size_must_be_0x438[sizeof(daHolhei_c) == 0x438 ? 1 : -1];
#endif

#endif /* DAHOLHEI_C_H */
