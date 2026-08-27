//cpp
// @symbol _ZN12WorkElevator6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "WorkElevator.h"
struct Obj {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void Target(int);
};

int WorkElevator::Render()
{
    unsigned short v = *(unsigned short*)(((char *)this)+0xc00+0x74);
    if (v < 0x2d && (v & 1)) return 1;
    Obj *o = (Obj*)((char *)&mModel);
    o->Target(0);
    int arg = 0;
    int i = 0;
    char *p = ((char *)this)+0x320;
    do {
        Obj *o2 = (Obj*)p;
        o2->Target(arg);
        i++;
        p += 0x50;
    } while (i < 4);
    return 1;
}
