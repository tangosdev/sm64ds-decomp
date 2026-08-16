//cpp
// @symbol _ZN11RickshawBdwD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * THREE vtable stores, and the middle one is the finding. `struct RickshawBdw :
 * daObjKurumajiku_c : Platform` emits its own vptr, then daObjKurumajiku_c's --
 * inlined, because that destructor is defined in its class body -- then
 * Platform's, then Platform's MovingMeshCollider and Model, then Actor. Nothing
 * in the chain adds a member with a destructor.
 *
 * Modelled flat as `RickshawBdw : Platform` the body compiles to 0x44 against the
 * ROM's 0x50: one `ldr` + `str` + pool word short, exactly the store the missing
 * class accounts for.
 */
#include "RickshawBdw.h"

RickshawBdw::~RickshawBdw()
{
}
