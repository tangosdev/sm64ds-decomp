//cpp
// @symbol _ZN15TtcRotatingCube6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "TtcRotatingCube.h"
struct Base {
    virtual void method0();
    virtual void method1();
    virtual void method2();
    virtual void method3();
    virtual void method4();
    virtual void method5(int x);
};

int TtcRotatingCube::Render()
{
    int flags;
    int b;
    flags = mFlags;
    b = flags & 8;
    b = (b != 0);
    if (!b) {
        ((Base*)((char *)&mModel1))->method5(0);
        ((Base*)((char *)&mModel2))->method5(0);
    }
    return 1;
}
