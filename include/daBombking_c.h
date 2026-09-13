#ifndef DABOMBKING_C_H
#define DABOMBKING_C_H

#include "types.h"

/* Derives from dEnemyBase_c: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything this header used to restate below 0x110 belonged to the
 * chain above and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS RTTI names the implementation daBombking_c. The reconstructed
 * factory daBombking_c_classInit (historical alias
 * KingBobOmb_Spawn) constructs it for the BOMBKING
 * registry profile.
 */

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "BlendModelAnim.h"
#include "CommonModel.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct Player;

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daBombking_c : dEnemyBase_c {
    /* Declared first, DEFINED OUT OF LINE at the top of src/actors/daBombking_c.cpp.
     * Measured on this TU under `#pragma defer_codegen off`: an out-of-line
     * definition emits D1, D0, D2 in that order, which puts D1 (0x02123740) and
     * D0 (0x02123798) at the head of .text exactly as the cartridge has them.
     * An INLINE body emits D1 then D0 too, but at the END of the TU -- one
     * ordinal inversion (50, 0) and a hard linkcheck [4b/8] refusal.
     * D2 is homeless (the ROM has no D2) and is licensed as `deadstrip`. */
    virtual ~daBombking_c();

    dBgCh_Actr mWithMeshClsn;       /* 0x110 */
    BlendModelAnim mBlendModelAnim;   /* 0x2cc */
    dCcAcPos_c mdCcAcPos_c;/* 0x33c */
    dCcAcPos_c mdCcAcPos_c2;/* 0x37c */
    CommonModel mCommonModel;         /* 0x3bc */
    ShadowModel mShadowModel;         /* 0x3f8 */
    /* The reconstructed state dispatcher uses two member-pointer records.
       KingBobOmb_SetState invokes the first; Behavior invokes the second.
       The int return contract preserves the current dispatcher and does not
       establish the original signatures of all state handlers. */
    typedef int (daBombking_c::*StateFunction)();
    StateFunction *mState;            /* 0x420 */
    /* Unique ids of the two spawned bob-ombs. Init zeros both; func_ov078_02125350
       writes dActor_c::Spawn's uniqueID into a free slot; func_ov078_02123864
       walks them with FindWithID. */
    s32 mSpawnedId[2];                /* 0x424 */
    /* 1 after the throw anim has launched that slot's bob-omb. */
    u8  mSpawnedThrown[2];            /* 0x42c */
    u8  pad_42e[0x2];
    /* ClosestPlayer / the actor handed to ShowMessage and GetTalkState. */
    Player *mTalkingPlayer;           /* 0x430 */
    /* DropShadowRadHeight matrix: IDENTITY then translation = pos >> 3. */
    s32 mShadowMtx[12];               /* 0x434 */
    u8  pad_464[0x30];
    /* TryGrab / DropActor target. Render also tests its +0xc8 mtx pointer. */
    Player *mHeldActor;               /* 0x494 */
    u8  unk_498;                      /* 0x498 -- Init stores 0x1f */
    /* 0/1/2 substep of the current state. */
    u8  mActionStep;                  /* 0x499 */
    u8  pad_49a[0x2];
    /* Index into mSpawnedId / mSpawnedThrown. */
    s32 mSpawnSlot;                   /* 0x49c */
    s32 mPhase;                       /* 0x4a0 */
    /* Bone 0x1e0 * model mtx. Behavior points actor+0xc8 here while holding. */
    s32 mHoldMtx[12];                 /* 0x4a4 */
    s32 mArenaPosX;                   /* 0x4d4 */
    s32 mArenaPosY;                   /* 0x4d8 */
    s32 mArenaPosZ;                   /* 0x4dc */
    s32 mHomePosX;                    /* 0x4e0 */
    s32 mHomePosY;                    /* 0x4e4 */
    s32 mHomePosZ;                    /* 0x4e8 */
    /* Hand world pos, LSL'd from mHoldMtx translation. Copied onto a spawned
       bob-omb in func_ov078_021250f8. */
    s32 mThrowPosX;                   /* 0x4ec */
    s32 mThrowPosY;                   /* 0x4f0 */
    s32 mThrowPosZ;                   /* 0x4f4 */
    s16 mInitAngleY;                  /* 0x4f8 */
    s16 mTargetAngY;                  /* 0x4fa */
    s32 mAnimSpeed;                   /* 0x4fc */
    s32 mHealth;                      /* 0x500 */
    u8  unk_504;                      /* 0x504 -- DecIfAbove0_Byte; armed to 0x64 */
    u8  unk_505;                      /* 0x505 -- DecIfAbove0_Byte; armed to 5 / 0xf / 0x14 */
    /* 1 while fight BGM layer 3 is up. */
    u8  mMusicLayer3;                 /* 0x506 */
    u8  mStarTracked;                 /* 0x507 */
    u8  mIntroTalked;                 /* 0x508 */
    u8  mStarID;                      /* 0x509 */
    /* Counts while mMusicLayer3; resets the fight when it exceeds 0xc8. */
    u8  unk_50a;                      /* 0x50a */
    u8  pad_50b[0x1];

    /* --- vtable --- */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

/* ~daBombking_c, the key function, owns the compiler-emitted definition of this
 * vtable. The factory is `return new daBombking_c()`; the leaf size_t operator
 * new forwards to `_ZN7fBase_cnwEj` until #2570's fBase overload lands. */
extern int _ZTV12daBombking_c[];

typedef char daBombking_c_size_must_be_0x50c[sizeof(daBombking_c) == 0x50c ? 1 : -1];

#endif /* __cplusplus */

#endif /* DABOMBKING_C_H */
