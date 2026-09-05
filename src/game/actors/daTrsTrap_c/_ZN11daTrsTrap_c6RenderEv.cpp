//cpp
// @symbol _ZN11daTrsTrap_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daTrsTrap_c.h"
struct Base {
    virtual int vf0(int);
    virtual int vf1(int);
    virtual int vf2(int);
    virtual int vf3(int);
    virtual int vf4(int);
    virtual int vfunc(int a);
};
struct Obj {
    char pad[0xd4];
    Base sub;
};

int daTrsTrap_c::Render()
{
    if (*(unsigned char*)((char*)&mVisible) == 0) return 1;
    ((Obj *)this)->sub.vfunc(0);
    return 1;
}
