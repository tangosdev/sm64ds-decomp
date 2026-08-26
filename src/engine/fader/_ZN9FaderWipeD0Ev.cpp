//cpp
// @symbol _ZN9FaderWipeD0Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor, vtable slot 1. Everything in it is a
 * consequence of `struct FaderWipe : FaderColor` plus the `Model model` member
 * at 0x10: store this class's vptr over the one the base constructor left,
 * destroy members in reverse declaration order (the Model, out of line because
 * Model.h declares ~Model and no TU here defines it), run the base sub-object
 * destructor FaderColor::~FaderColor (D2), then this object's class operator
 * delete -- which FaderWipe does not declare, so it is Fader's, three levels
 * up, inlined to Memory::operator_delete2. The body is empty.
 *
 * Defining the destructor out of line makes this the key-function TU, so mwcc
 * also emits _ZTV9FaderWipe and the D1/D2 variants beside D0; objisolate keeps
 * the one this file is bound to and rebinds the vtable reference to the ROM's
 * own _ZTV9FaderWipe at 0x0208ea9c.
 */
#include "FaderWipe.h"

FaderWipe::~FaderWipe()
{
}
