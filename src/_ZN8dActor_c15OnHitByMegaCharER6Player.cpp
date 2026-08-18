//cpp
/* dActor_c::OnHitByMegaChar(Player&) at 0x02010130, 4 bytes -- vtable slot 27.
 *
 * A bare `bx lr`. The base actor ignores a mega-form player; leaf classes override.
 *
 * The declared `int` return is include/dActor_c.h's and is not observable here:
 * the ROM sets no r0, so `int` and `void` compile to the same one instruction.
 * See src/_ZN8dActor_c8OnPushedERS_.cpp for the full note.
 */
#include "dActor_c.h"

int dActor_c::OnHitByMegaChar(Player &player)
{
}
