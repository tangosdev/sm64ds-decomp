#ifndef SPINDRIFT_H
#define SPINDRIFT_H

#include "types.h"

/* Derives from Enemy, and TWO INDEPENDENT WITNESSES agree on the layout: the
 * class's own destructor `_ZN9SpindriftD1Ev` destroys each member, and
 * `Spindrift_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV9Spindrift`. Everything this header used to restate below 0x110
 * belongs to Enemy and Actor and is inherited now.
 *
 * The members close on each other, which is what makes the layout a reading
 * rather than a guess:
 *
 *     0x110 ModelAnim                  0x64    -> 0x174
 *     0x174 ShadowModel                0x28    -> 0x19c
 *     0x19c MovingCylinderClsn         0x34    -> 0x1d0
 *     0x1d0 WithMeshClsn               0x1bc   -> 0x38c
 *
 * SIZE IS THE ROM'S OWN: `Spindrift_Spawn` calls `ActorBase::operator new(924)`
 * -- 0x39c -- and stores this class's vtable, so that literal IS this
 * class's sizeof.
 *
 * The ROM's RTTI names this class daHuwa_c.
 */

#include "Enemy.h"
#include "ModelAnim.h"
#include "MovingCylinderClsn.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

struct Spindrift : Enemy {
    ModelAnim                    mModelAnim;            /* 0x110 */
    ShadowModel                  mShadowModel;          /* 0x174 */
    MovingCylinderClsn           mMovingCylinderClsn;   /* 0x19c */
    WithMeshClsn                 mWithMeshClsn;         /* 0x1d0 */
    u8  pad_38c[0xc];
    s16                          unk_398;               /* 0x398 */
    u8                           unk_39a;               /* 0x39a */
    u8  pad_39b[0x1];

    /* --- vtable --- */
    virtual ~Spindrift();

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    /* --- non-virtual --- */
    int Behavior();
    int InitResources();
    int Render();
};

typedef char Spindrift_size_must_be_0x39c[sizeof(Spindrift) == 0x39c ? 1 : -1];

#endif /* SPINDRIFT_H */
