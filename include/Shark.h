#ifndef SHARK_H
#define SHARK_H

#include "types.h"

/* Derives from dEnemyBase_c: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything this header used to restate below 0x110 belonged to the
 * chain above and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

struct Shark : dEnemyBase_c {
    dCcAcPos_c mdCcAcPos_c;/* 0x110 */
    dBgCh_Actr mWithMeshClsn;       /* 0x150 */
    ModelAnim mModelAnim;             /* 0x30c */
    /* The state pointer. func_ov090_021338b4 sets it (InitResources passes
       data_ov090_021345cc) and Behavior calls the pointer-to-member-function
       at +8 through it. */
    s32 mState;                       /* 0x370 */
    Vector3 mClsnOffset;              /* 0x374 */
    u8  pad_380[0x8];
    s32 mPathID;                      /* 0x388 */
    s32 mPathNodeCount;               /* 0x38c */
    s32 mPathNodeIdx;                 /* 0x390 */

    /* --- vtable --- */
    virtual ~Shark();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    /* Tail padding. The field span stops short of the real size: Shark_Spawn
       calls fBase_c::operator new(0x3a0), read off the retail
       instruction. A span is only a LOWER BOUND. */
    u8 pad_394[0xc];      /* 0x394, to the ROM's 0x3a0 */
};

typedef char Shark_size_must_be_0x3a0[sizeof(Shark) == 0x3a0 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct Shark {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    /* 0x05c..0x08c is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 mPosX;                 /* 0x05c */
    s32 mPosY;                   /* 0x060 */
    s32 mPosZ;                   /* 0x064 */
    s32 mPrevPosX;                 /* 0x068 */
    s32 mPrevPosY;                 /* 0x06c */
    s32 mPrevPosZ;                 /* 0x070 */
    s32 mCamSpacePosX;           /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    s16 mPrevAngleX;            /* 0x092 */
    s16 mPrevAngleY;            /* 0x094 */
    s16 mPrevAngleZ;            /* 0x096 */
    u8  pad_098[0x4];
    s32 mVertAccel;            /* 0x09c */
    s32 mTerminalVelocity;            /* 0x0a0 */
    u8  unk_0a4;            /* 0x0a4 */
    u8  pad_0a5[0x3];
    s32 mVertSpeed;            /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac */
    u8  pad_0b0[0x50];
    u8  mStateTimer;            /* 0x100 */
    u8  pad_101[0xf];
    /* dCcAcPos_c member, named by the class's own destructor calling
       dCcAcPos_c's D1 at +0x110 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN5SharkD1Ev.c] */
    dCcAcPos_c mdCcAcPos_c;            /* 0x110 */
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x150 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN5SharkD1Ev.c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x150 */
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x30c -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    ModelAnim mModelAnim;            /* 0x30c */
    s32 mState;            /* 0x370 */
    s32 mClsnOffsetX;            /* 0x374 */
    s32 mClsnOffsetY;            /* 0x378 */
    s32 mClsnOffsetZ;            /* 0x37c */
    u8  pad_380[0x8];
    s32 mPathID;            /* 0x388 */
    s32 mPathNodeCount;            /* 0x38c */
    s32 mPathNodeIdx;            /* 0x390 */
};

#endif /* __cplusplus */

#endif /* SHARK_H */
