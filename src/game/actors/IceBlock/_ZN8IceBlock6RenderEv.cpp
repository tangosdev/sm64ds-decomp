//cpp
// @symbol _ZN8IceBlock6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "IceBlock.h"
class Sub {
public:
    virtual void F0();
    virtual void F1();
    virtual void F2();
    virtual void F3();
    virtual void F4();
    virtual void F5(int* v);
};

int IceBlock::Render()
{
    int v[3];
    v[0] = mScale;
    v[1] = mScale;
    v[2] = mScale;
    Sub *sub = (Sub*)((char *)&mModel);
    sub->F5(v);
    return 1;
}
