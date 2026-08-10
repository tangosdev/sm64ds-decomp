#ifndef FLOATINGFLOORLLLBIG_H
#define FLOATINGFLOORLLLBIG_H

#include "types.h"
#include "Platform.h"

/* Derives from Platform: the destructor stores this class's vtable, then
 * Platform's -- inlined -- and then destroys the MovingMeshCollider at 0x124
 * and the Model at 0xd4 before chaining to Actor. Those three are Platform's
 * and are declared there; everything this header used to restate below 0x31e
 * was Actor's and Platform's.
 *
 * Any field at 0x31e or 0x31f sits in Platform's tail padding, which the
 * Itanium ABI lets a derived class use because Platform is non-POD: its last
 * field ends at 0x31e and its size rounds to 0x320.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up -- not independent evidence. It
 * is a regression guard on this declaration, not a claim about the ROM.
 */

#ifdef __cplusplus

struct FloatingFloorLllBig : Platform {
    s32 unk_320;                      /* 0x320 */
    s16 unk_324;                      /* 0x324 */

    /* --- vtable --- */
    virtual ~FloatingFloorLllBig();

    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char FloatingFloorLllBig_size_must_be_0x328[sizeof(FloatingFloorLllBig) == 0x328 ? 1 : -1];

#endif /* __cplusplus */

#endif /* FLOATINGFLOORLLLBIG_H */
