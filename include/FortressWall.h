#ifndef FORTRESSWALL_H
#define FORTRESSWALL_H

#include "types.h"
#include "Platform.h"

/* Derives from Platform: the destructor stores this class's vtable, then
 * Platform's -- inlined -- then destroys the MovingMeshCollider at 0x124 and
 * the Model at 0xd4 before chaining to Actor. All three are Platform's.
 * Everything this header used to restate below 0x31e was Actor's and
 * Platform's.
 *
 * THE FIELDS AT 0x31e/0x31f ARE IN PLATFORM'S TAIL PADDING. Platform's last
 * field ends at 0x31e and its size rounds to 0x320; the Itanium ABI lets a
 * derived class use a non-POD base's tail padding, and the ROM does. The bytes
 * settle it -- code here reads this+0x31e and reproduces.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

struct FortressWall : Platform {
    u8  unk_31e;                      /* 0x31e - in Platform's tail padding */
    u8  unk_31f;                      /* 0x31f */
    u8  pad_320[0x1];
    u8  unk_321;                      /* 0x321 */
    u8  unk_322;                      /* 0x322 */

    /* --- vtable --- */
    virtual ~FortressWall();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char FortressWall_size_must_be_0x324[sizeof(FortressWall) == 0x324 ? 1 : -1];

#endif /* __cplusplus */

#endif /* FORTRESSWALL_H */
