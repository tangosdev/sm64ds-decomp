//cpp
// @symbol _ZN18BowserFireSeaArenaD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is vtable slot 17: destroy -- through both this class and its dBgActor_c base,
 * which is why two vptr stores appear -- then return the object to the actor heap.
 * The deallocation is the inline dActor_c::operator delete, which is why nothing here
 * mentions the heap.
 */
#include "BowserFireSeaArena.h"

BowserFireSeaArena::~BowserFireSeaArena()
{
}
