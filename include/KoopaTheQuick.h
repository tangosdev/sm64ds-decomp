#ifndef KOOPATHEQUICK_H
#define KOOPATHEQUICK_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN13KoopaTheQuickD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `dEnemyBase_c::~dEnemyBase_c`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another, and dEnemyBase_c's own 0x110 closes
 * exactly on the first. Member NAMES are the ones this header already used --
 * a rebase should not also rename things its callers spell:
 *
 *     0x110 dCcAc_c       0x34   -> 0x144
 *     0x144 dBgCh_Actr             0x1bc  -> 0x300
 *     0x300 ModelAnim                0x64   -> 0x364
 *     0x364 ShadowModel              0x28   -> 0x38c
 *     0x3cc Vector3                  0xc    -> 0x3d8
 *     0x3d8 PathPtr                  0x8    -> 0x3e0
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS RTTI names the implementation daRNk_c. The reconstructed factory
 * daRNk_c_classInit (historical alias KoopaTheQuick_Spawn)
 * installs this class's cartridge vtable; the reconstructed profile
 * global g_profile_RACE_NOKO (historical alias KoopaTheQuick_SpawnInfo)
 * is its registry descriptor.
 */

#include "dEnemyBase_c.h"
#include "BlendModelAnim.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "PathPtr.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct KoopaTheQuick : dEnemyBase_c {
    dCcAc_c           mdCcAc_c;   /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x144 */
    ModelAnim                    mModelAnim;            /* 0x300 */
    ShadowModel                  mShadowModel;          /* 0x364 */
    s32                          mState;               /* 0x38c */
    u8  pad_390[0x4];
    s32                          mFlagID;               /* 0x394 */
    s32                          mPlayer;               /* 0x398 */
    s32                          unk_39c;               /* 0x39c */
    s32                          unk_3a0;               /* 0x3a0 */
    u8                           unk_3a4;               /* 0x3a4 */
    u8  pad_3a5[0x5];
    s16                          unk_3aa;               /* 0x3aa */
    u8                           mHasPlayerUsedCannon;               /* 0x3ac */
    u8                           mPathPtToJumpAt1;               /* 0x3ad */
    u8                           mPathPtToJumpAt2;               /* 0x3ae */
    u8  pad_3af[0x1];
    u8                           mTrackedStar;               /* 0x3b0 */
    u8                           mStarID;               /* 0x3b1 */
    u8  pad_3b2[0x2];
    u8                           mHasFinished;               /* 0x3b4 */
    u8                           mIsRacing;               /* 0x3b5 */
    u8                           mIsTalkingToMario;               /* 0x3b6 */
    u8  pad_3b7[0x1];
    s32                          mNumPathPts;               /* 0x3b8 */
    s32                          mCurPathPt;               /* 0x3bc */
    s32                          unk_3c0;               /* 0x3c0 */
    s32                          unk_3c4;               /* 0x3c4 */
    u8                           unk_3c8;               /* 0x3c8 */
    u8  pad_3c9[0x3];
    Vector3                      mPathTarget;           /* 0x3cc */
    PathPtr                      mPathPtr;              /* 0x3d8 */

    /* --- vtable --- */
    virtual ~KoopaTheQuick();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char KoopaTheQuick_size_must_be_0x3e0[sizeof(KoopaTheQuick) == 0x3e0 ? 1 : -1];

#endif /* KOOPATHEQUICK_H */
