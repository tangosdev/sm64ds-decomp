//cpp
// @symbol _ZN9PowerStarD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and 6 destructor calls, every one a consequence of
 * `struct PowerStar : Enemy` and the members that declaration now types:
 * its own vptr, then ShadowModel (0x3d4), ModelAnim (0x370), ModelAnim (0x30c), 
 * WithMeshClsn (0x150), 
 * MovingCylinderClsnWithPos (0x110)
 * in reverse declaration order, then Enemy::~Enemy.
 *
 * This body is the evidence for the header. It was the hand-written C that
 * named those offsets in the first place, and `PowerStar_Spawn` constructs the
 * same types at the same offsets.
 */
#include "PowerStar.h"

PowerStar::~PowerStar()
{
}
