//cpp
/* Scene::ResetFadersAndSound() at 0x0202e66c, 0x5c bytes.
 *
 * Non-virtual, and takes `this`: it publishes the scene as the current actor at
 * 0x0209f5c0 and hands the same pointer to ActorBase::BeforeInitResources.
 *
 * The qualified call is deliberate. ActorBase::BeforeInitResources is virtual and
 * Scene overrides it, so an unqualified call here would dispatch through the
 * vtable and land straight back in the caller -- infinite recursion. The ROM does
 * `bl 0x02043c78`, a direct call to the base implementation. */
#include "Scene.h"
#include "FaderBrightness.h"

/* `extern` on every one of these -- a braced `extern "C" { }` is a linkage
   specification wrapped around a DEFINITION, not a declaration. */
extern "C" {
extern ActorBase *data_0209f5c0;
/* Really a FaderColor: __sinit_02074edc constructs it up the chain and leaves
   the FaderColor vtable (data_0208eb2c) in its vptr. Spelled as the base because
   include/FaderColor.h is still a flat generated struct that does not name a base,
   so the upcast this line needs cannot be written yet. */
extern FaderBrightness data_0209f5e8;
extern u32 data_0209f1e4;
extern void func_02011b7c(void);
}

int Scene::ResetFadersAndSound()
{
    data_0209f5c0 = this;
    if (!ActorBase::BeforeInitResources())
        return 0;
    SetFaders(&data_0209f5e8);
    data_0209f1e4 = 0;
    func_02011b7c();
    return 1;
}
