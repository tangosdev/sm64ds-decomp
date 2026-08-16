//cpp
// @symbol _ZN16daObjRc_Dorifu_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Three vtable stores and five destructor calls, every one a consequence of
 * `struct daObjRc_Dorifu_c : daObjDorifu_c : Platform`: its own vptr, then
 * daObjDorifu_c's, then that class's MovingMeshCollider[5] at 0x4b0 and Model[5]
 * at 0x320 through __destroy_arr, then Platform's vptr and Platform's own
 * MovingMeshCollider and Model, then Actor.
 */
#include "daObjRc_Dorifu_c.h"

daObjRc_Dorifu_c::~daObjRc_Dorifu_c()
{
}
