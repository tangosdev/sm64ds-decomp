//cpp
// @symbol _ZN6DorrieD0Ev
/* A delete-expression forces mwccarm to materialize Dorrie's deleting
 * destructor and use dActor_c's actor-heap deallocator. */
#include "Dorrie.h"

void Dorrie_EmitDeletingDestructor(Dorrie *p)
{
    delete p;
}
