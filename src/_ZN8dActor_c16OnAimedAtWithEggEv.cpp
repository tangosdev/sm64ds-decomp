//cpp
/* dActor_c::OnAimedAtWithEgg() at 0x02010124, 8 bytes -- vtable slot 29.
 *
 * `mov r0,#0x14000; bx lr`. The egg auto-aim lock-on radius, as a 20.12
 * fixed-point scalar: 0x14000 == 81920 == 20.0. Leaf classes override to
 * widen or narrow the radius at which an egg will home on them.
 *
 * `mov` with an 8-bit immediate rotated right by 12 encodes 0x14000 in one
 * instruction, which is why the constant costs nothing here.
 *
 * The value is a Fix12i but the slot is declared `int` in include/dActor_c.h, and
 * Fix12i is itself a typedef of s32 -- the caller at 0x02010114 adds the result
 * straight to a position component, which is what a fix12 radius is for. Kept
 * as the header spells it so declaration and definition agree.
 */
#include "dActor_c.h"

int dActor_c::OnAimedAtWithEgg()
{
    return 0x14000;
}
