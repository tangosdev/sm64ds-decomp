//cpp
// @symbol _ZN11dCapEnemy_cD2Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The mirror of the constructor, and the second witness for the layout: one
 * vtable store, then mCapIcon at 0x164 and mModel at 0x114 torn down in reverse
 * declaration order, then dEnemyBase_c::~dEnemyBase_c. Every one of those is a consequence of
 * `struct dCapEnemy_c : dEnemyBase_c` and the two members that declaration types.
 *
 * dCapIcon_c's ROM-proven destructor is now an actual member destructor, so
 * the empty body emits it first and Model second before the base teardown.
 *
 * The overlay D1 and D0 variants use this same native definition; the inline
 * operator delete on the immediate base is what lets D0 reproduce its heap call.
 */
#include "dCapEnemy_c.h"

dCapEnemy_c::~dCapEnemy_c()
{
}
