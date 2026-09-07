//cpp
// @symbol _ZN7daPkn_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and seven destructor calls, every one a consequence of
 * `struct daPkn_c : dEnemyBase_c` and the members that declaration types, destroyed in
 * reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 *
 * This body is the evidence for the header: each member's size closes exactly
 * on the next one's offset.
 */
#include "daPkn_c.h"

daPkn_c::~daPkn_c()
{
}
