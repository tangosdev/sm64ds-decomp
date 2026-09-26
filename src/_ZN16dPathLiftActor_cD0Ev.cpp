//cpp
// @symbol _ZN16dPathLiftActor_cD0Ev
/* The deleting (D0) half of the dPathLiftActor_c lifecycle pair. A
 * `delete` expression is what materialises D0 -- destroy through the class and
 * its bases, which is why more than one vptr store appears, then hand the
 * object to the inherited operator delete.
 */
#include "PathLift.h"

void dPathLiftActor_c_EmitDeletingDestructor(dPathLiftActor_c *p)
{
    delete p;
}
