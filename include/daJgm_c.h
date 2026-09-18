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

/* daJgm_c is the ROM's own RTTI name for this class (this tree once coined it
 * Lakitu): the typeinfo at ov077 0x0212783c names dActor_c as the sole base at
 * offset 0, and the class's vtable at 0x0212786c (31 slots, same count as
 * dActor_c's) is what pairs it to daJgm_c_classInit (historical alias
 * Lakitu_Spawn), which stores that address after allocating 0x420 bytes via
 * fBase_c::operator new.
 *
 * TWO WITNESSES, and they close on each other:
 *   daJgm_c_classInit  fBase_c::operator new(0x420), dActor_c::dActor_c(),
 *                      stores _ZTV7daJgm_c, then the six members below in
 *                      this order.
 *   ~daJgm_c           the same members destroyed in reverse, then ~dActor_c.
 *
 * Own vtable slots, from the ROM table diffed against dActor_c's: 0
 * InitResources, 3 CleanupResources, 6 Behavior, 9 Render, 12
 * OnPendingDestroy, 16/17 the destructor pair, 18 OnYoshiTryEat, 19
 * OnTurnIntoEgg, 29 OnAimedAtWithEgg. Every other slot is inherited.
 *
 * The five state pairs are recovered from __sinit_ov077_02127240, which copies
 * ten ROM PMF constants into data_ov077_02127bc4. SetState indexes that table
 * at state * 0x10 and invokes the first PMF; Behavior invokes the second.
 * Descriptive original names are absent; the spellings below are inferred.
 *
 * OnTurnIntoEgg mangles as R6Player (reference). A pointer (P6Player)
 * generates identical ARM; the ROM cannot prove which. UpdateWallAndWater
 * mangles as R10dBgCh_Actr for the same reason. SetState's signed parameter
 * spelling is not distinguishable from other 32-bit forms.
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

    /* Declared first on purpose, same reasoning as dActor_c.h: the key
       function pins where mwcc anchors the vtable. */
    /* Inline plus vtable instantiation is load-bearing: mwcc emits retail's
       D1 then D0 pair, with no homeless D2. InitResources is the first
       out-of-line virtual and anchors this TU's vtable/RTTI group. */
    virtual ~daJgm_c() {}

    virtual s32  InitResources();       /* slot 0 */
    virtual s32  CleanupResources();    /* slot 3 */
    virtual s32  Behavior();            /* slot 6 */
    virtual s32  Render();              /* slot 9 */
    virtual void OnPendingDestroy();    /* slot 12 */
    virtual int  OnYoshiTryEat();       /* slot 18 */
    /* Player is a reference in the coined mangling; a pointer generates
       identical ARM. */
    virtual void OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual int  OnAimedAtWithEgg();    /* slot 29 */

    /* Leaf in-class operator new until fBase_c::operator new(unsigned long)
       lands (#2570). unsigned long, not unsigned int: `new daJgm_c()` looks
       up _Znwm. */
    static void *operator new(size_t size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

private:
    /* Class ownership, calls, bodies, layout, relocations and codegen are
       proven. These readable private spellings are inferred aliases; no
       original names survive. UpdateWallAndWater mangles as R10dBgCh_Actr
       (reference). A pointer (P10dBgCh_Actr) generates identical ARM; the
       ROM cannot prove which. SetState's `int` is a 32-bit register argument
       whose signedness the bytes cannot distinguish. */
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
