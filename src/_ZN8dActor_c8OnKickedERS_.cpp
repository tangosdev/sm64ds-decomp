//cpp
/* dActor_c::OnKicked(dActor_c&) at 0x0201013c, 4 bytes -- vtable slot 24.
 *
 * A bare `bx lr`. The base actor does not react to being kicked; leaf classes override.
 *
 * The declared `int` return is include/dActor_c.h's and is not observable here:
 * the ROM sets no r0, so `int` and `void` compile to the same one instruction.
 * See src/_ZN8dActor_c8OnPushedERS_.cpp for the full note.
 */
#include "dActor_c.h"

int dActor_c::OnKicked(dActor_c &kicker)
{
}
