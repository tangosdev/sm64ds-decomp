//cpp
/* dActor_c::OnAttacked1(dActor_c&) at 0x02010144, 4 bytes -- vtable slot 22.
 *
 * A bare `bx lr`. The first of two attack hooks; the base ignores both.
 *
 * The declared `int` return is include/dActor_c.h's and is not observable here:
 * the ROM sets no r0, so `int` and `void` compile to the same one instruction.
 * See src/_ZN8dActor_c8OnPushedERS_.cpp for the full note.
 */
#include "dActor_c.h"

int dActor_c::OnAttacked1(dActor_c &attacker)
{
}
