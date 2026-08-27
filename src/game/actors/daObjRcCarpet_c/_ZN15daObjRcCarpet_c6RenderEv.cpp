//cpp
// @symbol _ZN15daObjRcCarpet_c6RenderEv
/* daObjRcCarpet_c::Render -- vtable slot 9. Real C++ method over the shared
 * header; the u16 at +0x428 (PathLift's tail-padding, read by raw offset) and
 * the Model sub-object at +0x450 keep the recovered layout. */
#include "daObjRcCarpet_c.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0x428]; unsigned short fld; char pad2[0x450-0x42a]; Base base; };
int daObjRcCarpet_c::Render() {
    Derived *d = (Derived *)this;
    if (d->fld < 0x5a && (d->fld & 1)) return 1;
    Base *b = &d->base; b->m(0);
    return 1;
}
