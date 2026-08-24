#ifndef STARSWITCH_H
#define STARSWITCH_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"

struct StarSwitch : dBgActor_c {
    u8  pad_31e[0x2];
    s32 mDrawScaleX;                      /* 0x320 */
    s32 mDrawScaleY;                      /* 0x324 */
    s32 mDrawScaleZ;                      /* 0x328 */
    u8  pad_32c[0x8];
    s32 mMusicVolume;                      /* 0x334 */
    u16 mTimer;                      /* 0x338 */
    u16 mTimeLimit;                      /* 0x33a */
    s32 mSwitchType;                      /* 0x33c */
    s32 unk_340;                      /* 0x340 */
    u32 mTargetActorID;               /* 0x344 */
    s32 mTargetActor;                      /* 0x348 */
    u8 mResourceIdx;                       /* 0x34c */
    u8 mMusicFadeDone;                       /* 0x34d */
    u8 mEventBit;                       /* 0x34e */
    u8 unk_34f;                       /* 0x34f */
    u8 unk_350;                       /* 0x350 */
    u8 unk_351;                       /* 0x351 */
    u8  pad_352[0x1];
    s8 mHomeAreaId;                       /* 0x353 */

    /* --- vtable --- */
    virtual ~StarSwitch();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    void OnGroundPounded(dActor_c &other);
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
    /* 0x060..0x08e is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
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
    /* 0x0b0..0x0cc is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
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
    /* dBgW_KcMbg member. The cartridge's own ~StarSwitch calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    s32 mDrawScaleX;            /* 0x320 */
    s32 mDrawScaleY;            /* 0x324 */
    s32 mDrawScaleZ;            /* 0x328 */
    u8  pad_32c[0x8];
    s32 mMusicVolume;            /* 0x334 */
    u16 mTimer;            /* 0x338 */
    u16 mTimeLimit;            /* 0x33a */
    s32 mSwitchType;            /* 0x33c */
    s32 unk_340;            /* 0x340 */
    u32 mTargetActorID;            /* 0x344 */
    s32 mTargetActor;            /* 0x348 */
    u8  mResourceIdx;            /* 0x34c */
    u8  mMusicFadeDone;            /* 0x34d */
    u8  mEventBit;            /* 0x34e */
    u8  unk_34f;            /* 0x34f */
    u8  unk_350;            /* 0x350 */
    u8  unk_351;            /* 0x351 */
    u8  pad_352[0x1];
    s8  mHomeAreaId;            /* 0x353 */
};

#endif /* __cplusplus */

#endif /* STARSWITCH_H */
