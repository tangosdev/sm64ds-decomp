// @symbol daPeach_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV13PrincessPeach */
/* Reconstructed source-style name: SM64DS proves daPeach_c through RTTI,
 * allocation size, vtable identity, and the PEACH_PRINCESS registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: PrincessPeach_Spawn. */
int *daPeach_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(876);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV13PrincessPeach;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN11ShadowModelC1Ev((char *)p + 0x138);
        _ZN7dCcAc_cC1Ev((char *)p + 0x160);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x194);
    }
    return p;
}
