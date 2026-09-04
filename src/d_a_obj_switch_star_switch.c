// @symbol daObjSwitch_c_classInit_STAR_SWITCH
/* recovered: globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV10StarSwitch */
/* Reconstructed source-style name: SM64DS proves daObjSwitch_c through RTTI,
 * allocation size, vtable identity, and the STAR_SWITCH registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: StarSwitch_Spawn. */
int *daObjSwitch_c_classInit_STAR_SWITCH(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(852);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV10StarSwitch; }
    return p;
}
