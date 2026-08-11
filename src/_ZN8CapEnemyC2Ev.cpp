//cpp
// @symbol _ZN8CapEnemyC2Ev
/* recovered: real C++ constructor -- the compiler emits the whole body
 *
 * Four things happen here and the compiler is responsible for three of them:
 * Enemy's constructor runs because CapEnemy derives from Enemy, the vtable
 * store happens because the class declares a virtual destructor, and Model's
 * constructor runs because mModel is a member. Only the CapIcon call is
 * written out, and only because that type's constructor is still spelt
 * func_ov001_020ab3c4 rather than CapIcon::CapIcon -- naming it would make
 * this body empty too.
 *
 * Placing it in the body is not a workaround for the ordering: a constructor
 * body runs after every member's constructor, and mCapIcon is the last member,
 * so "last member constructed" and "first statement of the body" are the same
 * position. That is exactly where the ROM calls it.
 */
#include "CapEnemy.h"

extern "C" void func_ov001_020ab3c4(CapIcon *icon);

CapEnemy::CapEnemy()
{
    func_ov001_020ab3c4(&mCapIcon);
}
