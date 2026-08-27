//cpp
// @symbol _ZN12FallBlockBbhD0Ev
/* D0, the DELETING destructor: run the destructor body, then hand the object
 * to operator delete. Longer than the D1/D2 pair, but built from the same
 * one `FallBlockBbh::~FallBlockBbh()` definition. The class `operator delete` in the header
 * is what makes the tail call land on the right deallocator. */
#include "FallBlockBbh.h"

FallBlockBbh::~FallBlockBbh()
{
}
