// @symbol daObjKm1_Kurumajiku_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV17daObjKurumajiku_c[];
extern int _ZTV11RickshawBdw[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV17daObjKurumajiku_c */
/* Reconstructed source-style name: SM64DS proves daObjKm1_Kurumajiku_c
 * through RTTI, allocation size, vtable identity, and the KM1_KURUMAJIKU
 * registry profile; later EAD lineage supplies classInit. Exact original
 * spelling is not preserved. The project's RickshawBdw implementation alias
 * remains unchanged. Historical alias: RickshawBdw_Spawn. */
int *daObjKm1_Kurumajiku_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(816);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV17daObjKurumajiku_c;
        p[0] = (int)_ZTV11RickshawBdw;
    }
    return p;
}
