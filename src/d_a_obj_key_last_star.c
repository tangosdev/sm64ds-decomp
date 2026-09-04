// @symbol daObjKey_c_classInit_LAST_STAR
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAcPos_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV3Key */
/* Reconstructed source-style name: SM64DS proves daObjKey_c through RTTI,
 * allocation size, vtable identity, and the LAST_STAR registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: LastStar_Spawn. */
int *daObjKey_c_classInit_LAST_STAR(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1136);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV3Key;
        _ZN9ModelAnimC1Ev((char *)p + 0x114);
        _ZN5ModelC1Ev((char *)p + 0x178);
        _ZN11ShadowModelC1Ev((char *)p + 0x1c8);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x220);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x260);
    }
    return p;
}
