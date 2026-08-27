//cpp
// @symbol _ZN5Model6RenderEPK7Vector3
#include "Model.h"
extern "C" {
extern Matrix4x3 data_0209b3ec;
void MulMat4x3Mat4x3(const Matrix4x3 *m1, const Matrix4x3 *m0, Matrix4x3 *mF);
}

void Model::Render(const Vector3 *scale)
{
    Matrix4x3 temp;
    MulMat4x3Mat4x3(&mat4x3, &data_0209b3ec, &temp);
    data.Render(&temp, (Vector3 *)scale);
}
