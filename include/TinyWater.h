#ifndef TINYWATER_H
#define TINYWATER_H

#include "types.h"

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
#include "TextureTransformer.h"

struct TinyWater : dBgActor_c {
    u8  pad_31e[0x2];
    TextureTransformer mTextureTransformer;/* 0x320 */
    s32 mMinPosY;                      /* 0x334 */
    u8  pad_338[0x4];
    u16 mSoundTimer;              /* 0x33c */

    /* --- vtable --- */
    virtual ~TinyWater();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char TinyWater_size_must_be_0x340[sizeof(TinyWater) == 0x340 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct TinyWater {
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
       checks. Was a u8 marker. [_ZN9TinyWaterD1Ev.c] */
    TextureTransformer mTextureTransformer;            /* 0x320 */
    s32 mMinPosY;            /* 0x334 */
    u8  pad_338[0x4];
    u16 mSoundTimer;              /* 0x33c */
};

#endif /* __cplusplus */

#endif /* TINYWATER_H */
