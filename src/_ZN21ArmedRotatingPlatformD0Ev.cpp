//cpp
// @symbol _ZN21ArmedRotatingPlatformD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is vtable slot 17: destroy through this class and its Platform base --
 * which is why two vptr stores appear -- then return the object to the actor
 * heap. The deallocation is Actor's inline operator delete, so nothing here
 * mentions a heap either.
 *
 * The body is the same `~ArmedRotatingPlatform()` the D1 file declares; mwcc emits D2, D0
 * and D1 together and objisolate keeps whichever one the file is bound to.
 */
#include "ArmedRotatingPlatform.h"

ArmedRotatingPlatform::~ArmedRotatingPlatform()
{
}
