//cpp
// @symbol _ZN25MovingCylinderClsnWithPosD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version spelled out the two things a complete-object
 * destructor does anyway: store this class's vtable over the one the base
 * constructor left behind, then run the MovingCylinderClsn subobject destructor. Both
 * come out of `struct MovingCylinderClsnWithPos : MovingCylinderClsn` and `virtual ~MovingCylinderClsnWithPos()` in the
 * header, so there is nothing left to write.
 *
 * This class adds no members of its own; it combines the owner pointer it
 * inherits with a position. The whole destructor is the vtable store and the
 * base call.
 */
#include "MovingCylinderClsnWithPos.h"

MovingCylinderClsnWithPos::~MovingCylinderClsnWithPos()
{
}
