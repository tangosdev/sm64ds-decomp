//cpp
// @symbol _ZN3Amp6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Amp.h"

int Amp::Render()
{
    mModelAnim.Render(0);

    if (mState != 0 && mState != 2) {
        mTextureSequence.Update(mModel.data);
        mTextureTransformer.Update(mModel.data);
        mModel.Render((Vector3 *)&mScaleX);
    }

    return 1;
}
