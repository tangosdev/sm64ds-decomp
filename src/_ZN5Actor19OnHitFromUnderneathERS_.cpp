//cpp
/* Actor::OnHitFromUnderneath(Actor&) at 0x0201012c, 4 bytes -- vtable slot 28.
 *
 * A bare `bx lr`. The base actor ignores being struck from below; leaf classes override.
 *
 * The declared `int` return is include/Actor.h's and is not observable here:
 * the ROM sets no r0, so `int` and `void` compile to the same one instruction.
 * See src/_ZN5Actor8OnPushedERS_.cpp for the full note.
 */
#include "Actor.h"

int Actor::OnHitFromUnderneath(Actor &attacker)
{
}
