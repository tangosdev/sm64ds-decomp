//cpp
/* dActor_c::OnAttacked2(dActor_c&) at 0x02010140, 4 bytes -- vtable slot 23.
 *
 * A bare `bx lr`. The second of two attack hooks; the base ignores both.
 *
 * The declared `int` return is include/dActor_c.h's and is not observable here:
 * the ROM sets no r0, so `int` and `void` compile to the same one instruction.
 * See src/_ZN8dActor_c8OnPushedERS_.cpp for the full note.
 */
#include "dActor_c.h"

int dActor_c::OnAttacked2(dActor_c &attacker)
{
}
