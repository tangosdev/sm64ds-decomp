#ifndef UPDOWNLIFTBBH_H
#define UPDOWNLIFTBBH_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

struct UpDownLiftBbh {
    u8  pad_000[0xc];
    u16 actorID;                 /* 0x00c */
    u8  aliveState;              /* 0x00e */
    u8  shouldBeKilled;          /* 0x00f */
    u8  unk_010;                 /* 0x010 */
    u8  unk_011;                 /* 0x011 */
    u8  unk_012;                 /* 0x012 */
    u8  unk_013;                 /* 0x013 */
    u8  sceneNode[0x14];               /* 0x014 */
    u8  behavNode[0x10];               /* 0x028 */
    u8  renderNode[0x10];              /* 0x038 */
    u8  pad_048[0x18];
    s32 mPosY;                 /* 0x060 */
    s32 mPosZ;                   /* 0x064 */
    s32 unk_068;                 /* 0x068 */
    s32 unk_06c;                 /* 0x06c */
    s32 unk_070;                 /* 0x070 */
    s32 mCamSpacePosX;           /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;                 /* 0x08e */
    s16 mAngleZ;                 /* 0x090 */
    s16 unk_092;                 /* 0x092 */
    s16 mPrevAngleY;             /* 0x094 */
    s16 unk_096;                 /* 0x096 */
    s32 mHorzSpeed;              /* 0x098 */
    s32 mVertAccel;              /* 0x09c */
    s32 mTerminalVelocity;       /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;              /* 0x0a8 */
    u8  pad_0ac[0x4];
    u32 mFlags;                  /* 0x0b0 */
    s32 unk_0b4;                 /* 0x0b4 */
    s32 unk_0b8;                 /* 0x0b8 */
    s32 unk_0bc;                 /* 0x0bc */
    s32 unk_0c0;                 /* 0x0c0 */
    u8  unk_0c4;                 /* 0x0c4 */
    u8  pad_0c5[0x7];
    s8  mAreaId;                 /* 0x0cc */
    u8  pad_0cd[0x1];
    s16 unk_0ce;                 /* 0x0ce */
    u8  pad_0d0[0x4];
    Model mModel;            /* 0x0d4 */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x14];
    u16 unk_300;            /* 0x300 */
    u8  pad_302[0x1e];
    s32 mRider;            /* 0x320 */
    s32 mClosestPlayer;            /* 0x324 */
    s32 mVariant;            /* 0x328 */
    s32 mState;            /* 0x32c */
    s32 mPlayerPosY;            /* 0x330 */
    /* The heights the lift works between. Every access in the ROM is a 32-bit
       one; the generated header spelled these u8 plus padding. */
    s32 mTopY;            /* 0x334 */
    s32 mBottomY;            /* 0x338 */
    s32 mMiddleY;            /* 0x33c */
    u32 mSoundHandle;            /* 0x340 */
    u16 mStateTimer;            /* 0x344 */
    u8  unk_346;            /* 0x346 */
    u8  mIsArmed;            /* 0x347 */
    u8  mIsRidden;            /* 0x348 */
    u8  unk_349;            /* 0x349 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

typedef char UpDownLiftBbh_size_must_be_0x34c[sizeof(struct UpDownLiftBbh) == 0x34c ? 1 : -1];

#endif
