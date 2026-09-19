#ifndef DAKING_DONKETU_C_H
#define DAKING_DONKETU_C_H

#include "types.h"

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "ShadowModel.h"
#include "BlendModelAnim.h"
#include "dBgCh_Actr.h"
#include "dCcAcPos_c.h"

/* Chief Chilly (KING_DONKETU 218) -- ov073/daKing_Donketu_c.
 *
 * RTTI ov073:0x02123058 is the length-prefixed string 16daKing_Donketu_c;
 * _ZTI16daKing_Donketu_c at 0x0212304c points its name word there and its
 * base at dEnemyBase_c. The tree used to carry this class under the coined
 * English name ChiefChilly. Factory allocates 0x504.
 *
 * daKing_Donketu_c_classInit is reconstructed (RTTI daKing_Donketu_c,
 * KING_DONKETU registry). Retail does not store that spelling.
 * Historical alias: ChiefChilly_Spawn. */

struct daKing_Donketu_c : dEnemyBase_c {
    dCcAcPos_c mdCcAcPos_c;                                /* 0x110 */
    dBgCh_Actr mWithMeshClsn;                              /* 0x150 */
    BlendModelAnim mBlendModelAnim;                        /* 0x30c */
    void *mState;                                          /* 0x37c */
    ShadowModel mShadowModel;                              /* 0x380 */
    u8  pad_3a8[0x30];
    /* InitResources copies mPosX/mPosY/mPosZ here once, in the same breath as
       seeding both waypoint arrays with the same position. */
    s32 mSpawnPosX;                                        /* 0x3d8 */
    s32 mSpawnPosY;                                        /* 0x3dc */
    s32 mSpawnPosZ;                                        /* 0x3e0 */
    u8  pad_3e4[0x4];
    /* Two eight-entry waypoint sets, both seeded with the boss's own position by
       InitResources and refilled by func_ov073_021222ec. The steering helpers
       (func_ov073_021203ac / _02120610) index one of them with the cursor byte at
       0x4c4 and hand the element to Vec3_HorzAngle/Vec3_VertAngle as the point to
       turn towards -- set B while mHitsRemaining reads 2, set A otherwise. */
    Vector3 mWaypointsA[8];                                /* 0x3e8 */
    Vector3 mWaypointsB[8];                                /* 0x448 */
    u8  pad_4a8[0x14];
    s32 unk_4bc;                                           /* 0x4bc -- InitResources stores 2; no reader */
    u8  pad_4c0[0x5];
    u8  unk_4c5;                                           /* 0x4c5 -- InitResources stores 0xff; no reader */
    u8  pad_4c6[0x3];
    /* The forward ground ray found nothing while the boss was moving fast.
       Behavior sets it on a miss and clears it on a hit; func_ov073_02120ed0
       tests it for 1. */
    u8  mNoGroundAhead;                                    /* 0x4c9 */
    u8  pad_4ca[0x1];
    /* Knock-downs left. InitResources sets 3, func_ov073_02120c7c decrements it,
       func_ov073_02120910 forces 1, and 2 vs 1 selects the waypoint set, the
       ground ray's reach and two behaviour branches. */
    u8  mHitsRemaining;                                    /* 0x4cb */
    u8  pad_4cc[0x8];
    Vector3 unk_4d4[2];                                    /* 0x4d4 -- rebuilt each frame by
                                                              func_ov073_021215cc from a bone
                                                              transform; func_ov073_02120ed0
                                                              spawns something at [0] */
    /* Where the boss WAS when the ground ray missed: Behavior stores the live
       position here and then rewinds mPos to mPrevPos. Nothing in the tree reads
       the copy back, so it is written for a consumer that is not recovered. */
    s32 unk_4ec;                                           /* 0x4ec */
    s32 unk_4f0;                                           /* 0x4f0 */
    s32 unk_4f4;                                           /* 0x4f4 */
    /* trailing extent the ROM's `new daKing_Donketu_c` literal proves; see tools/opnew_sizes.py */
    u8 pad_4f8[0xc];

    /* OUT OF LINE, DECLARED FIRST. `#pragma defer_codegen off` in the TU
       emits D1 then D0 then homeless D2, the cartridge's order. */
    virtual ~daKing_Donketu_c();

    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daKing_Donketu_c_size_must_be_0x504[
    sizeof(daKing_Donketu_c) == 0x504 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAKING_DONKETU_C_H */
