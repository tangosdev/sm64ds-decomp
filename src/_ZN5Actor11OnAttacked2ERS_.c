// @symbol _ZN5Actor11OnAttacked2ERS_
/* recovered: named members + shared header */
#include "Actor.h"
/* Actor::OnAttacked2(Actor&) at 0x02010140 -- vtable slot 22, combat hook.
 * Base Actor does nothing; leaf classes override to react to an attack.
 */

struct Actor;

void _ZN5Actor11OnAttacked2ERS_(struct Actor* self, struct Actor* attacker)
{
}
