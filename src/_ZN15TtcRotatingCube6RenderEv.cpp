//cpp
// @symbol _ZN15TtcRotatingCube6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "TtcRotatingCube.h"
int TtcRotatingCube::Render()
{
    int flags;
    int b;
    flags = mFlags;
    b = flags & 8;
    b = (b != 0);
    if (!b) {
        mModel.Render(0);
        mRotatingModel.Render(0);
    }
    return 1;
}
