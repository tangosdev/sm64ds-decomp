//cpp
#include "fBase_c.h"

extern "C" int func_0204424c(void *);

int fBase_c::BeforeCleanupResources()
{
    if (lifecycleState != 0) {
        if (func_0204424c(lifecycleState) == 0)
            goto notReady;
    }
    if (manager.sceneNode.child == 0)
        goto ready;
notReady:
    return 0;
ready:
    return 1;
}
