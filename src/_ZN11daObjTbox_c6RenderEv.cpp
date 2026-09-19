//cpp
// @symbol _ZN11daObjTbox_c6RenderEv
#include "daObjTbox_c.h"

int daObjTbox_c::Render()
{
    Model *model = &mModelAnim;
    model->Render(0);
    return 1;
}
