// @symbol daObj_volcanoCannon_c_classInit
/* The factory remains C because a natural `new daObj_volcanoCannon_c()` makes mwccarm
 * call unresolved `_Znwm`; the ROM calls the class allocator
 * `_ZN7fBase_cnwEj`. The hand-spelled form preserves that destination exactly. */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"

int *daObj_volcanoCannon_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0x11c);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV21daObj_volcanoCannon_c;
        _ZN7dCcAc_cC1Ev((char *)p + 0xd4);
    }
    return p;
}
