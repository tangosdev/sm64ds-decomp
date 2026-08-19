#ifndef SPINY_H
#define SPINY_H

#include "types.h"
#include "dActor_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "MovingCylinderClsn.h"
#include "WithMeshClsn.h"

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
    MovingCylinderClsn mMovingCylinderClsn;  /* 0x1b0 */
    WithMeshClsn mWithMeshClsn;              /* 0x1e4 */
    u8  unk_3a0;            /* 0x3a0 */
    u8  pad_3a1[0x37];
    s32 unk_3d8;            /* 0x3d8 */
    u8  pad_3dc[0xd];
    u8  unk_3e9;            /* 0x3e9 */
    u8  pad_3ea[0x2];

    virtual ~Spiny();            /* slots 16 (D1), 17 (D0) */

    virtual int   OnYoshiTryEat();               /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual int   OnAimedAtWithEgg();            /* slot 29 */

    int Behavior();
    int InitResources();
    int Render();
};

typedef char Spiny_size_must_be_0x3ec[sizeof(Spiny) == 0x3ec ? 1 : -1];

#endif /* SPINY_H */
