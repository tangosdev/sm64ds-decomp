//cpp
// @symbol _ZN9ModelAnim9Virtual18EjPK7Vector3
#include "ModelAnim.h"

void ModelAnim::Virtual18(u32 mat, const Vector3 *scale)
{
    Virtual10(*(Matrix4x3 *)mat);
    Model::Render(scale);
}
