#ifndef FIREBALL_H
#define FIREBALL_H

#include "types.h"

/* Derives from Enemy, on the evidence of its own destructor: `_ZN8FireballD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `Enemy::~Enemy`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x110 MovingCylinderClsn         0x34   -> 0x144
 *     0x144 WithMeshClsn               0x1bc  -> 0x300
 *     0x300 ShadowModel                0x28   -> 0x328
 *
 * Typing them absorbed these markers, which were a member's insides:
 *   - 0x12c unk_12c      = mMovingCylinderClsn + 0x1c
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

struct Fireball : Enemy {
    MovingCylinderClsn           mMovingCylinderClsn;   /* 0x110 */
    WithMeshClsn                 mWithMeshClsn;         /* 0x144 */
    ShadowModel                  mShadowModel;          /* 0x300 */
    u8  pad_328[0x38];
    s32                          unk_360;               /* 0x360 */
    s32                          unk_364;               /* 0x364 */
    u8  pad_368[0x2];
    u16                          unk_36a;               /* 0x36a */
    u8  pad_36c[0x1];
    u8                           unk_36d;               /* 0x36d */
    u8  pad_36e[0x2];
    s32                          unk_370;               /* 0x370 */
    s32                          unk_374;               /* 0x374 */

    /* --- vtable --- */
    virtual ~Fireball();

    virtual s32   OnYoshiTryEat();         /* slot 18 */

    int Behavior();
    int InitResources();
    int Render();
};

typedef char Fireball_size_must_be_0x378[sizeof(Fireball) == 0x378 ? 1 : -1];

#endif /* FIREBALL_H */
