#ifndef DABTN_C_H
#define DABTN_C_H

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN7daBtn_cD1Ev` destroys each member, and
 * `daBtn_c_classInit_BATAN` constructs the same types at the same offsets before
 * storing `_ZTV7daBtn_c`. Everything this header used to restate below
 * 0x110 belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 dBgCh_Actr               0x1bc   -> 0x2cc
 *     0x2cc ModelAnim                  0x64    -> 0x330
 *     0x330 TextureSequence            0x14    -> 0x344
 *     0x344 ShadowModel                0x28    -> 0x36c
 *     0x418 dBgW_KcMbg         0x1c8   -> 0x5e0
 *
 * Typing them absorbed markers that were their insides:
 *   - unk_338 = TextureSequence.currFrame
 *   - unk_33c = TextureSequence.speed
 *
 * Field provenance: notes/enemy-leaf-provenance.md.
 *
 * SIZE IS THE ROM'S OWN: `daBtn_c_classInit_BATAN` calls
 * `fBase_c::operator new(1552)` -- 0x610 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 *
 * SM64DS RTTI names the implementation daBtn_c. The reconstructed
 * factory daBtn_c_classInit_BATAN (historical alias
 * Whomp_Spawn) constructs it for the BATAN
 * registry profile.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dBgW_KcMbg.h"
#include "ShadowModel.h"
#include "TextureSequence.h"
#include "dBgCh_Actr.h"

struct daBtn_c : dEnemyBase_c {
    dBgCh_Actr                 mWithMeshClsn;         /* 0x110 */
    ModelAnim                    mModelAnim;            /* 0x2cc */
    TextureSequence              mTextureSequence;      /* 0x330 */
    ShadowModel                  mShadowModel;          /* 0x344 */
    /* The drop shadow's matrix: func_ov079_02124188 builds it every frame
       and hands it to dActor_c::DropShadowScaleXYZ. */
    s32                          mShadowMatrix[12];     /* 0x36c */
    /* InitResources zeroes all four and then refills as many as data_0209f21c
       says from data_0209f394, the live player list. func_ov079_02123f34
       measures each one's distance and bearing. */
    Player                      *mPlayers[4];           /* 0x39c */
    /* The player whose touch counted as a hit this frame: func_ov079_02125058
       records it when func_ov002_020dd8b8 accepts the contact. The king's
       lying-down handler bursts a particle at it, and the defeat talk
       (state 8) is addressed to it. */
    Player                      *mPounder;              /* 0x3ac */
    /* Behavior's state index: it selects the per-state handler out of the
       pointer-to-member table data_ov079_02128280, and the king path skips its
       camera-target update while it reads 9. */
    s32                          mState;                /* 0x3b0 */
    /* The state a hit interrupted (func_ov079_02123804 saves it), put back by
       the wobble (state 10) and by the get-up in state 6. */
    s32                          mResumeState;          /* 0x3b4 */
    /* func_ov079_02123f34: the bearing to the nearest watched player. */
    s16                          mTargetAngle;          /* 0x3b8 */
    u8  pad_3ba[0x2];
    /* InitResources copies mPosX/mPosY/mPosZ here once. */
    s32                          mSpawnPosX;            /* 0x3bc */
    s32                          mSpawnPosY;            /* 0x3c0 */
    s32                          mSpawnPosZ;            /* 0x3c4 */
    /* Added to mAngleX every frame while the body tips over (states 4 and 11)
       or rises again (state 6). */
    s32                          mPitchSpeed;           /* 0x3c8 */
    s32                          unk_3cc;               /* 0x3cc -- zeroed with mPitchSpeed, no reader */
    s32                          unk_3d0;               /* 0x3d0 -- zeroed with mPitchSpeed, no reader */
    /* Behavior snapshots the live position into these every frame UNLESS
       dEnemyBase_c::IsGoingOffCliff says the next step leaves the ledge, in which
       case it restores the position FROM them instead. */
    s32                          mSafePosX;             /* 0x3d4 */
    s32                          mSafePosY;             /* 0x3d8 */
    s32                          mSafePosZ;             /* 0x3dc */
    /* The model's first bone's rotation, read back every frame by
       func_ov079_02124188; the collision mesh is posed with it. */
    s16                          mBodyPitch;            /* 0x3e0 */
    s16                          mBodyYaw;              /* 0x3e2 */
    s16                          mBodyRoll;             /* 0x3e4 */
    /* InitResources copies mAngleX/mAngleY/mAngleZ here once. */
    s16                          mSpawnAngleX;          /* 0x3e6 */
    s16                          mSpawnAngleY;          /* 0x3e8 */
    s16                          mSpawnAngleZ;          /* 0x3ea */
    /* func_ov079_02123f34: index into mPlayers of the nearest player, of the
       nearest one in front (within 0x2000 of the facing), and their distances. */
    s32                          mNearestPlayer;        /* 0x3ec */
    s32                          mFrontPlayer;          /* 0x3f0 */
    s32                          mNearestDist;          /* 0x3f4 */
    s32                          mFrontDist;            /* 0x3f8 */
    /* A second frame counter the lying-down state (6) runs its phases on. */
    u16                          mSubTimer;             /* 0x3fc */
    /* mStateTimer saved when a hit interrupts a state, restored on resume. */
    u16                          mSavedStateTimer;      /* 0x3fe */
    /* Frames left in the hit wobble (state 10); set to 90 by the hit. */
    u8                           mWobbleTimer;          /* 0x400 */
    /* Ground-pounds left: 3 for the king, 1 otherwise. */
    u8                           mHitPoints;            /* 0x401 */
    /* A ground-pound landed this frame (func_ov079_02125058). */
    u8                           mPounded;              /* 0x402 */
    /* A player touched the body this frame; cleared by InitResources AND by
       the last statement of every Behavior frame. */
    u8                           mTouched;              /* 0x403 */
    /* Render draws nothing at all while this is 0; InitResources sets it to 1. */
    u8                           mShouldRender;         /* 0x404 */
    /* Set once the current touch has paid out a coin; the lying-down
       handlers clear it. */
    u8                           mCoinGiven;            /* 0x405 */
    /* Coins the body still has to give: 4 for the king, 6 otherwise. */
    u8                           mCoinsLeft;            /* 0x406 */
    u8                           unk_407;               /* 0x407 -- zeroed by InitResources */
    u8                           unk_408;               /* 0x408 -- dActor_c::TrackStar(mStarID, 2)'s result */
    /* param1 & 0xf, handed straight to dActor_c::TrackStar as the star number.
       King only -- the plain daBtn_c never touches it. */
    u8                           mStarID;               /* 0x409 */
    /* The footstep sound for this stride has played. */
    u8                           mStepPlayed;           /* 0x40a */
    /* A hit interrupted a state and mResumeState is waiting to be put back. */
    u8                           mResumePending;        /* 0x40b */
    /* Phase within the current state; zeroed whenever mState changes. */
    u8                           mSubState;             /* 0x40c */
    /* mSubState saved alongside mSavedStateTimer. */
    u8                           mSavedSubState;        /* 0x40d */
    u8  pad_40e[0x2];
    /* Height added to the model and collision-mesh position as the body
       tips over (states 4 and 11, capped) and eased back while it lies (6). */
    s32                          mTipLift;              /* 0x410 */
    u8                           mIsKing;               /* 0x414 */
    u8  pad_415[0x3];
    dBgW_KcMbg           mMovingMeshCollider;   /* 0x418 */
    /* The collision mesh's matrix, rebuilt every frame by func_ov079_02124008
       from the model's; the words at 9..11 are its translation. */
    s32                          mMeshMatrix[12];       /* 0x5e0 */

    /* --- vtable --- */
    /* Inline is load-bearing: out-of-line emits D0 before D1. */
    virtual ~daBtn_c() {}

    virtual s32   OnAimedAtWithEgg();      /* slot 29 */
    virtual Vector3 OnAimedAtWithEggReturnVec();       /* slot 30 */

    virtual void OnHitByMegaChar(Player &player);      /* slot 27 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daBtn_c_size_must_be_0x610[sizeof(daBtn_c) == 0x610 ? 1 : -1];
#endif

#endif /* DABTN_C_H */
