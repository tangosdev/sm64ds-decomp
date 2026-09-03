// @symbol daObjKm1_Kuruma_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV13daObjKuruma_c[];
extern int _ZTV17daObjKm1_Kuruma_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV13daObjKuruma_c */
/* Reconstructed source-style name: SM64DS proves daObjKm1_Kuruma_c through
 * RTTI, allocation size, vtable identity, and the KM1_KURUMA registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. The project's daObjKm1_Kuruma_c implementation alias remains
 * unchanged. Historical alias: daObjKm1_Kuruma_c_Spawn. */
int *daObjKm1_Kuruma_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV13daObjKuruma_c;
        p[0] = (int)_ZTV17daObjKm1_Kuruma_c;
    }
    return p;
}
