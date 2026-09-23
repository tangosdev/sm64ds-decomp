//cpp
// @symbol _ZN10daDpLift_cD1Ev
/* Real compiler-spelled complete destructor. daDpLift_c's destructor is
 * inline in the class so its D1/D0 order and lack of a homeless D2 match the
 * original TU. This uncalled helper forces mwcc to materialize D1;
 * objisolate keeps the enrolled destructor and discards the helper. */
#include "daDpLift_c.h"

void PyramidLift_EmitDestructor(daDpLift_c *p)
{
    p->~daDpLift_c();
}
