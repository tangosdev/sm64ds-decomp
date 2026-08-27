//cpp
// @symbol _ZN10ChainChompD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * Destroy through ChainChomp and dEnemyBase_c, then hand the object back through
 * dActor_c's inline operator delete. The hand-written version declared its own
 * `data_020a0eac` for the actor heap, which collides with the `void *` dActor_c.h
 * supplies for that same symbol once the real header is in scope.
 */
#include "ChainChomp.h"

ChainChomp::~ChainChomp()
{
}
