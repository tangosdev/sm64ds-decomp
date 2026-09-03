//cpp
// @symbol _ZN16daObjKm2_Agaru_c6RenderEv
#include "daObjKm2_Agaru_c.h"
// recovered name: daObjKm2_Agaru_c_Render
/* recovered: renamed to Class_Method */
/* daObjKm2_Agaru_c::Render - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };
s32 daObjKm2_Agaru_c::Render() {
    Derived * d = (Derived *)this; Base *b = &d->base; b->m(0); return 1; }
