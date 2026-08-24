#ifndef MRBLIZZARD_H
#define MRBLIZZARD_H

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
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct MrBlizzard : dEnemyBase_c {
    dCcAcPos_c mdCcAcPos_c;/* 0x110 */
    dBgCh_Actr mWithMeshClsn;       /* 0x150 */
    ModelAnim mModelAnim;             /* 0x30c */
    ShadowModel mShadowModel;         /* 0x370 */
    u8  pad_398[0x60];
    /* The state pointer. func_ov081_02125488 sets it (InitResources passes
       data_ov081_02128e54 / _02128e84), and Behavior calls the
       pointer-to-member-function at +8 through it and compares it against the
       ov081 state tables _02128e24 / _02128e64 / _02128e84 / _02128e94. */
    void *mState;                     /* 0x3f8 */
    s32 unk_3fc;                      /* 0x3fc */
    s32 mCapUniqueID;                 /* 0x400 */
    u8  pad_404[0x10];
    s16 mInitAngleY;                  /* 0x414 */
    u8  pad_416[0x2];
    s32 mPathId;                      /* 0x418 */
    s32 mType;                        /* 0x41c */
    s32 mPathNodeCount;               /* 0x420 */
    s32 mPathNodeIndex;               /* 0x424 */
    u8  pad_428[0x24];
    Vector3 mHomePos;                 /* 0x44c */

    /* --- vtable --- */
    virtual ~MrBlizzard();

    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    /* Tail padding. The field span stops short of the real size: MrBlizzard_Spawn
       calls fBase_c::operator new(0x46c), read off the retail
       instruction. A span is only a LOWER BOUND. */
    u8 pad_458[0x14];      /* 0x458, to the ROM's 0x46c */
};

typedef char MrBlizzard_size_must_be_0x46c[sizeof(MrBlizzard) == 0x46c ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct MrBlizzard {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x6];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x18];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x3b];
    u8  unk_108;            /* 0x108 */
    u8  pad_109[0x1];
    u8  unk_10a;            /* 0x10a */
    u8  pad_10b[0x5];
    /* dCcAcPos_c member, named by the class's own destructor calling
       dCcAcPos_c's D1 at +0x110 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10MrBlizzardD1Ev.c] */
    dCcAcPos_c mdCcAcPos_c;            /* 0x110 */
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x150 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10MrBlizzardD1Ev.c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x150 */
    /* ModelAnim member, named by the class's own destructor calling
       ModelAnim's D1 at +0x30c -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10MrBlizzardD1Ev.c] */
    ModelAnim mModelAnim;            /* 0x30c */
    u8  mShadowModel;            /* 0x370 */
    u8  pad_371[0x87];
    void *mState;            /* 0x3f8 */
    s32 unk_3fc;            /* 0x3fc */
    s32 mCapUniqueID;            /* 0x400 */
    u8  pad_404[0x10];
    s16 mInitAngleY;            /* 0x414 */
    u8  pad_416[0x2];
    s32 mPathId;            /* 0x418 */
    s32 mType;            /* 0x41c */
    s32 mPathNodeCount;            /* 0x420 */
    s32 mPathNodeIndex;            /* 0x424 */
    u8  pad_428[0x24];
    s32 mHomePosX;            /* 0x44c */
    s32 mHomePosY;            /* 0x450 */
    s32 mHomePosZ;            /* 0x454 */
};

#endif /* __cplusplus */

#endif /* MRBLIZZARD_H */
