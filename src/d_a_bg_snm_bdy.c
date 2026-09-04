// @symbol daBgSnmBdy_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV11SnowmanBody */
extern void _ZN7PathPtrC1Ev(void *);
/* Reconstructed source-style name: SM64DS proves daBgSnmBdy_c through RTTI,
 * allocation size, vtable identity, and the BIG_SNOWMAN_BODY registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: SnowmanBody_Spawn. */
int *daBgSnmBdy_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(936);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV11SnowmanBody;
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN11ShadowModelC1Ev((char *)p + 0x124);
        _ZN7dCcAc_cC1Ev((char *)p + 0x14c);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x180);
        _ZN7PathPtrC1Ev((char *)p + 0x380);
    }
    return p;
}
