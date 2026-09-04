// @symbol daObjRc_Kaitendai_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV16daObjKaitendai_c[];
extern int _ZTV19daObjRc_Kaitendai_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV16daObjKaitendai_c */
/* Reconstructed source-style name: SM64DS proves daObjRc_Kaitendai_c through
 * RTTI, allocation size, vtable identity, and the RC_KAITEN registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. The project's daObjRc_Kaitendai_c implementation alias remains
 * unchanged. Historical alias: RotatingPlatformRr_Spawn. */
int *daObjRc_Kaitendai_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV16daObjKaitendai_c;
        p[0] = (int)_ZTV19daObjRc_Kaitendai_c;
    }
    return p;
}
