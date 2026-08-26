//cpp
// @symbol _ZN9ArrowLiftD0Ev
/* A delete-expression forces mwccarm to emit the deleting destructor. The
 * inherited inline dActor_c::operator delete supplies the actor-heap release
 * used by the ROM. */
#include "ArrowLift.h"

void ArrowLift_EmitDeletingDestructor(ArrowLift *p)
{
    delete p;
}
