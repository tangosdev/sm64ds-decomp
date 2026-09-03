//cpp
// @symbol _ZN12daObjClock_cD1Ev
/* Force mwccarm to emit the class-body destructor as a genuine C++ D1. */
#include "daObjClock_c.h"

void daObjClock_c_EmitDestructor(daObjClock_c *hand)
{
    hand->~daObjClock_c();
}
