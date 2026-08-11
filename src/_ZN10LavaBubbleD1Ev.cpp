//cpp
// @symbol _ZN10LavaBubbleD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and 3 destructor calls, every one a consequence of
 * `struct LavaBubble : Enemy` and the members that declaration now types:
 * its own vptr, then WithMeshClsn (0x144), 
 * MovingCylinderClsn (0x110)
 * in reverse declaration order, then Enemy::~Enemy.
 *
 * This body is the evidence for the header. It was the hand-written C that
 * named those offsets in the first place, and `LavaBubble_Spawn` constructs the
 * same types at the same offsets.
 */
#include "LavaBubble.h"

LavaBubble::~LavaBubble()
{
}
