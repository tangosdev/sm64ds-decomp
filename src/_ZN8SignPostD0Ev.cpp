//cpp
// @symbol _ZN8SignPostD0Ev
/* recovered: real C++ deleting destructor -- forced out of line via the key function
 *
 * SignPost.h defines `~SignPost() {}` in the class body on purpose, so no TU
 * that merely includes the header emits _ZN8SignPostD0Ev -- but the ROM
 * carries one at ov002 0x020bae2c. The vtable is what needs the deleting
 * destructor as a symbol, and the vtable is emitted by the TU that defines
 * the key function (Kill, the first non-inline virtual). Defining Kill here
 * makes mwcc emit _ZTV8SignPost and the destructor variants alongside it --
 * exactly what src/_ZN8SignPost4KillEv.cpp does today -- and objisolate keeps
 * only the D0 .text this file is bound to, discarding the duplicate Kill body
 * and the .data (notes/dtor-migration.md section 3).
 */
#include "SignPost.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC has no
 * key function, so the empty Kill below would only be a second, empty
 * definition of ?Kill@SignPost@@UAEXXZ -- the one the port's slot 31 would
 * then bind to in place of the ROM's own Kill in src/_ZN8SignPost4KillEv.cpp.
 * ~SignPost() is defined in the class body, and the port's D1 for this class
 * is its flat ROM name, the name the class's vtable slot 16 holds, so this arm
 * makes the D0's two calls: the D1 body through that flat name, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds the
 * `#else` arm and emits the ROM bytes it always emitted, and the object is
 * byte-identical either way. */
extern "C" void _ZN8SignPostD1Ev(void *self);
extern "C" SignPost *_ZN8SignPostD0Ev(SignPost *thiz)
{
    _ZN8SignPostD1Ev(thiz);           /* the D1 body, through its flat ROM name */
    SignPost::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
void SignPost::Kill()
{
}
#endif
