#ifndef DAWANWAN_C_H
#define DAWANWAN_C_H

#include "types.h"
#include "dEnemyBase_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "dExtShadowModel_c.h"
#include "dCcAcPos_c.h"

/* Bob-omb Battlefield Chain Chomp. Seven links, so seven of each subobject.
 * dEnemyBase_c ends at 0x110; dCcAcPos_c, ModelAnim and dExtShadowModel_c close on
 * 0x150, 0x1b4 and 0x1dc; then Model[7], dExtShadowModel_c[7] and two Vector3[7].
 */
struct daWanwan_c : dEnemyBase_c {
    dCcAcPos_c mdCcAcPos_c;                                /* 0x110 */
    ModelAnim mModelAnim;                                  /* 0x150 */
    dExtShadowModel_c mShadowModel;                        /* 0x1b4 */
    Model mLinkModels[7];                                  /* 0x1dc */
    dExtShadowModel_c mLinkShadows[7];                     /* 0x40c */
    /* InitResources copies the chomp position into every link. 0211250c then
       walks [1..6]; [0] is the head, written from the actor position each frame. */
    Vector3 mLinkPos[7];                                   /* 0x524 */
    /* 0211250c adds the stored value into the link step, then replaces it with
       the scaled movement. [0] is constructed and that loop never touches it. */
    Vector3 mLinkDelta[7];                                 /* 0x578 */
    u8  pad_5cc[4];
    /* 0211250c, one floor per link [1..6]. Raised to mSpawnPosY + 0x28000. */
    s32 mLinkFloorY[6];                                    /* 0x5d0 */
    u8  pad_5e8[4];
    /* InitResources copies the position here, then adds 0xc8000 to the live
       position. Behavior clamps mPosY up to mSpawnPosY + 0xc8000. */
    s32 mSpawnPosX;         /* 0x5ec */
    s32 mSpawnPosY;         /* 0x5f0 */
    s32 mSpawnPosZ;         /* 0x5f4 */
    s32 mChainExtension;    /* 0x5f8 -- leash slack; 02111fe0 uses it * 7 */
    u16 mActionTimer;       /* 0x5fc -- DecIfAbove0_Short */
    u16 mSecretSound;       /* 0x5fe -- Sound::PlaySecretSound counter */
    s16 unk_600;            /* 0x600 -- idle lunge and release enter store 0; unread */
    s16 mTargetAngY;        /* 0x602 -- idle faces this; HorzAngleToCPlayer writes it */
    u8  mReleaseStep;       /* 0x604 -- release cutscene step, 021115ec */
    u8  mChainBroken;       /* 0x605 -- 02111f54 writes 1; Behavior skips the leash */
    u8  pad_606[2];
    s32 mStumpUniqueID;     /* 0x608 -- STUMP (0x1b) spawned by InitResources */
    s32 mFenceUniqueID;     /* 0x60c -- CHAIN_CHOMP_FENCE (0x29), found by Behavior */
    s32 mState;             /* 0x610 -- index into data_ov014_0211476c */
    u8  pad_614[4];
    s32 mReleaseSpeed;      /* 0x618 -- release step 6 approach speed */
    u8  mIsOnGround;        /* 0x61c -- set when mPosY was clamped to the rest height */
    u8  mWasOnGround;       /* 0x61d -- previous frame; landing fires on the rising edge */

    /* Defined INLINE on purpose. Out of line, mwccarm emits D2, D0, D1; the ROM
       has D1 at 0x02111308 then D0 at 0x021113bc and no D2 anywhere in ov014.
       Inline, mwccarm emits D1 then D0 and no D2 -- the cartridge's own order.
       The body is empty either way: the 0xb4 bytes of D1 are the compiler's own
       teardown of the four arrays and three member subobjects. */
    virtual ~daWanwan_c() {}   /* slots 16 (D1), 17 (D0) */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    void func_ov014_02111ebc(int i);
    void func_ov014_02111f08();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daWanwan_c_size_must_be_0x620[sizeof(daWanwan_c) == 0x620 ? 1 : -1];
#endif

#endif /* DAWANWAN_C_H */
