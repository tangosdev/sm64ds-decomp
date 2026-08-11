//cpp
// @symbol _ZN13OneUpMushroom6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "OneUpMushroom.h"
struct Obj {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void func5(int x);
};

int OneUpMushroom::Render()
{
    if (unk_38e == 0 || unk_38f == 0)
        return 1;
    {
        int b = (mFlags & 0x40000) ? 1 : 0;
        if (b)
            return 1;
    }
    {
        Obj *o = (Obj *)((char *)&mModel);
        o->func5(0);
    }
    return 1;
}
