#ifndef RABBITKEY_H
#define RABBITKEY_H

#include "types.h"

/* Derives from Enemy, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN9RabbitKeyD1Ev` destroys each member, and
 * `RabbitKey_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV9RabbitKey`. Everything this header used to restate below
 * 0x110 belongs to Enemy and Actor and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 Model                      0x50    -> 0x160
 *     0x160 ShadowModel                0x28    -> 0x188
 *
 * SIZE IS THE ROM'S OWN: `RabbitKey_Spawn` calls
 * `ActorBase::operator new(416)` -- 0x1a0 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 */

#include "Enemy.h"
#include "Model.h"
#include "ShadowModel.h"

struct RabbitKey : Enemy {
    Model                        mModel;                /* 0x110 */
    ShadowModel                  mShadowModel;          /* 0x160 */
    u8                           unk_188;               /* 0x188 */
    u8  pad_189[0x7];
    s32                          unk_190;               /* 0x190 */
    u8  pad_194[0x8];
    s32                          unk_19c;               /* 0x19c */

    /* --- vtable --- */
    virtual ~RabbitKey();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char RabbitKey_size_must_be_0x1a0[sizeof(RabbitKey) == 0x1a0 ? 1 : -1];

#endif /* RABBITKEY_H */
