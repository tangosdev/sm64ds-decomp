//cpp
// @symbol _ZN21daObj_volcanoCannon_cD0Ev
/* A delete expression forces the deleting destructor. dActor_c's inline
 * operator delete supplies the actor-heap deallocation used by the ROM. */
#include "daObj_volcanoCannon_c.h"

void daObj_volcanoCannon_c_EmitDeletingDestructor(daObj_volcanoCannon_c *fire)
{
    delete fire;
}
