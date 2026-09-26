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

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~FaderWipe() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" FaderWipe *_ZN9FaderWipeD0Ev(FaderWipe *thiz)
{
    thiz->FaderWipe::~FaderWipe();     /* the D1 body, through the one host symbol */
    FaderWipe::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
FaderWipe::~FaderWipe()
{
}
#endif
