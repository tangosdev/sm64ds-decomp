#ifndef DATBASKET_C_H
#define DATBASKET_C_H

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree -- both read straight out
 * of the ROM, because this class had no source file at all until now.
 *
 * `BooCage_Spawn` (ov063:0x0211c4d0) constructs, in order:
 *
 *     _ZN7fBase_cnwEj(0x380)      <- the allocation, so 0x380 IS the sizeof
 *     _ZN12dEnemyBase_cC2Ev                 <- the base, so this derives from dEnemyBase_c
 *     str  _ZTV11daTBasket_c         <- and it is this class, not a relative
 *     +0x110 _ZN7dCcAc_cC1Ev
 *     +0x144 _ZN10dBgCh_ActrC1Ev
 *     +0x300 _ZN5ModelC1Ev
 *     +0x350 _ZN11ShadowModelC1Ev
 *
 * `_ZN11daTBasket_cD1Ev` (ov063:0x02115fc4) destroys the same four at the same offsets in
 * exactly the reverse order and then chains to `_ZN12dEnemyBase_cD2Ev`. Construction order
 * forward, destruction order backward, same offsets, same types: that is a layout
 * read twice, not once.
 *
 * It closes: 0x350 + sizeof(ShadowModel) 0x28 = 0x378, and 8 bytes of tail padding
 * reach the 0x380 the ROM allocates.
 *
 * The ROM's RTTI names this class daTBasket_c. The member shape is OneUpMushroom's
 * exactly -- same four types at the same four offsets -- which is what a shared
 * spawn-and-hold idiom looks like; the classes differ only in total size.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

/* ROM-authenticated lifecycle identity.  `_ZTI11daTBasket_c` / `_ZTS11daTBasket_c`
 * exist in ov063; no `_ZTI7BooCage` / `_ZTS7BooCage` ever did, so the cartridge
 * names this class and `BooCage` was only ever the actor's coined name -- which
 * is why `BooCage_Spawn` / `BooCage_SpawnInfo` keep it, exactly as ov018 keeps
 * `MotherPenguin_Spawn` for `daPgMthr_c`. */
struct daTBasket_c : dEnemyBase_c {
    dCcAc_c           mdCcAc_c;   /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x144 */
    Model                        mModel;                /* 0x300 */
    ShadowModel                  mShadowModel;          /* 0x350 */
    s32                          mParticleID;               /* 0x378 */
    s16                          mSoundTimer;               /* 0x37c */
    u8                           mMuteSecretSound;      /* 0x37e -- nonzero skips Sound::PlaySecretSound */
    u8  pad_37f[0x1];

    /* --- vtable ---
     * Overrides of fBase_c virtuals, so each takes the base's slot regardless of
     * the order declared here; the ROM's _ZTV11daTBasket_c @ 0x0211e930 puts
     * ov063 code in exactly these five and inherits every other entry. */
    virtual ~daTBasket_c();          /* slots 16 (D1), 17 (D0) */
    virtual s32 InitResources();     /* slot  0 -- ov063:0x0211c35c */
    virtual s32 CleanupResources();  /* slot  3 -- ov063:0x0211ae1c */
    virtual s32 Behavior();          /* slot  6 -- ov063:0x0211b888 */
    virtual s32 Render();            /* slot  9 -- ov063:0x0211b078 */
};

typedef char daTBasket_c_size_must_be_0x380[sizeof(daTBasket_c) == 0x380 ? 1 : -1];

#endif /* DATBASKET_C_H */
