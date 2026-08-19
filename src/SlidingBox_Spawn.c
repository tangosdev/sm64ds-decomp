// @symbol SlidingBox_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV10SlidingBox */
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
