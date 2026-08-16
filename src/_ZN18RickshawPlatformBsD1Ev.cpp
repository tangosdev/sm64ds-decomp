//cpp
// @symbol _ZN18RickshawPlatformBsD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Three vtable stores and five destructor calls, every one a consequence of
 * `struct RickshawPlatformBs : daObjDorifu_c : Platform`: its own vptr, then
 * daObjDorifu_c's, then that class's MovingMeshCollider[5] at 0x4b0 and Model[5]
 * at 0x320 through __destroy_arr, then Platform's vptr and Platform's own
 * MovingMeshCollider and Model, then Actor. The two arrays are declared in
 * include/daObjDorifu_c.h, so the empty body reproduces them.
 */
#include "RickshawPlatformBs.h"

RickshawPlatformBs::~RickshawPlatformBs()
{
}
