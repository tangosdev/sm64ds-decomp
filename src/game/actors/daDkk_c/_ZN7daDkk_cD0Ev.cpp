//cpp
// @symbol _ZN7daDkk_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * same chain as D1 -- then return the object to its heap. Nobody writes
 * that; declaring `~daDkk_c()` is enough, because mwcc emits D2, D0 and D1
 * together and objisolate keeps the one this file is bound to. The
 * deallocation is an explicit Memory::Deallocate call, matching the
 * un-migrated func_ov025_02111928 this file replaces.
 */
#include "daDkk_c.h"

daDkk_c::~daDkk_c()
{
}
