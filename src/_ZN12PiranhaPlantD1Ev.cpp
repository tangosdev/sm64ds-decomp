//cpp
// @symbol _ZN12PiranhaPlantD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and seven destructor calls, every one a consequence of
 * `struct PiranhaPlant : dEnemyBase_c` and the members that declaration types, destroyed in
 * reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 *
 * This body is the evidence for the header: each member's size closes exactly
 * on the next one's offset.
 */
#include "PiranhaPlant.h"

PiranhaPlant::~PiranhaPlant()
{
}
