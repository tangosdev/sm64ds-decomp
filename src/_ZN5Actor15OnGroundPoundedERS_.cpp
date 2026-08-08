//cpp
/* Actor::OnGroundPounded(Actor&) at 0x02010148, 4 bytes -- vtable slot 21.
 *
 * A bare `bx lr`. The base actor does not react to a ground pound; leaf classes override.
 *
 * The declared `int` return is include/Actor.h's and is not observable here:
 * the ROM sets no r0, so `int` and `void` compile to the same one instruction.
 * See src/_ZN5Actor8OnPushedERS_.cpp for the full note.
 */
#include "Actor.h"

int Actor::OnGroundPounded(Actor &pounder)
{
}
