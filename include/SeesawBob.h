#ifndef SEESAWBOB_H
#define SEESAWBOB_H

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

struct SeesawBob : Platform {
    u8  pad_31e[0x2];
    s32 unk_320;                      /* 0x320 */
    s16 unk_324;                      /* 0x324 */
    u8 unk_326;                       /* 0x326 */

    /* --- vtable --- */
    virtual ~SeesawBob();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char SeesawBob_size_must_be_0x328[sizeof(SeesawBob) == 0x328 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct SeesawBob {
    u8  pad_000[0xc];
    u16 unk_00c;            /* 0x00c */
    u8  pad_00e[0x7e];
    s16 unk_08c;            /* 0x08c */
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x20];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x20];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    s32 unk_320;            /* 0x320 */
    s16 unk_324;            /* 0x324 */
    u8  unk_326;            /* 0x326 */
};

#endif /* __cplusplus */

#endif /* SEESAWBOB_H */
