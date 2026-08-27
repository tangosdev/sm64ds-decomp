//cpp
// @symbol _ZN8Fireball6RenderEv
/* recovered: shared header, real C++ method
 *
 * Fireball draws nothing of its own -- it returns 1 and lets the particle
 * system it spawned do the work. Four bytes plus the return in the ROM.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "Fireball.h"

int Fireball::Render()
{
    return 1;
}
