//cpp
// @symbol _ZN5WhompD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and 6 destructor calls, every one a consequence of
 * `struct Whomp : Enemy` and the members that declaration now types:
 * its own vptr, then MovingMeshCollider (0x418), ShadowModel (0x344), 
 * TextureSequence (0x330), ModelAnim (0x2cc), 
 * WithMeshClsn (0x110)
 * in reverse declaration order, then Enemy::~Enemy.
 *
 * This body is the evidence for the header. It was the hand-written C that
 * named those offsets in the first place, and `Whomp_Spawn` constructs the
 * same types at the same offsets.
 */
#include "Whomp.h"

Whomp::~Whomp()
{
}
