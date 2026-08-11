#ifndef ONEUPMUSHROOM_H
#define ONEUPMUSHROOM_H

#include "types.h"

/* Derives from Enemy, and TWO INDEPENDENT WITNESSES agree on the layout: the
 * class's own destructor `_ZN13OneUpMushroomD1Ev` destroys each member, and
 * `OneUpMushroom_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV13OneUpMushroom`. Everything this header used to restate below 0x110
 * belongs to Enemy and Actor and is inherited now.
 *
 * The members close on each other, which is what makes the layout a reading
 * rather than a guess:
 *
 *     0x110 MovingCylinderClsn         0x34    -> 0x144
 *     0x144 WithMeshClsn               0x1bc   -> 0x300
 *     0x300 Model                      0x50    -> 0x350
 *     0x350 ShadowModel                0x28    -> 0x378
 *
 * SIZE IS THE ROM'S OWN: `OneUpMushroom_Spawn` calls `ActorBase::operator new(920)`
 * -- 0x398 -- and stores this class's vtable, so that literal IS this
 * class's sizeof.
 *
 * The ROM's RTTI names this class da1up_c.
 */

#include "Enemy.h"
#include "Model.h"
#include "MovingCylinderClsn.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

struct OneUpMushroom : Enemy {
    MovingCylinderClsn           mMovingCylinderClsn;   /* 0x110 */
    WithMeshClsn                 mWithMeshClsn;         /* 0x144 */
    Model                        mModel;                /* 0x300 */
    ShadowModel                  mShadowModel;          /* 0x350 */
    s32                          unk_378;               /* 0x378 */
    s32                          unk_37c;               /* 0x37c */
    s32                          unk_380;               /* 0x380 */
    s32                          mMushroomType;         /* 0x384 */
    s32                          unk_388;               /* 0x388 */
    u8  pad_38c[0x2];
    u8                           unk_38e;               /* 0x38e */
    u8                           unk_38f;               /* 0x38f */
    s32                          unk_390;               /* 0x390 */
    s32                          unk_394;               /* 0x394 */

    /* --- vtable --- */
    virtual ~OneUpMushroom();

    /* --- non-virtual --- */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char OneUpMushroom_size_must_be_0x398[sizeof(OneUpMushroom) == 0x398 ? 1 : -1];

#endif /* ONEUPMUSHROOM_H */
