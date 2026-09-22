// @symbol daNknk_c_classInit_NOKONOKO_S
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV5Koopa */
/* Reconstructed source-style name: SM64DS proves daNknk_c through RTTI,
 * allocation size, vtable identity, and the NOKONOKO_S registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: KoopaSmall_Spawn. */
int *daNknk_c_classInit_NOKONOKO_S(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(976);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV5Koopa;
        _ZN7dCcAc_cC1Ev((char *)p + 0x110);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x144);
        _ZN9ModelAnimC1Ev((char *)p + 0x300);
        _ZN11ShadowModelC1Ev((char *)p + 0x364);
    }
    return p;
}
