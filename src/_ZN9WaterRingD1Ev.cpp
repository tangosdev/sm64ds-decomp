//cpp
// @symbol _ZN9WaterRingD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and 5 destructor calls, every one a consequence of
 * `struct WaterRing : Enemy` and the members that declaration now types:
 * its own vptr, then TextureTransformer (0x35c), Model (0x30c), WithMeshClsn (0x150), 
 * MovingCylinderClsnWithPos (0x110)
 * in reverse declaration order, then Enemy::~Enemy.
 *
 * This body is the evidence for the header. It was the hand-written C that
 * named those offsets in the first place, and `WaterRing_Spawn` constructs the
 * same types at the same offsets.
 */
#include "WaterRing.h"

WaterRing::~WaterRing()
{
}
