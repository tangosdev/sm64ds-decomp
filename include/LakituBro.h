#ifndef LAKITUBRO_H
#define LAKITUBRO_H

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN9LakituBroD1Ev` destroys each member, and
 * `LakituBro_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV9LakituBro`. Everything this header used to restate below
 * 0x110 belongs to dEnemyBase_c and dActor_c and is inherited now.
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
 * `fBase_c::operator new(744)` -- 0x2e8 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 *
 * SM64DS proves this class as daC_Jugem_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings below are reconstructed
 * source-style names -- evidence-bounded proposals, not recovered SM64DS
 * symbols.
 *
 * daC_Jugem_c_classInit at 0x0212ed54 (historical alias LakituBro_Spawn)
 * allocates 0x2e8 and installs this class's cartridge vtable. It backs the
 * C_JUGEM registry profile, whose descriptor at 0x02130320 is reconstructed
 * as g_profile_C_JUGEM.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "TextureSequence.h"

struct LakituBro : dEnemyBase_c {
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
