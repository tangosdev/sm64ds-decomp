//cpp
// @symbol _ZN11PyramidLiftD1Ev
/* Real compiler-spelled complete destructor. PyramidLift's destructor is
 * inline in the class so its D1/D0 order and lack of a homeless D2 match the
 * original TU. This uncalled helper forces mwcc to materialize D1;
 * objisolate keeps the enrolled destructor and discards the helper. */
#include "PyramidLift.h"

void PyramidLift_EmitDestructor(PyramidLift *p)
{
    p->~PyramidLift();
}
