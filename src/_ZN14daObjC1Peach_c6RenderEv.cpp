//cpp
// @symbol _ZN14daObjC1Peach_c6RenderEv
#include "daObjC1Peach_c.h"

int daObjC1Peach_c::Render()
{
    if (mOpacity == 0)
        return 1;

    mModel.Render(0);
    return 1;
}
