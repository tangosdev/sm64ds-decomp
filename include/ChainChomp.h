#ifndef CHAINCHOMP_H
#define CHAINCHOMP_H

#include "types.h"

/* The Chain Chomp, and its chain -- seven links, which is why seven of nearly
 * everything. NINE CONSECUTIVE BOUNDARIES close on sizes other headers assert,
 * the longest unbroken run this branch has found:
 *
 *     Enemy                     ends 0x110
 *     MovingCylinderClsnWithPos 0x110 +   0x40  = 0x150  -> ModelAnim
 *     ModelAnim                 0x150 +   0x64  = 0x1b4  -> ShadowModel
 *     ShadowModel               0x1b4 +   0x28  = 0x1dc  -> the link models
 *     Model[7]                  0x1dc + 7*0x50  = 0x40c  -> the link shadows
 *     ShadowModel[7]            0x40c + 7*0x28  = 0x524  -> the first triple
 *     Vector3[7]                0x524 + 7*0x0c  = 0x578  -> the second
 *     Vector3[7]                0x578 + 7*0x0c  = 0x5cc
 *
 * Every element type was already named in the tree, so this class needed no new
 * one: the two Vector3 arrays are the per-link positions.
 */

#ifdef __cplusplus

#include "Enemy.h"
#include "Model.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "MovingCylinderClsnWithPos.h"

struct ChainChomp : Enemy {
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;  /* 0x110 */
    ModelAnim mModelAnim;                                  /* 0x150 */
    ShadowModel mShadowModel;                              /* 0x1b4 */
    Model mLinkModels[7];                                  /* 0x1dc */
    ShadowModel mLinkShadows[7];                           /* 0x40c */
    Vector3 unk_524[7];                                    /* 0x524 */
    Vector3 unk_578[7];                                    /* 0x578 */
    u8  pad_5cc[0x24];
    s32 unk_5f0;            /* 0x5f0 */
    u8  pad_5f4[0x11];
    u8  unk_605;            /* 0x605 */
    u8  pad_606[0x6];
    s32 unk_60c;            /* 0x60c */
    u8  pad_610[0xc];
    u8  unk_61c;            /* 0x61c */
    u8  unk_61d;            /* 0x61d */

    virtual ~ChainChomp();

    int Behavior();
    int InitResources();
    int Render();
};

#else

/* The same object for a C translation unit, flat. */
struct ChainChomp {
    u8  pad_000[0x60];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x1c];
    /* 0x080..0x110 is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
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
    s32 unk_0b4;                 /* 0x0b4 */
    s32 unk_0b8;                 /* 0x0b8 */
    s32 unk_0bc;                 /* 0x0bc */
    s32 unk_0c0;                 /* 0x0c0 */
    u8  unk_0c4;                 /* 0x0c4 */
    u8  pad_0c5[0x7];
    s8  mAreaId;                 /* 0x0cc */
    u8  pad_0cd[0x1];
    s16 unk_0ce;                 /* 0x0ce */
    u8  pad_0d0[0x40];
    u8  mMovingCylinderClsnWithPos;            /* 0x110 */
    u8  pad_111[0x3f];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x150 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    u8  mModelAnim[0x64];            /* 0x150 */
    u8  mShadowModel;            /* 0x1b4 */
    u8  pad_1b5[0x43b];
    s32 unk_5f0;            /* 0x5f0 */
    u8  pad_5f4[0x11];
    u8  unk_605;            /* 0x605 */
    u8  pad_606[0x6];
    s32 unk_60c;            /* 0x60c */
    u8  pad_610[0xc];
    u8  unk_61c;            /* 0x61c */
    u8  unk_61d;            /* 0x61d */
};

#endif /* __cplusplus */

#endif /* CHAINCHOMP_H */
