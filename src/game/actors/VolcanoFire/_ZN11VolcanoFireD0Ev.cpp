//cpp
// @symbol _ZN11VolcanoFireD0Ev
/* A delete expression forces the deleting destructor. dActor_c's inline
 * operator delete supplies the actor-heap deallocation used by the ROM. */
#include "VolcanoFire.h"

void VolcanoFire_EmitDeletingDestructor(VolcanoFire *fire)
{
    delete fire;
}
