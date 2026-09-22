//cpp
// @symbol _ZN6KleptoD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and six destructor calls, every one a consequence of
 * `struct Klepto : dEnemyBase_c` and the members that declaration types, destroyed in
 * reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 *
 * This body is the evidence for the header: each member's size closes exactly
 * on the next one's offset.
 */
#include "Klepto.h"

Klepto::~Klepto()
{
}
