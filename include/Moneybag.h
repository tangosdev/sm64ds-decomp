#ifndef MONEYBAG_H
#define MONEYBAG_H

#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "Model.h"
#include "ShadowModel.h"
#include "MovingCylinderClsn.h"
#include "WithMeshClsn.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   Moneybag_Spawn  fBase_c::operator new(1012 = 0x3f4), dActor_c::dActor_c(), stores _ZTV8Moneybag,
 *                   then the five members below in this order.
 *   ~Moneybag       the same members destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0x3f4 is the factory's own literal, and the trailing byte fields close exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c (relocs.txt, ov081). Only the
 * slots declared below differ; every other slot holds the base's own word and is inherited,
 * so it is deliberately not redeclared here.
 */
struct Moneybag : dActor_c {
    u8  pad_0d0[0x4];
    ModelAnim mModelAnim;                    /* 0x0d4 */
    Model mModel;                            /* 0x138 */
    ShadowModel mShadowModel;                /* 0x188 */
    MovingCylinderClsn mMovingCylinderClsn;  /* 0x1b0 */
    WithMeshClsn mWithMeshClsn;              /* 0x1e4 */
    u8  unk_3a0;            /* 0x3a0 */
    u8  pad_3a1[0x2f];
    s32 unk_3d0;            /* 0x3d0 */
    s32 unk_3d4;            /* 0x3d4 */
    s32 unk_3d8;            /* 0x3d8 */
    u8  pad_3dc[0x14];
    u8  unk_3f0;            /* 0x3f0 */
    u8  pad_3f1[0x3];

    virtual ~Moneybag();            /* slots 16 (D1), 17 (D0) */

    virtual int   OnYoshiTryEat();               /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual int   OnAimedAtWithEgg();            /* slot 29 */

    int Behavior();
    int InitResources();
    int Render();
};

typedef char Moneybag_size_must_be_0x3f4[sizeof(Moneybag) == 0x3f4 ? 1 : -1];

#endif /* MONEYBAG_H */
