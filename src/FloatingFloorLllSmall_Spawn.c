// @symbol FloatingFloorLllSmall_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int data_ov002_0210912c[];
extern int _ZTV21FloatingFloorLllSmall[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov002_0210912c */
int *FloatingFloorLllSmall_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(816);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)data_ov002_0210912c;
        p[0] = (int)_ZTV21FloatingFloorLllSmall;
    }
    return p;
}
