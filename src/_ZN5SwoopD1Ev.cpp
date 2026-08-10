//cpp
// @symbol _ZN5SwoopD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and six destructor calls, every one a consequence of
 * `struct Swoop : Enemy` and the members that declaration types:
 *   0x3c8 ShadowModel, 0x364 and 0x300 the two ModelAnims,
 *   0x144 WithMeshClsn, 0x110 MovingCylinderClsn,
 * then Enemy::~Enemy, in reverse declaration order.
 *
 * This body is the evidence for the header. Each member's size closes exactly
 * on the next one's offset.
 */
#include "Swoop.h"

Swoop::~Swoop()
{
}
