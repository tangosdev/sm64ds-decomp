// @symbol daObjPowerUpItem_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV11PowerFlower */
/* Reconstructed source-style name: SM64DS proves daObjPowerUpItem_c through RTTI,
 * allocation size, vtable identity, and the POWER_UP_ITEM registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: PowerFlower_Spawn. */
int *daObjPowerUpItem_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(972);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV11PowerFlower;
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN5ModelC1Ev((char *)p + 0x124);
        _ZN11ShadowModelC1Ev((char *)p + 0x174);
        _ZN7dCcAc_cC1Ev((char *)p + 0x1cc);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x200);
    }
    return p;
}
