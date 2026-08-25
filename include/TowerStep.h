#ifndef TOWERSTEP_H
#define TOWERSTEP_H

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
#include "ShadowModel.h"

struct TowerStep : dBgActor_c {
    u8  pad_31e[0x2];
    ShadowModel mShadowModel;         /* 0x320 */
    u8  pad_348[0x30];
    s32 mFloorPosY;                      /* 0x378 */
    s32 mMinPosY;                      /* 0x37c */
    s32 mMaxPosY;                      /* 0x380 */
    s32 mShadowOffsetX;                      /* 0x384 */
    s32 mShadowOffsetY;                      /* 0x388 */
    s32 mSoundID;                      /* 0x38c */
    u8 mMoveTimer;                       /* 0x390 */
    u8 mJustSteppedOn;                       /* 0x391 */
    u8 mMove;                       /* 0x392 */

    /* --- vtable --- */
    virtual ~TowerStep();

    int CleanupResources();
    int Behavior();
    int Render();
    virtual void OnHitByMegaChar(Player &player);   /* slot 27 */
    /* dBgActor_c's own slot, overridden here: _ZTV9TowerStep+0x7c relocates to
       0x0211233c while _ZTV10dBgActor_c+0x7c relocates to _ZN10dBgActor_c4KillEv. An
       override, so it adds no slot and no field. */
    virtual void Kill();              /* slot 31 */
};

typedef char TowerStep_size_must_be_0x394[sizeof(TowerStep) == 0x394 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct TowerStep {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x2];
    s32 mHorzSpeed;            /* 0x098 */
    u8  pad_09c[0x4];
    s32 mTerminalVelocity;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;            /* 0x0a8 */
    u8  pad_0ac[0x28];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~TowerStep calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    ShadowModel mShadowModel; /* 0x320 */
    u8  pad_348[0x30];
    s32 mFloorPosY;            /* 0x378 */
    s32 mMinPosY;            /* 0x37c */
    s32 mMaxPosY;            /* 0x380 */
    s32 mShadowOffsetX;            /* 0x384 */
    s32 mShadowOffsetY;            /* 0x388 */
    s32 mSoundID;            /* 0x38c */
    u8  mMoveTimer;            /* 0x390 */
    u8  mJustSteppedOn;            /* 0x391 */
    u8  mMove;            /* 0x392 */
};

#endif /* __cplusplus */

#endif /* TOWERSTEP_H */
