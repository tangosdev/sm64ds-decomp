//cpp
// @symbol _ZN9UkikiCageD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap. Nobody writes that; declaring `~UkikiCage()` is enough, because
 * mwcc emits D2, D0 and D1 together and objisolate keeps the one this file is
 * bound to.
 *
 * The deallocation is dActor_c's inline `operator delete` (include/dActor_c.h),
 * which is why nothing below mentions a heap -- it matches the pre-migration
 * C body's trailing `Memory::Deallocate(t, data_020a0eac)` call.
 */
#include "UkikiCage.h"

UkikiCage::~UkikiCage()
{
}
