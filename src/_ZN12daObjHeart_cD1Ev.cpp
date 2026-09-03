//cpp
// @symbol _ZN12daObjHeart_cD1Ev
#include "daObjHeart_c.h"

/* Force mwccarm to emit the class-body destructor as a genuine C++ D1. */
void daObjHeart_c_EmitDestructor(daObjHeart_c *heart)
{
    heart->~daObjHeart_c();
}
