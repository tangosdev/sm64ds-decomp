//cpp
// @symbol _ZN11daObjFire_cD1Ev
/* Force mwccarm to emit the class-body destructor as a genuine C++ D1. */
#include "daObjFire_c.h"

void daObjFire_c_EmitDestructor(daObjFire_c *flame)
{
    flame->~daObjFire_c();
}
