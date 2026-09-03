//cpp
// @symbol _ZN14daObjC1Peach_cD1Ev
/* Force mwccarm to emit the class-body destructor as a genuine C++ D1. */
#include "daObjC1Peach_c.h"

void PeachPainting_EmitDestructor(daObjC1Peach_c *painting)
{
    painting->~daObjC1Peach_c();
}
