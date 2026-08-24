#ifndef WHOMP_H
#define WHOMP_H

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN5WhompD1Ev` destroys each member, and
 * `Whomp_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV5Whomp`. Everything this header used to restate below
 * 0x110 belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 dBgCh_Actr               0x1bc   -> 0x2cc
 *     0x2cc ModelAnim                  0x64    -> 0x330
 *     0x330 TextureSequence            0x14    -> 0x344
 *     0x344 ShadowModel                0x28    -> 0x36c
 *     0x418 dBgW_KcMbg         0x1c8   -> 0x5e0
 *
 * Typing them absorbed markers that were their insides:
 *   - unk_338 = TextureSequence.currFrame
 *   - unk_33c = TextureSequence.speed
 *
 * Field provenance: notes/enemy-leaf-provenance.md.
 *
 * SIZE IS THE ROM'S OWN: `Whomp_Spawn` calls
 * `fBase_c::operator new(1552)` -- 0x610 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dBgW_KcMbg.h"
#include "ShadowModel.h"
#include "TextureSequence.h"
#include "dBgCh_Actr.h"

struct Whomp : dEnemyBase_c {
    dBgCh_Actr                 mWithMeshClsn;         /* 0x110 */
    ModelAnim                    mModelAnim;            /* 0x2cc */
    TextureSequence              mTextureSequence;      /* 0x330 */
    ShadowModel                  mShadowModel;          /* 0x344 */
    u8  pad_36c[0x30];
    /* InitResources zeroes the first four and then refills as many as
       data_0209f21c says from data_0209f394. Reached only through the
       `struct WithArr { char pad[0x39c]; s32 arr[8]; }` stand-in that file used
       to carry; declared here so the stand-in could go. */
    s32                          unk_39c[4];            /* 0x39c */
    u8  pad_3ac[0x4];
    /* Behavior's state index: it selects the per-state handler out of the
       pointer-to-member table data_ov079_02128280, and the king path skips its
       camera-target update while it reads 9. */
    s32                          mState;                /* 0x3b0 */
    u8  pad_3b4[0x8];
    /* InitResources copies mPosX/mPosY/mPosZ here once. */
    s32                          mSpawnPosX;            /* 0x3bc */
    s32                          mSpawnPosY;            /* 0x3c0 */
    s32                          mSpawnPosZ;            /* 0x3c4 */
    u8  pad_3c8[0xc];
    /* Behavior snapshots the live position into these every frame UNLESS
       dEnemyBase_c::IsGoingOffCliff says the next step leaves the ledge, in which
       case it restores the position FROM them instead. */
    s32                          mSafePosX;             /* 0x3d4 */
    s32                          mSafePosY;             /* 0x3d8 */
    s32                          mSafePosZ;             /* 0x3dc */
    u8  pad_3e0[0x21];
    u8                           unk_401;               /* 0x401 -- InitResources: 3 for the king, 1 otherwise */
    u8                           unk_402;               /* 0x402 -- zeroed by InitResources */
    u8                           unk_403;               /* 0x403 -- zeroed by InitResources AND by the last
                                                            statement of every Behavior frame */
    /* Render draws nothing at all while this is 0; InitResources sets it to 1. */
    u8                           mShouldRender;         /* 0x404 */
    u8  pad_405[0x1];
    u8                           unk_406;               /* 0x406 -- InitResources: 4 for the king, 6 otherwise */
    u8                           unk_407;               /* 0x407 -- zeroed by InitResources */
    u8                           unk_408;               /* 0x408 -- dActor_c::TrackStar(mStarID, 2)'s result */
    /* param1 & 0xf, handed straight to dActor_c::TrackStar as the star number.
       King only -- the plain Whomp never touches it. */
    u8                           mStarID;               /* 0x409 */
    u8  pad_40a[0x1];
    u8                           unk_40b;               /* 0x40b -- zeroed by InitResources */
    u8                           unk_40c;               /* 0x40c -- zeroed by InitResources and again
                                                            whenever mState changes */
    u8  pad_40d[0x3];
    s32                          unk_410;               /* 0x410 -- zeroed by InitResources */
    u8                           mIsKing;               /* 0x414 */
    u8  pad_415[0x3];
    dBgW_KcMbg           mMovingMeshCollider;   /* 0x418 */
    u8  pad_5e0[0x30];

    /* --- vtable --- */
    virtual ~Whomp();

    virtual s32   OnAimedAtWithEgg();      /* slot 29 */
    virtual Vector3 OnAimedAtWithEggReturnVec();       /* slot 30 */

    virtual void OnHitByMegaChar(Player &player);      /* slot 27 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char Whomp_size_must_be_0x610[sizeof(Whomp) == 0x610 ? 1 : -1];

#endif /* WHOMP_H */
