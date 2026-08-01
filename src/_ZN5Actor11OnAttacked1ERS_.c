// @symbol _ZN5Actor11OnAttacked1ERS_
/* recovered: named members + shared header */
#include "Actor.h"
/* Actor::OnAttacked1(Actor&) at 0x02010144 -- vtable slot 21, combat hook.
 * Base Actor does nothing; leaf classes override to react to an attack.
 */

struct Actor;

void _ZN5Actor11OnAttacked1ERS_(struct Actor* self, struct Actor* attacker)
{
}
