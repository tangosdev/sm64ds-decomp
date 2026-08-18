//cpp
/* dActor_c::OnGroundPounded(dActor_c&) at 0x02010148, 4 bytes -- vtable slot 21.
 *
 * A bare `bx lr`. The base actor does not react to a ground pound; leaf classes override.
 *
 * Declared `void` in include/dActor_c.h -- corrected from `int`, which was
 * unfalsifiable here (one instruction either way) but proven wrong by
 * Stump::OnGroundPounded (slot 21, ov091 0x02133648): that override compares
 * two fields and returns early, where `int` vs `void` changes mwcc's
 * register allocation even with r0 untouched. See include/Stump.h.
 */
#include "dActor_c.h"

void dActor_c::OnGroundPounded(dActor_c &pounder)
{
}
