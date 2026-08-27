//cpp
// @symbol _ZN8IceSheet15OnGroundPoundedER8dActor_c
#include "IceSheet.h"

/* IceSheet::OnGroundPounded -- vtable slot 21, ov018 0x021128e0.
 *
 * Real member function: param1 is fBase_c's own field (include/fBase_c.h);
 * the trailing unqualified Kill() call reaches this class's own slot-31
 * override (include/IceSheet.h) through the vtable, same idiom as
 * MovingBar/TowerStep::OnHitByMegaChar. kind 2 is the ground-pound actor
 * kind that triggers the sheet's own Kill. */
void IceSheet::OnGroundPounded(dActor_c &other)
{
    if (&other == 0) return;
    if (other.param1 != 2) return;
    Kill();
}
