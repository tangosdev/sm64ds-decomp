#ifndef RABBIT_H
#define RABBIT_H

#include "types.h"

/* Derives from Enemy, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN6RabbitD1Ev` destroys each member, and
 * `Rabbit_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV6Rabbit`. Everything this header used to restate below
 * 0x110 belongs to Enemy and Actor and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 MovingCylinderClsn         0x34    -> 0x144
 *     0x144 WithMeshClsn               0x1bc   -> 0x300
 *     0x300 ModelAnim                  0x64    -> 0x364
 *     0x368 ShadowModel                0x28    -> 0x390
 *     0x3c0 ShadowModel                0x28    -> 0x3e8
 *
 * Typing them absorbed markers that were their insides:
 *   - unk_308 = ModelAnim.data.modelFile
 *   - unk_30c = ModelAnim.data.materials
 *   - unk_35c = ModelAnim.speed
 *
 * SIZE IS THE ROM'S OWN: `Rabbit_Spawn` calls
 * `ActorBase::operator new(1140)` -- 0x474 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 */

#include "Enemy.h"
#include "ModelAnim.h"
#include "MovingCylinderClsn.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

struct Rabbit : Enemy {
    MovingCylinderClsn           mMovingCylinderClsn;   /* 0x110 */
    WithMeshClsn                 mWithMeshClsn;         /* 0x144 */
    ModelAnim                    mModelAnim;            /* 0x300 */
    s32                          unk_364;               /* 0x364 */
    ShadowModel                  mShadowModel1;         /* 0x368 */
    u8  pad_390[0x30];
    ShadowModel                  mShadowModel2;         /* 0x3c0 */
    u8  pad_3e8[0x38];
    s32                          mColorVariant;         /* 0x420 */
    u8  pad_424[0x2];
    u8                           unk_426;               /* 0x426 */
    u8                           unk_427;               /* 0x427 */
    u8                           unk_428;               /* 0x428 */
    u8                           unk_429;               /* 0x429 */
    u8                           unk_42a;               /* 0x42a */
    u8  pad_42b[0xd];
    s32                          unk_438;               /* 0x438 */
    s32                          mRabbitId;             /* 0x43c */
    s32                          mCharacterId;          /* 0x440 */
    s32                          unk_444;               /* 0x444 */
    s32                          unk_448;               /* 0x448 */
    u8  pad_44c[0x10];
    s32                          unk_45c;               /* 0x45c */
    u8  pad_460[0x8];
    s32                          unk_468;               /* 0x468 */
    u8  pad_46c[0x4];
    s32                          unk_470;               /* 0x470 */

    /* --- vtable --- */
    virtual ~Rabbit();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char Rabbit_size_must_be_0x474[sizeof(Rabbit) == 0x474 ? 1 : -1];

#endif /* RABBIT_H */
