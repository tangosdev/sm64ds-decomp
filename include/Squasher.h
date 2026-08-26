#ifndef SQUASHER_H
#define SQUASHER_H

#include "types.h"
#include "math/Matrix.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "ShadowModel.h"

struct Squasher : dBgActor_c {
    s16 mAngVelX;                      /* 0x31e */
    u16 mStateTimer;                  /* 0x320 -- ROM reads it with ldrh */
    u8 mState;                       /* 0x322 */
    u8  pad_323[0x1];
    ShadowModel mShadowModel;         /* 0x324 */

    /* --- vtable --- */
    virtual ~Squasher();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* Tail padding. The field span stops short of the real size: Squasher_Spawn
       calls fBase_c::operator new(0x37c), read off the retail
       instruction. A span is only a LOWER BOUND. */
    Matrix4x3 mShadowMat;        /* 0x34c */
};

typedef char Squasher_size_must_be_0x37c[sizeof(Squasher) == 0x37c ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct Squasher {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0xc];
    /* 0x074..0x08c is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 mCamSpacePosX;                 /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;            /* 0x08c */
    u16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member, named by the class's own destructor calling
       dBgW_KcMbg's D1 at +0x124 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN8SquasherD1Ev.c] */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  mClsnMat;            /* 0x2ec */
    u8  pad_2ed[0x31];
    s16 mAngVelX;            /* 0x31e */
    s16 mStateTimer;            /* 0x320 */
    u8  mState;            /* 0x322 */
    u8  pad_323[0x1];
    ShadowModel mShadowModel; /* 0x324 */
};

#endif /* __cplusplus */

#endif /* SQUASHER_H */
