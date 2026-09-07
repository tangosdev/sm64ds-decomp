// @symbol daCoin_c_classInit_COIN
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV4Coin */
/* Reconstructed source-style name: SM64DS proves daCoin_c through RTTI,
 * allocation size, vtable identity, and the COIN registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Coin_Spawn. */
int *daCoin_c_classInit_COIN(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(948);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV4Coin;
        _ZN11CommonModelC1Ev((char *)p + 0xd8);
        _ZN11CommonModelC1Ev((char *)p + 0x114);
        _ZN11ShadowModelC1Ev((char *)p + 0x150);
        _ZN7dCcAc_cC1Ev((char *)p + 0x178);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x1ac);
    }
    return p;
}
