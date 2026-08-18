//cpp
/* Actor::OnHitByMegaChar(Player&) at 0x02010130, 4 bytes -- vtable slot 27.
 *
 * A bare `bx lr`. The base actor ignores a mega-form player; leaf classes override.
 *
 * The declared `int` return is include/Actor.h's and is not observable here:
 * the ROM sets no r0, so `int` and `void` compile to the same one instruction.
 * See src/_ZN5Actor8OnPushedERS_.cpp for the full note.
 */
#include "Actor.h"

int Actor::OnHitByMegaChar(Player &player)
{
}
