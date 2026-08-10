//cpp
// @symbol _ZN8PathLiftD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One array cleanup and Platform's two members, in reverse declaration order.
 * The array at 0x320 is Model[3]: the ROM destroys it with
 * __destroy_arr(ptr, 3, 0x50, _ZN5ModelD1Ev), where 0x50 is sizeof(Model) and
 * the element destructor is a name the tree already had.
 */
#include "PathLift.h"

PathLift::~PathLift()
{
}
