//cpp
// @symbol _ZN15RotatingFirebarD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One array cleanup and dBgActor_c's two members, in reverse declaration order.
 * The array at 0x360 is dCcPos_c[8], one collision cylinder per
 * flame: __destroy_arr(ptr, 8, 0x3c, _ZN8dCcPos_cD1Ev), and 0x3c
 * is sizeof(dCcPos_c).
 */
#include "RotatingFirebar.h"

RotatingFirebar::~RotatingFirebar()
{
}
