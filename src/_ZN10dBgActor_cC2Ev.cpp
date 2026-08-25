//cpp
// @symbol _ZN10dBgActor_cC2Ev
/* recovered: real C++ base-object constructor
 *
 * The ROM's four steps are the dActor_c base step (_ZN8dActor_cC2Ev), the
 * _ZTV10dBgActor_c vptr store, then the two member constructions in
 * declaration order -- mModel at +0xd4, mMeshCollider at +0x124. That is the
 * compiler's own order for `dBgActor_c : dActor_c` with typed members, so
 * the body is empty.
 */
#include "dBgActor_c.h"

dBgActor_c::dBgActor_c()
{
}
