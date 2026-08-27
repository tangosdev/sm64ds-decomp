//cpp
// @symbol _ZN14BlendModelAnim11UpdateVertsEv
#include "BlendModelAnim.h"
extern "C" void func_0204531c(ModelComponents *data, s32 weight);

void BlendModelAnim::UpdateVerts()
{
    s32 frame = currFrame;
    data.UpdateBones(file, (u32)(frame << 4) >> 0x10);
    if (blendWeight < 0x1000) {
        func_0204531c(&data, blendWeight);
    } else {
        data.UpdateVertsUsingBones();
    }
}
