#ifndef POWERSTAR_H
#define POWERSTAR_H

#include "types.h"

/* Derives from Enemy, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN9PowerStarD1Ev` destroys each member, and
 * `PowerStar_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV9PowerStar`. Everything this header used to restate below
 * 0x110 belongs to Enemy and Actor and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 MovingCylinderClsnWithPos  0x40    -> 0x150
 *     0x150 WithMeshClsn               0x1bc   -> 0x30c
 *     0x30c ModelAnim                  0x64    -> 0x370
 *     0x370 ModelAnim                  0x64    -> 0x3d4
 *     0x3d4 ShadowModel                0x28    -> 0x3fc
 *
 * SIZE IS THE ROM'S OWN: `PowerStar_Spawn` calls
 * `ActorBase::operator new(1220)` -- 0x4c4 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 */

#include "Enemy.h"
#include "ModelAnim.h"
#include "MovingCylinderClsnWithPos.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

struct PowerStar : Enemy {
    MovingCylinderClsnWithPos    mCylinderClsn;         /* 0x110 */
    WithMeshClsn                 mWithMeshClsn;         /* 0x150 */
    ModelAnim                    mModelAnim1;           /* 0x30c */
    ModelAnim                    mModelAnim2;           /* 0x370 */
    ShadowModel                  mShadowModel;          /* 0x3d4 */
    u8  pad_3fc[0x40];
    s32                          unk_43c;               /* 0x43c */
    s32                          unk_440;               /* 0x440 */
    u8  pad_444[0x54];
    s8                           unk_498;               /* 0x498 */
    u8  pad_499[0x1];
    u8                           unk_49a;               /* 0x49a */
    u8  pad_49b[0x2];
    u8                           unk_49d;               /* 0x49d */
    u8  pad_49e[0x1];
    u8                           unk_49f;               /* 0x49f */
    u8  pad_4a0[0x2];
    u8                           unk_4a2;               /* 0x4a2 */
    u8  pad_4a3[0x5];
    s32                          unk_4a8;               /* 0x4a8 */
    s32                          unk_4ac;               /* 0x4ac */
    s32                          unk_4b0;               /* 0x4b0 */
    u8  pad_4b4[0x10];

    /* --- vtable --- */
    virtual ~PowerStar();

    virtual s32   OnYoshiTryEat();         /* slot 18 */

    void AddStarMarker();
    int Behavior();
    int CleanupResources();
    s32 InitResources();
    int Render();
};

typedef char PowerStar_size_must_be_0x4c4[sizeof(PowerStar) == 0x4c4 ? 1 : -1];

#endif /* POWERSTAR_H */
