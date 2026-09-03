//cpp
// @symbol _ZN16daObjFl_London_c6RenderEv
#include "daObjFl_London_c.h"
// recovered name: daObjFl_London_c_Render
/* recovered: renamed to Class_Method */
/* daObjFl_London_c::Render - recovered from vtable slot identity */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };
s32 daObjFl_London_c::Render() {
    Derived * d = (Derived *)this; Base *b = &d->base; b->m(0); return 1; }
