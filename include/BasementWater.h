#ifndef BASEMENTWATER_H
#define BASEMENTWATER_H

#include "types.h"
#include "dBgW_KcMbg.h"

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

struct BasementWater : dBgActor_c {
    u8  pad_31e[0x2];
    TextureTransformer mTextureTransformer;/* 0x320 */
    s32 mLoweredY;                    /* 0x334 */
    u32 mSoundID;                      /* 0x338 */
    u16 mSoundTimer;              /* 0x33c */
    u8 mWasJustDrained;                       /* 0x33e */

    /* --- vtable --- */
    virtual ~BasementWater() {}

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char BasementWater_size_must_be_0x340[sizeof(BasementWater) == 0x340 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct BasementWater {
    u8  pad_000[0x60];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x10];
    /* 0x074..0x08e is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 mCamSpacePosX;                 /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad stopped
       short of the object, so the member also takes over unk_0dc (+0x8 = data), which the
       header declared separately inside it. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~BasementWater calls _ZN10dBgW_KcMbgD1Ev
       at +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    TextureTransformer mTextureTransformer; /* 0x320 */
    s32 mLoweredY;            /* 0x334 */
    u32 mSoundID;            /* 0x338 */
    u16 mSoundTimer;              /* 0x33c */
    u8  mWasJustDrained;            /* 0x33e */
};

#endif /* __cplusplus */

#endif /* BASEMENTWATER_H */
