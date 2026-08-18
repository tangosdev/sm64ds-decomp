//cpp
// @symbol _ZN10daPgDfdr_c6RenderEv
/* daPgDfdr_c::Render -- vtable slot 9. Extern "C" free function under the
   mangled name; see src/_ZN7daDgr_c13InitResourcesEv.cpp for why it is not
   converted to a true method body. The local shadow struct reaches
   dBgActor_c's MeshColliderBase-style virtual the same way the pre-migration
   file did; unrelated to daPgDfdr_c's own layout. */
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Base { char pad[0x320]; Sub sub; };
extern "C" void _ZN15TextureSequence6UpdateER15ModelComponents(void *, void *);
extern "C" int _ZN10daPgDfdr_c6RenderEv(Base *c) { _ZN15TextureSequence6UpdateER15ModelComponents((char *)c + 0x384, (char *)c + 0x328); Sub *b = &c->sub; b->m(0); return 1; }
