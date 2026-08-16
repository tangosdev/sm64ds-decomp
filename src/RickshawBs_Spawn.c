// @symbol RickshawBs_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV17daObjKurumajiku_c[];
extern int data_ov047_021122a0[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV17daObjKurumajiku_c */
int *RickshawBs_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(816);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV17daObjKurumajiku_c;
        p[0] = (int)data_ov047_021122a0;
    }
    return p;
}
