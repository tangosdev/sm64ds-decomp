#ifndef DABAKUBAKU_C_H
#define DABAKUBAKU_C_H

#include "common.h"
#include "types.h"
#include "dEnemyBase_c.h"
#include "dExtAnmModel_c.h"
#include "dCcAcPos_c.h"
#include "dExtShadowModel_c.h"
#include "dBgCh_Actr.h"

/**
 * Jolly Roger Bay Bubba (`BAKUBAKU` 228).
 */
struct daBakubaku_c : dEnemyBase_c {
    /* The { enter, main } record the state machine runs; the five tables
     * live in ov032 .bss, filled by __sinit_d_a_bakubaku.cpp from the PMF
     * constants at 0x0211377c..0x021137c4. Both slots take the address
     * of int-returning members. */
    typedef int (daBakubaku_c::*StateFn)();
    struct State {
        StateFn enter;
        StateFn main;
    };

    dCcAcPos_c  mBodyClsn;       /* 0x110 */
    dCcAcPos_c  mHeadClsn;       /* 0x150 */
    dBgCh_Actr  mWithMeshClsn;   /* 0x190 */
    dExtAnmModel_c mAnmModel;    /* 0x34c */
    State      *mState;          /* 0x3b0 -- the table this fish is running */
    dExtShadowModel_c mShadowModel;    /* 0x3b4 */
    Matrix4x3   mShadowMat;      /* 0x3dc -- DropShadowRadHeight source */
    Vector3 mSpawnPos;           /* 0x40c */
    Vector3 mTargetPos;          /* 0x418 */
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

    int SetState(void *state);

    /* The state hooks and helpers. The ROM records no English names; the
     * pointer-to-member constants at 0x0211377c..0x021137c4 prove ten of
     * them are members. The hooks take the EnterX/ExecuteX spelling of
     * daBasabasa_c and daBbl_c, which use the same { enter, main } record. */
    int EnterWander();        /* wander enter */
    int ExecuteWander();      /* wander main */
    int EnterPause();         /* pause enter */
    int ExecutePause();       /* pause main */
    int EnterChase();         /* chase enter */
    int ExecuteChase();       /* chase main */
    int EnterSurface();       /* surface enter */
    int ExecuteSurface();     /* surface main */
    int EnterDive();          /* dive enter */
    int ExecuteDive();        /* dive main */
    int ShouldReturnHome();   /* abort test: player gone, wall, too far */
    int FindChaseTarget();    /* chase-target test: stores player pos */
    void HandleCylinderHit(); /* cylinder collision: bite / mega-kill */
    void UpdateModelMatrix(); /* model + shadow matrices */
};

typedef char daBakubaku_c_size_must_be_0x438[sizeof(daBakubaku_c) == 0x438 ? 1 : -1];

#endif /* DABAKUBAKU_C_H */
