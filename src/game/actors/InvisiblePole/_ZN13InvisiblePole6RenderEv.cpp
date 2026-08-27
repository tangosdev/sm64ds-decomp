//cpp
// @symbol _ZN13InvisiblePole6RenderEv
/* Vtable slot 9. The pole is invisible, so rendering it is a no-op that still
 * reports success. */
#include "InvisiblePole.h"

s32 InvisiblePole::Render()
{
    return 1;
}
