#ifndef ROTATINGUPDOWNPLATFORM_H
#define ROTATINGUPDOWNPLATFORM_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

struct RotatingUpDownPlatform {
    u8  pad_000[0x8];
    u32 mParam;                  /* 0x008 */
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
    u8  pad_048[0x14];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    Model mModel;            /* 0x0d4 */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    s32 mState;            /* 0x320 */
    s32 mNodeCount;            /* 0x324 */
    s32 mNodeIndex;            /* 0x328 */
    s32 mBasePosX;            /* 0x32c */
    s32 mBasePosY;            /* 0x330 */
    s32 mBasePosZ;            /* 0x334 */
    s32 mTargetPosX;            /* 0x338 */
    s32 mTargetPosY;            /* 0x33c */
    s32 mTargetPosZ;            /* 0x340 */
    u8  mPathPtr;            /* 0x344 */
    u8  pad_345[0x7];
    s32 mSinkOffsetY;            /* 0x34c */
    s16 mBaseAngleY;            /* 0x350 */
    u8  mVariant;            /* 0x352 */
    u8  pad_353[0x1];
    u16 mStateTimer;            /* 0x354 */
    u8  mIsPressed;            /* 0x356 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

typedef char RotatingUpDownPlatform_size_must_be_0x358[
    sizeof(struct RotatingUpDownPlatform) == 0x358 ? 1 : -1];

#endif
