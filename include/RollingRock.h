#ifndef ROLLINGROCK_H
#define ROLLINGROCK_H

#include "types.h"

/* Derives from Enemy, on the evidence of its own destructor: `_ZN11RollingRockD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `Enemy::~Enemy`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x110 Model                      0x50   -> 0x160
 *     0x160 ShadowModel                0x28   -> 0x188
 *     0x1b8 MovingCylinderClsnWithPos  0x40   -> 0x1f8
 *     0x1f8 WithMeshClsn               0x1bc  -> 0x3b4
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

struct RollingRock : Enemy {
    Model                        mModel;                /* 0x110 */
    ShadowModel                  mShadowModel;          /* 0x160 */
    u8  pad_188[0x30];
    MovingCylinderClsnWithPos    mMovingCylinderClsnWithPos; /* 0x1b8 */
    WithMeshClsn                 mWithMeshClsn;         /* 0x1f8 */
    u32                          unk_3b4;               /* 0x3b4 */
    u8  pad_3b8[0x6];
    u8                           mType;                 /* 0x3be */
    u8                           unk_3bf;               /* 0x3bf */
    u8                           unk_3c0;               /* 0x3c0 */
    u8                           unk_3c1;               /* 0x3c1 */
    u8                           unk_3c2;               /* 0x3c2 */
    u8  pad_3c3[0x1];

    /* --- vtable --- */
    virtual ~RollingRock();

    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int InitResources();
    int Render();
    int CleanupResources();
};

typedef char RollingRock_size_must_be_0x3c4[sizeof(RollingRock) == 0x3c4 ? 1 : -1];

#endif /* ROLLINGROCK_H */
