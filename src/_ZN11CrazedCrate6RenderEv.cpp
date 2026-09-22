//cpp
// @symbol _ZN11CrazedCrate6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "CrazedCrate.h"
class Sub {
public:
    virtual void F0();
    virtual void F1();
    virtual void F2();
    virtual void F3();
    virtual void F4();
    virtual void F5(int n);
};

int CrazedCrate::Render()
{
    int r1 = mFlags;
    r1 = r1 & 0x40000;
    r1 = r1 ? 1 : 0;
    if (r1) return 1;
    Sub *sub = (Sub*)((char *)&mModel);
    sub->F5(0);
    return 1;
}
