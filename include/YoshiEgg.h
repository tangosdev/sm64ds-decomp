#ifndef YOSHIEGG_H
#define YOSHIEGG_H

#include "types.h"
#include "Enemy.h"
#include "ModelAnim.h"
#include "MovingCylinderClsn.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

/* daYegg_c in the ROM's RTTI. Derives from Enemy, and both witnesses agree:
 * YoshiEgg_Spawn allocates 0x42c, calls _ZN5EnemyC2Ev, stores _ZTV8YoshiEgg and
 * constructs the four members below in order; _ZN8YoshiEggD1Ev destroys the same four
 * in reverse and chains to _ZN5EnemyD2Ev.
 *
 * SIZE 0x42c, the literal in the factory's ActorBase::operator new. ShadowModel ends
 * at 0x38c, so everything below that is this class's own.
 *
 * The old flat header also carried a marker at 0x350 called mAnimation. That is not a
 * member of this class at all -- ModelAnim derives from BOTH Model and Animation, and
 * the Animation base sits at +0x50, so 0x300 + 0x50 is mModelAnim's Animation
 * subobject. It disappears here because the type expresses it.
 */
struct YoshiEgg : Enemy {
    MovingCylinderClsn  mMovingCylinderClsn;    /* 0x110 */
    WithMeshClsn        mWithMeshClsn;          /* 0x144 */
    ModelAnim           mModelAnim;             /* 0x300 */
    ShadowModel         mShadowModel;           /* 0x364 */
    /* The ROM loads this WORD and passes it to _ZN6Player16IsInsideOfCannonEv as that
       function's `this`, which is an object address -- so the word is a Player *. It
       says nothing about the 0x60 below it, which stays explicit padding. */
    Player             *mPlayer;                /* 0x38c */
    u8  pad_390[0x60];
    s32 unk_3f0;                                /* 0x3f0 */
    u8  pad_3f4[0x2c];
    u8  unk_420;                                /* 0x420 */
    u8  pad_421[0xb];

    virtual ~YoshiEgg();

    /* methods */
    int Behavior();
    int CleanupResources();
    int Render();
};

typedef char YoshiEgg_size_must_be_0x42c[sizeof(YoshiEgg) == 0x42c ? 1 : -1];

#endif /* YOSHIEGG_H */
