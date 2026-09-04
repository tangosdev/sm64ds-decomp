// @symbol daJango_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_BlendModelAnim.h"
#include "decl_Enemy.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV6Klepto */
/* Reconstructed source-style name: SM64DS proves daJango_c through RTTI,
 * allocation size, vtable identity, and the JANGO registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Klepto_Spawn. */
int *daJango_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1168);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV6Klepto;
        _ZN7dCcAc_cC1Ev((char *)p + 0x110);
        _ZN7dCcAc_cC1Ev((char *)p + 0x144);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x178);
        _ZN14BlendModelAnimC1Ev((char *)p + 0x334);
        _ZN11ShadowModelC1Ev((char *)p + 0x3a4);
    }
    return p;
}
