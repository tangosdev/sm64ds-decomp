#ifndef TRMPITEM_H
#define TRMPITEM_H

#include "types.h"
#include "dExtSimpleModel_c.h"
#include "dExtAnmTexSRT_c.h"
#include "Vector2.h"

/* TrmpItem, one of Trampoline2's floating rings (its three models are
 * ASSET_RINGA/B/C_BMD, sTrmpItemRing*ModelFile): the 0x1d0-byte record the
 * TrmpItem_* helpers in src/minigames/unit020cd744.cpp spawn, move and draw
 * (the scene builds ten). The name is coined. A layout view only: the helpers
 * are free functions over a char* record and read it through their TRMPITEM()
 * cast. mMotionKind picks the motion (TrmpItem_MotionStill .. SpinDrift);
 * unnamed regions are written in one place only, or sit inside an embedded
 * object. */
struct TrmpItem {
    TrmpItem();
    ~TrmpItem();

    s32 mState[2];        /* 0x000 -- the current motion's member-pointer record */
    Vector3 mPos;         /* 0x008 */
    Vector3 mPrevPos;     /* 0x014 -- mPos before this frame's move */
    Vector3 mSpawnPos;    /* 0x020 -- written once, in TrmpItem_Init, from the spawn position */
    Vector3 mVel;         /* 0x02c -- added to mPos every frame */
    Vector3 mForward;     /* 0x038 -- facing; the spin motions turn it */
    Vector3 mSide;        /* 0x044 -- mForward x the spin axis */
    Vector3 mPrevForward; /* 0x050 */
    Vector3 mPrevSide;    /* 0x05c */
    Vector3 mScale;       /* 0x068 -- grows in TrmpItem_GrowScale */
    s32 mRotation[4];     /* 0x074 -- quaternion built from mForward */
    s32 mActive;          /* 0x084 -- updated and drawn only while set */
    s32 mAppearScale;     /* 0x088 -- grows to 0x800 as the item appears */
    u8 pad_08c[0xa];      /* 0x08c */
    s16 mSizeIdx;         /* 0x096 -- row of the speed, range and hit-width tables */
    s16 mMotionKind;      /* 0x098 -- 0 still .. 8 spin-drift */
    u16 mPhase;           /* 0x09a -- the sway and hover motions' phase */
    u8 mAlpha;            /* 0x09c -- opacity, applied in TrmpItem_UpdateMatrix */
    u8 pad_09d[0x3];      /* 0x09d */
    /* Three members, not an array: the constructor builds them with three
       calls and the destructor tears them down with three more, where an
       array member goes through __cxa_vec_cleanup (MEASURED). */
    dExtSimpleModel_c mRingA;     /* 0x0a0 */
    dExtSimpleModel_c mRingB;     /* 0x0f0 */
    dExtSimpleModel_c mRingC;     /* 0x140 */
    dExtSimpleModel_c *mModel;    /* 0x190 -- the variant drawn */
    dExtAnmTexSRT_c mTexAnim;     /* 0x194 */
    /* Five Vector2s: the constructor builds them and the destructor tears
       them down through the type's arm9 structors (func_0203d738 /
       NullDestructor_0203d47c), but nothing in either trampoline TU reads or
       writes them, so the role, and the name, are unknown. */
    Vector2 mUnusedVec[5]; /* 0x1a8 */
};

#ifndef SM64DS_PLATFORM_PC
typedef char TrmpItem_size_must_be_0x1d0[sizeof(TrmpItem) == 0x1d0 ? 1 : -1];
#endif

#endif
