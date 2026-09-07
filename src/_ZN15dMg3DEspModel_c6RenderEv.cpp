//cpp
#include "dMg3DEspModel_c.h"

extern "C" {
    void func_02046208(char* a, int b, int c);
}

extern int data_ov006_0213c744[];

// @symbol _ZN15dMg3DEspModel_c6RenderEv
void dMg3DEspModel_c::Render()
{
    char* c = (char*)this;
    int* p = (int*)(((int)c + 0x210));
    int* d = data_ov006_0213c744;
    if (p[0] == d[0]) {
        if (p[1] == d[1]) return;
        if (*(int*)(c + 0x210) == 0) return;
    }

    func_02046208(c + 0x14, *(unsigned char*)(c + 0x21a), 0);

    *(int*)(c + 0x78) = (unsigned)((*(short*)(c + 0x218)) << 0x11) >> 4;
    mTextureSequence.Update(mModelAnim.data);

    ResetTransform();

    mModelAnim.Render(0);

    mAnimSet.Render();
}
