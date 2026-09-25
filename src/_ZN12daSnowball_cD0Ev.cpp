//cpp
// @symbol _ZN12daSnowball_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases, then
 * return the object to its heap. Declaring `~daSnowball_c()` is enough -- mwcc emits
 * D2, D0 and D1 together and objisolate keeps the one this file is bound to.
 *
 * The deallocation is an inline operator delete -- dEnemyBase_c's, reachable because
 * dEnemyBase_c is this class's IMMEDIATE base.
 */
#include "daSnowball_c.h"

daSnowball_c::~daSnowball_c()
{
}
