#ifndef KOOPA_H
#define KOOPA_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN5KoopaD1Ev`
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
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#include "dEnemyBase_c.h"
#include "BlendModelAnim.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "PathPtr.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct Koopa : dEnemyBase_c {
    dCcAc_c           mdCc_c;         /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x144 */
    ModelAnim                    mModelAnim;            /* 0x300 */
    ShadowModel                  mShadowModel;          /* 0x364 */
    s32                          mState;                /* 0x38c */
    s32                          mKoopaVariant;         /* 0x390 */
    s32                          mModelIndex;           /* 0x394 */
    u8  pad_398[0x4];
    s32                          unk_39c;               /* 0x39c */
    s32                          unk_3a0;               /* 0x3a0 */
    s32                          unk_3a4;               /* 0x3a4 */
    s32                          unk_3a8;               /* 0x3a8 */
    s32                          unk_3ac;               /* 0x3ac */
    s32                          unk_3b0;               /* 0x3b0 */
    u8  pad_3b4[0x8];
    s32                          mAnimSpeed;               /* 0x3bc */
    u8  pad_3c0[0x4];
    u16                          mWalkState;               /* 0x3c4 */
    u8  pad_3c6[0x4];
    u16                          mInvincibleTimer;               /* 0x3ca */
    u8  pad_3cc[0x2];
    u8                           mLandingDustTimer;               /* 0x3ce */
    u8  pad_3cf[0x1];

    /* --- vtable --- */
    virtual ~Koopa() {}

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char Koopa_size_must_be_0x3d0[sizeof(Koopa) == 0x3d0 ? 1 : -1];

#endif /* KOOPA_H */
