#ifndef BOOKSHOT_H
#define BOOKSHOT_H

#include "types.h"

/* Derives from Enemy, and TWO INDEPENDENT WITNESSES agree on the layout: the
 * class's own destructor `_ZN8BookShotD1Ev` destroys each member, and
 * `BookShot_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV8BookShot`. Everything this header used to restate below 0x110
 * belongs to Enemy and Actor and is inherited now.
 *
 * The members close on each other, which is what makes the layout a reading
 * rather than a guess:
 *
 *     0x110 ModelAnim                  0x64    -> 0x174
 *     0x174 Model                      0x50    -> 0x1c4
 *     0x1c4 ShadowModel                0x28    -> 0x1ec
 *     0x21c MovingCylinderClsnWithPos  0x40    -> 0x25c
 *     0x25c WithMeshClsn               0x1bc   -> 0x418
 *
 * SIZE IS THE ROM'S OWN: `BookShot_Spawn` calls `ActorBase::operator new(1108)`
 * -- 0x454 -- and stores this class's vtable, so that literal IS this
 * class's sizeof.
 *
 * The ROM's RTTI names this class daBook_c.
 */

#include "Enemy.h"
#include "Model.h"
#include "ModelAnim.h"
#include "MovingCylinderClsnWithPos.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

struct BookShot : Enemy {
    ModelAnim                    mModelAnim;            /* 0x110 */
    Model                        mModel;                /* 0x174 */
    ShadowModel                  mShadowModel;          /* 0x1c4 */
    u8                           unk_1ec;               /* 0x1ec */
    u8  pad_1ed[0x2f];
    MovingCylinderClsnWithPos    mMovingCylinderClsnWithPos; /* 0x21c */
    WithMeshClsn                 mWithMeshClsn;         /* 0x25c */
    s32                          unk_418;               /* 0x418 */
    s32                          unk_41c;               /* 0x41c */
    s32                          unk_420;               /* 0x420 */
    s32                          mState;                /* 0x424 */
    s32                          unk_428;               /* 0x428 */
    s32                          unk_42c;               /* 0x42c */
    s32                          unk_430;               /* 0x430 */
    s32                          unk_434;               /* 0x434 */
    s32                          unk_438;               /* 0x438 */
    s32                          unk_43c;               /* 0x43c */
    s32                          unk_440;               /* 0x440 */
    u8  pad_444[0x8];
    s32                          unk_44c;               /* 0x44c */
    s8                           unk_450;               /* 0x450 */
    u8  pad_451[0x3];

    /* --- vtable --- */
    virtual ~BookShot();

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    /* --- non-virtual --- */
    int Behavior();
    int CleanupResources();
    int InitResources();
};

typedef char BookShot_size_must_be_0x454[sizeof(BookShot) == 0x454 ? 1 : -1];

#endif /* BOOKSHOT_H */
