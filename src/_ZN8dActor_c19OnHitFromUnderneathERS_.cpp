//cpp
/* dActor_c::OnHitFromUnderneath(dActor_c&) at 0x0201012c, 4 bytes -- vtable slot 28.
 *
 * A bare `bx lr`. The base actor ignores being struck from below; leaf classes override.
 *
 * The declared `int` return is include/dActor_c.h's and is not observable here:
 * the ROM sets no r0, so `int` and `void` compile to the same one instruction.
 * See src/_ZN8dActor_c8OnPushedERS_.cpp for the full note.
 */
#include "dActor_c.h"

int dActor_c::OnHitFromUnderneath(dActor_c &attacker)
{
}
