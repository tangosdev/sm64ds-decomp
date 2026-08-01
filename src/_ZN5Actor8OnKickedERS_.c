// @symbol _ZN5Actor8OnKickedERS_
/* recovered: named members + shared header */
#include "Actor.h"
/* Actor::OnKicked(Actor&) at 0x0201013c -- vtable slot 23, combat hook.
 * Base Actor does nothing; leaf classes override to react to being kicked.
 */

struct Actor;

void _ZN5Actor8OnKickedERS_(struct Actor* self, struct Actor* kicker)
{
}
