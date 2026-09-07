/* The three up/down lifts in BBH, HMC and RR share this implementation and
 * _ZTV13UpDownLiftBbh. Their factories allocate 0x34c bytes, call
 * dBgActor_c::dBgActor_c(), and install this class's vtable.
 *
 * The destructor proves the hierarchy and ownership independently: it changes
 * the vptr from UpDownLiftBbh to dBgActor_c, destroys dBgActor_c's
 * dBgW_KcMbg and Model members, then chains to dActor_c. In C++ those are all
 * consequences of deriving from dBgActor_c; the source destructor is empty.
 *
 * Two inherited angle slots have an unusual actor-specific reading worth
 * preserving: InitResources treats mPrevAngleX and mPrevAngleZ as UNSIGNED
 * shaft measurements:
 *                           mBottomY = mTopY - (unk_092 << 12)
 *                           mTopY    = mPosY + (unk_096 << 12)   (variant only)
 * Nothing matched writes either slot, so the base names remain provisional for
 * this use even though the physical inheritance is proven.
 *
 * unk_346 and unk_349 are the actor's own and are still unk_ for the ordinary
 * reason (write-only in matched code), but unk_349 carries one observation that
 * should not be lost: InitResources stores only 0 or 1 into it -- 1 for
 * actorID 0x83 -- and then, four statements later, tests it for `== 2`. That
 * branch (the one that raises mTopY by unk_096) is unreachable in the shipped
 * ROM. It is reproduced as written because the cartridge contains it.
 *
 * SM64DS proves this class as daUdlift_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings below are reconstructed
 * source-style names -- evidence-bounded proposals, not recovered SM64DS
 * symbols.
 *
 * daUdlift_c_classInit_UDLIFT_TERESA at 0x021367fc (historical alias
 * UpDownLiftBbh_Spawn) allocates 0x34c and installs this class's cartridge
 * vtable. It backs the UDLIFT_TERESA registry profile, whose descriptor at
 * 0x021375cc is reconstructed as g_profile_UDLIFT_TERESA.
 */
#ifndef UPDOWNLIFTBBH_H
#define UPDOWNLIFTBBH_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

#ifdef __cplusplus

#include "dBgActor_c.h"

struct UpDownLiftBbh : dBgActor_c {
    /* dBgActor_c's last member ends at 0x31e. Keep its two bytes of tail
       padding so the first word owned by this class starts at 0x320. */
    u8 pad_31e[0x2];
    Player *mRider;                 /* 0x320 */
    Player *mClosestPlayer;         /* 0x324 */
    s32 mVariant;                   /* 0x328 */
    s32 mState;                     /* 0x32c */
    s32 mPlayerPosY;                /* 0x330 */
    s32 mTopY;                      /* 0x334 */
    s32 mBottomY;                   /* 0x338 */
    s32 mMiddleY;                   /* 0x33c */
    u32 mSoundHandle;               /* 0x340 */
    u16 mStateTimer;                /* 0x344 */
    u8 unk_346;                     /* 0x346 */
    u8 mIsArmed;                    /* 0x347 */
    u8 mIsRidden;                   /* 0x348 */
    u8 unk_349;                     /* 0x349 */

    /* Declared first so the destructor TUs, rather than an ordinary virtual
       method TU, remain the key-function/vtable owners. */
    virtual ~UpDownLiftBbh();
    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

#else

/* Flat C spelling retained for any future C consumer. */
struct UpDownLiftBbh {
    u8  pad_000[0xc];
    u16 actorID;                 /* 0x00c */
    u8  aliveState;              /* 0x00e */
    u8  shouldBeKilled;          /* 0x00f */
    u8  unk_010;                 /* 0x010 */
    u8  unk_011;                 /* 0x011 */
    u8  unk_012;                 /* 0x012 */
    u8  pauseFlags;                 /* 0x013 */
    u8  sceneNode[0x14];               /* 0x014 */
    u8  behavNode[0x10];               /* 0x028 */
    u8  renderNode[0x10];              /* 0x038 */
    u8  pad_048[0x18];
    s32 mPosY;                 /* 0x060 */
    s32 mPosZ;                   /* 0x064 */
    s32 mPrevPosX;                 /* 0x068 */
    s32 mPrevPosY;                 /* 0x06c */
    s32 mPrevPosZ;                 /* 0x070 */
    s32 mCamSpacePosX;           /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;                 /* 0x08e */
    s16 mAngleZ;                 /* 0x090 */
    s16 mPrevAngleX;                 /* 0x092 */
    s16 mPrevAngleY;             /* 0x094 */
    s16 mPrevAngleZ;                 /* 0x096 */
    s32 mHorzSpeed;              /* 0x098 */
    s32 mVertAccel;              /* 0x09c */
    s32 mTerminalVelocity;       /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;              /* 0x0a8 */
    u8  pad_0ac[0x4];
    u32 mFlags;                  /* 0x0b0 */
    s32 mClipOffsetY;                 /* 0x0b4 */
    s32 mClipRadius;                 /* 0x0b8 */
    s32 mClipDistance;                 /* 0x0bc */
    s32 mFarDistance;                 /* 0x0c0 */
    u8  mClipResult;                 /* 0x0c4 */
    u8  pad_0c5[0x7];
    s8  mAreaId;                 /* 0x0cc */
    u8  pad_0cd[0x1];
    s16 mDeathTableID;                 /* 0x0ce */
    u8  pad_0d0[0x4];
    Model mModel;            /* 0x0d4 */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x14];
    u16 unk_300;            /* 0x300 */
    u8  pad_302[0x1e];
    s32 mRider;            /* 0x320 */
    s32 mClosestPlayer;            /* 0x324 */
    s32 mVariant;            /* 0x328 */
    s32 mState;            /* 0x32c */
    s32 mPlayerPosY;            /* 0x330 */
    /* The heights the lift works between. Every access in the ROM is a 32-bit
       one; the generated header spelled these u8 plus padding. */
    s32 mTopY;            /* 0x334 */
    s32 mBottomY;            /* 0x338 */
    s32 mMiddleY;            /* 0x33c */
    u32 mSoundHandle;            /* 0x340 */
    u16 mStateTimer;            /* 0x344 */
    u8  unk_346;            /* 0x346 */
    u8  mIsArmed;            /* 0x347 */
    u8  mIsRidden;            /* 0x348 */
    u8  unk_349;            /* 0x349 */
};

#endif /* __cplusplus */

typedef char UpDownLiftBbh_size_must_be_0x34c[sizeof(struct UpDownLiftBbh) == 0x34c ? 1 : -1];

#endif
