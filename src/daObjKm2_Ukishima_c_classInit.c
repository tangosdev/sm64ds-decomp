// @symbol daObjKm2_Ukishima_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV14daObjUkiyuka_c[];
extern int _ZTV19daObjKm2_Ukishima_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV14daObjUkiyuka_c */
/* Reconstructed source-style name: SM64DS proves daObjKm2_Ukishima_c through
 * RTTI, allocation size, vtable identity, and the KM2_UKISHIMA registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: FloatingFloorBfs_Spawn. */
int *daObjKm2_Ukishima_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(812);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV14daObjUkiyuka_c;
        p[0] = (int)_ZTV19daObjKm2_Ukishima_c;
    }
    return p;
}
