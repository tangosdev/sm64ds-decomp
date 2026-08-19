//cpp
// @symbol _ZN7daDgr_c6RenderEv
/* daDgr_c::Render -- vtable slot 9. Extern "C" free function under the
   mangled name; see src/_ZN7daDgr_c13InitResourcesEv.cpp for why it is not
   converted to a true method body. The local shadow struct reaches
   dBgActor_c's dBgW-style virtual at +0xd4 the same way the
   pre-migration file did; unrelated to daDgr_c's own layout. */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };
extern "C" int _ZN7daDgr_c6RenderEv(Derived *d) { Base *b = &d->base; b->m(0); return 1; }
