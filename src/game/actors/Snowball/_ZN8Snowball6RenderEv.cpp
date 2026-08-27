//cpp
// @symbol _ZN8Snowball6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Snowball.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0x300]; Base base; };

int Snowball::Render()
{
 Base *b = &((Derived *)this)->base; b->m(0); return 1;
}
