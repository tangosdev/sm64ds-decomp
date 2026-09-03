//cpp
// @symbol _ZN11daObjFire_cD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. */
#include "daObjFire_c.h"

void daObjFire_c_EmitDeletingDestructor(daObjFire_c *flame)
{
    delete flame;
}
