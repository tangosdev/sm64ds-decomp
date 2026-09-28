#ifndef DATGZ_C_H
#define DATGZ_C_H

#include "types.h"
#include "dActor_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

/* daTgz_c -- Spiny (TOGEZO). sizeof is the factory literal 1004 (0x3ec).
 *
 * daTgz_c_classInit does fBase_c::operator new(1004), dActor_c::C2, stores
 * _ZTV7daTgz_c, then constructs Model, ModelAnim, ShadowModel, dCcAc_c and
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
    /* Actor holding this Spiny. State 4 copies its position and yaw, then
       clears the pointer once the Spiny is thrown. */
    dActor_c *mCarrier;                  /* 0x0d0 */
    Model mModel;                        /* 0x0d4 */
    ModelAnim mModelAnim;                /* 0x124 */
    ShadowModel mShadowModel;            /* 0x188 */
    dCcAc_c mdCcAc_c;                    /* 0x1b0 */
    dBgCh_Actr mWithMeshClsn;            /* 0x1e4 */
    /* Drop-shadow matrix. InitResources copies IDENTITY_MATRIX4X3 here and
       the shadow helper writes the translation as position >> 3. Twelve
       words, not Matrix4x3: this header must not pick a Matrix4x3 spelling. */
    s32 mShadowMatrix[12];               /* 0x3a0 */
    /* PMF pair. func_ov077_02125e94 stores data_ov077_02127c28 + i*16.
       func_ov077_02125e5c calls slot 0; func_ov077_02125e20 calls slot 1. */
    void *mStateDesc;                    /* 0x3d0 */
    /* Player stored when a hit sends the Spiny into state 2. */
    Player *mChasePlayer;                /* 0x3d4 */
    /* Render draws the still Model in states 0 and 4 and the ModelAnim
       otherwise. Behavior keeps states 4 and 5 running however far the
       player is, and state 1 only once it is on the ground. */
    s32 mState;                          /* 0x3d8 */
    /* 0 until a water surface is found under the Spiny, then that height. */
    s32 mWaterY;                         /* 0x3dc */
    /* Particle::System::New id for the ripple while underwater. */
    u32 mRippleId;                       /* 0x3e0 */
    u8 mInWater;                         /* 0x3e4 */
    u8 pad_3e5;                          /* 0x3e5 */
    /* ApproachLinear target for mAngleY while walking. */
    s16 mTurnTarget;                     /* 0x3e6 */
    /* DecIfAbove0_Byte. State 5 arms 45 frames; state 1 arms a random byte. */
    u8 mActionTimer;                     /* 0x3e8 */
    /* Seeded 0x2c (44 frames) in InitResources and counted down only while
       the Spiny is too far from the player to behave. At 0 it is destroyed. */
    u8 mDespawnTimer;                    /* 0x3e9 */
    u8 pad_3ea[2];                       /* 0x3ea */

    virtual ~daTgz_c();                  /* slots 16 (D1), 17 (D0) */

    virtual int   OnYoshiTryEat();               /* slot 18 */
    virtual void  OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual int   OnAimedAtWithEgg();            /* slot 29 */

    int Behavior();
    int CleanupResources();              /* slot  3 */
    int InitResources();
    void OnPendingDestroy();             /* slot 12 -- empty body in the ROM */
    int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daTgz_c_size_must_be_0x3ec[sizeof(daTgz_c) == 0x3ec ? 1 : -1];
#endif

#endif /* DATGZ_C_H */
