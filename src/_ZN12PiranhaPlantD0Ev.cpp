//cpp
// @symbol _ZN12PiranhaPlantD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases, then
 * return the object to its heap. Nobody writes that; declaring `~PiranhaPlant()` is
 * enough, because mwcc emits D2, D0 and D1 together and objisolate keeps the
 * one this file is bound to.
 *
 * The deallocation is an inline operator delete -- Enemy's, reachable because
 * Enemy is this class's IMMEDIATE base.
 */
#include "PiranhaPlant.h"

PiranhaPlant::~PiranhaPlant()
{
}
