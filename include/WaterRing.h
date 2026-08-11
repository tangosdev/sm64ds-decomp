#ifndef WATERRING_H
#define WATERRING_H

#include "types.h"

/* Derives from Enemy, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN9WaterRingD1Ev` destroys each member, and
 * `WaterRing_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV9WaterRing`. Everything this header used to restate below
 * 0x110 belongs to Enemy and Actor and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 MovingCylinderClsnWithPos  0x40    -> 0x150
 *     0x150 WithMeshClsn               0x1bc   -> 0x30c
 *     0x30c Model                      0x50    -> 0x35c
 *     0x35c TextureTransformer         0x14    -> 0x370
 *
 * Typing them absorbed markers that were their insides:
 *   - unk_368 = TextureTransformer.speed
 *
 * SIZE IS THE ROM'S OWN: `WaterRing_Spawn` calls
 * `ActorBase::operator new(912)` -- 0x390 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 */

#include "Enemy.h"
#include "Model.h"
#include "MovingCylinderClsnWithPos.h"
#include "TextureTransformer.h"
#include "WithMeshClsn.h"

struct WaterRing : Enemy {
    MovingCylinderClsnWithPos    mMovingCylinderClsnWithPos; /* 0x110 */
    WithMeshClsn                 mWithMeshClsn;         /* 0x150 */
    Model                        mModel;                /* 0x30c */
    TextureTransformer           mTextureTransformer;   /* 0x35c */
    u8                           unk_370;               /* 0x370 */
    u8  pad_371[0xb];
    s32                          unk_37c;               /* 0x37c */
    s8                           unk_380;               /* 0x380 */
    u8  pad_381[0xf];

    /* --- vtable --- */
    virtual ~WaterRing();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char WaterRing_size_must_be_0x390[sizeof(WaterRing) == 0x390 ? 1 : -1];

#endif /* WATERRING_H */
