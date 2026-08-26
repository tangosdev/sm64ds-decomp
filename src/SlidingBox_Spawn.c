// @symbol SlidingBox_Spawn
/* Measured C/ABI wall: `return new SlidingBox` reproduces the instruction
 * bytes, but its allocator relocation names unresolved global `_Znwm` while
 * the ROM calls fBase_c::operator new at 0x02043444. Keep the explicit actor
 * allocation and construction sequence until that allocator can be expressed
 * honestly in C++. */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"

int *SlidingBox_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1272);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV10SlidingBox;
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x324);
    }
    return p;
}
