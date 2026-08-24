#ifndef SPINY_H
#define SPINY_H

#include "types.h"
#include "dActor_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   Spiny_Spawn  fBase_c::operator new(1004 = 0x3ec), dActor_c::dActor_c(), stores _ZTV5Spiny,
 *                then the five members below in this order.
 *   ~Spiny       the same members destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0x3ec is the factory's own literal, and the trailing byte fields close exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c (relocs.txt, ov077). Only the
 * slots declared below differ; every other slot holds the base's own word and is inherited,
 * so it is deliberately not redeclared here.
 */
struct Spiny : dActor_c {
    u8  pad_0d0[0x4];
    Model mModel;                            /* 0x0d4 */
    ModelAnim mModelAnim;                    /* 0x124 */
    ShadowModel mShadowModel;                /* 0x188 */
    dCcAc_c mdCcAc_c;  /* 0x1b0 */
    dBgCh_Actr mWithMeshClsn;              /* 0x1e4 */
    /* InitResources assigns IDENTITY_MATRIX4X3 into this slot, so it begins a
       Matrix4x3. Still spelt u8 + pad so the header need not pull in
       math/Matrix.h. [_ZN5Spiny13InitResourcesEv.cpp] */
    u8  mMatrix;            /* 0x3a0 */
    u8  pad_3a1[0x37];
    /* Render draws the still Model in states 0 and 4 and the ModelAnim
       otherwise; Behavior treats 1 (only once on the ground), 4 and 5 as states
       that must keep running whatever the distance to the player.
       [_ZN5Spiny6RenderEv.cpp, _ZN5Spiny8BehaviorEv.cpp] */
    s32 mState;            /* 0x3d8 */
    u8  pad_3dc[0xd];
    /* Seeded 0x2c (44 frames) in InitResources and counted down ONLY on the
       frames Spiny is too far from the player to behave; at 0 it marks itself
       for destruction. [_ZN5Spiny13InitResourcesEv.cpp,
        _ZN5Spiny8BehaviorEv.cpp] */
    u8  mDespawnTimer;            /* 0x3e9 */
    u8  pad_3ea[0x2];

    virtual ~Spiny();            /* slots 16 (D1), 17 (D0) */

    virtual int   OnYoshiTryEat();               /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual int   OnAimedAtWithEgg();            /* slot 29 */

    int Behavior();
    int CleanupResources();                  /* slot  3 */
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();
};

typedef char Spiny_size_must_be_0x3ec[sizeof(Spiny) == 0x3ec ? 1 : -1];

#endif /* SPINY_H */
