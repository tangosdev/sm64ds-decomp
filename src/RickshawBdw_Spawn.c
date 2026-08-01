// @symbol RickshawBdw_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV21daObjKm1_Kurumajiku_c */
int *RickshawBdw_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(816);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV21daObjKm1_Kurumajiku_c;
        p[0] = (int)VT1;
    }
    return p;
}
