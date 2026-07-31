// @symbol _ZN5Actor15OnGroundPoundedERS_
/* recovered: named members + shared header */
#include "Actor.h"
/* Actor::OnGroundPounded(Actor&) at 0x02010148 -- vtable slot 20, combat hook.
 * Base Actor does nothing; leaf classes override to react to a ground pound.
 */

struct Actor;

void _ZN5Actor15OnGroundPoundedERS_(struct Actor* self, struct Actor* pounder)
{
}
