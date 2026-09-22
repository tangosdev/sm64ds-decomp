//cpp
// @symbol _ZN10dCcAcPos_c6GetPosEv
/* recovered: real C++ method, named members
 *
 * Vtable slot 2. `add r0, r0, #0x34; bx lr'. This shadows dCcAc_c's own slot 2,
 * which returns the OWNER's position; this class carries its own.
 */
#include "dCcAcPos_c.h"

Vector3 &dCcAcPos_c::GetPos()
{
    return pos;
}
