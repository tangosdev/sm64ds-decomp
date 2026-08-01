// @symbol func_ov047_021113bc
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV20daObjKm3_Kaitendai_c */
int *func_ov047_021113bc(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(800);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV20daObjKm3_Kaitendai_c;
        p[0] = (int)VT1;
    }
    return p;
}
