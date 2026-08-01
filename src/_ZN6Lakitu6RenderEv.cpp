//cpp
// @symbol _ZN6Lakitu6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Lakitu.h"
struct ModelComponents;
struct TextureSequence { void Update(ModelComponents &m); };
struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int x);
};

int Lakitu::Render()
{
    int b = (int)((unk_0b0 & 0x40000) != 0);
    if (b != 0)
        return 1;
    ((TextureSequence *)((char *)&mTextureSequence))->Update(*(ModelComponents *)((char *)&unk_0dc));
    ((VObj *)((char *)&mModelAnim))->m(0);
    if (unk_3f4 == 1) {
        unsigned int v = ((unsigned int)(unk_12c << 4)) >> 0x10;
        if (v >= 0x19 && v <= 0x3a)
            ((VObj *)((char *)&mModel))->m(0);
    }
    return 1;
}
