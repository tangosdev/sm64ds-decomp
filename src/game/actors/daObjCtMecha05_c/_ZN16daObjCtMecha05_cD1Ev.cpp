//cpp
// @symbol _ZN16daObjCtMecha05_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and three destructor calls, every one a consequence of
 * `struct daObjCtMecha05_c : dBgActor_c`: its own vptr, then dBgActor_c's --
 * inlined, because dBgActor_c's destructor is defined in its class body --
 * then dBgActor_c's dBgW_KcMbg and Model, then dActor_c. This class
 * adds one member with a destructor of its own, ShadowModel at 0x33c, which
 * the compiler destroys first.
 */
#include "daObjCtMecha05_c.h"

daObjCtMecha05_c::~daObjCtMecha05_c()
{
}
