#ifndef DABAKUBAKU_C_H
#define DABAKUBAKU_C_H

#include "types.h"
#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Jolly Roger Bay's Bubba (`bakubaku`). Factory size 0x438.
 * ov027 maps SlidingIce at the same vtable VA -- resolve inside ov032.
 *
 * `daBakubaku_c` is the RTTI name.
 */
struct daBakubaku_c : dEnemyBase_c {
    dCcAcPos_c  mBodyClsn;       /* 0x110 */
    dCcAcPos_c  mHeadClsn;       /* 0x150 */
    dBgCh_Actr               mWithMeshClsn;   /* 0x190 */
    ModelAnim                  mModelAnim;      /* 0x34c */
    /* Pointer to a { PMF enter; PMF main } table entry. Left void *:
       completing the pointed-to class as daBakubaku_c makes mwccarm ICE. */
    void                      *mState;          /* 0x3b0 */
    ShadowModel                mShadowModel;    /* 0x3b4 */
    s32                        mShadowMat[12];  /* 0x3dc -- DropShadow source */
    s32                        mSpawnPosX;      /* 0x40c */
    s32                        mSpawnPosY;      /* 0x410 */
    s32                        mSpawnPosZ;      /* 0x414 */
    s32                        mTargetPosX;     /* 0x418 */
    s32                        mTargetPosY;     /* 0x41c */
    s32                        mTargetPosZ;     /* 0x420 */
    s32                        mLungePhase;     /* 0x424 */
    u8                         mMouthOpen;      /* 0x428 */
    u8                         unk_429;         /* 0x429 -- only cleared */
    u16                        mChaseCooldown;  /* 0x42a -- 0x28/0x64; gates 02111254 */
    s32                        mSplashParticle; /* 0x42c */
    s16                        mAngTarget;      /* 0x430 */
    u8                         pad_432[2];
    s32                        mDiveStartY;     /* 0x434 */

    virtual s32  InitResources();
    virtual s32  CleanupResources();
    virtual s32  Behavior();
    virtual s32  Render();
    virtual void OnPendingDestroy();
    virtual int  OnAimedAtWithEgg();

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }

    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daBakubaku_c() {}
};

typedef char daBakubaku_c_size_must_be_0x438[sizeof(daBakubaku_c) == 0x438 ? 1 : -1];

#endif /* DABAKUBAKU_C_H */
