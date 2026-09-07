//cpp
// @symbol _ZN16BowserShockwavesD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is vtable slot 17: destroy, then return the object to the actor heap. All
 * eight member destructors, the base chain and the deallocation come from the same
 * `~BowserShockwaves()` the D1 file declares; the deallocation is the inline
 * dActor_c::operator delete, which is why nothing here mentions the heap.
 */
#include "BowserShockwaves.h"

BowserShockwaves::~BowserShockwaves()
{
}
