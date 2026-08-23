//cpp
// @symbol _ZN13daObjEmmLog_c6RenderEv
/* daObjEmmLog_c::Render - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. Real C++ method over the shared header; the Model
   sub-object at +0xd4 is rendered through its own vtable. */
#include "daObjEmmLog_c.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };
int daObjEmmLog_c::Render() { Base *b = &((Derived *)this)->base; b->m(0); return 1; }
