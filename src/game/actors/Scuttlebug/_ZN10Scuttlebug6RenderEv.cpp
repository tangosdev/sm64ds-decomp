//cpp
// @symbol _ZN10Scuttlebug6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Scuttlebug.h"
struct Sub {
    virtual int method0();
    virtual int method1();
    virtual int method2();
    virtual int method3();
    virtual int method4();
    virtual int method5(int);
};
struct Obj {
    char pad1[0xb0];
    int flags;
    char pad2[0x20];
    Sub sub;
    char pad3[0x2c3];
    int state39c;
};

int Scuttlebug::Render()
{
    int flag = (((Obj *)this)->flags & 0x40000) ? 1 : 0;
    if (flag) goto ret;
    if (!((Obj *)this)->state39c) goto ret;
    goto call;
ret:
    return 1;
call:
    ((Obj *)this)->sub.method5(0);
    return 1;
}
