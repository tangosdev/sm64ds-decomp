//cpp
// @symbol _ZN6CoffinD0Ev
/* A delete-expression forces mwccarm to emit the deleting destructor. The
 * inherited inline dActor_c::operator delete supplies the actor-heap release
 * used by the ROM. */
#include "Coffin.h"

void Coffin_EmitDeletingDestructor(Coffin *p)
{
    delete p;
}
