//cpp
// @symbol _ZN16daObjCtMecha05_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap. Nobody writes that; declaring `~daObjCtMecha05_c()` is enough,
 * because mwcc emits D2, D0 and D1 together and objisolate keeps the one
 * this file is bound to.
 *
 * The deallocation is an explicit Memory::Deallocate call, matching the
 * un-migrated func_ov065_0211abac this file replaces.
 */
#include "daObjCtMecha05_c.h"

daObjCtMecha05_c::~daObjCtMecha05_c()
{
}
