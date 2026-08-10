#ifndef MOVINGBARSMALL_H
#define MOVINGBARSMALL_H

#include "types.h"
#include "Platform.h"
#include "ShadowModel.h"

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

struct MovingBarSmall : Platform {
    u8  pad_31e[0x2];
    ShadowModel mShadowModel;         /* 0x320 */
    u8  pad_348[0x30];
    s32 unk_378;                      /* 0x378 */
    s32 unk_37c;                      /* 0x37c */
    s32 unk_380;                      /* 0x380 */
    s32 unk_384;                      /* 0x384 */
    s32 unk_388;                      /* 0x388 */
    s32 unk_38c;                      /* 0x38c */
    u8 unk_390;                       /* 0x390 */
    u8 unk_391;                       /* 0x391 */
    u8 unk_392;                       /* 0x392 */

    /* --- vtable --- */
    virtual ~MovingBarSmall();

    int CleanupResources();
    int Render();
};

typedef char MovingBarSmall_size_must_be_0x394[sizeof(MovingBarSmall) == 0x394 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct MovingBarSmall {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x2];
    s32 unk_098;            /* 0x098 */
    u8  pad_09c[0x4];
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x28];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mShadowModel;            /* 0x320 */
    u8  pad_321[0x57];
    s32 unk_378;            /* 0x378 */
    s32 unk_37c;            /* 0x37c */
    s32 unk_380;            /* 0x380 */
    s32 unk_384;            /* 0x384 */
    s32 unk_388;            /* 0x388 */
    s32 unk_38c;            /* 0x38c */
    u8  unk_390;            /* 0x390 */
    u8  unk_391;            /* 0x391 */
    u8  unk_392;            /* 0x392 */
};

#endif /* __cplusplus */

#endif /* MOVINGBARSMALL_H */
