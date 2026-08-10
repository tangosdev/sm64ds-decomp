#ifndef SQUASHER_H
#define SQUASHER_H

#include "types.h"

/* Derives from Platform: the destructor stores this class's vtable, then
 * Platform's -- inlined -- then destroys the MovingMeshCollider at 0x124 and
 * the Model at 0xd4 before chaining to Actor. All three belong to Platform.
 * Everything this header used to restate below 0x31e was Actor's and
 * Platform's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

#include "Platform.h"
#include "ShadowModel.h"

struct Squasher : Platform {
    s16 unk_31e;                      /* 0x31e */
    s16 unk_320;                      /* 0x320 */
    u8 unk_322;                       /* 0x322 */
    u8  pad_323[0x1];
    ShadowModel mShadowModel;         /* 0x324 */

    /* --- vtable --- */
    virtual ~Squasher();

    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char Squasher_size_must_be_0x34c[sizeof(Squasher) == 0x34c ? 1 : -1];

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
    /* 0x074..0x08c is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 unk_074;                 /* 0x074 */
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
    /* MovingMeshCollider member, named by the class's own destructor calling
       MovingMeshCollider's D1 at +0x124 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN8SquasherD1Ev.c] */
    MovingMeshCollider mMeshCollider;            /* 0x124 */
    u8  unk_2ec;            /* 0x2ec */
    u8  pad_2ed[0x31];
    s16 unk_31e;            /* 0x31e */
    s16 unk_320;            /* 0x320 */
    u8  unk_322;            /* 0x322 */
    u8  pad_323[0x1];
    u8  mShadowModel;            /* 0x324 */
};

#endif /* __cplusplus */

#endif /* SQUASHER_H */
