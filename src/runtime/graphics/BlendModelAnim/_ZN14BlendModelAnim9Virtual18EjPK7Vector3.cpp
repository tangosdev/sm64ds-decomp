//cpp
// @symbol _ZN14BlendModelAnim9Virtual18EjPK7Vector3
#include "BlendModelAnim.h"

void BlendModelAnim::Virtual18(u32 mat, const Vector3 *scale)
{
    Virtual10(*(Matrix4x3 *)mat);
    Model::Render(scale);
}
