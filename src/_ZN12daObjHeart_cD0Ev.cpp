//cpp
// @symbol _ZN12daObjHeart_cD0Ev
#include "daObjHeart_c.h"

/* A delete expression forces the compiler-spelled deleting destructor. */
void daObjHeart_c_EmitDeletingDestructor(daObjHeart_c *heart)
{
    delete heart;
}
