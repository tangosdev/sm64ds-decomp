#ifndef DAYUREI_MUCHO_C_H
#define DAYUREI_MUCHO_C_H

#include "types.h"

/* ROM RTTI names this class daYurei_Mucho_c. Overlay_actors maps profile 236
 * YUREI_MUCHO to SNUFIT. It is a dEnemyBase_c; the four subobjects occupy
 * 0x110..0x38c and the registry factory allocates 0x3e4. Field names below
 * are reconstructed from their use in this TU.
 */

#include "dEnemyBase_c.h"
#include "dExtAnmModel_c.h"
#include "dCcAc_c.h"
#include "dExtShadowModel_c.h"
#include "dBgCh_Actr.h"

struct daYurei_Mucho_c : dEnemyBase_c {
    /* Two 8-byte PMFs, called on state entry and each frame. The names and
       int signatures are reconstructed; forwarding r0 does not establish
       an original C++ return type (one retained entry helper returns short). */
    struct State {
        int (daYurei_Mucho_c::*mEnter)();      /* 0x00 */
        int (daYurei_Mucho_c::*mMain)();       /* 0x08 */
    };

    dCcAc_c mdCcAc_c;                 /* 0x110 */
    dBgCh_Actr mWithMeshClsn;         /* 0x144 */
    dExtAnmModel_c mAnmModel;         /* 0x300 */
    dExtShadowModel_c mShadowModel;         /* 0x364 */
    /* DropShadowRadHeight matrix. UpdateModelTransform writes a translation of
       (mPos.x, mPos.y - 0x18000, mPos.z) >> 3. */
    Matrix4x3 mShadowMat;             /* 0x38c */
    State *mCurrentState;             /* 0x3bc */
    /* Bone-derived shot origin. UpdateModelTransform fills it from the last
       column of the transformed model matrix (<< 3, y -= 0xa000);
       UpdateShoot passes it to dActor_c::Spawn as the BULLET (0xe9)
       position. */
    Vector3 mShotPos; /* 0x3c0 */
    /* InitResources copies the spawn position into these three, Behavior
       re-captures the live position into them in the Yoshi-eat branch, and
       reads mHomePos.y back when steering. */
    Vector3 mHomePos; /* 0x3cc */
    /* Hover phase: Behavior adds 0x200 a frame and indexes FX_SinCosTable_ to
       bob mPos.y around mHomePos.y + 0xb4000. */
    s32 mBobAngle;                    /* 0x3d8 */
    s32 mShotCount;                   /* 0x3dc */
    s16 mTargetAngle;                 /* 0x3e0 */
    u8  pad_3e2[2];                   /* unobserved */

    /* --- vtable --- */
    /* The inline form emits the retail D1/D0 pair under the pinned compiler. */
    virtual ~daYurei_Mucho_c() {}

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual void OnTurnIntoEgg(daPly_c &player); /* slot 19 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    /* Non-virtual. Unowned .data PMFs resolve these by symbols.txt name. */
    void UpdateModelTransform();
    int ChangeState(State *s);
    int EnterIdle();
    int UpdateIdle();
    int EnterChase();
    int UpdateChase();
    short EnterShoot();
    int UpdateShoot();
    int EnterBumped();
    int UpdateBumped();
    void HandleCollision();
    void KillWithCoins();
};

/* Inline destructor is the key function, so this TU emits the vtable. The
 * factory is `return new daYurei_Mucho_c()`, which calls the inherited
 * fBase_c::operator new (`_ZN7fBase_cnwEm`). */

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daYurei_Mucho_c_size_must_be_0x3e4[sizeof(daYurei_Mucho_c) == 0x3e4 ? 1 : -1];
#endif

#endif /* DAYUREI_MUCHO_C_H */
