//cpp
// @symbol _ZN9FaderWipeD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * D1 is the COMPLETE-object destructor, vtable slot 0. Everything in it is a
 * consequence of `struct FaderWipe : FaderColor` plus the `Model model` member
 * at 0x10: store this class's vptr over the one the base constructor left,
 * destroy members in reverse declaration order (the Model, out of line because
 * Model.h declares ~Model and no TU here defines it), then run the base
 * sub-object destructor FaderColor::~FaderColor (D2). The body is empty.
 *
 * Defining the destructor out of line makes this the key-function TU, so mwcc
 * also emits _ZTV9FaderWipe and the D2/D0 variants beside D1; objisolate keeps
 * the one this file is bound to and rebinds the vtable reference to the ROM's
 * own _ZTV9FaderWipe at 0x0208ea9c.
 */
#include "FaderWipe.h"

FaderWipe::~FaderWipe()
{
}
