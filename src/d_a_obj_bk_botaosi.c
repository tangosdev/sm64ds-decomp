// @symbol daObjBk_Botaosi_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV17daObjBk_Botaosi_c */
/* Reconstructed source-style name: SM64DS proves daObjBk_Botaosi_c through
 * RTTI, allocation size, vtable identity, and the BK_BOTAOSI registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. The class carried the coined name KnockDownPlank
 * before the rename. Historical alias: KnockDownPlank_Spawn. */
int *daObjBk_Botaosi_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(924);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV17daObjBk_Botaosi_c;
        _ZN11ShadowModelC1Ev((char *)p + 0x320);
    }
    return p;
}
