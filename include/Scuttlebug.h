#ifndef SCUTTLEBUG_H
#define SCUTTLEBUG_H

#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   Scuttlebug_Spawn  fBase_c::operator new(940 = 0x3ac), dActor_c::dActor_c(), stores _ZTV10Scuttlebug,
 *                 then the four members below in this order.
 *   ~Scuttlebug   the same members destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0x3ac is the factory's own literal, and the trailing byte fields close exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c (relocs.txt, ov071). Only the
 * slots declared below differ; every other slot holds the base's own word and is inherited,
 * so it is deliberately not redeclared here.
 */
struct Scuttlebug : dActor_c {
    u8  pad_0d0[0x4];
    ModelAnim mModelAnim;                    /* 0x0d4 */
    ShadowModel mShadowModel;                /* 0x138 */
    dCcAc_c mdCcAc_c;  /* 0x160 */
    dBgCh_Actr mWithMeshClsn;              /* 0x194 */
    u8  pad_350[0x58];
    /* Counted down once per frame at the top of Behavior with
       DecIfAbove0_Short -- as a HALFWORD, though the generated header typed the
       slot u8; the result is discarded there, so what expiring means is up to
       the not-yet-enrolled helpers of this class.
       [_ZN10Scuttlebug8BehaviorEv.cpp] */
    u8  mTimer;                             /* 0x3a8 */
    u8  pad_3a9[0x3];

    virtual ~Scuttlebug();            /* slots 16 (D1), 17 (D0) */

    virtual int   OnYoshiTryEat();               /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual int   OnAimedAtWithEgg();            /* slot 29 */

    int Behavior();
    int CleanupResources();                  /* slot  3 */
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();
};

typedef char Scuttlebug_size_must_be_0x3ac[sizeof(Scuttlebug) == 0x3ac ? 1 : -1];

#endif /* SCUTTLEBUG_H */
