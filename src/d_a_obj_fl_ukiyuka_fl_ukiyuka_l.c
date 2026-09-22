// @symbol daObjFl_Ukiyuka_c_classInit_FL_UKIYUKA_L
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV14daObjUkiyuka_c[];
extern int _ZTV21FloatingFloorLllSmall[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV14daObjUkiyuka_c */
/* Reconstructed source-style name: SM64DS proves daObjFl_Ukiyuka_c through RTTI,
 * allocation size, vtable identity, and the FL_UKIYUKA_L registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: FloatingFloorLllSmall_Spawn. */
int *daObjFl_Ukiyuka_c_classInit_FL_UKIYUKA_L(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(816);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV14daObjUkiyuka_c;
        p[0] = (int)_ZTV21FloatingFloorLllSmall;
    }
    return p;
}
