//cpp
// @symbol _ZN6RabbitD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and 6 destructor calls, every one a consequence of
 * `struct Rabbit : Enemy` and the members that declaration now types:
 * its own vptr, then ShadowModel (0x3c0), ShadowModel (0x368), ModelAnim (0x300), 
 * WithMeshClsn (0x144), 
 * MovingCylinderClsn (0x110)
 * in reverse declaration order, then Enemy::~Enemy.
 *
 * This body is the evidence for the header. It was the hand-written C that
 * named those offsets in the first place, and `Rabbit_Spawn` constructs the
 * same types at the same offsets.
 */
#include "Rabbit.h"

Rabbit::~Rabbit()
{
}
