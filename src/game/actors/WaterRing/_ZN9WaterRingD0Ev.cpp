//cpp
// @symbol _ZN9WaterRingD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases, then
 * return the object to its heap. Nobody writes that; declaring `~WaterRing()`
 * is enough, because mwcc emits D2, D0 and D1 together and objisolate keeps the
 * one this file is bound to.
 *
 * The deallocation is an inline operator delete -- dEnemyBase_c's, reached because
 * dEnemyBase_c is this class's IMMEDIATE base -- which is why nothing below mentions a
 * heap.
 */
#include "WaterRing.h"

WaterRing::~WaterRing()
{
}
