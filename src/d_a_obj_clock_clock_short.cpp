//cpp
// @symbol daObjClock_c_classInit_CLOCK_SHORT
#include "ClockPaintingHandShort.h"
#include "decl_common.h"

extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN8dActor_cC2Ev(void *self);
void _ZN5ModelC1Ev(void *self);
}

/* Reconstructed source-style name: SM64DS proves daObjClock_c through RTTI,
 * allocation size, vtable identity, and the CLOCK_SHORT registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: ClockPaintingHandShort_Spawn. */
extern "C" ClockPaintingHandShort *daObjClock_c_classInit_CLOCK_SHORT()
{
    ClockPaintingHandShort *actor =
        (ClockPaintingHandShort *)_ZN7fBase_cnwEj(sizeof(ClockPaintingHandShort));
    if (actor) {
        _ZN8dActor_cC2Ev(actor);
        *(void **)actor = _ZTV22ClockPaintingHandShort;
        _ZN5ModelC1Ev(&actor->mModel);
    }
    return actor;
}
