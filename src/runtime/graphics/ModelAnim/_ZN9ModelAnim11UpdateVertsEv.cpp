//cpp
// @symbol _ZN9ModelAnim11UpdateVertsEv
#include "ModelAnim.h"

void ModelAnim::UpdateVerts()
{
    s32 frame = currFrame;
    data.UpdateBones(file, (u32)(frame << 4) >> 0x10);
    data.UpdateVertsUsingBones();
}
