// @symbol FloatingFloorBfs_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV14daObjUkiyuka_c[];
extern int _ZTV16FloatingFloorBfs[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV14daObjUkiyuka_c */
int *FloatingFloorBfs_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(812);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV14daObjUkiyuka_c;
        p[0] = (int)_ZTV16FloatingFloorBfs;
    }
    return p;
}
