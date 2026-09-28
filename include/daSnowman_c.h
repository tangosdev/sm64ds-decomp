#ifndef DASNOWMAN_C_H
#define DASNOWMAN_C_H

#include "types.h"

/* Derives from dEnemyBase_c: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything below 0x110 belongs to the chain above and is inherited.
 *
 * SM64DS proves this class as daSnowman_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings below are reconstructed
 * source-style names -- evidence-bounded proposals, not recovered SM64DS
 * symbols.
 *
 * daSnowman_c_classInit at 0x02125ec0 (historical alias MrBlizzard_Spawn)
 * allocates 0x46c and installs this class's cartridge vtable. It backs the
 * SNOWMAN registry profile, whose descriptor at 0x021289cc is reconstructed
 * as g_profile_SNOWMAN.
 *
 * Field names from 0x398 down marked "coined" are named for what the matched
 * bodies in src/actors/daSnowman_c.cpp do with them, not recovered.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct daSnowman_c : dEnemyBase_c {
    /* One state: `enter` runs once from func_ov081_02125488, `execute` every
       frame from Behavior. The ten records live in ov081 .bss
       (0x02128e14..0x02128ea4, 0x10 apart); __sinit_ov081_02128154 copies
       each pair of member pointers in from .data 0x021288f8..0x02128998.
       State, enter and execute are coined names. */
    struct State {
        int (daSnowman_c::*enter)();
        int (daSnowman_c::*execute)();
    };

    dCcAcPos_c mdCcAcPos_c;           /* 0x110 */
    dBgCh_Actr mWithMeshClsn;         /* 0x150 */
    ModelAnim mModelAnim;             /* 0x30c */
    ShadowModel mShadowModel;         /* 0x370 */
    Matrix4x3 mShadowMatrix;          /* 0x398 -- coined; handed to DropShadowRadHeight */
    /* Coined. The render step builds it from bone 5 of mModelAnim and hands
       its address to the cap actor at +0xc8, which rides on it. */
    Matrix4x3 mCapMatrix;             /* 0x3c8 */
    State *mState;                    /* 0x3f8 */
    /* Coined. The uniqueID of the actor func_ov081_02124dfc spawns (0xe0);
       it is carried at mHandPos and thrown toward the closest player. */
    u32 mSnowballID;                  /* 0x3fc */
    s32 mCapUniqueID;                 /* 0x400 */
    u8  pad_404[0x4];
    /* Coined. Walk uses it as a hop-phase counter (0 start, 1 landed,
       then frames until the next hop). Spin stores mAngleY - player
       angle and uses the sign to pick the spin direction. */
    s32 mStep;                        /* 0x408 */
    s32 mClosestPlayerIdx;            /* 0x40c -- coined; written by func_ov081_021245e8 */
    s32 mTimer;                       /* 0x410 -- coined; lean frames, then spin frames */
    s16 mInitAngleY;                  /* 0x414 */
    u8  pad_416[0x2];
    s32 mPathId;                      /* 0x418 */
    s32 mType;                        /* 0x41c */
    s32 mPathNodeCount;               /* 0x420 */
    s32 mPathNodeIndex;               /* 0x424 */
    u8  pad_428[0xc];
    Vector3 mHandPos;                 /* 0x434 -- coined; the held snowball is pinned here */
    Vector3 mCapPos;                  /* 0x440 -- coined; the cap actor is pinned here */
    Vector3 mHomePos;                 /* 0x44c */
    s32 mHopStep;                     /* 0x458 -- coined; distance moved per frame toward a path node */
    s32 mSinkOffsetY;                 /* 0x45c -- coined; added to mPosY when rendering */
    u32 mParticleID1;                 /* 0x460 -- coined; Particle::System::New handle */
    u32 mParticleID2;                 /* 0x464 -- coined; Particle::System::New handle */
    /* Coined. Set to 2 on a 0x40 hit while the player's param1 is 2.
       Behavior plays 0x166 once the death update has finished, then clears it. */
    u8  mDeathSound;                  /* 0x468 */
    /* Coined. Kind 3's cap watch: 0 until the first look, 1 while the
       cap stays lost, 2 once the player has it. Nonzero also suppresses
       the coin drop, so a watcher that becomes kind 2 does not pay coins. */
    u8  mCapPhase;                    /* 0x469 */
    u8  pad_46a[0x2];

    /* --- vtable --- */
    virtual ~daSnowman_c();

    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daSnowman_c_size_must_be_0x46c[sizeof(daSnowman_c) == 0x46c ? 1 : -1];
#endif

#endif /* DASNOWMAN_C_H */
