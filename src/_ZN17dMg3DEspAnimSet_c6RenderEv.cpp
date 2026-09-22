//cpp
// @symbol _ZN17dMg3DEspAnimSet_c6RenderEv
#include "common.h"
#include "dMg3DEspAnimSet_c.h"

extern "C" {
extern Matrix4x3 data_020a0e68;
void Matrix4x3_FromTranslation(Matrix4x3* matrix, int x, int y, int z);
}

void dMg3DEspAnimSet_c::Render()
{
    Matrix4x3_FromTranslation(&data_020a0e68, 0x8c000, 0x80000, 0x40000);
    for (int i = 0; i < 3; i++) {
        if (unk_168[i] != 0) {
            mModels[i].mat4x3 = data_020a0e68;
            mMaterialChangers[i].Update(mModels[i].data);
            mModels[i].Render(0);
        }
    }
}
