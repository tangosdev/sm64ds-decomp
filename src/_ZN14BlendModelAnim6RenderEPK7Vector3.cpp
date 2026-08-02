//cpp
// @symbol _ZN14BlendModelAnim6RenderEPK7Vector3
#include "BlendModelAnim.h"

void BlendModelAnim::Render(const Vector3 *scale)
{
    UpdateVerts();
    Model::Render(scale);
}
