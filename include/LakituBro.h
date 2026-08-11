#ifndef LAKITUBRO_H
#define LAKITUBRO_H

#include "types.h"

/* Derives from Enemy, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN9LakituBroD1Ev` destroys each member, and
 * `LakituBro_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV9LakituBro`. Everything this header used to restate below
 * 0x110 belongs to Enemy and Actor and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 ModelAnim                  0x64    -> 0x174
 *     0x174 ModelAnim                  0x64    -> 0x1d8
 *     0x1d8 TextureSequence            0x14    -> 0x1ec
 *     0x1f0 ShadowModel                0x28    -> 0x218
 *     0x218 ShadowModel                0x28    -> 0x240
 *
 * Typing them absorbed markers that were their insides:
 *   - unk_16c = ModelAnim.speed
 *
 * SIZE IS THE ROM'S OWN: `LakituBro_Spawn` calls
 * `ActorBase::operator new(744)` -- 0x2e8 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 */

#include "Enemy.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "TextureSequence.h"

struct LakituBro : Enemy {
    ModelAnim                    mModelAnim1;           /* 0x110 */
    ModelAnim                    mModelAnim2;           /* 0x174 */
    TextureSequence              mTextureSequence;      /* 0x1d8 */
    u8  pad_1ec[0x4];
    ShadowModel                  mShadowModel1;         /* 0x1f0 */
    ShadowModel                  mShadowModel2;         /* 0x218 */
    u8  pad_240[0x90];
    s32                          unk_2d0;               /* 0x2d0 */
    u8  pad_2d4[0x8];
    u8                           unk_2dc;               /* 0x2dc */
    u8  pad_2dd[0xb];

    /* --- vtable --- */
    virtual ~LakituBro();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char LakituBro_size_must_be_0x2e8[sizeof(LakituBro) == 0x2e8 ? 1 : -1];

#endif /* LAKITUBRO_H */
