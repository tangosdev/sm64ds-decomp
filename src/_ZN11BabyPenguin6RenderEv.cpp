//cpp
// @symbol _ZN11BabyPenguin6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "BabyPenguin.h"

int BabyPenguin::Render()
{
    unsigned int f = mFlags;
    int b = ((f & 0x40000) != 0);
    if(b) return 1;
    mModelAnim.Render((Vector3 *)&mScaleX);
    return 1;
}
