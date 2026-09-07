/* WorkElevator is a dBgActor_c with four visible platform models and four
 * moving-mesh colliders. Its constructor and destructor provide both halves
 * of that ownership evidence: the factory builds the arrays in declaration
 * order, while D1/D0 destroy them in reverse order before the inherited
 * dBgActor_c members. */
#ifndef WORKELEVATOR_H
#define WORKELEVATOR_H
#include "types.h"

#ifdef __cplusplus

#include "dBgActor_c.h"

struct WorkElevator : dBgActor_c {
    Model mPlatformModels[4];            /* 0x320 */
    Matrix4x3 mPlatformMats[4];          /* 0x460 */
    dBgW_KcMbg mPlatformColliders[4];    /* 0x520 */
    Vector3 mTargetPos;                  /* 0xc40 */
    s32 unk_c4c[8];                      /* 0xc4c */
    s32 unk_c6c;                         /* 0xc6c */
    u32 unk_c70;                         /* 0xc70 */
    u16 unk_c74;                         /* 0xc74 */
    u16 unk_c76;                         /* 0xc76 */
    u16 unk_c78;                         /* 0xc78 */
    s8 mLoweredPlatform;                 /* 0xc7a */
    s8 unk_c7b;                          /* 0xc7b */
    u8 unk_c7c;                          /* 0xc7c */
    u8 unk_c7d;                          /* 0xc7d */
    u8 unk_c7e;                          /* 0xc7e */
    u8 pad_c7f;                          /* 0xc7f */

    /* --- vtable overrides --- */
    virtual ~WorkElevator();
    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

#else

#include "Model.h"
#include "dBgW_KcMbg.h"

struct WorkElevator {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member. The cartridge's own ~WorkElevator calls _ZN5ModelD1Ev at +0x0d4
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~WorkElevator calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  mClsnMat;            /* 0x2ec */
    u8  pad_2ed[0x233];
    u8  mPlatformClsn0;            /* 0x520 */
    u8  pad_521[0x1c7];
    u8  mPlatformClsn1;            /* 0x6e8 */
    u8  pad_6e9[0x1c7];
    u8  mPlatformClsn2;            /* 0x8b0 */
    u8  pad_8b1[0x1c7];
    u8  mPlatformClsn3;            /* 0xa78 */
    u8  pad_a79[0x1c7];
    s32 unk_c40;            /* 0xc40 */
    s32 unk_c44;            /* 0xc44 */
    s32 unk_c48;            /* 0xc48 */
    u8  pad_c4c[0x20];
    s32 unk_c6c;            /* 0xc6c */
    u8  pad_c70[0xa];
    s8  mLoweredPlatform;            /* 0xc7a */
    /* trailing extent the ROM's `new WorkElevator` literal proves; see tools/opnew_sizes.py */
    u8 pad_c7c[0x4];
};

#endif /* __cplusplus */

typedef char WorkElevator_size_must_be_0xc80[sizeof(struct WorkElevator) == 0xc80 ? 1 : -1];

#endif
