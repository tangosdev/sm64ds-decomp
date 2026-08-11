//cpp
// @symbol _ZN8CapEnemyD2Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The mirror of the constructor, and the second witness for the layout: one
 * vtable store, then mCapIcon at 0x164 and mModel at 0x114 torn down in reverse
 * declaration order, then Enemy::~Enemy. Every one of those is a consequence of
 * `struct CapEnemy : Enemy` and the two members that declaration types.
 *
 * The CapIcon call is written out for the same reason as in the constructor,
 * and sits in the body for the mirror-image reason: the body runs BEFORE any
 * member destructor, and mCapIcon is the last member, so it is destroyed first
 * either way.
 *
 * This is the destructor in vtable slot 16. Slot 17 is _ZN8CapEnemyD0Ev, which
 * stays as it is -- see the note in include/Enemy.h about why the inline
 * operator delete on the immediate base is what lets a D0 reproduce at all.
 */
#include "CapEnemy.h"

extern "C" void func_ov001_020ab3a0(CapIcon *icon);

CapEnemy::~CapEnemy()
{
    func_ov001_020ab3a0(&mCapIcon);
}
