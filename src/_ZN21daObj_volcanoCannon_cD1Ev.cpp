//cpp
// @symbol _ZN21daObj_volcanoCannon_cD1Ev
/* Force mwccarm to materialize the inline class-body complete destructor.
 * objisolate retains the enrolled D1 and discards this forcing helper. */
#include "daObj_volcanoCannon_c.h"

void daObj_volcanoCannon_c_EmitDestructor(daObj_volcanoCannon_c *fire)
{
    fire->~daObj_volcanoCannon_c();
}
