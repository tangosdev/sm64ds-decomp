#ifndef FIREPIRANHAPLANTBIG_H
#define FIREPIRANHAPLANTBIG_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN19FirePiranhaPlantBigD1Ev`
 * stores this vtable, destroys three members, then calls `dEnemyBase_c::~dEnemyBase_c`.
 * Everything this header used to restate below 0x110 belongs to that chain and
 * is inherited now.
 *
 * The members close exactly on one another, and dEnemyBase_c's own 0x110 closes
 * exactly on the first:
 *
 *     0x110 ModelAnim                  0x64   -> 0x174
 *     0x174 dCcAc_c         0x34   -> 0x1a8
 *     0x1a8 dCcAcPos_c  0x40   -> 0x1e8
 *
 * Typing them absorbed these markers, which were a member's insides:
 *   - 0x160 mAnimation   = the Animation base of mModelAnim
 *   - 0x178 unk_178      = mdCcAc_c.radius (+0x04)
 *   - 0x17c unk_17c      = mdCcAc_c.height (+0x08)
 *   - 0x1ac unk_1ac      = mdCcAcPos_c.radius (+0x04)
 *   - 0x1b0 unk_1b0      = mdCcAcPos_c.height (+0x08)
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "Model.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

struct FirePiranhaPlantBig : dEnemyBase_c {
    ModelAnim                    mModelAnim;            /* 0x110 */
    dCcAc_c           mdCcAc_c;   /* 0x174 */
    dCcAcPos_c    mdCcAcPos_c; /* 0x1a8 */
    s32                          mRespawnMode;          /* 0x1e8 */
    s32                          mState;                /* 0x1ec */
    s32                          mGroupLeaderID;        /* 0x1f0 */
    s32                          unk_1f4;               /* 0x1f4 */
    Vector3                      mClsnOffset;           /* 0x1f8 */
    s32                          mScale;                /* 0x204 */
    s32                          mClsnRadiusFactor;     /* 0x208 */
    s32                          mClsnHeightFactor;     /* 0x20c */
    s32                          mMaxScale;             /* 0x210 */
    s32                          mScaleRate;            /* 0x214 */
    u8  pad_218[0x2];
    u8                           mGroupAliveCount;      /* 0x21a */
    u8                           mGroupDefeatedCount;   /* 0x21b */
    u8                           unk_21c;               /* 0x21c */
    u8                           unk_21d;               /* 0x21d */
    u8                           mSuppressDeathReward;  /* 0x21e */
    u8                           mStarID;               /* 0x21f */
    u8                           mAlive;                /* 0x220 */
    u8  pad_221[0x3];
    s32                          unk_224;               /* 0x224 */
    s32                          unk_228;               /* 0x228 */

    /* --- vtable --- */
    virtual ~FirePiranhaPlantBig();

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char FirePiranhaPlantBig_size_must_be_0x22c[sizeof(FirePiranhaPlantBig) == 0x22c ? 1 : -1];

#endif /* FIREPIRANHAPLANTBIG_H */
