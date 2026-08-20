// @symbol UkikiCage_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
extern int _ZTV9UkikiCage[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV9UkikiCage */
int *UkikiCage_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1248);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV9UkikiCage;
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x320);
    }
    return p;
}
