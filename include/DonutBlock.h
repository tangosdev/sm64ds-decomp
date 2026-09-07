#ifndef DONUTBLOCK_H
#define DONUTBLOCK_H

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
#include "dBgCh_Actr.h"

struct DonutBlock : dBgActor_c {
    u8  pad_31e[0x2];
    dBgCh_Actr mWithMeshClsn;       /* 0x320 */
    s32 mHomePosX;                    /* 0x4dc -- InitResources copies mPosX/Y/Z; state 1 restores the block from them */
    s32 mHomePosY;                    /* 0x4e0 */
    s32 mHomePosZ;                    /* 0x4e4 */
    u8 mHadClsn;                       /* 0x4e8 */
    u8 mClsnTimer;                       /* 0x4e9 */
    u8 mState;                        /* 0x4ea */

    /* --- vtable --- */
    virtual ~DonutBlock();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();
};

typedef char DonutBlock_size_must_be_0x4ec[sizeof(DonutBlock) == 0x4ec ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct DonutBlock {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0xc];
    s32 mVertAccel;         /* 0x09c */
    s32 mTerminalVelocity;  /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;         /* 0x0a8 */
    u8  pad_0ac[0x28];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~DonutBlock calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x320 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10DonutBlockD1Ev.c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x320 */
    s32 mHomePosX;            /* 0x4dc */
    s32 mHomePosY;            /* 0x4e0 */
    s32 mHomePosZ;            /* 0x4e4 */
    u8  mHadClsn;            /* 0x4e8 */
    u8  mClsnTimer;            /* 0x4e9 */
    u8  mState;            /* 0x4ea */
};

#endif /* __cplusplus */

#endif /* DONUTBLOCK_H */
