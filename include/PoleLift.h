#ifndef POLELIFT_H
#define POLELIFT_H

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
#include "MovingCylinderClsn.h"

struct PoleLift : Platform {
    u8  pad_31e[0x2];
    MovingCylinderClsn mMovingCylinderClsn;/* 0x320 */
    u16 unk_354;                      /* 0x354 */

    /* --- vtable --- */
    virtual ~PoleLift();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char FireSeaElevator_size_must_be_0x358[sizeof(PoleLift) == 0x358 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct PoleLift {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0x82];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mMovingMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    /* MovingCylinderClsn member, named by the class's own destructor calling
       MovingCylinderClsn's D1 at +0x320 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN8PoleLiftD1Ev.c] */
    MovingCylinderClsn mMovingCylinderClsn;            /* 0x320 */
    u16 unk_354;            /* 0x354 */
};

#endif /* __cplusplus */

#endif /* POLELIFT_H */
