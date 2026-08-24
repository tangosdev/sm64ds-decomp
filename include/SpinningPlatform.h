#ifndef SPINNINGPLATFORM_H
#define SPINNINGPLATFORM_H
#include "types.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dBgW_KcMbg.h"

struct SpinningPlatform {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x2];
    s16 mPrevAngleX;            /* 0x092 */
    u8  pad_094[0x40];
    Model mModel;            /* 0x0d4 */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    /* The collider's transform: InitResources hands +0x2ec to
       dBgW_KcMbg::SetFile as its `const Matrix4x3 &`. Was a u8 marker
       plus its pad. */
    Matrix4x3 mClsnMat;            /* 0x2ec */
    u8  pad_31c[0x2];
    s8  mRandDirection;            /* 0x31e */
    u8  pad_31f[0x1];
    u16 mRandTimer;            /* 0x320 */
    u16 mRandFrames;            /* 0x322 */
    s32 mFloorPosY;            /* 0x324 */
    ShadowModel mShadowModel; /* 0x328 */
    /* The shadow's transform. ShadowModel + Matrix4x3 is the same pair
       HauntedChair evidences by byte (a 48-byte identity block-copied over
       +0x14c, which lands exactly on the next member), and 0x350 + 0x30 closes
       on the 0x380 SpinningPlatform_Spawn allocates. */
    Matrix4x3 mShadowMat;            /* 0x350 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

typedef char SpinningPlatform_size_must_be_0x380[
    sizeof(struct SpinningPlatform) == 0x380 ? 1 : -1];

#endif
