//cpp
// @symbol _ZN10daPgDfdr_c6RenderEv
/* daPgDfdr_c::Render -- vtable slot 9. Real C++ method over the shared header.
   The local shadow struct reaches dBgActor_c's dBgW-style virtual the same way
   the pre-migration file did; unrelated to daPgDfdr_c's own layout. */
#include "daPgDfdr_c.h"
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Base { char pad[0x320]; Sub sub; };
extern "C" void _ZN15TextureSequence6UpdateER15ModelComponents(void *, void *);
s32 daPgDfdr_c::Render() {
    _ZN15TextureSequence6UpdateER15ModelComponents((char *)this + 0x384, (char *)this + 0x328);
    Sub *b = &((Base *)this)->sub; b->m(0);
    return 1;
}
