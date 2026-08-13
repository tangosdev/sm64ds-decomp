#ifndef LAVABUBBLE_H
#define LAVABUBBLE_H

#include "types.h"

/* Derives from Enemy, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN10LavaBubbleD1Ev` destroys each member, and
 * `LavaBubble_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV10LavaBubble`. Everything this header used to restate below
 * 0x110 belongs to Enemy and Actor and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 MovingCylinderClsn         0x34    -> 0x144
 *     0x144 WithMeshClsn               0x1bc   -> 0x300
 *
 * Typing them absorbed markers that were their insides:
 *   - unk_128 = MovingCylinderClsn.flags
 *   - unk_130 = MovingCylinderClsn.hitFlags
 *   - unk_134 = MovingCylinderClsn.otherOwner
 *
 * SIZE IS THE ROM'S OWN: `LavaBubble_Spawn` calls
 * `ActorBase::operator new(796)` -- 0x31c -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 */

#include "Enemy.h"
#include "MovingCylinderClsn.h"
#include "WithMeshClsn.h"

struct LavaBubble : Enemy {
    MovingCylinderClsn           mMovingCylinderClsn;   /* 0x110 */
    WithMeshClsn                 mWithMeshClsn;         /* 0x144 */
    s32                          unk_300;               /* 0x300 */
    s32                          unk_304;               /* 0x304 */
    s32                          unk_308;               /* 0x308 */
    s32                          unk_30c;               /* 0x30c */
    u8                           unk_310;               /* 0x310 */
    u8  pad_311[0xb];

    /* --- vtable --- */
    virtual ~LavaBubble();

    virtual s32   OnYoshiTryEat();         /* slot 18 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char LavaBubble_size_must_be_0x31c[sizeof(LavaBubble) == 0x31c ? 1 : -1];

#endif /* LAVABUBBLE_H */
