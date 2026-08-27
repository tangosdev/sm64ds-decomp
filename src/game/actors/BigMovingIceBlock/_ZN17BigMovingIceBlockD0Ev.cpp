//cpp
// @symbol _ZN17BigMovingIceBlockD0Ev
/* Real compiler-spelled deleting destructor. The delete-expression selects
 * D0 and reaches dActor_c's inline actor-heap deallocator after the complete
 * BigMovingIceBlock destruction sequence. */
#include "BigMovingIceBlock.h"

void BigMovingIceBlock_EmitDeletingDestructor(BigMovingIceBlock *p)
{
    delete p;
}
