//cpp
/* The TERESA (Boo) and BOSS_TERESA (Big Boo) registry factories -- ov063/daTrs_c.
 *
 * Both profiles construct the ROM-proven daTrs_c class (size 0x5e0, vtable
 * _ZTV7daTrs_c): the init param's actor id (0xd1 ordinary, 0xd2 boss) selects
 * the shape InitResources builds, not the class. A class-anchored classInit
 * rename would collide, so each factory keeps its profile-suffixed spelling.
 *
 * The two bodies sit adjacent in the ROM's own .text order (BOSS_TERESA at
 * 0x0211c520, TERESA at 0x0211c590) with no gap and no other class between
 * them, bounded below by daTBasket_c's factory and above by daTrsTrap_c's
 * destructors. They are written here highest-address-first: mwccarm lays
 * .text down in reverse source order.
 *
 * C LINKAGE IS LOAD-BEARING -- the ROM symbols are the bare names.
 * Typed C-ABI seam, daTBasket_c precedent. `return new daTrs_c()` reproduces
 * every instruction but emits the vptr store against _ZTV7daTrs_c+8, the
 * emitting-TU spelling, while this TU only imports the vtable (it stays a
 * ROM blob homed outside this TU) and the ROM symbol IS the slot array:
 * text-only production isolation cannot rewrite a nonzero RTTI addend, so
 * the hand-rolled bodies keep every imported-symbol reference at +0.
 * Leaf operator new forwards until #2570.
 *
 * Reconstructed source-style names: SM64DS proves daTrs_c through RTTI,
 * allocation size, vtable identity, and the TERESA / BOSS_TERESA registry
 * profiles; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical aliases: Boo_Spawn, BigBoo_Spawn.
 */

#include "daTrs_c.h"

extern "C" {
extern void *_ZN11dCapEnemy_cC2Ev(void *self);
extern void *_ZN10dCcAcPos_cC1Ev(void *self);
extern void *_ZN10dBgCh_ActrC1Ev(void *self);
extern void *_ZN9ModelAnimC1Ev(void *self);
extern void *_ZN5ModelC1Ev(void *self);
extern void *_ZN11ShadowModelC1Ev(void *self);
}

/* Importing-TU spelling: &_ZTV[0]. [2] is the emitting-TU form and lands +8
 * past the slot array: both factories' ROM pool words are 0x0211e828, the
 * _ZTV symbol itself (slot 0 is InitResources). The link harnesses pass
 * either spelling -- no relocs.txt edge records this import -- so the +0
 * spelling is proven by reloc dump + ROM bytes, not by a gate. Proof:
 * notes/experiments/boo-2710-factory-vtable.md. */
extern int _ZTV7daTrs_c[];

extern "C" {

// @symbol daTrs_c_classInit_TERESA
daTrs_c *daTrs_c_classInit_TERESA(void)
{
    daTrs_c *actor = (daTrs_c *)_ZN7fBase_cnwEj(sizeof(daTrs_c));
    if (actor) {
        _ZN11dCapEnemy_cC2Ev(actor);
        *(int *)actor = (int)&_ZTV7daTrs_c[0];
        _ZN10dCcAcPos_cC1Ev(&actor->mdCcAcPos_c);
        _ZN10dBgCh_ActrC1Ev(&actor->mWithMeshClsn);
        _ZN9ModelAnimC1Ev(&actor->mModelAnim);
        _ZN5ModelC1Ev(&actor->mBodyModel);
        _ZN11ShadowModelC1Ev(&actor->mShadowModel1);
        _ZN11ShadowModelC1Ev(&actor->mShadowModel2);
    }
    return actor;
}

// @symbol daTrs_c_classInit_BOSS_TERESA
daTrs_c *daTrs_c_classInit_BOSS_TERESA(void)
{
    daTrs_c *actor = (daTrs_c *)_ZN7fBase_cnwEj(sizeof(daTrs_c));
    if (actor) {
        _ZN11dCapEnemy_cC2Ev(actor);
        *(int *)actor = (int)&_ZTV7daTrs_c[0];
        _ZN10dCcAcPos_cC1Ev(&actor->mdCcAcPos_c);
        _ZN10dBgCh_ActrC1Ev(&actor->mWithMeshClsn);
        _ZN9ModelAnimC1Ev(&actor->mModelAnim);
        _ZN5ModelC1Ev(&actor->mBodyModel);
        _ZN11ShadowModelC1Ev(&actor->mShadowModel1);
        _ZN11ShadowModelC1Ev(&actor->mShadowModel2);
    }
    return actor;
}

}
