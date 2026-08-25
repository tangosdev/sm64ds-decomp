/* Seeded from matched-function evidence by tools/gen_header.py, then named by
 * hand. Offsets/widths are observed, not guessed. Gaps are explicit padding.
 *
 * class IceSlideManager: 4 matched functions.
 *
 * 0x000..0x0d4 is dActor_c's layout, written out flat here rather than
 * inherited -- this struct does not derive from dActor_c, so the names in that
 * range are COPIED from include/dActor_c.h at the matching offset, not
 * independently evidenced. That is also why they cannot shadow anything: there
 * is no base class here to shadow.
 *
 * Only 0x0d4 and 0x0d6 are the actor's own, and both are witnessed:
 *
 *   0x0d4  mKillTimer   InitResources seeds it with 0x78 (120 frames); Behavior
 *                       runs it down with DecIfAbove0_Short and, at zero, plays
 *                       one more sound and calls
 *                       dActor_c::KillAndTrackInDeathTable. Armed once, expires
 *                       once, and its expiry IS the kill.
 *   0x0d6  mState       Behavior switches on it over exactly {0, 1}. State 0
 *                       waits for DistToCPlayer() < 0x180000, plays a sound and
 *                       increments it; state 1 is the countdown above. A
 *                       two-state machine, not a flag: the increment is spelt
 *                       through a byte pointer at this + 0xd6.
 */
#ifndef ICESLIDEMANAGER_H
#define ICESLIDEMANAGER_H
#include "types.h"

struct IceSlideManager {
    u8  pad_000[0x5c];
    /* dActor_c::mPosX -- dActor_c.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 mPosX;            /* 0x05c */
    /* dActor_c::mPosY -- dActor_c.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 mPosY;            /* 0x060 */
    /* 0x064..0x0d4 is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 mPosZ;                 /* 0x064 */
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
    s16 mPrevAngleX;             /* 0x092 */
    s16 mPrevAngleY;             /* 0x094 */
    s16 mPrevAngleZ;             /* 0x096 */
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
    s16 mKillTimer;            /* 0x0d4 */
    u8  mState;            /* 0x0d6 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

typedef char IceSlideManager_size_must_be_0xd8[sizeof(struct IceSlideManager) == 0xd8 ? 1 : -1];

#endif
