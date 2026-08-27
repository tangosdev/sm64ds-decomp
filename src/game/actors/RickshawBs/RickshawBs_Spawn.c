// @symbol RickshawBs_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV17daObjKurumajiku_c[];
extern int _ZTV21daObjKm3_Kurumajiku_c[];
/* recovered: vtable identified. This is the factory for daObjKm3_Kurumajiku_c
   (paired by vtable address, not by this function's own name -- see
   include/daObjKm3_Kurumajiku_c.h). */
int *RickshawBs_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(816);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV17daObjKurumajiku_c;
        p[0] = (int)_ZTV21daObjKm3_Kurumajiku_c;
    }
    return p;
}
