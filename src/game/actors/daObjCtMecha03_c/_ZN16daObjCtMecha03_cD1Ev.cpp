//cpp
// @symbol _ZN16daObjCtMecha03_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and three destructor calls, every one a consequence of
 * `struct daObjCtMecha03_c : dBgActor_c`: its own vptr, then dBgActor_c's --
 * inlined, because dBgActor_c's destructor is defined in its class body --
 * then dBgActor_c's Model and dBgW_KcMbg, then dActor_c. This class
 * adds one member with a destructor of its own, ShadowModel at 0x330, which
 * the compiler destroys first.
 */
#include "daObjCtMecha03_c.h"

daObjCtMecha03_c::~daObjCtMecha03_c()
{
}
