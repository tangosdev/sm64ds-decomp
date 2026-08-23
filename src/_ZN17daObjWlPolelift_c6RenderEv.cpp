//cpp
// @symbol _ZN17daObjWlPolelift_c6RenderEv
/* daObjWlPolelift_c::Render -- recovered from vtable slot identity. Real C++
 * method over the shared header; the Model sub-object at +0xd4 is rendered
 * through its own vtable. */
#include "daObjWlPolelift_c.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };
s32 daObjWlPolelift_c::Render() { Base *b = &((Derived *)this)->base; b->m(0); return 1; }
