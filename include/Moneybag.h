#ifndef MONEYBAG_H
#define MONEYBAG_H

#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

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
    dCcAc_c mdCcAc_c;  /* 0x1b0 */
    dBgCh_Actr mWithMeshClsn;              /* 0x1e4 */
    /* InitResources assigns IDENTITY_MATRIX4X3 into this slot, and
       0x3a0..0x3cf is exactly the 0x30 bytes a Matrix4x3 occupies. Still spelt
       u8 + pad so the header need not pull in math/Matrix.h.
       [_ZN8Moneybag13InitResourcesEv.cpp] */
    u8  mMatrix;            /* 0x3a0 */
    u8  pad_3a1[0x2f];
    /* Copy of mPosX/Y/Z taken once in InitResources.
       [_ZN8Moneybag13InitResourcesEv.cpp] */
    s32 mSpawnPosX;            /* 0x3d0 */
    s32 mSpawnPosY;            /* 0x3d4 */
    s32 mSpawnPosZ;            /* 0x3d8 */
    u8  pad_3dc[0x14];
    /* Set to 1 by InitResources. Render draws the ModelAnim only above 1 and
       the Model only at or below 0x1f, so the two overlap for 2..0x1f and the
       high values are a state in which neither is drawn.
       [_ZN8Moneybag13InitResourcesEv.cpp, _ZN8Moneybag6RenderEv.cpp] */
    u8  mState;            /* 0x3f0 */
    u8  pad_3f1[0x3];

    virtual ~Moneybag();            /* slots 16 (D1), 17 (D0) */

    virtual int   OnYoshiTryEat();               /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual int   OnAimedAtWithEgg();            /* slot 29 */

    int Behavior();
    int CleanupResources();                  /* slot  3 */
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();
};

typedef char Moneybag_size_must_be_0x3f4[sizeof(Moneybag) == 0x3f4 ? 1 : -1];

#endif /* MONEYBAG_H */
