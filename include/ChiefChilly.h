#ifndef CHIEFCHILLY_H
#define CHIEFCHILLY_H

#include "types.h"

/* The Chief Chilly boss. Its destructor is the layout, and every member closes
 * exactly on the next one:
 *
 *     Enemy                      ends 0x110
 *     MovingCylinderClsnWithPos  0x110 + 0x040 = 0x150
 *     WithMeshClsn               0x150 + 0x1bc = 0x30c
 *     BlendModelAnim             0x30c + 0x070 = 0x37c   -> mState
 *     ShadowModel                0x380 + 0x028 = 0x3a8
 *     Vector3[8]                 0x3e8 + 0x060 = 0x448   -> the next array
 *     Vector3[8]                 0x448 + 0x060 = 0x4a8
 *     Vector3[2]                 0x4d4 + 0x018 = 0x4ec   -> unk_4ec
 *
 * Eight boundaries, each from a size another header asserts independently.
 *
 * THE THREE ARRAYS ARE Vector3, and that is what named func_020072c0. The ROM
 * destroys them with __destroy_arr(ptr, N, 0xc, ...), which a POD array would
 * never need, and InitResources fills each element as x/y/z from the actor's
 * position. See the note on Vector3 in include/types.h.
 *
 * unk_3d8/3dc/3e0 are NOT a Vector3 despite looking like one: the destructor
 * does not touch 0x3d8, and a Vector3 member would be destroyed there.
 */

#ifdef __cplusplus

#include "Enemy.h"
#include "ShadowModel.h"
#include "BlendModelAnim.h"
#include "WithMeshClsn.h"
#include "MovingCylinderClsnWithPos.h"

struct ChiefChilly : Enemy {
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;  /* 0x110 */
    WithMeshClsn mWithMeshClsn;                            /* 0x150 */
    BlendModelAnim mBlendModelAnim;                        /* 0x30c */
    void *mState;                                          /* 0x37c */
    ShadowModel mShadowModel;                              /* 0x380 */
    u8  pad_3a8[0x30];
    s32 unk_3d8;                                           /* 0x3d8 */
    s32 unk_3dc;                                           /* 0x3dc */
    s32 unk_3e0;                                           /* 0x3e0 */
    u8  pad_3e4[0x4];
    Vector3 unk_3e8[8];                                    /* 0x3e8 */
    Vector3 unk_448[8];                                    /* 0x448 */
    u8  pad_4a8[0x14];
    s32 unk_4bc;                                           /* 0x4bc */
    u8  pad_4c0[0x5];
    u8  unk_4c5;                                           /* 0x4c5 */
    u8  pad_4c6[0x3];
    u8  unk_4c9;                                           /* 0x4c9 */
    u8  pad_4ca[0x1];
    u8  unk_4cb;                                           /* 0x4cb */
    u8  pad_4cc[0x8];
    Vector3 unk_4d4[2];                                    /* 0x4d4 */
    s32 unk_4ec;                                           /* 0x4ec */
    s32 unk_4f0;                                           /* 0x4f0 */
    s32 unk_4f4;                                           /* 0x4f4 */

    virtual ~ChiefChilly();

    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();
};

#endif /* __cplusplus */

#endif /* CHIEFCHILLY_H */
