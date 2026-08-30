#ifndef TTC_MOVINGBEAM_H
#define TTC_MOVINGBEAM_H

#include "types.h"
#include "math/Matrix.h"

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

struct TTC_MovingBeam : dBgActor_c {
    u8  pad_31e[0x2];
    s32 mStartPosY;                      /* 0x320 */
    s32 mEndPosY;                      /* 0x324 */
    u8 mDirection;                       /* 0x328 */
    u8  pad_329[0x7];
    s32 mGroundY;                     /* 0x330 -- InitResources' dBgCh_Gnd raycast result, falling back to the probe height */
    ShadowModel mShadowModel;         /* 0x334 */

    /* --- vtable --- */
    virtual ~TTC_MovingBeam();

    /* An override the cartridge proves and this header never declared.
       _ZTV14TTC_MovingBeam slot 6 pointed at fBase_c::Behavior; the ROM has
       ov065:_ZN14TTC_MovingBeam8BehaviorEv (0x0211bd8c, 0x178 bytes), named in
       symbols.txt but not yet decompiled -- the slot needs the symbol, not a body.
       No `virtual` keyword, matching the overrides beside it: a derived declaration
       of a base virtual overrides whether or not it repeats the word.
       Measured by tools/romdata_check.py, the only gate that reads vtable bytes. */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* Tail padding. The field span stops short of the real size: TTC_MovingBeam_Spawn
       calls fBase_c::operator new(0x38c), read off the retail
       instruction. A span is only a LOWER BOUND. */
    Matrix4x3 mShadowMat;        /* 0x35c */
};

typedef char TTC_MovingBeam_size_must_be_0x38c[sizeof(TTC_MovingBeam) == 0x38c ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct TTC_MovingBeam {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x10];
    s32 mTerminalVelocity;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;            /* 0x0a8 */
    u8  pad_0ac[0x28];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member, named by the class's own destructor calling
       dBgW_KcMbg's D1 at +0x124 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN14TTC_MovingBeamD1Ev.c] */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  mClsnMat;            /* 0x2ec */
    u8  pad_2ed[0x33];
    s32 mStartPosY;            /* 0x320 */
    s32 mEndPosY;            /* 0x324 */
    u8  mDirection;            /* 0x328 */
    u8  pad_329[0x7];
    s32 mGroundY;            /* 0x330 */
    ShadowModel mShadowModel; /* 0x334 */
};

#endif /* __cplusplus */

#endif /* TTC_MOVINGBEAM_H */
