//cpp
// @symbol _ZN11ShadowModel9RenderAllEv
#include "ShadowModel.h"
extern "C" {
extern ShadowModel *data_0209cef4;
extern u8 data_0209ceec;
extern Matrix4x3 data_020a0e68;      /* shared render matrix scratch */
extern Matrix4x3 data_0209b3ec;      /* the camera-space matrix Model::Render uses */
void func_0203c178(Matrix4x3 *m, Fix12i sx, Fix12i sy, Fix12i sz);
void MulMat4x3Mat4x3(const Matrix4x3 *m1, const Matrix4x3 *m0, Matrix4x3 *mF);
void func_02046120(ModelComponents *data, u32 opacity);
void func_02046088(ModelComponents *data, u32 opacity, int f);
}

void ShadowModel::RenderAll()
{
    ShadowModel *node = data_0209cef4;
    if (node) {
        do {
            ModelComponents *model = node->data;
            func_0203c178(&data_020a0e68, node->scale.x, node->scale.y, node->scale.z);
            MulMat4x3Mat4x3(&data_020a0e68, node->mat, &data_020a0e68);
            data_020a0e68.t.y += 0x2000;
            MulMat4x3Mat4x3(&data_020a0e68, &data_0209b3ec, &data_020a0e68);
            func_02046120(model, node->opacity);
            model->Render(&data_020a0e68, 0);
            func_02046088(model, node->opacity, 1);
            model->Render(&data_020a0e68, 0);
            node = node->next;
        } while (node);
    }
    data_0209ceec = 1;
}
