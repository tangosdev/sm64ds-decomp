//cpp
// @symbol _ZN15daObjHmMaruta_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * THREE vtable stores, and the middle one is the finding. `struct daObjHmMaruta_c :
 * daObjMaruta_c : dBgActor_c` emits its own vptr, then daObjMaruta_c's --
 * inlined, because that destructor is defined in its class body -- then dBgActor_c's,
 * then dBgActor_c's dBgW_KcMbg and Model, then dActor_c. Nothing in the chain
 * adds a member with a destructor, so the body is empty.
 */
#include "daObjHmMaruta_c.h"

daObjHmMaruta_c::~daObjHmMaruta_c()
{
}
