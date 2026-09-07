#ifndef SNUFIT_H
#define SNUFIT_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN6SnufitD1Ev`
 * stores this vtable, destroys four members, then calls `dEnemyBase_c::~dEnemyBase_c`.
 * Everything this header used to restate below 0x110 belongs to that chain and
 * is inherited now.
 *
 * The four members close exactly on one another, and dEnemyBase_c's own 0x110 closes
 * exactly on the first:
 *
 *     0x110 dCcAc_c  0x34   -> 0x144
 *     0x144 dBgCh_Actr        0x1bc  -> 0x300
 *     0x300 ModelAnim           0x64   -> 0x364
 *     0x364 ShadowModel         0x28   -> 0x38c
 *
 * Two fields the auto-generated header never listed turned up in Behavior,
 * reachable only as raw `c + 0x...`: the state pointer at 0x3bc and the
 * angle at 0x3d8. The class is 0x3dc, not the 0x3d8 its old field span
 * implied.
 *
 * Typing mModelAnim absorbed unk_35c, which sat at +0x5c inside it -- the
 * `speed` of its Animation base (base at +0x50, speed at +0x0c). Same shape as
 * HootTheOwl's.
 *
 * SIZE IS THE ROM'S OWN, not a rounded-up field span: `daYurei_Mucho_c_classInit` calls
 * `fBase_c::operator new(996)` -- 0x3e4 -- and stores `_ZTV6Snufit`,
 * so that literal IS this class's sizeof. The observed fields only span to
 * 0x3dc; the difference is trailing space no source reads.
 *
 * SM64DS RTTI names the implementation daYurei_Mucho_c. The reconstructed
 * factory daYurei_Mucho_c_classInit (historical alias
 * Snufit_Spawn) constructs it for the YUREI_MUCHO
 * registry profile.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct Snufit : dEnemyBase_c {
    /* What mCurrentState points at. Behavior compares it against three objects
       in ov065's data and calls the handler at +0x08 through it. Only that
       handler is evidenced. */
    struct State {
        u8  pad_00[0x8];
        void (Snufit::*mMain)();      /* 0x08 */
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
    u8  pad_3dc[0x8];

    /* --- vtable --- */
    virtual ~Snufit();

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char Snufit_size_must_be_0x3e4[sizeof(Snufit) == 0x3e4 ? 1 : -1];

#endif /* SNUFIT_H */
