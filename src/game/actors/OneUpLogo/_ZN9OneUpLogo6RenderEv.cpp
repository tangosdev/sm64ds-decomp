//cpp
// @symbol _ZN9OneUpLogo6RenderEv

#include "OneUpLogo.h"

int OneUpLogo::Render()
{
    if (mDelay != 0) {
        mDelay--;
        return 1;
    }
    mTextureSequence.Update(mModel.data);
    mModel.Render(0);
    return 1;
}
