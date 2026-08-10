#ifndef CANNONHATCH_H
#define CANNONHATCH_H

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

struct CannonHatch : Platform {
    u8  pad_31e[0x2];
    s32 unk_320;                      /* 0x320 */
    s32 unk_324;                      /* 0x324 */
    s32 unk_328;                      /* 0x328 */
    u8 unk_32c;                       /* 0x32c */
    u8 unk_32d;                       /* 0x32d */
    u8 unk_32e;                       /* 0x32e */

    /* --- vtable --- */
    virtual ~CannonHatch();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char CannonHatch_size_must_be_0x330[sizeof(CannonHatch) == 0x330 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct CannonHatch {
    u8  pad_000[0x8];
    /* 0x008..0x05c is ActorBase's, and ActorBase.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    u32 mParam;                  /* 0x008 */
    u16 actorID;                 /* 0x00c */
    u8  aliveState;              /* 0x00e */
    u8  shouldBeKilled;          /* 0x00f */
    u8  unk_010;                 /* 0x010 */
    u8  unk_011;                 /* 0x011 */
    u8  unk_012;                 /* 0x012 */
    u8  unk_013;                 /* 0x013 */
    u8  sceneNode[0x14];               /* 0x014 */
    u8  behavNode[0x10];               /* 0x028 */
    u8  renderNode[0x10];              /* 0x038 */
    u8  pad_048[0x14];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* MovingMeshCollider member, named by the class's own destructor calling
       MovingMeshCollider's D1 at +0x124 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN11CannonHatchD1Ev.c] */
    MovingMeshCollider mMeshCollider;            /* 0x124 */
    u8  unk_2ec;            /* 0x2ec */
    u8  pad_2ed[0x33];
    s32 unk_320;            /* 0x320 */
    s32 unk_324;            /* 0x324 */
    s32 unk_328;            /* 0x328 */
    u8  unk_32c;            /* 0x32c */
    u8  unk_32d;            /* 0x32d */
    u8  unk_32e;            /* 0x32e */
};

#endif /* __cplusplus */

#endif /* CANNONHATCH_H */
