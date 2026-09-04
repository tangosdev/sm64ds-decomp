// @symbol daObjBk_Rotebar_c_classInit
/* recovered: globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV17daObjBk_Rotebar_c */
/* Reconstructed source-style name: SM64DS proves daObjBk_Rotebar_c through
 * RTTI, allocation size, vtable identity, and the BK_ROTEBAR registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. The project's daObjBk_Rotebar_c implementation aliases remain
 * unchanged. Historical alias: RotatingBridge_Spawn. */
int *daObjBk_Rotebar_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(804);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV17daObjBk_Rotebar_c; }
    return p;
}
