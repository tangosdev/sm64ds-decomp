//cpp
// @symbol _ZN11SnowmanHead6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "SnowmanHead.h"
int SnowmanHead::Render()
{
    mTextureSequence.Update(mModel.data);
    mModel.Render((Vector3 *)&mScaleX);
    return 1;
}
