//cpp
// @symbol _ZN6Snufit6RenderEv
/* recovered: named members + shared header, real C++ method
 *
 * The six-slot `struct Obj` this file used to cast mModelAnim to was ModelAnim's
 * own vtable, and the slot it called is Render.
 */
#include "Snufit.h"

int Snufit::Render()
{
    int b = ((mFlags & 0x40000) != 0);
    if (b) return 1;
    mModelAnim.Render(0);
    return 1;
}
