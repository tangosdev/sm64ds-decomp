//cpp
// @symbol _ZN9daDossy_cD0Ev
/* A delete-expression forces mwccarm to materialize daDossy_c's deleting
 * destructor and use dActor_c's actor-heap deallocator. */
#include "daDossy_c.h"

void daDossy_c_EmitDeletingDestructor(daDossy_c *p)
{
    delete p;
}
