//cpp
// @symbol _ZN13KoopaTheQuickD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and five destructor calls, every one a consequence of
 * `struct KoopaTheQuick : Enemy` and the members that declaration types, destroyed in
 * reverse declaration order, then Enemy::~Enemy.
 *
 * This body is the evidence for the header: each member's size closes exactly
 * on the next one's offset.
 */
#include "KoopaTheQuick.h"

KoopaTheQuick::~KoopaTheQuick()
{
}
