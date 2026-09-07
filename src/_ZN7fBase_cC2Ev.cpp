//cpp
// @symbol _ZN7fBase_cC2Ev
/* recovered: real C++ constructor -- mwcc emits both C1 and C2 from this
 * definition; this file is enrolled against the base-object C2 variant. */
#include "fBase_c.h"

struct fBaseActorInfo {
    u32 unk_000;
    u16 behaviorPriority;
    u16 renderPriority;
};

extern "C" {
extern u32 data_02099e70;
extern u32 data_020a4b60;
extern u16 data_020a4b54;
extern u8 data_020a4b48;
extern fBase_c::SceneNode *data_020a4b64;
extern fBase_c::SceneNode data_020a4b6c;
extern fBaseActorInfo **data_020a4bb8;

int func_0203b438(fBase_c::SceneNode *root,
                  fBase_c::SceneNode *node,
                  fBase_c::SceneNode *parent);
fBase_c *func_02043810(fBase_c *self);
}

fBase_c::fBase_c() : manager(this)
{
    uniqueID = data_02099e70;
    data_02099e70++;
    param1 = data_020a4b60;
    actorID = data_020a4b54;
    unk_012 = data_020a4b48;

    func_0203b438(&data_020a4b6c, &manager.sceneNode, data_020a4b64);

    fBaseActorInfo *info = data_020a4bb8[actorID];
    manager.SetBehaviorPriority(info->behaviorPriority);
    manager.SetRenderPriority(info->renderPriority);

    fBase_c *parent = func_02043810(this);
    if (parent != 0) {
        if ((parent->pauseFlags & 1) || (parent->pauseFlags & 2))
            pauseFlags |= 2;
        if ((parent->pauseFlags & 4) || (parent->pauseFlags & 8))
            pauseFlags |= 8;
    }
}
