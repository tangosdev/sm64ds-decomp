// @symbol daObjFl_Koma_D_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV16daObjKaitendai_c[];
extern int _ZTV19RotatingPlatformLll[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV16daObjKaitendai_c */
/* Reconstructed source-style name: SM64DS proves daObjFl_Koma_D_c through
 * RTTI, allocation size, most-derived vtable identity, and the FL_KOMA_D
 * registry profile; later EAD lineage supplies classInit. Exact original
 * spelling is not preserved. The project's RotatingPlatformLll implementation
 * aliases remain unchanged. Historical alias: RotatingPlatformLll_Spawn. */
int *daObjFl_Koma_D_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV16daObjKaitendai_c;
        p[0] = (int)_ZTV19RotatingPlatformLll;
    }
    return p;
}
