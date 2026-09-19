//cpp
// @symbol _ZN9daPeach_cD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. */
#include "daPeach_c.h"

void daPeach_c_EmitDeletingDestructor(daPeach_c *peach)
{
    delete peach;
}
