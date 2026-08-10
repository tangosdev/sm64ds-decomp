#ifndef BIGBRICKBLOCK_H
#define BIGBRICKBLOCK_H

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

struct BigBrickBlock : Platform {
    u8  unk_31e;                      /* 0x31e - in Platform's tail padding */
    u8  unk_31f;                      /* 0x31f */
    u8  mEventID;                     /* 0x320 */
    u8  pad_321[0x3];
    Actor *mSwitch;                   /* 0x324 */

    /* --- vtable --- */
    virtual ~BigBrickBlock();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char BigBrickBlock_size_must_be_0x328[sizeof(BigBrickBlock) == 0x328 ? 1 : -1];

#endif /* __cplusplus */

#endif /* BIGBRICKBLOCK_H */
