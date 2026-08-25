#ifndef CHAINCHOMP_H
#define CHAINCHOMP_H

#include "types.h"

/* The Chain Chomp, and its chain -- seven links, which is why seven of nearly
 * everything. NINE CONSECUTIVE BOUNDARIES close on sizes other headers assert,
 * the longest unbroken run this branch has found:
 *
 *     dEnemyBase_c                     ends 0x110
 *     dCcAcPos_c 0x110 +   0x40  = 0x150  -> ModelAnim
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

#include "dEnemyBase_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"

struct ChainChomp : dEnemyBase_c {
    dCcAcPos_c mdCcAcPos_c;  /* 0x110 */
    ModelAnim mModelAnim;                                  /* 0x150 */
    ShadowModel mShadowModel;                              /* 0x1b4 */
    Model mLinkModels[7];                                  /* 0x1dc */
    ShadowModel mLinkShadows[7];                           /* 0x40c */
    /* InitResources seeds all seven with the chomp's own position, one per link;
       unk_578 is a second seven-element run of the same shape that nothing
       matched writes, so which of the two is the chain's previous-position
       history is unevidenced and it keeps its unk_ name. */
    Vector3 mLinkPos[7];                                   /* 0x524 */
    Vector3 unk_578[7];                                    /* 0x578 */
    u8  pad_5cc[0x20];
    /* InitResources copies mPosX/mPosY/mPosZ here and then moves the actor itself
       by +200.0 on each axis, so this is where it started. Behavior clamps
       mPosY up to mSpawnPosY + 0xc8000 every frame. */
    s32 mSpawnPosX;         /* 0x5ec */
    s32 mSpawnPosY;         /* 0x5f0 */
    s32 mSpawnPosZ;         /* 0x5f4 */
    s32 mChainExtension;    /* 0x5f8 */
    u8  pad_5fc[0x9];
    u8  unk_605;            /* 0x605 -- gates three Behavior helpers; nothing writes it */
    u8  pad_606[0x2];
    /* uniqueIDs (fBase_c +0x04) of two other actors. 0x1b and 0x29 are resolved
       through ACTOR_SPAWN_TABLE at 0x02090864 -- see notes/enemy-leaf-provenance.md. */
    s32 mStumpUniqueID;     /* 0x608 -- the actor 0x1b InitResources spawns */
    s32 mFenceUniqueID;     /* 0x60c -- actor 0x29, found lazily by Behavior */
    u8  pad_610[0xc];
    /* Set when this frame's position had to be clamped up to the rest height,
       and last frame's copy of it. Behavior fires func_ov014_02111fb8 only on
       the rising edge, which is what a landing one-shot looks like. */
    u8  mIsOnGround;        /* 0x61c */
    u8  mWasOnGround;       /* 0x61d */

    virtual ~ChainChomp();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char ChainChomp_size_must_be_0x620[sizeof(ChainChomp) == 0x620 ? 1 : -1];

#else

/* The same object for a C translation unit, flat. */
struct ChainChomp {
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
    s32 mLinkPos[21];       /* 0x524 -- the C++ branch's Vector3 mLinkPos[7], flat */
    u8  pad_578[0x74];
    s32 mSpawnPosX;         /* 0x5ec */
    s32 mSpawnPosY;         /* 0x5f0 */
    s32 mSpawnPosZ;         /* 0x5f4 */
    s32 mChainExtension;    /* 0x5f8 */
    u8  pad_5fc[0x9];
    u8  unk_605;            /* 0x605 */
    u8  pad_606[0x2];
    s32 mStumpUniqueID;     /* 0x608 */
    s32 mFenceUniqueID;     /* 0x60c */
    u8  pad_610[0xc];
    u8  mIsOnGround;        /* 0x61c */
    u8  mWasOnGround;       /* 0x61d */
};

#endif /* __cplusplus */

#endif /* CHAINCHOMP_H */
