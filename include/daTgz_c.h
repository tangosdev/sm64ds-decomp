#ifndef DATGZ_C_H
#define DATGZ_C_H

#include "types.h"
#include "dActor_c.h"
#include "dExtSimpleModel_c.h"
#include "dExtAnmModel_c.h"
#include "dExtShadowModel_c.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

/* daTgz_c -- Spiny (TOGEZO). sizeof is the factory literal 1004 (0x3ec).
 *
 * daTgz_c_classInit does fBase_c::operator new(1004), dActor_c::C2, stores
 * _ZTV7daTgz_c, then constructs dExtSimpleModel_c, dExtAnmModel_c, dExtShadowModel_c, dCcAc_c and
 * dBgCh_Actr in that order. ~daTgz_c destroys those five in reverse. The
 * pointer at 0xd0 is not constructed.
 *
 * The vtable was diffed against _ZTV8dActor_c. Only the slots declared below
 * differ; every other slot is the base's word and is not redeclared.
 *
 * RTTI names this class daTgz_c. The factory (historical alias Spiny_Spawn)
 * builds it for the TOGEZO profile.
 */
struct daTgz_c : dActor_c {
    dExtSimpleModel_c mModel;            /* 0x0d4 */
    dExtAnmModel_c mAnmModel;            /* 0x124 */
    dExtShadowModel_c mShadowModel;            /* 0x188 */
    dCcAc_c mdCcAc_c;                    /* 0x1b0 */
    dBgCh_Actr mWithMeshClsn;            /* 0x1e4 */
    /* Drop-shadow matrix. InitResources copies IDENTITY_MATRIX4X3 here and
       the shadow helper writes the translation as position >> 3. */
    Matrix4x3 mShadowMatrix; /* 0x3a0 */
    /* PMF pair. SetState stores &gTgzStates[i] (d_a_tgz.cpp).
       CallStateEnter calls slot 0; CallStateUpdate calls slot 1. */
    void *mStateDesc;                    /* 0x3d0 */
    /* daPly_c stored when a hit sends the Spiny into state 2 (Knockback),
       which reads it once for the heading away from him and then clears it.
       The "Chase" in the name is a coined guess that the body does not bear
       out; the state hops away from this player, not after him. */
    daPly_c *mChasePlayer; /* 0x3d4 */
    /* Render draws the still dExtSimpleModel_c in states 0 and 4 and the dExtAnmModel_c
       otherwise. Behavior keeps states 4 and 5 running however far the
       player is, and state 1 only once it is on the ground. */
    s32 mState;                          /* 0x3d8 */
    /* 0 until a water surface is found under the Spiny, then that height. */
    s32 mWaterY;                         /* 0x3dc */
    /* Particle::System::New id for the ripple while underwater. */
    u32 mRippleId;                       /* 0x3e0 */
    u8 mInWater;                         /* 0x3e4 */
    u8 pad_3e5;                          /* 0x3e5 */
    /* ApproachLinear target for mAngle.y while walking. */
    s16 mTurnTarget;                     /* 0x3e6 */
    /* DecIfAbove0_Byte. State 5 arms 45 frames; state 1 arms a random byte. */
    u8 mActionTimer;                     /* 0x3e8 */
    /* Seeded 0x2c (44 frames) in InitResources and counted down only while
       the Spiny is too far from the player to behave. At 0 it is destroyed. */
    u8 mDespawnTimer;                    /* 0x3e9 */
    u8 pad_3ea[2];                       /* 0x3ea */

    virtual ~daTgz_c();                  /* slots 16 (D1), 17 (D0) */

    virtual int   OnYoshiTryEat();               /* slot 18 */
    virtual void OnTurnIntoEgg(daPly_c &player); /* slot 19 */
    virtual int   OnAimedAtWithEgg();            /* slot 29 */

    int Behavior();
    int CleanupResources();              /* slot  3 */
    int InitResources();
    void OnPendingDestroy();             /* slot 12 -- empty body in the ROM */
    int Render();

    /* State handlers, named by gTgzStates in d_a_tgz.cpp. */
    int St_KnockedAway_Main();
    int St_KnockedAway_Init();
    int St_Spit_Main();
    int St_Spit_Init();
    int St_YoshiMouth_Main();
    int St_YoshiMouth_Init();
    int St_Knockback_Main();
    int St_Knockback_Init();
    int St_Walk_Main();
    void St_Walk_Init();
    int St_Spin_Main();
    int St_Spin_Init();
    int GetHeightAboveWater();
    int IsUnderwater();
    void UpdateGroundCollision(dBgCh_Actr *clsn);
    void HandlePlayerCollision();
    void HandlePlayerCollisionSpinning();
    void UpdateTumbleMatrix();
    void UpdateModelMatrix();
    void UpdateMatrixAndShadow();
    void CallStateUpdate();
    void CallStateEnter();
    void SetState(int i);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daTgz_c_size_must_be_0x3ec[sizeof(daTgz_c) == 0x3ec ? 1 : -1];
#endif

#endif /* DATGZ_C_H */
