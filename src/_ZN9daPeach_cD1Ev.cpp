//cpp
// @symbol _ZN9daPeach_cD1Ev
/* Force mwccarm to emit the class-body destructor as a genuine C++ D1. */
#include "daPeach_c.h"

void daPeach_c_EmitDestructor(daPeach_c *peach)
{
    peach->~daPeach_c();
}
