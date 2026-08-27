//cpp
// @symbol _ZN11PyramidLiftD0Ev
/* Real compiler-spelled deleting destructor. The delete-expression selects
 * D0 and reaches dActor_c's inline actor-heap deallocator after the complete
 * PyramidLift destruction sequence. */
#include "PyramidLift.h"

void PyramidLift_EmitDeletingDestructor(PyramidLift *p)
{
    delete p;
}
