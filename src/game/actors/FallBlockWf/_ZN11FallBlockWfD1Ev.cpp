//cpp
// @symbol _ZN11FallBlockWfD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * THREE vtable stores, and the middle one is the finding. `struct FallBlockWf :
 * daObjFallBlock_c : dBgActor_c` emits its own vptr, then daObjFallBlock_c's --
 * inlined, because that destructor is defined in its class body -- then dBgActor_c's,
 * then dBgActor_c's dBgW_KcMbg and Model, then dActor_c. Nothing in the chain
 * adds a member with a destructor, so the body is empty.
 */
#include "FallBlockWf.h"

FallBlockWf::~FallBlockWf()
{
}
