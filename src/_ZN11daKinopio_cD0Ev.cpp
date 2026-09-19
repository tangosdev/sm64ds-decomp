//cpp
// @symbol _ZN11daKinopio_cD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. */
#include "daKinopio_c.h"

void daKinopio_c_EmitDeletingDestructor(daKinopio_c *toad)
{
    delete toad;
}
