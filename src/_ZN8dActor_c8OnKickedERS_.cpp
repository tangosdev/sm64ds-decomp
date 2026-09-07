//cpp
/* dActor_c::OnKicked(dActor_c&) at 0x0201013c, 4 bytes -- vtable slot 24.
 *
 * A bare `bx lr`. The base actor does not react to being kicked; leaf classes override.
 *
 * Declared `void` in include/dActor_c.h -- corrected from `int`, which was
 * unfalsifiable here (the ROM sets no r0, one instruction either way) but
 * proven wrong by BigBrickBlock::OnKicked (slot 24, ov002 0x020b36dc): that
 * override has locals and early returns, where `int` vs `void` changes
 * mwcc's register allocation even with r0 untouched. See
 * include/BigBrickBlock.h.
 */
#include "dActor_c.h"

void dActor_c::OnKicked(dActor_c &kicker)
{
}
