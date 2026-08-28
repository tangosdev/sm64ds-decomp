//cpp
// @symbol _ZN7dBase_cD0Ev
#include "dBase_c.h"

/* A delete expression forces the compiler-spelled deleting destructor. */
void dBase_c_EmitDeletingDestructor(dBase_c *p)
{
    delete p;
}
