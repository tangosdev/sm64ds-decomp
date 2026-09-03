//cpp
// @symbol _ZN12daObjClock_cD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. */
#include "daObjClock_c.h"

void daObjClock_c_EmitDeletingDestructor(daObjClock_c *hand)
{
    delete hand;
}
