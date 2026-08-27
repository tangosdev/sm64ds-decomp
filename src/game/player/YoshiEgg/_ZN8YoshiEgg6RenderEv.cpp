//cpp
// @symbol _ZN8YoshiEgg6RenderEv
/* recovered: named members + shared header, real C++ method -- vtable slot 9 */
#include "YoshiEgg.h"
#include "Player.h"

int YoshiEgg::Render()
{
    /* The temporary is load-bearing and must not be folded into the `if`, the
       same way it is in daBakubaku_c::Render: `if (mFlags & 0x40000)` tests the
       masked word directly, while the ROM materialises the 0/1 first. Folding it
       changes the function's SIZE, which is what a `999 word(s) differ` says. */
    int b = (int)((mFlags & 0x40000) != 0);
    if (b) return 1;

    if (mPlayer->IsInsideOfCannon()) return 1;
    if (mPlayer->mOpacity < 1) return 1;
    mModelAnim.Render(0);
    return 1;
}
