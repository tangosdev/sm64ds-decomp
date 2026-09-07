// @symbol daObjSimpleBg_c_classInit_BK_TOWER
/* recovered: globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV13FortressTower */
/* Reconstructed source-style name: SM64DS proves daObjSimpleBg_c through RTTI,
 * allocation size, vtable identity, and the BK_TOWER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: FortressTower_Spawn. */
int *daObjSimpleBg_c_classInit_BK_TOWER(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV13FortressTower; }
    return p;
}
