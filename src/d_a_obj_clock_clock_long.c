// @symbol daObjClock_c_classInit_CLOCK_LONG
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV22ClockPaintingHandShort */
/* Reconstructed source-style name: SM64DS proves daObjClock_c through RTTI,
 * allocation size, vtable identity, and the CLOCK_LONG registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: ClockPaintingHandLong_Spawn. */
int *daObjClock_c_classInit_CLOCK_LONG(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(296);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV22ClockPaintingHandShort;
        _ZN5ModelC1Ev((char *)p + 0xd4);
    }
    return p;
}
