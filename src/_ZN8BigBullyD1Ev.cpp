//cpp
// @symbol _ZN8BigBullyD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Identical in shape to Bully's, and for the identical reason: `struct BigBully :
 * daOts_c` with no member of its own that has a destructor. Its own vptr, daOts_c's
 * inlined, the base's four members backwards, then dEnemyBase_c.
 */
#include "BigBully.h"

BigBully::~BigBully()
{
}
