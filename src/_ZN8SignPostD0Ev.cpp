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

void SignPost::Kill()
{
}
