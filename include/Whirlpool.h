#ifndef WHIRLPOOL_H
#define WHIRLPOOL_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN9WhirlpoolD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `dEnemyBase_c::~dEnemyBase_c`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x114 ModelAnim                  0x64   -> 0x178
 *     0x178 TextureTransformer         0x14   -> 0x18c
 *
 * Typing them absorbed these markers, which were a member's insides:
 *   - 0x184 unk_184      = mTextureTransformer + 0x0c
 *
 * Member NAMES are the ones this header already used -- a rebase should not
 * also rename things its callers spell.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS RTTI names the implementation daWater_Tatumaki_c. The reconstructed factory
 * daWater_Tatumaki_c_classInit (historical alias Whirlpool_Spawn) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_WATER_TATUMAKI
 * (historical alias Whirlpool_SpawnInfo) is its registry descriptor.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "TextureTransformer.h"
#include "dBgCh_Actr.h"

struct Whirlpool : dEnemyBase_c {
    s32                          unk_110;               /* 0x110 */
    ModelAnim                    mModelAnim;            /* 0x114 */
    TextureTransformer           mTextureTransformer;   /* 0x178 */
    u8  pad_18c[0x1c];
    s32                          unk_1a8;               /* 0x1a8 */
    s32                          unk_1ac;               /* 0x1ac */
    s32                          unk_1b0;               /* 0x1b0 */
    u8  pad_1b4[0x4];
    u8                           unk_1b8;               /* 0x1b8 */
    u8  pad_1b9[0x3];

    /* --- vtable --- */
    virtual ~Whirlpool();

    int Behavior();
    int InitResources();
    int Render();
    int CleanupResources();
    void OnPendingDestroy();
};

typedef char Whirlpool_size_must_be_0x1bc[sizeof(Whirlpool) == 0x1bc ? 1 : -1];

#endif /* WHIRLPOOL_H */
