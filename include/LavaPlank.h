#ifndef FLOATINGFLOORLLLBIG_H
#define FLOATINGFLOORLLLBIG_H

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

struct FloatingFloorLllBig : Platform {
    u8  pad_31e[0x2];
    s32 unk_320;                      /* 0x320 */
    s16 unk_324;                      /* 0x324 */

    /* --- vtable --- */
    virtual ~FloatingFloorLllBig();

    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char FloatingFloorLllBig_size_must_be_0x328[sizeof(FloatingFloorLllBig) == 0x328 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct FloatingFloorLllBig {
    u8  pad_000[0x60];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x28];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    s32 unk_320;            /* 0x320 */
    s16 unk_324;            /* 0x324 */
};

#endif /* __cplusplus */

#endif /* FLOATINGFLOORLLLBIG_H */
