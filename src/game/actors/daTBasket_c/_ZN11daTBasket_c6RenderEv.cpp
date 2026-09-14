//cpp
// @symbol _ZN11daTBasket_c6RenderEv
/* Vtable slot 9. Draws the cage model with no scale override. */

#include "daTBasket_c.h"

int daTBasket_c::Render()
{
    mModel.Render(0);
    return 1;
}
