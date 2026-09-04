// @symbol daWater_Suikomi_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_dCcAcPos_c.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV12WaterSuction */
/* Reconstructed source-style name: SM64DS proves daWater_Suikomi_c through RTTI,
 * allocation size, vtable identity, and the WATER_SUIKOMI registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: WaterSuction_Spawn. */
int *daWater_Suikomi_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(792);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV12WaterSuction;
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x110);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x150);
    }
    return p;
}
