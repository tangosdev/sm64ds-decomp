//cpp
// @symbol _ZN9ModelAnim6RenderEPK7Vector3
#include "ModelAnim.h"

void ModelAnim::Render(const Vector3 *scale)
{
    UpdateVerts();
    Model::Render(scale);
}
