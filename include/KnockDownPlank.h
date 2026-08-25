#ifndef KNOCKDOWNPLANK_H
#define KNOCKDOWNPLANK_H

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

struct KnockDownPlank : dBgActor_c {
    u8  pad_31e[0x2];
    ShadowModel mShadowModel;         /* 0x320 */
    u8  pad_348[0x30];
    /* The point 0x32000 units in front of the plank: InitResources rotates
       (0, 0, 0x32000) by mAngleY through data_020a0e68 and adds the plank's own
       position. mFrontFloorY below is the ground height under it. */
    s32 mFrontPosX;                   /* 0x378 */
    s32 mFrontPosY;                      /* 0x37c */
    s32 mFrontPosZ;                      /* 0x380 */
    s32 mFrontFloorY;                      /* 0x384 */
    s32 mOriginalPosY;                      /* 0x388 */
    s32 mJumpSpeed;                      /* 0x38c */
    u16 mWobbleAng;                      /* 0x390 */
    s16 mFallAngVel;                      /* 0x392 */
    s16 mWobbleTimer;                      /* 0x394 */
    s8 mKnockDir;                       /* 0x396 */
    u8 mState;                       /* 0x397 */
    /* KnockDownPlank_Spawn, the one factory storing _ZTV14KnockDownPlank
       (ov015:0x02114420), calls fBase_c::operator new(0x39c). The field span
       stopping at 0x398 is a lower bound, not the size. */
    u8 pad_398[0x4];                  /* 0x398, to the ROM's 0x39c */

    /* --- vtable --- */
    virtual ~KnockDownPlank();

    int InitResources();
    int CleanupResources();
    int Render();

    virtual int  OnAttacked2(dActor_c &other);       /* slot 23 */
    virtual void OnKicked(dActor_c &other);          /* slot 24 */
    virtual void OnHitByMegaChar(Player &player);     /* slot 27 */
};

typedef char KnockDownPlank_size_must_be_0x39c[sizeof(KnockDownPlank) == 0x39c ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct KnockDownPlank {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~KnockDownPlank calls _ZN10dBgW_KcMbgD1Ev
       at +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    ShadowModel mShadowModel; /* 0x320 */
    u8  pad_348[0x30];
    s32 mFrontPosX;            /* 0x378 */
    s32 mFrontPosY;            /* 0x37c */
    s32 mFrontPosZ;            /* 0x380 */
    s32 mFrontFloorY;            /* 0x384 */
    s32 mOriginalPosY;            /* 0x388 */
    s32 mJumpSpeed;            /* 0x38c */
    u16 mWobbleAng;            /* 0x390 */
    s16 mFallAngVel;            /* 0x392 */
    s16 mWobbleTimer;            /* 0x394 */
    s8  mKnockDir;            /* 0x396 */
    u8  mState;            /* 0x397 */
};

#endif /* __cplusplus */

#endif /* KNOCKDOWNPLANK_H */
