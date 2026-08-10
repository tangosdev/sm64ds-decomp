#ifndef ICEBLOCK_H
#define ICEBLOCK_H

#include "types.h"
#include "Platform.h"
#include "MovingCylinderClsn.h"

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

struct IceBlock : Platform {
    u8  pad_31e[0x2];
    MovingCylinderClsn mMovingCylinderClsn;/* 0x320 */
    u8 unk_354;                       /* 0x354 */
    u8  pad_355[0x3];
    s32 mScale;                       /* 0x358 */
    u32 unk_35c;                      /* 0x35c */
    u32 unk_360;                      /* 0x360 */
    u8 unk_364;                       /* 0x364 */

    /* --- vtable --- */
    virtual ~IceBlock();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char IceBlock_size_must_be_0x368[sizeof(IceBlock) == 0x368 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct IceBlock {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN8IceBlockD1Ev.c] */
    Model mModel;            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mMovingCylinderClsn;            /* 0x320 */
    u8  pad_321[0x1f];
    s32 unk_340;            /* 0x340 */
    s32 unk_344;            /* 0x344 */
    u8  pad_348[0xc];
    u8  unk_354;            /* 0x354 */
    u8  pad_355[0x3];
    s32 mScale;            /* 0x358 */
    u32 unk_35c;            /* 0x35c */
    u32 unk_360;            /* 0x360 */
    u8  unk_364;            /* 0x364 */
};

#endif /* __cplusplus */

#endif /* ICEBLOCK_H */
