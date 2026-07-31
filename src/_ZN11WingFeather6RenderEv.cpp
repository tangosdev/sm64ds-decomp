//cpp
// @symbol _ZN11WingFeather6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "WingFeather.h"
struct Sub {
    virtual ~Sub() {}
    virtual void a() {}
    virtual void b() {}
    virtual void c() {}
    virtual int f4(int) = 0;
};

int WingFeather::Render()
{
    unsigned char b = *(unsigned char*)((char*)&unk_384);
    if (b < 0x2d) {
        if (b & 1) return 1;
    }
    Sub *sub = (Sub*)((char*)&mModel);
    sub->f4(0);
    return 1;
}
