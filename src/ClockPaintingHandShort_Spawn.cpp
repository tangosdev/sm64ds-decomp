//cpp
// @symbol ClockPaintingHandShort_Spawn
#include "daObjClock_c.h"
#include "decl_common.h"

extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN8dActor_cC2Ev(void *self);
void _ZN5ModelC1Ev(void *self);
}

extern "C" daObjClock_c *ClockPaintingHandShort_Spawn()
{
    daObjClock_c *actor =
        (daObjClock_c *)_ZN7fBase_cnwEj(sizeof(daObjClock_c));
    if (actor) {
        _ZN8dActor_cC2Ev(actor);
        *(void **)actor = _ZTV12daObjClock_c;
        _ZN5ModelC1Ev(&actor->mModel);
    }
    return actor;
}
