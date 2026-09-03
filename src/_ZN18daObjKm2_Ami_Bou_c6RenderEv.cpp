//cpp
// @symbol _ZN18daObjKm2_Ami_Bou_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daObjKm2_Ami_Bou_c.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };

int daObjKm2_Ami_Bou_c::Render()
{
 Base *b = &((Derived *)this)->base; b->m(0); return 1;
}
