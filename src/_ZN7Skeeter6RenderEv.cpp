//cpp
// @symbol _ZN7Skeeter6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Skeeter.h"
struct Arg;
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(Arg*); };
struct Obj { char pad[0xb0]; unsigned int flags; char pad2[0x30c-0xb4]; Sub sub; };

int Skeeter::Render()
{
    unsigned int f = ((Obj *)this)->flags;
    int b = ((f & 0x40000) != 0);
    if(b) return 1;
    ((Obj *)this)->sub.m((Arg*)((char*)&mScaleX));
    return 1;
}
