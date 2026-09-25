//cpp
// @symbol _ZN11daTrsIcon_cD1Ev
#include "daTrsIcon_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D1 NAME, AND MSVC ONLY MAKES ONE DESTRUCTOR.
 * It folds the Itanium D1/D0 pair into the single ??1daTrsIcon_c that
 * src/game/actors/daTrsIcon_c/_ZN11daTrsIcon_cD0Ev.cpp compiles for the port,
 * so this file cannot carry the same definition (LNK2005). This arm spells the
 * ROM's own D1 instead, step for step: the store of this class's own table
 * (_ZTV10BigBooIcon, the name the class's factory stores), then the dActor_c
 * base D2, and no deallocation (slot 16's caller frees). Those are the calls
 * the slot-16 thunk bbi_d1 in hal/actor_classes_ov063.cpp made by hand, which
 * forwards here now. Nothing here reaches mwccarm: it builds the `#else` arm
 * and emits the ROM bytes it always emitted, and the object is byte-identical
 * either way. */
extern "C" {
void *_ZN8dActor_cD2Ev(void *self);
extern void *_ZTV10BigBooIcon[];
}
extern "C" daTrsIcon_c *_ZN11daTrsIcon_cD1Ev(daTrsIcon_c *thiz)
{
    *(void **)thiz = (void *)_ZTV10BigBooIcon;  /* this class's own table */
    _ZN8dActor_cD2Ev(thiz);                     /* the base, no deallocation */
    return thiz;
}
#else

daTrsIcon_c::~daTrsIcon_c()
{
}
#endif
