//cpp
// @symbol _ZN18daObjKm2_Ami_Bou_cD0Ev
/* A delete-expression forces mwccarm to emit the deleting destructor. The
 * inherited inline dActor_c::operator delete supplies the actor-heap release
 * used by the ROM, which is why nothing below mentions a heap.
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap.
 */
#include "daObjKm2_Ami_Bou_c.h"

void daObjKm2_Ami_Bou_c_EmitDeletingDestructor(daObjKm2_Ami_Bou_c *p)
{
    delete p;
}
