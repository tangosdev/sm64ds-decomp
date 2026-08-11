#ifndef BOOCAGE_H
#define BOOCAGE_H

#include "types.h"

/* Derives from Enemy, and TWO INDEPENDENT WITNESSES agree -- both read straight out
 * of the ROM, because this class had no source file at all until now.
 *
 * `BooCage_Spawn` (ov063:0x0211c4d0) constructs, in order:
 *
 *     _ZN9ActorBasenwEj(0x380)      <- the allocation, so 0x380 IS the sizeof
 *     _ZN5EnemyC2Ev                 <- the base, so this derives from Enemy
 *     str  _ZTV7BooCage             <- and it is this class, not a relative
 *     +0x110 _ZN18MovingCylinderClsnC1Ev
 *     +0x144 _ZN12WithMeshClsnC1Ev
 *     +0x300 _ZN5ModelC1Ev
 *     +0x350 _ZN11ShadowModelC1Ev
 *
 * `_ZN7BooCageD1Ev` (ov063:0x02115fc4) destroys the same four at the same offsets in
 * exactly the reverse order and then chains to `_ZN5EnemyD2Ev`. Construction order
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

#include "Enemy.h"
#include "Model.h"
#include "MovingCylinderClsn.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

struct BooCage : Enemy {
    MovingCylinderClsn           mMovingCylinderClsn;   /* 0x110 */
    WithMeshClsn                 mWithMeshClsn;         /* 0x144 */
    Model                        mModel;                /* 0x300 */
    ShadowModel                  mShadowModel;          /* 0x350 */
    s32                          unk_378;               /* 0x378 */
    s16                          unk_37c;               /* 0x37c */
    u8                           unk_37e;               /* 0x37e */
    u8  pad_37f[0x1];

    /* --- vtable --- */
    virtual ~BooCage();

    /* methods */
    int InitResources();
    int Render();
};

typedef char BooCage_size_must_be_0x380[sizeof(BooCage) == 0x380 ? 1 : -1];

#endif /* BOOCAGE_H */
