#ifndef STARSWITCH_H
#define STARSWITCH_H

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

struct StarSwitch : Platform {
    u8  pad_31e[0x2];
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

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct StarSwitch {
    u8  pad_000[0x8];
    u32 mParam;            /* 0x008 */
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x52];
    /* 0x060..0x08e is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 unk_060;                 /* 0x060 */
    s32 mPosZ;                   /* 0x064 */
    s32 unk_068;                 /* 0x068 */
    s32 unk_06c;                 /* 0x06c */
    s32 unk_070;                 /* 0x070 */
    s32 mCamSpacePosX;           /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x20];
    /* 0x0b0..0x0cc is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    u32 unk_0b0;                 /* 0x0b0 */
    s32 unk_0b4;                 /* 0x0b4 */
    s32 unk_0b8;                 /* 0x0b8 */
    s32 unk_0bc;                 /* 0x0bc */
    s32 unk_0c0;                 /* 0x0c0 */
    u8  unk_0c4;                 /* 0x0c4 */
    u8  pad_0c5[0x7];
    u8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    s32 unk_320;            /* 0x320 */
    s32 unk_324;            /* 0x324 */
    s32 unk_328;            /* 0x328 */
    u8  pad_32c[0x8];
    s32 unk_334;            /* 0x334 */
    s16 unk_338;            /* 0x338 */
    u16 unk_33a;            /* 0x33a */
    s32 unk_33c;            /* 0x33c */
    u8  pad_340[0x4];
    u32 mTargetActorID;            /* 0x344 */
    s32 unk_348;            /* 0x348 */
    u8  unk_34c;            /* 0x34c */
    u8  unk_34d;            /* 0x34d */
    u8  unk_34e;            /* 0x34e */
    u8  unk_34f;            /* 0x34f */
    u8  unk_350;            /* 0x350 */
    u8  unk_351;            /* 0x351 */
    u8  pad_352[0x1];
    s8  unk_353;            /* 0x353 */
};

#endif /* __cplusplus */

#endif /* STARSWITCH_H */
