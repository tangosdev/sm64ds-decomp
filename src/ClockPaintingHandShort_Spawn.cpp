//cpp
// @symbol ClockPaintingHandShort_Spawn
#include "ClockPaintingHandShort.h"

extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN8dActor_cC2Ev(void *self);
void _ZN5ModelC1Ev(void *self);
extern void *_ZTV22ClockPaintingHandShort;
}

extern "C" ClockPaintingHandShort *ClockPaintingHandShort_Spawn()
{
    ClockPaintingHandShort *actor =
        (ClockPaintingHandShort *)_ZN7fBase_cnwEj(sizeof(ClockPaintingHandShort));
    if (actor) {
        _ZN8dActor_cC2Ev(actor);
        *(void **)actor = &_ZTV22ClockPaintingHandShort;
        _ZN5ModelC1Ev(&actor->mModel);
    }
    return actor;
}
