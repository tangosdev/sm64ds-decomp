//cpp
// @symbol _ZN20SwitchActivatedPlankD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body.
 *
 * D0 is the DELETING destructor: run the complete-object destruction (same
 * shape as our D1 -- see its header comment for the daObjWc_Obj04_c vtable
 * alias), then return the object to its heap. Nobody writes that; declaring
 * ~SwitchActivatedPlank() is enough, because mwccarm emits D2/D1/D0 together.
 *
 * The deallocation is the inline operator delete inherited from dActor_c --
 * Memory::Deallocate(ptr, data_020a0eac), the actor heap -- which is why
 * nothing below mentions a heap.
 */
#include "SwitchActivatedPlank.h"

SwitchActivatedPlank::~SwitchActivatedPlank()
{
}
