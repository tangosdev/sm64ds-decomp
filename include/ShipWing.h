#ifndef SHIPWING_H
#define SHIPWING_H

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
#include "WithMeshClsn.h"

struct ShipWing : Platform {
    u8  pad_31e[0x2];
    WithMeshClsn mWithMeshClsn;       /* 0x320 */
    s32 unk_4dc;                      /* 0x4dc */
    s32 unk_4e0;                      /* 0x4e0 */
    s32 unk_4e4;                      /* 0x4e4 */
    u8 unk_4e8;                       /* 0x4e8 */
    u8 unk_4e9;                       /* 0x4e9 */
    u8 mState;                        /* 0x4ea */

    /* --- vtable --- */
    virtual ~ShipWing();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char ShipWing_size_must_be_0x4ec[sizeof(ShipWing) == 0x4ec ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct ShipWing {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0xc];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x28];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    /* WithMeshClsn member, named by the class's own destructor calling
       WithMeshClsn's D1 at +0x320 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN8ShipWingD1Ev.c] */
    WithMeshClsn mWithMeshClsn;            /* 0x320 */
    s32 unk_4dc;            /* 0x4dc */
    s32 unk_4e0;            /* 0x4e0 */
    s32 unk_4e4;            /* 0x4e4 */
    u8  unk_4e8;            /* 0x4e8 */
    u8  unk_4e9;            /* 0x4e9 */
    u8  mState;            /* 0x4ea */
};

#endif /* __cplusplus */

#endif /* SHIPWING_H */
