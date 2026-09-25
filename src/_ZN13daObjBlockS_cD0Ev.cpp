//cpp
// @symbol _ZN13daObjBlockS_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases,
 * then return the object to its heap. Declaring `~daObjBlockS_c()` is enough; mwcc
 * emits D2, D0 and D1 together and objisolate keeps the one this file is
 * bound to. The deallocation is an inline operator delete, matching
 * daObjBlockL_c's D0 (include/daObjBlockL_c.h, src/actors/daObjBlockL_c.cpp).
 */
#include "daObjBlockS_c.h"

daObjBlockS_c::~daObjBlockS_c()
{
}
