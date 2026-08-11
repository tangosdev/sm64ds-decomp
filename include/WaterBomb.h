#ifndef WATERBOMB_H
#define WATERBOMB_H

#include "types.h"

/* Derives from Enemy, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN9WaterBombD1Ev` destroys each member, and
 * `WaterBomb_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV9WaterBomb`. Everything this header used to restate below
 * 0x110 belongs to Enemy and Actor and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 MovingCylinderClsn         0x34    -> 0x144
 *     0x144 WithMeshClsn               0x1bc   -> 0x300
 *     0x300 Model                      0x50    -> 0x350
 *     0x350 ShadowModel                0x28    -> 0x378
 *
 * SIZE IS THE ROM'S OWN: `WaterBomb_Spawn` calls
 * `ActorBase::operator new(972)` -- 0x3cc -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 */

#include "Enemy.h"
#include "Model.h"
#include "MovingCylinderClsn.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

struct WaterBomb : Enemy {
    MovingCylinderClsn           mMovingCylinderClsn;   /* 0x110 */
    WithMeshClsn                 mWithMeshClsn;         /* 0x144 */
    Model                        mModel;                /* 0x300 */
    ShadowModel                  mShadowModel;          /* 0x350 */
    u8  pad_378[0x30];
    /* Reachable only through the shadow struct InitResources used to cast
       `this` to, so gen_header.py never saw them. 0x3b4 is the one that source
       spelled `+ 0x300 + 0xb4`: it folds to 0x3b4, which is past mModel's end
       at 0x3a0, so it is this class's field and not something inside the Model. */
    s32                          unk_3a8;               /* 0x3a8 */
    s32                          unk_3ac;               /* 0x3ac */
    s32                          unk_3b0;               /* 0x3b0 */
    s16                          unk_3b4;               /* 0x3b4 */
    u8                           unk_3b6;               /* 0x3b6 */
    u8  pad_3b7[0xd];
    s32                          unk_3c4;               /* 0x3c4 */
    s32                          unk_3c8;               /* 0x3c8 */

    /* --- vtable --- */
    virtual ~WaterBomb();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char WaterBomb_size_must_be_0x3cc[sizeof(WaterBomb) == 0x3cc ? 1 : -1];

#endif /* WATERBOMB_H */
