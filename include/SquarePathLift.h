#ifndef SQUAREPATHLIFT_H
#define SQUAREPATHLIFT_H

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

struct SquarePathLift : Platform {
    u8  pad_31e[0x2];
    u8 mPath;                         /* 0x320 */
    u8  pad_321[0x7];
    s32 mNodeIndex;                   /* 0x328 */
    s32 mPathDir;                     /* 0x32c */

    /* --- vtable --- */
    virtual ~SquarePathLift();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char SquarePathLift_size_must_be_0x330[sizeof(SquarePathLift) == 0x330 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct SquarePathLift {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x8];
    s32 mMoveSpeed;            /* 0x098 */
    u8  pad_09c[0x38];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mPath;            /* 0x320 */
    u8  pad_321[0x7];
    s32 mNodeIndex;            /* 0x328 */
    s32 mPathDir;            /* 0x32c */
};

#endif /* __cplusplus */

#endif /* SQUAREPATHLIFT_H */
