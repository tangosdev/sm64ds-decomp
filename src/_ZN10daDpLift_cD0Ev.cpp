//cpp
// @symbol _ZN10daDpLift_cD0Ev
/* Real compiler-spelled deleting destructor. The delete-expression selects
 * D0 and reaches dActor_c's inline actor-heap deallocator after the complete
 * daDpLift_c destruction sequence. */
#include "daDpLift_c.h"

void PyramidLift_EmitDeletingDestructor(daDpLift_c *p)
{
    delete p;
}
