#ifndef DAKRB_C_H
#define DAKRB_C_H
#include "types.h"
#include "dCapEnemy_c.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "MaterialChanger.h"

/* Goomba (KURIBO 200 / KURIBO_S 201 / KURIBO_L 202) -- ov084/daKrb_c.
 *
 * RTTI ov084:0x021308d4 is the length-prefixed string 7daKrb_c;
 * _ZTI7daKrb_c at 0x021308e0 points its name word there and its base
 * at dCapEnemy_c. The tree used to carry this class under the coined
 * English name Goomba.
 *
 * Factories daKrb_c_classInit_KURIBO / _S / _L allocate 0x478 bytes;
 * that is this class's size. Member NAMES below are from this TU's
 * own methods.
 *
 * SM64DS RTTI names the implementation daKrb_c. The reconstructed
 * factories daKrb_c_classInit_KURIBO / _S / _L (historical aliases
 * Goomba_Spawn / GoombaSmall_Spawn / GoombaLarge_Spawn) install this
 * class's cartridge vtable; the reconstructed profile globals
 * g_profile_KURIBO / _S / _L are the registry descriptors. Exact
 * original SM64DS member spellings are not preserved.
 */

struct daKrb_c : dCapEnemy_c {
    dCcAc_c mdCcAc_c;         /* 0x180 */
    dBgCh_Actr mWithMeshClsn; /* 0x1b4 */
    ModelAnim mModelAnim;     /* 0x370 */
    ShadowModel mShadowModel; /* 0x3d4 */
    MaterialChanger mMaterialChanger; /* 0x3fc */
    Vector3 mSafePos;       /* 0x410 */
    Vector3 mHomePos;       /* 0x41c */
    Vector3 mStuckCheckPos; /* 0x428 */
    s32 mState;            /* 0x434 */
    s32 unk_438;            /* 0x438 */
    s32 mTargetUniqueID;    /* 0x43c */
    s32 mDistToPlayer;      /* 0x440 */
    s32 unk_444;            /* 0x444 */
    u8  pad_448[0x4];
    s32 mSavedParam;            /* 0x44c */
    s16 mHeadingHoldTimer;  /* 0x450 */
    u8  pad_452[0x2];
    s16 mWanderRerollTimer; /* 0x454 */
    u16 mStuckTimer;            /* 0x456 */
    u16 mTimer458;            /* 0x458 */
    s16 mInitAngleY;            /* 0x45a */
    u8  pad_45c[0x4];
    s32 mGoombaType;            /* 0x460 */
    u8  mRewardType;            /* 0x464 */
    s8  mStarTracked;            /* 0x465 */
    u8  mStarID;            /* 0x466 */
    u8  unk_467;            /* 0x467 */
    u8  mSoundLatchFlags;   /* 0x468 */
    u8  pad_469[0xf];

    /* OUT OF LINE, DECLARED FIRST. `#pragma defer_codegen off` in the TU
       emits D1 then D0 then homeless D2, the cartridge's order. */
    virtual ~daKrb_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();

    int OnYoshiTryEat();                        /* slot 18 */
    void OnTurnIntoEgg(Player &player);          /* slot 19 */
    int OnAimedAtWithEgg();                     /* slot 29 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daKrb_c_size_must_be_0x478[sizeof(daKrb_c) == 0x478 ? 1 : -1];
#endif

#endif
