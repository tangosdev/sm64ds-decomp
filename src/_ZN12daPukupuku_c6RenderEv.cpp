//cpp
// @symbol _ZN12daPukupuku_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daPukupuku_c.h"
struct Obj {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void Target(int);
};

int daPukupuku_c::Render()
{
    int b = ((mFlags & 0x40000) != 0);
    if (b) return 1;
    Obj *o = (Obj*)((char *)&(*(u8 *)&mModelAnim));
    o->Target(0);
    return 1;
}
