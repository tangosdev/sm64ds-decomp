#ifndef TTC_MOVINGBAR_H
#define TTC_MOVINGBAR_H

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

struct TTC_MovingBar : dBgActor_c {
    u8  mVariant;                     /* 0x31e -- indexes both ov065 file tables, in InitResources and CleanupResources */
    u8  pad_31f[0x1];
    s32 mGroundY;                     /* 0x320 -- InitResources' raycast result, falling back to the probe height */
    ShadowModel mShadowModel;         /* 0x324 */

    /* --- vtable --- */
    virtual ~TTC_MovingBar() {}

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* Tail padding. The field span stops short of the real size: TtcRotatingGear_Spawn and TtcRotatingTriangle_Spawn
       call fBase_c::operator new(0x37c), read off the retail
       instruction. A span is only a LOWER BOUND. */
    u8 pad_34c[0x30];      /* 0x34c, to the ROM's 0x37c */
};

typedef char TTC_MovingBar_size_must_be_0x37c[sizeof(TTC_MovingBar) == 0x37c ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct TTC_MovingBar {
    u8  pad_000[0xc];
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~TTC_MovingBar calls _ZN10dBgW_KcMbgD1Ev
       at +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x32];
    u8  mVariant;            /* 0x31e */
    u8  pad_31f[0x1];
    s32 mGroundY;            /* 0x320 */
    u8  mShadowModel;            /* 0x324 */
};

#endif /* __cplusplus */

#endif /* TTC_MOVINGBAR_H */
