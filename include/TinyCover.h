#ifndef TINYCOVER_H
#define TINYCOVER_H

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
#include "TextureTransformer.h"

struct TinyCover : Platform {
    u8  pad_31e[0x2];
    TextureTransformer mTextureTransformer;/* 0x320 */
    s32 unk_334;                      /* 0x334 */
    u8  pad_338[0x4];
    u8 unk_33c;                       /* 0x33c */

    /* --- vtable --- */
    virtual ~TinyCover();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char TinyCover_size_must_be_0x340[sizeof(TinyCover) == 0x340 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct TinyCover {
    u8  pad_000[0x60];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x2a];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad stopped
       short of the object, so the member also takes over unk_0dc (+0x8 = data), which the
       header declared separately inside it. */
    Model mModel;            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    /* TextureTransformer member, named by the class's own destructor calling
       TextureTransformer's D1 at +0x320 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN9TinyCoverD1Ev.c] */
    TextureTransformer mTextureTransformer;            /* 0x320 */
    s32 unk_334;            /* 0x334 */
    u8  pad_338[0x4];
    u8  unk_33c;            /* 0x33c */
};

#endif /* __cplusplus */

#endif /* TINYCOVER_H */
