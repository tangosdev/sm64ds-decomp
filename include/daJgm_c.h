#ifndef DAJGM_C_H
#define DAJGM_C_H

#include "dActor_c.h"
#include "ModelAnim.h"
#include "Model.h"
#include "ShadowModel.h"
#include "TextureSequence.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daJgm_c;

typedef s32 (daJgm_c::*DaJgmStateHandler)();

struct DaJgmStateHandlers {
    DaJgmStateHandler enter;
    DaJgmStateHandler update;
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char DaJgmStateHandlers_size_must_be_0x10[
    sizeof(DaJgmStateHandlers) == 0x10 ? 1 : -1];
#endif

/* Lakitu (JUGEM 265) -- ov077/daJgm_c.
 *
 * ov077 is mixed (HEAVE_HO 238 / SPINY 260 / LAKITU 265). RTTI
 * ov077:0x02127830 names daJgm_c; the debug table names JUGEM.
 * Direct base is dActor_c. Not daC_Jugem_c (C_JUGEM 235, ov085).
 *
 * Factory allocates 0x420. Member NAMES below are from this TU's
 * own methods. The reconstructed factory daJgm_c_classInit
 * (historical alias Lakitu_Spawn) installs this class's cartridge
 * vtable; g_profile_JUGEM is the registry descriptor.
 *
 * deslop leftovers (this TU):
 * - Factory stays `return new daJgm_c()`. Leaf unsigned-long
 *   operator new forwards `_ZN7fBase_cnwEj` until #2570.
 * - Inline destructor: out-of-line emits D0 before D1. Reverse
 *   source order of the ordinary members is the TU's.
 * - sizeof wrap stays (0x420 / DaJgmStateHandlers 0x10).
 * - Private helper spellings are inferred; this TU proves
 *   ownership. OnTurnIntoEgg R6Player / UpdateWallAndWater
 *   R10dBgCh_Actr: a pointer generates identical ARM. SetState's
 *   `int` signedness is not distinguishable.
 */
struct daJgm_c : dActor_c {
    dActor_c                 *mCarryActor;          /* 0x0d0 */
    ModelAnim                 mModelAnim;            /* 0x0d4 */
    Model                     mModel;                /* 0x138 */
    ShadowModel               mShadowModel;          /* 0x188 */
    TextureSequence           mTextureSequence;      /* 0x1b0 */
    dCcAcPos_c                mdCcAcPos_c;           /* 0x1c4 */
    dBgCh_Actr                mWithMeshClsn;         /* 0x204 */
    /* InitResources assigns IDENTITY_MATRIX4X3 straight into this slot, so
       0x3c0 begins a Matrix4x3 (0x30 bytes, through 0x3ef). */
    Matrix4x3                 mMatrix;               /* 0x3c0 */
    DaJgmStateHandlers       *mStateHandlers;        /* 0x3f0 */
    s32                       mState;                /* 0x3f4 */
    s32                       mSpawnPosX;            /* 0x3f8 */
    s32                       mSpawnPosY;            /* 0x3fc */
    s32                       mSpawnPosZ;            /* 0x400 */
    s32                       mThrowPosX;            /* 0x404 */
    s32                       mThrowPosY;            /* 0x408 */
    s32                       mThrowPosZ;            /* 0x40c */
    u32                       mSoundHandle;          /* 0x410 */
    s32                       mBobOffsetY;           /* 0x414 */
    s16                       mBobAngle;             /* 0x418 */
    s16                       mAngVelY;              /* 0x41a */
    u8                        mTimer;                /* 0x41c */
    u8                        mTurnDir;              /* 0x41d */
    u8                        pad_41e[0x2];

    /* Inline is load-bearing: out-of-line emits D0 before D1. */
    virtual ~daJgm_c() {}

    virtual s32  InitResources();       /* slot 0 */
    virtual s32  CleanupResources();    /* slot 3 */
    virtual s32  Behavior();            /* slot 6 */
    virtual s32  Render();              /* slot 9 */
    virtual void OnPendingDestroy();    /* slot 12 */
    virtual int  OnYoshiTryEat();       /* slot 18 */
    virtual void OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual int  OnAimedAtWithEgg();    /* slot 29 */

    static void *operator new(size_t size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

private:
    void EnterState();
    void UpdateState();
    void SetState(int state);
    s32  EnterHoverState();
    s32  UpdateHoverState();
    s32  EnterThrowState();
    s32  UpdateThrowState();
    s32  EnterYoshiMouthState();
    s32  UpdateYoshiMouthState();
    s32  EnterSpitState();
    s32  UpdateSpitState();
    s32  EnterFallState();
    s32  UpdateFallState();
    void UpdateModels();
    void UpdateWallAndWater(dBgCh_Actr &collision);
    void HandlePlayerCollision();
    void DieAndDropCoins();
    void UpdateFlight();
    int  TurnOffsetFromDist(int dist);
    int  CountSpinies();
    void UpdateHoverBob();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daJgm_c_size_must_be_0x420[
    sizeof(daJgm_c) == 0x420 ? 1 : -1];
#endif

#endif /* DAJGM_C_H */
