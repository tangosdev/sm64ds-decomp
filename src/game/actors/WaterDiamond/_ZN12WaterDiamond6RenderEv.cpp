//cpp
// @symbol _ZN12WaterDiamond6RenderEv
#include "WaterDiamond.h"

int WaterDiamond::Render()
{
    Model *model = &mModel;
    model->Render(0);
    return 1;
}
