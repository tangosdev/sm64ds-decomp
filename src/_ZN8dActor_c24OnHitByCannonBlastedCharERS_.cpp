//cpp
/* Actor::OnHitByCannonBlastedChar(Actor&) at 0x02010134, 4 bytes -- vtable slot 26.
 *
 * A bare `bx lr`. The base actor ignores a character fired out of a cannon.
 *
 * The declared `int` return is include/Actor.h's and is not observable here:
 * the ROM sets no r0, so `int` and `void` compile to the same one instruction.
 * See src/_ZN5Actor8OnPushedERS_.cpp for the full note.
 */
#include "Actor.h"

int Actor::OnHitByCannonBlastedChar(Actor &attacker)
{
}
