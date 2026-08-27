//cpp
// @symbol _ZN13TreasureChest6RenderEv
#include "TreasureChest.h"

int TreasureChest::Render()
{
    Model *model = &mModelAnim;
    model->Render(0);
    return 1;
}
