#ifndef DABAKUBAKU_C_H
#define DABAKUBAKU_C_H

#include "common.h"
#include "types.h"
#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

/* Forward: the { enter, main } record in ov032 BSS. The TU completes it.
   The record holds a pointer-to-member of this class, so it is not a
   member; nesting that type here is the old compiler ICE. */
struct BakubakuState;

/**
 * Jolly Roger Bay Bubba (`BAKUBAKU` 228).
 *
 * common.h is first so Matrix4x3 is the flat 12-word spelling. Model.h's
 * nested { Matrix3x3 r; Vector3 t } scalarizes mShadowMat, and Vector3's
 * destructor would then run from this class's inline D1.
 */
struct daBakubaku_c : dEnemyBase_c {
    dCcAcPos_c  mBodyClsn;       /* 0x110 */
    dCcAcPos_c  mHeadClsn;       /* 0x150 */
    dBgCh_Actr  mWithMeshClsn;   /* 0x190 */
    ModelAnim   mModelAnim;      /* 0x34c */
    BakubakuState *mState;       /* 0x3b0 */
    ShadowModel mShadowModel;    /* 0x3b4 */
    Matrix4x3   mShadowMat;      /* 0x3dc -- DropShadowRadHeight source */
    s32         mSpawnPosX;      /* 0x40c */
    s32         mSpawnPosY;      /* 0x410 */
    s32         mSpawnPosZ;      /* 0x414 */
    s32         mTargetPosX;     /* 0x418 */
    s32         mTargetPosY;     /* 0x41c */
    s32         mTargetPosZ;     /* 0x420 */
    s32         mLungePhase;     /* 0x424 */
    u8          mMouthOpen;      /* 0x428 */
    u8          unk_429;         /* 0x429 -- only cleared */
    u16         mChaseCooldown;  /* 0x42a -- gates the chase test */
    s32         mSplashParticle; /* 0x42c */
    s16         mAngTarget;      /* 0x430 */
    u8          pad_432[2];
    s32         mDiveStartY;     /* 0x434 */

    virtual s32  InitResources();
    virtual s32  CleanupResources();
    virtual s32  Behavior();
    virtual s32  Render();
    virtual void OnPendingDestroy();
    virtual int  OnAimedAtWithEgg();

    /* fBase_c::operator new(size_t). No leaf copy: the factory is
       `return new daBakubaku_c()`. */

    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daBakubaku_c() {}

    int func_ov032_02111ff4(void *state);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daBakubaku_c_size_must_be_0x438[sizeof(daBakubaku_c) == 0x438 ? 1 : -1];
#endif

#endif /* DABAKUBAKU_C_H */
