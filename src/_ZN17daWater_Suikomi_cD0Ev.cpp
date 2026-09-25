//cpp
// @symbol _ZN17daWater_Suikomi_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: the same body as D1 (its own vptr, the two
 * collision members, dEnemyBase_c's D2), then return the object to its
 * heap. Nobody writes that; declaring `~daWater_Suikomi_c()` is enough, because mwcc
 * emits D2, D0 and D1 together and objisolate keeps the one this file is bound
 * to.
 *
 * The deallocation is an inline operator delete, which is why nothing below
 * mentions a heap.
 */
#include "daWater_Suikomi_c.h"

daWater_Suikomi_c::~daWater_Suikomi_c()
{
}
