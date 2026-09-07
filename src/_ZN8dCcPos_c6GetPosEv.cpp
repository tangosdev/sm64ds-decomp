//cpp
// @symbol _ZN8dCcPos_c6GetPosEv
/* recovered: real C++ method, named members
 *
 * Vtable slot 2. `add r0, r0, #0x30; bx lr' -- the whole body is the reference
 * itself, so returning the member by reference is the entire function.
 */
#include "dCcPos_c.h"

Vector3 &dCcPos_c::GetPos()
{
    return pos;
}
