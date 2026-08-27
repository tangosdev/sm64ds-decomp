//cpp
// @symbol _ZN12dEnemyBase_cC2Ev
/* recovered: real C++ base-object constructor
 *
 * The ROM's two steps are the dActor_c base step (_ZN8dActor_cC2Ev) and the
 * _ZTV12dEnemyBase_c vptr store -- exactly what `dEnemyBase_c : dActor_c`
 * with a declared base constructor and the vptr store means, so the body is
 * the vptr store only.
 */
#include "dEnemyBase_c.h"

dEnemyBase_c::dEnemyBase_c()
{
    *(void**)this = data_ov002_021081e4;
}
