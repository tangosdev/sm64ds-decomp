#ifndef SLIDINGICE_H
#define SLIDINGICE_H

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

struct SlidingIce : Platform {
    s16 unk_31e;                      /* 0x31e */
    s8 unk_320;                       /* 0x320 */
    u8  pad_321[0x3];
    s32 unk_324;                      /* 0x324 */
    s32 unk_328;                      /* 0x328 */

    /* --- vtable --- */
    virtual ~SlidingIce();

    int Behavior();
    int CleanupResources();
    int Render();
};

typedef char SlidingIce_size_must_be_0x32c[sizeof(SlidingIce) == 0x32c ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct SlidingIce {
    u8  pad_000[0xc];
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x2];
    s32 unk_098;            /* 0x098 */
    u8  pad_09c[0x30];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x4b];
    s8  unk_170;            /* 0x170 */
    u8  pad_171[0x1ad];
    s16 unk_31e;            /* 0x31e */
    s8  unk_320;            /* 0x320 */
    u8  pad_321[0x3];
    s32 unk_324;            /* 0x324 */
    s32 unk_328;            /* 0x328 */
};

#endif /* __cplusplus */

#endif /* SLIDINGICE_H */
