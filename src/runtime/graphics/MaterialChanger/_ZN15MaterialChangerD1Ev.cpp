//cpp
// @symbol _ZN15MaterialChangerD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version spelled out the two things a complete-object
 * destructor does anyway: store this class's vtable over the one the base
 * constructor left behind, then run the Animation subobject destructor. Both
 * come out of `struct MaterialChanger : Animation` and `virtual ~MaterialChanger()` in the
 * header, so there is nothing left to write.
 *
 * The one member is a BMA_File pointer, which has no destructor, so the ROM's
 * 0x24 bytes are the vtable store and the base call.
 */
#include "MaterialChanger.h"

MaterialChanger::~MaterialChanger()
{
}
