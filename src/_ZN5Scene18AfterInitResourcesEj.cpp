//cpp
/* Scene::AfterInitResources(u32) at 0x0202e62c, 0xc bytes -- vtable slot 2.
 *
 * A tail call to ActorDerived::AfterInitResources (0x02013ef4), not to
 * ActorBase's. That is the load-bearing detail in the whole hierarchy: slot 2 is
 * the one and only slot ActorDerived overrides, so a Scene that forwards to it is
 * a Scene that has ActorDerived as a base. See the chain argument in
 * include/Scene.h. */
#include "Scene.h"

void Scene::AfterInitResources(u32 vfSuccess)
{
    ActorDerived::AfterInitResources(vfSuccess);
}
