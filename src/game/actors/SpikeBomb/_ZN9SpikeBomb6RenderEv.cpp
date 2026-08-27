//cpp
// @symbol _ZN9SpikeBomb6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "SpikeBomb.h"
struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int);
};

int SpikeBomb::Render()
{
    if (mStateIndex != 0) return 1;
    if (mOpacity < 8) return 1;
    ((Sub*)((unsigned char*)&mModel))->m(0);
    return 1;
}
