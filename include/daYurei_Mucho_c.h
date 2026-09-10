#ifndef DAYUREI_MUCHO_C_H
#define DAYUREI_MUCHO_C_H

#include "types.h"

/* ROM RTTI identifies daYurei_Mucho_c as a dEnemyBase_c. Its four subobjects
 * occupy 0x110..0x38c; the registry factory allocates 0x3e4 bytes. Field names
 * are reconstructed from their use. See the class handoff for the evidence.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct daYurei_Mucho_c : dEnemyBase_c {
    /* Two 8-byte PMFs, called on state entry and each frame. The names and
       int signatures are reconstructed; forwarding r0 does not establish
       an original C++ return type (one retained entry helper returns short). */
    struct State {
        int (daYurei_Mucho_c::*mEnter)();      /* 0x00 */
        int (daYurei_Mucho_c::*mMain)();       /* 0x08 */
    };

    dCcAc_c mdCcAc_c; /* 0x110 */
    dBgCh_Actr mWithMeshClsn;       /* 0x144 */
    ModelAnim mModelAnim;             /* 0x300 */
    ShadowModel mShadowModel;         /* 0x364 */
    u8  pad_38c[0x30];
    State *mCurrentState;             /* 0x3bc */
    u8  pad_3c0[0xc];
    /* InitResources copies the spawn position into these three, Behavior
       re-captures the live position into them in the Yoshi-eat branch, and
       reads mHomePosY back when steering. */
    s32 mHomePosX;                    /* 0x3cc */
    s32 mHomePosY;                    /* 0x3d0 */
    s32 mHomePosZ;                    /* 0x3d4 */
    s32 unk_3d8;                      /* 0x3d8 -- advanced by 0x200 a frame */
    s32 mShotCount;                   /* 0x3dc */
    s16 mTargetAngle;                 /* 0x3e0 */
    u8  pad_3e2[2];                   /* unobserved */

    /* --- vtable --- */
    /* The inline form emits the retail D1/D0 pair under the pinned compiler. */
    virtual ~daYurei_Mucho_c() {}

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual void  OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char daYurei_Mucho_c_size_must_be_0x3e4[sizeof(daYurei_Mucho_c) == 0x3e4 ? 1 : -1];

#endif /* DAYUREI_MUCHO_C_H */
