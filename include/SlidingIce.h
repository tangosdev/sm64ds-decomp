#ifndef SLIDINGICE_H
#define SLIDINGICE_H

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
 *
 * SM64DS RTTI names the implementation daObjSlIceBlock_c. The reconstructed factory
 * daObjSlIceBlock_c_classInit_SL_ICEBLOCK_SHOT (historical alias SlidingIce_Spawn) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_SL_ICEBLOCK_SHOT
 * (historical alias SlidingIce_SpawnInfo) is its registry descriptor.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"

struct SlidingIce : dBgActor_c {
    s16 mDelayTimer;                  /* 0x31e -- DecIfAbove0_Short; both variants of Behavior do nothing until it reaches 0 */
    s8 mNumToBigIce;                       /* 0x320 */
    u8  pad_321[0x3];
    s32 mMinPosY;                      /* 0x324 */
    s32 mSoundID;                      /* 0x328 */

    /* --- vtable --- */
    virtual ~SlidingIce();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    virtual void OnHitByMegaChar(Player &player);   /* slot 27 */
};

typedef char SlidingIce_size_must_be_0x32c[sizeof(SlidingIce) == 0x32c ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct SlidingIce {
    u8  pad_000[0xc];
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x2];
    s32 mHorzSpeed;            /* 0x098 */
    u8  pad_09c[0x30];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~SlidingIce calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base.
       The marker's pad stopped short of the object, so the member also takes over
       unk_170 (+0x4c = the dBgW_Kc base's unk_4c), which the header declared
       separately inside it. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x32];
    s16 mDelayTimer;            /* 0x31e */
    s8  mNumToBigIce;            /* 0x320 */
    u8  pad_321[0x3];
    s32 mMinPosY;            /* 0x324 */
    s32 mSoundID;            /* 0x328 */
};

#endif /* __cplusplus */

#endif /* SLIDINGICE_H */
