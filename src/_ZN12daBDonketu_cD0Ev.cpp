//cpp
// @symbol _ZN12daBDonketu_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap. Nobody writes that; declaring `~daBDonketu_c()` is enough, because mwcc
 * emits D2, D0 and D1 together and objisolate keeps the one this file is bound
 * to.
 *
 * The deallocation is an inline operator delete, which is why nothing below
 * mentions a heap.
 */
#include "daBDonketu_c.h"

daBDonketu_c::~daBDonketu_c()
{
}
