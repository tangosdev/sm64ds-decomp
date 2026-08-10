#ifndef ARROWSIGNRIGHT_H
#define ARROWSIGNRIGHT_H

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

struct ArrowSignRight : Platform {
    u8  pad_31e[0x2];
    ShadowModel mShadowModel;         /* 0x320 */
    u8 unk_348;                       /* 0x348 */
    u8  pad_349[0x33];
    u8 unk_37c;                       /* 0x37c */

    /* --- vtable --- */
    virtual ~ArrowSignRight();

    int Behavior();
    int CleanupResources();
    int Render();
};

typedef char ArrowSignRight_size_must_be_0x380[sizeof(ArrowSignRight) == 0x380 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct ArrowSignRight {
    u8  pad_000[0xc];
    u16 unk_00c;            /* 0x00c */
    u8  pad_00e[0x80];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x320 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN14ArrowSignRightD1Ev.c] */
    ShadowModel mShadowModel;            /* 0x320 */
    u8  unk_348;            /* 0x348 */
    u8  pad_349[0x33];
    u8  unk_37c;            /* 0x37c */
};

#endif /* __cplusplus */

#endif /* ARROWSIGNRIGHT_H */
