#ifndef STARSWITCH_H
#define STARSWITCH_H

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

struct StarSwitch : Platform {
    s32 unk_320;                      /* 0x320 */
    s32 unk_324;                      /* 0x324 */
    s32 unk_328;                      /* 0x328 */
    u8  pad_32c[0x8];
    s32 unk_334;                      /* 0x334 */
    s16 unk_338;                      /* 0x338 */
    u16 unk_33a;                      /* 0x33a */
    s32 unk_33c;                      /* 0x33c */
    u8  pad_340[0x4];
    u32 mTargetActorID;               /* 0x344 */
    s32 unk_348;                      /* 0x348 */
    u8 unk_34c;                       /* 0x34c */
    u8 unk_34d;                       /* 0x34d */
    u8 unk_34e;                       /* 0x34e */
    u8 unk_34f;                       /* 0x34f */
    u8 unk_350;                       /* 0x350 */
    u8 unk_351;                       /* 0x351 */
    u8  pad_352[0x1];
    s8 unk_353;                       /* 0x353 */

    /* --- vtable --- */
    virtual ~StarSwitch();

    int Behavior();
    int CleanupResources();
    int Render();
};

typedef char StarSwitch_size_must_be_0x354[sizeof(StarSwitch) == 0x354 ? 1 : -1];

#endif /* __cplusplus */

#endif /* STARSWITCH_H */
