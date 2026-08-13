#ifndef BOBOMB_H
#define BOBOMB_H

#include "types.h"

/* Derives from Enemy, on the evidence of its own destructor: `_ZN6BobOmbD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `Enemy::~Enemy`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x110 MovingCylinderClsn         0x34   -> 0x144
 *     0x144 WithMeshClsn               0x1bc  -> 0x300
 *     0x300 ModelAnim                  0x64   -> 0x364
 *     0x364 ShadowModel                0x28   -> 0x38c
 *
 * Typing them absorbed these markers, which were a member's insides:
 *   - 0x128 unk_128      = mCylinderClsn + 0x18
 *   - 0x130 unk_130      = mCylinderClsn + 0x20
 *   - 0x134 unk_134      = mCylinderClsn + 0x24
 *
 * Member NAMES are the ones this header already used -- a rebase should not
 * also rename things its callers spell.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#include "Enemy.h"
#include "Model.h"
#include "ModelAnim.h"
#include "MovingCylinderClsn.h"
#include "MovingCylinderClsnWithPos.h"
#include "ShadowModel.h"
#include "TextureTransformer.h"
#include "WithMeshClsn.h"

struct BobOmb : Enemy {
    MovingCylinderClsn           mCylinderClsn;         /* 0x110 */
    WithMeshClsn                 mWithMeshClsn;         /* 0x144 */
    ModelAnim                    mModelAnim;            /* 0x300 */
    ShadowModel                  mShadowModel;          /* 0x364 */
    u8  pad_38c[0x4];
    /* Everything from here down was reachable only through the `struct Obj`
       shadow InitResources used to carry, so the generated header never had
       it. unk_394 is a 0x30-byte block copied wholesale from data_02082128. */
    u32                          unk_390;               /* 0x390 */
    s32                          unk_394[12];           /* 0x394 */
    s32                          mHomePosX;             /* 0x3c4 */
    s32                          mHomePosY;             /* 0x3c8 */
    s32                          mHomePosZ;             /* 0x3cc */
    u8  pad_3d0[0xc];
    s32                          unk_3dc;               /* 0x3dc */
    s32                          unk_3e0;               /* 0x3e0 */
    u8  pad_3e4[0x4];
    u16                          unk_3e8;               /* 0x3e8 */
    u16                          unk_3ea;               /* 0x3ea */
    u16                          unk_3ec;               /* 0x3ec */
    u16                          unk_3ee;               /* 0x3ee */
    u16                          unk_3f0;               /* 0x3f0 */
    u8                           unk_3f2;               /* 0x3f2 */
    u8                           unk_3f3;               /* 0x3f3 */
    u8                           unk_3f4;               /* 0x3f4 */
    u8                           unk_3f5;               /* 0x3f5 */
    u8                           unk_3f6;               /* 0x3f6 */
    u8  pad_3f7[0x1];

    /* --- vtable --- */
    virtual ~BobOmb();

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char BobOmb_size_must_be_0x3f8[sizeof(BobOmb) == 0x3f8 ? 1 : -1];

#endif /* BOBOMB_H */
