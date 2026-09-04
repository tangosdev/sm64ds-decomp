// @symbol daSCoin_c_classInit
/* recovered: vtable identified, declarations from a shared header. Was
 * InvisibleSecret_Spawn -- renamed to match the RTTI class name
 * (_ZTS9daSCoin_c), see include/daSCoin_c.h. */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
extern int _ZTV9daSCoin_c[];
/* vtable identified: VT0 = _ZTV9daSCoin_c */
/* Reconstructed source-style name: SM64DS proves daSCoin_c through RTTI,
 * allocation size, vtable identity, and the SECRET_COIN registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: daSCoin_c_Spawn. */
int *daSCoin_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(276);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV9daSCoin_c;
        _ZN7dCcAc_cC1Ev((char *)p + 0xd4);
    }
    return p;
}
