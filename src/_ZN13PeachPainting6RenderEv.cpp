//cpp
// @symbol _ZN13PeachPainting6RenderEv
#include "PeachPainting.h"

int PeachPainting::Render()
{
    if (mOpacity == 0)
        return 1;

    mModel.Render(0);
    return 1;
}
