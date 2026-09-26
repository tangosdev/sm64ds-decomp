#ifndef DASHL_C_H
#define DASHL_C_H

#include "types.h"
#include "dEnemyBase_c.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"
#include "dCcAc_c.h"

/* Koopa shell (profile SHELL). ROM RTTI daShl_c; direct base dEnemyBase_c
 * (the __si_class_type_info at ov102:0x0214e620 names _ZTI12dEnemyBase_c).
 * daShl_c_classInit allocates 0x3e0.
 *
 * Six boundaries close on sizes other headers assert:
 *
 *     dEnemyBase_c               ends 0x110
 *     dCcAc_c     0x110 + 0x034 = 0x144  -> dBgCh_Actr
 *     dBgCh_Actr  0x144 + 0x1bc = 0x300  -> Model
 *     Model       0x300 + 0x050 = 0x350  -> ShadowModel
 *     ShadowModel 0x350 + 0x028 = 0x378  -> the second cylinder
 *     dCcAc_c     0x378 + 0x034 = 0x3ac  -> mState
 *
 * Two cylinder collisions: mdCc_c is set up in InitResources, mdCc_c2 only
 * when the shell is spat out and sent sliding (func_ov102_0214d148).
 *
 * The vtable (ov102:0x0214e650, 31 slots) was read slot by slot: only the
 * slots declared below differ from dEnemyBase_c's inherited words.
 *
 * daShl_c_classInit is reconstructed (RTTI daShl_c, SHELL registry).
 * Retail does not store that spelling. Historical aliases: KoopaShell
 * (the class), KoopaShell_Spawn.
 */
struct daShl_c : dEnemyBase_c {
    dCcAc_c mdCc_c;            /* 0x110 */
    dBgCh_Actr mMeshClsn;      /* 0x144 */
    Model mModel;              /* 0x300 */
    ShadowModel mShadowModel;  /* 0x350 */
    dCcAc_c mdCc_c2;           /* 0x378 */
    /* Current state record: {enter, tick}, two pointers-to-member. Behavior
       compares it against the four file-scope records by ADDRESS. */
    void* mState;              /* 0x3ac */
    /* Where it was spawned: InitResources copies mPos here verbatim, and
       Behavior stashes mPrevAngleY into mSpawnAngleY when the shell is spat
       back out of Yoshi's mouth. */
    s32 mSpawnPosX;            /* 0x3b0 */
    s32 mSpawnPosY;            /* 0x3b4 */
    s32 mSpawnPosZ;            /* 0x3b8 */
    s16 mSpawnAngleY;          /* 0x3bc */
    u8  pad_3be[0x2];
    /* The player riding or holding the shell (dActor_c *, stored as a word). */
    s32 mCarrier;              /* 0x3c0 */
    /* Model index, 0 or 1, off bit 0 of param1. Behavior gives index 0 a
       per-frame splash/trail pass the other does not get. */
    u8  mModelIndex;           /* 0x3c4 */
    u8  mVariant;              /* 0x3c5 -- param1 bit 4 */
    /* Despawn countdown, and it only runs in one state. Render blinks the
       shell while it is below 0x2d by skipping odd values, so the shell
       flashes out rather than vanishing. */
    u8  mDespawnTimer;         /* 0x3c6 */
    u8  pad_3c7[0x1];
    /* Particle handles for the four trail/splash effects. */
    s32 mParticleHandle_3c8;   /* 0x3c8 */
    s32 mParticleHandle_3cc;   /* 0x3cc */
    s32 mParticleHandle_3d0;   /* 0x3d0 */
    s32 mParticleHandle_3d4;   /* 0x3d4 */
    /* Looping surface sound: handle, then the sound ID it was started with. */
    s32 mSoundHandle;          /* 0x3d8 */
    s32 mSoundID;              /* 0x3dc */

    /* Inline is load-bearing: out-of-line emits D0 before D1. */
    virtual ~daShl_c() {}

    virtual s32  InitResources();      /* slot  0 */
    virtual s32  CleanupResources();   /* slot  3 */
    virtual s32  Behavior();           /* slot  6 */
    virtual s32  Render();             /* slot  9 */
    virtual void OnPendingDestroy();   /* slot 12 */
    virtual int  OnYoshiTryEat();      /* slot 18 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daShl_c_size_must_be_0x3e0[sizeof(daShl_c) == 0x3e0 ? 1 : -1];
#endif

#endif /* DASHL_C_H */
