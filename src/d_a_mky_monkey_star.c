// @symbol daMky_c_classInit_MONKEY_STAR
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV5Ukiki */
extern void _ZN7PathPtrC1Ev(void *);
/* Reconstructed source-style name: SM64DS proves daMky_c through RTTI,
 * allocation size, vtable identity, and the MONKEY_STAR registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: UkikiStar_Spawn. */
int *daMky_c_classInit_MONKEY_STAR(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(972);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV5Ukiki;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN11ShadowModelC1Ev((char *)p + 0x138);
        _ZN7dCcAc_cC1Ev((char *)p + 0x160);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x194);
        _ZN7PathPtrC1Ev((char *)p + 0x398);
    }
    return p;
}
