//cpp
// @symbol _ZN6BobOmb6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "BobOmb.h"
struct VBase {
    virtual void method0() = 0;
    virtual void method1() = 0;
    virtual void method2() = 0;
    virtual void method3() = 0;
    virtual void method4() = 0;
    virtual void method5(char *arg);
};

int BobOmb::Render()
{
    int result = 1;
    if (unk_3f3 != 0) {
        int flags = mFlags;
        int b = (flags & 0x40000) != 0;
        if (!b) {
            VBase *obj = (VBase*)((char *)&mModelAnim);
            obj->method5((char *)&mScaleX);
        }
    }
    return result;
}
