//cpp
// @symbol _ZN6Cannon6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Cannon.h"

int Cannon::Render()
{
    if (mState == 3 && mFireStep >= 3)
        return 1;

    mModel.UpdateVerts();
    mModel.Render(0);
    return 1;
}
