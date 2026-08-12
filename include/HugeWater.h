#ifndef HUGEWATER_H
#define HUGEWATER_H

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

struct HugeWater : Platform {
    u8  pad_31e[0x2];
    TextureTransformer mTextureTransformer;/* 0x320 */

    /* --- vtable --- */
    virtual ~HugeWater();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char HugeCover_size_must_be_0x334[sizeof(HugeWater) == 0x334 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct HugeWater {
    u8  pad_000[0xd4];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad stopped
       short of the object, so the member also takes over unk_0dc (+0x8 = data), which the
       header declared separately inside it. */
    Model mModel;            /* 0x0d4 */
    u8  mMovingMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mTextureTransformer;            /* 0x320 */
    u8  pad_321[0xb];
    s32 unk_32c;            /* 0x32c */
};

#endif /* __cplusplus */

#endif /* HUGEWATER_H */
