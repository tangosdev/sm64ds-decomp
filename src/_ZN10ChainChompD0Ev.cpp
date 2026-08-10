//cpp
// @symbol _ZN10ChainChompD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * Destroy through ChainChomp and Enemy, then hand the object back through
 * Actor's inline operator delete. The hand-written version declared its own
 * `data_020a0eac` for the actor heap, which collides with the `void *` Actor.h
 * supplies for that same symbol once the real header is in scope.
 */
#include "ChainChomp.h"

ChainChomp::~ChainChomp()
{
}
