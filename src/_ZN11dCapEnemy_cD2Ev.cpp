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
 * This is the destructor in vtable slot 16. Slot 17 is _ZN11dCapEnemy_cD0Ev, which
 * stays as it is -- see the note in include/dEnemyBase_c.h about why the inline
 * operator delete on the immediate base is what lets a D0 reproduce at all.
 */
#include "dCapEnemy_c.h"

dCapEnemy_c::~dCapEnemy_c()
{
}
