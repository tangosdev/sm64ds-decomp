// @symbol _ZN5Actor19OnHitFromUnderneathERS_
/* recovered: named members + shared header */
#include "Actor.h"
/* Actor::OnHitFromUnderneath(Actor&) at 0x0201012c -- vtable slot 27, combat hook.
 * Base Actor does nothing; leaf classes override to react when struck from below.
 */

struct Actor;

void _ZN5Actor19OnHitFromUnderneathERS_(struct Actor* self, struct Actor* attacker)
{
}
