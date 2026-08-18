//cpp
// @symbol _ZN5CrateD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases,
 * then return the object to its heap. Declaring `~Crate()` is enough; mwcc
 * emits D2, D0 and D1 together and objisolate keeps the one this file is
 * bound to. The deallocation is an inline operator delete, matching
 * BigBrickBlock's D0 (include/BigBrickBlock.h, src/_ZN13BigBrickBlockD0Ev.cpp).
 */
#include "Crate.h"

Crate::~Crate()
{
}
