//cpp
// @symbol _ZN9OneUpLogo6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "OneUpLogo.h"
typedef unsigned short u16;

struct VObj {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void m5(int);
};

extern "C" {
void _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
}

int OneUpLogo::Render()
{
    if (*(u16*)(((char*)this) + 0x100 + 0x4c) != 0) {
        *(u16*)(((long long)(int)((char*)&unk_14c))) -= 1;
        return 1;
    }
    _ZN15TextureSequence6UpdateER15ModelComponents(((char*)this) + 0x124, ((char*)this) + 0xdc);
    ((VObj*)((char*)&mModel))->m5(0);
    return 1;
}
