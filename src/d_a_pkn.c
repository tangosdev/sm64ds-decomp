// @symbol daPkn_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_dCcAcPos_c.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV12PiranhaPlant */
/* Reconstructed source-style name: SM64DS proves daPkn_c through RTTI,
 * allocation size, vtable identity, and the PAKUN registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: PiranhaPlant_Spawn. */
int *daPkn_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1148);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV12PiranhaPlant;
        _ZN9ModelAnimC1Ev((char *)p + 0x110);
        _ZN5ModelC1Ev((char *)p + 0x174);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x1c4);
        _ZN7dCcAc_cC1Ev((char *)p + 0x380);
        _ZN7dCcAc_cC1Ev((char *)p + 0x3b4);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x3e8);
    }
    return p;
}
