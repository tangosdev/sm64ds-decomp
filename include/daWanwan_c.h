#ifndef DAWANWAN_C_H
#define DAWANWAN_C_H

#include "types.h"

/* Bob-omb Battlefield Chain Chomp. Seven links, so seven of each subobject.
 * dEnemyBase_c ends at 0x110; dCcAcPos_c, ModelAnim and ShadowModel close on
 * 0x150, 0x1b4 and 0x1dc; then Model[7], ShadowModel[7] and two Vector3[7].
 */

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"

struct daWanwan_c : dEnemyBase_c {
    dCcAcPos_c mdCcAcPos_c;  /* 0x110 */
    ModelAnim mModelAnim;                                  /* 0x150 */
    ShadowModel mShadowModel;                              /* 0x1b4 */
    Model mLinkModels[7];                                  /* 0x1dc */
    ShadowModel mLinkShadows[7];                           /* 0x40c */
    /* InitResources copies the chomp position into every link. 0211250c then
       walks [1..6]; [0] is the head, written from the actor position each frame. */
    Vector3 mLinkPos[7];                                   /* 0x524 */
    /* 0211250c adds the stored value into the link step, then replaces it with
       the scaled movement. [0] is constructed and that loop never touches it. */
    Vector3 mLinkDelta[7];                                 /* 0x578 */
    u8  pad_5cc[4];
    /* 0211250c, one floor per link [1..6]. Raised to mSpawnPosY + 0x28000. */
    s32 mLinkFloorY[6];                                    /* 0x5d0 */
    u8  pad_5e8[4];
    /* InitResources copies the position here, then adds 0xc8000 to the live
       position. Behavior clamps mPosY up to mSpawnPosY + 0xc8000. */
    s32 mSpawnPosX;         /* 0x5ec */
    s32 mSpawnPosY;         /* 0x5f0 */
    s32 mSpawnPosZ;         /* 0x5f4 */
    s32 mChainExtension;    /* 0x5f8 -- leash slack; 02111fe0 uses it * 7 */
    u16 mActionTimer;       /* 0x5fc -- DecIfAbove0_Short */
    u16 mSecretSound;       /* 0x5fe -- Sound::PlaySecretSound counter */
    s16 unk_600;            /* 0x600 -- idle lunge and release enter store 0; unread */
    s16 mTargetAngY;        /* 0x602 -- idle faces this; HorzAngleToCPlayer writes it */
    u8  mReleaseStep;       /* 0x604 -- release cutscene step, 021115ec */
    u8  mChainBroken;       /* 0x605 -- 02111f54 writes 1; Behavior skips the leash */
    u8  pad_606[2];
    s32 mStumpUniqueID;     /* 0x608 -- STUMP (0x1b) spawned by InitResources */
    s32 mFenceUniqueID;     /* 0x60c -- CHAIN_CHOMP_FENCE (0x29), found by Behavior */
    s32 mState;             /* 0x610 -- index into data_ov014_0211476c */
    u8  pad_614[4];
    s32 mReleaseSpeed;      /* 0x618 -- release step 6 approach speed */
    u8  mIsOnGround;        /* 0x61c -- set when mPosY was clamped to the rest height */
    u8  mWasOnGround;       /* 0x61d -- previous frame; landing fires on the rising edge */

    /* Defined INLINE on purpose. Out of line, mwccarm emits D2, D0, D1; the ROM
       has D1 at 0x02111308 then D0 at 0x021113bc and no D2 anywhere in ov014.
       Inline, mwccarm emits D1 then D0 and no D2 -- the cartridge's own order.
       The body is empty either way: the 0xb4 bytes of D1 are the compiler's own
       teardown of the four arrays and three member subobjects. */
    virtual ~daWanwan_c() {}   /* slots 16 (D1), 17 (D0) */


    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    void func_ov014_02111ebc(int i);
    void func_ov014_02111f08();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daWanwan_c_size_must_be_0x620[sizeof(daWanwan_c) == 0x620 ? 1 : -1];
#endif

#else

/* The same object for a C translation unit, flat. */
struct daWanwan_c {
    u8  pad_000[0x60];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x1c];
    /* 0x080..0x110 is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;                 /* 0x08e */
    s16 mAngleZ;                 /* 0x090 */
    s16 mPrevAngleX;             /* 0x092 */
    s16 mPrevAngleY;             /* 0x094 */
    s16 mPrevAngleZ;             /* 0x096 */
    s32 mHorzSpeed;              /* 0x098 */
    s32 mVertAccel;              /* 0x09c */
    s32 mTerminalVelocity;       /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;              /* 0x0a8 */
    u8  pad_0ac[0x4];
    u32 mFlags;                  /* 0x0b0 */
    s32 mClipOffsetY;            /* 0x0b4 */
    s32 mClipRadius;             /* 0x0b8 */
    s32 mClipDistance;           /* 0x0bc */
    s32 mFarDistance;            /* 0x0c0 */
    u8  mClipResult;             /* 0x0c4 */
    u8  pad_0c5[0x7];
    s8  mAreaId;                 /* 0x0cc */
    u8  pad_0cd[0x1];
    s16 mDeathTableID;           /* 0x0ce */
    u8  pad_0d0[0x40];
    u8  mdCcAcPos_c;            /* 0x110 */
    u8  pad_111[0x3f];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x150 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    u8  mModelAnim[0x64];            /* 0x150 */
    u8  mShadowModel;            /* 0x1b4 */
    u8  pad_1b5[0x36f];
    s32 mLinkPos[21];       /* 0x524 -- Vector3 mLinkPos[7] */
    s32 mLinkDelta[21];     /* 0x578 */
    u8  pad_5cc[4];
    s32 mLinkFloorY[6];     /* 0x5d0 */
    u8  pad_5e8[4];
    s32 mSpawnPosX;         /* 0x5ec */
    s32 mSpawnPosY;         /* 0x5f0 */
    s32 mSpawnPosZ;         /* 0x5f4 */
    s32 mChainExtension;    /* 0x5f8 */
    u16 mActionTimer;       /* 0x5fc */
    u16 mSecretSound;       /* 0x5fe */
    s16 unk_600;            /* 0x600 */
    s16 mTargetAngY;        /* 0x602 */
    u8  mReleaseStep;       /* 0x604 */
    u8  mChainBroken;       /* 0x605 */
    u8  pad_606[2];
    s32 mStumpUniqueID;     /* 0x608 */
    s32 mFenceUniqueID;     /* 0x60c */
    s32 mState;             /* 0x610 */
    u8  pad_614[4];
    s32 mReleaseSpeed;      /* 0x618 */
    u8  mIsOnGround;        /* 0x61c */
    u8  mWasOnGround;       /* 0x61d */
};

#endif /* __cplusplus */

#endif /* DAWANWAN_C_H */
