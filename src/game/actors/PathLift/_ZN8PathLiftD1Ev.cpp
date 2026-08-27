//cpp
// @symbol _ZN8PathLiftD1Ev
/* PathLift's destructor is defined INLINE in include/PathLift.h, so that
 * subclasses inline its cleanup the way the ROM's daObjPathLift_c and
 * daObjRcCarpet_c do. The vtable still needs an out-of-line address for slot
 * 0, and this uncalled helper is what makes mwcc materialise it; objisolate
 * keeps the D1 variant this file is bound to and discards the helper.
 */
#include "PathLift.h"

void PathLift_EmitDestructor(PathLift *p)
{
    p->~PathLift();
}
