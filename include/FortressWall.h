#ifndef FORTRESSWALL_H
#define FORTRESSWALL_H

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

struct FortressWall : dBgActor_c {
    /* Which of the two file sets this wall uses: 0 for the breakable fortress
       wall (actorID 0x30), 1 for every other id. It indexes the three ov079
       SharedFilePtr tables -- model, KCL and CLPS block -- in InitResources,
       and the first two again in CleanupResources. */
    u8 mVariant;                      /* 0x31e */
    /* param1 & 0xff, with 0xff read as 0. Behavior hands it to
       dActor_c::Spawn(0xb2, mStarId | 0x40, ...); actor 0xb2 is the star and its
       spawn word is starID | (howToSpawnStar << 4)
       (src/_ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h.cpp), so the low
       nibble this contributes is a star index. */
    u8 mStarId;                       /* 0x31f */
    u8  pad_320[0x1];
    /* Kill() sets this on the breakable wall instead of destroying it. Render
       draws nothing while it is set, and Behavior runs the break-and-spawn
       sequence only while it is set. */
    u8 mBroken;                       /* 0x321 */
    /* The state cell Sound::PlaySecretSound(dActor_c *, u16 *) advances; Behavior
       spawns the star and marks the wall for destruction on the frame that call
       returns nonzero. u16 because that is the parameter type -- 0x323 was the
       struct's tail padding. */
    u16 mBreakSoundState;             /* 0x322 */

    /* --- vtable --- */
    virtual ~FortressWall();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    virtual int OnHitByCannonBlastedChar(dActor_c &other); /* slot 26 */
    /* dBgActor_c's own slot, overridden here: _ZTV12FortressWall+0x7c relocates to
       0x02127280 while _ZTV10dBgActor_c+0x7c relocates to _ZN10dBgActor_c4KillEv. An
       override, so it adds no slot and no field. */
    virtual void Kill();              /* slot 31 */
};

typedef char FortressWall_size_must_be_0x324[sizeof(FortressWall) == 0x324 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct FortressWall {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
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
    u8  pad_090[0x3c];
    u8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~FortressWall calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    /* dBgActor_c's own collision matrix, the Matrix4x3 InitResources passes to
       dBgW_KcMbg::SetFile. A u8 marker, the idiom the other twins in this family
       use for a member whose type this flat spelling does not need. */
    u8  mClsnMat;            /* 0x2ec */
    u8  pad_2ed[0x31];
    u8  mVariant;            /* 0x31e */
    u8  mStarId;            /* 0x31f */
    u8  pad_320[0x1];
    u8  mBroken;            /* 0x321 */
    u16 mBreakSoundState;            /* 0x322 */
};

#endif /* __cplusplus */

#endif /* FORTRESSWALL_H */
