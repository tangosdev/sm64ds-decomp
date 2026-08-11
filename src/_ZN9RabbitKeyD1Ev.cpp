//cpp
// @symbol _ZN9RabbitKeyD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and 3 destructor calls, every one a consequence of
 * `struct RabbitKey : Enemy` and the members that declaration now types:
 * its own vptr, then ShadowModel (0x160), 
 * Model (0x110)
 * in reverse declaration order, then Enemy::~Enemy.
 *
 * This body is the evidence for the header. It was the hand-written C that
 * named those offsets in the first place, and `RabbitKey_Spawn` constructs the
 * same types at the same offsets.
 */
#include "RabbitKey.h"

RabbitKey::~RabbitKey()
{
}
