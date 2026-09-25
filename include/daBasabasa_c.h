#ifndef DABASABASA_C_H
#define DABASABASA_C_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN12daBasabasa_cD1Ev`
 * stores this vtable, destroys five members, then calls `dEnemyBase_c::~dEnemyBase_c`.
 *
 * The five members close exactly on one another, and dEnemyBase_c's own 0x110 closes
 * exactly on the first. The destructor destroys TWO ModelAnims, which is what
 * makes the pair at 0x300 and 0x364 two members rather than one and a gap:
 *
 *     0x110 dCcAc_c       0x34   -> 0x144
 *     0x144 dBgCh_Actr    0x1bc  -> 0x300
 *     0x300 ModelAnim     0x64   -> 0x364
 *     0x364 ModelAnim     0x64   -> 0x3c8
 *     0x3c8 ShadowModel   0x28   -> 0x3f0
 *
 * SIZE IS THE ROM'S OWN: daBasabasa_c_classInit passes 0x440 to
 * fBase_c::operator new, and the last field (0x43c) closes on it.
 *
 * SM64DS RTTI names the implementation daBasabasa_c. The factory
 * daBasabasa_c_classInit (historical alias Swoop_Spawn) constructs it for
 * the BASABASA registry profile.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct daBasabasa_c : dEnemyBase_c {
    /* One state: `enter` runs once from SetState, `execute` every frame from
       Behavior. The four records live in ov065 .bss (0x0211d6e0, 0x0211d6f0,
       0x0211d700, 0x0211d710); the static initializer copies each pair of
       member pointers in from .data 0x0211cc20..0x0211cc60. State, enter and
       execute are coined names. */
    struct State {
        int (daBasabasa_c::*enter)();
        int (daBasabasa_c::*execute)();
    };

    dCcAc_c mdCcAc_c;                 /* 0x110 */
    dBgCh_Actr mWithMeshClsn;         /* 0x144 */
    ModelAnim mModelAnim1;            /* 0x300 -- flying model */
    ModelAnim mModelAnim2;            /* 0x364 -- hanging model */
    ShadowModel mShadowModel;         /* 0x3c8 */
    Matrix4x3 mShadowMatrix;          /* 0x3f0 */
    State *mCurrentState;             /* 0x420 */
    /* InitResources copies the spawn position into these three. */
    s32 mHomePosX;                    /* 0x424 */
    s32 mHomePosY;                    /* 0x428 */
    s32 mHomePosZ;                    /* 0x42c */
    /* Coined names from here down. */
    s32 mPerchTimer;                  /* 0x430 -- frames spent back at home before perching */
    s16 mTargetAngleY;                /* 0x434 */
    u8  pad_436[0x6];
    u8  mIsFlying;                    /* 0x43c -- 1 draws and animates mModelAnim1 */

    /* --- vtable --- */
    virtual ~daBasabasa_c();

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual void  OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    /* Coined names. SetState enters a state; the four states follow, each
       as its enter and execute pair. */
    int SetState(State *state);
    int EnterReturn();      /* fly back to the home position */
    int ExecuteReturn();
    int EnterChase();       /* fly at the closest player */
    int ExecuteChase();
    int EnterDrop();        /* let go of the ceiling */
    int ExecuteDrop();
    int EnterPerch();       /* hang at home until a player comes near */
    int ExecutePerch();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daBasabasa_c_size_must_be_0x440[sizeof(daBasabasa_c) == 0x440 ? 1 : -1];
#endif

#endif /* DABASABASA_C_H */
