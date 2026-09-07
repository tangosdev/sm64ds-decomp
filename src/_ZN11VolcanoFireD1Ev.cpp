//cpp
// @symbol _ZN11VolcanoFireD1Ev
/* Force mwccarm to materialize the inline class-body complete destructor.
 * objisolate retains the enrolled D1 and discards this forcing helper. */
#include "VolcanoFire.h"

void VolcanoFire_EmitDestructor(VolcanoFire *fire)
{
    fire->~VolcanoFire();
}
