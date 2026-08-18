#ifndef POKEY_H
#define POKEY_H

#include "types.h"
#include "dActor_c.h"
#include "Model.h"
#include "ShadowModel.h"
#include "MovingCylinderClsn.h"
#include "WithMeshClsn.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   Pokey_Spawn  fBase_c::operator new(944 = 0x3b0), dActor_c::dActor_c(), stores _ZTV5Pokey,
 *                then the four members below in this order.
 *   ~Pokey       the same members destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0x3b0 is the factory's own literal, and the trailing byte fields close exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c (relocs.txt, ov096). Only the
 * slots declared below differ; every other slot holds the base's own word and is inherited,
 * so it is deliberately not redeclared here.
 */
struct Pokey : dActor_c {
    u8  pad_0d0[0x4];
    Model mModel;                            /* 0x0d4 */
    ShadowModel mShadowModel;                /* 0x124 */
    MovingCylinderClsn mMovingCylinderClsn;  /* 0x14c */
    WithMeshClsn mWithMeshClsn;              /* 0x180 */
    u8  unk_33c;            /* 0x33c */
    u8  pad_33d[0x2f];
    s32 unk_36c;            /* 0x36c */
    s32 unk_370;            /* 0x370 */
    s32 unk_374;            /* 0x374 */
    u8  pad_378[0x14];
    s32 unk_38c;            /* 0x38c */
    s32 unk_390;            /* 0x390 */
    s32 unk_394;            /* 0x394 */
    u8  pad_398[0x10];
    u8  unk_3a8;            /* 0x3a8 */
    u8  pad_3a9[0x7];

    virtual ~Pokey();            /* slots 16 (D1), 17 (D0) */

    virtual int   OnYoshiTryEat();               /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual int   OnAimedAtWithEgg();            /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    void OnPendingDestroy();
};

typedef char Pokey_size_must_be_0x3b0[sizeof(Pokey) == 0x3b0 ? 1 : -1];

#endif /* POKEY_H */
