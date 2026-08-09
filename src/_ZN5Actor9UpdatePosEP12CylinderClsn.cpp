//cpp
/* Actor::UpdatePos(CylinderClsn*) at 0x02010c30, 0x2c bytes.
 *
 * The whole per-frame position step in one call: integrate horizontal speed
 * along the facing angle and apply gravity, then move and resolve against the
 * collision cylinder. Callers that want only one half call that half directly.
 *
 * Takes a POINTER, not a reference -- the mangled name says `P12CylinderClsn`
 * -- and hands it straight to UpdatePosWithOnlySpeed. Its sibling
 * UpdatePosWithHorzSpeedAndAng takes nothing and works entirely through `this`.
 *
 * A member: `mov r5, r0` saves `this` across the first call and restores it
 * into r0 for the second, with the clsn shuttled through r4.
 */
#include "Actor.h"

void Actor::UpdatePos(CylinderClsn *clsn)
{
    UpdatePosWithHorzSpeedAndAng();
    UpdatePosWithOnlySpeed(clsn);
}
