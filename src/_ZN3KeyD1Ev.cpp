//cpp
// @symbol _ZN3KeyD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and 6 destructor calls, every one a consequence of
 * `struct Key : Enemy` and the members that declaration now types:
 * its own vptr, then WithMeshClsn (0x260), MovingCylinderClsnWithPos (0x220), 
 * ShadowModel (0x1c8), Model (0x178), 
 * ModelAnim (0x114)
 * in reverse declaration order, then Enemy::~Enemy.
 *
 * This body is the evidence for the header. It was the hand-written C that
 * named those offsets in the first place, and `Key_Spawn` constructs the
 * same types at the same offsets.
 */
#include "Key.h"

Key::~Key()
{
}
