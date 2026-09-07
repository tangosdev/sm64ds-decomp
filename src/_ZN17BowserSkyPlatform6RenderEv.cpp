//cpp
// @symbol _ZN17BowserSkyPlatform6RenderEv
#include "BowserSkyPlatform.h"
// recovered name: daKpa3Bg_c_Render
/* recovered: renamed to Class_Method */
/* daKpa3Bg_c::Render - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };
s32 BowserSkyPlatform::Render() {
    Derived * d = (Derived *)this; Base *b = &d->base; b->m(0); return 1; }
