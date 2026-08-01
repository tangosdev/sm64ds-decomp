//cpp
// @symbol _ZN4Bird6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Bird.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };

int Bird::Render()
{
 Base *b = &((Derived *)this)->base; b->m(0); return 1;
}
