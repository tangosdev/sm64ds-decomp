//cpp
// @symbol _ZN9PowerStarD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and 6 destructor calls, every one a consequence of
 * `struct PowerStar : dEnemyBase_c` and the members that declaration now types:
 * its own vptr, then ShadowModel (0x3d4), ModelAnim (0x370), ModelAnim (0x30c), 
 * dBgCh_Actr (0x150), 
 * dCcAcPos_c (0x110)
 * in reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 *
 * This body is the evidence for the header. It was the hand-written C that
 * named those offsets in the first place, and `daStar_c_classInit_STAR` constructs the
 * same types at the same offsets.
 */
#include "PowerStar.h"

PowerStar::~PowerStar()
{
}
