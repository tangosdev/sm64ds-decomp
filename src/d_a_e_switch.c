// @symbol daESwitch_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV14EnemySwitchTag */
/* Reconstructed source-style name: SM64DS proves daESwitch_c through RTTI,
 * allocation size, vtable identity, and the ENEMY_SWITCH registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: EnemySwitchTag_Spawn. */
int *daESwitch_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(272);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV14EnemySwitchTag;
        _ZN7dCcAc_cC1Ev((char *)p + 0xd4);
    }
    return p;
}
