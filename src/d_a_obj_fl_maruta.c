// @symbol daObjFlMaruta_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV13RollingLogLll[];
extern int _ZTV13daObjMaruta_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV13daObjMaruta_c */
/* Reconstructed source-style name: SM64DS proves daObjFlMaruta_c through RTTI,
 * allocation size, most-derived vtable identity, and the FL_MARUTA registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. The project's RollingLogLll implementation aliases remain
 * unchanged. Historical alias: RollingLogLll_Spawn. */
int *daObjFlMaruta_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(836);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV13daObjMaruta_c;
        p[0] = (int)_ZTV13RollingLogLll;
    }
    return p;
}
