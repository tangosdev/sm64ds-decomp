//cpp
// @symbol _ZN20daObjFl_Fall_Block_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * THREE vtable stores, matching the sibling include/FallBlockWf.h documents
 * for daObjBk_Fall_Block_c: `struct daObjFl_Fall_Block_c : daObjFallBlock_c :
 * dBgActor_c` emits its own vptr, then daObjFallBlock_c's -- inlined, because
 * that destructor is defined in its class body -- then dBgActor_c's, then
 * dBgActor_c's dBgW_KcMbg and Model, then dActor_c. Nothing in the
 * chain adds a member with a destructor.
 */
#include "daObjFl_Fall_Block_c.h"

daObjFl_Fall_Block_c::~daObjFl_Fall_Block_c()
{
}
