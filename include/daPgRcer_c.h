#ifndef DAPGRCER_C_H
#define DAPGRCER_C_H

#include "types.h"
#include "ModelAnim.h"
#include "TextureSequence.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"
#include "PathPtr.h"
#include "dActor_c.h"

struct Player;

/* Cool Cool Mountain racing penguin. RTTI ov019:0x021132d0 names daPgRcer_c;
 * overlay_actors maps ov019 RACING_PENGUIN (259). Direct base is dActor_c
 * (_ZTI word 0x0208e390). SIZE 0x398 is daPgRcer_c_classInit's allocation.
 *
 * Factory is `return new daPgRcer_c()`; fBase_c::operator new(unsigned long)
 * is inherited. daPgRcer_c_classInit is reconstructed (RTTI + PENGUIN_RACER
 * registry). Retail does not store that spelling.
 *
 * Tail fields are named from this TU's own writes: InitResources seeds the
 * path and TrackStar slot; the state table at 0x370 is the PMF pair
 * func_ov019_021122dc installs; the race helpers write the checkpoint /
 * finish / cheat bytes.
 */

struct daPgRcer_c;
typedef void (daPgRcer_c::*daPgRcerStateMethod)();

struct daPgRcerState {
    daPgRcerStateMethod init;
    daPgRcerStateMethod behavior;
};

#ifndef SM64DS_PLATFORM_PC
typedef char daPgRcerState_size_must_be_0x10[
    sizeof(daPgRcerState) == 0x10 ? 1 : -1];
#endif

struct daPgRcer_c : dActor_c {
    u8 pad_0d0[0x4];
    ModelAnim mModelAnim;             /* 0x0d4 */
    TextureSequence mTextureSequence; /* 0x138 */
    ShadowModel mShadowModel;         /* 0x14c */
    dCcAc_c mdCcAc_c;                 /* 0x174 */
    dBgCh_Actr mWithMeshClsn;         /* 0x1a8 */
    PathPtr mPath;                    /* 0x364 */
    s32 mPathNodeIndex;               /* 0x36c */
    daPgRcerState *mStateDesc;        /* 0x370 -- func_ov019_021122dc */
    s32 mAction;                      /* 0x374 -- state index 0..5 */
    Player *mTalkPlayer;              /* 0x378 */
    s32 mFallAccum;                   /* 0x37c -- |player vertSpeed| while airborne */
    s32 mPathDist;                    /* 0x380 -- remaining dist to current node */
    u32 mSlideSound;                  /* 0x384 -- Sound::PlayLong handle */
    u32 mDustParticle;                /* 0x388 -- Particle::System::New handle */
    s16 mTargetAngY;                  /* 0x38c */
    u8 mTalkWaitTimer;                /* 0x38e -- DecIfAbove0_Byte */
    u8 mActionStep;                   /* 0x38f */
    u8 mPenguinCheckpoints;           /* 0x390 */
    u8 mPlayerCheckpoints;            /* 0x391 */
    u8 mPenguinFinished;              /* 0x392 */
    u8 mPlayerFinished;               /* 0x393 */
    u8 mPlayerCheated;                /* 0x394 -- long fall or cheat volume */
    u8 mPenguinWon;                   /* 0x395 */
    u8 mStarSlot;                     /* 0x396 -- TrackStar result */
    u8 pad_397;                       /* 0x397 -- closes on 0x398 */

    /* Inline so mwcc emits retail D1 then D0 and no D2. Slots 16/17. */
    virtual ~daPgRcer_c() {}
    virtual int InitResources();      /* slot 0 */
    virtual int CleanupResources();   /* slot 3 */
    virtual int Behavior();           /* slot 6 */
    virtual int Render();             /* slot 9 */
    virtual void OnPendingDestroy();  /* slot 12 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daPgRcer_c_size_must_be_0x398[
    sizeof(daPgRcer_c) == 0x398 ? 1 : -1];
#endif

#endif
