//cpp
// @symbol _ZN5Swoop6RenderEv
/* recovered: named members + shared header, real C++ method
 *
 * The six-slot `struct Base` this file used to cast each ModelAnim to was
 * ModelAnim's own vtable, and the slot it called is Render. unk_43c picks
 * which of the two models is drawn.
 */
#include "Swoop.h"

int Swoop::Render()
{
    int flag = (mFlags & 0x40000) != 0;
    if (flag != 0) return 1;
    if (unk_43c == 1) {
        mModelAnim1.Render(0);
    } else {
        mModelAnim2.Render(0);
    }
    return 1;
}
