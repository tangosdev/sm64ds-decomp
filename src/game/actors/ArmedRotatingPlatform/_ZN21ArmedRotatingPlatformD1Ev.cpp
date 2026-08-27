//cpp
// @symbol _ZN21ArmedRotatingPlatformD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and three destructor calls, every one a consequence of
 * `struct ArmedRotatingPlatform : dBgActor_c`: its own vptr, then dBgActor_c's -- inlined,
 * because dBgActor_c's destructor is defined in its class body -- then
 * dBgActor_c's Model and dBgW_KcMbg, then dActor_c. ArmedRotatingPlatform adds one
 * s16, which has no destructor.
 */
#include "ArmedRotatingPlatform.h"

ArmedRotatingPlatform::~ArmedRotatingPlatform()
{
}
