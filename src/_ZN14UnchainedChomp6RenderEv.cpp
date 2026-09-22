//cpp
// @symbol _ZN14UnchainedChomp6RenderEv
/* recovered: real C++ method over the typed model members */
#include "UnchainedChomp.h"

int UnchainedChomp::Render()
{
    mModelAnim.Render((Vector3 *)&mScaleX);
    for (int i = 0; i < 5; i++)
        mModels[i].Render(0);
    return 1;
}
