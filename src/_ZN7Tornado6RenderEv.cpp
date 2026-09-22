//cpp
// @symbol _ZN7Tornado6RenderEv
#include "Tornado.h"

int Tornado::Render()
{
    mTextureTransformer.Update(mModelAnim.data);
    mModelAnim.Render((const Vector3 *)&mScaleX);
    return 1;
}
