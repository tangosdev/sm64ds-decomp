/* class UpDownLiftBbh: 4 matched functions.
 *
 * 0x000..0x0d4 is the fBase_c -> dBase_c -> dActor_c layout written out flat --
 * this struct does not derive from any of them -- so the names in that range are
 * COPIED from include/fBase_c.h and include/dActor_c.h at the matching offset,
 * not independently evidenced here. Nothing can shadow: there is no base class.
 *
 * FOUR SLOTS IN THAT RANGE ARE DELIBERATELY STILL unk_, and the reason is worth
 * reading before someone "finishes the job":
 *
 *   0x010, 0x011, 0x012   fBase_c does not name them either. Copying a name
 *                         across would be inventing one, not importing one.
 *   0x092, 0x096          These are dActor_c's mPrevAngleX and mPrevAngleZ
 *                         slots, and this actor reads them as UNSIGNED shaft
 *                         measurements in InitResources:
 *                           mBottomY = mTopY - (unk_092 << 12)
 *                           mTopY    = mPosY + (unk_096 << 12)   (variant only)
 *                         Nothing matched WRITES either slot, so which of the
 *                         two readings is right -- the base's snapshot, or an
 *                         actor-specific reuse of the same words -- is not
 *                         settled by any body that reproduces the cartridge.
 *                         Naming them either way would pick a side on no
 *                         evidence, so they keep the offset name.
 *
 * unk_346 and unk_349 are the actor's own and are still unk_ for the ordinary
 * reason (write-only in matched code), but unk_349 carries one observation that
 * should not be lost: InitResources stores only 0 or 1 into it -- 1 for
 * actorID 0x83 -- and then, four statements later, tests it for `== 2`. That
 * branch (the one that raises mTopY by unk_096) is unreachable in the shipped
 * ROM. It is reproduced as written because the cartridge contains it.
 */
#ifndef UPDOWNLIFTBBH_H
#define UPDOWNLIFTBBH_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

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
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

typedef char UpDownLiftBbh_size_must_be_0x34c[sizeof(struct UpDownLiftBbh) == 0x34c ? 1 : -1];

#endif
