//cpp
// @symbol _ZN10HootTheOwlD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and five destructor calls, every one a consequence of
 * `struct HootTheOwl : Enemy` and the four members that declaration now types:
 * its own vptr, then ShadowModel (0x370), ModelAnim (0x30c), WithMeshClsn
 * (0x150) and MovingCylinderClsnWithPos (0x110) in reverse declaration order,
 * then Enemy::~Enemy.
 *
 * This body is the evidence for the whole header. It was the hand-written C
 * that named those four offsets in the first place, and each member's size
 * closes exactly on the next one's offset.
 */
#include "HootTheOwl.h"

HootTheOwl::~HootTheOwl()
{
}
