//cpp
// @symbol _ZN9daRFlag_cD1Ev
#include "daRFlag_c.h"

/* Force mwccarm to emit the class-body destructor as a genuine C++ D1. */
void daRFlag_c_EmitDestructor(daRFlag_c *flag)
{
    flag->~daRFlag_c();
}
