// @symbol FloatingFloorLllSmall_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV14daObjUkiyuka_c[];
extern int _ZTV21FloatingFloorLllSmall[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV14daObjUkiyuka_c */
int *FloatingFloorLllSmall_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(816);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV14daObjUkiyuka_c;
        p[0] = (int)_ZTV21FloatingFloorLllSmall;
    }
    return p;
}
