// @symbol daObjFire_c_classInit_OBJ_BLUE_FIRE
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9BlueFlame */
/* Reconstructed source-style name: SM64DS proves daObjFire_c through RTTI,
 * allocation size, vtable identity, and the OBJ_BLUE_FIRE registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: BlueFlame_Spawn. */
int *daObjFire_c_classInit_OBJ_BLUE_FIRE(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(280);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV9BlueFlame;
        _ZN7dCcAc_cC1Ev((char *)p + 0xe4);
    }
    return p;
}
