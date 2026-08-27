//cpp
// @symbol _ZN9WaterBombD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and 5 destructor calls, every one a consequence of
 * `struct WaterBomb : dEnemyBase_c` and the members that declaration now types:
 * its own vptr, then ShadowModel (0x350), Model (0x300), dBgCh_Actr (0x144), 
 * dCcAc_c (0x110)
 * in reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 *
 * This body is the evidence for the header. It was the hand-written C that
 * named those offsets in the first place, and `WaterBomb_Spawn` constructs the
 * same types at the same offsets.
 */
#include "WaterBomb.h"

WaterBomb::~WaterBomb()
{
}
