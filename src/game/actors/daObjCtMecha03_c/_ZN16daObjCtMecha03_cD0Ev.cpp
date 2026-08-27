//cpp
// @symbol _ZN16daObjCtMecha03_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap. Nobody writes that; declaring `~daObjCtMecha03_c()` is enough,
 * because mwcc emits D2, D0 and D1 together and objisolate keeps the one
 * this file is bound to.
 *
 * The deallocation is an explicit Memory::Deallocate call, matching the
 * un-migrated func_ov065_02119f88 this file replaces.
 */
#include "daObjCtMecha03_c.h"

daObjCtMecha03_c::~daObjCtMecha03_c()
{
}
