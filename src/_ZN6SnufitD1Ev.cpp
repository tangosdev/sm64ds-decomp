//cpp
// @symbol _ZN6SnufitD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and five destructor calls, every one a consequence of
 * `struct Snufit : Enemy` and the members that declaration types:
 *   0x364 ShadowModel, 0x300 ModelAnim, 0x144 WithMeshClsn,
 *   0x110 MovingCylinderClsn,
 * then Enemy::~Enemy, in reverse declaration order.
 *
 * This body is the evidence for the header. Each member's size closes exactly
 * on the next one's offset.
 */
#include "Snufit.h"

Snufit::~Snufit()
{
}
