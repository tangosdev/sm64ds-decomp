//cpp
// @symbol _ZN11SnowmanBody6RenderEv
#include "SnowmanBody.h"

int SnowmanBody::Render()
{
    mModel.Render((Vector3 *)&mScaleX);
    return 1;
}
