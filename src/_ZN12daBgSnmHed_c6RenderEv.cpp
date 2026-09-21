//cpp
// @symbol _ZN12daBgSnmHed_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daBgSnmHed_c.h"
int daBgSnmHed_c::Render()
{
    mTextureSequence.Update(mModel.data);
    mModel.Render((Vector3 *)&mScaleX);
    return 1;
}
