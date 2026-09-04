#include "Coffin.h"
// @symbol daObjCasket_c_classInit
/* recovered: globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV6Coffin */
/* Reconstructed source-style name: SM64DS proves daObjCasket_c through RTTI,
 * allocation size, vtable identity, and the CASKET registry profile; later EAD
 * lineage supplies classInit. Exact original spelling is not preserved. The
 * project's Coffin implementation aliases remain unchanged. Historical alias:
 * Coffin_Spawn. */
int *daObjCasket_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(sizeof(struct Coffin));
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV6Coffin; }
    return p;
}
