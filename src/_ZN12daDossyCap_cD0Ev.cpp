//cpp
// @symbol _ZN12daDossyCap_cD0Ev
/* A delete-expression forces mwccarm to materialize daDossyCap_c's deleting
 * destructor and use dActor_c's actor-heap deallocator. */
#include "daDossyCap_c.h"

void daDossyCap_c_EmitDeletingDestructor(daDossyCap_c *cap)
{
    delete cap;
}
