// @symbol RickshawBdw_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV17daObjKurumajiku_c[];
extern int _ZTV11RickshawBdw[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV17daObjKurumajiku_c */
int *RickshawBdw_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(816);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV17daObjKurumajiku_c;
        p[0] = (int)_ZTV11RickshawBdw;
    }
    return p;
}
