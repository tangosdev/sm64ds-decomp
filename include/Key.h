#ifndef KEY_H
#define KEY_H

#include "types.h"

/* Derives from Enemy, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN3KeyD1Ev` destroys each member, and
 * `Key_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV3Key`. Everything this header used to restate below
 * 0x110 belongs to Enemy and Actor and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x114 ModelAnim                  0x64    -> 0x178
 *     0x178 Model                      0x50    -> 0x1c8
 *     0x1c8 ShadowModel                0x28    -> 0x1f0
 *     0x220 MovingCylinderClsnWithPos  0x40    -> 0x260
 *     0x260 WithMeshClsn               0x1bc   -> 0x41c
 *
 * Typing them absorbed markers that were their insides:
 *   - unk_124 = ModelAnim.data.bones
 *   - unk_128 = ModelAnim.data.transforms
 *   - mAnimation = the Animation base subobject
 *   - unk_174 = ModelAnim.file
 *
 * THE FIRST MEMBER IS AT 0x114, NOT 0x110: Key keeps four bytes of its own
 * between Enemy's end and the ModelAnim. Every other class in this batch
 * starts its members flush against Enemy.
 *
 * SIZE IS THE ROM'S OWN: `Key_Spawn` calls
 * `ActorBase::operator new(1136)` -- 0x470 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 */

#include "Enemy.h"
#include "Model.h"
#include "ModelAnim.h"
#include "MovingCylinderClsnWithPos.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

struct Key : Enemy {
    s32                          unk_110;               /* 0x110 */
    ModelAnim                    mModelAnim;            /* 0x114 */
    Model                        mModel;                /* 0x178 */
    ShadowModel                  mShadowModel;          /* 0x1c8 */
    u8  pad_1f0[0x30];
    MovingCylinderClsnWithPos    mMovingCylinderClsnWithPos; /* 0x220 */
    WithMeshClsn                 mWithMeshClsn;         /* 0x260 */
    s32                          unk_41c;               /* 0x41c */
    s32                          unk_420;               /* 0x420 */
    s32                          unk_424;               /* 0x424 */
    u8  pad_428[0x18];
    s16                          mSpinSpeed;            /* 0x440 */
    u8                           unk_442;               /* 0x442 */
    u8                           unk_443;               /* 0x443 */
    s32                          mState;                /* 0x444 */
    s32                          unk_448;               /* 0x448 */
    u8  pad_44c[0x18];
    s32                          unk_464;               /* 0x464 */
    s32                          unk_468;               /* 0x468 */
    s32                          unk_46c;               /* 0x46c */

    /* --- vtable --- */
    virtual ~Key();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char Key_size_must_be_0x470[sizeof(Key) == 0x470 ? 1 : -1];

#endif /* KEY_H */
