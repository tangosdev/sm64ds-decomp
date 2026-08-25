#ifndef MOVINGBAR_H
#define MOVINGBAR_H

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

struct MovingBar : dBgActor_c {
    u8  pad_31e[0x2];
    s32 mHomePosX;                      /* 0x320 */
    s32 mHomePosY;                      /* 0x324 */
    s32 mHomePosZ;                      /* 0x328 */
    s32 mVariant;                     /* 0x32c */
    /* Both factories storing _ZTV9MovingBar (ov015:0x0211458c) -- MovingBarBig_Spawn
       and MovingBarSmall_Spawn -- call fBase_c::operator new(0x338). They agree, so
       they are two spawn-info variants of one actor, and 0x330 was the field span
       rather than the size. */
    u32 mState;              /* 0x330 */
    u8  pad_334[0x4];

    /* --- vtable --- */
    virtual ~MovingBar();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    virtual void OnHitByMegaChar(Player &player);   /* slot 27 */
    /* dBgActor_c's own slot, overridden here: _ZTV9MovingBar+0x7c relocates to
       0x02111c3c while _ZTV10dBgActor_c+0x7c relocates to _ZN10dBgActor_c4KillEv. An
       override, so it adds no slot and no field. */
    virtual void Kill();              /* slot 31 */
};

typedef char MovingBar_size_must_be_0x338[sizeof(MovingBar) == 0x338 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct MovingBar {
    u8  pad_000[0xc];
    /* 0x00c..0x05c is fBase_c's, and fBase_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    u16 mActorID;                 /* 0x00c */
    u8  aliveState;              /* 0x00e */
    u8  shouldBeKilled;          /* 0x00f */
    u8  unk_010;                 /* 0x010 */
    u8  unk_011;                 /* 0x011 */
    u8  unk_012;                 /* 0x012 */
    u8  pauseFlags;                 /* 0x013 */
    u8  sceneNode[0x14];               /* 0x014 */
    u8  behavNode[0x10];               /* 0x028 */
    u8  renderNode[0x10];              /* 0x038 */
    u8  pad_048[0x14];
    /* dActor_c::mPosX -- dActor_c.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 mPosX;            /* 0x05c */
    /* dActor_c::mPosY -- dActor_c.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 mPosY;            /* 0x060 */
    /* 0x064..0x08e is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 mPosZ;                 /* 0x064 */
    s32 mPrevPosX;                 /* 0x068 */
    s32 mPrevPosY;                 /* 0x06c */
    s32 mPrevPosZ;                 /* 0x070 */
    s32 mCamSpacePosX;           /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~MovingBar calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    s32 mHomePosX;            /* 0x320 */
    s32 mHomePosY;            /* 0x324 */
    s32 mHomePosZ;            /* 0x328 */
    s32 mVariant;            /* 0x32c */
    u32 mState;            /* 0x330 */
    u8  pad_334[0x4];
};

#endif /* __cplusplus */

#endif /* MOVINGBAR_H */
