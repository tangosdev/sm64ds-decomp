#ifndef YOSHIEGG_H
#define YOSHIEGG_H

#include "types.h"
#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

/* daYegg_c in the ROM's RTTI. Derives from dEnemyBase_c, and both witnesses agree:
 * YoshiEgg_Spawn allocates 0x42c, calls _ZN12dEnemyBase_cC2Ev, stores _ZTV8YoshiEgg and
 * constructs the four members below in order; _ZN8YoshiEggD1Ev destroys the same four
 * in reverse and chains to _ZN12dEnemyBase_cD2Ev.
 *
 * SIZE 0x42c, the literal in the factory's fBase_c::operator new. ShadowModel ends
 * at 0x38c, so everything below that is this class's own.
 *
 * The old flat header also carried a marker at 0x350 called mAnimation. That is not a
 * member of this class at all -- ModelAnim derives from BOTH Model and Animation, and
 * the Animation base sits at +0x50, so 0x300 + 0x50 is mModelAnim's Animation
 * subobject. It disappears here because the type expresses it.
 */
struct YoshiEgg : dEnemyBase_c {
    dCcAc_c  mdCcAc_c;    /* 0x110 */
    dBgCh_Actr        mWithMeshClsn;          /* 0x144 */
    ModelAnim           mModelAnim;             /* 0x300 */
    ShadowModel         mShadowModel;           /* 0x364 */
    /* The ROM loads this WORD and passes it to _ZN6Player16IsInsideOfCannonEv as that
       function's `this`, which is an object address -- so the word is a Player *. It
       says nothing about the 0x60 below it, which stays explicit padding. */
    Player             *mPlayer;                /* 0x38c */
    u8  pad_390[0x30];
    /* InitResources copies the actor's own position here once, and nothing
       migrated writes it again. */
    s32 mSpawnPosX;                             /* 0x3c0 */
    s32 mSpawnPosY;                             /* 0x3c4 */
    s32 mSpawnPosZ;                             /* 0x3c8 */
    u8  pad_3cc[0x18];
    /* TWO s16 triples, and InitResources seeds BOTH from mAngleX/Y/Z with the
       same three loads. Nothing in any migrated function tells them apart, so
       neither gets a name it has not earned. */
    s16 unk_3e4;                                /* 0x3e4 */
    s16 unk_3e6;                                /* 0x3e6 */
    s16 unk_3e8;                                /* 0x3e8 */
    s16 unk_3ea;                                /* 0x3ea */
    s16 unk_3ec;                                /* 0x3ec */
    s16 unk_3ee;                                /* 0x3ee */
    s32 unk_3f0;                                /* 0x3f0 -- param1 & 3; picks the
                                                   collision size, and Behavior
                                                   treats 1 as its own case */
    u8  pad_3f4[0x8];
    s32 unk_3fc[5];                             /* 0x3fc -- zeroed as a run of five */
    u8  pad_410[0xf];
    u8  unk_41f;                                /* 0x41f */
    /* Behavior indexes unk_421 with unk_420 and stops at 5, which is what makes
       the array five wide and puts the byte at 0x426 outside it. */
    u8  unk_420;                                /* 0x420 */
    u8  unk_421[5];                             /* 0x421 */
    u8  pad_426[0x1];
    u8  mStarSlot;                              /* 0x427 -- dActor_c::TrackStar's answer,
                                                   the same shape as daIDonketu_c's */
    u8  unk_428;                                /* 0x428 -- param1 >> 4 */
    u8  pad_429[0x3];

    virtual ~YoshiEgg();

    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char YoshiEgg_size_must_be_0x42c[sizeof(YoshiEgg) == 0x42c ? 1 : -1];

#endif /* YOSHIEGG_H */
