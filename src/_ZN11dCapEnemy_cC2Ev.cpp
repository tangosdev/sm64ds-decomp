//cpp
// @symbol _ZN11dCapEnemy_cC2Ev
/* recovered: real C++ constructor -- the compiler emits the whole body
 *
 * Four things happen here and the compiler is responsible for three of them:
 * dEnemyBase_c's constructor runs because dCapEnemy_c derives from dEnemyBase_c, the vtable
 * store happens because the class declares a virtual destructor, and Model's
 * constructor runs because mModel is a member, and dCapIcon_c's ROM-proven
 * constructor runs because mCapIcon is the final member. The empty body is
 * therefore the original declaration's complete construction sequence.
 */
#include "dCapEnemy_c.h"

dCapEnemy_c::dCapEnemy_c()
{
}
