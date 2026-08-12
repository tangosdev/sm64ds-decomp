#ifndef WDW_WATER_H
#define WDW_WATER_H

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

struct WDW_Water : Platform {
    u8  pad_31e[0x2];
    TextureTransformer mTextureTransformer;/* 0x320 */
    s32 mTargetPosY;                  /* 0x334 */
    u8 unk_338;                       /* 0x338 */
    u8  pad_339[0x3];
    s32 unk_33c;                      /* 0x33c */
    u8 unk_340;                       /* 0x340 */
    u8  pad_341[0x3];
    s32 unk_344;                      /* 0x344 */

    /* --- vtable --- */
    virtual ~WDW_Water();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char RotatingPlatformWdw_size_must_be_0x348[sizeof(WDW_Water) == 0x348 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct WDW_Water {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x54];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x8];
    s32 unk_06c;            /* 0x06c */
    u8  pad_070[0x1e];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x3c];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad stopped
       short of the object, so the member also takes over unk_0dc (+0x8 = data), which the
       header declared separately inside it. */
    Model mModel;            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mTextureTransformer;            /* 0x320 */
    u8  pad_321[0xb];
    s32 unk_32c;            /* 0x32c */
    u8  pad_330[0x4];
    s32 mTargetPosY;            /* 0x334 */
    u8  unk_338;            /* 0x338 */
    u8  pad_339[0x3];
    s32 unk_33c;            /* 0x33c */
    u8  unk_340;            /* 0x340 */
    u8  pad_341[0x3];
    s32 unk_344;            /* 0x344 */
};

#endif /* __cplusplus */

#endif /* WDW_WATER_H */
