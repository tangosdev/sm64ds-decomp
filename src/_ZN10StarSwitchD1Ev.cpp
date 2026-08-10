//cpp
// @symbol _ZN10StarSwitchD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and three destructor calls, all of them consequences of
 * `struct StarSwitch : Platform`: its own vptr, then Platform's -- inlined,
 * because Platform's destructor is defined in its class body -- then
 * Platform's Model and MovingMeshCollider, then Actor.
 *
 * This class adds no member with a destructor, so it contributes nothing.
 */
#include "StarSwitch.h"

StarSwitch::~StarSwitch()
{
}
