#include "types.h"
/* Scene::BeforeRender — delegates to ActorBase::BeforeRender and returns its result as bool.
 * ActorBase::BeforeRender returns 0 or non-zero; Scene converts to bool (0 or 1).
 *
 * Callee: 0x02043ac8 = ActorBase::BeforeRender()
 *   _ZN9ActorBase12BeforeRenderEv
 */
struct Scene;

extern int _ZN9ActorBase12BeforeRenderEv(struct Scene* self);

int _ZN5Scene12BeforeRenderEv(struct Scene* this_)
{
    return _ZN9ActorBase12BeforeRenderEv(this_) != 0;
}
