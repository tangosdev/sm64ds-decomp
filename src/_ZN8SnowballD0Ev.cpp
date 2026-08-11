//cpp
// @symbol _ZN8SnowballD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases, then
 * return the object to its heap. Declaring `~Snowball()` is enough -- mwcc emits
 * D2, D0 and D1 together and objisolate keeps the one this file is bound to.
 *
 * The deallocation is an inline operator delete -- Enemy's, reachable because
 * Enemy is this class's IMMEDIATE base.
 */
#include "Snowball.h"

Snowball::~Snowball()
{
}
