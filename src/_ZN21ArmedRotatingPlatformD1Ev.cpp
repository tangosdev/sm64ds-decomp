//cpp
// @symbol _ZN21ArmedRotatingPlatformD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and three destructor calls, every one a consequence of
 * `struct ArmedRotatingPlatform : Platform`: its own vptr, then Platform's -- inlined,
 * because Platform's destructor is defined in its class body -- then
 * Platform's Model and MovingMeshCollider, then Actor. ArmedRotatingPlatform adds one
 * s16, which has no destructor.
 */
#include "ArmedRotatingPlatform.h"

ArmedRotatingPlatform::~ArmedRotatingPlatform()
{
}
