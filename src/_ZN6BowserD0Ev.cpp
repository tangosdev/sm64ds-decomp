//cpp
// @symbol _ZN6BowserD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is vtable slot 17: destroy, then return the object to the actor heap. All five
 * member destructors, the base chain and the deallocation come from the same
 * `~Bowser()` the D1 file declares; the deallocation is the inline
 * dActor_c::operator delete, which is why nothing here mentions the heap.
 */
#include "Bowser.h"

Bowser::~Bowser()
{
}
