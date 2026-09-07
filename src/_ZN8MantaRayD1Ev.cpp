//cpp
// @symbol _ZN8MantaRayD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and 4 destructor calls, every one a consequence of
 * `struct MantaRay : dEnemyBase_c` and the members that declaration now types:
 * its own vptr, then ModelAnim (0x30c), dBgCh_Actr (0x150), 
 * dCcAcPos_c (0x110)
 * in reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 *
 * This body is the evidence for the header. It was the hand-written C that
 * named those offsets in the first place, and `MantaRay_Spawn` constructs the
 * same types at the same offsets.
 */
#include "MantaRay.h"

MantaRay::~MantaRay()
{
}
