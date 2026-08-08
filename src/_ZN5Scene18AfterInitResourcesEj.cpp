//cpp
/* Scene::AfterInitResources(u32) at 0x0202e62c, 0xc bytes -- vtable slot 2.
 *
 * A tail call to ActorDerived::AfterInitResources (0x02013ef4), not to
 * ActorBase's. Slot 2 is ActorDerived's only FUNCTIONAL override -- it also
 * carries its own D1/D0 at slots 16/17 -- so a Scene forwarding here is a strong
 * hint that ActorDerived is a base.
 *
 * A hint, not a proof, and the distinction matters: an extern "C" call to the
 * same address reproduces these three words exactly, which is what the file this
 * replaced did. What settles the hierarchy is the RTTI chain and the destructor's
 * vptr sequence; see reading 3 and 4 in include/Scene.h. */
#include "Scene.h"

void Scene::AfterInitResources(u32 vfSuccess)
{
    ActorDerived::AfterInitResources(vfSuccess);
}
