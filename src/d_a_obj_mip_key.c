// @symbol daObj_Mip_Key_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_Model.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9RabbitKey */
/* Reconstructed source-style name: SM64DS proves the daObj_Mip_Key_c RTTI
 * identity, OBJ_MIP_KEY registry ID, profile/factory relationship, allocation
 * size, and vtable identity; later EAD lineage supplies the classInit spelling
 * prior. Exact original SM64DS spelling is not preserved. Historical project
 * alias: RabbitKey_Spawn. */
int *daObj_Mip_Key_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(416);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV9RabbitKey;
        _ZN5ModelC1Ev((char *)p + 0x110);
        _ZN11ShadowModelC1Ev((char *)p + 0x160);
    }
    return p;
}
