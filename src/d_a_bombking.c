// @symbol daBombking_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_BlendModelAnim.h"
#include "decl_Enemy.h"
#include "decl_dCcAcPos_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV12daBombking_c */
/* Reconstructed source-style name: SM64DS proves daBombking_c through RTTI,
 * allocation size, vtable identity, and the BOMBKING registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: KingBobOmb_Spawn. */
int *daBombking_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1292);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV12daBombking_c;
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x110);
        _ZN14BlendModelAnimC1Ev((char *)p + 0x2cc);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x33c);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x37c);
        _ZN11CommonModelC1Ev((char *)p + 0x3bc);
        _ZN11ShadowModelC1Ev((char *)p + 0x3f8);
    }
    return p;
}
