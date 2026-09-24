//cpp
// @symbol _ZN13daObjCasket_cD0Ev
/* A delete-expression forces mwccarm to emit the deleting destructor. The
 * inherited inline dActor_c::operator delete supplies the actor-heap release
 * used by the ROM. */
#include "daObjCasket_c.h"

void Coffin_EmitDeletingDestructor(daObjCasket_c *p)
{
    delete p;
}
