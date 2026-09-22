//cpp
#include "dMgPsOpt_c.h"

extern "C" int func_ov004_020b8f18(void *self)
{
    dMgPsOpt_c *options = (dMgPsOpt_c *)self;
    int iconIndex;

    if (options->mActive == 0)
        return 0;
    for (iconIndex = 0; iconIndex < options->mIconCount; iconIndex++) {
        options->mIcons[iconIndex].Render();
    }
    return 1;
}
