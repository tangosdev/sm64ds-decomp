//cpp
// @symbol _ZN15RotatingFirebarD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One array cleanup and Platform's two members, in reverse declaration order.
 * The array at 0x360 is CylinderClsnWithPos[8], one collision cylinder per
 * flame: __destroy_arr(ptr, 8, 0x3c, _ZN19CylinderClsnWithPosD1Ev), and 0x3c
 * is sizeof(CylinderClsnWithPos).
 */
#include "RotatingFirebar.h"

RotatingFirebar::~RotatingFirebar()
{
}
