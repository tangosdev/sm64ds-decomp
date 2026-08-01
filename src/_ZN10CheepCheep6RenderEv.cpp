//cpp
// @symbol _ZN10CheepCheep6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "CheepCheep.h"
struct Obj {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void Target(int);
};

int CheepCheep::Render()
{
    int b = ((unk_0b0 & 0x40000) != 0);
    if (b) return 1;
    Obj *o = (Obj*)((char *)&mModelAnim);
    o->Target(0);
    return 1;
}
