//cpp
// @symbol BigBooIcon_Spawn
#include "daTrsIcon_c.h"

/* CodeWarrior has no usable source spelling for the class-specific allocator
 * or placement new (notes/ctor-migration.md, factory wall), so the factory
 * keeps that ABI boundary explicit while using the real class and sizeof. */
extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN8dActor_cC2Ev(void *self);
extern void *_ZTV11daTrsIcon_c;
}

extern "C" daTrsIcon_c *BigBooIcon_Spawn()
{
    daTrsIcon_c *actor = (daTrsIcon_c *)_ZN7fBase_cnwEj(sizeof(daTrsIcon_c));
    if (actor) {
        _ZN8dActor_cC2Ev(actor);
        *(void **)actor = &_ZTV11daTrsIcon_c;
    }
    return actor;
}
