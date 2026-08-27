//cpp
// @symbol _ZN12daBakubaku_c6RenderEv
/* Vtable slot 9, previously func_ov032_02112164.
 *
 * Skips drawing entirely when dActor_c flag 0x40000 is set, otherwise renders the
 * animated model with a null scale.
 *
 * The file used to reach the model through a six-slot stand-in class whose last
 * member was called `Target(int)`. That shape was load-bearing, not decoration: it
 * made the call a VIRTUAL dispatch through slot 5, and slot 5 of ModelAnim is
 * Render(const Vector3 *). Naming the member is what lets the real declaration
 * replace the stand-in without changing what is emitted.
 */
#include "daBakubaku_c.h"

s32 daBakubaku_c::Render()
{
    /* The temporary is load-bearing and must not be folded into the `if`. It makes
       the compiler materialise the test as a 0/1 value -- movne #1 / moveq #0 / cmp
       -- before branching, three instructions the ROM has and that
       `if ((mFlags & 0x40000) != 0)` does not emit. */
    int b = ((mFlags & 0x40000) != 0);
    if (b) return 1;
    mModelAnim.Render(0);
    return 1;
}
