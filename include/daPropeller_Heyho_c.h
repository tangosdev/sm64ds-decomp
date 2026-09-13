#ifndef DAPROPELLER_HEYHO_C_H
#define DAPROPELLER_HEYHO_C_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN19daPropeller_Heyho_cD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `dEnemyBase_c::~dEnemyBase_c`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x110 dCcAc_c         0x34   -> 0x144
 *     0x144 dBgCh_Actr               0x1bc  -> 0x300
 *     0x300 ModelAnim                  0x64   -> 0x364
 *     0x364 ShadowModel                0x28   -> 0x38c
 *
 * Member NAMES are the ones this header already used -- a rebase should not
 * also rename things its callers spell.
 *
 * SIZE IS THE ROM'S OWN, not a rounded-up field span: `daPropeller_Heyho_c_classInit` calls
 * `fBase_c::operator new(1000)` -- 0x3e8 -- and stores `_ZTV19daPropeller_Heyho_c`,
 * so that literal IS this class's sizeof. mTargetAngY closes the object at 0x3e8.
 *
 * THE NAME IS THE CARTRIDGE'S OWN. ov070 0x0212312c holds the length-prefixed
 * string `19daPropeller_Heyho_c`. Fly Guy is the English enemy; the class is
 * not a sibling of daKrpa_c / daKpFr_c in this mixed overlay.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "TextureTransformer.h"
#include "dBgCh_Actr.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daPropeller_Heyho_c : dEnemyBase_c {
    /* What mCurrentState points at. ChangeState writes the pointer and calls
       the init PMF at +0x00 when it is non-zero; Behavior calls the main PMF
       at +0x08. Both it and the cooldown at 0x3cc were reachable only as raw
       `c + 0x...`, so the generated header never had them. */
    struct State {
        int  (daPropeller_Heyho_c::*mInit)();  /* 0x00 */
        void (daPropeller_Heyho_c::*mMain)();  /* 0x08 */
    };

    dCcAc_c           mdCcAc_c;   /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x144 */
    ModelAnim                    mModelAnim;            /* 0x300 */
    ShadowModel                  mShadowModel;          /* 0x364 */
    /* 0x30-byte block copy from data_020a0e68 in func_ov070_02120070; that
       helper then passes it to DropShadowRadHeight. Left as twelve words
       rather than Matrix4x3: Vector3's destructor would change D1/D0.
       [func_ov070_02120070, in src/game/actors/d_a_propeller_heyho.cpp] */
    s32                          mShadowMatrix[12];     /* 0x38c */
    State                       *mCurrentState;         /* 0x3bc */
    /* InitResources copies mPosX/Y/Z here. Wander/chase compare Vec3_Dist
       against this triple; several states rewrite it from the live position
       and add a Y lift (0xc8000 / 0x12c000). 0211fd98 only reads it;
       0211f62c writes `+= 0x12c000`.
       [InitResources / func_ov070_0211f62c / func_ov070_0211fae4 /
        func_ov070_0211f6e0, in src/game/actors/d_a_propeller_heyho.cpp] */
    s32                          mHomePosX;             /* 0x3c0 */
    s32                          mHomePosY;             /* 0x3c4 */
    s32                          mHomePosZ;             /* 0x3c8 */
    /* Attack cooldown. Behavior ticks it with DecIfAbove0_Short; wander
       early-outs while it is non-zero; fire/dive exits store 0x5a.
       [Behavior / func_ov070_0211fd98 / func_ov070_0211f48c /
        func_ov070_0211f62c / func_ov070_0211f6e0,
        in src/game/actors/d_a_propeller_heyho.cpp] */
    u16                          mCooldown;             /* 0x3cc */
    u8  pad_3ce[0x2];
    /* Particle uniqueIDs: System::New 0x13a and NewUnkCallback818 0x13b in
       the defeated-state helper, only while mStateStep is set.
       [func_ov070_0211f368, in src/game/actors/d_a_propeller_heyho.cpp] */
    u32                          mParticle0;            /* 0x3d0 */
    u32                          mParticle1;            /* 0x3d4 */
    /* Per-state step/sub-phase word. Dive init (0211fa80), retreat init
       (0211f694), and return-to-wander (0211f62c) write 0; 0211f100 writes
       1 when the HURT anim starts.
       [func_ov070_0211fa80 / func_ov070_0211f694 / func_ov070_0211f62c /
        func_ov070_0211f100, in src/game/actors/d_a_propeller_heyho.cpp] */
    s32                          mStateStep;            /* 0x3d8 */
    /* Set 1 when collision starts the hurt anim during the hover-attack
       state; only the dive init (0211fa80) clears it. 0211f694 only reads
       it. Dive's finished-anim path only returns to wander while this is 1.
       [func_ov070_0211f100 / func_ov070_0211fa80 / func_ov070_0211f6e0 /
        func_ov070_0211f694, in src/game/actors/d_a_propeller_heyho.cpp] */
    s32                          mHitDuringAttack;      /* 0x3dc */
    /* param1 & 0xff, with 0xff folded to 0. Zero disables the fireball
       state; non-zero is a 50% roll in the chase helper.
       [InitResources / func_ov070_0211fae4,
        in src/game/actors/d_a_propeller_heyho.cpp] */
    s32                          mCanSpitFire;          /* 0x3e0 */
    u8  pad_3e4[0x2];
    /* Heading toward the player or home. Chase/fire/dive inits and
       ApproachAngle targets write it.
       [func_ov070_0211fa80 / func_ov070_0211fae4 / func_ov070_0211fd98 /
        func_ov070_0211f48c / func_ov070_0211f6e0,
        in src/game/actors/d_a_propeller_heyho.cpp] */
    s16                          mTargetAngY;           /* 0x3e6 */

    /* --- vtable --- */
    virtual ~daPropeller_Heyho_c() {}

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual void  OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

/* Inline destructor is not the key function; OnYoshiTryEat is the first
 * out-of-line virtual, so this TU emits the vtable. The factory is
 * `return new daPropeller_Heyho_c()`; the leaf size_t operator new
 * forwards to `_ZN7fBase_cnwEj` until #2570's fBase overload lands. */
extern int _ZTV19daPropeller_Heyho_c[];

typedef char daPropeller_Heyho_c_State_size_must_be_0x10[
    sizeof(daPropeller_Heyho_c::State) == 0x10 ? 1 : -1];
typedef char daPropeller_Heyho_c_size_must_be_0x3e8[
    sizeof(daPropeller_Heyho_c) == 0x3e8 ? 1 : -1];

#endif /* DAPROPELLER_HEYHO_C_H */
