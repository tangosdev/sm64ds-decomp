#ifndef UNCHAINEDCHOMP_H
#define UNCHAINEDCHOMP_H

#include "types.h"
#include "Enemy.h"
#include "Model.h"
#include "ModelAnim.h"
#include "MovingCylinderClsnWithPos.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

/* daWanwan2_c in the ROM's RTTI. Derives from Enemy, and the destructor is an
 * unusually strong witness because six of the nine members are ARRAYS: __destroy_arr
 * takes a count and a stride, so it names not just the type at an offset but how many
 * and how far apart. Six arrays tile 0x370..0x78c with no overlap and no gap:
 *
 *     0x370  Model       x6   stride 0x50
 *     0x550  ShadowModel x6   stride 0x28
 *     0x640  ShadowModel  1
 *     0x6d8  Vector3     x6   stride 0x0c
 *     0x720  Vector3     x6   stride 0x0c
 *     0x768  Vector3s    x6   stride 0x06   -> ends 0x78c
 *
 * UnchainedChomp_Spawn constructs the same six through func_020733a8, which takes the
 * same counts and strides, and allocates 0x7a4 -- so 0x18 of tail is spare and stays
 * padding.
 */
struct UnchainedChomp : Enemy {
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;  /* 0x110 */
    WithMeshClsn        mWithMeshClsn;      /* 0x150 */
    ModelAnim           mModelAnim;         /* 0x30c */
    Model               mModels[6];         /* 0x370 */
    ShadowModel         mShadowModels[6];   /* 0x550 */
    ShadowModel         mShadowModel;       /* 0x640 */
    /* Behavior loads a pointer from here and calls a member function through it --
       see the note in _ZN14UnchainedChomp8BehaviorEv.cpp. */
    void               *unk_668;            /* 0x668 */
    u8  pad_66c[0x40];
    s32 unk_6ac;                            /* 0x6ac */
    s32 unk_6b0;                            /* 0x6b0 */
    s32 unk_6b4;                            /* 0x6b4 */
    s32 unk_6b8;                            /* 0x6b8 */
    u8  pad_6bc[0xd];
    u8  unk_6c9;                            /* 0x6c9 */
    u8  pad_6ca[0x2];
    s32 unk_6cc;                            /* 0x6cc */
    s32 unk_6d0;                            /* 0x6d0 */
    s32 unk_6d4;                            /* 0x6d4 */
    Vector3             mUnk_6d8[6];        /* 0x6d8 */
    Vector3             mUnk_720[6];        /* 0x720 */
    Vector3s            mUnk_768[6];        /* 0x768 */
    u8  pad_78c[0x18];

    virtual ~UnchainedChomp();

    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    /* methods */
    int Behavior();
    int Render();
};

typedef char UnchainedChomp_size_must_be_0x7a4[sizeof(UnchainedChomp) == 0x7a4 ? 1 : -1];

#endif /* UNCHAINEDCHOMP_H */
