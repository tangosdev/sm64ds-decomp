#ifndef DAOBJHATENABLOCK_C_H
#define DAOBJHATENABLOCK_C_H

#include "types.h"

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"

/* Question / item / VS-item / cap blocks (HATENA_BLOCK 20, ITEM_BLOCK 21,
 * VS_ITEM_BLOCK 22, CAP_BLOCK_M/W/L 23-25). ROM RTTI daObjHatenaBlock_c;
 * direct base dBgActor_c. Factory allocates 0x3f8.
 *
 * daObjHatenaBlock_c_classInit_* are reconstructed (RTTI daObjHatenaBlock_c,
 * those six registry IDs). Retail does not store those spellings. */
struct daObjHatenaBlock_c : dBgActor_c {
    u8  pad_31e[0x2];
    ModelAnim mModelAnim;             /* 0x320 */
    ShadowModel mShadowModel;         /* 0x384 */
    /* InitResources copies mModel.mat4x3 here. DropShadowScaleXYZ takes
       this pairing. 0x3ac + 0x30 = 0x3dc. */
    Matrix4x3 mShadowMat;             /* 0x3ac */
    /* Bounce squash Y added onto mPosY when writing the model/collider
       translation (func_ov102_02149ff0 / 02149e38 / 021498e0). */
    s32 mBounceYOffs;                 /* 0x3dc */
    s32 mHomePosY;                    /* 0x3e0 */
    /* Floor Y from the ground raycast (func_ov102_02149610); shadow height. */
    s32 mFloorY;                      /* 0x3e4 */
    s32 mState;                       /* 0x3e8 */
    u16 mBounceAng;                   /* 0x3ec */
    u16 mBounceTimer;                 /* 0x3ee */
    u8 mStarTracked;                  /* 0x3f0 */
    u8 mStarId;                       /* 0x3f1 */
    /* Truncated from the hitter's param1 by every combat callback, then
       func_ov102_02149da8(this, 1). */
    u8 mHitterParam;                  /* 0x3f2 */
    u8 mContentType;                  /* 0x3f3 */
    /* Prize actor held through the bounce; bounce-end (func_ov102_021498e0)
       reads actorID 0x149 (SECRET_COIN) here. */
    dActor_c *mHeldActor;             /* 0x3f4 */

    /* Inline is load-bearing: out-of-line emits D0 before D1. */
    virtual ~daObjHatenaBlock_c() {}

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    void OnGroundPounded(dActor_c &other);     /* slot 21 */
    int OnAttacked1(dActor_c &other);          /* slot 22 */
    void OnKicked(dActor_c &other);            /* slot 24 */
    void OnHitByMegaChar(Player &player);      /* slot 27 */
    int OnHitFromUnderneath(dActor_c &other);  /* slot 28 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjHatenaBlock_c_size_must_be_0x3f8[sizeof(daObjHatenaBlock_c) == 0x3f8 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJHATENABLOCK_C_H */
