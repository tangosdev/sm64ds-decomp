//cpp
// @symbol _ZN10dMgState_c6RenderEv
#include "dMgState_c.h"

void dMgState_c::Render()
{
    if (mState == -1)
        return;
    if (mRender == 0)
        return;
    (this->*mRender)();
}
