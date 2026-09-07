#ifndef TTCROTATINGGEAR_H
#define TTCROTATINGGEAR_H

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
 * SM64DS RTTI names the implementation daObjCtKaitendai_c. The reconstructed
 * factory daObjCtKaitendai_c_classInit_CT_MECHA06 (historical alias
 * TtcRotatingGear_Spawn) constructs it for the CT_MECHA06
 * registry profile.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"

struct TtcRotatingGear : dBgActor_c {
    u8  pad_31e[0x2];
    s32 mHomePosX;                    /* 0x320 -- InitResources copies mPosX/Y/Z here */
    s32 mHomePosY;                    /* 0x324 -- Behavior clamps mPosY to [mHomePosY, mHomePosY + 0x14a000] */
    s32 mHomePosZ;                    /* 0x328 */
    u16 mMoveTimer;                   /* 0x32c -- DecIfAbove0_Short'ed; reloaded from the per-setting table on expiry */
    u8 mMoveDir;                      /* 0x32e */

    /* --- vtable --- */
    virtual ~TtcRotatingGear();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char TtcRotatingGear_size_must_be_0x330[sizeof(TtcRotatingGear) == 0x330 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct TtcRotatingGear {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x10];
    s32 mTerminalVelocity;  /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;         /* 0x0a8 */
    u8  pad_0ac[0x28];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~TtcRotatingGear calls _ZN10dBgW_KcMbgD1Ev
       at +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    s32 mHomePosX;            /* 0x320 */
    s32 mHomePosY;            /* 0x324 */
    s32 mHomePosZ;            /* 0x328 */
    u16 mMoveTimer;            /* 0x32c */
    u8  mMoveDir;            /* 0x32e */
};

#endif /* __cplusplus */

#endif /* TTCROTATINGGEAR_H */
