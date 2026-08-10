//cpp
// @symbol _ZN17BowserSkyPlatformD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is vtable slot 17: destroy, then return the object to the actor heap. Both
 * halves come from the same `~BowserSkyPlatform()` the D1 file declares; the
 * deallocation is the inline Actor::operator delete, which is why nothing here
 * mentions the heap.
 */
#include "BowserSkyPlatform.h"

BowserSkyPlatform::~BowserSkyPlatform()
{
}
