//cpp
// @symbol _ZN8PathLiftD0Ev
/* The deleting (D0) half of the pair src/_ZN8PathLiftD1Ev.cpp forces. A
 * `delete` expression is what materialises D0 -- destroy through the class and
 * its bases, which is why more than one vptr store appears, then hand the
 * object to the inherited operator delete.
 */
#include "PathLift.h"

void PathLift_EmitDeletingDestructor(PathLift *p)
{
    delete p;
}
