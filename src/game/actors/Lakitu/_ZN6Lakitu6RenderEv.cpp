//cpp
// @symbol _ZN6Lakitu6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Lakitu.h"
struct ModelComponents;

int Lakitu::Render()
{
    int b = (int)((mFlags & 0x40000) != 0);
    if (b != 0)
        return 1;
    mTextureSequence.Update(*(ModelComponents *)((char *)this + 0xdc));
    mModelAnim.Render(0);
    if (mState == 1) {
        unsigned int v = ((unsigned int)(*(s32 *)((char *)this + 0x12c) << 4)) >> 0x10;
        if (v >= 0x19 && v <= 0x3a)
            mModel.Render(0);
    }
    return 1;
}
