//cpp
// @symbol _ZN9ModelAnim9Virtual10ER9Matrix4x3
#include "ModelAnim.h"

void ModelAnim::Virtual10(Matrix4x3 &mat)
{
    s32 frame = currFrame;
    data.UpdateBones(file, (u32)(frame << 4) >> 0x10);
    Model::Virtual10(mat);
}
