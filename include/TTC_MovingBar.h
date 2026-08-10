#ifndef TTC_MOVINGBAR_H
#define TTC_MOVINGBAR_H

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

struct TTC_MovingBar : Platform {
    u8 unk_31e;                       /* 0x31e */
    u8  pad_31f[0x1];
    s32 unk_320;                      /* 0x320 */
    ShadowModel mShadowModel;         /* 0x324 */

    /* --- vtable --- */
    virtual ~TTC_MovingBar();

    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char TTC_MovingBar_size_must_be_0x34c[sizeof(TTC_MovingBar) == 0x34c ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct TTC_MovingBar {
    u8  pad_000[0xc];
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1f9];
    u8  unk_31e;            /* 0x31e */
    u8  pad_31f[0x1];
    s32 unk_320;            /* 0x320 */
    u8  mShadowModel;            /* 0x324 */
};

#endif /* __cplusplus */

#endif /* TTC_MOVINGBAR_H */
