//cpp
// @symbol _ZN11CastleWater6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "CastleWater.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };

int CastleWater::Render()
{
 Base *b = &((Derived *)this)->base; b->m(0); return 1;
}
