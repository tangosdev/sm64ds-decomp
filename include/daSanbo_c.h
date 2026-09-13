#ifndef DASANBO_C_H
#define DASANBO_C_H

#include "types.h"
#include "dActor_c.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daSanbo_c_classInit_SANBO  fBase_c::operator new(944 = 0x3b0), dActor_c::dActor_c(), stores _ZTV9daSanbo_c,
 *                then the four members below in this order.
 *   ~daSanbo_c       the same members destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0x3b0 is the factory's own literal, and the trailing byte fields close exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c (relocs.txt, ov096). Only the
 * slots declared below differ; every other slot holds the base's own word and is inherited,
 * so it is deliberately not redeclared here.
 *
 * SM64DS RTTI names the implementation daSanbo_c. The reconstructed
 * factory daSanbo_c_classInit_SANBO (historical alias
 * Pokey_Spawn) constructs it for the SANBO
 * registry profile.
 */
extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daSanbo_c : dActor_c {
    /* Dispatch ignores results; void is a reconstructed interface. The state
       handlers retain their existing external identities until methodization. */
    typedef void (daSanbo_c::*StateFunc)();

    u8  pad_0d0[0x4];
    Model mModel;                            /* 0x0d4 */
    ShadowModel mShadowModel;                /* 0x124 */
    dCcAc_c mdCcAc_c;  /* 0x14c */
    dBgCh_Actr mWithMeshClsn;              /* 0x180 */
    Matrix4x3 mMatrix;      /* 0x33c -- initialized from IDENTITY_MATRIX4X3 */
    /* A daSanbo_c is two actors: the head (actorID 0xf0) and its body segments
       (actorID 0xf1). The head seeds mRootPos from its OWN mPosX/Y/Z; a segment
       finds its previous segment with dActor_c::FindWithID(param1), and copies
       that segment's mRootPos triple at this same 0x36c offset. Each segment
       therefore inherits the head's root position through the chain.
       [InitResources, in src/actors/daSanbo_c.cpp] */
    s32 mRootPosX;            /* 0x36c */
    s32 mRootPosY;            /* 0x370 */
    s32 mRootPosZ;            /* 0x374 */
    /* Segment-follow offset: sin/cos of mSegAngY * 0xe000. func_ov096_02136754
       / func_ov096_02136264 write it; func_ov096_02135efc adds it to pos for
       the model translation. */
    s32 mOffsetX;             /* 0x378 */
    s32 mOffsetY;             /* 0x37c */
    s32 mOffsetZ;             /* 0x380 */
    StateFunc *mStateFunctions; /* 0x384 -- current entry/update pair */
    dActor_c *mHitActor;        /* 0x388 */
    /* Behavior early-outs on distance from the player UNLESS mState is 2 or 5,
       which keep running however far away the player is.
       [Behavior, in src/actors/daSanbo_c.cpp] */
    s32 mState;            /* 0x38c */
    /* The head has no previous segment. Unlinking a body segment updates both
       neighbors; finding the head requires following mPrevSegment. */
    daSanbo_c *mPrevSegment;    /* 0x390 */
    daSanbo_c *mNextSegment;    /* 0x394 */
    u8  pad_398[0x8];
    /* Particle uniqueIDs: System::New 0x13a and NewUnkCallback818 0x13b in
       func_ov096_02136434 / func_ov096_02135948. */
    s32 mParticle0;             /* 0x3a0 */
    s32 mParticle1;             /* 0x3a4 */
    /* Set by the head after loading the blue-coin model. func_ov096_0213670c
       reads it through the head to choose a zero or 90-frame regrowth delay;
       func_ov096_021365d4 clears it after the segment count reaches three. */
    u8  unk_3a8;            /* 0x3a8 */
    u8  pad_3a9[0x1];
    /* Heading copied from the previous segment + 0x13000; feeds the sin table
       that fills mOffset*. */
    s16 mSegAngY;           /* 0x3aa */
    /* Regrowth / bounce countdown. func_ov096_0213670c stores 0 or 0x5a;
       func_ov096_02136134 stores 0x2d; DecIfAbove0_Byte ends those states. */
    u8  mTimer;             /* 0x3ac */
    u8  pad_3ad[0x3];

    /* Inline, and declared first: mwccarm emits the vague-linkage D1/D0 pair
       from this body, in the cartridge's D1-then-D0 order, and no D2. */
    virtual ~daSanbo_c() {
    }


    virtual int   OnYoshiTryEat();               /* slot 18 */
    virtual void  OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual int   OnAimedAtWithEgg();            /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    void OnPendingDestroy();

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

/* Inline destructor is the key function, so this TU emits the vtable. Both
 * factories are `return new daSanbo_c()`; the leaf size_t operator new
 * forwards to `_ZN7fBase_cnwEj` until #2570's fBase overload lands. */
extern int _ZTV9daSanbo_c[];

typedef char daSanbo_c_size_must_be_0x3b0[sizeof(daSanbo_c) == 0x3b0 ? 1 : -1];

#endif /* DASANBO_C_H */
