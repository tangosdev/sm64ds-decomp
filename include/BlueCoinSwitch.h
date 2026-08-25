#ifndef BLUECOINSWITCH_H
#define BLUECOINSWITCH_H

#include "types.h"
#include "Model.h"
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

struct BlueCoinSwitch : dBgActor_c {
    u8  pad_31e[0x2];
    s32 mStopPosY;                      /* 0x320 */
    s32 mTickSound;                      /* 0x324 */
    u16 mCoinTimer;                      /* 0x328 */
    u16 mCoinTimerSeed;                      /* 0x32a */
    u8 mPressed;                       /* 0x32c */
    u8 mEventBit;                       /* 0x32d */
    u8 mHomeAreaId;                       /* 0x32e */

    /* --- vtable --- */
    virtual ~BlueCoinSwitch();

    s32 Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char BlueCoinSwitch_size_must_be_0x330[sizeof(BlueCoinSwitch) == 0x330 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct BlueCoinSwitch {
    u8  pad_000[0x8];
    /* Spawn word, unpacked into two fields by InitResources: bits 0-3 become
       the event bit mEventBit, bits 8-15 the timer seed mCoinTimerSeed. */
    u32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    /* The switch top, and it MOVES: once pressed, Behavior walks mPosY down by
       0x14000 a frame until it reaches mStopPosY. Render draws the model only
       while mPosY is still above mStopPosY, so the switch disappears exactly when
       it bottoms out rather than on a separate flag. */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x3c];
    /* Set to -1 the moment the switch is pressed, and its ORIGINAL value was
       copied into mHomeAreaId at init -- so the area it belongs to is remembered
       after the switch stops claiming membership. */
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    /* Model member. The cartridge's own ~BlueCoinSwitch calls _ZN5ModelD1Ev at +0x0d4
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~BlueCoinSwitch calls _ZN10dBgW_KcMbgD1Ev
       at +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    /* Real type, not a marker: InitResources hands it to
       dBgW_KcMbg::SetFile as the collider's transform. */
    Matrix4x3 mMatrix;            /* 0x2ec */
    u8  pad_31c[0x4];
    /* Where mPosY stops: seeded at init as mPosY - 0x64000, so the switch
       always sinks the same distance from wherever it was placed. */
    s32 mStopPosY;            /* 0x320 */
    s32 mTickSound;            /* 0x324 */
    /* The blue-coin countdown, in frames, and it is BOTH the timer and the
       state: non-zero means running, and the code parks it at 1 rather than 0
       to mean "expired but still latched". */
    u16 mCoinTimer;            /* 0x328 */
    /* Its seed, from mParam's second byte -- multiplied by 10 at init, or
       forced to 0xfa when that byte is 0 or 0xff. So the spawn word carries
       tenths of the real count. */
    u16 mCoinTimerSeed;            /* 0x32a */
    u8  mPressed;            /* 0x32c */
    u8  mEventBit;            /* 0x32d */
    u8  mHomeAreaId;            /* 0x32e */
};

#endif /* __cplusplus */

#endif /* BLUECOINSWITCH_H */
