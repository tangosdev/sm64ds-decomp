//cpp
/* dActor_c::OnHitByMegaChar(Player&) at 0x02010130, 4 bytes -- vtable slot 27.
 *
 * A bare `bx lr`. The base actor ignores a mega-form player; leaf classes override.
 *
 * Declared `void` in include/dActor_c.h -- corrected from `int`, which was
 * unfalsifiable here (one instruction either way) but proven wrong by
 * Stump::OnHitByMegaChar (slot 27, ov091 0x021335d4): four early-return
 * field checks, where `int` vs `void` changes mwcc's register allocation
 * even with r0 untouched. See include/Stump.h.
 */
#include "dActor_c.h"

void dActor_c::OnHitByMegaChar(Player &player)
{
}
