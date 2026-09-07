//cpp
// @symbol _ZN4Fish6RenderEv
/* recovered: real C++ method over the owned ModelAnim */
#include "Fish.h"

int Fish::Render()
{
    if (mHidden == 0)
        mModelAnim.Render(0);
    return 1;
}
