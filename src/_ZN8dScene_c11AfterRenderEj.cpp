//cpp
/* Scene::AfterRender(u32) at 0x0202e398, 0xc bytes -- vtable slot 11.
 *
 * The whole body is a tail call to ActorBase::AfterRender (0x02043ac4), which
 * mwccarm emits under -interworking as `ldr ip,[pc]; bx ip; .word target` rather
 * than a plain `b`. The three-word shape is about interworking, not distance: the
 * target here is 87KB away, comfortably inside `b` range.
 *
 * The parameter type is NOT derived from these bytes -- it cannot be, because a
 * tail call never touches r0-r3, so any prototype at all would reproduce them.
 * It comes from the definition of the target, include/ActorBase.h slot 11. */
#include "Scene.h"

void Scene::AfterRender(u32 vfSuccess)
{
    ActorBase::AfterRender(vfSuccess);
}
