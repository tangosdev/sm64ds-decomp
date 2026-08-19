#ifndef LAKITU_H
#define LAKITU_H

#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "Model.h"
#include "ShadowModel.h"
#include "TextureSequence.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   Lakitu_Spawn  fBase_c::operator new(1056 = 0x420), dActor_c::dActor_c(), stores _ZTV6Lakitu,
 *                 then the six members below in this order.
 *   ~Lakitu       the same members destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0x420 is the factory's own literal, and the trailing byte fields close exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c (relocs.txt, ov077). Only the
 * slots declared below differ; every other slot holds the base's own word and is inherited,
 * so it is deliberately not redeclared here.
 */
struct Lakitu : dActor_c {
    u8  pad_0d0[0x4];
    ModelAnim mModelAnim;                                /* 0x0d4 */
    Model mModel;                                        /* 0x138 */
    ShadowModel mShadowModel;                            /* 0x188 */
    TextureSequence mTextureSequence;                    /* 0x1b0 */
    dCcAcPos_c mdCcAcPos_c; /* 0x1c4 */
    dBgCh_Actr mWithMeshClsn;                          /* 0x204 */
    u8  unk_3c0;            /* 0x3c0 */
    u8  pad_3c1[0x33];
    s32 unk_3f4;            /* 0x3f4 */
    /* Was declared as a u8 marker; InitResources writes/reads it as a full
       word (a copy of mPosX), and it directly abuts unk_3fc with no gap. */
    s32 unk_3f8;            /* 0x3f8 */
    /* Was declared as a u8 marker; InitResources writes/reads it as a full
       word (a copy of mPosY), and it directly abuts unk_400 with no gap. */
    s32 unk_3fc;            /* 0x3fc */
    s32 unk_400;            /* 0x400 */
    u8  pad_404[0xc];
    s32 unk_410;            /* 0x410 */
    u8  pad_414[0xc];

    virtual ~Lakitu();            /* slots 16 (D1), 17 (D0) */

    virtual int   OnYoshiTryEat();               /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual int   OnAimedAtWithEgg();            /* slot 29 */

    int Behavior();
    int InitResources();
    int Render();
    int CleanupResources();
    void OnPendingDestroy();
};

typedef char Lakitu_size_must_be_0x420[sizeof(Lakitu) == 0x420 ? 1 : -1];

#endif /* LAKITU_H */
