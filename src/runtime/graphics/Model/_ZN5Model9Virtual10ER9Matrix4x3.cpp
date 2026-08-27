//cpp
// @symbol _ZN5Model9Virtual10ER9Matrix4x3
#include "Model.h"
extern "C" void func_02045074(ModelComponents *data, BMD_Bone *bones);

void Model::Virtual10(Matrix4x3 &mat)
{
    /* The copy is one flat 12-word ldm/stm run in the ROM. CW copies the
       nested Matrix4x3 (Matrix3x3 + Vector3) memberwise, which is bigger,
       so it goes through a flat 12-word view instead. */
    struct Mtx12w { Fix12i m[12]; };
    *(Mtx12w *)data.transforms = *(const Mtx12w *)&mat;
    func_02045074(&data, data.modelFile->bones);
}
