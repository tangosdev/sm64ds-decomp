#ifndef SNOWBALL_H
#define SNOWBALL_H

#include "types.h"

/* Derives from Enemy, on the evidence of its own destructor: `_ZN8SnowballD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `Enemy::~Enemy`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x110 MovingCylinderClsn         0x34   -> 0x144
 *     0x144 WithMeshClsn               0x1bc  -> 0x300
 *     0x300 Model                      0x50   -> 0x350
 *     0x350 ShadowModel                0x28   -> 0x378
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

struct Snowball : Enemy {
    MovingCylinderClsn           mMovingCylinderClsn;   /* 0x110 */
    WithMeshClsn                 mWithMeshClsn;         /* 0x144 */
    Model                        mModel;                /* 0x300 */
    ShadowModel                  mShadowModel;          /* 0x350 */
    s32                          unk_378;               /* 0x378 */
    s32                          unk_37c;               /* 0x37c */
    s32                          unk_380;               /* 0x380 */
    s32                          unk_384;               /* 0x384 */

    /* --- vtable --- */
    virtual ~Snowball();

    int Behavior();
    int InitResources();
    int Render();
    int CleanupResources();
    void OnPendingDestroy();
};

typedef char Snowball_size_must_be_0x388[sizeof(Snowball) == 0x388 ? 1 : -1];

#endif /* SNOWBALL_H */
