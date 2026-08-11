#ifndef WHOMP_H
#define WHOMP_H

#include "types.h"

/* Derives from Enemy, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN5WhompD1Ev` destroys each member, and
 * `Whomp_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV5Whomp`. Everything this header used to restate below
 * 0x110 belongs to Enemy and Actor and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 WithMeshClsn               0x1bc   -> 0x2cc
 *     0x2cc ModelAnim                  0x64    -> 0x330
 *     0x330 TextureSequence            0x14    -> 0x344
 *     0x344 ShadowModel                0x28    -> 0x36c
 *     0x418 MovingMeshCollider         0x1c8   -> 0x5e0
 *
 * Typing them absorbed markers that were their insides:
 *   - unk_338 = TextureSequence.currFrame
 *   - unk_33c = TextureSequence.speed
 *
 * SIZE IS THE ROM'S OWN: `Whomp_Spawn` calls
 * `ActorBase::operator new(1552)` -- 0x610 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 */

#include "Enemy.h"
#include "ModelAnim.h"
#include "MovingMeshCollider.h"
#include "ShadowModel.h"
#include "TextureSequence.h"
#include "WithMeshClsn.h"

struct Whomp : Enemy {
    WithMeshClsn                 mWithMeshClsn;         /* 0x110 */
    ModelAnim                    mModelAnim;            /* 0x2cc */
    TextureSequence              mTextureSequence;      /* 0x330 */
    ShadowModel                  mShadowModel;          /* 0x344 */
    u8  pad_36c[0x44];
    s32                          unk_3b0;               /* 0x3b0 */
    u8  pad_3b4[0x8];
    s32                          unk_3bc;               /* 0x3bc */
    s32                          unk_3c0;               /* 0x3c0 */
    s32                          unk_3c4;               /* 0x3c4 */
    u8  pad_3c8[0x39];
    u8                           unk_401;               /* 0x401 */
    u8                           unk_402;               /* 0x402 */
    u8                           unk_403;               /* 0x403 */
    u8                           unk_404;               /* 0x404 */
    u8  pad_405[0x1];
    u8                           unk_406;               /* 0x406 */
    u8                           unk_407;               /* 0x407 */
    u8                           unk_408;               /* 0x408 */
    u8                           unk_409;               /* 0x409 */
    u8  pad_40a[0x1];
    u8                           unk_40b;               /* 0x40b */
    u8                           unk_40c;               /* 0x40c */
    u8  pad_40d[0x3];
    s32                          unk_410;               /* 0x410 */
    u8                           mIsKing;               /* 0x414 */
    u8  pad_415[0x3];
    MovingMeshCollider           mMovingMeshCollider;   /* 0x418 */
    u8  pad_5e0[0x30];

    /* --- vtable --- */
    virtual ~Whomp();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char Whomp_size_must_be_0x610[sizeof(Whomp) == 0x610 ? 1 : -1];

#endif /* WHOMP_H */
