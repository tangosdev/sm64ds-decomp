// @symbol daObjSeesaw_c_classInit_KM1_SEESAW
/* recovered: globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV9SeesawBob */
/* Reconstructed source-style name: SM64DS proves daObjSeesaw_c through RTTI,
 * allocation size, vtable identity, and the KM1_SEESAW registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: SeesawBdw_Spawn. */
int *daObjSeesaw_c_classInit_KM1_SEESAW(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(808);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV9SeesawBob; }
    return p;
}
