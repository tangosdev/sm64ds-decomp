// @symbol daObjBk_Ukisima_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV16daObjKaitendai_c[];
extern int _ZTV17daObjBk_Ukisima_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV16daObjKaitendai_c */
/* Reconstructed source-style name: SM64DS proves daObjBk_Ukisima_c through
 * RTTI, allocation size, most-derived vtable identity, and the BK_UKISIMA
 * registry profile; later EAD lineage supplies classInit. Exact original
 * spelling is not preserved. The project's daObjBk_Ukisima_c implementation
 * aliases remain unchanged. Historical alias: RotatingPlatformWf_Spawn. */
int *daObjBk_Ukisima_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV16daObjKaitendai_c;
        p[0] = (int)_ZTV17daObjBk_Ukisima_c;
    }
    return p;
}
