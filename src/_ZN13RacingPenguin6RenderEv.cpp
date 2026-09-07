//cpp
// @symbol _ZN13RacingPenguin6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "RacingPenguin.h"

int RacingPenguin::Render()
{
    mTextureSequence.Update(mModelAnim.data);
    mModelAnim.Render((Vector3 *)&mScaleX);
    return 1;
}
