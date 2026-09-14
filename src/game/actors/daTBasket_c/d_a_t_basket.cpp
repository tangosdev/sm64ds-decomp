//cpp
// @symbol daTBasket_c_classInit
/* The T_BASKET registry factory.
 *
 * Reconstructed source-style name: SM64DS proves daTBasket_c through RTTI,
 * allocation size, vtable identity, and the T_BASKET registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: BooCage_Spawn.
 *
 * Typed C-ABI seam, daObjPathLift_c precedent. `return new daTBasket_c()`
 * reproduces every instruction but links the vptr slot +8 past the ROM: the
 * implicit constructor stores &_ZTV[2], the emitting-TU spelling, while this
 * TU only imports the vtable (the key function lives in the D1 shard) and the
 * ROM symbol IS the slot array. The -8 correction only fires when isolation
 * rewrites the object, which a single-function factory never needs. Hand-rolled
 * until the Boo-family merge puts this factory in the emitting TU (daWanwan
 * precedent). C LINKAGE IS LOAD-BEARING -- the ROM symbol is the bare name. */

#include "daTBasket_c.h"

extern "C" {
extern void _ZN12dEnemyBase_cC2Ev(void *self);
extern void _ZN7dCcAc_cC1Ev(void *self);
extern void _ZN10dBgCh_ActrC1Ev(void *self);
extern void _ZN5ModelC1Ev(void *self);
extern void _ZN11ShadowModelC1Ev(void *self);
}

/* Importing-TU spelling: &_ZTV[0]. [2] is the emitting-TU form and lands +8
 * past the slot array (measured WRONG at linkcheck). */
extern int _ZTV11daTBasket_c[];

extern "C" daTBasket_c *daTBasket_c_classInit(void)
{
    daTBasket_c *actor = (daTBasket_c *)_ZN7fBase_cnwEj(sizeof(daTBasket_c));
    if (actor) {
        _ZN12dEnemyBase_cC2Ev(actor);
        *(int *)actor = (int)&_ZTV11daTBasket_c[0];
        _ZN7dCcAc_cC1Ev(&actor->mdCcAc_c);
        _ZN10dBgCh_ActrC1Ev(&actor->mWithMeshClsn);
        _ZN5ModelC1Ev(&actor->mModel);
        _ZN11ShadowModelC1Ev(&actor->mShadowModel);
    }
    return actor;
}
