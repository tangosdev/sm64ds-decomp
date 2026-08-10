#ifndef CHAINCHOMPFENCE_H
#define CHAINCHOMPFENCE_H

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

struct ChainChompFence : Platform {
    u8 unk_31e;                       /* 0x31e */

    /* --- vtable --- */
    virtual ~ChainChompFence();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char ChainChompFence_size_must_be_0x320[sizeof(ChainChompFence) == 0x320 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct ChainChompFence {
    u8  pad_000[0x8e];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mMovingMeshCollider;            /* 0x124 */
    u8  pad_125[0x1f9];
    u8  unk_31e;            /* 0x31e */
};

#endif /* __cplusplus */

#endif /* CHAINCHOMPFENCE_H */
