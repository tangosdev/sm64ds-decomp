//cpp
// @symbol _ZN6ShipUpD0Ev
/* The deleting (D0) half of the pair. A `delete` expression is what
 * materialises D0 -- destroy through this class and its bases, which is why
 * more than one vptr store appears, then hand the object back through the
 * inherited operator delete, which is why nothing below mentions a heap.
 */
#include "ShipUp.h"

void ShipUp_EmitDeletingDestructor(ShipUp *p)
{
    delete p;
}
