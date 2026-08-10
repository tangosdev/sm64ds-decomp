#ifndef FORTRESSWALL_H
#define FORTRESSWALL_H

#include "types.h"
#include "Platform.h"

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

struct FortressWall : Platform {
    u8 unk_31e;                       /* 0x31e */
    u8 unk_31f;                       /* 0x31f */
    u8  pad_320[0x1];
    u8 unk_321;                       /* 0x321 */
    u8 unk_322;                       /* 0x322 */

    /* --- vtable --- */
    virtual ~FortressWall();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char FortressWall_size_must_be_0x324[sizeof(FortressWall) == 0x324 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct FortressWall {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    /* Actor::mPosX -- Actor.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 unk_05c;            /* 0x05c */
    /* Actor::mPosY -- Actor.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 unk_060;            /* 0x060 */
    /* 0x064..0x08e is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 unk_064;                 /* 0x064 */
    s32 unk_068;                 /* 0x068 */
    s32 unk_06c;                 /* 0x06c */
    s32 unk_070;                 /* 0x070 */
    s32 mCamSpacePosX;           /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x3c];
    u8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1f9];
    u8  unk_31e;            /* 0x31e */
    u8  unk_31f;            /* 0x31f */
    u8  pad_320[0x1];
    u8  unk_321;            /* 0x321 */
    u8  unk_322;            /* 0x322 */
};

#endif /* __cplusplus */

#endif /* FORTRESSWALL_H */
