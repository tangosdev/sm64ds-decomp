#ifndef KOOPA_H
#define KOOPA_H

#include "types.h"

/* Retail ov062 RTTI at 0x0211da5c spells "8daNknk_c". Its typeinfo at
 * 0x0211da68 names dEnemyBase_c; the vtable address point is 0x0211dab4.
 * Normal and small Koopa factories both allocate 0x3d0 bytes of this class.
 * Member names are reconstructed; the member destructor calls and offsets
 * establish mdCc_c@0x110, mWithMeshClsn@0x144, mModelAnim@0x300 and
 * mShadowModel@0x364. The compiler checks the observed final field span.
 * Existing virtual names/signatures, including OnTurnIntoEgg's reference,
 * remain project reconstructions, not original English spellings in the ROM.
 */

#include "dEnemyBase_c.h"
#include "BlendModelAnim.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "PathPtr.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct daNknk_c : dEnemyBase_c {
    dCcAc_c         mdCc_c;                  /* 0x110 */
    dBgCh_Actr      mWithMeshClsn;           /* 0x144 */
    ModelAnim       mModelAnim;              /* 0x300 */
    ShadowModel     mShadowModel;            /* 0x364 */
    s32             mState;                  /* 0x38c */
    s32             mKoopaVariant;           /* 0x390 */
    s32             mModelIndex;             /* 0x394 */
    u8              pad_398[0x4];
    s32             unk_39c;                 /* 0x39c */
    s32             unk_3a0;                 /* 0x3a0 */
    s32             unk_3a4;                 /* 0x3a4 */
    s32             unk_3a8;                 /* 0x3a8 */
    s32             unk_3ac;                 /* 0x3ac */
    s32             unk_3b0;                 /* 0x3b0 */
    u8              pad_3b4[0x8];
    s32             mAnimSpeed;              /* 0x3bc */
    u8              pad_3c0[0x4];
    u16             mWalkState;              /* 0x3c4 */
    u8              pad_3c6[0x4];
    u16             mInvincibleTimer;        /* 0x3ca */
    u8              pad_3cc[0x2];
    u8              mLandingDustTimer;       /* 0x3ce */
    u8              pad_3cf[0x1];

    /* --- vtable --- */
    virtual ~daNknk_c() {}

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual void  OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

// Community source compatibility name; retail RTTI is daNknk_c.
typedef daNknk_c Koopa;

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Koopa_size_must_be_0x3d0[sizeof(Koopa) == 0x3d0 ? 1 : -1];
#endif

#endif /* KOOPA_H */
