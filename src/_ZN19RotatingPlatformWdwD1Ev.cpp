//cpp
// @symbol _ZN19RotatingPlatformWdwD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * THREE vtable stores, and the middle one is the finding. `struct RotatingPlatformWdw :
 * daObjKaitendai_c : Platform` emits its own vptr, then daObjKaitendai_c's --
 * inlined, because that destructor is defined in its class body -- then Platform's,
 * then Platform's MovingMeshCollider and Model, then Actor. Nothing in the chain
 * adds a member with a destructor, so the body is empty.
 */
#include "RotatingPlatformWdw.h"

RotatingPlatformWdw::~RotatingPlatformWdw()
{
}
