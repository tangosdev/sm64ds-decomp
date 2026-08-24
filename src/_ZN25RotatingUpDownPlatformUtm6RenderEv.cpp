//cpp
// @symbol _ZN25RotatingUpDownPlatformUtm6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "RotatingUpDownPlatformUtm.h"
struct Sub {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void doit(int);
};

int RotatingUpDownPlatformUtm::Render()
{
    if (mIsDead) return 1;
    if (param1 == 0xffff) return 1;
    ((struct Sub*)((char *)&mModel))->doit(0);
    return 1;
}
