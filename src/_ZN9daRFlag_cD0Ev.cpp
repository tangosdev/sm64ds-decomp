//cpp
// @symbol _ZN9daRFlag_cD0Ev
#include "daRFlag_c.h"

/* A delete expression forces the compiler-spelled deleting destructor. */
void daRFlag_c_EmitDeletingDestructor(daRFlag_c *flag)
{
    delete flag;
}
