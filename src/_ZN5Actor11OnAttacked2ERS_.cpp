//cpp
/* Actor::OnAttacked2(Actor&) at 0x02010140, 4 bytes -- vtable slot 23.
 *
 * A bare `bx lr`. The second of two attack hooks; the base ignores both.
 *
 * The declared `int` return is include/Actor.h's and is not observable here:
 * the ROM sets no r0, so `int` and `void` compile to the same one instruction.
 * See src/_ZN5Actor8OnPushedERS_.cpp for the full note.
 */
#include "Actor.h"

int Actor::OnAttacked2(Actor &attacker)
{
}
