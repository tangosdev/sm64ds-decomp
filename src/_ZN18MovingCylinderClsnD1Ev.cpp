//cpp
// @symbol _ZN18MovingCylinderClsnD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version spelled out the two things a complete-object
 * destructor does anyway: store this class's vtable over the one the base
 * constructor left behind, then run the CylinderClsn subobject destructor. Both
 * come out of `struct MovingCylinderClsn : CylinderClsn` and `virtual ~MovingCylinderClsn()` in the
 * header, so there is nothing left to write.
 *
 * The one member is a back-pointer to the owning Actor -- not owned, and with
 * no destructor -- so the ROM's 0x24 bytes are the vtable store and the base
 * call.
 */
#include "MovingCylinderClsn.h"

MovingCylinderClsn::~MovingCylinderClsn()
{
}
