//cpp
/* dActor_c::OnHitByCannonBlastedChar(dActor_c&) at 0x02010134, 4 bytes -- vtable slot 26.
 *
 * A bare `bx lr`. The base actor ignores a character fired out of a cannon.
 *
 * The declared `int` return is include/dActor_c.h's and is not observable here:
 * the ROM sets no r0, so `int` and `void` compile to the same one instruction.
 * See src/_ZN8dActor_c8OnPushedERS_.cpp for the full note.
 */
#include "dActor_c.h"

int dActor_c::OnHitByCannonBlastedChar(dActor_c &attacker)
{
}
