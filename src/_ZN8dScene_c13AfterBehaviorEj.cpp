//cpp
/* Scene::AfterBehavior(u32) at 0x0202e3c8, 0xc bytes -- vtable slot 8.
 *
 * A tail call to ActorBase::AfterBehavior (0x02043af8); see
 * src/_ZN5Scene11AfterRenderEj.cpp for why that is three words and why the
 * parameter type comes from the target's declaration rather than from these bytes. */
#include "Scene.h"

void Scene::AfterBehavior(u32 vfSuccess)
{
    ActorBase::AfterBehavior(vfSuccess);
}
