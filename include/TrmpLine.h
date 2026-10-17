#ifndef TRMPLINE_H
#define TRMPLINE_H

#include "types.h"

/* TrmpLine, one stylus-drawn trampoline of Trampoline and Trampoline2: the
 * 0x32c-byte slot the TrmpLine_* helpers in src/minigames/unit020cd744.cpp
 * build, flex and draw (the scenes construct three or four). The name is coined.
 * A layout view only: the helpers are free functions over a char* slot and read
 * it through their TRMPLINE() cast. Unnamed regions are touched in one place at
 * most; +0x329 is read as an int by ApproachLinear2 and stays an offset. */
struct TrmpLine {
    s32 mState[2];          /* 0x000 -- the current state's member-pointer record */
    Vector3 mCenter;        /* 0x008 -- midpoint of the drawn stroke */
    Vector3 mNormal;        /* 0x014 -- copied into a jumper's mContactNormal */
    Vector3 mAxis;          /* 0x020 -- unit vector along the stroke */
    Vector3 mScale;         /* 0x02c -- 0x1000 on build */
    Vector3 mContactPoint;  /* 0x038 -- where the last jumper met the line */
    Vector3 mContactMotion; /* 0x044 -- that jumper's motion across it */
    s16 mStart[2];          /* 0x050 -- stroke start, screen x/y */
    s16 mEnd[2];            /* 0x054 -- stroke end */
    s32 mHalfWidth;         /* 0x058 -- half the stroke's length */
    Vector3 mVerts[16];     /* 0x05c -- the 4x4 patch drawn */
    Vector3 mRestVerts[16]; /* 0x11c -- the patch at rest; the flex states start from it */
    u8 pad_1dc[0x140];      /* 0x1dc */
    s16 mLifeTimer;         /* 0x31c -- 0x258 frames, then the line fades */
    u8 pad_31e[0x2];        /* 0x31e */
    u16 mWobblePhase;       /* 0x320 -- sine phase of the flex */
    u16 mWobbleSpeed;       /* 0x322 -- added to mWobblePhase each frame */
    u16 mBouncesLeft;       /* 0x324 -- at 0 the line settles */
    u16 mBounceCount;       /* 0x326 */
    u8 mStatus;             /* 0x328 -- 0 free; 3 settling */
    u8 pad_329[0x3];        /* 0x329 */
};

#ifndef SM64DS_PLATFORM_PC
typedef char TrmpLine_size_must_be_0x32c[sizeof(TrmpLine) == 0x32c ? 1 : -1];
#endif

#endif
