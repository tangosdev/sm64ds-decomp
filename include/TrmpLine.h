#ifndef TRMPLINE_H
#define TRMPLINE_H

#include "types.h"

/* TrmpLine, one stylus-drawn trampoline of Trampoline and Trampoline2: the
 * 0x32c-byte slot the TrmpLine_* helpers in src/minigames/unit020cd744.cpp
 * build, flex and draw (the scenes construct three or four). The name is coined.
 * The constructor and destructor are the class's own, out of line in that file
 * (ROM 0x020d100c and 0x020d1008): the constructor marks the slot free, the
 * destructor is empty. The other helpers are free functions over a char* slot
 * and read it through their TRMPLINE() cast. Unnamed regions are touched in one
 * place at most. mAlpha is a byte, but the fade states step it with the int
 * ApproachLinear2 through an (int *) cast of its address. */
struct TrmpLine {
    TrmpLine();
    ~TrmpLine();

    /* One patch vertex. Plain data on purpose: Vector3 declares a constructor
       and a destructor, and an array of it makes TrmpLine's constructor and
       destructor call __cxa_vec_ctor / __cxa_vec_cleanup over the three patch
       arrays, where the cartridge's are a single store and an empty body
       (MEASURED: 0x80 and 0x54 bytes against 0xc and 0x4). */
    struct Vtx {
        Fix12i x, y, z;
    };

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
    Vtx mVerts[16];         /* 0x05c -- the 4x4 patch drawn */
    Vtx mRestVerts[16];     /* 0x11c -- the patch at rest; the flex states start from it */
    Vtx mNormals[16];       /* 0x1dc -- per-vertex normals, TrmpLine_ComputeNormals */
    s32 mFlexAmplitude[16]; /* 0x29c -- per-vertex flex depth: TrmpLine_PointDistance sets
                               it; the flex states scale the deflection by it and decay it */
    s32 mPackedNormals[16]; /* 0x2dc -- mNormals packed 10:10:10 for REG_NORMAL */
    s16 mLifeTimer;         /* 0x31c -- 0x258 frames, then the line fades */
    s16 mSlotIndex;         /* 0x31e -- index in sTrmpLines; TrmpLine_Create stores it and
                               the start-line builders store 3. TrmpLine_Draw uses it + 1
                               as the polygon ID; StateFlex never fades slot 3 */
    u16 mWobblePhase;       /* 0x320 -- sine phase of the flex */
    u16 mWobbleSpeed;       /* 0x322 -- added to mWobblePhase each frame */
    u16 mBouncesLeft;       /* 0x324 -- at 0 the line settles */
    u16 mBounceCount;       /* 0x326 -- picks TrmpLine_Draw's diffuse/ambient colour */
    u8 mStatus;             /* 0x328 -- 0 free; 1 flexing; 3 settling */
    u8 mAlpha;              /* 0x329 -- 0x1f on build; TrmpLine_Draw writes it to the
                               polygon alpha bits, the fade states step it to 0 */
    u8 pad_32a[0x2];        /* 0x32a */
};

#ifndef SM64DS_PLATFORM_PC
typedef char TrmpLine_Vtx_size_must_be_0xc[sizeof(TrmpLine::Vtx) == 0xc ? 1 : -1];
typedef char TrmpLine_size_must_be_0x32c[sizeof(TrmpLine) == 0x32c ? 1 : -1];
#endif

#endif
