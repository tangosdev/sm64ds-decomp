#ifndef POKEY_H
#define POKEY_H

#include "types.h"
#include "dActor_c.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

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
    dCcAc_c mdCcAc_c;  /* 0x14c */
    dBgCh_Actr mWithMeshClsn;              /* 0x180 */
    /* 0x33c..0x36b is exactly 0x30 bytes and InitResources assigns
       IDENTITY_MATRIX4X3 straight into it, so this slot plus its pad is one
       Matrix4x3. Still spelt u8 + pad because giving it the real type would
       drag math/Matrix.h into every includer of this header.
       [_ZN5Pokey13InitResourcesEv.cpp] */
    u8  mMatrix;            /* 0x33c */
    u8  pad_33d[0x2f];
    /* A Pokey is two actors: the head (actorID 0xf0) and its body segments
       (actorID 0xf1). The head seeds mRootPos from its OWN mPosX/Y/Z; a segment
       finds the head with dActor_c::FindWithID(param1), keeps it in mHead, and
       copies the head's mRootPos triple word for word out of the head object at
       this same 0x36c offset. So every actor in one Pokey carries the same root
       position. [_ZN5Pokey13InitResourcesEv.cpp] */
    s32 mRootPosX;            /* 0x36c */
    s32 mRootPosY;            /* 0x370 */
    s32 mRootPosZ;            /* 0x374 */
    u8  pad_378[0x14];
    /* Behavior early-outs on distance from the player UNLESS mState is 2 or 5,
       which keep running however far away the player is.
       [_ZN5Pokey8BehaviorEv.cpp] */
    s32 mState;            /* 0x38c */
    /* mHead is a dActor_c* to the 0xf0 head, spelt s32 and cast at every use;
       0 on the head itself. mNextSegment chains the segments: OnPendingDestroy
       (head only) walks p = mNextSegment, then p->mNextSegment at the same
       0x394 offset, tearing each one down.
       [_ZN5Pokey13InitResourcesEv.cpp, _ZN5Pokey16OnPendingDestroyEv.cpp] */
    s32 mHead;            /* 0x390 */
    s32 mNextSegment;            /* 0x394 */
    u8  pad_398[0x10];
    /* Set to 1 by the head only, after it loads the blue-coin model; no
       enrolled body reads it back. [_ZN5Pokey13InitResourcesEv.cpp] */
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
