#ifndef TTC_MOVINGBEAM_H
#define TTC_MOVINGBEAM_H

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
#include "ShadowModel.h"

struct TTC_MovingBeam : Platform {
    u8  pad_31e[0x2];
    s32 unk_320;                      /* 0x320 */
    s32 unk_324;                      /* 0x324 */
    u8 unk_328;                       /* 0x328 */
    u8  pad_329[0x7];
    s32 unk_330;                      /* 0x330 */
    ShadowModel mShadowModel;         /* 0x334 */

    /* --- vtable --- */
    virtual ~TTC_MovingBeam();

    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char TtcMovingCubeA_size_must_be_0x35c[sizeof(TTC_MovingBeam) == 0x35c ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct TTC_MovingBeam {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x10];
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x28];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* MovingMeshCollider member, named by the class's own destructor calling
       MovingMeshCollider's D1 at +0x124 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN14TTC_MovingBeamD1Ev.c] */
    MovingMeshCollider mMeshCollider;            /* 0x124 */
    u8  unk_2ec;            /* 0x2ec */
    u8  pad_2ed[0x33];
    s32 unk_320;            /* 0x320 */
    s32 unk_324;            /* 0x324 */
    u8  unk_328;            /* 0x328 */
    u8  pad_329[0x7];
    s32 unk_330;            /* 0x330 */
    u8  mShadowModel;            /* 0x334 */
};

#endif /* __cplusplus */

#endif /* TTC_MOVINGBEAM_H */
