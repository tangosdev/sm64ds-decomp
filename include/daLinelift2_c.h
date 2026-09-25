#ifndef DALINELIFT2_C_H
#define DALINELIFT2_C_H

#include "types.h"
#include "dBgActor_c.h"
#include "PathPtr.h"

/* A lift that rides a level path (profiles KM2_SUSUMU and KM3_LIFT, both
 * in ov091). ROM RTTI daLinelift2_c; direct base dBgActor_c. Both
 * factories allocate 0x358 and construct PathPtr at +0x344, and
 * mIsPressed's padding closes exactly on it.
 *
 * The vtable at ov091:0x02134ec0 is 32 slots, the same as dBgActor_c's,
 * and differs from it only in the slots declared below.
 *
 * mState indexes a three-entry pointer-to-member table that ov091's static
 * initializer fills: 0 waits for a rider, 1 runs forward along the path,
 * 2 turns round and runs back.
 *
 * Historical aliases: RotatingUpDownPlatform (the class),
 * SquareMetalNetLift_Spawn and ArrowPathLift_Spawn (the two factories).
 */
struct daLinelift2_c : dBgActor_c {
    s32 mState;                       /* 0x320 */
    s32 mNodeCount;                   /* 0x324 */
    s32 mNodeIndex;                   /* 0x328 */
    s32 mBasePosX;                    /* 0x32c -- the node just left */
    s32 mBasePosY;                    /* 0x330 */
    s32 mBasePosZ;                    /* 0x334 */
    s32 mTargetPosX;                  /* 0x338 -- the node being headed for */
    s32 mTargetPosY;                  /* 0x33c */
    s32 mTargetPosZ;                  /* 0x340 */
    PathPtr mPathPtr;                 /* 0x344 */
    s32 mSinkOffsetY;                 /* 0x34c */
    s16 mBaseAngleY;                  /* 0x350 */
    u8  mVariant;                     /* 0x352 -- param1 bits 8..15: resource set */
    u8  pad_353;
    u16 mStateTimer;                  /* 0x354 */
    u8  mIsPressed;                   /* 0x356 -- set by the mesh callback */
    u8  pad_357;

    /* Inline is load-bearing: out-of-line emits D0 before D1. */
    virtual ~daLinelift2_c() {}

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daLinelift2_c_size_must_be_0x358[
    sizeof(daLinelift2_c) == 0x358 ? 1 : -1];
#endif

#endif /* DALINELIFT2_C_H */
